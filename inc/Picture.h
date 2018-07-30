#if !defined(AFX_PICTURE_H__26DCD704_4146_4EB7_9570_419030FFBA06__INCLUDED_)
#define AFX_PICTURE_H__26DCD704_4146_4EB7_9570_419030FFBA06__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Picture.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPictureDoc document

class CPictureDoc : public CDocument
{
protected:
	CPictureDoc();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CPictureDoc)

// Attributes
public:

  
// Operations
public:


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPictureDoc)
	public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	protected:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPictureDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CPictureDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CPictureView view

class CPictureView : public CScrollView
{
protected:
	CPictureView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CPictureView)

// Attributes
public:

CFont cf;


// Operations
public:

  void DrawDialog(LPDIALOGRO prun);
  void HiLite(int objid);
  void ObjHiLite(LPDIALOGRO pc);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPictureView)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnInitialUpdate();     // first time after construct
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
  virtual HBRUSH OnCtlColor( CDC* pDC, CWnd* pWnd, UINT nCtlColor );
 
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CPictureView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CPictureView)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PICTURE_H__26DCD704_4146_4EB7_9570_419030FFBA06__INCLUDED_)
