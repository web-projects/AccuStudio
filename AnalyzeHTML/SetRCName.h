#pragma once


// CSetRCName dialog

class CSetRCName : public CDialog
{
	DECLARE_DYNAMIC(CSetRCName)

public:
	CSetRCName(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSetRCName();

// Dialog Data
	enum { IDD = IDD_GENRC };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_rcfilename;
};
