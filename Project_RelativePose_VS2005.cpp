// Project_RelativePose_VS2005.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "stdlib.h"
#include "stdio.h"
#include "string.h"


//opencv lib
#ifdef OPENCV_1X 
#include "cv.h"
#include "highgui.h"
#include "cxcore.h"
#else
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc_c.h"
#include "opencv2/highgui/highgui.hpp"
using namespace cv;
#endif

//cvlib
#include "feature.hpp"
#include "sift.hpp"
#include "register.hpp"
#include "relativepose.hpp"


//coredll
#include "main.h"


IplImage* VerticalMosaic(IplImage* pLeft, IplImage* pRight)
{	
	int nLeftWd = pLeft->width;
	int nLeftHt = pLeft->height;
	int nScanWdLeft = pLeft->widthStep;

	int nRightWd = pRight->width;
	int nRightHt = pRight->height;
	int nScanWdRight = pRight->widthStep;

	int oht = nLeftHt + nRightHt;
	int owd = max(nLeftWd, nRightWd);

	IplImage* pMosaicImage = cvCreateImage( cvSize(owd,oht), 8, 3);
	int oscanWd = pMosaicImage->widthStep;


	for(int j=0; j<nLeftHt; j++)
		for(int i=0; i<nLeftWd; i++)
		{
			pMosaicImage->imageData[j*oscanWd + i*3]   = pLeft->imageData[j*nScanWdLeft + i*3];
			pMosaicImage->imageData[j*oscanWd + i*3+1] = pLeft->imageData[j*nScanWdLeft + i*3+1];
			pMosaicImage->imageData[j*oscanWd + i*3+2] = pLeft->imageData[j*nScanWdLeft + i*3+2];
		}

	for(int j=0; j<nRightHt; j++)
		for(int i=0; i<nRightWd; i++)
		{
			pMosaicImage->imageData[(j+nLeftHt)*oscanWd + i*3]   = pRight->imageData[j*nScanWdRight + i*3];
			pMosaicImage->imageData[(j+nLeftHt)*oscanWd + i*3+1] = pRight->imageData[j*nScanWdRight + i*3+1];
			pMosaicImage->imageData[(j+nLeftHt)*oscanWd + i*3+2] = pRight->imageData[j*nScanWdRight + i*3+2];
		}

	return pMosaicImage;
}


