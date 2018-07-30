// SetRCName.cpp : implementation file
//

#include "stdafx.h"
#include "AnalyzeHTML.h"
#include "SetRCName.h"


// CSetRCName dialog

IMPLEMENT_DYNAMIC(CSetRCName, CDialog)

CSetRCName::CSetRCName(CWnd* pParent /*=NULL*/)
	: CDialog(CSetRCName::IDD, pParent)
	, m_rcfilename(_T(""))
{

}

CSetRCName::~CSetRCName()
{
}

void CSetRCName::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_RCFILENAME, m_rcfilename);
}


BEGIN_MESSAGE_MAP(CSetRCName, CDialog)
END_MESSAGE_MAP()


// CSetRCName message handlers
