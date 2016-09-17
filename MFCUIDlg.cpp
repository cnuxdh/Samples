
// MFCUIDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MFCUI.h"
#include "MFCUIDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "DlgProgress.h"
#include "FolderDlg.h"
#include "DlgBlendInput.h"
#include "BatDialog.h"
#include "File.h"
#include "QualityDialog.h"
#include "utility.h"


#include "fcntl.h"
#include "io.h"
#include "geotiff.h"

#include "def.h"

#include "commonfile.h"

#include "bundlerio.hpp"


#include <TlHelp32.h>


//opencv
//#include "opencv2/opencv.hpp"
//using namespace cv;

#include "SplashWnd.h"


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}


void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMFCUIDlg 对话框


/*
int ReadProgressFile(CString filename, double& dValue)
{
	FILE* fp = fopen(m_strProgressFile, "r");
	if(fp!=NULL)
	{
		fscanf(fp, "%lf", &dValue);
		fclose(fp);
	}
	return 0;
}

int WriteProgressFile(CString filename, double& dValue)
{
	FILE* fp = fopen(m_strProgressFile, "r");
	if(fp!=NULL)
	{
		fscanf(fp, "%lf", &dValue);
		fclose(fp);
	}
	return 0;
}
*/


BEGIN_MESSAGE_MAP(CMFCUIDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_LOAD_IMAGES, &CMFCUIDlg::OnLoadImages)
	ON_COMMAND(ID_LOAD_DIR, &CMFCUIDlg::OnLoadDir)
	ON_COMMAND(ID_MOSAIC, &CMFCUIDlg::OnMosaic)
	ON_MESSAGE(WM_UPDATEDATA, &CMFCUIDlg::OnUpdateData)  
	ON_WM_CLOSE()
	ON_COMMAND(ID_OUTPUT_BLENDORTHO, &CMFCUIDlg::OnOutputBlendortho)
	ON_WM_TIMER()
	ON_COMMAND(ID_SAVE_PROJECT, &CMFCUIDlg::OnSaveProject)
	ON_COMMAND(ID_OPEN_PROJECT, &CMFCUIDlg::OnOpenProject)
	ON_COMMAND(ID_CLOSE_PROJECT, &CMFCUIDlg::OnCloseProject)
	ON_COMMAND(ID_DELETE_IMAGES, &CMFCUIDlg::OnDeleteImages)
	ON_BN_CLICKED(IDOK, &CMFCUIDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CMFCUIDlg::OnBnClickedCancel)
	ON_COMMAND(ID_DISP_PROGRESSBAR, &CMFCUIDlg::OnDispProgressbar)
	ON_COMMAND(ID_ADD_MOREIMAGES, &CMFCUIDlg::OnAddMoreimages)
	ON_COMMAND(ID_BATCH_GO, &CMFCUIDlg::OnBatchGo)
	ON_COMMAND(ID_MVS, &CMFCUIDlg::OnMvs)
	ON_COMMAND(ID_GENERATE_ORTHO, &CMFCUIDlg::OnGenerateOrtho)
	ON_COMMAND(ID_FLY_QUALITY, &CMFCUIDlg::OnFlyQuality)
END_MESSAGE_MAP()



int UseCheck(CString exepath)
{	
	char binPath[512];
	strcpy(binPath, exepath);

	//generate version.dat path
	char checkFile[512];
	strcpy(checkFile, binPath);
	strcat(checkFile, "\\version.dat");

	int ncount = 0;
	FILE* fp = fopen(checkFile, "rb");
	if(fp==NULL)
	{
		fp = fopen(checkFile, "wb");
		int ncount = 1;
		fwrite(&ncount, sizeof(int), 1, fp);
		fclose(fp);

		return 1;
	}

	fread(&ncount, sizeof(int), 1, fp);
	fclose(fp);

	if( ncount>1500 )
		return 0;

	ncount++;
	fp = fopen(checkFile, "wb");
	fwrite(&ncount, sizeof(int), 1, fp);
	fclose(fp);

	return 1;
}





// CMFCUIDlg 消息处理程序
int bIsFileExit(char* filepath)
{
	FILE* fp = fopen(filepath, "r");
	if(fp!=NULL)
	{
		fclose(fp);
		return 1;
	}
	else
		return 0;
}
int GetFileRows(char* file)
{
	char  sline[512];
	FILE* fp = NULL;
	int rows = 0;
	fp = fopen(file, "r");
	if(fp==NULL)
		return 0;
	while( fgets(sline, 512, fp)!=NULL )
		rows++;
	fclose(fp);
	return rows;
}


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

int CreateDir(CString dirName)
{
	// 判断路径是否存在 
	//if(!PathIsDirectory(dirName) ) 
	while( !PathIsDirectory(dirName) )
	{ 
		CreateDirectory(dirName, NULL ); 
	} 
	return 0;
}

int DeleteDir(CString directory_path)   //删除一个文件夹下的所有内容  
{     
	if(!PathIsDirectory(directory_path) ) 
		return -1;

    CFileFind finder;  
    CString path;  
    path.Format("%s/*.*",directory_path);  
    BOOL bWorking = finder.FindFile(path);  
    while(bWorking)
	{  
        bWorking = finder.FindNextFile();  
        if(finder.IsDirectory() && !finder.IsDots()){//处理文件夹  
            DeleteDir(finder.GetFilePath()); //递归删除文件夹  
            RemoveDirectory(finder.GetFilePath());  
        }  
        else
		{   //处理文件  
            DeleteFile(finder.GetFilePath());  
        }  
    }  

	RemoveDirectory(directory_path);

	return 0;
}

int CloseProgram(CString strProgram)
{
	int nFind = 0;

	HANDLE handle; //定义CreateToolhelp32Snapshot系统快照句柄
	HANDLE handle1; //定义要结束进程句柄
	handle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);//获得系统快照句柄
	PROCESSENTRY32 *info; //定义PROCESSENTRY32结构字指
	
	//PROCESSENTRY32 结构的 dwSize 成员设置成 sizeof(PROCESSENTRY32)
	info = new PROCESSENTRY32;
	info->dwSize = sizeof(PROCESSENTRY32);
	//调用一次 Process32First 函数，从快照中获取进程列表
	Process32First(handle, info);
	//重复调用 Process32Next，直到函数返回 FALSE 为止
	while(Process32Next(handle, info) != FALSE)
	{  
		CString strTmp = info->szExeFile;     //指向进程名字  
		//strcmp字符串比较函数同要结束相同  
		//if(strcmp(c, info->szExeFile) == 0 )  
		if (strProgram.CompareNoCase(info->szExeFile) == 0 )  
		{  
			//PROCESS_TERMINATE表示为结束操作打开,FALSE=可继承,info->th32ProcessID=进程ID   
			handle1 = OpenProcess(PROCESS_TERMINATE, FALSE, info->th32ProcessID);
			//结束进程   
			TerminateProcess(handle1, 0);   
			nFind = 1;
		}  
	}
	delete info;
	CloseHandle(handle);

	return nFind;
}

