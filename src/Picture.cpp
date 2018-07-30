// Picture.cpp : implementation file
//

#include "stdafx.h"
#include "AccuStudio.h"
#include "GrammarWizard.h"
//#include "project.h"
#include "Picture.h"
#include "accuspeech.h"
#include "common.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CPictureView * ppictview;

/////////////////////////////////////////////////////////////////////////////
// CPictureDoc

IMPLEMENT_DYNCREATE(CPictureDoc, CDocument)

int NumWindows = 0;
HWND  pwins[256];
HWND hiwnd;

LPCONTROLRO pcid[256];
RECT hrect;

extern CGrammarWizard pgw;
extern CObjectWizard powiz;
extern CGramExerciser pge;
extern int IsWizard;

CPictureDoc::CPictureDoc()
{
}

BOOL CPictureDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CPictureDoc::~CPictureDoc()
{
  ppictview = NULL;
}


BEGIN_MESSAGE_MAP(CPictureDoc, CDocument)
	//{{AFX_MSG_MAP(CPictureDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPictureDoc diagnostics

#ifdef _DEBUG
void CPictureDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CPictureDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPictureDoc serialization

void CPictureDoc::Serialize(CArchive& ar)
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
// CPictureDoc commands
/////////////////////////////////////////////////////////////////////////////
// CPictureView

IMPLEMENT_DYNCREATE(CPictureView, CScrollView)

CPictureView::CPictureView()
{
  NumWindows = 0;
}

CPictureView::~CPictureView()
{
}


BEGIN_MESSAGE_MAP(CPictureView, CScrollView)
	//{{AFX_MSG_MAP(CPictureView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPictureView drawing

extern ControlDef VControl[];

//CWnd wins[256];

void CPictureView::DrawDialog(LPDIALOGRO prun)
{
	if (!prun)
		return;
  LPCONTROLRO pc = prun->controlthread;
  char ClassName[256], Caption[256];
  RECT r;
  int i, style;
  CWnd t;
  HFONT hf; 

	
	//HWND win = GetParent()->m_hWnd;
	//LONG nOldStyle = ::GetWindowLong(win, GWL_STYLE);
 //  LONG nNewStyle = nOldStyle | WS_HSCROLL | WS_VSCROLL;
	// ::SetWindowLong(win, GWL_STYLE, nNewStyle);
	

	memset(&hrect, 0, sizeof(RECT));

  r.left= prun->Coords[0];
  r.top= prun->Coords[1];
  r.right= prun->Coords[2]+15;
  r.bottom= prun->Coords[3]+15;
  if (IsWizard == 1)
    MapDialogRect(pgw, &r);
  else if (IsWizard == 0)
    MapDialogRect(powiz, &r);
  else
    MapDialogRect(pge, &r);
  GetParentFrame()->MoveWindow(&r,TRUE);

  hf =(HFONT) cf; 

  for (i=0; i < NumWindows; i++)
    if (pwins[i])
    ::DestroyWindow(pwins[i]);
  
  NumWindows = 0;
  
  if (prun->ControlType != C_DIALOG)
    return;
	CDocument* pDoc = GetDocument();
  if (prun->Caption >= 0)
    pDoc->SetTitle((CString)"Dialog: " + pnames[prun->Caption]);
  else
    pDoc->SetTitle((CString)"Dialog: " + pdefines[prun->ControlIDOrd]);

  while (pc)
  {
//    if (pc->ControlType == C_CONTROL)
//      goto skip;
    ClassName[0] = Caption[0] = 0;
    if (VControl[pc->ControlType].ControlClass)
      strcpy(ClassName, VControl[pc->ControlType].ControlClass);
    else if (pc->ClassName >= 0)
      strcpy(ClassName, pnames[pc->ClassName]);
    if (pc->Caption >= 0)
      strcpy(Caption, pnames[pc->Caption]);

    r.left= pc->Coords[0];
    r.top= pc->Coords[1];
    r.right= pc->Coords[2];
    r.bottom= pc->Coords[3];
    if (IsWizard == 1)
      MapDialogRect(pgw, &r);
    else if (IsWizard == 0)
      MapDialogRect(powiz, &r);
    else
      MapDialogRect(pge, &r);
 
    style = (WS_VISIBLE | VControl[pc->ControlType].wtype | pc->Style) & (~pc->NotStyle);
    pwins[NumWindows] = CreateWindow(ClassName, Caption, WS_CHILD | style, 
      r.left, r.top, r.right, r.bottom, 
      this->m_hWnd, NULL, 
      AfxGetInstanceHandle( ), NULL);
    if (pwins[NumWindows] == NULL)
      goto skip;
//    ::ShowWindow(pwins[NumWindows], SW_SHOW);
    ::SendMessage(pwins[NumWindows], WM_SETFONT, (WPARAM) hf, 1);
		

    /*
    wins[NumWindows].CreateEx(0, ClassName, Caption, WS_CHILD | 
      VControl[pc->ControlType].wtype | pc->Style, 
      r.left, r.top, r.right, r.bottom, 
      this->m_hWnd, 0, 
      NULL);
    if (wins[NumWindows].m_hWnd == NULL)
      goto skip;
    wins[NumWindows].ShowWindow(SW_SHOW);
    pwins[NumWindows] = wins[NumWindows].m_hWnd;
    ::SendMessage(pwins[NumWindows], WM_SETFONT, (WPARAM) hf, 1);
		*/ 

    pcid[NumWindows] = pc;
    NumWindows++;
    
skip:
    pc = pc->lpNext;
  }
}

