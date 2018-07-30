// AnalyzeHTMLDoc.cpp : implementation of the CAnalyzeHTMLDoc class
//

#include "stdafx.h"
#include "AnalyzeHTML.h"

#include "AnalyzeHTMLDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAnalyzeHTMLDoc

IMPLEMENT_DYNCREATE(CAnalyzeHTMLDoc, CDocument)

BEGIN_MESSAGE_MAP(CAnalyzeHTMLDoc, CDocument)
END_MESSAGE_MAP()


// CAnalyzeHTMLDoc construction/destruction

CAnalyzeHTMLDoc::CAnalyzeHTMLDoc()
{
	// TODO: add one-time construction code here

}

CAnalyzeHTMLDoc::~CAnalyzeHTMLDoc()
{
}

BOOL CAnalyzeHTMLDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}




// CAnalyzeHTMLDoc serialization

void CAnalyzeHTMLDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}


// CAnalyzeHTMLDoc diagnostics

#ifdef _DEBUG
void CAnalyzeHTMLDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CAnalyzeHTMLDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CAnalyzeHTMLDoc commands
