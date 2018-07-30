// Find.cpp : implementation file
//

#include "stdafx.h"
#include "AccuStudio.h"
#include "Find.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CString match;
BOOL matchcase = FALSE;
BOOL wholeword = FALSE;

/////////////////////////////////////////////////////////////////////////////
// CFind dialog


CFind::CFind(CWnd* pParent /*=NULL*/)
	: CDialog(CFind::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFind)
	m_findstr = match;
	m_matchcase = matchcase;
	m_wholeword = wholeword;
	//}}AFX_DATA_INIT
}


void CFind::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFind)
	DDX_Text(pDX, IDC_FINDSTR, m_findstr);
  match = m_findstr;
	DDX_Check(pDX, IDC_MATCHCASE, m_matchcase);
	matchcase = m_matchcase;
	DDX_Check(pDX, IDC_WHOLEWORD, m_wholeword);
	wholeword = m_wholeword;
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFind, CDialog)
	//{{AFX_MSG_MAP(CFind)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFind message handlers
