/* 
 *  Copyright (c) 2008-2010  Noah Snavely (snavely (at) cs.cornell.edu)
 *    and the University of Washington
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 */

/* KeyMatchFull.cpp */
/* Read in keys, match, write results to a file */

#include <assert.h>
#include <time.h>
#include <string.h>

#include "keys2a.h"

//corelib
#include "commonfile.h"
//#include "commondata.h"

//opensift dll
#include "imgfeatures.h"
#include "kdtree.h"
#include "match.h"


#include "tinythread.h"
using namespace tthread;

#include "iostream"
#include "fstream"
#include "string"
#include <list>
using namespace std;



mutex gMutex;
condition_variable gCV;
int running_count;

struct param_run_match
{
	int imageIndex;
	int resizeDim;
	double ratio;
	std::vector<std::string> key_files;
};


struct param_run_sift
{
	int left,right; //index of image pair
	int num_keys;
	unsigned char *keys;
	//ANNkd_tree *tree;
	struct kd_node* kd_tree;
	int resizeDim;
	double ratio; 
	int threadID;
	int i;
};

typedef struct stiPoint
{
	int x,y;
}iPoint;

typedef struct MatchStruct
{
	int left,right;
	vector<iPoint> mp;
}stMatchStruct;


//for each image pari
vector<stMatchStruct> vecMatchPair;


//for images
vector< vector<stMatchStruct> > vecMatchImage;


