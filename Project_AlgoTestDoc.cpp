
// Project_AlgoTestDoc.cpp : CProject_AlgoTestDoc 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "Project_AlgoTest.h"
#endif

#include "Project_AlgoTestDoc.h"


//source header files
#include "Matrix.h"
#include "warp.hpp"
#include "geotiff.h"
#include "panorama.hpp"
#include "register.hpp"
#include "bundlerio.hpp"


//interface file
#include "cvInvoke.hpp"



#ifdef OPENCV_1X 
#include "cv.h"
#include "highgui.h"
#include "cxcore.h"
#else
//opencv
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#endif


#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CProject_AlgoTestDoc

IMPLEMENT_DYNCREATE(CProject_AlgoTestDoc, CDocument)

BEGIN_MESSAGE_MAP(CProject_AlgoTestDoc, CDocument)
	ON_COMMAND(ID_PANORAMA_RELATIVEPOSE, &CProject_AlgoTestDoc::OnPanoramaRelativepose)
	ON_COMMAND(ID_PANORAMA_DLT, &CProject_AlgoTestDoc::OnPanoramaDlt)
	ON_COMMAND(ID_SIMULATE_PANORAMA, &CProject_AlgoTestDoc::OnPanoramaSimulate)
	ON_COMMAND(ID_SIMULATE_RELATIVEPOSE, &CProject_AlgoTestDoc::OnRelativePoseSimulate)
	ON_COMMAND(ID_SIMULATE_PANORELATIVEPOSE, &CProject_AlgoTestDoc::OnRelativePosePanoSimulate)
END_MESSAGE_MAP()


// CProject_AlgoTestDoc 构造/析构

CProject_AlgoTestDoc::CProject_AlgoTestDoc()
{
	// TODO: 在此添加一次性构造代码

}

CProject_AlgoTestDoc::~CProject_AlgoTestDoc()
{
}

BOOL CProject_AlgoTestDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)

	return TRUE;
}




// CProject_AlgoTestDoc 序列化

void CProject_AlgoTestDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
	}
}

#ifdef SHARED_HANDLERS

// 缩略图的支持
void CProject_AlgoTestDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// 修改此代码以绘制文档数据
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// 搜索处理程序的支持
void CProject_AlgoTestDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// 从文档数据设置搜索内容。
	// 内容部分应由“;”分隔

	// 例如:  strSearchContent = _T("point;rectangle;circle;ole object;")；
	SetSearchContent(strSearchContent);
}

void CProject_AlgoTestDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CProject_AlgoTestDoc 诊断

#ifdef _DEBUG
void CProject_AlgoTestDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CProject_AlgoTestDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CProject_AlgoTestDoc 命令


void CProject_AlgoTestDoc::OnPanoramaRelativepose()
{
	// TODO: 在此添加命令处理程序代码
		
	printf("Panorama relative pose estimation ... \n");

	char* file1 = ""; 
	char* file2 = ""; 
	
	IplImage* lImage = cvLoadImage(file1);
	IplImage* rImage = cvLoadImage(file2);
	printf("%s \n", file1);
	printf("%s \n", file2);
	
	/*
	//1. feature detection
	CFeatureBase* pFeatDetect = new CSIFTFloat(); 
	pFeatDetect->Detect(file1, lImageFeats);
	pFeatDetect->Detect(file2, rImageFeats);
	*/

	ImgFeature lImageFeats;
	ImgFeature rImageFeats;

	DetectFileFeaturePts(file1, lImageFeats, 480);
	DetectFileFeaturePts(file2, rImageFeats, 480);

	//convert from spherical to real 3D
	int ht = lImage->height;
	int wd = lImage->width;

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

		CvPoint pl;
		pl.x = lImageFeats.featPts[li].col;
		pl.y = lImageFeats.featPts[li].row;
		CvPoint pr;
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
	
	double R[9];
	double T[3];
	vector<double> residual;
	residual.resize(lptPano.size());
	dll_EstimatePose5Point_Pano(lptPano, rptPano, radius, 500, 2.5, R, T, residual);


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

	/*
	double E[9];
	CalculateEssentialMatrix(R, T, E);
	fp = fopen("c:\\temp\\panoFM.txt", "w");
	for(int j=0; j<3; j++)
	{
		for(int i=0; i<3; i++)
		{
			fprintf(fp, "%lf ", E[j*3+i]);
		}
		fprintf(fp, "\n");
	}
	fclose(fp);
	*/


	//5. 3D reconstruction and output
	double dis = 4.94;     //the real distance of 
	double R0[9];
	double T0[3];
	memset(R0, 0, sizeof(double)*9);
	memset(T0, 0, sizeof(double)*3);
	R0[0] = R0[4] = R0[8] = 1;

	vector<Point3DDouble> vPts;
	vector<Point3DDouble> vColors;
	for(int i=0; i<lptPano.size(); i++)
	{
		Point3DDouble gp;
		if(residual[i]<3)
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
			vColors.push_back(color);
		}
	}

	CModelFileBase* pModel = new CPlyModel();
	pModel->Save("c:\\temp\\sphericalModel.ply", vPts, vColors);

	printf("Finished! \n");
}