CMFCUIDlg::CMFCUIDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMFCUIDlg::IDD, pParent)
{
	m_strProductPath = "c:\\sfm";
	CreateDir(m_strProductPath);

	if( UseCheck(m_strProductPath)<1 )
	{
		AfxMessageBox("该免费版本已经过期，请联系软件开发商进行免费升级！");
		AfxGetMainWnd()->SendMessage(WM_CLOSE);
	}

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCUIDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

int  CMFCUIDlg::InitProductDirectory()
{
	if(m_fileVector.size()<2)
	{
		AfxMessageBox("图像数量太少！");
		return -1;
	}

	CString path = m_fileVector[0];
	int index = path.ReverseFind('\\');
	path = path.Left(index+1);

	CString tempProductPath = path + "product";
	CreateDir(tempProductPath);
	
	CString resultPath = path + "result";
	CreateDir(resultPath);
	
	//generate mvs directory
	m_strMVSDirectory = path+"mvs";
	DeleteDir(m_strMVSDirectory);

	CreateDir(m_strMVSDirectory); 

	m_strMVSImageDirectory  = m_strMVSDirectory+"\\visualize";
	CreateDir(m_strMVSImageDirectory);

	m_strMVSCameraDirectory = m_strMVSDirectory+"\\txt";
	CreateDir(m_strMVSCameraDirectory);

	m_strMVSModelDirectory  = m_strMVSDirectory+"\\models";
	CreateDir(m_strMVSModelDirectory);


	//save the PMVS ply file
	m_strPMVSPlyFile    = resultPath + "\\model.ply";
	
	
	return 0;
}


BOOL CMFCUIDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	//AfxGetMainWnd()->SendMessage(WM_CLOSE);

	m_strExePath = GetFilePath();

		
	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	//add menu
	m_menu.LoadMenu(IDR_MENU1);
	SetMenu(&m_menu);

	// TODO: 在此添加额外的初始化代码

	//console window, for debug
	InitConsoleWindow();	
	
	/////////////////// for Tab control /////////////////////
	m_fileList.InsertColumn(0,_T("File path"),LVCFMT_LEFT,400);
	CFont font;
	font.CreatePointFont(85,_T("Tahoma"));
	m_multiPaneCtrl.SetFont(&font);
	m_multiPaneCtrl.SetFontSelect(&font);
	m_multiPaneCtrl.InstallStyle(&m_multiPaneCtrl.styleBase);
	//////////////////////////////////////////////////////////
	


	/*
	m_strImageListFile = m_strExePath + "\\imagelist.txt";
	m_strListFile      = m_strExePath + "\\list.txt";
	m_strPosFile       = m_strExePath + "\\pos.txt";
	m_strMatchFile     = m_strExePath + "\\match_init.txt";
	m_strBundleOutFile = m_strExePath + "\\bundle.out";
	*/
	 
	m_strImageListFile  = m_strProductPath + "\\imagelist.txt";
	m_strListFile       = m_strProductPath + "\\list.txt";
	m_strPosFile        = m_strProductPath + "\\pos.txt";
	m_strMatchFile      = m_strProductPath + "\\match_init.txt";
	m_strBundleOutFile  = m_strProductPath + "\\bundle.out";
	m_strFilteredPoints = m_strProductPath + "\\fpt.txt";

	m_strProgressFile = m_strProductPath + "\\progress.txt";
	
	FILE* fp = fopen(m_strProgressFile, "w");
	fprintf(fp, "%lf", 0);
	fclose(fp);

	m_projectFilesVec.clear();
	m_projectFilesVec.push_back(m_strListFile);
	m_projectFilesVec.push_back(m_strPosFile);
	m_projectFilesVec.push_back(m_strMatchFile);
	m_projectFilesVec.push_back(m_strBundleOutFile);
	m_projectFilesVec.push_back(m_strFilteredPoints);
	//m_projectFilesVec.push_back(m_strPMVSPlyFile);


	DeleteTempProjectFiles();

	//parameters
	m_nScaleImageHt = 900;
	m_dMosaicResolution = 0.1;
	m_dMosaicRatio = 1;
	m_bIsBlend = 1;

	//for point cloud filtering
	m_nNeiborPts = 1024;
	m_dSigma = 0.6;

	m_dProgressValue = 0;
	m_pDlgProgressDlg = NULL;

	m_pMyThread = NULL;

	m_bIsProjectSave = false;

	m_strDetectProgram = "Project_SIFT.exe"; 
	//m_strMatchProgram  = "KeyMatchFull.exe";
	m_strMatchProgram  = "Project_KeyMatch_VS2010.exe"; //multi-core version
	m_strDetectProgramSift = "glsl-sift.exe";
	m_strMatchProgramSift  = "glsl-match.exe";

	m_strBundleProgram = "Bundler.exe";
	//m_strBundleProgram = "Bundler_pba.exe";
	m_strOrthoProgram  = "Project_OrthoImage_VS2005.exe";
	m_strFusionProgram = "Project_Fusion_VS2010.exe";

	m_strCMVS = "cmvs.exe";
	m_strPMVS = "pmvs2.exe";
	m_strRadialUndistort = "RadialUndistort.exe";
	m_strOptionGen  = "genOption.exe";
		

	/*
	m_pDlgProgressDlg = new CDlgProgress(this);
	m_pDlgProgressDlg->Create(IDD_DIALOG_PROGRESS, this);
	m_pDlgProgressDlg->ShowWindow(SW_HIDE);
	m_pDlgProgressDlg->CenterWindow(this);
	*/

	GetDlgItem(IDC_TAB)->ShowWindow(SW_HIDE);
	SetTabsPosition();   
	
	/*
	m_menu.EnableMenuItem(ID_MOSAIC, MF_GRAYED);
	m_menu.EnableMenuItem(ID_OUTPUT_BLENDORTHO, MF_GRAYED);	
	m_menu.EnableMenuItem(ID_CLOSE_PROJECT, MF_GRAYED);
	m_menu.EnableMenuItem(ID_SAVE_PROJECT, MF_GRAYED);
	*/

	SetMenuEnable();

	
	m_bIsThreadOn = false;

	m_bIsAddNewImages = true;

	//m_pMyThread = NULL;

	SetWindowText("易图快拼");
	
	//HICON m_hIcon;
	//m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
	//SetIcon(m_hIcon, TRUE); // Set big icon,32x32

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}


void CMFCUIDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else if(nID == SC_RESTORE)
	{
		//AfxMessageBox("Restore");
		if(!m_bIsThreadOn)
		{
			if(m_pDlgProgressDlg!=NULL)
			{
				if( m_pDlgProgressDlg!=NULL )
				{
					delete m_pDlgProgressDlg;
					m_pDlgProgressDlg = NULL;
				}
			}
		}
	}
	else
	{

		//CDialogEx::OnSysCommand(nID, lParam);
	}

	CDialogEx::OnSysCommand(nID, lParam);
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMFCUIDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMFCUIDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CMFCUIDlg::SetTabsPosition(void)
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
int CMFCUIDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	CSplashWnd * pSplashWnd=new CSplashWnd; 
	pSplashWnd->CreateSplashWnd(); 
	pSplashWnd->CenterWindow(); 
	pSplashWnd->ShowWindow (SW_SHOW); 
	pSplashWnd->UpdateWindow(); 
	Sleep(2000); //等待函数指定秒钟 
	if (pSplashWnd!=NULL)pSplashWnd->SendMessage (WM_CLOSE); //关闭窗口 
	delete pSplashWnd;
		
	/////////////////// for Tab control /////////////////////
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
	if( m_editOutput.Create(WS_CHILD | ES_MULTILINE,CRect(0,0,0,0),this,3007)==0 )
		return -1;
	
	//insert widget into the tab control
	MultiPaneCtrl::Tabs tabs;
	tabs.Add(m_dialogDisp, _T("Disp"), 0);
	tabs.Add(m_fileList,   _T("File List"), 1);
	tabs.Add(m_editOutput, _T("Output"), 2);
	//tab layout setting
	int portion[2];
	if(m_multiPaneCtrl.LoadState( AfxGetApp(),_T("MultiPaneCtrl_DemoDlg"),_T("State"),&tabs,false)==false)
	{
		HPANE h1 = m_multiPaneCtrl.ConvertToLine(NULL,true);
			HPANE h2 = m_multiPaneCtrl.ConvertToLine(h1,false);
				HPANE h3 = m_multiPaneCtrl.ConvertToLine(h2,true);
					m_multiPaneCtrl.AddTab(h3,tabs[1]); //file list window
				HPANE h4 = m_multiPaneCtrl.Add(h2);
					m_multiPaneCtrl.AddTab(h4,tabs[0]); //display window
				portion[0]=20; portion[1]=80;
				m_multiPaneCtrl.SetPortions(h2, portion);
			HPANE h5 = m_multiPaneCtrl.Add(h1);
				m_multiPaneCtrl.AddTab(h5,tabs[2]);		//output window		
				//m_multiPaneCtrl.AddTab(h5,tabs[3]);		//points list window		
	
		
		portion[0]=80; portion[1]=20;
		m_multiPaneCtrl.SetPortions(h1, portion);
	}
	//////////////////////////////////////////////////////////////////////////////////////////////
	
	return 0;
}



void CMFCUIDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码

	SetTabsPosition();
}

void CMFCUIDlg::OnLoadImages()
{
	// TODO: 在此添加命令处理程序代码

	//m_bIsAddNewImages = true;

	int i;

	CFileDialog dlg(TRUE,NULL,NULL,
		OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_ALLOWMULTISELECT|OFN_EXPLORER,
		_T("tiff file (*.tif)|*.tif|tiff file (*.TIF)|*.TIF|jpg file (*.jpg)|*.jpg|JPG file(*.JPG)|*.JPG||"));
	
	TCHAR* szPathMulti  = new TCHAR[8*1024*1024];  //alloc memory for multiple files
	dlg.m_ofn.nMaxFile  = 8*1024*1024; 
	dlg.m_ofn.lpstrFile = szPathMulti; 
	dlg.m_ofn.lpstrFile[0] = 0; 

	if(dlg.DoModal()==IDOK)
	{	
		m_editOutput.Clear();
		m_editOutput.SetWindowText("\0");
		
		m_bIsProjectSave = false;
		
		//if the path contains space, then return , added by xiedonghai
		CString path = dlg.GetPathName();
		if( path.Find(" ")>0)
		{
			AfxMessageBox(" 图像路径或图像名称中不能有空格！ ");
			return;
		}
		//m_fileVector.clear();

		POSITION  pos = dlg.GetStartPosition(); 
				
		if(m_bIsAddNewImages)
			m_fileVector.clear();

		//read file paths
		while(pos != NULL) 
		{ 
			CString filepath = dlg.GetNextPathName(pos); 
					
			//determine if the file has exit
			vector<CString>::iterator result = find( m_fileVector.begin(), m_fileVector.end(), filepath ); 
			if( result != m_fileVector.end( ) ) 
				continue;
			
			m_fileVector.push_back(filepath);	
		} 	
		
		//sort the filenames
		sort(m_fileVector.begin(), m_fileVector.end());
		m_fileList.DeleteAllItems();
		int index = 0;
		for( i=0; i<m_fileVector.size(); i++ )
		{
			m_fileList.InsertItem(index, m_fileVector[i]);
			index++;
		}

		DeleteTempProjectFiles();

		//write the imagelist.txt		
		FILE* fp = fopen(m_strImageListFile, "w");
		if(fp==NULL)
		{
			AfxMessageBox("Error to write file !\n");
			return ;
		}
		for(int i=0; i<m_fileVector.size(); i++)
			fprintf(fp, "%s \n", m_fileVector[i]);
		fclose(fp);

		ImagePreprocess();

		//loading pos.txt and draw points
		LoadPOSData();

		InitProductDirectory();

		//CString sout = "加载图像...";
		//sout.Format(_T("%d "), j);
		//m_editOutput.Clear();
		CString strlog;
		strlog.Format("加载了%d张图像 \r\n", m_fileVector.size());
		m_strLog.Empty();
		OutputMessage(strlog);	

		SetMenuEnable();
	}
}



