// AddProjFiles.cpp : implementation file
//

#include "stdafx.h"
#include "AccuStudio.h"
#include "AddProjFiles.h"
#include "project.h"
#include "common.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern int FindGramName(LPSTR pname);

/////////////////////////////////////////////////////////////////////////////
// CAddProjFiles dialog


CAddProjFiles::CAddProjFiles(CWnd* pParent /*=NULL*/)
	: CDialog(CAddProjFiles::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAddProjFiles)
	m_GrammarSetName = -1;
	//}}AFX_DATA_INIT
}


void CAddProjFiles::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAddProjFiles)
	DDX_CBIndex(pDX, IDC_GRAMMARSETLIST, m_GrammarSetName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAddProjFiles, CDialog)
	//{{AFX_MSG_MAP(CAddProjFiles)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAddProjFiles message handlers

BOOL CAddProjFiles::OnInitDialog() 
{
	int i;
  LPGRAMSETTHREAD tg = lpGramSetThread;
  CString path = DirPath;
  CComboBox * cc = (CComboBox *)GetDlgItem(IDC_GRAMMARSETLIST);
  CListBox * cf = (CListBox *)GetDlgItem(IDC_FILELIST);
	LPSTR p;

  CDialog::OnInitDialog();

  while (tg)
  {
    cc->AddString(pgramsetnames[tg->ID]);
    tg = tg->next;
  }
  
  path += "*.abnf";

  cf->Dir(DDL_READWRITE, path);
	for (i=0; i < cf->GetCount(); i++)
	{
		cf->GetText(i, path);
		p = strstr(path.GetBuffer(0), ".abnf");
		if (p)
		{
			*p = 0;
			if (FindGramName(path.GetBuffer(0)) >= 0)
			{
				cf->DeleteString(i);
				i--;
			}
		}
	}

  cc->SetCurSel(0);
  cc->SetTopIndex(0);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAddProjFiles::OnOK() 
{
  CListBox * cf = (CListBox *)GetDlgItem(IDC_FILELIST);
  CString name;
  char temp[256];

  int ac, j, i = cf->GetSelCount();

  LPINT p = (LPINT)calloc(i*sizeof(int), 1);

  GetDlgItemText(IDC_GRAMMARSETLIST, name);
  j = GetGramSetIndex(name.GetBuffer(0));

  ac = cf->GetSelItems(i, p);

  for (i=0; i < ac; i++)
  {
    cf->GetText(p[i], name);
    strcpy(temp, name);
    temp[strlen(temp)-5] = 0;
    AddGramName(j, temp);
  }
	if (i)
	{
		BuildProject(TRUE, DirPath, ProjName);
		//MessageBox("Run the 'Build Project' command to compile your Grammars.");
		SaveProject();
	}
  ShowProjWindow();

  free(p);
  
  CDialog::OnOK();
}
