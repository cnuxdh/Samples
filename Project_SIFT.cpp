// Project_SIFT.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <time.h>
#include "stdio.h"

#include "cv.h"
#include "cxcore.h"
#include "highgui.h"

#include "vim_sift.h"
//#include "siftfeature.h"
//#define SINGLE_IMAGE_HT     900


//corelib
#include "commonfile.h"

//coredll
#include "geotiff.h"



#define FOCUS_RATIO         1.0
#define SINGLE_IMAGE_HT_GPU 800

#include "string"
#include "fstream"
#include "iostream"

#include "tinythread.h"
#include <list>

using namespace std;
using namespace tthread;

/*
mutex *mtx;
condition_variable *cv;
int running_count;
int running;
*/

mutex gMutex;
condition_variable gCV;
int running_count;


int  GetFileRows(char* file)
{
	char  sline[512];
	FILE* fp = NULL;
	int rows = 0;	
	fp = fopen(file, "r");	
	if(fp==NULL)
		return 0;	
	while( fgets(sline, 512, fp)!=NULL )
		rows++;	
	fclose(fp);	
	return rows;
}
/*
char **f2c (int nr, int nc)
{
	char **x;
	int i;
	
	x = (char **)calloc ( nr, sizeof (BYTE *) );
	if (x == NULL)	
		return NULL;		
	
	for (i=0; i<nr; i++)
	{  
		x[i] = (char *) calloc ( nc, sizeof (BYTE)  );
		if (x[i] == NULL)
			return NULL;				  
	}
	return x;
}
void FreeArray_char(char **array, int row, int col)
{
	int i;
	for(i = 0;i < row; ++i)
		free(array[i]);
	free(array);
}
*/

struct param_run_sift
{
	string imageFilename;
	string keyFilename;
	int    tArgv2;
	int    threadId;
};


void RunSIFT(void *aArg)
{
	//printf("thread: %d \n", running);

	param_run_sift *param = (param_run_sift*)aArg;
	
	//lock_guard<mutex> lock(gMutex);	
	//printf("%d %d \n", running, running_count);
	//printf("%d \n", running);

	//parse the input 
    char filePath[256];
    strcpy(filePath, param->imageFilename.c_str());
	//printf("%s \n", filePath);
	
	char keyfile[256];
	strcpy(keyfile, param->keyFilename.c_str());
	
	int scaleImgHt = param->tArgv2;
	//printf("scale image height: %d \n", scaleImgHt);
	
	
	//reading the image of first band using gdal
	printf("reading the image of first band using gdal \n");
	unsigned char* imageBuffer = NULL;
	int nImageHt,nImageWd;
	ReadGeoFileByte(filePath, 3, 1, &imageBuffer, nImageHt, nImageWd);
	
	
	//convert the image into the opencv format
	IplImage* pImage = cvCreateImage(cvSize(nImageWd, nImageHt),8,1);
	int nFeatImgHt  = pImage->height;
	int nFeatImgWd  = pImage->width;
	int nFeatScanwd = pImage->widthStep;
	for(int j=0; j<nFeatImgHt; j++)
		for(int i=0; i<nFeatImgWd; i++)
		{
			pImage->imageData[j*nFeatScanwd+i] = imageBuffer[j*nFeatImgWd+i];
		}
	free(imageBuffer);


	//if the image is not jpg format, then generate one jpg file, because 
	//bundler will load the jpg image , added by Xie Donghai, 2016.9.1
	char* postfix;
	GetPostfix(filePath, &postfix);
	if( strcmp(postfix, "tif")==0 || strcmp(postfix, "TIF")==0 )
	{	

		//save the jpg file
		//char* pdes = strrchr(filePath, '.');
		//int   index = pdes - filePath + 1;
		char* jpgfile;
		//strcpy(jpgfile, filePath);
		//strcpy(jpgfile+index, "jpg");
		GenerateProductFile(filePath, "product", "jpeg", &jpgfile);
		cvSaveImage(jpgfile, pImage);
	}
	
	//using opencv to load image
	/*
	//save the image dimension after resize
	IplImage* pImage = cvLoadImage(filePath, 0);
	int nFeatImgHt = pImage->height;
	int nFeatImgWd = pImage->width;
	*/

	double ratio = 1;
	if( pImage->height > scaleImgHt )
	{
		ratio = (double)(pImage->height) / (double)(scaleImgHt);
		nFeatImgHt = pImage->height / ratio;
		nFeatImgWd = pImage->width / ratio;
	}
	//cvReleaseImage(&pImage);
		
	//loading image and scale
	//pImage = cvLoadImage(filePath, 0);
	IplImage* pScaleImage = cvCreateImage( cvSize(nFeatImgWd, nFeatImgHt), 8, 1);
	cvResize(pImage, pScaleImage);		
	//printf("ht:%d  wd:%d \n", nFeatImgHt, nFeatImgWd);

	//histgram equalization
	cvEqualizeHist(pScaleImage, pScaleImage);
	

	//sift feature detect
	struct feature* pFeat = NULL;
	int nFeat;
	nFeat = sift_features(pScaleImage, &pFeat);
	//nFeat = sift_features_float(pScaleImage, &pFeat);

	for(int i=0; i<nFeat; i++)
	{
		pFeat[i].x *= ratio;
		pFeat[i].y *= ratio;
	}

	//printf("Feature Number: %d \n", nFeat);
	
	//save feature points		
	//writeSiftForBundler(pFeat, nFeat, keyfile); 

	//cout<<"output to file: "<<keyfile<<endl;
	writeSiftForBundlerBin(pFeat, nFeat, keyfile);
	free(pFeat);
	//////////////////////////////////////////////////////////////////////////
	
	//GiveOutCtrl();		
	//progressDialog->Update(fSingleStep*i, msg);
	
	cvReleaseImage(&pImage);
	cvReleaseImage(&pScaleImage);
			
	//lock_guard<mutex> lock(gMutex);	
	
	/*
	lock_guard<mutex> guard(mtx[running]);
	running_count--;	
	cv[running].notify_one();	
	*/

	lock_guard<mutex> guard(gMutex);
	running_count--;
	gCV.notify_all();
}

