// Project_BATest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

//#include "mmsystem.h"


//cvlib
#include "relativepose.hpp"
#include "register.hpp"
#include "cvInvoke.hpp"
#include "ba.hpp"
#include "ceresba.hpp"
#include "panorama.hpp"
#include "bundlerio.hpp"
#include "panoba.hpp"
#include "sift.hpp"
#include "interactReg.hpp"
#include "pos.hpp"
#include "funcs.hpp"


//corelib
#include "commonfile.h"
//#include "File.h"

#ifdef OPENCV_1X 
#include "cv.h"
#include "highgui.h"
#include "cxcore.h"
#else
//opencv
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/imgproc/imgproc_c.h"
#endif


#include <vector>
using namespace std;
using namespace cv;


int main_realimages(int argc, char* argv[])
{
	//DWORD start = timeGetTime();
	double t = (double)getTickCount();

	//set the type of camera
	CameraType camType = PerspectiveCam;

	printf("SFM integration .... \n");

	char imagepath[256] = "C:\\Work\\Data\\ba1";
	//char imagepath[256] = "C:\\Work\\Data\\panorama\\test";

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
	DetectFileFeaturePts(filenames, nfile, imgFeatures, 480);

	//2. matching images 
	vector<PairMatchRes> matchRes; 
	MatchImageFiles(imgFeatures, matchRes, camType);

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
		cameras[i].rows = imgFeatures[0].ht;
		cameras[i].cols = imgFeatures[0].wd;
		cameras[i].camtype = camType;
	}
	
	CBABase* pBA = NULL;

	if(camType == PerspectiveCam)
	{
		pBA = new CCeresBA();
	}
	else if(camType == PanoramCam)
	{
		pBA = new CPanoBA();
	}

	pBA->BundleAdjust( cameras.size(), cameras, imgFeatures, matchRes, tracks); 
	
	delete pBA;

	t = ((double)getTickCount() - t)/getTickFrequency();

	printf("\n running time: %lf \n\n", t);

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
			//GrdToImg(grdpt[i][0], grdpt[i][1], grdpt[i][2], &ix, &iy, 
				//R, t, focus, x0, y0, ht, wd );

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


