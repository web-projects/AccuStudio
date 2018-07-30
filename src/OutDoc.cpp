// OutDoc.cpp : implementation file
//

#include "stdafx.h"
#include "AccuStudio.h"
#include "OutDoc.h"
#include "parse.h"
#include "project.h"
#include "AccuStudioDoc.h"
#include "AccuStudioView.h"
#include "vbnfcomp.h"
CRichEditCtrl * re;
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern void SetOptions(CRichEditCtrl & pedit);
extern COutDoc * poutdoc;
extern CAccuStudioApp theApp;
extern CMultiDocTemplate* pDocTemplate;
extern int FindTemplate();
extern CDocument * FindOpenDocument(CString & filename);
extern void FreeTemplates();


/////////////////////////////////////////////////////////////////////////////
// COutDoc

IMPLEMENT_DYNCREATE(COutDoc, CDocument)

COutDoc::COutDoc()
{
	FreeTemplates();
}

BOOL COutDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

COutDoc::~COutDoc()
{
  poutdoc = NULL;
	FreeTemplates();
}


BEGIN_MESSAGE_MAP(COutDoc, CDocument)
	//{{AFX_MSG_MAP(COutDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COutDoc diagnostics

#ifdef _DEBUG
void COutDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void COutDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// COutDoc serialization

void COutDoc::Serialize(CArchive& ar)
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
// COutDoc commands
/////////////////////////////////////////////////////////////////////////////
// COutView

IMPLEMENT_DYNCREATE(COutView, CRichEditView)

COutView::COutView()
{
  m_nWordWrap = WrapNone;
}

COutView::~COutView()
{
}


BEGIN_MESSAGE_MAP(COutView, CRichEditView)
	//{{AFX_MSG_MAP(COutView)
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COutView drawing

void COutView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// COutView diagnostics

#ifdef _DEBUG
void COutView::AssertValid() const
{
	CRichEditView::AssertValid();
}

void COutView::Dump(CDumpContext& dc) const
{
	CRichEditView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// COutView message handlers

void COutView::OnInitialUpdate() 
{
	CRichEditView::OnInitialUpdate();
	
  CRichEditCtrl & pedit = GetRichEditCtrl();
  SetOptions(pedit);
  pedit.SetReadOnly();
}

BOOL COutView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
//  dwStyle |= ES_READONLY;

	return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

int COutView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
//  lpCreateStruct->style |= ES_READONLY;

	if (CRichEditView::OnCreate(lpCreateStruct) == -1)
		return -1;
		
	return 0;
}

CDocument * DoFile(char * pline, int line, int col)
{
	int action = -1;
	LPSTR q;
	extern char lToken[];

	int linenum = line, colnum = col;
	while (WhiteSpace(*pline))
    ++pline;
	if (!strncmp(pline, "Compiling", 9))
		action = 0;
  else if (!strncmp(pline, "Including", 9))
		action = 1;
	else if (!strncmp(pline, "Expanding", 9))
		action = 2;
	if (action < 0)
    return NULL;
	pline += (strlen(pline) - 1);
	*pline = 0;
	while (!WhiteSpace(*(pline-1)))
		pline--;
	strcpy(lToken, pline);
	q = strchr(pline, '.');
	if (action == 2)
	{
		int n = FindTemplate();
		if (n < 0)
			return NULL;
		strcpy(lToken, ptemplates[n].pTemplateFile);
		linenum = ptemplates[n].LineNum + line;
		colnum = ptemplates[n].ColNum + col;
		q = NULL;
	}
	if (!q)
		strcat(lToken, ".abnf");
  CString fn = DirPath + (CString)lToken;
  CFile ef;
  if (ef.Open(fn, CFile::modeRead) == FALSE)
    return NULL;
  ef.Close();
	CDocument * pdoc = FindOpenDocument(fn);
	if (!pdoc)
		pdoc = pDocTemplate->OpenDocumentFile(fn);

	//if (linenum >= 0)
	{
		CAccuStudioDoc * pd = (CAccuStudioDoc *)pdoc;
		POSITION pos = pd->GetFirstViewPosition();
		CAccuStudioView * lpv = (CAccuStudioView * ) pd->GetNextView(pos);
		lpv->startline = linenum;
		lpv->startcol = colnum;
		lpv->DoGoTo(linenum,colnum);
	}
	return pdoc;
}

void COutView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CRichEditView::OnLButtonDown(nFlags, point);
  char line[256];	
  re = &GetRichEditCtrl();
  LPSTR p, q;
  long st, en, level;
  int linenum, colnum;

  if (buildinprogress)
    return;
  re->GetSel(st,en);
  if (st < 0)
    return;
  st = re->LineFromChar(st);
  int num = re->GetLine(st, line, 256);
  line[num] = 0;
  if ( (p = strchr(line, '*')) == NULL)
  {
    DoFile(line, 1, 1);
    return;
  }
  q = strstr(line, "Line");
  if (q == NULL)
    return;
  q += 5;
  linenum = atoi(q);
  while (isdigit(*q))
    q++;
  colnum = atoi(q+4);
  level = (ULONG)p - (ULONG) &line[0];
  while (st--)
  {
    num = re->GetLine(st, line, 256);
    line[num] = 0; 
    if (strchr(line, '*'))
      continue;
    p = line;
    while (WhiteSpace(*p))
      p++;
    en = (ULONG)p - (ULONG) &line[0];
    if (level == (en+1))
    {
      CAccuStudioDoc * pd = (CAccuStudioDoc *)DoFile(line, linenum, colnum);
      //if (pd == NULL)
      //  return;
      //POSITION pos = pd->GetFirstViewPosition();
      //CAccuStudioView * lpv = (CAccuStudioView * ) pd->GetNextView(pos);
      //lpv->startline = linenum;
      //lpv->startcol = colnum;
      //lpv->DoGoTo(linenum,colnum);
      return;
    }
  }
}