int _tmain(int argc, char* argv[])
{
	printf("%d \n", argc);

	char leftImageFile[256];
	char rightImageFile[256];
	char mosaicFile[256];
	char outFile[256];
	double focalLen = 0;
	bool bIsBundlerOut = false;
	

	strcpy(leftImageFile, "\0");
	strcpy(rightImageFile, "\0");


	//get the param
	for(int i=1; i<argc; i++)
	{
		//printf("%s \n", argv[i]);

		if( strcmp( argv[i], "-left" ) == 0 ) //left image
		{
			strcpy(leftImageFile, argv[i+1]);
			printf("%s \n", leftImageFile);
			i++;
		}
		else if( strcmp( argv[i], "-right" ) == 0 ) //right image
		{
			strcpy(rightImageFile, argv[i+1]);
			printf("%s \n", rightImageFile);
			i++;
		}
		else if( strcmp( argv[i], "-out" ) == 0 ) //output file
		{
			strcpy(outFile, argv[i+1]);
			printf("%s \n", outFile);
			i++;
		}
		else if( strcmp( argv[i], "-focallen" ) == 0 ) //input focal length
		{
			focalLen = atof( argv[i+1] );
			printf("%lf \n", focalLen);
			i++;
		}
		else if( strcmp( argv[i], "-bundlerout" ) == 0 ) //output same as bundler
		{
			bIsBundlerOut = true;
			printf("output according to bundler's format! \n");
		}
		else if( strcmp( argv[i], "-mosaic" ) == 0 )
		{
			strcpy(mosaicFile, argv[i+1]);
			printf("mosaic file: %s \n", mosaicFile);
			i++;
		}
	}
	//////////////////////////////////////////////////////////////////////////

	if( strlen(leftImageFile)<3 || strlen(leftImageFile)<3)
	{
		printf("no files... \n");
		return -1;
	}


	IplImage* pLeft  = cvLoadImage(leftImageFile);
	IplImage* pRight = cvLoadImage(rightImageFile);

	if(focalLen == 0)
	{
		focalLen = (pLeft->height + pLeft->width) * 0.5;
	}

	//1. detect feature points 
	printf("\n");
	CFeatureBase* pFeatDetect = new CSIFTFloat();
	ImgFeature lImageFeature, rImageFeature;
	pFeatDetect->Detect(leftImageFile, lImageFeature);
	pFeatDetect->Detect(rightImageFile, rImageFeature);
	delete[] pFeatDetect;
	

	//2. matching 
	printf("\n");
	CMatchBase* pMatch = new CSiftMatch();
	PairMatchRes mp;
	pMatch->Match(lImageFeature, rImageFeature, mp);
	delete[] pMatch;
	int nMatch = mp.matchs.size();
	//printf("Matching Point Number: %d \n", nMatch);
	
		
	//display 
	srand(100);
	for(int i=0; i<mp.matchs.size(); i++)
	{
		int li = mp.matchs[i].l;
		int ri = mp.matchs[i].r;
		CvPoint pl;
		pl.x = lImageFeature.featPts[li].x;
		pl.y = lImageFeature.featPts[li].y;
		CvPoint pr;
		pr.x = rImageFeature.featPts[ri].x;
		pr.y = rImageFeature.featPts[ri].y;
		int r = (double)(rand())/(double)(RAND_MAX)*255;
		int g = (double)(rand())/(double)(RAND_MAX)*255;
		int b = (double)(rand())/(double)(RAND_MAX)*255;
		cvDrawCircle(pLeft, pl, 2, CV_RGB(r,g,b), 2);
		cvDrawCircle(pRight, pr, 2, CV_RGB(r,g,b), 2);
	}
	//cvSaveImage("d:\\left.jpg", pLeft);
	//cvSaveImage("d:\\right.jpg", pRight);

	//3. relative pose 
	vector<Point2DDouble> lpts,rpts;
	nMatch = mp.matchs.size();
	for(int i=0; i<nMatch; i++)
	{
		int li = mp.matchs[i].l;
		int ri = mp.matchs[i].r;
		Point2DDouble pl,pr;
		pl.p[0] = lImageFeature.featPts[li].cx;
		pl.p[1] = lImageFeature.featPts[li].cy;
		lpts.push_back(pl);
		pr.p[0] = rImageFeature.featPts[ri].cx;
		pr.p[1] = rImageFeature.featPts[ri].cy;
		rpts.push_back(pr);
	}

	printf("\n ");
	CRelativePoseBase* pRP = new CEstimatePose5Point();
	CameraPara cam1, cam2;
	//intrinsic parameters
	cam1.focus = focalLen; //pLeft->width*0.5;
	cam2.focus = focalLen; //pRight->width*0.5;
	pRP->EstimatePose(lpts, rpts, cam1, cam2 );
	delete[] pRP;
	

	//output the results of matching
	if(strlen(mosaicFile)>3)
	{
		IplImage* pMosaic = VerticalMosaic(pLeft, pRight);

		for(int k=0; k<nMatch; k++)
		{	
			int    lx,ly;
			int    rx,ry;
			int    nLeftHt = pLeft->height;

			lx = lImageFeature.featPts[ mp.matchs[k].l ].x;
			ly = lImageFeature.featPts[ mp.matchs[k].l ].y;
			rx = rImageFeature.featPts[ mp.matchs[k].r ].x;
			ry = rImageFeature.featPts[ mp.matchs[k].r ].y;

			cvDrawCircle(pMosaic, cvPoint(lx, ly), 1, CV_RGB(255,0,0), 2);
			cvDrawCircle(pMosaic, cvPoint(rx, ry+nLeftHt), 1, CV_RGB(255,0,0), 2);
			cvDrawLine( pMosaic, cvPoint(lx, ly), cvPoint(rx, ry+nLeftHt), CV_RGB(255,0,0), 1 );
		}
		cvSaveImage(mosaicFile, pMosaic);
		cvReleaseImage(&pMosaic);
	}

	//display
	int rht1 = 480;
	double ratio1 = (double)(pLeft->height) / (double)(rht1);
	int rwd1 = (double)(pLeft->width) / (double)(ratio1);

	int rht2 = 480;
	double ratio2 = (double)(pRight->height) / (double)(rht2);
	int rwd2 = (double)(pRight->width) / (double)(ratio2);

	//show images
	cvNamedWindow("Left", 0);
	cvNamedWindow("Right", 0);
	cvResizeWindow("Left", rwd1, rht1);
	cvResizeWindow("Right", rwd2, rht2);
	cvShowImage("Left",  pLeft);
	cvShowImage("Right", pRight);

	cvWaitKey();

	cvReleaseImage(&pLeft);
	cvReleaseImage(&pRight);

	return 0;
}