int main_pano_rp(int argc, char* argv[])
{
	char* imagepath = "C:\\Work\\Data\\panorama\\ladybug_jpg";

	char** filenames = NULL;
	int n=0;
	int nfile =0;
	GetDirFileName(filenames, imagepath, &n, &nfile, "jpg", 0);
	filenames = f2c(nfile, 256);
	GetDirFileName(filenames, imagepath, &n, &nfile, "jpg", 1);

	FILE* fp = fopen("c:\\temp\\5point_relativepose.txt", "w");
	//fprintf(fp, "pitch	 roll	 yaw	 t0	 t1	 t2 \n");
	
	for(int i=0; i<nfile-1; i++)
	//int i = 1;
	{
		printf("%d-%d \n", i, i+1);

		ImgFeature lImageFeats;
		ImgFeature rImageFeats;
		
		DetectFileFeaturePts(filenames[i],   lImageFeats, 640);
		DetectFileFeaturePts(filenames[i+1], rImageFeats, 640);

		int ht = lImageFeats.ht;
		int wd = lImageFeats.wd;

		//2. matching
		printf("Matching... \n");
		CMatchBase* pMatch = new CPanoMatch();
		vector<MatchPairIndex> mi;
		pMatch->Match(lImageFeats, rImageFeats, mi);
		printf("Match Pair: %d \n", mi.size());
		delete pMatch;

		//3. pose estimation and compute residual error 
		double ratio = 0.6;
		vector<Point3DDouble> lptPano;     //3D spherical points
		vector<Point3DDouble> rptPano;
		vector<Point2DDouble> lptImagePts; //image points
		vector<Point2DDouble> rptImagePts;
		vector<int> matchIndex;
		double radius = (double)(wd) / (2*PI); 

		for(int i=0; i<mi.size(); i++)
		{
			int li = mi[i].l;
			int ri = mi[i].r;

			Point2DDouble pl;
			pl.x = lImageFeats.featPts[li].col;
			pl.y = lImageFeats.featPts[li].row;
			Point2DDouble pr;
			pr.x = rImageFeats.featPts[ri].col;
			pr.y = rImageFeats.featPts[ri].row;

			//remove the match without motion
			double dis = sqrt( (double)( (pl.x-pr.x)*(pl.x-pr.x) + (pl.y-pr.y)*(pl.y-pr.y)) );
			if(dis<5) continue;

			if( pl.y<ht*ratio && pr.y<ht*ratio )
			{
				Point2DDouble ip;
				ip.p[0] = pl.x;
				ip.p[1] = pl.y;
				lptImagePts.push_back(ip);
				ip.p[0] = pr.x;
				ip.p[1] = pr.y;
				rptImagePts.push_back(ip);

				Point3DDouble gp;					
				SphereTo3D_center(pl.x, pl.y, radius, gp.p[0], gp.p[1], gp.p[2]);
				lptPano.push_back(gp);
				SphereTo3D_center(pr.x, pr.y, radius, gp.p[0], gp.p[1], gp.p[2]);
				rptPano.push_back(gp);
			}
		}

		double R[9];
		double T[3];
		vector<double> residual;
		residual.resize(lptPano.size());
		dll_EstimatePose5Point_Pano(lptPano, rptPano, radius, 500, 2.5, R, T, residual);
	
		
		//double eT[3];
		//CalculateExplicitT(R, T, eT);
		double ax = atan( R[5]/R[8] ) / PI*180; 
		double ay = asin( -R[2] ) /PI*180;
		double az = atan( R[1]/R[0]) /PI*180;		
		printf("%d-%d \n", i, i+1);
		printf("Rotation... \n");
		printf("%lf %lf %lf \n", ax, ay, az);
		printf("Translation .... \n");
		printf("%lf %lf %lf \n", T[0], T[1], T[2]);	
		//fprintf(fp, "%d-%d \n", i, i+1);
		//fprintf(fp, "rotation: %lf %lf %lf \n", ax, ay, az);
		//fprintf(fp, "translation: %lf %lf %lf \n", T[0], T[1], T[2]);
		fprintf(fp, "%lf %lf %lf %lf %lf %lf\n", ax, ay, az, T[0], T[1], T[2]);


		
		//############# detect sift feature points of epipolar images ###########
		GeneratePanoEpipolarImageHeading(R, T, filenames[i], filenames[i+1]);
		//GeneratePanoEpipolarImage(R, T, filenames[i], filenames[i+1]);
		char* leftImageFile  = "C:\\temp\\epipolarLeftHeading.jpg";
		char* rightImageFile = "C:\\temp\\epipolarRightHeading.jpg";
		ImgFeature lEpipolarImageFeats;
		ImgFeature rEpipolarImageFeats;
		DetectFileFeaturePts(leftImageFile,  lEpipolarImageFeats, 640);
		DetectFileFeaturePts(rightImageFile, rEpipolarImageFeats, 640);
		//convert from spherical to real 3D
		ht = lEpipolarImageFeats.ht;
		wd = lEpipolarImageFeats.wd;

		//matching
		printf("Matching... \n");
		CMatchBase* pMatch1 = new CPanoMatch();
		vector<MatchPairIndex> mi1;
		pMatch1->Match(lEpipolarImageFeats, rEpipolarImageFeats, mi1);
		printf("Match Pair: %d \n", mi1.size());
		delete pMatch1;

		int nMatch = 0;
		double ferror = 0;
		printf("epipolar error.... \n");
		for(int i=0; i<mi1.size(); i++)
		{
			int nl = mi1[i].l;
			int nr = mi1[i].r;

			double lx,ly;
			lx = lEpipolarImageFeats.featPts[nl].col;
			ly = lEpipolarImageFeats.featPts[nl].row;
			double rx,ry;
			rx = rEpipolarImageFeats.featPts[nr].col;
			ry = rEpipolarImageFeats.featPts[nr].row;

			if( abs(lx-rx)>2 )
				continue;
			else
			{
				printf("%.4lf ", lx-rx);
				ferror += abs(lx-rx);
				nMatch ++;
			}
		}			
		printf("\n");

		double merror = ferror/(double)(nMatch);
		printf("\n epipolar error: %lf \n",  merror);
		FILE* fpEpipolarError = fopen("c:\\temp\\epipolarError.txt", "a+");
		fprintf(fpEpipolarError, "%lf \n", merror);
		fclose(fpEpipolarError);
		//##########################################################################
	}
	fclose(fp);


	return 0;
}


