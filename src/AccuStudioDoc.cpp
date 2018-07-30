// AccuStudioDoc.cpp : implementation of the CAccuStudioDoc class
//

#include "stdafx.h"
#include "AccuStudio.h"

#include "AccuStudioDoc.h" 
#include "AccuStudioView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern void SetOptions(CRichEditCtrl & pedit);
extern CMultiDocTemplate* pDocTemplate;

/////////////////////////////////////////////////////////////////////////////
// CAccuStudioDoc

IMPLEMENT_DYNCREATE(CAccuStudioDoc, CDocument)

BEGIN_MESSAGE_MAP(CAccuStudioDoc, CDocument)
	//{{AFX_MSG_MAP(CAccuStudioDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_FILE_SAVE, CAccuStudioDoc::OnFileSave)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAccuStudioDoc construction/destruction

CAccuStudioDoc::CAccuStudioDoc()
{
	// TODO: add one-time construction code here

}

CAccuStudioDoc::~CAccuStudioDoc()
{
}

BOOL CAccuStudioDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

  POSITION pos = GetFirstViewPosition();
  CAccuStudioView * pView = (CAccuStudioView *)GetNextView(pos);
  CRichEditCtrl & pedit = pView->GetRichEditCtrl();
  SetOptions(pedit);

	return TRUE;
}

CDocument * FindOpenDocument(CString & filename)
{
	CString dname;
	CDocument * pdoc;
	POSITION viewposition, position = pDocTemplate->GetFirstDocPosition();
	while (position)
	{
		pdoc = pDocTemplate->GetNextDoc(position);
		dname = pdoc->GetPathName();

		viewposition = pdoc->GetFirstViewPosition();
		CAccuStudioView * pView = (CAccuStudioView *) pdoc->GetNextView(viewposition);
		if (filename == dname)
		{
			POSITION posOpenDoc = pdoc->GetFirstViewPosition();
			if (posOpenDoc != NULL)
			{
				CView* pView = pdoc->GetNextView(posOpenDoc); // get first one
				CFrameWnd* pFrame = pView->GetParentFrame();
				pFrame->ActivateFrame();
				if (pFrame->GetParent() != NULL)
				{
					CFrameWnd* pAppFrame;
					if (pFrame != (pAppFrame = (CFrameWnd*)AfxGetApp()->m_pMainWnd))
					{
						pAppFrame->ActivateFrame();
					}
				}
			}
			return pdoc;
		}
	}
	return NULL;
}

BOOL UpdateOpenDocument(CString & filename, BOOL reload)
{
	CString dname;
	CDocument * pdoc;
	POSITION viewposition, position = pDocTemplate->GetFirstDocPosition();
	while (position)
	{
		pdoc = pDocTemplate->GetNextDoc(position);
		dname = pdoc->GetPathName();

		viewposition = pdoc->GetFirstViewPosition();
		CAccuStudioView * pView = (CAccuStudioView *) pdoc->GetNextView(viewposition);
		CRichEditCtrl & pedit = pView->GetRichEditCtrl();
		if (pedit.GetModify())
		{
			return FALSE;
		}
		if (reload && filename == dname)
		{
			pdoc->OnCloseDocument();
			pDocTemplate->OpenDocumentFile(dname, 1);
			return TRUE;
		}
	}
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CAccuStudioDoc serialization

void CAccuStudioDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
	}
	else
	{
	}
}

/////////////////////////////////////////////////////////////////////////////
// CAccuStudioDoc diagnostics

#ifdef _DEBUG
void CAccuStudioDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CAccuStudioDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CAccuStudioDoc commands

BOOL CAccuStudioDoc::OnSaveDocument(LPCTSTR lpszPathName) 
{
  CDocument::OnSaveDocument(lpszPathName);

  POSITION pos = GetFirstViewPosition();
  CAccuStudioView * pView = (CAccuStudioView *) GetNextView(pos);
  CRichEditCtrl & pedit = pView->GetRichEditCtrl();
  CFile pf;
  if ( pf.Open(lpszPathName, CFile::modeWrite) == 0 ) 
    return FALSE;
  CString ctext;
  pedit.GetWindowText(ctext);
  DWORD size = ctext.GetLength();

  pf.Write(ctext.GetBuffer(0), size);
  pf.Close();
  pedit.SetModify(FALSE);
  return TRUE;	
}

BOOL CAccuStudioDoc::SaveModified() 
{
  POSITION pos = GetFirstViewPosition();
  CAccuStudioView * pView = (CAccuStudioView *) GetNextView(pos);
  CRichEditCtrl & pedit = pView->GetRichEditCtrl();
  if (pedit.GetModify())
    SetModifiedFlag(TRUE);

	return CDocument::SaveModified();
}

void CAccuStudioDoc::SetTitle(LPCTSTR lpszTitle) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CDocument::SetTitle(lpszTitle);
}

void CAccuStudioDoc::OnFileSave()
{
	CDocument::OnFileSave();
	// TODO: Add your command handler code here
}
