// AccuStudio.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include <afxcview.h>
#include <tchar.h> 
#include <stdio.h>

#include "AccuStudio.h" 
#include "AccuSpeech.h" 

#include "MainFrm.h"
#include "ChildFrm.h"
#include "AccuStudioDoc.h"
#include "AccuStudioView.h"
#include "project.h"
#include "projdoc.h"
#include "outdoc.h"
#include "picture.h"
#include "pcode.h"

#include "common.h"
#include "compilermodules.h"
#include "mainfrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CMultiDocTemplate* pDocTemplate, * pAppDoc, * pOutDoc, * pObjPicture;

CMainFrame* pMainFrame;
CString ExecDir;
extern char * * pnames;        //  For Captions, class names, etc..
extern int FindGramName(LPSTR pname);
extern BYTE * pgramparent;

/////////////////////////////////////////////////////////////////////////////
// CAccuStudioApp

BEGIN_MESSAGE_MAP(CAccuStudioApp, CWinApp)
	//{{AFX_MSG_MAP(CAccuStudioApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAccuStudioApp construction

CAccuStudioApp::CAccuStudioApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CAccuStudioApp object

CAccuStudioApp theApp;


void CAccuStudioApp::MFCIdle(void)
{
  MSG msg;

  while ( ::PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) ) 
  { 
      if ( !PumpMessage( ) ) 
      { 
//            bDoingBackgroundProcessing = FALSE; 
//            ::PostQuitMessage( ); 
          break; 
      } 
  } 
  // let MFC do its idle processing
  LONG lIdle = 0;
  while ( AfxGetApp()->OnIdle(lIdle++ ) )
      ;  
}


/////////////////////////////////////////////////////////////////////////////
// CAccuStudioApp initialization

void FixRes(LPSTR p)
{
	char parg[1024];
	strcpy(parg, p);
	ResPath = DirPath;
	ResName = parg;
	p = strrchr(parg,'\\');
	if (!p)
		p = strrchr(parg,'/');
	if (p) // Full path to the rc?
	{
		ResName = p + 1;
		*(p+1) = 0;
		ResPath = parg;
	}
}

BOOL CAccuStudioApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

	char Buf[1024];
	
	CString path = this->m_pszHelpFilePath;
	strcpy(Buf, path.GetBuffer(0));
	LPSTR p = strrchr(Buf, '\\');
	if (p)
	{
		*p = 0;
		BOOL res = SetCurrentDirectory(Buf);
	}
	

	GetCurrentDirectory(256, (LPSTR)Buf);


  strcat(Buf,"\\"); 
  ExecDir = Buf;

//	MessageBox(NULL, ExecDir, NULL, MB_OK);

	LPDIALOGRO		pdialog = NULL;

//	if (strchr(Buf, ' '))
//		MessageBox(NULL,
//		"The file path to the AccuSpeech Studio cannot have spaces.", "Path Error", MB_OK); 

/*
#undef _AFXDLL

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
*/

  // Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
