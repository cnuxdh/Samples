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


#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/calib3d/calib3d.hpp"
using namespace cv;


int main_simulate_multviews(int argc, char* argv[])
//int main()
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
			double rx = ( (double)(rand())/(double)(RAND_MAX) - 0.5 )*1;
			double ry = ( (double)(rand())/(double)(RAND_MAX) - 0.5 )*1;

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
	double focalLen = focus*1.2; //feats[0].ht; //provide the initial focal length
	for(int i=0; i<numImage; i++)
	{
		cameras[i].focus = focalLen;
	}
	CBABase* pBA = new CCeresBA();
	pBA->BundleAdjust( cameras.size(), cameras, feats, matchRes, tracks); 
	

	return 0;
}


//test the ceres ba based on the simulated values
int main_simulate_pair(int argc, char* argv[])
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
	double R1[9],R2[9],R3[9];
	double t1[3],t2[3],t3[3];    
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
	t2[0] = 10; t2[1] = 10; t2[2] = 1;
	for(int j=0; j<3; j++)
	{
		for(int i=0; i<3; i++)
		{
			printf("%lf ", R2[j*3+i]);
		}
		printf("\n");
	}
	printf("\n");


	//image 2
	omiga  = -1;
	phi    = 1;
	kapa   = 1.2;
	//GenerateRMatrix(omiga, phi, kapa, R2);
	GenerateRMatrixDirect(omiga, phi, kapa, R3);
	t3[0] = 20; t3[1] = 20; t3[2] = 0.5;


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
	vector<Point2DDouble> pts1;
	vector<Point2DDouble> pts2;
	vector<Point2DDouble> pts3;

	
	ImgFeature feat1;
	ImgFeature feat2;
	ImgFeature feat3;
	feat1.ht = ht;
	feat1.wd = wd;
	feat2.ht = ht;
	feat2.wd = wd;	
	feat3.ht = ht;
	feat3.wd = wd;
	
	for(int i=0; i<numpt; i++)
	{
		srand(time(0));

		double ix1,iy1;
		GrdToImg(grdpt[i][0], grdpt[i][1], grdpt[i][2], &ix1, &iy1, 
			R1, t1, focus, x0, y0, ht, wd );

		double ix2,iy2;
		GrdToImg(grdpt[i][0], grdpt[i][1], grdpt[i][2], &ix2, &iy2, 
			R2, t2, focus, x0, y0, ht, wd );

		double ix3,iy3;
		GrdToImg(grdpt[i][0], grdpt[i][1], grdpt[i][2], &ix3, &iy3, 
			R3, t3, focus, x0, y0, ht, wd );


		//printf("%lf %lf  %lf %lf \n", ix1, iy1, ix2, iy2);

		//random noise
		double rx1 = ( (double)(rand())/(double)(RAND_MAX) - 0.5)*1.2;
		double ry1 = ( (double)(rand())/(double)(RAND_MAX) - 0.5)*1.2;
		double rx2 = ( (double)(rand())/(double)(RAND_MAX) - 0.5)*1.2;
		double ry2 = ( (double)(rand())/(double)(RAND_MAX) - 0.5)*1.2;

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
		
		pts1.push_back(pl);
		pts2.push_back(pr);

		//save the feature points
		stPtFeature featpt;
		featpt.cx = pl.x;
		featpt.cy = pl.y;
		feat1.featPts.push_back(featpt);

		featpt.cx = pr.x;
		featpt.cy = pr.y;
		feat2.featPts.push_back(featpt);
		
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

	features.push_back(feat1);
	features.push_back(feat2);

	//getchar();

	//relative pose estimation
	CRelativePoseBase* pRP = new CEstimatePose5Point(); 
	cameras[0].focus = focus*0.9;
	cameras[1].focus = focus*0.9;
	pRP->EstimatePose(pts1, pts2, cameras[0], cameras[1]);

	//recover 3D point
	CTriangulateBase* pTriangulate = new CTriangulateCV();
	vector<Point3DDouble> gpts;
	pTriangulate->Triangulate(pts1, pts2, cameras[0], cameras[1], gpts);

	for(int i=0; i<gpts.size(); i++)
	{
		tracks[i].grd = gpts[i];
	}

	CeresBA(tracks, features, /*cameraIDOrder,*/ cameras);
	

	return 0;
}


//process the real images
//int main_realimages(int argc, char* argv[])
int main_realimages(int argc, char* argv[])
{
	printf("SFM integration .... \n");

	char imagepath[256] = "C:\\Work\\Data\\ba1";

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

int TestRotate()
{
	double ax = 0;
	double ay = 0;
	double az = 0;

	//generate rotation matrix based on Tsai paper
	double R[9];
	GenerateRMatrixDirect(ax, ay, az, R);

	printf("\n Rotation Angle x:%lf y:%lf z:%lf \n", ax, ay, az);
	for(int j=0; j<3; j++)
	{
		for(int i=0; i<3; i++)
		{
			printf("%lf ", R[j*3+i]);
		}
		printf("\n");
	}

	//rotation matrix to axis-angle
	double aa[3];
	rot2aa(R, aa);
	printf("\n axis-angle vector: %lf %lf %lf \n", aa[0], aa[1], aa[2]);


	//axis-angle to rotation matrix column order 
	double Rt[9];
	aa2rot(aa, Rt);
	printf("\n Rotation generate by axis-angle vector... \n");
	for(int j=0; j<3; j++)
	{
		for(int i=0; i<3; i++)
		{
			printf("%lf ", Rt[j*3+i]);
		}
		printf("\n");
	}

	double nR[9];
	//transpose(Rt, nR, 3, 3);
	for(int j=0; j<3; j++)
	{
		for(int i=0; i<3; i++)
		{
			nR[j*3+i] = Rt[i*3 + j];
		}
	}

	//rotation matrix to Eular angle
	double ea[3];
	rot2eular(nR, ea);
	printf("\n Eular angle: %lf %lf %lf \n", ea[0], ea[1], ea[2]);

	return 0;
}



void on_leftimage_mouse( int event, int x, int y, int flags, void* param)
{
	if( event == CV_EVENT_LBUTTONDOWN )
	{
		CvPoint cp;
		cp.x = x;
		cp.y = y;
		
	}
}

void on_rightimage_mouse( int event, int x, int y, int flags, void* param)
{
	if( event == CV_EVENT_LBUTTONDOWN )
	{

		CvPoint cp;
		cp.x = x;
		cp.y = y;

	}
}

int TestPanoMatch()
{

	char* lImageFile = "C:\\Work\\Data\\panorama\\Pano_Cam_1108\\ladybug_panoramic_000170.jpg";
	char* rImageFile = "C:\\Work\\Data\\panorama\\Pano_Cam_1108\\ladybug_panoramic_000170.jpg";
	
	
	cvSetMouseCallback("Left", on_leftimage_mouse, NULL );
	cvSetMouseCallback("Right", on_rightimage_mouse, NULL );

	return 0;
}

int main_realimages_bundler(int argc, char* argv[])
{
	char imagepath[256] = "C:\\Work\\Data\\ba1";

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





	return 0;
}


int main(int argc, char* argv[])
{
	//TestRotate();

	//main_simulate_pair(argc, argv);
	//main_simulate_multviews(argc, argv);
	main_realimages(argc,argv);
}