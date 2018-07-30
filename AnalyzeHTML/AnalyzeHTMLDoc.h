// AnalyzeHTMLDoc.h : interface of the CAnalyzeHTMLDoc class
//


#pragma once


class CAnalyzeHTMLDoc : public CDocument
{
protected: // create from serialization only
	CAnalyzeHTMLDoc();
	DECLARE_DYNCREATE(CAnalyzeHTMLDoc)

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// Implementation
public:
	virtual ~CAnalyzeHTMLDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};