/*
char **f2c (int nr, int nc)
{
	char **x;
	int i;

	x = (char **)calloc ( nr, sizeof (BYTE *) );
	if (x == NULL)	
		return NULL;		

	for (i=0; i<nr; i++)
	{  
		x[i] = (char *) calloc ( nc, sizeof (BYTE)  );
		if (x[i] == NULL)
			return NULL;				  
	}
	return x;
}

void FreeArray_char(char **array, int row, int col)
{
	int i;
	for(i = 0;i < row; ++i)
		free(array[i]);
	free(array);
}*/


void CMFCUIDlg::OnLoadDir()
{
	// TODO: 在此添加命令处理程序代码

	//UpdateData();

	LPCTSTR lpszTitle = _T( "Select the folder you like best of all:" );
	UINT	uFlags	  = BIF_RETURNONLYFSDIRS | BIF_USENEWUI;

	CFolderDialog* pFolderDlg = NULL;
	pFolderDlg = new CFolderDialog( lpszTitle, NULL, this, uFlags);

	ASSERT( pFolderDlg != NULL );

	if( pFolderDlg != NULL )
	{
		//if( !m_strRoot.IsEmpty() )
		//	pFolderDlg->SetRootFolder( m_strRoot );
		//else
		pFolderDlg->SetRootFolder( NULL );

		if( pFolderDlg->DoModal() == IDOK )
		{
			m_strImagePath = pFolderDlg->GetFolderPath();

			if( m_strImagePath.Find(" ")>0)
			{
				AfxMessageBox(" 图像路径中不能有空格！ ");
				return;
			}

			//m_strName = pFolderDlg->GetFolderName();			
			//pFolderDlg->SetRootFolder( m_strRoot );
			//m_strImagePath = m_strPath;
			//UpdateData( FALSE );

			//retrieve the files in the image path
			char imagePath[256];
			strcpy(imagePath, m_strImagePath);
			
			int n=0;
			int nfile=0;
			char** filenames = NULL;
			GetDirFileName(filenames, imagePath, &n, &nfile, "jpg", 0);
			
			if(nfile==0)
			{
				GetDirFileName(filenames, imagePath, &n, &nfile, "JPG", 0);
				if(nfile==0)
				{
					GetDirFileName(filenames, imagePath, &n, &nfile, "JPEG", 0);
					filenames = f2c(nfile, 256);
					GetDirFileName(filenames, imagePath, &n, &nfile, "JPEG", 1);
				}
				else
				{
					filenames = f2c(nfile, 256);
					GetDirFileName(filenames, imagePath, &n, &nfile, "JPG", 1);
				}
			}
			else
			{
				filenames = f2c(nfile, 256);
				GetDirFileName(filenames, imagePath, &n, &nfile, "jpg", 1);
			}

			
			if(nfile<1)
			{
				AfxMessageBox("目录中没有图像！");
				return ;
			}
						
			CString cfile;
			cfile.Format("%s", filenames[0]);
			if( cfile.Find(" ")>0)
			{
				AfxMessageBox(" 图像名称中不能有空格！ ");
				return;
			}

			//save image files into m_fileVector
			m_fileVector.clear();
			for(int i=0; i<nfile; i++)
			{
				CString cfile;
				cfile.Format("%s", filenames[i]);
				
				/*
				//determine if the file has exit
				vector<CString>::iterator result = find( m_fileVector.begin(), m_fileVector.end(), cfile); 
				if( result != m_fileVector.end( ) ) 
					continue;
				*/			
				m_fileVector.push_back(cfile);
			}

			//sort the filenames
			sort(m_fileVector.begin(), m_fileVector.end());

			m_fileList.DeleteAllItems();
			int index = 0;
			for(int i=0; i<m_fileVector.size(); i++ )
			{
				m_fileList.InsertItem(index, m_fileVector[i]);
				index++;
			}
			
			DeleteTempProjectFiles();

			//write the imagelist.txt		
			FILE* fp = fopen(m_strImageListFile, "w");
			if(fp==NULL)
			{
				AfxMessageBox("Error to write file !\n");
				return ;
			}
			for(int i=0; i<m_fileVector.size(); i++)
				fprintf(fp, "%s \n", m_fileVector[i]);
			fclose(fp);
			
			FreeArray_char(filenames, nfile, 256);

			ImagePreprocess();
			
			//loading pos.txt and draw points
			LoadPOSData();

			InitProductDirectory();

			m_bIsProjectSave = false;

			//CString sout = "加载图像...";
			//sout.Format(_T("%d "), j);
			//m_editOutput.Clear();
			CString strlog;
			strlog.Format("加载了%d张图像 \r\n", m_fileVector.size());
			m_strLog += strlog;
			m_editOutput.SetWindowText(m_strLog);
			UpdateData(true);

			SetMenuEnable();
		}
	}
	printf("Finished !");
}


//generate list.txt pos.txt and draw 
int CMFCUIDlg::ImagePreprocess(void)
{
	char cmdString[512];
	
	char imageListFile[256];
	strcpy(imageListFile, m_strImageListFile);
	if( bIsFileExit( imageListFile ) )
	{
		sprintf(cmdString, "%s\\Project_GenerateList_VS2005.exe %s %s %s",
			m_strExePath,
			m_strImageListFile, m_strListFile, m_strPosFile);	
		printf("%s \n", cmdString);
		InvokeExe(cmdString);
	}
	return 0;
}

CString CMFCUIDlg::GetFilePath(void)
{
	char  exepath[MAX_PATH];
	CString  strdir,tmpdir; 
	memset(exepath,0,MAX_PATH); 
	GetModuleFileName(NULL,exepath,MAX_PATH); 
	tmpdir=exepath; 
	strdir=tmpdir.Left(tmpdir.ReverseFind('\\'));
	return strdir; 	
}

void CMFCUIDlg::SetProgressBar(int value)
{
	SendMessage(WM_UPDATEDATA, value);//向主线程发送消息更新进度条
}

LRESULT CMFCUIDlg::OnUpdateData(WPARAM wParam, LPARAM lParam)
{
	int iTmp = (int)wParam;	
	if(m_pDlgProgressDlg!=NULL)
		m_pDlgProgressDlg->SetProgressValue(iTmp);	
	UpdateData(false);  //实时更新主界面
	return 0;
}

double GetResolution(char* listFile)
{
	double resolution = 0;

	char jpgfile[256];
	char* geofile;// = (char*)malloc(256);
	char* tiffile;
	//char* tifffile = (char*)malloc(256);
	int  ti;
	double focalLen;
	bool bHaveTif=false;
	bool bHaveGeofile=false;
	FILE* fp = fopen(listFile, "r");
	while(!feof(fp))
	{
		fscanf(fp, "%s %d %lf", jpgfile, &ti, &focalLen);
		
		GenerateProductFile(jpgfile, "product", "tif", &tiffile);
		GenerateProductFile(jpgfile, "product", "geo", &geofile);
				
		//if(bIsFileExit(tiffile))
		if(bIsFileExit(geofile))
		{
			bHaveGeofile = true;
			break;
		}
		if(bIsFileExit(tiffile))
		{
			bHaveTif = true;
			break;
		}
	}
	fclose(fp);

	if(bHaveGeofile)
	{
		stGeoInfo geoinfo;
		ReadGeoFile(geofile, geoinfo);
		resolution = fabs( geoinfo.dx );
	}
	if(bHaveTif)
	{
		stGeoInfo geoinfo;
		GetGeoInformation(tiffile, geoinfo);
		resolution = fabs( geoinfo.dx );
	}

	free(tiffile);
	free(geofile);
	
	return resolution;
}



UINT  CMFCUIDlg::ProcessBlend(LPVOID pParam)
{
	CMFCUIDlg *pdlg = (CMFCUIDlg * )pParam;

	pdlg->m_bIsThreadOn = true;

	char cmdString[256];

	char imagelistFile[256];
	strcpy(imagelistFile, pdlg->m_strImageListFile);

	char outfile[256];
	strcpy(outfile, pdlg->m_strMosaicFile);
	
	printf("mosaic resolution: %lf \n", pdlg->m_dMosaicResolution);

	int type = 0;
	if(pdlg->m_bIsBlend)
		type = 1;
	else
		type = 0;
	//type = 0;

	//6. mosaic and blend
	pdlg->m_pDlgProgressDlg->SetWindowTextA("图像镶嵌.....");
	if( bIsFileExit(imagelistFile) )
	{
		sprintf(cmdString, "%s\\Project_Fusion_VS2010.exe %s %s %lf %d",
			pdlg->m_strExePath, 
			imagelistFile, 
			outfile, 
			pdlg->m_dMosaicResolution, 
			type);
		InvokeExe(cmdString);
	}

	pdlg->m_pDlgProgressDlg->ShowWindow(SW_HIDE);
	pdlg->KillTimer(100);

	pdlg->m_bIsThreadOn = false;

	//if(pdlg->m_pDlgProgressDlg != NULL)
	//	delete pdlg->m_pDlgProgressDlg; 

	CString strlog;
	strlog.Format("拼接完成，结果存储到文件:  ' %s '  中。\r\n", outfile);
	pdlg->OutputMessage(strlog);	


	return 0;
}

