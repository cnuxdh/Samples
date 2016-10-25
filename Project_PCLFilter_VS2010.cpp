// Project_PCLFilter_VS2010.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
using namespace std;

//pcl library
#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>
#include <pcl/filters/statistical_outlier_removal.h>
//using namespace pcl;



//coredll
#include "defs.h"

//cvlib
#include "bundlerio.hpp"


//corelib
#include "commonfile.h"



int PointCloudFilter(double* px, double* py, double* pz, int nPt,
					 int numNeibor, double stddev_mult)
{
	//prepare the 3D points
	//pcl::PointCloud<pcl::PointXYZ> ptCloud;
	pcl::PointCloud<pcl::PointXYZ>::Ptr ptCloud(new pcl::PointCloud<pcl::PointXYZ>);

	//pcl::PointXYZ pt;
	ptCloud->height = 1;
	ptCloud->width = nPt;
	ptCloud->points.resize(nPt);
	for(int i=0; i<nPt; i++)
	{
		ptCloud->points[i].x = px[i];
		ptCloud->points[i].y = py[i];
		ptCloud->points[i].z = pz[i];
	}
		
	//reading the file into the cloud structure
	pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_filtered(new pcl::PointCloud<pcl::PointXYZ>);
		
	std::cerr << "Cloud before filtering: " << std::endl;
	std::cerr << *ptCloud << std::endl;
	
	// Create the filtering object
	pcl::StatisticalOutlierRemoval<pcl::PointXYZ> sor;
	sor.setInputCloud(ptCloud);
	sor.setMeanK(numNeibor);
	sor.setStddevMulThresh(stddev_mult);
	sor.filter(*cloud_filtered);

	std::cerr << "Cloud after filtering: " << std::endl;
	std::cerr << *cloud_filtered << std::endl;


	//pcl::PCDWriter writer;
	//writer.write<pcl::PointXYZ> ("d:\\table_scene_lms400_inliers.pcd", *cloud_filtered, false);
	//sor.setNegative (true);
	//sor.filter (*cloud_filtered);
	//writer.write<pcl::PointXYZ> ("d:\\table_scene_lms400_outliers.pcd", *cloud_filtered, false);


	//output the smoothed points
	int nSmoothPt = 0;
	nSmoothPt = cloud_filtered->size();
	for(int i=0; i<nSmoothPt; i++)
	{
		px[i] = cloud_filtered->points[i].x;
		py[i] = cloud_filtered->points[i].y;
		pz[i] = cloud_filtered->points[i].z;
	}

	return nSmoothPt;
}


/*
   argv[1]: bundle.out
   argv[2]: number of neighbor points
   argv[3]: std 
   argv[4]: output file 
*/
int main(int argc, char* argv[])
{
	printf("point cloud filter .... \n");
	
	if(argc!=5)
	{
		printf("input error ! \n");
		return -1;
	}

	char filePath[256];
	strcpy(filePath, argv[1]);
	printf("%s \n", filePath);

	int     numNeibor   = atoi(argv[2]);
	double  stddev_mult = atof(argv[3]);
	
	char outfile[256];
	strcpy(outfile, argv[4]);


	/*
	//////////////////////////// reading pcd file  //////////////////////////////
	pcl::PointCloud<pcl::PointXYZ>::Ptr cloud( new pcl::PointCloud<pcl::PointXYZ> );
	//Fill in the cloud data
	pcl::PCDReader reader;
	//Replace the path below with the path where you saved your file
	//reader.read<pcl::PointXYZ> ("F:\\Data\\PointCloud\\table_scene_lms400.pcd", *cloud);
	reader.read<pcl::PointXYZ> (filePath, *cloud);
	printf( "%d ", cloud->size() );
	int nPt = cloud->size();
	double* px = (double*)malloc( nPt*sizeof(double));
	double* py = (double*)malloc( nPt*sizeof(double));
	double* pz = (double*)malloc( nPt*sizeof(double));
	for(int i=0; i<nPt; i++)
	{
		px[i] = cloud->points[i].x;
		py[i] = cloud->points[i].y;
		pz[i] = cloud->points[i].z;
	}
	*/
	

	////////////////////////////// reading bundle.out  ////////////////////
	//char* bundleFile = "c:\\sfm\\bundle.out";	
	//reading tracks from bundle out file
	vector<stPOS> camParas;
	vector<stTrack> tracks;
	
	char* postfix = NULL;
	GetPostfix(filePath, &postfix);

	if( strcmp(postfix, "out")==0 )
	{
		int res = ReadBundlerOutFile(filePath, camParas, tracks);
	}
	if( strcmp(postfix, "ply")==0 )
	{
		//reading ply file
		ReadPMVSPly(filePath, tracks);
	}	

	printf("number of points: %d \n", tracks.size());
	if( tracks.size() > 1000000 )
	{
		printf("points too large ! \n");
		return -1;
	}
	
	int nPt = tracks.size();
	double* px = (double*)malloc( nPt*sizeof(double));
	double* py = (double*)malloc( nPt*sizeof(double));
	double* pz = (double*)malloc( nPt*sizeof(double));
	for(int i=0; i<nPt; i++)
	{
		px[i] = tracks[i].x;
		py[i] = tracks[i].y;
		pz[i] = tracks[i].z;
	}
	
	//point cloud filter
	int nSmoothPt = PointCloudFilter(px, py, pz, nPt, numNeibor, stddev_mult);
	
	//output the filtered points
	FILE* fp = fopen(outfile, "w");
	fprintf(fp, "%d \n", nSmoothPt);
	for(int i=0; i<nSmoothPt; i++)
	{
		fprintf(fp, "%lf %lf %lf \n", px[i], py[i], pz[i]);
	}
	fclose(fp);	
	
	//for debug
	//WritePMVSPly("d:\\smooth.ply", px, py, pz, nSmoothPt);

	free(px);
	free(py);
	free(pz);
	
	printf("Finished! \n");
	
	return 0;
}