//
////kd-tree is created in the thread
//void RunMatchV2(void *aArg)
//{
//	param_run_match *param = (param_run_match*)aArg;
//    printf("imageid: %d \n", param->imageIndex);
//
//	vector<string> key_files = param->key_files;
//
//	//loading keys
//	int num_images = param->imageIndex+1;
//	unsigned char** keys = new unsigned char *[num_images];
//	int* num_keys = new int[num_images];
//	// Read all keys
//	int resizeDim = param->resizeDim; 
//	for (int i = 0; i < num_images; i++) 
//	{   
//		//lock_guard<mutex> guard(gMutex);
//		keys[i] = NULL;
//		num_keys[i] = ReadKeysBinResize(key_files[i].c_str(), keys+i, resizeDim);
//	}
//
//	//Create a tree from the keys
//	int imageIndex = param->imageIndex;
//	ANNkd_tree *tree = CreateSearchTree(num_keys[imageIndex], keys[imageIndex], resizeDim);
//	
//	for(int j=0; j<imageIndex; j++)
//	{
//		printf("Matching... \n");
//		std::vector<KeypointMatch> matches = 
//			 MatchKeys(num_keys[j], keys[j], tree, param->resizeDim, param->ratio);
//		
//		int num_matches = (int)matches.size();
//		printf("match number: %d \n", num_matches);
//	}
//	delete tree;
//	
//	//Free keypoints
//	for(int i = 0; i < num_images; i++) 
//	{
//		if (keys[i] != NULL)
//			delete[] keys[i];
//	}
//	delete[] keys;
//	delete[] num_keys;
//
//
//	lock_guard<mutex> guard(gMutex);
//	running_count--;
//	gCV.notify_all();
//}
//
//
//void RunMatch(void *aArg)
//{
//	param_run_sift *param = (param_run_sift*)aArg;
//	
//	//printf("%d %d \n", running, running_count);
//	printf("%d \n", param->threadID);
//	//start = clock();
//				
//	/* Compute likely matches between two sets of keypoints */
//	//printf("%d %d %lf %d \n", param->num_keys, param->resizeDim, param->ratio, param->i);
//	
//	//if( param->keys == NULL || param->tree==NULL )
//	//	printf("error ... \n");
//		
//	std::vector<KeypointMatch> matches = 
//		MatchKeys(param->num_keys, 
//				  param->keys, 
//				  param->tree, 
//				  param->resizeDim, 
//				  param->ratio);
//		
//	int num_matches = (int) matches.size();
//	printf("match number: %d \n", num_matches);
//		
//
//	/*
//	//printf("output matching result... \n");
//	if (num_matches >= 16) 
//	{
//		for (int i = 0; i < num_matches; i++) 
//		{
//			iPoint pair;
//			pair.x = matches[i].m_idx1;
//			pair.y = matches[i].m_idx2;
//			vecMatchPair[ param->i ].mp.push_back(pair);
//		}
//	}*/
//	
//	/*
//	running_count--;		
//	lock_guard<mutex> guard(mtx[running]);
//	cv[running].notify_one();	
//	*/
//	//cv[param->threadID].notify_all();		
//		
//	lock_guard<mutex> guard(gMutex);
//	running_count--;
//	gCV.notify_all();
//}
//  
////multiple thread based on ann
//int main_ann(int argc, char **argv) 
//{
//    char *list_in;
//    char *file_out;
//    double ratio; 
//     
//    if (argc != 4) {
//	printf("Usage: %s <list.txt> <match_init.txt> <ratio>\n", argv[0]);
//	return -1;
//    }
//    
//    list_in  = argv[1];
//    file_out = argv[2];    
//	ratio    = atof(argv[3]) ; //0.6;
//
//    unsigned char **keys;
//    int *num_keys;
//
//    /* Read the list of files */
//    std::vector<std::string> key_files;
//    
//    FILE *f = fopen(list_in, "r");
//    if (f == NULL) {
//        printf("Error opening file %s for reading\n", list_in);
//        return 1;
//    }
//
//    char buf[512];
//    while (fgets(buf, 512, f)) 
//	{
//        /* Remove trailing newline */
//        if (buf[strlen(buf) - 1] == '\n')
//            buf[strlen(buf) - 1] = 0;
//		  
//		char   imagefile[256];
//		int    nfisheye;
//		double focus; 
//		sscanf(buf, "%s %d %lf", imagefile, &nfisheye, &focus);
//		
//		char*  keyfile = (char*)malloc(256);
//		GenerateProductFile(imagefile, "product", "key", &keyfile);        
//		printf("keyfile: %s \n", keyfile);
//		key_files.push_back(std::string(keyfile));
//		free(keyfile);
//    }
//    fclose(f);
//	
//
//	/*
//	int num_images = (int) key_files.size();
//	SYSTEM_INFO si;
//	GetSystemInfo(&si);
//	int cpu_count = si.dwNumberOfProcessors;	
//	printf("cpu: %d \n", cpu_count);
//	//cpu_count = 2;
//
//	param_run_match *param = new param_run_match[num_images];
//	list<thread*> threadList;
//
//	clock_t start = clock();
//	for(int i=0; i<num_images; i++)
//	{
//		param[i].imageIndex = i;
//		param[i].resizeDim = 32;
//		param[i].ratio = 0.6;
//		param[i].key_files = key_files;
//		
//		lock_guard<mutex> guard(gMutex); 
//		while (running_count == cpu_count)
//			gCV.wait(gMutex);
//		running_count++;	
//
//		threadList.push_back(new thread(RunMatchV2, &param[i]));  
//	}
//
//	//wait the threads
//	list<thread*>::iterator it;
//	for (it = threadList.begin();it != threadList.end();it++)
//	{
//		thread *t = *it;
//		t->join();
//		delete t;
//	}
//
//	clock_t end = clock();    
//	printf("[KeyMatchFull] Matching took %0.3fs\n", 
//		(end - start) / ((double) CLOCKS_PER_SEC));
//
//	*/
//
//	
//    int num_images = (int) key_files.size();
//    keys = new unsigned char *[num_images];
//    num_keys = new int[num_images];
//    // Read all keys
//    int resizeDim = 32; //changed by xiedonghai, 2015.10.16
//	for (int i = 0; i < num_images; i++) 
//	{
//        keys[i] = NULL;
//        num_keys[i] = ReadKeysBinResize(key_files[i].c_str(), keys+i, resizeDim);
//    }
//
//    double dstep = 95.0 / double(num_images);
//	
//	//init multi-thread
//	SYSTEM_INFO si;
//	GetSystemInfo(&si);
//	int cpu_count = si.dwNumberOfProcessors;	
//	cpu_count = 1;
//	printf("cpu: %d \n", cpu_count);
//		
//	//mtx_file = new mutex[num_images];
//	//cv_file = new condition_variable[num_images];
//	//w_file = 0;	
//	
//	int ti=0;	
//	param_run_sift *param = new param_run_sift[num_images];
//	list<thread*> threadList;
//	
//	int nMatchPair = num_images*(num_images-1)*0.5;
//	vecMatchPair.resize(nMatchPair);
//	//init multi-thread end
//	//fill the param structures
//	//clock_t start = clock();
//	for (int i = 0; i < num_images; i++) 
//	{
//		//Multi-Thread
//		param[i].left  = 0;
//		param[i].right = 0;
//		param[i].num_keys = num_keys[i];
//		param[i].keys = keys[i];
//		param[i].tree = NULL;
//		param[i].resizeDim = resizeDim;
//		param[i].ratio = ratio;
//	}
//	
//	//clock_t start = clock();
//
//	int index = 0;
//    //for (int i = 0; i < num_images; i++) 
//	int i=8;
//	{
//		WriteProgressValueToFile(dstep);
//
//        //if (num_keys[i] == 0)
//        //    continue;
//
//        printf("[KeyMatchFull] Matching to image %d\n", i);
//		
//		clock_t start = clock();
//
//		//Create a tree from the keys
//		//ANNkd_tree *tree = CreateSearchTree(num_keys[i], keys[i], resizeDim);
//		      		
//		//lock_guard<mutex> guard(gMutex); 
//        vector< ANNkd_tree*>  vecTrees;
//		for (int j = 0; j < i; j++) 
//		{
//			ANNkd_tree *tree = CreateSearchTree(num_keys[i], keys[i], resizeDim);
//			vecTrees.push_back(tree);
//			param[j].tree = tree;
//		}		
//		
//		running_count = 0;
//		threadList.clear();
//
//		/*
//		param[0].left  = 0;
//		param[0].right = 1;
//		param[0].tree = vecTrees[1];
//		param[0].threadID = 0;
//		vecMatchPair[0].left  = 0;
//		vecMatchPair[0].right = 1;
//		param[2].left  = 2;
//		param[2].right = 3;
//		param[2].tree = vecTrees[3];
//		param[2].threadID = 1;
//		vecMatchPair[1].left  = 2;
//		vecMatchPair[1].right = 3;		
//		threadList.push_back(new thread(RunMatch, &param[0]));  
//		threadList.push_back(new thread(RunMatch, &param[2]));  
//		*/
//
//		
//        for (int j = 0; j < i; j++) 
//		{
//			//printf("%d \n", j);
//
//			param[j].left  = i;
//			param[j].right = j;
//			//param[j].tree = tree;
//			param[j].i = index;
//			param[j].threadID = j;
//
//			vecMatchPair[index].left  = i;
//			vecMatchPair[index].right = j;
//			
//			lock_guard<mutex> guard(gMutex); 
//			while (running_count == cpu_count)
//				gCV.wait(gMutex);
//			running_count++;	
//			
//			threadList.push_back(new thread(RunMatch, &param[j]));  
//
//			index++;
//        }
//		
//		//wait the threads
//		list<thread*>::iterator it;
//		for (it = threadList.begin();it != threadList.end();it++)
//		{
//			thread *t = *it;
//			t->join();
//			delete t;
//		}
//		
//		//delete tree;
//		for(int k=0; k<vecTrees.size(); k++)
//			delete vecTrees[k];
//		
//		clock_t end = clock();    
//		printf("[KeyMatchFull] Matching took %0.3fs\n", 
//			(end - start) / ((double) CLOCKS_PER_SEC));
//    }
//	    
//    //Free keypoints
//    for (int i = 0; i < num_images; i++) 
//	{
//        if (keys[i] != NULL)
//            delete [] keys[i];
//    }
//    delete [] keys;
//    delete [] num_keys;
//	    
//	f = fopen(file_out, "w");
//	assert(f != NULL);
//	if(f==NULL)
//		return -1;	
//	for( int i=0; i<vecMatchPair.size(); i++)
//	{
//		fprintf(f, "%d %d\n", vecMatchPair[i].left, vecMatchPair[i].right);
//		int nMatchNum = vecMatchPair[i].mp.size();
//		fprintf(f, "%d\n", nMatchNum);
//		
//		for(int j=0; j<nMatchNum; j++)
//		{
//			fprintf(f, "%d %d\n", vecMatchPair[i].mp[j].x, vecMatchPair[i].mp[j].y);
//		}
//	}	
//    fclose(f);
//	
//	//WriteProgressValueToFile(5.0);
//	//WriteProgressValueToFile(-100.0);   
//    
//
//    return 0;
//}
//



