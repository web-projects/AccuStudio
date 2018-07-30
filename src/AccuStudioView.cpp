// AccuStudioView.cpp : implementation of the CAccuStudioView class
//

#include "stdafx.h"
#include "AccuStudio.h"
#include "MainFrm.h"

#include "AccuStudioDoc.h"
#include "AccuStudioView.h"
#include "project.h"
#include "find.h"
#include "GoTo.h"

#include "common.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// CAccuStudioView

IMPLEMENT_DYNCREATE(CAccuStudioView, CRichEditView)

BEGIN_MESSAGE_MAP(CAccuStudioView, CRichEditView)
	//{{AFX_MSG_MAP(CAccuStudioView)
	ON_COMMAND(ID_FILE_NEW_PROJECT, OnFileNewProject)
	ON_COMMAND(ID_FILE_CLOSE_PROJECT, OnFileCloseProject)
	ON_COMMAND(ID_FILE_OPEN_PROJECT, OnFileOpenProject)
	ON_COMMAND(ID_FILE_SAVE_PROJECT, OnFileSaveProject)
	ON_COMMAND(ID_VIEW_PROJECT, OnViewProject)
	ON_COMMAND(ID_PROJECT_ADD_GRAMMAR, OnProjectAddGrammar)
	ON_COMMAND(ID_PROJECT_ADD_GRAMMARSET, OnProjectAddGrammarset)
	ON_COMMAND(ID_PROJECT_ADDGRAMMARFILES, OnProjectAddgrammarfiles)
	ON_COMMAND(ID_FILE_SAVEAS, OnFileSaveas)
	ON_COMMAND(ID_BUILD_BUILDFROMDEFINITIONFILE, OnBuildBuildfromdefinitionfile)
	ON_COMMAND(ID_BUILD_BUILD_PROJECT, OnBuildBuildProject)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PROJECT, OnUpdateViewProject)
	ON_COMMAND(ID_VIEW_RESULTS, OnViewResults)
	ON_UPDATE_COMMAND_UI(ID_VIEW_RESULTS, OnUpdateViewResults)
	ON_UPDATE_COMMAND_UI(ID_PROJECT_ADD_GRAMMAR, OnUpdateProjectAddGrammar)
	ON_UPDATE_COMMAND_UI(ID_PROJECT_ADD_GRAMMARSET, OnUpdateProjectAddGrammarset)
	ON_UPDATE_COMMAND_UI(ID_PROJECT_ADDGRAMMARFILES, OnUpdateProjectAddgrammarfiles)
	ON_WM_LBUTTONDOWN()
	ON_WM_KEYDOWN()
	ON_WM_DESTROY()
	ON_COMMAND(ID_EDIT_FIND, OnEditFind)
	ON_COMMAND(ID_EDIT_GOTO, OnEditGoto)
	ON_COMMAND(ID_VIEW_OBJECTWIZARD, OnViewObjectwizard)
	ON_UPDATE_COMMAND_UI(ID_VIEW_OBJECTWIZARD, OnUpdateViewObjectwizard)
	ON_COMMAND(ID_GRAMMAR_EXERCISE, OnGrammarExercise)
	ON_UPDATE_COMMAND_UI(ID_GRAMMAR_EXERCISE, OnUpdateGrammarExercise)
	//}}AFX_MSG_MAP
	// Standard printing commands
//	ON_COMMAND(ID_FILE_PRINT, CRichEditView::OnFilePrint)
//	ON_COMMAND(ID_FILE_PRINT_DIRECT, CRichEditView::OnFilePrint)
//	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CRichEditView::OnFilePrintPreview)
	ON_COMMAND(ID_PROJECT_UPDATEPROJECTFROMRESOURCEFILE32825, CMainFrame::OnProjectUpdateprojectfromresourcefile32825)
	ON_UPDATE_COMMAND_UI(ID_PROJECT_UPDATEPROJECTFROMRESOURCEFILE32825, CMainFrame::OnUpdateProjectUpdateprojectfromresourcefile32825)
	ON_COMMAND(ID_PROJECT_ADDRESOURCESFROMRESOURCEFILE, CMainFrame::OnProjectAddresourcesfromresourcefile)
	ON_UPDATE_COMMAND_UI(ID_PROJECT_ADDRESOURCEFROMRESOURCEFILE, &CAccuStudioView::OnUpdateProjectAddresourcefromresourcefile)
	ON_COMMAND(ID_HELP_SEMANTICCODES32835, &CAccuStudioView::OnHelpSemanticcodes32835)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAccuStudioView construction/destruction

CAccuStudioView * pv;

void CAccuStudioView::ShowXY(void)
{
  POINT  cp;
	long line, col;
  char temp[256];
	CPoint pos;

	::GetCaretPos(&cp);
	pos.x = cp.x;
	pos.y = cp.y;
  CRichEditCtrl & pedit = GetRichEditCtrl();
	int curindex = pedit.CharFromPos(pos); // current character index
	line = pedit.LineFromChar(-1);				// current line
	int lineoff = pedit.LineIndex(line);	// current line char offset
	col = curindex - lineoff + 1;
	line++;
  sprintf(temp, "ln %i, Col %i", line, col);
  UpdatePane(temp);
}

