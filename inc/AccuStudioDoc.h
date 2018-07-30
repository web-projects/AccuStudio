// AccuStudioDoc.h : interface of the CAccuStudioDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_ACCUSTUDIODOC_H__79D80F55_D8C0_4150_9F38_B7D22CA65A19__INCLUDED_)
#define AFX_ACCUSTUDIODOC_H__79D80F55_D8C0_4150_9F38_B7D22CA65A19__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CAccuStudioDoc : public CDocument
{
protected: // create from serialization only
	CAccuStudioDoc();
	DECLARE_DYNCREATE(CAccuStudioDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAccuStudioDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual void SetTitle(LPCTSTR lpszTitle);
	protected:
	virtual BOOL SaveModified();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CAccuStudioDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CAccuStudioDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnFileSave();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ACCUSTUDIODOC_H__79D80F55_D8C0_4150_9F38_B7D22CA65A19__INCLUDED_)
