// Project_OrthoImage_VS2005.cpp : Defines the entry point for the console application.
//

//#include "winbase.h"


#include "stdafx.h"
#include "string.h"
#include "stdlib.h"


#include "OrthoImage.h"



/* for wuyu's output
inputs:
	argv[1]: image directory
	argv[2]: output file 
	argv[3]: resolution level, 0-high, 1-medium, 2-low(default)
*/

//int _tmain(int argc, char* argv[])
int _tmain_wy(int argc, char* argv[])
{
	if(argc!=4)
	{
		printf("input error! \n");
		printf("inputs:  *.exe  imagepath  outfile  resolution_level \n");
		return -1;
	}
    
	//inputs
	char dir_path[256];
	sprintf(dir_path,"%s",argv[1]);
	//printf("%s\n",dir_path);
    char out_path[256];
	sprintf(out_path,"%s",argv[2]);
	int nLevel = atoi(argv[3]);
    
	//
	char list_path[256];
	char cam_path[256], pt_path[256];

	//input
	sprintf(list_path,"%s\\List.txt",dir_path);
	sprintf(cam_path,"%s\\cam.txt",dir_path);
	sprintf(pt_path,"%s\\pt.txt",dir_path);

	//output
	//sprintf(out_path, "%s\\out.tif", dir_path);    
	
	MosaicOnBundleWithWYOut(list_path, cam_path, pt_path, nLevel, out_path);
		
	return 0;
}



/* for bundler's output
*/
int _tmain(int argc, char* argv[])
//int _tmain_bundler(int argc, char* argv[])
{	

	printf("usage:  -l listfile.txt -b bundle.out -p posfile.txt  -s  fpt.txt  -o out.tif -g (0/1) -r (0/1/2) \n");
	printf("-l:  listfile for images \n ");
	printf("-b:  bundle output file \n ");
	printf("-s:  smoothed point file  \n ");
	//printf("-o:  output *.tif file \n ");
	printf("optional: \n");
	printf("-p:  pos file  \n ");
	printf("-g:  if geo-referenced , default is 0 \n ");
	printf("-r:  output image level, default is 2 \n ");

	int nIsGeoCorrection = 0;
	int nLevel = 2;

	char listfile[256];
	char bundleoutfile[256];
	char smoothedFile[256];

	strcpy(listfile, "\0");
	strcpy(bundleoutfile, "\0");
	strcpy(smoothedFile, "\0");


	printf("%d \n", argc);

	for(int i=1; i<argc; i++)
	{
		//printf("%s \n", argv[i]);

		if( strcmp( argv[i], "-l" ) == 0 )
		{
			strcpy(listfile, argv[i+1]);
			printf("%s \n", listfile);
			i++;
		}
		else if( strcmp( argv[i], "-b" ) == 0 )
		{
			strcpy(bundleoutfile, argv[i+1]);
			printf("%s \n", bundleoutfile);
			i++;
		}
		else if( strcmp( argv[i], "-p" ) == 0 )
		{
			//strcpy(posfile, argv[i+1]);
			//i++;
		}
		else if( strcmp( argv[i], "-o" ) == 0 )
		{
			//strcpy(outfile, argv[i+1]);
			//printf("%s \n", outfile);
			//i++;
		}
		else if( strcmp( argv[i], "-s" ) == 0 )
		{
			strcpy(smoothedFile, argv[i+1]);
			printf("%s \n", smoothedFile);
			i++;
		}
		else if( strcmp( argv[i], "-g" ) == 0 ) // georeferenced correction, i.e. absolute pose estimation
		{
			nIsGeoCorrection = atoi(argv[i+1]);
			i++;
		}
		else if( strcmp( argv[i], "-r" ) == 0 ) // output level
		{
			nLevel = atoi(argv[i+1]);
			i++;
		}
		else
		{

		}
	}

	//if( strlen(listfile)==0 || strlen(bundleoutfile)==0 || strlen(outfile)==0 )
	if( strlen(listfile)==0 || strlen(bundleoutfile)==0 )
	{
		printf(" Input is wrong or bundle adjustment is not finished ! \n");
	
		return -1;
	}

	printf("generate orthoimage .... \n");
	MosaicOnBundleWithDEM(listfile, bundleoutfile, smoothedFile, NULL, nIsGeoCorrection, NULL, nLevel);


	printf("Finished!  \n");

	return 0;
}

