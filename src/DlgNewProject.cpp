// DlgNewProject.cpp : implementation file
//

#include "stdafx.h"
#include "AccuStudio.h"
#include "DlgNewProject.h"
#include "common.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgNewProject dialog


CDlgNewProject::CDlgNewProject(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgNewProject::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgNewProject)
	m_location = _T("");
	m_resloc = _T("");
	//}}AFX_DATA_INIT
  m_projname = _T("");
  m_resname= _T("");
}


void CDlgNewProject::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgNewProject)
	DDX_Control(pDX, IDC_RESSEARCH, m_res_search);
	DDX_Control(pDX, IDC_LOCSEARCH, m_locsearch);
	DDX_Text(pDX, IDC_PROJ_LOCATION, m_location);
	DDX_Text(pDX, IDC_PROJ_RESLOC, m_resloc);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgNewProject, CDialog)
	//{{AFX_MSG_MAP(CDlgNewProject)
	ON_BN_CLICKED(IDC_LOCSEARCH, OnLocsearch)
	ON_BN_CLICKED(IDC_RESSEARCH, OnRessearch)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgNewProject message handlers

void CDlgNewProject::OnLocsearch() 
{
  char buf[256] = "Project Directory";

	// Put up a standard file dialog to allow output file selection

	CFileDialog dlg(FALSE, ".vprj", "untitled", OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT, 
    "Project files (*.vprj)|*.vprj||");
  dlg.m_ofn.lpstrTitle  = (LPSTR) & buf;
	if( dlg.DoModal() == IDOK )
	{
    char fullname[256], filename[256], ext[32];
    char * p;
    
    strcpy(fullname, dlg.GetPathName());
    strcpy(filename, dlg.GetFileName());
    strcpy(ext, dlg.GetFileExt());
    if (stricmp(ext, "vprj"))
      strcat(filename, "vprj");
    p = strstr(fullname, filename);
    if (p)
      *p = 0;
		m_projpath = fullname;

		/*
		if (strchr(fullname, ' '))
		{
			MessageBox(
		"The file path to the project cannot have spaces.", "Path Error", MB_OK);
			goto restart;
		}
		*/
    m_projname = dlg.GetFileTitle();

    CString tt = m_projpath + filename;
    SetDlgItemText(IDC_PROJ_LOCATION, tt);
	}	  	
}

void CDlgNewProject::OnRessearch() 
{
	// Put up a standard file dialog to allow output file selection
	CFileDialog dlg(TRUE, ".rc", NULL, OFN_PATHMUSTEXIST | OFN_CREATEPROMPT, 
    "Resource files (*.rc)|*.rc| XML files (*.xml)|*.xml||");
	if( dlg.DoModal() == IDOK )
	{
    char fullname[256], filename[256];
    char * p;
		m_respath = dlg.GetPathName();
    m_resname = dlg.GetFileName();
    
    strcpy(fullname, m_respath);
    strcpy(filename, m_resname);
    p = strstr(fullname, filename);
    if (p)
      *p = 0;
    SetDlgItemText(IDC_PROJ_RESLOC, m_respath);
    m_respath = fullname;
	}	
}


void CDlgNewProject::OnOK() 
{
  if (m_projpath.GetLength() == 0)
  {
    MessageBox("Please Enter a Project Name and Path","New Project");
    return;
  }
  
  if (m_respath.GetLength() == 0)
  {
//    MessageBox("Please Enter a Resource File Name", "New Project");
//    return;
  }

	CDialog::OnOK();
}
