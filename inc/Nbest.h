#if !defined(AFX_NBEST_H__87230239_33D1_4BD0_8127_8E4B336AEB15__INCLUDED_)
#define AFX_NBEST_H__87230239_33D1_4BD0_8127_8E4B336AEB15__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Nbest.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNbest dialog

class CNbest : public CDialog
{
// Construction
public:
	CNbest(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CNbest)
	enum { IDD = IDD_SHOWNBEST };
	CString	m_bestcandidate;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNbest)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNbest)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NBEST_H__87230239_33D1_4BD0_8127_8E4B336AEB15__INCLUDED_)
