// Settings.cpp : implementation file
//

#include "stdafx.h"
#include "AccuStudio.h"
#include "Settings.h"
#include "project.h"

extern LPDIALOGRO pdres;       // running dialog res ptr

// CPreRec dialog

IMPLEMENT_DYNAMIC(CPreRec, CDialog)

CPreRec::CPreRec(CWnd* pParent /*=NULL*/)
	: CDialog(CPreRec::IDD, pParent)
	, m_vp(FALSE)
	, m_vc(FALSE)
	, m_vw(FALSE)
	, m_actions(_T(""))
	, m_dynamic(FALSE)
{

}

CPreRec::~CPreRec()
{
}

void CPreRec::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_VOICEPROMPT, m_vp);
	DDX_Check(pDX, IDC_WAVECAPTURE, m_vc);
	DDX_Check(pDX, IDC_PLAYWAV, m_vw);
	DDX_Text(pDX, IDC_ACTIONS, m_actions);
	DDX_Control(pDX, IDC_DYNAMIC, m_variable);
	DDX_Check(pDX, IDC_DYNAMIC, m_dynamic);
}


BEGIN_MESSAGE_MAP(CPreRec, CDialog)
	ON_BN_CLICKED(IDC_WAVECAPTURE, CPreRec::OnBnClickedWavecapture)
	ON_BN_CLICKED(IDC_VOICEPROMPT, CPreRec::OnBnClickedVoiceprompt)
	ON_BN_CLICKED(IDC_PLAYWAV, CPreRec::OnBnClickedPlaywav)
END_MESSAGE_MAP()


// CPreRec message handlers

void CPreRec::OnBnClickedWavecapture()
{
	int pos;
	UINT state = IsDlgButtonChecked(IDC_WAVECAPTURE);
	GetDlgItemText(IDC_ACTIONS,m_actions);
	int len = m_actions.GetLength();
	pos = m_actions.Find("WAVECAPTURE");
	if (pos < 0 && state == BST_CHECKED)
	{ // not present
		if (len)
			m_actions += '+';
		CString ts = pdefines[pdres->ControlIDOrd];
		m_actions += "WAVECAPTURE(" + ts + (CString)")";
	}
	else if (pos >= 0 && state != BST_CHECKED) 
	{ // Is present
		if (pos && m_actions.GetAt(pos-1) == '+')
			pos--;		// should be a + sign
		int last = m_actions.Find(')',pos);
		if (last > pos)
			m_actions = m_actions.Left(pos) + m_actions.Right(len-last-1);
	}
	SetDlgItemText(IDC_ACTIONS, m_actions);
}

void CPreRec::OnBnClickedVoiceprompt()
{
	int pos;
	UINT state = IsDlgButtonChecked(IDC_VOICEPROMPT);
	GetDlgItemText(IDC_ACTIONS,m_actions);
	int len = m_actions.GetLength();
	pos = m_actions.Find("SAYSTRING");
	if (pos < 0 && state == BST_CHECKED)
	{ // not present
		if (len)
			m_actions = "SAYSTRING(\"___\")+" + m_actions;
		else
			m_actions = "SAYSTRING(\"___\")" + m_actions;
	}
	else if (pos == 0 && state != BST_CHECKED) 
	{ // Is present
		int last = m_actions.Find(')',pos);
		if (last > pos)
		{
			if (m_actions.GetAt(last+1) == '+')
				last++;
			m_actions = m_actions.Right(len-last-1);
		}
	}
	SetDlgItemText(IDC_ACTIONS, m_actions);
}

void CPreRec::OnBnClickedPlaywav()
{
	int pos;
	UINT state = IsDlgButtonChecked(IDC_PLAYWAV);
	GetDlgItemText(IDC_ACTIONS,m_actions);
	int len = m_actions.GetLength();
	pos = m_actions.Find("PLAYWAVE");
	if (pos < 0 && state == BST_CHECKED)
	{ // not present
		if (len)
			m_actions += '+';
		m_actions += "PLAYWAVE(\"___\")";
	}
	else if (pos >= 0 && state != BST_CHECKED) 
	{ // Is present
		if (pos && m_actions.GetAt(pos-1) == '+')
			pos--;		// should be a + sign
		int last = m_actions.Find(')',pos);
		if (last > pos)
			m_actions = m_actions.Left(pos) + m_actions.Right(len-last-1);
	}
	SetDlgItemText(IDC_ACTIONS, m_actions);
}
// C:\AccuSpeechProjects\Products\AccuStudio\Settings.cpp : implementation file
//

#include "stdafx.h"
#include "AccuStudio.h"
#include "Settings.h"


// CSelTemplate dialog

extern int		templateID;

IMPLEMENT_DYNAMIC(CSelTemplate, CDialog)

CSelTemplate::CSelTemplate(CWnd* pParent /*=NULL*/)
	: CDialog(CSelTemplate::IDD, pParent)
{

}

BOOL CSelTemplate::OnInitDialog()
{
	CComboBox * pc = (CComboBox *)GetDlgItem(IDC_SELTEMPLATE);
	pc->Clear();
	pc->AddString("None");
	pc->AddString("Select an option by number");
	pc->AddString("Select a variable option by number");
	pc->AddString("Select an option by line number");
	pc->SetCurSel(templateID);
	templateID = 0;
	return TRUE;
}

CSelTemplate::~CSelTemplate()
{
}

void CSelTemplate::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSelTemplate, CDialog)
	ON_CBN_SELCHANGE(IDC_SELTEMPLATE, &CSelTemplate::OnCbnSelchangeSeltemplate)
	ON_BN_CLICKED(IDOK, &CSelTemplate::OnBnClickedOk)
END_MESSAGE_MAP()


// CSelTemplate message handlers

void CSelTemplate::OnCbnSelchangeSeltemplate()
{
	// TODO: Add your control notification handler code here
}

void CSelTemplate::OnBnClickedOk()
{
	CComboBox * pc = (CComboBox *)GetDlgItem(IDC_SELTEMPLATE);
	templateID = pc->GetCurSel();
	OnOK();
}
