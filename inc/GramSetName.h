#if !defined(AFX_GRAMSETNAME_H__1BF8B197_1030_41EC_9318_1141FEC253FA__INCLUDED_)
#define AFX_GRAMSETNAME_H__1BF8B197_1030_41EC_9318_1141FEC253FA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GramSetName.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGramSetName dialog

class CGramSetName : public CDialog
{
// Construction
public:
	CGramSetName(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGramSetName)
	enum { IDD = IDD_GRAMSETNAME };
	CString	m_name;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGramSetName)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGramSetName)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GRAMSETNAME_H__1BF8B197_1030_41EC_9318_1141FEC253FA__INCLUDED_)
