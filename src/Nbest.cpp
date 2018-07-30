// Nbest.cpp : implementation file
//

#include "stdafx.h"
#include "AccuStudio.h"
#include "Nbest.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNbest dialog

extern CString SelText;


CNbest::CNbest(CWnd* pParent /*=NULL*/)
	: CDialog(CNbest::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNbest)
	m_bestcandidate = SelText;
	//}}AFX_DATA_INIT
}


void CNbest::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNbest)
	DDX_Text(pDX, IDC_NBESTCANDIDATE, m_bestcandidate);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNbest, CDialog)
	//{{AFX_MSG_MAP(CNbest)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNbest message handlers

BOOL CNbest::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
