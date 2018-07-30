// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "AccuStudio.h"

#include "MainFrm.h"
#include "project.h"
#include "AddProjFiles.h"
#include "gramsetname.h"
#include "SettingsDlg.h"
#include "compilermodules.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


extern HTREEITEM root, gramroot, resroot;
extern CTreeCtrl * pct;

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_FILE_CLOSE_PROJECT, OnFileCloseProject)
	ON_COMMAND(ID_FILE_NEW_PROJECT, OnFileNewProject)
	ON_COMMAND(ID_FILE_OPEN_PROJECT, OnFileOpenProject)
	ON_COMMAND(ID_VIEW_PROJECT, OnViewProject)
	ON_COMMAND(ID_FILE_SAVE_PROJECT, OnFileSaveProject)
	ON_COMMAND(ID_PROJECT_ADDGRAMMARFILES, OnProjectAddgrammarfiles)
	ON_COMMAND(ID_PROJECT_ADD_GRAMMARSET, OnProjectAddGrammarset)
	ON_COMMAND(ID_BUILD_BUILD_PROJECT, OnBuildBuildProject)
	ON_COMMAND(ID_BUILD_BUILDFROMDEFINITIONFILE, OnBuildBuildfromdefinitionfile)
	ON_WM_INITMENUPOPUP()
	ON_UPDATE_COMMAND_UI(ID_BUILD_BUILD_PROJECT, OnUpdateBuildBuildProject)
	ON_UPDATE_COMMAND_UI(ID_BUILD_BUILDFROMDEFINITIONFILE, OnUpdateBuildBuildfromdefinitionfile)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PROJECT, OnUpdateViewProject)
	ON_UPDATE_COMMAND_UI(ID_FILE_CLOSE_PROJECT, OnUpdateFileCloseProject)
	ON_UPDATE_COMMAND_UI(ID_FILE_NEW_PROJECT, OnUpdateFileNewProject)
	ON_UPDATE_COMMAND_UI(ID_FILE_OPEN_PROJECT, OnUpdateFileOpenProject)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_PROJECT, OnUpdateFileSaveProject)
	ON_COMMAND(ID_VIEW_GRAMMAR_WIZARD, OnViewGrammarWizard)
	ON_UPDATE_COMMAND_UI(ID_VIEW_GRAMMAR_WIZARD, OnUpdateViewGrammarWizard)
	ON_COMMAND(ID_VIEW_RESULTS, OnViewResults)
	ON_UPDATE_COMMAND_UI(ID_VIEW_RESULTS, OnUpdateViewResults)
	ON_UPDATE_COMMAND_UI(ID_PROJECT_ADD_GRAMMARSET, OnUpdateProjectAddGrammarset)
	ON_UPDATE_COMMAND_UI(ID_PROJECT_ADDGRAMMARFILES, OnUpdateProjectAddgrammarfiles)
	ON_COMMAND(ID_VIEW_OBJECTWIZARD, OnViewObjectwizard)
	ON_UPDATE_COMMAND_UI(ID_VIEW_OBJECTWIZARD, OnUpdateViewObjectwizard)
	ON_COMMAND(ID_GRAMMAR_EXERCISE, OnGrammarExercise)
	ON_UPDATE_COMMAND_UI(ID_GRAMMAR_EXERCISE, OnUpdateGrammarExercise)
	ON_COMMAND(ID_TOOLS_ANALYSEHTML, OnToolsAnalysehtml)
	//ON_COMMAND(ID_GRAMMAR_BUILDLIBRARY, OnGrammarBuildlibrary)
	//}}AFX_MSG_MAP
//	ON_COMMAND(ID_FILE_SAVEAS, &CMainFrame::OnFileSaveas)
ON_COMMAND(ID_PROJECT_SETTINGS, CMainFrame::OnProjectSettings)
ON_UPDATE_COMMAND_UI(ID_PROJECT_SETTINGS, CMainFrame::OnUpdateProjectSettings)
ON_COMMAND(ID_PROJECT_UPDATEPROJECTFROMRESOURCEFILE32825, CMainFrame::OnProjectUpdateprojectfromresourcefile32825)
ON_UPDATE_COMMAND_UI(ID_PROJECT_UPDATEPROJECTFROMRESOURCEFILE32825, CMainFrame::OnUpdateProjectUpdateprojectfromresourcefile32825)
ON_COMMAND(ID_PROJECT_ADDRESOURCESFROMRESOURCEFILE, CMainFrame::OnProjectAddresourcesfromresourcefile)
ON_UPDATE_COMMAND_UI(ID_PROJECT_ADDRESOURCESFROMRESOURCEFILE, CMainFrame::OnUpdateProjectAddresourcesfromresourcefile)
ON_COMMAND(ID_HELP_SEMANTICCODES, &CMainFrame::OnHelpSemanticcodes)
END_MESSAGE_MAP()

extern CMainFrame* pMainFrame;

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
//	ID_INDICATOR_CAPS,
//	ID_INDICATOR_NUM,
//	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
}

