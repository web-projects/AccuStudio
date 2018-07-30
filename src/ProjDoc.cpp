// ProjDoc.cpp : implementation file
//

#include "stdafx.h"
#include "AccuStudio.h"
#include "ProjDoc.h"
#include "project.h"
#include "AccuStudioDoc.h"
#include "AccuStudioView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CTreeCtrl * pct;
extern HTREEITEM root, gramroot, resroot;
extern CAccuStudioApp theApp;
extern int FindGramName(LPSTR pname);
extern char * * pgramnames;   // Grammar Names
extern BYTE * pgramparent;		 // Grammar parent
extern CAccuStudioView * pv;

/////////////////////////////////////////////////////////////////////////////
// CProjDoc

IMPLEMENT_DYNCREATE(CProjDoc, CDocument)

CProjDoc::CProjDoc()
{
}

BOOL CProjDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CProjDoc::~CProjDoc()
{
  if (PModified)
  {
    if (MessageBox(GetFocus(), "Do you wish to save your Project changes?",
      "Closing Project", MB_YESNO) == IDYES)
      SaveProject();
  }    
}


BEGIN_MESSAGE_MAP(CProjDoc, CDocument)
	//{{AFX_MSG_MAP(CProjDoc)
	ON_COMMAND(ID_VIEW_GRAMMAR_WIZARD, OnViewGrammarWizard)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProjDoc diagnostics

#ifdef _DEBUG
void CProjDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CProjDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CProjDoc serialization

void CProjDoc::Serialize(CArchive& ar)
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

/////////////////////////////////////////////////////////////////////////////
// CProjDoc commands
/////////////////////////////////////////////////////////////////////////////
// CProjView

extern CProjDoc * pdoc;

IMPLEMENT_DYNCREATE(CProjView, CTreeView)

CProjView::CProjView()
{
}

CProjView::~CProjView()
{
  pdoc = NULL;
  Enable(ID_VIEW_PROJECT);
}


BEGIN_MESSAGE_MAP(CProjView, CTreeView)
	//{{AFX_MSG_MAP(CProjView)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	ON_NOTIFY_REFLECT(NM_RCLICK, OnRclick)
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProjView drawing

void CProjView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CProjView diagnostics

#ifdef _DEBUG
void CProjView::AssertValid() const
{
	CTreeView::AssertValid();
}

void CProjView::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CProjView message handlers

void CProjView::OnInitialUpdate() 
{

	CTreeView::OnInitialUpdate();
		
}

BOOL CProjView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class
	
  dwStyle |= (TVS_HASBUTTONS | 
                    TVS_HASLINES | TVS_LINESATROOT | // TVS_EDITLABELS | 
                    TVS_SHOWSELALWAYS | TVS_NOTOOLTIPS);
  dwStyle &= ~WS_MAXIMIZE;
  dwStyle &= ~WS_MINIMIZE;

	return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

void OpenSelFile()
{
	CString newname, fname;
  HTREEITEM oneup, current, twoup;
  current = pct->GetSelectedItem( );
  if (current == NULL)
    return;
  oneup = pct->GetParentItem(current);
  if (oneup == NULL)
    return;
  twoup = pct->GetParentItem(oneup);
  if (twoup == gramroot)
  {
    fname = pct->GetItemText(current);
		int index = FindGramName(fname.GetBuffer(0));
		if (index >= 0)
		{
				if (pgramparent[index])
					newname = pgramnames[pgramparent[index]-1];
				else
					newname = pgramnames[index];
				theApp.OpenDocumentFile(DirPath+ newname + ".abnf");
				if (pv == NULL)
					return;

				CRichEditCtrl & pedit = pv->GetRichEditCtrl();
				//CHARRANGE mycr = {1,100};
				//pedit.SetSel(mycr);
				FINDTEXTEX ft;
				fname = (CString)"<" + fname + (CString)">";
				ft.chrg.cpMin = 0;
				ft.chrg.cpMax = -1;
				ft.lpstrText = (LPSTR) fname.GetBuffer(0);
				long n = pedit.FindText(FR_DOWN, &ft);
				if (n != -1)
				{
					//ft.chrgText.cpMin = ft.chrgText.cpMax;
					pedit.SetSel(ft.chrgText);
					pedit.SetSel(ft.chrgText.cpMax,ft.chrgText.cpMax+1);
				}
				return;
		}
    theApp.OpenDocumentFile(DirPath+fname + ".abnf");
  }
}

void CProjView::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	
  OpenSelFile();
	*pResult = 0;
}

void CProjDoc::OnViewGrammarWizard() 
{
  HTREEITEM oneup, current;
  current = pct->GetSelectedItem( );
  if (current == NULL)
    goto bad;
  oneup = pct->GetParentItem(current);
  if (oneup == NULL)
    goto bad;
  if (oneup == resroot)
  {
    CString fname = pct->GetItemText(current);
    LPDIALOGRO pnode = (LPDIALOGRO) pct->GetItemData(current);
    GrammarWizard(pnode);
    return;
  }
bad:
  GrammarWizard(NULL);
}

