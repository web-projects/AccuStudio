// FindURL.cpp : implementation file
//

#include "stdafx.h"
#include "AnalyzeHTML.h"
#include "FindURL.h"


// CFindURL dialog

IMPLEMENT_DYNAMIC(CFindURL, CDialog)

BOOL active = FALSE;

CFindURL::CFindURL(CWnd* pParent /*=NULL*/)
	: CDialog(CFindURL::IDD, pParent)
	, m_urlname(_T(""))
{
	active = FALSE;
}

CFindURL::~CFindURL()
{
}

void CFindURL::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CFindURL, CDialog)
	ON_CBN_SELCHANGE(IDC_URLLIST, &CFindURL::OnCbnSelchangeUrllist)
	ON_WM_ACTIVATE()
	ON_BN_CLICKED(IDOK, &CFindURL::OnBnClickedOk)
END_MESSAGE_MAP()


// CFindURL message handlers

void CFindURL::OnCbnSelchangeUrllist()
{
	// TODO: Add your control notification handler code here
}

void CFindURL::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	char ts[1024];
	CDialog::OnActivate(nState, pWndOther, bMinimized);
	if (active++)
		return;
	CComboBox * thecb;
	thecb = (CComboBox *)GetDlgItem(IDC_URLLIST);
	if (!thecb)
		return;
	thecb->SetFocus();
	FILE * pf;
	pf = fopen("urllist", "r");
	if (!pf)
		return;
	while(1)
	{
		if (fgets(ts, sizeof(ts),pf) == NULL)
			break;
		ts[strlen(ts)-1] = 0;
		thecb->AddString(ts);
	}
	fclose(pf);
	thecb->SetCurSel(0);
	thecb->SetTopIndex(0);
}

void CFindURL::OnBnClickedOk()
{
	OnOK();
	CComboBox * thecb;
	CString ts, tt;
	thecb = (CComboBox *)GetDlgItem(IDC_URLLIST);
	if (!thecb)
		return;
	FILE * pf;
	pf = fopen("urllist", "w");
	if (!pf)
		return;
	GetDlgItemText(IDC_URLLIST,ts);
	if (ts.GetLength())
	{
		m_urlname = ts;
		ts += (CString)"\n";
		fwrite(ts, 1, ts.GetLength(), pf);
	}
	for(int i=0; i < thecb->GetCount(); i++)
	{
		thecb->GetLBText(i, tt);
		if (tt.GetLength() == 0)
			break;
		tt += (CString)"\n";
		if(tt != ts)
			fwrite(tt, 1, tt.GetLength(), pf);
	}
	fclose(pf);
}