int main_generate_pmvsfiles(int argc, char* argv[])
{
	char* leftImageFile  = "C:\\Work\\Data\\panorama\\ladybug_jpg\\ladybug_panoramic_000000.jpg";
	char* rightImageFile = "C:\\Work\\Data\\panorama\\ladybug_jpg\\ladybug_panoramic_000001.jpg";


	//char* leftImageFile = "C:\\Work\\Data\\panorama\\2016.10.13-yizhuang\\L10_1013\\indoor\\jpg\\ladybug_panoramic_000006.jpg";
	//char* rightImageFile = "C:\\Work\\Data\\panorama\\2016.10.13-yizhuang\\L10_1013\\indoor\\jpg\\ladybug_panoramic_000007.jpg";


	IplImage* lImage = cvLoadImage(leftImageFile);
	IplImage* rImage = cvLoadImage(rightImageFile);
	printf("%s \n", leftImageFile);
	printf("%s \n", rightImageFile);
	
	ImgFeature lImageFeats;
	ImgFeature rImageFeats;

	DetectFileFeaturePts(leftImageFile, lImageFeats, 800);
	DetectFileFeaturePts(rightImageFile, rImageFeats, 800);

	//convert from spherical to real 3D
	int ht = lImageFeats.ht;
	int wd = lImageFeats.wd;

	//2. matching
	printf("Matching... \n");
	CMatchBase* pMatch = new CPanoMatch();
	vector<MatchPairIndex> mi;
	pMatch->Match(lImageFeats, rImageFeats, mi);
	printf("Match Pair: %d \n", mi.size());
	
	//3. pose estimation and compute residual error 
	double ratio = 0.6;
	vector<Point3DDouble> lptPano;     //3D spherical points
	vector<Point3DDouble> rptPano;
	vector<Point2DDouble> lptImagePts; //image points
	vector<Point2DDouble> rptImagePts;
	vector<int> matchIndex;
	double radius = (double)(wd) / (2*PI); 

	for(int i=0; i<mi.size(); i++)
	{
		int li = mi[i].l;
		int ri = mi[i].r;

		Point2DDouble pl;
		pl.x = lImageFeats.featPts[li].col;
		pl.y = lImageFeats.featPts[li].row;
		Point2DDouble pr;
		pr.x = rImageFeats.featPts[ri].col;
		pr.y = rImageFeats.featPts[ri].row;

		//remove the match without motion
		double dis = sqrt( (double)( (pl.x-pr.x)*(pl.x-pr.x) + (pl.y-pr.y)*(pl.y-pr.y)) );
		if(dis<5) continue;

		if( pl.y<ht*ratio && pr.y<ht*ratio )
		{
			Point2DDouble ip;
			ip.p[0] = pl.x;
			ip.p[1] = pl.y;
			lptImagePts.push_back(ip);
			ip.p[0] = pr.x;
			ip.p[1] = pr.y;
			rptImagePts.push_back(ip);

			Point3DDouble gp;					
			SphereTo3D(pl.x, pl.y, radius, gp.p[0], gp.p[1], gp.p[2]);
			lptPano.push_back(gp);
			SphereTo3D(pr.x, pr.y, radius, gp.p[0], gp.p[1], gp.p[2]);
			rptPano.push_back(gp);
		}
	}
	
	double R0[9];
	double T0[3];
	memset(R0, 0, sizeof(double)*9);
	memset(T0, 0, sizeof(double)*3);
	R0[0] = R0[4] = R0[8] = 1;

	double R[9];
	double T[3];
	vector<double> residual;
	residual.resize(lptPano.size());
	dll_EstimatePose5Point_Pano(lptPano, rptPano, radius, 512, 2.5, R, T, residual);


	GeneratePanoEpipolarImageHeading(R, T, leftImageFile, rightImageFile);
	//GeneratePanoEpipolarImage(R, T, leftImageFile, rightImageFile);


	//output the relative pose estimation result
	FILE* fp = fopen("c:\\temp\\pano_rt.txt", "w");		
	fprintf(fp, "%d %d \n", ht, wd);
	for(int j=0; j<3; j++)
	{
		for(int i=0; i<3; i++)
		{
			fprintf(fp, " %lf ", R[j*3+i]);
		}
		fprintf(fp, "\n");
	}
	fprintf(fp, " %lf %lf %lf \n", T[0], T[1], T[2]);
	fclose(fp);

	//double eT[3];
	//CalculateExplicitT(R, T, eT);

	double ea[3];
	rot2eular(R, ea);
	printf("angle: %lf %lf %lf \n", ea[0], ea[1], ea[2]);
	printf("translation: %lf %lf %lf \n", T[0], T[1], T[2]);

	/*
	if(0)
	{
		IplImage* pMosaic = VerticalMosaic1(lImage, rImage);

		for(int i=0; i<lptImagePts.size(); i++)
		{				
			//if(residual[i]>2.5)
			//	continue;

			int r,g,b;
			if(residual[i]>2.5)
			{
				//continue;
				r = 0;
				g = 0;
				b = 0;

			}
			else
			{
				r = (double)(rand())/(double)(RAND_MAX)*255;
				g = (double)(rand())/(double)(RAND_MAX)*255;
				b = (double)(rand())/(double)(RAND_MAX)*255;

				CvPoint pl;
				pl.x = lptImagePts[i].p[0];
				pl.y = lptImagePts[i].p[1];
				CvPoint pr;
				pr.x = rptImagePts[i].p[0];
				pr.y = rptImagePts[i].p[1];	

				//remove the match without motion
				double dis = sqrt( (double)( (pl.x-pr.x)*(pl.x-pr.x) + (pl.y-pr.y)*(pl.y-pr.y)) );
				if(dis<5) continue;

				cvDrawCircle(lImage, pl, 2, CV_RGB(r,g,b),2);
				cvDrawCircle(rImage, pr, 2, CV_RGB(r,g,b),2);
				cvDrawLine(pMosaic, pl, cvPoint( pr.x, pr.y+ht ), CV_RGB(255,0,0));
			}				
		}			
		//cvNamedWindow("Match1");
		//cvShowImage("Match1", lImage);
		//cvNamedWindow("Match2");
		//cvShowImage("Match2", rImage);

		cvSaveImage("c:\\temp\\left.jpg",  lImage);
		cvSaveImage("c:\\temp\\right.jpg", rImage);
		cvSaveImage("c:\\temp\\match.jpg", pMosaic);
		cvReleaseImage(&pMosaic);		
	}
	*/
	
	
	//5. 3D reconstruction and output
	double dis = 2;     //the real distance of 
	//double R0[9];
	//double T0[3];
	//memset(R0, 0, sizeof(double)*9);
	//memset(T0, 0, sizeof(double)*3);
	//R0[0] = R0[4] = R0[8] = 1;

	vector<Point3DDouble> vPts;
	vector<Point3DDouble> vColors;
	for(int i=0; i<lptPano.size(); i++)
	{
		Point3DDouble gp;
		if(residual[i]<8)
		{
			Point2DDouble lp,rp;
			//normalized coordinates
			lp.p[0] = lptPano[i].p[0] / lptPano[i].p[2];
			lp.p[1] = lptPano[i].p[1] / lptPano[i].p[2];
			rp.p[0] = rptPano[i].p[0] / rptPano[i].p[2];
			rp.p[1] = rptPano[i].p[1] / rptPano[i].p[2];
			double error = 0;
			Point3DDouble gp = dll_TriangulatePt(lp, rp, R0, T0, R, T, &error);
			gp.p[0] *= dis;
			gp.p[1] *= dis;
			gp.p[2] *= dis;

			if( fabs(gp.p[0])>50 || fabs(gp.p[1])>50 || fabs(gp.p[2])>50 )
				continue;

			vPts.push_back(gp);

			//get color
			Point3DDouble color;
			
			
			int x = lptImagePts[i].p[0];
			int y = lImage->height - lptImagePts[i].p[1];
			int scanWd = lImage->widthStep;
			int r = (unsigned char)(lImage->imageData[ y*scanWd + x*3 ]);
			int g = (unsigned char)(lImage->imageData[ y*scanWd + x*3 + 1 ]);
			int b = (unsigned char)(lImage->imageData[ y*scanWd + x*3 + 2 ]);

			color.p[0] = r;
			color.p[1] = g;
			color.p[2] = b;
			

			//color.p[0] = 255;
			//color.p[1] = 0;
			//color.p[2] = 0;
			
			vColors.push_back(color);
		}
	}

	CModelFileBase* pModel = new CPlyModel();
	pModel->Save("c:\\temp\\sphericalModel.ply", vPts, vColors);
	

	if(0)
	{			
	//6. generate projection images, save them and the corresponding projective matrix into the files
	vector<stCameraPara> camParas;
	bool bIsBundlerOut = false;
	double vangle=60,hangle=60;
	double anglestep = 60;
	double focalratio = 1;

	PanoToPlanes(0, leftImageFile, anglestep, vangle, hangle, focalratio,  R0, T0, camParas);
	PanoToPlanes(1, rightImageFile, anglestep, vangle, hangle, focalratio, R,  T,  camParas);
	
	//generate the bundler.out file 
	WriteBundlerOutFile("bundler.out", camParas);
	

	printf("generate pmvs_options.txt ... \n");
	int nImageNum = 2 * (360/anglestep);
	/* Write the options file */
	char buf[512];
	sprintf(buf, "pmvs_options.txt");
	FILE *f_opt = fopen(buf, "w");
	fprintf(f_opt, "level 1\n");
	fprintf(f_opt, "csize 2\n");
	fprintf(f_opt, "threshold 0.7\n");
	fprintf(f_opt, "wsize 7\n");
	fprintf(f_opt, "minImageNum 2\n");
	fprintf(f_opt, "CPU 4\n");
	fprintf(f_opt, "setEdge 0\n");
	fprintf(f_opt, "useBound 0\n");
	fprintf(f_opt, "useVisData 0\n");
	fprintf(f_opt, "sequence -1\n");
	fprintf(f_opt, "timages -1 0 %d\n", nImageNum);
	fprintf(f_opt, "oimages -3\n");
	fclose(f_opt);
	}

	printf("Finished! \n");
	
	return 0;
}






