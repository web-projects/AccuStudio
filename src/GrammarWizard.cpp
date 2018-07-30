// GrammarWizard.cpp : implementation file
//

#include "stdafx.h"
#include "AccuStudio.h"
#include "GrammarWizard.h"
//#include "project.h"
#include "AccuSpeech.h"
#include "picture.h"
#include "parse.h"
#include "nbest.h"
#include "common.h"
#include "winuser.h"
#include "mainfrm.h"
#include "settings.h"
#include "pcode.h"
#include "project.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define POPUP_BASE	10000

/////////////////////////////////////////////////////////////////////////////
// CGrammarWizard dialog

extern ControlDef VControl[];
extern CMultiDocTemplate* pDocTemplate, * pAppDoc, * pOutDoc, * pObjPicture;
extern int IsWizard;
extern BOOL SubInFile(LPSTR filename, LPSTR poriginal, LPSTR pnew);
extern int FindName(LPSTR ptoken);
extern UINT AddName(char * pname);
extern CMainFrame* pMainFrame;
extern int FindGramName(LPSTR pname);
extern BYTE * pgramparent;
extern BOOL aisalpha(char cc);
extern BOOL UpdateOpenDocument(CString & filename, BOOL reload);

//BOOL IsRecognizing = FALSE;
FILE * fdialog;
LPDIALOGRO prun;
CPictureDoc * ppictdoc;
CPictureView * ppictview;
LPDIALOGRO pstart;
int RecCount;
FILE * flog;
int		templateID;
HMENU UtilsMenu;
CString DlgGramName, DlgGramPath;
CGrammarWizard * pMe;
void SelectSpeechObject(CWnd * me, CString itemtext);
void Align(CWnd * me);
BOOL NextSpeechObject(CWnd * me);
void StripPrompt(CString & ps);
void StripName(CString & ps);
BOOL BuildSpeechObject(CWnd * me);
BOOL DoShowSpeechObject(CWnd * me);
BOOL DoShowSpeechObjectEx(CWnd * me, BOOL bSetup);
void DoOnSelchangeDlg(CWnd * me);
long filelen;


// New Section Handlers

LPSTR pDialog;
UINT DialogSize;

void AddToSection(LPSTR &p, LPSTR temp, UINT &len)
{
	int slen = strlen(temp);
	p = (LPSTR) realloc(p, len + slen + 8);
	strcpy(p+len, temp);
	len += slen;
	p[len] = 0;
	return;
}

void EndSection(CString Name, LPSTR &p, UINT &len)
{
	char t[256];
	sprintf(t, "<!-- %s -->\n", Name);
	AddToSection(p, t, len);
}

LPSTR StartSection(CString Name, UINT &len)
{
	LPSTR ps;
	char t[256];
	sprintf(t, "<!-- %s -->\n", Name);
	len = strlen(t);
	ps = (LPSTR)calloc(1, len+8);
	strcpy(ps, t);
	return ps;
}

void InsertSection(LPSTR p, UINT len, UINT insertoff)
{
	pDialog = (LPSTR)realloc(pDialog, DialogSize + len + 8);
	LPSTR piloc = pDialog + insertoff;
	memmove(piloc + len, piloc, DialogSize - insertoff);
	memcpy(piloc, p, len);
	DialogSize += len;
	pDialog[DialogSize] = 0;
}

void AppendSection(LPSTR p, UINT len)
{
	pDialog = (LPSTR)realloc(pDialog, DialogSize + len + 8);
	strcpy(pDialog+DialogSize, p);
	DialogSize += len;
	pDialog[DialogSize] = 0;
}

void DeleteSection(LPSTR p, UINT len)
{
	if (!p || !len)
		return;
	memcpy(p, p+len, DialogSize - ((UINT)p - (UINT)pDialog + len));
	DialogSize -= len;
	pDialog = (LPSTR)realloc(pDialog, DialogSize+8);
	pDialog[DialogSize] = 0;
}

void ReplaceSection(LPSTR opLocalGramSection, UINT oLocalGramSize, LPSTR pLocalGramSection, UINT LocalGramSize)
{
	if (oLocalGramSize == 0)
	{
		AppendSection(pLocalGramSection, LocalGramSize);
		return;
	}
	UINT poff = (UINT) opLocalGramSection - (UINT) pDialog;
	DeleteSection(opLocalGramSection, oLocalGramSize);
	InsertSection(pLocalGramSection, LocalGramSize, poff);
}

void FreeDialog()
{
	if (DialogSize == 0)
		return;
	DialogSize = 0;
	if (pDialog)
		free(pDialog);
	pDialog = NULL;
}

void DumpDialog()
{
	if (DialogSize == 0)
		return;
  fdialog = fopen(DlgGramPath, "w");
  if (fdialog == NULL)
  {
    return;
  }
	fwrite(pDialog, 1, DialogSize, fdialog);
	fclose(fdialog);
	UpdateOpenDocument(DlgGramPath, TRUE);
}

LPSTR FindInSection (LPSTR psec, UINT len, LPSTR token)
{
	LPSTR pres;
	char ec = psec[len];
	psec[len] = 0;
	pres = strstr(psec, token);
	psec[len] = ec;
	return pres;
}

LPSTR FindSection(CString Name, UINT &len)
{
	char t[256];
	len = 0;
	sprintf(t, "<!-- %s", Name);
	if (!DialogSize)
		return NULL;
	LPSTR p = strstr(pDialog, t);
	if (!p)
		return NULL;
	LPSTR q = strstr(p+10, t);
	if (!q)
		return NULL;
	LPSTR qq = strchr(q, '\n');
	if (!qq)
		return NULL;
	len = qq + 1 - p;
	return p;
}

void * LoadFile(LPSTR pname)
{
	void * pdat;
  FILE * pin = fopen(pname, "rb");
  if (pin == NULL)
    return NULL;
  fseek(pin, 0, SEEK_END);
  filelen = ftell(pin);
  fseek(pin, 0, SEEK_SET);
  pdat = (LPSTR)malloc(filelen);
  fread(pdat, 1, filelen, pin);
	fclose(pin);
	return pdat;
}

extern CString ExecDir;

CGrammarWizard::CGrammarWizard(CWnd* pParent /*=NULL*/)
	: CDialog(CGrammarWizard::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGrammarWizard)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

}


void CGrammarWizard::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGrammarWizard)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGrammarWizard, CDialog)
	//{{AFX_MSG_MAP(CGrammarWizard)
	ON_EN_SETFOCUS(IDC_GRAMNAME, OnSetfocusGramname)
	ON_EN_SETFOCUS(IDC_CONTROLNAME, OnSetfocusControlname)
	ON_EN_SETFOCUS(IDC_FOCUS_PROMPT, OnSetfocusFocusPrompt)
	ON_EN_SETFOCUS(IDC_DATA_ENTRY_PROMPT, OnSetfocusDataEntryPrompt)
	ON_BN_CLICKED(IDC_NEXT_CONTROL, OnNextControl)
	ON_CBN_SELCHANGE(IDC_DLG, OnSelchangeDlg)
	ON_CBN_SETFOCUS(IDC_PHRASE_LIST, OnSetfocusPhraseList)
	ON_BN_CLICKED(IDC_BUILD, OnGrammarBuild)
	//ON_BN_CLICKED(IDC_BUILD, OnBuild)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_SUBTRACT, OnSubtract)
	ON_WM_CLOSE()
	ON_WM_RBUTTONDOWN()
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_DEADD, OnDeadd)
	ON_NOTIFY(NM_RCLICK, IDC_DATA_ENTRY_PROMPT, OnRclickDataEntryPrompt)
	ON_NOTIFY(NM_RDBLCLK, IDC_DATA_ENTRY_PROMPT, OnRdblclkDataEntryPrompt)
	ON_NOTIFY(NM_RETURN, IDC_DATA_ENTRY_PROMPT, OnReturnDataEntryPrompt)
	ON_NOTIFY(NM_CLICK, IDC_DATA_ENTRY_PROMPT, OnClickDataEntryPrompt)
	//}}AFX_MSG_MAP
    ON_CBN_SELCHANGE(IDC_CONTROL_COMBO, OnSelchangeDlgControl)	
	ON_BN_CLICKED(IDB_PREREC, CGrammarWizard::OnBnClickedPrerec)
//	ON_BN_CLICKED(IDB_PREREC2, CGrammarWizard::OnBnClickedPrerec2)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGrammarWizard message handlers

void CGrammarWizard::SetSpeechObject(LPCONTROLRO pc)
{
  pdres = pc;
  DoShowSpeechObject(this);
}

int GetTextLen(CListBox* me, LPCTSTR lpszText)
{
	ASSERT(AfxIsValidString(lpszText));

	CDC *pDC = me->GetDC();
	ASSERT(pDC);

	CSize size;
	CFont* pOldFont = pDC->SelectObject(me->GetFont());
	if ((me->GetStyle() & LBS_USETABSTOPS) == 0)
	{
		size = pDC->GetTextExtent(lpszText, (int) _tcslen(lpszText));
		size.cx += 3;
	}
	else
	{
		// Expand tabs as well
		size = pDC->GetTabbedTextExtent(lpszText, (int) _tcslen(lpszText), 0, NULL);
		size.cx += 2;
	}
	pDC->SelectObject(pOldFont);
	me->ReleaseDC(pDC);

	return size.cx;
}


void ResetHExtent(CListBox* mylist)
{
	if (mylist->GetCount() == 0)
	{
		mylist->SetHorizontalExtent(0);
		return;
	}

	CWaitCursor cwc;
	int iMaxHExtent = 0;
	for (int i = 0; i < mylist->GetCount(); i++)
	{
		CString csText;
		mylist->GetText(i, csText);
		int iExt = GetTextLen(mylist, csText);
		if (iExt > iMaxHExtent)
			iMaxHExtent = iExt;
	}
	mylist->SetHorizontalExtent(iMaxHExtent);
}

void SetNewHExtent(CListBox* mylist, LPCTSTR lpszNewString)
{
	int iExt = GetTextLen(mylist, lpszNewString);
	if (iExt > mylist->GetHorizontalExtent())
		mylist->SetHorizontalExtent(iExt);
}

BOOL BuildSpeechObject(CWnd * me)
{	
    CComboBox * pMyComboBox = (CComboBox *) me->GetDlgItem(IDC_CONTROL_COMBO);

	while(pMyComboBox->GetCount() > 0)
	{
		pMyComboBox->SetCurSel(0);
		pMyComboBox->DeleteString(0);
	}

	CRect saverec;  
	me->GetWindowRect(&saverec);
	//me->ClientToScreen(&saverec);
	me->MoveWindow(0, 0, 0, 0);

	LPDIALOGRO lastres;
	pdres = NULL;
	do
	{
		if (pdres == NULL)
		{
			if (prun)
				pdres = prun->controlthread;
		}
		else
			pdres = pdres->lpNext;

		if(pdres != NULL)
		{			
			lastres = pdres;
			DoShowSpeechObjectEx(me, TRUE);
		}
	}
	while(pdres != NULL);
	
	me->MoveWindow(saverec);

	Align(me);
	pdres = prun->controlthread;
	DoShowSpeechObject(me);

	return TRUE;
}

