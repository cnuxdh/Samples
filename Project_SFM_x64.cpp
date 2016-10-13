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
	/* test
	imgFeatures.resize(3);
	imgFeatures[0].featPts.resize(6);
	imgFeatures[1].featPts.resize(6);
	imgFeatures[2].featPts.resize(6);
	char* matchfile = "c:\\temp\\match_file.txt";
	LoadMatchFile(matchfile, matchRes);
	*/

	vector<TrackInfo> tracks; 
	CGenerateTracksBase* pGenerateTrack = new CFastGenerateTrack();
	pGenerateTrack->GenerateTracks(imgFeatures, matchRes, tracks);
	
	for(int i=0; i<tracks.size(); i++)
	{
		printf("track: %d \n", i);
		for(int j=0; j<tracks[i].views.size(); j++)
		{
			printf(" %d-%d  ", tracks[i].views[j].first, tracks[i].views[j].second );
		}
		printf("\n");
	}


	return 0;
}