UINT  CMFCUIDlg::Process(LPVOID pParam)
{

	CMFCUIDlg *pdlg = (CMFCUIDlg * )pParam;
		
	pdlg->m_bIsThreadOn = true;
	
	char cmdString[256];
	
	char imagepathFile[256];
	strcpy(imagepathFile, pdlg->m_strImageListFile);
	char listFile[256];
	strcpy(listFile, pdlg->m_strListFile);
	char matchfile[256];
	strcpy(matchfile, pdlg->m_strMatchFile);
	char bundleoutfile[256];
	strcpy(bundleoutfile, pdlg->m_strBundleOutFile);
	char filteredPtFile[256];
	strcpy(filteredPtFile, pdlg->m_strFilteredPoints);

	//2. detect feature points	
	//if( bIsFileExit(listFile) == 0 )
	//	return 0;

	FILE* progressFp = fopen(pdlg->m_strProgressFile, "w");
	fprintf(progressFp, "%lf", 0.0);
	fclose(progressFp);


	pdlg->m_pDlgProgressDlg->SetProgressValue(0);
	pdlg->m_pDlgProgressDlg->SetWindowTextA(" [共3步|第1步]: 提取特征点.....");
	/*
	int nImage = GetFileRows(listFile);
	double fstep = 100.0 / double(nImage);
	FILE* fp = fopen(pdlg->m_strListFile, "r");
	for(int i=0; i<nImage; i++)
	{
		FILE* progressFp = fopen(pdlg->m_strProgressFile, "w");
		fprintf(progressFp, "%lf", fstep*i);
		fclose(progressFp);
		
		char imageFile[256];
		int  ti;
		double focalLen;
		fscanf(fp, "%s %d %lf", imageFile, &ti, &focalLen);

		//generate key file path
		//char keyfile[256];			
		//strcpy(keyfile, imageFile);
		//strcpy(keyfile+strlen(keyfile)-3, "key");
		//printf("%s \n", keyfile);			
		//printf("keyfile: %s \n", keyfile);
		

		char* keyfile = (char*)malloc(256);
		GenerateProductFile(imageFile, "product", "key", &keyfile);
		
		if( bIsFileExit(keyfile) )
			continue;

		sprintf(cmdString, "%s\\Project_SIFT.exe %s %s 1000",
			pdlg->m_strExePath, imageFile, keyfile);

		free(keyfile);

		//sprintf(cmdString, "%s\\glsl-sift.exe -i %s -o %s -maxd 1600 -d 4 ",
		//	pdlg->m_strExePath, imageFile, keyfile);
		
		InvokeExe(cmdString);
	}
	fclose(fp);	
	progressFp = fopen(pdlg->m_strProgressFile, "w");
	fprintf(progressFp, "%lf", 0.0);
	fclose(progressFp);
	*/

	/////////////////////////////////////////////////
	//multiple core version
	sprintf(cmdString, "%s\\Project_SIFT.exe %s %d",
		pdlg->m_strExePath, imagepathFile, pdlg->m_nScaleImageHt);
	InvokeExe(cmdString);
	/////////////////////////////////////////////////
		

	//3. matching
	pdlg->m_pDlgProgressDlg->SetProgressValue(0);
	pdlg->m_pDlgProgressDlg->SetWindowTextA("[共3步|第2步]: 特征点匹配....");
	if( bIsFileExit(matchfile) == 0 )
	{
		//sprintf(cmdString, "%s\\KeyMatchFull.exe %s %s 0.6",
		//	pdlg->m_strExePath, pdlg->m_strListFile, pdlg->m_strMatchFile);
		
		//sprintf(cmdString, "%s\\glsl-match.exe %s %s",
		//	pdlg->m_strExePath, pdlg->m_strListFile, pdlg->m_strMatchFile);
		

		//multi-core version
		sprintf(cmdString, "%s\\%s %s %s 0.6",
			pdlg->m_strExePath, 
			pdlg->m_strMatchProgram, 
			pdlg->m_strListFile, 
			pdlg->m_strMatchFile);


		InvokeExe(cmdString);
	}
		
	//4. bundle	
	pdlg->m_pDlgProgressDlg->SetProgressValue(0);
	pdlg->m_pDlgProgressDlg->SetWindowTextA("[共3步|第3步]: 空三处理.....");
	bool bIsBundler = false;
	if( bIsFileExit(bundleoutfile) == 0 )
	{		
		//delete bundle directory
		//sprintf(cmdString, "cmd /c rd %s\\bundle /s /q", pdlg->m_strExePath);
		//InvokeExe(cmdString);

		//make bundle directory
		//sprintf(cmdString, "cmd /c mkdir %s\\bundle", pdlg->m_strExePath);
		//InvokeExe(cmdString);
		
		//bunder
		char optionFile[256];
		sprintf(optionFile, "%s\\options.txt", pdlg->m_strExePath);
		sprintf(cmdString, "%s\\%s %s --match_table %s --options_file %s",
			pdlg->m_strExePath, 
			pdlg->m_strBundleProgram,
			pdlg->m_strListFile, 
			pdlg->m_strMatchFile, 
			optionFile);
		InvokeExe(cmdString);

		//copy the final results
		sprintf(cmdString, "cmd /c copy %s\\bundle\\bundle.out  %s", 
			pdlg->m_strExePath, pdlg->m_strProductPath);
		//InvokeExe(cmdString);

		bIsBundler = true;
	}
	
	
	pdlg->m_pDlgProgressDlg->ShowWindow(SW_HIDE);
	pdlg->KillTimer(100);

	//pdlg->m_pDlgProgressDlg->SetWindowTextA("处理.....");
	//pdlg->m_pDlgProgressDlg->ShowWindow(SW_HIDE);

	pdlg->SetMenuEnable();

	pdlg->m_bIsThreadOn = false;
	
	//pdlg->m_pMyThread = NULL;

	//if(pdlg->m_pDlgProgressDlg != NULL)
	//	delete pdlg->m_pDlgProgressDlg;
	
	
	return 0;
}

UINT  CMFCUIDlg::ProcessMVS(LPVOID pParam)
{
	CMFCUIDlg *pdlg = (CMFCUIDlg * )pParam;
	
	char cmdString[256];

	char listFile[256];
	strcpy(listFile, pdlg->m_strListFile);
	char matchfile[256];
	strcpy(matchfile, pdlg->m_strMatchFile);
	char bundleoutfile[256];
	strcpy(bundleoutfile, pdlg->m_strBundleOutFile);
	char filteredPtFile[256];
	strcpy(filteredPtFile, pdlg->m_strFilteredPoints);

	pdlg->m_bIsThreadOn = true;

	FILE* progressFp = fopen(pdlg->m_strProgressFile, "w");
	fprintf(progressFp, "%lf", 0.0);
	fclose(progressFp);

	
	//1. Generate camera file
	pdlg->m_pDlgProgressDlg->SetWindowTextA("生成相机投影文件.....");
	if( bIsFileExit(listFile) && bIsFileExit(bundleoutfile))
	{
		sprintf(cmdString, "%s\\Bundle2PMVS.exe %s %s %s",
			pdlg->m_strExePath,
			pdlg->m_strListFile, 
			pdlg->m_strBundleOutFile,
			pdlg->m_strMVSDirectory);	
		printf("%s \n", cmdString);
		InvokeExe(cmdString);
	}
	pdlg->m_pDlgProgressDlg->SetProgressValue(5);

	
	//2. Undistort images 
	pdlg->m_pDlgProgressDlg->SetWindowTextA("图像畸变校正.....");
	if( bIsFileExit(listFile) && bIsFileExit(bundleoutfile))
	{
		sprintf(cmdString, "%s\\RadialUndistort.exe %s %s %s",
			pdlg->m_strExePath,
			pdlg->m_strListFile, 
			pdlg->m_strBundleOutFile,
			pdlg->m_strMVSDirectory);	
		printf("%s \n", cmdString);
		InvokeExe(cmdString);
	}
	
	//3. Bundle2Vis 
	pdlg->m_pDlgProgressDlg->SetWindowTextA("可视化处理.....");
	if( bIsFileExit(listFile) && bIsFileExit(bundleoutfile))
	{
		char bundle_rd_file[512];
		sprintf(bundle_rd_file, "%s\\bundle.rd.out", pdlg->m_strMVSDirectory);
		char visdatFile[512];
		sprintf(visdatFile, "%s\\vis.dat", pdlg->m_strMVSDirectory);

		sprintf(cmdString, "%s\\Bundle2Vis.exe %s %s",
			pdlg->m_strExePath,
			bundle_rd_file,
			visdatFile);	
		printf("%s \n", cmdString);
		InvokeExe(cmdString);
	}
		
	//4. CMVS 
	pdlg->m_pDlgProgressDlg->SetWindowTextA("分块处理.....");
	if( bIsFileExit(listFile) && bIsFileExit(bundleoutfile))
	{
		sprintf(cmdString, "%s\\cmvs.exe %s 20 6",  //prefix maximage[=100] CPU[=4]
			pdlg->m_strExePath,
			pdlg->m_strMVSDirectory+"\\");	
		printf("%s \n", cmdString);
		InvokeExe(cmdString);
	}
		
	//5. genOption
	pdlg->m_pDlgProgressDlg->SetWindowTextA("分块处理.....");
	if( bIsFileExit(listFile) && bIsFileExit(bundleoutfile))
	{
		sprintf(cmdString, "%s\\genOption.exe %s 4 2 0.7 7 2 ", //level csize threshold wsize minImageNum
			pdlg->m_strExePath,
			pdlg->m_strMVSDirectory+"\\");	
		printf("%s \n", cmdString);
		InvokeExe(cmdString);
	}	
	
	//6. PMVS
	progressFp = fopen(pdlg->m_strProgressFile, "w");
	fprintf(progressFp, "%lf", 0.0);
	fclose(progressFp);

	pdlg->m_pDlgProgressDlg->SetWindowTextA("密集匹配.....");
	if( bIsFileExit(listFile) && bIsFileExit(bundleoutfile))
	{		
		//int nOptionFile = GetFileRows()
		for(int i=0; i<100; i++)
		{
			char absOptionFile[512];
			sprintf(absOptionFile, "%s\\option-%.4d", pdlg->m_strMVSDirectory, i);
			
			if(bIsFileExit(absOptionFile))
			{
				char optionfile[512];					
				sprintf(optionfile, "option-%.4d", i);

				sprintf(cmdString, "%s\\pmvs2.exe %s %s",
				pdlg->m_strExePath,
				pdlg->m_strMVSDirectory+"\\",
				optionfile);	
				printf("%s \n", cmdString);
				InvokeExe(cmdString);
			}
		}
	}	
	
		
	//////////////////////////////////////////////////////////////////////////
	//6. merge the ply
	char modelPath[512];
	strcpy(modelPath, pdlg->m_strMVSModelDirectory);
	char** filenames = NULL;
	int n=0,nfile=0;
	GetDirFileName(filenames, modelPath, &n, &nfile, "ply", 0);
	filenames = f2c(nfile, 256);
	GetDirFileName(filenames, modelPath, &n, &nfile, "ply", 1);

	vector<stTrack> allTracks;
	for(int i=0; i<nfile; i++)
	{
		stTrack* st;
		int nTrack;
		ReadPMVSPly(filenames[i], &st, &nTrack);
		
		for(int k=0; k<nTrack; k++)
		{
			allTracks.push_back( st[k] );
		}
		free(st);
	}
	printf("ply file: %s \n", pdlg->m_strPMVSPlyFile);
	
	char plyfile[512];
	strcpy(plyfile, pdlg->m_strPMVSPlyFile);
	WritePMVSPly(plyfile, allTracks);
	//////////////////////////////////////////////////////////////////////////
	
	
	pdlg->m_bIsThreadOn = false;

	pdlg->m_pDlgProgressDlg->ShowWindow(SW_HIDE);
	pdlg->KillTimer(100);
	pdlg->SetMenuEnable();

	return 0;
}