CMainFrame::~CMainFrame()
{
}

CStatusBar * CMainFrame::GetStatusBar()
{
   return  & m_wndStatusBar;
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

  Disable(ID_VIEW_PROJECT);
  Disable(ID_VIEW_GRAMMAR_WIZARD);
  Disable(ID_VIEW_OBJECTWIZARD);
  Disable(ID_GRAMMAR_EXERCISE);
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers



void CMainFrame::OnFileCloseProject() 
{
  CloseProject();
}

void CMainFrame::OnFileNewProject() 
{
  NewProject();
  if (ProjName.GetLength())
    pMainFrame->SetWindowText("AccuSPEECH Studio - " + ProjName);
}

void CMainFrame::OnFileOpenProject() 
{
  OpenProject();
  if (ProjName.GetLength())
    pMainFrame->SetWindowText("AccuSPEECH Studio - " + ProjName);
}


void CMainFrame::OnViewProject()   
{
  ShowProjWindow();	
}

void CMainFrame::OnFileSaveProject() 
{
  SaveProject();
}

void CMainFrame::OnProjectAddgrammarfiles() 
{
  AddGrammarFiles();
}

void CMainFrame::OnProjectAddGrammarset() 
{
  AddGrammarset();
}



void CMainFrame::OnBuildBuildProject() 
{
  BuildProject(TRUE, DirPath, ProjName);	
}

void CMainFrame::OnBuildBuildfromdefinitionfile() 
{
  BuildFromDef();	
}

void CMainFrame::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu) 
{
	CMDIFrameWnd::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);	
}

void CMainFrame::OnUpdateBuildBuildProject(CCmdUI* pCmdUI) 
{
  if (IsDisabled(pCmdUI->m_nID))
    pCmdUI->Enable(FALSE);
  else
    pCmdUI->Enable(TRUE);
}

void CMainFrame::OnUpdateBuildBuildfromdefinitionfile(CCmdUI* pCmdUI) 
{
//  if (IsDisabled(pCmdUI->m_nID))
    pCmdUI->Enable(FALSE);
//  else
//    pCmdUI->Enable(TRUE);
}

void CMainFrame::OnUpdateViewProject(CCmdUI* pCmdUI) 
{
  if (IsDisabled(pCmdUI->m_nID))
    pCmdUI->Enable(FALSE);
  else
    pCmdUI->Enable(TRUE);
}

void CMainFrame::OnUpdateFileCloseProject(CCmdUI* pCmdUI) 
{
  if (IsDisabled(pCmdUI->m_nID))
    pCmdUI->Enable(FALSE);
  else
    pCmdUI->Enable(TRUE);
}

void CMainFrame::OnUpdateFileNewProject(CCmdUI* pCmdUI) 
{
  if (IsDisabled(pCmdUI->m_nID))
    pCmdUI->Enable(FALSE);
  else
    pCmdUI->Enable(TRUE);
}

void CMainFrame::OnUpdateFileOpenProject(CCmdUI* pCmdUI) 
{
  if (IsDisabled(pCmdUI->m_nID))
    pCmdUI->Enable(FALSE);
  else
    pCmdUI->Enable(TRUE);
}

void CMainFrame::OnUpdateFileSaveProject(CCmdUI* pCmdUI) 
{
  if (IsDisabled(pCmdUI->m_nID))
    pCmdUI->Enable(FALSE);
  else
    pCmdUI->Enable(TRUE);
}

void CMainFrame::OnViewGrammarWizard() 
{
  GrammarWizard(NULL);
}

void CMainFrame::OnUpdateViewGrammarWizard(CCmdUI* pCmdUI) 
{
  if (IsDisabled(pCmdUI->m_nID))
    pCmdUI->Enable(FALSE);
  else
    pCmdUI->Enable(TRUE);
}

void CMainFrame::OnViewResults() 
{
  ViewResults();
}

void CMainFrame::OnUpdateViewResults(CCmdUI* pCmdUI) 
{
  if (IsDisabled(pCmdUI->m_nID))
    pCmdUI->Enable(FALSE);
  else
    pCmdUI->Enable(TRUE);	
}


void CMainFrame::OnUpdateProjectAddGrammarset(CCmdUI* pCmdUI) 
{
//  if (IsDisabled(pCmdUI->m_nID))
    pCmdUI->Enable(FALSE);
//  else
 //   pCmdUI->Enable(TRUE);
}

void CMainFrame::OnUpdateProjectAddgrammarfiles(CCmdUI* pCmdUI) 
{
  if (IsDisabled(pCmdUI->m_nID))
    pCmdUI->Enable(FALSE);
  else
    pCmdUI->Enable(TRUE);
}


void CMainFrame::OnViewObjectwizard() 
{
  ObjectWizard(NULL);
}

void CMainFrame::OnUpdateViewObjectwizard(CCmdUI* pCmdUI) 
{
  if (IsDisabled(pCmdUI->m_nID))
    pCmdUI->Enable(FALSE);
  else
    pCmdUI->Enable(TRUE);
}