void CPictureView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();
	CSize sizeTotal;
	// TODO: calculate the total size of this view
	sizeTotal.cx = sizeTotal.cy = 1500;
	SetScrollSizes(MM_TEXT, sizeTotal);
  GetParentFrame()->RecalcLayout();
  ResizeParentToFit();
  cf.CreatePointFont(8, "MS Sans Serif");
}

void CPictureView::OnDraw(CDC* pDC)
{
  RECT r;
	int i;
  CBrush cb;
  cb.CreateSolidBrush(RGB(465,465,465));
//  FillOutsideRect( pDC, &cb );
  GetClientRect(&r);
  pDC->FillRect(&r, &cb); 

	for (i=0; i < NumWindows; i++)
		::RedrawWindow(pwins[i], NULL, NULL, RDW_UPDATENOW);
	pDC->InvertRect(&hrect);
}

/////////////////////////////////////////////////////////////////////////////
// CPictureView diagnostics

#ifdef _DEBUG
void CPictureView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CPictureView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPictureView message handlers


HBRUSH CPictureView::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CWnd::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	
	// TODO: Return a different brush if the default is not desired
	return hbr;
}


BOOL CPictureView::OnCommand(WPARAM wParam, LPARAM lParam) 
{
  HWND win;
  int i;

  win = (HWND)lParam;
  for (i=0; i < NumWindows; i++)
    if (win == pwins[i])
      break;
  if (i < NumWindows)
  {
    HiLite(i);
    if (IsWizard == 1)
      pgw.SetSpeechObject(pcid[i]);
    else if (IsWizard == 0)
      powiz.SetSpeechObject(pcid[i]);
    else
      pge.SetSpeechObject(pcid[i]);
  }
	return CScrollView::OnCommand(wParam, lParam);
}

void CPictureView::ObjHiLite(LPDIALOGRO pc)
{
  for (int i=0; i < NumWindows; i++)
    if (pc == pcid[i])
    {
      HiLite(i);
      return;
    }
}

void CPictureView::HiLite(int objid)
{ 
	RECT r;
  
  hiwnd = pwins[objid]; 
  ::GetWindowRect(pwins[objid], &hrect);
  ScreenToClient(&hrect);
	GetClientRect(&r);
	InvalidateRect(&r);
}


BOOL CPictureView::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CScrollView::PreTranslateMessage(pMsg);
}

