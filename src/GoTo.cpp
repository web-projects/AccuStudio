// GoTo.cpp : implementation file
//

#include "stdafx.h"
#include "AccuStudio.h"
#include "GoTo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGoTo dialog


CGoTo::CGoTo(CWnd* pParent /*=NULL*/)
	: CDialog(CGoTo::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGoTo)
	m_linenum = _T("");
	//}}AFX_DATA_INIT
}


void CGoTo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGoTo)
	DDX_Text(pDX, IDC_LINENUM, m_linenum);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGoTo, CDialog)
	//{{AFX_MSG_MAP(CGoTo)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGoTo message handlers