void CMainFrame::OnGrammarExercise() 
{
  GrammarExerciser(NULL);
}

void CMainFrame::OnUpdateGrammarExercise(CCmdUI* pCmdUI) 
{
  if (IsDisabled(pCmdUI->m_nID))
    pCmdUI->Enable(FALSE);
  else
    pCmdUI->Enable(TRUE);
}

//void CMainFrame::OnFileSaveas()
//{
//	// TODO: Add your command handler code here
//}

extern BOOL Launch(LPSTR lpName, BOOL ShareConsole);

void CMainFrame::OnToolsAnalysehtml()  
{
	Launch("AnalyzeHTML.exe", TRUE);
}

void CMainFrame::OnGrammarBuildlibrary() 
{	
	BuildLib();
}

extern void CreateNode(LPSTR sv, LPDIALOGRO pdres);
extern void CreateInclude();

void AddDefaultGrammars(int ptype)
{
	CString spath, tpath;
	int j, val;
	extern CString ExecDir;
	if (ptype == PROJECT_TYPE_TE)
	{
		LPDIALOGRO pbase = pdreshead; 
		tpath = DirPath + (CString) "AWL_Activation_Grm.abnf";
		spath = ExecDir + (CString)"RuleLib\\TELib\\AWL_Activation_Grm.abnf";
		j = CopyFile(spath,tpath,FALSE);
		AddGramName(0, "AWL_Activation_Grm");
		tpath = DirPath + (CString) "DID_0_Grm.abnf";
		spath = ExecDir + (CString)"RuleLib\\TELib\\DID_0_Grm.abnf";
		j = CopyFile(spath,tpath,FALSE);
		int gi = AddGramName(0, "DID_0_Grm");
		tpath = DirPath + (CString) "vkeys.h";
		spath = ExecDir + (CString)"RuleLib\\TELib\\vkeys.h";
		j = CopyFile(spath,tpath,FALSE);
		while (pbase)
		{
			val = atoi(pvalues[pbase->ControlIDOrd]);
			if (!val)
			{
				LinkGrammar(pbase, 0, gi);
				break;
			}
			pbase = pbase->lpNext;
		}
		if (!pbase)
		{ // No screen 0 dialog - create one
			char sv[] = "0";			
			pbase = (LPDIALOGRO) calloc(sizeof(DIALOGRO), 1);
			CreateNode(sv, pbase);
			LinkGrammar(pbase, 0, gi);
			CreateInclude();
		}
	}
	else
	{
		tpath = DirPath + (CString) "AWL_Activation_Grm.abnf";
		spath = ExecDir + (CString)"RuleLib\\AWL_Activation_Grm.abnf";
		j = CopyFile(spath,tpath,FALSE);
		AddGramName(0, "AWL_Activation_Grm");
		tpath = DirPath + (CString) "MessageBox_Grm.abnf";
		spath = ExecDir + (CString)"RuleLib\\MessageBox_Grm.abnf";
		j = CopyFile(spath,tpath,FALSE);
		tpath = DirPath + (CString) "vkeys.h";
		spath = ExecDir + (CString)"RuleLib\\TELib\\vkeys.h";
		j = CopyFile(spath,tpath,FALSE);
	}
	PModified = TRUE;
}

void CMainFrame::OnProjectSettings()
{
//	int i;
	Recognizer_Type currec = Recognizer;

	CSettingsDlg * psettings = new CSettingsDlg;
	psettings->DoModal();
	if (currec != Recognizer)
	{
		ProjSettings &= ~PROJ_REC_MSK;
		ProjSettings |= (Recognizer << 1);
		PModified = TRUE;
	}
	delete psettings;
	if (PModified)
		SaveProject();
}

void CMainFrame::OnUpdateProjectSettings(CCmdUI *pCmdUI)
{
  if (IsDisabled(pCmdUI->m_nID))
    pCmdUI->Enable(FALSE);
  else
    pCmdUI->Enable(TRUE);	
}

void CMainFrame::OnProjectUpdateprojectfromresourcefile32825()
{
	UpdateProjectFromRes();
}

void CMainFrame::OnUpdateProjectUpdateprojectfromresourcefile32825(CCmdUI *pCmdUI)
{
  if (IsDisabled(pCmdUI->m_nID))
    pCmdUI->Enable(FALSE);
  else
    pCmdUI->Enable(TRUE);	
}

void CMainFrame::OnProjectAddresourcesfromresourcefile()
{
	AddResToProject();
}

void CMainFrame::OnUpdateProjectAddresourcesfromresourcefile(CCmdUI *pCmdUI)
{
  if (IsDisabled(pCmdUI->m_nID))
    pCmdUI->Enable(FALSE);
  else
    pCmdUI->Enable(TRUE);	
}

extern void DisplayOpcodes();
void CMainFrame::OnHelpSemanticcodes()
{
	DisplayOpcodes();
}
