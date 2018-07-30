#if !defined(AFX_ADDPROJFILES_H__49D0D9D2_7B3B_4800_9CB8_7C2ED89AFBC5__INCLUDED_)
#define AFX_ADDPROJFILES_H__49D0D9D2_7B3B_4800_9CB8_7C2ED89AFBC5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AddProjFiles.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAddProjFiles dialog

class CAddProjFiles : public CDialog
{
// Construction
public:
	CAddProjFiles(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAddProjFiles)
	enum { IDD = IDD_PROJECTFILES };
	int		m_GrammarSetName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAddProjFiles)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAddProjFiles)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADDPROJFILES_H__49D0D9D2_7B3B_4800_9CB8_7C2ED89AFBC5__INCLUDED_)