void RunMatchOpenSift(void *aArg)
{
	param_run_sift *param = (param_run_sift*)aArg;
	
	int* index1 = NULL;
	int* index2 = NULL;
	int   matchNum = 0;

	OpenSiftMatch( param->resizeDim, 
		param->kd_tree, 
		param->keys, 
		param->num_keys, 
		param->ratio,
		&index1, &index2, &matchNum );
	
	printf("match number: %d \n", matchNum);
		
	//printf("output matching result... \n");
	if (matchNum >= 16) 
	{
		for (int i = 0; i < matchNum; i++) 
		{
			iPoint pair;
			pair.x = index1[i];
			pair.y = index2[i];
			vecMatchPair[ param->i ].mp.push_back(pair);
		}
	}

	free(index1);
	free(index2);

	lock_guard<mutex> guard(gMutex);
	running_count--;
	gCV.notify_all();
}
  

//multiple thread based on opensift
int main_mt(int argc, char **argv) 
{
    char *list_in;
    char *file_out;
    double ratio; 
     
    if (argc != 4) {
	printf("Usage: %s <list.txt> <match_init.txt> <ratio>\n", argv[0]);
	return -1;
    }
    
    list_in  = argv[1];
    file_out = argv[2];    
	ratio    = atof(argv[3]) ; //0.6;

    unsigned char **keys;
    int *num_keys;

    /* Read the list of files */
    std::vector<std::string> key_files;
    
    FILE *f = fopen(list_in, "r");
    if (f == NULL) {
        printf("Error opening file %s for reading\n", list_in);
        return 1;
    }

    char buf[512];
    while (fgets(buf, 512, f)) 
	{
        /* Remove trailing newline */
        if (buf[strlen(buf) - 1] == '\n')
            buf[strlen(buf) - 1] = 0;
		  
		char   imagefile[256];
		int    nfisheye;
		double focus; 
		sscanf(buf, "%s %d %lf", imagefile, &nfisheye, &focus);
		
		char*  keyfile = (char*)malloc(256);
		GenerateProductFile(imagefile, "product", "key", &keyfile);        
		printf("keyfile: %s \n", keyfile);
		key_files.push_back(std::string(keyfile));
		free(keyfile);
    }
    fclose(f);
	
		
    int num_images = (int) key_files.size();
    keys = new unsigned char *[num_images];
    num_keys = new int[num_images];
    // Read all keys
    int resizeDim = 32; //changed by xiedonghai, 2015.10.16
	for (int i = 0; i < num_images; i++) 
	{
        keys[i] = NULL;
        num_keys[i] = ReadKeysBinResize(key_files[i].c_str(), keys+i, resizeDim);
    }

    double dstep = 95.0 / double(num_images);
	
	//init multi-thread
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	int cpu_count = si.dwNumberOfProcessors;	
	//cpu_count = 1;
	printf("cpu: %d \n", cpu_count);
		 
		
	int ti=0;	
	param_run_sift *param = new param_run_sift[num_images];
	list<thread*> threadList;
	
	int nMatchPair = num_images*(num_images-1)*0.5;
	vecMatchPair.resize(nMatchPair);	
	
	//fill the param structures
	for (int i = 0; i < num_images; i++) 
	{
		//Multi-Thread
		param[i].left  = 0;
		param[i].right = 0;
		param[i].num_keys = num_keys[i];
		param[i].keys = keys[i];
		param[i].kd_tree = NULL;
		param[i].resizeDim = resizeDim;
		param[i].ratio = ratio;
	}
	
	double step = 100.0 / num_images;

	int index = 0;
    for (int i = 0; i < num_images; i++) 
	{
		WriteProgressValueToFile(dstep);

		//converted to OpenSift format
		struct feature* feats = (struct feature*)malloc( num_keys[i]*sizeof(struct feature) );
		for(int ki=0; ki<num_keys[i]; ki++)
		{
			feats[ki].index = ki;
			feats[ki].d = resizeDim;
			for(int kj=0; kj<resizeDim; kj++)
			{
				feats[ki].descr[kj] = keys[i][ki*resizeDim + kj];
			}
		}

		//construct the kd-tree
        printf("[KeyMatchFull] Matching to image %d\n", i);
        vector< struct kd_node*>  vecTrees;
		for (int j = 0; j < i; j++) 
		{
			//printf("%d  %d \n", j, num_keys[i]);
			struct kd_node* kd_tree = kdtree_build(feats, num_keys[i]);
			vecTrees.push_back(kd_tree);
			param[j].kd_tree = kd_tree;
		}		
		

		printf("multithread processing ... \n");
		clock_t start = clock();
		running_count = 0;
		threadList.clear();				
        for (int j = 0; j < i; j++) 
		{
			param[j].left  = i;    // bench image
			param[j].right = j;    // other images
			param[j].threadID = j;
			param[j].i = index;

			vecMatchPair[index].left  = i;
			vecMatchPair[index].right = j;
			
			lock_guard<mutex> guard(gMutex); 
			while (running_count == cpu_count)
				gCV.wait(gMutex);
			running_count++;	
			
			threadList.push_back(new thread(RunMatchOpenSift, &param[j]));  

			index++;						
        }
		
		//wait the threads
		list<thread*>::iterator it;
		for (it = threadList.begin();it != threadList.end();it++)
		{
			thread *t = *it;
			t->join();
			delete t;
		}
		
		//free memory 		
		for(int k=0; k<vecTrees.size(); k++)
		{
			free( vecTrees[k]->features );
			kdtree_release(vecTrees[k]);
		}
		free(feats);
		
		clock_t end = clock();    
		printf("[KeyMatchFull] Matching took %0.3fs\n", 
			(end - start) / ((double) CLOCKS_PER_SEC));
    }	    
    
	printf("Free memory .... \n");
	//Free keypoints
    for (int i = 0; i < num_images; i++) 
	{
        if (keys[i] != NULL)
            delete [] keys[i];
		
    }
	delete[] param; // added by xiedonghai, 2016.9.13
    delete[] keys;
    delete[] num_keys;
	//////////////////////////////////////////////////////////////////////////
	
	    
	f = fopen(file_out, "w");
	assert(f != NULL);
	if(f==NULL)
		return -1;	
	for( int i=0; i<vecMatchPair.size(); i++)
	{
		//fprintf(f, "%d %d\n", vecMatchPair[i].left, vecMatchPair[i].right);
		fprintf(f, "%d %d\n", vecMatchPair[i].right, vecMatchPair[i].left);
		
		int nMatchNum = vecMatchPair[i].mp.size();
		fprintf(f, "%d\n", nMatchNum);
		
		for(int j=0; j<nMatchNum; j++)
		{
			//fprintf(f, "%d %d\n", vecMatchPair[i].mp[j].x, vecMatchPair[i].mp[j].y);
			fprintf(f, "%d %d\n", vecMatchPair[i].mp[j].y, vecMatchPair[i].mp[j].x);
		}
	}	
    fclose(f);
	
	//WriteProgressValueToFile(5.0);
	//WriteProgressValueToFile(-100.0);   

    return 0;
}