void SelectSpeechObject(CWnd * me, CString itemtext)
{
	int i = 0;
	CString tt;

	CRect saverec;  
	me->GetWindowRect(&saverec);
	//me->ClientToScreen(&saverec);
	me->MoveWindow(0, 0, 0, 0);

	pdres = NULL;
	for(i=0;i<NumDefines+1;i++)
	{
		if (pdres == NULL)
		{
			if (prun)
				pdres = prun->controlthread;
		}
		else
			pdres = pdres->lpNext;

		
		if(pdres != NULL)
		{
			tt = (CString) VControl[pdres->ControlType].ControlName + "  " +
				(CString)(CString)pdefines[pdres->ControlIDOrd];

			if(itemtext == tt)
			{
				DoShowSpeechObjectEx(me, FALSE);
				break;
			}
		}
		else
			break;
	}
	
	me->MoveWindow(saverec);

  if (ppictview)
    ppictview->ObjHiLite(pdres);
}

LPDIALOGRO NextSpeechObjectEx()
{
  if (pdres == NULL)
  {
     if (prun)
      pdres = prun->controlthread;
  }
  else
    pdres = pdres->lpNext;

  return pdres;
}

BOOL NextSpeechObject(CWnd * me)
{
//  return ShowSpeechObject();
	NextSpeechObjectEx();
  return DoShowSpeechObject(me);
}


void Show(CWnd * me, UINT id, BOOL action)
{
  CWnd * hid = me->GetDlgItem(id);
  if (hid == NULL || IsWindow(hid->m_hWnd) == FALSE)
    return;
  if (action)
	{
		hid->EnableWindow(TRUE);
//    hid->ModifyStyle(WS_DISABLED, 0, SWP_NOSIZE);
	}
  else
	{
//   hid->ModifyStyle(0, WS_DISABLED, SWP_NOSIZE);
		hid->EnableWindow(FALSE);
	}
  me->RedrawWindow();
}

BOOL DoShowSpeechObject(CWnd * me)
{
	return DoShowSpeechObjectEx(me, FALSE);
}

void ResizeComboBox(CComboBox * pMyComboBox)
{
	//Get rectangles  
	CRect rctComboBox, rctDropDown;  
	pMyComboBox->GetClientRect(&rctComboBox); 
	
	//Combo rect  
	pMyComboBox->GetDroppedControlRect(&rctDropDown); 
	
	//DropDownList rect
	int itemHeight = pMyComboBox->GetItemHeight(-1); 
	
	//Get Item height  
	pMyComboBox->GetParent()->ScreenToClient(&rctDropDown); 
	
	int items = pMyComboBox->GetCount();

	if(items > 10)
		items = 10;

	//Converts coordinates  
	rctDropDown.bottom = rctDropDown.top + rctComboBox.Height() + itemHeight*items; 
	//Set height  
	pMyComboBox->MoveWindow(&rctDropDown); //enable changes}
}

BOOL DoShowSpeechObjectEx(CWnd * me, BOOL bSetup)
{
//  CComboBox * pcom = (CComboBox *) me->GetDlgItem(IDC_BASELINE_GRAMMAR);


  if (pdres == NULL)
    return FALSE;
	Show(me, IDC_FOCUS_PROMPT, TRUE);				
  while (pdres)
    if (IsSpeechObj(pdres) == FALSE)
    {
			if (pdres->ControlType == C_MENU)
			{
				Show(me, IDC_FOCUS_PROMPT, TRUE);				
				goto showit;
			}
      pdres = pdres->lpNext;
    }
    else 
    {
showit:
			//Show(me, IDC_BASELINE_RULE, FALSE);
      if (IsDataObj(pdres) == FALSE)
      {
//        Show(me, IDC_DATA_ENTRY_PROMPT_TEXT, FALSE);
        Show(me, IDC_DATA_ENTRY_PROMPT, FALSE);
        Show(me, IDC_BASELINE_GRAMMAR, FALSE);
//        Show(me, IDC_BASELINE_GRAMMAR_TEXT, FALSE);
        Show(me, IDC_CONTROLNAME, FALSE);
//        Show(me, IDC_CONTROLNAME_TEXT, FALSE);
				Show(me, IDC_DEADD, FALSE);
      }
      else
      {
				Show(me, IDC_DEADD, TRUE);
        Show(me, IDC_DATA_ENTRY_PROMPT, TRUE);
//        Show(me, IDC_DATA_ENTRY_PROMPT_TEXT, TRUE);
        Show(me, IDC_BASELINE_GRAMMAR, TRUE);
//        Show(me, IDC_BASELINE_GRAMMAR_TEXT, TRUE);
        Show(me, IDC_CONTROLNAME, TRUE);
//        Show(me, IDC_CONTROLNAME_TEXT, TRUE);
      }
      if (IsListObj(pdres))
      {
				Show(me, IDC_DEADD, FALSE);
        Show(me, IDC_PHRASE_LIST, TRUE);
				Show(me, IDC_ADD, TRUE);
				Show(me, IDC_SUBTRACT, TRUE);
//        Show(me, IDC_RECPHRASELIST_TEXT, TRUE);
        Show(me, IDC_BASELINE_GRAMMAR, FALSE);
//        Show(me, IDC_BASELINE_GRAMMAR_TEXT, FALSE);
      }
      else
      {
        Show(me, IDC_PHRASE_LIST, FALSE);
				Show(me, IDC_ADD, FALSE);
				Show(me, IDC_SUBTRACT, FALSE);
//        Show(me, IDC_RECPHRASELIST_TEXT, FALSE);
      }
      CString tt = (CString) VControl[pdres->ControlType].ControlName + "  " +
        (CString)(CString)pdefines[pdres->ControlIDOrd];
	  
      CComboBox * pMyComboBox = (CComboBox *) me->GetDlgItem(IDC_CONTROL_COMBO);

		if(bSetup == TRUE)
		{
			pMyComboBox->AddString(tt);		
			ResizeComboBox(pMyComboBox);
			//CRect myRect;
			//pc->GetDroppedControlRect(&myRect);
			//pc->MoveWindow(myRect.left, myRect.right, myRect.Width(),myRect.Height() * 2, 1);

		}
		else
			pMyComboBox->SelectString(-1, tt);

      //me->SetDlgItemText(IDC_CONTROL, tt);

//      Show(me, IDC_CONTROL, FALSE);

      CComboBox * pc = (CComboBox *) me->GetDlgItem(IDC_PHRASE_LIST);
      if (pc)
			{
        pc->ResetContent();
				if (pdres->PhraseList)
				{
					CString ts = pnames[pdres->PhraseList];
					LPSTR q, p = ts.GetBuffer(0);
					q = p;
					while (*p)
					{
						q = strchr(p, '\n');
						if (!q)
							break;						
						*q = 0;
						pc->AddString(p);						
						p = q + 1;
					}
				}
				pc->SetCurSel(0);
			}
      if (me->GetDlgItem(IDC_FOCUS_PROMPT))
        me->SetDlgItemText(IDC_FOCUS_PROMPT, "");
      if (me->GetDlgItem(IDC_DATA_ENTRY_PROMPT))
        me->SetDlgItemText(IDC_DATA_ENTRY_PROMPT, "");
      if (me->GetDlgItem(IDC_CONTROLNAME))
			{
        me->SetDlgItemText(IDC_CONTROLNAME, "");
        Show(me, IDC_CONTROLNAME, TRUE);
				if (pdres->lpGram)
				{
					me->SetDlgItemText(IDC_CONTROLNAME, pgramnames[pdres->lpGram->nGrammarOrdinal]);
					Show(me, IDC_CONTROLNAME, FALSE);
				}
				else if (IsDataObj(pdres))
				{
					CString ts = pdefines[pdres->ControlIDOrd] + (CString) "_Grm";
					me->SetDlgItemText(IDC_CONTROLNAME, ts);
				}
				else
					Show(me, IDC_CONTROLNAME, FALSE);
			}

			me->SetDlgItemText(IDC_BASELINE_RULE, "");
			if (pdres->DataType > 0)		// set baseline rule
			{
        CString ts = pnames[pdres->DataType];
        if (me->GetDlgItem(IDC_BASELINE_RULE))
          me->SetDlgItemText(IDC_BASELINE_RULE, ts);
			}

      if (pdres->NavigationPrompt > 0)
      {
        CString ts = pnames[pdres->NavigationPrompt];
//        StripPrompt(ts);
        if (me->GetDlgItem(IDC_FOCUS_PROMPT))
          me->SetDlgItemText(IDC_FOCUS_PROMPT, ts);
      }
      if (pdres->DataEntryPrompt > 0)
      {
        CString ts = pnames[pdres->DataEntryPrompt];
//        StripPrompt(ts);
        if (me->GetDlgItem(IDC_DATA_ENTRY_PROMPT))
          me->SetDlgItemText(IDC_DATA_ENTRY_PROMPT, ts);
      }

      return TRUE;
    }
  return FALSE;
}


BOOL firstmenu = TRUE;

void DoSubMenu(LPSTR &pMenuSection, UINT &MenuSectionLen, LPDIALOGRO pmenu, BOOL highlevel, CString ps)
{
	CString FocusPrompt;
  char ts[256], temp[256];
  LPSTR p;
  int pos;
  while(pmenu)
  {
    if (pmenu->Caption >= 0 && pmenu->ControlIDOrd >= 0)
    {
			if (ps.GetLength())
				strcpy(ts,ps);
			else
			{
				strcpy(ts, pnames[pmenu->Caption]);
				if ( p = strchr(ts, '\\'))
					*p = 0;
				for (pos=0; ts[pos] > 0; pos++)
				{
					if (aisalpha(ts[pos]) == FALSE && !WhiteSpace(ts[pos]) && isdigit(ts[pos]) == FALSE)
					{
						strcpy(&ts[pos], &ts[pos+1]);
						pos--;
					}
				}
			}
      if (ts[0])
      {
        if (firstmenu == FALSE)
          AddToSection(pMenuSection, " | ", MenuSectionLen);
        sprintf(temp, "%s =>SELECTMENUITEM(%s)\n", ts, 
          pdefines[pmenu->ControlIDOrd]);
				AddToSection(pMenuSection, temp, MenuSectionLen);
        firstmenu = FALSE;
      }
    }
    if (pmenu->controlthread)
      DoSubMenu(pMenuSection, MenuSectionLen, pmenu->controlthread, FALSE,"");
    if (highlevel)
      return;
    pmenu = pmenu->lpNext;
  }

}

void DoMenu(LPDIALOGRO pmenu)
{
	FILE * fmenu;
	char temp[256];
  SetCurrentDirectory(DirPath);

  CString MenuName = (CString)ProjName + "_" + 
      (CString) pdefines[pmenu->ControlIDOrd];
  CString tt = (DirPath + MenuName) + ".abnf";
  if (FileExists(tt.GetBuffer(0)))
    return;
  fmenu = fopen(tt, "w");
  if (fmenu == NULL)
  {
    MessageBox(GetFocus(), "Unable to create this Menu Grammar file", 
                         tt, MB_OK);
    return;
  }

  fprintf(fmenu, "//\n// Main menu\n//\n\n");
  fprintf(fmenu, "#include \"%s_res.h\"\n\n", ProjName);
  fprintf(fmenu, "#start <%s>\n\n", MenuName);

  firstmenu = TRUE;
	UINT MenuSectionLen;
  LPSTR pMenuSection = StartSection(MenuName, MenuSectionLen);
	sprintf(temp, "<%s> = \n", MenuName);
	AddToSection(pMenuSection, temp, MenuSectionLen);

	DoSubMenu(pMenuSection, MenuSectionLen, pmenu, TRUE, "");
  AddToSection(pMenuSection, ";\n", MenuSectionLen);
	EndSection(MenuName, pMenuSection, MenuSectionLen);

	fwrite(pMenuSection, 1, MenuSectionLen, fmenu);
	free(pMenuSection);
  fclose(fmenu);
  AddGramName(0, MenuName.GetBuffer(0)); 
//  MessageBox(NULL, "A Grammar has been created for this high-level Menu",
//    MenuName, MB_OK);
}