void CProject_AlgoTestDoc::OnPanoramaDlt()
{
	// TODO: 在此添加命令处理程序代码
	
	//simulate the data
	
	
}


void CProject_AlgoTestDoc::OnPanoramaSimulate()
{
	// TODO: 在此添加命令处理程序代码
	
	
}




//3D points for simulation
double grdpt[20][3] = {10, -10,  -220,
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

void SimulatePair()
{
	//intrinsic parameters
	double focus = 240;
	int ht = 480;
	int wd = 640;
	int x0 = 0;
	int y0 = 0;

	//outer parameters
	double R1[9],R2[9];
	double t1[3],t2[3];    
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
	t2[0]  = 10; 
	t2[1]  = 10; 
	t2[2]  = 1;

	
	dll_GenerateRMatrix(omiga, phi, kapa, R2);
	//GenerateRMatrixDirect(omiga, phi, kapa, R2);
	for(int j=0; j<3; j++)
	{
		for(int i=0; i<3; i++)
		{
			printf("%lf ", R2[j*3+i]);
		}
		printf("\n");
	}
	printf("\n");

	////image 2
	//double R3[9];
	//double t3[3];
	//omiga  = -1.5;
	//phi    = -1;
	//kapa   = 0.5;
	////GenerateRMatrix(omiga, phi, kapa, R2);
	//GenerateRMatrixDirect(omiga, phi, kapa, R3);
	//t3[0] = 20; t3[1] = 20; t3[2] = 0.5;
	//
	//printf("Relative Pose \n");

	int numpt = 20;
	FILE* fp = fopen("c:\\temp\\simulate_pair.txt", "w");
	fprintf(fp, "%d\n", numpt);
	for(int i=0; i<numpt; i++)
	{
		srand(time(0));

		double ix1,iy1;
		dll_GrdToImg(grdpt[i][0], grdpt[i][1], grdpt[i][2], &ix1, &iy1, 
			R1, t1, focus, x0, y0, ht, wd );

		double ix2,iy2;
		dll_GrdToImg(grdpt[i][0], grdpt[i][1], grdpt[i][2], &ix2, &iy2, 
			R2, t2, focus, x0, y0, ht, wd );

		/*double ix3,iy3;
		GrdToImg(grdpt[i][0], grdpt[i][1], grdpt[i][2], &ix3, &iy3, 
			R3, t3, focus, x0, y0, ht, wd );
		*/

		//printf("%lf %lf  %lf %lf %lf %lf \n", ix1, iy1, ix2, iy2, ix3, iy3);

		//random noise
		double rx1 = ( (double)(rand())/(double)(RAND_MAX) - 0.5)*2;
		double ry1 = ( (double)(rand())/(double)(RAND_MAX) - 0.5)*2;
		double rx2 = ( (double)(rand())/(double)(RAND_MAX) - 0.5)*2;
		double ry2 = ( (double)(rand())/(double)(RAND_MAX) - 0.5)*2;
		double rx3 = ( (double)(rand())/(double)(RAND_MAX) - 0.5)*2;
		double ry3 = ( (double)(rand())/(double)(RAND_MAX) - 0.5)*2;

		//coordinate translation from topleft to the center
		fprintf(fp, "%lf %lf  %lf %lf \n", 
			ix1-wd*0.5+rx1, ht*0.5-iy1+ry1, 
			ix2-wd*0.5+rx2, ht*0.5-iy2+ry2);		
	}
	fclose(fp);
}


void CProject_AlgoTestDoc::OnRelativePoseSimulate()
{
	// TODO: 在此添加命令处理程序代码
	//printf("simulate relative pose ..... \n");

	//generate simulated pair
	SimulatePair();
	
	vector<Point2DDouble> p1;
	vector<Point2DDouble> p2;
	vector<Point2DDouble> pts;

	printf("Relative Pose Estimation ... \n");
	FILE* fp = fopen("c:\\temp\\simulate_pair.txt", "r");
	int numPt;
	fscanf(fp, "%d", &numPt);
	for(int i=0; i<numPt; i++)
	{
		double lx,ly,rx,ry;
		fscanf(fp, "%lf %lf   %lf %lf ", &lx, &ly, &rx, &ry);
		Point2DDouble tp1,tp2;
		tp1.p[0] = lx;
		tp1.p[1] = ly;
		tp2.p[0] = rx;
		tp2.p[1] = ry;
		p1.push_back(tp1);
		p2.push_back(tp2);		
	}
	fclose(fp);


	//relative pose estimation
	//CRelativePoseBase* pRP = new CEstimatePose5Point(); 
	CameraPara cam1,cam2;
	double focus = 240;    

	//initilize the intrinsic parameters
	cam1.focus = focus;
	cam2.focus = focus;
	cam1.k1 = 0;
	cam1.k2 = 0;
	cam2.k1 = 0;
	cam2.k2 = 0;

	//do 5-point relative pose estimation
	dll_EstimatePose(p1, p2, cam1, cam2);

	printf("rotation: %lf %lf %lf \n", cam2.ax, cam2.ay, cam2.az);
	for(int j=0; j<3; j++)
	{
		for(int i=0; i<3; i++)
		{
			printf("%lf ", cam2.R[j*3+i]);
		}
		printf("\n");
	}
	printf("Translation: \n");
	for(int j=0; j<3; j++)
		printf("%lf ", cam2.t[j]);
	printf("\n\n");


	//recover 3D point
	/*
	CTriangulateBase* pTriangulate = new CTriangulateCV();
	//CameraPara cam1,cam2;
	vector<Point3DDouble> gpts;
	pTriangulate->Triangulate(p1, p2, cam1, cam2, gpts);
	printf("Trianglate.... \n");
	for(int i=0; i<gpts.size(); i++)
	{
		double rx = gpts[i].p[0] / grdpt[i][0];
		double ry = gpts[i].p[1] / grdpt[i][1];
		double rz = gpts[i].p[2] / grdpt[i][2];

		printf("%lf %lf %lf   %lf %lf %lf \n", gpts[i].p[0], gpts[i].p[1], gpts[i].p[2],
			rx, ry, rz);
	}
	*/


	//triangulate using multiple point interface
	for(int i=0; i<p1.size(); i++)
	{
		vector<Point2DDouble> pts;
		vector<CameraPara> cams;

		pts.push_back(p1[i]);
		cams.push_back(cam1);
		pts.push_back(p2[i]);
		cams.push_back(cam2);

		Point3DDouble grd;
		double ferror;;

		//pTriangulate->Triangulate(pts, cams, grd, true, ferror);
		dll_Triangulate(pts, cams, grd, true, ferror);

		double rx = grd.p[0] / (grdpt[i][0] + 0.000001);
		double ry = grd.p[1] / (grdpt[i][1] + 0.000001);
		double rz = grd.p[2] / (grdpt[i][2] + 0.000001);

		printf("%lf %lf %lf   %lf %lf %lf   error: %lf \n", grd.p[0], grd.p[1], grd.p[2],
			rx, ry, rz, ferror);
	}	
}


void CProject_AlgoTestDoc::OnRelativePosePanoSimulate()
{
	printf("panorama relative pose estimation .... \n");


	int ht,wd;
	ht = 512;
	wd = 1024;
	double radius = (double)(wd) / (2*PI);
	printf("radius: %lf \n", radius);

	//input pose parameters for simulation
	double R1[9],R2[9];
	double t1[3],t2[3];    
	memset(R1, 0, sizeof(double)*9);
	memset(R2, 0, sizeof(double)*9);
	memset(t1, 0, sizeof(double)*3);
	memset(t2, 0, sizeof(double)*3);
	R1[0] = R1[4] = R1[8] = 1;

	printf("Simulation... \n");
	double omiga, phi, kapa;

	//image 1
	omiga  = -1.5;
	phi    = -2.5;
	kapa   = 3.5;
	t2[0]  = 2; 
	t2[1]  = -1; 
	t2[2]  = 0.1;

	dll_GenerateRMatrix(omiga, phi, kapa, R2);

	for(int j=0; j<3; j++)
	{
		for(int i=0; i<3; i++)
		{
			printf("%lf ", R2[j*3+i]);
		}
		printf("\n");
	}
	printf("\n");

	//from 3D to 2D
	double grdpt[11][3] = {
		10,10,   3,
		-8,15,   -3,
		12,-10,  2,
		-11,-10, 4,
		5,20,   7,
		13,-1,   -0.5,
		-17,10,  3,
		-20,-6,  -2.5,
		-5,5,    5,
		5,7,    1.6,
		-12,6,   -6
	}; //meters

	//projection to 2D image
	vector<Point2DDouble> lPts;
	vector<Point2DDouble> rPts;
	for(int i=0; i<11; i++)
	{
		double gx,gy,gz;
		gx = grdpt[i][0];
		gy = grdpt[i][1];
		gz = grdpt[i][2];

		//first image
		double p[3];
		p[0] = gx;
		p[1] = gy;
		p[2] = gz;
		double res[3];
		mult(R1, p, res, 3, 3, 1);
		res[0] += t1[0];
		res[1] += t1[1];
		res[2] += t1[2];
		double ix1,iy1;
		GrdToSphere(res[0], res[1], res[2], radius, ix1, iy1 );
		Point2DDouble lpt;
		lpt.p[0] = ix1;
		lpt.p[1] = iy1;
		lPts.push_back(lpt);

		//second image
		p[0] = gx;
		p[1] = gy;
		p[2] = gz;
		mult(R2, p, res, 3, 3, 1);
		res[0] += t2[0];
		res[1] += t2[1];
		res[2] += t2[2];
		double ix2,iy2;
		GrdToSphere(res[0], res[1], res[2], radius, ix2, iy2 );
		Point2DDouble rpt;
		rpt.p[0] = ix2;
		rpt.p[1] = iy2;
		rPts.push_back(rpt);

		printf("%lf %lf  %lf %lf \n", ix1, iy1, ix2, iy2);
	}

	//from 2D to 3D
	printf("2d to 3D ... \n");
	int row = lPts.size();
	vector<Point3DDouble> lptPano;
	vector<Point3DDouble> rptPano;
	lptPano.resize(row);
	rptPano.resize(row);
	for(int i=0; i<row; i++)
	{
		double gx,gy,gz;
		double sx = lPts[i].p[0];
		double sy = lPts[i].p[1];			
		SphereTo3D(sx,sy,radius,gx,gy,gz);
		lptPano[i].p[0] = gx;
		lptPano[i].p[1] = gy;
		lptPano[i].p[2] = gz;
		printf("%lf %lf %lf %lf %lf %lf \n", gx, gy, gz, gx/grdpt[i][0], gy/grdpt[i][1], gz/grdpt[i][2] );

		sx = rPts[i].p[0];
		sy = rPts[i].p[1];			
		SphereTo3D(sx,sy,radius,gx,gy,gz);
		rptPano[i].p[0] = gx;
		rptPano[i].p[1] = gy;
		rptPano[i].p[2] = gz;
		printf("%lf %lf %lf  \n", gx, gy, gz);
	}

	//pose estimation
	double R[9];
	double T[3];
	vector<double> residual;
	residual.resize(lptPano.size());
	dll_EstimatePose5Point_Pano(lptPano, rptPano, radius, 500, 2.5, R, T, residual);


	double R0[9];
	double T0[3];
	memset(R0, 0, sizeof(double)*9);
	memset(T0, 0, sizeof(double)*3);
	R0[0] = R0[4] = R0[8] = 1;


	//3D reconstruction
	printf("Triangulate..... \n");
	for(int i=0; i<lPts.size(); i++)
	{
		Point2DDouble lp,rp;

		//normalized coordinates
		lp.p[0] = lptPano[i].p[0] / lptPano[i].p[2];
		lp.p[1] = lptPano[i].p[1] / lptPano[i].p[2];

		rp.p[0] = rptPano[i].p[0] / rptPano[i].p[2];
		rp.p[1] = rptPano[i].p[1] / rptPano[i].p[2];

		double error = 0;
		Point3DDouble gp = dll_TriangulatePt(lp, rp, R0, T0, R, T, &error); //triangulate(lp, rp, R0, T0, R, T, &error);
		printf("%8.2lf %8.2lf %8.2lf | %8.2lf %8.2lf %8.2lf  err:%lf \n", 
			grdpt[i][0], grdpt[i][1], grdpt[i][2], 
			gp.p[0], gp.p[1], gp.p[2], error);
	}	
}