int DrawCross(CvPoint p, int len, IplImage* pImage)
{
	cvLine( pImage, cvPoint( p.x-len,p.y), cvPoint( p.x+len,p.y),CV_RGB(255,0,0),3,8,0);
	cvLine( pImage, cvPoint( p.x,p.y-len), cvPoint( p.x,p.y+len),CV_RGB(255,0,0),3,8,0);

	return 0;
}

IplImage* pLeft  = NULL;
IplImage* pRight = NULL;
CIRegBase* pIReg = NULL;
Point2DDouble srcPt,dstPt;     //top-left
Point2DDouble leftPt, rightPt; // center
CameraPara leftCam,rightCam;
Point3DDouble grdpt;

int CalculateGrd()
{
	CTriangulateBase* pTri = new CTriangulateCV();
	vector<CameraPara> cams;
	vector<Point2DDouble> pts;
	cams.push_back(leftCam);
	cams.push_back(rightCam);
	pts.push_back(leftPt);
	pts.push_back(rightPt);
	double ferror;
	pTri->Triangulate(pts, cams, grdpt, true, ferror);
	delete pTri;

	//printf("Left: %lf %lf \n", leftPt.p[0], leftPt.p[1]);
	//printf("right: %lf %lf \n", rightPt.p[0], rightPt.p[1]);
	printf("ground pt: %lf %lf %lf    error: %lf \n", grdpt.p[0], grdpt.p[1], grdpt.p[2], ferror);
	//printf("triangulate error: %lf \n", ferror);

	return 0;
}

