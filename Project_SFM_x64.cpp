// Project_SFM_x64.cpp : 定义控制台应用程序的入口点。
//

#include "stdio.h"

#include "register.hpp"



#include <vector>
using namespace std;

int main(int argc, char** argv[])
{
	printf("SFM integration .... \n");

	//1. generating the image feature points
	vector<ImgFeature> imgFeatures;


	//2. matching images 
	vector<PairMatchRes> matchRes; 


	
	//3.generating the tracks
	//test
	imgFeatures.resize(4);
	int nFeat = 1000;
	imgFeatures[0].featPts.resize(nFeat);
	imgFeatures[1].featPts.resize(nFeat);
	imgFeatures[2].featPts.resize(nFeat);
	imgFeatures[3].featPts.resize(nFeat);

	//char* matchfile = "c:\\temp\\match_file.txt";
	char* matchfile = "c:\\sfm\\match_init.txt";
	LoadMatchFile(matchfile, matchRes);
	

	vector<TrackInfo> tracks; 
	CGenerateTracksBase* pGenerateTrack = new CFastGenerateTrack();
	pGenerateTrack->GenerateTracks(imgFeatures, matchRes, tracks);
	
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

	return 0;
}

