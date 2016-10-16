// Project_PGStationDlg.cpp : implementation file
//

#include "stdafx.h"
#include "math.h"
#include "stdio.h"
#include "io.h"
#include <fcntl.h>
#include <complex>

#include "Project_PGStation.h"
#include "Project_PGStationDlg.h"
#include "DlgInputRI.h"
#include "DlgInput.h"
#include "DlgHJAC.h"

//eigen


//boost
//#include <boost/lexical_cast.hpp>     
//#include <iostream>   
//#include <boost/shared_ptr.hpp>

//mba
#include "mba/mbaExports.h"


//coredll
#include "CoreDll/CoreDll/image.h"
#include "CoreDll/CoreDll/vim_sift.h"
#include "CoreDll/CoreDll/MosaicFuncs.h"
#include "CoreDll/CoreDll/delaunay.h"
#include "CoreDll/CoreDll/CommonFuncs1.h"
#include "CoreDll/CoreDll/keys2a.h"
#include "CoreDll/CoreDll/matching.h"
#include "CoreDll/CoreDll/SuperResolution.h"
#include "CoreDll/CoreDll/wavelet.h"
#include "CoreDll/CoreDll/geotiff.h"
#include "CoreDll/CoreDll/kalman.h"
#include "CoreDll/CoreDll/dodging.h"
#include "CoreDll/CoreDll/hist.h"
#include "CoreDll/CoreDll/ellipse.h"
#include "CoreDll/CoreDll/line.h"
#include "CoreDll/CoreDll/cluster.h"
//#include "CoreDll/CoreDll/orthoimage.h"


//imagelib
//#include "imagelib/triangulate.h"


//corelib
#include "Corelib/Gui.h"
#include "Corelib/CommonFuncs.h"
#include "Corelib/image.h"
#include "Corelib/ImageSegment.h"
#include "Corelib/ImageFunc.h"
#include "Corelib/Matrix.h"
#include "Corelib/tinyxml2.h"
#include "Corelib/File.h"
#include "Corelib/commonfile.h"
#include "Corelib/LatLong-UTMconversion.h"
#include "Corelib/CalcAngle.h"
#include "Corelib/harrfeature.h"
#include "Corelib/uav.h"

//RSLib
#include "defs.h"
#include "mie.h"
#include "Polder.h"
#include "brdf.h"
#include "Aerosol.h"
#include "gfaerosol.h"
#include "bpdf.h"
#include "InversionSimpleDll.h"
#include "HJ.h"
#include "Modis.h"


//PRLib
#include "PRLib/BeltDetectSimple.h"
#include "PRLib/track.h"
#include "PRLib/smoke.h"
//#include "PRLib/eigenObject.h"
#include "PRLib/export.h"

//SIFTlib
#include "siftLib/siftfeature.h"
#include "siftLib/siftmatch.h"


//CVLib
#include "CVLib/surf.hpp"
#include "CVLib/register.hpp"
#include "CVLib/sift.hpp"
#include "CVLib/relativepose.hpp"
#include "CVLib/ba.hpp"
#include "CVLib/mosaic.hpp"
#include "CVLib/track.hpp"
#include "CVLib/ortho.hpp"
#include "CVLib/imagedata.hpp"
#include "CVLib/bundlerio.hpp"
#include "CVLib/panorama.hpp"
#include "CVLib/cali.hpp"
#include "CVLib/OrthoImage.h"
#include "CVLib/opticalflow.hpp"
#include "CVLib/fusion.hpp"
#include "CVLib/blend.hpp"
#include "CVLib/xml.hpp"
#include "CVLib/fgbg.hpp"
#include "CVLib/ColorCorrection.hpp"
#include "CVLib/blend.hpp"


//Gdal
#include "gdal_priv.h"

//GPU
#include "GPULib/utils.h"
//#include "GPULib/feature.h"
//#include "GPULib/core.h"

//TinLib
#include "TinLib/dllmain.h"


//others
#include "imagelib/util.h"
#include "BundlerDll/BundlerDll/exports.h"
#include "siftGPU/SiftGPU.h"
//#include "matrix/matrix.h"


//IDL
#include "IDL/idl_export.h"



//Jhead
#include "jhead.h"


//pba dll
#include "pba/pba/pba.h"
#include "pba/pba/util.h"


//CImg
//#include "cimg/CImg.h"
//using namespace cimg_library;



//for OpenCV 2.x
//#include "opencv2/core/core.hpp"
//#include "opencv2/highgui/highgui.hpp"
//using namespace cv;

//for OpenCV 1.x
//#include "cv.h"
//#include "cxcore.h"
//#include "highgui.h"
//#include "cvaux.h"


#ifdef OPENCV_1X 
#include "cv.h"
#include "highgui.h"
#include "cxcore.h"
#else
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/calib3d/calib3d.hpp"
using namespace cv;
#endif



#include <string>
using namespace std;


#if defined( _MSC_VER )
#include <direct.h>		// _mkdir
#include <crtdbg.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
_CrtMemState startMemState;
_CrtMemState endMemState;
#else
#include <sys/stat.h>	// mkdir
#endif

using namespace tinyxml2;


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//int g_nCurrentIndex = -1;

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBaSimulate();
	//afx_msg void OnBaRealimages();
	afx_msg void OnAngstromBatch();
	afx_msg void OnHjInvertsimulate();
	//afx_msg void OnParasolLevel1();
	//afx_msg void OnParasolLevel2();
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{

}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//ON_COMMAND(ID_BA_SIMULATE, &CAboutDlg::OnBaSimulate)
	//ON_COMMAND(ID_BA_REALIMAGES, &CAboutDlg::OnBaRealimages)
	ON_COMMAND(ID_ANGSTROM_BATCH, &CAboutDlg::OnAngstromBatch)
	ON_COMMAND(ID_HJ_INVERTSIMULATE, &CAboutDlg::OnHjInvertsimulate)
	//ON_COMMAND(ID_PARASOL_LEVEL3, &CAboutDlg::OnParasolLevel1)
	//ON_COMMAND(ID_PARASOL_LEVEL4, &CAboutDlg::OnParasolLevel2)
END_MESSAGE_MAP()


void InitConsoleWindow()
{
	int nCrt = 0;
	FILE* fp;
	AllocConsole();
	nCrt = _open_osfhandle((long)GetStdHandle(STD_OUTPUT_HANDLE), _O_TEXT);
	fp = _fdopen(nCrt, "w");
	*stdout = *fp;
	setvbuf(stdout, NULL, _IONBF, 0);
}


void ReadProjections(char* filename, vector<Point2DDouble>& pts)
{
	int nPt;
	Point2DDouble pt;

	FILE* fp = fopen(filename, "r");
	fscanf(fp, "%d", &nPt);
	for(int i=0; i<nPt; i++)
	{
		double x,y;
		fscanf(fp, "%lf %lf", &x, &y);
		pt.p[0] = x;
		pt.p[1] = y;
		pts.push_back(pt);
	}
	fclose(fp);

	printf( "rows of file: %d \n", pts.size() );
}


//mosaic the image for feature matching
IplImage* VerticalMosaic(IplImage* pLeft, IplImage* pRight)
{	
	int nLeftWd = pLeft->width;
	int nLeftHt = pLeft->height;
	int nScanWdLeft = pLeft->widthStep;

	int nRightWd = pRight->width;
	int nRightHt = pRight->height;
	int nScanWdRight = pRight->widthStep;

	int oht = nLeftHt + nRightHt;
	int owd = max(nLeftWd, nRightWd);

	IplImage* pMosaicImage = cvCreateImage( cvSize(owd,oht), 8, 3);
	int oscanWd = pMosaicImage->widthStep;


	for(int j=0; j<nLeftHt; j++)
		for(int i=0; i<nLeftWd; i++)
		{
			pMosaicImage->imageData[j*oscanWd + i*3]   = pLeft->imageData[j*nScanWdLeft + i*3];
			pMosaicImage->imageData[j*oscanWd + i*3+1] = pLeft->imageData[j*nScanWdLeft + i*3+1];
			pMosaicImage->imageData[j*oscanWd + i*3+2] = pLeft->imageData[j*nScanWdLeft + i*3+2];
		}

		for(int j=0; j<nRightHt; j++)
			for(int i=0; i<nRightWd; i++)
			{
				pMosaicImage->imageData[(j+nLeftHt)*oscanWd + i*3]   = pRight->imageData[j*nScanWdRight + i*3];
				pMosaicImage->imageData[(j+nLeftHt)*oscanWd + i*3+1] = pRight->imageData[j*nScanWdRight + i*3+1];
				pMosaicImage->imageData[(j+nLeftHt)*oscanWd + i*3+2] = pRight->imageData[j*nScanWdRight + i*3+2];
			}

			return pMosaicImage;
}



// CProject_PGStationDlg dialog

CProject_PGStationDlg::CProject_PGStationDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProject_PGStationDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CProject_PGStationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_TAB, m_tabCtrl);
}

BEGIN_MESSAGE_MAP(CProject_PGStationDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(ID_BUTTON_MEM,OnButtonMem)
	ON_COMMAND(ID_FILE_OPEN32771, &CProject_PGStationDlg::OnFileOpen32771)
	ON_NOTIFY(LVN_ITEMCHANGED, 3002, OnItemchangedList) 
	ON_NOTIFY(LVN_ITEMCHANGED, 3003, OnPointItemchangedList) 
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_COMMAND(ID_FEATURE_HARRIS, &CProject_PGStationDlg::OnFeatureHarris)
	ON_WM_SIZE()
	ON_COMMAND(ID_EDGE_CANNY, &CProject_PGStationDlg::OnEdgeCanny)
	ON_COMMAND(ID_REGISTER_SITF, &CProject_PGStationDlg::OnRegisterSitf)
	ON_COMMAND(ID_FEATURE_SIFT, &CProject_PGStationDlg::OnFeatureSift)
	ON_COMMAND(ID_SUPERRESOLUTION_REGISTERTRANSFORM, &CProject_PGStationDlg::OnSuperresolutionRegistertransform)
	ON_COMMAND(ID_SUPERRESOLUTION_OPTICALFLOW, &CProject_PGStationDlg::OnSuperresolutionOpticalflow)
	ON_COMMAND(ID_GEOMETRY_TRIANGLE, &CProject_PGStationDlg::OnGeometryTriangle)
	ON_COMMAND(ID_SIMULATION_TRANSLATION, &CProject_PGStationDlg::OnSimulationTranslation)
	ON_COMMAND(ID_SUPERRESOLUTION_GENERATEGRIDS, &CProject_PGStationDlg::OnSuperresolutionGenerategrids)
	ON_COMMAND(ID_BUNDLER_GENERATEFEATURE, &CProject_PGStationDlg::OnBundlerGeneratefeature)
	ON_COMMAND(ID_BUNDLER_MATCHING, &CProject_PGStationDlg::OnBundlerMatching)
	ON_COMMAND(ID_BUNDLER_GENERATETRACKS, &CProject_PGStationDlg::OnBundlerGeneratetracks)
	ON_COMMAND(ID_BUNDLER_BUNDLER, &CProject_PGStationDlg::OnBundlerBundler)
	ON_COMMAND(ID_BUNDLER_CALIBRATION, &CProject_PGStationDlg::OnBundlerCalibration)
	ON_COMMAND(ID_BUNDLER_MOSAIC, &CProject_PGStationDlg::OnBundlerMosaic)
	ON_COMMAND(ID_EDGE_SOBEL, &CProject_PGStationDlg::OnEdgeSobel)
	ON_COMMAND(ID_TOOL_SMOOTH, &CProject_PGStationDlg::OnToolSmooth)
	ON_COMMAND(ID_SOBEL_X, &CProject_PGStationDlg::OnSobelX)
	ON_COMMAND(ID_SOBEL_Y, &CProject_PGStationDlg::OnSobelY)
	ON_WM_ERASEBKGND()
	ON_WM_SHOWWINDOW()
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_DETECT_LICENSEPLATE, &CProject_PGStationDlg::OnDetectLicenseplate)
	ON_COMMAND(ID_HOUGH_LINE, &CProject_PGStationDlg::OnHoughLine)
	ON_COMMAND(ID_SEGMENT_ADAPTIVE, &CProject_PGStationDlg::OnSegmentAdaptive)
	ON_COMMAND(ID_SEGMENT_MY, &CProject_PGStationDlg::OnSegmentMy)
	ON_COMMAND(ID_MORPHOLOGY_DILATE, &CProject_PGStationDlg::OnMorphologyDilate)
	ON_COMMAND(ID_MORPHOLOGY_ERODE, &CProject_PGStationDlg::OnMorphologyErode)
	ON_COMMAND(ID_SEGMENT_SIMPLE, &CProject_PGStationDlg::OnSegmentSimple)
	ON_COMMAND(ID_SEGMENT_OTSU, &CProject_PGStationDlg::OnSegmentOtsu)
	ON_COMMAND(ID_CONVERT_RGB, &CProject_PGStationDlg::OnConvertRgb2YUV)
	ON_COMMAND(ID_SUPERRESOLUTION_BATCH, &CProject_PGStationDlg::OnSuperresolutionBatch)
	ON_COMMAND(ID_DETECT_LP, &CProject_PGStationDlg::OnDetectLp)
	ON_COMMAND(ID_FILE_SAVEAS, &CProject_PGStationDlg::OnFileSaveas)
	ON_COMMAND(ID_PLATE_BASEDONTEXTURE, &CProject_PGStationDlg::OnPlateBasedontexture)
	ON_COMMAND(ID_EDGE_VERTICALEDGE, &CProject_PGStationDlg::OnEdgeVerticaledge)
	ON_COMMAND(ID_PLATE_HYBRID, &CProject_PGStationDlg::OnPlateHybrid)
	ON_COMMAND(ID_PLATE_BATCH, &CProject_PGStationDlg::OnPlateBatch)
	ON_COMMAND(ID_PLATE_BASEDONSEGMENT, &CProject_PGStationDlg::OnPlateBasedonsegment)
	ON_COMMAND(ID_QUALITYACCESS_ENTROPY, &CProject_PGStationDlg::OnQualityaccessEntropy)
	ON_COMMAND(ID_QUALITYACCESS_GRADIENT, &CProject_PGStationDlg::OnQualityaccessGradient)
	ON_COMMAND(ID_DECOMPOSE_SIZE, &CProject_PGStationDlg::OnDecomposeSize)
	ON_COMMAND(ID_DECOMPOSE_AOD, &CProject_PGStationDlg::OnDecomposeAod)
	ON_COMMAND(ID_XML_RW, &CProject_PGStationDlg::OnXmlRw)
	ON_COMMAND(ID_BUNDLER_BUNDLER_SRC, &CProject_PGStationDlg::OnBundlerBundlerSrc)
	ON_COMMAND(ID_BUNDLER_MOSAIC, &CProject_PGStationDlg::OnBundlerMosaic)
    ON_COMMAND(ID_SIMULATION_PLANEFITTING, &CProject_PGStationDlg::OnSimulatePlaneFitting)
	ON_COMMAND(ID_XML_WRITE, &CProject_PGStationDlg::OnXmlWrite)
	ON_COMMAND(ID_HISTOGRAM_MATCHING, &CProject_PGStationDlg::OnHistogramMatching)
	ON_COMMAND(ID_HOG_GENERATE, &CProject_PGStationDlg::OnHogGenerate)
	ON_COMMAND(ID_BELT_BASEDONEDGE, &CProject_PGStationDlg::OnBeltBasedonedge)
	ON_COMMAND(ID_DETECT_ECLIPSE, &CProject_PGStationDlg::OnDetectEclipse)
	ON_COMMAND(ID_TOOL_IMAGECROP, &CProject_PGStationDlg::OnToolImagecrop)
	ON_COMMAND(ID_SHARP_LAPLACE, &CProject_PGStationDlg::OnSharpLaplace)
	ON_COMMAND(ID_SHARP_GAUSSIAN, &CProject_PGStationDlg::OnSharpGaussian)
	ON_COMMAND(ID_IMAGEIO_WRITE, &CProject_PGStationDlg::OnImageioWrite)
	ON_COMMAND(ID_DEM_INTERPOLATION, &CProject_PGStationDlg::OnDemInterpolation)
	ON_COMMAND(ID_GPU_SIFT, &CProject_PGStationDlg::OnGpuSift)
	ON_COMMAND(ID_CUDA_INIT, &CProject_PGStationDlg::OnCudaInit)
	ON_COMMAND(ID_DEBUG_CALLMATLAB, &CProject_PGStationDlg::OnDebugCallmatlab)
	ON_COMMAND(ID_CALLMATLAB_MCC, &CProject_PGStationDlg::OnCallmatlabMcc)
	ON_COMMAND(ID_WRITE_BINARYFILE, &CProject_PGStationDlg::OnWriteBinaryfile)
	ON_COMMAND(ID_SOBEL_THIN, &CProject_PGStationDlg::OnSobelThin)
	ON_COMMAND(ID_CONVOLUTION_EDGE, &CProject_PGStationDlg::OnConvolutionEdge)
	ON_COMMAND(ID_MODEL_GENERATEMODELFOR6SV, &CProject_PGStationDlg::OnModelGeneratemodelfor6sv)
	ON_COMMAND(ID_FEATURE_CONTOUR, &CProject_PGStationDlg::OnFeatureContour)
	ON_COMMAND(ID_STD_VECTOR, &CProject_PGStationDlg::OnStdVector)
	ON_COMMAND(ID_AEROSOL_MIE, &CProject_PGStationDlg::OnAerosolMie)
	ON_COMMAND(ID_BELT_JUDGEDIRECTION, &CProject_PGStationDlg::OnBeltJudgedirection)
	ON_COMMAND(ID_PARASOL_READ, &CProject_PGStationDlg::OnParasolRead)
	ON_COMMAND(ID_TRIANGLE_CTINCLASS, &CProject_PGStationDlg::OnTriangleCtinclass)
	ON_COMMAND(ID_TRIANGLE_TINLIB, &CProject_PGStationDlg::OnTriangleTinlib)
	ON_COMMAND(ID_BRDF_ROUJEAN, &CProject_PGStationDlg::OnBrdfRoujean)
	ON_COMMAND(ID_TRANSMITTANCE_EXPERIMENTAL, &CProject_PGStationDlg::OnTransmittanceExperimental)
	ON_COMMAND(ID_TRANSMITTANCE_BASEDONLUTS, &CProject_PGStationDlg::OnTransmittanceBasedonluts)
	ON_COMMAND(ID_ATMOSPHERECORRECTION_RT3, &CProject_PGStationDlg::OnAtmospherecorrectionRt3)
	ON_COMMAND(ID_BPDF_BREON, &CProject_PGStationDlg::OnBpdfBreon)
	ON_COMMAND(ID_SIMULATION_USINGRT3, &CProject_PGStationDlg::OnSimulationUsingrt3)
	ON_COMMAND(ID_SIMULATE_USING6SV, &CProject_PGStationDlg::OnSimulateUsing6sv)
	ON_COMMAND(ID_AEROSOL_LEGENDRETOPHASE, &CProject_PGStationDlg::OnAerosolLegendretophase)
	ON_COMMAND(ID_SUPERRESOLUTION_POCS, &CProject_PGStationDlg::OnSuperresolutionPocs)
	ON_COMMAND(ID_FILTER_CUSTOM, &CProject_PGStationDlg::OnFilterCustom)
	ON_COMMAND(ID_SCALESPACE_DOG, &CProject_PGStationDlg::OnScalespaceDog)
	ON_COMMAND(ID_HJ_AEROSOLINVERT, &CProject_PGStationDlg::OnHjAerosolinvert)
	ON_COMMAND(ID_FILE_CLEAR, &CProject_PGStationDlg::OnFileClear)
	ON_COMMAND(ID_SOBEL_GRADIENT, &CProject_PGStationDlg::OnSobelGradient)
	ON_COMMAND(ID_MATH_ATAN2, &CProject_PGStationDlg::OnMathAtan2)
	ON_COMMAND(ID_VEHICLELOGO_LBP, &CProject_PGStationDlg::OnVehiclelogoLbp)
	ON_COMMAND(ID_LPB_HISTGRAM, &CProject_PGStationDlg::OnLpbHistgram)
	ON_COMMAND(ID_VEHICLELOGO_TEMPLATEMATCHING, &CProject_PGStationDlg::OnVehiclelogoTemplatematching)
	ON_COMMAND(ID_LPB_TEMPLATEMATCHING, &CProject_PGStationDlg::OnLpbTemplatematching)
	ON_COMMAND(ID_EIGENOBJECT_VEHICLELOG, &CProject_PGStationDlg::OnEigenobjectVehiclelog)
	ON_COMMAND(ID_LPB_IMAGE, &CProject_PGStationDlg::OnLpbImage)
	ON_COMMAND(ID_TEMPLATEMATCHING_VEHICLELOGO, &CProject_PGStationDlg::OnTemplatematchingVehiclelogo)
	ON_COMMAND(ID_SIZEDISTRIBUTION_GENERATE, &CProject_PGStationDlg::OnSizedistributionGenerate)
	ON_COMMAND(ID_PARASOL_SINGLEPOINTINVERSION, &CProject_PGStationDlg::OnParasolSinglepointinversion)
	ON_COMMAND(ID_BRDF_ROSS, &CProject_PGStationDlg::OnBrdfRossLi)
	ON_COMMAND(ID_BPDF_NADAL, &CProject_PGStationDlg::OnBpdfNadal)
	ON_COMMAND(ID_LUT_RETRIEVE, &CProject_PGStationDlg::OnLutRetrieve)
	ON_COMMAND(ID_LUT_RETRIVEMOCULEANDAEROSOL, &CProject_PGStationDlg::OnLutRetrivemoculeandaerosol)
	ON_COMMAND(ID_MOSAIC_SEAM, &CProject_PGStationDlg::OnMosaicSeam)
	ON_COMMAND(ID_TRIANGLE_VORONOI, &CProject_PGStationDlg::OnTriangleVoronoi)
	ON_COMMAND(ID_POINT_SIFTFLOAT, &CProject_PGStationDlg::OnPointSiftfloat)
	ON_COMMAND(ID_TRACKING_KLT, &CProject_PGStationDlg::OnTrackingKlt)
	ON_COMMAND(ID_TRIANGLE_DELAUNAY, &CProject_PGStationDlg::OnTriangleDelaunay)
	ON_COMMAND(ID_SFM_SIMULATE, &CProject_PGStationDlg::OnSfmSimulate)
	//ON_COMMAND(ID_SOBEL_MEM, &CProject_PGStationDlg::OnSobelMem)
	ON_COMMAND(ID_ATMOSPHERECORRECTION_GUI, &CProject_PGStationDlg::OnAtmospherecorrectionGui)
	ON_COMMAND(ID_KLT_LOSTOBJ, &CProject_PGStationDlg::OnKltLostobj)
	ON_COMMAND(ID_KLT_FRAMEDIFF, &CProject_PGStationDlg::OnKltFramediff)
	ON_COMMAND(ID_ATMOSPHERECORRECTION_BAT, &CProject_PGStationDlg::OnAtmospherecorrectionBat)
	ON_COMMAND(ID_KLT_TWOFRAMES, &CProject_PGStationDlg::OnKltTwoframes)
	ON_COMMAND(ID_KLT_SINGLEPT, &CProject_PGStationDlg::OnKltSinglept)
	ON_COMMAND(ID_BPDF_POLARIZEDFRESNEL, &CProject_PGStationDlg::OnBpdfPolarizedfresnel)
	ON_COMMAND(ID_BLOCKMATCHING_TWOFRAMES, &CProject_PGStationDlg::OnBlockmatchingTwoframes)
	ON_COMMAND(ID_REGISTER_SIFT, &CProject_PGStationDlg::OnRegisterSiftFloat)
	ON_COMMAND(ID_DISTRIBUTION_GAUSSIAN, &CProject_PGStationDlg::OnDistributionGaussian)
	ON_COMMAND(ID_SMOKE_COLOR, &CProject_PGStationDlg::OnSmokeColor)
	ON_COMMAND(ID_TRACK_FRAMEDIFFERENCING, &CProject_PGStationDlg::OnTrackFramedifferencing)
	ON_COMMAND(ID_PARASOL_AREAINVERSION, &CProject_PGStationDlg::OnParasolAreainversion)
	ON_COMMAND(ID_PARASOL_SINGLEPOINTINVERSION33004, &CProject_PGStationDlg::OnParasolSinglepointinversion33004)
	ON_COMMAND(ID_WAVELET_TEST, &CProject_PGStationDlg::OnWaveletTest)
	ON_COMMAND(ID_SURFACE_DEM33011, &CProject_PGStationDlg::OnSurfaceDem)
	ON_COMMAND(ID_SURFACE_SLOPE, &CProject_PGStationDlg::OnSurfaceSlope)
	ON_COMMAND(ID_GEOTRANSFORM_GRD2LL, &CProject_PGStationDlg::OnGeotransformGrd2ll)
	ON_COMMAND(ID_GEOTRANSFORM_LL2GRD, &CProject_PGStationDlg::OnGeotransformLl2grd)
	ON_COMMAND(ID_SURFACE_EXTRACTAREA, &CProject_PGStationDlg::OnSurfaceExtractarea)
	ON_COMMAND(ID_SMOKE_WAVELET, &CProject_PGStationDlg::OnSmokeWavelet)
	ON_COMMAND(ID_KALMANFILTER_WAVELET, &CProject_PGStationDlg::OnKalmanfilterWavelet)
	ON_COMMAND(ID_OPENCV2_IMAGE, &CProject_PGStationDlg::OnOpencv2Image)
	ON_COMMAND(ID_SMOKE_SOBEL, &CProject_PGStationDlg::OnSmokeSobel)
	ON_COMMAND(ID_ERDAS_MERGE, &CProject_PGStationDlg::OnErdasMerge)
	ON_COMMAND(ID_KLT_WITHKALMAN, &CProject_PGStationDlg::OnKltWithkalman)
	ON_COMMAND(ID_KLT_SINGLEPTWITHKALMAN, &CProject_PGStationDlg::OnKltSingleptwithkalman)
	ON_COMMAND(ID_PARASOL_POINTSATELLITE, &CProject_PGStationDlg::OnParasolPointsatellite)
	ON_COMMAND(ID_SURFACE_MOSAIC, &CProject_PGStationDlg::OnSurfaceMosaic)
	ON_COMMAND(ID_DODGING_MASK, &CProject_PGStationDlg::OnDodgingMask)
	ON_COMMAND(ID_DEM_MEDIANFILTER, &CProject_PGStationDlg::OnDemMedianfilter)
	ON_COMMAND(ID_TRANSFORM_DISTANCE, &CProject_PGStationDlg::OnTransformDistance)
	ON_COMMAND(ID_POINT_SURF, &CProject_PGStationDlg::OnPointSurf)
	ON_COMMAND(ID_REGISTER_SURF, &CProject_PGStationDlg::OnRegisterSurf)
	ON_COMMAND(ID_REGISTER_GENERAL, &CProject_PGStationDlg::OnRegisterGeneral)
	ON_COMMAND(ID_GEOMOSAIC_HISTMATCHING, &CProject_PGStationDlg::OnGeomosaicHistmatching)
	ON_COMMAND(ID_SFM_DETECTFEATURES, &CProject_PGStationDlg::OnSfmDetectfeatures)
	ON_COMMAND(ID_SFM_MATCHING, &CProject_PGStationDlg::OnSfmMatching)
	ON_COMMAND(ID_MATCHING_PAIR, &CProject_PGStationDlg::OnMatchingPair)
	ON_COMMAND(ID_MATCHING_SEQS, &CProject_PGStationDlg::OnMatchingSeqs)
	ON_COMMAND(ID_SFM_GENERATETRACKS, &CProject_PGStationDlg::OnSfmGeneratetracks)
	ON_COMMAND(ID_SFM_RELATIVEPOSE, &CProject_PGStationDlg::OnSfmRelativepose)
	ON_COMMAND(ID_SMOKE_DEMO, &CProject_PGStationDlg::OnSmokeDemo)
	ON_COMMAND(ID_RELATIVEPOSE_SIMULATE, &CProject_PGStationDlg::OnRelativeposeSimulate)
	ON_COMMAND(ID_RELATIVEPOSE_REALIMAGES, &CProject_PGStationDlg::OnRelativeposeRealimages)
	ON_COMMAND(ID_SIMULATEDATA_TWOVIEWS, &CProject_PGStationDlg::OnSimulatedataTwoviews)
	ON_COMMAND(ID_SIMULATEDATA_MULTIVIEWS, &CProject_PGStationDlg::OnSimulatedataMultiviews)
	ON_COMMAND(ID_OBSOLUTEPOSE_SIMULATE, &CProject_PGStationDlg::OnObsoluteposeSimulate)
	ON_COMMAND(ID_EIGENOBJECT_TRAINGING, &CProject_PGStationDlg::OnEigenobjectTrainging)
	ON_COMMAND(ID_EIGENOBJECT_RECOGNITION, &CProject_PGStationDlg::OnEigenobjectRecognition)
	ON_COMMAND(ID_TOOL_NORMALIZE, &CProject_PGStationDlg::OnToolNormalize)
	ON_COMMAND(ID_EIGENOBJECT_PROJECTION, &CProject_PGStationDlg::OnEigenobjectProjection)
	ON_COMMAND(ID_VEHICLELOGO_PCA, &CProject_PGStationDlg::OnVehiclelogoPca)
	ON_COMMAND(ID_SIMULATEDATA_SINGLEVIEW, &CProject_PGStationDlg::OnSimulatedataSingleview)
	ON_COMMAND(ID_HJ_ATMOSPHERICCORRECTION, &CProject_PGStationDlg::OnHjAtmosphericcorrection)
	ON_COMMAND(ID_BA_SIMULATE, &CProject_PGStationDlg::OnBaSimulate)
	ON_COMMAND(ID_BA_REALIMAGES, &CProject_PGStationDlg::OnBaRealimages)
	ON_COMMAND(ID_MOSAIC_SEAMFINDER, &CProject_PGStationDlg::OnMosaicSeamfinder)
	ON_COMMAND(ID_MOSAIC_BLEND, &CProject_PGStationDlg::OnMosaicBlend)
	ON_COMMAND(ID_GEOMOSAIC_WITHBLEND, &CProject_PGStationDlg::OnGeomosaicWithblend)
	ON_COMMAND(ID_EIGENOBJECT_HISTOGRAMMATCHING, &CProject_PGStationDlg::OnEigenobjectHistogrammatching)
	ON_COMMAND(ID_EIGENOBJECT_SIMILARITY, &CProject_PGStationDlg::OnEigenobjectSimilarity)
	ON_COMMAND(ID_HJ_ANGLEINTERPOLATION, &CProject_PGStationDlg::OnHjAngleinterpolation)
	ON_COMMAND(ID_TEMPLATEMATCHING_FAST, &CProject_PGStationDlg::OnTemplatematchingFast)
	ON_COMMAND(ID_HARR_INTEGRALIMAGE, &CProject_PGStationDlg::OnHarrIntegralimage)
	ON_COMMAND(ID_TEMPLATEMATCHING_HARR, &CProject_PGStationDlg::OnTemplatematchingHarr)
	ON_COMMAND(ID_EXTERNALFORMAT_HDF, &CProject_PGStationDlg::OnExternalformatHdf)
	ON_COMMAND(ID_FILL_SCANFILL, &CProject_PGStationDlg::OnFillScanfill)
	ON_COMMAND(ID_ATMOSPHERECORRECTION_HJ, &CProject_PGStationDlg::OnAtmospherecorrectionHj)
	ON_COMMAND(ID_EPIPOLARGEOMETRY_FUNDAMENTALMATRIX33088, &CProject_PGStationDlg::OnEpipolargeometryFundamentalmatrix)
	ON_COMMAND(ID_NORMALIZE_COLOR, &CProject_PGStationDlg::OnNormalizeColor)
	ON_COMMAND(ID_ATMOSPHERECORRECTION_PARASOL, &CProject_PGStationDlg::OnAtmospherecorrectionParasol)
	ON_COMMAND(ID_CONVERT_SDS, &CProject_PGStationDlg::OnConvertSds)
	ON_COMMAND(ID_ORTHOIMAGE_UAV, &CProject_PGStationDlg::OnOrthoimageUav)
	ON_COMMAND(ID_SINUSOIDAL_GRID2LL, &CProject_PGStationDlg::OnSinusoidalGrid2ll)
	ON_COMMAND(ID_STRING_VECTOR, &CProject_PGStationDlg::OnStringVector)
	ON_COMMAND(ID_BUNDLER_READTRACKS, &CProject_PGStationDlg::OnBundlerReadtracks)
	ON_COMMAND(ID_MOSAIC_OPTICALFLOW, &CProject_PGStationDlg::OnMosaicOpticalflow)
	ON_COMMAND(ID_PARASOL_READLEVEL2, &CProject_PGStationDlg::OnParasolReadlevel2)
	ON_COMMAND(ID_GF1_INVERT, &CProject_PGStationDlg::OnGf1Invert)
	ON_COMMAND(ID_IMAGECUT_MOD02, &CProject_PGStationDlg::OnImagecutMod02)
	ON_COMMAND(ID_IMAGECUT_CUT, &CProject_PGStationDlg::OnImagecutCut)
	ON_COMMAND(ID_BA_NEWAPI, &CProject_PGStationDlg::OnBaNewapi)
	ON_COMMAND(ID_IGBP_SINOID, &CProject_PGStationDlg::OnIgbpSinoid)
	ON_COMMAND(ID_HJ_AEROSOLINVERTDDV, &CProject_PGStationDlg::OnHjAerosolinvertddv)
	ON_COMMAND(ID_INTERPOLATION_2D, &CProject_PGStationDlg::OnInterpolation2d)
	ON_COMMAND(ID_SIMULATION_DT, &CProject_PGStationDlg::OnSimulationDt)
	//ON_COMMAND(ID_HJ_SIMULATION, &CProject_PGStationDlg::OnHjSimulation)
	ON_COMMAND(ID_HJ_INVERTSINGLEPT, &CProject_PGStationDlg::OnHjInvertsinglept)
	ON_COMMAND(ID_MODIS_INVERTAOD, &CProject_PGStationDlg::OnModisInvertaod)
	//ON_COMMAND(ID_GPU_DOT, &CProject_PGStationDlg::OnGpuDot)
	ON_COMMAND(ID_AEROSOL_ANGSTROM, &CProject_PGStationDlg::OnAerosolAngstrom)
	ON_COMMAND(ID_HJ_SIMULATETOA, &CProject_PGStationDlg::OnHjSimulatetoa)
	ON_COMMAND(ID_LUT_INTERPOLATION, &CProject_PGStationDlg::OnLutInterpolation)
	ON_COMMAND(ID_HOUGH_CIRCLE, &CProject_PGStationDlg::OnHoughCircle)
	ON_COMMAND(ID_ECLIPSE_RANDOM, &CProject_PGStationDlg::OnEclipseRandom)
	ON_COMMAND(ID_HOUGH_LINENEW, &CProject_PGStationDlg::OnHoughLinenew)
	ON_COMMAND(ID_HJ_CALCDDV, &CProject_PGStationDlg::OnHjCalcddv)
	ON_COMMAND(ID_HJ_SIMULATEFROMAOD, &CProject_PGStationDlg::OnHjSimulatefromaod)
	ON_COMMAND(ID_CALLIDL_DLL, &CProject_PGStationDlg::OnCallidlDll)
	ON_COMMAND(ID_HJ_DATASMOOTH, &CProject_PGStationDlg::OnHjDatasmooth)
	//ON_COMMAND(ID_HJ_SIMULATES, &CProject_PGStationDlg::OnHjSimulates)
	ON_COMMAND(ID_HJ_SIMULATESNR, &CProject_PGStationDlg::OnHjSimulatesnr)
	ON_COMMAND(ID_SNR_ESTIMATE, &CProject_PGStationDlg::OnSnrEstimate)
	ON_COMMAND(ID_HJ_INVERTSINGLEPTSAT, &CProject_PGStationDlg::OnHjInvertsingleptsat)
	ON_COMMAND(ID_HJ_UI, &CProject_PGStationDlg::OnHjUi)
	ON_COMMAND(ID_MOSAIC_LAPLACIAN, &CProject_PGStationDlg::OnMosaicLaplacian)
	ON_COMMAND(ID_MOSAIC_BLENDBYHAND, &CProject_PGStationDlg::OnMosaicBlendbyhand)
	ON_COMMAND(ID_HJ_AEROSOLINVERTDDVHIGH, &CProject_PGStationDlg::OnHjAerosolinvertddvhigh)
	ON_COMMAND(ID_HJ_DOWNSAMPLE, &CProject_PGStationDlg::OnHjDownsample)
	ON_COMMAND(ID_HJ_INTERPOLATION, &CProject_PGStationDlg::OnHjInterpolation)
	ON_COMMAND(ID_ANGSTROM_BATCH, &CProject_PGStationDlg::OnAngstromBatch)
	ON_COMMAND(ID_AERONET_SIZEDISTRIBUTION, &CProject_PGStationDlg::OnAeronetSizedistribution)
	ON_COMMAND(ID_AERONET_SIZEDISTRIBUTIONDISCRETE, &CProject_PGStationDlg::OnAeronetSizedistributiondiscrete)
	ON_COMMAND(ID_AERONET_SIZEDISTRIBUTIONDISCRETESEASONS, &CProject_PGStationDlg::OnAeronetSizedistributiondiscreteseasons)
	ON_COMMAND(ID_SIZEDISTRIBUTIONSEASONS_SPRING, &CProject_PGStationDlg::OnSizedistributionseasonsSpring)
	ON_COMMAND(ID_SIZEDISTRIBUTIONSEASONS_SUMMER, &CProject_PGStationDlg::OnSizedistributionseasonsSummer)
	ON_COMMAND(ID_SIZEDISTRIBUTIONSEASONS_AUTUMN, &CProject_PGStationDlg::OnSizedistributionseasonsAutumn)
	ON_COMMAND(ID_SIZEDISTRIBUTIONSEASONS_WINTER, &CProject_PGStationDlg::OnSizedistributionseasonsWinter)
	ON_COMMAND(ID_AERONET_CLUSTER, &CProject_PGStationDlg::OnAeronetCluster)
	ON_COMMAND(ID_CLUSTER_ISODATA, &CProject_PGStationDlg::OnClusterIsodata)
	ON_COMMAND(ID_HJ_SIMULATE33156, &CProject_PGStationDlg::OnHjSimulate33156)
	ON_COMMAND(ID_HJ_SIMULATE33157, &CProject_PGStationDlg::OnHjSimulate33157)
	ON_COMMAND(ID_PLY_READ, &CProject_PGStationDlg::OnPlyRead)
	ON_COMMAND(ID_TRIANGULATE_FITPLANE, &CProject_PGStationDlg::OnTriangulateFitplane)
	ON_COMMAND(ID_BOOST_TEST, &CProject_PGStationDlg::OnBoostTest)
	ON_COMMAND(ID_DEM_MBA, &CProject_PGStationDlg::OnDemMba)
	ON_COMMAND(ID_ORTHO_MOSAIC, &CProject_PGStationDlg::OnOrthoMosaic)
	ON_COMMAND(ID_TRANSFORM_ROTATION, &CProject_PGStationDlg::OnTransformRotation)
	ON_COMMAND(ID_CONVERT_MOD09, &CProject_PGStationDlg::OnConvertMod09)
	ON_COMMAND(ID_VALIDATE_MODIS, &CProject_PGStationDlg::OnValidateModis)
	ON_COMMAND(ID_BLUR_ENTROPY, &CProject_PGStationDlg::OnBlurEntropy)
	ON_COMMAND(ID_ENTROPY_MAX, &CProject_PGStationDlg::OnEntropyMax)
	ON_COMMAND(ID_BLUR_BATCH, &CProject_PGStationDlg::OnBlurBatch)
	ON_COMMAND(ID_SEA_IMAGECROP, &CProject_PGStationDlg::OnSeaImageCrop)
	ON_COMMAND(ID_DATABASE_CIFAR, &CProject_PGStationDlg::OnDatabaseCifar)
	ON_COMMAND(ID_DATABASE_GENERATELABEL, &CProject_PGStationDlg::OnDatabaseGeneratelabel)
	ON_COMMAND(ID_ATMOSPHERECORRECTION_HJ33190, &CProject_PGStationDlg::OnAtmospherecorrectionHj33190)
	ON_COMMAND(ID_SIMULATION_OOP, &CProject_PGStationDlg::OnSimulationOop)
	ON_COMMAND(ID_HJ_INVERTSIMULATE, &CProject_PGStationDlg::OnHjInvertsimulate)
	ON_COMMAND(ID_SENSITIVITY_RATIO, &CProject_PGStationDlg::OnSensitivityRatio)
	ON_COMMAND(ID_SENSITIVITY_ERRORBYRATIO, &CProject_PGStationDlg::OnSensitivityErrorbyratio)
	ON_COMMAND(ID_SEA_USINGPOS, &CProject_PGStationDlg::OnSeaUsingpos)
	ON_COMMAND(ID_HJ_BLUE, &CProject_PGStationDlg::OnHjBlueRed)
	ON_COMMAND(ID_SEA_PROCESSPOS, &CProject_PGStationDlg::OnSeaProcesspos)
	ON_COMMAND(ID_MODIS_MOD09, &CProject_PGStationDlg::OnModisMod09)
	ON_COMMAND(ID_PYTHON_INVOKE, &CProject_PGStationDlg::OnPythonInvoke)
	ON_COMMAND(ID_READING_PARASOLLS2, &CProject_PGStationDlg::OnReadingParasolls2)
	//ON_COMMAND(ID_PARASOL_LEVEL1, &CProject_PGStationDlg::OnParasolLevel1)
	//ON_COMMAND(ID_PARASOL_LEVEL2, &CProject_PGStationDlg::OnParasolLevel2)
	ON_COMMAND(ID_PARASOL_LEVEL3, &CProject_PGStationDlg::OnParasolLevel3)
	ON_COMMAND(ID_PARASOL_LEVEL4, &CProject_PGStationDlg::OnParasolLevel4)
	ON_COMMAND(ID_SINUSOIDAL_ROWCOL, &CProject_PGStationDlg::OnSinusoidalRowcol)
	ON_COMMAND(ID_PANORAMIC_RELATIVEPOSEESTIMATION, &CProject_PGStationDlg::OnPanoramicRelativeposeestimation)
	ON_COMMAND(ID_PANORAMIC_MATCHING, &CProject_PGStationDlg::OnPanoramicMatching)
	ON_COMMAND(ID_PANORAMIC_SIMULATION, &CProject_PGStationDlg::OnPanoramicSimulation)
	ON_COMMAND(ID_CALIBRATION_OPENCV, &CProject_PGStationDlg::OnCalibrationOpencv)
	ON_COMMAND(ID_PANORAMIC_READINGPOS, &CProject_PGStationDlg::OnPanoramicReadingpos)
	ON_COMMAND(ID_PANORAMIC_EPIPOLARIMAGE, &CProject_PGStationDlg::OnPanoramicEpipolarimage)
	ON_COMMAND(ID_MOSAIC_GEOTIFF, &CProject_PGStationDlg::OnMosaicGeotiff)
	ON_COMMAND(ID_MOSAIC_BUNDLER, &CProject_PGStationDlg::OnMosaicBundler)
	ON_COMMAND(ID_FILEHEADER_JPEG, &CProject_PGStationDlg::OnFileheaderJpeg)
	ON_COMMAND(ID_MOSAIC_BUNDLER33231, &CProject_PGStationDlg::OnMosaicBundler33231)
	ON_COMMAND(ID_PBA_CPU, &CProject_PGStationDlg::OnPbaCpu)
	ON_COMMAND(ID_MATCHING_SIMPLEFLOW, &CProject_PGStationDlg::OnMatchingSimpleflow)
	ON_COMMAND(ID_PANORAMIC_DENSEPOINTCLOUD, &CProject_PGStationDlg::OnPanoramicDensepointcloud)
	ON_COMMAND(ID_PANORAMIC_RELATIVEPOSEBATCH, &CProject_PGStationDlg::OnPanoramicRelativeposebatch)
	ON_COMMAND(ID_TRANSFORM_ROTATEATOB, &CProject_PGStationDlg::OnTransformRotateatob)
	ON_COMMAND(ID_FUSION_LOG, &CProject_PGStationDlg::OnFusionLog)
	ON_COMMAND(ID_FUSION_LOGSEQUENCES, &CProject_PGStationDlg::OnFusionLogsequences)
	ON_COMMAND(ID_XML_OPENCV, &CProject_PGStationDlg::OnXmlOpencv)
	ON_COMMAND(ID_MRT_GENERATEFILELISTS, &CProject_PGStationDlg::OnMrtGeneratefilelists)
	ON_COMMAND(ID_MODIS_GOETIFF, &CProject_PGStationDlg::OnModisGoetiff)
	ON_COMMAND(ID_FOREGROUND_BACKGROUNDMODEL, &CProject_PGStationDlg::OnForegroundBackgroundmodel)
	ON_COMMAND(ID_PANORAMIC_PROJECTIONTOPLANE, &CProject_PGStationDlg::OnPanoramicProjectiontoplane)
	ON_COMMAND(ID_KLT_PANOIMAGES, &CProject_PGStationDlg::OnKltPanoimages)
	ON_COMMAND(ID_CALIBRATION_GENERATECHESSBOARD, &CProject_PGStationDlg::OnCalibrationGeneratechessboard)
	ON_COMMAND(ID_COLORCORRECTION_VIGNETTING, &CProject_PGStationDlg::OnColorcorrectionVignetting)
	ON_COMMAND(ID_MATRIX_SVD, &CProject_PGStationDlg::OnMatrixSvd)
	ON_COMMAND(ID_CROWDDENSITY_BACKGROUND, &CProject_PGStationDlg::OnCrowddensityBackground)
	ON_COMMAND(ID_CHAR_GENERATEPRODUCTFILE, &CProject_PGStationDlg::OnCharGenerateproductfile)
	ON_COMMAND(ID_DEBUG_FILLBLACKPIXEL, &CProject_PGStationDlg::OnDebugFillblackpixel)
	ON_COMMAND(ID_DEBUG_GAUSSIANSMOOTH, &CProject_PGStationDlg::OnDebugGaussiansmooth)
	ON_COMMAND(ID_DEBUG_MYSMOOTH, &CProject_PGStationDlg::OnDebugMysmooth)
	ON_COMMAND(ID_GDAL_WRITEJPEG, &CProject_PGStationDlg::OnGdalWritejpeg)
	ON_COMMAND(ID_SHADOW_LITVINOV, &CProject_PGStationDlg::OnShadowLitvinov)
	ON_COMMAND(ID_SHADOW_ROUJEAN, &CProject_PGStationDlg::OnShadowRoujean)
	ON_COMMAND(ID_SHADOW_ROSSLI, &CProject_PGStationDlg::OnShadowRossli)
	ON_COMMAND(ID_BRDF_READING, &CProject_PGStationDlg::OnBrdfReading)
	ON_COMMAND(ID_BRDF_LOCATIONSEPERATE, &CProject_PGStationDlg::OnBrdfLocationseperate)
	ON_COMMAND(ID_DEBUG_SLOPE, &CProject_PGStationDlg::OnDebugSlope)
	ON_COMMAND(ID_BRDF_NDVI, &CProject_PGStationDlg::OnBrdfNdvi)
	ON_COMMAND(ID_BPDF_BARESOIL, &CProject_PGStationDlg::OnBpdfBaresoil)
	ON_COMMAND(ID_CIMG_LOADING, &CProject_PGStationDlg::OnCimgLoading)
	ON_COMMAND(ID_SURFACE_NDVI, &CProject_PGStationDlg::OnSurfaceNdvi)
	ON_COMMAND(ID_PANORAMIC_EPIPOLARMATCHDISP, &CProject_PGStationDlg::OnPanoramicEpipolarmatchdisp)
	ON_COMMAND(ID_PANORAMIC_CONVERTTOCYLINDER, &CProject_PGStationDlg::OnPanoramicConverttocylinder)
	ON_COMMAND(ID_BPDF_POLARDATA, &CProject_PGStationDlg::OnBpdfPolardata)
	ON_COMMAND(ID_SELECT_FILES, &CProject_PGStationDlg::OnSelectFiles)
	ON_COMMAND(ID_XML_JPEG, &CProject_PGStationDlg::OnXmlJpeg)
	ON_COMMAND(ID_FORMAT_AUV, &CProject_PGStationDlg::OnFormatAuv)
	ON_COMMAND(ID_PLY_MOSAIC, &CProject_PGStationDlg::OnPlyMosaic)
	ON_COMMAND(ID_TEMPLATE_FUNCTION, &CProject_PGStationDlg::OnTemplateFunction)
	//ON_COMMAND(ID_EIGEN_TEST, &CProject_PGStationDlg::OnEigenTest)
	ON_COMMAND(ID_EIGEN_TEST, &CProject_PGStationDlg::OnEigenTest)
	END_MESSAGE_MAP()


//mouse event handler
CvPoint lButtonDownPt;
CvPoint lButtonUpPt;
bool    bIsLButtonDown = false;
void on_image_mouse( int event, int x, int y, int flags, void* param)
{
	//get the dialog pointer
	CProject_PGStationDlg* pWnd = (CProject_PGStationDlg*)(param);

	if( event == CV_EVENT_LBUTTONDOWN )
	{
		lButtonDownPt.x = x;
		lButtonDownPt.y = y;
		bIsLButtonDown = true;
	}
	else if ( event == CV_EVENT_MOUSEMOVE )
	{
		if(bIsLButtonDown)
		{
			IplImage* disp = pWnd->CloneCurrentImageColor();
			CvPoint cpt;
			cpt.x = x;
			cpt.y = y;
			
			//cvLine(disp, lButtonDownPt, cpt, CV_RGB(255,0,0));
			//cvRect(lButtonDownPt.x, lButtonDownPt.y, )
			cvDrawRect(disp, lButtonDownPt, cpt, CV_RGB(255,0,0), 2);
			cvShowImage("Image", disp);
			cvReleaseImage(&disp);
		}
	
	}
	else if( event == CV_EVENT_LBUTTONUP )
	{
		lButtonUpPt.x = x;
		lButtonUpPt.y = y;
		bIsLButtonDown = false;

		//crop the image to get the template image
		CvRect rect;
		rect.x = min( lButtonDownPt.x, lButtonUpPt.x );
		rect.y = min( lButtonDownPt.y, lButtonUpPt.y );
		rect.width = abs(lButtonDownPt.x - lButtonUpPt.x );
		rect.height = abs(lButtonDownPt.y - lButtonUpPt.y );

		if( rect.width>16 && rect.height>16 )
		{
			IplImage* pImage = pWnd->CloneCurrentImage();
			cvSetImageROI(pImage, rect);
			cvSaveImage("c:\\temp\\template.jpg", pImage);
			cvResetImageROI(pImage);
			cvReleaseImage(&pImage);
		}
	}
}

// CProject_PGStationDlg message handlers
void on_triangle_mouse(int event, int x, int y, int flags, void* param)
{		
	CvPoint pt1, pt2;
	int     rh,rw;

	//get the dialog pointer
	CProject_PGStationDlg* pWnd = (CProject_PGStationDlg*)(param);

	if( event == CV_EVENT_LBUTTONDOWN )
	{
		pWnd->PtInTriangle(x, y);
	}
	if( event == CV_EVENT_MOUSEMOVE )
	{		
	}
	if( event == CV_EVENT_LBUTTONUP)
	{
	}		
}

void on_left_mouse( int event, int x, int y, int flags, void* param)
{
	CProject_PGStationDlg* pWnd = (CProject_PGStationDlg*)(param);

	if( event == CV_EVENT_LBUTTONDOWN )
	{
		CvPoint cp;
		cp.x = x;
		cp.y = y;


		Point2DDouble pt;
		pt.p[0] = x;
		pt.p[1] = y;		
		double a,b,c;
		pWnd->m_pEstimateFM->CalculateEpipolarLine(pt, 0, a, b, c);

		int wd = pWnd->m_pRightImage->width;
		CvPoint p1,p2;
		p1.x = 0;
		p1.y = -c/b;
        p2.x = wd;
		p2.y = -(a*wd+c)/b;

		IplImage* pImage = cvCloneImage(pWnd->m_pRightImage);
		cvLine(pImage, p1, p2, CV_RGB(255,0,0), 2);
		cvShowImage("Right", pImage);
		cvReleaseImage(&pImage);

		pImage = cvCloneImage(pWnd->m_pLeftImage);
		cvDrawCircle(pImage, cp, 3, CV_RGB(255,255,255), 3);
		cvShowImage("Left", pImage);
		cvReleaseImage(&pImage);


	}
}
void on_right_mouse( int event, int x, int y, int flags, void* param)
{
	CProject_PGStationDlg* pWnd = (CProject_PGStationDlg*)(param);

	if( event == CV_EVENT_LBUTTONDOWN )
	{

		CvPoint cp;
		cp.x = x;
		cp.y = y;

		Point2DDouble pt;
		pt.p[0] = x;
		pt.p[1] = y;		
		
		double a,b,c;
		pWnd->m_pEstimateFM->CalculateEpipolarLine(pt, 1, a, b, c);

		int wd = pWnd->m_pLeftImage->width;
		CvPoint p1,p2;
		p1.x = 0;
		p1.y = -c/b;
		p2.x = wd;
		p2.y = -(a*wd+c)/b;

		IplImage* pImage = cvCloneImage(pWnd->m_pLeftImage);
		cvLine(pImage, p1, p2, CV_RGB(255,0,0), 2);
		cvShowImage("Left", pImage);
		cvReleaseImage(&pImage);


		pImage = cvCloneImage(pWnd->m_pRightImage);
		cvDrawCircle(pImage, cp, 3, CV_RGB(255,255,255), 3);
		cvShowImage("Right", pImage);
		cvReleaseImage(&pImage);
	}
}


void PairMatch(IplImage* pLeft, IplImage* pRight, MatchPairVector& matchPair)
{
	int i,j;
	IplImage* pDispLeft = cvCloneImage(pLeft);
	IplImage* pDispRight = cvCloneImage(pRight);

	//detect sift features 
	struct feature* m_pFeat1 = NULL;
	struct feature* m_pFeat2 = NULL;
	int m_nFeat1, m_nFeat2;
	if(m_pFeat1!=NULL) free(m_pFeat1);
	if(m_pFeat2!=NULL) free(m_pFeat2);
	
	m_nFeat1 = sift_features( pLeft,  &m_pFeat1);
	m_nFeat2 = sift_features( pRight, &m_pFeat2);

	printf("%d  %d \n", m_nFeat1, m_nFeat2);	

	//registration
	int nMaxpt = max(m_nFeat1, m_nFeat2);
	struct feature* mp1 = (struct feature*)malloc(nMaxpt*sizeof(struct feature));
	struct feature* mp2 = (struct feature*)malloc(nMaxpt*sizeof(struct feature));
	int m_ncp=0;
	FindCorrespondence(m_pFeat1, m_nFeat1, m_pFeat2, m_nFeat2, mp1, mp2, &m_ncp);
	
	draw_features(pDispLeft,  mp1, m_ncp);
	draw_features(pDispRight, mp2, m_ncp);
	
	/*
	//extract feature patch from the images	
	for(i=0; i<m_ncp; i++)
	{
		IplImage* pPatch = extractPatch(mp1[i], pLeft, 0);
		cvSaveImage("c:\\temp\\patch1.jpg", pPatch);
		unsigned char* pbuffer = NULL;
		int ht,wd;
		IplImageToGrayImage(pPatch, &pbuffer, &ht, &wd);
		double lbpHist1[256];
		GenerateLBP(pbuffer, ht, wd, lbpHist1);
		free(pbuffer);
		cvReleaseImage(&pPatch);

		double relativeAngle = mp2[i].ori - mp1[i].ori;
		pPatch = extractPatch(mp2[i], pRight, -relativeAngle);
		cvSaveImage("c:\\temp\\patch2.jpg", pPatch);
		IplImageToGrayImage(pPatch, &pbuffer, &ht, &wd);
		double lbpHist2[256];
		GenerateLBP(pbuffer, ht, wd, lbpHist2);
		free(pbuffer);
		cvReleaseImage(&pPatch);

		//matching using LBP
		double sim = HistSimilar(lbpHist1, lbpHist2, 256, 0);
		//sim = HistSimilar(lbpHist1, lbpHist1, 256, 3);
		printf("%d  similarity: %lf \n", i, sim);
	}	
	*/

	//save the matched result
	writeSift(mp1, m_ncp, "c:\\temp\\lpt.txt");
	writeSift(mp2, m_ncp, "c:\\temp\\rpt.txt");

	//insert into match pair
	CvFont font;	
	cvInitFont( &font, CV_FONT_VECTOR0, 0.5, 0.5, 0, 1, 8 );
	for(int k=0; k<m_ncp; k++)
	{
		MyPointF tp1,tp2;
		tp1.x = mp1[k].x;
		tp1.y = mp1[k].y;
		tp2.x = mp2[k].x;
		tp2.y = mp2[k].y;
		matchPair.push_back( MatchPair( tp1, tp2) );
		
		char stmp[256];
		sprintf(stmp, "%d", k);
		cvPutText(pDispLeft,  stmp, cvPoint(tp1.x+2, tp1.y), &font, CV_RGB(255,255,255)); 		
		cvPutText(pDispRight, stmp, cvPoint(tp2.x+2, tp2.y), &font, CV_RGB(255,255,255)); 
	}

	//generate correspondence
	/*
	MyPointF*  m_pt1 = NULL;
	MyPointF*  m_pt2 = NULL;
	int m_ncp = 0;
	if(m_pt1!=NULL) free(m_pt1);
	if(m_pt2!=NULL) free(m_pt2);
	int npt = max(m_nFeat1, m_nFeat2);
	m_pt1 = (MyPointF*)malloc(sizeof(MyPointF)*npt);
	m_pt2 = (MyPointF*)malloc(sizeof(MyPointF)*npt);

	FindCorrespondence(m_pFeat1, m_nFeat1, m_pFeat2, m_nFeat2, m_pt1, m_pt2, &m_ncp);
		
	//if(fundamental_matrix!=NULL)
	//{
	//cvReleaseMat(&fundamental_matrix);
	//fundamental_matrix = NULL;
	//}
	//m_ncp = FundamentalRefine(m_pt1, m_pt2, m_ncp, &fundamental_matrix);
	//draw_features(m_pLeftImageDisp,  m_pFeat1, m_nFeat1);
	//draw_features(m_pRightImageDisp, m_pFeat2, m_nFeat2);
	//

	//draw detected sift features
	CvPoint tp1,tp2;
	int len=3;
	CvScalar color;
	int k;
	//CvFont font;	
	//cvInitFont( &font, CV_FONT_VECTOR0,0.5, 0.5, 0, 1, 8);	

	for(k=0; k<m_ncp; k++)
	{
		color = CV_RGB(255,0,0);
		tp1.x = m_pt1[k].x;
		tp1.y = m_pt1[k].y;
		tp2.x = m_pt2[k].x;
		tp2.y = m_pt2[k].y;

		matchPair.push_back( MatchPair(m_pt1[k], m_pt2[k]) );

		//cvCircle(pDispLeft, tp1, 3, CV_RGB(255,0,255), 2, 8, 0);
		//cvCircle(pDispRight, tp2, 3, CV_RGB(255,0,255), 2, 8, 0);	

		cvLine( pDispLeft, cvPoint( tp1.x-3,tp1.y), cvPoint( tp1.x+3,tp1.y),CV_RGB(255,0,255),1,8,0);
		cvLine( pDispLeft, cvPoint( tp1.x,tp1.y-3), cvPoint( tp1.x,tp1.y+3),CV_RGB(255,0,255),1,8,0);
		cvLine( pDispRight, cvPoint( tp2.x-3,tp2.y), cvPoint( tp2.x+3,tp2.y),CV_RGB(255,0,255),1,8,0);
		cvLine( pDispRight, cvPoint( tp2.x,tp2.y-3), cvPoint( tp2.x,tp2.y+3),CV_RGB(255,0,255),1,8,0);

		char stmp[256];
		sprintf(stmp, "%d", k);
		//cvPutText(m_pLeftImageDisp, stmp, cvPoint(pt1[k].x+2, pt1[k].y), &font, CV_RGB(255,0,0)); 		
		//cvPutText(m_pRightImageDisp, stmp, cvPoint(pt2[k].x+2, pt2[k].y), &font, CV_RGB(255,0,0)); 
	}
	//cvShowImage("Left",  m_pLeftImageDisp);
	//cvShowImage("Right", m_pRightImageDisp);
	free(m_pt1);
	free(m_pt2);
	*/


#ifdef _DEBUG
	cvSaveImage("c:\\temp\\left.bmp", pDispLeft);
	cvSaveImage("c:\\temp\\right.bmp", pDispRight);
#endif // _DEBUG

	cvNamedWindow("Left");
	cvNamedWindow("Right");
	cvShowImage("Left",  pDispLeft);
	cvShowImage("Right", pDispRight);

	cvReleaseImage(&pDispLeft);
	cvReleaseImage(&pDispRight);
	if(m_pFeat1!=NULL) free(m_pFeat1);
	if(m_pFeat2!=NULL) free(m_pFeat2);
	
}


//given one point, find the triangle containing it and draw the vertex 
void CProject_PGStationDlg::PtInTriangle(int x, int y)
{
	IplImage* pImage = NULL;
	int index[3];
	int i;
	int px,py;

	if(m_nCurrentIndex<0)
		return;
	if(m_pTin==NULL)
		return;

	pImage = CloneCurrentImage();
	
	DrawDelaunayTriangle(pImage, m_pTin);	
	cvDrawCircle(pImage, cvPoint(x,y), 2, CV_RGB(255,255,255),2);    
	
	if( SelectTriangle(x, y, m_pTin, index))
	{
		for(i=0; i<3; i++)
		{
			px = m_featPtVector[index[i]].x;
			py = m_featPtVector[index[i]].y;
			cvDrawCircle(pImage, cvPoint(px,py), 3, CV_RGB(255,255,255), 2);
		}
	}

	cvShowImage("Triangle", pImage);
	cvReleaseImage(&pImage);
}

void CProject_PGStationDlg::SetTabsPosition()
{	
	CWnd *pBaseWnd = GetDlgItem(IDC_TAB);
	// 
	if(pBaseWnd!=NULL)
	{	
		CRect rcTab, rcDlg;
		pBaseWnd->GetWindowRect(&rcTab);
		ScreenToClient(&rcTab);
		GetClientRect(&rcDlg);
		// 
		rcTab.right = max(rcTab.left,rcDlg.right-5);
		rcTab.bottom = max(rcTab.top,rcDlg.bottom-4);
		// 
		m_multiPaneCtrl.MoveWindow(&rcTab);
	}
}

BOOL CProject_PGStationDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);
	
	//add menu
	m_menu.LoadMenu(IDR_MENU);
	SetMenu(&m_menu);

	if (!m_toolBar.CreateEx( this,TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP|CBRS_GRIPPER|CBRS_TOOLTIPS,
		CRect(4,4,0,0)) || !m_toolBar.LoadToolBar(IDR_TOOLBAR) )
	{
		TRACE0("failed to create toolbar\n");
		return FALSE;
	}
	m_toolBar.SetButtonStyle(0,TBBS_CHECKBOX); 
	//m_toolBar.SetButtonStyle(1,TBBS_CHECKBOX);
	m_toolBar.ShowWindow(SW_SHOW);
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);


	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here


	m_fileList.InsertColumn(0,_T("File path"),LVCFMT_LEFT,400);
	m_ptList.InsertColumn(0,_T("index"),LVCFMT_CENTER,80);
	m_ptList.InsertColumn(1,_T("x"),LVCFMT_CENTER,80);
	m_ptList.InsertColumn(2,_T("y"),LVCFMT_CENTER,80);	
	m_ptList.InsertColumn(3,_T("x"),LVCFMT_CENTER,80);
	m_ptList.InsertColumn(4,_T("y"),LVCFMT_CENTER,80);	
	

	GetDlgItem(IDC_TAB)->ShowWindow(SW_HIDE);
	SetTabsPosition();   

	CFont font;
	font.CreatePointFont(85,_T("Tahoma"));
	m_multiPaneCtrl.SetFont(&font);
	m_multiPaneCtrl.SetFontSelect(&font);
	
	// 
	// 
	m_multiPaneCtrl.InstallStyle(&m_multiPaneCtrl.styleBase);
	//m_multiPaneCtrl.SetDockMarkers( MarkersLayoutA(),DockingMarkers::Params(25,true,14),61);
	// 
	//m_multiPaneCtrl.SetAbilityManager(this);
	//m_multiPaneCtrl.SetNotifyManager(this);
	// 
	//m_multiPaneCtrl.SetCursors(IDC_CURSOR1,IDC_CURSOR2,IDC_CURSOR3,IDC_CURSOR4,IDC_CURSOR5);
	//m_multiPaneCtrl.RemoveTabEnable(true);
	//m_multiPaneCtrl.DragTabEnable(true);
	//m_multiPaneCtrl.SetMinSize(CSize(20,20));
	// 
	//m_multiPaneCtrl.Update();
    
	m_bIsUsingMem = false;
	m_nCurrentIndex = -1;
	m_pCurrentImageBuffer = NULL;

	m_pTin = NULL;
	m_pEstimateFM = NULL;
	m_pLeftImage  = NULL;
	m_pRightImage = NULL;

	//retrieve the bin path
	CString appPath = AfxGetApp()->m_pszHelpFilePath;
	char* pdes;
	int index;
	strcpy(m_binPath, appPath);	
	pdes = strrchr(m_binPath, '\\');
	index = pdes-m_binPath+1;
	strcpy(m_binPath+index, "\0");

	//cvSetMouseCallback("Triangle", on_triangle_mouse,0);

	//ShowWindow(SW_MAXIMIZE);


	InitConsoleWindow();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CProject_PGStationDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}


void CProject_PGStationDlg::DrawImage()
{
	if(m_nCurrentIndex>=0)
	{
		//display the selected 		
		CRect rect;
		m_dialogDisp.GetWindowRect(&rect);
		//m_dialogDisp.Invalidate(false);

		int desht,deswd;
		int ht,wd;
		desht = rect.Height();
		deswd = rect.Width();
		ht = m_imagePointerVector[m_nCurrentIndex]->GetHt();
		wd = m_imagePointerVector[m_nCurrentIndex]->GetWd();

		if(ht>desht || wd>deswd)
		{
			double scale = max( (double)(ht)/(double)(desht), (double)(wd)/(double)(deswd)  );
			desht = ht / scale;
			deswd = wd / scale;
		}
		else
		{
			desht = ht;
			deswd = wd;
		}

		CClientDC dc(&m_dialogDisp);
		
		//erase the background
		CBrush  brush;//( RGB(128, 128, 128) );
		brush.CreateSolidBrush(RGB(0,0,0));		
		HGDIOBJ hobject =  dc.SelectObject(brush);
		Rectangle(dc.m_hDC, 0, 0, rect.Width(), rect.Height());
		dc.SelectObject(hobject);
				
		//SetBkColor(dc.m_hDC, RGB(0, 0, 0));
						
		HDC hdc = dc.m_hDC;
		DispBmp( m_imagePointerVector[m_nCurrentIndex]->GetBuffer(), 
			m_imagePointerVector[m_nCurrentIndex]->GetHt(), 
			m_imagePointerVector[m_nCurrentIndex]->GetWd(), 
			1,
			hdc, 
			desht, deswd);


		//display image using OpenCV
		if( m_imagePointerVector[m_nCurrentIndex]->GetCvArrColor()!=NULL )
		{
			cvNamedWindow("Image", 0);
			int oht,owd;
			oht = m_imagePointerVector[m_nCurrentIndex]->GetHt();
			owd = m_imagePointerVector[m_nCurrentIndex]->GetWd();
			if(m_imagePointerVector[m_nCurrentIndex]->GetHt()>480)
			{			
				owd = owd/((double)(oht)/480.0);
				oht = 480;
			}
			cvResizeWindow("Image", owd, oht);
			cvShowImage("Image", m_imagePointerVector[m_nCurrentIndex]->GetCvArrColor());

			cvSetMouseCallback("Image", on_image_mouse, this );
		}
	}	
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CProject_PGStationDlg::OnPaint()
{
	//srand( (int)(time(NULL)) );

	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
		
		//AfxMessageBox("Paint 1");
		//FILE* fp = fopen("c:\\temp\\message2.txt", "w");
		//fprintf(fp, "%d \n", rand());
		//fclose(fp);		
	}
	else
	{
		CDialog::OnPaint();
		//AfxMessageBox("Paint 2");
		//FILE* fp = fopen("c:\\temp\\message1.txt", "w");
		//fprintf(fp, "%d \n", rand());
		//fclose(fp);

		//DrawImage();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CProject_PGStationDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CProject_PGStationDlg::OnFileOpen32771()
{
	// TODO: Add your command handler code here
	int i;

	CFileDialog dlg(TRUE,NULL,NULL,
		OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_ALLOWMULTISELECT|OFN_EXPLORER,
		_T("jpg file (*.jpg)|*.jpg|bitmap file(*.bmp)|*.bmp|Tiff file(*.tif)|*.tif|IMG file(*.img)|*.img|PNG file(*.png)|*.png||"));
	
	TCHAR* szPathMulti  = new TCHAR[8*1024*1024];  //alloc memory for multiple files
	dlg.m_ofn.nMaxFile  = 8*1024*1024; 
	dlg.m_ofn.lpstrFile = szPathMulti; 
	dlg.m_ofn.lpstrFile[0] = 0; 

	m_fileVector.clear();

	if(dlg.DoModal()==IDOK)
	{
		POSITION  pos   =   dlg.GetStartPosition(); 
		int index = 0;
		//m_fileList.DeleteAllItems();

		/*
		//release image object buffer
		for(i=0; i<m_imagePointerVector.size(); i++)
		{
			delete m_imagePointerVector[i];
		}
		m_fileVector.clear();
		m_imagePointerVector.clear();
		*/
		
		//read file paths
		while(pos != NULL) 
		{ 
			//insert image filepath into the file list control
			CString filepath = dlg.GetNextPathName(pos); 			
			m_fileVector.push_back(filepath);	
		} 	
		
		//sort the filenames
		//sort(m_fileVector.begin(), m_fileVector.end());

		if(m_fileVector.size()<1)
			return;

		int currentImageSum = m_fileVector.size();
		//load images
		for( i=0; i<m_fileVector.size(); i++ )
		{
			m_fileList.InsertItem(index+currentImageSum, m_fileVector[i]);
			index++;

			//load images into memory
			//CImageParent* pImage = new CImageX();
			CImageParent* pImage = new CImageXColor();
			//CImageParent* pImage = new CImageGdal();
			char cFilePath[256];

			//strcpy(cFilePath, (LPCSTR)(filepath));
			sprintf(cFilePath, "%s", m_fileVector[i]);
			pImage->Load(cFilePath);
			m_imagePointerVector.push_back(pImage);

			printf("ht: %d  wc:\\temp%d \n", pImage->GetHt(), pImage->GetWd());
		}

		//display
		m_nCurrentIndex = 0;

		//update data in the display dialog		
		m_dialogDisp.SetImagePointVector(m_imagePointerVector);
		m_dialogDisp.SetCurrentImageIndex(0);

		DrawImage();

		//output image path into list.txt for bundler
		char listpath[256];
		strcpy(listpath, AfxGetApp()->m_pszHelpFilePath);
		FindPath(listpath);
		strcat(listpath, "\\list.txt");
		double focus = 420;      //camera focus, from calibbration
		FILE* fp = fopen(listpath, "w");
		for( i=0; i<m_fileVector.size(); i++ )
		{
			fprintf(fp, "%s %d %lf\n", m_fileVector[i], 0, focus);
		}
		fclose(fp);
	}
	printf("click menu! \n");
}

int CProject_PGStationDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	
	//create pane control
	if( m_multiPaneCtrl.Create(this,WS_CHILD | WS_VISIBLE,CRect(0,0,0,0),3001)==false ) 
		return -1;
    
	//create dialog 
	if( m_dialogDisp.Create(IDD_DIALOG_DISP, this) == 0)
		return -1;
	m_dialogDisp.SetDlgCtrlID(3009);

	//create list
	if( m_fileList.Create(WS_CHILD | LVS_REPORT,CRect(0,0,0,0),this,3002)==0 )
		return -1;
	if( m_ptList.Create(WS_CHILD | LVS_REPORT,CRect(0,0,0,0),this,3003)==0 )
		return -1;
	if( m_editOutput.Create(WS_CHILD | ES_MULTILINE,CRect(0,0,0,0),this,3007)==0 )
		return -1;

	//set style
	DWORD dwStyle = m_ptList.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;  //full select(only for report)
	dwStyle |= LVS_EX_GRIDLINES;      //grid (only for report)
	//dwStyle |= LVS_EX_CHECKBOXES;    //check box
	m_ptList.SetExtendedStyle(dwStyle); //set extended style


	//insert widget into the tab control
	MultiPaneCtrl::Tabs tabs;
	tabs.Add(m_dialogDisp, _T("Disp"), 0);
	tabs.Add(m_fileList,   _T("File List"), 1);
	tabs.Add(m_editOutput, _T("Output"), 2);
	tabs.Add(m_ptList, _T("Points List"), 3);
	    
	//tab layout setting
	if(m_multiPaneCtrl.LoadState( AfxGetApp(),_T("MultiPaneCtrl_DemoDlg"),_T("State"),&tabs,false)==false)
	{
		HPANE h1 = m_multiPaneCtrl.ConvertToLine(NULL,true);
			HPANE h2 = m_multiPaneCtrl.ConvertToLine(h1,false);
				HPANE h3 = m_multiPaneCtrl.ConvertToLine(h2,true);
					m_multiPaneCtrl.AddTab(h3,tabs[1]); //file list window
				HPANE h4 = m_multiPaneCtrl.Add(h2);
					m_multiPaneCtrl.AddTab(h4,tabs[0]); //display window
			HPANE h5 = m_multiPaneCtrl.Add(h1);
				m_multiPaneCtrl.AddTab(h5,tabs[2]);		//output window		
				m_multiPaneCtrl.AddTab(h5,tabs[3]);		//points list window		
	}
	return 0;
}

void CProject_PGStationDlg::OnDestroy()
{
	__super::OnDestroy();

	//save tab state
	m_multiPaneCtrl.SaveState(AfxGetApp(),
		_T("MultiPaneCtrl_DemoDlg"),
		_T("State"));


	//this function must be used, 
	//otherwise error will appear when closed, added by xdh, 2012.6.26
	cvDestroyAllWindows();

	for(int i=0; i<m_imagePointerVector.size(); i++)
	{
		delete m_imagePointerVector[i];
	}

	// TODO: Add your message handler code here
}


void CProject_PGStationDlg::OnPointItemchangedList(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW*   pNMListView   =   (NM_LISTVIEW*)pNMHDR;     
	*pResult   =   NULL;     

	if(pNMListView->iItem ==-1)     
		return;     

	if(pNMListView->uNewState & LVIS_SELECTED == LVIS_SELECTED)  
	{     	
		int a = pNMListView->iItem;		

		//CString sMsg;
		//sMsg.Format(_T("%d"), a);		
		//AfxMessageBox(sMsg);

		//show point on the image
		if(a>=0 && m_nCurrentIndex>=0)
		{
			if(m_featPtVector.size()>0)
			{			
				MyPointF p;
				p = m_featPtVector[a];

				/*
				//display the selected 		
				CRect rect;
				m_dialogDisp.GetWindowRect(&rect);
				//m_dialogDisp.Invalidate(false);

				int desht,deswd;
				int ht,wd;
				desht = rect.Height();
				deswd = rect.Width();
				ht = m_imagePointerVector[m_nCurrentIndex]->GetHt();
				wd = m_imagePointerVector[m_nCurrentIndex]->GetWd();

				if(ht>desht || wd>deswd)
				{
				double scale = max( (double)(ht)/(double)(desht), (double)(wd)/(double)(deswd)  );
				desht = ht / scale;
				deswd = wd / scale;
				}
				else
				{
				desht = ht;
				deswd = wd;
				}
				CClientDC dc(&m_dialogDisp);		
				HDC hdc = dc.m_hDC;
				unsigned char* pbuffer = (unsigned char*)malloc(ht*wd*sizeof(unsigned char));

				for(int j=0; j<ht*wd; j++)
				pbuffer[j] = m_imagePointerVector[m_nCurrentIndex]->GetBuffer()[j];

				DrawCrossPtToBmp(pbuffer, ht, wd, p.x, p.y, 7);
				DispBmp( pbuffer, ht, wd, 1, hdc, desht, deswd);	
				free(pbuffer);
				*/
			}

			if(m_matchPairVector.size()>0)
			{
				IplImage* pDispLeft = cvCloneImage( m_imagePointerVector[0]->GetCvArrColor() );
				IplImage* pDispRight = cvCloneImage( m_imagePointerVector[1]->GetCvArrColor() );

				MyPointF tp1,tp2;
				for(int i=0; i<m_matchPairVector.size(); i++)
				{					
					tp1 = m_matchPairVector[i].first;
					tp2 = m_matchPairVector[i].second;

					/*
					if(i==a)
					{		
						//cvLine( pDispLeft, cvPoint( tp1.x-3,tp1.y), cvPoint( tp1.x+3,tp1.y),CV_RGB(255,0,0),1,8,0);
						//cvLine( pDispLeft, cvPoint( tp1.x,tp1.y-3), cvPoint( tp1.x,tp1.y+3),CV_RGB(255,0,0),1,8,0);
						//cvLine( pDispRight, cvPoint( tp2.x-3,tp2.y), cvPoint( tp2.x+3,tp2.y),CV_RGB(255,0,0),1,8,0);
						//cvLine( pDispRight, cvPoint( tp2.x,tp2.y-3), cvPoint( tp2.x,tp2.y+3),CV_RGB(255,0,0),1,8,0);
						cvDrawCircle(pDispLeft, cvPoint( tp1.x,tp1.y), 2, CV_RGB(255,0,0),3);
						cvDrawCircle(pDispRight, cvPoint( tp2.x,tp2.y), 2, CV_RGB(255,0,0),3);
					}
					else
					*/
					{
						///cvLine( pDispLeft, cvPoint( tp1.x-3,tp1.y), cvPoint( tp1.x+3,tp1.y),CV_RGB(255,0,255),1,8,0);
						//cvLine( pDispLeft, cvPoint( tp1.x,tp1.y-3), cvPoint( tp1.x,tp1.y+3),CV_RGB(255,0,255),1,8,0);
						//cvLine( pDispRight, cvPoint( tp2.x-3,tp2.y), cvPoint( tp2.x+3,tp2.y),CV_RGB(255,0,255),1,8,0);
						//cvLine( pDispRight, cvPoint( tp2.x,tp2.y-3), cvPoint( tp2.x,tp2.y+3),CV_RGB(255,0,255),1,8,0);
						cvDrawCircle(pDispLeft, cvPoint( tp1.x,tp1.y), 2, CV_RGB(0,0,255),2);
						cvDrawCircle(pDispRight, cvPoint( tp2.x,tp2.y), 2, CV_RGB(0,0,255),2);
					}
				}

				//MyPointF tp1,tp2;
				tp1 = m_matchPairVector[a].first;
				tp2 = m_matchPairVector[a].second;
				cvDrawCircle(pDispLeft, cvPoint( tp1.x,tp1.y), 3, CV_RGB(255,0,0),2);
				cvDrawCircle(pDispRight, cvPoint( tp2.x,tp2.y), 3, CV_RGB(255,0,0),2);
				
				cvNamedWindow("Left");
				//cvResizeWindow("Left", 480, 320);
				cvShowImage("Left", pDispLeft);
				cvNamedWindow("Right");
				//cvResizeWindow("Right", 480, 320);
				cvShowImage("Right", pDispRight);

				cvReleaseImage(&pDispLeft);
				cvReleaseImage(&pDispRight);
			}
		}
	}
}


void CProject_PGStationDlg::OnItemchangedList(NMHDR* pNMHDR, LRESULT* pResult)
{

	NM_LISTVIEW*   pNMListView   =   (NM_LISTVIEW*)pNMHDR;     
	*pResult   =   NULL;     
	    
	if(pNMListView->iItem ==-1)     
	    return;     
	   
	if(pNMListView->uNewState & LVIS_SELECTED == LVIS_SELECTED)  
	{     	
		int a = pNMListView->iItem;		
		CString sMsg;
		sMsg.Format(_T("%d"), a);		
		m_nCurrentIndex = a;

		m_dialogDisp.SetCurrentImageIndex(m_nCurrentIndex);

		DrawImage();
	}
}
void CProject_PGStationDlg::OnFeatureHarris()
{
	// TODO: Add your command handler code here
	printf("harris! \n");
	
	IplImage* pImage = NULL;
	CvPoint tp1;
	int len=3;
	CvScalar color;

	double dInput;
	CDlgInput dlgInput;
	dlgInput.m_para1Name = "Feature Number: ";
	if(dlgInput.DoModal() == IDOK)
	{
		dInput = dlgInput.m_inputPara1;
	}

	int count = dInput;

	//input the paramters
	/*
	int nInputCount = 0;
	printf("input the number of detected feature points: ....\n");
	scanf("%d ", &nInputCount);
	*/

	if(m_nCurrentIndex>=0)
	{
		pImage = CloneCurrentImage();
		IplImage* pDisp = CloneCurrentImageColor();
	
		IplImage* eig = cvCreateImage( cvGetSize(pImage), 32, 1 );
		IplImage* temp = cvCreateImage( cvGetSize(pImage), 32, 1 );
		double quality = 0.01;
		double min_distance = 10;
		int win_size = 10;

		CvPoint2D32f* points = (CvPoint2D32f*)malloc(MAX_COUNT*sizeof(CvPoint2D32f));

		DWORD t1,t2;
		

		t1 = timeGetTime();
		cvGoodFeaturesToTrack( pImage, eig, temp, points, &count,
			quality, min_distance, 0, 3, 0, 0.04 );
		t2 = timeGetTime();
        
		printf("count: %d \n", count);
		printf("Time for detecting Good Features:  %d \n", t2-t1);

		//cvFindCornerSubPix( pImage, points, count,
		//	cvSize(win_size,win_size), cvSize(-1,-1),
		//	cvTermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.03));

		cvReleaseImage( &eig );
		cvReleaseImage( &temp );
   
		//display feature points
		m_featPtVector.clear();
		for(int k=0; k<count; k++)
		{
			color = CV_RGB(255,0,0);
			tp1.x = points[k].x;
			tp1.y = points[k].y;
			MyPointF p;
			p.x = points[k].x;
			p.y = points[k].y;
			m_featPtVector.push_back(p);

			cvLine( pDisp, cvPoint( tp1.x-5,tp1.y), cvPoint( tp1.x+5,tp1.y),CV_RGB(255,0,0),1,8,0);
			cvLine( pDisp, cvPoint( tp1.x,tp1.y-5), cvPoint( tp1.x,tp1.y+5),CV_RGB(255,0,0),1,8,0);			
		}
		
		//save the feature points into list control
		m_ptList.DeleteAllItems();
		for(int i=0; i<count; i++)
		{			
			CString sx,sy,sindex;			
			sindex.Format(_T("%d"), i);
			sx.Format(_T("%lf"), points[i].x);
			sy.Format(_T("%lf"), points[i].y);			
			m_ptList.InsertItem(i, sindex);
			m_ptList.SetItemText(i,1,sx);
			m_ptList.SetItemText(i,2,sy);
		}		

		cvNamedWindow("Harris");
		cvShowImage("Harris", pDisp);

#ifdef _DEBUG
		cvSaveImage("c:\\temp\\harris.png", pDisp);
#endif // _DEBUG

		cvReleaseImage(&pImage);
		cvReleaseImage(&pDisp);
		free(points);
	}
}

void CProject_PGStationDlg::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here

	//the following line must be run, or the control will not change 
	//when the size of dialog changes, added by xiedonghai, 2012.6.3
	SetTabsPosition();
}

void CProject_PGStationDlg::OnEdgeCanny()
{
	// TODO: Add your command handler code here

	if(m_nCurrentIndex>=0)
	{
		IplImage* pImage = CloneCurrentImage();
        
		if(pImage == NULL)
			return;

		cvSmooth(pImage, pImage);
		cvCanny(m_imagePointerVector[m_nCurrentIndex]->GetCvArr(), pImage, 10, 70);

		cvNamedWindow("Canny");
		//cvResizeWindow("Image", )
		cvShowImage("Canny", pImage);
		
		cvReleaseImage(&pImage);
	}
}

void CProject_PGStationDlg::OnRegisterSitf()
{
	// TODO: Add your command handler code here

	if( m_imagePointerVector.size()>1 )
	{
		CWaitCursor waitCursor;

		m_matchPairVector.clear();
		PairMatch(m_imagePointerVector[0]->GetCvArrColor(), 
			      m_imagePointerVector[1]->GetCvArrColor(),
			      m_matchPairVector);		

		//save into the match pair list
		m_ptList.DeleteAllItems();
		for(int i=0; i<m_matchPairVector.size(); i++)
		{			
			CString sx,sy,sindex;			
			MyPointF lp,rp;

			sindex.Format(_T("%d"), i);
			m_ptList.InsertItem(i, sindex);

			lp = m_matchPairVector[i].first;
			rp = m_matchPairVector[i].second;

			sx.Format(_T("%lf"), lp.x);
			sy.Format(_T("%lf"), lp.y);		
			m_ptList.SetItemText(i,1,sx);
			m_ptList.SetItemText(i,2,sy);

			sx.Format(_T("%lf"), rp.x);
			sy.Format(_T("%lf"), rp.y);		
			m_ptList.SetItemText(i,3,sx);
			m_ptList.SetItemText(i,4,sy);
		}
		printf("Matching! \n");
	}
	else
	{
		AfxMessageBox("Number of Images is less than 2 !");
	}
}


//detect feature points using SIFT
void CProject_PGStationDlg::OnFeatureSift()
{
	// TODO: Add your command handler code here

	MyPointF pt;
	IplImage* pImage = NULL;
	CvScalar color;
	CvPoint tp;

	if(m_nCurrentIndex>=0)
	{
		struct feature* pFeat = NULL;
		int nFeat;
		char   filepath[256];

		pImage = CloneCurrentImage();//cvCloneImage(m_imagePointerVector[m_nCurrentIndex]->GetCvArr());
		
		nFeat = sift_features(pImage, &pFeat);

		//save the feature information as file
		strcpy(filepath, m_imagePointerVector[m_nCurrentIndex]->GetFilePath());
		strcat(filepath, ".key");
		writeSift(pFeat, nFeat, filepath);

		//insert the point info into the list
		m_featPtVector.clear();
		m_ptList.DeleteAllItems();
		for(int i=0; i<nFeat; i++)
		{			
			CString sx,sy,sindex;			
			sindex.Format(_T("%d"), i);
			sx.Format(_T("%lf"), pFeat[i].x);
			sy.Format(_T("%lf"), pFeat[i].y);			
			m_ptList.InsertItem(i, sindex);
			m_ptList.SetItemText(i,1,sx);
			m_ptList.SetItemText(i,2,sy);

			pt.x = pFeat[i].x;
			pt.y = pFeat[i].y;
			m_featPtVector.push_back(pt);
		}

		//save the feature points
		//display feature points, only position
		IplImage* pColor = CloneCurrentImageColor();
		for(int k=0; k<nFeat; k++)
		{
			color = CV_RGB(255,0,0);
			tp.x = pFeat[k].x;
			tp.y = pFeat[k].y;
			cvLine( pColor, cvPoint( tp.x-5,tp.y), cvPoint( tp.x+5,tp.y),CV_RGB(255,0,255),1,8,0);
			cvLine( pColor, cvPoint( tp.x,tp.y-5), cvPoint( tp.x,tp.y+5),CV_RGB(255,0,255),1,8,0);			
		}
	
		//draw sift feature, including orientation, scale, position
		draw_features(pColor, pFeat, nFeat);

		//save into image
		printf("save into the file: c:\\temp\sift_byte.bmp \n");
		cvSaveImage("c:\\temp\\sift_byte.bmp", pColor);

		cvNamedWindow("SIFT");
		cvMoveWindow("SIFT", 100, 100);
		//cvResizeWindow("SIFT", 640, 640);
		cvShowImage("SIFT", pColor);

		printf("%d \n", nFeat);
		free(pFeat);
		cvReleaseImage(&pImage);
		cvReleaseImage(&pColor);
	}
}


void CProject_PGStationDlg::OnSuperresolutionRegistertransform()
{
	// TODO: Add your command handler code here
    
	if( m_imagePointerVector.size()>1 )
	{
		CWaitCursor waitCursor;
		
		m_matchPairVector.clear();
		PairMatch(m_imagePointerVector[0]->GetCvArr(), 
			m_imagePointerVector[1]->GetCvArr(),
			m_matchPairVector);		

		//save into the match pair list
		m_ptList.DeleteAllItems();
		for(int i=0; i<m_matchPairVector.size(); i++)
		{			
			CString sx,sy,sindex;			
			MyPointF lp,rp;

			sindex.Format(_T("%d"), i);
			m_ptList.InsertItem(i, sindex);
  
			lp = m_matchPairVector[i].first;
			rp = m_matchPairVector[i].second;
			
			sx.Format(_T("%lf"), lp.x);
			sy.Format(_T("%lf"), lp.y);		
			m_ptList.SetItemText(i,1,sx);
			m_ptList.SetItemText(i,2,sy);

			sx.Format(_T("%lf"), rp.x);
			sy.Format(_T("%lf"), rp.y);		
			m_ptList.SetItemText(i,3,sx);
			m_ptList.SetItemText(i,4,sy);
		}
		printf("Matching! \n");
	}
	else
	{
		AfxMessageBox("Number of Images is less than 2 !");
	}
}


void CProject_PGStationDlg::OnSuperresolutionOpticalflow()
{
	// TODO: Add your command handler code here

	IplImage* pFirst = NULL;
	IplImage* pSecond = NULL;
	IplImage *curPYR = 0;
	IplImage *nextPYR = 0;
	CvPoint tp1;
	MyPointF pt;
	//int len=3;
	CvScalar color;
	int ht,wd,scanwd;
	int Level = 2;
	int k = 0;
	int i,j;
	double* px;
	double* py;		
	int     npt;	
	int     index[3];
    double  tx,ty;
	MyPointF tpt[3];
	double len[3];
	double weight[3];
	MyPointF tp;
	double alllen;			
	DWORD t1,t2;
   
	
	if(m_imagePointerVector.size()>1)
	{
		pFirst = cvCloneImage(m_imagePointerVector[0]->GetCvArr() );
		pSecond = cvCloneImage(m_imagePointerVector[1]->GetCvArr() );

		ht = pFirst->height;
		wd = pFirst->width;
		scanwd = pFirst->widthStep;

		curPYR  = cvCreateImage( cvSize(wd, ht), 8, 1 );
		nextPYR = cvCreateImage( cvSize(wd, ht), 8, 1 );

		//1. find good features (harris feature points)
		IplImage* eig = cvCreateImage( cvGetSize(pFirst), 32, 1 );
		IplImage* temp = cvCreateImage( cvGetSize(pFirst), 32, 1 );
		double quality = 0.01;
		double min_distance = 10;
		int win_size = 10;
		CvPoint2D32f* points = (CvPoint2D32f*)malloc(MAX_COUNT*sizeof(CvPoint2D32f));
		CvPoint2D32f* npoints = (CvPoint2D32f*)malloc(MAX_COUNT*sizeof(CvPoint2D32f));        
		int count = MAX_COUNT;
		cvGoodFeaturesToTrack( pFirst, eig, temp, points, &count,
			quality, min_distance, 0, 3, 0, 0.04 );
		//cvFindCornerSubPix( pImage, points, count,
		//	cvSize(win_size,win_size), cvSize(-1,-1),
		//	cvTermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.03));
		//save the feature points from bench image 
		m_featPtVector.clear();
		for( k=0; k<count; k++)
		{			
			pt.x = points[k].x;
			pt.y = points[k].y;
			m_featPtVector.push_back(pt);
		}
		cvReleaseImage( &eig );
		cvReleaseImage( &temp );



		t1 = timeGetTime();
		//2. calculate optical flow
		char* status = (char*)malloc(count);
		cvCalcOpticalFlowPyrLK( pFirst, pSecond, curPYR, nextPYR, 
			points, npoints, 
			count, 
			cvSize(10,10), 
			Level, 
			status, 0, 
			cvTermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.03),
			0);
		free(status);
		t2 = timeGetTime();
		
		printf("Time for Optical Flow: %d \n", t2-t1);
		
		//calculate the mean translation		
		tx = 0;
		ty = 0;
		for(k=0; k<count; k++)
		{
			tx += (points[k].x - npoints[k].x);
			ty += (points[k].y - npoints[k].y);
		}
		tx /= (double)(count);
		ty /= (double)(count);

		//save the sparse optical flow 
		char sopticalFlow[256];
		strcpy(sopticalFlow, m_binPath);
		strcat(sopticalFlow, "opticalFlow.txt");
		FILE* fp = fopen(sopticalFlow, "w");
		fprintf(fp, "%d \n", count);
		for(k=0; k<count; k++)
		{
			fprintf(fp, "%lf %lf %lf %lf \n", points[k].x, points[k].y, npoints[k].x,npoints[k].y); 
		}
		fclose(fp);

		CString sout;
		sout.Format(_T("mean translation: %lf %lf \n"), tx, ty);
		m_editOutput.Clear();
		m_editOutput.SetWindowText(sout);
		// display feature points
		for( k=0; k<count; k++)
		{
			color = CV_RGB(255,0,0);
			tp1.x = points[k].x;
			tp1.y = points[k].y;
			cvLine( pFirst, cvPoint( tp1.x-3,tp1.y), cvPoint( tp1.x+3,tp1.y),CV_RGB(255,0,255),1,8,0);
			cvLine( pFirst, cvPoint( tp1.x,tp1.y-3), cvPoint( tp1.x,tp1.y+3),CV_RGB(255,0,255),1,8,0);			
		
			tp1.x = npoints[k].x;
			tp1.y = npoints[k].y;
			cvLine( pSecond, cvPoint( tp1.x-3,tp1.y), cvPoint( tp1.x+3,tp1.y),CV_RGB(255,0,255),1,8,0);
			cvLine( pSecond, cvPoint( tp1.x,tp1.y-3), cvPoint( tp1.x,tp1.y+3),CV_RGB(255,0,255),1,8,0);
			cvLine( pFirst, cvPoint( tp1.x-3,tp1.y), cvPoint( tp1.x+3,tp1.y),CV_RGB(0,0,0),1,8,0);
			cvLine( pFirst, cvPoint( tp1.x,tp1.y-3), cvPoint( tp1.x,tp1.y+3),CV_RGB(0,0,0),1,8,0);					
		}
		//save the feature points into list control
		m_ptList.DeleteAllItems();
		for(int i=0; i<count; i++)
		{			
			CString sx,sy,sindex;			
			sindex.Format(_T("%d"), i);
			
			m_ptList.InsertItem(i, sindex);

			sx.Format(_T("%lf"), points[i].x);
			sy.Format(_T("%lf"), points[i].y);						
			m_ptList.SetItemText(i,1,sx);
			m_ptList.SetItemText(i,2,sy);

			sx.Format(_T("%lf"), npoints[i].x);
			sy.Format(_T("%lf"), npoints[i].y);						
			m_ptList.SetItemText(i,3,sx);
			m_ptList.SetItemText(i,4,sy);
		}

		cvNamedWindow("First");
		cvShowImage("First", pFirst);
		cvNamedWindow("Second");
		cvShowImage("Second", pSecond);

		cvReleaseImage(&pFirst);
		cvReleaseImage(&pSecond);
		cvReleaseImage(&curPYR);
		cvReleaseImage(&nextPYR);

		free(points);
		free(npoints);
	}
}

//generate Delaunay triangle
void CProject_PGStationDlg::OnGeometryTriangle()
{
	// TODO: Add your command handler code here
	if(m_nCurrentIndex>=0)
	{
		int     npt;
		double* px;
		double* py;
		int     i;
		
		npt = m_featPtVector.size();

		if(npt<1)
			return;

		px = (double*)malloc(npt*sizeof(double));
		py = (double*)malloc(npt*sizeof(double));

		for(i=0; i<npt; i++)
		{
			px[i] = m_featPtVector[i].x;
			py[i] = m_featPtVector[i].y;
		}

		if(m_pTin!=NULL)
			delete m_pTin;

		m_pTin = new CTINClass("aaa");
		GenerateDelaunayTriangle(m_pTin, px, py, npt);

		//IplImage* pImage = cvCloneImage(m_imagePointerVector[m_nCurrentIndex]->GetCvArr());
		IplImage* pImage = CloneCurrentImageColor();

		DrawDelaunayTriangle(pImage, m_pTin);

		cvNamedWindow("Triangle");
		cvSetMouseCallback("Triangle", on_triangle_mouse, this);
		cvShowImage("Triangle", pImage);

		cvSaveImage("c:\\temp\\triangle.png", pImage);

		cvReleaseImage(&pImage);    
		free(px);
		free(py);
	}
	
}

void CProject_PGStationDlg::OnSimulationTranslation()
{
	// TODO: Add your command handler code here

	IplImage* pImage = NULL;
	IplImage* pSimImage = NULL;	
	double dTx = 0; //0.5
	double dTy = 1; //0.3
	int    ht, wd;
	int    scanwd;
	int    i,j;
	double tx,ty;

	//simulate subpixel translation image
	if(m_nCurrentIndex>=0)
	{
		//pImage = CloneCurrentImage();
		pImage = CloneCurrentImageColor();

		if( pImage->nChannels==1 )
		{
			pSimImage = cvCreateImage(cvGetSize(pImage), 8, 1);

			ht = pImage->height;
			wd = pImage->width;
			scanwd = pImage->widthStep;

			for(j=0; j<ht; j++)
				for(i=0; i<wd; i++)
				{
					tx = i + dTx;
					ty = j + dTy;

					if(tx>=0 && tx<wd-1 && ty>=0 && ty<ht-1)
					{
						pSimImage->imageData[j*scanwd+i] = BilinearPixel(tx, ty, pImage);
					}
					else
						pSimImage->imageData[j*scanwd+i] = 0;
				}    
			
			cvSaveImage("c:\\temp\\simulateImage.jpg", pSimImage);
			cvReleaseImage(&pSimImage);
		}

		if( pImage->nChannels==3 )
		{
			IplImage* pR = cvCreateImage(cvGetSize(pImage), 8, 1);
			IplImage* pG = cvCreateImage(cvGetSize(pImage), 8, 1);
			IplImage* pB = cvCreateImage(cvGetSize(pImage), 8, 1);

			IplImageSplit(pImage, pR, pG, pB);

			pSimImage = cvCreateImage(cvGetSize(pImage), 8, 3);

			ht = pImage->height;
			wd = pImage->width;
			scanwd = pImage->widthStep;

			for(j=0; j<ht; j++)
				for(i=0; i<wd; i++)
				{
					tx = i + dTx;
					ty = j + dTy;

					if(tx>=0 && tx<wd-1 && ty>=0 && ty<ht-1)
					{
						pSimImage->imageData[j*scanwd+3*i]   = BilinearPixel(tx, ty, pR);
						pSimImage->imageData[j*scanwd+3*i+1] = BilinearPixel(tx, ty, pG);
						pSimImage->imageData[j*scanwd+3*i+2] = BilinearPixel(tx, ty, pB);
					}
					else
					{
						pSimImage->imageData[j*scanwd+3*i] = 0;
						pSimImage->imageData[j*scanwd+3*i+1] =0;
						pSimImage->imageData[j*scanwd+3*i+2] = 0;
					}
				}    
	
			cvReleaseImage(&pR);
			cvReleaseImage(&pG);
			cvReleaseImage(&pB);


					
			cvSaveImage("c:\\temp\\simulateImage.jpg", pSimImage);
			cvReleaseImage(&pSimImage);
		}
		
		cvReleaseImage(&pImage);
		AfxMessageBox("Simulated Image is saved in 'c:\\temp\\simulateImage.jpg' ");		
	}
}

void CProject_PGStationDlg::GenerateGrid(IplImage* pSecond)
{
	// TODO: Add your command handler code here
	int i,j,k;
	double* px;
	double* py;
	MyPointF* points = NULL;
	MyPointF* npoints = NULL;
	int npt;
	FILE* fp;
	char sopticalFlow[256];
	int ht, wd;
	int scanwd;
	double  tx,ty;	
	MyPointF tp;
	double   alllen;		
	int      index[3];
	double   len[3];
	double   weight[3];
	MyPointF tpt[3];
	double   tlx[3],tly[3];

	ht = pSecond->height;
	wd = pSecond->width;
	scanwd = pSecond->widthStep;

	CWaitCursor wt;

	strcpy(sopticalFlow, m_binPath);
	strcat(sopticalFlow, "opticalFlow.txt");

	//read optical flow of two images from file
	fp = fopen(sopticalFlow, "r");
	fscanf(fp, "%d ", &npt);
	points = (MyPointF*)malloc(sizeof(MyPointF)*npt);
	npoints = (MyPointF*)malloc(sizeof(MyPointF)*npt);
	for(k=0; k<npt; k++)
	{
		fscanf(fp, "%lf %lf %lf %lf ", &(points[k].x), &(points[k].y), 
			&(npoints[k].x), &(npoints[k].y) ); 
	}
	fclose(fp);

	//mean translation
	tx = 0;
	ty = 0;
	for(k=0; k<npt; k++)
	{
		tx += (points[k].x - npoints[k].x);
		ty += (points[k].y - npoints[k].y);
	}
	tx /= (double)(npt);
	ty /= (double)(npt);

	// calculate the translation of every pixel	
	if(m_pTin!=NULL)
		delete m_pTin;
	m_pTin = new CTINClass("aaa");

	px = (double*)malloc(npt*sizeof(double));	
	py = (double*)malloc(npt*sizeof(double));		
	for(i=0; i<npt; i++)
	{
		px[i] = points[i].x;
		py[i] = points[i].y;
	}
	GenerateDelaunayTriangle(m_pTin, px, py, npt);

	//save the pixel value according to the coordinates of bench image
	char pixelPath[256];
	strcpy(pixelPath, m_binPath);
	strcat(pixelPath, "pixel.txt");

	fp = fopen(pixelPath, "a+");
	for(j=0; j<ht; j++)
		for(i=0; i<wd; i++)
		{
			double ftx,fty;
			ftx = 0;
			fty = 0;

			if( SelectTriangle(i,j,m_pTin,index))
			{
				//calculate the translation
				for(k=0; k<3; k++)
				{
					tpt[k].x = px[index[k]];
					tpt[k].y = py[index[k]];						
					tlx[k] = points[ index[k] ].x - npoints[ index[k] ].x;
					tly[k] = points[ index[k] ].y - npoints[ index[k] ].y;
				}

				//translation interpolation							
				tp.x = i;
				tp.y = j;
				for(k=0; k<3; k++)
				{						
					len[k] = P2PDistance(tp, tpt[k]);
				}
				for(k=0; k<3; k++)
				{
					weight[k] = 1.0 / (len[k]+0.000001);
				}
				ArrayNorm(weight, 3);					

				ftx = 0;
				fty = 0;
				for(k=0; k<3; k++)
				{
					ftx += tlx[k]*weight[k];
					fty += tly[k]*weight[k];
				}
				
			}
			else
			{
				ftx = tx;
				fty = ty;
			}				
			if(pSecond->nChannels==1)
			{
				fprintf(fp, "%lf %lf %d \n", i+ftx, j+fty, (unsigned char)(pSecond->imageData[j*scanwd+i]) );
			}
			if(pSecond->nChannels==3)
			{
				fprintf(fp, "%lf %lf %d %d %d \n", i+ftx, j+fty, 
					(unsigned char)(pSecond->imageData[j*scanwd+3*i]),
					(unsigned char)(pSecond->imageData[j*scanwd+3*i+1]),
					(unsigned char)(pSecond->imageData[j*scanwd+3*i+2]));
			}
			
			GiveOutCtrl();

			CString sout;
			//sout.Format(_T("%d %d"), i,j);
			sout.Format(_T("%d "), j);
			m_editOutput.Clear();
			m_editOutput.SetWindowText(sout);
		}
		fclose(fp);

		free(px);
		free(py);
		free(points);
		free(npoints);
}

void CProject_PGStationDlg::OnSuperresolutionGenerategrids()
{
	// TODO: Add your command handler code here
	int i,j,k;
	double* px;
	double* py;
	MyPointF* points = NULL;
	MyPointF* npoints = NULL;
    int npt;
	FILE* fp;
	char sopticalFlow[256];
    IplImage* pFirst;
	IplImage* pSecond;
	int ht, wd;
	int scanwd;
	double  tx,ty;	
	MyPointF tp;
	double   alllen;		
	int      index[3];
	double   len[3];
	double   weight[3];
	MyPointF tpt[3];
	double   tlx[3],tly[3];

	if(m_imagePointerVector.size()>1)
	{
		//pImage = m_imagePointerVector[m_nCurrentIndex]->GetCvArr();
		pFirst  = cvCloneImage(m_imagePointerVector[0]->GetCvArr());
		pSecond = cvCloneImage(m_imagePointerVector[1]->GetCvArr());

		ht = pFirst->height;
		wd = pFirst->width;
		scanwd = pFirst->widthStep;
	}
	else
		return;

	CWaitCursor wt;

	strcpy(sopticalFlow, m_binPath);
	strcat(sopticalFlow, "opticalFlow.txt");
	
	//read optical flow of two images from file
	fp = fopen(sopticalFlow, "r");
	fscanf(fp, "%d ", &npt);
	points = (MyPointF*)malloc(sizeof(MyPointF)*npt);
	npoints = (MyPointF*)malloc(sizeof(MyPointF)*npt);
	for(k=0; k<npt; k++)
	{
		fscanf(fp, "%lf %lf %lf %lf ", &(points[k].x), &(points[k].y), 
			&(npoints[k].x), &(npoints[k].y) ); 
	}
	fclose(fp);

	//mean translation
	tx = 0;
	ty = 0;
	for(k=0; k<npt; k++)
	{
		tx += (points[k].x - npoints[k].x);
		ty += (points[k].y - npoints[k].y);
	}
	tx /= (double)(npt);
	ty /= (double)(npt);

	// calculate the translation of every pixel	
	if(m_pTin!=NULL)
		delete m_pTin;
	m_pTin = new CTINClass("aaa");
	
	px = (double*)malloc(npt*sizeof(double));	
	py = (double*)malloc(npt*sizeof(double));		
	for(i=0; i<npt; i++)
	{
		px[i] = points[i].x;
		py[i] = points[i].y;
	}
	GenerateDelaunayTriangle(m_pTin, px, py, npt);

	//save the pixel value according to the coordinates of bench image
	char pixelPath[256];
	strcpy(pixelPath, m_binPath);
	strcat(pixelPath, "pixel.txt");

	fp = fopen(pixelPath, "a+");
	for(j=0; j<ht; j++)
		for(i=0; i<wd; i++)
		{
			double ftx,fty;

			if( SelectTriangle(i,j,m_pTin,index))
			{
				//calculate the translation
				for(k=0; k<3; k++)
				{
					tpt[k].x = px[index[k]];
					tpt[k].y = py[index[k]];						
					tlx[k] = points[ index[k] ].x - npoints[ index[k] ].x;
					tly[k] = points[ index[k] ].y - npoints[ index[k] ].y;
				}

				//translation interpolation							
				tp.x = i;
				tp.y = j;
				for(k=0; k<3; k++)
				{						
					len[k] = P2PDistance(tp, tpt[k]);
				}
				for(k=0; k<3; k++)
				{
					weight[k] = 1.0 / (len[k]+0.000001);
				}
				ArrayNorm(weight, 3);					

				ftx = 0;
				fty = 0;
				for(k=0; k<3; k++)
				{
					ftx += tlx[k]*weight[k];
					fty += tly[k]*weight[k];
				}
			}
			else
			{
				ftx = tx;
				fty = ty;
			}				
			fprintf(fp, "%lf %lf %d \n", i+ftx, j+fty, (unsigned char)(pSecond->imageData[j*scanwd+i]) );

			GiveOutCtrl();
			
			CString sout;
			//sout.Format(_T("%d %d"), i,j);
			sout.Format(_T("%d "), j);
			m_editOutput.Clear();
			m_editOutput.SetWindowText(sout);
		}
		fclose(fp);

		free(px);
		free(py);
		free(points);
		free(npoints);
		cvReleaseImage(&pFirst);
		cvReleaseImage(&pSecond);
}

void CProject_PGStationDlg::OnBundlerGeneratefeature()
{
	// TODO: Add your command handler code here

	//generate feature of each file and save it into file,
	//added by xiedonghai, 2012.6.26
	struct feature* pFeat = NULL;
	int nFeat;
	char   filepath[256];
    IplImage* pImage = NULL;
	int i;

	CWaitCursor wt;

	//generate sift feature points from each image and save 
	if( m_imagePointerVector.size()>0)
	{
		for(i=0; i<m_imagePointerVector.size(); i++ )
		{
			printf("Detecting feature points from %s ... \n", m_imagePointerVector[i]->GetFilePath());

			pImage = cvCloneImage(m_imagePointerVector[i]->GetCvArr());


			//resize images
			

			nFeat = sift_features(pImage, &pFeat);

			//save the feature information as file
			strcpy(filepath, m_imagePointerVector[i]->GetFilePath());
			char* pdes = strrchr(filepath, '.');
			int   index = pdes - filepath;
			strcpy(filepath+index, "\0");
			strcat(filepath, ".key");

			//writeSift(pFeat, nFeat, filepath);
			writeSiftForBundler(pFeat, nFeat, filepath);

			GiveOutCtrl();

			CString sout;
			sout.Format(_T("%d "), i);
			m_editOutput.Clear();
			m_editOutput.SetWindowText(sout);

			cvReleaseImage(&pImage);
			free(pFeat);
		}
		printf("Detecting Features Finished! \n");
	}
	else
	{
		AfxMessageBox("No Images !");
	}
}

void CProject_PGStationDlg::OnBundlerMatching()
{
	// TODO: Add your command handler code here

	int i;
	char binpath[256];
	char filepath[256];
	char matchfile[256];
	int  num_images;
	unsigned char** keys = NULL;
	int* num_keys = NULL;
	FILE* fp = NULL;

	CWaitCursor wt;

	num_images = m_imagePointerVector.size();

	//read .key files and do initial matching
	if( num_images>0 )
	{
		keys = new unsigned char *[num_images];
		num_keys = new int[num_images];

		bool bIsKeysLoad = true;
		for(i=0; i<num_images; i++ )
		{
			//save the feature information as file
			strcpy(filepath, m_imagePointerVector[i]->GetFilePath());
			char* pdes = strrchr(filepath, '.');
			int   index = pdes - filepath;
			strcpy(filepath+index, "\0");
			strcat(filepath, ".key");
			
			printf("Loading %s \n", filepath);
			/*fp = fopen(filepath, "r");
			if(fp==NULL)
			{				
				printf("Can not Loading %s \n", filepath);
				bIsKeysLoad = false;
			}
			*/
			num_keys[i] = ReadKeys(filepath, keys+i);	
			//num_keys[i] = ReadSiftKeys(fp, keys+i);
			fclose(fp);
		}

		//matching and save
		if( bIsKeysLoad )
		{
			strcpy(binpath, AfxGetApp()->m_pszHelpFilePath);
			FindPath(binpath);
			strcpy(matchfile, binpath);
			strcat(matchfile, "\\matches.init.txt");

			printf("Matching ... \n");
			MathcingUsingSiftKey(num_images, keys, num_keys, matchfile);
			printf("Matching Finished ... \n");
		}
		else
		{
			printf("Please generate feature points firstly! \n");			
		}

		/* Free keypoints */
		for (int i = 0; i < num_images; i++)
		{
			if (keys[i] != NULL)
				delete[] keys[i];
		}
		delete[] keys;
		delete[] num_keys;	
	}
	else
	{
		AfxMessageBox("No Images !");
	}
}

void CProject_PGStationDlg::OnBundlerGeneratetracks()
{
	// TODO: Add your command handler code here	
	// generate tracks, track corresponds to the ground points, which 
	// have projections in several images	

	//char binpath[256];
	//strcpy(binpath, AfxGetApp()->m_pszHelpFilePath);
	//char* pdes = strrchr(binpath, '\\');
	//int index = pdes-binpath;
	//strcpy(binpath+index, "\0");

	////load image and key data
	//char imageList[256];
	//sprintf(imageList, "%s\\list.txt", binpath);
	//LoadImageList(imageList);

	////load match result
	//char cMatchInit[256];
	//sprintf(cMatchInit, "%s\\matches.init.txt", binpath);
 //   LoadMatchTable(cMatchInit);

	////generate tracks
	//char cTracks[256];
	//sprintf(cTracks, "%s\\bundle\\constraints.txt", binpath);
	//GenerateTracks(cTracks);
}

void CProject_PGStationDlg::OnBundlerBundlerSrc()
{
	// TODO: Add your command handler code here

	char binpath[256];
	strcpy(binpath, AfxGetApp()->m_pszHelpFilePath);
	char* pdes = strrchr(binpath, '\\');
	int index = pdes-binpath;
	strcpy(binpath+index, "\0");

	//BundleInitPath(binpath);
	//RunBundle();
}

void CProject_PGStationDlg::OnBundlerBundler()
{
	// TODO: Add your command handler code here

	//invoke bundler.exe
	char binpath[256];
	strcpy(binpath, AfxGetApp()->m_pszHelpFilePath);
	char* pdes = strrchr(binpath, '\\');
	int index = pdes-binpath;
	strcpy(binpath+index, "\0");
	
	SetCurrentDirectory(binpath);

	char bundlerExe[256];
	sprintf(bundlerExe, "%s\\bundler.exe", binpath);
      
	char exeString[256];
	sprintf(exeString, "%s list.txt --options_file options.txt", bundlerExe);
	system(exeString);
}

void CProject_PGStationDlg::OnBundlerCalibration()
{
	// TODO: Add your command handler code here	
}

void CProject_PGStationDlg::OnBundlerMosaic()
{
	// TODO: Add your command handler code here
	printf("Bundler Mosaic ... \n");  

	char binpath[256];
	strcpy(binpath, AfxGetApp()->m_pszHelpFilePath);
	char* pdes = strrchr(binpath, '\\');
	int index = pdes-binpath;
	strcpy(binpath+index, "\0");

	//load image and key data
	char imageList[256];
	sprintf(imageList, "%s\\list.txt", binpath);
    
	char bundlefile[256];
	sprintf(bundlefile, "%s\\bundle\\bundle.out", binpath);
    
	if( m_imagePointerVector.size()>0 )
	{
		int ht = m_imagePointerVector[0]->GetHt();
		int wd = m_imagePointerVector[0]->GetWd();
	
		BundleMosaic(imageList, bundlefile, ht, wd);
	}
}

void CProject_PGStationDlg::OnEdgeSobel()
{
	// TODO: Add your command handler code here
}

void CProject_PGStationDlg::OnToolSmooth()
{
	// TODO: Add your command handler code here
	IplImage* pImage = NULL;

	if(m_nCurrentIndex>=0)
	{
		//pImage = m_imagePointerVector[m_nCurrentIndex]->GetCvArr();
		pImage = CloneCurrentImage();//cvCloneImage(m_imagePointerVector[m_nCurrentIndex]->GetCvArr());

		cvSmooth(pImage, pImage, CV_GAUSSIAN, 5);

		//SetImageBuffer(pImage);

		cvNamedWindow("Smooth");
		//cvResizeWindow("Image", )
		cvShowImage("Smooth", pImage);
		
		cvReleaseImage(&pImage);
	}
}

void CProject_PGStationDlg::OnSobelX()
{
	// TODO: Add your command handler code here
	IplImage* pImage = NULL;

	if(m_nCurrentIndex>=0)
	{
		//pImage = m_imagePointerVector[m_nCurrentIndex]->GetCvArr();
		pImage = CloneCurrentImage(); //cvCloneImage(m_imagePointerVector[m_nCurrentIndex]->GetCvArr());

		//cvCanny(m_imagePointerVector[m_nCurrentIndex]->GetCvArr(), pImage, 10, 100);
		//cvSmooth(m_imagePointerVector[m_nCurrentIndex]->GetCvArr(), pImage);
		//cvSobel(pImage, pImage, 0, 1);
		
		DWORD t1 = timeGetTime();
		
		cvSobel(m_imagePointerVector[m_nCurrentIndex]->GetCvArr(), pImage, 1, 0);

		DWORD t2 = timeGetTime();

		printf("Time: %d \n", t2-t1);

		SetImageBuffer(pImage);

		cvNamedWindow("Sobel-x");
		cvShowImage("Sobel-x", pImage);

		cvReleaseImage(&pImage);
	}
}

void CProject_PGStationDlg::OnSobelY()
{
	// TODO: Add your command handler code here
	IplImage* pImage = NULL;

	if(m_nCurrentIndex>=0)
	{
		//pImage = m_imagePointerVector[m_nCurrentIndex]->GetCvArr();
		pImage = cvCloneImage(m_imagePointerVector[m_nCurrentIndex]->GetCvArr());

		//cvCanny(m_imagePointerVector[m_nCurrentIndex]->GetCvArr(), pImage, 10, 100);
		//cvSmooth(m_imagePointerVector[m_nCurrentIndex]->GetCvArr(), pImage);
		//cvSobel(pImage, pImage, 0, 1);
		cvSobel(m_imagePointerVector[m_nCurrentIndex]->GetCvArr(), pImage, 0, 1);
		
		SetImageBuffer(pImage);

		cvNamedWindow("Sobel-y");
		cvShowImage("Sobel-y", pImage);
		cvReleaseImage(&pImage);
	}
}

BOOL CProject_PGStationDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

	//return false;
	//Invalidate(true);    	
	
	//OnPaint();

	return __super::OnEraseBkgnd(pDC);
}

void CProject_PGStationDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	__super::OnShowWindow(bShow, nStatus);
	// TODO: Add your message handler code here
}

void CProject_PGStationDlg::OnSetFocus(CWnd* pOldWnd)
{
	__super::OnSetFocus(pOldWnd);
	//Invalidate(false);
	// TODO: Add your message handler code here
}


//character segmentation based on connceted component analysis
int CharacterSeg(IplImage* pImage)
{
	int ht = pImage->height;
	int wd = pImage->width;
	int scanwd = pImage->widthStep;
	unsigned char* mask = (unsigned char*)malloc(ht*wd);
	for(int j=0; j<ht; j++)
		for(int i=0; i<wd; i++)
		{
			mask[j*wd+i] = pImage->imageData[j*scanwd+i];
		}

	MyRect* pRect = (MyRect*)malloc(512*sizeof(MyRect));
	int  nRect = 0;
	SegLabel(mask, ht, wd, pRect, &nRect);
	int index = 0;
	for(int i=0; i<nRect; i++)
	{
		double ratio = (double)(pRect[i].bottom-pRect[i].top) / (double)(pRect[i].right-pRect[i].left);  
		if( ratio>1 && ratio<3 )
			index++;
	}
	free(pRect);	

	free(mask);

	return index;
}

//deal with each candidate crop image 
int CropDeal(MyRect rect, IplImage* pImage, int ri)
{	
	//shape filter	
	double h,w,ratio;
	w = rect.right - rect.left;
	h = rect.bottom - rect.top;
	ratio = w/h;

	//detect characters
	CvRect r;
	r.x = rect.left - w/4;
	r.y = rect.top - h/4;
	r.width = w*1.5;
	r.height = h*1.5;			
	unsigned char* pbuffer = (unsigned char*)malloc(r.height*r.width);
	IplImage* pCrop = cvCreateImage( cvSize(r.width, r.height), 8, 1);
	int index = 0;
	for(int j=r.y; j<(r.y+r.height); j++)
		for(int i=r.x; i<(r.x+r.width); i++)
		{
			pbuffer[index] = (unsigned char)(pImage->imageData[j*pImage->widthStep+i]);
			pCrop->imageData[ (j-r.y)*pCrop->widthStep+(i-r.x)] = 
				(unsigned char)(pImage->imageData[j*pImage->widthStep+i]);

			index++;					
		}		

	int threshold = Otsu(pbuffer, r.height, r.width);
	free(pbuffer);
	
	//char cropfile[256];
	//sprintf(cropfile, "c:\\temp\\crop_%d.jpg", ri);
	//cvSaveImage(cropfile, pCrop);	
	IplImage* pI1 = cvCloneImage(pCrop);
	IplImage* pI2 = cvCloneImage(pCrop);
	cvThreshold(pCrop, pI1, threshold, 255, CV_THRESH_BINARY);
	cvThreshold(pCrop, pI1, threshold, 255, CV_THRESH_BINARY_INV);
	//sprintf(cropfile, "c:\\temp\\cropseg_%d.jpg", ri);
	//cvSaveImage(cropfile, pCrop);	
   	
	int r1 = CharacterSeg(pI1);
	int r2 = CharacterSeg(pI2);

	cvReleaseImage(&pI1);
	cvReleaseImage(&pI2);
	cvReleaseImage(&pCrop);

	return max(r1, r2);
}


int ProjectAnalysis(MyRect rect, IplImage* pImage)
{
	//cvSaveImage("c:\\temp\\edge.jpg", pImage);

	int ht = pImage->height;
	int wd = pImage->width;
	int scanwd = pImage->widthStep;
	int* pProjHist = (int*)malloc(sizeof(int)*ht);	
	memset(pProjHist, 0, sizeof(int)*ht);

	int index = 0;
	for(int j=rect.top; j<rect.bottom; j++)
	{
		for(int i=rect.left; i<rect.right; i++)
		{
			unsigned char value =  (unsigned char)(pImage->imageData[j*scanwd+i]);
			if( value>0 )
				pProjHist[index]++;
		}
		index++;
	}

	int res = 0;
	int t1 = (rect.right-rect.left)*0.15;
	int t2 = (rect.right-rect.left)*0.55;
	for(int i=0; i<index; i++)
	{
		if( pProjHist[i]>t1 && pProjHist[i]<t2 )
			res ++;
	}
	free(pProjHist);

	if( res>(rect.bottom-rect.top)*0.6 )
		return 1;
	else
		return 0;
}

int IsNearBorder(MyRect rect, int ht, int wd)
{
	int nIsBorder = 0;
	int nDist = 5;	
	if( rect.left<nDist || rect.right>wd-nDist || rect.top<nDist || rect.bottom>ht-nDist )
		nIsBorder = 1;
	return nIsBorder;
}

int IsTooSmall(MyRect rect)
{
	int nMinHt = 5;
	int s = (rect.bottom-rect.top)*(rect.right-rect.left);

	if(   (rect.bottom-rect.top)<nMinHt || s<300)
		return 1;

	return 0;
}


//plate detection based on segmentation and morphology
void CProject_PGStationDlg::OnDetectLicenseplate()
{
	// TODO: Add your command handler code here
	if(m_nCurrentIndex>=0)
	{	
		IplImage* pOri = CloneCurrentImage();//cvCloneImage(m_imagePointerVector[m_nCurrentIndex]->GetCvArr());
		
		//scale 
		int ht = pOri->height;
		int wd = pOri->width;
		IplImage* pImage;
		double fht = 640;
		if(ht>fht)
		{			
			double ratio = (double)(fht / ht);
			int nht = fht;
			int nwd = wd*ratio;

			pImage = cvCreateImage(cvSize(nwd, nht), 8, 1);
			cvResize(pOri, pImage);
		}
		else
		{
			pImage = cvCloneImage(pOri);
		}
        cvReleaseImage(&pOri); 

		IplImage* pDisp = cvCloneImage(pImage);
		IplImage* pEdge = cvCloneImage(pImage);
	
		//1. vertical edge detection		
		
		//cvSmooth(pImage, pImage, CV_MEDIAN);
		cvSobel(pImage, pImage,1,0);
		cvThreshold(pImage, pImage, 50, 255, CV_THRESH_BINARY);
		cvSmooth(pImage, pImage, CV_MEDIAN);	
		//cvSmooth(pImage, pEdge, CV_MEDIAN);			
		//		
		
		//cvSmooth(pImage, pImage, CV_MEDIAN);
		//VerticalHaarSeg(pImage, pImage);		
		//cvSaveImage("c:\\temp\\seg1.jpg", pImage);
		//cvSmooth(pImage, pImage, CV_MEDIAN, 1);
		//cvSaveImage("c:\\temp\\seg2.jpg", pImage);		
	    
		//2. Morphological filtering according to the characteristics of plate
		IplConvKernel* element = NULL;

		element = cvCreateStructuringElementEx( 15, 1, 7, 0, CV_SHAPE_RECT, 0 );		
		cvDilate(pImage, pImage, element);
		cvReleaseStructuringElement(&element);     
		
		//belement = cvCreateStructuringElementEx( 7, 1, 1, 1, CV_SHAPE_RECT, 0 );
		cvErode(pImage, pImage, element);
		cvReleaseStructuringElement(&element);						
		
		//3. connected components
		ht = pImage->height;
		wd = pImage->width;
		int scanwd = pImage->widthStep;
		unsigned char* mask = (unsigned char*)malloc(ht*wd);
		for(int j=0; j<ht; j++)
			for(int i=0; i<wd; i++)
			{
				mask[j*wd+i] = pImage->imageData[j*scanwd+i];
			}

		MyRect* pRect = (MyRect*)malloc(512*sizeof(MyRect));
		int  nRect = 0;
		SegLabel(mask, ht, wd, pRect, &nRect);
		
		//4. object filter		
		for(int i=0; i<nRect; i++)
		{
			//if too small
			if( IsTooSmall(pRect[i]) )
				continue;

			//if near border
			if( IsNearBorder(pRect[i], ht, wd))
				continue;

			//shape filter	
			double h,w,ratio;
			w = pRect[i].right  - pRect[i].left;
			h = pRect[i].bottom - pRect[i].top;
			ratio = w/h;			
			if( ratio<2 || ratio>10 )
				continue;

			//foreground ratio
			int s = 0;
			for(int m=pRect[i].top; m<pRect[i].bottom; m++)
				for(int n=pRect[i].left; n<pRect[i].right; n++)
				{
					s += mask[m*wd+n]/255;
				}
			double fs = s / (w*h);
			if(fs<0.5)
				continue;
			
			//projection condition
			//if( ProjectAnalysis(pRect[i], pEdge)<1)
			//	continue;
		    
			cvDrawRect( pDisp, cvPoint(pRect[i].left, pRect[i].top), 
					cvPoint(pRect[i].right,pRect[i].bottom), CV_RGB(255,255,255), 2 );							
		}
		free(mask);		
		free(pRect);

		cvNamedWindow("ConnectedComponent");
		cvShowImage("ConnectedComponent", pDisp);
		cvReleaseImage(&pDisp);

		cvNamedWindow("PlateDetect");
		cvShowImage("PlateDetect", pImage);
		cvReleaseImage(&pImage);

		cvReleaseImage(&pEdge);		
	}	
}

void CProject_PGStationDlg::OnHoughLine()
{	
	// TODO: Add your command handler code here
	if( m_nCurrentIndex>=0 )
	{
		IplImage* src = m_imagePointerVector[m_nCurrentIndex]->GetCvArr();
		IplImage* dst;
		IplImage* color_dst = m_imagePointerVector[m_nCurrentIndex]->GetCvArrColor();
		CvMemStorage* storage = cvCreateMemStorage(0);
		CvSeq* lines = 0;
		int i;

		if(src==NULL)
			return;

		dst = cvCreateImage( cvGetSize(src), 8, 1 );
		//color_dst = cvCreateImage( cvGetSize(src), 8, 3 );
        
		cvSmooth(src,src);
		cvCanny( src, dst, 8, 36, 3 );
		//cvCvtColor( dst, color_dst, CV_GRAY2BGR );


#if 0
		lines = cvHoughLines2( dst, storage, CV_HOUGH_STANDARD, 1, CV_PI/180, 50, 0, 0 );
		
		printf("%d \n", lines->total);
		for( i = 0; i < lines->total; i++ )
		{
			float* line = (float*)cvGetSeqElem(lines,i);
			float rho = line[0];
			float theta = line[1];
			float angle = theta/PI*180;
			printf("%lf %lf \n", rho, angle);

			CvPoint pt1, pt2;
			double a = cos(theta), b = sin(theta);
			double x0 = a*rho, y0 = b*rho;
			pt1.x = cvRound(x0 + 1000*(-b));
			pt1.y = cvRound(y0 + 1000*(a));
			pt2.x = cvRound(x0 - 1000*(-b));
			pt2.y = cvRound(y0 - 1000*(a));
			cvLine( color_dst, pt1, pt2, CV_RGB(255,0,0), 1, CV_AA, 0 );
		}
#else
		lines = cvHoughLines2( dst, storage, CV_HOUGH_PROBABILISTIC, 1, CV_PI/180, 64, 8, 12 );
		for( i = 0; i < lines->total; i++ )
		{
			CvPoint* line = (CvPoint*)cvGetSeqElem(lines,i);
			cvLine( color_dst, line[0], line[1], CV_RGB(255,0,0), 1, CV_AA, 0 );
		}
#endif


		/*
		lines = cvHoughLines2( dst, storage, CV_HOUGH_STANDARD, 0.5, CV_PI/90, 30, 30, 50 );
		printf("number of hough lines: %d \n", lines->total);
		for( i = 0; i < lines->total; i++ )
		{
			CvPoint* line = (CvPoint*)cvGetSeqElem(lines,i);
			cvLine( color_dst, line[0], line[1], CV_RGB(255,0,0), 1, CV_AA, 0 );
			//calculate rou and sita

		}
		*/

		cvNamedWindow( "Source", 1 );
		cvShowImage( "Source", src );
		cvNamedWindow( "Hough", 1 );
		cvShowImage( "Hough", color_dst );

		//cvReleaseImage(&src);
		cvReleaseImage(&color_dst);
		cvReleaseMemStorage(&storage);		
	}	
}

void CProject_PGStationDlg::OnSegmentAdaptive()
{
	// TODO: Add your command handler code here

	if(m_nCurrentIndex>=0)
	{
		IplImage* pImage = CloneCurrentImage();//cvCloneImage(m_imagePointerVector[m_nCurrentIndex]->GetCvArr());
		IplImage* pRes = cvCloneImage(pImage);

		cvAdaptiveThreshold(pImage, pRes, 255, 
			CV_ADAPTIVE_THRESH_MEAN_C, 
			CV_THRESH_BINARY, 9, -5);

		cvNamedWindow("AdaptiveThreshold");
		cvShowImage("AdaptiveThreshold", pRes);
		
		cvSaveImage("c:\\temp\\binary.jpg", pRes);

		SetImageBuffer(pRes);

		cvReleaseImage(&pImage);
		cvReleaseImage(&pRes);
	}
}

void CProject_PGStationDlg::OnSegmentMy()
{
	// TODO: Add your command handler code here
	if( m_nCurrentIndex>=0 )
	{
		unsigned char* pbuffer = m_imagePointerVector[m_nCurrentIndex]->GetBuffer();
		IplImage* pOri = cvCloneImage( m_imagePointerVector[m_nCurrentIndex]->GetCvArr() );
		//cvCvtColor(pOri, pOri, CV_BGR2GRAY);

		//scale 
		int ht = pOri->height;
		int wd = pOri->width;
		IplImage* pImage;
		double fht = 640;
		if(ht>fht)
		{			
			double ratio = (double)(fht / ht);
			int nht = fht;
			int nwd = wd*ratio;

			pImage = cvCreateImage(cvSize(nwd, nht), 8, 1);
			cvResize(pOri, pImage);
		}
		else
		{
			pImage = cvCloneImage(pOri);
		}
		cvReleaseImage(&pOri); 

		//cvSmooth(pOri, pOri);
		//segmentation
		ht = pImage->height;
		wd = pImage->width;
		IplImage* pres = cvCreateImage(cvSize(wd,ht), 8, 1);
		VerticalHaarSeg(pImage, pres);
   
		//dilate and erosion

		cvNamedWindow("Segment");
		cvShowImage("Segment", pres);

		SetImageBuffer(pres);

		//release memory
		cvReleaseImage(&pImage);
		cvReleaseImage(&pres);
	}
}

void CProject_PGStationDlg::OnButtonMem()
{
	m_bIsUsingMem = !m_bIsUsingMem;
}

void CProject_PGStationDlg::SetImageBuffer(IplImage* pCurrentImage)
{
	if(m_pCurrentImageBuffer!=NULL)
		cvReleaseImage(&m_pCurrentImageBuffer);
	
	m_pCurrentImageBuffer = cvCloneImage(pCurrentImage);
}

IplImage* CProject_PGStationDlg::CloneCurrentImageColor()
{
	if(m_nCurrentIndex<0)
		return NULL;

	if( m_imagePointerVector[m_nCurrentIndex]->GetCvArrColor() ==NULL )
	{
		return cvCloneImage( m_imagePointerVector[m_nCurrentIndex]->GetCvArr() );
	}
	else
		return cvCloneImage( m_imagePointerVector[m_nCurrentIndex]->GetCvArrColor() );
}

char*     CProject_PGStationDlg::GetCurrentImageFilePath()
{

	if(m_nCurrentIndex<0)
		return NULL;
	
	return (m_imagePointerVector[m_nCurrentIndex])->GetFilePath();

}

IplImage* CProject_PGStationDlg::CloneCurrentImage()
{
	if(m_nCurrentIndex<0)
		return NULL;
	
	if(m_bIsUsingMem && m_pCurrentImageBuffer!=NULL)
	{
		return cvCloneImage(m_pCurrentImageBuffer);
	}
	else
	{		
		return cvCloneImage( m_imagePointerVector[m_nCurrentIndex]->GetCvArr() );
	}
}

void CProject_PGStationDlg::OnMorphologyDilate()
{
	// TODO: Add your command handler code here

	if(m_nCurrentIndex>=0)
	{
		IplImage* pImage = CloneCurrentImage();

		cvDilate(pImage, pImage);
		
		cvNamedWindow("Dilate");
		cvShowImage("Dilate", pImage);

		SetImageBuffer(pImage);
		cvReleaseImage(&pImage);
	}
}

void CProject_PGStationDlg::OnMorphologyErode()
{
	// TODO: Add your command handler code here
	if(m_nCurrentIndex>=0)
	{
		IplImage* pImage = CloneCurrentImage();

		IplConvKernel* element = cvCreateStructuringElementEx( 3, 1, 1, 0, CV_SHAPE_RECT, 0 );
		//cvDilate(v_plane, v_plane, element);
		cvErode(pImage, pImage, element);
		cvReleaseStructuringElement(&element);		

		//cvErode(pImage, pImage);

		cvNamedWindow("Erode");
		cvShowImage("Erode", pImage);

		SetImageBuffer(pImage);
		cvReleaseImage(&pImage);
	}
}

void CProject_PGStationDlg::OnSegmentSimple()
{
	// TODO: Add your command handler code here
	if(m_nCurrentIndex>=0)
	{
		IplImage* pImage = CloneCurrentImage();

		cvThreshold(pImage, pImage, 50, 255, CV_THRESH_BINARY);

		cvNamedWindow("Threshold");
		cvShowImage("Threshold", pImage);

		SetImageBuffer(pImage);
		cvReleaseImage(&pImage);
	}
}

//calculate the threshold using Otsu method, added by xdh, 2012.7.20
void CProject_PGStationDlg::OnSegmentOtsu()
{
	// TODO: Add your command handler code here
	IplImage* pImage = CloneCurrentImage();

	if(pImage!=NULL)
	{
		int ht,wd;
		unsigned char* pbuffer = NULL;		
		IplImageToGrayImage(pImage, &pbuffer, &ht, &wd);

		int threshold = Otsu(pbuffer, ht, wd); //ComputeThresholdValue(pbuffer, ht, wd);
		
		//output the threshold in the output window
		CString sout;
		sout.Format(_T("Otsu thresholc:\\temp %d \n"), threshold);
		m_editOutput.Clear();
		m_editOutput.SetWindowText(sout);

		cvThreshold(pImage, pImage, threshold, 255, CV_THRESH_BINARY);

		cvNamedWindow("OtsuThreshold");
		cvShowImage("OtsuThreshold", pImage);

		SetImageBuffer(pImage);
		cvReleaseImage(&pImage);
		free(pbuffer);
	}
}

void CProject_PGStationDlg::OnConvertRgb2YUV()
{
	// TODO: Add your command handler code here
	IplImage* pColor = CloneCurrentImageColor();
    
	if(pColor!=NULL)
	{		
		IplImage* pYUVImage = cvCreateImage( cvGetSize(pColor), 8, 3 );

		cvCvtColor(pColor, pYUVImage, CV_BGR2YCrCb);

		IplImage* y_plane  = cvCreateImage( cvGetSize(pColor), 8, 1 );
		IplImage* u_plane  = cvCreateImage( cvGetSize(pColor), 8, 1 );
		IplImage* v_plane  = cvCreateImage( cvGetSize(pColor), 8, 1 );

		cvCvtPixToPlane( pYUVImage, y_plane, u_plane, v_plane, 0 );
	
#ifdef _DEBUG
		cvSaveImage("c:\\temp\\color.jpg", pColor);
		cvSaveImage("c:\\temp\\y.jpg", y_plane);
		cvSaveImage("c:\\temp\\u.jpg", u_plane);
		cvSaveImage("c:\\temp\\v.jpg", v_plane);
#endif

		SetImageBuffer(v_plane);

		cvNamedWindow("Color",0);
		cvResizeWindow("Color", 320, 240);
		cvShowImage("Color", pColor);
		
		cvNamedWindow("YUV-Y",0);
		cvResizeWindow("YUV-Y", 320, 240);
		cvShowImage("YUV-Y", y_plane);
		
		cvNamedWindow("YUV-U",0);
		cvResizeWindow("YUV-U", 320, 240);
		cvShowImage("YUV-U", u_plane);

		cvNamedWindow("YUV-V",0);
		cvResizeWindow("YUV-V", 320, 240);
		cvShowImage("YUV-V", v_plane);		


		/*
		//cvAdaptiveThreshold(v_plane, v_plane, 255, 0, 0, 31);
		int ht,wd;
		unsigned char* pbuffer = NULL;		
		IplImageToGrayImage(v_plane, &pbuffer, &ht, &wd);
		int threshold = Otsu(pbuffer, ht, wd);//ComputeThresholdValue(pbuffer, ht, wd);
		IplImage* pres = cvCloneImage(v_plane);
		cvThreshold(v_plane, pres, threshold, 255, CV_THRESH_BINARY);
		cvSmooth(pres, pres, CV_MEDIAN);
		free(pbuffer);		

		cvNamedWindow("v-seg",0);
		cvResizeWindow("v-seg", 320, 240);
		cvShowImage("v-seg", pres);
		cvReleaseImage(&pres);
		*/

		cvReleaseImage(&pColor);
		cvReleaseImage(&pYUVImage);
		cvReleaseImage(&y_plane);
		cvReleaseImage(&u_plane);
		cvReleaseImage(&v_plane);
	}	
}

void CProject_PGStationDlg::OnSuperresolutionBatch()
{
	// TODO: Add your command handler code here

	CString output;
	m_editOutput.Clear();

	char sopticalFlow[256];
	strcpy(sopticalFlow, m_binPath);
	strcat(sopticalFlow, "opticalFlow.txt");

	IplImage* pFirst = cvCloneImage( m_imagePointerVector[0]->GetCvArr() );

	
	char pixelPath[256];
	strcpy(pixelPath, m_binPath);
	strcat(pixelPath, "pixel.txt");

	//save the first image into file
	IplImage* pFirstColor = cvCloneImage( m_imagePointerVector[0]->GetCvArrColor() );
	int ht = pFirstColor->height;
	int wd = pFirstColor->width;
	int scanwd = pFirstColor->widthStep;
	FILE* fp = fopen(pixelPath, "w");
	for(int j=0; j<ht; j++)
		for(int i=0; i<wd; i++)
		{
			if(pFirstColor->nChannels==1)
			{			
				fprintf(fp, "%lf %lf %d\n", (double)(i), (double)(j), 
					(unsigned char)(pFirstColor->imageData[j*scanwd+i]));
			}
			if(pFirstColor->nChannels==3)
			{			
				fprintf(fp, "%lf %lf %d %d %d\n", (double)(i), (double)(j), 
					(unsigned char)(pFirstColor->imageData[j*scanwd+3*i]),
					(unsigned char)(pFirstColor->imageData[j*scanwd+3*i+1]),
					(unsigned char)(pFirstColor->imageData[j*scanwd+3*i+2]) );
			}
		}		
	fclose(fp);
	cvReleaseImage(&pFirstColor);

	//super resolution 
	for(int i=1; i<m_imagePointerVector.size(); i++)
	{
		//CString sout;
		//sout.Format(_T("mean translation: %lf %lf \n"), tx, ty);
		//m_editOutput.SetWindowText(sout);

		IplImage* pSecond = cvCloneImage( m_imagePointerVector[i]->GetCvArr() );			
		GenerateOpticalFlow(pFirst, pSecond, sopticalFlow);
		cvReleaseImage(&pSecond);
		
		//OnSuperresolutionGenerategrids();
		IplImage* pSecondColor = cvCloneImage( m_imagePointerVector[i]->GetCvArrColor() );			
		GenerateGrid(pSecondColor);
		cvReleaseImage(&pSecondColor);	
	}
	cvReleaseImage(&pFirst);

	//

	AfxMessageBox("Finished!");
}

void CProject_PGStationDlg::OnDetectLp()
{
	// TODO: Add your command handler code here

	///////////////// detection based on color /////////////////////////////////
	IplImage* pColor = CloneCurrentImageColor();

	if(pColor!=NULL)
	{		
		IplImage* pYUVImage = cvCreateImage( cvGetSize(pColor), 8, 3 );

		cvCvtColor(pColor, pYUVImage, CV_BGR2YCrCb);

		IplImage* y_plane  = cvCreateImage( cvGetSize(pColor), 8, 1 );
		IplImage* u_plane  = cvCreateImage( cvGetSize(pColor), 8, 1 );
		IplImage* v_plane  = cvCreateImage( cvGetSize(pColor), 8, 1 );

		cvCvtPixToPlane( pYUVImage, y_plane, u_plane, v_plane, 0 );
		//IplImage* yBak = cvCloneImage(y_plane);

		// image segmentation using V 
		cvThreshold(v_plane, v_plane, 145, 255, CV_THRESH_BINARY);

		//median filter
		cvSmooth(v_plane, v_plane, CV_MEDIAN);

		//morphology 
		IplConvKernel* element = cvCreateStructuringElementEx( 11, 3, 5, 1, CV_SHAPE_RECT, 0 );
		cvDilate(v_plane, v_plane, element);
		cvErode(v_plane, v_plane, element);
		cvReleaseStructuringElement(&element);		

		//connected component
		int ht = v_plane->height;
		int wd = v_plane->width;
		int scanwd = v_plane->widthStep;
		unsigned char* mask = (unsigned char*)malloc(ht*wd);
		for(int j=0; j<ht; j++)
			for(int i=0; i<wd; i++)
			{
				mask[j*wd+i] = v_plane->imageData[j*scanwd+i];
			}
		MyRect* pRect = (MyRect*)malloc(512*sizeof(MyRect));
		int  nRect = 0;
		SegLabel(mask, ht, wd, pRect, &nRect);

		// object filter		
		for(int i=0; i<nRect; i++)
		{
			//shape filter	
			double h,w,ratio;
			w = pRect[i].right - pRect[i].left;
			h = pRect[i].bottom - pRect[i].top;
			ratio = w/h;			
			if( ratio<2.8 || ratio>4.1 )
				continue;

			//detect characters
			CvRect r;
			r.x = pRect[i].left - w/4;
			r.y = pRect[i].top - h/4;
			r.width = w*1.5;
			r.height = h*1.5;			
			unsigned char* pbuffer = (unsigned char*)malloc(r.height*r.width);
			IplImage* pCrop = cvCreateImage( cvSize(r.width, r.height), 8, 1);
			int index = 0;
			for(int j=r.y; j<(r.y+r.height); j++)
				for(int i=r.x; i<(r.x+r.width); i++)
				{
					pbuffer[index] = (unsigned char)(y_plane->imageData[j*y_plane->widthStep+i]);
					pCrop->imageData[ (j-r.y)*pCrop->widthStep+(i-r.x)] = (unsigned char)(y_plane->imageData[j*y_plane->widthStep+i]);
					index++;					
				}		

			int threshold = Otsu(pbuffer, r.height, r.width);
			free(pbuffer);
			
			char cropfile[256];
			sprintf(cropfile, "c:\\temp\\crop_%d.jpg", i);
			cvSaveImage(cropfile, pCrop);	
			cvThreshold(pCrop, pCrop, threshold, 255, CV_THRESH_BINARY);
			sprintf(cropfile, "c:\\temp\\cropseg_%d.jpg", i);
			cvSaveImage(cropfile, pCrop);	
			cvReleaseImage(&pCrop);
			
			cvDrawRect( pColor, cvPoint(pRect[i].left, pRect[i].top), 
				cvPoint(pRect[i].right,pRect[i].bottom), CV_RGB(255,0,0), 2 );		

		}
		free(mask);
		free(pRect);

		cvNamedWindow("Color",0);
		cvResizeWindow("Color", 400, 300);
		cvShowImage("Color", pColor);
		
		cvNamedWindow("VSegment",0);
		cvResizeWindow("VSegment", 400, 300);
		cvShowImage("VSegment", v_plane);

		cvReleaseImage(&pColor);
		cvReleaseImage(&pYUVImage);
		cvReleaseImage(&y_plane);
		cvReleaseImage(&u_plane);
		cvReleaseImage(&v_plane);
		//cvReleaseImage(&vBak);
	}
}

void CProject_PGStationDlg::OnFileSaveas()
{
	// TODO: Add your command handler code here
	CFileDialog dlg(false,NULL,NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("jpg file (*.jpg)|*.jpg|bitmap file(*.bmp)|*.bmp||"));

	if(dlg.DoModal() == IDOK)
	{
		CString filepath = dlg.GetPathName();		
		IplImage* pImage = CloneCurrentImageColor();

		if(pImage!=NULL)
			cvSaveImage(filepath, pImage);
		cvReleaseImage(&pImage);
	}
}


void PlateBasedontexture(IplImage* pOri, char* filepath)
{
	if(pOri != NULL)
	{		
		//scale 
		int ht = pOri->height;
		int wd = pOri->width;
		IplImage* pImage;
		double fht = 800;
		if(ht>fht)
		{			
			double ratio = (double)(fht / ht);
			int nht = fht;
			int nwd = wd*ratio;

			pImage = cvCreateImage(cvSize(nwd, nht), 8, 1);
			cvResize(pOri, pImage);
		}
		else
		{
			pImage = cvCloneImage(pOri);
		}
		//cvReleaseImage(&pOri); 


		ht = pImage->height;
		wd = pImage->width;
		int scanWd = pImage->widthStep;

		//0. plate image segmentation
		//unsigned char* mask = (unsigned char*)malloc(ht*wd);
		//PlateImageSeg(pImage, mask);

		//1. sobel gradient
		cvSmooth(pImage, pImage, CV_GAUSSIAN);
		//cvSaveImage("c:\\temp\\image.jpg", pImage);
		IplImage* pSobel = cvCloneImage(pImage);
		cvSobel(pImage, pSobel, 1, 0);
		//cvSmooth(pSobel, pSobel, CV_MEDIAN);

		//2. vertical edge detection		
		IplImage* pEdge = cvCloneImage(pSobel);
		//cvSaveImage("c:\\temp\\sobel.jpg", pEdge);
		memset(pEdge->imageData, 0, ht*scanWd);
		for(int j=0; j<ht; j++)
			for(int i=2; i<wd-2; i++)
			{
				int c = (unsigned char)( pSobel->imageData[j*scanWd+i] );
				int l = (unsigned char)( pSobel->imageData[j*scanWd+i-1] );
				int r = (unsigned char)( pSobel->imageData[j*scanWd+i+1] );

				if( c>30 )
				{					
					if( c>l && c>=r)
					{
						pEdge->imageData[j*scanWd+i] = 255;
					}
					else
					{
						pEdge->imageData[j*scanWd+i] = 0;
					}					
				}
				else
				{
					pEdge->imageData[j*scanWd+i] = 0;
				}
			}
			//cvSaveImage("c:\\temp\\edge.jpg", pEdge);

			//3. delete too long and short edge
			unsigned char* pBuffer;
			int* pPixelLen = (int*)malloc(ht*wd*sizeof(int));
			IplImageToGrayImage(pEdge, &pBuffer, &ht, &wd);
			CalcPixelEdgeLen(pBuffer, ht, wd, pPixelLen);
			//SaveBmp("c:\\temp\\pixelLen.bmp", pPixelLen, ht, wd);
			RemoveEdge(pBuffer, ht, wd, 36, 5);
			//CalcPixelEdgeLen(pBuffer, ht, wd, pPixelLen);
			
			for(int j=0; j<ht; j++)
				for(int i=0; i<wd; i++)
				{
					pEdge->imageData[j*scanWd+i] = pBuffer[j*wd+i];
				}

			//4. calculate integral image
			int* pIntegral = (int*)malloc(ht*wd*sizeof(int));
			memset(pIntegral,0,ht*wd*sizeof(int));
			for(int j=0; j<ht*wd; j++)
				pIntegral[j] = pBuffer[j]/255;
			CalculateIntegralImage1D(pIntegral, ht, wd);
			free(pBuffer);
			pBuffer = NULL;


			//5. plate detection using slide window
			double ratio = 3.3;  // plate_width / plate_height
			int    minHt = 12;  // minimal plate height
			int    maxHt = 48; // maximal plate height
			int    nHtStep = 4; //plate height step
			int    nScanStep = 4; //scan size step
			int    level = (maxHt-minHt)/nHtStep+1;
			//int    nRect = 0;
			//int    nMaxRect = 1024;
			//MyRect* pRect = (MyRect*)malloc(nMaxRect*sizeof(MyRect));
			//double* pW = (double*)malloc(nMaxRect*sizeof(double));
			for(int j=0; j<level; j++)
			{
				int slideHt = minHt + j*nHtStep;
				int slideWd = slideHt*ratio;

				//nScanStep = slideHt/4;

				int row = (ht-slideHt)/nScanStep;
				int col = (wd-slideWd)/nScanStep;

				for(int m=0; m<row; m++)
					for(int n=0; n<col; n++)
					{
						int x = n*nScanStep;
						int y = m*nScanStep;

						//area
						int v  = RectValueUsingIntegral(pIntegral, ht, wd, x, y, slideWd, slideHt);
						if(v<(slideHt*4.5) )
							continue;
						if(v>(slideHt*10) )
							continue;
						
						//local maximal
						int vHull  = RectValueUsingIntegral(pIntegral, ht, wd, 
							max(0,x-1), 
							max(0,y-1), 
							slideWd+2, 
							slideHt+2);
						if( (vHull-v)>20 ) continue;

						int vl = RectValueUsingIntegral(pIntegral, ht, wd, x-nScanStep, y, slideWd, slideHt); 
						int vr = RectValueUsingIntegral(pIntegral, ht, wd, x+nScanStep, y, slideWd, slideHt); 
						int vt = RectValueUsingIntegral(pIntegral, ht, wd, x, y-nScanStep,  slideWd, slideHt); 
						int vb = RectValueUsingIntegral(pIntegral, ht, wd, x, y+nScanStep,  slideWd, slideHt);
						if( v<=vl || v<=vr || v<=vt || v<=vb  ) //&& v>vlt && v>vrt && v>vlb && v>vrb)
							continue;

						int vlt = RectValueUsingIntegral(pIntegral, ht, wd, x-nScanStep, y-nScanStep, slideWd, slideHt); 
						int vrt = RectValueUsingIntegral(pIntegral, ht, wd, x+nScanStep, y-nScanStep, slideWd, slideHt); 
						int vlb = RectValueUsingIntegral(pIntegral, ht, wd, x-nScanStep, y+nScanStep, slideWd, slideHt); 
						int vrb = RectValueUsingIntegral(pIntegral, ht, wd, x+nScanStep, y+nScanStep, slideWd, slideHt); 
						if( v<=vlt || v<=vrt || v<=vlb || v<=vrb ) 
							continue;

						//left and right neighbor						
						int nl = RectValueUsingIntegral(pIntegral, ht, wd, x-slideWd*1, y, slideWd, slideHt); 
						int nr = RectValueUsingIntegral(pIntegral, ht, wd, x+slideWd*1, y, slideWd, slideHt); 
						double ratio1,ratio2;
						ratio1 = double(v)/double(nl+0.000001);
						ratio2 = double(v)/double(nr+0.000001);
						if(  (ratio1<2.5 || ratio2<2.5) ) continue;
						//if( nl>(slideHt*4) || nr>(slideHt*4) ) continue;

						//symmetric 
						int il = RectValueUsingIntegral(pIntegral, ht, wd, x, y, slideWd*0.5, slideHt); 
						int ir = RectValueUsingIntegral(pIntegral, ht, wd, x+slideWd*0.5, y, slideWd*0.5, slideHt); 
                        double iratio = max(il,ir) / ( min(il, ir) + 0.1);
						if(iratio>2) continue;

						//wave analysis


						cvDrawRect( pEdge, cvPoint(x, y), 
							cvPoint(x+slideWd,y+slideHt), CV_RGB(128,128,128), 1 );

						//int numTop    = RectValueUsingIntegral(pIntegral, ht, wd, x, y, slideWd, slideHt*0.5);
						//int numBottom = RectValueUsingIntegral(pIntegral, ht, wd, x, y+slideHt*0.5, slideWd, slideHt*0.5);
						//double numRatio = (double)(numTop) / (double)(numBottom);
						//int halfL = RectValueUsingIntegral(pIntegral, ht, wd, x, y, slideWd*0.5, slideHt);
						//int halfR = RectValueUsingIntegral(pIntegral, ht, wd, x+slideWd*0.5, y, slideWd*0.5, slideHt);
						//if( abs(halfL-halfR)> ((halfL+halfR)*0.2) )
						//	continue;
						//if( numRatio>1.5 || numRatio<0.65 ) continue;											
						//pRect[nRect].left  = x;
						//pRect[nRect].right = x+slideWd;
						//pRect[nRect].top = y;
						//pRect[nRect].bottom = y+slideHt;
						//pW[nRect] = (double)(v) / (double)(slideHt*slideWd);						
						//nRect++;					
						

						//judge the edge length in the detected rectangle
						int lenHist[1024];
						memset(lenHist, 0, 1024*sizeof(int));
						for(int mj=y; mj<(y+slideHt); mj++)
							for(int mi=x; mi<(x+slideWd); mi++)
							{
								lenHist[ pPixelLen[mj*wd+mi] ] ++;
							}

						//very long edge 
						int nLongEdge = 0;
						for(int mi=slideHt*2; mi<1024; mi++)
						{
							if( lenHist[mi]>5 )
								nLongEdge ++;
						}
						if(nLongEdge>0) continue;
						
						//long edge
						nLongEdge = 0;
						for(int mi=slideHt*1.2; mi<1024; mi++)
						{
							if( lenHist[mi]>(slideHt*0.1) )
								nLongEdge ++;
						}
						if(nLongEdge>2)	continue;
						
						//
						int nLineNum = 0;
						for(int mi=(slideHt*0.6); mi<slideHt*1.2; mi++)
						{
							if( lenHist[mi]>0 )
							{
								nLineNum += (int)( (double)(lenHist[mi])/(double)(mi-1)+0.5);
							}
						}
						if(nLineNum<4) continue;
						
						
						/*
						//mean edge length
						int meanLen = 0;
						int np = 0;
						for(int mi=slideHt*0.3; mi<slideHt*1.2; mi++)
						{
							meanLen += ( lenHist[mi]/(mi-1) )*mi*mi;
							np += lenHist[mi];
						}
						meanLen /= (np+0.000001);
						if( meanLen<(slideHt*0.6) ) continue;	
						//if( meanLen<(slideHt*0.6) && nLineNum<5 ) continue;	
						*/
						
						cvDrawRect( pEdge, cvPoint(x, y), 
							cvPoint(x+slideWd,y+slideHt), CV_RGB(255,255,255), 1 );
						
						
						//cvSaveImage("c:\\temp\\image.jpg", pImage);
						//double h1 = RectVariance( (unsigned char*)(pImage->imageData), ht, scanWd, x, y, slideWd*0.5, slideHt );
						//double h2 = RectVariance( (unsigned char*)(pImage->imageData), ht, scanWd, x+slideWd*0.5, y, slideWd*0.5, slideHt );
						double hv1,hv2;
						double hm1,hm2;
						RectVarianceMean( (unsigned char*)(pImage->imageData), ht, scanWd, x, y, slideWd*0.5, slideHt, 
							&hv1, &hm1);
						RectVarianceMean( (unsigned char*)(pImage->imageData), ht, scanWd, x+slideWd*0.5, y, slideWd*0.5, slideHt,
							&hv2, &hm2);
						//horizontal mean gray difference
						//double tm = (hm1+hm2)*0.1;
						//double tm = min(hm1,hm2)/max(hm1,hm2);
						//if( tm<0.6 ) continue;
						//if( fabs(hm1-hm2)>20) continue;						
						//if( fabs(hm1-hm2)>tm ) continue;
						if(fabs(hm1-hm2)>30) continue;
						//horizontal covariance difference
						double th = (hv1+hv2)*0.15;
						//if( hv1<12 || hv2<12 ) continue;
						if( fabs(hv1-hv2)>th ) continue;

						//double th = fabs( hv1/hv2 - 1);
						//if( th>0.2 ) continue;
						/*
						int nLineNum1 = 0;
						for(int mi=5; mi<slideHt*0.5; mi++)
						{
						if( lenHist[mi]>0 )
						nLineNum++;
						}
						if(nLineNum>3)
						continue;		
						*/		
						/*
						int s = 0;
						for(int m=y; m<(y+slideHt); m++)
						for(int n=x; n<(x+slideWd); n++)
						{
						s += mask[m*wd+n]/255;
						}
						double fs = s / (double)(slideWd*slideHt);
						if(fs>0.6)
						*/
						/*
						//histogram similarity
						FILE* fp = fopen("c:\\temp\\hist.txt", "w");
						double wholeHist[64];
						double hist[3][64];  //0.0-0.4, 0.4-0.7, 0.7-1.0
						memset(wholeHist,0,sizeof(double)*64);
						//memset(hist,0,sizeof(double)*64);						
						//whole window histogram
						int nPixel = 0;
						for(int mj=y; mj<(y+slideHt); mj++)
						for(int mi=x; mi<(x+slideWd); mi++)
						{
						int gray = (unsigned char)(pImage->imageData[mj*scanWd+mi]) / 4;
						wholeHist[gray]++;							
						nPixel ++;
						}						
						for(int mi=0; mi<64; mi++)
						{
						wholeHist[mi] /= (double)(nPixel); 
						fprintf(fp, "%lf ", wholeHist[mi]);
						}
						fprintf(fp, "\n");

						//vertical subwindow hitstogram
						double sim[3];
						int offset[4] = {0.0, 0.3*slideWd, 0.6*slideWd, slideWd};
						for(int ki=0; ki<3; ki++)
						{
						int nPixel = 0;
						memset(hist[ki], 0, sizeof(double)*64);

						for(int mj=y; mj<(y+slideHt); mj++)
						for(int mi=(x+offset[ki]); mi<(x+offset[ki+1]); mi++)
						{
						int gray = (unsigned char)(pImage->imageData[mj*scanWd+mi]) / 4;
						hist[ki][gray]++;							
						nPixel ++;
						}

						for(int mi=0; mi<64; mi++)
						{
						hist[ki][mi] /= (double)(nPixel); 
						fprintf(fp, "%lf ", hist[mi]);
						}
						fprintf(fp, "\n");

						//similarity using covariance
						//sim[ki] = Cov(wholeHist, hist, 64);
						}
						fclose(fp);
						sim[0] = Cov( hist[0], hist[1], 64 );
						sim[1] = Cov( hist[0], hist[2], 64 );
						sim[2] = Cov( hist[1], hist[2], 64 );
						bool bIsPass = true;
						for(int ki=0; ki<3; ki++)
						{
						if(sim[ki]<0.7)
						bIsPass = false;
						}
						if(!bIsPass)
						continue;			
						*/
						/*
						//horizon split subwindow
						double hsim;
						int    hoffset[3] = {0.0, 0.5*slideHt, slideHt};
						double hhist[2][64];
						for(int ki=0; ki<2; ki++)
						{
							int nPixel = 0;
							memset(hhist[ki], 0, sizeof(double)*64);

							for(int mj=(y+hoffset[ki]); mj<(y+hoffset[ki+1]); mj++)
								for(int mi=x; mi<(x+slideWd); mi++)
								{
									int gray = (unsigned char)(pImage->imageData[mj*scanWd+mi]) / 4;
									hhist[ki][gray]++;							
									nPixel ++;
								}

							for(int mi=0; mi<64; mi++)
							{
								hhist[ki][mi] /= (double)(nPixel); 
								//fprintf(fp, "%lf ", hhist[mi]);
							}
							//fprintf(fp, "\n");							
						}
						//fclose(fp);
						hsim = Cov( hhist[0], hhist[1], 64 );			
						if(hsim<0.7) continue;								
						
						//vertical split subwindow
						double vsim;
						int    voffset[3] = {0.0, 0.5*slideWd, slideWd};
						double vhist[2][64];
						for(int ki=0; ki<2; ki++)
						{
						int nPixel = 0;
						memset(vhist[ki], 0, sizeof(double)*64);

						for(int mj=(y); mj<(y+slideHt); mj++)
						for(int mi=(x+voffset[ki]); mi<(x+voffset[ki+1]); mi++)
						{
						int gray = (unsigned char)(pImage->imageData[mj*scanWd+mi]) / 4;
						vhist[ki][gray]++;							
						nPixel ++;
						}

						for(int mi=0; mi<64; mi++)
						{
						vhist[ki][mi] /= (double)(nPixel); 
						//fprintf(fp, "%lf ", hhist[mi]);
						}
						//fprintf(fp, "\n");							
						}
						//fclose(fp);
						vsim = Cov( vhist[0], vhist[1], 64 );			
						if(vsim<0.5) continue;
						*/
						
						{							
							cvDrawRect( pImage, cvPoint(x, y), 
								        cvPoint(x+slideWd,y+slideHt), CV_RGB(255,255,255), 2);
							//cvDrawRect( pSobel, cvPoint(x, y), 
							//        cvPoint(x+slideWd,y+slideHt), CV_RGB(255,255,255), 1 );
						}
					
					}
			}

			/*
			int* pIndex = (int*)malloc(sizeof(int)*nRect);
			for(int j=0; j<nRect; j++)
			pIndex[j] = j;

			for(int j=0; j<nRect; j++)
			{
			for(int i=j+1; i<nRect; i++)
			{
			if( pW[j]>pW[i] )
			{
			double t1 = pW[j];
			pW[j] = pW[i];
			pW[i] = t1;

			int nt = pIndex[j];
			pIndex[j] = pIndex[i];
			pIndex[i] = nt;
			}
			}
			}
			for(int j=0; j<1; j++)
			{
			int index = pIndex[j];
			int x1 = pRect[index].left;
			int y1 = pRect[index].top;
			int x2 = pRect[index].right;
			int y2 = pRect[index].bottom;
			cvDrawRect( pImage, cvPoint(x1, y1), 
			cvPoint(x2,y2), CV_RGB(255,255,255), 2 );	
			}
			*/

			if(filepath == NULL)
			{
			cvNamedWindow("VerticalSobel");
			cvShowImage("VerticalSobel", pSobel);
			cvNamedWindow("VerticalEdge");
			cvShowImage("VerticalEdge", pEdge);
			cvNamedWindow("Detect");
			cvShowImage("Detect", pImage);
			}

			if(filepath!=NULL)
				cvSaveImage(filepath, pImage);

			//cvReleaseImage(&pOri);
			cvReleaseImage(&pSobel);
			cvReleaseImage(&pEdge);
			cvReleaseImage(&pImage);
			//free(pRect);
			//free(pW);	
			free(pIntegral);
			free(pPixelLen);
			//free(mask);			
	}
}


//detection based on slide window
void CProject_PGStationDlg::OnPlateBasedontexture()
{
	// TODO: Add your command handler code here
	
	IplImage* pOri = CloneCurrentImage();
	
	if(pOri!=NULL)
	{
		PlateBasedontexture(pOri, NULL);
		cvReleaseImage(&pOri);
	}
}

void CProject_PGStationDlg::OnEdgeVerticaledge()
{
	// TODO: Add your command handler code here

	IplImage* pOri = CloneCurrentImage();

	if(pOri != NULL)
	{		
		//scale 
		int ht = pOri->height;
		int wd = pOri->width;
		IplImage* pImage;
		double fht = 640;
		if(ht>fht)
		{			
			double ratio = (double)(fht / ht);
			int nht = fht;
			int nwd = wd*ratio;

			pImage = cvCreateImage(cvSize(nwd, nht), 8, 1);
			cvResize(pOri, pImage);
		}
		else
		{
			pImage = cvCloneImage(pOri);
		}
		cvReleaseImage(&pOri); 


		ht = pImage->height;
		wd = pImage->width;
		int scanWd = pImage->widthStep;

		//1. sobel gradient
		cvSmooth(pImage, pImage, CV_GAUSSIAN);
		IplImage* pSobel = cvCloneImage(pImage);
		cvSobel(pImage, pSobel, 1, 0);
		//cvSmooth(pSobel, pSobel, CV_MEDIAN);

		//2. vertical edge detection		
		IplImage* pEdge = cvCloneImage(pSobel);
		memset(pEdge->imageData, 0, ht*scanWd);
		for(int j=0; j<ht; j++)
			for(int i=2; i<wd-2; i++)
			{
				int c = (unsigned char)( pSobel->imageData[j*scanWd+i] );
				int l = (unsigned char)( pSobel->imageData[j*scanWd+i-1] );
				int r = (unsigned char)( pSobel->imageData[j*scanWd+i+1] );

				if( c>30 )
				{					
					if( c>l && c>=r)
					{
						pEdge->imageData[j*scanWd+i] = 255;
					}
					else
					{
						pEdge->imageData[j*scanWd+i] = 0;
					}					
				}
				else
				{
					pEdge->imageData[j*scanWd+i] = 0;
				}
			}

		cvNamedWindow("Edge");
		cvShowImage("Edge", pEdge);

		//3. delete too long and short edge
		unsigned char* pBuffer;		
		IplImageToGrayImage(pEdge, &pBuffer, &ht, &wd);
		RemoveEdge(pBuffer, ht, wd, 32, 5);
		for(int j=0; j<ht; j++)
			for(int i=0; i<wd; i++)
			{
				pEdge->imageData[j*scanWd+i] = pBuffer[j*wd+i];
			}

		cvNamedWindow("RemoveEdge");
		cvShowImage("RemoveEdge", pEdge);
       
		free(pBuffer);
		cvReleaseImage(&pEdge);
		cvReleaseImage(&pImage);
		cvReleaseImage(&pOri);
	}

}


//edge detection + morphology
void CProject_PGStationDlg::OnPlateHybrid()
{
	// TODO: Add your command handler code here

	IplImage* pOri = CloneCurrentImage();

	if(pOri != NULL)
	{		
		//scale 
		int ht = pOri->height;
		int wd = pOri->width;
		IplImage* pImage;
		double fht = 640;
		if(ht>fht)
		{			
			double ratio = (double)(fht / ht);
			int nht = fht;
			int nwd = wd*ratio;

			pImage = cvCreateImage(cvSize(nwd, nht), 8, 1);
			cvResize(pOri, pImage);
		}
		else
		{
			pImage = cvCloneImage(pOri);
		}
		cvReleaseImage(&pOri); 


		ht = pImage->height;
		wd = pImage->width;
		int scanWd = pImage->widthStep;

		//1. vertical edge detection
		IplImage* pEdge = cvCloneImage(pImage);
		VerticalEdgeDetect(pEdge, pEdge);
		cvNamedWindow("VerticalEdge");
		cvShowImage("VerticalEdge", pEdge);
	
		/*
		//2. calculate the edge length of each pixel
		int* pPixelLen = (int*)malloc(ht*wd*sizeof(int));
		memset(pPixelLen, 0, ht*wd*sizeof(int));
		unsigned char* pBuffer;
		IplImageToGrayImage(pEdge, &pBuffer, &ht, &wd);
		CalcPixelEdgeLen(pBuffer, ht, wd, pPixelLen);
		*/


		//4. remove the neighbor of pixel whose length greater than threshold
		unsigned char* pBuffer;
		IplImageToGrayImage(pEdge, &pBuffer, &ht, &wd);
		RemoveEdge(pBuffer, ht, wd, 32, 5);
		for(int j=0; j<ht; j++)
			for(int i=0; i<wd; i++)
			{
				pEdge->imageData[j*scanWd+i] = pBuffer[j*wd+i];
			}		
		//cvSmooth(pEdge, pEdge, CV_MEDIAN);
		cvNamedWindow("VerticalEdgeClean");
		cvShowImage("VerticalEdgeClean", pEdge);
        

		/*
		//3. sobel image binary
		IplImage* pSobel = cvCloneImage(pImage);
		cvSobel(pImage, pSobel, 1, 0);
		cvThreshold(pSobel, pSobel, 50, 255, CV_THRESH_BINARY);
		cvNamedWindow("SobelBinary");
		cvShowImage("SobelBinary", pSobel);
		cvSaveImage("c:\\temp\\sobel.jpg", pSobel);

		//4. remove the neighbor of pixel whose length greater than threshold
		for(int j=2; j<ht-2; j++)
			for(int i=2; i<wd-2; i++)
			{
				int index = j*wd+i;
				if( pPixelLen[index]>32 )
				{
					pSobel->imageData[index]   = 0;
					pSobel->imageData[index+1] = 0;
					pSobel->imageData[index+2] = 0;					
					pSobel->imageData[index-1] = 0;
					pSobel->imageData[index-2] = 0;										
				}
			}
		cvNamedWindow("SobelBinary1");
		cvShowImage("SobelBinary1", pSobel);
		*/

		//5. morphology processing
		IplConvKernel* element = NULL;
		element = cvCreateStructuringElementEx( 17, 1, 8, 0, CV_SHAPE_RECT, 0 );		
		cvDilate(pEdge, pEdge, element);
		cvReleaseStructuringElement(&element);     
		//belement = cvCreateStructuringElementEx( 7, 1, 1, 1, CV_SHAPE_RECT, 0 );
		cvErode(pEdge, pEdge, element);
		cvReleaseStructuringElement(&element);	
		cvNamedWindow("MorphSeg");
		cvShowImage("MorphSeg", pEdge);


		//6. connected component analysis
		MyRect* pRect = (MyRect*)malloc(512*sizeof(MyRect));
		int  nRect = 0;
		unsigned char* mask;
		IplImageToGrayImage(pEdge, &mask, &ht, &wd);	
		SegLabel(mask, ht, wd, pRect, &nRect);
		
		//7. plate detection
		for(int i=0; i<nRect; i++)
		{
			//if too small
			if( IsTooSmall(pRect[i]) )
				continue;

			//if near border
			if( IsNearBorder(pRect[i], ht, wd))
				continue;

			//shape filter	
			double h,w,ratio;
			w = pRect[i].right  - pRect[i].left;
			h = pRect[i].bottom - pRect[i].top;
			ratio = w/h;			
			if( ratio<2 || ratio>10 )
				continue;

			//foreground ratio
			int s = 0;
			for(int m=pRect[i].top; m<pRect[i].bottom; m++)
				for(int n=pRect[i].left; n<pRect[i].right; n++)
				{
					s += mask[m*wd+n]/255;
				}
			double fs = s / (w*h);
			if(fs<0.5)
				continue;

			//projection condition
			//if( ProjectAnalysis(pRect[i], pEdge)<1)
			//	continue;

			cvDrawRect( pImage, cvPoint(pRect[i].left, pRect[i].top), 
				cvPoint(pRect[i].right,pRect[i].bottom), CV_RGB(255,255,255), 2 );							
		}
		free(mask);		
		free(pRect);

		cvNamedWindow("Detect");
		cvShowImage("Detect", pImage);		
	}
}

void CProject_PGStationDlg::OnPlateBatch()
{
	// TODO: Add your command handler code here

	CWaitCursor wt;

	int nImage = m_imagePointerVector.size();

	for(int i=0; i<nImage; i++)
	{
		IplImage* pOri = cvCloneImage( m_imagePointerVector[i]->GetCvArr() );
        char filepath[256];

		//sprintf(filepath, "c:\\temp\\Data\\Plate\\res\\%.4d.jpg", i+1);
		sprintf(filepath, "%s_res.png", m_imagePointerVector[i]->GetFilePath());

		PlateBasedontexture(pOri, filepath);

		cvReleaseImage(&pOri);
	}
}

void CProject_PGStationDlg::OnPlateBasedonsegment()
{
	// TODO: Add your command handler code here

	IplImage* pOri = CloneCurrentImage();

	if(pOri != NULL)
	{		
		//scale 
		int ht = pOri->height;
		int wd = pOri->width;
		IplImage* pImage;
		double fht = 800;
		if(ht>fht)
		{			
			double ratio = (double)(fht / ht);
			int nht = fht;
			int nwd = wd*ratio;

			pImage = cvCreateImage(cvSize(nwd, nht), 8, 1);
			cvResize(pOri, pImage);
		}
		else
		{
			pImage = cvCloneImage(pOri);
		}

		//adaptive threshold 
		IplImage* pBinary = cvCloneImage(pImage);
		//cvAdaptiveThreshold(pBinary, pBinary, 255, CV_ADAPTIVE_THRESH_MEAN_C,
		//	CV_THRESH_BINARY, 7, -8);
		cvAdaptiveThreshold(pBinary, pBinary, 255, CV_ADAPTIVE_THRESH_MEAN_C,
			CV_THRESH_BINARY, 11, -16);

		//seglabel
		unsigned char* pbuffer = NULL;
		int nht, nwd;
		IplImageToGrayImage(pBinary, &pbuffer, &nht, &nwd);
		MyRect* pRect = (MyRect*)malloc( sizeof(MyRect)*U8_MAXMEASURE );
		int nRect;
		SegLabel(pbuffer, nht, nwd, pRect, &nRect);
		free(pbuffer);

		//filter the found rectangle
		MyRect* pFilteredRect = (MyRect*)malloc( sizeof(MyRect)*U8_MAXMEASURE );
		int nFilteredRect = 0;
		for(int i=0; i<nRect; i++)
		{
			if(  RectWidth(pRect[i])>RectHeight(pRect[i]) )	continue;

			pFilteredRect[nFilteredRect].left   = pRect[i].left;
			pFilteredRect[nFilteredRect].right  = pRect[i].right;
			pFilteredRect[nFilteredRect].top    = pRect[i].top;
			pFilteredRect[nFilteredRect].bottom = pRect[i].bottom;
			nFilteredRect++;

			cvDrawRect(pBinary, cvPoint( pRect[i].left, pRect[i].top ), 
				cvPoint( pRect[i].right, pRect[i].bottom ), CV_RGB(128, 128, 128) );
		}

		//rect sort according to the x coordinate
		for(int j=0; j<nFilteredRect; j++)
			for(int i=j+1; i<nFilteredRect; i++ )
			{
				if( pFilteredRect[j].left>pFilteredRect[i].left )
				{
					int l,r,t,b;

					l = pFilteredRect[j].left;
					r = pFilteredRect[j].right;
					t = pFilteredRect[j].top;
					b = pFilteredRect[j].bottom;

					pFilteredRect[j].left   = pFilteredRect[i].left;
					pFilteredRect[j].right  = pFilteredRect[i].right;
					pFilteredRect[j].top    = pFilteredRect[i].top;
					pFilteredRect[j].bottom = pFilteredRect[i].bottom;

					pFilteredRect[i].left   = l;
					pFilteredRect[i].right  = r;
					pFilteredRect[i].top    = t;
					pFilteredRect[i].bottom = b;
				}
			}
		
		//scan and  rectangle union
		int* pLabel = (int*)malloc(sizeof(int)*nFilteredRect);
		memset(pLabel, 0, sizeof(int)*nFilteredRect);
		for(int i=0; i<nFilteredRect; i++)
		{
			if(pLabel[i])	continue;
			
			int rectWd = RectWidth(pFilteredRect[i]);
			int rectHt = RectHeight(pFilteredRect[i]);
			int x = pFilteredRect[i].right;
			int y = pFilteredRect[i].top;

			MyRect urect;
			memcpy(&urect, &pFilteredRect[i], sizeof(MyRect)); 
			int nInterSectNum = 1;  //the number of rect intersected with current rect
			
			for(int j=i+1; j<nFilteredRect; j++)
			{
				if(pLabel[j])	continue;

				//searched neighbor area
				MyRect srect;
				srect.left = x;
				srect.right = x+rectWd*3;
				srect.top = y;
				srect.bottom = y+rectHt;
				
				//judge if it intersect with the other rectangle
				MyRect irect = RectIntersection(srect, pFilteredRect[j]);
				double ratio1 = (double)( RectWidth(irect)*RectHeight(irect) ) / 
								(double)(rectHt*rectWd);
				double ratio2 = (double)( RectWidth(irect)*RectHeight(irect) ) / 
					            (double)( RectWidth(pFilteredRect[j])*RectHeight(pFilteredRect[j]) );

				double htRatio1 = (double)(RectHeight(irect)) / (double)( RectHeight(pFilteredRect[i]) );
				double htRatio2 = (double)(RectHeight(irect)) / (double)( RectHeight(pFilteredRect[j]) );

				if(htRatio1>0.6 && htRatio2>0.6)
				{
					//rectangle union
					urect = RectUnion( urect, pFilteredRect[j] );
					x = urect.right;
					y = urect.top;
					rectWd = max(rectWd, RectWidth(pFilteredRect[j]));
					rectHt = max(rectHt, RectHeight(pFilteredRect[j]));
					pLabel[j] = 1;
					nInterSectNum ++;
				}						
			}
			if( nInterSectNum>4 )
			{
				cvDrawRect( pImage, cvPoint(urect.left, urect.top), 
					cvPoint(urect.right,urect.bottom), 
					CV_RGB(255,255,255), 2 );							
			}
		}

		cvNamedWindow("BinaryImage");
		cvShowImage("BinaryImage", pBinary);

		cvNamedWindow("Detect");
		cvShowImage("Detect", pImage);

		free(pLabel);
		free(pFilteredRect);
		free(pRect);
		cvReleaseImage(&pBinary);
		cvReleaseImage(&pImage);
		cvReleaseImage(&pOri);
	}
}

void CProject_PGStationDlg::OnQualityaccessEntropy()
{
	// TODO: Add your command handler code here

	//get current gray image
	IplImage* pImage = CloneCurrentImage();
	
	if(pImage!=NULL)
	{
		//generate histogram
		int hist[256];
		memset(hist, 0, sizeof(int)*256);
		
		//
		int ht = pImage->height;
		int wd = pImage->width;
		int scanWd = pImage->widthStep;
		GenerateHist( (unsigned char*)(pImage->imageData), ht, wd, scanWd, hist);

		//
		double s = 0;
		for(int i=0; i<256; i++)
		{
			s += hist[i];
		}

		double entropy = 0;
		for(int i=0; i<256; i++)
		{
			//calculate the probability
			double p = (double)(hist[i]) / s;
			entropy += -p*log(p+0.000001);
		}

		CString csShow;
		csShow.Format("entropy: %lf", entropy);
		AfxMessageBox(csShow);

		cvReleaseImage(&pImage);
	}
}

void CProject_PGStationDlg::OnQualityaccessGradient()
{
	// TODO: Add your command handler code here
    
	IplImage* pImage = CloneCurrentImage();

	if(pImage!=NULL)
	{
		IplImage* pSobelX = cvCloneImage(pImage);
		IplImage* pSobelY = cvCloneImage(pImage);

		cvSobel(pImage, pSobelX, 1, 0);
		cvSobel(pImage, pSobelY, 0, 1);

		double sGradient = 0;
		int ht = pImage->height;
		int wd = pImage->width;
		int scanWd = pImage->widthStep;

		for(int j=0; j<ht; j++)
			for(int i=0; i<wd; i++)
			{
				double gx = pSobelX->imageData[j*scanWd+i];
				double gy = pSobelY->imageData[j*scanWd+i];
				sGradient += sqrt( (gx*gx + gy*gy)*0.5 );
			}
		cvReleaseImage(&pImage);

		CString csShow;
		csShow.Format("entropy: %lf", sGradient/(double)(ht*wd));
		AfxMessageBox(csShow);
	}
}



void CProject_PGStationDlg::OnDecomposeSize()
{
	// TODO: Add your command handler code here

	//read the AERONET inverted size file and decompose it into files indexed by day
	CFileDialog dlg(TRUE,NULL,NULL,
		OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_ALLOWMULTISELECT|OFN_EXPLORER,
		_T("Level 1.0 file (*.lev10)|*.siz|"));

	if(dlg.DoModal() == IDOK)
	{


	} 
}

void CProject_PGStationDlg::OnDecomposeAod()
{
	// TODO: Add your command handler code here


	//read the AERONET AOD file and decompose it into files indexed by day
	CFileDialog dlg(TRUE,NULL,NULL,
		OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_ALLOWMULTISELECT|OFN_EXPLORER,
		_T("Level 1.0 file(*.lev10)|*.lev10|Level 1.5 file(*.lev15)|*.lev15|Level 2.0 file(*.lev20)|*.lev20||"));

	if(dlg.DoModal() == IDOK)
	{
		CString filepath = dlg.GetPathName();        

		char file[512];
		strcpy(file, filepath);
		int row = GetFileRows(file);		

		char strLine[1024];
		FILE* fp = fopen(filepath, "r");		
		for(int i=0; i<5; i++)
			fgets(strLine, 1024, fp);    
		//m_editOutput.Clear();
		//m_editOutput.SetWindowText(strLine);		

		while( !feof(fp) )
		{
			fgets(strLine, 1024, fp);    
		}

		fclose(fp);

	}  
}

//test XML read and write
void CProject_PGStationDlg::OnXmlRw()
{		
	char xmlFile[512] = 
		"c:\\temp\\Data\\HJ\\2011\\05\\13\\HJ1A-CCD2-457-68-20110513-L20000536606.XML";

	/*xml reading based on Opencv
	CvFileNode *fileNode;//文件结点   
	CvFileStorage *fsRead=NULL;//文件存储   
	double gy;

	fsRead = cvOpenFileStorage(xmlFile, 0, CV_STORAGE_READ); 
	//cvReadByName(fsRead, NULL, "dataUpperLeftY", NULL);  
	fileNode = cvGetFileNodeByName(fsRead,NULL,"dataUpperLeftY");
	cvReadRawData(fsRead, fileNode, &gy, "d");
	printf("%lf \n", gy);

	cvReleaseFileStorage(&fsRead);
	*/

	tinyxml2::XMLDocument doc;	
	if( doc.LoadFile(xmlFile)!=XML_SUCCESS )
		return;
	XMLElement* pRoot = doc.RootElement();
	XMLElement* pNode = NULL;
	const char* pText = NULL;

	if(pRoot) 
	{
		/*
		pNode = pRoot->FirstChildElement("data")->FirstChildElement(); 
		//find every node
		while( pNode!=NULL )
		{		
			pText = pNode->GetText();
			printf("%s: %s \n", pNode->Name(), pText);
			pNode = pNode->NextSiblingElement();
		}
		*/

		pNode = pRoot->FirstChildElement("data")->FirstChildElement("satelliteId"); 
		pText = pNode->GetText();
		//double upLeftX = atof(pText);
		printf("satelliteIc:\\temp %s \n", pText);

		
		pNode = pRoot->FirstChildElement("data")->FirstChildElement("dataUpperLeftX"); 
	    pText = pNode->GetText();
		double upLeftX = atof(pText);

		pNode = pRoot->FirstChildElement("data")->FirstChildElement("dataUpperLeftY"); 
		pText = pNode->GetText();
		double upLeftY = atof(pText);

		printf("%lf %lf \n", upLeftX, upLeftY);

		pNode = pRoot->FirstChildElement("data")->FirstChildElement("earthModel"); 
		pText = pNode->GetText();
		printf("earthModel: %s \n", pText);
		
	} 
}


void CProject_PGStationDlg::OnXmlWrite()
{
	// TODO: Add your command handler code here
	
	/*
	int a=1;  
	float b=2.;  
	double c[]={4.5,6.7,8.9};  

	printf("write xml file ... \n");
	CvFileStorage *fs=cvOpenFileStorage("c:\\temp\\test.xml",0,CV_STORAGE_WRITE);  
	cvWriteComment(fs,"my data",1);  		

	//开始写数据   
	cvStartWriteStruct(fs,"DATA",CV_NODE_MAP,0,cvAttrList(0,0));  
	{
		//写入数据c数组   
		cvStartWriteStruct(fs,"c",CV_NODE_SEQ,0,cvAttrList(0,0));  
		cvWriteRawData(fs,c,3,"d");  
		cvEndWriteStruct(fs);  	

		//写入整型数据a   
		cvWriteInt(fs,"a",a);  	
		//写入浮点型数据b   
		cvWriteReal(fs,"b",b); 		

		cvStartWriteStruct(fs,"c",CV_NODE_SEQ,0,cvAttrList(0,0));  
		cvWriteRawData(fs,c,3,"d");   // "d"-double, "u"-unsigned char		
		cvEndWriteStruct(fs);
	}
	cvEndWriteStruct(fs);  
	cvReleaseFileStorage(&fs);  


	//写入单位矩阵mat   
	CvMat *mat=cvCreateMat(3,3,CV_32SC1);  
	cvSetIdentity(mat);//初始化这个矩阵   
	cvSave("c:\\temp\\mat.xml",mat);
	cvReleaseMat(&mat);  
	printf("Write xml finished! \n");
	*/

	
	tinyxml2::XMLDocument* doc = new tinyxml2::XMLDocument();	
	XMLNode* pdata = doc->NewElement( "data" );	
	doc->InsertFirstChild( pdata );    
	
	//insert
	XMLNode* pNode = doc->NewElement("x");
    XMLText* pText = doc->NewText("100.0");
	pNode->InsertFirstChild(pText);
	pdata->InsertFirstChild(pNode);
	
	//insert
	XMLNode* pNode1 = doc->NewElement("y");
	XMLText* pText1 = doc->NewText("200.0");
	pNode1->InsertFirstChild(pText1);
	pdata->InsertAfterChild(pNode, pNode1);

	
	//replace
	XMLNode* pFind =  pdata->FirstChildElement("x");
	pFind->DeleteChildren();
	pText = doc->NewText("101.0");
	pNode->InsertFirstChild(pText);
	
	doc->SaveFile("c:\\temp\\output\\test.xml");
	delete doc;
}

void CProject_PGStationDlg::OnSimulatePlaneFitting()
{
	//set the plane normal to [1,1,1] and intersection point to [-1,-1,-1]
    double a,b,c;
	double x0,y0,z0;
	double x,y,z;
	double *px= (double*)malloc(sizeof(double)*1000);
	double *py= (double*)malloc(sizeof(double)*1000);
	double *pz= (double*)malloc(sizeof(double)*1000);

	a = 1/sqrt(3.0);
	b = 1/sqrt(3.0);
	c = 1/sqrt(3.0);
	x0 = -30;
	y0 = -30;  
	z0 = -30;
    
	srand(10);

	int index = 0;
	for(y=-50; y<50; y+=5)
		for(x=-50; x<50; x+=5)
		{			
			z = (a*(x-x0)+b*(y-y0)) / (-c) + z0 + rand_double(-5,5);
			printf("%c:\\temp %lf %lf %lf \n", index, x, y, z);
			
			px[index] = x;
			py[index] = y;
			pz[index] = z;

			index ++;
		}

	double R[9];
	FitPlane1(px, py, pz, index, R);

	printf("Rotated result: \n");
	for(y=-50; y<50; y+=10)
		for(x=-50; x<50; x+=10)
		{			
			z = (a*(x-x0)+b*(y-y0)) / (-c) + z0 + rand_double(-5,5);
			double res[3];
			double op[3];
			op[0] = x;
			op[1] = y;
			op[2] = z;

			mult(R,op,res,3,3,1);
			printf("%lf %lf %lf \n", res[0], res[1], res[2]);			
		}	
}

void CProject_PGStationDlg::OnHistogramMatching()
{
	// TODO: Add your command handler code here
	
	if(m_imagePointerVector.size()>1)
	{		
		IplImage* pImage = cvCloneImage( m_imagePointerVector[0]->GetCvArrColor());
		IplImage* pTargetImage = cvCloneImage( m_imagePointerVector[1]->GetCvArrColor());
        
		printf("Histogram Matching .... \n");
		//HistMatchingColor(pImage, pTargetImage);

		cvNamedWindow("HistMatch");
		cvShowImage("HistMatch", pTargetImage);
		cvSaveImage("c:\\temp\\histmatch.jpg", pTargetImage);
	}
}

void CProject_PGStationDlg::OnHogGenerate()
{
	// TODO: Add your command handler code here

	//char filepath[256] = "c:\\temp\\roi.jpg";
	//char filepath[256] = "c:\\temp\\data\\testHog.jpg";

	//IplImage* pImage = cvLoadImage(filepath, 0);
	

	IplImage* pImage = CloneCurrentImage();	
	if(pImage != NULL)
	{

		dllGenerateHOG(pImage);	
		
		cvNamedWindow("Image");
		cvShowImage("Image", pImage);
	}
}

double SegDistance(double l1, double r1, double l2, double r2 )
{
	double bx1,bx2;
	double tx1,tx2;
	double cx1,cx2;
	bx1 = l1;
	bx2 = r1;
	tx1 = l2;
	tx2 = r2;
	cx1 = (bx1+bx2)*0.5;
	cx2 = (tx1+tx2)*0.5;
	double len1 = fabs(bx1-bx2);
	double len2 = fabs(tx1-tx2);
	double len3 = fabs(cx1-cx2);
	double dif = ( len3 - len1*0.5 - len2*0.5 );

	return dif;
}
 
void CProject_PGStationDlg::OnBeltBasedonedge()
{
	// TODO: Add your command handler code here
    int i,j;
	IplImage* pImage = CloneCurrentImage();	
	IplImage* pColorImage = CloneCurrentImageColor();
	
	IplImage* pDisp = cvCreateImage( cvGetSize(pColorImage), pColorImage->depth, 3 );
	//cvCvtColor(pColorImage, pDisp, CV_BGR2GRAY);
    
	for(j=0; j<pColorImage->height; j++)
		for(i=0; i<pColorImage->width; i++)
		{
			pDisp->imageData[j*pDisp->widthStep + i*3]   = pImage->imageData[j*pImage->widthStep+i];
			pDisp->imageData[j*pDisp->widthStep + i*3+1] = pImage->imageData[j*pImage->widthStep+i];
			pDisp->imageData[j*pDisp->widthStep + i*3+2] = pImage->imageData[j*pImage->widthStep+i];
		}


	if(pImage!=NULL)
	{
        int scanwd = pImage->widthStep;

		//smooth
		cvSmooth(pImage, pImage);

		unsigned char* pBuffer;
		int ht,wd;
		IplImageToGrayImage(pImage, &pBuffer, &ht, &wd);
        
		//1. detect canny edge
		IplImage* pEdge = cvCloneImage(pImage);
		cvCanny(pImage, pEdge, 10, 50);
		cvSaveImage("c:\\temp\\output\\cannyedge.jpg", pEdge);

		//2. find the belt top and bottom border
		IplImage* pTopEdge = cvCloneImage(pEdge);
		IplImage* pBottomEdge = cvCloneImage(pEdge);
		for(int j=0; j<ht; j++)
		{
			for(int i=0; i<wd; i++)
			{
				int edgeValue = (unsigned char)( pEdge->imageData[j*scanwd+i] );
                if(edgeValue>0)
				{
					double dx,dy;

					GeneratePixelSobelGradient(pBuffer, ht, wd, i, j, &dx, &dy);
					double angle = cvFastAtan2( dy, dx );
					if( fabs(angle-225)>30)
					{
						pTopEdge->imageData[j*scanwd+i] = 0;
					}
					else
					{
						//cvDrawCircle(pColorImage, cvPoint(i,j), 1, CV_RGB(255, 0, 0));
					}
					
					if( fabs(angle-45)>30)
					{
						pBottomEdge->imageData[j*scanwd+i] = 0;						
					}
					else
					{
						//cvDrawCircle(pColorImage, cvPoint(i,j), 1, CV_RGB(0, 255, 0));
					}
				}		
			}
		}
		//fclose(fp);
				
		//3. hough transform
		//cvSaveImage("c:\\temp\\output\\topEdge.jpg", pTopEdge);
		//cvSaveImage("c:\\temp\\output\\bottomEdge.jpg", pBottomEdge);
		//cvSaveImage("c:\\temp\\output\\topEdge.jpg", pColorImage);
		//cvSaveImage("c:\\temp\\output\\bottomEdge.jpg", pBottomEdge);
		
		CvMemStorage* storage = cvCreateMemStorage(0);
		CvSeq* lines = 0;
		vector<stLINE> topLines;
		vector<stLINE> bottomLines;
		
		cvClearMemStorage(storage);
		lines = cvHoughLines2( pTopEdge, storage, CV_HOUGH_PROBABILISTIC, 0.5, CV_PI/180, 10, 0, 10);
		//printf("top line number: %d \n", lines->total);
		FILE* fp = fopen("c:\\temp\\output\\topline.txt", "w");
		for(int i=0; i<lines->total; i++)
		{
			CvPoint* line = (CvPoint*)cvGetSeqElem(lines,i);
			//cvLine( pColorImage, line[0], line[1], CV_RGB(255,0,0), 1, CV_AA, 0 );
			stLINE l;
			l.v1.x = line[0].x;
			l.v1.y = line[0].y;
			l.v2.x = line[1].x;
			l.v2.y = line[1].y;
			CalculateLinePolarParams(&l);
			//printf("%lf %lf \n", l.rou, l.sita);
			
			if( fabs(l.sita-45)<30 && l.len>10)
			{
				fprintf(fp, "%lf %lf %lf %lf  rou: %lf  sita: %lf len: %lf \n", l.v1.x, l.v1.y, l.v2.x, l.v2.y,
				l.rou, l.sita, l.len);
				topLines.push_back(l);
			}
		}
		fclose(fp);
		
		cvClearMemStorage(storage);
		lines = cvHoughLines2( pBottomEdge, storage, CV_HOUGH_PROBABILISTIC, 0.5, CV_PI/180, 10, 0, 10);
		fp = fopen("c:\\temp\\output\\bottomline.txt", "w");
		for(int i=0; i<lines->total; i++)
		{
			CvPoint* line = (CvPoint*)cvGetSeqElem(lines,i);
			//cvLine( pColorImage, line[0], line[1], CV_RGB(0,255,0), 1, CV_AA, 0 );
			stLINE l;
			l.v1.x = line[0].x;
			l.v1.y = line[0].y;
			l.v2.x = line[1].x;
			l.v2.y = line[1].y;
			CalculateLinePolarParams(&l);
			
			//printf("%lf %lf \n", l.rou, l.sita);			
			if( fabs(l.sita-45)<30 && l.len>10)
			{
				fprintf(fp, "%lf %lf %lf %lf  rou: %lf  sita: %lf len: %lf\n", l.v1.x, l.v1.y, l.v2.x, l.v2.y,
						l.rou, l.sita, l.len);			
				bottomLines.push_back(l);
			}
		}
		fclose(fp);

		//merging lines
		MergeParallelLines(topLines);
		MergeParallelLines(bottomLines);

		printf("top line number: %d \n", topLines.size());
		printf("bottom line number: %d \n", bottomLines.size());       


		//draw merged lines
		for( i=0; i<topLines.size(); i++)
		{
			CvPoint p1,p2;
			p1.x = topLines[i].v1.x;
			p1.y = topLines[i].v1.y;
			p2.x = topLines[i].v2.x;
			p2.y = topLines[i].v2.y;
			//cvLine( pColorImage, p1, p2, CV_RGB(255,0,0), 1, CV_AA, 0 );
		}
		for( i=0; i<bottomLines.size(); i++)
		{
			CvPoint p1,p2;
			p1.x = bottomLines[i].v1.x;
			p1.y = bottomLines[i].v1.y;
			p2.x = bottomLines[i].v2.x;
			p2.y = bottomLines[i].v2.y;
			//cvLine( pColorImage, p1, p2, CV_RGB(0,255,0), 1, CV_AA, 0 );
		}
		cvSaveImage("c:\\temp\\output\\colorEdge1.bmp", pColorImage);
				
		//4. constraints
		vector<MyPoint> pair;
		for( j=0; j<topLines.size(); j++)
		{		
			double tRou = topLines[j].rou;
			double tSita = (topLines[j].sita / 180.0 ) * PI;
			for( i=0; i<bottomLines.size(); i++)
			{
				//condition 1: bottom line vertex must lie the right of the top line
				double x1 = bottomLines[i].v1.x;
				double y1 = bottomLines[i].v1.y;
				double rou1 = x1*cos(tSita) +y1*sin(tSita);
				double x2 = bottomLines[i].v2.x;
				double y2 = bottomLines[i].v2.y;
				double rou2 = x2*cos(tSita) +y2*sin(tSita);				
				if(rou1<tRou || rou2<tRou)
					continue;

				//condition 2: the bottom and top line must be close
				double dif1 = SegDistance(bottomLines[i].v1.x, bottomLines[i].v2.x,
					topLines[j].v1.x, topLines[j].v2.x);
				double dif2 = SegDistance(bottomLines[i].v1.y, bottomLines[i].v2.y,
					topLines[j].v1.y, topLines[j].v2.y);
				if(dif1>10 || dif2>10)
					continue;

				//condition 3: the intersection angle should be small than set threshold
				int sitadif =  fabs(bottomLines[i].sita-topLines[j].sita);
				if(sitadif>5)
					continue;

				//condition 4: the distance of the two lines
				MyPointF bc,tc; //center point of the two lines
				bc.x = (bottomLines[i].v1.x + bottomLines[i].v2.x)*0.5;
				bc.y = (bottomLines[i].v1.y + bottomLines[i].v2.y)*0.5;
				tc.x = (topLines[j].v1.x + topLines[j].v2.x)*0.5;
				tc.y = (topLines[j].v1.y + topLines[j].v2.y)*0.5;				
				double a,b,c;
				a = topLines[j].a;
				b = topLines[j].b;
				c = topLines[j].c;
				double len1 = fabs(bc.x*a+bc.y*b+c) / ( sqrt(a*a+b*b) );
				a = bottomLines[i].a;
				b = bottomLines[i].b;
				c = bottomLines[i].c;
				double len2 = fabs(tc.x*a+tc.y*b+c) / ( sqrt(a*a+b*b) );
				double len = (len1+len2)*0.5;
				//double beltLen = (bottomLines[i].len + topLines[j].len)*0.5;
				if(len<8 || len>40)
					continue;

				//condition 5: the length of belt 		


				MyPoint p;
				p.x = j;
				p.y = i;
				pair.push_back(p);
			}
		}

		for(int j=0; j<pair.size(); j++)
		{
			int ti,bi;
			CvPoint p1,p2;
			ti = pair[j].x;
			bi = pair[j].y;
			
			p1.x = topLines[ti].v1.x;
			p1.y = topLines[ti].v1.y;
			p2.x = topLines[ti].v2.x;
			p2.y = topLines[ti].v2.y;
			cvLine( pColorImage, p1, p2, CV_RGB(255,0,0), 2, CV_AA, 0 );
			cvLine( pDisp, p1, p2, CV_RGB(255,0,0), 2, CV_AA, 0 );

			p1.x = bottomLines[bi].v1.x;
			p1.y = bottomLines[bi].v1.y;
			p2.x = bottomLines[bi].v2.x;
			p2.y = bottomLines[bi].v2.y;
			cvLine( pColorImage, p1, p2, CV_RGB(0,255,0), 2, CV_AA, 0 );
			cvLine( pDisp, p1, p2, CV_RGB(0,255,0), 2, CV_AA, 0 );
		}

		//cvSaveImage("c:\\temp\\output\\beltBinary.jpg", pEdge);
		cvSaveImage("c:\\temp\\output\\colorEdge2.bmp", pDisp);
			
		cvNamedWindow("Edge");
		cvShowImage("Edge", pEdge);

		cvNamedWindow("image");
		cvShowImage("image", pDisp);
     
		cvReleaseImage(&pImage);
		cvReleaseImage(&pColorImage);
		cvReleaseImage(&pEdge);
		cvReleaseImage(&pTopEdge);
		cvReleaseImage(&pBottomEdge);
		free(pBuffer);
	}
}

void CProject_PGStationDlg::OnDetectEclipse()
{
	// TODO: Add your command handler code here

	IplImage* pImage = CloneCurrentImage();	
	IplImage* pColorImage = CloneCurrentImageColor();

	if(pImage!=NULL)
	{
		//1. canny detection
		IplImage* pEdge = cvCloneImage(pImage);
		cvSmooth(pImage, pImage);
		cvCanny(pImage, pEdge, 8, 36);
		//cvSaveImage("c:\\temp\\edge.jpg", pEdge);

		//2.
		CvMemStorage* stor = cvCreateMemStorage(0);
		CvSeq* cont = cvCreateSeq(CV_SEQ_ELTYPE_POINT, sizeof(CvSeq), sizeof(CvPoint) , stor);
		int Nc = cvFindContours(pEdge, stor, &cont);
		
		
		CvBox2D32f* box;
		CvPoint* PointArray;
		CvPoint2D32f* PointArray2D32f;

		// This cycle draw all contours and approximate it by ellipses.
		for(;cont;cont = cont->h_next)
		{   
			int i; // Indicator of cycle.
			int count = cont->total; // This is number point in contour
			CvPoint center;
			CvSize size;

			// Number point must be more than or equal to 6 (for cvFitEllipse_32f).        
			if( count < 6 )
				continue;

			// Alloc memory for contour point set.    
			PointArray = (CvPoint*)malloc( count*sizeof(CvPoint) );
			PointArray2D32f= (CvPoint2D32f*)malloc( count*sizeof(CvPoint2D32f) );

			// Alloc memory for ellipse data.
			box = (CvBox2D32f*)malloc(sizeof(CvBox2D32f));

			// Get contour point set.
			cvCvtSeqToArray(cont, PointArray, CV_WHOLE_SEQ);

			// Convert CvPoint set to CvBox2D32f set.
			vector<POINT2> pts;
			for(i=0; i<count; i++)
			{
				PointArray2D32f[i].x = (float)PointArray[i].x;
				PointArray2D32f[i].y = (float)PointArray[i].y;

				POINT2 tp;
				tp.x = (float)PointArray[i].x;
				tp.y = (float)PointArray[i].y;
				pts.push_back(tp);	
			}

			// Fits ellipse to current contour.
			cvFitEllipse(PointArray2D32f, count, box);
			
			stEllipse ellipsePara;
			double err = 0;
			InvertEllipse(pts, ellipsePara, err);
			printf("angle: %lf \n", ellipsePara.angle);
			
			for(int i=0; i<pts.size(); i++)
			{
				double err = EllipseFunc(pts[i].x, pts[i].y, ellipsePara);
				printf("%lf  ", err);
			}

			//
			DrawEllipse(ellipsePara, pColorImage);


			/*
			// Draw current contour.
			cvDrawContours(pColorImage,cont,CV_RGB(255,255,255),CV_RGB(255,255,255),0,1,8,cvPoint(0,0));

			// Convert ellipse data from float to integer representation.
			center.x = cvRound(box->center.x);
			center.y = cvRound(box->center.y);
			size.width = cvRound(box->size.width*0.5);
			size.height = cvRound(box->size.height*0.5);
			box->angle = -box->angle;

			// Draw ellipse.
			cvEllipse(pColorImage, center, size,
				box->angle, 0, 360,
				CV_RGB(0,0,255), 1, CV_AA, 0);
			*/

			// Free memory.          
			free(PointArray);
			free(PointArray2D32f);
			free(box);
		}
		
		cvNamedWindow("Ellipse");
		cvShowImage("Ellipse", pColorImage);
		
		cvReleaseImage(&pEdge);
		cvReleaseImage(&pColorImage);
	}
}

void CProject_PGStationDlg::OnToolImagecrop()
{
	// TODO: Add your command handler code here		
	//select files to be cropped
	CFileDialog dlg(TRUE,NULL,NULL,
		OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_ALLOWMULTISELECT|OFN_EXPLORER,
		_T("jpg file (*.jpg)|*.jpg|bitmap file(*.bmp)|*.bmp|Tiff file(*.tif)|*.tif||"));

	TCHAR* szPathMulti  = new TCHAR[8*1024*1024];  //alloc memory for multiple files
	dlg.m_ofn.nMaxFile  = 8*1024*1024; 
	dlg.m_ofn.lpstrFile = szPathMulti; 
	dlg.m_ofn.lpstrFile[0] = 0; 

	vector<CString> fileVector;
	if(dlg.DoModal()==IDOK)
	{
		POSITION  pos   =   dlg.GetStartPosition(); 		
		//read file paths
		while(pos != NULL) 
		{ 
			//insert image filepath into the file list control
			CString filepath = dlg.GetNextPathName(pos); 			
			fileVector.push_back(filepath);	
		} 
	}

	//crop image
	CvRect rect;
	//1 cropland
	//rect.x = 8400;	
	//rect.y = 3750;
	//rect.width = 500;
	//rect.height = 500;
	

	
	//3 airport
	//rect.x = 3451;
	//rect.y = 7268;
	//rect.width = 500;
	//rect.height = 250;	

	//4 city
	rect.x = 3630;
	rect.y = 9396;
	rect.width = 500;
	rect.height = 400;	

	//5 urban
	rect.x = 1552;
	rect.y = 3969;
	rect.width = 500;
	rect.height = 400;	

	//6 road
	rect.x = 2425;
	rect.y = 9252;
	rect.width = 500;
	rect.height = 300;	

	//2 boat
	rect.x = 7396;
	rect.y = 5659;
	rect.width = 1200;
	rect.height = 1200;	

	//for IR image from Liming Zhao, 2013.5.30
	rect.x = 0;
	rect.y = 256;
	rect.width = 360;
	rect.height = 250;	

	//for COMS image, added by xiedonghai, 2013.6.8
	rect.x = 775;
	rect.y = 520;
	rect.width = 385;
	rect.height = 436;	


	for(int i=0; i<fileVector.size(); i++)
	{
		IplImage* pImage = cvLoadImage(fileVector[i]);
		cvSetImageROI(pImage, rect);

		//save crop file
		CString cropfile = fileVector[i];
		char cfilepath[512];
		strcpy(cfilepath, cropfile);
		char* pdes = strrchr(cfilepath, '.');
		int index = pdes - cfilepath + 1;
		strcpy(cfilepath+index, "\0");
		strcat(cfilepath, "crop.jpg");

		IplImage* dst = cvCreateImage(cvSize(rect.width, rect.height),
			pImage->depth, pImage->nChannels);

		cvCopy(pImage,dst,0);
		cvResetImageROI(pImage);

		cvSaveImage(cfilepath, dst);

		cvReleaseImage(&pImage);
		cvReleaseImage(&dst);
	}

	AfxMessageBox("Crop Finished!");

}

void CProject_PGStationDlg::OnSharpLaplace()
{
	// TODO: Add your command handler code here

	IplImage* pImage = CloneCurrentImage();	
	//IplImage* pColorImage = CloneCurrentImageColor();
	if(pImage!=NULL)
	{
		ImageSharpLaplace((unsigned char*)(pImage->imageData), pImage->height, pImage->width, pImage->widthStep, 0.5);

		cvNamedWindow("Sharp");
		cvShowImage("Sharp", pImage);

		cvSaveImage("c:\\temp\\sharp.jpg", pImage);
		cvReleaseImage(&pImage);
		//cvReleaseImage(&pColorImage);
	}
}

void CProject_PGStationDlg::OnSharpGaussian()
{
	// TODO: Add your command handler code here
	IplImage* pImage = CloneCurrentImage();	
	//IplImage* pColorImage = CloneCurrentImageColor();
	if(pImage!=NULL)
	{
		ImageSharpGaussian((unsigned char*)(pImage->imageData), pImage->height, pImage->width,
			pImage->widthStep);

		cvNamedWindow("Sharp");
		cvShowImage("Sharp", pImage);

		cvSaveImage("c:\\temp\\sharp.jpg", pImage);

		cvReleaseImage(&pImage);
		//cvReleaseImage(&pColorImage);
	}
}

void CProject_PGStationDlg::OnImageioWrite()
{
	// TODO: Add your command handler code here
	int owd,oht;
	oht = 20000;
	owd = 20000;
	IplImage* mosaicImg = cvCreateImage(cvSize(owd,oht), IPL_DEPTH_8U, 3);
    if(mosaicImg==NULL)
	{
	}
}

void CProject_PGStationDlg::OnDemInterpolation()
{
	// TODO: Add your command handler code here
	
	
	//read bundle.out file
	//generate triangle



	//fast interpolation

}

#define FREE_MYLIB FreeLibrary
#define GET_MYPROC GetProcAddress
#define SIFTGPU_DLL_RUNTIME
void CProject_PGStationDlg::OnGpuSift()
{
	// TODO: Add your command handler code here
	#ifdef SIFTGPU_DLL_RUNTIME
		#ifdef _WIN32
			#ifdef _DEBUG
				HMODULE  hsiftgpu = LoadLibrary("siftgpu_d.dll");
			#else
				HMODULE  hsiftgpu = LoadLibrary("siftgpu.dll");
			#endif
		#else
			void * hsiftgpu = dlopen("libsiftgpu.so", RTLD_LAZY);
		#endif
	#endif

	if(hsiftgpu == NULL) 
		return ;

	SiftGPU* (*pCreateNewSiftGPU)(int) = NULL;
	SiftMatchGPU* (*pCreateNewSiftMatchGPU)(int) = NULL;
	pCreateNewSiftGPU = (SiftGPU* (*) (int)) GET_MYPROC(hsiftgpu, "CreateNewSiftGPU");
	pCreateNewSiftMatchGPU = (SiftMatchGPU* (*)(int)) GET_MYPROC(hsiftgpu, "CreateNewSiftMatchGPU");
	SiftGPU* sift = pCreateNewSiftGPU(1);
	SiftMatchGPU* matcher = pCreateNewSiftMatchGPU(4096);

	vector<float > descriptors1(1), descriptors2(1);
	vector<SiftGPU::SiftKeypoint> keys1(1), keys2(1);    
	int num1 = 0, num2 = 0;

	char * argv[] = {"-fo", "-1",  "-v", "1"};//
	int argc = sizeof(argv)/sizeof(char*);
	sift->ParseParam(argc, argv);
		
	if(sift->CreateContextGL() != SiftGPU::SIFTGPU_FULL_SUPPORTED) 
		return ;

	char lfile[256] = "c:\\temp\\Data\\Registration\\zy3-sp-pan-1.jpg";
	char rfile[256] = "c:\\temp\\Data\\Registration\\spot-summerpalace-pan.jpg";

	//use opendialog to select the filepath

	/*
	if( sift->RunSIFT(filepath) ) 
	{
		//Call SaveSIFT to save result to file, the format is the same as Lowe's
		printf("save feature points \n");
		sift->SaveSIFT("c:\\temp\\sift.txt"); //Note that saving ASCII format is slow
		printf("Save finished !\n");
		
		//get feature count
		num1 = sift->GetFeatureNum();
		
		//allocate memory
		keys1.resize(num1);   
		descriptors1.resize(128*num1);
		
		//reading back feature vectors is faster than writing files
		//if you dont need keys or descriptors, just put NULLs here
		sift->GetFeatureVector(&keys1[0], &descriptors1[0]);
		//this can be used to write your own sift file.       

        //draw feature
		IplImage* pColor = cvLoadImage(filepath);
		for(int i=0; i<num1; i++)
		{
			int x = keys1[i].x;
			int y = keys1[i].y;			
			cvLine( pColor, cvPoint( x-3,y), cvPoint( x+3,y),CV_RGB(255,0,255),1,8,0);
			cvLine( pColor, cvPoint( x,y-3), cvPoint( x,y+3),CV_RGB(255,0,255),1,8,0);
			//cvDrawCircle(pImage, );
		}
		cvSaveImage("c:\\temp\\siftpt.jpg", pColor);
		cvReleaseImage(&pColor);
	}
	*/

	if( sift->RunSIFT(lfile) )
	{
		num1 = sift->GetFeatureNum();
		keys1.resize(num1);   
		descriptors1.resize(128*num1);
		sift->GetFeatureVector(&keys1[0], &descriptors1[0]);
	}

	//You can have at most one OpenGL-based SiftGPU (per process).
	//Normally, you should just create one, and reuse on all images. 
	if( sift->RunSIFT(rfile) )
	{
		num2 = sift->GetFeatureNum();
		keys2.resize(num2);
		descriptors2.resize(128*num2);
		sift->GetFeatureVector(&keys2[0], &descriptors2[0]);
	}

	//Verify current OpenGL Context and initialize the Matcher;
	//If you don't have an OpenGL Context, call matcher->CreateContextGL instead;
	matcher->VerifyContextGL();    //must call once
	//Set descriptors to match, the first argument must be either 0 or 1
	//if you want to use more than 4096 or less than 4096
	//call matcher->SetMaxSift() to change the limit before calling setdescriptor
	matcher->SetDescriptors(0, num1, &descriptors1[0]); //image 1
	matcher->SetDescriptors(1, num2, &descriptors2[0]); //image 2
	//match and get result.    
	int (*match_buf)[2] = new int[num1][2];
	//use the default thresholds. Check the declaration in SiftGPU.h
	int num_match = matcher->GetSiftMatch(num1, match_buf);
	std::cout << num_match << " sift matches were found;\n";

	IplImage* pLeft = cvLoadImage(lfile);
	IplImage* pRight = cvLoadImage(rfile);
	//enumerate all the feature matches
	for(int i  = 0; i < num_match; ++i)
	{
		//How to get the feature matches: 
		SiftGPU::SiftKeypoint & key1 = keys1[match_buf[i][0]];
		SiftGPU::SiftKeypoint & key2 = keys2[match_buf[i][1]];
		//key1 in the first image matches with key2 in the second image

		int x = key1.x;
		int y = key1.y;			
		cvLine( pLeft, cvPoint( x-5,y), cvPoint( x+5,y),CV_RGB(255,0,255),1,8,0);
		cvLine( pLeft, cvPoint( x,y-5), cvPoint( x,y+5),CV_RGB(255,0,255),1,8,0);

		x = key2.x;
		y = key2.y;			
		cvLine( pRight, cvPoint( x-5,y), cvPoint( x+5,y),CV_RGB(255,0,255),1,8,0);
		cvLine( pRight, cvPoint( x,y-5), cvPoint( x,y+5),CV_RGB(255,0,255),1,8,0);
	}
	
	cvSaveImage("c:\\temp\\left.jpg", pLeft);
	cvSaveImage("c:\\temp\\right.jpg", pRight);

	cvReleaseImage(&pLeft);
	cvReleaseImage(&pRight);

	delete sift;
	delete matcher;
}



void CProject_PGStationDlg::OnCudaInit()
{
	// TODO: Add your command handler code here
	
	//EnumGPUDevice();
	//SetDevice();
	//findCapableDevice();
}

//#include "engine.h"

#define BUFSIZE 256
#pragma comment(lib, "libeng.lib")
#pragma comment(lib, "libmx.lib")
#pragma comment(lib, "libmat.lib")
void CProject_PGStationDlg::OnDebugCallmatlab()
{
	
	// TODO: Add your command handler code here
	//Engine *ep;
	//mxArray *T = NULL, *a = NULL, *d = NULL;
 //   mxArray *D = NULL;
	//mxArray *mx = NULL;
	//char buffer[BUFSIZE+1];
	//double *Dreal, *Dimag;
	//double time[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	//int i,j;


	//if ( !(ep = engOpen(NULL)) ) 
	//{
	//	printf("can not start matlab engine! \n");
	//	return;
	//}


	//engEvalString(ep, "rand('seed',0)");
	//engEvalString(ep, "x = rand(100,1)*4-2;");
	//engEvalString(ep, "y = rand(100,1)*4-2;");
	//engEvalString(ep, "z = x.*exp(-x.^2-y.^2");

	//engEvalString(ep, "ti = -2:.25:2"); 
	//engEvalString(ep, "[XI,YI] = meshgrid(ti,ti)");
	//engEvalString(ep, "ZI = griddata(x,y,z,XI,YI)");
 //   //engEvalString(ep, "d=ZI(:); d(find(isnan(d)==1))=0");
	////engEvalString(ep, "mesh(XI,YI,ZI), hold");
	////engEvalString(ep, "plot3(x,y,z,'o'), hold off");


	//mx = engGetVariable(ep, "ZI");
	//if(mx==NULL)
	//{
	//	printf("get variable error \n");
	//}
	//else
	//{
	//	double* out = mxGetPr(mx);
	//	printf("\n");
	//	for( j=0; j<17; j++)
	//	{
	//		for( i=0; i<17; i++)
	//		{
	//			printf("%lf ", out[j*17+i]);
	//		}
	//		printf("\n");
	//	}
	//}

	//engClose(ep);
}


void CProject_PGStationDlg::OnCallmatlabMcc()
{
	// TODO: Add your command handler code here
}



void CProject_PGStationDlg::OnWriteBinaryfile()
{
	// TODO: Add your command handler code here
	int count = 1;
	FILE* fp = fopen("c:\\temp\\version.dat", "wb");
	fwrite(&count, sizeof(int), 1, fp);
	fclose(fp);
}

void CProject_PGStationDlg::OnSobelThin()
{
	// TODO: Add your command handler code here

	IplImage* pImage = CloneCurrentImage();	
	IplImage* pColorImage = CloneCurrentImageColor();
	if(pImage!=NULL)
	{
		//smooth
		cvSmooth(pImage, pImage);

		//convert to image matrix
		unsigned char* pbuffer = NULL;
		unsigned char* pImageBuffer = NULL;
		int ht,wd;
		IplImageToGrayImage( pImage, &pbuffer, &ht, &wd );
		pImageBuffer = (unsigned char*)malloc(ht*wd);
		memcpy(pImageBuffer, pbuffer, ht*wd);
		SaveBmp("c:\\temp\\image.bmp", pImageBuffer, ht, wd);

		//sobel filter
		SobelEdge(pbuffer, ht, wd);
		SaveBmp("c:\\temp\\sobel.bmp", pbuffer, ht, wd);

		//nonmax filter
		unsigned char* pEdge = (unsigned char*)malloc(ht*wd);
		memset(pEdge, 0, ht*wd);
		NonMaxEdgeDetect(pbuffer, ht, wd, pEdge);
		SaveBmp("c:\\temp\\sobelEdge.bmp", pEdge, ht, wd);

		//gradient direction
		for(int j=0; j<ht; j++)
		{
			for(int i=0; i<wd; i++)
			{
				int edgeValue = pEdge[j*wd+i];
				if(edgeValue>0)
				{
					double dx,dy;
					GeneratePixelSobelGradient(pImageBuffer, ht, wd, i, j, &dx, &dy);
					double angle = cvFastAtan2( dy, dx );

					if( fabs(angle-225)<30 || fabs(angle-45)<30 )
						continue;
					pEdge[j*wd+i] = 0;


					/*if( fabs(angle-225)>30)
					{
						//pTopEdge->imageData[j*scanwd+i] = 0;
						pEdge[j*wd+i] = 0;
					}
					if( fabs(angle-45)>30 )
					{
						//pBottomEdge->imageData[j*scanwd+i] = 0;						
						pEdge[j*wd+i] = 0;
					}*/					
				}		
			}
		}
		SaveBmp("c:\\temp\\sobelEdgeBelt.bmp", pEdge, ht, wd);

		//
		IplImage* pCvEdge = cvCreateImage(cvSize(wd,ht), 8, 1);
		for(int j=0; j<ht; j++)
			for(int i=0; i<wd; i++)
			{
				if(pEdge[j*wd+i]>0)
				{
					pCvEdge->imageData[j*pCvEdge->widthStep+i] = -1;
				}
			}

		//hough transform
		CvMemStorage* storage = cvCreateMemStorage(0);
		CvSeq* lines = 0;
		vector<stLINE> topLines;
		vector<stLINE> bottomLines;

		cvClearMemStorage(storage);
		lines = cvHoughLines2( pCvEdge, storage, CV_HOUGH_PROBABILISTIC, 0.5, CV_PI/180, 10, 0, 10);
		//printf("top line number: %d \n", lines->total);
		for(int i=0; i<lines->total; i++)
		{
			CvPoint* line = (CvPoint*)cvGetSeqElem(lines,i);
			cvLine( pColorImage, line[0], line[1], CV_RGB(255,0,0), 1, CV_AA, 0 );				
		}

		free(pbuffer);
		free(pImageBuffer);
		free(pEdge);
	
		cvNamedWindow("Line");
		cvShowImage("Line", pColorImage);
	}
}

void CProject_PGStationDlg::OnConvolutionEdge()
{
	// TODO: Add your command handler code here
	IplImage* pImage = CloneCurrentImage();	
	IplImage* pColorImage = CloneCurrentImageColor();
	if(pImage!=NULL)
	{
		int ht = pImage->height;
		int wd = pImage->width;
		cvSmooth(pImage, pImage);

		//
		unsigned char* pbuffer = NULL;
		IplImageToGrayImage(pImage, &pbuffer, &ht, &wd);

		float vals[] = 
		{0,0,0,1, 0,
		 0,0,1,0,-1,
		 0,1,0,-1,0,
		 1,0,-1,0,0,
		 0,-1,0,0,0};

		CvMat* kernel = cvCreateMat(5, 5, CV_32FC1);
        cvInitMatHeader(kernel, 5, 5, CV_32FC1, vals);

		cvFilter2D(pImage, pImage, kernel);//, cvPoint(-2,-2) );

		//non-maximal 
		int scanwd = pImage->widthStep;
		for(int j=1; j<ht-1; j++)
			for(int i=1; i<wd-1; i++)
			{
				int cenPixel = (unsigned char)(pImage->imageData[j*scanwd+i]);
				int ltPixel = (unsigned char)(pImage->imageData[j*scanwd+i-scanwd-1]);
				int rbPixel = (unsigned char)(pImage->imageData[j*scanwd+i+scanwd+1]);
				
				if(cenPixel>10)
				{
					if( cenPixel>ltPixel && cenPixel>rbPixel )
						pImage->imageData[j*scanwd+i] = -1;
					else
						pImage->imageData[j*scanwd+i] = 0;
				}
				else
					pImage->imageData[j*scanwd+i] = 0;
			}

			//direction constraint
			for(int j=0; j<ht; j++)
			{
				for(int i=0; i<wd; i++)
				{
					int edgeValue = (unsigned char)(pImage->imageData[j*scanwd+i]);
					if(edgeValue>0)
					{
						double dx,dy;
						GeneratePixelSobelGradient(pbuffer, ht, wd, i, j, &dx, &dy);
						double angle = cvFastAtan2( dy, dx );

						if( fabs(angle-225)<30 || fabs(angle-45)<30 )
						{
							pImage->imageData[j*scanwd+i] = -1;
						}
						else
							pImage->imageData[j*scanwd+i] = 0;
					}		
				}
			}
		
		//hough transform
		CvMemStorage* storage = cvCreateMemStorage(0);
		CvSeq* lines = 0;
		vector<stLINE> topLines;
		vector<stLINE> bottomLines;
		cvClearMemStorage(storage);
		lines = cvHoughLines2( pImage, storage, CV_HOUGH_PROBABILISTIC, 0.5, CV_PI/180, 10, 0, 10);
		//printf("top line number: %d \n", lines->total);
		for(int i=0; i<lines->total; i++)
		{
			CvPoint* line = (CvPoint*)cvGetSeqElem(lines,i);
			cvLine( pColorImage, line[0], line[1], CV_RGB(255,0,0), 1, CV_AA, 0 );				
		}
		

		//cvCanny(pImage, pImage);
		//cvThreshold(pImage, pImage, 10, 255, CV_THRESH_BINARY);

		cvNamedWindow("Filter");
		cvShowImage("Filter", pImage);

		cvNamedWindow("Line");
		cvShowImage("Line", pColorImage);
	}

}

void CProject_PGStationDlg::OnModelGeneratemodelfor6sv()
{
	// TODO: Add your command handler code here

	//input aerosol double mode parameters, and output as 6sv format

	/*
	CDlgInputRI dlgInput;
	if(dlgInput.DoModal() == IDOK)
	{
		//double mr = atof( dlgInput.GetDlgItem() );
		//for(int )
	}
	*/
	
	double mr = 1.5;
	double mi = 0.0095;
	FILE* fp = fopen("c:\\temp\\6sv_aerosolinput.txt", "w");
	for(int i=0; i<20; i++)
	{
		fprintf(fp, "%6.4lf ", mr);
	}
	fprintf(fp, "\n");
	for(int i=0; i<20; i++)
	{
		fprintf(fp, "%6.4lf ", mi);
	}
	fclose(fp);


	//interpolation of refractive index
	//double wavelen[4] = {0.441, 0.675, 0.87, 1.02}; // um
	//double mr[4] = {1.6, 1.559400, 1.558700, 1.559700};
	//double mi[4] = {0.015651, 0.011748, 0.010426, 0.010307};
	

	/*
	//output the refractive index based on the interpolation model
	double wavelen[20] = {0.350,0.400,0.412,0.443,0.470,0.488,0.515,0.550,
		0.590,0.633,0.670,0.694,0.760,0.860,1.240,1.536,
		1.650,1.950,2.250,3.750};
	//real part: y = A1*exp(-x/t1) + y0	
	double a, y0,t1;
	a  = 1052.08872;
	y0 = 1.5592;
	t1 = 0.04342;

	//imaginary part: y = A + B*x + C*x^2 + D*x^3
    double pa,pb,pc,pd;
	pa = 0.03276;
	pb = -0.05498;
	pc = 0.04272;
	pd = -0.00989;

	FILE* fp = fopen("c:\\temp\\6sv_aerosolinput.txt", "w");
	for(int i=0; i<20; i++)
	{
		double mr = a*exp(-wavelen[i]/t1) + y0;
		fprintf(fp, "%6.4lf ", mr);
	}
	fprintf(fp, "\n");
	for(int i=0; i<20; i++)
	{
		double w = wavelen[i];
		double mi = pa+pb*w+pc*w*w+pd*w*w*w;
		fprintf(fp, "%6.4lf ", mi);
	}
	fclose(fp);
	*/
}

void CProject_PGStationDlg::OnFeatureContour()
{
	// TODO: Add your command handler code here

	srand(1);

	CvMemStorage* storage = cvCreateMemStorage(0);
	CvSeq* contours = 0;

	IplImage* pImage = CloneCurrentImage();	
	IplImage* pColor = CloneCurrentImageColor();	

	if(pImage!=NULL)
	{
		cvSmooth(pImage, pImage);
		//cvSaveImage("c:\\temp\\smooth.jpg", pImage);

		cvCanny(pImage, pImage, 10, 80);
		//cvSaveImage("c:\\temp\\canny.jpg", pImage);
		
		int Nc = cvFindContours(pImage, storage, &contours, sizeof(CvContour), CV_RETR_EXTERNAL);
        
		/*
		if(contours)
		{
			int r = (double)( rand() ) / (double)(RAND_MAX) * 255;
			int g = (double)( rand() ) / (double)(RAND_MAX) * 255;
			int b = (double)( rand() ) / (double)(RAND_MAX) * 255;
			printf("%d %d %d \n", r,g,b);

			//cvDrawContours(pImage, contours, cvScalarAll(255), cvScalarAll(255), 100);
			cvDrawContours(pColor, contours, CV_RGB(r,g,b), CV_RGB(r,g,b), 100);

			cvNamedWindow("Contours");
			cvShowImage("Contours", pColor);
		}
		*/


		int n=0; 
		printf("Total Contours Detectec:\\temp %d \n", Nc ); 
		for( CvSeq* c=contours; c!=NULL; c=c->h_next ) 
		{ 
			int r = (double)( rand() ) / (double)(RAND_MAX) * 255;
			int g = (double)( rand() ) / (double)(RAND_MAX) * 255;
			int b = (double)( rand() ) / (double)(RAND_MAX) * 255;
			printf("%d %d %d \n", r,g,b);
			cvDrawContours( pColor, c, CV_RGB(r,g,b), CV_RGB(r,g,b),0,1,8);
		}

		//cvSaveImage("c:\\temp\\contour.jpg", pColor);

		cvNamedWindow("Contours");
		cvShowImage("Contours", pColor);

		cvReleaseImage(&pImage);
	}
}

typedef complex<double> COMPLEX_TYPE;

void CProject_PGStationDlg::OnStdVector()
{
	// TODO: Add your command handler code here

	/*
	complex<double> m;
	m._Val[0] = 1;
	m._Val[1] = 2;
	cout<<m<<endl;
	complex<double> x(1, -1);
	cout<<(m+x)<<endl;

	cout<<"conj: "<<stc:\\temp:conj(x)<<endl;

    //
	complex<double> t(10,0);
	cout<<"multiple:"<<(m*t) <<endl;

	vector<COMPLEX_TYPE> carray(10);
	carray[8]._Val[0] = 1;
	carray[8]._Val[1] = -1;

	for(int i=0; i<carray.size(); i++)
		cout<<carray[i]<<endl;

	COMPLEX_TYPE t1 = COMPLEX_TYPE(1,-2)+m;
    cout<<t1<<endl;
	*/

	//test vector processing

	vector<int> array; 
	array.push_back(1); 
	array.push_back(2); 
	array.push_back(3); 
	array.push_back(4); 
	array.push_back(5);

	
	vector<int>::iterator itr = array.begin(); 
	while( itr != array.end() ) 
	{ 
		if (*itr == 3)
			array.erase(itr);//删除值为3的元素 
		++itr; 
	}
	

	array.erase( array.begin()+1 );


	printf("%d \n", array.size());


	vector<int> a1;
	vector<int> a2;

	a1.push_back(1);
	a1.push_back(2);
	a1.push_back(1);

	a2.push_back(-1);
	a2.push_back(-2);
	a2.push_back(3);


	//vector<int> a3 = a1+a2;




	
}

void CProject_PGStationDlg::OnAerosolMie()
{
	// TODO: Add your command handler code here

	int i,j;
    //input:
	
	double minr = 0.05;
	double maxr = 15;
	int    numIntegPoints = 500;
	int maxLegen = 300;
	stParticleDistribution sizeDistribution;
	sizeDistribution.mode = 2;
	

	//type 1: 0.219  0.531  0.153  2.724  0.583  0.131
	sizeDistribution.medianRadius[0]  =	0.219	 ;//0.162;//0.219; //0.176 ;//= 0.219;
	sizeDistribution.stdVariance[0]   =	0.531	 ;//0.538;//0.531;//0.517 ;//=  0.531;
	sizeDistribution.concentration[0] =	0.153	 ;//0.062;//0.153;//;0.073;//=  0.153;
	sizeDistribution.medianRadius[1]  =	2.724	 ;//2.286;//2.724;//2.768 ;//=  2.724;
	sizeDistribution.stdVariance[1]   =	0.583	 ;//0.594;//0.583;//0.623 ;//=  0.583;
	sizeDistribution.concentration[1] =	0.131	 ;//0.346;//0.131;//0.117;//=   0.131;
	/*
	double wavelen = 0.49;    
	COMPLEX_TYPE msphere;
	msphere._Val[0] = 1.47;   
	msphere._Val[1] = -0.0119; 
	*/
	double wavelen = 0.66;    
	COMPLEX_TYPE msphere;
	msphere._Val[0] = 1.48;   
	msphere._Val[1] = -0.0086; 

	/*
	//read parameters from file
	CFileDialog dlg(TRUE,NULL,NULL,
		OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_ALLOWMULTISELECT|OFN_EXPLORER,
		_T("text file (*.txt)|*.txt|"));
    CString strPath;
	if(dlg.DoModal()==IDOK)
	{
		strPath = dlg.GetPathName();
	}
	else
	{
		return;
	}

	char filepath[512];
	strcpy(filepath, strPath);
	FILE* fp = fopen(filepath, "r");
	if(fp!=NULL)
	{
		fscanf(fp, "%lf", &wavelen);
		fscanf(fp, "%lf %lf", &(msphere._Val[0]), &(msphere._Val[1]) );
		fscanf(fp, "%lf %lf %lf %lf %lf %lf", 
			&(sizeDistribution.medianRadius[0]),
			&(sizeDistribution.stdVariance[0]),
			&(sizeDistribution.concentration[0]),
			&(sizeDistribution.medianRadius[1]),
			&(sizeDistribution.stdVariance[1]),
			&(sizeDistribution.concentration[1]) 
			);
		fclose(fp);
	}
	*/

	//output
	double extinction, scatter,albedo;
	int    nLegen;
	double* coef1 = (double*)malloc(MAXN*sizeof(double));
	double* coef2 = (double*)malloc(MAXN*sizeof(double));
	double* coef3 = (double*)malloc(MAXN*sizeof(double));
	double* coef4 = (double*)malloc(MAXN*sizeof(double));

	MIE( wavelen, msphere, minr, maxr, numIntegPoints, maxLegen, sizeDistribution, 
		 extinction, scatter, albedo, nLegen, coef1, coef2, coef3, coef4);	

	printf("Legendre parameters are written into 'c:\\temp\\mieLegendre.txt': \n");
	FILE* fp = fopen("c:\\temp\\mieLegendre.txt", "w");
	fprintf(fp, "%lf \n", scatter);
	fprintf(fp, "%lf \n", extinction);
	fprintf(fp, "%lf %d \n", albedo, nLegen);
	for(i=1; i<=nLegen; i++)
	{
		fprintf(fp, "%lf %lf %lf %lf \n", coef1[i], coef2[i], coef3[i], coef4[i]);
		printf("%lf %lf %lf %lf \n", coef1[i], coef2[i], coef3[i], coef4[i]);
	}
	printf("\n");
	fclose(fp);

	//convert to phase function
	double f11[181];
	memset(f11, 0, sizeof(double)*181);
	LegendreToPhase(f11, coef1+1, nLegen);

	double f12[181];
	memset(f12, 0, sizeof(double)*181);
	LegendreToPhase(f12, coef2+1, nLegen);
	
	fp = fopen("c:\\temp\\miePhase.txt", "w");
	for( i=0; i<181; i++)
	{
		fprintf(fp, "%8.4lf  %8.4lf\n", f11[i], -f12[i]/f11[i]);
		printf("%lf %lf \n", f11[i], f12[i]);
	}
	fclose(fp);

    printf("Legendre coefficients are written into 'c:\\temp\\mieLegendre.txt': \n");
	printf("Phase function is written into 'c:\\temp\\miePhase.txt': \n");

	free(coef1);
	free(coef2);
	free(coef3);
	free(coef4);
}

void CProject_PGStationDlg::OnBeltJudgedirection()
{
	// TODO: Add your command handler code here

	int i,j;
	IplImage* pImage = CloneCurrentImage();	
	IplImage* pColor = CloneCurrentImageColor();
	if(pImage!=NULL)
	{
		
		cvSmooth(pImage, pImage, CV_GAUSSIAN, 5);

		/*
		int ht = pImage->height;
		int wd = pImage->width;
		int scanWd = pImage->widthStep;
		//sobel gradient		
		IplImage* pSobel = cvCloneImage(pImage);
		cvSobel(pImage, pSobel, 1, 0);
		//cvSmooth(pSobel, pSobel, CV_MEDIAN);

		//vertical edge detection		
		IplImage* pEdge = cvCloneImage(pSobel);
		memset(pEdge->imageData, 0, ht*scanWd);
		for(int j=0; j<ht; j++)
			for(int i=2; i<wd-2; i++)
			{
				int c = (unsigned char)( pSobel->imageData[j*scanWd+i] );
				int l = (unsigned char)( pSobel->imageData[j*scanWd+i-1] );
				int r = (unsigned char)( pSobel->imageData[j*scanWd+i+1] );

				if( c>30 )
				{					
					if( c>l && c>=r)
					{
						pEdge->imageData[j*scanWd+i] = -1;
					}
					else
					{
						pEdge->imageData[j*scanWd+i] = 0;
					}					
				}
				else
				{
					pEdge->imageData[j*scanWd+i] = 0;
				}
			}
		cvNamedWindow("Edge");
		cvShowImage("Edge", pEdge);		
		*/

		IplImage* pEdge = cvCloneImage(pImage);
		VerticalEdgeDetect(pEdge, 16);
		cvSaveImage("c:\\temp\\verticalEdge.jpg", pEdge);

		//contour detection
		CvMemStorage* storage = cvCreateMemStorage(0);
		CvSeq* contours = 0;
		int Nc = cvFindContours(pEdge, storage, &contours);
		int n=0; 
		int nMaxCountourPt = Nc*max( pImage->height, pImage->width );
		CvPoint* pContourPts = (CvPoint*)malloc( nMaxCountourPt*sizeof(CvPoint) );
		int nCount = 0;
		printf("Total Contours Detectec:\\temp %d \n", Nc ); 
		for( CvSeq* c=contours; c!=NULL; c=c->h_next ) 
		{ 
			if( c->total<8 )
				continue;

			//calculate the center of contour
			CvPoint cpt;
			cpt.x = 0;
			cpt.y = 0;
			for( i=0; i<c->total; i++)
			{
				CvPoint* p = CV_GET_SEQ_ELEM(CvPoint, c, i);
				cpt.x += p->x;
				cpt.y += p->y;
				//cvDrawCircle(pColor, *p, 1, CV_RGB(255,0,0));
			}
			cpt.x /= c->total;
			cpt.y /= c->total;
			
			//translation
			for( i=0; i<c->total; i++)
			{
				CvPoint* p = CV_GET_SEQ_ELEM(CvPoint, c, i);
                pContourPts[nCount].x = p->x - cpt.x;
				pContourPts[nCount].y = p->y - cpt.y;
				nCount++;
			}

			//cvDrawCircle(pColor, cpt, 3, CV_RGB(255,0,0));
			/*
			for(int i=0; i<c->total; i++)
			{
				CvPoint* p = CV_GET_SEQ_ELEM(CvPoint, c, i);
				cvDrawCircle(pColor, *p, 1, CV_RGB(255,0,0));
			}
			*/

			printf("contour points: %d \n", c->total);		
			
			//int r = (double)( rand() ) / (double)(RAND_MAX) * 255;
			//int g = (double)( rand() ) / (double)(RAND_MAX) * 255;
			//int b = (double)( rand() ) / (double)(RAND_MAX) * 255;
			//printf("%d %d %d \n", r,g,b);
			//cvDrawContours( pColor, c, CV_RGB(r,g,b), CV_RGB(r,g,b), 0, 1, 8);			
			cvDrawContours( pColor, c, CV_RGB(255,0,0), CV_RGB(255,0,0), 0, 1, 8);			
		}
		
		//line fitting
		CvMat pointMat = cvMat(1, nCount, CV_32SC2, pContourPts);
		float line[4];
		cvFitLine(&pointMat, CV_DIST_L1, 1, 0.001, 0.001, line);		
		double d = sqrt((double)line[0]*line[0] + (double)line[1]*line[1]);
		line[0] /= d;
		line[1] /= d;
		printf("dx: %lf  dy:%lf \n", line[0], line[1]);
		double angle = atan( line[0]/line[1] );
		printf("angle: %lf  sin(angle): %lf \n", angle/PI*180, sin(angle));
        CvPoint spt,ept;
		spt.x = pColor->width*0.5;
		spt.y = 0;
		ept.x = spt.x + pColor->height*(line[0]/line[1]);
		ept.y = pColor->height;
		cvDrawLine(pColor, spt, ept, CV_RGB(255,0,0), 2);
		
		
		//look for principal components (using PCA)
		

		
		cvSaveImage("c:\\temp\\contour.jpg", pColor);
		cvNamedWindow("Contours");
		cvShowImage("Contours", pColor);    		
   
		//cvReleaseImage(&pSobel);
		free(pContourPts);
	}
}

void CProject_PGStationDlg::OnParasolRead()
{
	// TODO: Add your command handler code here

	CFileDialog dlg(TRUE,NULL,NULL,
		OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_ALLOWMULTISELECT|OFN_EXPLORER,
		_T(" files (*.*)|*.*|"));
	CString strPath;
	if(dlg.DoModal()==IDOK)
	{
		strPath = dlg.GetPathName();
	}
	else
	{
		return;
	}

	//
	char leadFile[512] = "F:\\Data\\Parasol\\2005\\05\\24\\P3L1TBG1011149JL";
	
	CReasParasolLevel1File readParasol;
	strcpy(leadFile, strPath);
	readParasol.Load(leadFile);

	printf("year:%s  month:%s  day:%s  hour:%s \n", readParasol.GetYear(), 
		readParasol.GetMonth(), readParasol.GetDay(), readParasol.GetHour());

	stMultiView multiData;
	double lon,lat;	
	//Beijing AERONET location
	lon = 116.381;
	lat = 39.977;
	multiData = readParasol.GetPtMultiViewData(lon, lat);
	

	/*
	stMultiView mdata;
	int line = 901;
	int col  = 4846;
	mdata = readParasol.GetPtMultiViewData(line, col);
	
	stMultiView mdata;
	int line = 901;
	int col  = 4846;
	for(int i=-2; i<=2; i++)
	{
		mdata = readParasol.GetPtMultiViewData(line, col+i);
	}

	line = 900;
	col  = 4845;
	for(int i=-2; i<=2; i++)
	{
		readParasol.GetPtMultiViewData(line, col+i);
	}

	line = 899;
	col  = 4844;
	for(int i=-2; i<=2; i++)
	{
		readParasol.GetPtMultiViewData(line, col+i);
	}

	line = 902;
	col  = 4847;
	for(int i=-2; i<=2; i++)
	{
		readParasol.GetPtMultiViewData(line, col+i);
	}

	line = 903;
	col  = 4848;
	for(int i=-2; i<=2; i++)
	{
		readParasol.GetPtMultiViewData(line, col+i);
	}
	*/
	
	printf("Finished ! \n");	
}

void CProject_PGStationDlg::OnTriangleCtinclass()
{
	// TODO: Add your command handler code here
	
	srand(0);
	
	int npt = 100000;
    double* px = (double*)malloc(npt*sizeof(double));
	double* py = (double*)malloc(npt*sizeof(double));
	for(int i=0; i<npt; i++)
	{
		px[i] = (double)(rand()) / (double)(RAND_MAX) * 1000;
		py[i] = (double)(rand()) / (double)(RAND_MAX) * 1000;
	}

	//test the capability of CTinClass
	printf("Generate Tin ... \n");
	CTINClass* pTin = new CTINClass("aaa");
	GenerateDelaunayTriangle(pTin, px, py, npt);

	//IplImage* pImage = cvCloneImage(m_imagePointerVector[m_nCurrentIndex]->GetCvArr());
    IplImage* pImage = cvCreateImage( cvSize(1000,1000), 8, 3);
	DrawDelaunayTriangle(pImage, pTin);
    printf("Finished! \n");

	cvNamedWindow("Tin");
	cvShowImage("Tin", pImage);	

	free(px);
	free(py);
	cvReleaseImage(&pImage);
	delete pTin;
}

void CProject_PGStationDlg::OnTriangleTinlib()
{
	// TODO: Add your command handler code here

	//test tinlib 

}

void CProject_PGStationDlg::OnBrdfRoujean()
{
	// TODO: Add your command handler code here

	//brdf roujean model
	double thes = 25.0*DPI;
	double thev = 60.955500*DPI;
    double phi  = 37.032000*DPI;
    
	//0.053	0.001	0.247
	double k0 = 0.315;
	double k1 = 0.064;
	double k2 = 0.476;

	double brdf = RoujeanBRDF(k0, k1, k2, thes, thev, phi);

	printf("brdf: %lf \n", brdf);

}

void CProject_PGStationDlg::OnTransmittanceExperimental()
{
	// TODO: Add your command handler code here

	//experiment function for transmission calculation
    double moculAod = 0;
	double aerosolAod = 0;
    double angstrom = 1.5;
    


}

void CProject_PGStationDlg::OnTransmittanceBasedonluts()
{
	// TODO: Add your command handler code here    
	char clut0[256] = "c:\\temp\\Data\\LUTEastAsia\\Double_865_176_517_73_2768_623_117_1558_10_0.dat";
	char clut1[256] = "c:\\temp\\Data\\LUTEastAsia\\Double_865_176_517_73_2768_623_117_1558_10_10.dat";
	char clut2[256] = "c:\\temp\\Data\\LUTEastAsia\\Double_865_176_517_73_2768_623_117_1558_10_25.dat";

	double* pLut0 = (double*)malloc(sizeof(double)*LUT_DIM);
	double* pLut1 = (double*)malloc(sizeof(double)*LUT_DIM);
	double* pLut2 = (double*)malloc(sizeof(double)*LUT_DIM);

	//load lut
	LoadSingleLUT(clut0, pLut0, LUT_DIM);
	LoadSingleLUT(clut1, pLut1, LUT_DIM);
	LoadSingleLUT(clut2, pLut2, LUT_DIM);

	int nModeDim = AOD_NUM*SZA_NUM*NUMMU*NUMAZIMUTHS;
	int nDirDim  = SZA_NUM*NUMMU*NUMAZIMUTHS;
	int nViewDim = NUMMU*NUMAZIMUTHS;
	int nRazMode = NUMAZIMUTHS;   
    
	//input geometry conditions
	double sza = 50;
	double vza = 45;//61;
	double azi = 100;//;37;
	double aod = 0.14;

	//searching LUT
	double ra,r1,r2;
	double pra,pr1,pr2;
	GetLutValue(0, aod, sza, vza, azi, pLut0, nModeDim, nDirDim, nViewDim, nRazMode, &pra, &ra);
	GetLutValue(0, aod, sza, vza, azi, pLut1, nModeDim, nDirDim, nViewDim, nRazMode, &pr1, &r1);
	GetLutValue(0, aod, sza, vza, azi, pLut2, nModeDim, nDirDim, nViewDim, nRazMode, &pr2, &r2);
	    
	//calculate the transmission and atmosphere backscattering ratio
	double transmission, backRatio;
	CalcTransAndBackRatio(ra, r1, r2, 0.1, 0.25, &transmission, &backRatio);

	printf("Transmission: %lf  backRatio:%lf \n", transmission, backRatio);

	free(pLut0);
	free(pLut1);
	free(pLut2);
}
void CProject_PGStationDlg::OnAtmospherecorrectionRt3()
{
	// TODO: Add your command handler code here


	// TODO: Add your command handler code here    
	char clut0[256] = "c:\\temp\\Data\\LUTEastAsia\\Double_865_176_517_73_2768_623_117_1558_10_0.dat";
	char clut1[256] = "c:\\temp\\Data\\LUTEastAsia\\Double_865_176_517_73_2768_623_117_1558_10_10.dat";
	char clut2[256] = "c:\\temp\\Data\\LUTEastAsia\\Double_865_176_517_73_2768_623_117_1558_10_25.dat";

	
	//input geometry conditions
	double sza = 25;
	double vza = 61;
	double azi = 37;
	double aod = 0.34;


	/*
	double* pLut0 = (double*)malloc(sizeof(double)*LUT_DIM);
	double* pLut1 = (double*)malloc(sizeof(double)*LUT_DIM);
	double* pLut2 = (double*)malloc(sizeof(double)*LUT_DIM);

	//load lut
	LoadSingleLUT(clut0, pLut0, LUT_DIM);
	LoadSingleLUT(clut1, pLut1, LUT_DIM);
	LoadSingleLUT(clut2, pLut2, LUT_DIM);

	int nModeDim = AOD_NUM*SZA_NUM*NUMMU*NUMAZIMUTHS;
	int nDirDim  = SZA_NUM*NUMMU*NUMAZIMUTHS;
	int nViewDim = NUMMU*NUMAZIMUTHS;
	int nRazMode = NUMAZIMUTHS;   
	
	//searching LUT
	double ra,r1,r2;
	double pra,pr1,pr2;
	GetLutValue(0, aod, sza, vza, azi, pLut0, nModeDim, nDirDim, nViewDim, nRazMode, &pra, &ra);
	GetLutValue(0, aod, sza, vza, azi, pLut1, nModeDim, nDirDim, nViewDim, nRazMode, &pr1, &r1);
	GetLutValue(0, aod, sza, vza, azi, pLut2, nModeDim, nDirDim, nViewDim, nRazMode, &pr2, &r2);

	//calculate the transmission and atmosphere backscattering ratio
	double transmission, backRatio;
	CalcTransAndBackRatio(ra, r1, r2, 0.1, 0.25, &transmission, &backRatio);
	printf("Transmission: %lf  backRatio:%lf \n", transmission, backRatio);

	//atmosphere correction
	double apparentReflectance = 0.2;
	double cr = (apparentReflectance-ra)/( (apparentReflectance-ra)*backRatio+transmission );
	printf("Atmosphere Correction: %lf \n", cr);

	free(pLut0);
	free(pLut1);
	free(pLut2);
	*/

	//based on the object
	printf("Atmospheric Correction using object ...... \n");
	CAtmosphereCorrectionBase* pAC = new CAtmosphereCorrectionRt3();
	pAC->LoadLut(clut0, clut1, clut2, 0.865);
	pAC->Correct(sza, vza, azi, aod, 0.2, 0);
}

void CProject_PGStationDlg::OnBpdfBreon()
{
	// TODO: Add your command handler code here
	double sza = 25;
	double vza = 61;
	double azi = 37;
	double rp = BPDF_Breon(sza, vza, azi, 0.5);
	printf("Polarized Reflectance: %lf \n", rp);
}

void CProject_PGStationDlg::OnSimulationUsingrt3()
{
	// TODO: Add your command handler code here    
	
	//for 865nm
	//char clut0[256] = "c:\\temp\\Data\\LUTEastAsia\\Double_865_176_517_73_2768_623_117_1558_10_0.dat";
	//char clut1[256] = "c:\\temp\\Data\\LUTEastAsia\\Double_865_176_517_73_2768_623_117_1558_10_10.dat";
	//char clut2[256] = "c:\\temp\\Data\\LUTEastAsia\\Double_865_176_517_73_2768_623_117_1558_10_25.dat";

	//for 670nm
	//char clut0[256] = "c:\\temp\\Data\\LUTEastAsia\\Double_670_176_517_73_2768_623_117_1559_12_0.dat";
	//char clut1[256] = "c:\\temp\\Data\\LUTEastAsia\\Double_670_176_517_73_2768_623_117_1559_12_10.dat";
	//char clut2[256] = "c:\\temp\\Data\\LUTEastAsia\\Double_670_176_517_73_2768_623_117_1559_12_25.dat";

	//for 490nm
	char clut0[256] = "c:\\temp\\Data\\LUTEastAsia\\Double_490_176_517_73_2768_623_117_1572_14_0.dat";
	char clut1[256] = "c:\\temp\\Data\\LUTEastAsia\\Double_490_176_517_73_2768_623_117_1572_14_10.dat";
	char clut2[256] = "c:\\temp\\Data\\LUTEastAsia\\Double_490_176_517_73_2768_623_117_1572_14_25.dat";

	double* pLut0 = (double*)malloc(sizeof(double)*LUT_DIM);
	double* pLut1 = (double*)malloc(sizeof(double)*LUT_DIM);
	double* pLut2 = (double*)malloc(sizeof(double)*LUT_DIM);

	//load lut
	LoadSingleLUT(clut0, pLut0, LUT_DIM);
	LoadSingleLUT(clut1, pLut1, LUT_DIM);
	LoadSingleLUT(clut2, pLut2, LUT_DIM);

	int nModeDim = AOD_NUM*SZA_NUM*NUMMU*NUMAZIMUTHS;
	int nDirDim  = SZA_NUM*NUMMU*NUMAZIMUTHS;
	int nViewDim = NUMMU*NUMAZIMUTHS;
	int nRazMode = NUMAZIMUTHS;   

	//input geometry conditions
	double sza = 25;
	double vza = 61;
	double azi = 37;
	double aod490 = 0.729;
	double aod670 = 0.459;
	double aod865 = 0.318;
	
	//read geometry file
	char geofile[512] = "c:\\temp\\NetDisk\\doc\\papers\\Aerosol\\revise4\\data\\multiViewData.txt";
    stMultiView mdata;

	mdata.nView = GetFileRows(geofile);
	FILE* fp = fopen(geofile, "r");
	for(int i=0; i<	mdata.nView; i++)
	{
		fscanf( fp, "%lf %lf %lf %lf %lf   %lf %lf %lf   %lf %lf %lf", 
			&(mdata.mViewData[i].sza), 
			&(mdata.mViewData[i].vza),
			&(mdata.mViewData[i].rawAzi),
			&(mdata.mViewData[i].azi), 
			&(mdata.mViewData[i].sca), 
			&(mdata.mViewData[i].r4),
			&(mdata.mViewData[i].r6),
			&(mdata.mViewData[i].r8),
			&(mdata.mViewData[i].rp4),
			&(mdata.mViewData[i].rp6),
			&(mdata.mViewData[i].rp8) 
			);
	}
	fclose(fp);

	//generate simulated TOA
	double alfa = 0.01;
	double beita = 80;
	double k0 = 0.053;
	double k1 = 0.001;
	double k2 = 0.247;

	fp = fopen("c:\\temp\\simulate.txt", "w");
	for(int i=0; i<mdata.nView; i++)
	{
		//searching LUT
		double ra,r1,r2;
		double pra,pr1,pr2;
		sza = mdata.mViewData[i].sza;
		vza = mdata.mViewData[i].vza;
		azi = mdata.mViewData[i].azi;
		GetLutValue(0, aod490, sza, vza, azi, pLut0, nModeDim, nDirDim, nViewDim, nRazMode, &pra, &ra);
		GetLutValue(0, aod490, sza, vza, azi, pLut1, nModeDim, nDirDim, nViewDim, nRazMode, &pr1, &r1);
		GetLutValue(0, aod490, sza, vza, azi, pLut2, nModeDim, nDirDim, nViewDim, nRazMode, &pr2, &r2);

		//calculate the transmission and atmosphere backscattering ratio
		double transmission, backRatio;
		CalcTransAndBackRatio(ra, r1, r2, 0.1, 0.25, &transmission, &backRatio);
		//printf("Transmission: %lf  backRatio:%lf \n", transmission, backRatio);
				
		/*
		//polarized reflectance
		double pr = BPDF_Nadal(sza, vza, azi, alfa, beita);
		double toaPr = pra + (pr/(1-pr*backRatio))*transmission;
		printf("Trans: %lf  backR:%lf pr:%lf path:%lf toa: %lf \n", transmission, backRatio, pr, pra, toaPr);     
		fprintf(fp, "%lf %lf \n", mdata.mViewData[i].sca, toaPr);
		*/

		//reflectance	
		double surfR = RoujeanBRDF(k0, k1, k2, sza*DPI, vza*DPI, azi*DPI);
		double toaR = ra + (surfR/(1-surfR*backRatio))*transmission;
		printf("Trans: %lf  backR:%lf pr:%lf path:%lf toa: %lf \n", transmission, backRatio, surfR, ra, toaR);     
		fprintf(fp, "%lf %lf \n", mdata.mViewData[i].sca, toaR);
	}
	fclose(fp);
}

void CProject_PGStationDlg::OnSimulateUsing6sv()
{
	// TODO: Add your command handler code here

	//find the bin directory
	char binPath[512];
	strcpy(binPath, m_binPath);
	strcat(binPath, "Project_6sv.exe");
	
	//generate the input file
	double sza, vza, azi;
	double aod550;
	double waveLeft,waveRight;
    double k0,k1,k2;
	char inputfile[512];
	strcpy(inputfile, m_binPath);
	strcat(inputfile, "input.txt");

	double mr[20] = {};
	double mi[20] = {};

	FILE* fp = fopen(inputfile, "w");

	fclose(fp);

}

void CProject_PGStationDlg::OnAerosolLegendretophase()
{
	// TODO: Add your command handler code here
    //char miefile[512] = "c:\\temp\\Data\\LUTEastAsia\\mie_Double_865_176_517_73_2768_623_117_1558_10_0.sca"; 
	//char tmfile[512] = "c:\\temp\\legendre1.txt";
	char tmfile[512] = "c:\\temp\\Work\\IRSA\\Programs\\Fortran\\myTMatrix\\Project_TMatrix\\tmatr.write"; // result from T-Matrix 
	int i;
	int    index;
	double ssa;
	char   cline[1024];
	double F1[300];
	double F2[300];
	double F3[300];
	double F4[300];
	double F5[300];
	double F6[300];
	
	int nLegen = 19;

	FILE* fp = fopen(tmfile, "r");
	/*
	for(i=0; i<4; i++)
	{
		fgets(cline, 1024, fp);
	}	
	fscanf(fp, "%d", &nLegen);
    */
	fgets(cline, 1024, fp);
	fscanf(fp, "%lf %d", &ssa, &nLegen);
	for(i=0; i<nLegen; i++)
	{
		fscanf(fp, "%d %lf %lf %lf %lf %lf %lf", &index, 
			&(F1[i]), &(F2[i]),&(F3[i]),&(F4[i]),&(F5[i]),&(F6[i]));
	}
	fclose(fp);	

	double phase[180];
	LegendreToPhase(phase, F1, nLegen);
	char output[256] = "c:\\temp\\tmPhase.txt";
	fp = fopen(output, "w");
	for(i=0; i<180; i++)
	{
		fprintf(fp, "%lf \n", phase[i]);
		printf("%lf \n", phase[i]);
	}
	fclose(fp);
	printf("\n");

	printf("input: %s \n output: %s \n", tmfile, output);
}

void CProject_PGStationDlg::OnSuperresolutionPocs()
{
	// TODO: Add your command handler code here

	char pixelFile[512] = "E:\\SoftProject\\PGStation\\Bin\\pixel.txt";

	int npt = GetFileRows(pixelFile);
	printf("point number: %d \n", npt);

	double* x = (double*)malloc(npt*sizeof(double));
	double* y = (double*)malloc(npt*sizeof(double));
	int* r = (int*)malloc(npt*sizeof(int));
	int* g = (int*)malloc(npt*sizeof(int));
	int* b = (int*)malloc(npt*sizeof(int));

	FILE* fp = fopen(pixelFile, "r");
	if(fp==NULL)
	{
		printf("File open failed ! \n");
	}
	for(int i=0; i<npt; i++)
	{
		fscanf(fp, "%lf %lf %d %d %d ", x+i, y+i, r+i, g+i, b+i);
	}
	fclose(fp);
    
	POCS(x, y, r, npt, "c:\\temp\\r.bmp");
	POCS(x, y, g, npt, "c:\\temp\\g.bmp");
	POCS(x, y, b, npt, "c:\\temp\\b.bmp");
     

	free(x);
	free(y);
	free(r);
	free(g);
	free(b);
}

void CProject_PGStationDlg::OnFilterCustom()
{
	// TODO: Add your command handler code here

	IplImage* pImage = CloneCurrentImage();

	if(pImage!=NULL)
	{
		//filter kernel
		double kernel[25] = {   0.0119, 0,      0.0476, 0,      0.0119,
								0,      0.0476, 0.0952, 0.0476, 0,
								0.0476, 0.0952, 0.1905, 0.0952, 0.0476,
								0,      0.0476, 0.0952, 0.0476, 0,
								0.0119, 0,      0.0476, 0,      0.0119};
		int    kht = 5;
		int    kwd = 5;   

		unsigned char* pBuffer = NULL;
		int ht,wd;
		IplImageToGrayImage(pImage, &pBuffer, &ht, &wd);
		SaveBmp("c:\\temp\\src.bmp", pBuffer,  ht, wd);

		Filter2D(pBuffer, ht, wd, kernel, kht, kwd);
	}
}

//Difference of Gaussian
void CProject_PGStationDlg::OnScalespaceDog()
{
	// TODO: Add your command handler code here

}

void CProject_PGStationDlg::OnHjAerosolinvert()
{

	// TODO: Add your command handler code here
	char xmlpath[512] = "c:\\temp\\Data\\HJ\\Beijing\\HJ1A-CCD1-457-66-20100402-L20000277541\\277541\\HJ1A-CCD1-457-66-20100402-L20000277541.XML";
	char outpath[512] = "c:\\temp\\Data\\aod.tif";
	
	//opendialog
	//read parameters from file
	CFileDialog dlg(TRUE,NULL,NULL,
		OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_ALLOWMULTISELECT|OFN_EXPLORER,
		_T("text file (*.xml)|*.xml|"));
	CString strPath;
	if(dlg.DoModal()==IDOK)
	{
		strPath = dlg.GetPathName();
	}
	else
	{
		return;
	}

	//char filepath[512];
	strcpy(xmlpath, strPath);
    
	//invert
	printf("input : %s \n", xmlpath);
	printf("output : %s \n", outpath);
	//HJAerosolInvert(xmlpath, outpath, m_binPath);
}

void CProject_PGStationDlg::OnFileClear()
{
	// TODO: Add your command handler code here

	cvDestroyAllWindows();

	m_fileList.DeleteAllItems();
	
	//release image object buffer
	for(int i=0; i<m_imagePointerVector.size(); i++)
	{
		delete m_imagePointerVector[i];
	}
	m_fileVector.clear();
	m_imagePointerVector.clear();
}

void CProject_PGStationDlg::OnSobelGradient()
{
	// TODO: Add your command handler code here

	IplImage* pImage = CloneCurrentImage();
	if(pImage!=NULL)
	{
		unsigned char* pbuffer = NULL;
		int ht, wd;
		IplImageToGrayImage(pImage, &pbuffer, &ht, &wd);

		double* dx = (double*)malloc(ht*wd*sizeof(double));
		double* dy = (double*)malloc(ht*wd*sizeof(double));
		
		GenerateSobelGradient(pbuffer, ht, wd, dx, dy);

		int i,j;
		for(j=0; j<ht; j++)
			for(i=0; i<wd; i++)
			{
				int index = j*wd+i;
				pbuffer[index] = min( 255, sqrt(dx[index]*dx[index]+dy[index]*dy[index]) );
			}

		printf("Image is saved into:  c:\\temp\\gradientImage.bmp \n");
		SaveBmp("c:\\temp\\gradientImage.bmp", pbuffer, ht, wd);	
		
		cvReleaseImage(&pImage);
		free(pbuffer);
		free(dx);
		free(dy);
	}
}

void CProject_PGStationDlg::OnMathAtan2()
{
	// TODO: Add your command handler code here

	double dx = 0;
	double dy = -100;

	printf("dx: %lf dy: %lf  atan2: %lf \n ", dx, dy, atan2(dy, dx) );

}

void CProject_PGStationDlg::OnVehiclelogoLbp()
{
	// TODO: Add your command handler code here
   
	//cvNamedWindow("Image");

	int  i,j;
	char imagepath[256] = "c:\\temp\\Data\\Logo\\classification\\vw";
	//char imagepath[256] = "c:\\temp\\Data\\Logo\\all1";
	char** filenames = NULL;
	int n,nfile;
	n = nfile = 0;
    
	GetDirFileName(filenames, imagepath, &n, &nfile, "jpg", 0);
    filenames = f2c(nfile, 256);
	GetDirFileName(filenames, imagepath, &n, &nfile, "jpg", 1);
    
	IplImage* pStdImage = cvCreateImage(cvSize(48, 48), 8, 1);

	printf("Number: %d \n", nfile);
	for( i=0; i<nfile; i++)
	{
		//printf("%s \n", filenames[i]);
	}

	//pair match
	FILE* fp = fopen("c:\\temp\\sim.txt", "w");
	for(j=0; j<nfile; j++)
	{		
		double s = 0;
		for(i=0; i<nfile; i++)
		{
			IplImage* pPatch = cvLoadImage(filenames[j], 0);
			//resize
			cvResize(pPatch, pStdImage);
			//cvShowImage("Image", pStdImage);

			unsigned char* pbuffer = NULL;
			int ht,wd;
			IplImageToGrayImage(pStdImage, &pbuffer, &ht, &wd);
			double lbpHist1[256];
			GenerateLBP(pbuffer, ht, wd, lbpHist1);
			free(pbuffer);
			cvReleaseImage(&pPatch);

			pPatch = cvLoadImage(filenames[i], 0);
			//resize
			cvResize(pPatch, pStdImage);
			IplImageToGrayImage(pStdImage, &pbuffer, &ht, &wd);
			double lbpHist2[256];
			GenerateLBP(pbuffer, ht, wd, lbpHist2);
			free(pbuffer);
			cvReleaseImage(&pPatch);

			/*
			char output[256];
			sprintf(output, "c:\\temp\\%d_%d.txt", j,i);
			FILE* fout = fopen(output, "w");
			for(int mi=0; mi<256; mi++)
			{
				fprintf(fout, "%lf %lf \n", lbpHist1[mi], lbpHist2[mi]);
			}
			fclose(fout);
			*/

			//lbp similarity
			//matching using LBP			
			double sim = HistSimilar(lbpHist1, lbpHist2, 256, 0);
			//sim = HistSimilar(lbpHist1, lbpHist1, 256, 3);
			//printf("%lf ",sim);
			fprintf(fp, "%lf ",sim);

			if(j!=i)
			   s += sim;
		}
		printf("%s %lf \n", filenames[j],  s/(nfile-1) );
		fprintf(fp, "\n");
	}
	fclose(fp);
}


void CProject_PGStationDlg::OnLpbHistgram()
{
	// TODO: Add your command handler code here

	IplImage* pStdImage = cvCreateImage(cvSize(48, 48), 8, 1);

	//calculate the LBP histograms of two image and the distance between them
	if( m_imagePointerVector.size()>1 )
	{		
		IplImage* pPatch =m_imagePointerVector[0]->GetCvArr();

		//resize
		cvResize(pPatch, pStdImage);
		cvSaveImage("c:\\temp\\lLogo.jpg", pStdImage);
		//cvShowImage("Image", pStdImage);
		unsigned char* pbuffer = NULL;
		int ht,wd;
		IplImageToGrayImage(pStdImage, &pbuffer, &ht, &wd);
		double lbpHist1[256];
		GenerateLBP(pbuffer, ht, wd, lbpHist1);
		free(pbuffer);
		//cvReleaseImage(&pPatch);

		//cvNamedWindow("Left");
		//cvShowImage("Left", pStdImage);

		pPatch = m_imagePointerVector[1]->GetCvArr();
		//resize
		cvResize(pPatch, pStdImage);
		cvSaveImage("c:\\temp\\rLogo.jpg", pStdImage);
		IplImageToGrayImage(pStdImage, &pbuffer, &ht, &wd);
		double lbpHist2[256];
		GenerateLBP(pbuffer, ht, wd, lbpHist2);
		free(pbuffer);
		//cvReleaseImage(&pPatch);

		//cvNamedWindow("Right");
		//cvShowImage("Right", pStdImage);
		
		char output[256] = "c:\\temp\\lbpHist.txt";
		//sprintf(output, "c:\\temp\\%d_%d.txt", j,i);
		FILE* fout = fopen(output, "w");
		for(int mi=0; mi<256; mi++)
		{
			fprintf(fout, "%lf %lf \n", lbpHist1[mi], lbpHist2[mi]);
		}
		fclose(fout);
		
		//lbp similarity
		//matching using LBP			
		double sim = HistSimilar(lbpHist1, lbpHist2, 256, 0);
		//sim = HistSimilar(lbpHist1, lbpHist1, 256, 3);
		//printf("%lf ",sim);
		printf("%lf ",sim);		
	}

	cvReleaseImage(&pStdImage);
}

void CProject_PGStationDlg::OnVehiclelogoTemplatematching()
{
	// TODO: Add your command handler code here
    
	int  hist[500];
	memset(hist, 0, 500*sizeof(int));

	char templatepath[512] = "c:\\temp\\Data\\Logo\\All";
	char testPath[512] = "c:\\temp\\Data\\Logo\\classification\\vw";

	char** templateFiles = NULL;
	int n1,nfile1;
	n1 = nfile1 = 0;
	GetDirFileName(templateFiles, templatepath, &n1, &nfile1, "jpg", 0);
	templateFiles = f2c(nfile1, 256);
	GetDirFileName(templateFiles, templatepath, &n1, &nfile1, "jpg", 1);
	

	char** filenames = NULL;
	int n,nfile;
	n = nfile = 0;
	GetDirFileName(filenames, testPath, &n, &nfile, "jpg", 0);
	filenames = f2c(nfile, 256);
	GetDirFileName(filenames, testPath, &n, &nfile, "jpg", 1);

	int i,j,k;

	IplImage* src;
	IplImage* dst;
	IplImage* nsrc = cvCreateImage(cvSize(48, 48), 8, 1);
	IplImage* ndst = cvCreateImage(cvSize(48, 48), 8, 1);
	for(i=0; i<nfile; i++)
	{
		src = cvLoadImage( filenames[i], 0 );
		cvResize(src, nsrc);

		double mindif = 10000000;
		int    index = 0;
		for(j=0; j<nfile1; j++)
		{
			dst = cvLoadImage( templateFiles[j], 0 );
			cvResize(dst, ndst);

			double  dif = ImageSimilarity(nsrc, ndst);
			//double  dif = LBPSimilarity(nsrc, ndst);
			if(mindif>dif)
			{
				mindif = dif;
				index = j;
			}
			cvReleaseImage(&dst);
		}
		
		printf("\n %s \n", filenames[i]);
		printf(" %s %lf \n", templateFiles[index], mindif);
		hist[index]++;

		cvReleaseImage(&src);
	}
    
	for(i=0; i<nfile1; i++)
	{
		printf("%d ", hist[i]);
	}
}

void CProject_PGStationDlg::OnLpbTemplatematching()
{
	// TODO: Add your command handler code here

	IplImage* pStdImage1 = cvCreateImage(cvSize(48, 48), 8, 1);
	IplImage* pStdImage2 = cvCreateImage(cvSize(48, 48), 8, 1);

	if( m_imagePointerVector.size()>1 )
	{		
		//resize
		IplImage* pPatch = m_imagePointerVector[0]->GetCvArr();
		cvResize(pPatch, pStdImage1);
		cvSaveImage("c:\\temp\\lLogo.jpg", pStdImage1);

		//resize
		pPatch = m_imagePointerVector[1]->GetCvArr();
		cvResize(pPatch, pStdImage2);
		cvSaveImage("c:\\temp\\lLogo.jpg", pStdImage2);

		double dif = ImageSimilarity(pStdImage1, pStdImage2);
		//double dif = LBPSimilarity(pStdImage1, pStdImage2);

		printf("\n dif: %lf \n", dif);

		cvNamedWindow("Left");
		cvNamedWindow("Right");
		cvShowImage("Left", pStdImage1);	
		cvShowImage("Right", pStdImage2);	
	}

}

void CProject_PGStationDlg::OnEigenobjectVehiclelog()
{
	// TODO: Add your command handler code here

	//generate eigen-object of vehicle logo
	

}

void CProject_PGStationDlg::OnLpbImage()
{
	// TODO: Add your command handler code here

	IplImage* pImage = CloneCurrentImage();
	if(pImage!=NULL)
	{

		IplImage* pStdImage = cvCreateImage(cvSize(48, 48), 8, 1);
		cvResize(pImage, pStdImage);

		unsigned char* pbuffer = NULL;
		int ht,wd;
		IplImageToGrayImage(pStdImage, &pbuffer, &ht, &wd);
			
		double lbphist[256];
		memset(lbphist, 0, sizeof(double)*256);
		GenerateLBP(pbuffer, ht, wd, lbphist);
	
		FILE* fp = fopen("c:\\temp\\lbphist.txt", "w");   
		for(int i=0; i<256; i++)
		{
			printf("%d %lf \n", i, lbphist[i]);
			fprintf(fp, "%lf \n", lbphist[i]);
		}
		fclose(fp);

		free(pbuffer);
		cvReleaseImage(&pStdImage);
	}

	
}



void CProject_PGStationDlg::OnTemplatematchingVehiclelogo()
{
	// TODO: Add your command handler code here

	IplImage* pStdImage1 = cvCreateImage(cvSize(48, 48), 8, 1);
	IplImage* pStdImage2 = cvCreateImage(cvSize(48, 48), 8, 1);

	if( m_imagePointerVector.size()>1 )
	{		
		//resize
		IplImage* pPatch = m_imagePointerVector[0]->GetCvArr();
		cvResize(pPatch, pStdImage1);
		cvSaveImage("c:\\temp\\lLogo.jpg", pStdImage1);

		//resize
		pPatch = m_imagePointerVector[1]->GetCvArr();
		cvResize(pPatch, pStdImage2);
		cvSaveImage("c:\\temp\\lLogo.jpg", pStdImage2);

		double dif = ImageSimilarity(pStdImage1, pStdImage2);
		printf("\n dif: %lf \n", dif);

		cvNamedWindow("Left");
		cvNamedWindow("Right");
		cvShowImage("Left", pStdImage1);	
		cvShowImage("Right", pStdImage2);	
	}

}

void CProject_PGStationDlg::OnSizedistributionGenerate()
{
	// TODO: Add your command handler code here

	double radius[22] = {0.05,0.0656,0.08608,0.11294,
		0.14818,0.19443,0.25511,0.33472,0.43917,
		0.57623,0.75605,0.992,1.30157,1.70776,
		2.2407,2.93997,3.85745,5.06126,6.64074,
		8.71315,11.43229,15};

	stParticleDistribution pd;
	pd.mode = 2;

	//0.219  0.531  0.153  2.724  0.583  0.131 // type 1
	//0.176  0.517  0.073  2.768  0.623  0.117 // 2005.5.24
	//0.062  0.538 0.162 0.346 0.594 2.286     // type 5
	
	/*
	pd.medianRadius[0]  = 0.219;//0.162;  //; //0.176 ;//= 0.219;
	pd.stdVariance[0]   = 0.531; //0.538; //;//0.517 ;//= 0.531;
	pd.concentration[0] = 0.153*0.669;    //0.062;//;//;0.073;//= 0.153;
	pd.medianRadius[1]  = 2.724; //2.286; //;//2.768 ;//= 2.724;
	pd.stdVariance[1]   = 0.583; //0.594; //;//0.623 ;//= 0.583;
	pd.concentration[1] = 0.131*0.669;    //0.346;//;//0.117;//= 0.131;
	*/

	//0.176000	2.768000
	//0.517000	0.623000
	//0.073000	0.117000

	/*
	pd.medianRadius[0]  = 0.176;//0.162;//; //0.176 ;//= 0.219;
	pd.stdVariance[0]   = 0.517; //0.538;//;//0.517 ;//= 0.531;
	pd.concentration[0] = 0.073;//0.062;//;//;0.073;//= 0.153;
	pd.medianRadius[1]  = 2.768; //2.286;//;//2.768 ;//= 2.724;
	pd.stdVariance[1]   = 0.623; //0.594;//;//0.623 ;//= 0.583;
	pd.concentration[1] = 0.117;//0.346;//;//0.117;//= 0.131;
	*/

	pd.medianRadius[0]  = 0.177;//0.162;//; //0.176 ;//= 0.219;
	pd.stdVariance[0]   = 0.474; //0.5387;//;//0.517 ;//= 0.531;
	pd.concentration[0] = 0.070;//0.062;//;//;0.073;//= 0.153;
	pd.medianRadius[1]  = 2.265; //2.286;//;//2.768 ;//= 2.724;
	pd.stdVariance[1]   = 0.656; //0.594;//;//0.623 ;//= 0.583;
	pd.concentration[1] = 0.091;//0.346;//;//0.117;//= 0.131;


	double pdf[22];
	BimodalLogDistribution(pd, radius, pdf, 22);

	FILE* fp = fopen("c:\\temp\\aerosol_size.txt", "w");
	for(int i=0; i<22; i++)
	{
		fprintf(fp, "%lf %lf \n", radius[i], pdf[i]);
		printf("%lf %lf \n", radius[i], pdf[i]);
	}
	fclose(fp);
}

void CProject_PGStationDlg::OnParasolSinglepointinversion()
{
	// TODO: Add your command handler code here
	
	//load lut data
	int    modetypeNumber = 6;
	double ext[6][3];
	int    bandw[3]={4,6,8};
	double** ptypeLut = NULL;
	int      nSingLutDim = AOD_NUM*SZA_NUM*NUMMU*NUMAZIMUTHS*2;
	ptypeLut = f2d(modetypeNumber*3, nSingLutDim);	
	char lutpath[1024];
	sprintf(lutpath,"%s\\lut\\type",m_binPath);	
	printf("%s\n",lutpath);
	LoadMixedLut(lutpath, ptypeLut, modetypeNumber, nSingLutDim, ext );	
	printf("LoadMixedLut \n");


	//load single point multiple-angle data
	char mfile[256] = "c:\\temp\\NetDisk\\doc\\papers\\Aerosol\\revise4\\data\\simulate_multiViewData.txt";
	
	//using open dialog to select file
	char filepath[256];
	CFileDialog dlg(TRUE,NULL,NULL,
		OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_ALLOWMULTISELECT|OFN_EXPLORER,
		_T("text file (*.txt)|*.txt|"));
	CString strPath;
	if(dlg.DoModal()==IDOK)
	{
		strPath = dlg.GetPathName();
	}
	else
	{
		return;
	}
    
    strcpy(mfile, strPath);
	stMultiView mdata;
	LoadParasolMultiDataSingle(mfile, mdata);

	//invert using 490nm reflectance
	double aod4, aod6, aod8;
	PtInvert(mdata, ptypeLut, nSingLutDim, 6, ext, 10, &aod4, &aod6, &aod8);

	//invert using polarized reflectance
	double faod4, faod6, faod8;
	InvertAODWithConstraint(mdata, ptypeLut, nSingLutDim, 6, ext, 10, aod4, 
		&faod4, &faod6, &faod8);

	printf("aod 490: %lf \n", aod4);
}

void CProject_PGStationDlg::OnBrdfRossLi()
{
	// TODO: Add your command handler code here

	int i,j;
	//0.246   0.054   0.526
	double k0 = 0.246;
	double k1 = 0.054;
	double k2 = 0.526;

	double sza = 25;
	double vza = 0;
	double azi = 0;
	double sca;
	double ref = 0;

	int nstep = 5;

	//principle plane
	FILE* fp = fopen("c:\\temp\\rossli-brdf.txt", "w");
	for(i=60; i>0; i-=nstep)
	{	
		vza = i;				
		sca = CalcScatterAngle(sza, vza, azi);
		ref = RossLiBRDF(k0, k1, k2, sza, vza, azi);
		fprintf(fp, "%lf %lf \n", sca, ref);
		printf("%lf %lf \n", sca, ref);
	}

	azi = 180;
	for(i=0; i<60; i+=nstep)
	{
		vza = i;				
		sca = CalcScatterAngle(sza, vza, azi);
		ref = RossLiBRDF(k0, k1, k2, sza, vza, azi);
		fprintf(fp, "%lf %lf \n", sca, ref);	
		printf("%lf %lf \n", sca, ref);
	}
	fclose(fp);
}

void CProject_PGStationDlg::OnBpdfNadal()
{
	// TODO: Add your command handler code here

	int i,j;
	double alfa  = 0.0095;
	double beita = 90;
	double sza = 25;
	double vza = 0;
	double azi = 0;
	double sca;
	double ref = 0;

	int nstep = 5;

	//principle plane
	FILE* fp = fopen("c:\\temp\\Nadal_bpdf.txt", "w");
	for(i=60; i>0; i-=nstep)
	{	
		vza = i;				
		sca = CalcScatterAngle(sza, vza, azi);
		//ref = RossLiBRDF(k0, k1, k2, sza, vza, azi);
		ref = BPDF_Nadal(sza, vza, azi, alfa, beita);
		fprintf(fp, "%lf %lf \n", sca, ref);
		printf("%lf %lf \n", sca, ref);
	}

	azi = 180;
	for(i=0; i<60; i+=nstep)
	{
		vza = i;				
		sca = CalcScatterAngle(sza, vza, azi);
		//ref = RossLiBRDF(k0, k1, k2, sza, vza, azi);
		ref = BPDF_Nadal(sza, vza, azi, alfa, beita);
		fprintf(fp, "%lf %lf \n", sca, ref);	
		printf("%lf %lf \n", sca, ref);
	}
	fclose(fp);
}

void CProject_PGStationDlg::OnLutRetrieve()
{
	// TODO: Add your command handler code here
	int i,j;
	int nModeDim = AOD_NUM*SZA_NUM*NUMMU*NUMAZIMUTHS;
	int nDirDim  = SZA_NUM*NUMMU*NUMAZIMUTHS;
	int nViewDim = NUMMU*NUMAZIMUTHS;
	int nRazMode = NUMAZIMUTHS;   

	//load lut
	//char lutfile[256] = "c:\\temp\\Data\\LUTDouble\\type\\1\\4.dat";
	//char lutfile[256] = "c:\\temp\\Data\\LUTEastAsia\\Double_670_176_517_73_2768_623_117_1559_12_0.dat";
	char lutfile[256] = "c:\\temp\\Data\\LUTEastAsia\\Double_690_176_517_73_2768_623_117_1559_12_0.dat";
	int nLutDim = AOD_NUM*SZA_NUM*NUMMU*NUMAZIMUTHS*2;
	double * pLut = (double*)malloc(sizeof(double)*nLutDim);
	LoadSingleLUT(lutfile, pLut, nLutDim);

	//load single point multiple-angle data
	char mfile[256] = "c:\\temp\\NetDisk\\doc\\papers\\Aerosol\\revise4\\data\\simulate_multiViewData.txt";
	stMultiView m_multiData;
	LoadParasolMultiDataSingle(mfile, m_multiData);


	FILE* fp = fopen("c:\\temp\\aerosol690.txt", "w");
	double fr,frp;
	double aod = 0.4;	
	for(int i=0; i<m_multiData.nView; i++)
	{
		double sza = m_multiData.mViewData[i].sza;
		double vza = m_multiData.mViewData[i].vza;
		double azi = m_multiData.mViewData[i].azi;

		double frp_mocule, fr_mocule;
		double value = GetLutValue( 0, 0, sza,vza,azi,
			pLut,nModeDim, nDirDim, nViewDim, nRazMode, &frp_mocule, &fr_mocule);

		double frp_aerosol, fr_aerosol;
		value = GetLutValue( 0, aod, sza,vza,azi,
			pLut,nModeDim, nDirDim, nViewDim, nRazMode, &frp_aerosol, &fr_aerosol);
		
		
		fprintf(fp, "%lf %lf %lf\n", m_multiData.mViewData[i].sca, frp_aerosol, frp_aerosol-frp_mocule); 
		printf("%lf %lf %lf\n", m_multiData.mViewData[i].sca, frp_aerosol, frp_aerosol-frp_mocule);
	}
	fclose(fp);

	
	/*
	//sort according to the reflectance, added by xdh, 2013.5.5, from small to large
	for(j=0; j<m_multiData.nView; j++)
	{
		for(i=j+1; i<m_multiData.nView; i++)
		{
			stOneView one;
			if( m_multiData.mViewData[j].r4 > m_multiData.mViewData[i].r4  )
			{
				memcpy( &one, &(m_multiData.mViewData[j]), sizeof(stOneView) );
				memcpy( &(m_multiData.mViewData[j]), &(m_multiData.mViewData[i]), sizeof(stOneView) );
				memcpy( &(m_multiData.mViewData[i]), &one, sizeof(stOneView) );				
			}
		}
	}
	*/
	

	/*
	FILE* fp = fopen("c:\\temp\\invert_out_490.txt", "w");
	double fr,frp;
	double aod = 0.72;
	double angstrom = 1.3;
	double k0 = 0.053;
	double k1 = 0.001;
	double k2 = 0.247;
	for(int i=0; i<m_multiData.nView; i++)
	{
		double sza = m_multiData.mViewData[i].sza;
		double vza = m_multiData.mViewData[i].vza;
		double azi = m_multiData.mViewData[i].azi;
		double value = GetLutValue( 0, aod, sza,vza,azi,
			pLut,nModeDim, nDirDim, nViewDim, nRazMode, &frp, &fr);

		double tdown4  = DownTransition(0.490, aod, angstrom, sza);	
		double tup4    = DownTransition(0.490, aod, angstrom, vza);

		double surfR = RoujeanBRDF(k0, k1, k2, sza*DPI, vza*DPI, azi*DPI);
		fr += surfR*tdown4*tup4*1.5;

		fprintf(fp, "%lf %lf \n", m_multiData.mViewData[i].sca, fr);
		printf("%lf %lf \n", m_multiData.mViewData[i].sca, fr);
	}
	fclose(fp);
	*/
}

void CProject_PGStationDlg::OnLutRetrivemoculeandaerosol()
{
	// TODO: Add your command handler code here

	int i,j;
	int nModeDim = AOD_NUM*SZA_NUM*NUMMU*NUMAZIMUTHS;
	int nDirDim  = SZA_NUM*NUMMU*NUMAZIMUTHS;
	int nViewDim = NUMMU*NUMAZIMUTHS;
	int nRazMode = NUMAZIMUTHS;   

	//load lut
	char lutfile[256] = "c:\\temp\\Data\\LUTDouble\\type\\2\\4.dat";
	int nLutDim = AOD_NUM*SZA_NUM*NUMMU*NUMAZIMUTHS*2;
	double * pLut = (double*)malloc(sizeof(double)*nLutDim);
	LoadSingleLUT(lutfile, pLut, nLutDim);


	double aod = 0.386;
	int nstep = 5;
	double sca, sza, vza, azi;
	//principle plane
	FILE* fp = fopen("c:\\temp\\lut_retrieve.txt", "w");

    sza = 45;
	azi = 180;
	for(i=0; i<=70; i+=nstep)
	{   
		vza = i;				
		sca = CalcScatterAngle(sza, vza, azi);
		//ref = RossLiBRDF(k0, k1, k2, sza, vza, azi);
		//ref = BPDF_Nadal(sza, vza, azi, alfa, beita);

		double frp_mocule, fr_mocule;
		double value = GetLutValue( 0, 0, sza,vza,azi,
			pLut,nModeDim, nDirDim, nViewDim, nRazMode, &frp_mocule, &fr_mocule);

		double frp_aerosol, fr_aerosol;
		value = GetLutValue( 0, aod, sza,vza,azi,
			pLut,nModeDim, nDirDim, nViewDim, nRazMode, &frp_aerosol, &fr_aerosol);
		
		fprintf(fp, "%lf %lf %lf\n", sca, frp_mocule, frp_aerosol-frp_mocule);	
		//printf("%lf %lf \n", sca, ref);
		//printf("%d \n", i);
		printf("%lf %lf %lf\n", sca, frp_mocule, frp_aerosol-frp_mocule);	
	}
	fclose(fp);
}

void CProject_PGStationDlg::OnMosaicSeam()
{
	// TODO: Add your command handler code here


	//the algorithm to find the seam line 




}

void CProject_PGStationDlg::OnTriangleVoronoi()
{
	// TODO: Add your command handler code here

	char win[] = "source";
	int i;
	CvRect rect = { 0, 0, 600, 600 };
	CvMemStorage* storage;
	CvSubdiv2D* subdiv;
	IplImage* img;
	CvScalar active_facet_color, delaunay_color, voronoi_color, bkgnd_color;
	CvPoint2D32f  pts[4];
	pts[0].x = 100;	pts[0].y = 300;
	pts[1].x = 400;	pts[1].y = 300;
	pts[2].x = 100;	pts[2].y = 400;
	pts[3].x = 400;	pts[3].y = 500;


	active_facet_color = CV_RGB( 255, 0, 0 );
	delaunay_color  = CV_RGB( 0,0,0);
	voronoi_color = CV_RGB(0, 180, 0);
	bkgnd_color = CV_RGB(255,255,255);

	img = cvCreateImage( cvSize(rect.width,rect.height), 8, 3 );
	cvSet( img, bkgnd_color, 0 );

	cvNamedWindow( win, 1 );

	storage = cvCreateMemStorage(0);
	subdiv = init_delaunay( storage, rect );

	printf("Delaunay triangulation will be build now interactively.\n"
		"To stop the process, press any key\n\n");

	for( i=0; i<4; i++ )
	{
		//CvPoint2D32f fp = cvPoint2D32f( (float)(rand()%(rect.width-10)+5),
		//	                            (float)(rand()%(rect.height-10)+5));
		//cvSubdivDelaunay2DInsert( subdiv, fp);

		//locate_point( subdiv, fp, img, active_facet_color );
		//cvShowImage( win, img );
		//if( cvWaitKey( 100 ) >= 0 )
		//	break;
		//cvSubdivDelaunay2DInsert( subdiv, fp );
		//cvCalcSubdivVoronoi2D( subdiv );		
		//cvSet( img, bkgnd_color, 0 );
		//draw_subdiv( img, subdiv, delaunay_color, voronoi_color );
		//cvShowImage( win, img );
		//if( cvWaitKey( 100 ) >= 0 )
		//	break;

		cvSubdivDelaunay2DInsert( subdiv, pts[i] );
	}

	//cvCalcSubdivVoronoi2D( subdiv );		

	cvSet( img, bkgnd_color, 0 );
	paint_voronoi( subdiv, img );
	cvShowImage( win, img );
	
	//cvWaitKey(0);
	cvReleaseMemStorage( &storage );
	cvReleaseImage(&img);
	//cvDestroyWindow( win );
}

void CProject_PGStationDlg::OnPointSiftfloat()
{
	// TODO: Add your command handler code here

	IplImage* pImage = CloneCurrentImage();
	IplImage* pColor = CloneCurrentImageColor();

	if(pImage!=NULL)
	{
		int i,j;

		/*
		int ht = pImage->height;
		int wd = pImage->width;
		int scanwd = pImage->widthStep;
		float* fImage = (float*)malloc(ht*wd*sizeof(float));
        float  scale = 1.0;//1.0/256.0;
		for(j=0; j<ht; j++)
			for(i=0; i<wd; i++)
			{
				fImage[j*wd+i] = float(  (unsigned char)(pImage->imageData[j*scanwd+i]) )*scale;
			}
		*/

		float* fImage;
		int ht,wd;
		IplImageToFloatImage(pImage, &fImage, &ht, &wd);

		printf("Sift Feature detection ... \n");
		int keynumber = 0;
		Key_Point* featPts = SiftFeaturesFloat(fImage, wd, ht, keynumber);

		printf("Feature Number: %d \n", keynumber);

		for(i=0; i<keynumber; i++)
		{
			CvScalar color = CV_RGB(255,0,0);
			CvPoint tp;
			tp.x = featPts[i].key_column;
			tp.y = featPts[i].key_row;
			cvLine( pColor, cvPoint( tp.x-5,tp.y), cvPoint( tp.x+5,tp.y),CV_RGB(255,0,255),1,8,0);
			cvLine( pColor, cvPoint( tp.x,tp.y-5), cvPoint( tp.x,tp.y+5),CV_RGB(255,0,255),1,8,0);		
		}
        
		cvNamedWindow("Sift");
		cvShowImage("Sift", pColor);

		cvSaveImage("c:\\temp\\sift_float.bmp", pColor);

		//cvWaitKey(0);

		cvReleaseImage(&pImage);
		cvReleaseImage(&pColor);
	}
}

void CProject_PGStationDlg::OnTrackingKlt()
{
	// TODO: Add your command handler code here
	if( m_imagePointerVector.size()>1 )
	{		
		CKLTTracking kltTracking;
		vector<CvPoint> prePts;
		vector<CvPoint> curPts;
		
		//initialization from first frame
		kltTracking.Predict(m_imagePointerVector[0]->GetCvArr(), prePts, curPts);

		//detect feature points
		IplImage* eig = cvCreateImage( cvGetSize(m_imagePointerVector[0]->GetCvArr()), 32, 1 );
		IplImage* temp = cvCreateImage( cvGetSize(m_imagePointerVector[0]->GetCvArr()), 32, 1 );
		double quality = 0.01;
		double min_distance = 10;
		int win_size = 10;
		CvPoint2D32f* points = (CvPoint2D32f*)malloc(MAX_COUNT*sizeof(CvPoint2D32f));
		CvPoint2D32f* npoints = (CvPoint2D32f*)malloc(MAX_COUNT*sizeof(CvPoint2D32f));        
		int count = MAX_COUNT;
		cvGoodFeaturesToTrack( m_imagePointerVector[0]->GetCvArr(), eig, temp, points, &count,
			quality, min_distance, 0, 3, 0, 0.04 );

		//tracking
	}
}

void CProject_PGStationDlg::OnTriangleDelaunay()
{
	// TODO: Add your command handler code here

	// TODO: Add your command handler code here
	if(m_nCurrentIndex>=0)
	{
		int     npt;
		double* px;
		double* py;
		int     i;

		npt = m_featPtVector.size();

		if(npt<1)
			return;

		px = (double*)malloc(npt*sizeof(double));
		py = (double*)malloc(npt*sizeof(double));

		for(i=0; i<npt; i++)
		{
			px[i] = m_featPtVector[i].x;
			py[i] = m_featPtVector[i].y;
		}

		if(m_pTin!=NULL)
			delete m_pTin;

		m_pTin = new CTINClass("aaa");
		GenerateDelaunayTriangle(m_pTin, px, py, npt);

		//IplImage* pImage = cvCloneImage(m_imagePointerVector[m_nCurrentIndex]->GetCvArr());
		IplImage* pImage = CloneCurrentImageColor();

		DrawDelaunayTriangle(pImage, m_pTin);

		cvNamedWindow("Triangle");
		cvSetMouseCallback("Triangle", on_triangle_mouse, this);
		cvShowImage("Triangle", pImage);

		cvSaveImage("c:\\temp\\triangle.png", pImage);

		cvReleaseImage(&pImage);    
		free(px);
		free(py);
	}
}

void CProject_PGStationDlg::OnSfmSimulate()
{
	// TODO: Add your command handler code here

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
	GenerateRMatrix(omiga, phi, kapa, R2);
    t2[0] = 20; t2[1] = 20; t2[2] = 10;
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

	int numpt = 20;
	FILE* fp = fopen("f:\\data\\sfm\\simulatePair.txt", "w");
	fprintf(fp, "%d\n", numpt);
	for(int i=0; i<numpt; i++)
	{
		srand(time(0));

		double ix1,iy1;
		GrdToImg(grdpt[i][0], grdpt[i][1], grdpt[i][2], &ix1, &iy1, 
			R1, t1, focus, x0, y0, ht, wd );

		double ix2,iy2;
		GrdToImg(grdpt[i][0], grdpt[i][1], grdpt[i][2], &ix2, &iy2, 
			R2, t2, focus, x0, y0, ht, wd );

		printf("%lf %lf  %lf %lf \n", ix1, iy1, ix2, iy2);

		//random noise
		double rx1 = ( (double)(rand())/(double)(RAND_MAX) - 0.5)*2;
		double ry1 = ( (double)(rand())/(double)(RAND_MAX) - 0.5)*2;
		double rx2 = ( (double)(rand())/(double)(RAND_MAX) - 0.5)*2;
		double ry2 = ( (double)(rand())/(double)(RAND_MAX) - 0.5)*2;
		
		//coordinate translation from topleft to the center
		fprintf(fp, "%lf %lf  %lf %lf \n", ix1-wd*0.5+rx1, ht*0.5-iy1+ry1, ix2-wd*0.5+rx2, ht*0.5-iy2+ry2);
	}
	fclose(fp);
}

/*
//GPU running
void CProject_PGStationDlg::OnSobelMem()
{
	// TODO: Add your command handler code here

	IplImage* pImage = CloneCurrentImage();
	//IplImage* pColor = CloneCurrentImageColor();

	if(pImage!=NULL)
	{

		unsigned char* pbuffer;
		int ht,wd;

		IplImageToGrayImage(pImage, &pbuffer, &ht, &wd);

		unsigned char* pDst = (unsigned char*)malloc(ht*wd);
				
		//cuda initialize
		findCapableDevice();
		
		///sobel edge using CUDA
		SobelImage(pbuffer, ht, wd, pDst, ht, wd);

		SaveBmp("c:\\temp\\sobel.bmp", pDst, ht, wd);		

		printf("Finished !\n");
	}
}
*/

void CProject_PGStationDlg::OnAtmospherecorrectionGui()
{
	// TODO: Add your command handler code here

}

void CProject_PGStationDlg::OnKltLostobj()
{
	// TODO: Add your command handler code here
	char imagepath[256] = "c:\\temp\\Data\\video\\lost";

	char** filenames = NULL;
	int n,nfile;
	n = nfile = 0;

	GetDirFileName(filenames, imagepath, &n, &nfile, "jpg", 0);
	filenames = f2c(nfile, 256);
	GetDirFileName(filenames, imagepath, &n, &nfile, "jpg", 1);

	cvNamedWindow("Image");

	//tracked objects
	CKLTTrack kltTrack; 
	vector<CTrackObj> trackObjs;
	int i,j,k,m;
	for(int i=0; i<nfile; i++)
	{		
		IplImage* pImage = cvLoadImage(filenames[i], 0);

		//1. detect features	
		IplImage* eig = cvCreateImage( cvGetSize(pImage), 32, 1 );
		IplImage* temp = cvCreateImage( cvGetSize(pImage), 32, 1 );
		double quality = 0.01;
		double min_distance = 10;
		int win_size = 10;
		CvPoint2D32f* points = (CvPoint2D32f*)malloc(MAX_COUNT*sizeof(CvPoint2D32f));
		int count = MAX_COUNT;
		cvGoodFeaturesToTrack( pImage, eig, temp, points, &count,
			quality, min_distance, 0, 3, 0, 0.04 );
		cvReleaseImage( &eig );
		cvReleaseImage( &temp );

		//2. add features into the list
		vector<CvPoint2D32f> newFeatPts;
		for(k=0; k<count; k++)
		{
			bool bIsNew = true;
			for(m=0; m<trackObjs.size(); m++)
			{
				//calculate the distance between feature points and tracked object
				CvPoint2D32f tpt = trackObjs[m].GetCurPt();

				double dis = sqrt(  (points[k].x-tpt.x)*(points[k].x-tpt.x) 
					              + (points[k].y-tpt.y)*(points[k].y-tpt.y) );

				if(dis<5) 
				{
					bIsNew = false;
					break;
				}
			}
			if(bIsNew)
			{
				//trackObjs.push_back(points[k]);
				newFeatPts.push_back(points[k]);
			}
		}		
		for(k=0; k<newFeatPts.size(); k++)
		{
			CTrackObj* pObj = new CTrackObj();
			pObj->AddLocation(newFeatPts[k]);
			trackObjs.push_back( *pObj );
		}

		//3. tracking
		int nobj = trackObjs.size();
		CvPoint2D32f* cpts = (CvPoint2D32f*)malloc( sizeof(CvPoint2D32f)*nobj );
		CvPoint2D32f* npts = (CvPoint2D32f*)malloc( sizeof(CvPoint2D32f)*nobj );
		int nNewPt=nobj;
		for(k=0; k<nobj; k++)
		{
			cpts[k] = trackObjs[k].GetCurPt();
		}
		
		int bIsSuccess = kltTrack.Predict(pImage, cpts, nobj, npts, &nNewPt);		
		//add the tracked point into the trajectory
		if(bIsSuccess)
		{		
			for(k=0; k<nobj; k++)
			{
				trackObjs[k].AddLocation(npts[k]);
			}
		}
		
		//4. objects filtered
		vector<CTrackObj> filteredTrackObjs;
		for(k=0; k<trackObjs.size(); k++)
		{
			int    numPt  = trackObjs[k].GetTrajectoryPtNumber();
			double len    = trackObjs[k].CalculateTrajectoryLen();
			double curLen = trackObjs[k].CalculateCurrentLen();
			
			//if( numPt>=30 && len<8 ) continue;
			//if( curLen>30) continue;
			//if( numPt>=30 && len>50 ) continue;
			//if( len>80 ) continue;

			double angle = trackObjs[k].CalculateTrajectoryDirection();
			
			//if(curLen<3 && len>10)
			filteredTrackObjs.push_back( trackObjs[k] );
		}
		trackObjs.clear();
		trackObjs = filteredTrackObjs;
		filteredTrackObjs.clear();

		
		// show the trajectory
		for(k=0; k<trackObjs.size(); k++)
		{
			for(m=0; m<trackObjs[k].GetTrajectoryPtNumber()-1; m++)
			{
				CvPoint2D32f p1 = trackObjs[k].GetTrajectoryPt(m);
				CvPoint2D32f p2 = trackObjs[k].GetTrajectoryPt(m+1);

				CvPoint ip1,ip2;
				ip1.x = p1.x;
				ip1.y = p1.y;
				ip2.x = p2.x;
				ip2.y = p2.y;
				cvDrawLine(pImage, ip1, ip2, CV_RGB(255,255,255));
			}
		}

		cvShowImage("Image", pImage);		
		cvWaitKey();

		free(points);
		free(cpts);
		free(npts);

		cvReleaseImage(&pImage);
	}
}

void CProject_PGStationDlg::OnKltFramediff()
{
	// TODO: Add your command handler code here


	//tracking without recording trajectory
	int  i,j,k;
	char filename[256];
	CKLTTrack kltTrack;

	cvNamedWindow("Image");

	for(int i=45; i<700; i++)
	{	

		printf("current frame: %d \n", i);

		sprintf(filename, "c:\\temp\\Data\\video\\bridge\\%.4d.jpg", i);
		IplImage* pImage = cvLoadImage(filename, 0);

		//1. detect features	
		IplImage* eig = cvCreateImage( cvGetSize(pImage), 32, 1 );
		IplImage* temp = cvCreateImage( cvGetSize(pImage), 32, 1 );
		double quality = 0.02;
		double min_distance = 5;
		int win_size = 10;
		CvPoint2D32f* points = (CvPoint2D32f*)malloc(MAX_COUNT*sizeof(CvPoint2D32f));
		CvPoint2D32f* npoints = (CvPoint2D32f*)malloc(MAX_COUNT*sizeof(CvPoint2D32f));
		int count = MAX_COUNT;
		cvGoodFeaturesToTrack( pImage, eig, temp, points, &count,
			quality, min_distance, 0, 3, 0, 0.04 );
		cvReleaseImage( &eig );
		cvReleaseImage( &temp );

		//points[0].x = 214;
		//points[0].y = 105;
        //count = 1;

		//2. tracking between two frames
		int nNewPt = 0;
		int bIsSuccess = kltTrack.Predict(pImage, points, count, npoints, &nNewPt);

		//3. draw direction
		for(j=0; j<count; j++)
		{
			CvPoint p1,p2;
			p1.x = points[j].x;
			p1.y = points[j].y;

			p2.x = npoints[j].x;
			p2.y = npoints[j].y;

			//p2.x = p1.x+(p2.x-p1.x)*6;
			//p2.y = p1.y+(p2.y-p1.y)*6;

			cvCircle(pImage, p1, 1, CV_RGB(255,255,255));
            //cvCircle(pImage, p2, 1, CV_RGB(255,255,255));
			cvLine(pImage, p1, p2, CV_RGB(255,255,255), 1);
		}
		
		cvShowImage("Image", pImage);		
		cvWaitKey();
	}
}

void CProject_PGStationDlg::OnAtmospherecorrectionBat()
{
	// TODO: Add your command handler code here    
}

void CProject_PGStationDlg::OnKltTwoframes()
{
	// TODO: Add your command handler code here

	int i,j, k;

	cvNamedWindow("Left");

	char imagepath[256] = "c:\\temp\\Data\\video\\bridge\\";

	//optical of two frames
	for(k=150; k<700; k++)
	{
		char leftFile[256];//  = "c:\\temp\\Data\\video\\bridge\\0048.jpg";
		char rightFile[256];// = "c:\\temp\\Data\\video\\bridge\\0049.jpg";

		sprintf(leftFile,  "c:\\temp\\Data\\video\\bridge\\%.4d.jpg", k);
		sprintf(rightFile, "c:\\temp\\Data\\video\\bridge\\%.4d.jpg", k+1);

		printf("%d %d \n", k, k+1);

		IplImage* pImage = cvLoadImage(leftFile, 0);	
		IplImage* pCurrent = cvLoadImage(rightFile, 0);	

		int ht = pImage->height;
		int wd = pImage->width;
		int scanwd = pImage->widthStep;

		cvSmooth(pImage, pImage);
		cvSmooth(pCurrent, pCurrent);

		//1. detect features	
		IplImage* eig = cvCreateImage( cvGetSize(pImage), 32, 1 );
		IplImage* temp = cvCreateImage( cvGetSize(pImage), 32, 1 );
		double quality = 0.02;
		double min_distance = 5;
		int win_size = 10;
		CvPoint2D32f* points = (CvPoint2D32f*)malloc(MAX_COUNT*sizeof(CvPoint2D32f));
		int count = MAX_COUNT;
		cvGoodFeaturesToTrack( pImage, eig, temp, points, &count,
			quality, min_distance, 0, 3, 0, 0.04 );
		cvReleaseImage( &eig );
		cvReleaseImage( &temp );

		/*
		cvFindCornerSubPix(
			pImage,
			points,
			count,
			cvSize(win_size,win_size),
			cvSize(-1,-1),
			cvTermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.03)
			);
		*/

		//2. optical flow
		//KLT optical flow tracking
		//CvPoint2D32f* points = (CvPoint2D32f*)malloc(MAX_COUNT*sizeof(CvPoint2D32f));
		CvPoint2D32f* npoints = (CvPoint2D32f*)malloc(MAX_COUNT*sizeof(CvPoint2D32f));
		char* status = (char*)malloc(count);
		IplImage* prev_pyramid = cvCloneImage(pImage);
		IplImage* pyramid = cvCloneImage(pImage);
		float* trackError = (float*)malloc( sizeof(float)*MAX_COUNT );
		memset( trackError, 0, sizeof(float)*MAX_COUNT );

		cvCalcOpticalFlowPyrLK( pImage, pCurrent, prev_pyramid, pyramid, points, npoints, count, cvSize(31,31), 3, 
			status, trackError, cvTermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS, 20, 0.03), 0);

		for(i=0; i<count; i++)
		{
			CvScalar color = CV_RGB(255,0,0);
			CvPoint tp;
			tp.x = points[i].x;
			tp.y = points[i].y;
			CvPoint cp;
			cp.x = npoints[i].x;
			cp.y = npoints[i].y;

			//int s = (unsigned char)(status[i]);

			
			int len = sqrt( (double)( (tp.x-cp.x)*(tp.x-cp.x) + (tp.y-cp.y)*(tp.y-cp.y))  );
			int ns = (unsigned char)(status[i]);
			//printf("%d \n", ns);

			/*
			double patchA[625];
			double patchB[625];
			double dif = 0;		
			int m,n;
			int mi = 0;
			int noff = 9;
			for( m=-noff; m<=noff; m++)
				for(int n=-noff; n<=noff; n++)
				{
					int x = max(0, min(wd-1, n+tp.x));
					int y = max(0, min(ht-1, m+tp.y));				
					int g = (unsigned char)( pImage->imageData[y*scanwd+x] );
					patchA[mi] = g;

					x = max(0, min(wd-1, n+cp.x));
					y = max(0, min(ht-1, m+cp.y));				
					g = (unsigned char)( pCurrent->imageData[y*scanwd+x] );
					patchB[mi] = g;
					mi++;
				}

				
			//histogram similarity
			double suma = 0;
			double sumb = 0;
			for(m=0; m<mi; m++)
			{
				suma += patchA[m];
				sumb += patchB[m];
			}
			for(m=0; m<mi; m++)
			{
				patchA[m] /= suma;
				patchB[m] /= sumb;
			}
			double sim = HistSimilar(patchA, patchB, mi);

			//if( cp.x>tp.x)
			//if(len>80)
			//if(ns>0 && trackError[i]<150)
			if(ns>0 && sim>0.9)*/
			//printf("%d %f \n", ns, trackError[i]);
			//if( ns>0 && trackError[i]<60 )
			if(ns>0)
			{
				//printf("status:%d error:%lf  %d %d  %d  %d \n", ns, trackError[i], tp.x, tp.y, cp.x, cp.y);
				//cvDrawCircle( pCurrent, cvPoint( tp.x,tp.y),3,CV_RGB(255,255,255),2,8,0);
				cvDrawCircle( pCurrent, cvPoint( cp.x,cp.y),2,CV_RGB(255,255,255),1,8,0);
				cvLine( pCurrent, cvPoint( tp.x,tp.y), cvPoint( cp.x, cp.y),CV_RGB(255,255,255),2,8,0);
			}
			else
			{
				cvLine( pCurrent, cvPoint( tp.x,tp.y), cvPoint( cp.x, cp.y),CV_RGB(150,150,150),1,8,0);
			}
		}

		cvShowImage("Left", pCurrent);
		cvWaitKey();
		
		free(npoints);
		free(trackError);
		free(status);
		cvReleaseImage(&pImage);
		cvReleaseImage(&pCurrent);
		cvReleaseImage(&prev_pyramid);
		cvReleaseImage(&pyramid);
	}
}

void CProject_PGStationDlg::OnKltSinglept()
{
	// TODO: Add your command handler code here

	int i,j;
	double patchA[512];
	double patchB[512];

	char leftFile[256] = "c:\\temp\\IMG_9856.jpg";
	char rightFile[256] = "c:\\temp\\IMG_9857.jpg";

	//sprintf(leftFile, "c:\\temp\\Data\\video\\bridge\\%.4d.jpg", k);
	//sprintf(rightFile, "c:\\temp\\Data\\video\\bridge\\%.4d.jpg", k+1);
	//printf("%d %d \n", k, k+1);

	IplImage* pImage   = cvLoadImage(leftFile, 0);	
	IplImage* pCurrent = cvLoadImage(rightFile, 0);	

	int ht = pImage->height;
	int wd = pImage->width;
	int scanwd = pImage->widthStep;

	cvSmooth(pImage, pImage);
	cvSmooth(pCurrent, pCurrent);

	int nLevel = 5;

	//1. detect features		
	CvPoint2D32f* points = (CvPoint2D32f*)malloc(MAX_COUNT*sizeof(CvPoint2D32f));
	int count = 2;
	
	//points[0].x = 317;
	//points[0].y = 269;
	//points[0].x = 192;
	//points[0].y = 510;

	points[0].x = 280; //220
	points[0].y = 230; //354

	points[1].x = 488; //483
	points[1].y = 116; //118

	//2. optical flow
	//KLT optical flow tracking
	//CvPoint2D32f* points = (CvPoint2D32f*)malloc(MAX_COUNT*sizeof(CvPoint2D32f));
	CvPoint2D32f* npoints = (CvPoint2D32f*)malloc(MAX_COUNT*sizeof(CvPoint2D32f));
	char* status = (char*)malloc(count);
	IplImage* prev_pyramid = cvCloneImage(pImage);
	IplImage* pyramid = cvCloneImage(pImage);

	float* trackError = (float*)malloc( sizeof(float)*MAX_COUNT );
	memset( trackError, 0, sizeof(float)*MAX_COUNT );

	cvCalcOpticalFlowPyrLK( pImage, pCurrent, prev_pyramid, pyramid, points, npoints, count, cvSize(31, 31), nLevel, status, 
		trackError,	cvTermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS, 10, 0.03), 0);

	cvDrawCircle( pImage, cvPoint( points[0].x,points[0].y), 5, CV_RGB(255,255,255),2,8,0);
	cvDrawCircle( pCurrent, cvPoint( npoints[0].x,npoints[0].y), 5, CV_RGB(255,255,255),2,8,0);


	/*
	for(i=0; i<count; i++)
	{
		CvScalar color = CV_RGB(255,0,0);
		CvPoint tp;
		tp.x = points[i].x;
		tp.y = points[i].y;
		CvPoint cp;
		cp.x = npoints[i].x;
		cp.y = npoints[i].y;

		int len = sqrt( (double)( (tp.x-cp.x)*(tp.x-cp.x) + (tp.y-cp.y)*(tp.y-cp.y))  );

		int ns = status[i];
		printf("%d \n", ns);

		//retrieve the patch from the image
		double dif = 0;		
		int m,n;
		int mi = 0;
		int noff = 9;
		for( m=-noff; m<=noff; m++)
			for(int n=-noff; n<=noff; n++)
			{
				int x = max(0, min(wd-1, n+tp.x));
				int y = max(0, min(ht-1, m+tp.y));				
				int g = (unsigned char)( pImage->imageData[y*scanwd+x] );
				patchA[mi] = g;
				
				x = max(0, min(wd-1, n+cp.x));
				y = max(0, min(ht-1, m+cp.y));				
				g = (unsigned char)( pCurrent->imageData[y*scanwd+x] );
				patchB[mi] = g;
				mi++;
			}
		
		//difference
		for(m=0; m<mi; m++)
		{
			dif += abs( patchA[m]-patchB[m] );
		}

		//histogram similarity
		double suma = 0;
		double sumb = 0;
		for(m=0; m<mi; m++)
		{
			suma += patchA[m];
			sumb += patchB[m];
		}
		for(m=0; m<mi; m++)
		{
			patchA[m] /= suma;
			patchB[m] /= sumb;
		}
		double sim = HistSimilar(patchA, patchB, mi);
	
		if(ns>0)
		{
			printf("status:%d track error:%lf dif:%lf histogram:%lf  %d %d   %d  %d \n", ns, trackError[i], dif, sim,
				tp.x, tp.y, cp.x, cp.y);			
			
			cvDrawCircle( pImage, cvPoint( tp.x,tp.y), 1, CV_RGB(255,255,255),2,8,0);
			cvDrawCircle( pCurrent, cvPoint( cp.x,cp.y), 1, CV_RGB(255,255,255),2,8,0);
			
			cvLine( pImage, cvPoint( tp.x,tp.y), cvPoint( cp.x, cp.y),CV_RGB(255,255,255),1,8,0);
		}
		//cvLine( pImage, cvPoint( tp.x-5,tp.y), cvPoint( tp.x+5,tp.y),CV_RGB(255,255,255),1,8,0);
		//cvLine( pImage, cvPoint( tp.x,tp.y-5), cvPoint( tp.x,tp.y+5),CV_RGB(255,255,255),1,8,0);		
	}
	*/



	cvSaveImage("c:\\temp\\L.jpg", pImage);
	cvSaveImage("c:\\temp\\R.jpg", pCurrent);

	cvNamedWindow("Left", 0);
	cvShowImage("Left", pImage);

	cvNamedWindow("Right", 0);
	cvShowImage("Right", pCurrent);

	cvWaitKey();
}

void CProject_PGStationDlg::OnBpdfPolarizedfresnel()
{
	// TODO: Add your command handler code here	

	double dtr = 1.0/180.0*3.1415926;

	double sza = 30;
	double vza = 30;
	double phi = 180;


	double sca = CalcScatterAngle(sza, vza, phi);
	double pf  = PolarizedFresnel(sza, vza, phi)/cos(sza*dtr)/cos(vza*dtr);

	printf("%lf %lf %lf  sca: %lf polarized fresnel: %lf \n", sza, vza, phi, sca, pf);

	sza = 50;
	vza = 10;
	phi = 180;
	sca = CalcScatterAngle(sza, vza, phi);
	pf  = PolarizedFresnel(sza, vza, phi)/cos(sza*dtr)/cos(vza*dtr);
	printf("%lf %lf %lf  sca: %lf polarized fresnel: %lf \n", sza, vza, phi, sca, pf);

	//calculate Fresnel using multiple angles
	printf("Array ... \n");
	sza = 60;
	double vzaArray[17] = {80,70,60,50,40,30,20,10,0, 10,20,30,40,50,60,70,80};
	double phiArray[17] = {180,180,180,180,180,180,180,180,180, 0,0,0,0,0,0,0,0};
	for(int i=0; i<17; i++)
	{
		sca = CalcScatterAngle(sza, vzaArray[i], phiArray[i]);
		pf  = PolarizedFresnel(sza, vzaArray[i], phiArray[i]); ///cos(sza*dtr)/cos(vzaArray[i]*dtr);
		printf("%lf %lf %lf  sca: %lf polarized fresnel: %lf \n", sza, vzaArray[i], phiArray[i], sca, pf);
	}
}

void CProject_PGStationDlg::OnBlockmatchingTwoframes()
{
	// TODO: Add your command handler code here
	int i,j;
	char leftFile[256] = "c:\\temp\\Data\\video\\bridge\\0053.jpg";
	char rightFile[256] = "c:\\temp\\Data\\video\\bridge\\0054.jpg";

	//sprintf(leftFile, "c:\\temp\\Data\\video\\bridge\\%.4d.jpg", k);
	//sprintf(rightFile, "c:\\temp\\Data\\video\\bridge\\%.4d.jpg", k+1);
	//printf("%d %d \n", k, k+1);

	IplImage* grayA = cvLoadImage(leftFile, 0);	
	IplImage* grayB = cvLoadImage(rightFile, 0);	

	//cvResize();
	//cvSmooth(pImage, pImage);
	//cvSmooth(pCurrent, pCurrent);
 
	int BS = 4;
	CvSize size = cvGetSize(grayA);
	size.width /= BS;
	size.height /= BS;
	IplImage* result = cvCreateImage(size, IPL_DEPTH_8U, 1);
	for (int i=0; i<size.height; i++) 
	{
		for (int j=0; j<size.width; j++) 
		{
			cvSet(result, CV_RGB(255,255,255), NULL);
		}
	}

	IplImage* velx = cvCreateImage(size, IPL_DEPTH_32F, 1);
	IplImage* vely = cvCreateImage(size, IPL_DEPTH_32F, 1);
	cvCalcOpticalFlowBM(grayA, grayB, cvSize(BS, BS), cvSize(1, 1), cvSize(24, 24), 0, velx, vely);

	cvNamedWindow("HorFlowBM", CV_WINDOW_AUTOSIZE);
	cvShowImage("HorFlowBM", velx);
	cvNamedWindow("VerFlowBM", CV_WINDOW_AUTOSIZE);
	cvShowImage("VerFlowBM", vely);
	
	FILE* fp = fopen("c:\\temp\\blockMatch.txt", "w");
	for (i=0; i<size.height; i+=2) 
	{
		for (int j=0; j<size.width; j+=2) 
		{
			int dx = (int)cvGetReal2D(velx, i, j);
			int dy = (int)cvGetReal2D(vely, i, j);
			fprintf(fp, "%d %c:\\temp %d %d   \n",i,j, dx, dy);
			cvLine(result, cvPoint(j, i), cvPoint(j+dx, i+dy), CV_RGB(0,0,0), 1, 8, 0);
		}
	}
	cvNamedWindow("OpticalFlow", CV_WINDOW_AUTOSIZE);
	cvShowImage("OpticalFlow", result);
	cvWaitKey(0);
}

void CProject_PGStationDlg::OnRegisterSiftFloat()
{
	// TODO: Add your command handler code here

	if( m_imagePointerVector.size()>1 )
	{
		CWaitCursor waitCursor;

		//m_matchPairVector.clear();
		//PairMatch(m_imagePointerVector[0]->GetCvArr(), 
		//	m_imagePointerVector[1]->GetCvArr(),
		//	m_matchPairVector);		

		float* fImage1 = NULL;
		float* fImage2 = NULL;
		int ht1,wd1;
		int ht2,wd2;

		IplImageToFloatImage(m_imagePointerVector[0]->GetCvArr(), &fImage1, &ht1, &wd1);
		IplImageToFloatImage(m_imagePointerVector[1]->GetCvArr(), &fImage2, &ht2, &wd2);
   
		//1. detect feature points
		int nFeat1;
		Key_Point* feat1 = SiftFeaturesFloat(fImage1, wd1, ht1, nFeat1);
		printf("%d \n", nFeat1);

		int nFeat2;
		Key_Point* feat2 = SiftFeaturesFloat(fImage2, wd2, ht2, nFeat2);
		printf("%d \n", nFeat2);

		//2. matching
		MatchPoint* matches;
		int nMatch;
		SiftPairMatch(feat1, nFeat1, feat2, nFeat2, &matches, &nMatch);
		printf("matches: %d \n", nMatch);

		//3. remove the bad matches	

		//display the matches
		CvFont font;	
		cvInitFont( &font, CV_FONT_VECTOR0, 0.5, 0.5, 0, 1, 8 );

		IplImage* pImage1 = m_imagePointerVector[0]->GetCvArrColor();
		IplImage* pImage2 = m_imagePointerVector[1]->GetCvArrColor();

		for(int i=0; i<nMatch; i++)
		{
			CvPoint p1,p2;
			p1.x = matches[i].imageX1;
			p1.y = matches[i].imageY1;
			p2.x = matches[i].imageX2;
			p2.y = matches[i].imageY2;


			char stmp[256];
			sprintf(stmp, "%d", i);
			//cvPutText(pImage1, stmp, cvPoint(p1.x+2, p1.y), &font, CV_RGB(255,255,255)); 		
			//cvPutText(pImage2, stmp, cvPoint(p2.x+2, p2.y), &font, CV_RGB(255,255,255)); 

			cvDrawCircle(pImage1, p1, 1, CV_RGB(255,0,0),2);
			cvDrawCircle(pImage2, p2, 1, CV_RGB(255,0,0),2);
		}
	
		cvNamedWindow("Left");
		cvShowImage("Left", pImage1);

		cvNamedWindow("Right");
		cvShowImage("Right", pImage2);

		delete[] feat1;
		delete[] feat2;
		delete[] matches;
		free(fImage1);
		free(fImage2);
	}
}

void CProject_PGStationDlg::OnDistributionGaussian()
{
	// TODO: Add your command handler code here

	//load single point multiple-angle data
	char mfile[256] = "c:\\temp\\NetDisk\\doc\\papers\\Aerosol\\revise4\\data\\simulate_multiViewData.txt";
	stMultiView mdata;
	LoadParasolMultiDataSingle(mfile, mdata);


	//slope distribution
	FILE* fp = fopen("c:\\temp\\slopeDistribution.txt", "w");
	double sigma = 0.3;
	for(int i=0; i<20; i++)
	{
		double alfa = i*4.0;
		double un = cos( alfa/180.0*PI );
		double pf = 1/(2*PI*sigma*sigma*un*un*un)*exp( -(1-un*un)/(un*un*2*sigma*sigma) );
		//double pf = 1/(un*un*un*un*sigma*sigma)*exp( -(1-un*un)/(un*un*sigma*sigma) );

		printf("%lf %lf %lf \n", alfa, un, pf);
		fprintf(fp, "%lf %lf %lf \n", alfa, un, pf);
		
	}
	fclose(fp);
}

void CProject_PGStationDlg::OnSmokeColor()
{
	// TODO: Add your command handler code here
	
}

void CProject_PGStationDlg::OnTrackFramedifferencing()
{
	// TODO: Add your command handler code here

	if( m_imagePointerVector.size()>1 )
	{
		CWaitCursor waitCursor;
	
		/*
		float* fImage1 = NULL;
		float* fImage2 = NULL;
		int ht1,wd1;
		int ht2,wd2;
		IplImageToFloatImage(m_imagePointerVector[0]->GetCvArr(), &fImage1, &ht1, &wd1);
		IplImageToFloatImage(m_imagePointerVector[1]->GetCvArr(), &fImage2, &ht2, &wd2);
		*/

		if( m_nCurrentIndex>=1)
		{
			IplImage* pPreImage = cvCloneImage( m_imagePointerVector[m_nCurrentIndex-1]->GetCvArr() );
			IplImage* pCurImage = cvCloneImage( m_imagePointerVector[m_nCurrentIndex]->GetCvArr() );
			
			//frame difference
			IplImage* pDif = cvCloneImage(pPreImage);
			cvAbsDiff(pPreImage, pCurImage, pDif);

			//threshold 
			cvThreshold(pDif, pDif, 15, 255, CV_THRESH_BINARY);

			
			cvNamedWindow("Diff");
			cvShowImage("Diff", pDif);

			cvReleaseImage(&pPreImage);
			cvReleaseImage(&pCurImage);
			cvReleaseImage(&pDif);
		}
	}
}

void CProject_PGStationDlg::OnParasolAreainversion()
{
	// TODO: Add your command handler code here

	//invert total AOD and FMF of selected area

	//
	char finepath[256];
	char coarsepath[256];
	char fixedpath[256];
	char igbppath[256];

	//load lut and igbp data
	//string finepath   = binpath + "\\fine";
	//string coarsepath = binpath + "\\coarse";
	//string fixedpath  = binpath + "\\fixed";
	//string igbpfile   = binpath + "\\igbp\\eastAsia.dat";
	//dllLoadLutFineAndCoarse(finepath, coarsepath);
	//dllLoadLutType(fixedpath, 6);
	//dllLoadIGBP(igbpfile); 

	//m_binPath
	strcpy( finepath, m_binPath );
	strcat( finepath, "fine");
	printf("%s \n", finepath);
   
	strcpy( coarsepath, m_binPath );
	strcat( coarsepath, "coarse");
	printf("%s \n", coarsepath);

	strcpy( fixedpath, m_binPath );
	strcat( fixedpath, "fixed");
	printf("%s \n", fixedpath);

	strcpy( igbppath, m_binPath );
	strcat( igbppath, "igbp\\eastAsia.dat");
	printf("%s \n", igbppath);

	//loading support data
	dllLoadLutFineAndCoarse(finepath, coarsepath);	
	dllLoadLutType(fixedpath, 6);		
	dllLoadIGBP(igbppath);           


	//loading PARASOL Level 1 data
	char filepath[256];
	CFileDialog dlg(TRUE,NULL,NULL,
		OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_ALLOWMULTISELECT|OFN_EXPLORER,
		_T("text file (*.*)|*.*|"));
	CString strPath;
	if(dlg.DoModal()==IDOK)
	{
		strPath = dlg.GetPathName();
	}
	else
	{
		return;
	}
    strcpy(filepath, strPath);
	printf("input file: %s \n", filepath);
	dllLoadParasolData(filepath);

	//inversion
	double minlon=110, maxlon=130, minlat=20, maxlat=55;
	printf("Inverting ... \n");
    dllInvertArea(minlon, maxlon, minlat, maxlat);
    printf("Finished! \n");

	//display using IDL
	//inversiondisp.pro
}


//single point inversion using PARASOL Level1 data
void CProject_PGStationDlg::OnParasolSinglepointinversion33004()
{
	// TODO: Add your command handler code here

	//load lut data
	int    modetypeNumber = 6;
	double ext[6][3];
	int    bandw[3]={4,6,8};
	double** ptypeLut = NULL;
	int      nSingLutDim = AOD_NUM*SZA_NUM*NUMMU*NUMAZIMUTHS*2;
	ptypeLut = f2d(modetypeNumber*3, nSingLutDim);	
	char lutpath[1024];
	sprintf(lutpath,"%s\\lut\\type",m_binPath);	
	printf("%s\n",lutpath);
	LoadMixedLut(lutpath, ptypeLut, modetypeNumber, nSingLutDim, ext );	
	printf("LoadMixedLut \n");


	stMultiView mdata;
	
	//load single point multiple-angle data
	char mfile[256] = "c:\\temp\\NetDisk\\doc\\papers\\Aerosol\\revise4\\data\\simulate_multiViewData.txt";
	LoadParasolMultiDataSingle(mfile, mdata);
	
	//invert using 490nm reflectance
	double aod4, aod6, aod8;
	PtInvert(mdata, ptypeLut, nSingLutDim, 6, ext, 10, &aod4, &aod6, &aod8);

	//invert using polarized reflectance
	double faod4, faod6, faod8;
	InvertAODWithConstraint(mdata, ptypeLut, nSingLutDim, 6, ext, 10, aod4, 
		&faod4, &faod6, &faod8);
}

void CProject_PGStationDlg::OnWaveletTest()
{
	// TODO: Add your command handler code here

	if( m_imagePointerVector.size()>0 )
	{		
		//float* fImage = NULL;
		//int ht,wd;
		//IplImageToFloatImage(m_imagePointerVector[0]->GetCvArr(), &fImage, &ht, &wd);

		int i,j;
	
		//
		unsigned char* pImage;
		int ht,wd;
		IplImageToGrayImage(m_imagePointerVector[0]->GetCvArr(), &pImage, &ht, &wd);

		float* pWave;
		int dstHt, dstWd;
		DWTProcess(pImage, ht, wd, &pWave, &dstHt, &dstWd, 1);

		//dispaly
		IplImage* pDisp = cvCreateImage(cvSize(dstWd, dstHt), 8, 1);
		for( j=0; j<dstHt; j++)
			for( i=0; i<dstWd; i++)
			{
				pDisp->imageData[j*pDisp->widthStep+i] = max(0, min(255, (int)( pWave[j*dstWd+i]*255) ) );
			}

		//calculate the information of high frequency based on Wavelet 
		//int x = 285;
		//int y = 8;

		FILE* fp = fopen("c:\\temp\\wavelet_image.txt", "w");
		for(j=0; j<ht*0.5; j++)
		{
			for(i=0; i<wd*0.5; i++)
			{
				int px = i;
				int py = j;

				int x1 = px+dstWd/2;		int y1 = py;
				int x2 = px;     		    int y2 = py + dstHt/2;
				int x3 = px + dstWd/2;   	int y3 = py + dstHt/2;

				//double wp = pWave[y1*dstWd+x1]*pWave[y1*dstWd+x1] 
				//            + pWave[y2*dstWd+x2]*pWave[y2*dstWd+x2] 
				//			 + pWave[y3*dstWd+x3]*pWave[y3*dstWd+x3];

				double wp = fabs(pWave[y1*dstWd+x1])+fabs(pWave[y2*dstWd+x2])+fabs(pWave[y3*dstWd+x3]);

				//double kalmanFilterValue = kalmanFilter.Predict(wp);
				//fprintf(fp, "%lf ", pWave[j*dstWd+i]);
				fprintf(fp, "%lf ", wp);
			}
			fprintf(fp, "\n");
		}
        fclose(fp);


		cvNamedWindow("Wavelet");
		cvShowImage("Wavelet", pDisp);

		free(pWave);
		cvReleaseImage(&pDisp);
	}
}
void CProject_PGStationDlg::OnSurfaceDem()
{
	// TODO: Add your command handler code here

	char filepath[256];
	CFileDialog dlg(TRUE,NULL,NULL,
		OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_ALLOWMULTISELECT|OFN_EXPLORER,
		_T("img file (*.img)|*.img|"));
	CString strPath;
	if(dlg.DoModal()==IDOK)
	{
		strPath = dlg.GetPathName();
	}
	else
	{
		return;
	}
	strcpy(filepath, strPath);
	printf("input file: %s \n", filepath);
	
	double* pDst;
	int ht,wd;
	LoadGrayImageGeneral(filepath, &pDst, &ht, &wd);
	printf("%d %d \n", ht, wd);

	double ming = 1000000;
	double maxg = 0;
	for(int i=0; i<ht*wd; i++)
	{
		ming = min(ming, pDst[i]);
		maxg = max(maxg, pDst[i]);
	}

	ming = max(0, ming);
	printf("%lf %lf \n", ming, maxg);


	free(pDst);
}

void CProject_PGStationDlg::OnSurfaceSlope()
{
	// TODO: Add your command handler code here

	printf("type short - bytes: %d \n", sizeof(short));   //2 bytes
	printf("type float - bytes: %d \n", sizeof(float));   //4 bytes
	printf("type double - bytes: %d \n", sizeof(double)); //8 bytes

    int i;
	char filepath[256];
	CFileDialog dlg(TRUE,NULL,NULL,
		OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_ALLOWMULTISELECT|OFN_EXPLORER,
		_T("img file (*.img)|*.img|"));
	CString strPath;
	if(dlg.DoModal()==IDOK)
	{
		strPath = dlg.GetPathName();
	}
	else
	{
		return;
	}
	strcpy(filepath, strPath);
	printf("input file: %s \n", filepath);

	//print file information
	PrintImageInfo(filepath);

	double* pDst;
	int ht,wd;
	LoadGrayImageGeneral(filepath, &pDst, &ht, &wd);
	printf("%d %d \n", ht, wd);

	double minangle = 90;
	double maxangle = 0;
	for(int i=0; i<ht*wd; i++)
	{
		//double angle = atan( pDst[i] ) / PI * 180.0;
		double angle = pDst[i];
		if(angle<0)
			continue;
		minangle = min(minangle, angle);
		maxangle = max(maxangle, angle);
	}
    printf("%lf %lf \n", minangle, maxangle);

	//calculate the histogram
	double hist[90];
	memset(hist, 0, sizeof(double)*90);
	for( i=0; i<ht*wd; i++)
	{
		//double angle = atan( pDst[i] ) / PI * 180.0;
		double angle = pDst[i];
		if(angle<0)
			continue;
		//int ai = (int)(angle+0.5) + 90;
		int ai = (int)(angle);
		hist[ai] ++;
	}

	//print
	for(i=0; i<90; i++)
	{
		printf("%d %4.2lf \n", i, hist[i]);
	}	
}

void CProject_PGStationDlg::OnGeotransformGrd2ll()
{
	// TODO: Add your command handler code here

	double x = 329067.051146776;
	double y = 4987668.10056836;
	double lat,lon;
	int zoneNumber = 50;

    UTMtoLL(23, y, x, zoneNumber, lat, lon);
	printf("lat: %lf  lon:%lf \n", lat, lon);	

	x = 329067.051146776 + 90;
	y = 4987668.10056836;
	UTMtoLL(23, y, x, zoneNumber, lat, lon);
	printf("lat: %lf  lon:%lf \n", lat, lon);	
}

void CProject_PGStationDlg::OnGeotransformLl2grd()
{
	// TODO: Add your command handler code here

	double x,y;
	double lat = 39.974;
	double lon = 116.306;	
	
	int zoneNumber = 50;

	lon = 118.9793;
	lat = 29.58844;
	zoneNumber = int((lon + 180)/6) + 1;
	LLtoUTM(23, lat, lon, y, x, zoneNumber);
	printf("x: %lf  y:%lf \n", x, y);

	lon = 118.9792;
	lat = 29.58848;
	zoneNumber = int((lon + 180)/6) + 1;
	LLtoUTM(23, lat, lon, y, x, 50);
	printf("x: %lf  y:%lf \n", x, y);
	
	
	/*
	double left = 329067.051146776;
	double top  = 4987668.10056836;

	int row = (top-y) / 90;
	int col = (x-left) / 90;
	printf("row: %d  col:%d \n", row, col);
	*/
}

void CProject_PGStationDlg::OnSurfaceExtractarea()
{
	// TODO: Add your command handler code here

	int i,j;

	// location of Beijing
	double x,y;
	double lat;// = 39.974;
	double lon;// = 116.306;	
	int zoneNumber = 50;
	lon = 116.381;
	lat = 39.977;
	//lon = 116.198;
	//lat = 40.008;
	//UTMtoLL(23, y, x, zoneNumber, lat, lon);
	LLtoUTM(23, lat, lon, y, x, 50);
	printf("x: %lf  y:%lf \n", x, y);

	//retrieve the geo-information from the image 
	//char filename[256] = "c:\\temp\\Data\\DEM\\strm_60_04\\slope_60_04.img";
	//char filename[256] = "c:\\temp\\Data\\DEM\\strm_60_04\\utm_srtm_60_04.img";
	//char filename[256] = "c:\\temp\\Data\\DEM\\60-04-05-meters.tif";
	char filename[256] = "c:\\temp\\Data\\DEM\\slope-bj.tif";
    stGeoInfo geoInfo;
	GetGeoInformation(filename, geoInfo);
	double left = geoInfo.left; //329067.051146776;
	double top  = geoInfo.top;  //4987668.10056836;
	int row = (y-top) / geoInfo.dy;
	int col = (x-left) / geoInfo.dx;
	printf(" %lf %lf  row: %d  col:%d \n", y, x, row, col);
	
	//load image data
	double* pDst;
	int ht,wd;
	LoadGrayImageGeneral(filename, &pDst, &ht, &wd);
	printf("%d %d \n", ht, wd);

	//extract surface information( such as slope, et ac.) of special area from the DEM data
	FILE* fp = fopen("c:\\temp\\extractSlope.txt", "w");
	//FILE* fp = fopen("c:\\temp\\extractDEM.txt", "w");
	//FILE* fp1 = fopen("c:\\temp\\slopeDistribution1D.txt", "w");
	for(j=-48; j<49; j++)
	{
		for(i=-41; i<42; i++)
		{
			fprintf( fp, "%8.3lf ",  pDst[(j+row)*wd+(i+col)] );
			//fprintf( fp, "%8.3lf ",  tan( pDst[(j+row)*wd+(i+col)]/180.0*PI ) );
			//fprintf( fp1, " %8.3lf %8.3lf \n", tan( pDst[(j+row)*wd+(i+col)]/180.0*PI ) );
		}
		fprintf(fp, "\n");
	}
	fclose(fp);

	//write image to GeoTiff file


	free(pDst);

	printf("Extract Finished ! \n");
}

void CProject_PGStationDlg::OnSmokeWavelet()
{
	// TODO: Add your command handler code here
		
	int k;

	CKalManFilter kalmanFilter;

	//test single point
	int px = 233*0.5;
	int py = 29*0.5;

	/*
	//for sequence 1
	px = 93*0.5;	py = 173*0.5;
	px = 282*0.5;	py = 70*0.5;
	px = 212*0.5;	py = 101*0.5;
	px = 280*0.5;	py = 11*0.5;
	//px = 192*0.5; py = 157*0.5;
	px = 255*0.5;	py = 207*0.5;
	px = 138*0.5;	py = 133*0.5;
	*/

	//px = 164;
	//py = 100;
	px = 222*0.5;
	py = 78*0.5;

	//cvNamedWindow("Image");

	char imagepath[256] = "c:\\temp\\Data\\video\\smoke-seq1";

	FILE* fp = fopen("c:\\temp\\seq_wavelet.txt", "w");
	//optical of two frames
	for(k=0; k<400; k++)
	{
		char fileName[256];//  = "c:\\temp\\Data\\video\\bridge\\0048.jpg";
		sprintf(fileName, "%s\\%.4d.jpg", imagepath, k);
		//printf("%s \n", fileName);
		//printf("%d %d \n", k, k+1);

		IplImage* pInput = cvLoadImage(fileName, 0);

		//image smooth
		cvSmooth(pInput, pInput);


		//calculate the wavelet parameters
		unsigned char* pImage;
		int ht,wd;
		IplImageToGrayImage(pInput, &pImage, &ht, &wd);

		float* pWave;
		int dstHt, dstWd;
		DWTProcess(pImage, ht, wd, &pWave, &dstHt, &dstWd, 1);
        

		int x1 = px+dstWd/2;		int y1 = py;
		int x2 = px;     		    int y2 = py + dstHt/2;
		int x3 = px + dstWd/2;   	int y3 = py + dstHt/2;
	
        
		//double wp = pWave[y1*dstWd+x1]*pWave[y1*dstWd+x1] 
		//            + pWave[y2*dstWd+x2]*pWave[y2*dstWd+x2] 
		//			 + pWave[y3*dstWd+x3]*pWave[y3*dstWd+x3];

    	double wp = fabs(pWave[y1*dstWd+x1])+fabs(pWave[y2*dstWd+x2])+fabs(pWave[y3*dstWd+x3]);
 
		double kalmanFilterValue = kalmanFilter.Predict(wp);

        printf("%lf %lf \n", wp, kalmanFilterValue);    
		fprintf(fp, "%d %lf %lf \n", k, wp, kalmanFilterValue);

		//cvShowImage("Image", pInput);
		//cvWaitKey();

		free(pImage);
		free(pWave);
		cvReleaseImage(&pInput);
	}
	fclose(fp);
}

void CProject_PGStationDlg::OnKalmanfilterWavelet()
{
	// TODO: Add your command handler code here
	int i;
	double* wavelet = (double*)malloc(1000*sizeof(double));
    int index;

	//FILE* fp = fopen("c:\\temp\\Work\\DFHH\\Doc\\Research\\data\\seq1\\seq_wavelet-280-11.txt", "r");
	//FILE* fp = fopen("c:\\temp\\Work\\DFHH\\Doc\\Research\\data\\seq1\\seq_wavelet-192-157.txt", "r");
	FILE* fp = fopen("c:\\temp\\Work\\DFHH\\Doc\\Research\\data\\seq1\\seq_wavelet-255-207.txt", "r");
	for( i=0; i<400; i++)
	{
		fscanf(fp, "%d %lf", &index, &wavelet[i]);
	}
	fclose(fp);


	CPointWaveletAnalyse waveAnalyse;
	//kalman filter
	double* filterData = (double*)malloc(1000*sizeof(double));
	fp = fopen("c:\\temp\\wavelet_kalman.txt", "w");
	CKalManFilter kalmanFilter;
	for(i=1; i<300; i++)
	{
		double predict = kalmanFilter.Predict( wavelet[i] );
		printf("%lf %lf \n", wavelet[i], predict);
		fprintf(fp, "%lf %lf \n", wavelet[i], predict);
		filterData[i] = predict;

		waveAnalyse.Push(predict);
	}
	fclose(fp);	
	free(wavelet);

	//smoke detection
	waveAnalyse.WaveAnalyse();

	/*
	//analyze the kalman filtered data and detect the smoke
	double wmin = 100000;
	double wmax = 0;
	for(i=50; i<300; i++)
	{
		wmin = min(wmin, filterData[i]);
		wmax = max(wmax, filterData[i]);
	}
	printf("min: %lf max: %lf \n", wmin, wmax);
	double threshold = (wmin+wmax)*0.5;
	int lindex = 0;
	int hindex = 0;
	for(i=50; i<300; i++)
	{
		if( filterData[i]<threshold )
			lindex++;
		else
			hindex++;
	}
	printf("High: %d  Low: %d \n", hindex, lindex);
	*/

	
}

void CProject_PGStationDlg::OnOpencv2Image()
{
	// TODO: Add your command handler code here
	

}

void CProject_PGStationDlg::OnSmokeSobel()
{
	// TODO: Add your command handler code here
	int k;

	CKalManFilter kalmanFilter;

	//test single point
	int px = 0;
	int py = 0;
	px = 192;
	py = 157;

	//cvNamedWindow("Image");

	char imagepath[256] = "c:\\temp\\Data\\video\\smoke-seq1";

	FILE* fp = fopen("c:\\temp\\seq_wavelet_sobel.txt", "w");
	//optical of two frames
	for(k=100; k<400; k++)
	{
		char fileName[256];//  = "c:\\temp\\Data\\video\\bridge\\0048.jpg";
		sprintf(fileName, "%s\\%.4d.jpg", imagepath, k);
		printf("%s \n", fileName);
		//printf("%d %d \n", k, k+1);

		IplImage* pInput = cvLoadImage(fileName, 0);
		int ht = pInput->height;
		int wd = pInput->width;
		int scanwd = pInput->widthStep;

		//image smooth
		cvSmooth(pInput, pInput);

		//sobel
		IplImage* pImageX = cvCloneImage(pInput);
		cvSobel(pInput, pImageX, 1, 0);
		IplImage* pImageY = cvCloneImage(pInput);
		cvSobel(pInput, pImageY, 0, 1);
         

		double edgeResponse = (unsigned char)( pImageX->imageData[py*scanwd+px] ) 
			+ (unsigned char)( pImageY->imageData[py*scanwd+px] );


		double predict = kalmanFilter.Predict(edgeResponse);
		fprintf(fp, "%d %lf %lf\n", k, edgeResponse, predict);

		/*
		//calculate the wavelet parameters
		unsigned char* pImage;
		int ht,wd;
		IplImageToGrayImage(pInput, &pImage, &ht, &wd);
		
		float* pWave;
		int dstHt, dstWd;
		DWTProcess(pImage, ht, wd, &pWave, &dstHt, &dstWd, 1);
		int x1 = px+dstWd/2;		int y1 = py;
		int x2 = px;     		    int y2 = py + dstHt/2;
		int x3 = px + dstWd/2;   	int y3 = py + dstHt/2;
		//double wp = pWave[y1*dstWd+x1]*pWave[y1*dstWd+x1] 
		//            + pWave[y2*dstWd+x2]*pWave[y2*dstWd+x2] 
		//			 + pWave[y3*dstWd+x3]*pWave[y3*dstWd+x3];
		double wp = fabs(pWave[y1*dstWd+x1])+fabs(pWave[y2*dstWd+x2])+fabs(pWave[y3*dstWd+x3]);

		double kalmanFilterValue = kalmanFilter.Predict(wp);
		
		printf("%lf %lf \n", wp, kalmanFilterValue);    
		fprintf(fp, "%d %lf %lf \n", k, wp, kalmanFilterValue);
		*/

		//cvShowImage("Image", pInput);
		//cvWaitKey();
		//free(pImage);
		//free(pWave);

		cvReleaseImage(&pInput);
		cvReleaseImage(&pImageX);
		cvReleaseImage(&pImageY);
	}
	fclose(fp);
}

void CProject_PGStationDlg::OnErdasMerge()
{
	// TODO: Add your command handler code here

	//Merge two images of IMG format
	

}

void CProject_PGStationDlg::OnKltWithkalman()
{
	// TODO: Add your command handler code here

	char imagepath[256] = "c:\\temp\\Data\\video\\bridge";

	char** filenames = NULL;
	int n,nfile;
	n = nfile = 0;

	GetDirFileName(filenames, imagepath, &n, &nfile, "jpg", 0);
	filenames = f2c(nfile, 256);
	GetDirFileName(filenames, imagepath, &n, &nfile, "jpg", 1);

	cvNamedWindow("Image");

	//object tracking
	CKLTTrack kltTrack; 
	vector<CTrackObj> trackObjs;
	int i,j,k,m;
	for(int i=0; i<nfile; i++)
	{		
		IplImage* pImage = cvLoadImage(filenames[i], 0);

		//1. detect features	
		IplImage* eig = cvCreateImage( cvGetSize(pImage), 32, 1 );
		IplImage* temp = cvCreateImage( cvGetSize(pImage), 32, 1 );
		double quality = 0.01;
		double min_distance = 10;
		int win_size = 10;
		CvPoint2D32f* points = (CvPoint2D32f*)malloc(MAX_COUNT*sizeof(CvPoint2D32f));
		int count = MAX_COUNT;
		cvGoodFeaturesToTrack( pImage, eig, temp, points, &count,
			quality, min_distance, 0, 3, 0, 0.04 );
		cvReleaseImage( &eig );
		cvReleaseImage( &temp );

		//2. add features into the list
		vector<CvPoint2D32f> newFeatPts;
		for(k=0; k<count; k++)
		{
			bool bIsNew = true;
			for(m=0; m<trackObjs.size(); m++)
			{
				//calculate the distance between feature points and tracked object
				CvPoint2D32f tpt = trackObjs[m].GetCurPt();

				double dis = sqrt(  (points[k].x-tpt.x)*(points[k].x-tpt.x) 
					+ (points[k].y-tpt.y)*(points[k].y-tpt.y) );

				if(dis<5) 
				{
					bIsNew = false;
					break;
				}
			}
			if(bIsNew)
			{
				//trackObjs.push_back(points[k]);
				newFeatPts.push_back(points[k]);
			}
		}		
		for(k=0; k<newFeatPts.size(); k++)
		{
			CTrackObj* pObj = new CTrackObj();
			pObj->AddLocation(newFeatPts[k]);
			trackObjs.push_back( *pObj );
		}

		//3. tracking
		int nobj = trackObjs.size();
		CvPoint2D32f* cpts = (CvPoint2D32f*)malloc( sizeof(CvPoint2D32f)*nobj );
		CvPoint2D32f* npts = (CvPoint2D32f*)malloc( sizeof(CvPoint2D32f)*nobj );
		int nNewPt=nobj;
		for(k=0; k<nobj; k++)
		{
			cpts[k] = trackObjs[k].GetCurPt();
		}

		int bIsSuccess = kltTrack.Predict(pImage, cpts, nobj, npts, &nNewPt);		
		//add the tracked point into the trajectory
		if(bIsSuccess)
		{		
			for(k=0; k<nobj; k++)
			{
				trackObjs[k].AddLocation(npts[k]);
			}
		}



		// show the trajectory
		for(k=0; k<trackObjs.size(); k++)
		{
			for(m=0; m<trackObjs[k].GetTrajectoryPtNumber()-1; m++)
			{
				CvPoint2D32f p1 = trackObjs[k].GetTrajectoryPt(m);
				CvPoint2D32f p2 = trackObjs[k].GetTrajectoryPt(m+1);

				CvPoint ip1,ip2;
				ip1.x = p1.x;
				ip1.y = p1.y;
				ip2.x = p2.x;
				ip2.y = p2.y;
				cvDrawLine(pImage, ip1, ip2, CV_RGB(255,255,255));
			}
		}

		cvShowImage("Image", pImage);		
		cvWaitKey();

		free(points);
		free(cpts);
		free(npts);

		cvReleaseImage(&pImage);
	}



}

void CProject_PGStationDlg::OnKltSingleptwithkalman()
{
	// TODO: Add your command handler code here

	int  k,m,n;
	char imagepath[256] = "c:\\temp\\Data\\video\\bridge\\";
    char filename[256];

	CKLTTrack kltTrack; 
	
	//define the object to be tracked
	CTrackObj singleObj;
    CvPoint2D32f startPt;
    startPt.x = 547;
	startPt.y = 162;
	singleObj.AddLocation(startPt);

	//manage objects
	vector<CTrackObj> trackObjs;
	trackObjs.push_back(singleObj);

	cvNamedWindow("Image");

	//optical of two frames
	for(k=150; k<700; k++)
	{
		sprintf(filename, "c:\\temp\\Data\\video\\bridge\\%.4d.jpg", k);
		IplImage* pImage = cvLoadImage(filename, 0);
		
		CvPoint2D32f cpts[1];
		CvPoint2D32f npts[1];
		int nobj,nNewPt;
		cpts[0] = trackObjs[0].GetCurPt();
		nobj = 1;
		
		int bIsSuccess = kltTrack.Predict(pImage, cpts, nobj, npts, &nNewPt);	

		trackObjs[0].AddLocation(npts[0]);
	

		/*
		//if the object is out of the image, remove it
		for(n=0; n<trackObjs.size(); n++)
		{
			CvPoint2D32f cp = trackObjs[n].GetCurPt();			
		}
		*/
		
		//show the trajectory
		for(n=0; n<trackObjs.size(); n++)
		{
			for(m=0; m<trackObjs[n].GetTrajectoryPtNumber()-1; m++)
			{
				CvPoint2D32f p1 = trackObjs[n].GetTrajectoryPt(m);
				CvPoint2D32f p2 = trackObjs[n].GetTrajectoryPt(m+1);

				CvPoint ip1,ip2;
				ip1.x = p1.x;
				ip1.y = p1.y;
				ip2.x = p2.x;
				ip2.y = p2.y;
				cvDrawLine(pImage, ip1, ip2, CV_RGB(255,255,255));
			}
		}

		cvShowImage("Image", pImage);		
		cvWaitKey();	

		cvReleaseImage(&pImage);
	}
}

void CProject_PGStationDlg::OnParasolPointsatellite()
{
	// TODO: Add your command handler code here
	//dialog for opening file
	CFileDialog dlg(true,NULL,NULL,
		OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_ALLOWMULTISELECT|OFN_EXPLORER,
		_T("jpg file (*.*)|*.*||"));

	char filepath[256];
	if(dlg.DoModal() == IDOK)
	{	
		CString cfile = dlg.GetPathName();
		strcpy(filepath, cfile);
	}
	else
	{
		return;
	}

	//load lut data
	int    modetypeNumber = 6;
	double ext[6][3];
	int    bandw[3]={4,6,8};
	double** ptypeLut = NULL;
	int      nSingLutDim = AOD_NUM*SZA_NUM*NUMMU*NUMAZIMUTHS*2;
	ptypeLut = f2d(modetypeNumber*3, nSingLutDim);	
	char lutpath[1024];
	sprintf(lutpath,"%s\\lut\\type",m_binPath);	
	printf("%s\n",lutpath);
	LoadMixedLut(lutpath, ptypeLut, modetypeNumber, nSingLutDim, ext );	
	printf("LoadMixedLut \n");

	//load IGBP data
	int igbpType = 10;

	//read PARASOL data
	stMultiView mdata;
	double lon = 116.381;
	double lat = 39.977;
	//char parasolFile[256] = "c:\\temp\\Data\\Parasol\\2005\\03\\07\\P3L1TBG1006178JL";
	//char parasolFile[256] = "c:\\temp\\Data\\Parasol\\2005\\05\\24\\P3L1TBG1011149JL";
	CReasParasolLevel1File readParasol;

	printf("reading parasol data ... \n");
	readParasol.Load(filepath);
	mdata = readParasol.GetPtMultiViewData(lon, lat);

	//invert using 490nm reflectance
	double aod4, aod6, aod8;
	PtInvert(mdata, ptypeLut, nSingLutDim, modetypeNumber, ext, igbpType, &aod4, &aod6, &aod8);

	//invert using polarized reflectance
	double faod4, faod6, faod8;
	InvertAODWithConstraint(mdata, ptypeLut, nSingLutDim, modetypeNumber, ext, igbpType, aod4, 
		&faod4, &faod6, &faod8);

	printf("%lf %lf %lf \n", faod4, faod6, faod8);
}

void CProject_PGStationDlg::OnSurfaceMosaic()
{
	// TODO: Add your command handler code here


	//mosaic DEM files

	/*
	// location of Beijing
	double x,y;
	double lat;// = 39.974;
	double lon;// = 116.306;	
	int zoneNumber = 50;
	lon = 116.387;
	lat = 39.972;	
	//UTMtoLL(23, y, x, zoneNumber, lat, lon);
	LLtoUTM(23, lat, lon, y, x, 50);
	printf("x: %lf  y:%lf \n", x, y);


	char filename[256] = "c:\\temp\\Data\\DEM\\strm_60_04\\utm_srtm_60_04.img";
	stGeoInfo geoInfo;
	GetGeoInformation(filename, geoInfo);
	double left = geoInfo.left; //329067.051146776;
	double top  = geoInfo.top;  //4987668.10056836;
	int row = (y-top) / geoInfo.dy;
	int col = (x-left) / geoInfo.dx;
	printf(" %lf %lf  row: %d  col:%d \n", y, x, row, col);

	//load image data
	double* pDst;
	int ht,wd;
	LoadGrayImageGeneral(filename, &pDst, &ht, &wd);
	printf("%d %d \n", ht, wd);
    free(pDst);
	*/

	//
	char file1[256] = "c:\\temp\\Data\\DEM\\strm_60_04\\utm_srtm_60_04.img";
	char file2[256] = "c:\\temp\\Data\\DEM\\strm_60_05\\utm_srtm_60_05.img";
	char **files = f2c(2, 256);
	strcpy(files[0], file1);
	strcpy(files[1], file2);
	GeoImageMosaic(files, 2);	
}

void CProject_PGStationDlg::OnDodgingMask()
{
	// TODO: Add your command handler code here

	IplImage* pImage = CloneCurrentImage();
	IplImage* pColor = CloneCurrentImageColor();

	if(pColor!=NULL)
	{
		cvSaveImage("c:\\temp\\original.jpg", pColor);
		IplImage* pDst = cvCloneImage(pColor);
		MaskDodging(pColor, pDst);

		cvNamedWindow("Dodging");
		cvShowImage("Dodging", pDst);

		cvSaveImage("c:\\temp\\dodge.jpg", pDst);

		cvReleaseImage(&pDst);
	}
}

void CProject_PGStationDlg::OnDemMedianfilter()
{
	// TODO: Add your command handler code here

	char demfile[256] = "c:\\temp\\Data\\DEM\\60-04-05-meters.tif";


	//remove the wrong data after mosaic
	GeotiffMedianFilter(demfile);


	printf("Medain filter finished! \n");
}

void CProject_PGStationDlg::OnTransformDistance()
{
	// TODO: Add your command handler code here

	IplImage* pImage = CloneCurrentImage();
	if(pImage!=NULL)
	{

		IplImage* pDst = cvCreateImage(cvGetSize(pImage), IPL_DEPTH_32F, 1);


		cvDistTransform(pImage, pDst);
	

		/*
		double maxValue,minValue;
		cvMinMaxLoc(pDst, &minValue, &maxValue);		
		int ht = pImage->height;
		int wd = pImage->width;
		int scanwd = pImage->widthStep;
		for(int j=0; j<ht; j++)
			for(int i=0; i<wd; i++)
			{
				pImage->imageData[j*scanwd+i] =  (pDst->[j*scanwd+i]-minValue) / (maxValue-minValue)*255; //(pDstMat->data.db[j*wd+i]-minValue)/(maxValue-minValue); 
			}
		*/
		cvConvertScale(pDst, pImage, 2);

		cvNamedWindow("Image");
		cvShowImage("Image", pImage);

		//cvNamedWindow("DistTransform");
		//cvShowImage("DistTransform", pDst);

		cvWaitKey();
	}
}

void CProject_PGStationDlg::OnPointSurf()
{
	// TODO: Add your command handler code here
	

	IplImage* pImage = CloneCurrentImageColor(); //CloneCurrentImage();
	if(pImage!=NULL)
	{
		char* filePath = m_imagePointerVector[0]->GetFilePath();
		printf("input file: %s \n", filePath);

		CSURFKeyPoint surf;
		//CSURF surf;		
		//surf.Detect(filePath, "c:\\temp\\surf.dat");
	
		/*
		vector<double> px;
		vector<double> py;
		surf.Detect(filePath, px, py);
		for(int i=0; i<px.size(); i++)
		{
			CvPoint p;
			p.x = px[i];
			p.y = py[i];
			cvDrawCircle(pImage, p, 1, CV_RGB(255, 0, 0), 2);
		}
		*/

		vector<PtFeature> featPts;
		surf.Detect(filePath, featPts);

		for(int i=0; i<featPts.size(); i++)
		{
			CvPoint p;
			p.x = featPts[i].x;
			p.y = featPts[i].y;
			cvDrawCircle(pImage, p, 1, CV_RGB(255, 0, 0), 2);
		}
	
		cvNamedWindow("SURF");
		cvShowImage("SURF", pImage);
		cvReleaseImage(&pImage);
		cvWaitKey();	
	}
}
void CProject_PGStationDlg::OnRegisterSurf()
{
	// TODO: Add your command handler code here

	if( m_imagePointerVector.size()>1 )
	{

		char* file1 = m_imagePointerVector[0]->GetFilePath();
		char* file2 = m_imagePointerVector[1]->GetFilePath();
	
		printf("%s \n", file1);
		printf("%s \n", file2);

		//CRationMatch rmatch;
		//rmatch.Match(file1, file2);
	
		CSURF surf;	

		//vector<PtFeature> featPts1;
		ImgFeature img1;
		surf.Detect(file1, img1);
		printf("%d \n", img1.featPts.size());

		ImgFeature img2;
		surf.Detect(file2, img2);
		printf("%d \n", img2.featPts.size());
	
		//registration
		//CRationMatch rmatch;
		//rmatch.Match(img1, img2);		
	}
}

void CProject_PGStationDlg::OnRegisterGeneral()
{
	// TODO: Add your command handler code here

	if( m_imagePointerVector.size()>1 )
	{
		char* file1 = m_imagePointerVector[0]->GetFilePath();
		char* file2 = m_imagePointerVector[1]->GetFilePath();

		IplImage* lImage = cvLoadImage(file1);
		IplImage* rImage = cvLoadImage(file2);

		printf("%s \n", file1);
		printf("%s \n", file2);

		//1. feature detection
		CFeatureBase* pFeatDetect = new CSIFTFloat(); 
		//CFeatureBase* pFeatDetect = new CSURF(); 
        ImgFeature lImageFeats;
		ImgFeature rImageFeats;
        pFeatDetect->Detect(file1, lImageFeats);
		pFeatDetect->Detect(file2, rImageFeats);
        
		/*
		int i;
		for(i=0; i<lImageFeats.featPts.size(); i++)
		{
			int x = lImageFeats.featPts[i].x;
			int y = lImageFeats.featPts[i].y;
			cvDrawCircle(lImage, cvPoint(x,y), 2, CV_RGB(255,0,0));
		}

		for(i=0; i<rImageFeats.featPts.size(); i++)
		{
			int x = rImageFeats.featPts[i].x;
			int y = rImageFeats.featPts[i].y;
			cvDrawCircle(rImage, cvPoint(x,y), 2, CV_RGB(255,0,0));
		}

		cvNamedWindow("Feature1");
	    cvShowImage("Feature1", lImage);
		cvNamedWindow("Feature2");
		cvShowImage("Feature2", rImage);
		*/


		//2. matching
		printf("Matching... \n");
		CMatchBase* pMatch = new CKNNMatch();
		vector<MatchPairIndex> mi;
		pMatch->Match(lImageFeats, rImageFeats, mi);
		printf("Match Pair: %d \n", mi.size());
		

		//3. display 
		srand(100);
		for(int i=0; i<mi.size(); i++)
		{
			int li = mi[i].l;
			int ri = mi[i].r;
			CvPoint pl;
			pl.x = lImageFeats.featPts[li].x;
			pl.y = lImageFeats.featPts[li].y;
			CvPoint pr;
			pr.x = rImageFeats.featPts[ri].x;
			pr.y = rImageFeats.featPts[ri].y;

			int r = (double)(rand())/(double)(RAND_MAX)*255;
			int g = (double)(rand())/(double)(RAND_MAX)*255;
			int b = (double)(rand())/(double)(RAND_MAX)*255;

			cvDrawCircle(lImage, pl, 2, CV_RGB(r,g,b),2);
			cvDrawCircle(rImage, pr, 2, CV_RGB(r,g,b),2);
		}

		cvNamedWindow("Match1");
		cvShowImage("Match1", lImage);
		cvNamedWindow("Match2");
		cvShowImage("Match2", rImage);
	}
}

void CProject_PGStationDlg::OnGeomosaicHistmatching()
{
	// TODO: Add your command handler code here
	char** filesPath = f2c(10, 256);
	//strcpy(filesPath[0], "c:\\temp\\Data\\GF\\20130821GF-8m\\GF1_PMS1_E127.0_N53.8_20130818_L1A0000071047.tif");
	//strcpy(filesPath[1], "c:\\temp\\Data\\GF\\20130821GF-8m\\GF1_PMS1_E127.1_N54.0_20130818_L1A0000071046.tif");
	
	//strcpy(filesPath[0], "c:\\temp\\Data\\YG2\\abh\\YG2-4_CCD-1_000063303_004_042_004_L2\\YG2-4_CCD-1_000063303_004_042_004_L2.tiff");
	//strcpy(filesPath[1], "c:\\temp\\Data\\YG2\\abh\\YG2-4_CCD-2_000063303_003_042_004_L2\\YG2-4_CCD-2_000063303_003_042_004_L2.tiff");


	strcpy(filesPath[0], "c:\\temp\\Data\\YG2\\klmy\\YG2-2_CCD-1_000061314_002_018_003_L2\\YG2-2_CCD-1_000061314_002_018_003_L2.tiff");
	strcpy(filesPath[1], "c:\\temp\\Data\\YG2\\klmy\\YG2-2_CCD-1_000061524_006_002_002_L2\\YG2-2_CCD-1_000061524_006_002_002_L2.tiff");
	strcpy(filesPath[2], "c:\\temp\\Data\\YG2\\klmy\\YG2-3_CCD-2_000062055_010_019_002_L2\\YG2-3_CCD-2_000062055_010_019_002_L2.tiff");

	//GeoMosaicWithBalance(filesPath, 3);	
}

void CProject_PGStationDlg::OnSfmDetectfeatures()
{
	// TODO: Add your command handler code here

	if( m_imagePointerVector.size()>0 )
	{
		char* file = m_imagePointerVector[m_nCurrentIndex]->GetFilePath();
		
		//CFeatureBase* pFeatDetect = new CSIFTFloat();		
		CFeatureBase* pFeatDetect = new CSURF();		
		ImgFeature imageFeats;
		pFeatDetect->Detect(file, imageFeats);
		printf("ht:%d wc:\\temp%d Feature Number: %d dimension:%d \n", 
			imageFeats.ht, imageFeats.wd, imageFeats.featPts.size(),
			imageFeats.featPts[0].feat.size());
		
		IplImage* image = m_imagePointerVector[m_nCurrentIndex]->GetCvArrColor();
		for(int i=0; i<imageFeats.featPts.size(); i++)
		{
			int x = imageFeats.featPts[i].x;
			int y = imageFeats.featPts[i].y;
			cvDrawCircle(image, cvPoint(x,y), 2, CV_RGB(255,0,0), 2);
		}

		delete pFeatDetect;

		cvNamedWindow("Feature");
		cvShowImage("Feature", image);
	}
}

void CProject_PGStationDlg::OnSfmMatching()
{
	// TODO: Add your command handler code here
	if( m_imagePointerVector.size()>1 )
	{
		char* file1 = m_imagePointerVector[0]->GetFilePath();
		char* file2 = m_imagePointerVector[1]->GetFilePath();

		IplImage* lImage = cvLoadImage(file1);
		IplImage* rImage = cvLoadImage(file2);

		printf("%s \n", file1);
		printf("%s \n", file2);

		//1. feature detection
		CFeatureBase* pFeatDetect = new CSIFTFloat(); 
		//CFeatureBase* pFeatDetect = new CSURF(); 
		ImgFeature lImageFeats;
		ImgFeature rImageFeats;
		pFeatDetect->Detect(file1, lImageFeats);
		pFeatDetect->Detect(file2, rImageFeats);

		/*
		int i;
		for(i=0; i<lImageFeats.featPts.size(); i++)
		{
		int x = lImageFeats.featPts[i].x;
		int y = lImageFeats.featPts[i].y;
		cvDrawCircle(lImage, cvPoint(x,y), 2, CV_RGB(255,0,0));
		}

		for(i=0; i<rImageFeats.featPts.size(); i++)
		{
		int x = rImageFeats.featPts[i].x;
		int y = rImageFeats.featPts[i].y;
		cvDrawCircle(rImage, cvPoint(x,y), 2, CV_RGB(255,0,0));
		}

		cvNamedWindow("Feature1");
		cvShowImage("Feature1", lImage);
		cvNamedWindow("Feature2");
		cvShowImage("Feature2", rImage);
		*/


		//2. matching
		printf("Matching... \n");
		CMatchBase* pMatch = new CKNNMatch();
		vector<MatchPairIndex> mi;
		pMatch->Match(lImageFeats, rImageFeats, mi);
		printf("Match Pair: %d \n", mi.size());


		//3. display 
		srand(100);
		for(int i=0; i<mi.size(); i++)
		{
			int li = mi[i].l;
			int ri = mi[i].r;
			CvPoint pl;
			pl.x = lImageFeats.featPts[li].x;
			pl.y = lImageFeats.featPts[li].y;
			CvPoint pr;
			pr.x = rImageFeats.featPts[ri].x;
			pr.y = rImageFeats.featPts[ri].y;

			int r = (double)(rand())/(double)(RAND_MAX)*255;
			int g = (double)(rand())/(double)(RAND_MAX)*255;
			int b = (double)(rand())/(double)(RAND_MAX)*255;

			cvDrawCircle(lImage, pl, 2, CV_RGB(r,g,b),2);
			cvDrawCircle(rImage, pr, 2, CV_RGB(r,g,b),2);
		}

		cvNamedWindow("Match1");
		cvShowImage("Match1", lImage);
		cvNamedWindow("Match2");
		cvShowImage("Match2", rImage);
	}
}

void CProject_PGStationDlg::OnMatchingPair()
{
	// TODO: Add your command handler code here

}

void CProject_PGStationDlg::OnMatchingSeqs()
{
	// TODO: Add your command handler code here
	

}

void CProject_PGStationDlg::OnSfmGeneratetracks()
{
	// TODO: Add your command handler code here
	CFileDialog dlg(TRUE,NULL,NULL,
		OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_ALLOWMULTISELECT|OFN_EXPLORER,
		_T("jpg file (*.jpg)|*.jpg|bitmap file(*.bmp)|*.bmp|Tiff file(*.tif)|*.tif|IMG file(*.img)|*.img||"));

	TCHAR* szPathMulti  = new TCHAR[8*1024*1024];  //alloc memory for multiple files
	dlg.m_ofn.nMaxFile  = 8*1024*1024; 
	dlg.m_ofn.lpstrFile = szPathMulti; 
	dlg.m_ofn.lpstrFile[0] = 0; 

	vector<string> fileVector;
	if(dlg.DoModal()==IDOK)
	{
		POSITION  pos   =   dlg.GetStartPosition(); 
		int index = 0;		
		//read file paths
		while(pos != NULL) 
		{ 
			//insert image filepath into the file list control
			CString filepath = dlg.GetNextPathName(pos); 	
			string s(filepath.GetBuffer());
			fileVector.push_back(s);	
		} 			
		//sort the filenames
		sort( fileVector.begin(), fileVector.end() );
	}

	//1. generate features
	CFeatureBase* pFeatDetect = new CSIFTFloat();
	vector<ImgFeature> imgFeatures;
	imgFeatures.resize(fileVector.size());  //alloc memory
	for(int i=0; i<fileVector.size(); i++)
	{
		//string to char*
		char* pFile = (char*)( fileVector[i].c_str() ); //string to char*
		printf("%s \n", pFile);
		pFeatDetect->Detect( pFile, imgFeatures[i] );
	}

	//2. matching
	vector<PairMatchRes> vecMatch;
	CMatchBase* pMatch = new CKNNMatch();
	for(int i=0; i<fileVector.size(); i++)
		for(int j=i+1; j<fileVector.size(); j++)
		{
			printf("Matching %d-%d \n", i,j);
			PairMatchRes mt;
			mt.lId = i;
			mt.rId = j;
			pMatch->Match(imgFeatures[i], imgFeatures[j], mt);
			vecMatch.push_back(mt);
			printf("%lf %d \n", mt.inlierRatio, mt.matchs.size());
		}

	//3. generate tracks
	CGenerateTracksBase* pGenerateTracks = new CMyGenerateTrack();
	vector<TrackInfo> vecTrack;
	pGenerateTracks->GenerateTracks(vecMatch, vecTrack);


	//4. SFM
	//4.1 Relative Pose
	//find the match pair with maximal inlier
	double matchIdx = 0;
	double maxInlierRatio = 0;
	for(int i=0; i<vecMatch.size(); i++)
	{
		if(maxInlierRatio<vecMatch[i].inlierRatio)
		{
			matchIdx = i;
			maxInlierRatio = vecMatch[i].inlierRatio;
		}
	}

	matchIdx = 0;
    int li = vecMatch[matchIdx].lId;
	int ri = vecMatch[matchIdx].rId;    
	CRelativePoseBase* pRP = new CEstimatePose5Point();
	CameraPara cam1,cam2;
	//set intrisic parameters
	double FOCUS_RATIO = 1.48;
	double focus = imgFeatures[li].ht*FOCUS_RATIO;
	cam1.focus = focus;
	cam1.u0 = imgFeatures[li].wd * 0.5;
	cam1.v0 = imgFeatures[li].ht * 0.5;
	cam2.focus = focus;
	cam2.u0 = imgFeatures[ri].wd * 0.5;
	cam2.v0 = imgFeatures[ri].ht * 0.5;
	//relative pose estimation
	
	/*
	pRP->EstimatePose( vecMatch[matchIdx], imgFeatures[li], imgFeatures[ri], cam1, cam2 );
    FILE* fp = fopen("c:\\temp\\myRelativePose.txt", "w");
	fprintf(fp, "%lf %lf \n", cam1.focus, cam2.focus);
	fprintf(fp, "Rotation: \n");
	for(int j=0; j<3; j++)
	{
		for(int i=0; i<3; i++)
		{
			fprintf(fp, "%lf ", cam2.R[j*3+i]);
		}
		fprintf(fp, "\n");
	}
	fprintf(fp, "Translation: \n");
	for(int j=0; j<3; j++)
		fprintf(fp, "%lf ", cam2.t[j]);
	fprintf(fp, "\n");
	fclose(fp);
	*/
    

	//4.2 recover initial 3D points
    



	/*
	//display
	vector<IplImage*> vecIplImagePointer;
	for(int i=0; i<fileVector.size(); i++)
	{
		const char* filepath = fileVector[i].c_str();
		IplImage* image = cvLoadImage(filepath);
		vecIplImagePointer.push_back(image);
	}
	srand(111);
	for(int i=0; i<vecTrack.size(); i++)
	{
		if(vecTrack[i].views.size()>2)
		{
			int r = (double)(rand())/(double)(RAND_MAX)*255;
			int g = (double)(rand())/(double)(RAND_MAX)*255;
			int b = (double)(rand())/(double)(RAND_MAX)*255;

			for(int j=0; j<vecTrack[i].views.size(); j++)
			{
				ImageKey ik = vecTrack[i].views[j];
				CvPoint  p;
				p.x = imgFeatures[ik.first].featPts[ik.second].x;
				p.y = imgFeatures[ik.first].featPts[ik.second].y;
				cvDrawCircle(vecIplImagePointer[ik.first], p, 2, CV_RGB(r,g,b),2);	
			}
		}
	}

	for(int i=0; i<fileVector.size(); i++)
	{
		char file[256];
		sprintf(file, "c:\\temp\\match_%d.jpg", i);
		cvSaveImage(file, vecIplImagePointer[i]);
		cvReleaseImage(&vecIplImagePointer[i]);
	}
	*/
	printf("Finished! \n");
}



void CProject_PGStationDlg::OnSfmRelativepose()
{
	// TODO: Add your command handler code here

	//3D points for simulation
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


	vector<Point2DDouble> p1;
	vector<Point2DDouble> p2;
	printf("Relative Pose Estimation ... \n");
    
	//read simulated file
	//FILE* fp = fopen("c:\\temp\\pair1.txt", "r");
	FILE* fp = fopen("F:\\data\\sfm\\simulatePair.txt", "r");
	int numPt;
    fscanf(fp, "%d", &numPt);
	for(int i=0; i<numPt; i++)
	{
		double lx,ly,rx,ry;
		fscanf(fp, "%lf %lf %lf %lf ", &lx, &ly, &rx, &ry);
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
	CRelativePoseBase* pRP = new CEstimatePose5Point(); 
	CameraPara cam1,cam2;
	double focus = 260;    
	cam1.focus = focus;
	cam2.focus = focus;
	pRP->EstimatePose(p1, p2, cam1, cam2);

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
	
	//BA optimization
	printf("\n\n BA...\n");
	vector<ImageKeyVector> ptViews;
	//int numpt=20;
    ptViews.resize(numPt);
	for(int i=0; i<numPt; i++)
	{
		ImageKey key;		
		key.first = 0;
		key.second = i;
		ptViews[i].push_back(key);

		key.first = 1;
		key.second = i;
		ptViews[i].push_back(key);
	}

	vector<ImgFeature> imageFeatures;
	imageFeatures.resize(2);
	for(int j=0; j<numPt; j++)
		{ 
			stPtFeature pt;
			pt.cx = p1[j].p[0];
			pt.cy = p1[j].p[1];
			imageFeatures[0].featPts.push_back(pt);
		}
	for(int j=0; j<numPt; j++)
	{ 
		stPtFeature pt;
		pt.cx = p2[j].p[0];
		pt.cy = p2[j].p[1];
		imageFeatures[1].featPts.push_back(pt);
	}
	
	vector<int> cameraIDOrder;
	cameraIDOrder.push_back(0);
	cameraIDOrder.push_back(1);

    vector<CameraPara> cameras;
	cameras.push_back(cam1);
	cameras.push_back(cam2);

	CBABase* pBA = new CSBA();
	pBA->RunSFM(gpts, ptViews, imageFeatures, cameraIDOrder, cameras);
	

	printf("\n");
}

void CProject_PGStationDlg::OnSmokeDemo()
{
	// TODO: Add your command handler code here
}

void CProject_PGStationDlg::OnRelativeposeSimulate()
{
	// TODO: Add your command handler code here

	//3D points for simulation
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


	vector<Point2DDouble> p1;
	vector<Point2DDouble> p2;
	printf("Relative Pose Estimation ... \n");

	
	//read simulated file
	//FILE* fp = fopen("c:\\temp\\pair1.txt", "r");
	FILE* fp = fopen("F:\\data\\sfm\\simulatePair.txt", "r");
	int numPt;
	fscanf(fp, "%d", &numPt);
	for(int i=0; i<numPt; i++)
	{
		double lx,ly,rx,ry;
		fscanf(fp, "%lf %lf %lf %lf ", &lx, &ly, &rx, &ry);
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
	CRelativePoseBase* pRP = new CEstimatePose5Point(); 
	CameraPara cam1,cam2;
	double focus = 240;    
	cam1.focus = focus;
	cam2.focus = focus;
	pRP->EstimatePose(p1, p2, cam1, cam2);

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

    /*
	//BA optimization
	printf("\n\n BA...\n");
	vector<ImageKeyVector> ptViews;
	//int numpt=20;
	ptViews.resize(numPt);
	for(int i=0; i<numPt; i++)
	{
		ImageKey key;		
		key.first = 0;
		key.second = i;
		ptViews[i].push_back(key);

		key.first = 1;
		key.second = i;
		ptViews[i].push_back(key);
	}

	vector<ImgFeature> imageFeatures;
	imageFeatures.resize(2);
	for(int j=0; j<numPt; j++)
	{ 
		stPtFeature pt;
		pt.cx = p1[j].p[0];
		pt.cy = p1[j].p[1];
		imageFeatures[0].featPts.push_back(pt);
	}
	for(int j=0; j<numPt; j++)
	{ 
		stPtFeature pt;
		pt.cx = p2[j].p[0];
		pt.cy = p2[j].p[1];
		imageFeatures[1].featPts.push_back(pt);
	}

	vector<int> cameraIDOrder;
	cameraIDOrder.push_back(0);
	cameraIDOrder.push_back(1);

	vector<CameraPara> cameras;
	cameras.push_back(cam1);
	cameras.push_back(cam2);

	CBABase* pBA = new CSBA();
	pBA->RunSFM(gpts, ptViews, imageFeatures, cameraIDOrder, cameras);
	*/
}

void CProject_PGStationDlg::OnRelativeposeRealimages()
{
	// TODO: Add your command handler code here
    
	//char* leftImage  = "c:\\temp\\Data\\Registration\\kermit\\kermit000.jpg";
	//char* rightImage = "c:\\temp\\Data\\Registration\\kermit\\kermit001.jpg";
	//char* leftImage  = "c:\\temp\\Data\\Registration\\kermit\\kermit000.jpg";
	//char* rightImage = "c:\\temp\\Data\\Registration\\kermit\\kermit001.jpg";

	//char* leftImage  = "c:\\temp\\kermit001.jpg";
	//char* rightImage = "c:\\temp\\kermit000.jpg";

	//char* leftImage  = "F:\\Data\\sfm\\image\\IMG_0235.jpg";
	//char* rightImage = "F:\\Data\\sfm\\image\\IMG_0236.jpg";

	//char* leftImage  = "F:\\Data\\Registration\\IMG_0107.jpg";
	//char* rightImage = "F:\\Data\\Registration\\IMG_0108.jpg";

	//char* leftImage  = "F:\\Data\\panorama\\plane\\ladybug_panoramic_000000.jpg_left.jpg";
	//char* rightImage = "F:\\Data\\panorama\\plane\\ladybug_panoramic_000001.jpg_left.jpg";

	char* leftImage  = "c:\\Work\\Data\\relativePose\\IMG_1192.jpg";
	char* rightImage = "c:\\Work\\Data\\relativePose\\IMG_1193.jpg";


	IplImage* pLeft  = cvLoadImage(leftImage);
	IplImage* pRight = cvLoadImage(rightImage);

	//1. detect feature points 
	CFeatureBase* pFeatDetect = new CSIFT();
	ImgFeature lImageFeature, rImageFeature;
	pFeatDetect->Detect(leftImage, lImageFeature);
	pFeatDetect->Detect(rightImage, rImageFeature);

	//2. matching 
	CMatchBase* pMatch = new CSiftMatch();
	PairMatchRes mp;
	pMatch->Match(lImageFeature, rImageFeature, mp);
	int nMatch = mp.matchs.size();
	printf("Matching Point Number: %d \n", nMatch);


	//display 
	srand(100);
	for(int i=0; i<mp.matchs.size(); i++)
	{
		int li = mp.matchs[i].l;
		int ri = mp.matchs[i].r;

		CvPoint pl;
		pl.x = lImageFeature.featPts[li].x;
		pl.y = lImageFeature.featPts[li].y;
		CvPoint pr;
		pr.x = rImageFeature.featPts[ri].x;
		pr.y = rImageFeature.featPts[ri].y;
		
		int r = 255; //(double)(rand())/(double)(RAND_MAX)*255;
		int g = 0;   //(double)(rand())/(double)(RAND_MAX)*255;
		int b = 0;   //(double)(rand())/(double)(RAND_MAX)*255;
		cvDrawCircle(pLeft, pl, 2, CV_RGB(r,g,b),2);
		cvDrawCircle(pRight, pr, 2, CV_RGB(r,g,b),2);
	}
	cvSaveImage("c:\\temp\\left.jpg", pLeft);
	cvSaveImage("c:\\temp\\right.jpg", pRight);

	//3. relative pose 
	vector<Point2DDouble> lpts,rpts;
	nMatch = mp.matchs.size();
	for(int i=0; i<nMatch; i++)
	{
		int li = mp.matchs[i].l;
		int ri = mp.matchs[i].r;
		Point2DDouble pl,pr;
		pl.p[0] = lImageFeature.featPts[li].cx;
		pl.p[1] = lImageFeature.featPts[li].cy;
		lpts.push_back(pl);
		pr.p[0] = rImageFeature.featPts[ri].cx;
		pr.p[1] = rImageFeature.featPts[ri].cy;
		rpts.push_back(pr);
	}

	CRelativePoseBase* pRP = new CEstimatePose5Point();
	CameraPara cam1, cam2;
	//intrinsic parameters
	cam1.focus = 3212;//pLeft->width*0.5;
	cam2.focus = 3212;//pRight->width*0.5;
	pRP->EstimatePose(lpts, rpts, cam1, cam2 );
    
	//output
	printf("Focus: %lf \n", cam2.focus);
	printf("Rotation Angle x:%lf y:%lf z:%lf \n", cam2.ax, cam2.ay, cam2.az);
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
    
	//4. triangulation
    CTriangulateBase* pTriangulate = new CTriangulateCV();
	vector<Point3DDouble> gpts;
	pTriangulate->Triangulate(lpts, rpts, cam1, cam2, gpts);
    /*
	for(int i=0; i<gpts.size(); i++)
	{
		printf("%lf %lf %lf \n", gpts[i].p[0], gpts[i].p[1], gpts[i].p[2]);
	}
	*/

	//5. ouput 3D model
	CModelFileBase* pOutput = new CPlyModel();
	//color setting
	vector<Point3DDouble> colors;
	for(int i=0; i<gpts.size(); i++)
	{
		Point3DDouble c;
		c.p[0]=255; c.p[1]=0; c.p[2]=0;
		colors.push_back(c);
	}
	//add the camera position into the 3D point set
	Point3DDouble pos;
	pos.p[0]=cam1.t[0];  pos.p[1] = cam1.t[1];  pos.p[2] = cam1.t[2];
	gpts.push_back(pos);
	pos.p[0]=cam2.t[0];  pos.p[1] = cam2.t[1];  pos.p[2] = cam2.t[2];
	gpts.push_back(pos);
	Point3DDouble c;
	c.p[0]=0; c.p[1]=255; c.p[2]=0;
	colors.push_back(c);
	colors.push_back(c);

	pOutput->Save("c:\\temp\\model.ply", gpts,colors);

	printf("Finished ! \n");
}

void CProject_PGStationDlg::OnSimulatedataTwoviews()
{
	// TODO: Add your command handler code here

	OnSfmSimulate();

}

void CProject_PGStationDlg::OnSimulatedataMultiviews()
{
	// TODO: Add your command handler code here

}

void CProject_PGStationDlg::OnObsoluteposeSimulate()
{
	// TODO: Add your command handler code here

	int npt = 20;

	//read 3D points and projection points
	//3D points for simulation
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

	vector<Point3DDouble> pt3d;
	for(int i=0; i<npt; i++)
	{
		Point3DDouble pt;
		pt.p[0] = grdpt[i][0];
		pt.p[1] = grdpt[i][1];
		pt.p[2] = grdpt[i][2];
		pt3d.push_back(pt);
	}

	vector<Point2DDouble> p1;
	vector<Point2DDouble> p2;
	
	/*
	FILE* fp = fopen("c:\\temp\\imagept.txt", "r");
	int numPt;
	fscanf(fp, "%d", &numPt);
	for(int i=0; i<numPt; i++)
	{
		double rx,ry;
		fscanf(fp, "%lf %lf ", &rx, &ry);
		Point2DDouble tp1,tp2;
		tp2.p[0] = rx;
		tp2.p[1] = ry;
		p2.push_back(tp2);
	}
	fclose(fp);
	*/

	//read projection point coordinates
	char ptfile[256] = "c:\\temp\\Data\\SFM\\imagept2.txt";
	ReadProjections(ptfile, p2);

	double K[9],R[9],t[3];
	CPoseEstimationBase* pPoseEstimation = new CDLTPose();
	pPoseEstimation->EstimatePose(pt3d, p2, K, R, t);

}

void CProject_PGStationDlg::OnEigenobjectTrainging()
{
	// TODO: Add your command handler code here

	/*char samplesPath[256] = "c:\\temp\\Data\\Logo\\classification\\vw";
	char dataPath[256] = "c:\\temp\\pca.dat";

	CEigenObjBase* pEigenObj = new CEigenLogo();
	pEigenObj->Train(samplesPath, dataPath);*/
}


void CProject_PGStationDlg::OnEigenobjectRecognition()
{
	// TODO: Add your command handler code here
	//
	//char* trainedDataPath = "c:\\temp\\pca.dat";
	//char* benchPath = "c:\\temp\\Data\\Logo\\bench"; 
	//CEigenObjBase* pEigenObj = new CEigenLogo();
	//pEigenObj->Load(trainedDataPath);

	//printf("\n Bench Image... \n");
	//pEigenObj->GenerateBench(benchPath);

	//
	////1. normalized the images	
	//const int nht = 48;
	//const int nwd = 48;
	//int nDim = nht*nwd;
	//IplImage* pNormalImage = cvCreateImage( cvSize(nwd,nht), 8, 1);
	//float* pFeat = (float*)malloc(nht*nwd*sizeof(float)); 

	////calculate the projection parameters of bench images
	//char samplesPath[256] = "c:\\temp\\Data\\Logo\\test\\vw";
	////get the number of images(samples)
	//char** filenames = NULL;
	//int n,nfile;
	//n = nfile = 0;
	//GetDirFileName(filenames, samplesPath, &n, &nfile, "jpg", 0);
	//filenames = f2c(nfile, 256);
	//GetDirFileName(filenames, samplesPath, &n, &nfile, "jpg", 1);

	//for(int i=0; i<nfile; i++)
	//{
	//	//printf("%s \n", filenames[i]);
	//	IplImage* pImage = cvLoadImage( filenames[i], 0);
	//	cvResize(pImage, pNormalImage);
	//	for(int m=0; m<nht; m++)
	//		for(int n=0; n<nwd; n++)
	//		{
	//			pFeat[m*nwd+n] = (unsigned char)( pNormalImage->imageData[m*nwd+n]);
	//		}		
	//	
	//	int nclass = pEigenObj->Recognize(pFeat, nDim);
	//	
	//	printf("%d \n", nclass);

	//	cvReleaseImage(&pImage);
	//}   
}

void CProject_PGStationDlg::OnToolNormalize()
{
	// TODO: Add your command handler code here

	//image normalization
	IplImage* pImage = CloneCurrentImage();
	if(pImage!=NULL)
	{
		unsigned char* pBuffer = NULL;
		int ht,wd;
		IplImageToGrayImage(pImage, &pBuffer, &ht, &wd);
		
		//GrayNormFaceImage(pBuffer, ht, wd, 128, 12);
		//ProIlluminationImage(pBuffer, ht, wd);
		ImageNorm(pBuffer, ht, wd, 128, 36);

		SaveBmp("c:\\temp\\normImage.bmp", pBuffer, ht, wd);
		
		printf("Finished! \n");

		free(pBuffer);
		cvReleaseImage(&pImage);
	}
}

//
//double PCASim(IplImage* pNormalImage, CEigenObjBase* pEigenObj)
//{
//	int nht = pNormalImage->height;
//	int nwd = pNormalImage->width;
//
//	float* pbuffer = (float*)malloc(nht*nwd*sizeof(float));
//	for(int m=0; m<nht; m++)
//		for(int n=0; n<nwd; n++)
//		{
//			pbuffer[m*nwd+n] = (unsigned char)( pNormalImage->imageData[m*nwd+n]);
//		}	
//
//	//image normalization
//	ImageNorm(pbuffer, nht, nwd, IMAGE_MEAN_VALUE, IMAGE_STD);
//
//	//projection
//	float* projBuffer = (float*)malloc(nht*nwd*sizeof(float));
//	memcpy(projBuffer, pbuffer, nht*nwd*sizeof(float));
//	pEigenObj->Projection(projBuffer, nht*nwd);
//	//SaveBmp("c:\\temp\\norm.bmp", projBuffer, nht, nwd);
//
//	//similarity
//	//double dif = 0;
//	//for(int i=0; i<nht*nwd; i++)
//	double sim = SimilarityOfImage(pbuffer, projBuffer, nht, nwd);
//	//printf("Similarity: %lf \n", sim);
//
//	free(pbuffer);
//	free(projBuffer);
//
//	return sim;
//}

void CProject_PGStationDlg::OnEigenobjectProjection()
{
	// TODO: Add your command handler code here
	//char testfile[256] = "c:\\temp\\Data\\Logo\\test\\vw\\t0.jpg";
	//char testfile[256] = "c:\\temp\\Data\\Logo\\test\\benz1.jpg";
	//IplImage* pImage = cvLoadImage(testfile, 0);

	//char* trainedDataPath = "c:\\temp\\pca.dat";
	////char* benchPath = "c:\\temp\\Data\\Logo\\bench"; 
	//CEigenObjBase* pEigenObj = new CEigenLogo();
	//pEigenObj->Load(trainedDataPath);

	//IplImage* pImage = CloneCurrentImage();
	//if(pImage!=NULL)
	//{
	//	int nht = EIGEN_OBJ_HT;
	//	int nwd = EIGEN_OBJ_WD;
	//	IplImage* pNormalImage = cvCreateImage( cvSize(nwd, nht), 8, 1);
	//	cvResize(pImage, pNormalImage);

	//	
	//	double* pbuffer = (double*)malloc(nht*nwd*sizeof(double));
	//	for(int m=0; m<nht; m++)
	//		for(int n=0; n<nwd; n++)
	//		{
	//			pbuffer[m*nwd+n] = (unsigned char)( pNormalImage->imageData[m*nwd+n]);
	//		}	

	//	//image normalization
	//	ImageNorm(pbuffer, nht, nwd, IMAGE_MEAN_VALUE, IMAGE_STD);
	//	SaveBmp("c:\\temp\\norm.bmp", pbuffer, nht, nwd);

	//	//similarity based on histogram 
	//	double simHist = 0;
	//	double* pAverageImage;
	//	int     nDim;
	//	pEigenObj->GetAverage(&pAverageImage, &nDim);
	//	double hist1[256];
	//	double hist2[256];
	//	memset(hist1, 0, sizeof(double)*256);
	//	memset(hist2, 0, sizeof(double)*256);
	//	GenerateHist(pAverageImage, nDim, hist1);
	//	GenerateHist(pbuffer, nDim, hist2);
	//	simHist = HistSimilar(hist1, hist2, 256);
	//	

	//	//projection
	//	double* projBuffer = (double*)malloc(nht*nwd*sizeof(double));
	//	memcpy(projBuffer, pbuffer, nht*nwd*sizeof(double));
	//	pEigenObj->Projection(projBuffer, nht*nwd);
	//	SaveBmp("c:\\temp\\proj.bmp", projBuffer, nht, nwd);


	//	//similarity
	//	//double dif = 0;
	//	//for(int i=0; i<nht*nwd; i++)
	//	double sim = SimilarityOfImage(pbuffer, projBuffer, nht, nwd);
	//	printf("hist: %lf  eigen: %lf \n", simHist, sim);

	//	//
	//	for(int m=0; m<nht; m++)
	//		for(int n=0; n<nwd; n++)
	//		{
	//			//pbuffer[m*nwd+n] = (unsigned char)( pNormalImage->imageData[m*nwd+n]);
	//			pNormalImage->imageData[m*nwd+n] = projBuffer[m*nwd+n];
	//		}	
	//	free(pbuffer);
	//	free(projBuffer);
	//	

	//	//double sim = PCASim(pNormalImage, pEigenObj);
	//	//printf("sim: %lf \n", sim);

	//	cvNamedWindow("Projection");
	//	cvShowImage("Projection", pNormalImage);
	//	
	//	cvReleaseImage(&pImage);
	//	cvReleaseImage(&pNormalImage);
	//}
	//delete pEigenObj;
	////printf("EigenObject Projection Finished! \n");
}


void CProject_PGStationDlg::OnVehiclelogoPca()
{
	//// TODO: Add your command handler code here
	//char* trainedDataPath = "c:\\temp\\pca.dat";
	//CEigenObjBase* pEigenObj = new CEigenLogo();
	//pEigenObj->Load(trainedDataPath);

	//IplImage* pImage = CloneCurrentImage();
	//if(pImage!=NULL)
	//{
	//	int nht = EIGEN_OBJ_HT;
	//	int nwd = EIGEN_OBJ_WD;
	//	int nstep = 2;
	//	int nLevel = 10;
	//	double scale = 1;
	//	int ht = pImage->height;
	//	int wd = pImage->width;

	//	IplImage* pNorm = cvCreateImage(cvSize(nwd, nht), 8, 1);

	//	for(int i=0; i<nLevel; i++)
	//	{
	//		printf("Level: %d \n", i);

	//		scale = pow(1.1, i);
	//		int sht = ht/scale;
	//		int swd = wd/scale;
	//		
	//		IplImage* pScaleImage = cvCreateImage(cvSize(swd,sht), 8, 1);
	//		cvResize(pImage, pScaleImage);

	//		for(int m=0; m<(sht-nht-1); m+=nstep)
	//			for(int n=0; n<(swd-nwd-1); n+=nstep)
	//			{
	//				CvRect rect;
	//				rect.x = n;
	//				rect.y = m;
	//				rect.height = nht;
	//				rect.width  = nwd;
	//				cvSetImageROI(pScaleImage, rect);
	//				cvCopy(pScaleImage, pNorm);
	//				cvResetImageROI(pScaleImage);
	//				//cvSaveImage("c:\\temp\\patch.jpg", pNorm);

	//				//projection
	//				double sim = PCASim(pNorm, pEigenObj);



	//				if(sim>0.90)
	//				{
	//					printf("Similarity: %lf \n", sim);
	//					CvRect orect;
	//					orect.x = rect.x*scale;
	//					orect.y = rect.y*scale;
	//					orect.height = rect.height*scale;
	//					orect.width  = rect.width*scale;
	//					CvPoint p1,p2;
	//					p1.x = orect.x;
	//					p1.y = orect.y;
	//					p2.x = p1.x + orect.width;
	//					p2.y = p1.y + orect.height;
	//					cvDrawRect(pImage, p1, p2, CV_RGB(255,255,255));
	//				}
	//			}

	//		cvNamedWindow("LogoDetect");
	//		cvShowImage("LogoDetect", pImage);

	//		cvReleaseImage(&pScaleImage);
	//	}
	//}
}

void CProject_PGStationDlg::OnSimulatedataSingleview()
{
	// TODO: Add your command handler code here

	char filename[256];

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

	//intrinsic parameters
	double focus = 240;
	int ht = 480;
	int wd = 640;
	int x0 = 0;
	int y0 = 0;
	double R2[9], t2[3];
	double omiga, phi, kapa;

	//exterior parameters
	//image 0
	focus  = 300;
	x0 = 0; y0 = 0;
	omiga  = 0;
	phi    = 0;
	kapa   = 0;	
	t2[0] = 0; t2[1] = 0; t2[2] = 0;
	strcpy(filename, "c:\\temp\\Data\\SFM\\imagept0.txt");
	
	//image 1
	focus = 240;
	x0 = 0; y0 = 0;
	omiga  = 1;
	phi    = -2;
	kapa   = 1.5;
	t2[0] = 20; t2[1] = 20; t2[2] = 10;	
	strcpy(filename, "c:\\temp\\Data\\SFM\\imagept1.txt");
	
	/*
	//image 2
	focus = 180;
	x0 = 0; y0 = 0;
	omiga  = -0.5;
	phi    = 1;
	kapa   = 3;
	t2[0] = 10; t2[1] = -10; t2[2] = 5;
	strcpy(filename, "c:\\temp\\Data\\SFM\\imagept2.txt");
    */

	printf("Output: %s \n", filename);
	printf("focus: %lf \n", focus);
	printf("principal point: %lf %lf \n", x0, y0);
	printf("Translation: %lf %lf %lf \n", t2[0], t2[1], t2[2]);
	printf("Rotation: %lf %lf %lf \n", omiga, phi, kapa);
	GenerateRMatrix(omiga, phi, kapa, R2);	
	for(int j=0; j<3; j++)
	{
		for(int i=0; i<3; i++)
		{
			printf("%lf ", R2[j*3+i]);
		}
		printf("\n");
	}
	printf("\n");

	//output simulated projection points
	int numpt = 20;
	FILE* fp = fopen(filename, "w");
	fprintf(fp, "%d\n", numpt);
	for(int i=0; i<numpt; i++)
	{
		srand(time(0));
		double ix2,iy2;
		GrdToImg(grdpt[i][0], grdpt[i][1], grdpt[i][2], &ix2, &iy2, 
			R2, t2, focus, x0, y0, ht, wd );
		//printf("%lf %lf  %lf %lf \n", ix1, iy1, ix2, iy2);
		//random noise
		double rx2 = ( (double)(rand())/(double)(RAND_MAX) - 0.5 )*1;
		double ry2 = ( (double)(rand())/(double)(RAND_MAX) - 0.5 )*1;
		//coordinate translation from topleft to the center
		fprintf(fp, "%lf %lf  \n", ix2-wd*0.5+rx2, ht*0.5-iy2+ry2);
	}
	fclose(fp);
}




void CProject_PGStationDlg::OnHjAtmosphericcorrection()
{
	// TODO: Add your command handler code here

	//loading LUT data
	char lutDir[256] = "c:\\temp\\Data\\LutTypeHJ";
	vector<CAtmosphereCorrectionBase*> vecCorrectBlue;
	vector<CAtmosphereCorrectionBase*> vecCorrectRed;

	//int nTitle[3]={0,10,25};//index for lut of different reflectance

	int aerosolTypeNumber = 3;
	LoadLutsForDDV(lutDir, aerosolTypeNumber, vecCorrectBlue, vecCorrectRed);


	//char spath[256] = "c:\\temp\\Data\\HJ\\2013\\10\\17";
	//char spath[256] = "c:\\temp\\Data\\HJ\\2013\\10\\04";
	//char spath[256] = "c:\\temp\\Data\\HJ\\2013\\09\\28";

	//char spath[256] = "c:\\temp\\Data\\HJ\\2010\\9\\12";
	//char spath[256] = "c:\\temp\\Data\\HJ\\2010\\10\\29";
	char spath[256] = "c:\\temp\\Data\\HJ\\2010\\09\\30";
	//char spath[256] = "c:\\temp\\Data\\HJ\\2010\\9\\28";
	//char spath[256] = "c:\\temp\\Data\\HJ\\2010\\6\\25";
	//char spath[256] = "c:\\temp\\Data\\HJ\\2010\\08\\16";


	//wavelength for HJ bands (um)
	double blueWaveLen = 0.475;
	double redWaveLen  = 0.660;
	double irWaveLen   = 0.830;

	double fluxBlue = GetFlux(blueWaveLen);
	double fluxRed  = GetFlux(redWaveLen);
	double fluxIr   = GetFlux(irWaveLen);

	//char xmlFileTitle[256] = "HJ1A-CCD2-1-68-20100816-L20000375929.XML";         //2010.08.16
	//char xmlFileTitle[256] = "HJ1B-CCD2-2-68-20131004-L20001064410.XML";          //2013.10.04
	//char xmlFileTitle[256] = "HJ1A-CCD2-456-68-20131017-L20001068649.XML";     //2013.10.17
	//char xmlFileTitle[256] = "HJ1B-CCD2-2-68-20131004-L20001064410.XML";       //2013.10.04
	//char xmlFileTitle[256] = "HJ1A-CCD2-1-68-20130928-L20001059100.XML";       //2013.09.28
	char xmlFileTitle[256] = "HJ1B-CCD2-2-67-20100930-L20000401266.XML";         //2010.09.30
	//char xmlFileTitle[256] = "HJ1A-CCD2-456-68-20100912-L20000391060.XML";
	//char xmlFileTitle[256] = "HJ1A-CCD2-456-68-20100912-L20000391060.XML";
	//sprintf(xmlFile, "%s\\%s", spath, "HJ1A-CCD2-457-68-20101029-L20000417622.XML");
	//sprintf(xmlFile, "%s\\%s", spath, "HJ1B-CCD2-2-67-20100930-L20000401266.XML");
	//sprintf(xmlFile, "%s\\%s", spath, "HJ1A-CCD2-2-66-20100928-L20000399932.XML");
	//sprintf(xmlFile, "%s\\%s", spath, "HJ1B-CCD1-2-64-20100625-L20000331824.XML");

	char fileTitle[256];
	strcpy(fileTitle, xmlFileTitle);
	strcpy(fileTitle+strlen(fileTitle)-4, "\0");

	//load HJ XML data
	char xmlFile[256];
	sprintf(xmlFile, "%s\\%s", spath, xmlFileTitle);

	HJDATA hjdata;
	GetHJFilepaths(xmlFile, hjdata);
	ReadXMLFile(hjdata);
	double sunZenith = hjdata.solar_zenith*DPI;
	double sazimuth = hjdata.solar_azimuth;

	//load angle data
	char angleFile[256]; 
	sprintf(angleFile, "%s\\%s-SatAngle.txt", spath, fileTitle);

	CReadAngleBase* pReadAngle = new CReadHJAngle();
	pReadAngle->Read(angleFile);

	char oriTiffFile[256];
	sprintf(oriTiffFile, "%s\\%s-1.TIF", spath, fileTitle);

	stGeoInfo oriGeoInfo;
	GetGeoInformation(oriTiffFile, oriGeoInfo);

	//loading images
	char sband1[256];
	char sband2[256];
	char sband3[256];
	char sband4[256];
	sprintf(sband1, "%s\\%s", spath, "band1-clip-envi.tif");
	sprintf(sband2, "%s\\%s", spath, "band2-clip-envi.tif");
	sprintf(sband3, "%s\\%s", spath, "band3-clip-envi.tif");
	sprintf(sband4, "%s\\%s", spath, "band4-clip-envi.tif");

	//read geoinfo header for clipped image
	stGeoInfo subGeoInfo;
	GetGeoInformation(sband1, subGeoInfo);

	unsigned char* pband1 = NULL;
	unsigned char* pband2 = NULL;
	unsigned char* pband3 = NULL;
	unsigned char* pband4 = NULL;
	int ht,wd;
	ReadGeoFileByte(sband1, 1, &pband1, ht, wd);
	ReadGeoFileByte(sband2, 1, &pband2, ht, wd);
	ReadGeoFileByte(sband3, 1, &pband3, ht, wd);
	ReadGeoFileByte(sband4, 1, &pband4, ht, wd);

	//loading aod file
	char aodfile[256] = "c:\\temp\\aod-interpolation.tif";
    float* paod = NULL;
	int aodht,aodwd;
	ReadGeoFileFloat(aodfile, 1, &paod, aodht, aodwd);

	for(int j=0; j<ht; j++)
		for(int i=0; i<wd; i++)
		{
			//DN value
			unsigned char r,g,b,ir;
			b  = pband1[j*wd+i];
			r  = pband3[j*wd+i];
			ir = pband4[j*wd+i];

			//convert from DN to reflectance
			if(b>0 || r>0 || ir>0)
			{
				//convert to ground coordinate
				double gy = subGeoInfo.top  + j*subGeoInfo.dy;
				double gx = subGeoInfo.left + i*subGeoInfo.dx;
				int row =  (oriGeoInfo.top - gy)  / fabs(oriGeoInfo.dy);
				int col =  (gx - oriGeoInfo.left) / fabs(oriGeoInfo.dx);
				double vza, vazimuth;
				pReadAngle->GetViewGeo(row, col, vza, vazimuth);
				if(vazimuth<0) vazimuth += 360;
				double relativeAzimuth = fabs(vazimuth-sazimuth);
				if(relativeAzimuth>180) relativeAzimuth -= 180;

				//from DN to radiance
				double refBlue = pband1[j*wd+i] / hjdata.m_gain[0] + hjdata.m_offset[0]; //band 1
				double refRed  = pband3[j*wd+i] / hjdata.m_gain[2] + hjdata.m_offset[2]; //band 3
				double refIR   = pband4[j*wd+i] / hjdata.m_gain[3] + hjdata.m_offset[3]; //band 4				

				//from radiance to reflectance
				refBlue = (refBlue*PI)/(fluxBlue*cos(sunZenith));
				refRed  = (refRed*PI)/(fluxRed*cos(sunZenith));
				refIR   = (refIR*PI)/(fluxIr*cos(sunZenith));

				double ndvi = (refIR-refRed) / (refIR+refRed);
				
			}			
		}	


		printf("Finished! \n");
}

void CProject_PGStationDlg::OnBaSimulate()
{
	// TODO: Add your command handler code here

	//bundle adjustment using simulated data
	int npt = 20;
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

	vector<Point3DDouble> pt3d;
	for(int i=0; i<npt; i++)
	{
		Point3DDouble pt;
		pt.p[0] = grdpt[i][0];
		pt.p[1] = grdpt[i][1];
		pt.p[2] = grdpt[i][2];
		pt3d.push_back(pt);
	}

	//read projection points
	char filename0[256] = "c:\\temp\\Data\\SFM\\imagept0.txt";
	char filename1[256] = "c:\\temp\\Data\\SFM\\imagept1.txt";
	char filename2[256] = "c:\\temp\\Data\\SFM\\imagept2.txt";
	vector<Point2DDouble> vp1;
	vector<Point2DDouble> vp2;
	vector<Point2DDouble> vp3;
	ReadProjections(filename0, vp1);
	ReadProjections(filename1, vp2);
	ReadProjections(filename2, vp3);
	
	//relative pose estimation
	CRelativePoseBase* pRP = new CEstimatePose5Point();
	CameraPara cam1,cam2;
	double focus = 250;    
	cam1.focus = focus;
	cam2.focus = focus;
	pRP->EstimatePose(vp1, vp2, cam1, cam2);

	for(int j=0; j<3; j++)
	{
		for(int i=0; i<3; i++)
		{
			printf("%lf ", cam2.R[j*3+i]);
		}
		printf("\n");
	}
	printf("translation: %lf %lf %lf \n", cam2.t[0], cam2.t[1], cam2.t[2]);
	printf("rotation angle: %lf %lf %lf \n", cam2.ax, cam2.ay, cam2.az);

	//triangulation
	CTriangulateBase* pTriangulate = new CTriangulateCV();
	vector<Point3DDouble> gpts;
	pTriangulate->Triangulate(vp1, vp2, cam1, cam2, gpts);
	for(int i=0; i<gpts.size(); i++)
	{
		double rx = gpts[i].p[0] / grdpt[i][0];
		double ry = gpts[i].p[1] / grdpt[i][1];
		double rz = gpts[i].p[2] / grdpt[i][2];

		printf("%lf %lf %lf   %lf %lf %lf \n", gpts[i].p[0], gpts[i].p[1], gpts[i].p[2],
			rx, ry, rz);
	}
	
	//add new image
	CameraPara cam3;
	//double K[9], R[9], t[3];
	CPoseEstimationBase* pDLTPose = new CDLTPose();
	//pDLTPose->EstimatePose(gpts, vp3, cam3.K, cam3.R, cam3.t);
	pDLTPose->EstimatePose(gpts, vp3, cam3);
	
	/*
	printf("absolute pose estimation... \n");
	for(int j=0; j<3; j++)
	{
		for(int i=0; i<3; i++)
		{
			printf("%lf ", R[j*3+i]);
		}
		printf("\n");
	}
	printf("translation: %lf %lf %lf \n",t[0], t[1], t[2]);
	*/

	//bundle adjustment
	//tracks
	vector<ImageKeyVector> ptViews;
	int numPt=20;
	ptViews.resize(numPt);
	for(int i=0; i<numPt; i++)
	{
		ImageKey key;		
		key.first = 0;
		key.second = i;
		ptViews[i].push_back(key);

		key.first = 1;
		key.second = i;
		ptViews[i].push_back(key);

		key.first = 2;
		key.second = i;
		ptViews[i].push_back(key);
	}

	//projection points in image
	vector<ImgFeature> imageFeatures;
	imageFeatures.resize(3);
	for(int j=0; j<numPt; j++)
	{ 
		stPtFeature pt;
		pt.cx = vp1[j].p[0];
		pt.cy = vp1[j].p[1];
		imageFeatures[0].featPts.push_back(pt);

		pt.cx = vp2[j].p[0];
		pt.cy = vp2[j].p[1];
		imageFeatures[1].featPts.push_back(pt);

		pt.cx = vp3[j].p[0];
		pt.cy = vp3[j].p[1];
		imageFeatures[2].featPts.push_back(pt);
	}

	//camera order
	vector<int> cameraIDOrder;
	cameraIDOrder.push_back(0);
	cameraIDOrder.push_back(1);
	cameraIDOrder.push_back(2);

	vector<CameraPara> cameras;
	cameras.push_back(cam1);
	cameras.push_back(cam2);
	cameras.push_back(cam3);

	printf("\n\n Bundle Adjustment ... \n");
	CBABase* pBA = new CSBA();
	pBA->RunSFM(gpts, ptViews, imageFeatures, cameraIDOrder, cameras);

}

void CProject_PGStationDlg::OnBaRealimages()
{
	// TODO: Add your command handler code here
	CFileDialog dlg(TRUE,NULL,NULL,
		OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_ALLOWMULTISELECT|OFN_EXPLORER,
		_T("jpg file (*.jpg)|*.jpg|bitmap file(*.bmp)|*.bmp|Tiff file(*.tif)|*.tif|IMG file(*.img)|*.img||"));

	TCHAR* szPathMulti  = new TCHAR[8*1024*1024];  //alloc memory for multiple files
	dlg.m_ofn.nMaxFile  = 8*1024*1024; 
	dlg.m_ofn.lpstrFile = szPathMulti; 
	dlg.m_ofn.lpstrFile[0] = 0; 

	//vector<string> fileVector;
	vector<CString> fileVector;
	if(dlg.DoModal()==IDOK)
	{
		POSITION  pos   =   dlg.GetStartPosition(); 
		int index = 0;		
		
		//read file paths
		while(pos != NULL) 
		{ 
			//insert image filepath into the file list control
			CString filepath = dlg.GetNextPathName(pos); 	
			//string s(filepath.GetBuffer());
			//fileVector.push_back(s);	
			fileVector.push_back(filepath);	
		}

		//sort the filenames
		sort( fileVector.begin(), fileVector.end() );
	}

	//1. generate features
	vector<IplImage*> pImages;
	CFeatureBase* pFeatDetect = new CSIFTFloat();
	vector<ImgFeature> imgFeatures;
	vector<CameraPara> cameras;	
	imgFeatures.resize(fileVector.size());  //alloc memory
	for(int i=0; i<fileVector.size(); i++)
	{		
		char* pFile = fileVector[i].GetBuffer();
		printf("%s \n", pFile);
		pFeatDetect->Detect( pFile, imgFeatures[i] );
		
		//save feature points into the file
		
		IplImage* pImg = cvLoadImage(pFile);
		pImages.push_back(pImg);
	}

	//instrisic parameters initialization
	int numCamera = fileVector.size();
	cameras.resize(numCamera);
	for(int i=0; i<fileVector.size(); i++)
	{
		cameras[i].k1 = 0;
		cameras[i].k2 = 0;
		cameras[i].focus = imgFeatures[i].ht;
	}

	//2. matching
	vector<PairMatchRes> vecMatch;
	CMatchBase* pMatch = new CKNNMatch();
	for(int i=0; i<fileVector.size(); i++)
		for(int j=i+1; j<fileVector.size(); j++)
		{
			printf("Matching %d-%d \n", i,j);
			PairMatchRes mt;
			mt.lId = i;
			mt.rId = j;
			pMatch->Match(imgFeatures[i], imgFeatures[j], mt);

			if( mt.inlierRatio>0.5 && mt.matchs.size()>16 )
			{
				vecMatch.push_back(mt);
			}
			printf("%lf %d \n", mt.inlierRatio, mt.matchs.size());
		}

	//3. generate tracks
	CGenerateTracksBase* pGenerateTracks = new CMyGenerateTrack();
	vector<TrackInfo> vecTrack;
	pGenerateTracks->GenerateTracks(vecMatch, vecTrack);
	PrintTracks(vecTrack, "c:\\temp\\tracks.txt");

	
	//display feaure points
	for(int i=0; i<vecTrack.size(); i++)
	{
		int nview = vecTrack[i].views.size();
		if(nview==3)
		{		
		for(int j=0; j<nview; j++)
		{
			ImageKey key = vecTrack[i].views[j];
			int imgId = key.first;
			int ptId = key.second;
			CvPoint pt;
			pt.x = imgFeatures[imgId].featPts[ptId].x;
			pt.y = imgFeatures[imgId].featPts[ptId].y;
			cvDrawCircle( pImages[imgId], pt, 2, CV_RGB(i*2, 0, 0), 3);
		}
		}
	}
	for(int i=0; i<pImages.size(); i++)
	{
		char imagefile[256];
		sprintf(imagefile, "c:\\temp\\%d.jpg", i);
		cvSaveImage(imagefile, pImages[i]);
	}
	//////////////////////////////////////////////////////////////////////////
	

	//4. bundle adjustment
	CBABase* pBA = new CSBA();
	char* outPath = "F:\\data\\bundler";
	pBA->BundleAdjust(numCamera, cameras, imgFeatures, vecMatch, vecTrack, outPath);
	
	
}

void CProject_PGStationDlg::OnMosaicSeamfinder()
{
	// TODO: Add your command handler code here

	/*
	char* maskfile1 = "c:\\temp\\data\\mosaic\\mask.jpg";
	char* maskfile2 = "c:\\temp\\data\\mosaic\\mask.jpg";

	IplImage *pImage1 = cvLoadImage(maskfile1, 0);
	IplImage *pImage2 = cvLoadImage(maskfile2, 0);

	unsigned char* pmask1;
	int ht1,wd1;
	unsigned char* pmask2;
	int ht2,wd2;

	IplImageToGrayImage(pImage1, &pmask1, &ht1, &wd1);
	IplImageToGrayImage(pImage2, &pmask2, &ht2, &wd2);

	printf("%d %d %d %d \n", ht1, wd1, ht2, wd2);

	//seam finder
	iPoint corner1,corner2;
	CSeamFinderBase *pSeamFinder = new CVoroniSeamFinder();
	corner1.x = 0;
	corner1.y = 0;
	corner2.x = 60;
	corner2.y = 100;
	vector<iPoint> pts;
	pts.push_back(corner1);
	pts.push_back(corner2);

	MatrixByte mask1;
	MatrixByte mask2;
	mask1.pbuffer = pmask1;
	mask1.ht = ht1;
	mask1.wd = wd1;
	mask2.pbuffer = pmask2;
	mask2.ht = ht2;
	mask2.wd = wd2;

	//pSeamFinder->FindPair(pmask1, ht1, wd1, pmask2, ht2, wd2, corner1, corner2);
	pSeamFinder->FindPair(mask1, mask2, corner1, corner2);

	//SaveBmp("c:\\temp\\mask1.bmp", pmask1, ht1, wd1);
	//SaveBmp("c:\\temp\\mask2.bmp", pmask2, ht2, wd2);
	SaveBmp("c:\\temp\\mask1.bmp", mask1.pbuffer, mask1.ht, mask1.wd);
	SaveBmp("c:\\temp\\mask2.bmp", mask2.pbuffer, mask2.ht, mask2.wd);
	vector<MatrixByte> masks;
	masks.push_back(mask1);
	masks.push_back(mask2);

	//blend
	char* imgfile1 = "c:\\temp\\data\\mosaic\\lena1.jpg";
	char* imgfile2 = "c:\\temp\\data\\mosaic\\lena2.jpg";
	IplImage* pImg1 = cvLoadImage(imgfile1, 0);
	IplImage* pImg2 = cvLoadImage(imgfile1, 0);
    MatrixByte image1,image2;
	unsigned char* pbuffer;
	int ht,wd;
	IplImageToGrayImage(pImg1, &pbuffer, &ht, &wd);
	image1.pbuffer = pbuffer;
	image1.ht = ht;
	image1.wd = wd;
	IplImageToGrayImage(pImg2, &pbuffer, &ht, &wd);
	image2.pbuffer = pbuffer;
	image2.ht = ht;
	image2.wd = wd;
	
	vector<MatrixByte> images;
	images.push_back(image1);
	images.push_back(image2);


	CBlenderBase* pBlend = new CMultiBandBlender();
	MatrixByte res;
	MatrixByte outmask;
	pBlend->Blend(images, masks, pts, res, outmask);

	cvNamedWindow("mask1");
	cvShowImage("mask1", pImage1);
	cvNamedWindow("mask2");
	cvShowImage("mask2", pImage2);

	cvWaitKey();
	*/


}

void CProject_PGStationDlg::OnMosaicBlend()
{
	// TODO: Add your command handler code here

	vector<string> filenames;
	string outFilePath;

	filenames.resize(2);

	CBlenderBase* pBlend = new CMultiBandBlender();
	pBlend->GeoBlend(filenames, outFilePath);


}

void CProject_PGStationDlg::OnGeomosaicWithblend()
{
	// TODO: Add your command handler code here

	char** filesPath = f2c(10, 256);

	strcpy(filesPath[0], "c:\\temp\\Data\\YG2\\klmy\\YG2-2_CCD-1_000061314_002_018_003_L2\\YG2-2_CCD-1_000061314_002_018_003_L2.tiff");
	strcpy(filesPath[1], "c:\\temp\\Data\\YG2\\klmy\\YG2-2_CCD-1_000061524_006_002_002_L2\\YG2-2_CCD-1_000061524_006_002_002_L2.tiff");
	strcpy(filesPath[2], "c:\\temp\\Data\\YG2\\klmy\\YG2-3_CCD-2_000062055_010_019_002_L2\\YG2-3_CCD-2_000062055_010_019_002_L2.tiff");


	CBlenderBase* pBlend = new CMultiBandBlender();
	pBlend->GeoBlend(filesPath, 3, "c:\\temp\\mosaicWithBlend.tif");

}

void CProject_PGStationDlg::OnEigenobjectHistogrammatching()
{
	// TODO: Add your command handler code here

}

void CProject_PGStationDlg::OnEigenobjectSimilarity()
{
	// TODO: Add your command handler code here

	//char* trainedDataPath = "c:\\temp\\pca.dat";
	////char* benchPath = "c:\\temp\\Data\\Logo\\bench"; 
	//CEigenObjBase* pEigenObj = new CEigenLogo();
	//pEigenObj->Load(trainedDataPath);

	////image directory
	////char* imagepath = "c:\\temp\\Data\\Logo\\train\\vw1";
	//char* imagepath = "c:\\temp\\Data\\Logo\\train\\vw1";
	//char** filenames = NULL;
	//int n,nfile;
	//n = 0;
	//nfile = 0;
	//GetDirFileName(filenames, imagepath, &n, &nfile, "jpg", 0);
	//filenames = f2c(nfile, 256);
	//GetDirFileName(filenames, imagepath, &n, &nfile, "jpg", 1);

	//int nht = EIGEN_OBJ_HT;
	//int nwd = EIGEN_OBJ_WD;
	//IplImage* pNormalImage = cvCreateImage( cvSize(nwd, nht), 8, 1);


	//int nPos = 0;
	//for(int i=0; i<nfile; i++)
	//{
	//	printf("%s ", filenames[i]);

	//	IplImage* pImage = cvLoadImage(filenames[i], 0); //CloneCurrentImage();
	//	
	//	if(pImage!=NULL)
	//	{			
	//		cvResize(pImage, pNormalImage);

	//		//
	//		//cvTransform()


	//		/*
	//		double* pbuffer = (double*)malloc(nht*nwd*sizeof(double));
	//		for(int m=0; m<nht; m++)
	//			for(int n=0; n<nwd; n++)
	//			{
	//				pbuffer[m*nwd+n] = (unsigned char)( pNormalImage->imageData[m*nwd+n]);
	//			}	

	//		//image normalization
	//		ImageNorm(pbuffer, nht, nwd, IMAGE_MEAN_VALUE, IMAGE_STD);

	//		//similarity based on histogram 
	//		double simHist = 0;
	//		double* pAverageImage;
	//		int     nDim;
	//		pEigenObj->GetAverage(&pAverageImage, &nDim);
	//		double hist1[256];
	//		double hist2[256];
	//		memset(hist1, 0, sizeof(double)*256);
	//		memset(hist2, 0, sizeof(double)*256);
	//		GenerateHist(pAverageImage, nDim, hist1);
	//		GenerateHist(pbuffer, nDim, hist2);
	//		simHist = HistSimilar(hist1, hist2, 256);

	//		//projection
	//		double* projBuffer = (double*)malloc(nht*nwd*sizeof(double));
	//		memcpy(projBuffer, pbuffer, nht*nwd*sizeof(double));
	//		pEigenObj->Projection(projBuffer, nht*nwd);
	//		//SaveBmp("c:\\temp\\norm.bmp", projBuffer, nht, nwd);

	//		//similarity
	//		//double dif = 0;
	//		//for(int i=0; i<nht*nwd; i++)
	//		double sim = SimilarityOfImage(pbuffer, projBuffer, nht, nwd);
	//		printf("hist: %lf  eigen: %lf \n", simHist, sim);
	//	
	//		free(pAverageImage);
	//		free(pbuffer);
	//		free(projBuffer);
	//		*/

	//		double sim = PCASim(pNormalImage, pEigenObj);
	//		if(sim>0.85)
	//			nPos++;

	//		printf("%lf \n", sim);
	//		cvReleaseImage(&pImage);
	//	}
	//}


	//double pr = double(nPos) / double(nfile);
	//printf("ratio: %lf \n", pr);

	//cvReleaseImage(&pNormalImage);
	//delete pEigenObj;
}

void CProject_PGStationDlg::OnHjAngleinterpolation()
{
	// TODO: Add your command handler code here

	char tifPath[256]   = "c:\\temp\\Data\\HJ\\Beijing\\HJ1A-CCD1-457-66-20100402-L20000277541\\277541\\HJ1A-CCD1-457-66-20100402-L20000277541-1.TIF";
	char anglePath[256] = "c:\\temp\\Data\\HJ\\Beijing\\HJ1A-CCD1-457-66-20100402-L20000277541\\277541\\HJ1A-CCD1-457-66-20100402-L20000277541-SatAngle.txt";
	char xmlPath[256];

	//get the sun azimuth from the XML file
	double sunZenith = 0;


	stGeoInfo geoInfo;
	GetGeoInformation(tifPath, geoInfo);

	int ht = geoInfo.ht;
	int wd = geoInfo.wd;

	char  lineBuffer[1024];
	int nsize = sizeof(unsigned short);
	double mb = (double)(ht*wd*nsize) / 1024.0 / 1024.0;
	printf("unsigned short: %d  MB: %lf \n", nsize, mb);	


	FILE* fp = fopen(anglePath, "r");
	//skip the file header
	for(int i=0; i<3; i++)
	{
		fgets(lineBuffer, 1024, fp);
		printf("%s \n", lineBuffer);
	}
	//read data
	double value[8];
	int nrow = GetFileRows(anglePath);
	printf("%d \n", nrow);
	double* px      = (double*)malloc(sizeof(double)*nrow);
	double* py      = (double*)malloc(sizeof(double)*nrow);
	int* zenith  = (int*)malloc(sizeof(int)*nrow);
	int* azimuth = (int*)malloc(sizeof(int)*nrow);
	for(int i=0; i<nrow; i++)
	{
		for(int j=0; j<8; j++)
		{
			fscanf(fp, "%lf", &value[j]);
		}		
		//
		//printf("%lf %lf %lf %lf \n", value[2], value[3], value[6], value[7]);
		py[i] = value[2];
		px[i] = value[3];
		zenith[i]  = value[6]; //abs( value[6]*10 );

		azimuth[i] = abs( value[7]);
	}
	fclose(fp);	


	//HJAngleInterpolationForward(px, py, zenith, nrow, geoInfo, "c:\\temp\\zenith.tif");
	//HJAngleInterpolationForward(px, py, azimuth, nrow, geoInfo, "c:\\temp\\azimuth.tif");

	/*
	//generate triangle 
	CTINClass* pTin = new CTINClass("aaa");
	GenerateDelaunayTriangle(pTin, px, py, nrow);
	IplImage* pImage = cvCreateImage( cvSize(wd, ht), 8, 1);
	DrawDelaunayTriangle(pImage, pTin);
	cvSaveImage("c:\\temp\\tri.jpg", pImage);
	cvReleaseImage(&pImage);
	*/
	
	printf("Finished! \n");
}

void CProject_PGStationDlg::OnTemplatematchingFast()
{
	// TODO: Add your command handler code here

	char* oriFile      = "c:\\temp\\Data\\TemplateMatching\\Template_Matching_Original_Image.jpg";
	char* templateFile = "c:\\temp\\Data\\TemplateMatching\\Template_Matching_Template_Image.jpg";

	//char oriFile[256];
	//char templateFile[256];

	//IplImage* pImage = CloneCurrentImage();
	//if(pImage!=NULL)
	{
		//strcpy(oriFile, GetCurrentImageFilePath());
		//strcpy(templateFile, "c:\\temp\\template.jpg");

		//CTemplateMatchBase* pTM   = new CBasicTM();	
		CTemplateMatchBase* pTM = new CFastTM();
		pTM->Match(oriFile, templateFile);

		delete pTM;
	}
}

void CProject_PGStationDlg::OnHarrIntegralimage()
{
	// TODO: Add your command handler code here


	//IplImage* pImage = CloneCurrentImage();
	//if(pImage!=NULL)
	{
		//CV_16U
		//IplImage* sum = cvCreateImage( cvGetSize(pImage), 32, 1);
		//cvIntegral(pImage, sum);
		//cvReleaseImage(&pImage);

		HarrFeature* pHarr = new HarrFeature(64,64);
		pHarr->CalculateFeatNum();
		int nFeat = pHarr->GetAllSubWinNumber();
		printf("%d \n", nFeat);
	}
}

void CProject_PGStationDlg::OnTemplatematchingHarr()
{
	// TODO: Add your command handler code here
	//char* oriFile      = "c:\\temp\\Data\\TemplateMatching\\Template_Matching_Original_Image.jpg";
	//char* templateFile = "c:\\temp\\Data\\TemplateMatching\\Template_Matching_Template_Image.jpg";

	char* oriFile      = "c:\\temp\\Data\\video\\bridge\\0050.jpg";
	char* templateFile = "c:\\temp\\Data\\TemplateMatching\\car_template-small.jpg";

	//calculate the Harr feature of template image
	IplImage* pImage = cvLoadImage(templateFile, 0);
	int ht = pImage->height;
	int wd = pImage->width;
	int scanwd = pImage->widthStep;

	printf("Template: %d  %d \n", ht, wd);

	int* pIntegral = (int*)malloc( ht*wd*sizeof(int) );
	for(int j=0; j<ht; j++)
		for(int i=0; i<wd; i++)
		{
			pIntegral[j*wd+i] = (unsigned char)(pImage->imageData[j*scanwd+i]);
		}
	SaveBmp("c:\\temp\\ori.bmp", pIntegral, ht, wd);
	CalculateIntegralImage1D(pIntegral, ht, wd);


	int block = 8;
	int nFeat = block*block*9;
	double* pTemplateFeat = (double*)malloc(sizeof(double)*nFeat);
	memset(pTemplateFeat, 0, sizeof(double)*nFeat);
	MyRectF trect;
	trect.x = 0;
	trect.y = 0;
	trect.height = ht;
	trect.width  = wd;
    int nFeat1 = GenerateHarrFeature(pIntegral, ht, wd, trect, block, pTemplateFeat );
	printf("%d \n", nFeat1);
   	
	
	printf("\n");
	for(int i=0; i<nFeat; i++)
	{
		printf("%6.2lf ", pTemplateFeat[i]);
	}
	printf("\n");
	

	//search 
	//IplImage* pSrc = cvLoadImage(oriFile, 0);
	IplImage* pSrc = CloneCurrentImage();

	if(pSrc==NULL)
		return;
	
	int sht = pSrc->height;
	int swd = pSrc->width;
	int sscanwd = pSrc->widthStep;
	int* pSrcIntegral = (int*)malloc( sht*swd*sizeof(int) );
	for(int j=0; j<sht; j++)
		for(int i=0; i<swd; i++)
		{
			pSrcIntegral[j*swd+i] = (unsigned char)(pSrc->imageData[j*sscanwd+i]);
		}
	//SaveBmp("c:\\temp\\src.bmp", pSrcIntegral, sht, swd);
	CalculateIntegralImage1D(pSrcIntegral, sht, swd);
	
	int    dx,dy;
	double dscale = 1.0;
	double* pFeat = (double*)malloc(sizeof(double)*nFeat);
	int nstep = 2;
    
	//save the best feature
	double* pBestFeat = (double*)malloc(sizeof(double)*nFeat);
    int nBestFeatDim = 0;

	double scaleRatio = 1.1;
	double sratio = 1;
	int    rht,rwd;
	rht = sht/sratio;
	rwd = swd/sratio;

	double mindif = 10000000000;
	while( rht>trect.height && rwd>trect.width )
	{
		//printf("\n %lf %d %d \n", sratio, rht, rwd);
		printf("\n %lf \n", sratio);
	
		double zoom = 1.0/(sratio*sratio);
		for(int j=0; j<(rht-trect.height); j+=nstep)
			for(int i=0; i<(rwd-trect.width); i+=nstep)
			{
				MyRectF rect;
				rect.x = i*sratio;
				rect.y = j*sratio;
				rect.width = trect.width*sratio;
				rect.height = trect.height*sratio;
				
				
				//memset(pFeat, 0, sizeof(double)*nFeat);
				int nFeat1 = GenerateHarrFeature(pSrcIntegral, sht, swd, rect, block, pFeat );
				
				double dif = 0;				
				for(int k=0; k<nFeat1; k++)
				{
					double td = pFeat[k]*zoom-pTemplateFeat[k];
					if(td<0) td=-td;
					dif += td;
				}
				
				
				if(dif<mindif)
				{
					mindif = dif;
					dx = i;
					dy = j;
					dscale = sratio;
					//printf("%d %d  %lf ", dx, dy, mindif);
					printf(" %lf ", mindif);
					memcpy(pBestFeat, pFeat, sizeof(double)*nFeat);
					nBestFeatDim = nFeat1;
				}
			}
			sratio *= scaleRatio;
			rht /= scaleRatio;
			rwd /= scaleRatio;
	}

	CvPoint p1,p2;
	p1.x = dx*dscale;
	p1.y = dy*dscale;
	p2.x = p1.x+trect.width*dscale;
	p2.y = p1.y+trect.height*dscale;
	cvDrawRect(pSrc, p1, p2, CV_RGB(255,255,255), 2);

	//
	double sim = Cov(pBestFeat, pTemplateFeat, nBestFeatDim);



	printf("Result: %lf %d  %d \n", dscale, (int)(trect.height*dscale), (int)(trect.width*dscale) );
	printf("Similarity: %lf \n", sim);

	cvNamedWindow("TemplateMatch");
	cvShowImage("TemplateMatch", pSrc);
}


/*
//resize the image
sht /= scaleRatio;
swd /= scaleRatio;
IplImage* pScaleImage = cvCreateImage( cvSize(swd, sht), 8, 1);
cvResize( pSrc, pScaleImage);
int scanwd1 = pScaleImage->widthStep;
for(int m=0; m<sht; m++)
for(int n=0; n<swd; n++)
{
pSrcIntegral[m*swd+n] = (unsigned char)(pScaleImage->imageData[m*scanwd1+n]);
}
*/
//cvReleaseImage(&pSrc);
//pSrc = cvCloneImage(pScaleImage);

void CProject_PGStationDlg::OnExternalformatHdf()
{
	// TODO: Add your command handler code here
	
	//read HDF file
	char hdfFile[256] = "c:\\temp\\Data\\MODIS\\2006-05-17\\MOD021KM.A2006137.0310.005.2010177125010.hdf";
   	//char hdfFile[256] = "c:\\temp\\Data\\MODIS\\2013-1-11\\MOD04_L2.A2013011.0225.051.2013011173120.hdf";
	//char hdfFile[256] = "c:\\temp\\Data\\MODIS\\2013-7-23\\MOD04_L2.A2013204.0310.051.2013204152815.hdf";

	char** subDataDesc = f2c(1000, 256);

	//read subdata information
	int nSubData = ReadSubDataInfo(hdfFile, subDataDesc);

	FILE* fp = fopen("c:\\temp\\subDataDesc.txt", "w");
	for(int i=0; i<nSubData; i++)
	{
		printf("%s \n", subDataDesc[i]);
		fprintf(fp, "%s \n", subDataDesc[i]);
	}
	fclose(fp);

	/*
	double* pBuffer = NULL;
	int ht,wd;

	//read AOT 
	ReadSubData(subDataDesc[7], &pBuffer, &ht, &wd);

	fp = fopen("c:\\temp\\aot.txt", "w");
	for(int j=0; j<ht; j++)
	{
		for(int i=0; i<wd; i++)
		{
			fprintf(fp, "%lf ", pBuffer[j*wd+i]);
		}
		fprintf(fp, "\n");
	}
	fclose(fp);

	//read fmf
	double* pFMF = NULL;
	ReadSubData(subDataDesc[9], &pFMF, &ht, &wd);
	fp = fopen("c:\\temp\\fmf.txt", "w");
	for(int j=0; j<ht; j++)
	{
		for(int i=0; i<wd; i++)
		{
			fprintf(fp, "%lf ", pFMF[j*wd+i]);
		}
		fprintf(fp, "\n");
	}
	fclose(fp);

	//read lon/lat
	double* pLon = NULL;
	ReadSubData(subDataDesc[69], &pLon, &ht, &wd);
	fp = fopen("c:\\temp\\lon.txt", "w");
	for(int j=0; j<ht; j++)
	{
		for(int i=0; i<wd; i++)
		{
			fprintf(fp, "%lf ", pLon[j*wd+i]);
		}
		fprintf(fp, "\n");
	}
	fclose(fp);


	double* pLat = NULL;
	ReadSubData(subDataDesc[70], &pLat, &ht, &wd);
	fp = fopen("c:\\temp\\lat.txt", "w");
	for(int j=0; j<ht; j++)
	{
		for(int i=0; i<wd; i++)
		{
			fprintf(fp, "%lf ", pLat[j*wd+i]);
		}
		fprintf(fp, "\n");
	}
	fclose(fp);

	//save the result based on the selected area
	double minlat = 32;
	double maxlat = 46;
	double minlon = 102;
	double maxlon = 125;
    
	fp = fopen("c:\\temp\\modiAotScatter.txt", "w");
	for(int j=0; j<ht; j++)
	{
		for(int i=0; i<wd; i++)
		{
			if(  pLat[j*wd+i]>=minlat && pLat[j*wd+i]<=maxlat 
				 && pLon[j*wd+i]>=minlon && pLon[j*wd+i]<=maxlon  )
			{
				double aod = pBuffer[j*wd+i];
				if(pBuffer[j*wd+i]>=0)
					aod = pBuffer[j*wd+i]*0.001;	
				
				fprintf(fp, "%lf %lf %lf \n", pLon[j*wd+i], pLat[j*wd+i], aod);
			}
		}
	}
	fclose(fp);

	//save as gridded lon/lat 
	double xstep = 0.15;
	double ystep = 0.15;
	int sht = (maxlat-minlat)/ystep;
	int swd = (maxlon-minlon)/xstep;
	
	double* pAOT = (double*)malloc(sht*swd*sizeof(double));
	for(int j=0; j<sht; j++)
	{
		for(int i=0; i<swd; i++)
		{
			pAOT[j*swd+i] = -1;
		}
	}
	
	for(int j=0; j<ht; j++)
	{
		for(int i=0; i<wd; i++)
		{
			//i = 131;
			//j = 150;

			double aod = pBuffer[j*wd+i];
			if(pBuffer[j*wd+i]>=0)
				aod = pBuffer[j*wd+i]*0.001;	
			else
				aod = -1;

			int row = (pLat[j*wd+i]-minlat)/ystep;
			int col = (pLon[j*wd+i]-minlon)/xstep;

			if( row>0 && row<(sht-1) && col>0 && col<(swd-1) )
			{	
				for(int m=-1; m<=1; m++)
					for(int n=-1; n<=1; n++)
						pAOT[(row+m)*swd+(col+n)] = aod;
				//printf("%lf ", aod);
			}
		}
	}
	
	fp = fopen("c:\\temp\\modis-aot.txt", "w");
	for(int j=sht-1; j>=0; j--)
	{
		for(int i=0; i<swd; i++)
		{
			fprintf(fp, "%lf ", pAOT[j*swd+i]);
		}
		fprintf(fp, "\n");
	}
	fclose(fp);


	double* pSmallRatio = (double*)malloc(sht*swd*sizeof(double));
	for(int j=0; j<sht; j++)
	{
		for(int i=0; i<swd; i++)
		{
			pSmallRatio[j*swd+i] = -1;
		}
	}

	for(int j=0; j<ht; j++)
	{
		for(int i=0; i<wd; i++)
		{
			double ratio = pFMF[j*wd+i];
			if(pFMF[j*wd+i]>=0)
				ratio = pFMF[j*wd+i]*0.001;	
			else
				ratio = -1;

			int row = (pLat[j*wd+i]-minlat)/ystep;
			int col = (pLon[j*wd+i]-minlon)/xstep;

			if( row>0 && row<(sht-1) && col>0 && col<(swd-1) )
			{	
				for(int m=-1; m<=1; m++)
					for(int n=-1; n<=1; n++)
						pSmallRatio[(row+m)*swd+(col+n)] = ratio;
				//printf("%lf ", aod);
			}
		}
	}

	fp = fopen("c:\\temp\\modis-fmf.txt", "w");
	for(int j=sht-1; j>=0; j--)
	{
		for(int i=0; i<swd; i++)
		{
			fprintf(fp, "%lf ", pSmallRatio[j*swd+i]);
		}
		fprintf(fp, "\n");
	}
	fclose(fp);
	*/

}

void CProject_PGStationDlg::OnFillScanfill()
{
	// TODO: Add your command handler code here

	//fill the triangle 
	int ht = 100;
	int wd = 100;
	unsigned short* pMask = (unsigned short*)malloc( ht*wd*sizeof(unsigned short) );
	memset(pMask, 0, ht*wd*sizeof(unsigned short));
	
	int px[3] = {10, 100, 50};
	int py[3] = {10, 15, 100};

	GetInnerRegionMask(pMask, wd, ht, px, py, 3, 256, 220, 1);

	FILE* fp = fopen("c:\\temp\\mask.txt", "w");
	for(int j=0; j<ht; j++)
		for(int i=0; i<wd; i++)
		{

		}
	fclose(fp);

	SaveBmp("c:\\temp\\fillmask.bmp", pMask, ht, wd);

	printf("Finished! \n");
}

void CProject_PGStationDlg::OnAtmospherecorrectionHj()
{
	// TODO: Add your command handler code here

	char path[256] = "c:\\temp\\Data\\Lut\\2010_08_16\\660";
	char lut0[256];
	char lut1[256];
	char lut2[256];
	sprintf(lut0, "%s\\%d.dat", path, 0);
	sprintf(lut1, "%s\\%d.dat", path, 10);
	sprintf(lut2, "%s\\%d.dat", path, 25);

	//loading luts
	CAtmosphereCorrectionBase* pAC = new CAtmosphereCorrectionRt3();
    pAC->LoadLut(lut0, lut1, lut2, 0.66);

	double aod = 0.1415;
	double xa,xb,xc;
	//double sza = 30;
	//double vza = 1;
	//double azi = 180;
	
	/*
	for(int i=0; i<80; i+=5)
	{
		pAC->CalcCorrectParams(sza, vza, azi, aod, xa, xb, xc);
	}
	//printf("%lf %lf %lf \n", xa, xb, xc);
	*/
	
	//load HJ data
	char* tifFile = "c:\\temp\\Data\\HJ\\Beijing\\HJ1A-CCD2-1-68-20100816-L20000375929\\375929\\HJ1A-CCD2-1-68-20100816-L20000375929-3.TIF";
    stGeoInfo geoInfo;
	GetGeoInformation(tifFile, geoInfo);

	//FILE* fp = fopen("c:\\temp\\projection_string.txt", "w");
	//fprintf(fp, "%s\n", geoInfo.projectRef);
	//fclose(fp);

	char* subFile = "c:\\temp\\Data\\HJ\\Beijing\\HJ1A-CCD2-1-68-20100816-L20000375929\\375929\\2010-8-16-band3-crop.tif";
	stGeoInfo geoInfoSub;
	GetGeoInformation(subFile, geoInfoSub);

	int ht,wd;
	unsigned char *pBuffer = NULL;
	ReadGeoFileByte(subFile, 1, &pBuffer, ht, wd);
	SaveBmp("c:\\temp\\bj.bmp", pBuffer, ht, wd);

	//load HJ XML data
	char* xmlFile = "c:\\temp\\Data\\HJ\\Beijing\\HJ1A-CCD2-1-68-20100816-L20000375929\\375929\\HJ1A-CCD2-1-68-20100816-L20000375929.XML";
	HJDATA hjdata;
	GetHJFilepaths(xmlFile, hjdata);
	ReadXMLFile(hjdata);

	//load angle data
	char* angleFile = "c:\\temp\\Data\\HJ\\Beijing\\HJ1A-CCD2-1-68-20100816-L20000375929\\375929\\HJ1A-CCD2-1-68-20100816-L20000375929-SatAngle.txt";
    CReadAngleBase* pReadAngle = new CReadHJAngle();
	pReadAngle->Read(angleFile);


	//atmosphere correction
	//double minlon = 116;
	//double maxlon = 117;
	//double minlat = 39.5;
	//double maxlat = 40.3;

	double sza = hjdata.solar_zenith;
	double sazimuth = hjdata.solar_azimuth;

	unsigned char* pACBuffer = (unsigned char*)malloc(ht*wd);
	for(int j=0; j<ht; j++)
	//int j=2000;
	{
		for( int i=0; i<wd; i++)
		//int i=2000;
		{
			double gx = geoInfoSub.left + i*geoInfoSub.dx;
			double gy = geoInfoSub.top  + j*geoInfoSub.dy;
			int row =  (geoInfo.top - gy) / fabs(geoInfoSub.dy);
			int col =  (gx - geoInfo.left) / fabs(geoInfoSub.dx);
			double vza, vazimuth;
			pReadAngle->GetViewGeo(row, col, vza, vazimuth);
			if(vazimuth<0) vazimuth += 360;
			double relativeAzimuth = fabs(vazimuth-sazimuth);
			if(relativeAzimuth>180) relativeAzimuth -= 180;
		
			//convert DN to radiance
			pAC->CalcCorrectParams(sza, vza, relativeAzimuth, aod, xa,xb,xc);
			int rawValue = pBuffer[j*wd+i];
			int radiance = rawValue / hjdata.m_gain[2] + hjdata.m_offset[2];
            
			//correction using radiance
			double ac = 0;
			if(rawValue!=0)
			{
				double y = xa*radiance-xb;
				ac = y/(1.0+xc*y);					
			}
			ac = max( 0, min(1,ac) );

			//convert the reflectance to radiance
			double es = pAC->GetEs();
			ac = cos(sza/180.0*PI)*es*ac;
			ac=max(0, min(255,ac));
            			
			pACBuffer[j*wd+i] = ac;			
		}
		printf("%d \n", j);
	}

	//save the result into the tif file
	GdalWriteImageByte("c:\\temp\\ac.tif", pACBuffer, ht, wd);
	free(pACBuffer);

	printf("Finished!\n");
}

void CProject_PGStationDlg::OnEpipolargeometryFundamentalmatrix()
{
	// TODO: Add your command handler code here

	//char* leftImage  = "c:\\temp\\Data\\Registration\\kermit\\kermit000.jpg";
	//char* rightImage = "c:\\temp\\Data\\Registration\\kermit\\kermit001.jpg";
	//char* leftImage  = "c:\\temp\\Data\\Registration\\kermit\\kermit000.jpg";
	//char* rightImage = "c:\\temp\\Data\\Registration\\kermit\\kermit001.jpg";

	if( m_imagePointerVector.size()>0)
	{
	
	//char* leftImage  = "F:\\Data\\Registration\\IMG_0107.jpg";
	//char* rightImage = "F:\\Data\\Registration\\IMG_0108.jpg";

	IplImage* pLeft = cvCloneImage( m_imagePointerVector[0]->GetCvArrColor() ); //cvLoadImage(leftImage);
	IplImage* pRight = cvCloneImage( m_imagePointerVector[1]->GetCvArrColor() );//cvLoadImage(rightImage);

	char leftImage[256];
	char rightImage[256];
	strcpy(leftImage, m_imagePointerVector[0]->GetFilePath());
	strcpy(rightImage, m_imagePointerVector[1]->GetFilePath());

	//1. detect feature points 
	CFeatureBase* pFeatDetect = new CSIFTFloat();
	ImgFeature lImageFeature, rImageFeature;
	pFeatDetect->Detect(leftImage, lImageFeature);
	pFeatDetect->Detect(rightImage, rImageFeature);

	//2. matching 
	CMatchBase* pMatch = new CSiftMatch();
	PairMatchRes mp;
	pMatch->Match(lImageFeature, rImageFeature, mp);
	int nMatch = mp.matchs.size();
	printf("Matching Point Number: %d \n", nMatch);


	//3. calculate fundamental matrix
	vector<Point2DDouble> lPts;
	vector<Point2DDouble> rPts;
	for(int i=0; i<mp.matchs.size(); i++)
	{
		int li = mp.matchs[i].l;
		int ri = mp.matchs[i].r;
		
		Point2DDouble pl;
		pl.p[0] = lImageFeature.featPts[li].x;
		pl.p[1] = lImageFeature.featPts[li].y;
		lPts.push_back(pl);
		
		Point2DDouble pr;
		pr.p[0] = rImageFeature.featPts[ri].x;
		pr.p[1] = rImageFeature.featPts[ri].y;
		rPts.push_back(pr);
	}

	delete m_pEstimateFM;
	m_pEstimateFM = new COpenCVFM();
	vector<double> fm;
	m_pEstimateFM->Estimate(lPts, rPts, fm);
    

	//display 
	srand(100);
	for(int i=0; i<mp.matchs.size(); i++)
	{
		int li = mp.matchs[i].l;
		int ri = mp.matchs[i].r;
		CvPoint pl;
		pl.x = lImageFeature.featPts[li].x;
		pl.y = lImageFeature.featPts[li].y;
		CvPoint pr;
		pr.x = rImageFeature.featPts[ri].x;
		pr.y = rImageFeature.featPts[ri].y;
		int r = (double)(rand())/(double)(RAND_MAX)*255;
		int g = (double)(rand())/(double)(RAND_MAX)*255;
		int b = (double)(rand())/(double)(RAND_MAX)*255;
		cvDrawCircle(pLeft, pl, 2, CV_RGB(r,g,b),2);
		cvDrawCircle(pRight, pr, 2, CV_RGB(r,g,b),2);
	}

	cvSaveImage("c:\\temp\\left.jpg", pLeft);
	cvSaveImage("c:\\temp\\right.jpg", pRight);

	cvNamedWindow("Left");
	cvShowImage("Left", pLeft);
	cvNamedWindow("Right");
	cvShowImage("Right", pRight);

	cvReleaseImage(&m_pLeftImage);
	cvReleaseImage(&m_pRightImage);
	m_pLeftImage = cvCloneImage(pLeft);
	m_pRightImage = cvCloneImage(pRight);

	//
	cvSetMouseCallback("Left", on_left_mouse, this );
	cvSetMouseCallback("Right", on_right_mouse, this );

	cvReleaseImage(&pLeft);
	cvReleaseImage(&pRight);
	}
}

void CProject_PGStationDlg::OnNormalizeColor()
{
	// TODO: Add your command handler code here

	//char* filepath = "c:\\temp\\GF1_WFV4_E115.9_N33.5_20131026_L1A0000105274-resize.tif";
	//char* filepath = "c:\\temp\\GF1_WFV3_E113.8_N33.9_20131026_L1A0000105260-resize.tif";
	
	CFileDialog dlg(true,NULL,NULL,
		OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_ALLOWMULTISELECT|OFN_EXPLORER,
		_T("jpg file (*.tif)|*.tif|bitmap file(*.bmp)|*.bmp||"));

	if(dlg.DoModal() == IDOK)
	{
		CString cfile = dlg.GetPathName();	
		char filepath[256];
		strcpy(filepath, cfile);

		stGeoInfo geoInfo;
		GetGeoInformation(filepath, geoInfo);

		unsigned char* pbuffer = NULL;
		int ht,wd;

		unsigned char* r = NULL;
		unsigned char* g = NULL;
		unsigned char* b = NULL;

		ReadGeoFileByte(filepath, 1, &r, ht, wd);
		ImageNorm(r, ht, wd, 127, 60);	
		//GdalWriteJpgCopy("c:\\temp\\r.jpg", pbuffer, ht, wd);
		//free(pbuffer);

		ReadGeoFileByte(filepath, 2, &g, ht, wd);
		ImageNorm(g, ht, wd, 130, 60);	
		//GdalWriteJpgCopy("c:\\temp\\g.jpg", pbuffer, ht, wd);
		//free(pbuffer);

		ReadGeoFileByte(filepath, 3, &b, ht, wd);
		ImageNorm(b, ht, wd, 124, 60);	
		//GdalWriteJpgCopy("c:\\temp\\b.jpg", pbuffer, ht, wd);
		//free(pbuffer);

		GdalWriteImageColor("c:\\temp\\color1.tif", r, g, b, ht, wd, geoInfo);

		free(r);
		free(g);
		free(b);
		printf("Normalization Finished ! \n");
	}
}

void CProject_PGStationDlg::OnAtmospherecorrectionParasol()
{
	// TODO: Add your command handler code here

	//input
	char* lutpath  = "c:\\temp\\Data\\Aeronet\\2005-10-08\\beijing\\lut\\670";
	char* filepath = "c:\\temp\\Data\\Parasol\\Beijing\\2005_10_08_05_116.381_ 39.977.txt";
	double wavelen = 0.670;    //0.865
	double aod     = 0.07;     //0.054567;

	//read lut
	char clut0[256];
	char clut1[256];
	char clut2[256];
	sprintf(clut0, "%s\\0.dat",  lutpath);
	sprintf(clut1, "%s\\10.dat", lutpath);
	sprintf(clut2, "%s\\25.dat", lutpath);
	CAtmosphereCorrectionBase* pAC = new CAtmosphereCorrectionRt3();
	pAC->LoadLut(clut0, clut1, clut2, wavelen);
	
	//load multitple-angle data
	stMultiView multData;
  	LoadParasolMultiDataSingle(filepath, multData);

	//atmospheric correction
	printf("Atmospheric Correction using object ...... \n");	
	
	char acFile[512];
	sprintf(acFile, "%s-%5.3lf.ac",filepath, wavelen);
	FILE* fp = fopen(acFile, "w");
	double sza, vza, azi,rawazi;
	for(int i=0; i<multData.nView; i++)
	{
		sza = multData.mViewData[i].sza;
		vza = multData.mViewData[i].vza;
		azi = multData.mViewData[i].azi;
        rawazi = multData.mViewData[i].rawAzi;
		double sca = multData.mViewData[i].sca;
		double rp ;
        if(wavelen==0.865)
		{
			rp  = multData.mViewData[i].rp8;
		}
		else if(wavelen==0.67)
		{
			rp  = multData.mViewData[i].rp6;
		}
		else
		{
			rp  = multData.mViewData[i].rp4;
		}
		
		double ac = pAC->Correct(sza, vza, azi, aod, rp, 1);
		printf("%lf  %lf \n", sca, ac);
		fprintf(fp, "%lf %lf %lf %lf  %lf \n", sza, vza, azi, sca, ac);
	}
	fclose(fp);
}



void CProject_PGStationDlg::OnConvertSds()
{
	// TODO: Add your command handler code here

	char* hdfFile = "c:\\temp\\mod04.hdf";
	char* outfile = "c:\\temp\\out.tif";

	
	vector<string> subDataSets;
	vector<string> subDataDesc;
	//subDataSets.resize(10);
	//subDataSets[0] = "Hello";
	//subDataSets.push_back("Hello!");
	
	ReadSubDataInfo(hdfFile, subDataSets, subDataDesc);

	for(int i=0; i<subDataSets.size(); i++)
	{
		//printf("%s \n", subDataSets[i]);
		//printf("%s \n", subDataDesc[i]);
		cout<<subDataSets[i]<<endl;
		cout<<subDataDesc[i]<<endl;
	}
}

void CProject_PGStationDlg::OnOrthoimageUav()
{
	// TODO: Add your command handler code here

	//generate orthoimage using POS data from UAV, 2013.12.7,xiedonghai


	//intrisic paramater	
	//Dalian 7.20 afternoom IMG_0078 
	//78    星期2 15:34:07   121.9135151   39.0225681    381.103       .6            2.5           43.9 
    
	//
	double lon,lat;
	double flyHeight;
	double rx,ry,rz;
	double focus;
    lon = 121.9135151;
	lat = 39.0225681;
    flyHeight = 381.103;
	rx = 0.6;
	ry = 2.5;
	rz = 43.9;
	focus = 24 / 6.41 *1000; //from mm to pixel
    
	//from (lon,lat) to ground coordinate 
	double xs,ys,zs;
	//LLToGauss(lon, lat, &xs, &ys, WGS84);
	//printf("%lf %lf \n", xs, ys);
	int nZoneNumber = int((lon + 180)/6) + 1;
	LLtoUTM(23, lat, lon, ys, xs, nZoneNumber);
	printf("%lf %lf \n", xs, ys);

	zs = flyHeight;

	CameraPara camInfo;
	camInfo.focus = focus;
	camInfo.u0 = 0;
	camInfo.v0 = 0;
    camInfo.ax = rx;
	camInfo.ay = ry;
	camInfo.az = rz;
	camInfo.xs = xs;
	camInfo.ys = ys;
	camInfo.zs = zs;
	camInfo.lon = lon;
	camInfo.lat = lat;

	COrthoBase* pOrtho = new COrthoUAV();
	char* inputFile = "c:\\temp\\Data\\UAV\\dalian\\0720-afternoon\\100EOS5D\\IMG_0078.jpg";
	char* outFile = "c:\\temp\\ortho-78.tif";
	pOrtho->GenerateOrthoImage(camInfo, inputFile, outFile);



	printf("Finished! \n");
}


void CProject_PGStationDlg::OnSinusoidalGrid2ll()
{
	// TODO: Add your command handler code here

	int line = 901;
	int col  = 4846;

	double lat,lon;
	LinecolToLatlonHigh(line, col, &lon, &lat);

	printf("lon:%lf  lat:%lf  \n", lon, lat);
}


void CProject_PGStationDlg::OnStringVector()
{
	// TODO: Add your command handler code here

	CString s = "hello!";

	//vector<string> vs;
	//vs.push_back(s);

	cout<<s<<endl;
}

void CProject_PGStationDlg::OnBundlerReadtracks()
{
	// TODO: Add your command handler code here

	char* tracksFile = "c:\\temp\\tracks.txt";

	vector<TrackInfo> tracks;
	ReadTracks(tracksFile, tracks);

	vector<TrackInfo> trackSeq;
	GetMatch(0, 1, tracks, trackSeq);

	//
	int nProj = FindProjections(2, tracks, trackSeq);
	nProj = FindProjections(3, tracks, trackSeq);
}

void CProject_PGStationDlg::OnMosaicOpticalflow()
{
	// TODO: Add your command handler code here
	char* path = "c:\\temp\\Data\\seavideo\\image6";


	char** filenames=NULL;
	int n=0,nfile=0;
	GetDirFileName(filenames, path, &n, &nfile, "JPG", 0);
	filenames = f2c(nfile, 256);
	GetDirFileName(filenames, path, &n, &nfile, "JPG", 1);

	vector<CString> fileVec;
	for(int i=0; i<nfile; i++)
	{
		//string sfile = filenames[i];
		CString cfile = filenames[i];
		fileVec.push_back(cfile);
	}
	sort(fileVec.begin(), fileVec.end());
	for(int i=0; i<fileVec.size(); i++)
	{
		strcpy(filenames[i], fileVec[i]);
		printf("%s \n", filenames[i]);		
	}

	DirectMosaic(filenames, nfile, "c:\\temp\\sea.jpg");

	printf("Finished! \n");
}

void CProject_PGStationDlg::OnParasolReadlevel2()
{
	// TODO: Add your command handler code here
	//char parasolL2File[256] = "c:\\temp\\Data\\Parasol\\2013\\10\\5\\P3L2TLGC202181KL";
	//char parasolL2File[256] = "c:\\temp\\Data\\Parasol\\2013\\1\\11\\P3L2TLGC186012KL";
	//char parasolL2File[256] = "c:\\temp\\Data\\Parasol\\2013\\7\\23\\P3L2TLGC198033KL";
	//char parasolL2File[256] = "c:\\temp\\Data\\Parasol\\2013\\10\\9\\P3L2TLGC203006KL";
	//char parasolL2File[256] = "c:\\temp\\Data\\Parasol\\2013\\9\\17\\P3L2TLGC201151KL";
	//char parasolL2File[256] = "c:\\temp\\Data\\Parasol\\2013\\10\\4\\l2\\P3L2TLGC202166KL";


	char parasolL2File[256] = "F:\\Data\\PARASOL\\L2\\2005-10-08\\P3L2TLGA020047JL";
    
	
	CReasParasolLevel2File readParasolLevelTwo;

	readParasolLevelTwo.Load(parasolL2File);

	stLSAP data;
    double lon = 116.3;
	double lat = 39.9;
	data = readParasolLevelTwo.GetPtMultiViewData(lon, lat);

	//printf("aoc:\\temp %lf \n", data.aod865);
	//double minlon=102, maxlon=125, minlat=32, maxlat=46;
	double minlon=110, maxlon=130, minlat=20, maxlat=55;
    
	double step = 1.0/6.0;

	int ht = (maxlat-minlat) / step;
	int wd = (maxlon-minlon) / step;
    
	float* pAerosol = (float*)malloc(ht*wd*sizeof(float));
	memset(pAerosol, 0, ht*wd*sizeof(float));


	FILE* fp = fopen("c:\\temp\\parasolAot20130917.txt", "w");
	int index = 0;
	for(int j=ht-1; j>=0; j--)
	{
		for(int i=0; i<wd; i++)
		{
			lon = minlon + i*step;
			lat = minlat + j*step;
			data = readParasolLevelTwo.GetPtMultiViewData(lon, lat);
			
			//printf("%5.3lf ", data.aod865);
			fprintf(fp, "%5.3lf ", data.aod865);
			
			if(data.aod865>0)
				pAerosol[index] = data.aod865;
			
			index ++;
		}
		fprintf(fp, " \n");
	}	
	fclose(fp);


	printf("writing geotiff ... \n");
	//GdalWriteFloatLL(aodTiffFile, pAOD, ht, wd, minlon, maxlon, minlat, maxlat);
	GdalWriteFloatLL("c:\\temp\\parasol_fineaod.tif", pAerosol, ht, wd, minlon, maxlon, minlat, maxlat);

	free(pAerosol);
	printf("Reading Finished ! \n");
}



void CProject_PGStationDlg::OnGf1Invert()
{
	// TODO: Add your command handler code here

	char lutFile[256] = "c:\\temp\\Data\\LUTDouble\\type\\1\\4.dat";
	CLutBase* pLut = new CLutBinCont();
	pLut->Load(lutFile);

	char path[256] = "c:\\temp\\Data\\GF\\L2\\GF1_PMS2_E116.3_N39.9_20130627_L1A0000068947\\GF1_PMS2_E116.3_N39.9_20130627_L1A0000068947-MSS2";
	char band1_file[256];
	char band3_file[256];
	char band4_file[256];
	sprintf(band1_file, "%s-%d.tif", path, 1);
	sprintf(band3_file, "%s-%d.tif", path, 3);
	sprintf(band4_file, "%s-%d.tif", path, 4);
    
	PrintImageInfo(band1_file);

	//absolute calibration
	double gain[4] = {0.2072, 0.1776, 0.177, 0.1909};
	double off[4]  = {7.5348, 3.9395, -1.7445, -7.2053};


	int ht,wd;
	unsigned short* pBand1 = NULL;
	ReadGeoFileUShort(band1_file, 1, &pBand1, ht, wd);

	unsigned short* pBand3 = NULL;
	ReadGeoFileUShort(band3_file, 1, &pBand3, ht, wd);

	unsigned short* pBand4 = NULL;
	ReadGeoFileUShort(band4_file, 1, &pBand4, ht, wd);

	double sza = 20;
	double vza = 16.5;
	double azi = 140;

	double flux1 = GetFlux(0.49)*cos(sza*DPI);
	double flux3 = GetFlux(0.66)*cos(sza*DPI);
	double flux4 = GetFlux(0.86)*cos(sza*DPI);

	FILE* fp = fopen("c:\\temp\\gf-aot.txt", "w");
	for(int j=0; j<ht; j+=3)
	{
		for(int i=0; i<wd; i+=3)
		{
			double blue = pBand1[j*wd+i]; //*gain[0] + off[0];
			double red  = pBand3[j*wd+i]; //*gain[2] + off[2];
			double ir   = pBand4[j*wd+i]; //*gain[3] + off[3];

			double aod  = 0;
			if(blue==0 || red==0 || ir==0)
			{
				aod = -1;
			}
			else
			{
				blue = PI*(blue*gain[0] + off[0]) / flux1;
				red  = PI*(red*gain[2] + off[2]) / flux3; 
				ir   = PI*(ir*gain[3] + off[3]) / flux4;

				if(blue<0 || red<0 || ir<0)
				{
					aod = -1;
				}
				else
				{
					double ndvi = (ir-red) / (ir+red);
					double surf;
					if(ndvi<0)
						surf = 0;
					else
					    surf = 0.04*ndvi + 0.08*(1-ndvi);	            

					AerosolInvertSinglePt(pLut, 0.490, 700000, 1.25, blue*0.6, sza, vza, azi, surf, aod);
				}
			}

			fprintf(fp, "%5.3lf ", aod);
		}
		printf("%d \n", j);
		fprintf(fp, "\n");
	}
	fclose(fp);

}

void CProject_PGStationDlg::OnImagecutMod02()
{
	// TODO: Add your command handler code here

	char mod02File[256] = "F:\\Data\\Modis\\MOD02HKM.A2013278.0345.005.2013278134018.hdf";

	char** subDataDesc = f2c(1000, 256);

	//read subdata information
	int nSubData = ReadSubDataInfo(mod02File, subDataDesc);
	FILE* fp = fopen("c:\\temp\\subDataDesc.txt", "w");
	for(int i=0; i<nSubData; i++)
	{
		printf("%s \n", subDataDesc[i]);
		fprintf(fp, "%s \n", subDataDesc[i]);
	}
	fclose(fp);

	//read lon and lat
	double* pLon = NULL;
	double* pLat = NULL;
	int lHt,lWd;
    ReadSubData(subDataDesc[19], &pLat, &lHt, &lWd, 1);
	ReadSubData(subDataDesc[20], &pLon, &lHt, &lWd, 1);
   
	double* pBuffer = NULL;
	int ht,wd;
	//read 250m data
	//band 1
	ReadSubData(subDataDesc[7], &pBuffer, &ht, &wd, 5);
	
	//resample
    double minlon = 180;
	double maxlon = -180;
	double minlat = 90;
	double maxlat = -90;
	for(int j=0; j<lHt; j++)
		for(int i=0; i<lWd; i++)
		{
			minlon = min(minlon, pLon[j*lWd+i]);
			maxlon = max(maxlon, pLon[j*lWd+i]);
			minlat = min(minlat, pLat[j*lWd+i]);
			maxlat = max(maxlat, pLat[j*lWd+i]);
		}
	printf("lon: %lf %lf  lat: %lf %lf \n", minlon, maxlon, minlat, maxlat);
	
	double lonStep = (maxlon-minlon) / double(wd);
	double latStep = (maxlat-minlat) / double(ht);
	printf("%lf %lf \n", lonStep, latStep);
	double dstep = max(lonStep, latStep);
	int    sht = (maxlat-minlat)/dstep;
	int    swd = (maxlon-minlon)/dstep;
    
	//resample modis image 
	unsigned short* pImage = (unsigned short*)malloc(sht*swd*sizeof(unsigned short));
	memset(pImage, 0, sht*swd*sizeof(unsigned short));
	for(int j=0; j<ht; j++)
		for(int i=0; i<wd; i++)
		{
			//binear interpolation
			double tx = double(i) / 5.0;
			double ty = double(j) / 5.0;
			double lon = BilinearImagePixel(tx, ty, pLon, lHt, lWd);
			double lat = BilinearImagePixel(tx, ty, pLat, lHt, lWd);
			
			int col = (lon-minlon)/dstep;
			int row = (maxlat-lat)/dstep;
			row = min(sht-1, row);
			col = min(swd-1, col);

			pImage[row*swd+col] = pBuffer[ (int)( j*wd+i ) ];
		}
	GdalWriteImageUShort("c:\\temp\\lonlat.tif", pImage, sht, swd);

	//generate the convex polygon of the sampled image
	double px[100];
	double py[100];
	int index = 0;
	int i,j;
	j=0;
	for(int i=0; i<=10; i++)
	{
		//bilinear interpolation
		double tx  = double(i*0.1*wd) / 5.0;
		double ty  = double(j) / 5.0;
		double lon = BilinearImagePixel(tx, ty, pLon, lHt, lWd);
		double lat = BilinearImagePixel(tx, ty, pLat, lHt, lWd);
		int col = (lon-minlon)/dstep;
		int row = (maxlat-lat)/dstep;
		row = min(sht-1, row);
		col = min(swd-1, col);
		px[index] = col;
		py[index] = row;
		index++;
	}
	i=wd-1;
	for(j=1; j<=10; j++)
	{
		//binear interpolation
		double ty = double(j*0.1*ht) / 5.0;
		double tx = double(i) / 5.0;
		double lon = BilinearImagePixel(tx, ty, pLon, lHt, lWd);
		double lat = BilinearImagePixel(tx, ty, pLat, lHt, lWd);
		int col = (lon-minlon)/dstep;
		int row = (maxlat-lat)/dstep;
		row = min(sht-1, row);
		col = min(swd-1, col);
		px[index] = col;
		py[index] = row;
		index++;
	}
	j=ht-1;
	for( i=9; i>=0; i--)
	{
		//binear interpolation
		double tx = double(i*0.1*wd) / 5.0;
		double ty = double(j) / 5.0;
		double lon = BilinearImagePixel(tx, ty, pLon, lHt, lWd);
		double lat = BilinearImagePixel(tx, ty, pLat, lHt, lWd);
		int col = (lon-minlon)/dstep;
		int row = (maxlat-lat)/dstep;
		row = min(sht-1, row);
		col = min(swd-1, col);
		px[index] = col;
		py[index] = row;
		index++;
	}
	i=0;
	for( j=9; j>=0; j--)
	{
		//binear interpolation
		double ty = double(j*0.1*ht) / 5.0;
		double tx = double(i) / 5.0;
		double lon = BilinearImagePixel(tx, ty, pLon, lHt, lWd);
		double lat = BilinearImagePixel(tx, ty, pLat, lHt, lWd);
		int col = (lon-minlon)/dstep;
		int row = (maxlat-lat)/dstep;
		row = min(sht-1, row);
		col = min(swd-1, col);
		px[index] = col;
		py[index] = row;
		index++;
	}
	int nPolyPtNum = index;

	IplImage* pDisp = cvCreateImage(cvSize(swd, sht), 8, 3);
	for(int i=0; i<index-1; i++)
	{
		CvPoint pt1,pt2;
		pt1.x = px[i];
		pt1.y = py[i];
		pt2.x = px[i+1];
		pt2.y = py[i+1];
		cvDrawCircle(pDisp, pt1, 2, CV_RGB(0,255,0), 3);
		cvLine(pDisp, pt1, pt2, CV_RGB(255,0,0), 2);		
	}
	cvSaveImage("c:\\temp\\convex.jpg", pDisp);
    cvReleaseImage(&pDisp);

	/*
	BYTE* mask = (BYTE*)malloc( sizeof(BYTE)*ht*wd );
	memset(mask, 0, sizeof(BYTE)*ht*wd);
	GetInnerRegionMask(mask, wd, ht, px, py, index, 100, 255, true);
	SaveBmp("c:\\temp\\mask.bmp", mask, ht, wd);
	*/

	//interpolation
	unsigned short* pIpImage = (unsigned short*)malloc( sht*swd*sizeof(unsigned short) );
	memset(pIpImage, 0, sht*swd*sizeof(unsigned short));
	
	for(int j=0; j<sht; j++)
	{
		printf("%d \n", j);
		for(int i=0; i<swd; i++)
		{
			int value = pImage[j*swd+i];
			if(value>0)
			{
				pIpImage[j*swd+i] = value;
			}
			else
			{
				int nInSide = pnpoly(nPolyPtNum, px, py, i, j);
				if(nInSide>0)
				{
					bool bIsFindPixel = false;
					int  size = 2;
					while(!bIsFindPixel)
					{
						int l = i-size;
						int r = i+size;
						int t = j-size;
						int b = j+size;
						l = max(0, l);
						r = min(wd-1, r);
						t = max(0, t);
						b = min(ht-1, b);
						vector<double> vv;
						vector<double> vw;
						vv.clear();
						vw.clear();
						for(int m=t; m<=b; m++)
							for(int n=l; n<=r; n++)
							{
								int dn = pImage[ (m)*swd+(n) ];
								if( dn>0 )
								{
									vv.push_back( dn );
									double radius = sqrt( (double)( (m-j)*(m-j)+(n-i)*(n-i))) + 1; 
									double w = 1.0/radius;
									vw.push_back(w);
								}
							}
						if(vv.size()>0)
						{
							//interpolation
							double sdn = 0;
							double sw = 0;
							for(int k=0; k<vv.size(); k++)
							{
								sdn += vv[k]*vw[k];
								sw  += vw[k];
							}
							sdn = sdn/sw;
							
							pIpImage[j*swd+i] = sdn;

							bIsFindPixel = true;
						}
						else
						{
							size += 2;
						}
					}
				}
			}
		}
	}
	GdalWriteImageUShort("c:\\temp\\lonlat-ip.tif", pIpImage, sht, swd);

	fp = fopen("c:\\temp\\llarea.txt", "w");
	fprintf(fp, "%lf %lf %lf %lf \n", minlon, maxlon, minlat, maxlat);
	fprintf(fp, "%lf \n", dstep);
	fclose(fp);
	
	free(pIpImage);
	free(pImage);
	free(pBuffer);

	printf("Cut Finished ! \n");
}


#define BLOCK_IMAGE_SIZE  1000
void CProject_PGStationDlg::OnImagecutCut()
{
	// TODO: Add your command handler code here
	char filename[256] = "c:\\temp\\lonlat-ip.tif";

	//100.896812 130.198105 21.356579 42.545757 
	//0.021641 
	double minlon = 100.896812;
	double maxlon = 130.198105;
	double minlat = 21.356579;
	double maxlat = 42.545757;
    double dstep  = 0.021641;

	int level;
	double blocksize;
	double resolution = 1000; //meter
	ResolutionToLLBlockMapping(resolution, level, blocksize);
	printf("%d %lf \n", level, blocksize);
	
	int l  = int( (minlon+180)/blocksize );
	int r  = int( (maxlon+180)/blocksize );
	int t  = int( (minlat+90)/blocksize );
	int b = int( (maxlat+90)/blocksize );
	int oht = (b-t+1)*BLOCK_IMAGE_SIZE;   //(maxlat-minlat)/pixelsize;//采样后高度
	int owd = (r-l+1)*BLOCK_IMAGE_SIZE;   //(maxlon-minlon)/pixelsize;//采样后宽度
	printf("%d owc:\\temp %d \n", oht, owd);


	/*
	unsigned short* pBlockImage = (unsigned short*)malloc( BLOCK_IMAGE_SIZE*BLOCK_IMAGE_SIZE*sizeof(unsigned short) );
	double pixelStep = blocksize*0.001;
	for(int j=t; j<b; j++)
		for(int i=l; i<r; i++)
		{
			for(int m=0; m<BLOCK_IMAGE_SIZE; m++)
				for(int n=0; n<BLOCK_IMAGE_SIZE; n++)
				{
					double tlon = (double)(n)*pixelStep;
					double tlat = (double)(m)*pixelStep;
				}				
		}
	*/
}

void CProject_PGStationDlg::OnBaNewapi()
{
	// TODO: Add your command handler code here
	CFileDialog dlg(TRUE,NULL,NULL,
		OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_ALLOWMULTISELECT|OFN_EXPLORER,
		_T("jpg file (*.jpg)|*.jpg|bitmap file(*.bmp)|*.bmp|Tiff file(*.tif)|*.tif|IMG file(*.img)|*.img||"));

	TCHAR* szPathMulti  = new TCHAR[8*1024*1024];  //alloc memory for multiple files
	dlg.m_ofn.nMaxFile  = 8*1024*1024; 
	dlg.m_ofn.lpstrFile = szPathMulti; 
	dlg.m_ofn.lpstrFile[0] = 0; 

	//vector<string> fileVector;
	vector<CString> fileVector;
	if(dlg.DoModal()==IDOK)
	{
		POSITION  pos   =   dlg.GetStartPosition(); 
		int index = 0;		
		//read file paths
		while(pos != NULL) 
		{ 
			//insert image filepath into the file list control
			CString filepath = dlg.GetNextPathName(pos); 	
			//string s(filepath.GetBuffer());
			//fileVector.push_back(s);	
			fileVector.push_back(filepath);	
		}
		//sort the filenames
		sort( fileVector.begin(), fileVector.end() );
	}

	//1. feature detection
	//vector<IplImage*> pImages;
	vector<CImageDataBase*> vecImageDataPointer;
	for(int i=0; i<fileVector.size(); i++)
	{
		CImageDataBase* pImageData = new CImageFeature();
		char file[256];
		strcpy(file, fileVector[i]);
		pImageData->Load(file);
		pImageData->DetectPtFeature(SIFT_FEATURE);
		vecImageDataPointer.push_back(pImageData);
	}

	//2. matching
	vector<PairMatchRes> vecMatch;
	//CMatchBase* pMatch = new CKNNMatch();
	CMatchBase* pMatch = new CSiftMatch();
	for(int i=0; i<fileVector.size(); i++)
		for(int j=i+1; j<fileVector.size(); j++)
		{
			printf("Matching %d-%d \n", i,j);
			PairMatchRes mt;
			mt.lId = i;
			mt.rId = j;
			pMatch->Match( vecImageDataPointer[i]->GetImageFeature(), vecImageDataPointer[j]->GetImageFeature(), mt);

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
	PrintTracks(vecTrack, "c:\\temp\\tracks.txt");

	if(vecTrack.size()<8)
	{
		printf("Tracks are less than the threshold ! \n");
		return;
	}

	//display feature points
	/*
	CvFont font;	
	cvInitFont( &font, CV_FONT_VECTOR0, 0.5, 0.5, 0, 1, 8 );
	for(int i=0; i<vecTrack.size(); i++)
	{
		int nview = vecTrack[i].views.size();
		if(nview==3)
		{		
			for(int j=0; j<nview; j++)
			{
				ImageKey key = vecTrack[i].views[j];
				int imgId = key.first;
				int ptId = key.second;
				FeatPoint fpt = vecImageDataPointer[imgId]->GetKeyPoint(ptId);
				CvPoint pt;
				pt.x = fpt.x;//imgFeatures[imgId].featPts[ptId].x;
				pt.y = fpt.y;//imgFeatures[imgId].featPts[ptId].y;
				char text[256];
				sprintf(text, "%d", i);
                cvPutText(pImages[imgId], text, pt, &font, CV_RGB(255,0,0));  
				cvDrawCircle( pImages[imgId], pt, 2, CV_RGB(i*2, 0, 0), 3);
			}
		}
	}
	for(int i=0; i<pImages.size(); i++)
	{
		char imagefile[256];
		sprintf(imagefile, "c:\\temp\\%d.jpg", i);
		cvSaveImage(imagefile, pImages[i]);
	}*/
	//////////////////////////////////////////////////////////////////////////

	//4. bundle adjustment
    CBABase* pBA = new CSBA();
	vector<CameraPara> cameras;
    cameras.resize(fileVector.size());
	//double focus = vecImageDataPointer[0]->GetHt();
	for(int i=0; i<cameras.size(); i++)
	{
		if( vecImageDataPointer[i]->IsHasInitFocus() )
		{
			cameras[i].focus = vecImageDataPointer[i]->GetInitFocus();
		}
		else
		{
			double focus = vecImageDataPointer[i]->GetHt();
			cameras[i].focus = focus; //vecImageDataPointer[i]->GetHt();
			vecImageDataPointer[i]->SetInitFocus(focus);
		}
	}
	pBA->BundleAdjust( cameras.size(), cameras, vecImageDataPointer, vecMatch, vecTrack, "F:\\data\\bundler");

	for(int i=0; i<fileVector.size(); i++)
	{
		delete vecImageDataPointer[i];
		//cvReleaseImage(&pImages[i]);
	}
	delete pGenerateTracks;
	delete pMatch;
	delete pBA;

	printf("Finished! \n");
}

void CProject_PGStationDlg::OnIgbpSinoid()
{
	// TODO: Add your command handler code here
	
	//for east asia, the limits are known 
	//CReadIGBPBase* pReadIGBP = new CReadIGBP();
	//char* igbpfile = "c:\\temp\\Data\\IGBP\\eastAsia.dat";
	//pReadIGBP->Load(igbpfile, IGBP_RESOLUTION_INVERSE_QUAT);

	//for all world, 
	char* igbpfile = "c:\\temp\\Data\\IGBP\\IGBP-small";
	CReadIGBPBase* pReadIGBP = new CReadIGBPAll();
	pReadIGBP->Load(igbpfile, IGBP_RESOLUTION_INVERSE_QUAT);

	//get the IGBP type of Beijng
	double lon = 83;
	double lat = 39;
	int type = pReadIGBP->GetType(lat, lon);
	printf("igbp: %d \n", type);
}

void CProject_PGStationDlg::OnHjAerosolinvertddv()
{
	// TODO: Add your command handler code here

	//loading LUT data
	char lutDir[256] = "c:\\temp\\Data\\LutTypeHJ";
	vector<CAtmosphereCorrectionBase*> vecCorrectBlue;
	vector<CAtmosphereCorrectionBase*> vecCorrectRed;

	//int nTitle[3]={0,10,25};//index for lut of different reflectance

	int aerosolTypeNumber = 3;
	LoadLutsForDDV(lutDir, aerosolTypeNumber, vecCorrectBlue, vecCorrectRed);

	
	//char spath[256] = "c:\\temp\\Data\\HJ\\2013\\10\\17";
	char spath[256] = "c:\\temp\\Data\\HJ\\2013\\10\\04";
	//char spath[256] = "c:\\temp\\Data\\HJ\\2013\\09\\28";
	//char spath[256] = "c:\\temp\\Data\\HJ\\2010\\9\\12";
	//char spath[256] = "c:\\temp\\Data\\HJ\\2010\\10\\29";
	//char spath[256] = "c:\\temp\\Data\\HJ\\2010\\09\\30";
	//char spath[256] = "c:\\temp\\Data\\HJ\\2010\\9\\28";
	//char spath[256] = "c:\\temp\\Data\\HJ\\2010\\6\\25";
	//char spath[256] = "c:\\temp\\Data\\HJ\\2010\\08\\16";


	//wavelength for HJ bands (um)
	double blueWaveLen = 0.475;
	double redWaveLen  = 0.660;
	double irWaveLen   = 0.830;

	double fluxBlue = GetFlux(blueWaveLen);
	double fluxRed  = GetFlux(redWaveLen);
	double fluxIr   = GetFlux(irWaveLen);

	//char xmlFileTitle[256] = "HJ1A-CCD2-1-68-20100816-L20000375929.XML";         //2010.08.16
	char xmlFileTitle[256] = "HJ1B-CCD2-2-68-20131004-L20001064410.XML";       //2013.10.04
	//char xmlFileTitle[256] = "HJ1A-CCD2-456-68-20131017-L20001068649.XML";     //2013.10.17
	//char xmlFileTitle[256] = "HJ1B-CCD2-2-68-20131004-L20001064410.XML";       //2013.10.04
	//char xmlFileTitle[256] = "HJ1A-CCD2-1-68-20130928-L20001059100.XML";       //2013.09.28
	//char xmlFileTitle[256] = "HJ1B-CCD2-2-67-20100930-L20000401266.XML";      
	//char xmlFileTitle[256] = "HJ1A-CCD2-456-68-20100912-L20000391060.XML";
	//char xmlFileTitle[256] = "HJ1A-CCD2-456-68-20100912-L20000391060.XML";
	//sprintf(xmlFile, "%s\\%s", spath, "HJ1A-CCD2-457-68-20101029-L20000417622.XML");
	//sprintf(xmlFile, "%s\\%s", spath, "HJ1B-CCD2-2-67-20100930-L20000401266.XML");
	//sprintf(xmlFile, "%s\\%s", spath, "HJ1A-CCD2-2-66-20100928-L20000399932.XML");
	//sprintf(xmlFile, "%s\\%s", spath, "HJ1B-CCD1-2-64-20100625-L20000331824.XML");
    
	char fileTitle[256];
	strcpy(fileTitle, xmlFileTitle);
	strcpy(fileTitle+strlen(fileTitle)-4, "\0");
	
	//load HJ XML data
	char xmlFile[256];
	sprintf(xmlFile, "%s\\%s", spath, xmlFileTitle);
	
	HJDATA hjdata;
	GetHJFilepaths(xmlFile, hjdata);
	ReadXMLFile(hjdata);
	double sunZenith = hjdata.solar_zenith*DPI;
	double sazimuth = hjdata.solar_azimuth;

	//load angle data
	char angleFile[256]; 
	sprintf(angleFile, "%s\\%s-SatAngle.txt", spath, fileTitle);

	CReadAngleBase* pReadAngle = new CReadHJAngle();
	pReadAngle->Read(angleFile);
	
	char oriTiffFile[256];
	sprintf(oriTiffFile, "%s\\%s-1.TIF", spath, fileTitle);
    
	stGeoInfo oriGeoInfo;
	GetGeoInformation(oriTiffFile, oriGeoInfo);

	//loading images
	char sband1[256];
	char sband2[256];
	char sband3[256];
	char sband4[256];
	sprintf(sband1, "%s\\%s", spath, "band1-clip-envi.tif");
	sprintf(sband2, "%s\\%s", spath, "band2-clip-envi.tif");
	sprintf(sband3, "%s\\%s", spath, "band3-clip-envi.tif");
	sprintf(sband4, "%s\\%s", spath, "band4-clip-envi.tif");
	
	//read geoinfo header for clipped image
	stGeoInfo subGeoInfo;
	GetGeoInformation(sband1, subGeoInfo);

	unsigned char* pband1 = NULL;
	unsigned char* pband2 = NULL;
	unsigned char* pband3 = NULL;
	unsigned char* pband4 = NULL;
	int ht,wd;
	ReadGeoFileByte(sband1, 1, &pband1, ht, wd);
	ReadGeoFileByte(sband2, 1, &pband2, ht, wd);
	ReadGeoFileByte(sband3, 1, &pband3, ht, wd);
	ReadGeoFileByte(sband4, 1, &pband4, ht, wd);
	
	//calculate the NDVI
	int* pNDVI = (int*)malloc(ht*wd*sizeof(int));
	memset(pNDVI, -1, ht*wd*sizeof(int));
	//calculate the NDVI
	for(int j=0; j<ht; j++)
		for(int i=0; i<wd; i++)
		{
			//DN value
			unsigned char r,g,b,ir;
			b  = pband1[j*wd+i];
			r  = pband3[j*wd+i];
			ir = pband4[j*wd+i];

			//convert from DN to reflectance
			if(b>0 || r>0 || ir>0)
			{
				//from DN to radiance
				double refBlue = pband1[j*wd+i] / hjdata.m_gain[0] + hjdata.m_offset[0]; //band 1
				double refRed  = pband3[j*wd+i] / hjdata.m_gain[2] + hjdata.m_offset[2]; //band 3
				double refIR   = pband4[j*wd+i] / hjdata.m_gain[3] + hjdata.m_offset[3]; //band 4				
				
				//from radiance to reflectance
				refBlue = (refBlue*PI)/(fluxBlue*cos(sunZenith));
				refRed  = (refRed*PI)/(fluxRed*cos(sunZenith));
				refIR   = (refIR*PI)/(fluxIr*cos(sunZenith));

				double ndvi = (refIR-refRed) / (refIR+refRed);
				if(ndvi<0) ndvi = 0;
				pNDVI[j*wd+i] = ndvi*100;			
			}			
		}	
	SaveBmp("c:\\temp\\ndvi.bmp", pNDVI, ht, wd);
	
	
	//load NDVI of clear day
	//char ndviFile[256] = "c:\\temp\\Data\\HJ\\2010\\09\\ndvi.tif";
	char ndviFile[256] = "c:\\temp\\Data\\MODIS\\500m\\2010273\\jjt\\modis-9.tif";

	stGeoInfo benchGeoInfo;
	GetGeoInformation(ndviFile, benchGeoInfo);
    unsigned char* pClearNDVI;
	int ndviHt,ndviWd;
	ReadGeoFileByte(ndviFile, 1, &pClearNDVI, ndviHt, ndviWd);	   


	//aerosol inversion based on DDV
	int nStep = 20; //the resolution is down-sampled 
	int desHt = ht / nStep-1;
	int desWd = wd / nStep-1;
     
	unsigned char* pAerosolType = (unsigned char*)malloc(desHt*desWd);
	memset(pAerosolType, 0, desHt*desWd);
    
	float* pAodScatter = (float*)malloc(desHt*desWd*sizeof(float));
	memset(pAodScatter, 0, desHt*desWd*sizeof(float));
	//for(int i=0; i<(desHt*desWd); i++)
	//	pAodScatter[i] = -1;	

	float* pAod = (float*)malloc(desHt*desWd*sizeof(float));
	memset(pAod, 0, desHt*desWd*sizeof(float));
	for(int i=0; i<(desHt*desWd); i++)
		pAod[i] = -1;	

	int ndviThreshold = 50;   // clear
	//int ndviThreshold = 25; // polluted
	for(int j=0; j<desHt; j++)
	{
		for(int i=0; i<desWd; i++)
		{
			int y = j*nStep + nStep*0.5;
			int x = i*nStep + nStep*0.5;
			
			if( pNDVI[y*wd+x]<0 )
				continue;

			//label the pixel of the image
			if(pNDVI[y*wd+x]>=0) pAod[j*desWd+i]=0;
               
			//count the number of DDV pixel
			int nHit = 0;
			for(int m=-nStep/2; m<nStep/2; m++)
				for(int n=-nStep/2; n<nStep/2; n++)
				{
					double tx = subGeoInfo.left + (x+n)*subGeoInfo.dx;
					double ty = subGeoInfo.top + (y+m)*subGeoInfo.dy;
					int row = (ty-benchGeoInfo.top)/benchGeoInfo.dy;
					int col = (tx-benchGeoInfo.left)/benchGeoInfo.dx;

					//if( pNDVI[ (y+m)*wd + (x+n) ]>=ndviThreshold )
					if(pClearNDVI[row*ndviWd+col]>=ndviThreshold)
					{
						nHit ++;
					}
				}
			
			//when the number of DDV is more than the half 
			if(nHit>(nStep*nStep*0.3))
			{
				//average the reflectance
				double sumRefBlue = 0;
				double sumRefRed  = 0;
				for(int m=-nStep/2; m<nStep/2; m++)
					for(int n=-nStep/2; n<nStep/2; n++)
					{
						int index = (y+m)*wd + (x+n); 

						double tx = subGeoInfo.left + (x+n)*subGeoInfo.dx;
						double ty = subGeoInfo.top + (y+m)*subGeoInfo.dy;
						int row = (ty-benchGeoInfo.top)/benchGeoInfo.dy;
						int col = (tx-benchGeoInfo.left)/benchGeoInfo.dx;

						//if( pNDVI[index]>=ndviThreshold ) // ndvi=0.5
						if(pClearNDVI[row*ndviWd+col]>=ndviThreshold)
						{
							double refBlue = pband1[index] / hjdata.m_gain[0] + hjdata.m_offset[0]; //band 1
							double refRed  = pband3[index] / hjdata.m_gain[2] + hjdata.m_offset[2]; //band 3
							//double refIR   = pband4[index] / hjdata.m_gain[3] + hjdata.m_offset[3]; //band 4				
							
							//from radiance to reflectance
							refBlue = (refBlue*PI)/(fluxBlue*cos(sunZenith));
							refRed  = (refRed*PI)/(fluxRed*cos(sunZenith));
							//refIR   = (refIR*PI)/(fluxIr*cos(sunZenith));

							sumRefBlue += refBlue;
							sumRefRed  += refRed;
						}
					}
				sumRefBlue /= nHit;
				sumRefRed  /= nHit;

				//cloud recognition
				if(sumRefBlue>0.2 && sumRefRed>0.2)
					continue;

				//invert	
				//convert to ground coordinate
				double gy = subGeoInfo.top  + y*subGeoInfo.dy;
				double gx = subGeoInfo.left + x*subGeoInfo.dx;
				int row =  (oriGeoInfo.top - gy)  / fabs(oriGeoInfo.dy);
				int col =  (gx - oriGeoInfo.left) / fabs(oriGeoInfo.dx);
				double vza, vazimuth;
				pReadAngle->GetViewGeo(row, col, vza, vazimuth);
				if(vazimuth<0) vazimuth += 360;
				double relativeAzimuth = fabs(vazimuth-sazimuth);
				if(relativeAzimuth>180) relativeAzimuth -= 180;

				//aerosol inversion
				double minErr = 1000000;
				double invertAod = 0.01;
				int    nInvertType = 0;
				for(int k=1; k<200; k++) //for blue channel
				{
					double aod = k*0.02;
					double refError = 100000000;					
					int    nType = 0;
					//for(int m=0; m<aerosolTypeNumber; m++)
					int m = 0;
					{
						//get the extinction parameters
						double extRed  = vecCorrectRed[m]->GetExtinction();     
						double extBlue = vecCorrectBlue[m]->GetExtinction();    

						double redAod = extRed/extBlue*aod;
						double correctBlue = vecCorrectBlue[m]->CalculateGroundReflectance(hjdata.solar_zenith, vza, relativeAzimuth, aod, sumRefBlue);
						double correctRed  = vecCorrectRed[m]->CalculateGroundReflectance(hjdata.solar_zenith, vza, relativeAzimuth, redAod, sumRefRed);

						if(correctBlue>correctRed)
							continue;

						//very small reflectance is rare
						//if(correctBlue<0.01 )
						//	continue;

						if( correctBlue<0 || correctRed<0 )
							continue;

						//double err = correctBlue*2 - correctRed;
						double ratio = correctBlue/correctRed;
						double err  = fabs(ratio - 0.5);
						if(err<refError)
						{
							refError = err;
							nType = m;
						}
					}

					if(refError<minErr)
					{
						minErr = refError;
						invertAod = aod;
						nInvertType = nType+1;
					}
				}

				//printf("%lf ", invertAod);
				pAod[j*desWd+i]         = invertAod;
				pAodScatter[j*desWd+i]  = invertAod;
				pAerosolType[j*desWd+i] = nInvertType;
			}
		}
		printf(" %d \n", j);
	}
	free(pNDVI);
	
	//inverted aod smooth, remove the single pixel and average, added by Donghai,Xie, 2014.3.24
	
	//output the inverted aod
	subGeoInfo.dx *= nStep;
	subGeoInfo.dy *= nStep;
	
	DataSmoothMedian(pAod, desHt, desWd);
	DataSmoothMedian(pAodScatter, desHt, desWd);
	//DataSmoothMedian(pAod, desHt, desWd);

	GdalWriteFloat("c:\\temp\\aod.tif", pAod, desHt, desWd, subGeoInfo);   //for interpolation
	GdalWriteFloat("c:\\temp\\aodScatter.tif", pAodScatter, desHt, desWd, subGeoInfo); //for display
	GdalWriteByte("c:\\temp\\aodType.tif", pAerosolType, desHt, desWd, subGeoInfo);
    
	free(pAod);
	free(pAodScatter);
	free(pAerosolType);

	printf("Finished! \n");
}

void CProject_PGStationDlg::OnInterpolation2d()
{
	// TODO: Add your command handler code here
	
	//read the data into the buffer
	char aodfile[256] = "c:\\temp\\aod.tif";

	stGeoInfo geoInfo;
	GetGeoInformation(aodfile, geoInfo);

	float* pAod = NULL;
	int ht,wd;
	ReadGeoFileFloat(aodfile, 1, &pAod, ht, wd );

	float* pInterp = (float*)malloc(ht*wd*sizeof(float));
	for(int i=0; i<ht*wd; i++)
		pInterp[i] = pAod[i];

	//
	for(int j=1; j<ht-1; j++)
	{
		for(int i=1; i<wd-1; i++)
		{	
			if( pAod[j*wd+i] != 0 )
				continue;
			
			bool bIsFind = false;
			int  size = 3;

			vector<double> vv;
			vector<double> vw;
			
			while( !bIsFind )
			{
				vv.clear();
				vw.clear();
				
				/*
				if(size>5)
				{
					break;
				}
				*/
				                
				for(int m=-size/2; m<=size/2; m++)
					for(int n=-size/2; n<=size/2; n++)
					{
						int iy = max(0, min(ht-1, j+m));
						int ix = max(0, min(wd-1, i+n));

						if( pAod[iy*wd+ix]>0 )
						{
							vv.push_back( pAod[iy*wd+ix] );							
							double radius = sqrt( (double)( m*m+n*n+1) ); 
							double w = 1.0/radius;
							vw.push_back(w);
						}						
					}
				
				if(vv.size()>1)
				{
					bIsFind = true;
				}
				else
				{
					size += 2;
				}
			}

			//interpolation
			if(bIsFind)
			{
				double sdn = 0;
				double sw = 0;
				for(int k=0; k<vv.size(); k++)
				{
					sdn += vv[k]*vw[k];
					sw  += vw[k];
				}
				sdn = sdn/sw;				
				//pInterp[j*wd+i] = sdn;
				pAod[j*wd+i] = sdn;
			}
			else
			{
				//pInterp[j*wd+i] = -1;
				pAod[j*wd+i] = -1;
			}			
		}
		printf(".");
	}
	printf(" \n Finished!  \n");

	//GdalWriteFloat("c:\\temp\\aod-interp.tif", pInterp, ht, wd, geoInfo);
	GdalWriteFloat("c:\\temp\\aod-interp.tif", pAod, ht, wd, geoInfo);
	
	free(pAod);
	free(pInterp);	
}

void CProject_PGStationDlg::OnSimulationDt()
{
	// TODO: Add your command handler code here	
	
	//simulation for new algorithm of Dark Target

	//loading lut
	//loading LUT data
	char lutDir[256] = "c:\\temp\\Data\\LutTypeHJ";
	vector<CAtmosphereCorrectionBase*> vecCorrectBlue;
	vector<CAtmosphereCorrectionBase*> vecCorrectRed;

	int nTitle[3]={0,10,25};//index for lut of different reflectance
	
	int aerosolTypeNumber = 1;

	LoadLutsForDDV(lutDir, aerosolTypeNumber, vecCorrectBlue, vecCorrectRed);

	/*
	for(int i=0; i<aerosolTypeNumber; i++)
	{
		char lut[256];

		//blue channel
		char blueDir[256];
		sprintf(blueDir, "%s\\%d", lutDir, i+1);
		strcat(blueDir, "\\blue");

		CAtmosphereCorrectionBase* pCorrectBlue = new CAtmosphereCorrectionRt3();
		pCorrectBlue->SetWaveLen(0.475);
		//read the file containing the extinction parameters
		char extFile[256];
		sprintf(extFile, "%s\\ext.txt",  blueDir);
		double ext;
		FILE* fp = fopen(extFile, "r");
		fscanf(fp, "%lf", &ext);
		fclose(fp);
		pCorrectBlue->SetExtinction(ext);

		for(int k=0; k<3; k++)
		{
			sprintf(lut, "%s\\%d.dat",  blueDir, nTitle[k]);
			CLutBase* pLut = new CLutBinCont();
			pLut->Load(lut);
			pCorrectBlue->SetLut(pLut, k);
		}		
		vecCorrectBlue.push_back(pCorrectBlue);

		//red channel
		char redDir[256];
		sprintf(redDir, "%s\\%d", lutDir, i+1);
		strcat(redDir, "\\red");		

		CAtmosphereCorrectionBase* pCorrectRed = new CAtmosphereCorrectionRt3();
		pCorrectRed->SetWaveLen(0.66);
		//read the file containing the extinction parameters
		sprintf(extFile, "%s\\ext.txt",  redDir);
		fp = fopen(extFile, "r");
		fscanf(fp, "%lf", &ext);
		fclose(fp);
		pCorrectRed->SetExtinction(ext);

		for(int k=0; k<3; k++)
		{
			sprintf(lut, "%s\\%d.dat",  redDir, nTitle[k]);
			CLutBase* pLut = new CLutBinCont();
			pLut->Load(lut);
			pCorrectRed->SetLut(pLut, k);
		}
		vecCorrectRed.push_back(pCorrectRed);
	}
	*/




	double blueRef = 0.02;
	double ratio = 2;     // ratio = (reflectance of Red) /  (reflectance of Blue)
	double redRef  = blueRef*ratio;

	double sza = 30;
	double vza = 5;
	double azi = 100;
	
	double blueExt = vecCorrectBlue[0]->GetExtinction();
	double redExt  = vecCorrectRed[0]->GetExtinction();


	FILE* fp = fopen("c:\\temp\\hj_simulate.txt", "w");

	for(int i=1; i<11; i++)
	{
		double aodBlue = 0.1*i;
		double aodRed  = redExt/blueExt*aodBlue;

		printf("aodBlue: %lf   aodRec:\\temp%lf \n", aodBlue, aodRed);

		//blue
		double T,S;
		vecCorrectBlue[0]->CalculateTAndS(sza, vza, azi, aodBlue, T, S);
		printf("transmission:%lf  backscattering ratio:%lf \n", T, S);
		double r,rp,pa;
		vecCorrectBlue[0]->GetPathRef(sza, vza, azi, aodBlue, r, rp);
		pa = r;
		double toa = pa + (blueRef*T)/(1-S*blueRef);
		printf("blue toa:%lf \n", toa);

		fprintf(fp, "%lf %lf %lf %lf %lf", aodBlue, T, S, pa, toa);

		//red
		vecCorrectRed[0]->CalculateTAndS(sza, vza, azi, aodRed, T, S);
		printf("transmission:%lf  backscattering ratio:%lf \n", T, S);
		vecCorrectRed[0]->GetPathRef(sza, vza, azi, aodRed, r, rp);
		pa = r;
		toa = pa + (redRef*T)/(1-S*redRef);
		printf("red toa:%lf \n", toa);

		fprintf(fp, "   %lf %lf %lf %lf %lf \n", aodRed, T, S, pa, toa);

		printf("\n");		
	}
	fclose(fp);
}


void CProject_PGStationDlg::OnHjInvertsinglept()
{
	// TODO: Add your command handler code here

	//loading luts
	char lutDir[256] = "c:\\temp\\Data\\LutTypeHJ";
	vector<CAtmosphereCorrectionBase*> vecCorrectBlue;
	vector<CAtmosphereCorrectionBase*> vecCorrectGreen;
	vector<CAtmosphereCorrectionBase*> vecCorrectRed;

	int aerosolTypeNumber = 1;
	LoadLutsRGB(lutDir, aerosolTypeNumber, vecCorrectBlue, vecCorrectGreen, vecCorrectRed);

	//0.000000 0.091970 0.074507 
	//0.100000 0.095492 0.076015 
	//0.500000 0.110281 0.083591 
	//1.000000 0.130957 0.096100 
	//2.000000 0.170537 0.124531 

	//0.1
	double toaBlue = 0.095492;
	double toaRed = 0.076015;

	//0.5
	toaBlue = 0.110281;
	toaRed  = 0.083591;

	//1.0
	toaBlue = 0.130957;
	toaRed  = 0.096100;

	//2.0
	toaBlue = 0.170537;
	toaRed  = 0.124531;

	double sza = 30;
	double vza = 0;
	double azi = 180;	

	//aerosol inversion
	double minErr = 1000000;
	double invertAod = 0.0;
	int    nInvertType = 0;

	FILE* fp = fopen("c:\\temp\\invert_ac_aot_2.0.txt", "w");

	for(int k=0; k<=20; k++) //for blue channel
	{
		double aod = k*0.1;
		double refError = 100000000;					
		int    nType = 0;
		//for(int m=0; m<aerosolTypeNumber; m++)
		int m = 0;
		{
			//get the extinction parameters
			double extRed  = vecCorrectRed[m]->GetExtinction();     
			double extBlue = vecCorrectBlue[m]->GetExtinction();    

			double redAod = extRed/extBlue*aod;
			double correctBlue = vecCorrectBlue[m]->CalculateGroundReflectance(sza, vza, azi, aod, toaBlue);
			double correctRed  = vecCorrectRed[m]->CalculateGroundReflectance(sza, vza, azi, redAod, toaRed);

			if(correctBlue<0 || correctRed<0)
				continue;

			printf("%lf %lf %lf \n", aod, correctBlue, correctRed);
			double tr = correctBlue / correctRed;
			fprintf(fp, "%lf %lf %lf %lf \n", aod, tr, correctBlue, correctRed);

			if(correctBlue>correctRed)
				continue;

			//very small reflectance is rare
			//if(correctBlue<0.01 )
			//	continue;

			if( correctBlue<0 || correctRed<0 )
				continue;

			//double err = correctBlue*2 - correctRed;
			double ratio = correctBlue/correctRed;
			double err  = fabs(ratio - 0.5);
			if(err<refError)
			{
				refError = err;
				nType = m;
			}
		}

		if(refError<minErr)
		{
			minErr = refError;
			invertAod = aod;
			nInvertType = nType+1;
		}
	}

	printf("inverted aot: %lf \n", invertAod);
	fclose(fp);

	/*
	double toaBlue,toaRed;
	double t1,t2,s1,s2,pa1,pa2;
	double aodBlue, aodRed;

	FILE* fp = fopen("c:\\temp\\hj_simulate.txt", "r");
    for(int i=0; i<10; i++)
	{
		fscanf(fp, "%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf", 
				&aodBlue, &t1, &s1, &pa1, &toaBlue,
				&aodRed,  &t2, &s2, &pa2, &toaRed);
		


	}
	fclose(fp);
	*/


}

void CProject_PGStationDlg::OnModisInvertaod()
{
	// TODO: Add your command handler code here

	//loading lut
	char lutDir[256] = "c:\\temp\\Data\\LutTypeHJ";
	vector<CAtmosphereCorrectionBase*> vecCorrectBlue;
	vector<CAtmosphereCorrectionBase*> vecCorrectRed;
	int nAerosolType = 1;
	LoadLutsForDDV(lutDir, nAerosolType, vecCorrectBlue, vecCorrectRed);
	
	//loading MOD02
	//char* filePath = "c:\\temp\\Data\\MODIS\\2014-1-1\\MOD021KM.A2014001.0255.005.2014001100611.hdf";
	char* filePath = "c:\\temp\\Data\\MODIS\\MOD021KM.A2013336.0245.005.2013336103942.hdf";
	CReadDataBase* pReadMod02 = new CReadMOD02();
	pReadMod02->Read(filePath);

	double minlon,maxlon,minlat,maxlat;
	pReadMod02->GetLLRange(minlon, maxlon, minlat, maxlat);

	//
	int ht = pReadMod02->GetHt();
	int wd = pReadMod02->GetWd();

	double xstep = (maxlon-minlon) / double(wd);
	double ystep = (maxlat-minlat) / double(ht);
    double llstep = min(xstep, ystep);
    
	int llWd = (maxlon-minlon)/llstep;
	int llHt = (maxlat-minlat)/llstep;

	//unsigned char* pBlue = (unsigned char*)malloc(ht*wd);
	unsigned char* pRed = (unsigned char*)malloc(ht*wd);
    //unsigned char* pIR = (unsigned char*)malloc(ht*wd);
	unsigned char* pIR21 = (unsigned char*)malloc(ht*wd);
	unsigned char* pNDVI = (unsigned char*)malloc(ht*wd);
	memset(pNDVI, 0, ht*wd);

	for(int j=0; j<ht; j++)
		for(int i=0; i<wd; i++)
		{
			double blue,red,ir;
			double c5;    //channel 5 (1.24)
			double c7;    //channel 7 (2.1)

			pReadMod02->GetReflectance(2, j, i, blue);
			pReadMod02->GetReflectance(0, j, i, red);
			pReadMod02->GetReflectance(1, j, i, ir);			
			pReadMod02->GetReflectance(4, j, i, c5);
			pReadMod02->GetReflectance(6, j, i, c7);

			double solarZenith, solarAzi;
			pReadMod02->GetSolarGeo(j, i, solarZenith, solarAzi);
			double sensorZenith, sensorAzi;
			pReadMod02->GetViewGeo(j, i, sensorZenith, sensorAzi);

			blue /= cos(solarZenith*DPI);
			red  /= cos(solarZenith*DPI);
			ir   /= cos(solarZenith*DPI);
			c5   /= cos(solarZenith*DPI);
			c7   /= cos(solarZenith*DPI);

			double ndvi      = (ir-red)/(ir+red);
			double ndvi_swir = (c5-c7)/(c5+c7);
			
			if(ndvi<0) ndvi=0;
			if(ndvi_swir<0) ndvi_swir = 0;

			pNDVI[j*wd+i] = ndvi*100;

			//pBlue[j*wd+i] = blue*100;
			pRed[j*wd+i]  = red*100;
            //pIR[j*wd+i]   = ir*100;
			pIR21[j*wd+i] = c7*100;
		}
	SaveBmp("c:\\temp\\modis_ndvi.bmp", pNDVI, ht, wd);


	//
	unsigned char* pAod = (unsigned char*)malloc(ht*wd);
	memset(pAod, 0, ht*wd);

	float* pScatteredAod = (float*)malloc(llHt*llWd*sizeof(float));
	for(int i=0; i<(llHt*llWd); i++)
		pScatteredAod[i] = 0;
	
	//GetInnerRegionMask(mask, wd, ht, px, py, index, 100, 255, true);

	int offset = 10;
	for(int j=offset; j<ht-offset-1; j++)
	{
		printf("%d ", j);
		for(int i=offset; i<wd-offset-1; i++)
		{
			//i = 910;
			//j = 1593;

			double lon,lat;
			pReadMod02->GetLL(j, i, lon, lat );

			double solarZenith, solarAzi;
			pReadMod02->GetSolarGeo(j, i, solarZenith, solarAzi);
			double sensorZenith, sensorAzi;
			pReadMod02->GetViewGeo(j, i, sensorZenith, sensorAzi);

			//cloud removal
			double blue;
			pReadMod02->GetReflectance(2, j, i, blue);
			blue /= cos(solarZenith*DPI);
			//if(blue>0.1) continue;
			
			double red;
			pReadMod02->GetReflectance(0, j, i, red);
			red /= cos(solarZenith*DPI);
			if(red>0.2) continue;
			
			//water removal ( using 2.1 um)	
			double c7;
			pReadMod02->GetReflectance(6, j, i, c7);
			c7 /= cos(solarZenith*DPI);			
			if(c7<0.05)
				continue;
            
			int nDT = 0;
			for(int m=-offset; m<offset; m++ )
				for(int n=-offset; n<offset; n++ )
				{
					if( pIR21[(j+m)*wd+(i+n)]>1 && pIR21[(j+m)*wd+(i+n)]<25 )
						nDT++;
				}
			if(nDT>12)
			{
				if( pIR21[j*wd+i]>1 && pIR21[j*wd+i]<25 )
				{
					double blueSurf = 0.25*c7;
										
					double azi = fabs(solarAzi-sensorAzi);
					if(azi>180) azi-=180;

					//invert
					double minErr = 10000000;
					double finalAod = 0;
					for(int k=0; k<nAerosolType; k++)
					{						
						double err, invertAod;
						AerosolInvertSinglePt( vecCorrectBlue[k], blue, solarZenith, sensorZenith, azi, 
												blueSurf, err, invertAod );
						if(err<minErr)
						{
							minErr = err;
							finalAod = invertAod;
						}
					}
					
					if(finalAod>1.5)
						continue;
					
					pAod[j*wd+i] = finalAod*100;

					int ix = (lon-minlon)/llstep;
					int iy = (maxlat-lat)/llstep;
					

					pScatteredAod[iy*llWd+ix] = finalAod;
					
					//fill null location
					for(int m=-1; m<=1; m++)
						for(int n=-1; n<=1; n++)
						{
							if( pScatteredAod[(iy+m)*llWd+(ix+n)]==0 )
							{
								pScatteredAod[(iy+m)*llWd+(ix+n)] = finalAod;
							}
							else
							{
								pScatteredAod[(iy+m)*llWd+(ix+n)] = (pScatteredAod[(iy+m)*llWd+(ix+n)] + finalAod)*0.5;
							}
						}						
				}
			}
		}
	}

	//SaveBmp("c:\\temp\\darkMask.bmp", pDarkMask, ht, wd);
	SaveBmp("c:\\temp\\modis_aod.bmp", pAod, ht, wd);

	GdalWriteFloatLL("c:\\temp\\modis_aod.tif", pScatteredAod, llHt, llWd, minlon, maxlon, minlat, maxlat);

	free(pAod);
	free(pScatteredAod);

	printf("Finished! \n");
}

/*
void CProject_PGStationDlg::OnGpuDot()
{
	// TODO: Add your command handler code here	
	InitCUDA();
	InvokeDot();
}
*/

void CProject_PGStationDlg::OnAerosolAngstrom()
{
	// TODO: Add your command handler code here

	
	//bench wavelength
	double refWave   = 1.02;
	double refAod    = 0.084074;//0.106926; //0.366101; //0.653877;//1.469405; //0.034463;  //0.119;//0.243; //0.118698;
	double inputWave, inputAod;
	double angstrom;

	/*
	double inputWave = 0.87;
	double inputAod = 0.29; //0.131133;
	double angstrom = CalcAngstrom(inputWave, refWave, inputAod, refAod);
	printf("Angstrom: %lf \n", angstrom);

	inputWave = 0.675;
	inputAod  = 0.41; //0.131133;
	angstrom = CalcAngstrom(inputWave, refWave, inputAod, refAod);
	printf("Angstrom: %lf \n", angstrom);
	*/

	inputWave = 0.44;
	inputAod  = 0.196272;//0.244279; //1.125412; //1.801883;//4.763064;//0.075624; //0.266;//0.722; //0.131133;
	angstrom = CalcAngstrom(inputWave, refWave, inputAod, refAod);
	printf("Angstrom: %lf \n", angstrom);


	/*
	//red - blue
	refWave   = 0.675;
	refAod    = 0.41; 
	inputWave = 0.44;
	inputAod  = 0.71; 
	angstrom = CalcAngstrom(inputWave, refWave, inputAod, refAod);
	printf("Angstrom: %lf \n", angstrom);
	*/
	
	
	double estimateWave = 0.55; //0.675;
	double outAod = CalculateAODUsingAngstrom(angstrom, estimateWave, refWave, refAod);
	printf("Estimated AOD (550nm): %lf \n", outAod);
	
}

void CProject_PGStationDlg::OnHjSimulatetoa()
{
	// TODO: Add your command handler code here

	//1. loading LUTs for specified aerosol type
	char lutDir[256] = "c:\\temp\\Data\\LutTypeHJ";
	vector<CAtmosphereCorrectionBase*> vecCorrectBlue;
	vector<CAtmosphereCorrectionBase*> vecCorrectRed;
	int aerosolTypeNumber = 1;
	LoadLutsForDDV(lutDir, aerosolTypeNumber, vecCorrectBlue, vecCorrectRed);

	//2. initial parameters
	double sza = 30;
	double vza = 0;
	double azi = 180;
	double blueRef = 0.03;
	double redRef  = 0.0;

	double redExt = vecCorrectRed[0]->GetExtinction();
	double blueExt = vecCorrectBlue[0]->GetExtinction();

	//3. simulation
	FILE* fp = fopen("c:\\temp\\toa.txt", "w");
    //for(double ratio=0.4; ratio<=0.61; ratio+=0.02)
	for(double ratio=0.46; ratio<=0.54; ratio+=0.01)
	{	
		redRef = blueRef/ratio;

		printf(" ratio: %5.3lf \n", ratio);

		fprintf(fp, "%lf %6.4lf %6.4lf ", ratio, blueRef, redRef);

		for(double aod=0.1; aod<=2.0; aod+=0.1)
		{			
			double aodBlue = aod;
			double aodRed  = redExt/blueExt*aodBlue;

			//printf("aodBlue: %lf   aodRec:\\temp%lf \n", aodBlue, aodRed);

			//blue
			double T,S;
			vecCorrectBlue[0]->CalculateTAndS(sza, vza, azi, aodBlue, T, S);
			//printf("transmission:%lf  backscattering ratio:%lf \n", T, S);
			double r,rp,pa;
			vecCorrectBlue[0]->GetPathRef(sza, vza, azi, aodBlue, r, rp);
			pa = r;
			double toaBlue = pa + (blueRef*T)/(1-S*blueRef);
			//printf("blue toa:%lf \n", toa);
			//fprintf(fp, "%lf %lf %lf %lf %lf", aodBlue, T, S, pa, toa);

			//red
			vecCorrectRed[0]->CalculateTAndS(sza, vza, azi, aodRed, T, S);
			//printf("transmission:%lf  backscattering ratio:%lf \n", T, S);
			vecCorrectRed[0]->GetPathRef(sza, vza, azi, aodRed, r, rp);
			pa = r;
			double toaRed = pa + (redRef*T)/(1-S*redRef);
			//printf("red toa:%lf \n", toa);

			//fprintf(fp, "   %lf %lf %lf %lf %lf \n", aodRed, T, S, pa, toa);

			//fprintf(fp, " aoc:\\temp %5.3lf  bt: %5.3lf  rt: %5.3lf ", aod, toaBlue, toaRed);
			fprintf(fp, "   %5.3lf %5.3lf %5.3lf", aod, toaBlue, toaRed);

			//printf("\n");	

			//invert
			double invertAOD = DTSinglePtInvert(vecCorrectBlue, vecCorrectRed, sza, vza, azi, toaBlue, toaRed, 0.5);
			fprintf(fp, " %5.3lf     ", invertAOD);
		}
		fprintf(fp, "  \n");
	}
	fclose(fp);

}

void CProject_PGStationDlg::OnLutInterpolation()
{
	// TODO: Add your command handler code here
	char lutfile[256] = "c:\\temp\\Data\\LUTEastAsia\\Double_690_176_517_73_2768_623_117_1559_12_0.dat";

	CLutBase* pLut = new CLutBinCont();
	pLut->Load(lutfile);

	double sza = 30;
	double azi = 180;
	double aod = 0.2;

	for(int i=0; i<90; i+=5)
	{
		double vza = i;
		double r, rp;
		pLut->GetValue(sza, vza, azi, aod, r, rp);
		printf("%lf \n", r, rp);
	}

}

void CProject_PGStationDlg::OnHoughCircle()
{
	// TODO: Add your command handler code here

	IplImage* src = CloneCurrentImage();
    
	if(src != NULL)
	{
		//IplImage* src = cvLoadImage( argv[1], 0 );  
		IplImage* dst =  cvCloneImage(src); //cvLoadImage( argv[1], 0 );  
		IplImage* pEdge = cvCloneImage(dst);

		
		CvMemStorage* storage = cvCreateMemStorage(0);  
		cvSmooth( src, dst, CV_GAUSSIAN, 5, 5 );  //smooth  

		int higherT = 36;
		cvCanny(dst, pEdge, higherT*0.5, higherT);

		CvSeq* results = cvHoughCircles(  
			dst,  
			storage,  
			CV_HOUGH_GRADIENT,  
			2,              //dp
			dst->width/10,  //mindist
			higherT,		//higher threshold passed to Canny edge detector
			48				//accumulator threshold at the center detection stage
			);  

		for( int i = 0; i < results->total; i++ )  
		{  
			float* p = ( float* )cvGetSeqElem( results, i );  
			
			CvPoint pt = cvPoint( cvRound( p[0] ), cvRound( p[1] ) );  
			cvCircle(  
				dst,  
				pt,               //circle center  
				cvRound( p[2] ),  //radius  
				CV_RGB( 255, 255, 255 )  
				);   
		}  

		cvNamedWindow( "cvHoughCircles", 1 );  
		cvShowImage( "cvHoughCircles", dst );

		cvNamedWindow( "Edge", 1 );  
		cvShowImage( "Edge", pEdge );

		cvReleaseImage(&src);
		cvReleaseImage(&dst);
		cvReleaseImage(&pEdge);
	}

}

void CProject_PGStationDlg::OnEclipseRandom()
{
	// TODO: Add your command handler code here

	IplImage* src   = CloneCurrentImage();
	IplImage* pShow = CloneCurrentImageColor();

	if(src != NULL)
	{
		IplImage* pEdge = cvCloneImage(src);
		cvSmooth(src, src, 2, 5);
		cvCanny(src, pEdge, 4, 64);
		cvSaveImage("c:\\temp\\canny.jpg", pEdge);

		vector<stEllipse> ellipseArray;
		//DetectEllipse(pEdge, ellipseArray);
		//DetectEllipseUsingContour(pEdge, ellipseArray);
		CEllipseDetectBase* pDetectEllipse = new CEllipseDetectOneByOne(); //CEllipseDetectFromAll();	
		pDetectEllipse->Detect(pEdge, ellipseArray, 0.6, 24);

		//draw
		printf("detected number: %d \n", ellipseArray.size());
		for(int i=0; i<ellipseArray.size(); i++)
		{
			stEllipse ellipsePara = ellipseArray[i];

			if( fabs(ellipsePara.angle)<45 ) //&& ellipsePara.a<120)
			{
				printf(" ellipse: %lf %lf %lf %lf %lf \n", ellipsePara.x0, ellipsePara.y0, 
				ellipsePara.a, ellipsePara.b, ellipsePara.angle);			
				DrawEllipse(ellipsePara, pShow);
			}
		}

		cvNamedWindow("Ellipse");
		cvShowImage("Ellipse", pShow);
	}


}

void CProject_PGStationDlg::OnHoughLinenew()
{
	// TODO: Add your command handler code here

	IplImage* src = CloneCurrentImage();
	IplImage* pShow = CloneCurrentImageColor();

	if(src != NULL)
	{
		IplImage* pEdge = cvCloneImage(src);

		cvSmooth(src, src);
		cvCanny(src, pEdge, 8, 36);
		cvSaveImage("c:\\temp\\canny.jpg", pEdge);

		vector<stLINE> lineArray;
		HoughLines(pEdge, lineArray, 
			1, 
			PI/180.0, 
			48,        //threshold for accumulator
			48,        //line length
			10);       //line gap
		
		IplImage* pLineImage = cvCloneImage(pEdge);
		memset(pLineImage->imageData, 0, pLineImage->height*pLineImage->widthStep);

		for(int i=0; i<lineArray.size(); i++)
		{
			CvPoint p1,p2;
			p1.x = lineArray[i].v1.x;
			p1.y = lineArray[i].v1.y;
			p2.x = lineArray[i].v2.x;
			p2.y = lineArray[i].v2.y;
			cvLine( pShow, p1,p2,CV_RGB(255,0,0) );
			cvLine( pLineImage, p1,p2,CV_RGB(255,255,255) );
		}

		int ht = pEdge->height;
		int wd = pEdge->width;
		int swd = pEdge->widthStep;
		for(int j=0; j<ht; j++)
			for(int i=0; i<wd; i++)
			{
				int value = (unsigned char)(pLineImage->imageData[j*swd+i]);
				if( value>0 )
					pEdge->imageData[j*swd+i] = 0;
			}

		cvNamedWindow("canny");
		cvShowImage("canny", pEdge);

		cvNamedWindow("HoughLine");
		cvShowImage("HoughLine", pShow);
	}
}

void CProject_PGStationDlg::OnHjCalcddv()
{
	// TODO: Add your command handler code here
	//char filepath[256] = "c:\\temp\\Data\\HJ\\2013\\09\\26";
	//char filepath[256] = "c:\\temp\\Data\\HJ\\2010\\09\\28";
	char filepath[256] = "F:\\Data\\MODIS\\500m\\2010273\\jjj";

	double ndviThreshold = 0.35;


	char redFile[256];
	char irFile[256];

	//sprintf(redFile, "%s\\band3-clip-envi.tif", filepath);
	//sprintf(irFile,  "%s\\band4-clip-envi.tif", filepath);
	sprintf(redFile, "%s\\r.tif", filepath);
	sprintf(irFile,  "%s\\ir.tif", filepath);

	//loading image
    stGeoInfo geoInfo;
	GetGeoInformation(redFile, geoInfo);
    
	int ht,wd;
	unsigned short* pRed;
	ReadGeoFileUShort(redFile, 1, &pRed, ht, wd );
	//SaveBmp("c:\\temp\\red.bmp", pRed, ht, wd);

	unsigned short* pIR;
	ReadGeoFileUShort(irFile, 1, &pIR, ht, wd );
	//SaveBmp("c:\\temp\\red.bmp", pRed, ht, wd);

	//calculate the NDVI
	unsigned char* pNDVI = (unsigned char*)malloc( ht*wd );
	memset(pNDVI, 0, ht*wd*sizeof(unsigned char));
	
	for(int j=0; j<ht; j++)
		for(int i=0; i<wd; i++)
		{
			if( pIR[j*wd+i]==0 && pRed[j*wd+i]==0 )
				continue;

			double ndvi = (double)( pIR[j*wd+i]-pRed[j*wd+i] ) / (double)( pIR[j*wd+i]+pRed[j*wd+i] );
			if(ndvi<0) ndvi = 0;

			//ndvi *= 100;
			pNDVI[j*wd+i] = ndvi*100;

			/*
			if(ndvi>ndviThreshold)
				pNDVI[j*wd+i] = 255;
			else
				pNDVI[j*wd+i] = 1;
				*/
		}

	GdalWriteByte("c:\\temp\\modis-9.tif", pNDVI, ht, wd, geoInfo);

	free(pRed);
	free(pIR);

	printf("Finished! \n");

}

void CProject_PGStationDlg::OnHjSimulatefromaod()
{
	// TODO: Add your command handler code here
	
	//loading LUTs
	//1. loading LUTs for specified aerosol type
	char lutDir[256] = "c:\\temp\\Data\\LutTypeHJ";
	vector<CAtmosphereCorrectionBase*> vecCorrectBlue;
	vector<CAtmosphereCorrectionBase*> vecCorrectRed;
	int aerosolTypeNumber = 4;
	LoadLutsForDDV(lutDir, aerosolTypeNumber, vecCorrectBlue, vecCorrectRed);
    
	//calculate the Angstrom parameters


	//2. define the geometry 
	double  sza = 60;
	double  vza = 0;
	double  phi = 180;

	//3. define the aod
	double  aodArray[14] = {0.0, 0.1, 0.2, 0.3, 0.5, 0.8, 1.0, 1.2, 1.5, 2.0, 2.4, 3.0, 4.0, 5.0};


	FILE* fp = fopen("c:\\temp\\simulateDiffAOD.txt", "w");
	for(int i=0; i<14; i++)
	{
		printf("%d \n", i);
		fprintf(fp, "%lf ", aodArray[i]);
		for(int j=0; j<aerosolTypeNumber; j++)
		{	
			double r,rp;
			vecCorrectBlue[j]->GetPathRef(sza, vza, phi, aodArray[i], r, rp);
			fprintf(fp, "%lf ", r);
		}
		fprintf(fp, "\n");
	}
	fclose(fp);	
	
	printf("Finished ! \n");

}

void free_cb(unsigned char* a)
{
	free(a);
}

void CProject_PGStationDlg::OnCallidlDll()
{
	// TODO: Add your command handler code here
	//HANDLE hInstance=AfxGetInstanceHandle(); 
	//int rt = IDL_Win32Init(0,hInstance,NULL,0); 

	int rt = IDL_Win32Init(0,this,NULL,0);
	
	//rt = IDL_ExecuteStr(" .compile test.pro ");
    //rt = IDL_ExecuteStr("test");

	float *a=(float *) calloc(8,sizeof(float)); 
    for (int i=0;i<8;i++)  
	{ 
		a[i]=i; 
    } 

    IDL_MEMINT dim[]={2,4}; 
    IDL_ImportNamedArray("namea",2,dim,IDL_TYP_FLOAT,(UCHAR *)a,free_cb,0); 
	IDL_ExecuteStr("plot, namea"); 

	rt = IDL_Cleanup(true);
}

void CProject_PGStationDlg::OnHjDatasmooth()
{
	// TODO: Add your command handler code here

	//
	char aerosolFile[256] = "c:\\temp\\aodScatter.tif";

	stGeoInfo geoinfo;
	GetGeoInformation(aerosolFile, geoinfo);
	
	float* pBuffer;
	int ht,wd;
	ReadGeoFileFloat(aerosolFile, 1, &pBuffer, ht, wd );
   

	/*
	int    winSize = 3;
	double maxAerosol = 1.0;

	//
	float* pOut = (float*)malloc( ht*wd*sizeof(float) );
	memset(pOut, 0, ht*wd*sizeof(float));

	for(int j=1; j<ht-1; j++)
		for(int i=1; i<wd-1; i++ )
		{
			if( pBuffer[j*wd+i] ==0 )
				continue;

			//collect neighbor values
			vector<double> neibors;
			for(int m=-winSize/2; m<=winSize/2; m++)
				for(int n=-winSize/2; n<=winSize/2; n++)
				{
					if( pBuffer[(j+m)*wd+(i+n)]>0 )
						neibors.push_back(pBuffer[(j+m)*wd+(i+n)]);
				}

			if( neibors.size()==1 )
				continue;

			if( neibors.size()==2 )
			{
				pOut[j*wd+i] = ( neibors[0]+neibors[1] )*0.5;
				continue;
			}

			//sort
			sort(neibors.begin(), neibors.end());
			double mean = 0;
			for(int k=1; k<neibors.size()-1; k++)
			{
				mean += neibors[k];
			}
			mean /= neibors.size()-2;
			pOut[j*wd+i] = mean;
		}

	GdalWriteFloat("c:\\temp\\aodSmooth.tif", pOut, ht, wd, geoinfo);
    free(pOut);
	*/

	DataSmoothMedian(pBuffer, ht, wd);
   

	printf("Finished ! \n");

	free(pBuffer);
	
}


void CProject_PGStationDlg::OnHjSimulatesnr()
{
	// TODO: Add your command handler code here
	printf("HJ SNR simulation ... \n");
    

}

void CProject_PGStationDlg::OnSnrEstimate()
{
	// TODO: Add your command handler code here

	char file[256] = "c:\\temp\\Data\\HJ\\2013\\09\\26\\out-band1.jpg";
    
	int nStep = 5;

	IplImage* pImage = cvLoadImage(file, 0);
	int ht = pImage->height;
	int wd = pImage->width;
	int scanwd = pImage->widthStep;

	IplImage* pEdge = cvCloneImage(pImage);
	cvSmooth(pEdge, pEdge);
	cvCanny(pEdge, pEdge, 6, 24);
	cvSaveImage("c:\\temp\\edge.jpg", pEdge);


	/*
	stGeoInfo geoinfo;
	GetGeoInformation(file, geoinfo);
	ht = geoinfo.ht;
	wd = geoinfo.wd;
	*/

	int sht = ht/nStep;
	int swd = wd/nStep;
	
	vector<double> vecAverage;
	vector<double> vecStd;

	for(int j=0; j<sht; j++)
		for(int i=0; i<swd; i++)
		{
			printf(".");

			int l = i*nStep;
			int r = (i+1)*nStep;
			int t = j*nStep;
			int b = (j+1)*nStep;

			//determine if this area contains too many edges
			int nEdgePixelNum = 0;
			for(int m=t; m<b; m++)
				for(int n=l; n<r; n++)
				{

				}            
		
			//calculate the average
			double sum = 0;
			double nCount = 0;
			for(int m=t; m<b; m++)
				for(int n=l; n<r; n++)
				{
					sum += (unsigned char)( pImage->imageData[m*scanwd+n] );
					nCount++;
				}
			double average = sum / ( nCount );
			vecAverage.push_back(average);

			//calculate the std
			double std = 0;
			for(int m=t; m<b; m++)
				for(int n=l; n<r; n++)
				{
					double dif = (unsigned char)( pImage->imageData[m*scanwd+n] ) - average;
					std += dif*dif;
				}
			std = sqrt( std/(nCount-1) );
			vecStd.push_back(std);			
		}

		//calculate the whole average and std
		double wholeAverage = 0;
		double wholeStd = 0;
		for(int i=0; i<vecAverage.size(); i++)
		{
			wholeAverage += vecAverage[i];
			wholeStd += vecStd[i];
		}
		wholeAverage /= vecAverage.size();
		wholeStd /= vecAverage.size();

		printf(" %lf %lf  SNR: %lf \n", wholeAverage, wholeStd, wholeAverage/wholeStd );
}

void CProject_PGStationDlg::OnHjInvertsingleptsat()
{
	// TODO: Add your command handler code here
	
	//loading LUT data
	char lutDir[256] = "c:\\temp\\Data\\LutTypeHJ";
	vector<CAtmosphereCorrectionBase*> vecCorrectBlue;
	vector<CAtmosphereCorrectionBase*> vecCorrectRed;

	int aerosolTypeNumber = 1;
	LoadLutsForDDV(lutDir, aerosolTypeNumber, vecCorrectBlue, vecCorrectRed);

	//from radiance to reflectance
	//wavelength for HJ bands (um)
	double blueWaveLen = 0.475;
	double redWaveLen  = 0.660;
	double irWaveLen   = 0.830;

	double fluxBlue = GetFlux(blueWaveLen);
	double fluxRed  = GetFlux(redWaveLen);
	double fluxIr   = GetFlux(irWaveLen);


	/*
	//2011 calibration 
	double gain[4][4]={ 
		0.7696,	0.7815,	1.0914,	1.0281,
		0.7435,	0.7379,	1.0899, 1.0852,
		0.7060,	0.6960,	1.0082,	1.0068,
		0.8042,	0.7822,	1.0556,	0.9237
		};
	double offset[4][4] = {
		7.3250,	6.0737,	3.6123,	1.9028,
		4.6344,	4.0982,	3.7360,	0.7385,
		3.0089,	4.4487,	3.2144,	2.5609,
		2.2219,	4.0683,	5.2537,	6.3497
		};
	*/

	//2012 calibration 
	double gain[4][4]={
			0.7069,	0.7497,	1.0673,	1.0429,
			0.7257,	0.7291,	1.0464,	1.0519,
			0.6697,	0.7118,	1.0555,	1.1042,
			0.7587,	0.7629,	1.0245,	1.0146
			};

	double offset[4][4] = {
			7.3250,	6.0737,	3.6123,	1.9028,	
			4.6344,	4.0982,	3.7360,	0.7385,		
			3.0089,	4.4487,	3.2144,	2.5609,		
			2.2219,	4.0683,	5.2537,	6.3497
			};

	//batch processing
	//char datapath[256] = "c:\\temp\\Data\\HJ\\2012\\09";
	//char datapath[256] = "c:\\temp\\Data\\HJ\\2013\\09\\26";
	char datapath[256] = "c:\\temp\\Data\\HJ\\2013\\07\\24";

	//locate the XML files
	char** filenames = NULL;
	int n,nfile;
	n = nfile = 0;
    GetDirFileName(filenames, datapath, &n, &nfile, "XML", 0);
	filenames = f2c(nfile, 256);
	GetDirFileName(filenames, datapath, &n, &nfile, "XML", 1);
    

	for(int i=0; i<nfile; i++)
	{
		char xmlFile[256];
		strcpy(xmlFile, filenames[i]);

		//separate the image path from the XML file
		char spath[256];
		strcpy(spath, xmlFile);
		char* pos = strrchr(spath, '\\');
		int index = pos-spath;
		strcpy( spath+index, "\0");
		printf("Image path: %s \n", spath);

		char fileTitle[256];
		strcpy(fileTitle, xmlFile);
		strcpy(fileTitle+strlen(fileTitle)-4, "\0");

		//load HJ XML data
		HJDATA hjdata;
		GetHJFilepaths(xmlFile, hjdata);
		ReadXMLFile(hjdata);
		double sunZenith = hjdata.solar_zenith*DPI;
		double sazimuth = hjdata.solar_azimuth;

		/*
		//using calibration parameters from the http://www.secmep.cn/secPortal/portal/index.faces
        for(int ki=0; ki<4; ki++)
		{
			hjdata.m_gain[ki]   = gain[hjdata.sat_num][ki];
			hjdata.m_offset[ki] = offset[hjdata.sat_num][ki];
		}
		*/

		//load angle data
		char angleFile[256]; 
		sprintf(angleFile, "%s-SatAngle.txt", fileTitle);

		CReadAngleBase* pReadAngle = new CReadHJAngle();
		pReadAngle->Read(angleFile);

		//loading images
		char sband1[256];
		char sband2[256];
		char sband3[256];
		char sband4[256];
		sprintf(sband1, "%s-1.tif", fileTitle); //blue
		sprintf(sband2, "%s-2.tif", fileTitle); //green
		sprintf(sband3, "%s-3.tif", fileTitle); //red
		sprintf(sband4, "%s-4.tif", fileTitle); //ir

		unsigned char* pBlue = NULL;
		unsigned char* pRed = NULL;
		unsigned char* pIR = NULL;
		int ht,wd;
		
		ReadGeoFileByte(sband1, 1, &pBlue, ht, wd);
		ReadGeoFileByte(sband3, 1, &pRed,  ht, wd);
		//ReadGeoFileByte(sband4, 1, &pIR,   ht, wd);

		//if( pIR == NULL ) printf("malloc failed ! \n");

		stGeoInfo oriGeoInfo;
		GetGeoInformation(sband1, oriGeoInfo);

		//point position
		double lat,lon;
		double grdX,grdY;
		//loading point.txt file
		char ptfile[256];
		sprintf(ptfile, "%s\\point.txt", spath);
		FILE* fp = fopen(ptfile, "r");
		if(fp!=NULL)
		{
			fscanf(fp, "%lf, %lf", &grdX, &grdY);
			fclose(fp);
		}

		//calculate the image coordinate according to the geo-infomationOnHjInvertsingleptsat
		int col = (grdX - oriGeoInfo.left) / oriGeoInfo.dx;
		int row = (grdY - oriGeoInfo.top)  / oriGeoInfo.dy;
		printf("row: %d col: %d  \n", row, col);
		double sza = hjdata.solar_zenith;

		//
		double vza,vazimuth;
		printf(" Calculate one bye one... \n");
		int winSize = 10;
		for(int m=-winSize/2; m<=winSize/2; m++)
			for(int n=-winSize/2; n<=winSize/2; n++)
			{			
				pReadAngle->GetViewGeo(row+m, col+n, vza, vazimuth);
				double relativeAzimuth = fabs(vazimuth-sazimuth);
				if(relativeAzimuth>180) relativeAzimuth = 360 - relativeAzimuth;

				int index = (row+m)*wd + (col+n);
				double refBlue = pBlue[index] / hjdata.m_gain[0] + hjdata.m_offset[0]; //band 1
				double refRed  = pRed[index]  / hjdata.m_gain[2] + hjdata.m_offset[2]; //band 3
				refBlue = (refBlue*PI)/(fluxBlue*cos(sunZenith));
				refRed  = (refRed*PI)/(fluxRed*cos(sunZenith));

				//invert
				double invertAOD = DTSinglePtInvert(vecCorrectBlue, vecCorrectRed, sza, vza, relativeAzimuth, refBlue, refRed, 0.5);
				printf("%lf ", invertAOD);
			}

		//printf(" sza: %lf  vza: %lf  azi: %lf \n", sza, vza, relativeAzimuth);
        
		//first get the average and then invert
		printf("\n Average invert .... \n");
		double averageRed = 0;
		double averageBlue = 0;
		int nIndex = 0;
		for(int m=-winSize/2; m<=winSize/2; m++)
			for(int n=-winSize/2; n<=winSize/2; n++)
			{
				int index = (row+m)*wd + (col+n);
				double refBlue = pBlue[index] / hjdata.m_gain[0] + hjdata.m_offset[0]; //band 1
				double refRed  = pRed[index]  / hjdata.m_gain[2] + hjdata.m_offset[2]; //band 3
				refBlue = (refBlue*PI)/(fluxBlue*cos(sunZenith));
				refRed  = (refRed*PI)/(fluxRed*cos(sunZenith));

				averageBlue += refBlue;
				averageRed += refRed;

				nIndex ++;
			}

		averageBlue /= (double)(nIndex);
		averageRed /= (double)(nIndex);

		//double vza,vazimuth;
		pReadAngle->GetViewGeo(row, col, vza, vazimuth);
		double relativeAzimuth = fabs(vazimuth-sazimuth);
		if(relativeAzimuth>180) relativeAzimuth = 360 - relativeAzimuth;

		printf("sza: %lf vza: %lf azi: %lf \n", sza, vza, relativeAzimuth);
		printf("toa blue: %lf  rec:\\temp %lf  \n", averageBlue, averageRed);
         
		double invertAOD = DTSinglePtInvert(vecCorrectBlue, vecCorrectRed, sza, vza, relativeAzimuth, averageBlue, averageRed, 0.5);
		//printf("%lf ", invertAOD);
		printf("correction: %lf %lf \n", averageBlue, averageRed);

		//calculate the angstrom
		double blueExt = vecCorrectBlue[0]->GetExtinction();
		double redExt = vecCorrectRed[0]->GetExtinction();
		double angstrom = CalcAngstrom(0.475, 0.66, blueExt, redExt );

		double redAod = redExt / blueExt * invertAOD;

		double aod55 = CalculateAODUsingAngstrom(angstrom, 0.55, 0.66, redAod);

		printf("475nm:%lf  550nm:%lf  660nm:%lf \n", invertAOD, aod55, redAod);

		FILE* fpOut = fopen("c:\\temp\\HJAod.txt", "a+");
		fprintf(fpOut, "%s 475nm:%lf  550nm:%lf  660nm:%lf \n", fileTitle, invertAOD, aod55, redAod);
		fclose(fpOut);

		free(pBlue);
		free(pRed);
		free(pIR);

		delete pReadAngle;
	}

	printf("Finished! \n");

}

void CAboutDlg::OnAngstromBatch()
{
	// TODO: Add your command handler code here
}

void CProject_PGStationDlg::OnHjUi()
{
	// TODO: Add your command handler code here
	
	//loading LUTs
	char lutDir[256] = "c:\\temp\\Data\\LutTypeHJ";
	vector<CAtmosphereCorrectionBase*> vecCorrectBlue;
	vector<CAtmosphereCorrectionBase*> vecCorrectRed;
	int aerosolTypeNumber = 1;
	LoadLutsForDDV(lutDir, aerosolTypeNumber, vecCorrectBlue, vecCorrectRed);

	//loading data and processing
	CDlgHJAC dlgAC;
	CString ccdPath;
	if(dlgAC.DoModal()==IDOK)
	{
		ccdPath = dlgAC.m_strCCDPath;

		char path[256];
		strcpy(path, ccdPath);

		//locate the XML files
		char** filenames = NULL;
		int n,nfile;
		n = nfile = 0;
		GetDirFileName(filenames, path, &n, &nfile, "XML", 0);
		filenames = f2c(nfile, 256);
		GetDirFileName(filenames, path, &n, &nfile, "XML", 1);

		HJAerosolInvertDDV(vecCorrectBlue, vecCorrectRed, filenames[0]);
		//HJAerosolInvertDDVHigh(vecCorrectBlue, vecCorrectRed, filenames[0]);
		
		printf("Finished ! \n");

	}
}

void CProject_PGStationDlg::OnMosaicLaplacian()
{
	// TODO: Add your command handler code here

	char inputPath[256] = "F:\\Data\\Dodging\\rgb";
	char outPath[256] = "c:\\temp\\blendmosaic.tif";

	//search tif filesw
	char** filesPath = f2c(1000, 256);
	int n,nfile;
	n = nfile = 0;
	GetDirFileName(filesPath, inputPath, &n, &nfile, "tif", 0);
	GetDirFileName(filesPath, inputPath, &n, &nfile, "tif", 1);

	//blend
	//GeoMosaicWithBalance(filesPath, 3, outPath);
	CBlenderBase* pBlend = new CMultiBandBlender();
	pBlend->GeoBlend(filesPath, nfile, outPath);

	printf("Finished ! \n");
}

void CProject_PGStationDlg::OnMosaicBlendbyhand()
{
	// TODO: Add your command handler code here
	
	printf("Mosaic by hand... \n");


	char* imgfile1  = "F:\\data\\mosaic\\lena2.jpg";
	char* imgfile2  = "F:\\data\\mosaic\\IMG_0105.jpg";
	

	/*
	//read images
	vector<Mat>   images;	
	images[0] = imread(imgfile1); 
	images[1] = imread(imgfile2); 
    
	CBlenderBase* pBlend = new CMultiBandBlender();
	pBlend->GeoBlend(images, outFilePath);
	*/
	

	double dInput;
	CDlgInput dlgInput;
	dlgInput.m_para1Name = "Feature Number: ";
	if(dlgInput.DoModal() == IDOK)
	{
		dInput = dlgInput.m_inputPara1;
	}

	double blendWeight = max(1,dInput);

	CBlenderBase* pBlend = new CMultiBandBlender();
	pBlend->Blend(NULL, 0, blendWeight, "c:\\temp\\blend.jpg");

	IplImage* pImage = cvLoadImage("c:\\temp\\blend.jpg");
	cvNamedWindow("Blend");
	cvShowImage("Blend", pImage);
	cvReleaseImage(&pImage);

}

void CProject_PGStationDlg::OnHjAerosolinvertddvhigh()
{
	// TODO: Add your command handler code here
	//loading LUT data
	char lutDir[256] = "c:\\temp\\Data\\LutTypeHJ";
	vector<CAtmosphereCorrectionBase*> vecCorrectBlue;
	vector<CAtmosphereCorrectionBase*> vecCorrectGreen;
	vector<CAtmosphereCorrectionBase*> vecCorrectRed;

	//int nTitle[3]={0,10,25};//index for lut of different reflectance

	int aerosolTypeNumber = 1;
	LoadLutsRGB(lutDir, aerosolTypeNumber, vecCorrectBlue, vecCorrectGreen, vecCorrectRed);

	//wavelength for HJ bands (um)
	double blueWaveLen = 0.475;
	double greenWaveLen = 0.55;
	double redWaveLen  = 0.660;
	double irWaveLen   = 0.830;

	double fluxBlue  = GetFlux(blueWaveLen);
	double fluxGreen = GetFlux(greenWaveLen);
	double fluxRed   = GetFlux(redWaveLen);
	double fluxIr    = GetFlux(irWaveLen);

		
	char spath[256] = "c:\\temp\\Data\\HJ\\2010\\08\\16";
	char xmlFileTitle[256] = "HJ1A-CCD2-1-68-20100816-L20000375929.XML";           //2010.08.16
	
	//char spath[256] = "c:\\temp\\Data\\HJ\\2010\\09\\30";
	//char xmlFileTitle[256] = "HJ1B-CCD2-2-67-20100930-L20000401266.XML";         //2010.09.30
	
	//char spath[256] = "c:\\temp\\Data\\HJ\\2010\\9\\12";	
	//char xmlFileTitle[256] = "HJ1A-CCD2-456-68-20100912-L20000391060.XML";       //2010.9.12

	//char spath[256] = "c:\\temp\\Data\\HJ\\2010\\10\\29";
	//char xmlFileTitle[256] = "HJ1A-CCD2-457-68-20101029-L20000417622.XML";       //2010.10.29

	//char xmlFileTitle[256] = "HJ1B-CCD2-2-68-20131004-L20001064410.XML";          //2013.10.04
	
	//char spath[256] = "c:\\temp\\Data\\HJ\\2013\\10\\17";
	//char xmlFileTitle[256] = "HJ1A-CCD2-456-68-20131017-L20001068649.XML";     //2013.10.17
	
	//char spath[256] = "c:\\temp\\Data\\HJ\\2013\\10\\04";
	//char xmlFileTitle[256] = "HJ1B-CCD2-2-68-20131004-L20001064410.XML";       //2013.10.04

	//char spath[256] = "c:\\temp\\Data\\HJ\\2013\\09\\28";	
	//char xmlFileTitle[256] = "HJ1A-CCD2-1-68-20130928-L20001059100.XML";       //2013.09.28

	//char spath[256] = "c:\\temp\\Data\\HJ\\2013\\07\\24";	
	//char xmlFileTitle[256] = "HJ1B-CCD1-4-64-20130724-L20001027056.XML";       //2013.07.24


	char fileTitle[256];
	strcpy(fileTitle, xmlFileTitle);
	strcpy(fileTitle+strlen(fileTitle)-4, "\0");

	//load HJ XML data
	char xmlFile[256];
	sprintf(xmlFile, "%s\\%s", spath, xmlFileTitle);

	HJDATA hjdata;
	GetHJFilepaths(xmlFile, hjdata);
	ReadXMLFile(hjdata);
	double sunZenith = hjdata.solar_zenith*DPI;
	double sazimuth = hjdata.solar_azimuth;

	//load angle data
	char angleFile[256]; 
	sprintf(angleFile, "%s\\%s-SatAngle.txt", spath, fileTitle);

	CReadAngleBase* pReadAngle = new CReadHJAngle();
	pReadAngle->Read(angleFile);

	char oriTiffFile[256];
	sprintf(oriTiffFile, "%s\\%s-1.TIF", spath, fileTitle);

	stGeoInfo oriGeoInfo;
	GetGeoInformation(oriTiffFile, oriGeoInfo);

	//loading images
	char sband1[256];
	char sband2[256];
	char sband3[256];
	char sband4[256];
	/*
	sprintf(sband1, "%s\\%s", spath, "band1-clip-envi.tif");
	sprintf(sband2, "%s\\%s", spath, "band2-clip-envi.tif");
	sprintf(sband3, "%s\\%s", spath, "band3-clip-envi.tif");
	sprintf(sband4, "%s\\%s", spath, "band4-clip-envi.tif");
	*/
	sprintf(sband1, "%s\\%s", spath, "jjt\\band1.tif");
	sprintf(sband2, "%s\\%s", spath, "jjt\\band2.tif");
	sprintf(sband3, "%s\\%s", spath, "jjt\\band3.tif");
	sprintf(sband4, "%s\\%s", spath, "jjt\\band4.tif");

	//read geoinfo header for clipped image
	stGeoInfo subGeoInfo;
	GetGeoInformation(sband1, subGeoInfo);

	unsigned char* pband1 = NULL;
	unsigned char* pband2 = NULL;
	unsigned char* pband3 = NULL;
	unsigned char* pband4 = NULL;
	int ht,wd;
	ReadGeoFileByte(sband1, 1, &pband1, ht, wd);
	ReadGeoFileByte(sband2, 1, &pband2, ht, wd);
	ReadGeoFileByte(sband3, 1, &pband3, ht, wd);
	ReadGeoFileByte(sband4, 1, &pband4, ht, wd);
	
	/*
	//calculate the NDVI
	int* pNDVI = (int*)malloc(ht*wd*sizeof(int));
	memset(pNDVI, -1, ht*wd*sizeof(int));

	//unsigned char* pNDVIDisp = (int*)malloc(ht*wd*sizeof(unsigned char));
	//memset(pNDVIDisp, 0, ht*wd*sizeof(unsigned char));

	//calculate the NDVI
	for(int j=0; j<ht; j++)
		for(int i=0; i<wd; i++)
		{
			//DN value
			unsigned char r,g,b,ir;
			b  = pband1[j*wd+i];
			r  = pband3[j*wd+i];
			ir = pband4[j*wd+i];

			//convert from DN to reflectance
			if(b>0 || r>0 || ir>0)
			{
				//from DN to radiance
				double refBlue = pband1[j*wd+i] / hjdata.m_gain[0] + hjdata.m_offset[0]; //band 1
				double refRed  = pband3[j*wd+i] / hjdata.m_gain[2] + hjdata.m_offset[2]; //band 3
				double refIR   = pband4[j*wd+i] / hjdata.m_gain[3] + hjdata.m_offset[3]; //band 4				

				//from radiance to reflectance
				refBlue = (refBlue*PI)/(fluxBlue*cos(sunZenith));
				refRed  = (refRed*PI)/(fluxRed*cos(sunZenith));
				refIR   = (refIR*PI)/(fluxIr*cos(sunZenith));

				double ndvi = (refIR-refRed) / (refIR+refRed);
				if(ndvi<0) ndvi = 0;
				pNDVI[j*wd+i] = ndvi*100;	
				//if(ndvi>49)	pNDVIDisp[j*wd+i] = 255;
			}			
		}	
		SaveBmp("c:\\temp\\ndvi.bmp", pNDVI, ht, wd);
		//GdalWriteByte("c:\\temp\\ndvi.tif", pNDVIDisp, ht, wd, subGeoInfo);
		*/

		//load NDVI of clear day
		//char ndviFile[256] = "c:\\temp\\Data\\HJ\\2010\\09\\ndvi.tif";
		char ndviFile[256] = "c:\\temp\\Data\\MODIS\\500m\\2010273\\jjt\\modis-9.tif";

		stGeoInfo benchGeoInfo;
		GetGeoInformation(ndviFile, benchGeoInfo);
		unsigned char* pClearNDVI;
		int ndviHt,ndviWd;
		ReadGeoFileByte(ndviFile, 1, &pClearNDVI, ndviHt, ndviWd);	   

		//aerosol inversion based on DDV
		int nStep = 5; //the resolution is down-sampled 
		int nNeiborRadius = 10; //the neiborhood radis
		int desHt = ht/nStep-1;
		int desWd = wd/nStep-1;

		unsigned char* pAerosolType = (unsigned char*)malloc(desHt*desWd);
		memset(pAerosolType, 0, desHt*desWd);

		float* pAodScatter = (float*)malloc(desHt*desWd*sizeof(float));
		memset(pAodScatter, 0, desHt*desWd*sizeof(float));

		float* pAod = (float*)malloc(desHt*desWd*sizeof(float));
		memset(pAod, 0, desHt*desWd*sizeof(float));
		for(int i=0; i<(desHt*desWd); i++)
			pAod[i] = -1;	

		int ndviThreshold = 45;   // clear
		//int ndviThreshold = 25; // polluted
		for(int j=nNeiborRadius; j<desHt-nNeiborRadius; j++)
		{
			for(int i=nNeiborRadius; i<desWd-nNeiborRadius; i++)
			{
				int y = j*nStep;
				int x = i*nStep;

				//filter out the background pixel
				int blueDN = pband1[y*wd+x];
				if(blueDN<1) continue;
                
				//forground pixel
				pAod[j*desWd+i]=0;

				//label the pixel of the image
				//if(pNDVI[y*wd+x]>=0) pAod[j*desWd+i]=0;

				//count the number of DDV pixel
				int nHit = 0;
				int nCloudNum = 0;
				double maxNDVI = 0.0;
				double surfBlue,surfRed;
				double sumRefBlue = 0;
				double sumRefRed  = 0;
				for(int m=-nNeiborRadius/2; m<nNeiborRadius/2; m++)
					for(int n=-nNeiborRadius/2; n<nNeiborRadius/2; n++)
					{
						int index = (y+m)*wd + (x+n); 

						double tx = subGeoInfo.left + (x+n)*subGeoInfo.dx;
						double ty = subGeoInfo.top + (y+m)*subGeoInfo.dy;
						int row = (ty-benchGeoInfo.top)/benchGeoInfo.dy;
						int col = (tx-benchGeoInfo.left)/benchGeoInfo.dx;

						double refBlue = pband1[index] / hjdata.m_gain[0] + hjdata.m_offset[0]; //band 1
						double refRed  = pband3[index] / hjdata.m_gain[2] + hjdata.m_offset[2]; //band 3
						//double refIR   = pband4[index] / hjdata.m_gain[3] + hjdata.m_offset[3]; //band 4				

						//from radiance to reflectance
						refBlue = (refBlue*PI)/(fluxBlue*cos(sunZenith));
						refRed  = (refRed*PI)/(fluxRed*cos(sunZenith));
						//refIR   = (refIR*PI)/(fluxIr*cos(sunZenith));

						if( pClearNDVI[row*ndviWd+col]>=ndviThreshold && refBlue<0.25 && refRed<0.25 ) //pNDVI[index]>0
						{
							//if( pNDVI[index]>maxNDVI )
							if( pClearNDVI[row*ndviWd+col] > maxNDVI )
							{
								//maxNDVI = pNDVI[index];
								maxNDVI = pClearNDVI[row*ndviWd+col];
								surfBlue = refBlue;
								surfRed  = refRed;
							}

							sumRefBlue += refBlue;
							sumRefRed  += refRed;

							nHit ++;
						}

						//cloud recognition
						if( refBlue>0.3 || refRed>0.3  )
							nCloudNum ++;
					}

				if(nCloudNum>0)
					continue;
			
				if( nHit<(nStep*nStep*0.1) )
					continue;

				sumRefBlue /= nHit;
				sumRefRed  /= nHit;
												
				//invert	
				//convert to ground coordinate
				double gy = subGeoInfo.top  + y*subGeoInfo.dy;
				double gx = subGeoInfo.left + x*subGeoInfo.dx;
				int row =  (oriGeoInfo.top - gy)  / fabs(oriGeoInfo.dy);
				int col =  (gx - oriGeoInfo.left) / fabs(oriGeoInfo.dx);
				double vza, vazimuth;
				pReadAngle->GetViewGeo(row, col, vza, vazimuth);
				if(vazimuth<0) vazimuth += 360;
				double relativeAzimuth = fabs(vazimuth-sazimuth);
				if(relativeAzimuth>180) relativeAzimuth -= 180;

				//aerosol inversion
				double minErr = 1000000;
				double invertAod = 0.01;
				int    nInvertType = 0;
				for(int k=1; k<100; k++) //for blue channel
				{
					double aod = k*0.025;
					double refError = 100000000;					
					int    nType = 0;
					//for(int m=0; m<aerosolTypeNumber; m++)
					int m = 0;
					{
						//get the extinction parameters
						double extRed  = vecCorrectRed[m]->GetExtinction();     
						double extBlue = vecCorrectBlue[m]->GetExtinction();    

						double redAod = extRed/extBlue*aod;
						double correctBlue = vecCorrectBlue[m]->CalculateGroundReflectance(hjdata.solar_zenith, 
							vza, relativeAzimuth, aod, surfBlue);
						double correctRed  = vecCorrectRed[m]->CalculateGroundReflectance(hjdata.solar_zenith, 
							vza, relativeAzimuth, redAod, surfRed);

						if(correctBlue>correctRed)
							continue;

						//very small reflectance is rare
						//if(correctBlue<0.01 )
						//	continue;

						if( correctBlue<0 || correctRed<0 )
							continue;

						//double err = correctBlue*2 - correctRed;
						double ratio = correctBlue/correctRed;
						double err  = fabs(ratio - 0.5);
						if(err<refError)
						{
							refError = err;
							nType = m;
						}
					}

					if(refError<minErr)
					{
						minErr = refError;
						invertAod = aod;
						nInvertType = nType+1;
					}
				}

				//printf("%lf ", invertAod);
				pAod[j*desWd+i]         = invertAod;
				pAodScatter[j*desWd+i]  = invertAod;
				pAerosolType[j*desWd+i] = nInvertType;
			}
			printf(" %d \n", j);		
		}
			
	//free(pNDVI);

	DataSmoothMedian(pAod, desHt, desWd);
	DataSmoothMedian(pAodScatter, desHt, desWd);

	stGeoInfo aodGeo = subGeoInfo;
	aodGeo.dx *= nStep;
	aodGeo.dy *= nStep;
	GdalWriteFloat("c:\\temp\\aod.tif", pAod, desHt, desWd, aodGeo);   //for interpolation
	GdalWriteFloat("c:\\temp\\aodScatter.tif", pAodScatter, desHt, desWd, aodGeo); //for display
	GdalWriteByte("c:\\temp\\aodType.tif", pAerosolType, desHt, desWd, aodGeo);
	
	
	//interpolation and then smooth again
	AodInterpolation(pAod, desHt, desWd);
	DataSmoothMedian(pAod, desHt, desWd);
	GdalWriteFloat("c:\\temp\\aod-interpolation.tif", pAod, desHt, desWd, aodGeo);   //for interpolation
	


	//downsample
	int  nSmallHt = ht/nStep/nStep;
	int  nSmallWd = wd/nStep/nStep;
	float* pAodSmall = (float*)malloc(nSmallHt*nSmallWd*sizeof(float));
	DownSample(pAod, desHt, desWd, pAodSmall, nSmallHt, nSmallWd);

	free(pAod);
	free(pAodScatter);
	free(pAerosolType);

	stGeoInfo aodGeoSmall = subGeoInfo;
	aodGeoSmall.dx *= nStep*nStep;
	aodGeoSmall.dy *= nStep*nStep;
	GdalWriteFloat("c:\\temp\\smallaod.tif", pAodSmall, nSmallHt, nSmallWd, aodGeoSmall);
		
	double ratioX = (double)(nSmallWd) / (double)(wd);
	double ratioY = (double)(nSmallHt) / (double)(ht);
	
	/*
	//saving reflectance	
	int* cBlue  = (int*)malloc(sizeof(int)*ht*wd);
	int* cGreen = (int*)malloc(sizeof(int)*ht*wd);
	int* cRed   = (int*)malloc(sizeof(int)*ht*wd);
	memset(cBlue,  0, sizeof(int)*ht*wd);
	memset(cGreen, 0, sizeof(int)*ht*wd);
	memset(cRed,   0, sizeof(int)*ht*wd);

	//atmospheric correction
	for(int j=0; j<ht; j++)
	{
		for(int i=0; i<wd; i++)
		{
			double sx = min(nSmallWd-1, (double)(i) * ratioX );
			double sy = min(nSmallHt-1, (double)(j) * ratioY );
            
			//using minimal value or average 
			//double aod = pAod[sy*desWd+sx];
			//int l = max(0, i-2);
			//int r = min( ;

			double aod = BilinearAOD(sx, sy, pAodSmall, nSmallHt, nSmallWd);
           	aod *= 0.9;
            
			double blueExt  =  vecCorrectBlue[0]->GetExtinction();
			double greenExt =  vecCorrectGreen[0]->GetExtinction();
			double redExt   =  vecCorrectRed[0]->GetExtinction();
						
			double aodRed = aod*(redExt/blueExt);
			double aodGreen = aod*(greenExt/blueExt);
            
			//DN value
			unsigned char r,g,b,ir;
			b  = pband1[j*wd+i];
			g  = pband2[j*wd+i];
			r  = pband3[j*wd+i];
			ir = pband4[j*wd+i];

			//convert from DN to reflectance
			if(b>0 && r>0 && ir>0 && aod>0)
			{
				//from DN to radiance
				double refBlue  = pband1[j*wd+i] / hjdata.m_gain[0] + hjdata.m_offset[0]; //band 1
				double refGreen = pband2[j*wd+i] / hjdata.m_gain[1] + hjdata.m_offset[1]; //band 1
				double refRed   = pband3[j*wd+i] / hjdata.m_gain[2] + hjdata.m_offset[2]; //band 3
				double refIR    = pband4[j*wd+i] / hjdata.m_gain[3] + hjdata.m_offset[3]; //band 4				

				//from radiance to reflectance
				refBlue  = (refBlue*PI)/(fluxBlue*cos(sunZenith));
				refGreen = (refGreen*PI)/(fluxGreen*cos(sunZenith));
				refRed   = (refRed*PI)/(fluxRed*cos(sunZenith));
				refIR    = (refIR*PI)/(fluxIr*cos(sunZenith));
				double ndvi = (refIR-refRed) / (refIR+refRed);

				//convert to ground coordinate
				double gy = subGeoInfo.top  + j*subGeoInfo.dy;
				double gx = subGeoInfo.left + i*subGeoInfo.dx;
				int row =  (oriGeoInfo.top - gy)  / fabs(oriGeoInfo.dy);
				int col =  (gx - oriGeoInfo.left) / fabs(oriGeoInfo.dx);
				double vza, vazimuth;
				pReadAngle->GetViewGeo(row, col, vza, vazimuth);
				if(vazimuth<0) vazimuth += 360;
				double relativeAzimuth = fabs(vazimuth-sazimuth);
				if(relativeAzimuth>180) relativeAzimuth -= 180;
				

				double crBlue  = vecCorrectBlue[0]->Correct(hjdata.solar_zenith, vza, relativeAzimuth, aod, refBlue, 0);
				double crGreen = vecCorrectGreen[0]->Correct(hjdata.solar_zenith, vza, relativeAzimuth, aodGreen, refGreen, 0);
				double crRed   = vecCorrectRed[0]->Correct(hjdata.solar_zenith, vza, relativeAzimuth, aodRed, refRed, 0);
				
				if(crBlue<0)
					crBlue   = vecCorrectBlue[0]->Correct(hjdata.solar_zenith, vza, relativeAzimuth, 0, refBlue, 0);
				if(crGreen<0)
					crGreen  = vecCorrectGreen[0]->Correct(hjdata.solar_zenith, vza, relativeAzimuth, 0, refGreen, 0);
				if(crRed<0)
					crRed    = vecCorrectRed[0]->Correct(hjdata.solar_zenith, vza, relativeAzimuth, 0, refRed, 0);

				//crBlue = max(0.01, crBlue);
				//crGreen = max(0.01, crGreen);
				//crRed = max(0.01, crRed);

				if(crBlue>0 && crRed>0 && crGreen>0)
				{
					cBlue[j*wd+i] = crBlue*10000;
					double crIntensity =   min(255, crBlue*fluxBlue*cos(sunZenith)); // / PI ;			
					pband1[j*wd+i] = crIntensity;

					cGreen[j*wd+i] = crGreen*10000;
					crIntensity =   min(255, crGreen*fluxGreen*cos(sunZenith)); // / PI ;			
					pband2[j*wd+i] = crIntensity;

					cRed[j*wd+i] = crRed*10000;
					crIntensity =   min(255, crRed*fluxRed*cos(sunZenith)); // / PI ;			
					pband3[j*wd+i] = crIntensity;
				}
			}			
		}	
		printf(".");
	}
	
	GdalWriteByte("c:\\temp\\blueCr-radiance.tif", pband1, ht, wd, subGeoInfo);
	GdalWriteByte("c:\\temp\\greenCr-radiance.tif", pband2, ht, wd, subGeoInfo);
	GdalWriteByte("c:\\temp\\redCr-radiance.tif", pband3, ht, wd, subGeoInfo);	
	
	GdalWriteInt("c:\\temp\\blueCr-reflectance.tif", cBlue, ht, wd, subGeoInfo);
	GdalWriteInt("c:\\temp\\greenCr-reflectance.tif", cGreen, ht, wd, subGeoInfo);
	GdalWriteInt("c:\\temp\\redCr-reflectance.tif", cRed, ht, wd, subGeoInfo);
	free(cBlue);
	free(cGreen);
	free(cRed);
	*/
		
	free(pAodSmall);

	free(pband1);
	free(pband2);
	free(pband3);
	free(pband4);

	printf("Finished! \n");
}

void CProject_PGStationDlg::OnHjDownsample()
{
	// TODO: Add your command handler code here

	//read image
	char filename[256] = "c:\\temp\\aodScatter.tif";

	stGeoInfo geoinfo;
	GetGeoInformation(filename, geoinfo);

	float* pAod = NULL;
	int ht,wd;
    
	ReadGeoFileFloat(filename, 1, &pAod, ht, wd);
	printf("%d %d \n", ht, wd);

	double ratio = 0.125;
    int dht = ht*ratio;
	int dwd = wd*ratio;
	float* pDst = (float*)malloc(dht*dwd*sizeof(float));
	DownSample(pAod, ht, wd, pDst, dht, dwd);
    
	geoinfo.dx /= ratio;
	geoinfo.dy /= ratio;
	GdalWriteFloat("c:\\temp\\downsample.tif", pDst, dht, dwd, geoinfo);

	free(pAod);
	free(pDst);

	printf("Downsample finished !");

}

void CProject_PGStationDlg::OnHjInterpolation()
{
	// TODO: Add your command handler code here

	//read image
	char filename[256] = "c:\\temp\\aod.tif";

	stGeoInfo geoinfo;
	GetGeoInformation(filename, geoinfo);

	float* pAod = NULL;
	int ht,wd;

	ReadGeoFileFloat(filename, 1, &pAod, ht, wd);
	printf("%d %d \n", ht, wd);

	AodInterpolation(pAod, ht, wd);

	DataSmoothMedian(pAod, ht, wd);

	GdalWriteFloat("c:\\temp\\aod-interpolation.tif", pAod, ht, wd, geoinfo);
   
	printf("Finished ! \n");
}

void CProject_PGStationDlg::OnAngstromBatch()
{
	// TODO: Add your command handler code here

	//read data for AERONET file and calculate the AOD of 550nm based on Angstrom 
	char filename[256] = "c:\\temp\\Data\\HJ\\aeronet-1.txt";

	int row = GetFileRows(filename);

	char day[256];
	char time[256];
	char tc[256];
	char others[512];
	char line[1024];
	double juliaDay;
	double aot_1020,aot_870,aot_675,aot_440;

	FILE* fpOut = fopen("c:\\temp\\aeronet_550nm.txt", "w");

	FILE* fp = fopen(filename, "r");
	for(int i=0; i<row; i++)
	{
		fgets(line, 1024, fp);

		sscanf(line, "%s %s %lf %s %lf %lf %lf %s %s %s %s %s %s %s %s %lf %s %s %s %s", 
			day, time, &juliaDay,
			tc, &aot_1020, &aot_870, &aot_675,
			tc, tc, tc, tc,
			tc, tc, tc, tc,
			&aot_440, tc,tc,tc,
			others);

		//double angstrom = CalcAngstrom(0.44, 1.02, aot_440, aot_1020);
		//double aod550 = CalculateAODUsingAngstrom(angstrom, 0.55, 1.02, aot_1020);


		double angstrom = CalcAngstrom(0.44, 0.67, aot_440, aot_675);
		double aod550 = CalculateAODUsingAngstrom(angstrom, 0.55, 0.67, aot_675);

		printf("%s %s %lf %lf %lf %lf  550nm: %lf \n", day, time,  aot_1020, aot_870, aot_675, aot_440, aod550);
		fprintf(fpOut, "%s %s %lf %lf %lf %lf  550nm: %lf \n", day, time,  aot_1020, aot_870, aot_675, aot_440, aod550);



	}
	
	fclose(fp);

	fclose(fpOut);
}

void CProject_PGStationDlg::OnAeronetSizedistribution()
{
	// TODO: Add your command handler code here

	//read .vol file
	//char filename[256] = "c:\\temp\\Data\\Aeronet\\130101_131231_Beijing\\130101_131231_Beijing-space.vol";
	char filename[256] = "F:\\Data\\Aeronet\\130101_131231_XiangHe\\130101_131231_XiangHe-space.vol";

	char cline[1024];
	char tempString[1024];
	char date[256];
	char time[256];

	double day;
	int hour,minute,second;
	int dd,mm,yy;

	double tc,teRadius,tr,ts;
	double fc,feRadius,fr,fs;
	double cc,ceRadius,cr,cs;

	

	FILE* fp = fopen(filename, "r");
	
	for(int i=0; i<4; i++)
	{
		fgets(cline, 1024, fp);
	}

	double fc1=0,fr1=0,fs1=0;
	double cc1=0,cr1=0,cs1=0;
	int nSum = 0;

	double fc_1=0,fr_1=0,fs_1=0;
	double cc_1=0,cr_1=0,cs_1=0;
	int n1 = 0;


	double fc_2=0,fr_2=0,fs_2=0;
	double cc_2=0,cr_2=0,cs_2=0;
	int n2 = 0;

	double fc_3=0,fr_3=0,fs_3=0;
	double cc_3=0,cr_3=0,cs_3=0;
	int n3 = 0;


	double fc_4=0,fr_4=0,fs_4=0;
	double cc_4=0,cr_4=0,cs_4=0;
	int n4 = 0;

	while(!feof(fp))
	{
		fgets(cline, 1024, fp);
		sscanf(cline, "%s %s %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %s", date, time, &day, 
			&tc,&teRadius,&tr,&ts,
			&fc,&feRadius,&fr,&fs,
			&cc,&ceRadius,&cr,&cs, 
			tempString);

		sscanf(date, "%c:\\temp%c:\\temp%d", &dd, &mm, &yy);
		sscanf(time, "%c:\\temp%c:\\temp%d", &hour, &minute, &second);
        
		
		if( hour==2 )
		{
			fc1 += fc;
			fr1 += fr;
			fs1 += fs;
			cc1 += cc;
			cr1 += cr;
			cs1 += cs;
			nSum++;
		}

		//spring
		if(mm>=3 && mm<=5)
		{

		}

		//summer
		if(mm>=6 && mm<=8)
		{

		}

		//autumn
		if(mm>=9 && mm<=11)
		{

		}
		
		//winter
		if(mm==12 || mm==1 || mm==2)
		{

		}
	}

	fclose(fp);

	fc1 /= (double)(nSum);
	fr1 /= (double)(nSum);
	fs1 /= (double)(nSum);
	cc1 /= (double)(nSum);
	cr1 /= (double)(nSum);
	cs1 /= (double)(nSum);


	FILE* fpout = fopen("c:\\temp\\beijingVolumeSize.txt", "w");
	fprintf(fpout, "%lf %lf %lf %lf %lf %lf \n", fc1, fr1, fs1, cc1, cr1, cs1);
    fclose(fpout);


	double radius[22] = {0.05,0.0656,0.08608,0.11294,
		0.14818,0.19443,0.25511,0.33472,0.43917,
		0.57623,0.75605,0.992,1.30157,1.70776,
		2.2407,2.93997,3.85745,5.06126,6.64074,
		8.71315,11.43229,15};

	stParticleDistribution pd;
	pd.mode = 2;

	pd.medianRadius[0]  = fr1;//0.162;//; //0.176 ;//= 0.219;
	pd.stdVariance[0]   = fs1; //0.5387;//;//0.517 ;//= 0.531;
	pd.concentration[0] = fc1;//0.062;//;//;0.073;//= 0.153;
	pd.medianRadius[1]  = cr1; //2.286;//;//2.768 ;//= 2.724;
	pd.stdVariance[1]   = cs1; //0.594;//;//0.623 ;//= 0.583;
	pd.concentration[1] = cc1; //0.346;//;//0.117;//= 0.131;

	double pdf[22];
	BimodalLogDistribution(pd, radius, pdf, 22);

	fp = fopen("c:\\temp\\beijingSize.txt", "a+");
	for(int i=0; i<22; i++)
	{
		fprintf(fp, "%lf ", pdf[i]);
		//fprintf(fp, "%lf %lf \n", radius[i], pdf[i]);
		//printf("%lf %lf \n", radius[i], pdf[i]);
	}
	fprintf(fp, "\n");
	fclose(fp);

	printf("Finished! \n");
}


// retrieve the A.M 10:00 data from the size distribution file

void CProject_PGStationDlg::OnAeronetSizedistributiondiscrete()
{
	// TODO: Add your command handler code here

	//read .siz file
	//char filename[256] = "F:\\Data\\Aeronet\\Beijing\\130101_131231_Beijing\\130101_131231_Beijing-space.siz";
	char filename[256] = "F:\\Data\\Aeronet\\Beijing&Xianghe\\13-beijing_xianghe.siz";

	char cline[1024];
	char tempString[1024];
	char date[256];
	char time[256];

	double day;
	int hour,minute,second;
	int dd,mm,year;

	double pf[22];
	double spf[22];	
	double nSpring, springPf[22];
	double nSummer, summerPf[22];
	double nAutumn, autumnPf[22];
	double nWinter, winterPf[22];

	nSpring = 0;
	nSummer = 0;
	nAutumn = 0;
	nWinter = 0;
	for(int i=0; i<22; i++)
	{
		spf[i] = 0;
		springPf[i] = 0;
		summerPf[i] = 0;
		autumnPf[i] = 0;
		winterPf[i] = 0;
	}

	FILE* fpSpring = fopen("c:\\temp\\SizeSpring.txt", "w");
	FILE* fpSummer = fopen("c:\\temp\\SizeSummer.txt", "w");
	FILE* fpAutumn = fopen("c:\\temp\\SizeAutumn.txt", "w");
	FILE* fpWinter = fopen("c:\\temp\\SizeWinter.txt", "w");

	FILE* fpout = fopen("c:\\temp\\Size.txt", "w");

	FILE* fp = fopen(filename, "r");

	for(int i=0; i<4; i++)
	{
		fgets(cline, 1024, fp);
	}

	int nSum = 0;
	while(!feof(fp))
	{
		fgets(cline, 1024, fp);
		sscanf(cline, "%s %s %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf ", 
			date, time, &day, 
	        &pf[0],&pf[1],&pf[2],&pf[3],&pf[4],
			&pf[5],&pf[6],&pf[7],&pf[8],&pf[9],
			&pf[10],&pf[11],&pf[12],&pf[13],&pf[14],
			&pf[15],&pf[16],&pf[17],&pf[18],&pf[19],
			&pf[20],&pf[21]);

		sscanf(time, "%c:\\temp%c:\\temp%d", &hour, &minute, &second);
		sscanf(date, "%c:\\temp%c:\\temp%d", &dd, &mm, &year);
		

		//ignore the dust type
		if( hour==2)
		{
			//all days
			for(int i=0; i<22; i++)
				fprintf(fpout, "%lf ", pf[i]);
			fprintf(fpout, "\n");
			for(int i=0; i<22; i++)
				spf[i] += pf[i];
			nSum ++;

			//spring: 3-5
			if( mm>=3 && mm<=5 )
			{
				for(int i=0; i<22; i++)
					springPf[i] += pf[i];
				nSpring ++;

				for(int i=0; i<22; i++)
					fprintf(fpSpring, "%lf ", pf[i]);
				fprintf(fpSpring, "\n");
			}

			//summer: 6-8
			if( mm>=6 && mm<=8)
			{
				for(int i=0; i<22; i++)
					summerPf[i] += pf[i];
				nSummer ++;

				for(int i=0; i<22; i++)
					fprintf(fpSummer, "%lf ", pf[i]);
				fprintf(fpSummer, "\n");
			}

			//autumn: 9-11
			if(mm>=9 && mm<=11)
			{
				for(int i=0; i<22; i++)
					autumnPf[i] += pf[i];
				nAutumn ++;

				for(int i=0; i<22; i++)
					fprintf(fpAutumn, "%lf ", pf[i]);
				fprintf(fpAutumn, "\n");
			}

			//winter: 12,1, 2
			if( mm==12 || mm==1 || mm==2 )
			{
				for(int i=0; i<22; i++)
					winterPf[i] += pf[i];
				nWinter ++;

				for(int i=0; i<22; i++)
					fprintf(fpWinter, "%lf ", pf[i]);
				fprintf(fpWinter, "\n");
			}
		}		
	}

	printf("%d \n", nSum);

	
	//calculating average
	for(int i=0; i<22; i++)
	{
		spf[i] /= (double)(nSum);
		springPf[i] /= (double)(nSpring);
		summerPf[i] /= (double)(nSummer);
		autumnPf[i] /= (double)(nAutumn);
		winterPf[i] /= (double)(nWinter);
	}

	//output the average of all data
	for(int i=0; i<22; i++)
		fprintf(fpout, "%lf ", spf[i]);
	fprintf(fpout, "\n");

	//output the average of Spring
	for(int i=0; i<22; i++)
		fprintf(fpSpring, "%lf ", springPf[i]);
	fprintf(fpSpring, "\n");

	//output the average of Summer
	for(int i=0; i<22; i++)
		fprintf(fpSummer, "%lf ", summerPf[i]);
	fprintf(fpSummer, "\n");

	//output the average of Autumn
	for(int i=0; i<22; i++)
		fprintf(fpAutumn, "%lf ", autumnPf[i]);
	fprintf(fpAutumn, "\n");

	//output the average of Winter
	for(int i=0; i<22; i++)
		fprintf(fpWinter, "%lf ", winterPf[i]);
	fprintf(fpWinter, "\n");
    

	fclose(fp);
	fclose(fpout);
	fclose(fpSpring);
	fclose(fpSummer);
	fclose(fpAutumn);
	fclose(fpWinter);
	
	printf("Finished! \n");
}

void CProject_PGStationDlg::OnAeronetSizedistributiondiscreteseasons()
{
	// TODO: Add your command handler code here
}


void CProject_PGStationDlg::OnSizedistributionseasonsSpring()
{
	// TODO: Add your command handler code here

	//read .vol file
	char filename[256] = "c:\\temp\\Data\\Aeronet\\130101_131231_Beijing\\130101_131231_Beijing-space.vol";

	char cline[1024];
	char tempString[1024];
	char date[256];
	char time[256];

	double day;
	int hour,minute,second;
	int dd,mm,yy;

	double tc,teRadius,tr,ts;
	double fc,feRadius,fr,fs;
	double cc,ceRadius,cr,cs;



	FILE* fp = fopen(filename, "r");

	for(int i=0; i<4; i++)
	{
		fgets(cline, 1024, fp);
	}

	double fc1=0,fr1=0,fs1=0;
	double cc1=0,cr1=0,cs1=0;
	int nSum = 0;

	while(!feof(fp))
	{
		fgets(cline, 1024, fp);
		sscanf(cline, "%s %s %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %s", date, time, &day, 
			&tc,&teRadius,&tr,&ts,
			&fc,&feRadius,&fr,&fs,
			&cc,&ceRadius,&cr,&cs, 
			tempString);

		sscanf(date, "%c:\\temp%c:\\temp%d", &dd, &mm, &yy);
		sscanf(time, "%c:\\temp%c:\\temp%d", &hour, &minute, &second);

		//spring
		if(mm>=3 && mm<=5)
		{
			if( hour==2 )
			{
				fc1 += fc;
				fr1 += fr;
				fs1 += fs;
				cc1 += cc;
				cr1 += cr;
				cs1 += cs;
				nSum++;
			}
		}
	}

	fclose(fp);

	fc1 /= (double)(nSum);
	fr1 /= (double)(nSum);
	fs1 /= (double)(nSum);
	cc1 /= (double)(nSum);
	cr1 /= (double)(nSum);
	cs1 /= (double)(nSum);


	FILE* fpout = fopen("c:\\temp\\beijingVolumeSizeSpring.txt", "w");
	fprintf(fpout, "%lf %lf %lf %lf %lf %lf \n", fc1, fr1, fs1, cc1, cr1, cs1);
	fclose(fpout);


	double radius[22] = {0.05,0.0656,0.08608,0.11294,
		0.14818,0.19443,0.25511,0.33472,0.43917,
		0.57623,0.75605,0.992,1.30157,1.70776,
		2.2407,2.93997,3.85745,5.06126,6.64074,
		8.71315,11.43229,15};

	stParticleDistribution pd;
	pd.mode = 2;

	pd.medianRadius[0]  = fr1;//0.162;//; //0.176 ;//= 0.219;
	pd.stdVariance[0]   = fs1; //0.5387;//;//0.517 ;//= 0.531;
	pd.concentration[0] = fc1;//0.062;//;//;0.073;//= 0.153;
	pd.medianRadius[1]  = cr1; //2.286;//;//2.768 ;//= 2.724;
	pd.stdVariance[1]   = cs1; //0.594;//;//0.623 ;//= 0.583;
	pd.concentration[1] = cc1; //0.346;//;//0.117;//= 0.131;

	double pdf[22];
	BimodalLogDistribution(pd, radius, pdf, 22);

	fp = fopen("c:\\temp\\beijingSizeSpring.txt", "a+");
	for(int i=0; i<22; i++)
	{
		fprintf(fp, "%lf ", pdf[i]);
		//fprintf(fp, "%lf %lf \n", radius[i], pdf[i]);
		//printf("%lf %lf \n", radius[i], pdf[i]);
	}
	fprintf(fp, "\n");
	fclose(fp);

	printf("Finished! \n");


}

void CProject_PGStationDlg::OnSizedistributionseasonsSummer()
{
	// TODO: Add your command handler code here

	//read .vol file
	char filename[256] = "c:\\temp\\Data\\Aeronet\\130101_131231_Beijing\\130101_131231_Beijing-space.vol";

	char cline[1024];
	char tempString[1024];
	char date[256];
	char time[256];

	double day;
	int hour,minute,second;
	int dd,mm,yy;

	double tc,teRadius,tr,ts;
	double fc,feRadius,fr,fs;
	double cc,ceRadius,cr,cs;



	FILE* fp = fopen(filename, "r");

	for(int i=0; i<4; i++)
	{
		fgets(cline, 1024, fp);
	}

	double fc1=0,fr1=0,fs1=0;
	double cc1=0,cr1=0,cs1=0;
	int nSum = 0;

	while(!feof(fp))
	{
		fgets(cline, 1024, fp);
		sscanf(cline, "%s %s %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %s", date, time, &day, 
			&tc,&teRadius,&tr,&ts,
			&fc,&feRadius,&fr,&fs,
			&cc,&ceRadius,&cr,&cs, 
			tempString);

		sscanf(date, "%c:\\temp%c:\\temp%d", &dd, &mm, &yy);
		sscanf(time, "%c:\\temp%c:\\temp%d", &hour, &minute, &second);

		//summer
		if(mm>=6 && mm<=8)
		{
			if( hour==2 )
			{
				fc1 += fc;
				fr1 += fr;
				fs1 += fs;
				cc1 += cc;
				cr1 += cr;
				cs1 += cs;
				nSum++;
			}
		}
	}

	fclose(fp);

	fc1 /= (double)(nSum);
	fr1 /= (double)(nSum);
	fs1 /= (double)(nSum);
	cc1 /= (double)(nSum);
	cr1 /= (double)(nSum);
	cs1 /= (double)(nSum);


	FILE* fpout = fopen("c:\\temp\\beijingVolumeSizeSummer.txt", "w");
	fprintf(fpout, "%lf %lf %lf %lf %lf %lf \n", fc1, fr1, fs1, cc1, cr1, cs1);
	fclose(fpout);


	double radius[22] = {0.05,0.0656,0.08608,0.11294,
		0.14818,0.19443,0.25511,0.33472,0.43917,
		0.57623,0.75605,0.992,1.30157,1.70776,
		2.2407,2.93997,3.85745,5.06126,6.64074,
		8.71315,11.43229,15};

	stParticleDistribution pd;
	pd.mode = 2;

	pd.medianRadius[0]  = fr1;//0.162;//; //0.176 ;//= 0.219;
	pd.stdVariance[0]   = fs1; //0.5387;//;//0.517 ;//= 0.531;
	pd.concentration[0] = fc1;//0.062;//;//;0.073;//= 0.153;
	pd.medianRadius[1]  = cr1; //2.286;//;//2.768 ;//= 2.724;
	pd.stdVariance[1]   = cs1; //0.594;//;//0.623 ;//= 0.583;
	pd.concentration[1] = cc1; //0.346;//;//0.117;//= 0.131;

	double pdf[22];
	BimodalLogDistribution(pd, radius, pdf, 22);

	fp = fopen("c:\\temp\\beijingSizeSummer.txt", "a+");
	for(int i=0; i<22; i++)
	{
		fprintf(fp, "%lf ", pdf[i]);
		//fprintf(fp, "%lf %lf \n", radius[i], pdf[i]);
		//printf("%lf %lf \n", radius[i], pdf[i]);
	}
	fprintf(fp, "\n");
	fclose(fp);

	printf("Finished! \n");

}

void CProject_PGStationDlg::OnSizedistributionseasonsAutumn()
{
	// TODO: Add your command handler code here


	//read .vol file
	char filename[256] = "c:\\temp\\Data\\Aeronet\\130101_131231_Beijing\\130101_131231_Beijing-space.vol";

	char cline[1024];
	char tempString[1024];
	char date[256];
	char time[256];

	double day;
	int hour,minute,second;
	int dd,mm,yy;

	double tc,teRadius,tr,ts;
	double fc,feRadius,fr,fs;
	double cc,ceRadius,cr,cs;



	FILE* fp = fopen(filename, "r");

	for(int i=0; i<4; i++)
	{
		fgets(cline, 1024, fp);
	}

	double fc1=0,fr1=0,fs1=0;
	double cc1=0,cr1=0,cs1=0;
	int nSum = 0;

	while(!feof(fp))
	{
		fgets(cline, 1024, fp);
		sscanf(cline, "%s %s %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %s", date, time, &day, 
			&tc,&teRadius,&tr,&ts,
			&fc,&feRadius,&fr,&fs,
			&cc,&ceRadius,&cr,&cs, 
			tempString);

		sscanf(date, "%c:\\temp%c:\\temp%d", &dd, &mm, &yy);
		sscanf(time, "%c:\\temp%c:\\temp%d", &hour, &minute, &second);

		//autumn
		if(mm>=9 && mm<=11)
		{
			if( hour==2 )
			{
				fc1 += fc;
				fr1 += fr;
				fs1 += fs;
				cc1 += cc;
				cr1 += cr;
				cs1 += cs;
				nSum++;
			}
		}
	}

	fclose(fp);

	fc1 /= (double)(nSum);
	fr1 /= (double)(nSum);
	fs1 /= (double)(nSum);
	cc1 /= (double)(nSum);
	cr1 /= (double)(nSum);
	cs1 /= (double)(nSum);


	FILE* fpout = fopen("c:\\temp\\beijingVolumeSizeAutumn.txt", "w");
	fprintf(fpout, "%lf %lf %lf %lf %lf %lf \n", fc1, fr1, fs1, cc1, cr1, cs1);
	fclose(fpout);


	double radius[22] = {0.05,0.0656,0.08608,0.11294,
		0.14818,0.19443,0.25511,0.33472,0.43917,
		0.57623,0.75605,0.992,1.30157,1.70776,
		2.2407,2.93997,3.85745,5.06126,6.64074,
		8.71315,11.43229,15};

	stParticleDistribution pd;
	pd.mode = 2;

	pd.medianRadius[0]  = fr1;//0.162;//; //0.176 ;//= 0.219;
	pd.stdVariance[0]   = fs1; //0.5387;//;//0.517 ;//= 0.531;
	pd.concentration[0] = fc1;//0.062;//;//;0.073;//= 0.153;
	pd.medianRadius[1]  = cr1; //2.286;//;//2.768 ;//= 2.724;
	pd.stdVariance[1]   = cs1; //0.594;//;//0.623 ;//= 0.583;
	pd.concentration[1] = cc1; //0.346;//;//0.117;//= 0.131;

	double pdf[22];
	BimodalLogDistribution(pd, radius, pdf, 22);

	fp = fopen("c:\\temp\\beijingSizeAutumn.txt", "a+");
	for(int i=0; i<22; i++)
	{
		fprintf(fp, "%lf ", pdf[i]);
		//fprintf(fp, "%lf %lf \n", radius[i], pdf[i]);
		//printf("%lf %lf \n", radius[i], pdf[i]);
	}
	fprintf(fp, "\n");
	fclose(fp);

	printf("Finished! \n");


}

void CProject_PGStationDlg::OnSizedistributionseasonsWinter()
{
	// TODO: Add your command handler code here


	//read .vol file
	char filename[256] = "c:\\temp\\Data\\Aeronet\\130101_131231_Beijing\\130101_131231_Beijing-space.vol";

	char cline[1024];
	char tempString[1024];
	char date[256];
	char time[256];

	double day;
	int hour,minute,second;
	int dd,mm,yy;

	double tc,teRadius,tr,ts;
	double fc,feRadius,fr,fs;
	double cc,ceRadius,cr,cs;



	FILE* fp = fopen(filename, "r");

	for(int i=0; i<4; i++)
	{
		fgets(cline, 1024, fp);
	}

	double fc1=0,fr1=0,fs1=0;
	double cc1=0,cr1=0,cs1=0;
	int nSum = 0;

	while(!feof(fp))
	{
		fgets(cline, 1024, fp);
		sscanf(cline, "%s %s %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %s", date, time, &day, 
			&tc,&teRadius,&tr,&ts,
			&fc,&feRadius,&fr,&fs,
			&cc,&ceRadius,&cr,&cs, 
			tempString);

		sscanf(date, "%c:\\temp%c:\\temp%d", &dd, &mm, &yy);
		sscanf(time, "%c:\\temp%c:\\temp%d", &hour, &minute, &second);

		//winter
		if(mm==12 || mm==1 || mm==2)
		{
			if( hour==2 )
			{
				fc1 += fc;
				fr1 += fr;
				fs1 += fs;
				cc1 += cc;
				cr1 += cr;
				cs1 += cs;
				nSum++;
			}
		}
	}

	fclose(fp);

	fc1 /= (double)(nSum);
	fr1 /= (double)(nSum);
	fs1 /= (double)(nSum);
	cc1 /= (double)(nSum);
	cr1 /= (double)(nSum);
	cs1 /= (double)(nSum);


	FILE* fpout = fopen("c:\\temp\\beijingVolumeSizeWinter.txt", "w");
	fprintf(fpout, "%lf %lf %lf %lf %lf %lf \n", fc1, fr1, fs1, cc1, cr1, cs1);
	fclose(fpout);

	double radius[22] = {0.05,0.0656,0.08608,0.11294,
		0.14818,0.19443,0.25511,0.33472,0.43917,
		0.57623,0.75605,0.992,1.30157,1.70776,
		2.2407,2.93997,3.85745,5.06126,6.64074,
		8.71315,11.43229,15};

	stParticleDistribution pd;
	pd.mode = 2;

	pd.medianRadius[0]  = fr1;//0.162;//; //0.176 ;//= 0.219;
	pd.stdVariance[0]   = fs1; //0.5387;//;//0.517 ;//= 0.531;
	pd.concentration[0] = fc1;//0.062;//;//;0.073;//= 0.153;
	pd.medianRadius[1]  = cr1; //2.286;//;//2.768 ;//= 2.724;
	pd.stdVariance[1]   = cs1; //0.594;//;//0.623 ;//= 0.583;
	pd.concentration[1] = cc1; //0.346;//;//0.117;//= 0.131;

	double pdf[22];
	BimodalLogDistribution(pd, radius, pdf, 22);

	fp = fopen("c:\\temp\\beijingSizeWinter.txt", "a+");
	for(int i=0; i<22; i++)
	{
		fprintf(fp, "%lf ", pdf[i]);
		//fprintf(fp, "%lf %lf \n", radius[i], pdf[i]);
		//printf("%lf %lf \n", radius[i], pdf[i]);
	}
	fprintf(fp, "\n");
	fclose(fp);

	printf("Finished! \n");
}

void CProject_PGStationDlg::OnAeronetCluster()
{
	// TODO: Add your command handler code here

	stAerosolInfo* pAerosolInfo = new stAerosolInfo[372]; // 12 months * 31 days
    memset(pAerosolInfo, 0, sizeof(stAerosolInfo)*372);

	//input files: aerosol volume distribution, refractive index, SSA, asymmetric
	char path[256] = "c:\\temp\\Data\\Aeronet\\130101_131231_Beijing";

	char volFile[256];
	char ssaFile[256];
	char rinFile[256];
	char asyFile[256];
	char sizFile[256];

	sprintf(volFile, "%s\\130101_131231_Beijing-space.vol", path);
	sprintf(ssaFile, "%s\\130101_131231_Beijing-space.ssa", path);
	sprintf(rinFile, "%s\\130101_131231_Beijing-space.rin", path);
	sprintf(asyFile, "%s\\130101_131231_Beijing-space.asy", path);
	sprintf(sizFile, "%s\\130101_131231_Beijing-space.siz", path);

	//1. read volume size distribution file
	char cline[1024];
	char tempString[1024];
	char date[256];
	char time[256];

	double day;
	int hour,minute,second;
	int dd,mm,yy;

	double tc,teRadius,tr,ts;
	double fc,feRadius,fr,fs;
	double cc,ceRadius,cr,cs;
	int nSum = 0;

	FILE* fp = fopen(volFile, "r");
	for(int i=0; i<4; i++)
	{
		fgets(cline, 1024, fp);
	}
	while(!feof(fp))
	{
		fgets(cline, 1024, fp);
		sscanf(cline, "%s %s %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %s", date, time, &day, 
			&tc,&teRadius,&tr,&ts,
			&fc,&feRadius,&fr,&fs,
			&cc,&ceRadius,&cr,&cs, 
			tempString);
		sscanf(date, "%c:\\temp%c:\\temp%d", &dd, &mm, &yy);
		sscanf(time, "%c:\\temp%c:\\temp%d", &hour, &minute, &second);

		int index = (mm-1)*31+(dd-1);

		if(hour==2)
		{
			pAerosolInfo[index].fc = fc;
			pAerosolInfo[index].fr = fr;
			pAerosolInfo[index].fs = fs;
			pAerosolInfo[index].cc = cc;
			pAerosolInfo[index].cr = cr;
			pAerosolInfo[index].cs = cs;
			pAerosolInfo[index].bInvertSize = true;
		}
	}
	fclose(fp);


	//2. reading SSA file
	double ssa[4];
	fp = fopen(ssaFile, "r");
	for(int i=0; i<4; i++)
	{
		fgets(cline, 1024, fp);
	}
	while(!feof(fp))
	{
		fgets(cline, 1024, fp);
		sscanf(cline, "%s %s %lf %lf %lf %lf %lf  %s", date, time, &day, 
			&(ssa[0]), &(ssa[1]), &(ssa[2]), &(ssa[3]),	tempString);
		sscanf(date, "%c:\\temp%c:\\temp%d", &dd, &mm, &yy);
		sscanf(time, "%c:\\temp%c:\\temp%d", &hour, &minute, &second);

		int index = (mm-1)*31+(dd-1);

		if(hour==2)
		{
			for(int k=0; k<4; k++)
				pAerosolInfo[index].ssa[k] = ssa[k];
			pAerosolInfo[index].bInvertSSA = true;
		}
	}
	fclose(fp);

	//3. reading refractive index file
	double realPart[4];
	double imagPart[4];
	fp = fopen(rinFile, "r");
	for(int i=0; i<4; i++)
	{
		fgets(cline, 1024, fp);
	}
	while(!feof(fp))
	{
		fgets(cline, 1024, fp);
		sscanf(cline, "%s %s %lf  %lf %lf %lf %lf  %lf %lf %lf %lf   %s", date, time, &day, 
			&(realPart[0]), &(realPart[1]), &(realPart[2]), &(realPart[3]),
			&(imagPart[0]), &(imagPart[1]), &(imagPart[2]), &(imagPart[3]),
			tempString);
		sscanf(date, "%c:\\temp%c:\\temp%d", &dd, &mm, &yy);
		sscanf(time, "%c:\\temp%c:\\temp%d", &hour, &minute, &second);

		int index = (mm-1)*31+(dd-1);

		if(hour==2)
		{
			for(int k=0; k<4; k++)
			{
				pAerosolInfo[index].realPart[k] = realPart[k];
				pAerosolInfo[index].imagPart[k] = imagPart[k];
			}
			pAerosolInfo[index].bInvertRI = true;
		}
	}
	fclose(fp);


	//4. reading total asymmetric parameters
	double asy[4];
	fp = fopen(asyFile, "r");
	for(int i=0; i<4; i++)
	{
		fgets(cline, 1024, fp);
	}
	while(!feof(fp))
	{
		fgets(cline, 1024, fp);
		sscanf(cline, "%s %s %lf  %lf %lf %lf %lf %s", date, time, &day, 
			&(asy[0]), &(asy[1]), &(asy[2]), &(asy[3]),
			tempString);
		sscanf(date, "%c:\\temp%c:\\temp%d", &dd, &mm, &yy);
		sscanf(time, "%c:\\temp%c:\\temp%d", &hour, &minute, &second);

		int index = (mm-1)*31+(dd-1);

		if(hour==2)
		{
			for(int k=0; k<4; k++)
			{
				pAerosolInfo[index].asy[k] = asy[k];
			}
			pAerosolInfo[index].bInvertASY = true;
		}
	}
	fclose(fp);

	
	//5. reading discrete size distribution 
	fp = fopen(sizFile, "r");
	for(int i=0; i<4; i++)
	{
		fgets(cline, 1024, fp);
	}
	double pf[22];
	while(!feof(fp))
	{
		fgets(cline, 1024, fp);
		sscanf(cline, "%s %s %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf ", 
			date, time, &day, 
			&pf[0],&pf[1],&pf[2],&pf[3],&pf[4],
			&pf[5],&pf[6],&pf[7],&pf[8],&pf[9],
			&pf[10],&pf[11],&pf[12],&pf[13],&pf[14],
			&pf[15],&pf[16],&pf[17],&pf[18],&pf[19],
			&pf[20],&pf[21]);

		sscanf(time, "%c:\\temp%c:\\temp%d", &hour, &minute, &second);
		sscanf(date, "%c:\\temp%c:\\temp%d", &dd, &mm, &yy);

		int index = (mm-1)*31+(dd-1);

		if( hour==2 )
		{
			for(int k=0; k<22; k++)
				pAerosolInfo[index].pf[k] = pf[k];
		}
	}
	fclose(fp);
	



	//clustering
	vector<stPattern> samples;
	vector<int> dayIndex;
    for(int i=0; i<372; i++)
	{
		stPattern s;
		
		if( pAerosolInfo[i].bInvertASY && pAerosolInfo[i].bInvertRI && pAerosolInfo[i].bInvertSize && pAerosolInfo[i].bInvertSSA  )
		{
			double cc = pAerosolInfo[i].cc;
			double fc = pAerosolInfo[i].fc;

			/*
			//concentration normalization
			double nc = sqrt(cc*cc+fc*fc);
			cc = cc / nc;
			fc = fc / nc;
			*/

			s.s.push_back( cc );
			s.s.push_back( pAerosolInfo[i].cr );
			s.s.push_back( pAerosolInfo[i].cs );
			s.s.push_back( fc );
			s.s.push_back( pAerosolInfo[i].fr );
			s.s.push_back( pAerosolInfo[i].fs );

			for(int k=0; k<4; k++)
			{
				s.s.push_back( pAerosolInfo[i].realPart[k]);
				s.s.push_back( pAerosolInfo[i].imagPart[k]);
				s.s.push_back( pAerosolInfo[i].ssa[k]);
				s.s.push_back( pAerosolInfo[i].asy[k]);
			}
		
			samples.push_back(s);
			dayIndex.push_back(i);
		}
	}
	
	stIsoDataParas controlParas;
	controlParas.expectClusters = 4;
	controlParas.initClusters = 6;
	controlParas.limitForSplit = 0.3;
	controlParas.limitForMerge = 0.05;
    controlParas.maxIterations = 250;
	controlParas.minimalSamples = 16;
	controlParas.maximalNumberOfMerge = 2;

	vector<stCluster> clusters;
    IsoData(samples, clusters, controlParas);
    

	FILE* fpall = fopen("c:\\temp\\clusterVolSize.txt", "w" );
	printf("Clustered results: \n");
	for(int i=0; i<clusters.size(); i++)
	{		
		printf("%d  ", clusters[i].clusterSet.size());
		printf("%lf %lf %lf  %lf %lf %lf \n", 
			clusters[i].centroid.s[0],
			clusters[i].centroid.s[1],
			clusters[i].centroid.s[2],
			clusters[i].centroid.s[3],
			clusters[i].centroid.s[4],
			clusters[i].centroid.s[5]);

		int season[4];
		season[0]=season[1]=season[2]=season[3]=0;

		char clusterfile[256];
		sprintf(clusterfile, "c:\\temp\\cluster_%d.txt", i);		
		fp = fopen(clusterfile, "w");
		for(int k=0; k<clusters[i].clusterSet.size(); k++)
		{
			int ti = clusters[i].clusterSet[k];
			int si = dayIndex[ti];

			for(int ki=0; ki<22; ki++)
				fprintf(fp, "%lf ", pAerosolInfo[si].pf[ki]);
			fprintf(fp, "\n");
		
			//determine the season
			int m = si/31+1;
			if(m>=3 && m<=5)
				season[0]++;
			if(m>=6 && m<=8)
				season[1]++;
			if(m>=9 && m<=11)
				season[2]++;
			if(m==1 || m==2 || m==12)
				season[3]++;
		}
		
		// output the clustered 
		double radius[22] = {0.05,0.0656,0.08608,0.11294,
			0.14818,0.19443,0.25511,0.33472,0.43917,
			0.57623,0.75605,0.992,1.30157,1.70776,
			2.2407,2.93997,3.85745,5.06126,6.64074,
			8.71315,11.43229,15};
		stParticleDistribution pd;
		pd.mode = 2;
		pd.medianRadius[0]  = clusters[i].centroid.s[4]; //0.162;//; //0.176 ;//= 0.219;
		pd.stdVariance[0]   = clusters[i].centroid.s[5]; //0.5387;//;//0.517 ;//= 0.531;
		pd.concentration[0] = clusters[i].centroid.s[3]; //0.062;//;//;0.073;//= 0.153;
		pd.medianRadius[1]  = clusters[i].centroid.s[1]; //2.286;//;//2.768 ;//= 2.724;
		pd.stdVariance[1]   = clusters[i].centroid.s[2]; //0.594;//;//0.623 ;//= 0.583;
		pd.concentration[1] = clusters[i].centroid.s[0]; //0.346;//;//0.117;//= 0.131;
         
		double pdf[22];
		BimodalLogDistribution(pd, radius, pdf, 22);
		for(int i=0; i<22; i++)
		{
			fprintf(fp, "%lf ", pdf[i]);
		}
		fprintf(fp, "\n");
		fclose(fp);

		for(int i=0; i<22; i++)
		{
			fprintf(fpall, "%lf ", pdf[i]);
		}
		fprintf(fpall, "\n");

		//output the season ratio
		char seasonfile[256];
		sprintf(seasonfile, "c:\\temp\\season_%d.txt", i);		
		fp = fopen(seasonfile, "w");
		fprintf(fp, "%d %d %d %d \n", season[0], season[1], season[2], season[3]);
		fclose(fp);

	}
	fclose(fpall);
	
	//aerosol types
	FILE* fpType = fopen("c:\\temp\\clusterType.txt", "w");
	for(int i=0; i<clusters.size(); i++)
	{
		fprintf(fpType, "samples: %d   ", clusters[i].clusterSet.size());

		fprintf(fpType, "%lf %lf %lf  %lf %lf %lf  ", 
			clusters[i].centroid.s[0],
			clusters[i].centroid.s[1],
			clusters[i].centroid.s[2],
			clusters[i].centroid.s[3],
			clusters[i].centroid.s[4],
			clusters[i].centroid.s[5]);	
		for(int k=0; k<16; k++)
		{
			fprintf(fpType, " %lf ", clusters[i].centroid.s[6+k]);
		}
		fprintf(fpType, "\n");
	}
	fclose(fpType);



	printf("Finished ! \n");
}



void CProject_PGStationDlg::OnClusterIsodata()
{
	// TODO: Add your command handler code here

	vector<stPattern> samples;

	Point2D64f cp1, cp2;

	cp1.x = 0;
	cp1.y = 0;

	cp2.x = 20;
	cp2.y = 20;

	srand( time(NULL) );

	for(int i=0; i<20; i++)
	{
	
		double rx = ( (double) (rand()) / (double)(RAND_MAX) * 2 - 1 ) * 5;
		double ry = ( (double) (rand()) / (double)(RAND_MAX) * 2 - 1 ) * 5;
		double x = cp1.x + rx;
		double y = cp1.y + ry;
		stPattern pat;
		pat.s.push_back(x);
		pat.s.push_back(y);
		
		samples.push_back(pat);
	
		rx = ( (double) (rand()) / (double)(RAND_MAX) * 2 - 1 ) * 5;
		ry = ( (double) (rand()) / (double)(RAND_MAX) * 2 - 1 ) * 5;
		x = cp2.x + rx;
		y = cp2.y + ry;
		//stPattern pat;
		pat.s.clear();
		pat.s.push_back(x);
		pat.s.push_back(y);

		samples.push_back(pat);	
	}


	stIsoDataParas controlParas;
	controlParas.expectClusters = 2;
	controlParas.initClusters = 4;
	controlParas.limitForSplit = 5;
	controlParas.limitForMerge = 2;
	controlParas.maxIterations = 100;
	controlParas.minimalSamples = 10;
	controlParas.maximalNumberOfMerge = 2;

	vector<stCluster> clusters;
	IsoData(samples, clusters, controlParas);

	for(int i=0; i<clusters.size(); i++)
	{
		printf("%lf %lf \n", clusters[i].centroid.s[0], clusters[i].centroid.s[1]);
	}

	FILE* fp = fopen("c:\\temp\\ptCluster.txt", "w");
	for(int i=0; i<samples.size(); i++)
	{
		fprintf(fp, "%lf %lf \n", samples[i].s[0], samples[i].s[1]);
	}
	for(int i=0; i<clusters.size(); i++)
	{
		fprintf(fp, "%lf %lf \n", clusters[i].centroid.s[0], clusters[i].centroid.s[1]);
	}
	fclose(fp);
}

void CProject_PGStationDlg::OnHjSimulate33156()
{
	// TODO: Add your command handler code here


	//1. loading LUTs for specified aerosol type
	char lutDir[256] = "F:\\Data\\Luts\\HJ\\type";
	vector<CAtmosphereCorrectionBase*> vecCorrectBlue;
	vector<CAtmosphereCorrectionBase*> vecCorrectRed;
	int aerosolTypeNumber = 1;
	LoadLutsForDDV(lutDir, aerosolTypeNumber, vecCorrectBlue, vecCorrectRed);

	//2. initial parameters
	double sza = 30;
	double vza = 0;
	double azi = 150;

	double blueRef = 0.05;
	double ratio = 2;            // ratio = (reflectance of Red) /  (reflectance of Blue)
	double redRef  = blueRef*ratio;

	double redExt = vecCorrectRed[0]->GetExtinction();
	double blueExt = vecCorrectBlue[0]->GetExtinction();
	printf("%lf %lf \n", blueExt, redExt);

	double ext550 = 1.078559;

	FILE* fp = fopen("c:\\temp\\toa.txt", "w");

	for(double aod=0.0; aod<=2.01; aod+=0.1)
	{			

		double aodBlue = aod * (blueExt / ext550);
		double aodRed  = aod * (redExt / ext550);

		//printf("aodBlue: %lf   aodRec:\\temp%lf \n", aodBlue, aodRed);

		//blue
		double T,S;
		vecCorrectBlue[0]->CalculateTAndS(sza, vza, azi, aodBlue, T, S);
		//printf("transmission:%lf  backscattering ratio:%lf \n", T, S);
		double r,rp,pa;
		vecCorrectBlue[0]->GetPathRef(sza, vza, azi, aodBlue, r, rp);
		double mr,mrp; //mocular contribution
		vecCorrectBlue[0]->GetPathRef(sza, vza, azi, 0, mr, mrp);
		//printf(" blue moclular: %lf %lf \n", mr, mrp);

		pa = r;
		double toaBlue = pa + (blueRef*T)/(1-S*blueRef);
		//printf("blue toa:%lf \n", toa);
		//fprintf(fp, "%lf %lf %lf %lf %lf", aodBlue, T, S, pa, toa);

		//red
		vecCorrectRed[0]->CalculateTAndS(sza, vza, azi, aodRed, T, S);
		//printf("transmission:%lf  backscattering ratio:%lf \n", T, S);
		vecCorrectRed[0]->GetPathRef(sza, vza, azi, aodRed, r, rp);
		vecCorrectRed[0]->GetPathRef(sza, vza, azi, 0, mr, mrp);
		//printf(" red moclular: %lf %lf \n", mr, mrp);

		pa = r;
		double toaRed = pa + (redRef*T)/(1-S*redRef);
		//printf("red toa:%lf \n", toa);

		printf("toa: %lf %lf \n", toaBlue, toaRed);
		fprintf(fp, "%lf %lf %lf \n", aod, toaBlue, toaRed);
	}

	fclose(fp);
}

void CProject_PGStationDlg::OnHjSimulate33157()
{
	// TODO: Add your command handler code here

	/*********** simulate error caused by changed ratio *********************/

	//1. loading LUTs for specified aerosol type
	char lutDir[256] = "c:\\temp\\Data\\LutTypeHJ";
	vector<CAtmosphereCorrectionBase*> vecCorrectBlue;
	vector<CAtmosphereCorrectionBase*> vecCorrectRed;
	int aerosolTypeNumber = 1;
	LoadLutsForDDV(lutDir, aerosolTypeNumber, vecCorrectBlue, vecCorrectRed);

	//2. initial parameters
	double sza = 30;
	double vza = 0;
	double azi = 180;

	double redExt = vecCorrectRed[0]->GetExtinction();
	double blueExt = vecCorrectBlue[0]->GetExtinction();

	double fixedRatio = 0.5;
	
	double ratio   = 0.0;
	double blueRef = 0.03;
	double redRef  = 0.0;
	
	double aod = 2.0;

	double aodArray[4] = {0.2, 0.5, 1.2, 1.8};

	for(int i=0; i<4; i++)
	{
		char errorAodFile[256];
		sprintf(errorAodFile, "c:\\temp\\error_aod_%d.txt", i);
		
		aod = aodArray[i]; 

		FILE* fp = fopen(errorAodFile, "w");
		for( ratio=0.4; ratio<=0.601; ratio+=0.01)
		{
			redRef = blueRef / ratio;

			//1. using the wrong ratio to simulate the TOA reflectance
			double aodBlue = aod;
			double aodRed  = redExt/blueExt*aodBlue;
			//blue
			double T,S;
			vecCorrectBlue[0]->CalculateTAndS(sza, vza, azi, aodBlue, T, S);
			double r,rp,pa;
			vecCorrectBlue[0]->GetPathRef(sza, vza, azi, aodBlue, r, rp);
			pa = r;
			double toaBlue = pa + (blueRef*T)/(1-S*blueRef);
			//red
			vecCorrectRed[0]->CalculateTAndS(sza, vza, azi, aodRed, T, S);
			vecCorrectRed[0]->GetPathRef(sza, vza, azi, aodRed, r, rp);
			pa = r;
			double toaRed = pa + (redRef*T)/(1-S*redRef);

			//2. invert using the fixed ratio ( 0.49 or 0.5 )
			double invertAOD = DTSinglePtInvert(vecCorrectBlue, vecCorrectRed, sza, vza, azi, toaBlue, toaRed, 0.5);

			//3. calculate the error
			double err = invertAOD - aod;

			printf("ratio: %lf  error:%lf \n", ratio, err);
			fprintf(fp, "%lf %lf \n", ratio, err);
		}
		fclose(fp);
	}
}

void CProject_PGStationDlg::OnPlyRead()
{
	// TODO: Add your command handler code here

	////ascii format
	//char* filename = "c:\\temp\\Data\\UAV\\2014-06-13-1\\sparse.ply";
	////char* filename = "c:\\temp\\Data\\UAV\\2014-05-08\\half\\res.ply";
	////char* filename = "c:\\temp\\Data\\3D\\bunny\\reconstruction\\bun_zipper.ply";

	////binary format
	////char* filename = "c:\\temp\\Data\\UAV\\2014-05-08\\half\\111.0.ply";

	//double* x;
	//double* y;
	//double* z;
	//unsigned char* red;
	//unsigned char* green;
	//unsigned char* blue;
	//int npt;

	//GetPlyVertex(filename, &x, &y, &z, &red, &green, &blue, &npt);

	////generate triangles;
	//int* p1;
	//int* p2;
	//int* p3;
	//
	////int nTri = GenerateTrianglesUsingTinLib(x, y, z, npt, &p1, &p2, &p3);
	////int nTri = GenerateTrianglesCMU(x, y, z, npt, &p1, &p2, &p3);

	////printf("%d \n", nTri);

	////WritePly("c:\\temp\\mesh-tri.ply", x, y, z, red, green, blue, npt, p1, p2, p3, nTri);
	//

	//free(x);
	//free(y);
	//free(z);
	//free(red);
	//free(green);
	//free(blue);
	//free(p1);
	//free(p2);
	//free(p3);

	//printf("Generating Triangle Finished !\n");
}

void CProject_PGStationDlg::OnTriangulateFitplane()
{
//	// TODO: Add your command handler code here
//
//	//read *.out file
//	//char* outfile = "c:\\temp\\Data\\UAV\\2014-06-13-1\\sparse.out";
//	char* outfile = "c:\\temp\\NetDisk\\Program\\Assemble\\release\\bundle\\bundle_011.out";
//
//	vector<stPOS> camParas;
//	vector<stTrack> tracks;
//
//	ReadBundlerOutFile(outfile, camParas, tracks);
//
//	/*
//	for(int i=0; i<tracks.size(); i++)
//	{
//		double gx,gy,gz;
//		double ix,iy;
//		
//		gx = tracks[i].x;
//		gy = tracks[i].y;
//		gz = tracks[i].z;
//
//		for(int j=0; j<tracks[i].imgpt.size(); j++)
//		{
//			int imgId = tracks[i].imgid[j];
//
//			GrdToImg1( camParas[imgId].R, camParas[imgId].T, 
//				camParas[imgId].f, camParas[imgId].k1, camParas[imgId].k2, 
//				gx, gy, gz, &ix, &iy, 0, 0);
//
//			printf("%lf %lf  %lf %lf \n", tracks[i].imgpt[j].x, tracks[i].imgpt[j].y, ix, iy);
//		}
//	}
//	*/
//
//	//test plane fitting
//	double R[9];
//	int np = tracks.size();
//	int nall = np + camParas.size();
//	double* px = (double*)malloc( nall*sizeof(double) );
//	double* py = (double*)malloc( nall*sizeof(double) );
//	double* pz = (double*)malloc( nall*sizeof(double) );
//    unsigned char* red = (unsigned char*)malloc(nall);
//	unsigned char* green = (unsigned char*)malloc(nall);
//	unsigned char* blue = (unsigned char*)malloc(nall);
//
//	for(int i=0; i<tracks.size(); i++)
//	{
//		px[i] = tracks[i].x;
//		py[i] = tracks[i].y;
//		pz[i] = tracks[i].z;
//		red[i]   = tracks[i].r;
//		green[i] = tracks[i].g;
//		blue[i]  = tracks[i].b;
//	}	
//
//	FitPlane1(px, py, pz, np, R);
//    
//	//convert form RX+T to R( X - (-inv(R)*T) )
//	for(int i=0; i<camParas.size(); i++)
//	{
//		double t1[3];
//		double R1[9];
//        
//		memcpy(R1, camParas[i].R, 9*sizeof(double) );
//		invers_matrix(R1, 3);
//		mult(R1, camParas[i].T, t1, 3, 3, 1);
//		
//		px[i+np] = -t1[0];
//		py[i+np] = -t1[1];
//		pz[i+np] = -t1[2];
//
//		camParas[i].xs = -t1[0]; 
//		camParas[i].ys = -t1[1]; 
//		camParas[i].zs = -t1[2];
//
//		red[i+np]   = 255;
//		green[i+np] = 0;
//		blue[i+np]  = 0;
//	}
//
//	//transform
//	for(int i=0; i<nall; i++)
//	{
//		double gP[3];
//		double tp[3];
//		gP[0] = px[i]; 
//		gP[1] = py[i]; 
//		gP[2] = pz[i];
//		mult(R,gP,tp,3,3,1);
//		px[i] = tp[0]; 
//		py[i] = tp[1]; 
//		pz[i] = tp[2];
//	}
//
//	//save camera position
//	for(int i=0; i<camParas.size(); i++)
//	{
//		double gP[3];
//		double tp[3];
//		gP[0] = camParas[i].xs; 
//		gP[1] = camParas[i].ys; 
//		gP[2] = camParas[i].zs;
//		mult(R,gP,tp,3,3,1);
//		camParas[i].xs = tp[0]; 
//		camParas[i].ys = tp[1]; 
//		camParas[i].zs = tp[2];
//	}
//
//	//calculate the average height
//	double dHeight = 0;
//	double minx = 100000000;
//	double maxx = 0;
//	double miny = 100000000;
//	double maxy = 0;
//	for(int i=0; i<np; i++)
//	{
//		dHeight += pz[i];
//		minx = min(minx, px[i]);
//		maxx = max(maxx, px[i]);
//		miny = min(miny, py[i]);
//		maxy = max(maxy, py[i]);
//	}
//	dHeight /= np;
//	printf("height: %lf \n", dHeight);
//	printf("range: %lf %lf %lf %lf \n", minx, maxx, miny, maxy);
//
//	//calculate the area of each camera and resolution
//	int ht = 3744;
//	int wd = 5616;
//	double sInterval = 0;	
//	for(int i=0; i<camParas.size(); i++)
//	{
//		double height = fabs(dHeight-camParas[i].zs);
//		double grdWd = (double)(wd)/camParas[i].f * height;
//		double interval = grdWd/(double)(wd);
//		printf(" %lf ", interval);
//		sInterval += interval;
//	}
//	sInterval /= (double)( camParas.size() );
//	printf("interval: %lf \n", sInterval);
//
//	FILE* fp = fopen("c:\\temp\\flyInfo.txt", "w");
//	fprintf(fp, "%s %lf \n", "resolution", sInterval);
//	fclose(fp);
//	
//	int mht = (maxy-miny)/sInterval;
//	int mwd = (maxx-minx)/sInterval;
//	printf("mht: %d mwc:\\temp%d \n", mht, mwd);
//
//	//save as ply
//	WritePly("c:\\temp\\mesh-rotate.ply", px, py, pz, red, green, blue, np, NULL, NULL, NULL, 0);
//
//
//	//generate triangles
//	int* p1;
//	int* p2;
//	int* p3;
//	int nTri = GenerateTrianglesCMU(px, py, pz, np, &p1, &p2, &p3);
//	WritePly("c:\\temp\\mesh-tri.ply", px, py, pz, red, green, blue, np, p1, p2, p3, nTri);
//
//
//	//interpolation
//
//
//	//get the minimal distance of all edges
//	//double minInterval = GetMinInterval(px,py,pz,np,p1,p2,p3,nTri);
//	//printf("minimal interval: %lf \n", minInterval);

	printf("Finished ! \n");
}

void CProject_PGStationDlg::OnBoostTest()
{
	// TODO: Add your command handler code here

	mbaTest();

	printf("finished! \n");
}
void CProject_PGStationDlg::OnDemMba()
{
	// TODO: Add your command handler code here

	////multilevel b-spline interpolation
	//char* filename = "c:\\temp\\mesh-rotate.ply";

	////read scattered 3D points from *.ply file
	//double* x;
	//double* y;
	//double* z;
	//unsigned char* red;
	//unsigned char* green;
	//unsigned char* blue;
	//int npt;

	//GetPlyVertex(filename, &x, &y, &z, &red, &green, &blue, &npt);
 //  
	//double resolution;
	//char tc[256];
	//FILE* fp = fopen("c:\\temp\\flyInfo.txt", "r");
	//fscanf(fp, "%s %lf \n", tc, &resolution);
	//fclose(fp);

	//float* iz = NULL;
	//int desHt, desWd;
	//MBAInterpolation(x, y, z, npt, 50, resolution, desHt, desWd, &iz);
	
	printf("Finished! \n");
}



void CProject_PGStationDlg::OnOrthoMosaic()
{
	// TODO: Add your command handler code here
	char* mainPath = "c:\\temp\\data\\uav";
	char* nvmFile  = "c:\\temp\\data\\uav\\0613.nvm";
	char* outFile  = "c:\\temp\\data\\uav\\0613.out";
	
	MosaicWithDEM(nvmFile, outFile, mainPath);

	printf("Finished! \n ");

}

void CProject_PGStationDlg::OnTransformRotation()
{
	// TODO: Add your command handler code here

	double omiga,phi,kapa;
	
}

void CProject_PGStationDlg::OnConvertMod09()
{
	// TODO: Add your command handler code here

	char mod02File[256] = "c:\\temp\\mod02.hdf";

	char** subDataDesc = f2c(1000, 256);

	//read subdata information
	int nSubData = ReadSubDataInfo(mod02File, subDataDesc);
	FILE* fp = fopen("c:\\temp\\subDataDesc.txt", "w");
	for(int i=0; i<nSubData; i++)
	{
		printf("%s \n", subDataDesc[i]);
		fprintf(fp, "%s \n", subDataDesc[i]);
	}
	fclose(fp);
}


void CProject_PGStationDlg::OnValidateModis()
{
	// TODO: Add your command handler code here

	//2010.8.16
	//char modisFile[256] = "c:\\temp\\Data\\MODIS\\500m\\2010225\\blue.tif";
	//char hjFile[256] = "c:\\temp\\Data\\HJ\\2010\\08\\16\\blueCr-500m.tif";

	//2010.9.30
	//char modisFile[256] = "c:\\temp\\Data\\MODIS\\500m\\2010273\\bj-blue.tif";
	//char hjFile[256] = "c:\\temp\\Data\\HJ\\2010\\09\\30\\blueCr-500m.tif";

	//2010.10.29
	char modisFile[256] = "c:\\temp\\Data\\MODIS\\500m\\2010305\\bj-blue.tif";
	char hjFile[256] = "c:\\temp\\Data\\HJ\\2010\\10\\29\\blueCr-500m.tif";


	stGeoInfo modisGeo;
	stGeoInfo hjGeo;
	GetGeoInformation(modisFile, modisGeo);
	GetGeoInformation(hjFile, hjGeo);

	int* modisBuffer = NULL;
	int  mht,mwd;
	ReadGeoFileInt(modisFile, 1, &modisBuffer, mht,mwd);

	int* hjBuffer = NULL;
	int  hht,hwd;
	ReadGeoFileInt(hjFile, 1, &hjBuffer, hht,hwd);

	FILE* fp = fopen("c:\\temp\\mod_hj_reflectance.txt", "w");
	int nsum = 0;
	int nout = 0; 
	for(int j=1; j<hht-1; j+=2)
		for(int i=1; i<hwd-1; i+=2)
		{

			double hVal = (double)(hjBuffer[j*hwd+i]) / 10000.0;

			//considering neighborhood
			double mean = 0;
			for(int m=-1; m<=1; m++)
				for(int n=-1; n<=1; n++)
				{
					mean += (double)(hjBuffer[(j+m)*hwd+(i+n)]) / 10000.0;
				}
			hVal = mean/9.0;
			
			if(hVal<0.01)
				continue;

			nsum++;
			
			double gx = i*hjGeo.dx + hjGeo.left;
			double gy = j*hjGeo.dy + hjGeo.top;

			int x = (gx-modisGeo.left)/modisGeo.dx;
			int y = (gy-modisGeo.top)/modisGeo.dy;

			if( x>=0 && x<mwd && y>=0 && y<mht )
			{
				double mVal = (double)(modisBuffer[y*mwd+x]) / 10000.0;

				if(mVal<0.01)
					continue;	

				if( fabs(hVal-mVal)>0.04 )
					continue;
			
				if( hVal<0.2 && mVal<0.2 )
				{
					fprintf(fp, "%lf %lf \n", hVal, mVal);
					nout++;
				}
			}		
		}
	fclose(fp);

	printf( " out ratio: %lf \n ", (double)(nout)/(double)(nsum) );
	
	printf("Finished! \n");

}

void CProject_PGStationDlg::OnBlurEntropy()
{
	// TODO: Add your command handler code here
	char filename[256] = "c:\\temp\\Data\\Belt\\BlurDetection\\blur\\0007.jpg";
	//char filename[256] = "c:\\temp\\Data\\Belt\\BlurDetection\\clear\\0002.jpg";
	

	
	IplImage* pImage = cvLoadImage(filename, 0);

	//IplImageToGrayImage()
	 
	int ht = pImage->height;
	int wd = pImage->width;
	int scanwd = pImage->widthStep;


	//split the image into blocks
	int blockWd = 16;
	unsigned char* pBuffer = (unsigned char*)malloc(blockWd*blockWd);

	int nx = wd / blockWd;
	int ny = ht / blockWd;

	for(int j=0; j<ny-1; j++)
	{
		for(int i=0; i<nx-1; i++)
		{
			CvRect rect;
			rect.x = i*blockWd;
			rect.y = j*blockWd;

			rect.height = blockWd;
			rect.width  = blockWd;

			int index = 0;
			for(int m=rect.y; m<(rect.y+blockWd); m++ )
				for( int n=rect.x; n<(rect.x+blockWd); n++)
				{
					pBuffer[ index ] = pImage->imageData[ m*scanwd + n ];
					index++;
				}
			SaveBmp("c:\\temp\\rect.bmp", pBuffer, blockWd, blockWd);

			/*
			cvSetImageROI(pImage, rect);
			IplImage *rectImage = cvCloneImage(pImage);
			rectImage->height = rectImage->roi->height;
			rectImage->width  = rectImage->roi->width;
			rectImage->widthStep = rectImage->roi->width;
			cvSaveImage("c:\\temp\\rectImage.jpg", rectImage);
			cvResetImageROI(pImage);
			//
			unsigned char* pBuffer;
			int ht,wd;
			IplImageToGrayImage(rectImage, &pBuffer, &ht, &wd);
			SaveBmp("c:\\temp\\rect.bmp", pBuffer, ht, wd);
			free(pBuffer);			
			cvReleaseImage(&rectImage);
			*/

			//double  entropy =  CalculateEntropy(pBuffer, blockWd, blockWd);
			//printf("%3.1lf ", entropy);			
		}
		printf("\n");
	}

	free(pBuffer);
}

void CProject_PGStationDlg::OnEntropyMax()
{
	// TODO: Add your command handler code here

	double entropy = 0;
	for(int i=0; i<256; i++)
	{
		//calculate the probability
		double p = 1.0 / 256;
		entropy += -p*log(p+0.000001);
	}

	printf("entropy: %lf \n", entropy);
}

void CProject_PGStationDlg::OnBlurBatch()
{
	// TODO: Add your command handler code here

	char* imagePath = "c:\\temp\\Data\\Belt\\BlurDetection\\test\\all";
	//char* imagePath = "c:\\temp\\Data\\Belt\\BlurDetection\\test\\blur";
	//char* imagePath = "c:\\temp\\Data\\Belt\\BlurDetection\\test\\clear";
	

	char** filenames = NULL;
	int n=0,nfile=0;

	GetDirFileName(filenames, imagePath, &n, &nfile, "jpg", 0);
	//printf("%d \n", nfile);

	filenames = f2c(nfile, 256);
	GetDirFileName(filenames, imagePath, &n, &nfile, "jpg", 1);

	int nBlur = 0;
	for(int i=0; i<nfile; i++)
	{
		//printf("%s \n", filenames[i]);
		IplImage* pImage = cvLoadImage(filenames[i], 0);
		unsigned char* pBuffer = NULL;
		int ht,wd;
		IplImageToGrayImage(pImage, &pBuffer, &ht, &wd);

		int nIsBlur =  BlurDetectUsingEntropy(pBuffer, ht, wd, 8);
		
		printf("%d \n", nIsBlur);
		if(nIsBlur)
		{
			nBlur ++;
		}
		
		char filename[256];
        if(nIsBlur>0)
		{
			sprintf(filename, "c:\\temp\\blur_%d.jpg", i);
		}
		else
		{
			sprintf(filename, "c:\\temp\\clear_%d.jpg", i);
		}		
		cvSaveImage(filename, pImage);	

		cvReleaseImage(&pImage);
		free(pBuffer);
	}

	printf("blur=%lf    clear=%lf \n",  (double)(nBlur)/(double)(nfile), 1-(double)(nBlur)/(double)(nfile) ); 


}


void CProject_PGStationDlg::OnSeaImageCrop()
{

	printf("Sea image crop .... \n");

	char imagePath[256] = "F:\\Data\\Sea\\image";

	char** filenames = NULL;
	int nfile, n;
	n = nfile = 0;
	GetDirFileName(filenames, imagePath, &n, &nfile, "JPG", 0);
	filenames = f2c(nfile, 256);
	GetDirFileName(filenames, imagePath, &n, &nfile, "JPG", 1);


	CvRect rect;
    rect.x = 0;
	rect.y = 90;
	rect.width = 720;
	rect.height = 450;

	for(int i=0; i<nfile; i++)
	{
		char cfilepath[512];
		strcpy(cfilepath, filenames[i]);
		char* pdes = strrchr(cfilepath, '.');
		int index = pdes - cfilepath + 1;
		strcpy(cfilepath+index, "\0");
		strcat(cfilepath, "crop.jpg");

		IplImage* pImage = cvLoadImage(filenames[i], 1);

		cvSetImageROI(pImage, rect);

		IplImage* dst = cvCreateImage(cvSize(rect.width, rect.height),
			pImage->depth, pImage->nChannels);

		cvCopy(pImage,dst,0);
		cvResetImageROI(pImage);

		printf("%s \n", cfilepath);
		cvSaveImage(cfilepath, dst);

		cvReleaseImage(&pImage);
		cvReleaseImage(&dst);
	}      

}
void CProject_PGStationDlg::OnDatabaseCifar()
{
	// TODO: Add your command handler code here
	
	const int kCIFARSize = 32;
	const int kCIFARImageNBytes  = 3072;
	const int kCIFARBatchSize    = 10000;
	const int kCIFARTrainBatches = 5;

	//reading cifar-10 data
	char* filename = "F:\\Data\\DeepLearning\\cifar\\cifar-10-batches-bin\\data_batch_1.bin";
	
	unsigned char* pImage = (unsigned char*)malloc(kCIFARImageNBytes);

	FILE* fp = fopen(filename, "rb");

	for(int i=0; i<kCIFARBatchSize; i++)
	{
		unsigned char nLabel;
		fread(&nLabel, 1, 1, fp);
		fread(pImage, 1, kCIFARImageNBytes, fp);
        printf("%d ", nLabel);

		//save as image
		char outfile[256];
		sprintf(outfile, "F:\\Data\\DeepLearning\\cifar\\classes\\%d\\%.4d.jpg", nLabel, i);
		GdalWriteImageByteColor(outfile, pImage, pImage+1024, pImage+2048, kCIFARSize, kCIFARSize);
		
		//stGeoInfo geoInfo;
		//sprintf(outfile, "c:\\temp\\temp\\%.4d.tif", i);
		//GdalWriteImageColor(outfile, pImage, pImage+1024, pImage+2048, kCIFARSize, kCIFARSize, geoInfo);
	}

	fclose(fp);
}

void CProject_PGStationDlg::OnDatabaseGeneratelabel()
{
	// TODO: Add your command handler code here

	char* imagePath = "F:\\Data\\ImageNet\\Images\\truck";

	char** filenames = NULL;
	int n,nfile;
	n=0;
	nfile = 0;

	GetDirFileName(filenames, imagePath, &n, &nfile, "JPEG", 0);
	filenames = f2c(nfile, 256);
	GetDirFileName(filenames, imagePath, &n, &nfile, "JPEG", 1);

	for(int i=0; i<nfile; i++)
	{
		printf("%s \n", filenames[i]);

		//find the second place from the back
		int len = strlen(filenames[i]);
		int k=0;
		int hit = 0;
		for(k=len-1; k>=0; k--)
		{			
			char tc = filenames[i][k]; 
			char cslash = '\\'; 
			//if( strcmp(&tc, &cslash)==0 )
			if(tc==cslash)
			{

				filenames[i][k] = '//';
				hit ++;				
			}
			if(hit==2)
			{
				break;
			}
		}
		//
		char fileSubPath[256];
		strcpy(fileSubPath, filenames[i]+k+1);
		printf("%s \n", fileSubPath);

	}
}

void CProject_PGStationDlg::OnAtmospherecorrectionHj33190()
{
	// TODO: Add your command handler code here

	printf("Atmospheric correction using Aeronet Data..... \n");


	//reading LUTs



	//reading HJ-1 data



	//saving reflectance	
	/*
	int* cBlue  = (int*)malloc(sizeof(int)*ht*wd);
	int* cGreen = (int*)malloc(sizeof(int)*ht*wd);
	int* cRed   = (int*)malloc(sizeof(int)*ht*wd);
	memset(cBlue,  0, sizeof(int)*ht*wd);
	memset(cGreen, 0, sizeof(int)*ht*wd);
	memset(cRed,   0, sizeof(int)*ht*wd);

	//atmospheric correction
	for(int j=0; j<ht; j++)
	{
		for(int i=0; i<wd; i++)
		{
			double sx = min(nSmallWd-1, (double)(i) * ratioX );
			double sy = min(nSmallHt-1, (double)(j) * ratioY );
            
			//using minimal value or average 
			//double aod = pAod[sy*desWd+sx];
			//int l = max(0, i-2);
			//int r = min( ;

			double aod = BilinearAOD(sx, sy, pAodSmall, nSmallHt, nSmallWd);
           	aod *= 0.9;
            
			double blueExt  =  vecCorrectBlue[0]->GetExtinction();
			double greenExt =  vecCorrectGreen[0]->GetExtinction();
			double redExt   =  vecCorrectRed[0]->GetExtinction();
						
			double aodRed = aod*(redExt/blueExt);
			double aodGreen = aod*(greenExt/blueExt);
            
			//DN value
			unsigned char r,g,b,ir;
			b  = pband1[j*wd+i];
			g  = pband2[j*wd+i];
			r  = pband3[j*wd+i];
			ir = pband4[j*wd+i];

			//convert from DN to reflectance
			if(b>0 && r>0 && ir>0 && aod>0)
			{
				//from DN to radiance
				double refBlue  = pband1[j*wd+i] / hjdata.m_gain[0] + hjdata.m_offset[0]; //band 1
				double refGreen = pband2[j*wd+i] / hjdata.m_gain[1] + hjdata.m_offset[1]; //band 1
				double refRed   = pband3[j*wd+i] / hjdata.m_gain[2] + hjdata.m_offset[2]; //band 3
				double refIR    = pband4[j*wd+i] / hjdata.m_gain[3] + hjdata.m_offset[3]; //band 4				

				//from radiance to reflectance
				refBlue  = (refBlue*PI)/(fluxBlue*cos(sunZenith));
				refGreen = (refGreen*PI)/(fluxGreen*cos(sunZenith));
				refRed   = (refRed*PI)/(fluxRed*cos(sunZenith));
				refIR    = (refIR*PI)/(fluxIr*cos(sunZenith));
				double ndvi = (refIR-refRed) / (refIR+refRed);

				//convert to ground coordinate
				double gy = subGeoInfo.top  + j*subGeoInfo.dy;
				double gx = subGeoInfo.left + i*subGeoInfo.dx;
				int row =  (oriGeoInfo.top - gy)  / fabs(oriGeoInfo.dy);
				int col =  (gx - oriGeoInfo.left) / fabs(oriGeoInfo.dx);
				double vza, vazimuth;
				pReadAngle->GetViewGeo(row, col, vza, vazimuth);
				if(vazimuth<0) vazimuth += 360;
				double relativeAzimuth = fabs(vazimuth-sazimuth);
				if(relativeAzimuth>180) relativeAzimuth -= 180;
				

				double crBlue  = vecCorrectBlue[0]->Correct(hjdata.solar_zenith, vza, relativeAzimuth, aod, refBlue, 0);
				double crGreen = vecCorrectGreen[0]->Correct(hjdata.solar_zenith, vza, relativeAzimuth, aodGreen, refGreen, 0);
				double crRed   = vecCorrectRed[0]->Correct(hjdata.solar_zenith, vza, relativeAzimuth, aodRed, refRed, 0);
				
				if(crBlue<0)
					crBlue   = vecCorrectBlue[0]->Correct(hjdata.solar_zenith, vza, relativeAzimuth, 0, refBlue, 0);
				if(crGreen<0)
					crGreen  = vecCorrectGreen[0]->Correct(hjdata.solar_zenith, vza, relativeAzimuth, 0, refGreen, 0);
				if(crRed<0)
					crRed    = vecCorrectRed[0]->Correct(hjdata.solar_zenith, vza, relativeAzimuth, 0, refRed, 0);

				//crBlue = max(0.01, crBlue);
				//crGreen = max(0.01, crGreen);
				//crRed = max(0.01, crRed);

				if(crBlue>0 && crRed>0 && crGreen>0)
				{
					cBlue[j*wd+i] = crBlue*10000;
					double crIntensity =   min(255, crBlue*fluxBlue*cos(sunZenith)); // / PI ;			
					pband1[j*wd+i] = crIntensity;

					cGreen[j*wd+i] = crGreen*10000;
					crIntensity =   min(255, crGreen*fluxGreen*cos(sunZenith)); // / PI ;			
					pband2[j*wd+i] = crIntensity;

					cRed[j*wd+i] = crRed*10000;
					crIntensity =   min(255, crRed*fluxRed*cos(sunZenith)); // / PI ;			
					pband3[j*wd+i] = crIntensity;
				}
			}			
		}	
		printf(".");
	}
	
	GdalWriteByte("c:\\temp\\blueCr-radiance.tif", pband1, ht, wd, subGeoInfo);
	GdalWriteByte("c:\\temp\\greenCr-radiance.tif", pband2, ht, wd, subGeoInfo);
	GdalWriteByte("c:\\temp\\redCr-radiance.tif", pband3, ht, wd, subGeoInfo);	
	
	GdalWriteInt("c:\\temp\\blueCr-reflectance.tif", cBlue, ht, wd, subGeoInfo);
	GdalWriteInt("c:\\temp\\greenCr-reflectance.tif", cGreen, ht, wd, subGeoInfo);
	GdalWriteInt("c:\\temp\\redCr-reflectance.tif", cRed, ht, wd, subGeoInfo);
	free(cBlue);
	free(cGreen);
	free(cRed);
	*/

	printf("Finished ! \n");
}

void SimulateTOA(CAtmosphereCorrectionBase* pCR, double ext550, double ext, double waveLen, double surf, 
				 double sza, double vza, double azi, 
				 vector<double>& aotVec,
				 vector<double>& toaVec)
{
	//output TOA
	//FILE* fp = fopen("c:\\temp\\toa.txt", "w");

	aotVec.clear();
	toaVec.clear();

	//simulating the TOA with the change of AOD 
	for(int i=0; i<=20; i++)
	{
		double aot550 = 0.1*i;
		double aot = aot550 * ext / ext550;    

		//transmittance and spherical albedo
		double dT = 0; // double transition 
		double S = 0;  // albedo
		pCR->CalculateTAndS(sza, vza, azi, aot, dT, S);

		//path reflectance
		double pathRef, pathRefPol;
		pCR->GetPathRef(sza, vza, azi, aot, pathRef, pathRefPol);

		//calculate TOA
		double toa = pathRef + (surf*dT)/(1-S*surf);
		printf("aoc:\\temp %lf T: %lf  S: %lf  toa: %lf \n", aot550, dT, S, toa);

		//fprintf(fp, "%6.4lf  %6.4lf \n", aot550, toa);
		aotVec.push_back(aot550);
		toaVec.push_back(toa);
	}

	//fclose(fp);
}


void CProject_PGStationDlg::OnSimulationOop()
{
	// TODO: Add your command handler code here
	//simulating TOA based on OOP class

	//475nm blue channel
	char blut0[256] = "F:\\Data\\Luts\\HJ\\type\\1\\blue\\0.dat";
	char blut1[256] = "F:\\Data\\Luts\\HJ\\type\\1\\blue\\10.dat";
	char blut2[256] = "F:\\Data\\Luts\\HJ\\type\\1\\blue\\25.dat";
	double ext475 = 1.270033;
		

	//660nm red channel
	//double wavelen = 0.66;
	char rlut0[256] = "F:\\Data\\Luts\\HJ\\type\\1\\red\\0.dat";
	char rlut1[256] = "F:\\Data\\Luts\\HJ\\type\\1\\red\\10.dat";
	char rlut2[256] = "F:\\Data\\Luts\\HJ\\type\\1\\red\\25.dat";
	double ext660 = 0.870828;
	

	double ext550 = 1.078559;
	double sza = 30;
	double vza = 0;
	double azi = 150;
	double surfBlue = 0.05; 
	double ratio = 0.5;         // ratio of blue vs red reflectance 
	double surfRed = surfBlue / ratio; 
    

	CAtmosphereCorrectionBase* pCRBlue = new CAtmosphereCorrectionRt3();
	pCRBlue->LoadLut(blut0, blut1, blut2, 0.475);

	CAtmosphereCorrectionBase* pCRRed = new CAtmosphereCorrectionRt3();
	pCRRed->LoadLut(rlut0, rlut1, rlut2, 0.66);


	vector<double> aot;
	//blue 	
	vector<double> toa475;
	SimulateTOA(pCRBlue, ext550, ext475, 0.475, surfBlue, sza, vza, azi, aot, toa475);
	

	//red 	
	vector<double> toa660;
	SimulateTOA(pCRRed, ext550, ext660, 0.66, surfRed, sza, vza, azi, aot, toa660);


	FILE* fp = fopen("c:\\temp\\simulateTOA.txt", "w");
	for(int i=0; i<aot.size(); i++)
	{
		fprintf(fp, "%6.4lf %6.4lf %6.4lf  \n", aot[i], toa475[i], toa660[i]);
	}
	fclose(fp);

}


void CAboutDlg::OnHjInvertsimulate()
{
	// TODO: Add your command handler code here

	/*
	printf("Invert Simulated.... \n");

	//reading simulated data
	char simulatefile[256] = "E:\\Kuaipan\\doc\\papers\\HJ-Aerosol\\data\\simulateTOA.txt"; 
    
	int nRow = GetFileRows(simulatefile);
    printf("%d \n", nRow);

	FILE* fp = fopen(simulatefile, "r");

	for(int i=0; i<nRow; i++)
	{
		double aot, toaBlue, toaRed;
		fscanf(fp, "%lf %lf %lf ", &aot, &toaBlue, &toaRed);
		printf("%lf %lf %lf \n", aot, toaBlue, toaRed);


	}
	fclose(fp);
	*/
}

void CProject_PGStationDlg::OnHjInvertsimulate()
{
	// TODO: Add your command handler code here


	printf("Invert Simulated.... \n");

	//loading luts
	char lutDir[256] = "F:\\Data\\Luts\\HJ\\type";
	vector<CAtmosphereCorrectionBase*> vecCorrectBlue;
	vector<CAtmosphereCorrectionBase*> vecCorrectRed;
	int aerosolTypeNumber = 1;
	LoadLutsForDDV(lutDir, aerosolTypeNumber, vecCorrectBlue, vecCorrectRed);    
       

	//input parameters
	double sza = 30;
	double vza = 0;
	double azi = 150;
	double ext550 = 1.078559;
	double extBlue = vecCorrectBlue[0]->GetExtinction();

	//reading simulated data
	char simulatefile[256] = "E:\\Kuaipan\\doc\\papers\\HJ-Aerosol\\data\\simulateTOA.txt"; 
	int nRow = GetFileRows(simulatefile);
	printf("%d \n", nRow);
	FILE* fp = fopen(simulatefile, "r");
	for(int i=0; i<nRow; i++)
	{
		double aot, toaBlue, toaRed;
		fscanf(fp, "%lf %lf %lf ", &aot, &toaBlue, &toaRed);
		printf("input: %lf %lf %lf \n", aot, toaBlue, toaRed);

		//invert using ratio algorithm
		double invertAOD = DTSinglePtInvert(vecCorrectBlue, vecCorrectRed, sza, vza, azi, toaBlue, toaRed, 0.6);
		//printf("%6.4lf \n", invertAOD);

		//convert to 550nm
		double aot550 = invertAOD * (ext550/extBlue);
		printf("%6.4lf \n", aot550);

	}
	fclose(fp);
}

void CProject_PGStationDlg::OnSensitivityRatio()
{
	// TODO: Add your command handler code here

	//loading luts
	char lutDir[256] = "F:\\Data\\Luts\\HJ\\type";
	vector<CAtmosphereCorrectionBase*> vecCorrectBlue;
	vector<CAtmosphereCorrectionBase*> vecCorrectRed;
	int aerosolTypeNumber = 1;
	LoadLutsForDDV(lutDir, aerosolTypeNumber, vecCorrectBlue, vecCorrectRed);    
	//////////////////////////////////////////////////////////////////////////


	//input parameters
	double sza = 30;
	double vza = 0;
	double azi = 150;
	double ext550 = 1.078559;
	double extBlue = vecCorrectBlue[0]->GetExtinction();
	//////////////////////////////////////////////////////////////////////////


	//reading simulated data
	char simulatefile[256] = "E:\\Kuaipan\\doc\\papers\\HJ-Aerosol\\data\\simulateTOA.txt"; 
	int nRow = GetFileRows(simulatefile);
	printf("%d \n", nRow);
	vector<double> toaBlueArray;
	vector<double> toaRedArray;
	vector<double> aotArray;
	FILE* fp = fopen(simulatefile, "r");
	for(int i=0; i<nRow; i++)
	{
		double aot, toaBlue, toaRed;
		fscanf(fp, "%lf %lf %lf ", &aot, &toaBlue, &toaRed);
		toaBlueArray.push_back(toaBlue);
		toaRedArray.push_back(toaRed);
		aotArray.push_back(aot);
	}
	fclose(fp);
	//////////////////////////////////////////////////////////////////////////


	double aotArrayIndex[5] = {0,1,2,5,10};    
	vector<vector<double>> vecError;
    vecError.resize(5);
	for(int i=0; i<5; i++)
	{
		int idx = aotArrayIndex[i];
		double toaBlue = toaBlueArray[idx];
		double toaRed  = toaRedArray[idx];
		double simulateAot = aotArray[idx];

		//for(double dr = 0.0; dr<0.5; dr+=0.01)
		double dr = 0.1;
		{
			//invert using ratio algorithm
			double invertAODUp = DTSinglePtInvert(vecCorrectBlue, vecCorrectRed, sza, vza, azi, toaBlue, toaRed, 0.5+dr);
			double invertAODDown = DTSinglePtInvert(vecCorrectBlue, vecCorrectRed, sza, vza, azi, toaBlue, toaRed, 0.5-dr);
            
			//convert to 550nm
			double errUp    = fabs( invertAODUp * (ext550/extBlue) - simulateAot);
			double errDown  = fabs( invertAODDown * (ext550/extBlue) - simulateAot);
			double errTotal = (errUp+errDown)*0.5;
			
			printf("%lf %lf \n", errUp, errDown);
			vecError[i].push_back(errTotal);
			printf("%lf ", errTotal);
		}
		printf("\n");
	}

	printf("Finished! \n");
}

void CProject_PGStationDlg::OnSensitivityErrorbyratio()
{
	// TODO: Add your command handler code here


	//loading luts
	char lutDir[256] = "F:\\Data\\Luts\\HJ\\type";
	vector<CAtmosphereCorrectionBase*> vecCorrectBlue;
	vector<CAtmosphereCorrectionBase*> vecCorrectRed;
	int aerosolTypeNumber = 1;
	LoadLutsForDDV(lutDir, aerosolTypeNumber, vecCorrectBlue, vecCorrectRed);    
	//////////////////////////////////////////////////////////////////////////


	//input parameters
	double sza = 30;
	double vza = 10;
	double azi = 150;
	double ext550 = 1.078559;
	double extBlue = vecCorrectBlue[0]->GetExtinction();
	double extRed  = vecCorrectRed[0]->GetExtinction();
	double surfBlue = 0.03;
	double ratio = 0.5;
	double surfRed = surfBlue / ratio;
	//////////////////////////////////////////////////////////////////////////

	//calculate the scattering angle
	double sca = CalcScatterAngle(sza, vza, azi);
	printf("scattering angle: %lf \n", sca);

	//simulate TOA of 475nm and 660nm according to input aot and ratio
	double aot550 = 0.5;

	FILE* fp = fopen("c:\\temp\\error_by_surf.txt", "w");
	for(double dr=0.0; dr<0.1; dr+=0.01) //
	{	
		double tr[2];
		tr[0] = ratio + dr;
		tr[1] = ratio - dr;
		fprintf(fp, "%6.4lf  ", dr);
		
		for(int k=0; k<2; k++)
		{		
			double surfRed = surfBlue / tr[k];
			double toaBlue = SimulateTOASingle( vecCorrectBlue[0], ext550, aot550, extBlue, 0.475, surfBlue, sza, vza, azi);
			double toaRed  = SimulateTOASingle( vecCorrectRed[0], ext550, aot550, extRed, 0.660, surfRed, sza, vza, azi);
			double invertAOT = DTSinglePtInvert( vecCorrectBlue, vecCorrectRed, sza, vza, azi, toaBlue, toaRed, ratio);
			double invertAOT550   = invertAOT * ( ext550/extBlue );

			printf("simulate aot:%6.4lf  ratio: %lf %lf %lf %lf  error: %6.4lf \n", aot550, tr[k], toaBlue, toaRed, invertAOT550, 
				fabs(aot550-invertAOT550));

			//fprintf(fp, "%6.4lf %6.4lf %6.4lf \n", )
			fprintf(fp, "%6.4lf  ", fabs(aot550-invertAOT550));
		}
		fprintf(fp, "\n");

		printf("\n");
	}
	fclose(fp);
}



void CProject_PGStationDlg::OnHjBlueRed()
{
	// TODO: Add your command handler code here

	//calculate the ratio of blue and red 
	char blueFile[256] = "F:\\Data\\HJ\\2010\\09\\28\\beijing\\surfBand1.tif";
	char redFile[256]  = "F:\\Data\\HJ\\2010\\09\\28\\beijing\\surfBand3.tif";
	char irFile[256]   = "F:\\Data\\HJ\\2010\\09\\28\\beijing\\surfBand4.tif";

	int ht,wd;
	float* pBlue;
	float* pRed;
	float* pIR;
	ReadGeoFileFloat(blueFile, 1, &pBlue, ht, wd);
	ReadGeoFileFloat(redFile, 1, &pRed, ht, wd);
	ReadGeoFileFloat(irFile, 1, &pIR, ht, wd);

	printf("%d %d \n", ht, wd);

	//calculate the NDVI
	unsigned char* pNDVI = (unsigned char*)malloc(ht*wd);
	memset(pNDVI,0,ht*wd);
	for(int j=0; j<ht; j++)
		for(int i=0; i<wd; i++)
		{
			int index = j*wd+i;
			double ndvi = (pIR[index] - pRed[index]) / (pIR[index] + pRed[index]);
			pNDVI[index] = ndvi*100;
		}
	//cvSaveImage("c:\\temp\\ndvi.jpg", )
	GdalWriteJpgCopy("c:\\temp\\ndvi.jpg", pNDVI, ht, wd);
	free(pNDVI);
	

	//float* ratio = (float*)malloc(ht*wd*sizeof(float));
	//memset(ratio, 0, ht*wd*sizeof(float));

	FILE* fp1 = fopen("c:\\temp\\blue-red-ndvi-gt-0.5.txt", "w");

	int index = 0;
	for(int j=2; j<ht-2; j+=5)
	{
		for(int i=2; i<wd-2; i+=5)
		{
			//index = j*wd+i;
			//double surfBlue = pBlue[j*wd+i];
			//double surfRed  = pRed[j*wd+i];

			double sNdvi = 0;
			double sBlue = 0;
			double sRed = 0;
			int    num = 0;
			for(int m=-2; m<=2; m++)
				for(int n=-2; n<=2; n++)
				{
					int id = (j+m)*wd+(i+n);
					double surfBlue = pBlue[id];
					double surfRed  = pRed[id];
					double surfIR   = pIR[id];	
					double ndvi = (surfIR-surfRed)/(surfIR+surfRed);	
				
					if(ndvi>0.0)
					{
						sNdvi += ndvi;
						sBlue += surfBlue;
						sRed  += surfRed;
						num ++;
					}
				}

			sNdvi /= (double)(num);
			sBlue /= (double)(num);
			sRed  /= (double)(num);

			//if( surfBlue<0 || surfRed<0 || surfIR<0 )
			//	continue;
			//double ndvi = (surfIR-surfRed)/(surfIR+surfRed);						
			//if(ndvi<0.2 && ndvi>0.0){
			//}			

			double ratio = sBlue / sRed;
			if( sNdvi>0.6 && 
				sBlue>0.0 && sBlue<0.12 && 
				sRed>0.0  && sRed<0.25  && 
				ratio>0.0 && ratio<2 )
			{
				//fprintf(fp1, "%6.4lf %6.4lf %6.4lf \n", surfBlue, surfRed, surfBlue/surfRed );
				fprintf(fp1, "%6.4lf %6.4lf %6.4lf \n", sBlue, sRed, ratio );
			}			
		}
		printf("%d \n", j);
	}

	fclose(fp1);

	free(pBlue);
	free(pRed);
	free(pIR);

	printf("finished !\n");
}

void CProject_PGStationDlg::OnSeaProcesspos()
{
	// TODO: Add your command handler code here

	char posfile[256] = "F:\\Data\\OceanVideo\\Frame1\\pos_rov_interp.txt";

	double ms; //second
	double lon,lat;

	int nLines = GetFileRows(posfile);
	printf("%d \n", nLines);

	vector<double> vLat;
	vector<double> vLon;
	double heading,pitch,roll;
	FILE* fp = fopen(posfile, "r");
	FILE* fpout = fopen("F:\\Data\\OceanVideo\\Frame1\\pos_rov_utm.txt", "w");
	for(int i=0; i<nLines; i++)
	{
		fscanf(fp, "%lf %lf %lf %lf %lf %lf  ", &ms, &lon, &lat, &heading, &pitch, &roll);
		//printf("%12.2lf %14.8lf %14.8lf \n", ms, lon, lat);
		int zoneNumber = int((lon + 180)/6) + 1;
		double gx,gy;
		LLtoUTM(23, lat, lon, gy, gx, zoneNumber);
		printf("%lf %lf \n", gx, gy);

		fprintf(fpout, "%12.2lf   %14.8lf %14.8lf    %14.8lf %14.8lf   %8.4lf %8.4lf %8.4lf \n", ms, lon, lat, gx, gy, heading, pitch, roll);
	}
	fclose(fp);
	fclose(fpout);

}

void CProject_PGStationDlg::OnSeaUsingpos()
{
	// TODO: Add your command handler code here

	char posfile[256] = "F:\\Data\\OceanVideo\\Frames\\pos_rov_utm.txt";

	int nLines = GetFileRows(posfile);
	printf("%d \n", nLines);


	vector<double> vecGx;
	vector<double> vecGy;
	double index, gx, gy, heading, pitch, roll;

	//calculate the range
	double mingx = 100000000;
	double maxgx = 0;
	double mingy = 100000000;
	double maxgy = 0;

	FILE* fp = fopen(posfile, "r");
    for(int i=0; i<nLines; i++)
	{
		fscanf(fp, "%lf  %lf %lf  %lf %lf %lf ", &index, &gx, &gy, &heading, &pitch, &roll);
		vecGx.push_back(gx);
		vecGy.push_back(gy);

		mingx = min(gx, mingx);
		maxgx = max(gx, maxgx);
		mingy = min(gy, mingy);
		maxgy = max(gy, maxgy);
	}
	fclose(fp);

	printf("%lf %lf  %lf %lf \n", mingx, maxgx, mingy, maxgy);

	double  yrange = maxgy - mingy;
	double  xrange = maxgx - mingx;
	printf("%lf %lf \n", xrange, yrange);

}
void CProject_PGStationDlg::OnModisMod09()
{
	// TODO: Add your command handler code here

	char blueFile[256] = "F:\\Data\\Modis\\500m\\2010273\\bj\\bj-blue.tif";
	char redFile[256]  = "F:\\Data\\Modis\\500m\\2010273\\bj\\bj-red.tif";
	char irFile[256]   = "F:\\Data\\Modis\\500m\\2010273\\bj\\bj-ir.tif";
	
	int  ht, wd;
	int* pBlue = NULL;
	int* pRed  = NULL;
	int* pIR = NULL;
	
	ReadGeoFileInt(blueFile, 1, &pBlue, ht, wd);
	ReadGeoFileInt(redFile,  1, &pRed,  ht, wd);
	ReadGeoFileInt(irFile,  1, &pIR,  ht, wd);
	

	double ratio = 0.0001;
	
	FILE* fp = fopen("c:\\temp\\modis_blue_vs_red.txt", "w");
	for(int j=0; j<ht; j++)
	{
		for(int i=0; i<wd; i++)
		{
			double refBlue = (double)(pBlue[j*wd+i]) * ratio;
			double refRed  = (double)(pRed[j*wd+i]) * ratio;
			double refIR   = (double)(pIR[j*wd+i]) * ratio;
			
			double ndvi = (refIR-refRed) / (refIR+refRed);
			
			if(ndvi>0.5)
			{
				double r = refBlue / refRed;
				//printf("%4.2lf ", r);
				
				fprintf(fp, "%6.4lf %6.4lf %6.4lf \n", refBlue, refRed, r);
			}
		}
		printf("%d ", j);
	}

	fclose(fp);

	free(pBlue);
	free(pRed);
	free(pIR);

	printf("Finished! \n ");

}
void CProject_PGStationDlg::OnPythonInvoke()
{
	// TODO: Add your command handler code here	
		
	
}



void CProject_PGStationDlg::OnReadingParasolls2()
{
	// TODO: Add your command handler code here

	char parasolL2File[256] = "F:\\Data\\PARASOL\\L2\\2005-10-08\\P3L2TLGA020047JL";

	CReasParasolLS2 readParasolLevelTwo;
	readParasolLevelTwo.Load(parasolL2File);
	
		
	/*
	stLSAP data;
	double lon1  = 116.3;
	double lat1  = 39.9;
	int    line1 = 884;
	int    col1  = 4805;
	data = readParasolLevelTwo.GetPtMultiViewData(line1, col1);
	*/
	
	//reading rectangle
	double minlon=115, maxlon=118, minlat=39, maxlat=41;
	
	int minLine = 100000;
	int maxLine = 0;
	int minCol = 100000;
	int maxCol = 0;

	//LatlonToLinecolHigh(maxlat, minlon, &t, &l);
	//LatlonToLinecolHigh(minlat, maxlon, &b, &r);
	
	double lon[2] = {116, 117};
	double lat[2] = {39, 40};
	for(int j=0; j<2; j++)
		for(int i=0; i<2; i++)
		{
			int line,col;
			LatlonToLinecolHigh(lat[j], lon[i], &line, &col);
			minLine = min(minLine, line);
			maxLine = max(maxLine, line);
			minCol = min(minCol, col);
			maxCol = max(maxCol, col);
		}

	printf("%d %d %d %d \n", minLine, maxLine, minCol, maxCol);

	for(int j=minLine; j<=maxLine; j++)
	{
		
		for(int i=minCol; i<=maxCol; i++)
		{
			double tlon,tlat;
			LinecolToLatlonHigh(j, i, &tlon, &tlat);
			
			if(tlon>minlon && tlon<maxlon && tlat>minlat && tlat<maxlat)
			{				
				char filename[256];
				sprintf(filename, "c:\\temp\\%d_%d.txt", j, i);
				stLSAP data = readParasolLevelTwo.GetPtMultiViewData(j, i);
				
				FILE* fp = fopen(filename, "w");
				for(int k=0; k<data.nDir; k++)
				{
					fprintf(fp, "%6.4lf %6.4lf %6.4lf %6.4lf    %6.4lf \n", 
						data.sza, data.vza[k], data.azi[k], data.sca[k],
						data.pr865[k]);
				}
				fclose(fp);
			}
		}
	}



	/*
	//printf("aoc:\\temp %lf \n", data.aod865);
	//double minlon=102, maxlon=125, minlat=32, maxlat=46;
	double minlon=110, maxlon=130, minlat=20, maxlat=55;

	double step = 1.0/6.0;

	int ht = (maxlat-minlat) / step;
	int wd = (maxlon-minlon) / step;

	float* pAerosol = (float*)malloc(ht*wd*sizeof(float));
	memset(pAerosol, 0, ht*wd*sizeof(float));


	FILE* fp = fopen("c:\\temp\\parasolAot20130917.txt", "w");
	int index = 0;
	for(int j=ht-1; j>=0; j--)
	{
		for(int i=0; i<wd; i++)
		{
			lon = minlon + i*step;
			lat = minlat + j*step;
			data = readParasolLevelTwo.GetPtMultiViewData(lon, lat);

			//printf("%5.3lf ", data.aod865);
			fprintf(fp, "%5.3lf ", data.aod865);

			if(data.aod865>0)
				pAerosol[index] = data.aod865;

			index ++;
		}
		fprintf(fp, " \n");
	}	
	fclose(fp);


	printf("writing geotiff ... \n");
	//GdalWriteFloatLL(aodTiffFile, pAOD, ht, wd, minlon, maxlon, minlat, maxlat);
	GdalWriteFloatLL("c:\\temp\\parasol_fineaod.tif", pAerosol, ht, wd, minlon, maxlon, minlat, maxlat);

	free(pAerosol);
	
	*/

	printf("Reading Finished ! \n");
}


//for Parasol Level1
void CProject_PGStationDlg::OnParasolLevel3()
{
	// TODO: Add your command handler code here
	// TODO: Add your command handler code here

	//char parasolL2File[256] = "F:\\Data\\PARASOL\\L2\\2005-10-08\\P3L2TLGA020047JL";
	//CReasParasolLS2 readParasolLevelTwo;
	//readParasolLevelTwo.Load(parasolL2File);	

	//loading luts for atmospheric correction based on AERONET 
	char clut0[256] = "F:\\Data\\Aeronet\\Beijing-singleDay\\20051008\\865\\";
	char clut1[256] = "c:\\temp\\Data\\LUTEastAsia\\Double_865_176_517_73_2768_623_117_1558_10_10.dat";
	char clut2[256] = "c:\\temp\\Data\\LUTEastAsia\\Double_865_176_517_73_2768_623_117_1558_10_25.dat";
	CAtmosphereCorrectionBase* pAC = new CAtmosphereCorrectionRt3();
	pAC->LoadLut(clut0, clut1, clut2, 0.865);
	//pAC->Correct(sza, vza, azi, aod, 0.2, 0);
     


	char filename[256] = "F:\\Data\\PARASOL\\2005\\10\\08\\P3L1TBG1020047JL";
	CReasParasolLevel1File readParasol;
	readParasol.Load(filename);

	//reading rectangle
	double minlon=115, maxlon=118, minlat=39, maxlat=41;

	int minLine = 100000;
	int maxLine = 0;
	int minCol = 100000;
	int maxCol = 0;	

	double lon[2] = {115, 117};
	double lat[2] = {39, 41};
	for(int j=0; j<2; j++)
		for(int i=0; i<2; i++)
		{
			int line,col;
			LatlonToLinecolHigh(lat[j], lon[i], &line, &col);
			minLine = min(minLine, line);
			maxLine = max(maxLine, line);
			minCol = min(minCol, col);
			maxCol = max(maxCol, col);
		}

		printf("%d %d %d %d \n", minLine, maxLine, minCol, maxCol);

		for(int j=minLine; j<=maxLine; j++)
		{

			for(int i=minCol; i<=maxCol; i++)
			{
				printf("%d %d \n", j,i);

				double tlon,tlat;
				LinecolToLatlonHigh(j, i, &tlon, &tlat);

				if(tlon>minlon && tlon<maxlon && tlat>minlat && tlat<maxlat)
				{				
					char filename[256];
					sprintf(filename, "c:\\temp\\out1\\%d_%d_L1.txt", j, i);
					//stLSAP data = readParasol.GetPtMultiViewData(j, i);
					stMultiView data = readParasol.GetPtMultiViewData(j, i);


					//atmospheric correction
					


					FILE* fp = fopen(filename, "w");
					for(int k=0; k<data.nView; k++)
					{
						//fprintf(fp, "%6.4lf %6.4lf %6.4lf %6.4lf    %6.4lf \n", 
						//	data.sza, data.vza[k], data.azi[k], data.sca[k],
						//	data.pr865[k]);

						fprintf(fp, "%6.4lf %6.4lf %6.4lf %6.4lf    %6.4lf \n", 
							data.mViewData[k].sza, data.mViewData[k].vza, data.mViewData[k].azi,
							data.mViewData[k].sca, data.mViewData[k].rp8);
					}

					fclose(fp);
				}
			}
		}

		printf(" Finished ! \n");
}


//for Parasol Level2 
void CProject_PGStationDlg::OnParasolLevel4()
{
	// TODO: Add your command handler code here

	char parasolL2File[256] = "F:\\Data\\PARASOL\\L2\\2005-10-08\\P3L2TLGA020047JL";

	CReasParasolLS2 readParasolLevelTwo;
	readParasolLevelTwo.Load(parasolL2File);	

	//reading rectangle
	double minlon=115, maxlon=118, minlat=39, maxlat=41;

	int minLine = 100000;
	int maxLine = 0;
	int minCol = 100000;
	int maxCol = 0;	

	double lon[2] = {115, 117};
	double lat[2] = {39, 41};
	for(int j=0; j<2; j++)
		for(int i=0; i<2; i++)
		{
			int line,col;
			LatlonToLinecolHigh(lat[j], lon[i], &line, &col);
			minLine = min(minLine, line);
			maxLine = max(maxLine, line);
			minCol = min(minCol, col);
			maxCol = max(maxCol, col);
		}

	printf("%d %d %d %d \n", minLine, maxLine, minCol, maxCol);


	FILE* fpall = fopen("c:\\temp\\allpr.txt", "w");
	FILE* fp1 = fopen("c:\\temp\\linecol-latlon.txt", "w");
	for(int j=minLine; j<=maxLine; j++)
	{		
		for(int i=minCol; i<=maxCol; i++)
		{
			double tlon,tlat;
			LinecolToLatlonHigh(j, i, &tlon, &tlat);

			if(tlon>minlon && tlon<maxlon && tlat>minlat && tlat<maxlat)
			{	
				
				fprintf(fp1, "%d %d %lf %lf \n", j, i, tlat, tlon);

				char filename[256];
				sprintf(filename, "c:\\temp\\out1\\%d_%d_L2.txt", j, i);
				//stLSAP data = readParasolLevelTwo.GetPtMultiViewData(j, i);
				stMultiView data = readParasolLevelTwo.GetPtMultiViewDataOld(j, i);




				FILE* fp = fopen(filename, "w");
				for(int k=0; k<data.nView; k++)
				{
					fprintf(fp, "%6.4lf %6.4lf %6.4lf %6.4lf    %6.4lf %6.4lf %6.4lf \n", 
						data.mViewData[k].sza, data.mViewData[k].vza, data.mViewData[k].azi,
						data.mViewData[k].sca,
						data.mViewData[k].r6,
						data.mViewData[k].r8,
						data.mViewData[k].rp8);

					fprintf(fpall, "%6.4lf %6.4lf %6.4lf %6.4lf    %6.4lf %6.4lf %6.4lf \n", 
						data.mViewData[k].sza, data.mViewData[k].vza, data.mViewData[k].azi,
						data.mViewData[k].sca,
						data.mViewData[k].r6,
						data.mViewData[k].r8,
						data.mViewData[k].rp8);

				}
				fclose(fp);
			}
		}
	}
	fclose(fp1);
	fclose(fpall);

	printf("Finished ! \n");

}

void CProject_PGStationDlg::OnSinusoidalRowcol()
{
	// TODO: Add your command handler code here

	int line,col;

	line = 901;
	col  = 4849;

	//capital airport
	line = 899;
	col  = 4846;

	double lat,lon;
	LinecolToLatlonHigh(line, col, &lon, &lat);

	printf(" line: %d  col: %d   lat: %lf  lon: %lf  \n", line, col, lat, lon);


	/*
	for( line = 883; line<=918; line++)
		for( col = )

	*/
}

void CProject_PGStationDlg::OnPanoramicRelativeposeestimation()
{
	// TODO: Add your command handler code here


	char* matchPtFile = "F:\\Data\\panorama\\Matching\\0-1.txt";

	int row = GetFileRows(matchPtFile);

	vector<Point3DDouble> lpts;
	vector<Point3DDouble> rpts;

	lpts.resize(row);
	rpts.resize(row);

	int wd = 2048;
	double radius = (double)(wd) / (2*PI);

	//read match points
	FILE* fp = fopen(matchPtFile, "r");
	for(int i=0; i<row; i++)
	{
		double x1,y1,x2,y2;
		fscanf(fp, "%lf %lf %lf %lf ", &x1, &y1, &x2, &y2 );
		lpts[i].p[0] = x1;
		lpts[i].p[1] = y1;
		rpts[i].p[0] = x2;
		rpts[i].p[1] = y2;
	}
	fclose(fp);

	//converted to spherical coordinates
	v3_t* lp = new v3_t[row];
	v3_t* rp = new v3_t[row];
	for(int i=0; i<row; i++)
	{
		double gx,gy,gz;
		double sx = lpts[i].p[0];
		double sy = lpts[i].p[1];			
		SphereTo3D(sx,sy,radius,gx,gy,gz);
		lpts[i].p[0] = gx;
		lpts[i].p[1] = gy;
		lpts[i].p[2] = gz;

		sx = rpts[i].p[0];
		sy = rpts[i].p[1];			
		SphereTo3D(sx,sy,radius,gx,gy,gz);
		rpts[i].p[0] = gx;
		rpts[i].p[1] = gy;
		rpts[i].p[2] = gz;
	}

	//relative pose estimation
	double R[9];
	double t[3];
	vector<double> residual;
	residual.resize(100);

	EstimatePose5Point_Pano(lpts, rpts, radius, 500, 3, R, t, residual);
}

void CProject_PGStationDlg::OnPanoramicMatching()
{
	// TODO: Add your command handler code here

	if( m_imagePointerVector.size()>1 )
	{
			char* file1 = m_imagePointerVector[0]->GetFilePath();
			char* file2 = m_imagePointerVector[1]->GetFilePath();
	
			IplImage* lImage = cvLoadImage(file1);
			IplImage* rImage = cvLoadImage(file2);
	
			printf("%s \n", file1);
			printf("%s \n", file2);
	
			//1. feature detection
			CFeatureBase* pFeatDetect = new CSIFTFloat(); 
			//CFeatureBase* pFeatDetect = new CSURF(); 
			ImgFeature lImageFeats;
			ImgFeature rImageFeats;
			pFeatDetect->Detect(file1, lImageFeats);
			pFeatDetect->Detect(file2, rImageFeats);
			
			//convert from spherical to real 3D
			int ht = lImage->height;
			int wd = lImage->width;
	
			/*
			double radius = (double)(wd) / (2*PI);
			for(int i=0; i<lImageFeats.featPts.size(); i++)
			{
				double gx,gy,gz;
				double sx = lImageFeats.featPts[i].x;
				double sy = lImageFeats.featPts[i].y;			
				SphereTo3D(sx,sy,radius,gx,gy,gz);
				lImageFeats.featPts[i].x = gx/gz;
				lImageFeats.featPts[i].y = gy/gz;			
			}
			for(int i=0; i<rImageFeats.featPts.size(); i++)
			{
				double gx,gy,gz;
				double sx = rImageFeats.featPts[i].x;
				double sy = rImageFeats.featPts[i].y;			
				SphereTo3D(sx,sy,radius,gx,gy,gz);
				rImageFeats.featPts[i].x = gx/gz;
				rImageFeats.featPts[i].y = gy/gz;			
			}
			*/
			
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
			EstimatePose5Point_Pano(lptPano, rptPano, radius, 500, 2.5, R, T, residual);

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
			     

			//4. remove outliers and display
			srand(100);
			IplImage* pMosaic = VerticalMosaic(lImage, rImage);

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

					cvDrawCircle(lImage, pl, 2, CV_RGB(r,g,b),2);
					cvDrawCircle(rImage, pr, 2, CV_RGB(r,g,b),2);
					cvDrawLine(pMosaic, pl, cvPoint( pr.x, pr.y+ht ), CV_RGB(255,0,0));
				}				
			}
	
			/*
			cvNamedWindow("Match1");
			cvShowImage("Match1", lImage);
			cvNamedWindow("Match2");
			cvShowImage("Match2", rImage);
			*/

			cvSaveImage("c:\\temp\\left.jpg",  lImage);
			cvSaveImage("c:\\temp\\right.jpg", rImage);
			cvSaveImage("c:\\temp\\match.jpg", pMosaic);
			cvReleaseImage(&pMosaic);		


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
					Point3DDouble gp = TriangulatePt(lp, rp, R0, T0, R, T, &error);
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


			//projection from panorama to plane 
			int nAngleStep = 30;

			




		}	
}

void CProject_PGStationDlg::OnPanoramicSimulation()
{
	// TODO: Add your command handler code here	
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
	GenerateRMatrix(omiga, phi, kapa, R2);
	t2[0] = 2; t2[1] = -1; t2[2] = 0.1;

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
	EstimatePose5Point_Pano(lptPano, rptPano, radius, 500, 2.5, R, T, residual);

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
		Point3DDouble gp = TriangulatePt(lp, rp, R0, T0, R, T, &error); //triangulate(lp, rp, R0, T0, R, T, &error);
		printf("%8.2lf %8.2lf %8.2lf | %8.2lf %8.2lf %8.2lf  err:%lf \n", 
			grdpt[i][0], grdpt[i][1], grdpt[i][2], 
			gp.p[0], gp.p[1], gp.p[2], error);
	}

}


void CProject_PGStationDlg::OnCalibrationOpencv()
{
	// TODO: Add your command handler code here
	CreateChessBoard();
}

void CProject_PGStationDlg::OnPanoramicReadingpos()
{
	// TODO: Add your command handler code here
	char* filepath = "F:\\Data\\panorama\\ladybug_jpg\\ladybug_jpg\\0-40_jpg.cam";
	
	char sline[512];
	char sname[256];
	double lon,lat,gx,gy,gz,ax,ay,az;
	char   sGpsTime[256];
	char   sDay[256];
	char   sTime[256];

	//position of camera
	vector<double> vgx;
	vector<double> vgy;
	vector<double> vgz;
	//rotation angle of camera
	vector<double> vax;
	vector<double> vay;
	vector<double> vaz;

	FILE* fp = fopen(filepath, "r");
	fgets(sline, 512, fp);
	while(!feof(fp))
	{
		fscanf( fp, "%s  %lf %lf   %lf %lf %lf  %lf %lf %lf   %s %s %s ", sname, 
			&lat, &lon, &gx, &gy, &gz, &ax, &ay, &az, sGpsTime, sDay, sTime ); 
			
		//position of camera
		vgx.push_back(gx);
		vgy.push_back(gy);
		vgz.push_back(gz);
		
		//rotation angle
		ax = ax; // / PI * 180;
		ay = ay; // / PI * 180;
		az = az; // / PI * 180;
		vax.push_back(ax);
		vay.push_back(ay);
		vaz.push_back(az);
	}
	fclose(fp);

	fp = fopen("c:\\temp\\pos-relativepose.txt", "w");
	fprintf(fp, "pitch	 roll	yaw	 t0	 t1	 t2 \n");
	for(int i=0; i<vax.size()-1; i++)
	{
		printf("%d-%d \n", i, i+1);
		//fprintf(fp, "%d-%d \n", i, i+1);
		
		double R1[9], T1[3];
		double R2[9], T2[3];

		//double omiga,phi,kapa; //x,y,z
		GenerateRMatrixDirect(vay[i],   vax[i],   vaz[i], R1);
		GenerateRMatrixDirect(vay[i+1], vax[i+1], vaz[i+1], R2);
		
		/*
		//input must be degree
		double roll,pitch,yaw;
		roll=vax[0];	pitch=vay[0];	yaw=vaz[0];
		GenerateRMatrix(-pitch, -roll, yaw+90, R1);
		roll=vax[1];	pitch=vay[1];	yaw=vaz[1];
		GenerateRMatrix(-pitch, -roll, yaw+90, R2);
		*/

	   
		//convert to rotation angle
		invers_matrix(R1, 3);
		double R12[9];
		mult(R2, R1, R12, 3, 3, 3);
		double ax1 = atan( R12[5]/R12[8] ) / PI*180; 
		double ay1 = asin( -R12[2] ) /PI*180;
		double az1 = atan( R12[1]/R12[0]) /PI*180;
		printf("rotation: %lf %lf %lf \n", ax1, ay1, az1);
		//fprintf(fp, "rotation: %lf %lf %lf \n", ax1, ay1, az1);

		//calculate the implicit T
		double tT[3];
		tT[0] = vgx[i] - vgx[i+1];
		tT[1] = vgy[i] - vgy[i+1];
		tT[2] = vgz[i] - vgz[i+1];	
		double resT[3];
		mult(R2, tT, resT, 3, 3, 1);
		double distance = sqrt(  resT[0]*resT[0] + resT[1]*resT[1] + resT[2]*resT[2]  );
		resT[0]=resT[0]/distance;
		resT[1]=resT[1]/distance;
		resT[2]=resT[2]/distance;
		printf("translation: %lf %lf %lf \n", resT[0], resT[1], resT[2]);
		//fprintf(fp, "translation: %lf %lf %lf \n", resT[0], resT[1], resT[2]);

		fprintf(fp, "%lf %lf %lf  %lf %lf %lf \n", ax1, ay1, az1, resT[0], resT[1], resT[2]);

		//calculate the explicit T
		double eT[3];
		CalculateExplicitT(R12, resT, eT);

		/*
		mult(R1, tT, eT, 3, 3, 1);
		eT[0] = -eT[0];
		eT[1] = -eT[1];
		eT[2] = -eT[2];
		distance = sqrt(  eT[0]*eT[0] + eT[1]*eT[1] + eT[2]*eT[2]  );
		eT[0]=eT[0]/distance;
		eT[1]=eT[1]/distance;
		eT[2]=eT[2]/distance;
		*/
		//printf("translation: %lf %lf %lf \n", eT[0], eT[1], eT[2]);
		//fprintf(fp, "translation: %lf %lf %lf \n", eT[0], eT[1], eT[2]);
		
	}
	fclose(fp);
}

void CProject_PGStationDlg::OnPanoramicEpipolarimage()
{
	// TODO: Add your command handler code here

	if( m_imagePointerVector.size()>1 )
	{
		char* file1 = m_imagePointerVector[0]->GetFilePath();
		char* file2 = m_imagePointerVector[1]->GetFilePath();

		//IplImage* lImage = cvLoadImage(file1);
		//IplImage* rImage = cvLoadImage(file2);

		FILE* fp = fopen("c:\\temp\\pano_rt.txt", "r");
		if(fp==NULL)
			return;

		double T[3];
		double R[9];
		fscanf(fp, " %lf %lf %lf ", &(T[0]), &(T[1]), &(T[2]) );
		for(int j=0; j<3; j++)
		{
			for(int i=0; i<3; i++)
			{
				fscanf(fp, " %lf ", &(R[j*3+i]) );
			}
		}
		fclose(fp);

		GeneratePanoEpipolarImageHeading(R, T, file1, file2);
		GeneratePanoEpipolarImage(R, T, file1, file2);
	}
}

void CProject_PGStationDlg::OnMosaicGeotiff()
{
	// TODO: Add your command handler code here

	char imgPath[256] = "F:\\Data\\Mosaic\\sea";

	int n,nfile;
	n = 0;
	nfile = 0;
	GetDirFileName(NULL, imgPath, &n, &nfile, "tif", 0);
	printf("number of images: %d \n", nfile);
	char** blendFiles = f2c(nfile, 256);
	GetDirFileName(blendFiles, imgPath, &n, &nfile, "tif", 1);

	
	//blend 
	printf("Blending... \n");
	CBlenderBase *pBlend = new CMultiBandBlender();
	pBlend->GeoBlend(blendFiles, nfile, "c:\\temp\\blend.tif");
	delete pBlend;

	//mosaic
	GeoMosaicWithBalance(blendFiles, nfile, "c:\\temp\\mosaic.tif", GDT_Byte);



	printf("Finished ! \n");

}

void CProject_PGStationDlg::OnMosaicBundler()
{
	// TODO: Add your command handler code here

	//char* bundlefile   = "E:\\SoftProject\\Assemble\\SFM\\CPU\\32bit\\Release\\bundle\\bundle.out";
	//char* listfile   = "F:\\Data\\UAV\\wjg\\raw\\list.txt";
	//char* listfile   = "F:\\Data\\UAV\\CNU_Campus\\raw\\Line_4\\list.txt";
	
	//char* bundlefile   = "F:\\Data\\UAV\\巴彦淖尔\\2-sub\\product\\bundle.out";
	//char* listfile     = "F:\\Data\\UAV\\巴彦淖尔\\2-sub\\product\\list.txt";
	//char* bundlefile   = "F:\\Data\\UAV\\xrk\\1\\bundle.out";
	//char* listfile     = "F:\\Data\\UAV\\xrk\\1\\list.txt"; 

	char* bundlefile   = "c:\\sfm\\bundle.out";
	char* listfile     = "c:\\sfm\\list.txt"; 
	char* outfile      = "c:\\temp\\orthoMosaic.tif";
	char* smoothfile   = "c:\\sfm\\fpt.txt";

	MosaicOnBundleWithDEM(listfile, bundlefile, smoothfile, NULL, 1, outfile, 0);
	
}

void CProject_PGStationDlg::OnFileheaderJpeg()
{
	// TODO: Add your command handler code here

	char* filename = "F:\\Data\\OceanImages\\IOCAS_20160717_026\\165\\165_Stills\\165_0000\\image_D2016-07-17T06-50-18-484650Z_0.jpg";

	int ht,wd;
	double focalLen;
	double ccdWidth;
	double lat,lon,alt;
	int GpsInfoPresent;
	char CameraMake[256];
	char CameraModel[256];

	GetInfoFromJpegExif(filename, &ht, &wd, /*CameraMake, CameraModel,*/
		&focalLen, &ccdWidth, 
		&lat, &lon, &alt, &GpsInfoPresent);


	printf(" ht:%d  wc:\\temp%d  f:%lf  ccc:\\temp %lf  %lf %lf %lf ", /*CameraMake, CameraModel, */ ht, wd, focalLen, ccdWidth, lat, lon, alt);


	char* exifComment;
	GetJpegExifComment(&exifComment);

	//parse xml
	
	tinyxml2::XMLDocument doc;	
	if( doc.Parse(exifComment) != XML_SUCCESS )
		return;

	XMLElement* pRoot = doc.RootElement();
	XMLElement* pNode = NULL;
	const char* pText = NULL;

	if(pRoot) 
	{
		 cout << pRoot->Value() << endl;

		/*
		pNode = pRoot->FirstChildElement("Position")->FirstChildElement(); 
		//find every node
		while( pNode!=NULL )
		{		
			pText = pNode->GetText();
			printf("%s: %s \n", pNode->Name(), pText);
			pNode = pNode->NextSiblingElement();
		}*/

		XMLNode* child = pRoot->FirstChild();
		const char* name = child->ToElement()->GetText();  
		
		
		pNode = pRoot->FirstChildElement("Position"); 
		pText = pNode->GetText();
		//double upLeftX = atof(pText);
		printf("satelliteIc:\\temp %s \n", pText);
		
	}

}


void CProject_PGStationDlg::OnMosaicBundler33231()
{
	// TODO: Add your command handler code here

	char* imagepath = "F:\\Data\\UAV\\临时测试 xrk 巴彦淖尔";
	int   nLevel = 2;

	char dir_path[1024];
	sprintf(dir_path, "%s", imagepath);
	printf("%s\n",dir_path);
	
	char list_path[1024];
	char cam_path[1024], pt_path[1024], out_path[1024];

	//input
	sprintf(list_path,"%s\\List.txt",dir_path);
	sprintf(cam_path,"%s\\cam.txt",dir_path);
	sprintf(pt_path,"%s\\pt.txt",dir_path);

	//output
	sprintf(out_path,"%s\\ortho.tif",dir_path);


	MosaicOnBundleWithWYOut(list_path, cam_path, pt_path, nLevel, out_path);	
	
}

void CProject_PGStationDlg::OnPbaCpu()
{
	// TODO: Add your command handler code here
//
//	int argc = 3;
//	char** argv = f2c(3,256);
//	strcpy(argv[0], "pba.exe");
//	
//	//////////////////////////////////////////////////////
//	char* input_filename  = "F:\\data" ; //argv[1];            //first argument must be filename
//	char* driver_argument = "--float" ; //argv[argc - 1];    //last argument  for the driver
//
//	strcpy(argv[1], input_filename);
//	strcpy(argv[2], driver_argument);
//
//	/////////////////////////////////////////////////////////////////
//	//CameraT, Point3D, POINT2 are defined in src/pba/DataInterface.h
//	vector<CameraT>        camera_data;    //camera (input/ouput)
//	vector<Point3D>        point_data;     //3D point(iput/output)
//	vector<Point2D>        measurements;   //measurment/projection vector
//	vector<int>            camidx, ptidx;  //index of camera/point for each projection
//
//	/////////////////////////////////////
//	vector<string>         photo_names;        //from NVM file, not used in bundle adjustment
//	vector<int>            point_color;        //from NVM file, not used in bundle adjustment
//
//	/////////////////////////////////////////////////////////////
//	///load the data. You will need to replace the loader for your own data
//	
//	if(argc < 2 || ! LoadModelFile(input_filename, camera_data, point_data, measurements,
//		ptidx, camidx, photo_names, point_color))
//	{
//		stc:\\temp:cout << "==== Multicore Bundle Adjustment ---- Demo Driver Syntax ====\n"
//#ifdef WIN32
//			"driver(_x64)(_debug) "
//#else
//			"driver "
//#endif
//			"input [pba parameters][-out output_nvm][driver argument]\n"
//			"	input:            file must be NVM or bundler format\n"
//			"[driver argument] must be the last one. It can be one of the following\n"
//			"	--noise:          add 5% random noise to all parameters\n"
//			"	--float:          use CPU-float implementation instead of GPU\n"
//			"	--double:         use CPU-double implementation instead of GPU\n"
//			"[pba parameters]: -lmi <#>, -cgi <#>, -cgn <f>, -budget <#>...\n"
//			"	-lmi <#>:         specify the number of LM iterations\n"
//			"	-profile:         run profiling experiments and see the speeds\n"
//			"	                  check documentation for more details.\n";
//		//return 0;
//	}else
//	{
//		//if(strstr(driver_argument, "--checkv")) ExamineVisiblity(input_filename);
//
//		//remove file extension for conversion/saving purpose
//		char* dotpos = strrchr(input_filename, '.');
//		if(dotpos && strchr(dotpos, '/') == NULL && strchr(dotpos, '\\') == NULL) *dotpos = 0;
//
//		//////////////////////////////////////////////////////////
//		//use the last parameter for special purpose.
//		string surfix = "-converted";
//		if(strstr(driver_argument, "--fix_visibility"))
//		{
//			if(RemoveInvisiblePoints(camera_data, point_data, ptidx, camidx, measurements, photo_names, point_color))
//				surfix = "-fixed";
//		}else
//		{
//			if(strstr(driver_argument, "--noise") != NULL)     //add noise for experimentation
//			{
//				//AddNoise(camera_data, point_data, 0.05f);    //add 5% noise for experiments
//				AddStableNoise(camera_data, point_data, ptidx, camidx, 0.05f);
//				surfix = "-noised";
//			}
//		}
//
//		//file format conversion for experimentation
//		if(strstr(driver_argument, "--convert_to_nvm"))
//			SaveNVM(    (string(input_filename) + surfix + ".nvm").c_str(), camera_data, 
//			point_data, measurements, ptidx, camidx, photo_names, point_color); 
//		if(strstr(driver_argument, "--convert_to_bm"))
//			SaveBundlerModel(    (string(input_filename) + surfix + ".txt").c_str(), 
//			camera_data, point_data, measurements, ptidx, camidx);
//	}
//
//	/////////////////////////////////////////////////////////////////////////////////////////
//	ParallelBA::DeviceT device = ParallelBA::PBA_CUDA_DEVICE_DEFAULT;
//	if(strstr(driver_argument, "--float"))          device = ParallelBA::PBA_CPU_FLOAT;
//	else if(strstr(driver_argument, "--double"))    device = ParallelBA::PBA_CPU_DOUBLE;
//
//	/////////////////////////////////////////////////////////////////////
//	ParallelBA pba(device);          //You should reusing the same object for all new data
//
//	/////////////////////////////////////////////////////////
//	//Parameters can be changed before every call of RunBundleAdjustment
//	//But do not change them from another thread when it is running BA.
//	pba.ParseParam(argc, argv);      //indirect parameter tuning from commandline
//	//pba.SetFixedIntrinsics(true); //if your focal lengths are calibrated. 
//	//           equivalent to pba.GetInternalConfig()->__fixed_focallength = true;
//	//pba.EnableRadialDistortion(*); // if you want to enable radial distortion
//	//           equivalent to pba.GetInternalConfig()->__use_radial_distortion = *;
//	//check src/pba/ConfigBA.h for more details on the parameter system
//
//	////////////////////////////////////
//	//Tweaks before bundle adjustment
//	//1. For each camera, you can call CameraT::SetConstantCamera() to keep it unchanged.
//	//2. pba.SetNextBundleMode(ParallelBA::BUNDLE_ONLY_MOTION) //chose a truncated mode?
//
//	////////////////////////////////////////////////////////////////
//	pba.SetCameraData(camera_data.size(),  &camera_data[0]);                         //set camera parameters
//	pba.SetPointData(point_data.size(), &point_data[0]);                             //set 3D point data
//	pba.SetProjection(measurements.size(), &measurements[0], &ptidx[0], &camidx[0]); //set the projections
//
//	vector<int> cmask;
//	if(strstr(driver_argument, "--common"))
//	{
//		cmask.resize(camera_data.size(), 0);
//		pba.SetFocalMask(&cmask[0]);
//	}
//	//WARNING: measumrents must be stored in correct order
//	//all measutments (in different images) for a single 3D point must be stored continously, 
//	//Currently, there is no order verification internally.
//	//Basically, ptidx should be non-decreasing 
//
//	//////////////////////////////////////////////////////
//	//pba.SetTimeBudget(10);      //use at most 10 seconds?
//	pba.RunBundleAdjustment();    //run bundle adjustment, and camera_data/point_data will be modified
//
//
//	//Write the optimized system to file
//	const char*  outpath = pba.GetInternalConfig()->GetOutputParam();
//	SaveModelFile(outpath, camera_data, point_data, measurements, ptidx, camidx, photo_names, point_color);
//	//It is easy to visualize the camera/points yourself, 
}

void CProject_PGStationDlg::OnMatchingSimpleflow()
{
	// TODO: Add your command handler code here

	//char* lfile = "F:\\Data\\panorama\\ladybug_jpg\\ladybug_jpg\\ladybug_panoramic_000000.jpg";
	//char* rfile = "F:\\Data\\panorama\\ladybug_jpg\\ladybug_jpg\\ladybug_panoramic_000001.jpg";

	char* lfile = "c:\\temp\\out\\tsukuba_l.png";
	char* rfile = "c:\\temp\\out\\tsukuba_r.png";
	//char* lfile = "F:\\Data\\panorama\\ladybug_jpg\\ladybug_jpg\\ladybug_panoramic_000000.jpg_left.jpg";
	//char* rfile = "F:\\Data\\panorama\\ladybug_jpg\\ladybug_jpg\\ladybug_panoramic_000001.jpg_left.jpg";
	char* outfile = "c:\\temp\\flow.flo";
	
	vector<double> tx;
	vector<double> ty;
	int ht, wd;
	SimpleFlow(lfile, rfile, outfile, ht, wd, tx, ty);


}

void CProject_PGStationDlg::OnPanoramicDensepointcloud()
{
	// TODO: Add your command handler code here

	//loading images
		
	//reading relative pose parameters
	
	//reading dense matching results

}

void CProject_PGStationDlg::OnPanoramicRelativeposebatch()
{
	// TODO: Add your command handler code here

	char* imagepath = "F:\\Data\\panorama\\ladybug_jpg\\ladybug_jpg";
    
	char** filenames = NULL;
	int n=0;
	int nfile =0;
	GetDirFileName(filenames, imagepath, &n, &nfile, "jpg", 0);
	filenames = f2c(nfile, 256);
	GetDirFileName(filenames, imagepath, &n, &nfile, "jpg", 1);

	FILE* fp = fopen("c:\\temp\\5point_relativepose.txt", "w");
	fprintf(fp, "pitch	 roll	 yaw	 t0	 t1	 t2 \n");
	for(int i=0; i<nfile-1; i++)
	{
			printf("%d-%d \n", i, i+1);

			IplImage* lImage = cvLoadImage(filenames[i]);
			//IplImage* rImage = cvLoadImage(filenames[i+1]);
	
			//printf("%s \n", file1);
			//printf("%s \n", file2);
	
			//1. feature detection
			CFeatureBase* pFeatDetect = new CSIFTFloat(); 
			//CFeatureBase* pFeatDetect = new CSURF(); 
			ImgFeature lImageFeats;
			ImgFeature rImageFeats;
			pFeatDetect->Detect(filenames[i], lImageFeats);
			pFeatDetect->Detect(filenames[i+1], rImageFeats);
			
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
			EstimatePose5Point_Pano(lptPano, rptPano, radius, 500, 2.5, R, T, residual);

			double eT[3];
			CalculateExplicitT(R, T, eT);

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
			fprintf(fp, "%lf %lf %lf  %lf %lf %lf \n", ax, ay, az, T[0], T[1], T[2]);
	}
	fclose(fp);
}

void CProject_PGStationDlg::OnTransformRotateatob()
{
	// TODO: Add your command handler code here

	double a[3] = {0, 1, 0};
	double b[3] = {0.02, 0.9999, -0.034};

	/*
	double R1[9];
	CalculateAlignRotation(a, b, R1);

	for(int j=0; j<3; j++)
	{
		for(int i=0; i<3; i++)
		{
			printf("%lf ", R1[j*3+i]);
		}
		printf("\n");
	}
	*/

	double R2[9];
	CalculateAlignRotation1(a, b, R2);

	double Rt[9];
	transpose(R2, Rt, 3, 3);
	double rm[9];
	mult(Rt, R2, rm, 3, 3, 3);
	printf("Rt*R ...  \n");
	for(int j=0; j<3; j++)
	{
		for(int i=0; i<3; i++)
		{
			printf("%lf ", rm[j*3+i]);
		}
		printf("\n");
	}
 
	 
	printf(" \n ................................. \n");
	for(int j=0; j<3; j++)
	{
		for(int i=0; i<3; i++)
		{
			printf("%lf ", R2[j*3+i]);
		}
		printf("\n");
	}

	double res[3];
	mult(R2, a, res, 3, 3, 1);
    
	printf(" \n ................................. \n");
	printf("%lf %lf %lf \n", b[0], b[1], b[2]);
	printf("%lf %lf %lf \n", res[0], res[1], res[2]);
	printf("%lf %lf %lf \n", b[0]/res[0], b[1]/res[1], b[2]/res[2]);
}

void CProject_PGStationDlg::OnFusionLog()
{
	// TODO: Add your command handler code here


	char* filename = "c:\\temp\\DSC04583_geotag.tif";

	IplImage* pImage = cvLoadImage(filename, 0);

	int ht = pImage->height;
	int wd = pImage->width;
	int scanWd = pImage->widthStep;

	unsigned char* pIntImage = (unsigned char*)malloc(ht*wd*sizeof(unsigned char));	
	for(int j=0; j<ht; j++)
		for(int i=0; i<wd; i++)
		{
			pIntImage[j*wd+i] = pImage->imageData[j*scanWd+i];
		}

	PYRAMID_IMAGE_GENERAL<unsigned char> pyraImage; // = new PYRAMID_IMAGE_GENERAL();
	ConstructPyramid(pIntImage, ht, wd, pyraImage, 7);

	PYRAMID_IMAGE_GENERAL<unsigned char> laplacianPI;// = new PYRAMID_IMAGE_GENERAL();
	ConstructLaplacianPyramid(pyraImage, laplacianPI);
	
	unsigned char* reconstructImage = NULL;
	int  rht,rwd;
	ReconstrucFromLaplacian(laplacianPI, &reconstructImage, &rht, &rwd);
 
	SaveBmp("c:\\temp\\log_reconstruction.bmp", reconstructImage, rht, rwd);

	FreePyramidImageGeneral(pyraImage);
	FreePyramidImageGeneral(laplacianPI);

	cvReleaseImage(&pImage);

	printf("Finished ! \n");
}

void CProject_PGStationDlg::OnFusionLogsequences()
{
	// TODO: Add your command handler code here

	//char* imagepath = "F:\\Data\\UAV\\CNU_Campus\\raw\\Line_2";
	//char* imagepath = "F:\\Data\\UAV\\新疆\\盐场\\DCIM\\100EOS5D";
	//char* imagepath = "F:\\Data\\UAV\\byler\\1-sub\\test";
	//char* imagepath = "F:\\Data\\UAV\\广西\\10105\\product";
	//char* imagepath = "F:\\Data\\UAV\\byler\\2-sub-1\\product";
	char* imagepath = "F:\\Data\\MultiBand\\100m_TIF10\\product";
	//char* imagepath = "F:\\Data\\TestData\\product";
	//char* imagepath = "F:\\Data\\TestData\\product";


	char** filenames = NULL;
	int n=0;
	int nfile=0;
	
	GetDirFileName(filenames, imagepath, &n, &nfile, "tif", 0);
	filenames = f2c(nfile, 256);
	GetDirFileName(filenames, imagepath, &n, &nfile, "tif", 1);

	//stGeoInfo geoInfo;
	//GetGeoInformation( filenames[0], geoInfo);
     
	//LoGBlend(filenames, nfile, "c:\\temp\\blend.tif", 2);
	//BlendMosaic(filenames, nfile, "c:\\temp\\blend1.tif", 0.1, 0);
	//BlendMosaic(filenames, nfile, "c:\\temp\\blend1.tif", 0.1, 0);
	//GeoTiffBlend(filenames, 10, "c:\\temp\\blend1.tif", 0.2);
	MosaicGeoTiff(filenames, nfile, "c:\\temp\\directMosaic.tif", 0.0008, 1);
	
}

void CProject_PGStationDlg::OnXmlOpencv()
{
	// TODO: Add your command handler code here

	//string filename = "c:\\temp\\camera.yml";

	TestFileStorageWrite();
	TestFileStorageRead();
}

void CProject_PGStationDlg::OnMrtGeneratefilelists()
{
	// TODO: Add your command handler code here

	char* imagepath = "F:\\Data\\Modis\\MOD09";

	char** filenames = NULL;
	int n,nfile;

	n=nfile=0;
	GetDirFileName(filenames, imagepath, &n, &nfile, "hdf", 0);
	filenames = f2c(nfile, 256);
	GetDirFileName(filenames, imagepath, &n, &nfile, "hdf", 1);


	FILE* fp = fopen("c:\\temp\\hdflist.txt", "w");

	for(int i=0; i<nfile; i++)
	{
		printf("%s \n", filenames[i]);
		fprintf(fp, "%s \n", filenames[i]);
	}
	fclose(fp);

	printf("Over! \n");
}

void CProject_PGStationDlg::OnModisGoetiff()
{
	// TODO: Add your command handler code here

	char* filename = "F:\\Data\\Modis\\output\\h24v04.sur_refl_b01.tif";

	//int* pBuffer = NULL;
	//int ht,wd;
	//ReadGeoFileInt(filename, 1, &pBuffer, ht, wd);

	short* pBuffer = NULL;
	int ht,wd;
	ReadGeoFileShort(filename, 1, &pBuffer, ht, wd);
    
	int minValue = 100000000;
	int maxValue = -100000000;
	for(int i=0; i<ht*wd; i++)
	{
		minValue = min(minValue, pBuffer[i]);
		maxValue = max(maxValue, pBuffer[i]);
	}
	printf("%d %d \n", minValue, maxValue);
}

void CProject_PGStationDlg::OnForegroundBackgroundmodel()
{
	// TODO: Add your command handler code here

	char* videoFile = "F:\\Data\\video\\768x576.avi";

	ConstructBgModel(videoFile);
}

void CProject_PGStationDlg::OnPanoramicProjectiontoplane()
{
	// TODO: Add your command handler code here

	char* panofile = "F:\\Data\\panorama\\ladybug_jpg\\ladybug_jpg\\ladybug_panoramic_000001.jpg";
    double vangle = 60;
	double hangle = 90;
	double focalLenRation = 2;
	double direction[3];
	char outputfile[256];

	direction[0]=1;		direction[1]=0;		direction[2]=0;
	sprintf(outputfile, "%s_left.jpg", panofile);
	//PanoToPlane(panofile, outputfile, vangle, hangle, direction, focalLenRation);

	direction[0]=-1;		direction[1]=0;		direction[2]=0;
	sprintf(outputfile, "%s_right.jpg", panofile);
	//PanoToPlane(panofile, outputfile, vangle, hangle, direction, focalLenRation);



}



void CProject_PGStationDlg::OnKltPanoimages()
{
	// TODO: Add your command handler code here

	if(m_imagePointerVector.size()>0)
	{
		char leftFile[256]; 
		char rightFile[256];

		strcpy(leftFile, m_imagePointerVector[0]->GetFilePath());
		strcpy(rightFile, m_imagePointerVector[1]->GetFilePath());

		IplImage* pImage   = cvLoadImage(leftFile, 0);	
		IplImage* pCurrent = cvLoadImage(rightFile, 0);	

		IplImage* pImageColor   = cvLoadImage(leftFile, 1);	
		IplImage* pCurrentColor = cvLoadImage(rightFile, 1);	


		int ht = pImage->height;
		int wd = pImage->width;
		int scanwd = pImage->widthStep;

		cvSmooth(pImage, pImage);
		cvSmooth(pCurrent, pCurrent);

		//1. detect features	
		IplImage* eig = cvCreateImage( cvGetSize(pImage), 32, 1 );
		IplImage* temp = cvCreateImage( cvGetSize(pImage), 32, 1 );
		double quality = 0.02;
		double min_distance = 3;
		int win_size = 5;
		CvPoint2D32f* points = (CvPoint2D32f*)malloc(MAX_COUNT*sizeof(CvPoint2D32f));
		int count = MAX_COUNT;
		cvGoodFeaturesToTrack( pImage, eig, temp, points, &count,
			quality, min_distance, 0, 3, 0, 0.04 );
		cvReleaseImage( &eig );
		cvReleaseImage( &temp );

		/*
		cvFindCornerSubPix(
			pImage,
			points,
			count,
			cvSize(win_size,win_size),
			cvSize(-1,-1),
			cvTermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.03)
			);
		*/

		//2. optical flow
		//KLT optical flow tracking
		//CvPoint2D32f* points = (CvPoint2D32f*)malloc(MAX_COUNT*sizeof(CvPoint2D32f));
		CvPoint2D32f* npoints = (CvPoint2D32f*)malloc(MAX_COUNT*sizeof(CvPoint2D32f));
		char* status = (char*)malloc(count);
		IplImage* prev_pyramid = cvCloneImage(pImage);
		IplImage* pyramid = cvCloneImage(pImage);
		float* trackError = (float*)malloc( sizeof(float)*MAX_COUNT );
		memset( trackError, 0, sizeof(float)*MAX_COUNT );

		cvCalcOpticalFlowPyrLK( pImage, pCurrent, prev_pyramid, pyramid, points, npoints, count, 
			cvSize(31,31), 5, 
			status, 
			trackError, 
			cvTermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS, 20, 0.03), 
			0);

		for(int i=0; i<count; i++)
		{
			CvScalar color = CV_RGB(255,0,0);
			CvPoint tp;
			tp.x = points[i].x;
			tp.y = points[i].y;
			CvPoint cp;
			cp.x = npoints[i].x;
			cp.y = npoints[i].y;

			//int s = (unsigned char)(status[i]);
			
			int len = sqrt( (double)( (tp.x-cp.x)*(tp.x-cp.x) + (tp.y-cp.y)*(tp.y-cp.y))  );
			int ns = (unsigned char)(status[i]);
			//printf("%d \n", ns);

			if(ns>0)
			{
				//printf("status:%d error:%lf  %d %d  %d  %d \n", ns, trackError[i], tp.x, tp.y, cp.x, cp.y);
				//cvDrawCircle( pCurrent, cvPoint( tp.x,tp.y),3,CV_RGB(255,255,255),2,8,0);
				cvDrawCircle( pCurrent, cvPoint( cp.x,cp.y),2,CV_RGB(255,255,255),1,8,0);
				cvLine( pCurrent, cvPoint( tp.x,tp.y), cvPoint( cp.x, cp.y),CV_RGB(255,255,255),2,8,0);

				cvDrawCircle(pImageColor, cvPoint(tp.x, tp.y), 2,CV_RGB(255,0,0),1,8,0 );
				cvDrawCircle(pCurrentColor, cvPoint(cp.x, cp.y), 2,CV_RGB(255,0,0),1,8,0 );
			}
			else
			{
				cvLine( pCurrent, cvPoint( tp.x,tp.y), cvPoint( cp.x, cp.y),CV_RGB(150,150,150),1,8,0);
			}
		}

		cvNamedWindow("KLT");
		cvShowImage("KLT", pCurrent);
		
		cvSaveImage("c:\\temp\\left.jpg", pImageColor);
		cvSaveImage("c:\\temp\\right.jpg", pCurrentColor);
		
		cvWaitKey();
		
		free(npoints);
		free(trackError);
		free(status);
		cvReleaseImage(&pImage);
		cvReleaseImage(&pCurrent);
		cvReleaseImage(&pImageColor);
		cvReleaseImage(&pCurrentColor);

		cvReleaseImage(&prev_pyramid);
		cvReleaseImage(&pyramid);


	}
}

void CProject_PGStationDlg::OnCalibrationGeneratechessboard()
{
	// TODO: Add your command handler code here
	
	/*
	Mat calibration = Mat::zeros(1000, 1000, CV_8U);
	for (int i = 0; i < 999; i++)
	{
		for (int j = 0; j < 999; j++)
		{
			int ax = i / 100;
			int ay = j / 100;
			if ((ax + ay) % 2 != 0)
				calibration.at<uchar>(i, j) = 255;
		}
	}
	IplImage output = calibration;
	cvSaveImage("c:\\temp\\calibration.jpg", &output);
	*/
}

void CProject_PGStationDlg::OnColorcorrectionVignetting()
{
	// TODO: Add your command handler code here
    
	//char* filename = "F:\\Data\\vignetting\\flickr_3.jpg";
	//char* filename = "F:\\Data\\vignetting\\soapshop.jpg";
	//char* filename = "c:\\temp\\DSC04616_geotag.JPG";
	char* filename = "F:\\Data\\vignetting\\simulate\\M\\sea_Est_truth.jpg";
    


	/*
	IplImage* pImageColor = cvLoadImage(filename);
    //cvSaveImage("c:\\temp\\bak.jpg", pImageColor);

	IplImage* pGrayImage = cvLoadImage(filename, 0);


	int ht = pImageColor->height;
	int wd = pImageColor->width;
	int scanWd = pImageColor->widthStep;

	double ratio = 1;
	if(wd>64)
		ratio = 64.0 / double(wd);
		
	//resize the image
	int sht = pGrayImage->height*ratio + 0.5;
	int swd = pGrayImage->width*ratio  + 0.5;
	IplImage* pSmallImage = cvCreateImage( cvSize(swd, sht), 8, 1 );
	cvResize(pGrayImage, pSmallImage);
	//cvSaveImage("c:\\temp\\small.jpg", pSmallImage);
	unsigned char* pImageBuffer = (unsigned char*)malloc( pSmallImage->height*pSmallImage->width );
	for(int j=0; j<pSmallImage->height; j++)
		for(int i=0; i<pSmallImage->width; i++)
		{
			pImageBuffer[j*pSmallImage->width+i] = (unsigned char)(pSmallImage->imageData[j*pSmallImage->widthStep+i]);
		}
	
	//vignetting correction
	vector<double> vp;
	VignettingCorrectionUsingRG(pImageBuffer, pSmallImage->height, pSmallImage->width, vp);

	int nV = vp.size();
	for(int i=0; i<nV; i++)
	{
		vp[i] = exp(vp[i]);
		printf("%lf \n", vp[i]);
	}

	
	//apply the vignetting correction
	int halfHt = ht*0.5;
	int halfWd = wd*0.5;
	for(int j=0; j<ht; j++)
		for(int i=0; i<wd; i++)
		{
			double cx = i-halfWd;
			double cy = j-halfHt;

			double radius = sqrt(cx*cx+cy*cy) + 0.5;
			radius *= ratio;
            
			//linear interpolation
			double vValue = 1;
			int nR = int(radius);
			if( nR==0 )
			{
				vValue = vp[0];					
			}
			else if(nR<nV)
			{
				double dr = radius - nR;
				vValue = vp[nR-1]*(1-dr) + vp[nR]*dr;
			}
			else
			{
				vValue = vp[nV-1];
			}

			//radius = max(0, min(nV-1, radius) );			
			//double scale = 1.0 / vp[radius];
			double scale = 1.0/vValue;
						
			int r = (unsigned char)(pImageColor->imageData[j*scanWd+i*3]);
			int g = (unsigned char)(pImageColor->imageData[j*scanWd+i*3+1]);
			int b = (unsigned char)(pImageColor->imageData[j*scanWd+i*3+2]);
			r *= scale;
			g *= scale;
			b *= scale;
			
			//pImageColor->imageData[(ht-j-1)*scanWd+i*3]   = min(255,r);
			//pImageColor->imageData[(ht-j-1)*scanWd+i*3+1] = min(255,g);
			//pImageColor->imageData[(ht-j-1)*scanWd+i*3+2] = min(255,b);
			pImageColor->imageData[(j)*scanWd+i*3]   = min(255,r);
			pImageColor->imageData[(j)*scanWd+i*3+1] = min(255,g);
			pImageColor->imageData[(j)*scanWd+i*3+2] = min(255,b);
		}

		cvSaveImage("c:\\temp\\colorCorrect.jpg", pImageColor);

		cvReleaseImage(&pImageColor);
		cvReleaseImage(&pGrayImage);
		cvReleaseImage(&pSmallImage);
		free(pImageBuffer);
		*/

		VignettingCorrectFromFile(filename, "c:\\temp\\colorCorrect.jpg");


		printf("\n Finished ! \n");

}

void CProject_PGStationDlg::OnMatrixSvd()
{
	// TODO: Add your command handler code here

	int m=2;
	int n=2;

	double A[4] = {10,3,3,-1};

	double eps1 = 0.000001;
	int    ka = max(m,n)+1;;
    
	double u[4];
	double v[4];

	printf("A...\n");
	PrintMatrix(A, 2, 2);

	bmuav(A, 2, 2, u, v, eps1, ka);

	printf("S...\n");
	PrintMatrix(A, 2, 2);

	printf("u....\n");
	PrintMatrix(u, 2, 2);

	printf("v....\n");
	PrintMatrix(v, 2, 2);
}




void CProject_PGStationDlg::OnCrowddensityBackground()
{
	// TODO: Add your command handler code here

	char* videoPath = "F:\\Data\\video\\grandCenter\\grandcentral.avi";


	IplImage*       tmp_frame = NULL;
	CvCapture*      cap = NULL;

	cap = cvCaptureFromFile(videoPath);
	tmp_frame = cvQueryFrame(cap);
	if(!tmp_frame)
	{
		printf("bad video \n");
		exit(0);
	}

	cvNamedWindow("BG", 1);
	cvNamedWindow("FG", 1);

	/*
	//create BG model
	CvBGStatModel* bg_model = cvCreateFGDStatModel( tmp_frame );

	for( int fr = 1;tmp_frame; tmp_frame = cvQueryFrame(cap), fr++ )
	{
		double t = (double)cvGetTickCount();
		cvUpdateBGStatModel( tmp_frame, bg_model );
		t = (double)cvGetTickCount() - t;
		
		printf( "%.1f\n", t/(cvGetTickFrequency()*1000.) );
		cvShowImage("BG", bg_model->background);
		cvShowImage("FG", bg_model->foreground);

		char k = cvWaitKey(5);
		if( k == 27 ) break;
		//printf("frame# %d \r", fr);
	}


	cvReleaseBGStatModel( &bg_model );
	cvReleaseCapture(&cap);
	*/

}

void CProject_PGStationDlg::OnCharGenerateproductfile()
{
	// TODO: Add your command handler code here
	
	char* filename = "c:\\temp\\1.jpg";

	char* productfile;
	
	GenerateProductFile(filename, "product", "msk", &productfile);
	printf("%s \n", productfile);

	char* productPath;
	GenerateProductPath(filename, "product", &productPath);
	printf("%s \n", productPath);

	if(access(productPath, 0)!=0)
		mkdir(productPath);

}

void CProject_PGStationDlg::OnDebugFillblackpixel()
{
	// TODO: Add your command handler code here

	char* imagename = "F:\\Data\\UAV\\byler\\test1\\product\\DSC04582_geotag.tif";

	//reading image 
	IplImage* pResizeImage = cvLoadImage(imagename);
	
	int ht = pResizeImage->height;
	int wd = pResizeImage->width;
	int scanWd = pResizeImage->widthStep;
	unsigned char* pImageBuffer = (unsigned char*)malloc(ht*wd*sizeof(unsigned char));
	for(int kj=0; kj<ht; kj++)
		for(int ki=0; ki<wd; ki++)
		{
			pImageBuffer[kj*wd+ki] = (unsigned char)(pResizeImage->imageData[kj*scanWd + 3*ki]);
		}
	cvReleaseImage(&pResizeImage);	

	FillBlackPixel(pImageBuffer, ht, wd);

	SaveJpeg("c:\\temp\\fillblack.jpg", pImageBuffer, ht, wd);
}

void CProject_PGStationDlg::OnDebugGaussiansmooth()
{
	// TODO: Add your command handler code here
	int ht = 60;
	int wd = 80;
	
	unsigned char* pTestImage = (unsigned char*)malloc(ht*wd);
	memset(pTestImage, 0, ht*wd);
		
	for(int j=24; j<ht-24; j++)
		for(int i=24; i<wd-24; i++)
		{
			pTestImage[j*wd+i] = 255;
		}

	SaveToJpg(pTestImage, ht, wd, "c:\\temp\\dilate-no.jpg");
	MyDilate(pTestImage, ht, wd, 2);
	SaveToJpg(pTestImage, ht, wd, "c:\\temp\\dilate-yes.jpg");


	PYRAMID_IMAGE_GENERAL<unsigned char> pyramidImage; 
	ConstructPyramidWithMask(pTestImage, pTestImage, ht, wd, pyramidImage, 5);
    WritePyramidImage(pyramidImage);


	//generate laplacian pyramid of each image
	PYRAMID_IMAGE_GENERAL<short> laplacianPI; 
	ConstructLaplacianPyramid(pyramidImage, laplacianPI);
	WritePyramidImage(laplacianPI);


	//reconstruction 
	short* mosaicImage = NULL;
	int rht,rwd;
	ReconstrucFromLaplacian(laplacianPI, &mosaicImage, &rht, &rwd );
	unsigned char* pMosaicImage = (unsigned char*)malloc(rht*rwd);
	for(int i=0; i<rht*rwd; i++)
	{
		if( mosaicImage[i]<0 )
			mosaicImage[i] = 0;
		if( mosaicImage[i]>255 )
			mosaicImage[i] = 255;		
		
		pMosaicImage[i] = mosaicImage[i];		
	}

	SaveToJpg(pMosaicImage, rht, rwd, "c:\\temp\\reconstruction.jpg");

	/*
	SaveToJpg(pTestImage, ht, wd, "c:\\temp\\smooth-no.jpg");
	float k[5] = {0.05, 0.25, 0.4, 0.25, 0.05};
	int   size = 5;
	GaussianSmooth(pTestImage, pTestImage, ht, wd, k, size );
	SaveToJpg(pTestImage, ht, wd, "c:\\temp\\smooth-yes.jpg");
	*/

	printf("Finished ! \n");
}

void CProject_PGStationDlg::OnDebugMysmooth()
{
	// TODO: Add your command handler code here

	char* filename = "c:\\temp\\pyramid_5.jpg";
	IplImage* pImage = cvLoadImage(filename, 0);

	int ht = pImage->height;
	int wd = pImage->width;
	int scanwd = pImage->widthStep;
	unsigned char* pBuffer = (unsigned char*)malloc(ht*wd);
	
	for(int j=0; j<ht; j++)
		for(int i=0; i<wd; i++)
		{
			pBuffer[j*wd+i] = (unsigned char)pImage->imageData[j*scanwd+i];
		}

	SmoothImage(pBuffer, ht ,wd, 0, 2);
	//SmoothImage(pBuffer, ht ,wd, 0, 2);
	//SmoothImage(pBuffer, ht ,wd, 0, 2);
	SaveBmp("c:\\temp\\smooth.bmp", pBuffer, ht, wd);

	free(pBuffer);
	cvReleaseImage(&pImage);


	printf("finished ! \n");

}

void CProject_PGStationDlg::OnGdalWritejpeg()
{
	// TODO: Add your command handler code here

	char* filename = "c:\\temp\\china.tif";

	IplImage* pImage = cvLoadImage(filename, 1);

	int ht = pImage->height;
	int wd = pImage->width;
	int scanwd = pImage->widthStep;


	GDALAllRegister();
	char **papszOptions = NULL;

	char* drvName="MEM";
	GDALDriver *memDriver;
	memDriver = GetGDALDriverManager()->GetDriverByName(drvName);
	GDALDataset* ds_preview = memDriver->Create("default", wd, ht, 3, GDT_Byte, papszOptions);

	unsigned char* pBuffer = (unsigned char*)malloc(ht*wd);

	for(int k=0; k<3; k++)
	{	
		for(int j=0; j<ht; j++)
			for(int i=0; i<wd; i++)
			{
				pBuffer[j*wd+i] = (unsigned char)( pImage->imageData[j*scanwd+3*i+k] );
			}
		//GdalWriteJpgCopy("c:\\temp\\china.jpeg", pBuffer, ht, wd);

		GDALRasterBand* ba_preview=ds_preview->GetRasterBand(3-k);
		ba_preview->RasterIO(GF_Write, 0, 0, wd, ht, pBuffer, wd, ht, GDT_Byte, 0, 0);
	}

	char* jpgDrvName="JPEG";
	GDALDriver *jpgDriver=GetGDALDriverManager()->GetDriverByName(jpgDrvName);
	GDALDataset *dsjpg=jpgDriver->CreateCopy("c:\\temp\\china.ort", ds_preview,0,NULL,NULL,NULL);


	//close
	GDALClose( (GDALDatasetH) ds_preview );
	GDALClose( (GDALDatasetH) dsjpg );

	free(pBuffer);
	cvReleaseImage(&pImage);

	printf("Finished ! \n");

}

void CProject_PGStationDlg::OnShadowLitvinov()
{
	// TODO: Add your command handler code here

	double number = 30;
	double step = PI / number;

	double kr = 0.6;

	FILE* fp = fopen("c:\\temp\\litvinov.txt", "w");

	for(int i=0; i<=number; i++)
	{
		//double t = (1+cos( kr*(PI-i*step) )) * 0.5;
		//double s = t*t*t;

		double s = ShadowFunctionLitvinov(kr, i*step);

		double angle = i*step / PI * 180;

		fprintf(fp, "%lf %lf \n", angle, s);
		printf("%lf %lf \n", angle, s);
	}

	fclose(fp);
}

void CProject_PGStationDlg::OnShadowRoujean()
{
	// TODO: Add your command handler code here

	double phi  = 180.0;
	double thes = 45.0;    //sun zenith
	double thev = 0.0;
    
	FILE* fp = fopen("c:\\temp\\roujeanF1.txt", "w");

	phi = 180;
	for(int i=89; i>0; i--)
	{
		thev = i;
		double g   = RoujeanF1(thes*DPI, thev*DPI, phi*DPI);
		double sca = CalcScatterAngle(thes, thev, phi);

		fprintf(fp, "%lf %lf \n", sca, g);
		printf("%lf %lf \n", sca, g);
	}

	phi = 0.0;
	for(int i=0; i<thes; i++)
	{
		thev = i;
		double g = RoujeanF1(thes*DPI, thev*DPI, phi*DPI);
		double sca = CalcScatterAngle(thes, thev, phi);
      	
		fprintf(fp, "%lf %lf \n", sca, g);
		printf("%lf %lf \n", sca, g);
	}
	
	

	fclose(fp);
}

void CProject_PGStationDlg::OnShadowRossli()
{
	// TODO: Add your command handler code here


	double phi  = 180.0;
	double thes = 45.0;    //sun zenith
	double thev = 0.0;

	FILE* fp = fopen("c:\\temp\\RossLiF1.txt", "w");

	phi = 180;
	for(int i=89; i>0; i--)
	{
		thev = i;
		double g   = RossLiF1(thes*DPI, thev*DPI, phi*DPI);
		double sca = CalcScatterAngle(thes, thev, phi);

		fprintf(fp, "%lf %lf \n", sca, g);
		printf("%lf %lf \n", sca, g);
	}

	phi = 0.0;
	for(int i=0; i<thes; i++)
	{
		thev = i;
		double g = RossLiF1(thes*DPI, thev*DPI, phi*DPI);
		double sca = CalcScatterAngle(thes, thev, phi);

		fprintf(fp, "%lf %lf \n", sca, g);
		printf("%lf %lf \n", sca, g);
	}

	fclose(fp);

}

void CProject_PGStationDlg::OnBrdfReading()
{
	// TODO: Add your command handler code here


	//char* locateFile = "brdf_ndvi03.1031_4020.dat";
	//char* locateFile = "brdf_ndvi03.1354_3988.dat";

	//char* locateFile = "brdf_ndvi04.0972_1567.dat";
	//char* locateFile = "brdf_ndvi04.1096_4158.dat";

	//char* locateFile = "brdf_ndvi05.1010_1489.dat";
	//char* locateFile = "brdf_ndvi05.1020_1558.dat";

	char* locateFile = "brdf_ndvi06.0879_3124.dat";	
	//char* locateFile = "brdf_ndvi06.1008_1484.dat";

	//char* locateFile = "brdf_ndvi07.0834_3291.dat";
	//char* locateFile = "brdf_ndvi08.0874_3128.dat";
    

	char* dataPath = "F:\\Data\\BRDF\\IGBP_BASE_YEAR\\IGBP_13";

	char** filenames = NULL;
	int n =0;
	int nfile = 0;
	GetDirFileName(filenames, dataPath, &n, &nfile, "dat", 0);
	filenames = f2c(nfile, 256);
	GetDirFileName(filenames, dataPath, &n, &nfile, "dat", 1);

	vector<double> szaArr;
	vector<double> vzaArr;
	vector<double> aziArr;
	vector<double> scaArr;
	vector<double> slopeArr;
	vector<double> ndviArr;
	vector<double> rp865Arr;

	for(int i=0; i<nfile; i++)
	{

		char filepath[256];
		strcpy(filepath, filenames[i]);

		char* titleName;
		GetTitleWithPosfix(filepath, &titleName);
		
		if( strcmp(locateFile, titleName) !=0 )
			continue;

		free(titleName);

		printf("%s \n", filepath);		

        
		ReadingParasolBRDF(filepath, szaArr, vzaArr, aziArr, scaArr, slopeArr, ndviArr, rp865Arr);
   
		//output the data into txt file
		
				
        /*
		//generate polar map data
		char outFile[256];
		strcpy(outFile, filepath);
		strcpy(outFile+strlen(outFile)-3, "txt");	
		GeneratePolarMapData(vzaArr, aziArr, rp865Arr, 10.0, outFile);
		*/
		/*
		//generate the polar data using the BPDF model
		double alfa,beita;
		alfa  = 0.021372942;
		beita = 60.638894;
		vector<double> NadalBPDFModel;
		int nDirection = rp865Arr.size();
		NadalBPDFModel.resize(nDirection);
		for(int i=0; i<nDirection; i++)
		{
		double rp = BPDF_Nadal( szaArr[i], vzaArr[i], aziArr[i], alfa, beita );
		NadalBPDFModel[i] = rp;
		}
		GeneratePolarMapData(vzaArr, aziArr, NadalBPDFModel, 10.0, "c:\\temp\\modelPolar.txt");
		*/
	}

	for(int i=1; i<=6; i++)
	{
		int low  = i*10;
		int high = (i+1)*10;
		
		//output
		char filename[256];
		sprintf(filename, "c:\\temp\\out-%d-%d.txt", low, high );

		FILE* fp = fopen(filename, "w");
		for(int k=0; k<szaArr.size(); k++)
		{
			if( szaArr[k]>low && szaArr[k]<high )
			{
				fprintf(fp, "%lf %lf    %lf %lf %lf %lf %lf  \n", 
					rp865Arr[k],  //polarized reflectance
					ndviArr[k],   //ndvi
					szaArr[k],    //sun  zenith
					vzaArr[k],    //view zenith
					aziArr[k],    //azimuth
					scaArr[k],    //scattering angle
					slopeArr[k]   //slope angle 
				);
			}
		}
		fclose(fp);
	}

	FILE* fp = fopen("c:\\temp\\out-all.txt", "w");
	for(int k=0; k<szaArr.size(); k++)
	{
			fprintf(fp, "%lf %lf    %lf %lf %lf %lf %lf  \n", 
				rp865Arr[k],  //polarized reflectance
				ndviArr[k],   //ndvi
				szaArr[k],    //sun  zenith
				vzaArr[k],    //view zenith
				aziArr[k],    //azimuth
				scaArr[k],    //scattering angle
				slopeArr[k]   //slope angle 
				);
	}	
	fclose(fp);

	printf("Finished! \n");
}

void CProject_PGStationDlg::OnBrdfLocationseperate()
{
	// TODO: Add your command handler code here

	char* subPath = "F:\\Data\\BRDF\\IGBP_BASE_YEAR\\IGBP_13\\200511";
	char** subFilenames = NULL;
	int n1 =0;
	int nfile1 = 0;
	GetDirFileName(subFilenames, subPath, &n1, &nfile1, "dat", 0);
	subFilenames = f2c(nfile1, 256);
	GetDirFileName(subFilenames, subPath, &n1, &nfile1, "dat", 1);



	char* dataPath = "F:\\Data\\BRDF\\IGBP_BASE_YEAR\\IGBP_13";
	char** filenames = NULL;
	int n =0;
	int nfile = 0;
	GetDirFileName(filenames, dataPath, &n, &nfile, "dat", 0);
	filenames = f2c(nfile, 256);
	GetDirFileName(filenames, dataPath, &n, &nfile, "dat", 1);

	FILE* fp = fopen("c:\\temp\\bpdf-location.txt", "w");
	//find the number of file occuring repeatedly
	for(int i=0; i<nfile1; i++)
	{
     	//get the title of the file 
		char* title = NULL; //(char*)malloc(256);
		GetTitleWithPosfix(subFilenames[i], &title);
		
		int nOccurNumber = 0;
		for(int j=0; j<nfile; j++)
		{
			char* title1 = NULL; //(char*)malloc(256);
			GetTitleWithPosfix(filenames[j], &title1);

			if( strcmp(title, title1)==0 )
			{
				nOccurNumber ++;
			}
		}
		printf(" %s  %d  \n", title, nOccurNumber);
		fprintf(fp, " %s  %d  \n", title, nOccurNumber);
	}
	fclose(fp);
}

void CProject_PGStationDlg::OnDebugSlope()
{
	// TODO: Add your command handler code here


	printf(" 30 30 180 %lf \n",  CalculateSlopeAngle(30, 30, 180) );
	printf(" 30 30 0 %lf \n",    CalculateSlopeAngle(30, 30, 0) );
	printf(" 30 40 180 %lf \n",  CalculateSlopeAngle(30, 40, 180) );

}

void CProject_PGStationDlg::OnBrdfNdvi()
{
	// TODO: Add your command handler code here

	//reading the files in the brdf database and classification based on NDVI
	char* dataPath = "F:\\Data\\BRDF\\IGBP_BASE_MONTH\\IGBP_13";

	char** filenames = NULL;
	int n =0;
	int nfile = 0;
	GetDirFileName(filenames, dataPath, &n, &nfile, "dat", 0);
	filenames = f2c(nfile, 256);
	GetDirFileName(filenames, dataPath, &n, &nfile, "dat", 1);


	for(int i=0; i<nfile; i++)
	{
		char* title = NULL; //(char*)malloc(256);
		GetTitleWithPosfix(filenames[i], &title);

		printf("%s \n", title);
	
	}
}

void CProject_PGStationDlg::OnBpdfBaresoil()
{
	// TODO: Add your command handler code here

	char* filepath = "F:\\Data\\BRDF\\IGBP_BASE_MONTH\\IGBP_16\\200610\\brdf_ndvi03.0791_4417.dat";

	vector<double> szaArr;
	vector<double> vzaArr;
	vector<double> aziArr;
	vector<double> scaArr;
	vector<double> slopeArr;
	vector<double> ndviArr;
	vector<double> rp865Arr;

	ReadingParasolBRDF(filepath, szaArr, vzaArr, aziArr, scaArr, slopeArr, ndviArr, rp865Arr);

	FILE* fp = fopen("c:\\temp\\baresoil.txt", "w");
	for(int i=0; i<rp865Arr.size(); i++)
	{
		double breonRp = BPDF_Breon( szaArr[i], vzaArr[i], aziArr[i], 0.0 );
		double myRp = BPDF_BareSoil( szaArr[i], vzaArr[i], aziArr[i]);
	
		fprintf(fp, "%lf %lf %lf \n", rp865Arr[i], breonRp, myRp);
		printf("%lf %lf %lf \n", rp865Arr[i], breonRp, myRp);
	}
	fclose(fp);

}

void CProject_PGStationDlg::OnCimgLoading()
{
	// TODO: Add your command handler code here
}

void CProject_PGStationDlg::OnSurfaceNdvi()
{
	// TODO: Add your command handler code here

	char* dataPath = "F:\\Data\\BRDF\\IGBP_BASE_MONTH\\IGBP_13";

	char** filenames = NULL;
	int n =0;
	int nfile = 0;
	GetDirFileName(filenames, dataPath, &n, &nfile, "dat", 0);
	filenames = f2c(nfile, 256);
	GetDirFileName(filenames, dataPath, &n, &nfile, "dat", 1);
	
	int scatterAngle = 120;

	char outfile[256];
	sprintf(outfile, "c:\\temp\\%d_ndvi.txt", scatterAngle);

	FILE* fp = fopen(outfile, "w");

	for(int i=0; i<nfile; i++)
	{
		printf("%s \n", filenames[i]);

		vector<double> szaArr;
		vector<double> vzaArr;
		vector<double> aziArr;
		vector<double> scaArr;
		vector<double> slopeArr;
		vector<double> ndviArr;
		vector<double> rp865Arr;

		ReadingParasolBRDF(filenames[i], szaArr, vzaArr, aziArr, scaArr, slopeArr, ndviArr, rp865Arr);
            
        for(int k=0; k<scaArr.size(); k++)
		{
			if( fabs(scaArr[k]-scatterAngle)<0.5 )
			{
				fprintf( fp, "%lf %lf  %lf \n", rp865Arr[k], ndviArr[k], rp865Arr[k]/exp(-ndviArr[k]*0.7) );
			}
		}
	}
	fclose(fp);
}

/*
IplImage* VerticalMosaic(IplImage* pLeft, IplImage* pRight)
{	
	int nLeftWd = pLeft->width;
	int nLeftHt = pLeft->height;
	int nScanWdLeft = pLeft->widthStep;

	int nRightWd = pRight->width;
	int nRightHt = pRight->height;
	int nScanWdRight = pRight->widthStep;

	int oht = nLeftHt + nRightHt;
	int owd = max(nLeftWd, nRightWd);

	IplImage* pMosaicImage = cvCreateImage( cvSize(owd,oht), 8, 3);
	int oscanWd = pMosaicImage->widthStep;

	
	for(int j=0; j<nLeftHt; j++)
		for(int i=0; i<nLeftWd; i++)
		{
			pMosaicImage->imageData[j*oscanWd + i*3]   = pLeft->imageData[j*nScanWdLeft + i*3];
			pMosaicImage->imageData[j*oscanWd + i*3+1] = pLeft->imageData[j*nScanWdLeft + i*3+1];
			pMosaicImage->imageData[j*oscanWd + i*3+2] = pLeft->imageData[j*nScanWdLeft + i*3+2];
		}

	for(int j=0; j<nRightHt; j++)
		for(int i=0; i<nRightWd; i++)
		{
			pMosaicImage->imageData[(j+nLeftHt)*oscanWd + i*3]   = pRight->imageData[j*nScanWdRight + i*3];
			pMosaicImage->imageData[(j+nLeftHt)*oscanWd + i*3+1] = pRight->imageData[j*nScanWdRight + i*3+1];
			pMosaicImage->imageData[(j+nLeftHt)*oscanWd + i*3+2] = pRight->imageData[j*nScanWdRight + i*3+2];
		}

	return pMosaicImage;
}
*/



void CProject_PGStationDlg::OnPanoramicEpipolarmatchdisp()
{
	// TODO: Add your command handler code here
	char* leftFile = "F:\\Data\\panorama\\mvs\\visualize\\00000000.jpg";
	char* rightFile = "F:\\Data\\panorama\\mvs\\visualize\\00000001.jpg";

	//char* leftFile  = "F:\\Data\\UAV\\byler\\2-sub-1\\mvs\\visualize\\00000000.jpg";
	//char* rightFile = "F:\\Data\\UAV\\byler\\2-sub-1\\mvs\\visualize\\00000001.jpg";

	char* matchFile = "c:\\temp\\featureMatch.txt";

	IplImage* pLeftImage = cvLoadImage(leftFile, 1);
	IplImage* pRightImage = cvLoadImage(rightFile, 1);

	int nLeftHt = pLeftImage->height;

	IplImage* pMosaic = VerticalMosaic(pLeftImage, pRightImage);

	//cvSaveImage("c:\\temp\\verticalMosaic.jpg", pMosaic);

	int nPt = GetFileRows(matchFile) * 0.5;

	int nZoom = 4;

	//draw feature points
	FILE* fp = NULL;
	fp = fopen(matchFile, "r");
	for(int i=0; i<nPt; i++)
	{
		double lx,ly;
		fscanf(fp, "%lf %lf ", &lx, &ly);
		//draw point of the target image
		cvDrawCircle(pLeftImage, cvPoint(lx*nZoom, ly*nZoom), 5, CV_RGB(255,0,0), 3);
		int nMatchPt;
		fscanf(fp, "%d ", &nMatchPt);
		for(int k=0; k<nMatchPt; k++)
		{	
			double rx,ry;
			fscanf(fp, "%lf %lf ", &rx, &ry);
			//cvDrawLine( pMosaic, cvPoint(lx*nZoom,ly*nZoom), cvPoint(rx*nZoom, ry*nZoom+nLeftHt), CV_RGB(255,0,0) );
		}
	}
	fclose(fp);
	cvSaveImage("c:\\temp\\imageFeature.jpg", pLeftImage);


	
	//draw match line
	fp = fopen(matchFile, "r");
	for(int i=0; i<nPt; i++)
	{
		double lx,ly;
		fscanf(fp, "%lf %lf ", &lx, &ly);

		//draw point of the target image
        //cvDrawCircle(pMosaic, cvPoint(), 2, CV_RGB(255,0,0));
          
		int nMatchPt;
		fscanf(fp, "%d ", &nMatchPt);
		for(int k=0; k<nMatchPt; k++)
		{	
			double rx,ry;
			fscanf(fp, "%lf %lf ", &rx, &ry);
		
			if(i>200 && i<210){
			cvDrawCircle(pMosaic, cvPoint(lx*nZoom, ly*nZoom), 5, CV_RGB(255,0,0), 3);
			cvDrawCircle(pMosaic, cvPoint(rx*nZoom, ry*nZoom+nLeftHt), 5, CV_RGB(255,0,0), 3);
			cvDrawLine( pMosaic, cvPoint(lx*nZoom, ly*nZoom), cvPoint(rx*nZoom, ry*nZoom+nLeftHt), CV_RGB(255,0,0), 2 );
			}
		}
	}
	fclose(fp);

	cvSaveImage("c:\\temp\\verticalMosaic.jpg", pMosaic);

	printf("Finished! \n");

}

void CProject_PGStationDlg::OnPanoramicConverttocylinder()
{
	// TODO: Add your command handler code here

	char* infile = "F:\\Data\\panorama\\ladybug_jpg\\ladybug_jpg\\ladybug_panoramic_000001.jpg";
	char* outfile = "c:\\temp\\cylinder.jpg";    

	SphereToCilinder(infile, outfile);

	printf("Finished! \n");


}

void CProject_PGStationDlg::OnBpdfPolardata()
{
	// TODO: Add your command handler code here

	double sza = 25; //sun zenith degree

	for(int j=0; j<70; j++) // view zenith degree
	{		
		for(int i=0; i<360; i++) // azimuth degree
		{

			
		}
	}
}



int InvokeExe(char* cmdString)
{
	printf("%s \n", cmdString);
	STARTUPINFO   si={sizeof(si)};  //
	PROCESS_INFORMATION   pi;   //
	BOOL   success;  

	si.cb = sizeof(STARTUPINFO);
	si.lpReserved = NULL;
	si.lpDesktop = NULL;
	si.lpTitle = NULL;
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;
	si.cbReserved2 = NULL;
	si.lpReserved2 = NULL;
	
	success=::CreateProcess(NULL,
		cmdString,
		NULL,
		NULL, 
		FALSE,
		0,
		NULL,
		NULL,
		&si,
		&pi);     
	
	if(success==FALSE)  
	{
		AfxMessageBox("调用失败！");  
		return -1;
	}
	else  
	{    
		HANDLE   hProcess=pi.hProcess;   //获得开启进出那个HANDLE
		DWORD   dw=::WaitForSingleObject(hProcess,INFINITE);  //调用WaitForSingleObject等待事件
		/*
		switch(dw) 
		{  
		case   WAIT_OBJECT_0:  
			AfxMessageBox("程序关闭");  
			break;  
		case   WAIT_TIMEOUT:  
			AfxMessageBox("超时");  
			break;  
		case   WAIT_FAILEc:\\temp  
			AfxMessageBox("失败");  
			break;  
		}*/
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
	}
	return 0;
}

void CProject_PGStationDlg::OnSelectFiles()
{
	// TODO: Add your command handler code here


	char* batFile = "c:\\temp\\copy.bat";


	char* filepath = "F:\\Data\\OceanImages\\IOCAS_20160717_029";
	char* outpath  = "F:\\Data\\OceanImages\\029";

	char** filenames = NULL;
	int n=0,nfile=0;

	GetDirFileName(filenames, filepath, &n, &nfile, "jpg", 0);
	filenames = f2c(nfile, 256);
	GetDirFileName(filenames, filepath, &n, &nfile, "jpg", 1);


	FILE* fp = fopen(batFile, "w");
	for(int i=0; i<nfile; i+=2)
	{
		//get the file title
		char* titleName;
		GetTitleWithPosfix(filenames[i], &titleName);

		char cmd[256];
		sprintf(cmd, "copy %s %s\\%s",  filenames[i], outpath, titleName);	
		
		//system(cmd);
		//InvokeExe(cmd);
		printf("%s \n", cmd);

		fprintf(fp, "%s \n", cmd);
	}

	fclose(fp);

	printf("finished ! \n");
}	

void CProject_PGStationDlg::OnXmlJpeg()
{
	// TODO: Add your command handler code here

	/*
	char* xmlFile = "c:\\temp\\ocean.xml";

	const char * xmlFile = "school.xml";  
	TiXmlDocument doc;                                
	if (doc.LoadFile(xmlFile)) {  
		doc.Print();  
	} else {  
		cout << "can not parse school.xml" << endl;  
		return;  
	}  
	TiXmlElement* rootElement = doc.RootElement();  //School元素    
	TiXmlElement* classElement = rootElement->FirstChildElement();  // Class元素  
	TiXmlElement* studentElement = classElement->FirstChildElement();  //Students    
	for (; studentElement != NULL; studentElement = studentElement->NextSiblingElement() ) {  
		TiXmlAttribute* attributeOfStudent = studentElement->FirstAttribute();  //获得student的name属性    
		for (;attributeOfStudent != NULL; attributeOfStudent = attributeOfStudent->Next() ) {  
			cout << attributeOfStudent->Name() << " : " << attributeOfStudent->Value() << stc:\\temp:endl;         
		}                                   

		TiXmlElement* studentContactElement = studentElement->FirstChildElement();//获得student的第一个联系方式   
		for (; studentContactElement != NULL; studentContactElement = studentContactElement->NextSiblingElement() ) {  
			string contactType = studentContactElement->Value();  
			string contactValue = studentContactElement->GetText();  
			cout << contactType  << " : " << contactValue << stc:\\temp:endl;             
		}     
	}   
	*/

}

void CProject_PGStationDlg::OnFormatAuv()
{
	// TODO: Add your command handler code here


	//char phtfile[256] = "F:\\Data\\OceanImages\\AUV04\\pht_20160715212357.pht";
	//char outpath[256] = "F:\\Data\\OceanImages\\AUV04";

	char phtfile[256] = "F:\\Data\\OceanImages\\AUV07\\pht_20160719114934.pht";
	char outpath[256] = "F:\\Data\\OceanImages\\AUV07";


	int nRows = GetFileRows(phtfile);

	FILE* fp = fopen(phtfile, "r");

	for(int i=0; i<nRows; i++)
	{
		char date[256];
		char time[256];
		char filetitle[256];
		double lon, lat, depth, height, speed, yaw, pitch, roll;
	
		fscanf(fp, "%s %s %s  %lf %lf  %lf %lf %lf %lf %lf %lf ", 
								date, time, filetitle, 
								&lon, &lat, 
								&depth, &height, &speed, 
								&yaw, &pitch, &roll);

		printf("date: %s \n", date);

		strcpy(filetitle+strlen(filetitle)-4, "\0");

		char outfile[256];
		sprintf(outfile, "%s\\%s.out", outpath, filetitle);
		printf( "%s \n", outfile);

		FILE* ofp = fopen(outfile, "w");
		fprintf(ofp, "%lf %lf   %lf %lf %lf  %lf ",  lon, lat,  yaw, pitch, roll,  height);
		fclose(ofp);
	}
	fclose(fp);
}

void CProject_PGStationDlg::OnPlyMosaic()
{
	// TODO: Add your command handler code here



	//////////////////////////////////////////////////////////////////////////
	//6. merge the ply
	char modelPath[512] = "F:\\Data\\UAV\\山西省_航拍照片\\航拍照片\\mvs\\models";
	//strcpy(modelPath, pdlg->m_strMVSModelDirectory);
	char** filenames = NULL;
	int n=0,nfile=0;
	GetDirFileName(filenames, modelPath, &n, &nfile, "ply", 0);
	filenames = f2c(nfile, 256);
	GetDirFileName(filenames, modelPath, &n, &nfile, "ply", 1);

	vector<stTrack> allTracks;
	for(int i=0; i<nfile; i++)
	{
		vector<stTrack> st;
		ReadPMVSPly(filenames[i], st);
		for(int k=0; k<st.size(); k++)
		{
			allTracks.push_back( st[k] );
		}

	}
	char plyfile[512] = "c:\\temp\\m.ply";
	//strcpy(plyfile, pdlg->m_strPMVSPlyFile);
	WritePMVSPly(plyfile, allTracks);
	//////////////////////////////////////////////////////////////////////////

}

template<typename T>
int TestTemplateFunc()
{
	/*T* pBuffer = new T[100];
	for(int i=0; i<100; i++)
		T[i] = i;*/

	char* filepath = "F:\\Data\\UAV\\广西\\10105\\result\\111.tif";
	stGeoInfo geoinfo;
	GetGeoInformation(filepath, geoinfo);
	int ht = geoinfo.ht;
	int wd = geoinfo.wd;

	T* pbuffer = new T[ht*wd];

	ReadGeoFileByte(filepath, 1, 1, T);

	delete[] T;

	return 0;
}

void CProject_PGStationDlg::OnTemplateFunction()
{
	// TODO: Add your command handler code here
	
	//TestTemplateFunc();

}


void CProject_PGStationDlg::OnEigenTest()
{
	// TODO: 在此添加命令处理程序代码
	printf("eigen test .... \n");



}