//to save the memory, revised by xiedonghai, 2016.9.14
int main(int argc, char **argv) 
{
	char *list_in;
	char *file_out;
	double ratio; 

	if (argc != 4) {
		printf("Usage: %s <list.txt> <match_init.txt> <ratio>\n", argv[0]);
		return -1;
	}

	list_in  = argv[1];
	file_out = argv[2];    
	ratio    = atof(argv[3]) ; //0.6;

	unsigned char **keys;
	int *num_keys;

	/* Read the list of files */
	std::vector<std::string> key_files;

	FILE *f = fopen(list_in, "r");
	if (f == NULL) {
		printf("Error opening file %s for reading\n", list_in);
		return 1;
	}

	char buf[512];
	while (fgets(buf, 512, f)) 
	{
		/* Remove trailing newline */
		if (buf[strlen(buf) - 1] == '\n')
			buf[strlen(buf) - 1] = 0;

		char   imagefile[256];
		int    nfisheye;
		double focus; 
		sscanf(buf, "%s %d %lf", imagefile, &nfisheye, &focus);

		char*  keyfile = (char*)malloc(256);
		GenerateProductFile(imagefile, "product", "key", &keyfile);        
		printf("keyfile: %s \n", keyfile);
		key_files.push_back(std::string(keyfile));
		free(keyfile);
	}
	fclose(f);


	// Loading all keys
	int num_images = (int) key_files.size();
	keys = new unsigned char *[num_images];
	num_keys = new int[num_images];
	int resizeDim = 32; //changed by xiedonghai, 2015.10.16
	for (int i = 0; i < num_images; i++) 
	{
		keys[i] = NULL;
		num_keys[i] = ReadKeysBinResize(key_files[i].c_str(), keys+i, resizeDim);
	}

	double dstep = 95.0 / double(num_images);


	//init multi-thread
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	int cpu_count = si.dwNumberOfProcessors;	
	printf("cpu: %d \n", cpu_count);

	//set the image number to process in one iteration, to save the memory
	int nImageStep = cpu_count*2;


	int ti=0;	
	param_run_sift *param = new param_run_sift[nImageStep];
	list<thread*> threadList;

	//save all match results
	int nMatchPair = num_images*(num_images-1)*0.5;
	vecMatchPair.resize(nMatchPair);	

	//fill the param structures
	for (int i = 0; i < nImageStep; i++) 
	{
		param[i].num_keys = 0; 
		param[i].keys = NULL;
		param[i].kd_tree = NULL;
		param[i].resizeDim = resizeDim;
		param[i].ratio = ratio;
	}

	double step = 100.0 / num_images;

	int index = 0; //index of match pair
	for (int i = 0; i < num_images; i++) 
	{
		WriteProgressValueToFile(dstep);

		//0.convert the current image feature to OpenSift format
		printf("convert the feature point .... \n");
		struct feature* feats = (struct feature*)malloc( num_keys[i]*sizeof(struct feature) );
		for(int ki=0; ki<num_keys[i]; ki++)
		{
			feats[ki].index = ki;
			feats[ki].d = resizeDim;
			for(int kj=0; kj<resizeDim; kj++)
			{
				feats[ki].descr[kj] = keys[i][ki*resizeDim + kj];
			}
		}

		clock_t start = clock();    
		//match the points in other images
		//1. block by block
		//printf("block by block ... \n");
		int nBlock = i / nImageStep;
		for(int bi=0; bi<nBlock; bi++)
		{
			//alloc memory for threads
			int nI = 0;
			for(int bj=(bi*nImageStep); bj<((bi+1)*nImageStep); bj++)
			{			
				struct kd_node* kd_tree = kdtree_build(feats, num_keys[i]);
				param[nI].kd_tree  = kd_tree;		// the current image feature points
				param[nI].keys     = keys[bj];		// the other image feature points
				param[nI].num_keys = num_keys[bj];	// the number of feature points
				param[nI].i = index;                // the index of match pair
				param[nI].left = i;
				param[nI].right = bj;
				nI++;
			}

			//parallel computing
			clock_t start = clock();
			running_count = 0;
			threadList.clear();				
			for (int j = 0; j < nI; j++) 
			{
				vecMatchPair[index].left  = param[j].left;
				vecMatchPair[index].right = param[j].right;

				lock_guard<mutex> guard(gMutex); 
				while (running_count == cpu_count)
					gCV.wait(gMutex);
				running_count++;	

				threadList.push_back(new thread(RunMatchOpenSift, &param[j]));  

				index++;						
			}

			//wait the threads
			list<thread*>::iterator it;
			for (it = threadList.begin();it != threadList.end();it++)
			{
				thread *t = *it;
				t->join();
				delete t;
			}

			//free buffer for k-d tress
			for (int k = 0; k < nI; k++) 
			{
				free( param[k].kd_tree->features );
				kdtree_release(param[k].kd_tree);
			}
		}

		//2. for the remaining images
		//printf("processing the remaining images ... \n");
		int nI = 0;
		for(int bj=(nBlock*nImageStep); bj<i; bj++)
		{
			//printf("%d ", bj);
			struct kd_node* kd_tree = kdtree_build(feats, num_keys[i]);
			param[nI].kd_tree  = kd_tree;		// the current image feature points
			param[nI].keys     = keys[bj];		// the other image feature points
			param[nI].num_keys = num_keys[bj];	// the number of feature points
			param[nI].i = index;                // the index of match pair
			param[nI].left = i;
			param[nI].right = bj;
			nI++;
		}		
		//parallel computing
		//printf("\n number of threads: %d \n", nI);
		running_count = 0;
		threadList.clear();				
		for (int j = 0; j < nI; j++) 
		{
			vecMatchPair[index].left  = param[j].left;
			vecMatchPair[index].right = param[j].right;
			lock_guard<mutex> guard(gMutex); 
			while (running_count == cpu_count)
				gCV.wait(gMutex);
			running_count++;	
			threadList.push_back(new thread(RunMatchOpenSift, &param[j]));  
			index++;						
		}
		//wait the threads
		list<thread*>::iterator it;
		for (it = threadList.begin();it != threadList.end();it++)
		{
			thread *t = *it;
			t->join();
			delete t;
		}
		//free buffer for k-d tress
		for (int k = 0; k < nI; k++) 
		{
			free( param[k].kd_tree->features );
			kdtree_release(param[k].kd_tree);
		}
		

		//3. free the buffer to save the current image feature points
		free(feats);
		
		printf("[KeyMatchFull] Matching to image %d\n", i);
		clock_t end = clock();    
		printf("[KeyMatchFull] Matching took %0.3fs\n", 
			(end - start) / ((double) CLOCKS_PER_SEC));
	}	    

	printf("Free memory .... \n");
	//Free keypoints
	for (int i = 0; i < num_images; i++) 
	{
		if (keys[i] != NULL)
			delete [] keys[i];
	}
	delete[] param; // added by xiedonghai, 2016.9.13
	delete[] keys;
	delete[] num_keys;
	//////////////////////////////////////////////////////////////////////////


	f = fopen(file_out, "w");
	assert(f != NULL);
	if(f==NULL)
		return -1;	
	for( int i=0; i<vecMatchPair.size(); i++)
	{
		//fprintf(f, "%d %d\n", vecMatchPair[i].left, vecMatchPair[i].right);
		fprintf(f, "%d %d\n", vecMatchPair[i].right, vecMatchPair[i].left);

		int nMatchNum = vecMatchPair[i].mp.size();
		fprintf(f, "%d\n", nMatchNum);

		for(int j=0; j<nMatchNum; j++)
		{
			//fprintf(f, "%d %d\n", vecMatchPair[i].mp[j].x, vecMatchPair[i].mp[j].y);
			fprintf(f, "%d %d\n", vecMatchPair[i].mp[j].y, vecMatchPair[i].mp[j].x);
		}
	}	
	fclose(f);

	//WriteProgressValueToFile(5.0);
	//WriteProgressValueToFile(-100.0);   

	return 0;
}