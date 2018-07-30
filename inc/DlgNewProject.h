#if !defined(AFX_DLGNEWPROJECT_H__1FFC5F52_6811_4504_B9EB_A5D7DED9DCA2__INCLUDED_)
#define AFX_DLGNEWPROJECT_H__1FFC5F52_6811_4504_B9EB_A5D7DED9DCA2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgNewProject.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgNewProject dialog

class CDlgNewProject : public CDialog
{
// Construction
public:
	CDlgNewProject(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgNewProject)
	enum { IDD = IDD_NEW_PROJECT };
	CButton	m_res_search;
	CButton	m_locsearch;
	CString	m_location;
	CString	m_resloc;
	//}}AFX_DATA
  CString m_projname;
  CString m_projpath;
  CString m_resname;
  CString m_respath;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgNewProject)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgNewProject)
	afx_msg void OnLocsearch();
	afx_msg void OnRessearch();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGNEWPROJECT_H__1FFC5F52_6811_4504_B9EB_A5D7DED9DCA2__INCLUDED_)