void on_left_mouse( int event, int x, int y, int flags, void* param)
{
	if( event == CV_EVENT_LBUTTONDOWN )
	{
		CvPoint cp;
		cp.x = x;
		cp.y = y;

		IplImage* pLeftDisp = cvCloneImage(pLeft);

		leftPt.p[0] = x - pLeft->width*0.5;
		leftPt.p[1] = pLeft->height*0.5 - y;

		//cvDrawCircle(pLeftDisp, cp, 5, CV_RGB(255,0,0));
		DrawCross(cp, 21, pLeftDisp);
		cvShowImage("Left", pLeftDisp);
		//cvUpdateWindow("Left");
		//cvSaveImage("c:\\temp\\cross.jpg", pLeftDisp);
		cvReleaseImage(&pLeftDisp);
		
		srcPt.p[0] = x;
		srcPt.p[1] = y;

		if(pIReg!=NULL)
			pIReg->PtReg(srcPt, dstPt, 0);

		cp.x = dstPt.p[0];
		cp.y = dstPt.p[1];
		IplImage* pRightDisp = cvCloneImage(pRight);
		DrawCross(cp, 21, pRightDisp);
		cvShowImage("Right", pRightDisp);
		cvReleaseImage(&pRightDisp);

		rightPt.p[0] = dstPt.p[0] - pRight->width*0.5;
		rightPt.p[1] = pRight->height*0.5 - dstPt.p[1];
		
		CalculateGrd();
	}
}

