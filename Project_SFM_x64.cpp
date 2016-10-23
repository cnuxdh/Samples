// Project_SFM_x64.cpp : 定义控制台应用程序的入口点。
//

#include "stdio.h"

//cvlib
#include "register.hpp"
#include "cvInvoke.hpp"
#include "ba.hpp"


//corelib
#include "commonfile.h"
#include "file.h"




#include <vector>
using namespace std;

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