void CProjView::OnRclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
}

void AdjustGramSet(int gs, int ge)
{
	LPGRAMSETTHREAD gtp = lpGramSetThread;
	while (gtp)
	{
		LPGRAMTHREAD  prev,pt, pgrams = gtp->pgrams;
		prev = NULL;
		while(pgrams)
		{ 
			pt = pgrams->next;
			if (pgrams->ID >= (UINT)gs && pgrams->ID < (UINT)ge)
			{
				if (prev)
					prev->next = pt;
				else
					gtp->pgrams = pt;
				pgrams = pt;
				continue;
			}
			else if (pgrams->ID >= (UINT)ge)
			{
				pgrams->ID -= (ge - gs);
			} 
			prev = pgrams;
			pgrams = pt;
		}
		gtp = gtp->next;
	}
}

void AdjustGrammars(LPDIALOGRO pnode, UINT gs, UINT ge)
{
	LPCSOGRAMMAR pg, prev, pgnext;
	UINT adjust = ge - gs;
	while (pnode)
	{
		pg = pnode->lpGram;
		prev = NULL;
		while (pg)
		{
			pgnext = pg->lpNext;
			if (pg->nGrammarOrdinal >= gs && pg->nGrammarOrdinal < ge)
			{
				if (prev)
					prev->lpNext = pg->lpNext;
				else
					pnode->lpGram = pg->lpNext;
				free(pg);
				pg = pgnext;
				continue;
			}
			else if (pg->nGrammarOrdinal >= ge)
			{
				pg->nGrammarOrdinal -= adjust;
			}
			prev = pg;
			pg = pgnext;
		}
		if (pnode->controlthread)
			AdjustGrammars(pnode->controlthread, gs, ge);
		pnode = pnode->lpNext;
	}
}

void DoAdjustGrammars(int gs)
{
	int ge, yn, i;
	ge = gs+1;
	free(pgramnames[gs]);
	yn = NumGrams - ge ;
	memcpy(&pgramnames[gs], &pgramnames[ge], yn * sizeof(LPSTR));
	memcpy(&pgramparent[gs], &pgramparent[ge], yn);
	NumGrams = NumGrams - 1;
	AdjustGrammars(pdreshead, gs, ge);
	AdjustGramSet(gs, ge);
	for (i=0; i < NumGrams; i++)
	{
		yn = pgramparent[i]-1;
		if (yn >= 0 && yn >= gs)
			pgramparent[i]--;
	}
}

void CProjView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	
	CTreeView::OnKeyDown(nChar, nRepCnt, nFlags);
	if (nChar == VK_DELETE)		// deleting a tree entry
	{
		HTREEITEM oneup, current, twoup;
		current = pct->GetSelectedItem( );
		if (current == NULL)
			return;
		oneup = pct->GetParentItem(current);
		if (oneup == NULL)
			return;
		if (oneup == resroot)
		{/*
			CString fname = pct->GetItemText(current);
			LPDIALOGRO pnode = (LPDIALOGRO) pct->GetItemData(current);
			int yn = MessageBox("Are you sure you wish to delete this Dialog?", fname, MB_YESNO);
			if (yn == IDYES)
			{
				LPDIALOGRO prev, pnext, prun = pdreshead;
				prev = NULL;
				while (prun)
				{
					pnext = prun->lpNext;
					if (prun == pnode)
					{
						free(prun);
						if (prev)
							prev->lpNext = pnext;
						else
							pdreshead = pnext;
						ShowProjWindow();
						SaveProject();;
						return;
					}
					prev = prun;
					prun = pnext;
				}
			}
			*/
			return;
		}
		twoup = pct->GetParentItem(oneup);
		if (twoup == NULL)
			return;
		
		if (twoup == gramroot)
		{
			CString fname = pct->GetItemText(current);
			LPDIALOGRO pnode = (LPDIALOGRO) pct->GetItemData(current);
			int yn = MessageBox("Are you sure you wish to delete this Grammar?", fname, MB_YESNO);
			if (yn == IDYES)
			{
				int gd, i;
				yn = MessageBox("All references to this Grammar will be deleted", fname, MB_OKCANCEL);
				if (yn == IDCANCEL)
					return;
				gd = FindGramName(fname.GetBuffer(0));
				if (gd < 0)
					return;

				if (pgramparent[gd])
					DoAdjustGrammars(gd);		// Control grammar inside a dialog grammar
				else  // Dialog Grammar - delete it and all siblings
				{
					DoAdjustGrammars(gd);
					if (gd)
						gd--;
					for (i=0; i < NumGrams; i++)
					{
						if (pgramparent[i] == (gd+1))
						{
							DoAdjustGrammars(i);
							i--;
						}
					}
				}
				ShowProjWindow();
				SaveProject();
			}
		}
	}
}
