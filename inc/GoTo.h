#if !defined(AFX_GOTO_H__0845322E_D444_4FA0_9383_E8E9580DE105__INCLUDED_)
#define AFX_GOTO_H__0845322E_D444_4FA0_9383_E8E9580DE105__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GoTo.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGoTo dialog

class CGoTo : public CDialog
{
// Construction
public:
	CGoTo(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGoTo)
	enum { IDD = IDD_GOTO };
	CString	m_linenum;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGoTo)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGoTo)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GOTO_H__0845322E_D444_4FA0_9383_E8E9580DE105__INCLUDED_)
