// AnalyzeHTMLView.h : interface of the CAnalyzeHTMLView class
//


#pragma once


class CAnalyzeHTMLView : public CHtmlView
{
protected: // create from serialization only
	CAnalyzeHTMLView();
	DECLARE_DYNCREATE(CAnalyzeHTMLView)

// Attributes
public:
	CAnalyzeHTMLDoc* GetDocument() const;
	afx_msg void GenRC(IHTMLElementCollection *p);

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // called first time after construct

// Implementation
public:
	virtual ~CAnalyzeHTMLView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnFileOpen();
	afx_msg void OnUpdateFileOpen(CCmdUI *pCmdUI);
	afx_msg void OnGenerateCreatercfile();
};

#ifndef _DEBUG  // debug version in AnalyzeHTMLView.cpp
inline CAnalyzeHTMLDoc* CAnalyzeHTMLView::GetDocument() const
   { return reinterpret_cast<CAnalyzeHTMLDoc*>(m_pDocument); }
#endif