void Align(CWnd * me)
{
  WINDOWPLACEMENT dwp, pwp;
  RECT r, myrect, hisrect;
  int availwidth, mywidth, hiswidth, myheight;

  me->GetParent()->GetWindowRect(&r);
  me->GetWindowPlacement(&dwp);
  me->GetWindowRect(&myrect);
  ppictview->GetParentFrame()->GetWindowRect(&hisrect);
  ppictview->GetParentFrame()->GetWindowPlacement(&pwp);

  availwidth = r.right - r.left;
  mywidth = dwp.rcNormalPosition.right - dwp.rcNormalPosition.left;
  myheight = dwp.rcNormalPosition.bottom - dwp.rcNormalPosition.top;
  hiswidth = pwp.rcNormalPosition.right - pwp.rcNormalPosition.left;

  dwp.rcNormalPosition.top = hisrect.top;
  dwp.rcNormalPosition.bottom = hisrect.top + myheight;
  if (availwidth > (mywidth+hiswidth))
  { // enough for side-by-side
    dwp.rcNormalPosition.left = hisrect.right;
    dwp.rcNormalPosition.right = hisrect.right + mywidth;
  }
  else
  {
    dwp.rcNormalPosition.right = r.right;
    dwp.rcNormalPosition.left = r.right - mywidth;
  }
  me->SetWindowPlacement(&dwp);
}


void Arrange(CWnd * me)
{
  CComboBox * pcom = (CComboBox *) me->GetDlgItem(IDC_DLG);
  POSITION position;
	CWnd * parent;
	int i = 0, sel = 0;

	pObjPicture->OpenDocumentFile(NULL);
  position = pObjPicture->GetFirstDocPosition();
  ppictdoc = (CPictureDoc *) pObjPicture->GetNextDoc(position);
  position = ppictdoc->GetFirstViewPosition();
  ppictview = (CPictureView *) ppictdoc->GetNextView(position);
	parent = ppictview->GetParent();
	parent->ModifyStyle(WS_SYSMENU, 0, SWP_NOSIZE);

//  fdialog = NULL;
  prun = pdreshead;
  while (prun)
  {
		if (prun == pstart)
			sel = i;
		i++;
    if (prun->ControlType == C_MENU /* && IsWizard == 1 */)
      DoMenu(prun);
    else
    {
      CString ts = pdefines[prun->ControlIDOrd];
      if (prun->Caption >= 0)
        ts += (CString)" (" + pnames[prun->Caption] + (CString)")";
      pcom->AddString(ts);
    }
    prun = prun->lpNext;
  }
	pcom->SetCurSel(-1);
  pcom->SetCurSel(sel);

  ResizeComboBox(pcom);

  pcom = (CComboBox *) me->GetDlgItem(IDC_GRAMMARSET);
  if (pcom)
  {
    for (int i=0; i <NumGramSets; i++)
    {
      pcom->AddString((CString)pgramsetnames[i]);
    }
    pcom->SetCurSel(0);
  }
  
  ResizeComboBox(pcom);

  if (pstart == NULL)
    prun = pdreshead;
  else
    prun = pstart;

  while (prun && prun->ControlType == C_MENU)
    prun = prun->lpNext;


  if (IsWizard != 2 && pdefines && prun)
	{
		CString ts = pdefines[prun->ControlIDOrd] + (CString) "_Grm";
		me->SetDlgItemText(IDC_GRAMNAME, ts);
		Show(me, IDC_GRAMNAME, TRUE);
	}
  pdres = NULL;
  
  ppictview->DrawDialog(prun);

  Align(me);
  DoOnSelchangeDlg(me);
}