UINT CMFCUIDlg::ProcessOrtho(LPVOID pParam)
{
	CMFCUIDlg *pdlg = (CMFCUIDlg * )pParam;

	char cmdString[256];

	char listFile[256];
	strcpy(listFile, pdlg->m_strListFile);
	char matchfile[256];
	strcpy(matchfile, pdlg->m_strMatchFile);
	char bundleoutfile[256];
	strcpy(bundleoutfile, pdlg->m_strBundleOutFile);
	char filteredPtFile[256];
	strcpy(filteredPtFile, pdlg->m_strFilteredPoints);
	char pmvsPlyFile[256];
	strcpy(pmvsPlyFile, pdlg->m_strPMVSPlyFile);

	pdlg->m_bIsThreadOn = true;

	//5. point cloud smoothing based on sparse point cloud
	printf("point cloud smoothing .... \n");
	if( bIsFileExit(pmvsPlyFile) )
	{
		sprintf(cmdString, "%s\\Project_PCLFilter_VS2010.exe %s %d %lf %s ",
			pdlg->m_strExePath, 
			pmvsPlyFile, 
			pdlg->m_nNeiborPts,
			pdlg->m_dSigma,
			filteredPtFile);
		InvokeExe(cmdString);
	}
	else if( bIsFileExit(bundleoutfile) )
	{		
		sprintf(cmdString, "%s\\Project_PCLFilter_VS2010.exe %s %d %lf %s ",
			pdlg->m_strExePath, 
			bundleoutfile, 
			pdlg->m_nNeiborPts*0.1,
			pdlg->m_dSigma,
			filteredPtFile);	
		InvokeExe(cmdString);
	}
	else
	{
		printf("No point cloud \n");
		return -1;
	}

	//6. generate ortho-images
	printf("generating orthoimage .... \n");
	pdlg->m_pDlgProgressDlg->SetProgressValue(0);
	pdlg->m_pDlgProgressDlg->SetWindowTextA(" 生成正射影像.....");
	if( bIsFileExit(listFile) && bIsFileExit(bundleoutfile) && bIsFileExit(filteredPtFile)  ) // && bIsBundler )
	{
		sprintf(cmdString, 
			"%s\\Project_OrthoImage_VS2005.exe -l %s  -b %s  -s %s  -o %s -g 1 -r %d",
			pdlg->m_strExePath, 
			listFile, 
			bundleoutfile,
			filteredPtFile,
			"NULL", 
			1);
		InvokeExe(cmdString);
	}
	
	pdlg->m_pDlgProgressDlg->ShowWindow(SW_HIDE);
	pdlg->KillTimer(100);

	pdlg->m_bIsThreadOn = false;

	CString strlog = "正射影像生成完毕，请点击 '处理->输出' 来镶嵌图像。 \r\n";
	pdlg->OutputMessage(strlog);	
	
	return 0;
}