//multiple thread
int main(int argc, char* argv[])
{
	if(argc!=3)
	{
		printf("Usage: *.exe imagelist.txt scaleImgHt ");
		return 0;
	}

	char imagefilepath[256];
	int  nScaleImageHt;

	strcpy(imagefilepath, argv[1]);
	nScaleImageHt = atoi( argv[2]);

	int nImageCount = GetFileRows(imagefilepath);
	string *imageFilename = new string[nImageCount];
	string *keyFilename = new string[nImageCount];


	ifstream imageListReadIn;
	imageListReadIn.open(string(argv[1]), ios::in);
	for (int i = 0;i < nImageCount;i++)
	{
		imageListReadIn >> imageFilename[i];
		//cout<<imageFilename[i]<<endl;

		//generate key file
		char imagefile[256];
		strcpy(imagefile, imageFilename[i].c_str());
		//cout<<imagefile<<endl;

		char* keyfile = (char*)malloc(256);
		GenerateProductFile(imagefile, "product", "key", &keyfile);	
		keyFilename[i] = string(keyfile);

		//cout<<keyfile<<endl;
	}
	imageListReadIn.close();
	
	//tinythread 
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	int cpu_count = si.dwNumberOfProcessors; //reseve one core to deal other things
	//cpu_count = 1;

	/*
	mtx = new mutex[nImageCount];
	cv = new condition_variable[nImageCount];
	running_count = 0;
	running = 0;
	*/

	double step = 100.0 / nImageCount;

	param_run_sift *param = new param_run_sift[nImageCount];
	list<thread*> threadList;

	clock_t start = clock();

	for (int i = 0;i < nImageCount;i++)
	{
		WriteProgressValueToFile(step);
		
		FILE* fp = fopen( keyFilename[i].c_str(), "r" );
		if(fp!=NULL)
		{   
			fclose(fp);
			continue;
		}

		param[i].imageFilename = imageFilename[i];
		param[i].keyFilename = keyFilename[i];
		param[i].tArgv2 = atoi(argv[2]);
		//param[i].threadId = i;
				
		//running = i;		
		/*
		lock_guard<mutex> guard(mtx[i]);
		while (running_count == cpu_count)
			cv[i].wait(mtx[i]);
		running_count++;	
		*/

		lock_guard<mutex> guard(gMutex); 
		while (running_count == cpu_count)
			gCV.wait(gMutex);
		running_count++;	
		
		threadList.push_back(new thread(RunSIFT, &param[i]));

		//printf("  %d \n", running_count);

		//write the progress file
		
	} 

	list<thread*>::iterator it;
	for (it = threadList.begin();it != threadList.end();it++)
	{
		thread *t = *it;
		t->join();
		delete t;
	}

	clock_t end = clock();    
	printf("[SIFT] detecting features took %0.3fs\n", 
		(end - start) / ((double) CLOCKS_PER_SEC));

	return 0;
}



