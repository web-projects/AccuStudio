#if !defined(AFX_FIND_H__6406123E_9E5D_4697_96DC_3B19CE9DD526__INCLUDED_)
#define AFX_FIND_H__6406123E_9E5D_4697_96DC_3B19CE9DD526__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Find.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFind dialog

class CFind : public CDialog
{
// Construction
public:
	CFind(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFind)
	enum { IDD = IDD_FINDDLG };
	CString	m_findstr;
	BOOL	m_matchcase;
	BOOL	m_wholeword;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFind)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFind)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FIND_H__6406123E_9E5D_4697_96DC_3B19CE9DD526__INCLUDED_)
