// Project_Fusion_VS2010.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <iostream>
#include <string>
using namespace std;

#include "xml.hpp"

#include "blend.hpp"
#include "geotiff.h"

//corelib
#include "file.h"
#include "commonfile.h"
#include "CommonFuncs.h"


int bIsFileExit(char* filepath)
{
	FILE* fp = fopen(filepath, "r");
	if(fp!=NULL)
	{
		fclose(fp);
		return 1;
	}
	else
		return 0;
}

int MyGetFileRows(char* file)
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

/* V1.1: mosaic "jpeg" + "geo" format in "product" directory
 input: list.txt  outfile  output_resolution  
*/
int _tmain_yitu(int argc, char* argv[])
//int _tmain(int argc, _TCHAR* argv[])
{	
	if( argc!=5 )
	{
		printf("input error ! \n");
		return -1;
	}
	
	char imagelistFile[256];
	char outfile[256];
	double  outResolution = 0;
	int  nLevel = 1;
	int  ntype = 1;
	
	strcpy(imagelistFile, argv[1]);
	strcpy(outfile, argv[2]);
	outResolution = atof(argv[3]);
	ntype = atoi(argv[4]);

	//nLevel = atoi(argv[3]);
	
	//reading image list file
	int nImageNumber = MyGetFileRows(imagelistFile);
	printf("image number: %d \n", nImageNumber);

	if(nImageNumber<=1)
		return -1;

	char* orthofile = (char*)malloc(256);
	int nfile = nImageNumber;
	char** filenames = f2c(nfile, 256);
	int index = 0;
	FILE* fp = fopen(imagelistFile, "r");
	for(int i=0; i<nfile; i++)
	{
		char jpgFile[256];
		//int t;
		//double focalLen;
		//fscanf(fp, "%s %d %lf", jpgFile, &t, &focalLen);
		fscanf(fp, "%s ", jpgFile);
		
		/*
		char orthofile[256];
		strcpy(orthofile, jpgFile);
		int len = strlen(orthofile);
		strcpy(orthofile+len-3, "tif");
		*/
				
		GenerateProductFile(jpgFile, "product", "jpeg", &orthofile);

		if(bIsFileExit(orthofile))
		{
			strcpy(filenames[index], orthofile);
			index++;
		}
	}
	fclose(fp);

	free(orthofile);

	if(index<1)
	{
		printf("no files to mosaic... \n");
		return -1;
	}

	//BlendMosaic(filenames, index, outfile, outResolution, ntype);	
	//LoGBlend(filenames, index, outfile, nLevel);
	
	GeoTiffBlend(filenames, index, outfile, outResolution);
	 
	FreeArray_char(filenames, nfile, 256);

	return 0;
}

//for directly mosaic RGB *.tif 
int _tmain_directMosaic(int argc, char* argv[])
//int main(int argc, char* argv[])
{

	if(argc!=5)
	{
		printf("*.exe  imagelist.txt  outfile.tif  resolution  type  \n");
		return 1;
	}
	char imagelistfile[256];
	char outfile[256];

	strcpy(imagelistfile, argv[1]);
	strcpy(outfile, argv[2]);
	double outResolution = atof(argv[3]);
	int    nType = atoi(argv[4]);

	printf("resolution: %lf \n", outResolution);
	
	int nImageNumber = MyGetFileRows(imagelistfile);
	printf("image number: %d \n", nImageNumber);

	if(nImageNumber<=1)
		return -1;

	int nfile = nImageNumber;
	char** filenames = f2c(nfile, 256);
	FILE* fp = fopen(imagelistfile, "r");
	for(int i=0; i<nfile; i++)
	{
		char jpgFile[256];
		int t;
		double focalLen;
		fscanf(fp, "%s ", filenames[i]);
	}
	fclose(fp);
	
	MosaicGeoTiff(filenames, nfile, outfile, outResolution, nType);	

	FreeArray_char(filenames, nfile, 256);

	return 0;
}

//for multi-band mosaic in "product" directory
//int _tmain_mb(int argc, char* argv[])
int main(int argc, char* argv[])
{
	if(argc!=5)
	{
		printf("*.exe  imagelist.txt  outfile.tif  resolution  type  \n");
		return 1;
	}
	char imagelistfile[256];
	char outfile[256];

	strcpy(imagelistfile, argv[1]);
	strcpy(outfile, argv[2]);
	double outResolution = atof(argv[3]);
	int    nType = atoi(argv[4]);

	printf("resolution: %lf \n", outResolution);

	int nImageNumber = MyGetFileRows(imagelistfile);
	printf("image number: %d \n", nImageNumber);

	if(nImageNumber<=1)
		return -1;

	char* tiffile;
	char* jpegfile;

	int nfile = nImageNumber;
	char** filenames = f2c(nfile, 256);
	FILE* fp = fopen(imagelistfile, "r");
	int index = 0;
	for(int i=0; i<nfile; i++)
	{
		int t;
		double focalLen;
		char rawfile[256];
		fscanf(fp, "%s ", rawfile);


		GenerateProductFile(rawfile, "product", "tif", &tiffile);
		GenerateProductFile(rawfile, "product", "jpeg", &jpegfile);

		if( IsFileExist(tiffile) )
		{
			strcpy( filenames[index], tiffile );
			index ++;
		}
		else if(IsFileExist(jpegfile))
		{
			strcpy( filenames[index], jpegfile );
			index ++;
		}
	}
	fclose(fp);


	MosaicGeoTiff(filenames, index, outfile, outResolution, nType);	


	FreeArray_char(filenames, nfile, 256);

	return 0;
}

/*
 input: imagepath  outfile  nlevel  
*/
int _tmain_v_1_0(int argc, char* argv[])
{
	//string filename = "d:\\test.yml";
	//cout<<filename<<endl;
	//TestFileStorageWrite();

	if( argc!=4 )
	{
		printf("input error ! \n");

		return -1;
	}
	
	//char* imagepath = "F:\\Data\\UAV\\byler\\2-sub\\product";
	char imagepath[256];
	char outfile[256];
	int  nLevel = 0;

	strcpy(imagepath, argv[1]);
	strcpy(outfile, argv[2]);
	nLevel = atoi(argv[3]);
	 
	
	char** filenames = NULL;
	int n=0;
	int nfile=0;
	GetDirFileName(filenames, imagepath, &n, &nfile, "tif", 0);
	filenames = f2c(nfile, 256);
	GetDirFileName(filenames, imagepath, &n, &nfile, "tif", 1);

	printf("%d \n", nfile);

	//stGeoInfo geoinfo;
	//GetGeoInformation( filenames[0], geoinfo);
	
	LoGBlend(filenames, nfile, outfile, nLevel);
	


	return 0;
}