UINT  CMFCUIDlg::ProcessAll(LPVOID pParam)
{
	CMFCUIDlg *pdlg = (CMFCUIDlg * )pParam;

	pdlg->m_bIsThreadOn = true;

	char cmdString[256];


	char imagepathFile[256];
	strcpy(imagepathFile, pdlg->m_strImageListFile);
	char listFile[256];
	strcpy(listFile, pdlg->m_strListFile);
	char matchfile[256];
	strcpy(matchfile, pdlg->m_strMatchFile);
	char bundleoutfile[256];
	strcpy(bundleoutfile, pdlg->m_strBundleOutFile);
	char filteredPtFile[256];
	strcpy(filteredPtFile, pdlg->m_strFilteredPoints);

	
	FILE* progressFp = fopen(pdlg->m_strProgressFile, "w");
	fprintf(progressFp, "%lf", 0.0);
	fclose(progressFp);

	//1. generate list.txt 
	pdlg->m_pDlgProgressDlg->SetProgressValue(30);
	pdlg->m_pDlgProgressDlg->SetWindowTextA("提取图像头文件信息.....");
	char imageListFile[256];
	strcpy(imageListFile, pdlg->m_strImageListFile);
	if( bIsFileExit( imageListFile ) )
	{
		sprintf(cmdString, "%s\\Project_GenerateList_VS2005.exe %s %s %s",
			pdlg->m_strExePath,
			pdlg->m_strImageListFile, 
			pdlg->m_strListFile, 
			pdlg->m_strPosFile);	
		printf("%s \n", cmdString);
		InvokeExe(cmdString);
	}
	//loading pos.txt and draw points
	pdlg->LoadPOSData();

		
	//2. detect feature points
	pdlg->m_pDlgProgressDlg->SetProgressValue(0);
	pdlg->m_pDlgProgressDlg->SetWindowTextA(" [共5步|第1步]: 提取特征点.....");
	
	/*
	int nImage = GetFileRows(listFile);
	double fstep = 100.0 / double(nImage);
	FILE* fp = fopen(pdlg->m_strListFile, "r");
	for(int i=0; i<nImage; i++)
	{
		FILE* progressFp = fopen(pdlg->m_strProgressFile, "w");
		fprintf(progressFp, "%lf", fstep*i);
		fclose(progressFp);

		char imageFile[256];
		int  ti;
		double focalLen;
		fscanf(fp, "%s %d %lf", imageFile, &ti, &focalLen);

		//generate key file path
		//char keyfile[256];			
		//strcpy(keyfile, imageFile);
		//strcpy(keyfile+strlen(keyfile)-3, "key");
		//printf("%s \n", keyfile);			
		//printf("keyfile: %s \n", keyfile);


		char* keyfile = (char*)malloc(256);
		GenerateProductFile(imageFile, "product", "key", &keyfile);

		if( bIsFileExit(keyfile) )
			continue;

		sprintf(cmdString, "%s\\Project_SIFT.exe %s %s 1000",
			pdlg->m_strExePath, imageFile, keyfile);

		free(keyfile);

		//sprintf(cmdString, "%s\\glsl-sift.exe -i %s -o %s -maxd 1600 -d 4 ",
		//	pdlg->m_strExePath, imageFile, keyfile);

		InvokeExe(cmdString);
	}
	fclose(fp);

	progressFp = fopen(pdlg->m_strProgressFile, "w");
	fprintf(progressFp, "%lf", 0.0);
	fclose(progressFp);*/

	//multiple core version, added by xiedonghai, 2016.5.13
	sprintf(cmdString, "%s\\Project_SIFT.exe %s %d",
		pdlg->m_strExePath, imagepathFile, pdlg->m_nScaleImageHt);
	InvokeExe(cmdString);
	/////////////////////////////////////////////////

	//3. matching
	pdlg->m_pDlgProgressDlg->SetProgressValue(0);
	pdlg->m_pDlgProgressDlg->SetWindowTextA("[共5步|第2步]: 特征点匹配....");
	if( bIsFileExit(matchfile) == 0 )
	{
		//sprintf(cmdString, "%s\\KeyMatchFull.exe %s %s 0.6",
		//	pdlg->m_strExePath, pdlg->m_strListFile, pdlg->m_strMatchFile);

		//sprintf(cmdString, "%s\\glsl-match.exe %s %s",
		//	pdlg->m_strExePath, pdlg->m_strListFile, pdlg->m_strMatchFile);

		//multi-core version, added by xiedonghai, 2016.5.13
		sprintf(cmdString, "%s\\%s %s %s 0.6",
			pdlg->m_strExePath, 
			pdlg->m_strMatchProgram, 
			pdlg->m_strListFile, 
			pdlg->m_strMatchFile);

		InvokeExe(cmdString);
	}

	//4. bundle	
	pdlg->m_pDlgProgressDlg->SetProgressValue(0);
	pdlg->m_pDlgProgressDlg->SetWindowTextA("[共5步|第3步]: 空三处理.....");
	bool bIsBundler = false;
	if( bIsFileExit(bundleoutfile) == 0 )
	{		
		//delete bundle directory
		//sprintf(cmdString, "cmd /c rd %s\\bundle /s /q", pdlg->m_strExePath);
		//InvokeExe(cmdString);

		//make bundle directory
		//sprintf(cmdString, "cmd /c mkdir %s\\bundle", pdlg->m_strExePath);
		//InvokeExe(cmdString);

		//bunder
		char optionFile[256];
		sprintf(optionFile, "%s\\options.txt", pdlg->m_strExePath);
		sprintf(cmdString, "%s\\%s %s --match_table %s --options_file %s",
			pdlg->m_strExePath,
			pdlg->m_strBundleProgram,
			pdlg->m_strListFile, 
			pdlg->m_strMatchFile, 
			optionFile);
		InvokeExe(cmdString);

		//copy the final results
		sprintf(cmdString, "cmd /c copy %s\\bundle\\bundle.out  %s", 
			pdlg->m_strExePath, pdlg->m_strProductPath);
		//InvokeExe(cmdString);

		bIsBundler = true;
	}


	//5. point cloud smoothing
	char pmvsPlyFile[256];
	strcpy(pmvsPlyFile, pdlg->m_strPMVSPlyFile);
	if( bIsFileExit(pmvsPlyFile) )
	{
		sprintf(cmdString, "%s\\Project_PCLFilter_VS2010.exe %s %d %lf %s ",
			pdlg->m_strExePath, 
			pmvsPlyFile, 
			pdlg->m_nNeiborPts,
			pdlg->m_dSigma,
			filteredPtFile);

		InvokeExe(cmdString);
	}
	else if( bIsFileExit(bundleoutfile) )
	{		

		sprintf(cmdString, "%s\\Project_PCLFilter_VS2010.exe %s %d %lf %s ",
			pdlg->m_strExePath, 
			bundleoutfile, 
			pdlg->m_nNeiborPts*0.1,
			pdlg->m_dSigma,
			filteredPtFile);

		InvokeExe(cmdString);
	}
	else
	{
		printf("No point cloud \n");
		return -1;
	}

	//if( bIsFileExit(bundleoutfile) )
	//{
	//	sprintf(cmdString, "%s\\Project_PCLFilter_VS2010.exe %s 60 0.6 %s ",
	//		pdlg->m_strExePath, 
	//		bundleoutfile, 
	//		filteredPtFile);
	//	InvokeExe(cmdString);
	//}	

	//6. generate ortho-images
	pdlg->m_pDlgProgressDlg->SetProgressValue(0);
	pdlg->m_pDlgProgressDlg->SetWindowTextA("[共5步|第4步]: 生成正射影像.....");
	if( bIsFileExit(listFile) && bIsFileExit(bundleoutfile) && bIsFileExit(filteredPtFile)  ) // && bIsBundler )
	{
		sprintf(cmdString, 
			"%s\\Project_OrthoImage_VS2005.exe -l %s  -b %s  -s %s  -o %s -g 1 -r %d",
			pdlg->m_strExePath, 
			listFile, 
			bundleoutfile,
			filteredPtFile,
			"NULL", 
			1);
		InvokeExe(cmdString);
	}

	//7. mosaic and blend
	//char listFile[256];
	//strcpy(listFile, pdlg->m_strListFile);
	char outfile[256];
	strcpy(outfile, pdlg->m_strMosaicFile);
	printf("mosaic resolution: %lf \n", pdlg->m_dMosaicResolution);
	
	//int type = 1;	
	int type = 0;
	if(pdlg->m_bIsBlend)
		type = 1;
	else
		type = 0;

	pdlg->m_pDlgProgressDlg->SetWindowTextA("[共5步|第5步]: 图像镶嵌.....");
	if( bIsFileExit(listFile) )
	{
		pdlg->m_dMosaicResolution = GetResolution(listFile)*pdlg->m_dMosaicRatio;

		sprintf(cmdString, "%s\\Project_Fusion_VS2010.exe %s %s %lf %d",
			pdlg->m_strExePath, 
			imageListFile, 
			outfile, 
			pdlg->m_dMosaicResolution, 
			type);
		InvokeExe(cmdString);
	}

	//save project
	char projectfile[256];
	strcpy(projectfile, pdlg->m_strProjectFile);
	pdlg->SaveProject(projectfile);
	
	pdlg->m_pDlgProgressDlg->ShowWindow(SW_HIDE);
	pdlg->KillTimer(100);
	pdlg->SetMenuEnable();
	pdlg->m_bIsThreadOn = false;

	CString strlog;
	strlog.Format("拼接完成，镶嵌图像存储到:  ' %s '  中。\r\n", outfile);
	pdlg->OutputMessage(strlog);	
	strlog.Format("处理完成，工程文件存储到:  ' %s '  中。\r\n", pdlg->m_strProjectFile);
	pdlg->OutputMessage(strlog);	

	return 0;
}

void CMFCUIDlg::OnMosaic()
{
	// TODO: 在此添加命令处理程序代码

	//CDlgProgress dlgProgress;
	//dlgProgress.DoModal();	
	//if(m_pDlgProgressDlg!=NULL)
	//	delete m_pDlgProgressDlg;		
	//m_pDlgProgressDlg = new CDlgProgress(this);
	//m_pDlgProgressDlg->Create(IDD_DIALOG_PROGRESS, this);
	//m_pDlgProgressDlg->CenterWindow(this);
	//m_pDlgProgressDlg->ShowWindow(SW_SHOW);
	
	/*
	//determine if the path exist empty 
	if(m_fileVector.size()<2)
	{
		AfxMessageBox("图像数量太少！");
		return;
	}
	
	CString imagepath = m_fileVector[0];
	if( imagepath.Find(' ')!=-1 )
	{
		AfxMessageBox("图像目录和文件的路径中不能有空格！");
		return;
	}
	*/

	if(m_pDlgProgressDlg==NULL)
	{
		m_pDlgProgressDlg = new CDlgProgress(this);
		m_pDlgProgressDlg->Create(IDD_DIALOG_PROGRESS, this);
		m_pDlgProgressDlg->CenterWindow(this);
	}

	m_pDlgProgressDlg->ShowWindow(SW_SHOW);
	
	
	SetTimer(100, 1000, 0);
	
	//if( m_pMyThread ==NULL )
	m_pMyThread = AfxBeginThread(Process, (LPVOID)this);
	
}


void CMFCUIDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值			
	if( AfxMessageBox("确定退出吗？", MB_YESNO)==IDNO )
	{
		return;
	}
	
	if(m_pMyThread)
	{
		//::PostThreadMessage(m_pMyThread->m_nThreadID, WM_THREAD_STOP,0,0);
		TerminateThread(m_pMyThread->m_hThread, 0);

		CloseProgram(m_strDetectProgram);
		
		CloseProgram(m_strDetectProgramSift);
		
		if( CloseProgram(m_strMatchProgram)>0 )
			DeleteFile(m_strMatchFile);

		if( CloseProgram(m_strMatchProgramSift)>0 )
			DeleteFile(m_strMatchFile);

		if( CloseProgram(m_strBundleProgram)>0 )
			DeleteFile(m_strBundleOutFile);

		CloseProgram(m_strOrthoProgram);
		CloseProgram(m_strFusionProgram);	

		CloseProgram(m_strPMVS);
		CloseProgram(m_strCMVS);
		CloseProgram(m_strRadialUndistort);
		CloseProgram(m_strOptionGen);
	}
		
	char tfile[256];
	strcpy(tfile, m_strListFile);		
	if(bIsFileExit(tfile)>0 && !m_bIsProjectSave )
	{
		if( AfxMessageBox("需要保存工程吗？", MB_YESNO) ==IDYES )
		{
			OnSaveProject();
		}
	}
	
	if(m_pDlgProgressDlg!=NULL)
		delete m_pDlgProgressDlg;

	CDialogEx::OnClose();
}


void CMFCUIDlg::OnOutputBlendortho()
{	
	// TODO: 在此添加命令处理程序代码

	double minimumResolution = 0.01;
	char listfile[256];
	strcpy(listfile, m_strListFile);
	if(bIsFileExit(listfile))
	{
		minimumResolution = GetResolution(listfile);
		m_dMinimumResolution = minimumResolution;

		if(minimumResolution==0)
		{
			AfxMessageBox(" 不能进行镶嵌，请先进行空三处理！");
			return;
		}
	}
	else
	{
		AfxMessageBox("请先添加图像和打开已有的工程文件！");
		return;
	}	

	CDlgBlendInput dlgBlend;
	dlgBlend.SetMinimumResolution(minimumResolution);
	if( dlgBlend.DoModal()==IDOK )
	{
		m_strMosaicFile = dlgBlend.m_strMosaicFileName;

		if( m_strMosaicFile.Find(" ")>0 )
		{
			AfxMessageBox(" 图像路径或图像名称中不能有空格！ ");
			return;
		}

		m_dMosaicResolution = max(minimumResolution, dlgBlend.m_dResolution);
		printf("output resolution: %lf \n", m_dMosaicResolution);

		m_bIsBlend = dlgBlend.m_bIsBlend;

		if(m_strMosaicFile.IsEmpty())
		{
			AfxMessageBox("输出图像文件为空！ ");
			return;
		}		
		
		//WriteProgressValueToFile(0);
		FILE* progressFp = fopen(m_strProgressFile, "w");
		if(progressFp != NULL)
		{
			fprintf(progressFp, "%lf", 0.0);
			fclose(progressFp);
		}

		if(m_pDlgProgressDlg==NULL)
		{
			m_pDlgProgressDlg = new CDlgProgress(this);
			m_pDlgProgressDlg->Create(IDD_DIALOG_PROGRESS, this);
			m_pDlgProgressDlg->CenterWindow(this);
		}
		m_pDlgProgressDlg->SetProgressValue(0);
		m_pDlgProgressDlg->ShowWindow(SW_SHOW);

		SetTimer(100, 1000, 0);
		m_pMyThread = AfxBeginThread(ProcessBlend, (LPVOID)this);		
	}	
}


void CMFCUIDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	//printf(".");
	ReadProgressValueFile(m_dProgressValue);

	//printf("%d ", m_nProgressValue);
	//if(m_pDlgProgressDlg == NULL)
	//	m_pDlgProgressDlg = new CDlgProgress();	
	
	//m_pDlgProgressDlg->ShowWindow(SW_SHOW);
	m_pDlgProgressDlg->SetProgressValue( int(m_dProgressValue) );

	CDialogEx::OnTimer(nIDEvent);
}

/*
int TestFileStorageWrite()
{
	string file = "d:\\test.yml";
	FileStorage fs(file, FileStorage::WRITE);
	fs << "frameCount" << 5;
	time_t rawtime; time(&rawtime);
	fs << "calibrationDate" << asctime(localtime(&rawtime));
	Mat cameraMatrix = (Mat_<double>(3,3) << 1000, 0, 320, 0, 1000, 240, 0, 0, 1); //又一种Mat初始化方式
	Mat distCoeffs = (Mat_<double>(5,1) << 0.1, 0.01, -0.001, 0, 0);
	fs << "cameraMatrix" << cameraMatrix << "distCoeffs" << distCoeffs;

	//features为一个大小为3的向量,其中每个元素由随机数x,y和大小为8的uchar数组组成
	fs << "features" << "[";
	for( int i = 0; i < 3; i++ )
	{
		int x = rand() % 640;
		int y = rand() % 480;
		uchar lbp = rand() % 256;
		fs << "{:" << "x" << x << "y" << y << "lbp" << "[:";
		for( int j = 0; j < 8; j++ )
			fs << ((lbp >> j) & 1);
		fs << "]" << "}";
	}
	fs << "]";
	fs.release();

	return 0;
}
*/

int WriteTxtFile(FILE* dstFp, char* inputFileName)
{	
	FILE* fp = fopen(inputFileName, "r");
	//if(fp==NULL)
	//	return -1;
	int rows = GetFileRows(inputFileName);
	//fprintf(dstFp, "%s %d \n", inputFileName, rows);
	fprintf(dstFp, "%d\n", rows);
	for(int i=0; i<rows; i++)
	{
		char sline[256];
		fgets(sline, 256, fp);
		fprintf(dstFp, "%s", sline);
	}
	fclose(fp);
	
	return 0;
}


int CMFCUIDlg::SaveProject(char* filename)
{
	char tfile[256];
	strcpy(tfile, m_strListFile);		
	if(bIsFileExit(tfile)==0)
		return -1;

	FILE* fp = fopen(filename, "w");
	for(int i=0; i<m_projectFilesVec.size(); i++)
	{
		char tfile[256];
		strcpy(tfile, m_projectFilesVec[i]);		
		if(bIsFileExit(tfile)>0)
			fprintf(fp, "%d ", 1);
		else
			fprintf(fp, "%d ", 0);
	}
	fprintf(fp, "\n");
	
	
	double step = 100.0 / m_projectFilesVec.size();
	
	
	if( m_pDlgProgressDlg == NULL )
	{
		m_pDlgProgressDlg = new CDlgProgress();
		m_pDlgProgressDlg->Create(IDD_DIALOG_PROGRESS, this);
		m_pDlgProgressDlg->CenterWindow(this);
		m_pDlgProgressDlg->ShowWindow(SW_SHOW);
	}
	
	m_pDlgProgressDlg->ShowWindow(SW_SHOW);
	m_pDlgProgressDlg->SetWindowTextA("保存工程.....");
	m_pDlgProgressDlg->SetProgressValue(0);
		
	for(int i=0; i<m_projectFilesVec.size(); i++)
	{
		char tfile[256];
		strcpy(tfile, m_projectFilesVec[i]);		
		
		if(bIsFileExit(tfile)==0)
			continue;
		WriteTxtFile(fp, tfile);
		
		m_pDlgProgressDlg->SetProgressValue( (i+1)*step );
	}
	fclose(fp);
	
	m_pDlgProgressDlg->SetProgressValue(100);
	m_pDlgProgressDlg->ShowWindow(SW_HIDE);

	return 0;
}


void CMFCUIDlg::OnSaveProject()
{
	// TODO: 在此添加命令处理程序代码

	CFileDialog dlg(FALSE, NULL, NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("Project file (*.sfp)|*.sfp|"));

	CString strPath;
	if(dlg.DoModal()==IDOK)
	{
		strPath = dlg.GetPathName();
		CString strExt = dlg.GetFileExt();
		printf("file ext: %s \n", strExt);

		if( strExt!="sfp" )
			m_strProjectFile = strPath + ".sfp";
		else
			m_strProjectFile = strPath;

		printf("project file: %s \n", m_strProjectFile);
		
		char projectfile[256];
		strcpy(projectfile, m_strProjectFile);
		SaveProject(projectfile);

		m_bIsProjectSave = true;
		//m_pDlgProgressDlg->ShowWindow(SW_HIDE);
	}
	else
	{
		return;
	}	
}

void CMFCUIDlg::OnOpenProject()
{
	// TODO: 在此添加命令处理程序代码
	CFileDialog dlg(TRUE, NULL, NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("Project file (*.sfp)|*.sfp|"));
	
	CString strPath;
	if(dlg.DoModal()==IDOK)
	{
		strPath = dlg.GetPathName();		
		m_strProjectFile = strPath;

		char projectfile[256];
		strcpy(projectfile, m_strProjectFile);
		
		if(m_pDlgProgressDlg==NULL)
		{
			m_pDlgProgressDlg = new CDlgProgress(this);
			m_pDlgProgressDlg->Create(IDD_DIALOG_PROGRESS, this);
			m_pDlgProgressDlg->CenterWindow(this);
		}
		m_pDlgProgressDlg->ShowWindow(SW_SHOW);
		m_pDlgProgressDlg->SetWindowTextA("打开工程.....");

		OpenProject(projectfile);

		m_pDlgProgressDlg->ShowWindow(SW_HIDE);
	
		SetMenuEnable();
	}
}

int CMFCUIDlg::OpenProject(char* filename)
{	
	int nProjectFile = m_projectFilesVec.size();
	vector<int> vecFileTag;
	vecFileTag.resize(nProjectFile);

	FILE* fp = fopen(filename, "r");
	for(int i=0; i<nProjectFile; i++)
	{
		int tag;
		fscanf(fp, "%d", &tag);
		vecFileTag[i]=tag;
	}

	//m_pDlgProgressDlg->ShowWindow(SW_SHOW);

	double step = 60.0 / double(nProjectFile);

	for(int i=0; i<nProjectFile; i++)
	{
		m_pDlgProgressDlg->SetProgressValue( step*(i+1) );

		if(vecFileTag[i]==0)
			continue;

		int numline = 0;
		fscanf(fp, "%d", &numline);
		int ch = fgetc(fp);
		FILE* dstFp=fopen(m_projectFilesVec[i], "w");
		for(int k=0; k<numline; k++)
		{
			char sline[256];
			fgets(sline, 256, fp);
			fprintf(dstFp, "%s", sline);
		}
		fclose(dstFp);
	}

	//reading list.txt
	char listFile[256];
	strcpy(listFile, m_strListFile);
	char imagelistFile[256];
	strcpy(imagelistFile, m_strImageListFile);

	if(bIsFileExit(listFile))
	{
		m_fileVector.clear();

		int nLine = GetFileRows(listFile);

		FILE* fpImageList = fopen(imagelistFile, "w");
		FILE* fp = fopen(listFile, "r");
		for(int i=0; i<nLine; i++)
		{
			char filename[256];
			int t;
			double f;
			fscanf(fp, "%s %d %lf", filename, &t, &f);

			fprintf(fpImageList, "%s \n", filename);

			CString sFile;
			sFile.Format("%s", filename);
			m_fileVector.push_back(sFile);
		}
		fclose(fp);
		fclose(fpImageList);

		m_fileList.DeleteAllItems();		
		int index = 0;
		for(int i=0; i<m_fileVector.size(); i++ )
		{
			m_fileList.InsertItem(index, m_fileVector[i]);
			index++;
		}

		LoadPOSData();

		InitProductDirectory();

		CString strlog;
		strlog.Format("加载了%d张图像 \r\n", m_fileVector.size());
		m_strLog.Empty();
		OutputMessage(strlog);
	}
	
	m_pDlgProgressDlg->SetProgressValue(100.0);
	//m_pDlgProgressDlg->ShowWindow(SW_HIDE);
	
	return 0;
}

int CMFCUIDlg::DeleteTempProjectFiles(void)
{
	for(int i=0; i<m_projectFilesVec.size(); i++)
	{
		bool bIsSuccess = DeleteFile(m_projectFilesVec[i]);
		printf("%d \n", bIsSuccess);
	}	
	return 0;
}


void CMFCUIDlg::OnCloseProject()
{
	// TODO: 在此添加命令处理程序代码
	m_fileVector.clear();
	m_fileList.DeleteAllItems();
	
	DeleteTempProjectFiles();

	LoadPOSData();

	SetMenuEnable();

	m_strLog.Empty();
	OutputMessage(m_strLog);
}