//	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	LoadStdProfileSettings(NumMRU);  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

    
	pAppDoc = new CMultiDocTemplate(
		IDR_ACCUSTPTYPE, 
		RUNTIME_CLASS(CProjDoc),
		RUNTIME_CLASS(CMDIChildWnd), // custom MDI child frame
		RUNTIME_CLASS(CProjView)); 
  AddDocTemplate(pAppDoc);

	pDocTemplate = new CMultiDocTemplate(
		IDR_ACCUSTTYPE, //\nAccuSpeech\nAccuSpeech\n\n\nAccuStudio.Document\nAccuSpeech Document
		RUNTIME_CLASS(CAccuStudioDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CAccuStudioView));  
  AddDocTemplate(pDocTemplate);

	pOutDoc = new CMultiDocTemplate(
		IDR_ACCUSTPTYPE, 
		RUNTIME_CLASS(COutDoc),
		RUNTIME_CLASS(CMDIChildWnd), // custom MDI child frame
		RUNTIME_CLASS(COutView)); 
  AddDocTemplate(pOutDoc);

	pObjPicture = new CMultiDocTemplate(
		IDR_ACCUSTPTYPE, 
		RUNTIME_CLASS(CPictureDoc),
		RUNTIME_CLASS(CMDIChildWnd), // custom MDI child frame
		RUNTIME_CLASS(CPictureView)); 
  AddDocTemplate(pObjPicture);

  SetDialogBkColor(RGB(465,465,465));

	// create main MDI Frame window
	pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;

  /*
	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
  */


	// The main window has been initialized, so show and update it.
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

  AfxInitRichEdit( );
  
  Disable(ID_FILE_NEW_PROJECT);
  Enable(ID_FILE_NEW_MENU);
  Disable(ID_VIEW_OBJECTWIZARD);
  Disable(ID_FILE_SAVE_PROJECT);
  Disable(ID_FILE_CLOSE_PROJECT);
  Disable(ID_BUILD_BUILD_PROJECT);
  Disable(ID_VIEW_RESULTS);
  Disable(ID_PROJECT_ADD_GRAMMARSET);
  Disable(ID_PROJECT_ADDGRAMMARFILES);
  Disable(ID_PROJECT_ADD_GRAMMAR);
	Disable(ID_PROJECT_SETTINGS);
	Disable(ID_PROJECT_UPDATEPROJECTFROMRESOURCEFILE32825);
	Disable(ID_PROJECT_ADDRESOURCESFROMRESOURCEFILE);

	// Handle arguments
	if (__argc > 2)
	{

		//MessageBox(NULL, __argv[1], "arg 1",  MB_OK);
		//MessageBox(NULL, __argv[2], "arg 2",  MB_OK);
		//MessageBox(NULL, __argv[3], "arg 3",  MB_OK);
		//if (__argv[4])
		//	MessageBox(NULL, __argv[4], "arg 4",  MB_OK);

		ProjName = __argv[2]; 
		DirPath = __argv[1]; 
		DirPath.Replace('|', ' ');
		CString t = __argv[3];
		t.Replace('|', ' ');
		FixRes(t.GetBuffer(0));
		if (DoOpenProject() == FALSE)
		{
			if(gProjectLoadFailed)
				return FALSE;
			if(DoNewProject() == FALSE)
				return FALSE;
			pAppDoc->CloseAllDocuments(TRUE);
			ShowProjWindow();
		}
		else
		{
			//ResName = __argv[3];
			//ResPath = DirPath;
			FixRes(t.GetBuffer(0));
			pdialog = UpdateProjectFromRes();
			if (!pdialog)
				return TRUE; //FALSE;
			if (__argv[4])		// start wizard at indicated dialog
			{
				int val, id = atoi(__argv[4]);
				LPDIALOGRO pbase = pdreshead;
				while (pbase)
				{
					val = atoi(pvalues[pbase->ControlIDOrd]);
					if (val == id)
					{
						pdialog = pbase;
						break;
					}
					pbase = pbase->lpNext;
				}
			}
			if(gProjectType != PROJECT_TYPE_TE)
				GrammarWizard(pdialog);
			else
				TEGrammarWizard(pdialog);
		}
	}

  return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnHelpSemanticcodes();
	afx_msg void OnStnClickedAbout();
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
	ON_STN_CLICKED(IDC_ABOUT, &CAboutDlg::OnStnClickedAbout)
END_MESSAGE_MAP()

//void FileVersion()
//{
//    char display[1024];
//    char path[MAX_PATH + 1];
//    GetModuleFileName( NULL, path, MAX_PATH + 1 );
//
//	LPTSTR lpszFilePath = (LPTSTR)&path; 
//	DWORD dwDummy; 
//
//	DWORD dwFVISize = GetFileVersionInfoSize( lpszFilePath , &dwDummy ); 
//	LPBYTE lpVersionInfo = new BYTE[dwFVISize]; 
//	
//	GetFileVersionInfo( lpszFilePath , 0 , dwFVISize , lpVersionInfo ); 
//	
//	UINT uLen; 
//	VS_FIXEDFILEINFO *lpFfi; 
//	
//	VerQueryValue( lpVersionInfo , _T("\\") , (LPVOID *)&lpFfi , &uLen ); 
//	
//	DWORD dwFileVersionMS = lpFfi->dwFileVersionMS; 
//	DWORD dwFileVersionLS = lpFfi->dwFileVersionLS; 
//	delete [] lpVersionInfo; 
//	
//	printf( "Higher: %x\n" , dwFileVersionMS ); 
//	printf( "Lower: %x\n" , dwFileVersionLS ); 
//	
//	DWORD dwLeftMost = HIWORD(dwFileVersionMS); 
//	DWORD dwSecondLeft = LOWORD(dwFileVersionMS); 
//	DWORD dwSecondRight = HIWORD(dwFileVersionLS); 
//	DWORD dwRightMost = LOWORD(dwFileVersionLS); 
//	
//	sprintf( display, "Version: %d.%d.%d.%d\n" , dwLeftMost, dwSecondLeft, 
//	dwSecondRight, dwRightMost );
//	SetDlgItemText(IDC_ABOUT, display);
//}

BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
    char display[1024];
    char path[MAX_PATH + 1];
    GetModuleFileName( NULL, path, MAX_PATH + 1 );

	LPTSTR lpszFilePath = (LPTSTR)&path; 
	DWORD dwDummy; 

	DWORD dwFVISize = GetFileVersionInfoSize( lpszFilePath , &dwDummy ); 
	LPBYTE lpVersionInfo = new BYTE[dwFVISize]; 
	
	GetFileVersionInfo( lpszFilePath , 0 , dwFVISize , lpVersionInfo ); 
	
	UINT uLen; 
	VS_FIXEDFILEINFO *lpFfi; 
	
	VerQueryValue( lpVersionInfo , _T("\\") , (LPVOID *)&lpFfi , &uLen ); 
	
	DWORD dwFileVersionMS = lpFfi->dwFileVersionMS; 
	DWORD dwFileVersionLS = lpFfi->dwFileVersionLS; 
	delete [] lpVersionInfo; 
	
	printf( "Higher: %x\n" , dwFileVersionMS ); 
	printf( "Lower: %x\n" , dwFileVersionLS ); 
	
	DWORD dwLeftMost = HIWORD(dwFileVersionMS); 
	DWORD dwSecondLeft = LOWORD(dwFileVersionMS); 
	DWORD dwSecondRight = HIWORD(dwFileVersionLS); 
	DWORD dwRightMost = LOWORD(dwFileVersionLS); 
	
	sprintf( display, "AccuSPEECH Studio Version: %d.%d.%d.%d\n" , dwLeftMost, dwSecondLeft, 
	dwSecondRight, dwRightMost );
	SetDlgItemText(IDC_ABOUT, display);
/*
  char ver[128];
	int majorv = VERSION>>8;
	int minorv = (VERSION>>4) & 0x0f;
	int subv = VERSION & 0x0f;
	if (subv)
		sprintf(ver, "AccuSPEECH Studio Version %i.%i.%i", majorv, minorv, subv);
	else
		sprintf(ver, "AccuSPEECH Studio Version %i.%i", majorv, minorv);
	SetDlgItemText(IDC_ABOUT, ver);
	*/
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE

}

// App command to run the dialog
void CAccuStudioApp::OnAppAbout() 
{
	CAboutDlg  * aboutDlg = new CAboutDlg;
	aboutDlg->DoModal();
  delete aboutDlg;
}

/////////////////////////////////////////////////////////////////////////////
// CAccuStudioApp message handlers

/*
CDocument * ReadInFile(LPCTSTR lpfilename)
{
	char t[256];
	LPSTR p, q;
	strcpy(t, lpfilename);
	q = strstr(t, ".abnf");
	if (q)
		*q = 0;
	p = strrchr(t, '\\');
	if (!p)
		p = t;
	else
		p++;
	int i = FindGramName((LPSTR)p);
	if (q)
		*q = '.';
	if (i >= 0)
		if (pgramparent[i])
		{
			strcpy(t, pgramnames[pgramparent[i]-1]);
			strcat(t, ".abnf");
			p = t;
		}
	return pDocTemplate->OpenDocumentFile(p);
}
*/

CDocument* CAccuStudioApp::OpenDocumentFile(LPCTSTR lpszFileName) 
{
	CDocument * pt;

	pt =  CWinApp::OpenDocumentFile(lpszFileName);
	if (!pt)
		return pDocTemplate->OpenDocumentFile(lpszFileName);
	return pt;
}



void CAccuStudioApp::OnFileNew() 
{
	// TODO: Add your command handler code here
	
  pDocTemplate->OpenDocumentFile(NULL);

//  CWinApp::OnFileNew();
}


void CAccuStudioApp::OnFileOpen() 
{
  char buf[256] = "Open AccuStudio File";

	CFileDialog dlg(TRUE, "*.abnf", "*.abnf", OFN_PATHMUSTEXIST, 
    "ABNF files (*.abnf)|*.abnf|Include files (*.h)|*.h|All Files (*.*) |*.*||");
  dlg.m_ofn.lpstrTitle  = (LPSTR) & buf;
  if( dlg.DoModal() != IDOK )
    return;

//	if (theApp.OpenDocumentFile(dlg.GetFileName()) == NULL)
		pDocTemplate->OpenDocumentFile(dlg.GetPathName());
}



void CAboutDlg::OnStnClickedAbout()
{
	// TODO: Add your control notification handler code here
}
