// Project_BATest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"


//cvlib
#include "relativepose.hpp"
#include "register.hpp"
#include "cvInvoke.hpp"
#include "ba.hpp"
#include "ceresba.hpp"


//corelib
#include "commonfile.h"
//#include "File.h"


#include <vector>
using namespace std;


int main_realimages(int argc, char* argv[])
{
	printf("SFM integration .... \n");

	char imagepath[256] = "C:\\Work\\Data\\ba2";

	if(argc==2)
	{
		strcpy(imagepath, argv[1]);
		printf("path: %s \n", imagepath);
	}

	char** filenames = NULL;
	int n=0;
	int nfile = 0;

	
	GetDirFileName(filenames, imagepath, &n, &nfile, "JPG", 0);
	filenames = f2c(nfile, 256);
	GetDirFileName(filenames, imagepath, &n, &nfile, "JPG", 1);

	if(nfile==0)
	{
		GetDirFileName(filenames, imagepath, &n, &nfile, "jpg", 0);
		filenames = f2c(nfile, 256);
		GetDirFileName(filenames, imagepath, &n, &nfile, "jpg", 1);
	}
	

	printf("image number: %d \n", nfile);

	if(nfile<2)
	{
		printf("images are less than 2 ! \n");
		return -1;
	}

	//1. generating the image feature points
	vector<ImgFeature> imgFeatures;
	DetectFileFeaturePts(filenames, nfile, imgFeatures, 600);

	//2. matching images 
	vector<PairMatchRes> matchRes; 
	MatchImageFiles(imgFeatures, matchRes);

	//3.generating the tracks
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
	double focalLen = (imgFeatures[0].ht + imgFeatures[0].wd) * 0.5;
	for(int i=0; i<numImage; i++)
	{
		cameras[i].focus = focalLen; //initialize the focal length 
		memset(cameras[i].R, 0, sizeof(double)*9);
		cameras[i].R[0] = 1;
		cameras[i].R[4] = 1;
		cameras[i].R[8] = 1;
	}

	CBABase* pBA = new CCeresBA();
	pBA->BundleAdjust( cameras.size(), cameras, imgFeatures, matchRes, tracks); 
	
	return 0;
}



int main_realimages_bundler(int argc, char* argv[])
{
	char imagepath[256] = "C:\\Work\\Data\\ba2";

	if(argc==2)
	{
		strcpy(imagepath, argv[1]);
	}

	char** filenames = NULL;
	int n=0;
	int nfile = 0;
	GetDirFileName(filenames, imagepath, &n, &nfile, "JPG", 0);
	filenames = f2c(nfile, 256);
	GetDirFileName(filenames, imagepath, &n, &nfile, "JPG", 1);

	if(nfile==0)
	{
		GetDirFileName(filenames, imagepath, &n, &nfile, "jpg", 0);
		filenames = f2c(nfile, 256);
		GetDirFileName(filenames, imagepath, &n, &nfile, "jpg", 1);
	}

	printf("image number: %d \n", nfile);

	if(nfile<2)
	{
		printf("images are less than 2 ! \n");
		return -1;
	}

	
	//1. feature detection
	vector<CImageDataBase*> vecImageDataPointer;
	for(int i=0; i<nfile; i++)
	{
		CImageDataBase* pImageData = new CImageFeature();
		pImageData->Load(filenames[i]);
		pImageData->DetectPtFeature(SIFT_FLOAT_FEATURE);
		vecImageDataPointer.push_back(pImageData);
	}

	//2. matching
	vector<PairMatchRes> vecMatch;
	CMatchBase* pMatch = new CKNNMatch();
	//CMatchBase* pMatch = new CSiftMatch();
	for(int i=0; i<nfile; i++)
		for(int j=i+1; j<nfile; j++)
		{
			printf("Matching %d-%d \n", i,j);
			PairMatchRes mt;
			mt.lId = i;
			mt.rId = j;
			
			pMatch->Match( vecImageDataPointer[i]->GetImageFeature(), 
						   vecImageDataPointer[j]->GetImageFeature(), mt);

			//if( mt.inlierRatio>0.5 && mt.matchs.size()>16 )
			if( mt.matchs.size()>16 )
			{
				vecMatch.push_back(mt);
				printf(" inlier: %lf  match: %d \n", mt.inlierRatio, mt.matchs.size());
			}
		}

	//3. generating tracks
	printf("\n Generating Tracks.... \n");
	CGenerateTracksBase* pGenerateTracks = new CMyGenerateTrack();
	vector<TrackInfo> vecTrack;
	pGenerateTracks->GenerateTracks(vecMatch, vecImageDataPointer, vecTrack);
	PrintTracks(vecTrack, "c:\\temp\\bundle_tracks.txt");

	if(vecTrack.size()<8)
	{
		printf("Tracks are less than the threshold ! \n");
		return -1;
	}

	//4. bundle adjustment
    CBABase* pBA = new CSBA();
	vector<CameraPara> cameras;
    cameras.resize(nfile);
	for(int i=0; i<cameras.size(); i++)
	{
		cameras[i].focus = vecImageDataPointer[i]->GetInitFocus();
	}
	pBA->BundleAdjust( cameras.size(), cameras, vecImageDataPointer, 
		vecMatch, vecTrack, "c:\\temp");

	for(int i=0; i<nfile; i++)
	{
		delete vecImageDataPointer[i];
		//cvReleaseImage(&pImages[i]);
	}
	delete pGenerateTracks;
	delete pMatch;
	delete pBA;

	printf("Finished! \n");

	return 0;
}