void on_right_mouse( int event, int x, int y, int flags, void* param)
{
	if( event == CV_EVENT_LBUTTONDOWN )
	{
		CvPoint cp;
		cp.x = x;
		cp.y = y;

		IplImage* pRightDisp = cvCloneImage(pRight);
		DrawCross(cp, 21, pRightDisp);
		cvShowImage("Right", pRightDisp);
		//cvUpdateWindow("Left");
		//cvSaveImage("c:\\temp\\cross.jpg", pLeftDisp);
		cvReleaseImage(&pRightDisp);

		rightPt.p[0] = x - pRight->width*0.5;
		rightPt.p[1] = pRight->height*0.5 - y;
		
		CalculateGrd();
	}
}


int main_pano_match_usingpos()
{

	//################  loading pos data ###########################
	//char* camfile = "C:\\Work\\Data\\panorama\\20161202-linlonglu\\ladybug\\161130_025822Trig_zt.cam";
	char* camfile ="C:\\Work\\Data\\panorama\\Pano_Cam_1108\\1104_170_new.cam";
	CReadPosBase* pReadPos = new CReadCarPos();
	pReadPos->ReadPOSData(camfile);

	
	//###############   loading image filenames ##############################
	//char* imagepath="C:\\Work\\Data\\panorama\\20161202-linlonglu\\ladybug";
	char* imagepath="C:\\Work\\Data\\panorama\\Pano_Cam_1108";
	char** filenames = NULL;
	int n=0,nfile=0;
	GetDirFileName(filenames, imagepath, &n, &nfile, "jpg", 0);
	filenames = f2c(nfile, 512);
	GetDirFileName(filenames, imagepath, &n, &nfile, "jpg", 1);
	//from char to string
	vector<string> files;
	for(int i=0; i<nfile; i++)
	{
		string file = filenames[i];
		//printf("%s \n", filenames[i]);
		//cout<<file<<endl;
		files.push_back(file);
	}

	//######################  image show ###############################
	int windowWd = 630;
		
	pLeft  = cvLoadImage(files[0].c_str());
	pRight = cvLoadImage(files[1].c_str());
		
	//################  resize the images ##############
	int dstWd = 2048;
	int dstHt = 1024;
	IplImage* pResizeImg = cvCreateImage(cvSize(dstWd, dstHt), 8, pLeft->nChannels);
	cvResize(pLeft, pResizeImg);
	cvReleaseImage(&pLeft);
	pLeft = cvCloneImage(pResizeImg);
	
	cvResize(pRight, pResizeImg);
	cvReleaseImage(&pRight);
	pRight = cvCloneImage(pResizeImg);
    cvReleaseImage(&pResizeImg);
	//###################################################

	int imageHt = pLeft->height;
	int imageWd = pLeft->width;

	POSInfo leftPos,rightPos;
	pReadPos->GetPOS(0, leftPos);
	pReadPos->GetPOS(1, rightPos);

	leftCam.rows  = imageHt;	    leftCam.cols  = imageWd;
	rightCam.rows = imageHt;	    rightCam.cols = imageWd;
	leftCam.camtype = PanoramCam;	rightCam.camtype = PanoramCam;
	InitCamera(leftCam, leftPos);
	InitCamera(rightCam, rightPos);
	

	pIReg = new CIPanoRegDirect();
	pIReg->Init(pLeft, pRight, leftCam, rightCam);
	//pIReg->Init(leftImage, rightImage);
	
	//for display
	double ratio = (double)(windowWd) / (double)(imageWd);
	int windowHt = ratio*imageHt;
	ratio = (double)(windowWd) / (double)(imageWd);
	windowHt = ratio*imageHt;

	cvNamedWindow("Left", 0);
	cvMoveWindow("Left", 0, 0);
	cvResizeWindow("Left", windowWd, windowHt);
	cvShowImage("Left", pLeft);

	cvNamedWindow("Right", 0);
	cvMoveWindow("Right", windowWd+5, 0);
	cvResizeWindow("Right", windowWd, windowHt);
	cvShowImage("Right", pRight);

	//small window
	//cvNamedWindow("LeftSub",0);
	//cvMoveWindow("LeftSub", 0, windowHt+32);
	//cvResizeWindow("LeftSub", 64, 64);

	cvSetMouseCallback("Left",  on_left_mouse, NULL );
	cvSetMouseCallback("Right", on_right_mouse, NULL );
	
	int nCurrent = 0;
	while(true)
	{  
		int c = cv::waitKey(0);  
		//printf("%d \n", c);
		switch(c)
		{
		case 'a':
			//printf("press A \n");
			nCurrent --;
			nCurrent = max(0, nCurrent);
			//printf("%d \n", nCurrent);		

			break;
		case 'd':
			//printf("press D \n");
			nCurrent ++;
			nCurrent = min(files.size()-2, nCurrent);
			//printf("%d \n", nCurrent);		

			break;
		}

		cvReleaseImage(&pLeft);
		cvReleaseImage(&pRight);
		pLeft  = cvLoadImage(files[nCurrent].c_str());
		pRight = cvLoadImage(files[nCurrent+1].c_str());

		//################### resize ##########################################
		int dstWd = 2048;
		int dstHt = 1024;
		IplImage* pResizeImg = cvCreateImage(cvSize(dstWd, dstHt), 8, pLeft->nChannels);
		cvResize(pLeft, pResizeImg);
		cvReleaseImage(&pLeft);
		pLeft = cvCloneImage(pResizeImg);
		cvResize(pRight, pResizeImg);
		cvReleaseImage(&pRight);
		pRight = cvCloneImage(pResizeImg);
		cvReleaseImage(&pResizeImg);
		//######################################################################
		
		cvShowImage("Left", pLeft);
		cvShowImage("Right", pRight);

		POSInfo leftPos,rightPos;
		pReadPos->GetPOS(nCurrent, leftPos);
		pReadPos->GetPOS(nCurrent+1, rightPos);
		leftCam.rows  = imageHt;	    leftCam.cols  = imageWd;
		rightCam.rows = imageHt;	    rightCam.cols = imageWd;
		leftCam.camtype = PanoramCam;	rightCam.camtype = PanoramCam;
		CameraPara leftCam,rightCam;
		InitCamera(leftCam, leftPos);
		InitCamera(rightCam, rightPos);
		pIReg->Init(pLeft, pRight, leftCam, rightCam);
	}
	
	return 0;
}