BOOL CGrammarWizard::OnInitDialog() 
{
	MENUITEMINFO menuinfo = {0};
	HMENU localmenu = 0;
	BOOL res;
	long cid = POPUP_BASE, pid = 1;
	CDialog::OnInitDialog();
	pMe = this;
	char str[256];
	char * p;	
	int pos = 0, lpos = 0;
	CString expfile = ExecDir + (CString)"RuleLib\\Exports.txt";
	FILE * expf = fopen(expfile, "r");
	AfxInitRichEdit();
	UtilsMenu = NULL;
	if (!expf)
		goto unpopulate;
	UtilsMenu = CreatePopupMenu();
	localmenu = CreatePopupMenu();
	while (UtilsMenu)
	{
		if (fgets(str, sizeof(str), expf) == NULL)
			break;
		p = str;
		if (p[strlen(p)-1] == '\n')
			p[strlen(p)-1] = 0;
		if (*p == '\t')
		{	// rule def
			p++;
			menuinfo.cbSize = sizeof(MENUITEMINFO);
			menuinfo.fMask = MIIM_TYPE | MIIM_ID;
			menuinfo.fType = MFT_STRING;
			menuinfo.wID = cid + pid++;
			menuinfo.dwTypeData  = p;
			menuinfo.cch = strlen(p);
			res = InsertMenuItem(localmenu, lpos++, 1, &menuinfo);
		}
		else
		{	// rule file def
			if (lpos)
			{
				menuinfo.fMask = MIIM_SUBMENU;
				menuinfo.hSubMenu = localmenu;
				res = SetMenuItemInfo(UtilsMenu, pos-1, 1, &menuinfo);

				localmenu = CreatePopupMenu();
				lpos = 0;
				cid += 1000;
				pid = 1;
			}
			menuinfo.cbSize = sizeof(MENUITEMINFO);
			menuinfo.fMask = MIIM_TYPE | MIIM_ID | MIIM_SUBMENU;
			menuinfo.fType = MFT_STRING;
			menuinfo.wID = cid;
			menuinfo.dwTypeData  = p;
			menuinfo.cch = strlen(p);
			res = InsertMenuItem(UtilsMenu, pos++, 1, &menuinfo);
		}
	}
	if (localmenu)
	{
		menuinfo.fMask = MIIM_SUBMENU;
		menuinfo.hSubMenu = localmenu;
		res = SetMenuItemInfo(UtilsMenu, pos-1, 1, &menuinfo);
	}

	fclose(expf);
unpopulate:
  Arrange(this);
  
  //NextSpeechObject(this);
  if (ppictview)
    ppictview->ObjHiLite(pdres);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CGrammarWizard::OnSetfocusGramname() 
{
  UpdatePane(
    "The Grammar name and associated Grammar File (name.abnf).");
}

void CGrammarWizard::OnSetfocusControlname() 
{
  UpdatePane(
    "The Speech Object name and associated Grammar File (name.abnf).");
	
}

void CGrammarWizard::OnSetfocusFocusPrompt() 
{
  UpdatePane(
    "Utterance which causes focus to be set to the Control.");
}

void CGrammarWizard::OnSetfocusDataEntryPrompt() 
{
  UpdatePane(
    "Utterance which causes data to be entered into the Control.");
	
}

void StripName(CString & ps)
{
  char ch;
  static char validnamechars[] = "_.";
  for (int i=0; i < ps.GetLength(); i++)
  {
    ch = ps[i];
    if (aisalpha(ch) || isdigit(ch) || strchr(validnamechars, ch))
      continue;
    ps.Delete(i);
  }
}

void StripPrompt(CString & ps)
{
  char ch;
  static char validpromptchars[] = " _-";
  for (int i=0; i < ps.GetLength(); i++)
  {
    ch = ps[i];
    if (aisalpha(ch) || isdigit(ch) || strchr(validpromptchars, ch))
      continue;
    ps.Delete(i);
  }
}

BOOL FindInFile(LPSTR fname, LPSTR ps)
{
  FILE * f = fopen(fname, "r");
  char * p;

  if (f == NULL)
    return FALSE;
  fseek(f, 0, SEEK_END);
  UINT sz = ftell(f);
  fseek(f, 0, SEEK_SET);
  p = (LPSTR)calloc(sz, 1);
  fread(p, sz, 1, f);
  LPSTR rs = strstr(p, ps);
  fclose(f);
  free(p);
  if (rs)
    return TRUE;
  return FALSE;
}

int CGrammarWizard::BldDialogShell()
{
	return FindGramName(DlgGramName.GetBuffer(0));
}

BOOL FindGroup(LPCONTROLRO rpc, LPCONTROLRO &rpf, LPCONTROLRO &rpl)
{
  BOOL res = FALSE;
  long rmin = 0x7fffffff, rmax = 0; 
  LPCONTROLRO pdr = prun->controlthread;
 // rpf = rpl = NULL;

  while (pdr)
  {
    if (pdr->ControlType == C_RADIOBUTTON)
    {
      int val = atoi(pvalues[pdr->ControlIDOrd]);
      if (val < rmin)
      {
        rpf = pdr;
        rmin = val;
      }
      if (val > rmax)
      {
        rmax = val;
        rpl = pdr;
      }
    }
    else 
    {
      if (res)
        return TRUE;
//      rpf = rpl = NULL;
      rmin = 0x7fffffff;
      rmax = 0;
    }

    if (pdr == rpc)
      res = TRUE;
    pdr = pdr->lpNext;
  }
  return res;
}

BOOL RemoveRule(LPSTR rulename, char semi)
{
  BOOL res = FALSE;
  LPSTR prun, pdat, pt;
  FILE * pin = fopen(DlgGramPath, "rb");
  if (pin == NULL)
    return FALSE;
  fseek(pin, 0, SEEK_END);
  long len = ftell(pin);
  fseek(pin, 0, SEEK_SET);
  prun = pdat = (LPSTR)malloc(len+10);
  fread(pdat, 1, len, pin);

  prun = strstr(pdat, rulename);
  if (prun )
	{
		pt = prun;
		while(*pt && *pt++ != semi);
		while (*pt == 13 || *pt == 10)
			pt++;
		memcpy(prun, pt, (pdat+len) - pt);
		len = len - (pt-prun);
		res = TRUE;
	}
  fclose(pin);
  if (res)
  {
    pin = fopen(DlgGramPath, "wb");    
    fwrite(pdat, 1, len, pin);
    fclose(pin);
		//UpdateOpenDocument(DlgGramPath, TRUE);
  }
  free(pdat);
  return res;
}

int GetField(CString cs)
{
	if (cs.GetLength() == 0)
		return 0;
	int i = FindName(cs.GetBuffer(0));
	if (i < 0)
		i = AddName(cs.GetBuffer(0));
	return i;
}

void AddInclude(CString gramname)
{
	char temp[256];
	FILE * lfdialog;

	sprintf(temp, "%s_include.abnf", DlgGramName);
  if (FindInFile(temp, gramname.GetBuffer(0)) == FALSE)
  {
    lfdialog = fopen(temp, "a+");
    if (lfdialog == NULL)
    {
      MessageBox(GetFocus(), "Unable to modify the Dialog include file", temp, MB_OK);
      return;
    }
    fprintf(lfdialog, "#include %s\n", gramname);
    fclose(lfdialog);
  }
}


void CreateRuleBody(LPDIALOGRO lres, LPSTR bname)
{
	char temp[256], lgname[256];
	UINT len;
	LPSTR p;
	int mi = FindGramName(DlgGramName.GetBuffer(0));
	BOOL SkipGram = FALSE;
	char cmnt[3] = {0};

	if (gProjectType == PROJECT_TYPE_TE && lres->Style & ES_READONLY)
	{
		SkipGram = TRUE;		// null grammar for read-only control
		strcpy(cmnt, "//");
	}
	if(gProjectType != PROJECT_TYPE_TE)
	{
		p = StartSection(bname, len);
		sprintf(temp, "<%s_%s> = <VOID>;\n", DlgGramName, bname);
		AddToSection(p, temp, len);
		EndSection(bname, p, len);
		AppendSection(p, len);
		AppendSection("\n", 1);
		free(p);
	}
	if (IsDataObj(lres)== FALSE)  // ignore non-list controls
		return;
	// Now build a local grammar shell
	sprintf(lgname, "%s_%s_Grm", DlgGramName, bname);

	if (SkipGram == FALSE)
	{
		int gi = AddGramName(0, lgname); 
		pgramparent[gi] = mi + 1;
		LinkGrammar(lres, 0, gi);    // add grammar to Dialog object
	}
	p = StartSection(lgname, len);
	sprintf(temp, "\n%s#start <%s>\n", cmnt, lgname);
	AppendSection(temp, strlen(temp));
	sprintf(temp, "%s<%s> = <VOID>;\n", cmnt, lgname);
	AddToSection(p, temp, len);
	EndSection(lgname, p, len);
	AppendSection(p, len);
	AppendSection("\n", 1);
	free(p);
}

void CreateDlgBody(UINT pinsertloc)
{
	LPDIALOGRO lres;
	LPSTR p;
	UINT rc, len;
	char temp[256];

	p = StartSection(DlgGramName, len);
	// now go through and (re) build skeleton
	rc = 0;
	lres = prun->controlthread;

	if(gProjectType != PROJECT_TYPE_TE)
		while(lres)
		{
			if (IsSpeechObj(lres) || lres->ControlType == C_MENU)
			{
				char ch = ' ';
				if (rc)
					ch = '|';				
				sprintf(temp, "%c <%s_%s> ", ch, DlgGramName, pdefines[lres->ControlIDOrd]);
				if ( (++rc % 2) == 0)
					strcat(temp, "\n");
				AddToSection(p, temp, len);
			}
			lres = lres->lpNext;
		};
	if(!rc)
		sprintf(temp, "<VOID> \n");
	else
		sprintf(temp, "\n");
	AddToSection(p, temp, len);
	
	EndSection(DlgGramName, p, len);
	if (!pinsertloc)
		AppendSection(p, len);
	else
		InsertSection(p, len, pinsertloc);
	free(p);
}

BOOL CGrammarWizard::CreateDialogShell(CString gramname, LPDIALOGRO lprun)
{
	char temp[256];
	LPDIALOGRO lres;
	FILE * idialog;
	UINT len;

	DlgGramName = gramname;
	prun = lprun;
	DlgGramPath = (DirPath + gramname) + ".abnf";
	if (FileExists(DlgGramPath.GetBuffer(0)))
	{
		if (UpdateOpenDocument(DlgGramPath, FALSE) == FALSE)
		{
			MessageBox(DlgGramName + ".abnf is open and has been Modified", 
					"Creating Grammar shell", MB_OK);
			return FALSE;
		}
		fdialog = fopen(DlgGramPath, "r");
		if (fdialog == NULL)
			return FALSE;
		fseek(fdialog, 0, SEEK_END);
		DialogSize = ftell(fdialog);
		fseek(fdialog, 0, SEEK_SET);
		pDialog = (LPSTR)calloc(DialogSize+10,1);
		DialogSize = fread(pDialog, 1, DialogSize, fdialog);
		fclose(fdialog);
		LPSTR p = FindSection(DlgGramName, len);
		if (!p)
		{	// Dialog Section missing 
			FreeDialog();
			return FALSE;
		}
		BOOL found = FALSE;
		// Now check for  extra controls
		lres = prun->controlthread;
		while(lres)
		{
			if (IsSpeechObj(lres)|| lres->ControlType == C_MENU)
			{
				LPSTR q = FindInSection(p,len,pdefines[lres->ControlIDOrd]);
				if (!q)
				{
					UINT llen;
					CString ct = DlgGramName + "_" +(CString)pdefines[lres->ControlIDOrd] + "_Grm";
					q = FindSection(ct,llen);
					if (!q)
					{
						found = TRUE;
						CreateRuleBody(lres, pdefines[lres->ControlIDOrd]);
					}
					p = FindSection(DlgGramName, len);
				}
			}
			lres = lres->lpNext;
		}
		if (found)
		{
			if (gProjectType != PROJECT_TYPE_TE)
			{
				int blen = (UINT)p - (UINT)pDialog;
				DeleteSection(p,len);
				CreateDlgBody(blen);
			}
			PModified = TRUE;
			DumpDialog();
			return TRUE;
		}
		return FALSE;
	}
	else
	{
		sprintf(temp, "//\n// Dialog Grammar: %s\n//\n\n", gramname);
		AppendSection(temp, strlen(temp));
		sprintf(temp, "#include \"%s_res.h\"\n", ProjName);
		AppendSection(temp, strlen(temp));
		if(gProjectType == PROJECT_TYPE_TE)
		{
			sprintf(temp,"#include \"vkeys.h\"\n");
			AppendSection(temp, strlen(temp));
		}
		sprintf(temp, "#include \"%s_include\"\n\n", gramname);
		AppendSection(temp, strlen(temp));
		sprintf(temp, "#start <%s>", gramname);
		AppendSection(temp, strlen(temp));

		if (lprun->PreRecActions)
		{
			UINT actions = lprun->PreRecActions & 0x7fffffff;
			if (actions)
			{
				sprintf(temp, " => %s;", pnames[actions]);
				AppendSection(temp, strlen(temp));
			}
			sprintf(temp, "\n");
			AppendSection(temp, strlen(temp));
		}
		else
		{
			sprintf(temp, "\n");
			AppendSection(temp, strlen(temp));
		}
		sprintf(temp, "<%s> = \n", gramname);
		AppendSection(temp, strlen(temp));
		CreateDlgBody(0);
		sprintf(temp, ";\n\n");
		AppendSection(temp, strlen(temp));

		sprintf(temp, "%s%s_include.abnf", DirPath, gramname);
		idialog = fopen(temp, "w");
		if (idialog == NULL)
		{
			MessageBox("Unable to create the Dialog Grammar include file", 
													 temp);
			return FALSE;
		}
		fprintf(idialog, "//\n");
		fclose(idialog);
		int gi = AddGramName(0, gramname.GetBuffer(0)); 
		LinkGrammar(lprun, 0, gi);    // add grammar to Dialog object

		// Now build prototypes of the local control grammars
		lres = lprun->controlthread;
		while(lres)
		{
			if (IsSpeechObj(lres)|| lres->ControlType == C_MENU)
			{
				CreateRuleBody(lres, pdefines[lres->ControlIDOrd]);
			}
			lres = lres->lpNext;
		}
		DumpDialog();
		PModified = TRUE;
	}
	return TRUE;
}

void CGrammarWizard::OnGrammarBuild()
{
	char temp[256];
	LPSTR p;

	if (DialogSize == 0 || pdres == NULL)
	{
		MessageBeep(MB_ICONEXCLAMATION);
		DumpDialog();
		FreeDialog();
    return;
	}

	// handle Menu Control
	if (pdres->ControlType == C_MENU)
	{
		CString FocusPrompt;
		CString MenuName = pdefines[pdres->ControlIDOrd];
		UINT MenuSectionLen, oMenuSectionLen;
		LPSTR opMenuSection = FindSection(MenuName, oMenuSectionLen);
		LPSTR pMenuSection = StartSection(MenuName, MenuSectionLen);
		sprintf(temp, "<%s_%s> = \n", DlgGramName, MenuName);
		AddToSection(pMenuSection, temp, MenuSectionLen);
		firstmenu = TRUE;
		GetDlgItemText(IDC_FOCUS_PROMPT, FocusPrompt);
		//if (FocusPrompt.GetLength())
			pdres->NavigationPrompt = GetField(FocusPrompt);		
		DoSubMenu(pMenuSection, MenuSectionLen, pdres, TRUE, FocusPrompt);
		AddToSection(pMenuSection, ";\n", MenuSectionLen);
		EndSection(MenuName, pMenuSection, MenuSectionLen);
		ReplaceSection(opMenuSection, oMenuSectionLen, pMenuSection, MenuSectionLen);
		PModified = TRUE;
		OnNextControl();
	// end of old onbuild code

		DumpDialog();
		FreeDialog();
		return;
	}

  CString GramName, FocusPrompt, DataEntryPrompt, ControlName, pedit;
	CString oBaselineRule,BaselineRule, BaselineFile, enumrule;
	BOOL isdynamic = FALSE;
  char temp1[256];
  int i,rc;
  char ch;
	int masterindex = FindGramName(DlgGramName.GetBuffer(0));

  GetDlgItemText(IDC_GRAMNAME, GramName);
  if (GramName.GetLength() == 0)
    return;
  GetDlgItemText(IDC_FOCUS_PROMPT, FocusPrompt);
//  StripPrompt(FocusPrompt);

  GetDlgItemText(IDC_CONTROLNAME, ControlName);
  StripName(ControlName);

  GetDlgItemText(IDC_BASELINE_RULE, BaselineRule);

	if (!FocusPrompt.GetLength()&& !ControlName.GetLength() && !BaselineRule.GetLength())
	{
		MessageBeep(MB_ICONEXCLAMATION);
		return;
	}

	pdres->DataType = GetField(BaselineRule);		// save the baseline rule setting
	strcpy(temp1, BaselineRule.GetBuffer(0));
	p = strchr(temp1, ':');
	if (p)
	{
		*p++ = 0;
		BaselineFile = temp1 + (CString)">";
		BaselineRule = (CString)"<" + (CString)p;
	}

  if (IsDataObj(pdres) && /*FocusPrompt.GetLength() &&*/ (ControlName.GetLength() == 0))
  {
    MessageBox("Please supply a Grammar Name for this Control");
    GetDlgItem(IDC_CONTROLNAME)->SetFocus();
    return;
  }

  SetCurrentDirectory(DirPath);
  CComboBox * pc = (CComboBox *) GetDlgItem(IDC_PHRASE_LIST);
  
 	UINT oLocalGramSize, LocalGramSize;
	LPSTR pLocalGramSection = StartSection(pdefines[pdres->ControlIDOrd], LocalGramSize);

	rc = pc->GetCount();
	oBaselineRule = BaselineRule;

	sprintf(temp, "%s_%s_Rule", GramName, pdefines[pdres->ControlIDOrd]);
	BaselineRule = (CString)"<" + temp + (CString)">";
	sprintf(temp, "%s_%s_Rule_Enum", GramName, pdefines[pdres->ControlIDOrd]);
	enumrule = (CString)"<" + temp + (CString)">";
	isdynamic = pdres->PreRecActions & 0x80000000;
	if (isdynamic)	// we have a dynamic list
	{
		sprintf(temp, "#dynamic %s \n\n", enumrule);
		AddToSection(pLocalGramSection, temp, LocalGramSize);
	}
	else
		enumrule = BaselineRule;

	if (IsDataObj(pdres))
	{
		if (isdynamic)
		{
			sprintf(temp, "%s = %s; \n\n", BaselineRule, enumrule);
			AddToSection(pLocalGramSection, temp, LocalGramSize);
		}
		if (!rc && !pdres->PhraseList)
		{
			if (oBaselineRule.GetLength())
				sprintf(temp, "%s = %s;\n\n", enumrule, oBaselineRule);
			else
				sprintf(temp, "%s = #(0);\n\n", enumrule);
			AddToSection(pLocalGramSection, temp, LocalGramSize);
		}
	}
  if (rc || pdres->PhraseList)    // we have a list to select from
  {
		// save the list string
		CString pdat = "";
		for (int k = 0; k < rc; k++)
		{
			CString ct;
			pc->GetLBText(k, ct);
			pdat += ct;
			pdat += (CString)"\n";
		}
		pdres->PhraseList = 0;
		if (pdat.GetLength())
		{
			pdres->PhraseList = GetField(pdat);
		}

		if (!rc)
		{
			sprintf(temp, "%s = #(0);\n\n", enumrule);
			AddToSection(pLocalGramSection, temp, LocalGramSize);
		}
		else
		{
			sprintf(temp, "%s = #(\n", enumrule);
			AddToSection(pLocalGramSection, temp, LocalGramSize);
			for (i=0; i < rc; i++)
			{
				char ltemp[256];
				if (i)
					ch = ',';
				else
					ch = ' ';
				pc->GetLBText(i, ltemp);
				sprintf(temp, " %c %s", ch, ltemp);
				if ((i+1)%4 == 0)
					strcat(temp, "\n");
				AddToSection(pLocalGramSection, temp, LocalGramSize);
			}
			AddToSection(pLocalGramSection, ");\n\n", LocalGramSize);
		}
  }
  sprintf(temp, "<%s_%s> = \n", GramName, pdefines[pdres->ControlIDOrd]);
	AddToSection(pLocalGramSection, temp, LocalGramSize);

	pdres->NavigationPrompt = GetField(FocusPrompt);
	int haveprompt = FocusPrompt.GetLength();
  if (haveprompt)
  {
		// Store away Focus Prompt


    if (pdres->ControlType == C_RADIOBUTTON)
    {
      LPCONTROLRO rpf, rpl;
      rpf = rpl = pdres;
      FindGroup(pdres, rpf, rpl);
      sprintf(temp, "\t%s => SETRADIO(%s, %s, %s)\n", FocusPrompt.GetBuffer(0), 
         pdefines[pdres->ControlIDOrd], 
         pdefines[rpf->ControlIDOrd],pdefines[rpl->ControlIDOrd]);
      AddToSection(pLocalGramSection, temp, LocalGramSize);
    }
    else
    {
      if (pdres->ControlType >= C_PUSHBUTTON &&  pdres->ControlType < C_RADIOBUTTON)
      {
        sprintf(temp, "\t%s => DEPRESS(%s)\n", FocusPrompt.GetBuffer(0), 
           pdefines[pdres->ControlIDOrd]);
      }
      else
        sprintf(temp, "\t%s => SETFOCUS(%s)\n", FocusPrompt.GetBuffer(0), 
           pdefines[pdres->ControlIDOrd]);
			AddToSection(pLocalGramSection, temp, LocalGramSize);
    }
  }
  GetDlgItemText(IDC_DATA_ENTRY_PROMPT, DataEntryPrompt);
	pdres->DataEntryPrompt = GetField(DataEntryPrompt);
//  StripPrompt(DataEntryPrompt);

  if (DataEntryPrompt.GetLength())
  {
		haveprompt = 1;
		// remember the data entry prompt

    if (FocusPrompt.GetLength())
		AddToSection(pLocalGramSection, " | ", LocalGramSize);
    if (rc || IsListObj(pdres))  // do we have a List Grammar?
		{
      sprintf(temp, "\t%s %s => SELECTITEM(%s, %s)\n",  DataEntryPrompt, BaselineRule,
         pdefines[pdres->ControlIDOrd], BaselineRule);
			AddToSection(pLocalGramSection, temp, LocalGramSize);
		}
    else
		{
			if (BaselineRule.GetLength() == 0)
				sprintf(temp, "\t%s => SETFIELD(%s, \"\")\n", DataEntryPrompt, pdefines[pdres->ControlIDOrd]);
			else
				sprintf(temp, "\t%s %s => SETFIELD(%s, %s)\n", DataEntryPrompt, BaselineRule,
					pdefines[pdres->ControlIDOrd], BaselineRule);
			AddToSection(pLocalGramSection, temp, LocalGramSize);
		}
  }

	if (haveprompt)
		AddToSection(pLocalGramSection, ";\n", LocalGramSize);
	else
		AddToSection(pLocalGramSection, "<VOID>;\n", LocalGramSize);

  // Append to the main dialog Include files
  sprintf(temp, "%s_include.abnf", GramName.GetBuffer(0));
  if (BaselineRule.GetLength())
  {
    CString fif = BaselineRule;
    if (rc == 0)
      fif = BaselineFile;

		AddInclude("<utils>");
		if (fif != "<utils>" && !rc)
			AddInclude(fif);
  }
	// Replace old rule section with new one
	EndSection(pdefines[pdres->ControlIDOrd], pLocalGramSection, LocalGramSize);
	LPSTR opLocalGramSection = FindSection(pdefines[pdres->ControlIDOrd], oLocalGramSize);
	ReplaceSection(opLocalGramSection, oLocalGramSize, pLocalGramSection, LocalGramSize);
	
  // Now Generate the Control Grammar, if present
#ifdef GENLOCALGRAM
  if (ControlName.GetLength() && (FocusPrompt.GetLength() | DataEntryPrompt.GetLength()))
  {
		UINT ControlGramSectionLen;
		LPSTR pControlGramSection;
		int gi = FindGramName(ControlName.GetBuffer(0));
		CString ControlGramName; 
		if (gi < 0)
			ControlGramName = DlgGramName + "_" + ControlName;
		else
			ControlGramName = ControlName;
    CComboBox * pcom = (CComboBox *) GetDlgItem(IDC_GRAMMARSET);
    int gn = pcom->GetCurSel();
    gi = AddGramName(gn, ControlGramName.GetBuffer(0)); 
		pgramparent[gi] = masterindex + 1;
    LinkGrammar(pdres, gn, gi);    // add grammar to speech object

		pControlGramSection = StartSection(ControlGramName, ControlGramSectionLen);

    //sprintf(temp, "#start <%s>", ControlGramName);
		//AddToSection(pControlGramSection, temp, ControlGramSectionLen);
		//if (pdres->PreRecActions)
		//{
		//	UINT actions = pdres->PreRecActions & 0x7fffffff;
		//	temp[0] = 0;
		//	if (actions)
		//		sprintf(temp, " => %s;", pnames[actions]);
		//	strcat(temp, "\n\n");
		//	AddToSection(pControlGramSection, temp, ControlGramSectionLen);
		//}
		//else
		//	AddToSection(pControlGramSection, "\n\n", ControlGramSectionLen);
    if (rc || IsListObj(pdres))   // wja: 6/29/09: empty list/combo box
    {
			AddInclude("<utils>");
			if (BaselineRule.GetLength() == 0)
				sprintf(temp, "<%s> = <NULL>;\n\n", ControlGramName);				
			else
			{
				sprintf(temp, "<%s> = %s ", ControlGramName, BaselineRule);
				AddToSection(pControlGramSection, temp, ControlGramSectionLen);
				sprintf(temp, " => SELECTITEM(%s, %s);\n\n",
					 pdefines[pdres->ControlIDOrd], BaselineRule);
			}
			AddToSection(pControlGramSection, temp, ControlGramSectionLen);
    }
    else
    {
			AddInclude("<utils>");
			if (BaselineRule.GetLength() == 0)
				sprintf(temp, "<%s> = <NULL>;\n\n", ControlGramName);
			else
				sprintf(temp, "<%s> = %s => SETFIELD(%s, %s);\n\n", 
					ControlGramName, BaselineRule, pdefines[pdres->ControlIDOrd], BaselineRule);
			AddToSection(pControlGramSection, temp, ControlGramSectionLen);
    }
		// Replace old rule section with new one
		EndSection(ControlGramName, pControlGramSection, ControlGramSectionLen);
		UINT oControlGramSectionLen;
		LPSTR opControlGramSection = FindSection(ControlGramName, oControlGramSectionLen);
		ReplaceSection(opControlGramSection, oControlGramSectionLen, pControlGramSection, ControlGramSectionLen);
  }
#endif
	PModified = TRUE;
  OnNextControl();
// end of old onbuild code

	DumpDialog();
//	FreeDialog();
}

void CGrammarWizard::OnBuild() 
{
#ifdef OldOnBuild
  CString GramName, FocusPrompt, DataEntryPrompt, ControlName, pedit;
	CString oBaselineRule,BaselineRule, BaselineFile, enumrule;
  char temp[256], temp1[256];
  int i,rc;
  char ch;
	LPSTR p;
	int masterindex;

  if (pdres == NULL)
    return;
	masterindex = BldDialogShell();
  if (masterindex < 0)
		return;
  GetDlgItemText(IDC_GRAMNAME, GramName);
  if (GramName.GetLength() == 0)
    return;
  GetDlgItemText(IDC_FOCUS_PROMPT, FocusPrompt);
//  StripPrompt(FocusPrompt);

  GetDlgItemText(IDC_CONTROLNAME, ControlName);
  StripName(ControlName);


  GetDlgItemText(IDC_BASELINE_RULE, BaselineRule);

	if (!FocusPrompt.GetLength()&& !ControlName.GetLength() && !BaselineRule.GetLength())
	{
		MessageBeep(MB_ICONEXCLAMATION);
		fclose(fdialog);
		return;
	}
	pdres->DataType = GetField(BaselineRule);		// save the baseline rule setting
	strcpy(temp1, BaselineRule.GetBuffer(0));
	p = strchr(temp1, ':');
	if (p)
	{
		*p++ = 0;
		BaselineFile = temp1 + (CString)">";
		BaselineRule = (CString)"<" + (CString)p;
	}

  if (IsDataObj(pdres) && /*FocusPrompt.GetLength() &&*/ (ControlName.GetLength() == 0))
  {
    MessageBox("Please supply a Grammar Name for this Control");
    GetDlgItem(IDC_CONTROLNAME)->SetFocus();
		fclose(fdialog);
    return;
  }

  SetCurrentDirectory(DirPath);
  CComboBox * pc = (CComboBox *) GetDlgItem(IDC_PHRASE_LIST);
  
  sprintf(temp, "<%s_%s> =", GramName, pdefines[pdres->ControlIDOrd]);
	RemoveRule(temp, ';');
	fdialog = fopen(DlgGramPath, "a+");

	rc = pc->GetCount();
	oBaselineRule = BaselineRule;
//	if (rc || BaselineRule.GetLength() || pdres->PhraseList)
	{
		sprintf(temp, "%s_%s_Rule", GramName, pdefines[pdres->ControlIDOrd]);
		BaselineRule = (CString)"<" + temp + (CString)">";
		RemoveRule(BaselineRule.GetBuffer(0), ';');
		sprintf(temp, "%s_%s_Rule_Enum", GramName, pdefines[pdres->ControlIDOrd]);
		enumrule = (CString)"<" + temp + (CString)">";
		sprintf(temp, "#dynamic %s", enumrule);
		RemoveRule(temp, 13);
		RemoveRule(enumrule.GetBuffer(0), ';');
		if (pdres->PreRecActions & 0x80000000)	// we have a dynamic list
		{
			fprintf(fdialog, "#dynamic %s \n\n", enumrule);
		}
		fprintf(fdialog, "%s = %s; \n\n", BaselineRule, enumrule);
		if (!rc && !pdres->PhraseList)
		{
			if (oBaselineRule.GetLength())
				fprintf(fdialog, "%s = %s;\n\n", enumrule, oBaselineRule);
			else
				fprintf(fdialog, "%s = <VOID>;\n\n", enumrule);
		}
	}
  if (rc || pdres->PhraseList)    // we have a list to select from
  {
		// save the list string
		CString pdat = "";
		for (int k = 0; k < rc; k++)
		{
			CString ct;
			pc->GetLBText(k, ct);
			pdat += ct;
			pdat += (CString)"\n";
		}
		pdres->PhraseList = 0;
		if (pdat.GetLength())
		{
			pdres->PhraseList = GetField(pdat);
		}

		if (!rc)
		{
			fprintf(fdialog, "%s = <VOID>;\n\n", enumrule);
		}
		else
		{
			fprintf(fdialog, "%s = #(\n", enumrule);
			for (i=0; i < rc; i++)
			{
				if (i)
					ch = ',';
				else
					ch = ' ';
				pc->GetLBText(i, temp);
				fprintf(fdialog, " %c %s", ch, temp);
				if ((i+1)%4 == 0)
					fprintf(fdialog, "\n");
			}
			fprintf(fdialog, ");\n\n");
		}
  }

//  sprintf(temp, "%s_%s.abnf", GramName, pdefines[pdres->ControlIDOrd]);
//  fdialog = fopen(temp1, "a+");     // the local grammar include name
//  if (fdialog == NULL)
//  {
//    MessageBox("Unable to modify the Dialog Grammar file", temp);
//    return;
//  }
  fprintf(fdialog, "<%s_%s> = \n", GramName, pdefines[pdres->ControlIDOrd]);
	int haveprompt = FocusPrompt.GetLength();
  if (haveprompt)
  {
		// Store away Focus Prompt

		pdres->NavigationPrompt = GetField(FocusPrompt);

    if (pdres->ControlType == C_RADIOBUTTON)
    {
      LPCONTROLRO rpf, rpl;
      rpf = rpl = pdres;
      FindGroup(pdres, rpf, rpl);
      fprintf(fdialog, "\t%s => SETRADIO(%s, %s, %s)\n", FocusPrompt.GetBuffer(0), 
         pdefines[pdres->ControlIDOrd], 
         pdefines[rpf->ControlIDOrd],pdefines[rpl->ControlIDOrd]);
      
    }
    else
    {
      if (pdres->ControlType >= C_PUSHBUTTON &&  pdres->ControlType < C_RADIOBUTTON)
      {
        fprintf(fdialog, "\t%s => DEPRESS(%s)\n", FocusPrompt.GetBuffer(0), 
           pdefines[pdres->ControlIDOrd]);
      }
      else
        fprintf(fdialog, "\t%s => SETFOCUS(%s)\n", FocusPrompt.GetBuffer(0), 
           pdefines[pdres->ControlIDOrd]);
    }
  }
  GetDlgItemText(IDC_DATA_ENTRY_PROMPT, DataEntryPrompt);
//  StripPrompt(DataEntryPrompt);

  if (DataEntryPrompt.GetLength())
  {
		haveprompt = 1;
		// remember the data entry prompt
		pdres->DataEntryPrompt = GetField(DataEntryPrompt);

    if (FocusPrompt.GetLength())
      fprintf(fdialog, " | ");
    if (rc)  // do we have a List Grammar?
		{
      fprintf(fdialog, "\t%s %s => SELECTITEM(%s, %s)\n",  DataEntryPrompt, BaselineRule,
         pdefines[pdres->ControlIDOrd], BaselineRule);
//			BaselineRule.Empty();
		}
    else
		{
			if (BaselineRule.GetLength() == 0)
				fprintf(fdialog, "\t%s => SETFIELD(%s, \"\")\n", DataEntryPrompt, pdefines[pdres->ControlIDOrd]);
			else
				fprintf(fdialog, "\t%s %s => SETFIELD(%s, %s)\n", DataEntryPrompt, BaselineRule,
					pdefines[pdres->ControlIDOrd], BaselineRule);
		}
  }

	if (haveprompt)
		fprintf(fdialog, ";\n");
	else
		fprintf(fdialog, "<VOID>;\n");
  fclose(fdialog);

  // Append to the main dialog Include files
  sprintf(temp, "%s_include.abnf", GramName.GetBuffer(0));
  if (BaselineRule.GetLength())
  {
    CString fif = BaselineRule;
    if (rc == 0)
      fif = BaselineFile;

		AddInclude("<utils>");
		if (fif != "<utils>" && !pc)
			AddInclude(fif);
  }
  // Now Generate the Control Grammar, if present
  if (ControlName.GetLength())
  {
		int gi = FindGramName(ControlName.GetBuffer(0));
		CString ControlGramName; 
		if (gi < 0)
			ControlGramName = DlgGramName + "_" + ControlName;
		else
			ControlGramName = ControlName;
    CComboBox * pcom = (CComboBox *) GetDlgItem(IDC_GRAMMARSET);
    int gn = pcom->GetCurSel();
    gi = AddGramName(gn, ControlGramName.GetBuffer(0)); 
		pgramparent[gi] = masterindex + 1;
    LinkGrammar(pdres, gn, gi);    // add grammar to speech object

		sprintf(temp, "#start <%s>", ControlGramName);
		RemoveRule(temp, 13);
		sprintf(temp, "<%s> = ", ControlGramName);
		RemoveRule(temp, ';');

		fdialog = fopen(DlgGramPath, "a+");
//    fprintf(fdialog, "//\n// %s: local editing grammar\n//\n\n", ControlName);
//    fprintf(fdialog, "#include \"%s_res.h\"\n\n", ProjName);
//    fprintf(fdialog, "#include %s\n\n","<utils>");
//		if (BaselineFile.GetLength() && BaselineFile != "<utils>")
//			fprintf(fdialog, "#include %s\n\n",BaselineFile);

    fprintf(fdialog, "#start <%s>", ControlGramName);
		if (pdres->PreRecActions)
		{
			UINT actions = pdres->PreRecActions & 0x7fffffff;
			if (actions)
				fprintf(fdialog, " => %s;", pnames[actions]);
			fprintf(fdialog, "\n\n");
		}
		else
			fprintf(fdialog, "\n\n");
    if (rc)
    {
			AddInclude("<utils>");
			if (BaselineRule.GetLength() == 0)
				fprintf(fdialog, "<%s> = <fieldedit>;\n\n", ControlGramName);				
			else
			{
				fprintf(fdialog, "<%s> = %s ", ControlGramName, BaselineRule);
				fprintf(fdialog, " => SELECTITEM(%s, %s);\n\n",
					 pdefines[pdres->ControlIDOrd], BaselineRule);
			}
    }
    else
    {
			AddInclude("<utils>");
			if (BaselineRule.GetLength() == 0)
				fprintf(fdialog, "<%s> = <fieldedit>;\n\n", ControlGramName);
			else
				fprintf(fdialog, "<%s> = %s => SETFIELD(%s, %s);\n\n", 
					ControlGramName, BaselineRule, pdefines[pdres->ControlIDOrd], BaselineRule);
    }

    fclose(fdialog);
  }
	PModified = TRUE;
  OnNextControl();
#else
    MessageBox("The Wizard cannot be used with this Grammar file");
#endif
}

void CGrammarWizard::OnCancel() 
{
	FreeDialog();
  pObjPicture->CloseAllDocuments(TRUE);

  RestoreAll();
  Enable(ID_VIEW_GRAMMAR_WIZARD);
  
  if (gProjectLimited == FALSE)
  {
	Enable(ID_VIEW_OBJECTWIZARD);
  }

  Enable(ID_GRAMMAR_EXERCISE);
  if (PModified)
    SaveProject();
	CDialog::OnCancel();
}

void CGrammarWizard::OnSelchangeDlgControl()
{
	CString tt;
	CComboBox * pcom = (CComboBox *) this->GetDlgItem(IDC_CONTROL_COMBO);
	int n = pcom->GetLBTextLen(pcom->GetCurSel());
	pcom->GetLBText(pcom->GetCurSel(), tt.GetBuffer(n));
	tt.ReleaseBuffer(-1);
	SelectSpeechObject(this, tt.GetBuffer(0));

  if (ppictview)
    ppictview->ObjHiLite(pdres);
}

void CGrammarWizard::OnNextControl() 
{
  if (NextSpeechObject(this) == FALSE)
  {
    NextSpeechObject(this);
  }
  if (ppictview)
    ppictview->ObjHiLite(pdres);
}

void DoOnSelchangeDlg(CWnd * me) 
{
  CComboBox * pcom = (CComboBox *) me->GetDlgItem(IDC_DLG);

  int item = pcom->GetCurSel();
  
  if (item == CB_ERR)
    return;

	FreeDialog();
  if (me->GetDlgItem(IDC_GRAMNAME))
    me->SetDlgItemText(IDC_GRAMNAME, "");

  prun = pdreshead;
  while(prun)
  {
    while (prun->ControlType == C_MENU)
		{
			prun = prun->lpNext;
			if (prun == NULL)
				break;
		}
    if (item <= 0)
			break;
    item--;
    prun = prun->lpNext;
  }
  pdres = NULL;
  if (prun == NULL)
    return;
  if (ppictview)
    ppictview->DrawDialog(prun);

	CString ts = pdefines[prun->ControlIDOrd] + (CString) "_Grm";
	pMe->CreateDialogShell(ts, prun);
	me->SetDlgItemText(IDC_GRAMNAME, ts);
	Show(me, IDC_GRAMNAME, TRUE);

  Align(me);

  BuildSpeechObject(me);
}

void CGrammarWizard::OnSelchangeDlg() 
{
  DoOnSelchangeDlg(this);
  //NextSpeechObject(this);
  if (ppictview)
    ppictview->ObjHiLite(pdres);
}



void CGrammarWizard::OnSetfocusPhraseList() 
{
}

BOOL CGrammarWizard::CheckList()
{
  CString pdat;
  CComboBox * pc = (CComboBox *) GetDlgItem(IDC_PHRASE_LIST);
  GetDlgItemText(IDC_PHRASE_LIST, pdat);
  if (pdat.GetLength())
  {
    pc->AddString(pdat);
    SetDlgItemText(IDC_PHRASE_LIST,"");
    GetDlgItem(IDC_PHRASE_LIST)->SetFocus();
    return TRUE;
  }
  return FALSE;
}


BOOL CGrammarWizard::PreTranslateMessage(MSG* pMsg) 
{
	return CDialog::PreTranslateMessage(pMsg);
}


void CGrammarWizard::OnAdd() 
{
  CheckList();
}


void CGrammarWizard::OnSubtract() 
{
  CComboBox * pc = (CComboBox *) GetDlgItem(IDC_PHRASE_LIST);
	int i = pc->GetCurSel();
  i = pc->DeleteString(i);
	pc->SetCurSel(0);
}
/////////////////////////////////////////////////////////////////////////////
// CObjectWizard dialog


CObjectWizard::CObjectWizard(CWnd* pParent /*=NULL*/)
	: CDialog(CObjectWizard::IDD, pParent)
{
	//{{AFX_DATA_INIT(CObjectWizard)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CObjectWizard::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CObjectWizard)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CObjectWizard, CDialog)
	//{{AFX_MSG_MAP(CObjectWizard)
	ON_BN_CLICKED(IDC_NEXT_CONTROL, OnNextControl)
	ON_CBN_SELCHANGE(IDC_DLG, OnSelchangeDlg)
	ON_BN_CLICKED(IDC_ATTACH, OnAttach)	
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
    ON_CBN_SELCHANGE(IDC_CONTROL_COMBO, OnSelchangeDlgControl)	
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CObjectWizard message handlers


BOOL CObjectWizard::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	Arrange(this);
  
  //SetCandidates(prun);    // set grammars for main dialog
	
 // CObjectWizard::OnSelchangeDlgControl();	
  CComboBox * pcom = (CComboBox *) GetDlgItem(IDC_DLG);
	pcom->SetCurSel(-1);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CObjectWizard::OnSelchangeDlgControl()
{
	CString tt;
	CComboBox * pcom = (CComboBox *) this->GetDlgItem(IDC_CONTROL_COMBO);
	int n = pcom->GetLBTextLen(pcom->GetCurSel());
	pcom->GetLBText(pcom->GetCurSel(), tt.GetBuffer(n));
	SelectSpeechObject(this, tt.GetBuffer(0));
	tt.ReleaseBuffer(-1);

	SetCandidates(pdres);
  if (ppictview)
    ppictview->ObjHiLite(pdres);
}

void CObjectWizard::OnCancel() 
{
  pObjPicture->CloseAllDocuments(TRUE);
  RestoreAll();
  
  if (gProjectLimited == FALSE)
	  Enable(ID_VIEW_OBJECTWIZARD);

  if (PModified)
    SaveProject();
	CDialog::OnCancel();
}

void CObjectWizard::SetSpeechObject(LPCONTROLRO pc)
{
  CComboBox * pcom = (CComboBox *) GetDlgItem(IDC_DLG);
  int item = pcom->GetCurSel();
  if (item == CB_ERR)
	{
		MessageBox("Please select a Dialog");
    return;
	}
  pdres = pc;
  DoShowSpeechObject(this);
  SetCandidates(pc);
}


void CObjectWizard::OnNextControl() 
{
  if (NextSpeechObject(this) == FALSE)
  {
    NextSpeechObject(this);
  }
  SetCandidates(pdres);
  if (ppictview)
    ppictview->ObjHiLite(pdres);
}

int CObjectWizard::SetCandidates(LPCONTROLRO pnode)
{
  CListBox * pc = (CListBox *) GetDlgItem(IDC_CANDIDATE);
  CComboBox * pcom = (CComboBox *) GetDlgItem(IDC_GRAMMARSET);
  LPGRAMSETTHREAD lpgs = lpGramSetThread;
  int cgs = pcom->GetCurSel();
  pc->ResetContent();
  while (cgs-- && lpgs)
    lpgs = lpgs->next;
  if (lpgs == NULL)
    return 0;
  LPGRAMTHREAD lpgt = lpgs->pgrams;
  while (lpgt)
  {
    pc->AddString(pgramnames[lpgt->ID]);
    lpgt = lpgt->next;
  }
  
  ResetHExtent(pc);

	if (pnode == 0)
		return 0;
  LPCSOGRAMMAR pgram = pnode->lpGram;
  while (pgram)
  {
    pc->SetSel(pgram->nGrammarOrdinal);
    pgram = pgram->lpNext;
  }
  return 0;
}

void CObjectWizard::OnSelchangeDlg() 
{
  DoOnSelchangeDlg(this);
  pdres = NULL;
  SetCandidates(prun);
}

void CObjectWizard::OnAttach() 
{
  int * psel;
  LPDIALOGRO pobj = pdres;
  CListBox * pc = (CListBox *) GetDlgItem(IDC_CANDIDATE);
  CComboBox * pcom = (CComboBox *) GetDlgItem(IDC_GRAMMARSET);
  int cgs = pcom->GetCurSel();
  int num = pc->GetCount();
  if (num == 0)
    return;
  psel = new int[num];
  int numselected = pc->GetSelItems(num, psel);
  if (pobj == NULL)
    pobj = prun;
  UnlinkAllGrammars(pobj);
  for (int i=0; i < numselected; i++)
    LinkGrammar(pobj, cgs, psel[i]);    // add grammar to Dialog object
  delete psel;
	PModified = TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CGramExerciser dialog


CGramExerciser::CGramExerciser(CWnd* pParent /*=NULL*/)
	: CDialog(CGramExerciser::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGramExerciser)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CGramExerciser::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGramExerciser)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGramExerciser, CDialog)
	//{{AFX_MSG_MAP(CGramExerciser)
	ON_BN_CLICKED(IDC_NEXT_CONTROL, OnNextControl)
	ON_CBN_SELCHANGE(IDC_DLG, OnSelchangeDlg)
	ON_BN_CLICKED(IDC_RECOGNIZE, OnRecognize)
	ON_LBN_SELCHANGE(IDC_REC_RESULTS, OnSelchangeRecResults)	
	//}}AFX_MSG_MAP
    ON_CBN_SELCHANGE(IDC_CONTROL_COMBO, OnSelchangeDlgControl)	
	ON_WM_DESTROY()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGramExerciser message handlers

ASRRC recerr;

CGramExerciser * pgrame; 


void CALLBACK TextCallback(LPSTR p, UINT score)
{
  CListBox * pl;
  char str[256];
  sprintf(str, "%-8i: %s", score, p);
  pl = (CListBox *)pgrame->GetDlgItem(IDC_REC_RESULTS);
  if (RecCount++ == 0)
  {
    fputs("_______________________________\n", flog);
    pl->AddString("_____________________________________________________");
  }
  pl->AddString(str);
  fputs(str,flog);
  fputs("\n",flog);
  int n = pl->GetCount();
  pl->SetCurSel(n-1);
  pl->SetCurSel(-1);  
  ResetHExtent(pl);
}

extern LPSTR GetFuncName(UINT op);
extern int FindDefineValue(LPSTR ptoken);
extern BOOL OPCodeHasID(UINT op);

int IsGlobalField(LPSTR fid)
{
	DWORD fi = atol(fid);
	if (fi & 0x80000000)
		return (int)(fi & 0x7FFFFFFF);
	return -1;
}

void AppendCS(CString & ps, LPSTR p)
{
	if (*p)
		ps += p;
	else
		ps += "\"\"";
}

int CALLBACK SCallback(UINT funcnum,LPSTR * params, UINT numparams)
{
  CListBox * pl;
	CString ps;
  int index;
	if (params == NULL)
		return TRUE;
  ps = GetFuncName(funcnum);  // wja : 09/29/09: rewritten ..
  ps += "(";
  for (UINT i=0; i < numparams; i++)
  {
		if ((i == 0) && (funcnum == GETFIELD_OP || funcnum == SETFIELD_OP) && ((index = IsGlobalField(params[0])) >= 0))
		{
			char t[256];
			sprintf(t, "Field %i", index);
			ps += t;
		}
		else if ((i==0) && (OPCodeHasID(funcnum) == TRUE))
		{
			index = FindDefineValue(params[i]);
			if(index > 0)
				AppendCS(ps, pdefines[index]);
			else
				AppendCS(ps, params[i]);
		}					
    else
			AppendCS(ps, params[i]);

    if ((i+1) < numparams)
      ps += ",";
  }
  ps += ")";
  pl = (CListBox *)pgrame->GetDlgItem(IDC_SEMANTIC_CALLS);
  if (RecCount++ == 0)
  {
    pl->AddString("_____________________________________________________");
  }
	//if (ps.GetLength() >= 255)
	//	ps.SetAt(255,0);
  pl->AddString(ps);
  int n = pl->GetCount();
  pl->SetCurSel(n-1);
  pl->SetCurSel(-1);
  ResetHExtent(pl);
	if (funcnum >= ADD_OP || funcnum == SAYSTRING_OP || funcnum == ACTIVATE_OP)
		return FALSE;
	if (funcnum == GETFIELD_OP || funcnum == SETFIELD_OP)
		return FALSE;
  return TRUE;
}

BOOL CGramExerciser::OpenSpeech(void)
{
#define ASR_FLAG_ACCUSTUDIO 0x40000000
  CString cname;
	ASR_MODES omode;
	int id = GetCheckedRadioButton(IDC_PTT, IDC_AWL);
	if (id == IDC_PTT)
		omode = ASR_MODE_PTT;
	else
		omode = ASR_MODE_AWL;
	GetDlgItem(IDC_PTT)->EnableWindow(0);
	GetDlgItem(IDC_AWL)->EnableWindow(0);
  cname = DirPath + ProjName + (CString)".vbin";
  recerr = VVAppInitWindow(cname, (ASR_MODES) (omode | ASR_FLAG_NBEST | ASR_FLAG_ACCUSTUDIO),
	  pMainFrame->m_hWnd);
	if (recerr)
	{
		MessageBox("Unable to open Speech", "Grammar Exerciser", MB_OK);
		return FALSE;
	}
  VVRegisterTextCallback(TextCallback);
  VVRegisterSemanticCallback(SCallback);
	return TRUE;
}

BOOL CGramExerciser::OnInitDialog() 
{
	CDialog::OnInitDialog();
  	
  flog = fopen("rec_session.txt", "w");
  pgrame = this;
  recerr = ASR_OK;
	IsRecognizing = SpeechOpened = FALSE;
  Arrange(this);
  //SetCandidates(prun);    // set grammars for main dialog 
  //CGramExerciser::OnSelchangeDlgControl();
  CComboBox * pcom = (CComboBox *) GetDlgItem(IDC_DLG);
	pcom->SetCurSel(-1);
	CheckRadioButton(IDC_PTT, IDC_AWL, IDC_PTT);
	GetDlgItem(IDC_RECOGNIZE)->EnableWindow(0);

  //if (ppictview)
  //  VVDialogInit(ppictview->m_hWnd, atoi(pvalues[prun->ControlIDOrd]));
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
//
//LRESULT CGramExerciser::OnAddString(WPARAM wParam, LPARAM lParam)
//{
//	CListBox * pc = (CListBox *) this->GetDlgItem(IDC_CANDIDATE);
//	LRESULT lResult = Default();
//
//	if (!((lResult == LB_ERR) || (lResult == LB_ERRSPACE)))
//	{
//		SetNewHExtent(pc, (LPCTSTR) lParam);
//		pc = (CListBox *) this->GetDlgItem(IDC_SEMANTIC_CALLS);
//		SetNewHExtent(pc, (LPCTSTR) lParam);
//		pc = (CListBox *) this->GetDlgItem(IDC_REC_RESULTS);
//		SetNewHExtent(pc, (LPCTSTR) lParam);
//	}
//
//	return lResult;
//}

void CGramExerciser::SetSpeechObject(LPCONTROLRO pc)
{
  CComboBox * pcom = (CComboBox *) GetDlgItem(IDC_DLG);
  int item = pcom->GetCurSel();
  if (item == CB_ERR)
	{
		MessageBox("Please select a Dialog");
    return;
	}
  if (IsRecognizing)
		OnRecognize();
  pdres = pc;
  DoShowSpeechObject(this);
  SetCandidates(pc);
  if (recerr == 0)
	{
    //VVFocusChange(0); 
    VVFocusChange(atoi(pvalues[pc->ControlIDOrd])); 
	}
}


void CGramExerciser::OnSelchangeDlgControl()
{
	if (!pdres)
		return;
	CString tt;
	CComboBox * pcom = (CComboBox *) this->GetDlgItem(IDC_CONTROL_COMBO);
	int n = pcom->GetLBTextLen(pcom->GetCurSel());
	pcom->GetLBText(pcom->GetCurSel(), tt.GetBuffer(n));
	tt.ReleaseBuffer(-1);

  if (IsRecognizing)
		OnRecognize();

	SelectSpeechObject(this, tt.GetBuffer(0));

	SetCandidates(pdres);
	
  if (recerr == 0)
	{
    /*VVFocusChange(0);*/ 
    VVFocusChange(atoi(pvalues[pdres->ControlIDOrd]));
	}

  if (ppictview)
    ppictview->ObjHiLite(pdres);
}


void CGramExerciser::OnNextControl() 
{
  if (IsRecognizing)
		OnRecognize();
  if (NextSpeechObject(this) == FALSE)
  {
    NextSpeechObject(this);
  }
  SetCandidates(pdres);
  if (recerr == 0)
	{
    /*VVFocusChange(0);*/ 
    VVFocusChange(atoi(pvalues[pdres->ControlIDOrd]));  
	}
  if (ppictview)
    ppictview->ObjHiLite(pdres);
}

int CGramExerciser::SetCandidates(LPCONTROLRO pnode)
{
  if (IsRecognizing)
		OnRecognize();
  CListBox * pc = (CListBox *) GetDlgItem(IDC_CANDIDATE);
  CComboBox * pcom = (CComboBox *) GetDlgItem(IDC_GRAMMARSET);
  LPGRAMSETTHREAD lpgs = lpGramSetThread;
  char ts[256];
  int count = 0, cgs = pcom->GetCurSel();
  CWnd * pb = (CWnd *) GetDlgItem(IDC_RECOGNIZE);

  ts[0] = 0;
  if (pnode->ControlType != C_DIALOG)
    pc->GetText(0, (LPSTR)ts);
  pc->ResetContent();
  if (ts[0])
    pc->AddString(ts);

  ResetHExtent(pc);

  while (cgs-- && lpgs)
    lpgs = lpgs->next;
  if (lpgs == NULL)
    return 0;
  LPGRAMTHREAD lpgt = lpgs->pgrams;
  LPCSOGRAMMAR pgram = pnode->lpGram;
  while (pgram)
  {
    if (pnode->ControlType == C_DIALOG)
      ts[0] = 0;
    else
      strcpy(ts," ");
    strcat(ts, pgramnames[pgram->nGrammarOrdinal]); 
    pc->AddString(ts);
    pgram = pgram->lpNext;
    count++;
  }
  
  if (pc->GetCount() == 0)
  {
    pb->ModifyStyle(0, WS_DISABLED);
    pc->AddString("No defined grammars");
  }
  else
    pb->ModifyStyle(WS_DISABLED, 0);
  pb->RedrawWindow();

  ResetHExtent(pc);

  return count;
}

void CGramExerciser::OnSelchangeDlg() 
{
  if (IsRecognizing)
		OnRecognize();
  DoOnSelchangeDlg(this);
  pdres =		prun->controlthread;	//NULL;
  SetCandidates(prun);
  if (ppictview)
    if (recerr == 0)
    {
			GetDlgItem(IDC_RECOGNIZE)->EnableWindow(1);
      CListBox * pl = (CListBox *)pgrame->GetDlgItem(IDC_REC_RESULTS);
      pl->ResetContent();
      CListBox * pl1 = (CListBox *)pgrame->GetDlgItem(IDC_SEMANTIC_CALLS);
      pl1->ResetContent();
			if (SpeechOpened == FALSE)
				SpeechOpened = OpenSpeech();
			if (recerr == 0)
				VVDialogInit(ppictview->m_hWnd, atoi(pvalues[prun->ControlIDOrd]));
    }
}

extern HWND hiwnd;


void CGramExerciser::OnRecognize() 
{
  if (IsRecognizing)
  {
    if (recerr == 0)
      VVStopListening();
    SetDlgItemText(IDC_RECOGNIZE, "Exercise");
    IsRecognizing = FALSE;
  }
  else
  {
//    ::SetFocus(hiwnd);
    if (recerr == 0)
      VVStartListening();
    RecCount = 0;
    SetDlgItemText(IDC_RECOGNIZE, "Stop");
    IsRecognizing = TRUE;
  };
}

void CGramExerciser::OnDestroy()
{
  if (IsRecognizing)
	{
		VVStopListening();
	}
	if (SpeechOpened)
		VVAppClose();
	IsRecognizing = SpeechOpened = FALSE;
	CDialog::OnDestroy();
}

void CGramExerciser::OnCancel() 
{
  if (IsRecognizing)
	{
		VVStopListening();
	}
  	if (SpeechOpened)
		VVAppClose();
	IsRecognizing = SpeechOpened = FALSE;

  pObjPicture->CloseAllDocuments(TRUE);
  RestoreAll();
  Enable(ID_GRAMMAR_EXERCISE);
  if (flog)
    fclose(flog);
	CDialog::OnCancel();
}

CString SelText;

void CGramExerciser::OnSelchangeRecResults() 
{
  if (IsRecognizing)
		OnRecognize();
  CDialog * pshow;
  CListBox * pl = (CListBox *)pgrame->GetDlgItem(IDC_REC_RESULTS);
	int sel = pl->GetCurSel();
  if (sel == LB_ERR)
    return;
  pl->GetText(sel, SelText);
  pshow = (CNbest *) new CNbest;
  pshow->DoModal();
}


void CGrammarWizard::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CDialog::OnRButtonDown(nFlags, point);
}

void CObjectWizard::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	
	CDialog::OnKeyDown(nChar, nRepCnt, nFlags);
}