int main_simulate_multviews(int argc, char* argv[])
{
	//from 3D to 2D 
	double grdpt[20][3] = 
	{0,   0,  -220,
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

	//intrinsic parameters
	double focus = 240;
	int ht = 480;
	int wd = 640;
	int x0 = 0;
	int y0 = 0;

	double omiga[3] = {0,  1.5, 1};
	double phi[3]   = {0, -1,  1.2};
	double kapa[3]  = {0,  2,  -1.5};
	double tx[3] = {0, 10, 20};
	double ty[3] = {0, 10, 20};
	double tz[3] = {0, 2,  1.5};

	int numpt = 20;

	//1.generate the feature points of several images
	vector<ImgFeature> feats;
	for(int i=0; i<3; i++)
	{
		double R[9];
		GenerateRMatrixDirect(omiga[i], phi[i], kapa[i], R);
		double t[3];
		t[0] = tx[i]; t[1] = ty[i]; t[2] = tz[i];

		ImgFeature feat;
		feat.ht = ht;
		feat.wd = wd;

		for(int i=0; i<numpt; i++)
		{
			srand(time(0));

			double ix,iy;
			GrdToImg(grdpt[i][0], grdpt[i][1], grdpt[i][2], &ix, &iy, 
				R, t, focus, x0, y0, ht, wd );

			//random noise
			double rx = ( (double)(rand())/(double)(RAND_MAX) - 0.5 )*2;
			double ry = ( (double)(rand())/(double)(RAND_MAX) - 0.5 )*2;

			double cx = ix-wd*0.5+rx;
			double cy = ht*0.5-iy+ry;

			stPtFeature pt;
			pt.cx = cx;
			pt.cy = cy;

			feat.featPts.push_back(pt);	
		}
		feats.push_back(feat);
	}

	//2.generate the matches
	vector<PairMatchRes> matchRes; 
	for(int j=0; j<3; j++)
	{
		for(int i=j+1; i<3; i++)
		{
			PairMatchRes sm;
			sm.lId = j;
			sm.rId = i;
			sm.inlierRatio = 1;

			for(int k=0; k<numpt; k++)
			{
				MatchPairIndex mp;
				mp.l = k;
				mp.r = k;
				sm.matchs.push_back(mp);
			}		
			matchRes.push_back(sm);
		}
	}

	//3.generate the tracks
	vector<TrackInfo> tracks; 
	CGenerateTracksBase* pGenerateTrack = new CFastGenerateTrack();
	pGenerateTrack->GenerateTracks(feats, matchRes, tracks);


	//4. sparse bundle adjustment
	int numImage = feats.size();
	vector<CameraPara> cameras; 
	cameras.resize(numImage);
	double focalLen = focus*0.6; //feats[0].ht; //provide the initial focal length
	for(int i=0; i<numImage; i++)
	{
		cameras[i].focus = focalLen;
	}
	CBABase* pBA = new CCeresBA();
	pBA->BundleAdjust( cameras.size(), cameras, feats, matchRes, tracks); 


	return 0;
}


int _tmain(int argc, char* argv[])
{
	printf("BA test.... \n");

	//main_realimages_bundler(argc, argv);
	main_realimages(argc, argv);

	//main_simulate_multviews(argc, argv);


	return 0;
}

