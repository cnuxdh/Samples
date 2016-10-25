// Project_SFM_x64.cpp : 定义控制台应用程序的入口点。
//

#include "stdio.h"

//cvlib
#include "relativepose.hpp"
#include "register.hpp"
#include "cvInvoke.hpp"
#include "ba.hpp"
#include "ceresba.hpp"

//corelib
#include "commonfile.h"
#include "file.h"

#include <vector>
using namespace std;

//test the ceres ba based on the simulated values
int main_simluate()
{
	vector<CameraPara> cameras;
	vector<ImgFeature> features;
	vector<TrackInfo>  tracks;

	int numpt = 20;

	//features.resize(numpt);
	//tracks.resize(numpt);
	cameras.resize(2);

	//intrinsic parameters
	double focus = 240;
	int ht = 480;
	int wd = 640;
	int x0 = 0;
	int y0 = 0;

	//outer parameters
	double R1[9],R2[9];
	double t1[3],t2[3];    
	memset(R1, 0, sizeof(double)*9);
	memset(R2, 0, sizeof(double)*9);
	memset(t1, 0, sizeof(double)*3);
	memset(t2, 0, sizeof(double)*3);
	R1[0] = R1[4] = R1[8] = 1;

	printf("Simulation... \n");

	double omiga, phi, kapa;

	//image 1
	omiga  = 1;
	phi    = -2;
	kapa   = 1.5;
	//GenerateRMatrix(omiga, phi, kapa, R2);
	GenerateRMatrixDirect(omiga, phi, kapa, R2);
	t2[0] = 20; t2[1] = 20; t2[2] = 10;
	for(int j=0; j<3; j++)
	{
		for(int i=0; i<3; i++)
		{
			printf("%lf ", R2[j*3+i]);
		}
		printf("\n");
	}
	printf("\n");

	//from 3D to 2D
	double grdpt[20][3] = {0,   0,  -220,
		160, 140, -200,
		-230, 100, -250,
		100, -100, -280,
		-250, -200, -300,
		-240,  100, -320,
		-211, -203, -310,
		26,  220, -280,
		-20, -10, -250,
		150, -230, -230,
		-50, -140, -350,
		30,  80, -325,
		-28,  110, -306,
		23, -120, -315,
		-150, 203, -312,						   
		-70, 100, -321,
		80, -49, -314,
		-70, -50, -354,
		20, 40, -343,
		-15, -60, -323,
	};

	//printf("Relative Pose \n");
	FILE* fp = fopen("c:\\temp\\simulatePair.txt", "w");
	fprintf(fp, "%d\n", numpt);
	vector<Point2DDouble> leftImagePts;
	vector<Point2DDouble> rightImagePts;
	
	ImgFeature leftFeat;
	ImgFeature rightFeat;
	leftFeat.ht = ht;
	leftFeat.wd = wd;
	rightFeat.ht = ht;
	rightFeat.wd = wd;	
	
	for(int i=0; i<numpt; i++)
	{
		srand(time(0));

		double ix1,iy1;
		GrdToImg(grdpt[i][0], grdpt[i][1], grdpt[i][2], &ix1, &iy1, 
			R1, t1, focus, x0, y0, ht, wd );

		double ix2,iy2;
		GrdToImg(grdpt[i][0], grdpt[i][1], grdpt[i][2], &ix2, &iy2, 
			R2, t2, focus, x0, y0, ht, wd );

		//printf("%lf %lf  %lf %lf \n", ix1, iy1, ix2, iy2);

		//random noise
		double rx1 = ( (double)(rand())/(double)(RAND_MAX) - 0.5)*2;
		double ry1 = ( (double)(rand())/(double)(RAND_MAX) - 0.5)*2;
		double rx2 = ( (double)(rand())/(double)(RAND_MAX) - 0.5)*2;
		double ry2 = ( (double)(rand())/(double)(RAND_MAX) - 0.5)*2;

		//coordinate translation from topleft to the center
		fprintf(fp, "%lf %lf  %lf %lf \n", ix1-wd*0.5+rx1, ht*0.5-iy1+ry1, 
			ix2-wd*0.5+rx2, ht*0.5-iy2+ry2);

		printf("%lf %lf  %lf %lf \n", ix1-wd*0.5+rx1, ht*0.5-iy1+ry1, 
			ix2-wd*0.5+rx2, ht*0.5-iy2+ry2);

		Point2DDouble pl,pr;
		pl.x = ix1-wd*0.5 + rx1;
		pl.y = ht*0.5-iy1 + ry1;
		pr.x = ix2-wd*0.5 + rx2;
		pr.y = ht*0.5-iy2 + ry2;

		pl.p[0] = pl.x;
		pl.p[1] = pl.y;
		pr.p[0] = pr.x;
		pr.p[1] = pr.y;
		
		leftImagePts.push_back(pl);
		rightImagePts.push_back(pr);

		//save the feature points
		stPtFeature featpt;
		featpt.cx = pl.x;
		featpt.cy = pl.y;
		leftFeat.featPts.push_back(featpt);

		featpt.cx = pr.x;
		featpt.cy = pr.y;
		rightFeat.featPts.push_back(featpt);
		
		//save into the tracks
		TrackInfo singleTrack;
		ImageKey ikey;
		ikey.first = 0;
		ikey.second = i;
		singleTrack.views.push_back(ikey);
		ikey.first = 1;
		ikey.second = i;
		singleTrack.views.push_back(ikey);
		
		tracks.push_back(singleTrack);
	}
	fclose(fp);

	features.push_back(leftFeat);
	features.push_back(rightFeat);

	//getchar();

	//relative pose estimation
	CRelativePoseBase* pRP = new CEstimatePose5Point(); 
	cameras[0].focus = focus*0.98;
	cameras[1].focus = focus*0.98;
	pRP->EstimatePose(leftImagePts, rightImagePts, cameras[0], cameras[1]);

	//recover 3D point
	CTriangulateBase* pTriangulate = new CTriangulateCV();
	vector<Point3DDouble> gpts;
	pTriangulate->Triangulate(leftImagePts, rightImagePts, cameras[0], cameras[1], gpts);

	for(int i=0; i<gpts.size(); i++)
	{
		tracks[i].grd = gpts[i];
	}

	CeresBA(tracks, features, /*cameraIDOrder,*/ cameras);


	return 0;
}


