// SettingsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AccuStudio.h"
#include "SettingsDlg.h"
#include "project.h"
#include "compilermodules.h"
#include "accuspeech.h"
#include "pcode.h"

extern UINT SaveVersion;
extern DWORD ProjSettings;			// Project settings
// CSettingsDlg dialog

extern Recognizer_Type Recognizer;
extern BOOL NewLogic;
extern LPSTR Model_Nickname[];
extern Target_Models Model;
extern Target_Languages Language;
extern LPSTR LangNames[];
extern int GetModelByName(LPSTR pmodel);

void CSettingsDlg::ResizeComboBox(CComboBox * pMyComboBox)
{
	//Get rectangles  
	CRect rctComboBox, rctDropDown;  
	pMyComboBox->GetClientRect(&rctComboBox); 
	
	//Combo rect  
	pMyComboBox->GetDroppedControlRect(&rctDropDown); 
	
	//DropDownList rect
	int itemHeight = pMyComboBox->GetItemHeight(-1); 
	
	//Get Item height  
	pMyComboBox->GetParent()->ScreenToClient(&rctDropDown); 
	
	int items = pMyComboBox->GetCount();

	if(items > 10)
		items = 10;

	//Converts coordinates  
	rctDropDown.bottom = rctDropDown.top + rctComboBox.Height() + itemHeight*items; 
	//Set height  
	pMyComboBox->MoveWindow(&rctDropDown); //enable changes}
}

IMPLEMENT_DYNAMIC(CSettingsDlg, CDialog)

CSettingsDlg::CSettingsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSettingsDlg::IDD, pParent)
{
}

CSettingsDlg::~CSettingsDlg()
{
}

void CSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX); 
}


BEGIN_MESSAGE_MAP(CSettingsDlg, CDialog)
//	ON_BN_CLICKED(IDC_ENABLE_ACTIVATION, CSettingsDlg::OnBnClickedEnableActivation)
//	ON_BN_CLICKED(IDC_VIAVOICE_REC, CSettingsDlg::OnBnClickedViavoiceRec)
	ON_BN_CLICKED(IDC_NUANCE_REC, CSettingsDlg::OnBnClickedNuanceRec)
	ON_CBN_SELCHANGE(IDC_MODEL, &CSettingsDlg::OnCbnSelchangeModel)
	ON_CBN_SELCHANGE(IDC_LANGUAGE, &CSettingsDlg::OnCbnSelchangeLanguage)
	ON_BN_CLICKED(IDOK, &CSettingsDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_NEWLOGIC, &CSettingsDlg::OnBnClickedNewlogic)
END_MESSAGE_MAP()


// CSettingsDlg message handlers
extern int GetLangByName(LPSTR pLang);
extern BOOL FindLanguage(Target_Languages nlang, int nmodel);
//extern LPSTR LangCodes[];

void CSettingsDlg::FillModelBox(void)
{
	int i, index = -1, found = 0;
	int tModel = -1;
	CComboBox * pc = (CComboBox *)GetDlgItem(IDC_MODEL);
	pc->ResetContent();
	for (i=0; i < Num_Model; i++)
		if (FindLanguage(Language, i))
		{
			index++;
			pc->AddString(Model_Nickname[i]);
			if (index == 0)
				tModel = i;
			if (strcmp(Model_Nickname[i], Model_Nickname[Model]) == 0)
			{
				pc->SetCurSel(index);
				found = 1;
			}
		}
	if (!found && index >= 0)
	{
		Model = (Target_Models)tModel;
		pc->SetCurSel(0);
	}
	ResizeComboBox(pc);
}

BOOL CSettingsDlg::OnInitDialog()
{
	int nmodel,ii = -1;
	CDialog::OnInitDialog();

	CheckRadioButton(IDC_NUANCE_REC, IDC_NUANCE_REC, IDC_NUANCE_REC);
	Recognizer = NUANCE3200;
	CheckDlgButton(IDC_NEWLOGIC, NewLogic);
	CComboBox * pcl = (CComboBox *)GetDlgItem(IDC_LANGUAGE);

	pcl->ResetContent();
	int i, index = -1, cs = -1;
	for (i= 0; i < Num_Lang; i++)
	{
		for (nmodel = Full_Model; nmodel < Num_Model; nmodel++)
			if( FindLanguage((Target_Languages)i, nmodel))
			{
				if (ii < 0)
					ii = i;
				index++;
				pcl->AddString(LangNames[i]);
				if (strcmp(LangNames[i],LangNames[Language]) == 0)
					cs = index;
				break;
			}
	}
	if (cs >= 0)
		pcl->SetCurSel(cs);
	else
	{
		MessageBox("Currently selected Language is not present", LangNames[Language]);
		if (ii >= 0)
		{
			Language = (Target_Languages)ii;
			pcl->SetCurSel(0);
		}
	}
	ResizeComboBox(pcl);
	FillModelBox();
	int majorv = SaveVersion>>8;
	int minorv = (SaveVersion>>4) & 0x0f;
	int subv = SaveVersion & 0x0f;
	SetDlgItemInt(IDC_MAJOR, majorv);
	SetDlgItemInt(IDC_MINOR, minorv);
	SetDlgItemInt(IDC_SUB, subv);

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CSettingsDlg::OnBnClickedNuanceRec()
{
	Recognizer = NUANCE3200;
}

void CSettingsDlg::OnBnClickedOk()
{
	UINT majorv = GetDlgItemInt(IDC_MAJOR);
	UINT minorv = GetDlgItemInt(IDC_MINOR);
	UINT sub = GetDlgItemInt(IDC_SUB);
	UINT ver = majorv<<8 | minorv<<4 | sub;
	BOOL OldNewLogic;

	if (ver != SaveVersion)
	{
		PModified = TRUE;
		SaveVersion = ver;
	}
	OldNewLogic = NewLogic;
	if (IsDlgButtonChecked(IDC_NEWLOGIC) == BST_CHECKED)
		NewLogic = TRUE;
	else
		NewLogic = FALSE;
	if (OldNewLogic != NewLogic)
		PModified = TRUE;
	OnOK();
}

void CSettingsDlg::OnCbnSelchangeModel()
{
	char data[256];
	CComboBox * pc = (CComboBox *)GetDlgItem(IDC_MODEL);
	int cs = pc->GetCurSel();
	pc->GetLBText(cs, data);
	cs = GetModelByName(data);
	if (cs >= 0)
	{
		Model = (Target_Models) cs;
		PModified = TRUE;
		return;
	}
	MessageBeep(MB_ICONEXCLAMATION);
}

void CSettingsDlg::OnCbnSelchangeLanguage()
{
	char data[256];
	CComboBox * pc = (CComboBox *)GetDlgItem(IDC_LANGUAGE);
	int cs = pc->GetCurSel();
	pc->GetLBText(cs, data);
	cs = GetLangByName(data);
	if (cs >= 0)
	{
		Language = (Target_Languages) cs;
		PModified = TRUE;
		FillModelBox();
		return;
	}
	MessageBeep(MB_ICONEXCLAMATION);
}


void CSettingsDlg::OnBnClickedNewlogic()
{
	// TODO: Add your control notification handler code here
}
