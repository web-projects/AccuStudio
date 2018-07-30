#pragma once
#include "afxwin.h"


// CPreRec dialog

class CPreRec : public CDialog
{
	DECLARE_DYNAMIC(CPreRec)

public:
	CPreRec(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPreRec();

// Dialog Data
	enum { IDD = IDD_PREREC };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	BOOL m_vp;
public:
	BOOL m_vc;
	BOOL m_vw;
public:
	CString m_actions;
public:
	afx_msg void OnBnClickedWavecapture();
public:
	afx_msg void OnBnClickedVoiceprompt();
public:
	afx_msg void OnBnClickedPlaywav();
	CButton m_variable;
	BOOL m_dynamic;
};
#pragma once


// CSelTemplate dialog

class CSelTemplate : public CDialog
{
	DECLARE_DYNAMIC(CSelTemplate)

public:
	CSelTemplate(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSelTemplate();
	virtual BOOL OnInitDialog();

// Dialog Data
	enum { IDD = IDD_SELTEMPLATE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnCbnSelchangeSeltemplate();
	afx_msg void OnBnClickedOk();
};