CAccuStudioView::CAccuStudioView() 
{
  startline = startcol = IsModified = 0;
  m_nWordWrap = WrapNone;
  pv = this;
}

CAccuStudioView::~CAccuStudioView()
{
  pv = NULL;
}

BOOL CAccuStudioView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CRichEditView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CAccuStudioView drawing

void CAccuStudioView::OnDraw(CDC* pDC)
{
	CAccuStudioDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CAccuStudioView printing

BOOL CAccuStudioView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CAccuStudioView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CAccuStudioView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CAccuStudioView diagnostics

#ifdef _DEBUG
void CAccuStudioView::AssertValid() const
{
	CRichEditView::AssertValid();
}

void CAccuStudioView::Dump(CDumpContext& dc) const
{
	CRichEditView::Dump(dc);
}
#endif //_DEBUG

HRESULT CAccuStudioView::QueryAcceptData(LPDATAOBJECT lpdataobj,
		CLIPFORMAT* lpcfFormat, DWORD Reco, BOOL bReally, HGLOBAL hMetaPict)
{
	COleDataObject dataobj;
	dataobj.Attach(lpdataobj, FALSE);
	if (dataobj.IsDataAvailable(CF_TEXT))
	{
		*lpcfFormat = CF_TEXT;
		return S_OK;
	}	
	else
		return S_FALSE;
}

#ifdef _DEBUG
CAccuStudioDoc* CAccuStudioView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CAccuStudioDoc)));
	return (CAccuStudioDoc*)m_pDocument;
}
#endif //_DEBUG

void CAccuStudioView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{	
  if (this == pActivateView)
    pv = this;
	CRichEditView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

void SetOptions(CRichEditCtrl & pedit)
{
  CHARFORMAT cf;
  pedit.GetDefaultCharFormat(cf);
  strcpy(cf.szFaceName, _T("Courier"));
  cf.dwMask = CFM_FACE | CFM_BOLD;
  cf.dwEffects = 0;
  BOOL rc = pedit.SetDefaultCharFormat(cf);
	//pedit.SetEventMask(pedit.GetEventMask() | ENM_CHANGE | EN_CHANGE);
}

void CAccuStudioView::OnInitialUpdate() 
{

	CAccuStudioDoc* pDoc = GetDocument();

	CRichEditView::OnInitialUpdate();
	
  CRichEditCtrl & pedit = GetRichEditCtrl();
  
  CString title = pDoc->GetPathName();

  CFile pf;
  if ( pf.Open(title, CFile::modeRead) == 0 ) 
    return;
  DWORD size = (DWORD)pf.GetLength();

  LPSTR p = (LPSTR)malloc(size+1);
  pf.Read(p, size);
  p[size] = 0;
  pedit.ReplaceSel(p);
  free(p);
  pf.Close();
  pDoc->SetModifiedFlag(FALSE);
  pedit.SetModify(FALSE);
  pedit.SetSel(0,0);
  SetOptions(pedit);
//  DoGoTo(startline, startcol);
}

/////////////////////////////////////////////////////////////////////////////
// CAccuStudioView message handlers

void CAccuStudioView::OnFileNewProject() 
{
  NewProject();
}

void CAccuStudioView::OnFileCloseProject() 
{
  CloseProject();
}

void CAccuStudioView::OnFileOpenProject() 
{
  OpenProject();
}


void CAccuStudioView::OnFileSaveProject() 
{
  SaveProject();	
}

void CAccuStudioView::OnViewProject() 
{
  ShowProjWindow();	
}

void CAccuStudioView::OnProjectAddGrammar() 
{
}

void CAccuStudioView::OnProjectAddGrammarset() 
{
  AddGrammarset();	
}

void CAccuStudioView::OnProjectAddgrammarfiles() 
{
  AddGrammarFiles();
}


void CAccuStudioView::OnFileSaveas() 
{
  char buf[256] = "Open AccuStudio File";
	CAccuStudioDoc* pDoc = GetDocument();
  CString ct = pDoc->GetTitle();
	int len = ct.GetLength()-1;
	if (ct.GetAt(len) == '*')
		ct.SetAt(len,' ');
	// Put up a standard file dialog to allow output file selection
	CFileDialog dlg(NULL, NULL, ct, OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT, 
    "BNF file (*.abnf)|*.abnf|vdef file (*.vdef)|*.vdef||");
  dlg.m_ofn.lpstrTitle  = (LPSTR) & buf;
 	if( dlg.DoModal() == IDOK )
	{
    ct = dlg.GetFileName();
    pDoc->SetTitle(ct);
    pDoc->OnSaveDocument(dlg.GetPathName());
	}	
	
}


void CAccuStudioView::OnBuildBuildfromdefinitionfile() 
{
  BuildFromDef();		
}

void CAccuStudioView::OnBuildBuildProject() 
{
  BuildProject(TRUE, DirPath, ProjName);	
}

void CAccuStudioView::OnUpdateViewProject(CCmdUI* pCmdUI) 
{
  if (IsDisabled(pCmdUI->m_nID))
    pCmdUI->Enable(FALSE);
  else
    pCmdUI->Enable(TRUE);	
}

void CAccuStudioView::OnViewResults() 
{
  ViewResults();
}

void CAccuStudioView::OnUpdateViewResults(CCmdUI* pCmdUI) 
{
  if (IsDisabled(pCmdUI->m_nID))
    pCmdUI->Enable(FALSE);
  else
    pCmdUI->Enable(TRUE);	
}

void CAccuStudioView::OnUpdateProjectAddGrammar(CCmdUI* pCmdUI) 
{
  if (IsDisabled(pCmdUI->m_nID))
    pCmdUI->Enable(FALSE);
  else
    pCmdUI->Enable(TRUE);	
}

void CAccuStudioView::OnUpdateProjectAddGrammarset(CCmdUI* pCmdUI) 
{
  if (IsDisabled(pCmdUI->m_nID))
    pCmdUI->Enable(FALSE);
  else
    pCmdUI->Enable(TRUE);	
}

void CAccuStudioView::OnUpdateProjectAddgrammarfiles(CCmdUI* pCmdUI) 
{
  if (IsDisabled(pCmdUI->m_nID))
    pCmdUI->Enable(FALSE);
  else
    pCmdUI->Enable(TRUE);	
}


void CAccuStudioView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CRichEditView::OnLButtonDown(nFlags, point);
	ShowXY();
}