BOOL CGrammarWizard::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	if ((wParam & 0xffff) >= POPUP_BASE)
	{
		char rulestr[256], rulefile[256];
		MENUITEMINFO menuinfo = {0};
		menuinfo.cbSize = sizeof(MENUITEMINFO);
		menuinfo.cch = sizeof(rulestr);
		menuinfo.dwTypeData  = rulestr;
		menuinfo.fType = MFT_STRING;
		menuinfo.fMask = MIIM_TYPE | MIIM_ID | MIIM_DATA;
		BOOL res = GetMenuItemInfo(UtilsMenu, wParam, 0, &menuinfo);
		if (res)
		{
			menuinfo.dwTypeData  = rulefile;
			menuinfo.cch = sizeof(rulestr);
			wParam = (wParam / (POPUP_BASE/10)) * (POPUP_BASE/10);
			res = GetMenuItemInfo(UtilsMenu, wParam, 0, &menuinfo);
			if (res)		// we have a file and rule
			{
				char tt[256];
				sprintf(tt, "<%s:%s>", rulefile, rulestr);
				SetDlgItemText(IDC_BASELINE_RULE, tt);
				sprintf(tt, "<%s>", rulestr);
//				CEdit * ce = (CEdit *) GetDlgItem(IDC_DATA_ENTRY_PROMPT);
//				ce->ReplaceSel(tt);
			}
		}
	}
	return CDialog::OnCommand(wParam, lParam);
}

