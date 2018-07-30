#pragma once
#include "afxwin.h"


// CSettingsDlg dialog

class CSettingsDlg : public CDialog
{
	DECLARE_DYNAMIC(CSettingsDlg)

public:
	CSettingsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSettingsDlg();

// Dialog Data
	enum { IDD = SETTINGS_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	void ResizeComboBox(CComboBox * pMyComboBox);
	BOOL m_Activate_Settings;
	virtual BOOL OnInitDialog();
	void FillModelBox(void);
public:
	afx_msg void OnBnClickedNuanceRec();
	afx_msg void OnCbnSelchangeModel();
	afx_msg void OnCbnSelchangeLanguage();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedNewlogic();
};
