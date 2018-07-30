#pragma once


// CFindURL dialog

class CFindURL : public CDialog
{
	DECLARE_DYNAMIC(CFindURL)

public:
	CFindURL(CWnd* pParent = NULL);   // standard constructor
	virtual ~CFindURL();

// Dialog Data
	enum { IDD = IDD_GETURL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_urlname;
	afx_msg void OnCbnSelchangeUrllist();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnBnClickedOk();
};
