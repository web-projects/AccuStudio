// AccuStudioView.h : interface of the CAccuStudioView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_ACCUSTUDIOVIEW_H__64D7C894_B6D4_4249_BFC4_878306AFEC35__INCLUDED_)
#define AFX_ACCUSTUDIOVIEW_H__64D7C894_B6D4_4249_BFC4_878306AFEC35__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxrich.h>

class CAccuStudioView : public CRichEditView
{
protected: // create from serialization only
	CAccuStudioView();
	DECLARE_DYNCREATE(CAccuStudioView)

// Attributes
public:
	CAccuStudioDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAccuStudioView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual HRESULT QueryAcceptData(LPDATAOBJECT lpdataobj,
		CLIPFORMAT* lpcfFormat, DWORD /*dwReco*/, BOOL bReally, HGLOBAL hMetaPict);

	void ShowXY(void);

	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CAccuStudioView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

  int startline, startcol;
  void DoGoTo( int line, int col);
	BOOL IsModified;

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CAccuStudioView)
	afx_msg void OnFileNewProject();
	afx_msg void OnFileCloseProject();
	afx_msg void OnFileOpenProject();
	afx_msg void OnFileSaveProject();
	afx_msg void OnViewProject();
	afx_msg void OnProjectAddGrammar();
	afx_msg void OnProjectAddGrammarset();
	afx_msg void OnProjectAddgrammarfiles();
	afx_msg void OnFileSaveas();
	afx_msg void OnBuildBuildfromdefinitionfile();
	afx_msg void OnBuildBuildProject();
	afx_msg void OnUpdateViewProject(CCmdUI* pCmdUI);
	afx_msg void OnViewResults();
	afx_msg void OnUpdateViewResults(CCmdUI* pCmdUI);
	afx_msg void OnUpdateProjectAddGrammar(CCmdUI* pCmdUI);
	afx_msg void OnUpdateProjectAddGrammarset(CCmdUI* pCmdUI);
	afx_msg void OnUpdateProjectAddgrammarfiles(CCmdUI* pCmdUI);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnDestroy();
	afx_msg void OnEditFind();
	afx_msg void OnEditGoto();
	afx_msg void OnViewObjectwizard();
	afx_msg void OnUpdateViewObjectwizard(CCmdUI* pCmdUI);
	afx_msg void OnGrammarExercise();
	afx_msg void OnUpdateGrammarExercise(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnUpdateProjectAddresourcefromresourcefile(CCmdUI *pCmdUI);
	afx_msg void OnHelpSemanticcodes32835();
};

#ifndef _DEBUG  // debug version in AccuStudioView.cpp
inline CAccuStudioDoc* CAccuStudioView::GetDocument()
   { return (CAccuStudioDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ACCUSTUDIOVIEW_H__64D7C894_B6D4_4249_BFC4_878306AFEC35__INCLUDED_)