BOOL CGrammarWizard::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	// TODO: Add your specialized code here and/or call the base class
	return CDialog::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CGrammarWizard::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	
	CDialog::OnChar(nChar, nRepCnt, nFlags);
}

void CGrammarWizard::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	
	CDialog::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CGrammarWizard::OnRButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CDialog::OnRButtonDblClk(nFlags, point);
}

void CGrammarWizard::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CDialog::OnLButtonDown(nFlags, point);
}

void CGrammarWizard::WinHelp(DWORD dwData, UINT nCmd) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CDialog::WinHelp(dwData, nCmd);
}

void CGrammarWizard::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	
	if (UtilsMenu)
		DestroyMenu(UtilsMenu);
	UtilsMenu = NULL;
	CDialog::OnClose();
}

BOOL CGrammarWizard::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	FreeDialog();
	if (UtilsMenu)
		DestroyMenu(UtilsMenu);
	UtilsMenu = NULL;
	return CDialog::DestroyWindow();
}

void CGrammarWizard::OnDeadd() 
{	
	RECT pr;
	CEdit * pedit = (CEdit *) GetDlgItem(IDC_BASELINE_RULE); 
	pedit->GetWindowRect(&pr);
	TrackPopupMenu(UtilsMenu,TPM_LEFTALIGN | TPM_RIGHTBUTTON, pr.left, pr.top, 0, 
		this->m_hWnd, NULL);
}