//single thread
int main_singlethread(int argc, char* argv[])
{
	if(argc!=4)
	{
		printf("Usage: *.exe imagefile keyfile scaleImgHt ");
		return 0;
	}
	
	//parse the input 
    char filePath[256];
    strcpy(filePath, argv[1]);
	printf("%s \n", filePath);
	
	char keyfile[256];
	strcpy(keyfile, argv[2]);
	
	int scaleImgHt = atoi(argv[3]);
	printf("scale image height: %d \n", scaleImgHt);
		
	//save the image dimension after resize
	IplImage* pImage = cvLoadImage(filePath, 0);
	int nFeatImgHt = pImage->height;
	int nFeatImgWd = pImage->width;
	double ratio = 1;
	if( pImage->height > scaleImgHt )
	{
		ratio = (double)(pImage->height) / (double)(scaleImgHt);
		nFeatImgHt = pImage->height / ratio;
		nFeatImgWd = pImage->width / ratio;
	}
	//cvReleaseImage(&pImage);
		
	//detect feature points	
	//save the feature information as file

	/*
	char keyfile[512];		
	char* pdes = strrchr(filePath, '\\');
	int   index = pdes - filePath;
    //get the imagepath
	char imagepath[512];
	strncpy(imagepath, filePath, index-1);
	//get the file title
	char title[512];
	strcpy(title, filePath+index+1);
	//generate new file
	sprintf(keyfile, "%s\\product\\%s", imagepath, title);
	//generate key file
	strcpy(keyfile+strlen(keyfile)-3, "key");
	*/
	
	/*
	strcpy(keyfile, filePath);
	char* pdes = strrchr(keyfile, '.');
	int   index = pdes - keyfile;
	strcpy(keyfile+index, "\0");
	strcat(keyfile, ".key");
	*/
	
	//if( IsFileExist(keyfile) )
	//	continue;
	
	//loading image and scale
	//pImage = cvLoadImage(filePath, 0);
	IplImage* pScaleImage = cvCreateImage( cvSize(nFeatImgWd, nFeatImgHt), 8, 1);
	cvResize(pImage, pScaleImage);		
	printf("ht:%d  wd:%d \n", nFeatImgHt, nFeatImgWd);

	//histgram equalization
	cvEqualizeHist(pScaleImage, pScaleImage);


	//sift feature detect
	struct feature* pFeat = NULL;
	int nFeat;
	nFeat = sift_features(pScaleImage, &pFeat);
	//nFeat = sift_features_float(pScaleImage, &pFeat);

	for(int i=0; i<nFeat; i++)
	{
		pFeat[i].x *= ratio;
		pFeat[i].y *= ratio;
	}

	printf("Feature Number: %d \n", nFeat);
	
	//save feature points		
	//writeSiftForBundler(pFeat, nFeat, keyfile); 
	writeSiftForBundlerBin(pFeat, nFeat, keyfile);
	free(pFeat);
	//////////////////////////////////////////////////////////////////////////
	
	//GiveOutCtrl();		
	//progressDialog->Update(fSingleStep*i, msg);
	
	cvReleaseImage(&pImage);
	cvReleaseImage(&pScaleImage);
	
	
	return 0;
}




/*
int main(int argc, char* argv[])
{
	if(argc!=2)
	{
		printf("Usage: *.exe  listfile.txt ");
		return 0;
	}

	//parse the input 
    char listFile[256];
    strcpy(listFile, argv[1]);
	printf("%s \n", listFile);	

	//reading image file path
	int nFile;
	nFile = GetFileRows(listFile);

	char** filenames = f2c(nFile, 256);
	char ts[256];
	int fisheye;
	double focus;
	FILE* fp =fopen(listFile, "r");	
	for(int i=0; i<nFile; i++)
	{
		fscanf(fp, "%s %d %lf", filenames[i], &fisheye, &focus);
	}
	fclose(fp);

	//save the image dimension after resize
	IplImage* pImage = cvLoadImage(filenames[0], 0);
	int nFeatImgHt = pImage->height;
	int nFeatImgWd = pImage->width;
	if( pImage->height > SINGLE_IMAGE_HT )
	{
		double ratio = pImage->height / (double)(SINGLE_IMAGE_HT);
		nFeatImgHt = pImage->height / ratio;
		nFeatImgWd = pImage->width / ratio;
	}
	cvReleaseImage(&pImage);	
	
	//1. detect feature points
	//msg = _T("Generate Feature Points ...");
	//double fSingleStep = (double)(detectTime) / (double)(vecImagePath.size());
	for(i=0; i<nFile; i++)
	{
		printf("%s \n", filenames[i]);
		//save the feature information as file
		char keyfile[512];
		strcpy(keyfile, filenames[i]);
		char* pdes = strrchr(keyfile, '.');
		int   index = pdes - keyfile;
		strcpy(keyfile+index, "\0");
		strcat(keyfile, ".key");		
		//if( IsFileExist(keyfile) )
		//	continue;		
		//loading image and scale
		IplImage* pImage = cvLoadImage(filenames[i], 0);
		IplImage* pScaleImage = cvCreateImage( cvSize(nFeatImgWd, nFeatImgHt), 8, 1);
		cvResize(pImage, pScaleImage);		
		
		//sift feature detect
		struct feature* pFeat = NULL;
		int nFeat;
		nFeat = sift_features(pScaleImage, &pFeat);	
		printf("Feature Number: %d \n", nFeat);
		
		//save feature points		
		//writeSiftForBundler(pFeat, nFeat, keyfile); 
		writeSiftForBundlerBin(pFeat, nFeat, keyfile);
		free(pFeat);
		//////////////////////////////////////////////////////////////////////////
		
		//GiveOutCtrl();		
		//progressDialog->Update(fSingleStep*i, msg);
		
		cvReleaseImage(&pImage);
		cvReleaseImage(&pScaleImage);
	}		
	return 0;
}
*/