void CAccuStudioView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CRichEditView::OnKeyDown(nChar, nRepCnt, nFlags);
	ShowXY();
}

void CAccuStudioView::OnDestroy() 
{
	CRichEditView::OnDestroy();	
}

void CAccuStudioView::OnEditFind() 
{
  CFind * fd = new CFind;
  if (fd->DoModal() == IDOK)
  {
		long xstart, xmax;
		int flags = FR_DOWN;
		if (fd->m_matchcase)
			flags |= FR_MATCHCASE;
		if (fd->m_wholeword)
			flags |= FR_WHOLEWORD;
		CRichEditCtrl & pedit = GetRichEditCtrl();
		FINDTEXTEX ft;
		pedit.GetSel(xstart, xmax);
		ft.chrg.cpMin = xmax;
		ft.chrg.cpMax = -1;
		ft.lpstrText = (LPSTR) fd->m_findstr.GetBuffer(0);
		long n = pedit.FindText(flags, &ft);
		if (n != -1)
			pedit.SetSel(ft.chrgText);	
		else
			pedit.SetSel(xmax,xmax);
  }

  delete(fd);
}

void CAccuStudioView::DoGoTo( int line, int col)
{
  int lineindex = GetRichEditCtrl( ).LineIndex(line-1);
  if (lineindex >= 0)
    GetRichEditCtrl().SetSel(lineindex + col-1, lineindex + col);
}

void CAccuStudioView::OnEditGoto() 
{
  CGoTo * gt = new CGoTo;
  if (gt->DoModal() == IDOK)
  {
    int line = atoi(gt->m_linenum);
    DoGoTo(line,1);
  }

  delete(gt);
}

void CAccuStudioView::OnViewObjectwizard() 
{
  ObjectWizard(NULL);	
}

void CAccuStudioView::OnUpdateViewObjectwizard(CCmdUI* pCmdUI) 
{
  if (IsDisabled(pCmdUI->m_nID))
    pCmdUI->Enable(FALSE);
  else
    pCmdUI->Enable(TRUE);
}

void CAccuStudioView::OnGrammarExercise() 
{
  GrammarExerciser(NULL);	
}

void CAccuStudioView::OnUpdateGrammarExercise(CCmdUI* pCmdUI) 
{
  if (IsDisabled(pCmdUI->m_nID))
    pCmdUI->Enable(FALSE);
  else
    pCmdUI->Enable(TRUE);
}

void CAccuStudioView::OnUpdateProjectAddresourcefromresourcefile(CCmdUI *pCmdUI)
{
  if (IsDisabled(pCmdUI->m_nID))
    pCmdUI->Enable(FALSE);
  else
    pCmdUI->Enable(TRUE);	
}

BOOL CAccuStudioView::PreTranslateMessage(MSG* pMsg)
{
  CRichEditCtrl & pedit = GetRichEditCtrl();
	if (!pedit.GetModify())
	{
		IsModified = 0;
		return 0;
	}
	if (!IsModified)
	{
		IsModified = 1;
		CAccuStudioDoc* pDoc = GetDocument();
		pDoc->SetModifiedFlag(1);
		CString title = pDoc->GetTitle();
		int col = title.GetLength();
		if (col)
			if (title.GetAt(col-1) != '*')
				pDoc->SetTitle(title + (CString)"*");
	}
	return 0;
}


extern void DisplayOpcodes();

void CAccuStudioView::OnHelpSemanticcodes32835()
{
	DisplayOpcodes();
}