//process the real images
int main(int argc, char** argv[])
{
	printf("SFM integration .... \n");

	char* imagepath = "C:\\Work\\Data\\relativePose";

	char** filenames = NULL;
	int n=0;
	int nfile = 0;
	GetDirFileName(filenames, imagepath, &n, &nfile, "JPG", 0);
	filenames = f2c(nfile, 256);
	GetDirFileName(filenames, imagepath, &n, &nfile, "JPG", 1);

	printf("image number: %d \n", nfile);

	if(nfile<2)
	{
		printf("images are less than 2 ! \n");
		return -1;
	}

	//1. generating the image feature points
	vector<ImgFeature> imgFeatures;
	DetectFileFeaturePts(filenames, nfile, imgFeatures);
	
	
	//2. matching images 
	vector<PairMatchRes> matchRes; 
	MatchImageFiles(imgFeatures, matchRes);

	
	//3.generating the tracks
	////test
	//imgFeatures.resize(4);
	//int nFeat = 5000;
	//imgFeatures[0].featPts.resize(nFeat);
	//imgFeatures[1].featPts.resize(nFeat);
	//imgFeatures[2].featPts.resize(nFeat);
	//imgFeatures[3].featPts.resize(nFeat);
	////char* matchfile = "c:\\temp\\match_file.txt";
	//char* matchfile = "c:\\sfm\\match_init.txt";
	//LoadMatchFile(matchfile, matchRes);
	
	vector<TrackInfo> tracks; 
	CGenerateTracksBase* pGenerateTrack = new CFastGenerateTrack();
	pGenerateTrack->GenerateTracks(imgFeatures, matchRes, tracks);
	
	//output the tracks
	FILE* fp = fopen("c:\\temp\\mytrack.txt", "w");
	fprintf(fp, "%d \n", tracks.size());
	for(int i=0; i<tracks.size(); i++)
	{
		fprintf(fp, "%d  ", tracks[i].views.size());
		for(int j=0; j<tracks[i].views.size(); j++)
		{
			fprintf(fp, " %d %d ", tracks[i].views[j].first, tracks[i].views[j].second );
		}
		fprintf(fp, "\n");
	}
	fclose(fp);


	//4. bundle adjustment
	int numImage = imgFeatures.size();
	vector<CameraPara> cameras; 
	cameras.resize(numImage);
	double focalLen = imgFeatures[0].ht; //(imgFeatures[0].ht + imgFeatures[0].wd) * 0.5;
	for(int i=0; i<numImage; i++)
	{
		cameras[i].focus = focalLen;
	}
	CBABase* pBA = new CCeresBA();
	pBA->BundleAdjust( cameras.size(), cameras, imgFeatures, matchRes, tracks); 

	
	return 0;
}