BOOL CGrammarWizard::OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	if (message == WM_NOTIFY)
		wParam = wParam;
	return CDialog::OnChildNotify(message, wParam, lParam, pLResult);
}

int CGrammarWizard::DoModal() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CDialog::DoModal();
}

BOOL CGrammarWizard::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CDialog::OnNotify(wParam, lParam, pResult);
}

void CGrammarWizard::OnRclickDataEntryPrompt(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
}

void CGrammarWizard::OnRdblclkDataEntryPrompt(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
}

void CGrammarWizard::OnReturnDataEntryPrompt(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
}

void CGrammarWizard::OnClickDataEntryPrompt(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
}

void CGrammarWizard::PrerecCommon(LPDIALOGRO pdr)
{
	CPreRec * thed;

	if (pdr == 0)
		return;
	thed = new CPreRec;
	BOOL isdynamic = pdr->PreRecActions & 0x80000000;
	if (isdynamic)
		thed->m_dynamic = TRUE;

	UINT off = pdr->PreRecActions & 0x7fffffff;
	if (off)
	{
		thed->m_actions = pnames[off];
		if (thed->m_actions.Find("SAYSTRING") == 0)
			thed->m_vp = TRUE;
		if (thed->m_actions.Find("WAVECAPTURE") > 0)
			thed->m_vc = TRUE;
		if (thed->m_actions.Find("PLAYWAVE") > 0)
			thed->m_vw = TRUE;
	}
	int res = thed->DoModal();
	if (res == IDOK)
	{
		isdynamic = thed->m_dynamic;
		if (thed->m_actions.GetLength())
			pdr->PreRecActions = GetField(thed->m_actions);
		else
			pdr->PreRecActions = 0;
		if (isdynamic)
			pdr->PreRecActions |= 0x80000000;
	}
	delete thed;
	SaveProject();
}

void CGrammarWizard::OnBnClickedPrerec()
{
	PrerecCommon(pdres);
}

//void CGrammarWizard::OnBnClickedPrerec2()
//{
//	CSelTemplate * psel = new CSelTemplate();
//	psel->DoModal();
//	delete psel;
////	PrerecCommon(prun);
//}


