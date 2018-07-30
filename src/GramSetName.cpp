// GramSetName.cpp : implementation file
//

#include "stdafx.h"
#include "AccuStudio.h"
#include "GramSetName.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGramSetName dialog


CGramSetName::CGramSetName(CWnd* pParent /*=NULL*/)
	: CDialog(CGramSetName::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGramSetName)
	m_name = _T("");
	//}}AFX_DATA_INIT
}


void CGramSetName::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGramSetName)
	DDX_Text(pDX, IDC_NAME, m_name);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGramSetName, CDialog)
	//{{AFX_MSG_MAP(CGramSetName)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGramSetName message handlers