//interactive panorama matching
int main_pano_match(int argc, char* argv[])
{	
	char* leftImage  = "C:\\Work\\Data\\panorama\\ladybug_jpg\\ladybug_panoramic_000001.jpg";
	char* rightImage = "C:\\Work\\Data\\panorama\\ladybug_jpg\\ladybug_panoramic_000002.jpg";

	int windowWd = 630;
	
	pLeft  = cvLoadImage(leftImage);
	pRight = cvLoadImage(rightImage);
	int imageHt = pLeft->height;
	int imageWd = pLeft->width;
	
	//pIReg = new CIPanoRegTri();
	pIReg = new CIPanoRegDirect();
	pIReg->Init(leftImage, rightImage);
	

	//for display
	double ratio = (double)(windowWd) / (double)(imageWd);
	int windowHt = ratio*imageHt;
	ratio = (double)(windowWd) / (double)(imageWd);
	windowHt = ratio*imageHt;

	cvNamedWindow("Left", 0);
	cvMoveWindow("Left", 0, 0);
	cvResizeWindow("Left", windowWd, windowHt);
	cvShowImage("Left", pLeft);

	cvNamedWindow("Right", 0);
	cvMoveWindow("Right", windowWd+5, 0);
	cvResizeWindow("Right", windowWd, windowHt);
	cvShowImage("Right", pRight);

	//small window
	//cvNamedWindow("LeftSub",0);
	//cvMoveWindow("LeftSub", 0, windowHt+32);
	//cvResizeWindow("LeftSub", 64, 64);
	
	cvSetMouseCallback("Left", on_left_mouse, NULL );
	cvSetMouseCallback("Right", on_right_mouse, NULL );

	cvWaitKey();
	
	return 0;
}

int _tmain(int argc, char* argv[])
{
	printf("BA test.... \n");
	

	//main_generate_pmvsfiles(argc, argv);
	
	
	//main_pano_rp(argc, argv);
	
	//main_realimages_bundler(argc, argv);
	
	//########  bundle adjustment for real images ####
	//main_realimages(argc, argv);
	
	//main_simulate_multviews(argc, argv);


	//######## interactive panorama matching  ###################
	//main_pano_match(argc, argv);


	//######## interactive panorama matching based on pos #############
	main_pano_match_usingpos();



	return 0;
}