void CMFCUIDlg::OnDeleteImages()
{
	// TODO: 在此添加命令处理程序代码
	m_fileVector.clear();
	m_fileList.DeleteAllItems();

	DeleteTempProjectFiles();

	LoadPOSData();

	SetMenuEnable();

	m_strLog.Empty();
	OutputMessage(m_strLog);
}


int CMFCUIDlg::LoadPOSData(void)
{
	vector<POSData> vecPosData;

	char file[256];
	strcpy(file, m_strPosFile);
	//if( bIsFileExit(file) ==0 )
	//	return -1;
	
	int nLine = GetFileRows(file);
	POSData posRecord;
	FILE* fp = fopen(file, "r");
	if(fp!=NULL)
	{	
		for(int i=0; i<nLine; i++)
		{
			fscanf(fp, "%lf %lf %lf ", &(posRecord.lon), &(posRecord.lat), &(posRecord.alt));
			vecPosData.push_back(posRecord);
		}
		fclose(fp);
	}
	m_dialogDisp.SetPosData(vecPosData);
	
	return 0;
}


void CMFCUIDlg::OutputMessage(CString strMessage)
{
	m_strLog += strMessage;
	m_editOutput.SetWindowText(m_strLog);
	//UpdateData(true);
}



int CMFCUIDlg::SetMenuEnable(void)
{

	char cfile[256];
	strcpy(cfile, m_strListFile);

	char bundlefile[256];
	strcpy(bundlefile, m_strBundleOutFile);


	if(bIsFileExit(cfile))
	{
		m_menu.EnableMenuItem(ID_MOSAIC, MF_ENABLED);
		m_menu.EnableMenuItem(ID_MVS, MF_ENABLED);			
		m_menu.EnableMenuItem(ID_GENERATE_ORTHO, MF_ENABLED);
		m_menu.EnableMenuItem(ID_FLY_QUALITY, MF_ENABLED);	
	}
	else
	{
		m_menu.EnableMenuItem(ID_MOSAIC, MF_GRAYED);
		m_menu.EnableMenuItem(ID_MVS, MF_GRAYED);
		m_menu.EnableMenuItem(ID_GENERATE_ORTHO, MF_GRAYED);	
		m_menu.EnableMenuItem(ID_FLY_QUALITY, MF_GRAYED);	
	}

	if(bIsFileExit(bundlefile))
	{
		m_menu.EnableMenuItem(ID_MVS, MF_ENABLED);	
		m_menu.EnableMenuItem(ID_GENERATE_ORTHO, MF_ENABLED);
		m_menu.EnableMenuItem(ID_SAVE_PROJECT, MF_ENABLED);
		m_menu.EnableMenuItem(ID_CLOSE_PROJECT, MF_ENABLED);
	}
	else
	{
		m_menu.EnableMenuItem(ID_MVS, MF_GRAYED);	
		m_menu.EnableMenuItem(ID_GENERATE_ORTHO, MF_GRAYED);
		m_menu.EnableMenuItem(ID_SAVE_PROJECT, MF_GRAYED);
		m_menu.EnableMenuItem(ID_CLOSE_PROJECT, MF_GRAYED);
	}
	
	/*
	strcpy(cfile, m_strBundleOutFile);
	if(bIsFileExit(cfile))
	{
		m_menu.EnableMenuItem(ID_OUTPUT_BLENDORTHO, MF_ENABLED);
		m_menu.EnableMenuItem(ID_SAVE_PROJECT, MF_ENABLED);
		m_menu.EnableMenuItem(ID_CLOSE_PROJECT, MF_ENABLED);
	}
	else
	{
		m_menu.EnableMenuItem(ID_OUTPUT_BLENDORTHO, MF_GRAYED);
		m_menu.EnableMenuItem(ID_SAVE_PROJECT, MF_GRAYED);
		m_menu.EnableMenuItem(ID_CLOSE_PROJECT, MF_GRAYED);
	}
	*/

	return 0;
}


void CMFCUIDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码

	//printf("Message Return... \n");
	CDialogEx::OnOK();
}


void CMFCUIDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	
	CDialogEx::OnCancel();
}

BOOL CMFCUIDlg::PreTranslateMessage( MSG* pMsg)
{
	if( pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
	{
		printf("Message ESC... \n");
		return TRUE;      //屏蔽掉Esc键
	}
	if( pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		printf("Message RETURN... \n");
		return TRUE;      //屏蔽掉Enter键
	}

	return CDialog::PreTranslateMessage( pMsg );
}

void CMFCUIDlg::OnDispProgressbar()
{
	if( m_bIsThreadOn && m_pDlgProgressDlg!=NULL )
		m_pDlgProgressDlg->ShowWindow(SW_SHOW);
	
	// TODO: 在此添加命令处理程序代码


}


void CMFCUIDlg::OnAddMoreimages()
{
	// TODO: 在此添加命令处理程序代码

	m_bIsAddNewImages = false;
	OnLoadImages();
	m_bIsAddNewImages = true;
}


void CMFCUIDlg::OnBatchGo()
{
	// TODO: 在此添加命令处理程序代码

	CBatDialog batDlg;

	//set the file info into the batch processing dialog
	batDlg.SetData(m_fileVector);
	


	if(batDlg.DoModal()==IDOK)
	{
		if( batDlg.m_fileVector.size()<2 )
			return;

		m_fileVector     = batDlg.m_fileVector;
		m_strMosaicFile  = batDlg.m_sOutputMosaicFile;
		m_strProjectFile = batDlg.m_sOutputProjFile;
		m_dMosaicRatio   = batDlg.m_dResolutionRatio;
		//m_dMosaicResolution = batDlg


		m_fileList.DeleteAllItems();
		int index = 0;
		for(int i=0; i<m_fileVector.size(); i++ )
		{
			m_fileList.InsertItem(index, m_fileVector[i]);
			index++;
		}

		m_strLog.Empty();
		//m_editOutput.SetWindowText("");
		CString strlog;
		strlog.Format("加载了%d张图像 \r\n", m_fileVector.size());
		m_strLog.Empty();
		OutputMessage(strlog);	

		//generate files for processing
		DeleteTempProjectFiles();

		//write the imagelist.txt		
		FILE* fp = fopen(m_strImageListFile, "w");
		if(fp==NULL)
		{
			AfxMessageBox("Error to write file !\n");
			return ;
		}
		for(int i=0; i<m_fileVector.size(); i++)
			fprintf(fp, "%s \n", m_fileVector[i]);
		fclose(fp);
		

		if(m_pDlgProgressDlg == NULL)
		{
			m_pDlgProgressDlg = new CDlgProgress(this);
			m_pDlgProgressDlg->Create(IDD_DIALOG_PROGRESS, this);
			m_pDlgProgressDlg->CenterWindow(this);
		}
		
		//thread
		m_pDlgProgressDlg->ShowWindow(SW_SHOW);
		SetTimer(100, 1000, 0);
		m_pMyThread = AfxBeginThread(ProcessAll, (LPVOID)this);
	}
}


void CMFCUIDlg::OnMvs()
{
	// TODO: 在此添加命令处理程序代码

	//make directory

	if(m_fileVector.size()<1)
	{
		AfxMessageBox("图像数量太少！");
		return;
	}

	CString path = m_fileVector[0];
	int index = path.ReverseFind('\\');
	path = path.Left(index+1);
	

	//generate mvs directory
	m_strMVSDirectory = path+"mvs";
	DeleteDir(m_strMVSDirectory);
	
	CreateDir(m_strMVSDirectory);	
	CString m_strMVSImageDirectory  = m_strMVSDirectory+"\\visualize";
	CreateDir(m_strMVSImageDirectory);	
	CString m_strMVSCameraDirectory = m_strMVSDirectory+"\\txt";
	CreateDir(m_strMVSCameraDirectory);	
	CString m_strMVSModelDirectory  = m_strMVSDirectory+"\\models";
	CreateDir(m_strMVSModelDirectory);
	


	if(m_pDlgProgressDlg==NULL)
	{
		m_pDlgProgressDlg = new CDlgProgress(this);
		m_pDlgProgressDlg->Create(IDD_DIALOG_PROGRESS, this);
		m_pDlgProgressDlg->CenterWindow(this);
	}
	m_pDlgProgressDlg->ShowWindow(SW_SHOW);
	
	SetTimer(100, 100, 0);

	//if( m_pMyThread==NULL )
	m_pMyThread = AfxBeginThread(ProcessMVS, (LPVOID)this);
	
}


void CMFCUIDlg::OnGenerateOrtho()
{
	// TODO: 在此添加命令处理程序代码


	if(m_pDlgProgressDlg==NULL)
	{
		m_pDlgProgressDlg = new CDlgProgress(this);
		m_pDlgProgressDlg->Create(IDD_DIALOG_PROGRESS, this);
		m_pDlgProgressDlg->CenterWindow(this);
	}

	m_pDlgProgressDlg->ShowWindow(SW_SHOW);


	SetTimer(100, 1000, 0);

	//if( m_pMyThread ==NULL )
	m_pMyThread = AfxBeginThread(ProcessOrtho, (LPVOID)this);

}


void CMFCUIDlg::OnFlyQuality()
{
	// TODO: 在此添加命令处理程序代码
	CQualityDialog qualityDlg;

	CString strExeFilePath = m_strExePath + "\\Project_FlyQuality_VS2005.exe";
	//sprintf(strExeFilePath, "%s\\Project_FlyQuality_VS2005.exe",m_strExePath);

	qualityDlg.SetFiles(strExeFilePath, m_strListFile);
	
	qualityDlg.DoModal();
}
