
#include "stdio.h"

//corelib
#include "File.h"
#include "commonfile.h"


/*
*/
int main(int argc, char* argv[])
{
	char inputPath[512];
	char outputPath[512];

	/*if(argc!=3)
	{
		printf("useage: %s -input inputpath  -output outputpath \n", argv[0]);
		return -1;
	}*/
	
	for(int i=1; i<argc; i++)
	{
		if( strcmp( argv[i], "-input" ) == 0 )
		{
			strcpy(inputPath, argv[i+1]);
			printf("input path: %s \n", inputPath);
			i++;
		}
		else if( strcmp( argv[i], "-output" ) == 0 )
		{
			strcpy(outputPath, argv[i+1]);
			printf("output path:%s \n", outputPath);
			i++;
		}
	}
	
	int n,nfile;
	char** filenames = NULL;

	n = 0; 
	nfile = 0;
	
	GetDirFileName(filenames, inputPath, &n, &nfile, "tif", 0);
	filenames = f2c(nfile, 512);
	GetDirFileName(filenames, inputPath, &n, &nfile, "tif", 1);

	printf("%d \n", nfile);

	if(nfile<5)
	{
		printf("no image files ! \n");
		return -1;
	}

	int nMaxImageNum = 10000;
	byte* pCount = new byte[nMaxImageNum];
	memset(pCount, 0, nMaxImageNum);

	//retrieve the id of multiple band images
	for(int i=0; i<nfile; i++)
	{
		//printf("%s \n", filenames[i]);
		
		char* title = NULL;
		GetTitleWithPosfix(filenames[i], &title);
		//printf("%s \n", title);		

		/*char id[256];
		memset(id, "\0", 256);
		strcpy(id, title+4);*/

		string stitle = title;
		string sid = stitle.substr(4,4);
		//printf("%s \n", sid);

		int nid = atoi(sid.c_str());
		//printf("%d ", nid);
		
		pCount[nid]++;
	}


	//register each band to the bench
	for(int i=0; i<nMaxImageNum; i++)
	{
		if(pCount[i]==5)
		{
			//printf("%d ", i);

			//bench image
			char band1[256];
			sprintf(band1, "%s\\IMG_%.4d_%d", inputPath, i, 1);
			printf("%s \n", band1);

			//registered with source image



		}
	}

	return 0;
}


