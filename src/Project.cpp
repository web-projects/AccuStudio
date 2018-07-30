#include "stdafx.h"
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "resource.h"
#include "project.h"
#include "DlgNewProject.h"
#include "parse.h"
#include "accuspeech.h"
#include "AddProjFiles.h"
#include "GramSetName.h"
#include "mainFrm.h"
#include "AccuStudio.h"
#include "GrammarWizard.h"
#include "projdoc.h"
#include "outdoc.h"
#include "picture.h"
#include "VBNFComp.h"
#include "pcode.h"
#include "xml.h"
#include <afxrich.h>

#include "common.h"
#include "compilermodules.h"

UINT SaveVersion =  VERSION;

//#using <AccuSpeechCommon.dll> // dlltest.dll is the name of the dll which contains the C# code for the Adder class

//using namespace System;
//using namespace System::Collections;
//using namespace std;
//using namespace AccuSpeechCommon;

extern int plevel;
extern Recognizer_Type Recognizer;
extern CString ExecDir;
extern BYTE * pgramatts;			 // Grammar attributes
extern BOOL NewLogic;
extern CMainFrame* pMainFrame;
PROJHEADER  * ph;

UINT NewGramSetName(char * pname);

BOOL Disabled[1000] = {0};    // for disabling commands
BOOL OldDisabled[sizeof(Disabled)];
DWORD ProjSettings;			// Project settings
Target_Models Model = Compact_Model;
Target_Languages Language = English_Lang;

extern void AddDefaultGrammars(BOOL isxml);

extern CMultiDocTemplate* pDocTemplate, * pAppDoc, * pOutDoc, * pObjPicture;
extern BOOL UpdateOpenDocument(CString & filename, BOOL reload);

CTreeCtrl * pct;
CProjDoc * pdoc;
CProjView * pview;

CRichEditCtrl * poutct;
COutDoc * poutdoc;
COutView * poutview;
CFile fProject;

HTREEITEM root, gramroot, resroot;

void * ReAlloc(void * p, int size);
int FindGramName(LPSTR pname);

//  Data for the existing Project

unsigned char gOEMID = OEM_ID_DEMO;

CString ProjName;       // The project name
CString DirPath;        // Project directory path name
//CString ClientPath;     // Client Exectution Path Name
CString ResName;        // Name of the resource file
CString ResPath;        // Path to Resource file (*.rc)

BOOL PModified;         // TRUE if the project has been modified
char * * pnames;        //  For Captions, class names, etc..
int NumNames = 0;

char * * pgramsetnames;   // Grammar Set Names
int NumGramSets = 0;

char * * pgramnames;   // Grammar Names
BYTE * pgramparent;		 // Grammar parent

int NumGrams = 0;

char * * pdefines, * * pvalues;   // For #define names
int NumDefines = 0;
int NumReferences;

LPDIALOGRO pdreshead;   // head node for res dialog
LPDIALOGRO pdres;       // running dialog res ptr
LPGRAMSETTHREAD lpGramSetThread;   // Grammar set thread

int gProjectType = PROJECT_TYPE_NONE;

BOOL gProjectLimited = TRUE;
BOOL gProjectLoadFailed = FALSE;
BOOL gAppLoading = FALSE;

//  End of project data

BOOL   buildinprogress;
BOOL	bDontDisplayDemo = FALSE;

void GetCoords(LPDIALOGRO pdres);
int GetDefVal(LPSTR ptoken);

ControlDef VControl[NUMCONTROLTYPES] = {
  { "DIALOG",   NULL, 0},     // Dialog - not a control
  { "MENU",   NULL, 0},       // Menu container

  { "LTEXT",  "STATIC", SS_LEFT | WS_GROUP},   // STATIC text controls
  { "RTEXT",  "STATIC", SS_RIGHT | WS_GROUP},
  { "CTEXT",  "STATIC", SS_CENTER | WS_GROUP},
  { "ICON",   NULL,     0},
  { "GROUPBOX", "BUTTON",  BS_GROUPBOX | WS_GROUP},
  { "TAB", "SysTabControl32", 0},               // "SysTabControl32"
  { "CONTROL", NULL, 0},
  { "MENUITEM", NULL, 0},

  { "PUSHBUTTON", "BUTTON", BS_PUSHBUTTON},
  { "DEFPUSHBUTTON", "BUTTON", BS_DEFPUSHBUTTON},
  { "CHECKBOX", "BUTTON", BS_AUTOCHECKBOX},           // "Button",BS_AUTOCHECKBOX
  { "RADIOBUTTON", "BUTTON", BS_AUTORADIOBUTTON},       // "Button",BS_AUTORADIOBUTTON

  { "EDITTEXT", "EDIT", WS_BORDER}, 
  { "EDITWAVE", "EDIT", WS_BORDER}, 

  { "COMBOBOX", "COMBOBOX", CBS_DROPDOWN}, 
  { "COMBOXBOXEX", "ComboBoxEx32", 0},        // "ComboBoxEx32"
  { "LISTBOX", "LISTBOX", WS_BORDER}, 

  { "SCROLLBAR", "SCROLLBAR",  0},
  { "LISTVIEW", "SysListView32", 0},          // "SysListView32"
  { "SPINNER", "msctls_updown32", 0},           // "msctls_updown32"
  { "PROGRESS","msctls_progress32", 0},
  { "SLIDER","msctls_trackbar32", 0},
  { "TREEVIEW","SysTreeView32", 0},
  { "RICHEDIT", "RICHEDIT", 0},
  { "DATETIMEPICKER","SysDateTimePick32", 0},
  { "MONTHCALENDAR","SysMonthCal32", 0},
  { "IPADDRESS","SysIPAddress32", 0},
  { "POPUP", NULL, 0}
};

char * ControlClasses[] = 
{
  "Button",
  "ComboBoxEx32",
  "SysListView32",
  "msctls_updown32",
  "msctls_progress32",
  "msctls_trackbar32",
  "SysTreeView32",
  "SysTabControl32",
  "RICHEDIT",
  "SysDateTimePick32",
  "SysMonthCal32",
  "SysIPAddress32"
};

#define NumControlClasses (sizeof(ControlClasses)/sizeof(LPSTR))

BYTE ControlTypes[] =
{
  C_CHECKBOX,
  C_COMBOXBOXEX,
  C_LISTVIEW,
  C_SPINNER,
  C_PROGRESS,
  C_SLIDER,
  C_TREEVIEW,
  C_TAB,
  C_RICHEDIT,
  C_DATETIMEPICKER,
  C_MONTHCALENDAR,
  C_IPADDRESS
};

BOOL LoadRCFile(BOOL newprj, CString rpath, CString rname);

extern CAccuStudioApp theApp;

void DisableAll()
{
  memcpy(OldDisabled, Disabled, sizeof(Disabled));
  memset(Disabled, 1, sizeof(Disabled));
}

void RestoreAll()
{
  memcpy(Disabled, OldDisabled, sizeof(Disabled));
}

void Disable(UINT id)
{
  Disabled[id-32000] = TRUE;
}

void Enable(UINT id)
{
  Disabled[id-32000] = FALSE;
}

BOOL IsDisabled(UINT id)
{
  return Disabled[id-32000];
}

void ProjReport(CString msg)
{
  int te = poutct->GetTextLength()-1;
  poutct->SetSel(te,te);
  poutct->ReplaceSel(msg);
  poutct->ReplaceSel("\n");
}



LPSTR AddGramThread(LPSTR p, LPCONTROLRO parent)
{
  LPCSOGRAMMAR pc, pprev = NULL;
  while (TRUE)
  {
    pc = (LPCSOGRAMMAR) calloc(sizeof(CSOGRAMMAR), 1);
    memcpy(pc, p, sizeof(CSOGRAMMAR));
    p += sizeof(CSOGRAMMAR);
    if (pprev)
      pprev->lpNext = pc;
    pprev = pc;
    if (parent)
      parent->lpGram = pc;
    parent = NULL;
    if (pc->lpNext == NULL)
      break;
  }
  return p;
}

LPSTR AddObjectThread(LPSTR p, LPCONTROLRO parent)
{ 
  LPCONTROLRO tr, prun, prevrun = NULL;
  int size = ph->ControlSize;

  while (TRUE)
  {
    prun = (LPCONTROLRO) calloc(sizeof(CONTROLRO), 1);
    memcpy(prun, (LPCONTROLRO) p, size);
    p += size;

    if (pdreshead == NULL)
      pdreshead = prun;

    if (parent)
      parent->controlthread = prun;
    parent = NULL;
    
    if (prevrun)
      prevrun->lpNext = prun;
    prevrun = prun;

    tr = prun->controlthread;
    prun->controlthread = NULL;
    if (tr)
      p = AddObjectThread(p, prun);

    tr = prun->lpNext;
    prun->lpNext = NULL;
    if (prun->lpGram)
      p = AddGramThread(p, prun);
    if (tr == NULL)
      break;
  }
  return p;
}


void OpenProject()
{
	char buf[] = "Open Project";
  char fullname[256], filename[256];
  char * p;

	CFileDialog dlg(TRUE, "*.vprj", "*.vprj", OFN_PATHMUSTEXIST, 
    "Project files (*.vprj)|*.vprj||");
  dlg.m_ofn.lpstrTitle  = (LPSTR) & buf;
  if( dlg.DoModal() != IDOK )
    return;
  CloseProject();
  strcpy(fullname, dlg.GetPathName());
  strcpy(filename, dlg.GetFileName());
  p = strstr(fullname, filename);
  if (p)
    *p = 0;
  ProjName = dlg.GetFileTitle();
  DirPath = fullname;
	if (DoOpenProject() == FALSE)
	{
		MessageBox(GetFocus(), "Unable to open Project File", "Open Project", MB_OK);
		ProjName = "";
	}
}

BOOL ValidateVAP(BOOL isnew, PROJHEADER* ph, int projectype)
{
	char sn[16];
//	int res;
	sn[0] = 0x00;

  gProjectLimited = FALSE;
	/* no check */
	gProjectType = projectype;
	return TRUE;

	/*
	if(projectype == PROJECT_TYPE_TE) 
		strcpy(sn, "TE-VAP");
	else if(projectype == PROJECT_TYPE_WIN) 
		strcpy(sn, "WIN-VAP");
	else if(projectype == PROJECT_TYPE_WEB) 
		strcpy(sn, "WEB-VAP");
	else if(gProjectType == PROJECT_TYPE_NONE) 
	{
		//MessageBox(GetFocus(), "This is a legacy project file and support is limited.  Please contact your software vendor for information regarding upgrading your project.", "AccuStudio Error", MB_OK);
		//gProjectLoadFailed = TRUE;
		//gProjectLoadFailed = TRUE;	
		return TRUE;		
	}
	else
	{
		MessageBox(GetFocus(), "Unable to open Project File.  Please contact your software vendor for assistance.", "AccuStudio Error", MB_OK);
		gProjectLoadFailed = TRUE; 
		return FALSE;
	}

	gProjectType = projectype;
	return TRUE;

	String^ mystring = gcnew String(sn);
	int oem = -1;
	res = AccuSpeechCommon::Validate::Validate::ValidateStudio(mystring, oem);
	if((res == 0))
	{			
		if((oem != -1) && (ph != NULL) && (oem == ph->OEMID))
		{
			if(bDontDisplayDemo == FALSE)
			{
				MessageBox(GetFocus(), "The license for this product is either not install or has expired.  AccuStudio will provide limited functionality until the license is renewed.  Please contact your software vendor for assistance.", "AccuStudio License Expired", MB_OK);
				bDontDisplayDemo = TRUE;
			}
			gProjectLimited = TRUE;
		}
		else
		{
			MessageBox(GetFocus(), "Unable to open Project File.  Please contact your software vendor for assistance.", "AccuStudio Error", MB_OK);
			gProjectLoadFailed = TRUE;
			return FALSE;
		}
	}		

	if(isnew == TRUE)
		gOEMID = oem & 0xFF;

	gProjectType = projectype;

	if(ph == NULL) //new project
	{
		if(bDontDisplayDemo == FALSE)
		{
			if(oem == OEM_ID_DEMO)
				MessageBox(GetFocus(), "Demo license installed!\r\n\r\nThank you for trying AccuStudio.  This license is for demo purposes only.\r\n\r\n***WARNING***\r\n\r\nThis license will only build grammars that run in demo mode when deployed.  This project cannot be used to create production grammars.  Any work done with a demo license will have to be re-done with a production license.", "AccuStudio Demo Version", MB_OK);
			
			bDontDisplayDemo = TRUE;
		}
	}
	else
	{
		if((oem == OEM_ID_DEMO) && (OEM_ID_DEMO == ph->OEMID))
		{
			if(bDontDisplayDemo == FALSE)
			{
				MessageBox(GetFocus(), "Demo license installed!\r\n\r\nThank you for trying AccuStudio.  This license is for demo purposes only.\r\n\r\n***WARNING***\r\n\r\nThis license will only build grammars that run in demo mode when deployed.  This project cannot be used to create production grammars.  Any work done with a demo license will have to be re-done with a production license.", "AccuStudio Demo Version", MB_OK);
				bDontDisplayDemo = TRUE;
			}
		}
		else if(ph->OEMID == OEM_ID_DEMO)
		{
			gOEMID = OEM_ID_DEMO;

			if(bDontDisplayDemo == FALSE)
			{
				MessageBox(GetFocus(), "Demo Project!\r\n\r\n***WARNING***\r\n\r\nThis project is for demonstration purposes only and will only build grammars that run in demo mode when deployed.", "AccuStudio Demo Project", MB_OK);
				bDontDisplayDemo = TRUE;
			}
		}
		else if(oem != ph->OEMID)
		{
			MessageBox(GetFocus(), "Unable to open Project File.  Please contact your software vendor for assistance.", "AccuStudio Error", MB_OK);
			gProjectLoadFailed = TRUE;
			return FALSE;
		}
		else
			gOEMID = ph->OEMID;

	}

	return TRUE;
	*/
}

BOOL DoOpenProject()
{
  int i;
  DWORD * poff;
	char * p;
  CString t = DirPath;
  bDontDisplayDemo = FALSE;
	CFileException cfe;

	if (gProjectLoadFailed == 2)
		gProjectLoadFailed = FALSE;
  t += ProjName;
	if (fProject.Open(t + ".vprj", CFile::modeRead | CFile::shareExclusive, &cfe) == 0)
  {
		if (cfe.m_cause == CFileException::sharingViolation)
		{
			MessageBox(NULL, "This project is open within another AccuStudio session.", "Opening Project",
				MB_OK);
			gProjectLoadFailed = 2;
		}
    return FALSE;
  }
  DWORD size = (DWORD) fProject.GetLength();

  ph = (PROJHEADER  * ) calloc(size, 1);
  fProject.Read(ph, size);
	if (ph->ControlSize <= 1)
	{ // older-style project
		ph->version |= ph->ControlSize<<8;
		if (ph->ControlSize == 1)
			ph->ControlSize = sizeof(OLDV1CONTROLRO);  // recent version 1
		else
			ph->ControlSize = sizeof(OLDCONTROLRO);	// very old version
	}
	Model = (Target_Models) ph->ModelID;
	Language = (Target_Languages) ph->LangID;
	NewLogic = ph->NewLogic;
  //gProjectType = ph->ProjectType;
  
  if(ValidateVAP(FALSE, ph, ph->ProjectType) == FALSE)
	{
		fProject.Close();
	  return FALSE;
	}
	WORD majorv = ph->version >> 8;
	WORD minorv = (ph->version>>4) & 0x0f;
	WORD cmajorv = VERSION >> 8;
	WORD cminorv = (VERSION>>4) & 0x0f;
	if (majorv < cmajorv || (majorv == cmajorv && minorv < cminorv))
	{
		i = MessageBox(GetFocus(), "This is a legacy project file.  Do you wish to upgrade the version?\r\n\r\nWarning! Upgrading the version will cause the new VBIN to be incompatible with older versions of the runtime.", "AccuStudio Warning", MB_YESNO);
		if (i == IDNO)
		{
			SaveVersion = ph->version;
		}
	}

  //ResName = ph->ResName;
  //ResPath = ph->ResPath;
	ProjSettings = ph->ProjSettings;
	Recognizer = (Recognizer_Type) ((ProjSettings & PROJ_REC_MSK) >> 1);
	Recognizer = NUANCE3200;  // only choice for now
  LPSTR pstr = (LPSTR) ph + ph->StringsOff;

  poff = (DWORD *) ((LPSTR)ph + ph->DefineOff);
  NumDefines = *poff++;  
  pdefines = (LPSTR *) calloc(NumDefines * sizeof(LPSTR), 1);
  for (i = 0; i < NumDefines; i++)
  {
    p = pstr + *poff++;
    pdefines[i] = (LPSTR) calloc(strlen(p) + 1, 1);
    strcpy(pdefines[i], p);
  }

  poff = (DWORD *) ((LPSTR)ph + ph->ValuesOff);  
  NumDefines = *poff++;  
  pvalues = (LPSTR *) calloc(NumDefines * sizeof(LPSTR), 1);
  for (i = 0; i < NumDefines; i++)
  {
    p = pstr + *poff++;
    pvalues[i] = (LPSTR) calloc(strlen(p) + 1, 1);
    strcpy(pvalues[i], p);
  }

  poff = (DWORD *) ((LPSTR)ph + ph->NamesOff);
  NumNames = *poff++;  
  pnames = (LPSTR *) calloc(NumNames * sizeof(LPSTR), 1);
  for (i = 0; i < NumNames; i++)
  {
    p = pstr + *poff++;
    pnames[i] = (LPSTR) calloc(strlen(p) + 1, 1);
    strcpy(pnames[i], p);
  }

  poff = (DWORD *) ((LPSTR)ph + ph->GramSetOff);
  NumGramSets = *poff++;  
  pgramsetnames = (LPSTR *) calloc(NumGramSets * sizeof(LPSTR), 1);
  for (i = 0; i < NumGramSets; i++)
  {
    p = pstr + *poff++;
    pgramsetnames[i] = (LPSTR) calloc(strlen(p) + 1, 1);
    strcpy(pgramsetnames[i], p);
  }

  poff = (DWORD *) ((LPSTR)ph + ph->GramOff);
  NumGrams = *poff++;  
  pgramnames = (LPSTR *) calloc(NumGrams * sizeof(LPSTR), 1);
	pgramparent = (BYTE *) calloc(NumGrams * sizeof(BYTE), 1);

  for (i = 0; i < NumGrams; i++)
  {
		DWORD gramoff = *poff & 0x00ffffff;
		BYTE parent = (BYTE) (*poff >> 24);
    p = pstr + gramoff; 
		poff++;
		pgramparent[i] = parent;
    pgramnames[i] = (LPSTR) calloc(strlen(p) + 1, 1);
    strcpy(pgramnames[i], p);
  }

  // Now Load in the Grammar Thread
  LPGRAMSETTHREAD lpgs, prevlpgs = NULL, tr = NULL;
  LPGRAMTHREAD lpg, prevlpg = NULL, trr = NULL;

  p = (LPSTR) ph + ph->GramListOff;
  if (ph->GramListOff)
  {
    while (TRUE)
    {
			int size = sizeof(GRAMSETTHREAD);
      lpgs = (LPGRAMSETTHREAD) calloc(size, 1);
      memcpy(lpgs, p, size);
      p += size;

      if (lpGramSetThread == NULL)
        lpGramSetThread = lpgs;

      if (prevlpgs)
        prevlpgs->next = lpgs;
      prevlpgs = lpgs;

      lpg = lpgs->pgrams;
      lpgs->pgrams = NULL;
      prevlpg = NULL;

      while (lpg)
      {
        lpg = (LPGRAMTHREAD) calloc(sizeof(GRAMTHREAD), 1);
        memcpy(lpg, p, sizeof(GRAMTHREAD));
        trr = lpg->next;
        lpg->next = NULL;
        if (lpgs->pgrams == NULL)
          lpgs->pgrams = lpg;
        if (prevlpg)
          prevlpg->next = lpg;
        prevlpg = lpg;
        p += sizeof(GRAMTHREAD);
        if (trr == NULL)
          break;
      }

      tr = lpgs->next;
      lpgs->next = NULL;
      if (tr == NULL)
        break;
    }
  }

  // Now load in the resource objects
  pdreshead = NULL;  
  p = (LPSTR) ph + ph->ObjOff;
	if (ph->ObjOff != ph->StringsOff)
		AddObjectThread(p, NULL);
    
  free(ph);
  //fProject.Close();
  ShowProjWindow();
  Enable(ID_FILE_SAVE_PROJECT);
  Enable(ID_FILE_CLOSE_PROJECT);
  Enable(ID_BUILD_BUILD_PROJECT);
  if (gProjectLimited == FALSE)
  {
	  Enable(ID_PROJECT_ADD_GRAMMARSET);
	  Enable(ID_PROJECT_ADDGRAMMARFILES);
	  Enable(ID_PROJECT_ADD_GRAMMAR);
		Enable(ID_PROJECT_SETTINGS);
		Enable(ID_PROJECT_UPDATEPROJECTFROMRESOURCEFILE32825);
		Enable(ID_PROJECT_ADDRESOURCESFROMRESOURCEFILE);
  }
	return TRUE;
}

UINT  StringOff;
LPSTR pSavedStrings;

UINT SaveString(LPSTR pname)
{
  int len = strlen(pname)+1;
  pSavedStrings = (LPSTR) ReAlloc(pSavedStrings, StringOff + len);
  strcpy(&pSavedStrings[StringOff], pname);
  UINT res = StringOff;
  StringOff += len;
  return res;
}

CFile fp;

void DumpGram(LPCSOGRAMMAR lpGram)
{
  while (lpGram) 
  {
    fp.Write(lpGram, sizeof(CSOGRAMMAR));
    lpGram = lpGram->lpNext;
  }
}
void DumpNode(LPDIALOGRO phead)
{   
  while (phead)
  {
    fp.Write(phead, sizeof(CONTROLRO));
    if (phead->controlthread)
      DumpNode(phead->controlthread);
    if (phead->lpGram)
      DumpGram(phead->lpGram);
    phead = phead->lpNext;
  }
}

BOOL MakeFileWritable(CString infile)
{
	TRY
	{
		CFileStatus status;
		BOOL bRet = CFile::GetStatus(infile ,status);
		if(bRet)
		{
			status.m_attribute = CFile::normal; // remove read-only attribute
			CFile::SetStatus(infile, status);
		}
	}
	CATCH(CFileException, e)
	{
		e->ReportError(); // see what's going wrong

		return FALSE;
	}
	END_CATCH

return TRUE;
}

void SaveProject()
{
  PROJHEADER ph = {0};
  int off, i;

  if (ProjName.GetLength() == 0)
    return;
  pSavedStrings = NULL;
  StringOff = 0;
	ph.OEMID = gOEMID;
    ph.ProjectType = gProjectType;
	ph.ProjSettings = ProjSettings;
	ph.LangID = (BYTE) Language;
	ph.ModelID = Model;
	ph.NewLogic = NewLogic;
  CString ts = DirPath + ProjName + ".vprj";
  CString t = DirPath + ProjName + ".vprj~";
	
  if (fp.Open(t, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary
      | CFile::shareExclusive) == 0)
  {
    MessageBox(GetFocus(), "Unable to create Project File", "Save Project", MB_OK);
    return;
  }
  
  CWaitCursor wait;

  ph.version = SaveVersion;
	ph.ControlSize = sizeof(CONTROLRO);

  //strcpy(ph.ResName, ResName);
  //strcpy(ph.ResPath, ResPath);
	off = sizeof(PROJHEADER);
  fp.Write(&ph, off);

  ph.DefineOff = (UINT) fp.GetPosition();
  fp.Write(&NumDefines, sizeof(UINT));
  for (i=0; i < NumDefines; i++)
  {
    off = SaveString(pdefines[i]);
    fp.Write(&off, sizeof(UINT));
  }

  ph.ValuesOff = (UINT) fp.GetPosition();
  fp.Write(&NumDefines, sizeof(UINT));
  for (i=0; i < NumDefines; i++)
  {
    off = SaveString(pvalues[i]);
    fp.Write(&off, sizeof(UINT));
  }

  ph.NamesOff = (UINT)fp.GetPosition();
  fp.Write(&NumNames, sizeof(UINT));
  for (i=0; i < NumNames; i++)
  {
    off = SaveString(pnames[i]);
    fp.Write(&off, sizeof(UINT));
  }

  ph.GramSetOff = (UINT)fp.GetPosition();
  fp.Write(&NumGramSets, sizeof(UINT));
  for (i=0; i < NumGramSets; i++)
  {
    off = SaveString(pgramsetnames[i]);
    fp.Write(&off, sizeof(UINT));
  }

  ph.GramOff = (UINT)fp.GetPosition();
  fp.Write(&NumGrams, sizeof(UINT));
  for (i=0; i < NumGrams; i++)
  {
    off = SaveString(pgramnames[i]);
		off |= (pgramparent[i] << 24);
    fp.Write(&off, sizeof(UINT));
  }

  if (lpGramSetThread)
    ph.GramListOff = (UINT)fp.GetPosition();     // Now dump the grammar thread

  LPGRAMSETTHREAD lpg = lpGramSetThread;
  LPGRAMTHREAD  lg;
  while (lpg)
  {
    fp.Write(lpg, sizeof(GRAMSETTHREAD));
    lg = lpg->pgrams;
    while(lg)
    {
      fp.Write(lg, sizeof(GRAMTHREAD));
      lg = lg->next;
    }
    lpg = lpg->next;
  }

  ph.ObjOff = (UINT)fp.GetPosition();     // Now dump the objects

  DumpNode(pdreshead);

  ph.StringsOff = (UINT)fp.GetPosition();

  fp.Write(pSavedStrings, StringOff);
  fp.Seek(0, CFile::begin);
  fp.Write(&ph, sizeof(PROJHEADER));

  free(pSavedStrings);
  fp.Close();
  PModified = FALSE;

	if (fProject.m_hFile != CFile::hFileNull)
		fProject.Close();	// remove lock on project file
  if(MakeFileWritable(ts) == FALSE)
	  return;
	DeleteFile(ts);
	rename(t, ts);
	if (fProject.Open(ts, CFile::modeRead | CFile::shareExclusive) == 0)
  {
    return;
  }
}

void DoFreeGram(LPCSOGRAMMAR pgram)
{
  LPCSOGRAMMAR t;

  while (pgram)
  {
    t = pgram->lpNext;
    free(pgram);
    pgram = t;
  }
}

void DoFree(LPDIALOGRO p)
{
  LPDIALOGRO t;

  while(p)
  {
    if (p->controlthread)
      DoFree(p->controlthread);
    if (p->lpGram)
      DoFreeGram(p->lpGram);
    t = p->lpNext;
    free(p);
    p = t;
  }
}

void CloseProject()
{
  int i;

	if (fProject.m_hFile != CFile::hFileNull)
		fProject.Close();
  if (PModified)
  {
    if (MessageBox(GetFocus(), "Do you wish to save your Project changes?",
      "Closing Project", MB_YESNO) == IDYES)
      SaveProject();
  }
  PModified = FALSE;
	SaveVersion = VERSION;

  if (pdoc)
  {
    pdoc->OnCloseDocument();
    pdoc = NULL;
  }

  if (poutdoc)
  {
    poutdoc->OnCloseDocument();
    poutdoc = NULL;
  }

  ProjName.Empty();
  if (NumNames)
  {
    for (i=0; i < NumNames; i++)
      free(pnames[i]);
    free(pnames);
    NumNames = 0;
  }
  if (NumDefines)
  {
    for (i=0; i < NumDefines; i++)
    {
      free(pdefines[i]);
      free(pvalues[i]);
    }
    free(pdefines);
    free(pvalues);
    NumDefines = 0;
  }
  if (NumGramSets)
  {
    for (i=0; i < NumGramSets; i++)
      free(pgramsetnames[i]);
    free(pgramsetnames);
    NumGramSets = 0;
  }
  if (NumGrams)
  {
    for (i=0; i < NumGrams; i++)
		{
      free(pgramnames[i]);
		}
    free(pgramnames);
		free(pgramparent);
    NumGrams = 0;
  }
  pnames = pdefines = pvalues = pgramsetnames = pgramnames =  NULL;
	pgramparent = NULL;
  DoFree(pdreshead);
  pdreshead = pdres = NULL;

  LPGRAMSETTHREAD gtp;
  while (lpGramSetThread)
  {
    LPGRAMTHREAD  pt, pgrams = lpGramSetThread->pgrams;

    while(pgrams)
    { 
      pt = pgrams->next;
      free(pgrams);
      pgrams = pt;
    }
    gtp = lpGramSetThread->next;
    free(lpGramSetThread);
    lpGramSetThread = gtp;
  }

  lpGramSetThread = NULL;

  Disable(ID_FILE_SAVE_PROJECT);
  Disable(ID_FILE_CLOSE_PROJECT);
  Disable(ID_BUILD_BUILD_PROJECT);
	Disable(ID_PROJECT_SETTINGS);
	Disable(ID_PROJECT_UPDATEPROJECTFROMRESOURCEFILE32825);
	Disable(ID_PROJECT_ADDRESOURCESFROMRESOURCEFILE);
  Disable(ID_VIEW_RESULTS);
  Disable(ID_VIEW_PROJECT);
  Disable(ID_VIEW_GRAMMAR_WIZARD);
  Disable(ID_GRAMMAR_EXERCISE);
  Disable(ID_VIEW_OBJECTWIZARD);
  Disable(ID_PROJECT_ADD_GRAMMARSET);
  Disable(ID_PROJECT_ADDGRAMMARFILES);
  Disable(ID_PROJECT_ADD_GRAMMAR);
}

//void Error(LPSTR perr)
//{
//  MessageBox(GetFocus(),perr,"New Project",  MB_OK);
//}

BOOL DoNewProject()
{
	bDontDisplayDemo = FALSE;
  CString t = ProjName + "GS";
  NewGramSetName(t.GetBuffer(0));
  AddGramSetName(0);  
  if(LoadRCFile(TRUE, ResPath, ResName) == FALSE)
	  return FALSE;

	AddDefaultGrammars(gProjectType);
  SaveProject();
	BuildProject(TRUE, DirPath, ProjName);
  Enable(ID_FILE_SAVE_PROJECT);
  Enable(ID_FILE_CLOSE_PROJECT);
  Enable(ID_BUILD_BUILD_PROJECT);
  
  if (gProjectLimited == FALSE)
  {
		Enable(ID_PROJECT_SETTINGS);
		Enable(ID_PROJECT_UPDATEPROJECTFROMRESOURCEFILE32825);
		Enable(ID_PROJECT_ADDRESOURCESFROMRESOURCEFILE);
		Enable(ID_PROJECT_ADD_GRAMMARSET);
		Enable(ID_PROJECT_ADDGRAMMARFILES);
		Enable(ID_PROJECT_ADD_GRAMMAR);
  }

  return TRUE;
}

void NewProject(void)
{
  CDlgNewProject * proj = new CDlgNewProject;
  CFile pf;

	CloseProject();
	ProjSettings = (Recognizer << 1);
redo: 
  if (proj->DoModal() != IDOK)
    return;

  ProjName = proj->m_projname;
	if (ProjName.GetLength() > MAXGRAMSETNAME - 2)
	{
		char s[256];
		sprintf(s, "Project names must be %i characters or less", MAXGRAMSETNAME - 2);
		MessageBox(GetFocus(), s, 
			"New Project: " + ProjName, MB_OK);
		goto redo;
	}
  DirPath = proj->m_projpath;
  ResPath = proj->m_respath;  
  ResName = proj->m_resname;
  delete proj;

	DoNewProject();
}

// Routines for loading in an RC file

void * ReAlloc(void * p, int size)
{
  if (p == NULL)
    return calloc(1, size);
  return realloc(p, size);
}

// Grammar retrieval routines

int FindGramSetName(LPSTR pname)
{
  for (int i = 0; i < NumGramSets; i++)
    if (strcmp(pgramsetnames[i], pname) == 0)
      return i;
  return -1;
}

UINT NewGramSetName(char * pname)
{
  pgramsetnames = (char * *)ReAlloc(pgramsetnames, (NumGramSets+1)*sizeof(LPSTR));
  pgramsetnames[NumGramSets] = (char *)malloc(strlen(pname) + 1);
  strcpy(pgramsetnames[NumGramSets], pname);
  return NumGramSets++;
}

int GetGramSetIndex(LPSTR pname)
{
  int i = FindGramSetName(pname);
  if (i < 0)
    return NewGramSetName(pname);
  return i;
}

//

int FindGramName(LPSTR pname)
{
  for (int i = 0; i < NumGrams; i++)
    if (strcmp(pgramnames[i], pname) == 0)
      return i;
  return -1;
}

UINT NewGramName(char * pname)
{
  pgramnames = (char * *)ReAlloc(pgramnames, (NumGrams+1)*sizeof(LPSTR));
  pgramparent = (BYTE *)ReAlloc(pgramparent, (NumGrams+1)*sizeof(BYTE));
  pgramatts = (BYTE *)ReAlloc(pgramatts, (NumGrams+1)*sizeof(BYTE));
  pgramnames[NumGrams] = (char *)malloc(strlen(pname) + 1);
	pgramparent[NumGrams] = 0;
  strcpy(pgramnames[NumGrams], pname);
  return NumGrams++;
}

int GetGramIndex(LPSTR pname)
{
  int i = FindGramName(pname);
  if (i < 0)
    return NewGramName(pname);
  return i;
}

int AddGramName(int i, LPSTR pname)
{
  LPGRAMSETTHREAD lpgs = lpGramSetThread;
  LPGRAMTHREAD nlpg, lpg, prevlpg = NULL;

  while (lpgs && lpgs->ID != (UINT)i)
    lpgs = lpgs->next;
  if (lpgs == NULL)
    return -1;
  lpg = lpgs->pgrams;
  int j = GetGramIndex(pname);
  if (j < 0)
    return FALSE;
  while (lpg)
  {
    if (lpg->ID == (UINT)j)
      return j;
    prevlpg = lpg;
    lpg = lpg->next;
  }
  PModified = TRUE;
  nlpg = (LPGRAMTHREAD) calloc(sizeof(GRAMTHREAD), 1);
  nlpg->ID = j;
  if (prevlpg)
    prevlpg->next = nlpg;
  else
    lpgs->pgrams = nlpg;
  return j;
}


int FindName(LPSTR ptoken)
{
  for (int i = 0; i < NumNames; i++)
    if (strcmp(pnames[i], ptoken) == 0)
      return i;
  return -1;
}


UINT AddName(char * pname)
{
  pnames = (char * *)ReAlloc(pnames, (NumNames+1)*sizeof(LPSTR));
  pnames[NumNames] = (char *)malloc(strlen(pname) + 1);
  strcpy(pnames[NumNames], pname);
  return NumNames++;
}

int FindDefine(LPSTR ptoken)
{
  for (int i = 0; i < NumDefines; i++)
    if (strcmp(pdefines[i], ptoken) == 0)
      return i;
  return -1;
}

int FindDefineValue(LPSTR ptoken)
{
  for (int i = 0; i < NumDefines; i++)
    if (strcmp(pvalues[i], ptoken) == 0)
      return i;
  return -1;
}

void AddDefine(char * pname, char * pvalue)
{
  pdefines = (char * *)ReAlloc(pdefines, (NumDefines+1)*sizeof(LPSTR));
  pdefines[NumDefines] = (char *)malloc(strlen(pname) + 1);
  strcpy(pdefines[NumDefines], pname);
  pvalues = (char * *)ReAlloc(pvalues, (NumDefines+1)*sizeof(LPSTR));
  pvalues[NumDefines] = (char *)malloc(strlen(pvalue) + 1);
  strcpy(pvalues[NumDefines++], pvalue);
}

extern BOOL IsSysInclude;

void DoDefine()
{
  char defname[256];
  int i = 0;

  if (GetAlpha() == FALSE)
    return;
  strcpy(defname, Token);
  int index = FindDefine(Token);

  if (*pLineBuf == 0)
    return;
  SkipWhiteSpace();

  Token[i++] = CurChar;
  while (*pLineBuf && !WhiteSpace(*pLineBuf))
  {
    Token[i++] = *pLineBuf++;
  }
  Token[i] = 0;
  if (*pLineBuf)
    CurChar = ' ';
  else 
    CurChar = 0;
  if (isdigit(Token[0]) || (Token[0] == '(' && Token[1] == '-'))
    if (index < 0)
    {
      AddDefine(defname, Token);
      if ( plevel == 0 || IsSysInclude == FALSE)
      {
        GetDefVal(defname);      // user include symbols are always included
      }
    }
}

BOOL Declarative()
{
  BOOL Result = TRUE;
  if (CurChar != '#')
    return FALSE;
  GetChar();
  if (GetAlpha() == FALSE)
    return FALSE;
  if (strcmp(Token, "include") == 0)
  {
    DoInclude();
    Result = 2;
  }
  else if (strcmp(Token, "define") == 0)
    DoDefine();
	else if (strcmp(Token, "export") == 0)
	{
    SkipLine();		// no compile code for an export function
	}
  else if (InHFile)    // ignore other commands in a h file
  {
    SkipLine();
  }
  else
    return FALSE;
  return Result;
}

BOOL Translate()
{
  int i;
  
  if (isdigit(Token[0]))
    return TRUE;
  i = FindDefine(Token);
  if (i >= 0)
  {
    strcpy(Token, pvalues[i]);
    return TRUE;
  }
  return FALSE;
}

int GetDefVal(LPSTR ptoken)
{
  LPSTR t;

  int i = FindDefine(ptoken);
  if (i < 0)
  {
    MessageBox(GetFocus(), Token, "Missing Define Name", MB_OK);
    return 0;
  }
  if (i < NumReferences)
    return i;
  t = pdefines[i];
  pdefines[i] = pdefines[NumReferences];
  pdefines[NumReferences] = t;

  t = pvalues[i];
  pvalues[i] = pvalues[NumReferences];
  pvalues[NumReferences] = t;

  ++NumReferences;
  return NumReferences-1;
}


int GetCaption(BOOL strip)
{
  LPSTR p;

  if (GetQuoted('"') == FALSE)
    if (GetNum() < 0)
      return FALSE;
  if (strip)
    while(p = strchr(Token, '&'))
    {
      strcpy(p, p+1);  
    }
  int i = FindName(Token);
  if (i < 0)
    return AddName(Token);
  return i;
}

int GetClass()
{
  return GetCaption(TRUE);
}

void SkipParam()
{
  while (CurChar && CurChar != EOD && CurChar != ',')
    GetChar();
  if (CurChar == ',')
    GetChar();
}

void GetCoords(LPDIALOGRO pdres)
{
  pdres->Coords[0] = GetNum();
  SkipWhiteSpace();
  Find(",");
  SkipWhiteSpace();
  pdres->Coords[1] = GetNum();
  SkipWhiteSpace();
  Find(",");
  SkipWhiteSpace();
  pdres->Coords[2] = GetNum();
  SkipWhiteSpace();
  Find(",");
  SkipWhiteSpace();
  pdres->Coords[3] = GetNum();
}

LONG InclVal(LPSTR Token)
{
  LONG r = 0;
  int i;

  if (Token[0] == '0' && Token[1] == 'x')
  {
    for (i=2; ; i++)
    {
      if (Token[i] == 0 || Token[i] == 'L')
        break;
      if (isdigit(Token[i]))
        r = (r<<4) | (Token[i] - '0');
      else if (Token[i] >= 'A' && Token[i] <= 'F')
        r = (r<<4) | (Token[i] - 'A' + 10);
      else if (Token[i] >= 'a' && Token[i] <= 'f')
        r = (r<<4) | (Token[i] - 'a' + 10);
      else
        break;
    }
    return r;
  }
  else
    return atoi(Token);

}

BOOL GetStyle(LPCONTROLRO pcres)
{
	BOOL rc = FALSE;
  pcres->NotStyle = pcres->Style = 0;
  while (TRUE)
  {
    SkipWhiteSpace();
    GetAlpha();
    SkipWhiteSpace();
    if (strcmp(Token,"NOT") == 0)
    {
      SkipWhiteSpace();
      GetAlpha();
      if (Translate())
			{
        pcres->NotStyle |= InclVal(Token);
				rc = TRUE;
			}
      SkipWhiteSpace();
    }
    else if (Translate())
		{
      pcres->Style |= InclVal(Token);
			rc = TRUE;
		}
    if (CurChar != '|')
      break;
    GetChar();
  };
	return rc;
}


BOOL IsControl()
{
  int i,j, rc, classmap = FALSE;
  LPCONTROLRO pcres;
  LPCONTROLRO tc;       
	int lbstyle;

  if (pdres == NULL)
    return FALSE;
  if (GetAlpha() == FALSE)
    return FALSE;
  for (i=0; i < NUMCONTROLTYPES; i++)
    if (strcmp(VControl[i].ControlName, Token)==0)
      break;
  if (i >= NUMCONTROLTYPES || i == C_ICON)
    return FALSE;
  pcres = (LPCONTROLRO) calloc(sizeof(CONTROLRO), 1);
  pcres->ControlType = i;
  pcres->Caption = pcres->ClassName = -1;

  SkipWhiteSpace();
  switch (i)
  {
    case C_ICON:
      SkipLine();
      break;

    case C_CONTROL:    // CONTROL
      pcres->Caption = GetCaption(TRUE);       // Caption
      SkipWhiteSpace();
      Find(",");
      SkipWhiteSpace();

      GetAlpha();         // Control ID
      pcres->ControlIDOrd = GetDefVal(Token);
      SkipWhiteSpace();
      Find(",");
      SkipWhiteSpace();

      pcres->ClassName = GetClass();         // Control Class Name

      
      for (j=0; j < NumControlClasses; j++)
        if (strcmp(ControlClasses[j], Token) == 0)
          break;
      if (j < NumControlClasses)
			{
        pcres->ControlType = ControlTypes[j];
				if (j)
					classmap = TRUE;
			}

      SkipWhiteSpace();
      Find(",");
      SkipWhiteSpace();

      rc = GetStyle(pcres);    // Control Style			
			lbstyle = pcres->Style & 0xffff;
			if (rc && !classmap)
				switch (lbstyle)
				{
					case BS_PUSHBUTTON:
					case BS_USERBUTTON:
					case BS_OWNERDRAW:
					case BS_LEFTTEXT:
						pcres->ControlType = C_PUSHBUTTON;
						break;
					case BS_DEFPUSHBUTTON:
						pcres->ControlType = C_DEFPUSHBUTTON;
						break;
					case BS_CHECKBOX:
					case BS_AUTOCHECKBOX:
					case BS_3STATE:
					case BS_AUTO3STATE:
						pcres->ControlType = C_CHECKBOX;
						break;
					case BS_RADIOBUTTON:
					case BS_AUTORADIOBUTTON:
						pcres->ControlType = C_RADIOBUTTON;
							break;
					default:
						break;
				}

      SkipWhiteSpace();
      Find(",");
      SkipWhiteSpace();

      GetCoords(pcres);     // Control Coordinates
      break;

    case C_GROUPBOX:
				i = i;

    case C_PUSHBUTTON:      // PUSHBUTTON
    case C_DEFPUSHBUTTON:   // DEFPUSHBUTTON
		case C_RADIOBUTTON:			// RADIOBUTTON
    case C_LTEXT:           // LTEXT
    case C_RTEXT:           // RTEXT
    case C_CTEXT:           // CTEXT

      pcres->Caption = GetCaption(TRUE);       // Caption
      SkipWhiteSpace();
      Find(",");
      SkipWhiteSpace();

    default:
      GetAlpha();
      pcres->ControlIDOrd = GetDefVal(Token);
      SkipWhiteSpace();
      Find(",");
      SkipWhiteSpace();

      GetCoords(pcres);
      SkipWhiteSpace();
      if (CurChar == ',')
      {
        Find(",");
        SkipWhiteSpace();
        GetStyle(pcres);    // Control Style
      }
      break;
 }

  tc = pdres->controlthread;
  if (tc == NULL)
    pdres->controlthread = pcres;
  else
  {
    while (tc->lpNext)
      tc = tc->lpNext;
    tc->lpNext = pcres;
  } 

  return TRUE;
}

void LinkOuter(LPDIALOGRO &pdreshead, LPDIALOGRO pdres)
{
  LPDIALOGRO pdt;
  if (pdreshead == NULL)
    pdreshead = pdres;
  else
  {
    pdt = pdreshead;
    while (pdt->lpNext)
      pdt = pdt->lpNext;
    pdt->lpNext = pdres;
  }
}

BOOL DoMenuBlock(LPDIALOGRO phead)
{
  LPDIALOGRO psub, prev = NULL;

  SkipWhiteSpace();
  if (Find("BEGIN") == FALSE)
    return FALSE;
  while (CurChar != EOD)
  {
    SkipWhiteSpace();
    if (GetAlpha() == FALSE)
      break;
    if (strcmp(Token, "POPUP") == 0)
    {
      SkipWhiteSpace();
      psub = (LPDIALOGRO) calloc(sizeof(DIALOGRO), 1);
      psub->ClassName = psub->Caption = -1;
      psub->ControlType = C_POPUP;
      psub->Caption = GetCaption(FALSE);
      psub->ControlIDOrd = -1;
      if (phead)
        phead->controlthread = psub;
      phead = NULL;
      if (prev)
        prev->lpNext = psub;
      prev = psub;
      DoMenuBlock(psub);
    }
    else if (strcmp(Token, "END") == 0)
      break;
    else if (strcmp(Token, "MENUITEM") == 0)
    {
      SkipWhiteSpace();
      if (CurChar != '"')
      {
        Find("SEPARATOR");
        continue;   // ignore a SEPARATOR
      }
      psub = (LPDIALOGRO) calloc(sizeof(DIALOGRO), 1);
      psub->ClassName = psub->Caption = -1;
      psub->ControlType = C_MENUITEM;
      psub->Caption = GetCaption(FALSE);
      SkipWhiteSpace();
      Find(",");
      SkipWhiteSpace();
      GetAlpha();
      psub->ControlIDOrd = GetDefVal(Token);
      SkipLine();
      if (phead)
        phead->controlthread = psub;
      phead = NULL;
      if (prev)
        prev->lpNext = psub;
      prev = psub;
    }
  }
  return TRUE;
}

BOOL DoMenuDef()
{  
  UINT id;
	LPDIALOGRO pmres;

  if (GetAlpha() == FALSE)
    return FALSE;
  id = GetDefVal(Token);
  SkipWhiteSpace();
  if (GetAlpha() == FALSE)
    return FALSE;
  if (strcmp(Token, "MENU"))
    return FALSE;
  SkipLine();

  pmres = (LPDIALOGRO) calloc(sizeof(DIALOGRO), 1);
  pmres->ControlIDOrd = id;
  pmres->Caption = pmres->ClassName = -1;
  pmres->ControlType = C_MENU;

	if (pdres)
	{
		LPDIALOGRO tc = pdres->controlthread;
		if (tc == NULL)
			pdres->controlthread = pmres;
		else
		{
			while (tc->lpNext)
				tc = tc->lpNext;
			tc->lpNext = pmres;
		} 
	}
	else
		LinkOuter(pdreshead,pmres);
  SkipLine();
  DoMenuBlock(pmres);

  return TRUE;
}

BOOL DoDialogDef(int ctype, LPSTR pname)
{
  UINT id;

  if (GetAlpha() == FALSE)
    return FALSE;
  id = GetDefVal(Token);
  SkipWhiteSpace();
  if (GetAlpha() == FALSE)
    return FALSE;
  if (strcmp(Token, pname))
    return FALSE;

  pdres = (LPDIALOGRO) calloc(sizeof(DIALOGRO), 1);
  pdres->ControlIDOrd = id;
  pdres->Caption = pdres->ClassName = -1;
  pdres->ControlType = ctype;
  LinkOuter(pdreshead,pdres);

  while (CurChar && !isdigit(CurChar))
    GetChar();

  GetCoords(pdres);
  SkipLine();

  while (TRUE)
  {
    SkipWhiteSpace();
    GetAlpha();
    if (strcmp(Token, "BEGIN") == 0)
      break;
    if (strcmp(Token, "CAPTION") == 0)
    {
      SkipWhiteSpace();
      pdres->Caption = GetCaption(TRUE);       // Caption
    }
    else if (strcmp(Token, "STYLE") == 0)
    {
      SkipWhiteSpace();
      GetStyle(pdres);
    }
    else
      SkipLine();
  }
  return TRUE;
}

void LinkControl(LPCONTROLRO pcres)
{
  LPCONTROLRO tc = pdres->controlthread;
  if (tc == NULL)
    pdres->controlthread = pcres;
  else
  {
    while (tc->lpNext)
      tc = tc->lpNext;
    tc->lpNext = pcres;
  } 
}

int XPIXELS = 8;
int YPIXELS = 8;
int SCREENWIDTH = 128;
int SCREENHEIGHT = 128;

void CreateNode(LPSTR sv, LPDIALOGRO pdres)
{
		sprintf(Token, "DID_%s", sv);
		AddDefine(Token, sv);
		pdres->ControlIDOrd = GetDefVal(Token);
		pdres->Caption = pdres->ClassName = -1;
		pdres->ControlType = C_DIALOG;
		pdres->Coords[0] = 0;
		pdres->Coords[1] = 0;
 		pdres->Coords[2] = SCREENWIDTH;
		pdres->Coords[3] = SCREENHEIGHT;
  	LinkOuter(pdreshead,pdres);
}

void CreateNodeEx(LPSTR sv, LPSTR svp, LPDIALOGRO pdres)
{
		sprintf(Token, "DID_%s", sv);
		int found = FindDefine(Token);
		if (found >= 0)
		{
			// Screen already exists - update its ID value
			if (atoi(pvalues[found])!= atoi(svp))
			{
				free(pvalues[found]);
				pvalues[found] = strdup(svp);
			}
		}
		else
			AddDefine(Token, svp);
		pdres->ControlIDOrd = GetDefVal(Token);
		pdres->Caption = pdres->ClassName = -1;
		pdres->ControlType = C_DIALOG;
		pdres->Coords[0] = 0;
		pdres->Coords[1] = 0;
 		pdres->Coords[2] = SCREENWIDTH;
		pdres->Coords[3] = SCREENHEIGHT;
  	LinkOuter(pdreshead,pdres);
}

BOOL LoadXMLFile(BOOL newprj, CString rpath, CString rname)
{
	BOOL isTE = FALSE;
	int i, j, v, rc = FALSE;
	char sv[256], sn[256];
	CString fn = rpath + rname;
//	String^ mystring;

	gProjectType = PROJECT_TYPE_NONE; 

	XML* x = 0;
	XMLVariable * pv;
  LPCONTROLRO pcres = NULL;
	pdres = NULL;

	x = new XML(fn.GetBuffer(0));
	// Parse status check
	int iPS = x->ParseStatus(); // 0 OK , 1 Header warning (not fatal) , 2 Error in parse (fatal)
	bool iTT = x->IntegrityTest(); // TRUE OK
	if (iPS == 2 || iTT == false)
	{
		MessageBox(GetFocus(), "Cannot open XML File", "New Project", MB_OK);
		delete x;
		return FALSE;
	}
	if (newprj)
		NumReferences = 0;
	XMLElement* r = x->GetRootElement();

	pv = r->FindVariableZ("Type");
	if (pv)
	{
		v = pv->GetValue(sv);
		sn[0] = 0x00;
		sprintf(sn, "%s-VAP", sv);
//		mystring = gcnew String(sn);
		
		// Project type is Web or Form based 
		XPIXELS = 1;
		YPIXELS = 1;
		
		if (strcmp(sv,"TE") == 0)
		{// Project type is Web or Form based
			gProjectType = PROJECT_TYPE_TE;
			isTE = TRUE;
			// Initialize Pixel variables for TE interface (default)
			XPIXELS = 4;
			YPIXELS = 8;
			SCREENWIDTH = 80*4;
			SCREENHEIGHT = 24*8;
		}
		else if (strcmp(sv,"WEB") == 0)
			gProjectType = PROJECT_TYPE_WEB;
		else if (strcmp(sv,"WIN") == 0)
			gProjectType = PROJECT_TYPE_WIN;
		else
		{
			MessageBox(GetFocus(), "Unable to load file.  Please contact your software vendor for assistance.", "AccuStudio Error", MB_OK);
			return FALSE;
		}

		if(ValidateVAP(newprj, NULL, gProjectType) == FALSE)
			return FALSE;
		//CharToUni(wc, 16, sn, strlen(sn));
	}
	else
	{
		MessageBox(GetFocus(), "Unable to load file.  Please contact your software vendor for assistance.", "AccuStudio Error", MB_OK);
		return FALSE;
	}


	int nC = r->GetChildrenNum();
	for (i = 0 ; i < nC ; i++)
	{
		XMLElement* ch = r->GetChildren()[i];

	// Get screen length and height, if any
		pv = ch->FindVariableZ("Length");
		if (pv)
			SCREENWIDTH = pv->GetValueInt();
		pv = ch->FindVariableZ("Height");
		if (pv)
			SCREENHEIGHT = pv->GetValueInt();
		//if (isTE)
		//{
		//	SCREENWIDTH *= XPIXELS;
		//	SCREENHEIGHT *= YPIXELS;
		//}
// Create Dialog
		pdres = (LPDIALOGRO) calloc(1,sizeof(DIALOGRO));
		pv = ch->FindVariableZ("ID");
		if (!pv)
			goto badxml;
		v = pv->GetValue(sv);
		
		pcres = (LPCONTROLRO) calloc(sizeof(CONTROLRO), 1);
		pcres->ControlType = C_LTEXT;
		
		pv = ch->FindVariableZ("Name");
		if (!pv)
			goto badxml;
		
		v = pv->GetValue(sn);

		CreateNodeEx(sn, sv, pdres);

		if(isTE == TRUE)
		{
			//CreateNode(sn, pdres);

			pv = ch->FindVariableZ("SearchText");
			if (!pv)
				goto badxml;
		// Build a text control for display purposes
			v = pv->GetValue(sv);
			v = FindName(sv);
			if (v < 0)
				v = AddName(sv);

			pcres->Caption = v;
			pcres->ClassName = -1;

			pv = ch->FindVariableZ("X");
			if (!pv)
				goto badxml;
			pcres->Coords[0] = pv->GetValueInt()*XPIXELS;
			pv = ch->FindVariableZ("Y");
			if (!pv)
				goto badxml;
			pcres->Coords[1] = pv->GetValueInt()*YPIXELS;
			pcres->Coords[2] = strlen(sv) * XPIXELS;
			pcres->Coords[3] = YPIXELS;
			LinkControl(pcres);
		}
		//else
		//{
		//	pv = ch->FindVariableZ("Name");
		//	if (!pv)
		//		goto badxml;
		//	
		//	v = pv->GetValue(sn);
		//	CreateNodeEx(sn, sv, pdres);
		//}

		// Now process each field
		XMLElement* fieldelem;
		int xloc, yloc, id, ftype, flen, fheight = YPIXELS;
		for(j = 0 ; j < (int)ch->GetChildrenNum() ; j++)
		{
			fieldelem = ch->GetChildren()[j];
			// Build a field resource
			pv = fieldelem->FindVariableZ("X");
			if (!pv)
				goto badxml;
			xloc = pv->GetValueInt();
			pv = fieldelem->FindVariableZ("Y");
			if (!pv)
				goto badxml;
			yloc = pv->GetValueInt();
			pv = fieldelem->FindVariableZ("ID");
			if (!pv)
				goto badxml;
			id = pv->GetValue(Token); 
			pv = fieldelem->FindVariableZ("Type");
			if (!pv)
				goto badxml;
			ftype = pv->GetValueInt();
			pv = fieldelem->FindVariableZ("Length"); 
			if (!pv)
				goto badxml;
			flen = pv->GetValueInt();
			pv = fieldelem->FindVariableZ("Height");
			if (pv)
				fheight = pv->GetValueInt();

			pcres = (LPCONTROLRO) calloc(sizeof(CONTROLRO), 1); 
			pcres->Caption = -1;
			pcres->ClassName = -1;
			pv = fieldelem->FindVariableZ("Name");
			if (!pv)
				goto badxml;
			v = pv->GetValue(sn);
			if (sn[0] == 0)
			{
				free(pcres);
				continue;
			}

			v = FindName(sn);
			if (v < 0)
				v = AddName(sn);
			pcres->Caption = v;

			pv = fieldelem->FindVariableZ("ID");
			v = pv->GetValue(sv);

			//if(isTE == TRUE)
			//{				
			//	sprintf(Token, "CID_%s", sv);
			//	AddDefine(Token, sv);
			//}
			//else
			//{
				
				sprintf(Token, "CID_%s", sn);
				AddDefine(Token, sv);
			//}

			pcres->ControlIDOrd = GetDefVal(Token);

			if (gProjectType == PROJECT_TYPE_TE && ftype == C_CTEXT)
			{
				ftype = C_EDITTEXT;	// mark as read-only field and inhibit grammar creation
				pcres->Style |= ES_READONLY;
			}
			pcres->ControlType = (VCONTROLTYPE)ftype;
			pcres->Coords[0] = xloc*XPIXELS;
			pcres->Coords[1] = yloc*YPIXELS;
			if(ftype == C_CHECKBOX)
			{
				pcres->Coords[2] = (flen* XPIXELS) + (strlen(sn)* 4);
				pcres->Coords[3] = fheight;
			}
			else
			{
				pcres->Coords[2] = (flen* XPIXELS);
				pcres->Coords[3] = fheight;
			}
			LinkControl(pcres);
		}
	}
	NumDefines = NumReferences;	 
	if (newprj)
		ShowProjWindow();
	rc = TRUE;
badxml:
	delete x;
	return rc;
}

void CreateInclude()
{
	CString pin = DirPath + ProjName + (CString)"_res.h";
	
  if(MakeFileWritable(pin) == FALSE)
	  return;

	FILE * pinclude = fopen(pin, "w");
	if(pinclude != NULL)
	{
		for (int i=0; i < NumDefines; i++)
			fprintf(pinclude, "#define %s %s\n", pdefines[i], pvalues[i]);
		fclose(pinclude);
	}
	else
		MessageBox(GetFocus(), "Error! Some of the project files are either missing or read only.  Make sure all of the project files are writable before running AccuStudio.", NULL, MB_OK);
}

BOOL LoadRCFile(BOOL newprj, CString rpath, CString rname)
{
	BOOL isxml = FALSE;
  CWaitCursor wait;
	CString tc;

	gProjectType = PROJECT_TYPE_NONE;
	rname.MakeLower();
	if (strstr(rname.GetBuffer(0), ".xml"))
	{
		isxml= LoadXMLFile(newprj, rpath, rname);
		if(isxml == FALSE)
			return FALSE;

		goto process;
	}
	if(ValidateVAP(newprj, NULL, PROJECT_TYPE_WIN) == FALSE)
			return FALSE;
  int i;
  tc = ExecDir;
  tc += "WindowDefs.h"; //"winuser.h";

	if (rpath.GetLength())
		if (OpenParser(rpath, rname) == FALSE)
		{
			MessageBox(GetFocus(), "Cannot open Resource File", "New Project", MB_OK);
			return FALSE;
		}
	if (newprj)
		NumReferences = 0;

  IsSysInclude = TRUE;
	plevel = 0;
  OpenInclude(tc);
  while (TRUE)
  {
    SkipWhiteSpace();
    if (CurChar == EOD)
      break;
    if (Declarative())
      continue;
    if (strstr(pLineBuf, " DIALOG "))
    {
      if (DoDialogDef(C_DIALOG, "DIALOG"))
        continue;
    }
    else if (strstr(pLineBuf, " DIALOGEX "))
    {
      if (DoDialogDef(C_DIALOG, "DIALOGEX"))
        continue;
    }
    else if (strstr(pLineBuf, " MENU "))
    {
      if (DoMenuDef())
        continue;
    }
    else if (IsControl())
      continue;

    if (pdres && (strcmp(Token, "END") == 0))
      pdres = NULL; 
    SkipLine();
  }
  while (CurChar != EOD)
    GetChar();

  CloseParser();

process:
	for (i=NumReferences; i < NumDefines; i++)    // get rid of unreferences defines
	{
		free(pdefines[i]);
		free(pvalues[i]);
	}
	NumDefines = NumReferences;
	 
	if (newprj)
		ShowProjWindow();
	CreateInclude();
	return TRUE;
}

BOOL AddGramSetName(int i)
{
  LPGRAMSETTHREAD oldt=NULL,t,pg;
  
  t = lpGramSetThread;
  while (t)
  {
    if (t->ID == (UINT)i)
      return FALSE;
    oldt = t;
    t = t->next;
  }
  pg = (LPGRAMSETTHREAD) calloc(sizeof(GRAMSETTHREAD), 1);
  pg->ID = i;
  if (oldt)
    oldt->next = pg;
  if ( t == lpGramSetThread)
    lpGramSetThread = pg;        // first entry
  PModified = TRUE;
  ShowProjWindow();
  return TRUE;
}
void AddGrammarFiles()
{
  CAddProjFiles ap;
 
  if (lpGramSetThread == NULL)
  {
    MessageBox(GetFocus(),"No Grammar Sets have been defined.", 
      "Add Grammar Files", MB_OK);
    return;
  }

  if (ap.DoModal() == IDOK)
  {

  }
}

void AddGrammarset() 
{
  CGramSetName gn;

  if (ProjName.GetLength() == 0)
    return;
redo:
  if (gn.DoModal() != IDOK)
    return;
	if (gn.m_name.GetLength() > MAXGRAMSETNAME - 2)
	{
		char s[256];
		sprintf(s, "Grammar set names must be %i characters or less", MAXGRAMSETNAME - 2);
		MessageBox(GetFocus(), s, 
			"New Grammar Set", MB_OK);
		goto redo;
	}
  int i = GetGramSetIndex(gn.m_name.GetBuffer(0));
  if (AddGramSetName(i) == FALSE)
  {
    MessageBox(GetFocus(),"Duplicate Grammar Set Name", 
      "New Grammar Set", MB_OK);
  }
}

void DumpGrams(CFile &pf, LPDIALOGRO pres)
{
  char str[256];
  int i = 0;
  char ch;

  if (pres->lpGram)
  { 
    LPCSOGRAMMAR tg = pres->lpGram; 
    sprintf(str, ", Grammars (");
    pf.Write(str, strlen(str));
    while(tg)
    {
      if (i++ == 0)
        ch = ' ';
      else
        ch = ',';
      sprintf(str, "%c%s:%s", ch, pgramsetnames[tg->nGrammarSetOrdinal], 
        pgramnames[tg->nGrammarOrdinal]);
      pf.Write(str, strlen(str));
      tg = tg->lpNext;
    }
    sprintf(str, ")");
    pf.Write(str, strlen(str));
  }
}

BOOL BuildDefFile()
{
  char str[256];
  LPGRAMSETTHREAD pgramset = lpGramSetThread;
  LPDIALOGRO pres = pdreshead;
  LPCONTROLRO pcon;
  LPGRAMTHREAD pgram;
  CString name = DirPath + ProjName + ".vdef~";
  CString newname = DirPath + ProjName + ".vdef";
  int NumGrammars = 0;
  char first;
  int i = 0;
  char * incmsg = "\n// Speech Object Definitions\n\n";

  if(MakeFileWritable(newname) == FALSE)
	  return FALSE;

  CFile pf;
  if (pf.Open(name, CFile::modeCreate  | CFile::modeWrite) == 0)
  {
    MessageBox(GetFocus(),"Unable to create project 'vdef' file.  The file maybe read only.", "Project Build", MB_OK);
		return FALSE;
  }

  while (pgramset)
  {
    sprintf(str, "GrammarSet %s(\n", pgramsetnames[pgramset->ID]);
    pf.Write(str, strlen(str));
    pgram = pgramset->pgrams;
    first = ' ';
    while (pgram)
    {
      ++NumGrammars;
      sprintf(str, "%c%s", first, pgramnames[pgram->ID]);
      pf.Write(str, strlen(str));
      if (i++ == 8)
      {
        i = 0;
        pf.Write("\n", 1);
      }
      first = ',';
      pgram = pgram->next;
    }
    pf.Write(");\n\n", 3);
    pgramset = pgramset->next;
  }

  pf.Write(incmsg, strlen(incmsg));
	if (NumGrammars && pres)
	{
		sprintf(str, "#include \"%s_res.h\"\n\n", ProjName);
		pf.Write(str, strlen(str));
	}

  while(pres)
  {
    if (pres->ControlType != C_MENU)
    {
      sprintf(str, "Dialog %s", pdefines[pres->ControlIDOrd]);
      pf.Write(str, strlen(str));
      DumpGrams(pf, pres);
      sprintf(str,";\n");
      pf.Write(str, strlen(str));

      pcon = pres->controlthread;

      while (pcon)
      {
        if (pcon->ControlType > C_MENUITEM)
        {// ignore non-speech objects
          sprintf(str, "\tControl %s", pdefines[pcon->ControlIDOrd]);
          pf.Write(str, strlen(str));
          DumpGrams(pf, pcon);
          pf.Write(",",1);
          sprintf(str, "\t\tType %s;\n\n", VControl[pcon->ControlType].ControlName);
          pf.Write(str, strlen(str));
        }
        pcon = pcon->lpNext;

      }
    }
    pres = pres->lpNext;
  }

  pf.Close();

  DeleteFile(newname);
  rename(name, newname);

  if (NumGrammars == 0)
  {
    ProjReport("\nNo Grammars are Defined\n");
//    DeleteFile(name);
    return FALSE;
  }
  return TRUE;
}

void ShowResults(LPSTR Title)
{
  if (poutdoc == NULL)
  {
    pOutDoc->OpenDocumentFile(NULL); 
    POSITION position;
    position = pOutDoc->GetFirstDocPosition();
    poutdoc = (COutDoc *) pOutDoc->GetNextDoc(position);
    position = poutdoc->GetFirstViewPosition();
    poutview = (COutView *) poutdoc->GetNextView(position);
    poutct = &poutview->GetRichEditCtrl();
  }
  poutdoc->SetTitle(Title);
  poutct->SetWindowText("");
//  poutct->GetParent()->GetParent()->SetForegroundWindow();
}


int  BuildProject(BOOL bdef, CString DirPath, CString TheProjName)	
{
  CWaitCursor cw;
  FILE * rf;
  int rc;

	if (UpdateOpenDocument(DirPath, FALSE) == FALSE)
	{
		MessageBox(NULL, "An open document has been Modified", 
				"Performing Build Operation", MB_OK);
		return 1;
	}

//  pDocTemplate->CloseAllDocuments(TRUE);
	if (ProjName.GetLength() == 0)
		ProjName = TheProjName;
  if (TheProjName.GetLength() == 0)
    return 1;

  DisableAll();
//  Disable(ID_BUILD_BUILD_PROJECT);
//  Disable(ID_BUILD_BUILDFROMDEFINITIONFILE);
  ShowResults("Project Build Results");
  ProjReport("Building Project " + TheProjName);

  if (bdef)
    if (BuildDefFile() == FALSE)
    {
      RestoreAll();
      Enable(ID_VIEW_RESULTS);
      return 1;
    }
  SetCurrentDirectory(ExecDir);

  DeleteFile("bldres");
  rf = fopen("bldres", "w");

  CWaitCursor wait;

  buildinprogress = TRUE;
  CString t = DirPath+TheProjName;
  rc = VBNFComp(t.GetBuffer(0), rf, poutct);
  buildinprogress = FALSE;

  fclose(rf);
  RestoreAll();
//  Enable(ID_BUILD_BUILD_PROJECT);
//  Enable(ID_BUILD_BUILDFROMDEFINITIONFILE); 
  Enable(ID_VIEW_RESULTS);

	if (PModified)
	{
		SaveProject();
		ShowProjWindow();
	}
  return rc;
}

void AddToTree(LPDIALOGRO plist, HTREEITEM ptreeparent)
{
  HTREEITEM dlgroot;
  CString idt;
  LPCSOGRAMMAR pg;
  char temp[256];

  while (plist)
  {
    if (plist->ControlType <= C_MENU || IsSpeechObj(plist) || plist->ControlType == C_POPUP
			|| plist->ControlType == C_MENUITEM || plist->ControlType == C_GROUPBOX)
    {
      idt = VControl[plist->ControlType].ControlName + (CString) "   ";
      if (plist->ControlIDOrd >= 0)
        idt += pdefines[plist->ControlIDOrd];
      if (plist->Caption >= 0)
      {
        idt += ", \"";
        idt += pnames[plist->Caption];
        idt += "\"";
      }
      dlgroot = pct->InsertItem(idt, ptreeparent);
      pct->SetItemData(dlgroot, (DWORD) plist);
      pg = plist->lpGram;
      while(pg)
      {
        sprintf(temp, "Grammar %s:%s", pgramsetnames[pg->nGrammarSetOrdinal],
          pgramnames[pg->nGrammarOrdinal]);
        HTREEITEM ht = pct->InsertItem((CString) temp, dlgroot);
        pct->SetItemData(ht, (DWORD) pg);
        pg = pg->lpNext;
      }
      if (plist->controlthread)
        AddToTree(plist->controlthread, dlgroot);

    }
    plist = plist->lpNext;
  }
}

BOOL ShowProjWindow()
{
	HTREEITEM gt = NULL, firstgt = NULL;  
	BOOL rc = TRUE;
  LPDIALOGRO pgram = pdreshead;
//  POINT pos;

  if (ProjName.GetLength() == 0)
  {
    return TRUE;
  }

  Enable(ID_GRAMMAR_EXERCISE);
  Enable(ID_VIEW_GRAMMAR_WIZARD);
  
  if (gProjectLimited == FALSE)
  {
	  Enable(ID_VIEW_OBJECTWIZARD);
	  Disable(ID_VIEW_PROJECT);
  }

  if (pdoc == NULL)
  {
    pAppDoc->OpenDocumentFile(NULL); 
    POSITION position;
    position = pAppDoc->GetFirstDocPosition();
    pdoc = (CProjDoc *) pAppDoc->GetNextDoc(position);
    position = pdoc->GetFirstViewPosition();
    pview = (CProjView *) pdoc->GetNextView(position);

    CWnd * pw = pview->GetParent();
    pw->MoveWindow(0,0,250,400);

    pct = &pview->GetTreeCtrl();
  }
  pdoc->SetTitle(ProjName);
  pct->DeleteAllItems();

  root = pct->InsertItem("Project " + ProjName);
  gramroot = pct->InsertItem("Grammar Sets", root);
  resroot = pct->InsertItem("Resources", root);

  AddToTree(pdreshead, resroot);

  LPGRAMSETTHREAD tg = lpGramSetThread;
  LPGRAMTHREAD pgrm;

  while (tg)
  {
    HTREEITEM gsnode = pct->InsertItem(pgramsetnames[tg->ID],gramroot);
    pct->SetItemData(gsnode, (DWORD)tg);
    pgrm = tg->pgrams;
    while (pgrm)
    {
      gt = pct->InsertItem(pgramnames[pgrm->ID], gsnode);
			if (!firstgt)
				firstgt = gt;
      pct->SetItemData(gt, (DWORD)pgrm);
      pgrm = pgrm->next;
    }
    tg = tg->next;
  }
	if (firstgt)
		pct->Select(firstgt, TVGN_CARET);
  return rc;
}

void BuildFromDef()
{
  CString ft, ct, cpath;
  char fullname[256], filename[256];
  char * p;
  char buf[256] = "Project Directory";

	// Put up a standard file dialog to allow output file selection
	CFileDialog dlg(FALSE, ".vdef", "", OFN_PATHMUSTEXIST, 
    "vdef file (*.vdef)|*.vdef||");
  dlg.m_ofn.lpstrTitle  = (LPSTR) & buf;
	if( dlg.DoModal() == IDOK )
	{
    ft = dlg.GetFileTitle();
    ct = dlg.GetFileName();
    
    strcpy(fullname, dlg.GetPathName());
    strcpy(filename, dlg.GetFileName());
    p = strstr(fullname, filename);
    if (p)
      *p = 0;
		cpath = fullname;
    DirPath = cpath;
    BuildProject(FALSE, cpath, ft);
	}	
}

BOOL IsDataObj(LPCONTROLRO pc)
{
  int i = pc->ControlType;

  if ( i == C_EDITTEXT || i == C_COMBOBOX ||
       i == C_COMBOXBOXEX || i == C_LISTBOX || i == C_RICHEDIT || i == C_LISTVIEW)
    return TRUE;
  return FALSE;
}

BOOL IsListObj(LPCONTROLRO pc)
{
 if (pc->ControlType >= C_COMBOBOX && pc->ControlType <= C_LISTBOX)
   return TRUE;
 if (pc->ControlType == C_LISTVIEW || pc->ControlType == C_TREEVIEW)
   return TRUE;
 return FALSE;
}

BOOL IsSpeechObj(LPCONTROLRO pc)
{
  if (pc->ControlType < C_PUSHBUTTON)
    return FALSE;
  return TRUE;
}

extern LPDIALOGRO pstart;
CGrammarWizard pgw;
CObjectWizard powiz;

int IsWizard;

void TEGrammarWizard(LPDIALOGRO pnode)
{
	CGrammarWizard * pg = new CGrammarWizard;
	extern CString DlgGramName;
	DlgGramName = pdefines[pnode->ControlIDOrd] + (CString) "_Grm";
	BOOL changed = pg->CreateDialogShell(DlgGramName, pnode);
  if (PModified)
    SaveProject();
	delete pg;
  if (pdoc && changed)
    pAppDoc->CloseAllDocuments(TRUE); 
	pDocTemplate->OpenDocumentFile(DirPath + DlgGramName + ".abnf", 1);
}

void GrammarWizard(LPDIALOGRO pnode)
{
	//if (UpdateOpenDocument(ProjName, FALSE) == FALSE)
	//{
	//	MessageBox(NULL, "An open ABNF document has been Modified", 
	//			"Building a Grammar", MB_OK);
	//	return;
	//}

  if (ProjName.GetLength() == 0)
    return;
  IsWizard = 1;
  if (pdoc)
    pAppDoc->CloseAllDocuments(TRUE); 

  DisableAll();
//	pMainFrame->ShowWindow(SW_SHOWMAXIMIZED);
  pstart = pnode;
  pgw.DestroyWindow();
  pgw.Create(IDD_GRAMMAR_WIZARD, pMainFrame);
  
	//if(gProjectType != PROJECT_TYPE_TE)
	//	pgw.GetDlgItem(IDB_PREREC2)->EnableWindow(0);
}

extern void * LoadFile(LPSTR pname);

void BuildLib()
{
	LPSTR bdat, pdat, q;
	BOOL last;
	CFileFind cf;
	CString path = ExecDir + (CString)"RuleLib\\*.abnf";
	CString expfile = ExecDir + (CString)"RuleLib\\Exports.txt";
	CString fname;
	if (cf.FindFile(path) == FALSE)
		return;
	FILE * expf = fopen(expfile, "w");
	if (!expf)
		return;
	while (TRUE)
	{
		last = cf.FindNextFile();
		path = cf.GetFilePath();
		if (path.GetLength() == 0)
			break;
			
		bdat = pdat = (LPSTR) LoadFile(path.GetBuffer(0));
		if (pdat == NULL)
			continue;

		fname = cf.GetFileTitle();
		fprintf(expf, "%s\n", fname);
		while (q = strstr(pdat, "#export"))
		{
			pdat = q + strlen("#export");
			while (WhiteSpace(*pdat))
				pdat++;
			if (*pdat++ != '<')
			{
syntaxerr:
				path = "Invalid Rule Syntax" + (CString) pdat; 
				MessageBox(GetFocus(), path, "Library Build", MB_OK);
				break;
			}
			q = strchr(pdat, '>');
			if (q == NULL)
				goto syntaxerr;
			*q = 0;
			fprintf(expf, "\t%s\n", pdat);
			pdat = q + 1;
		}

		free(bdat);


		if (last == 0)
			break;
	}
	fclose(expf);
}

CGramExerciser  pge;

void GrammarExerciser(LPDIALOGRO pnode)
{
  struct stat proj, vdef;
  if (ProjName.GetLength() == 0)
    return;
  if (PModified)
    SaveProject();
  CString name = DirPath + ProjName + (CString)".vprj";
  if (stat(name, &proj) == 0)
  {
    name = DirPath + ProjName + (CString) ".vdef";
    if (FileExists(name.GetBuffer(0)) == FALSE)
      goto rebuild;
    if (stat(name, &vdef) == 0)
    {
      if (proj.st_mtime > vdef.st_mtime)
      {
rebuild:
        if (MessageBox(GetFocus(), "Your project need rebuilding. Do you wish to proceed?",
            "Grammar Exerciser", MB_YESNO) != IDYES)
          return;
        if (BuildProject(TRUE, DirPath, ProjName))
          return;
      }
    }
  }

  IsWizard = 2;
  if (pdoc)
    pAppDoc->CloseAllDocuments(TRUE); 

  DisableAll();
//	pMainFrame->ShowWindow(SW_SHOWMAXIMIZED);
  pstart = pnode;

	//pge = new CGramExerciser(NULL);
	//pge->DoModal();
	//delete pge;
	pge.DestroyWindow();
	pge.Create(IDD_GRAMMAR_EXERSIZER, pMainFrame);
}

void ObjectWizard(LPDIALOGRO pnode)
{
  if (ProjName.GetLength() == 0)
    return;
  IsWizard = 0;
  if (pdoc)
    pAppDoc->CloseAllDocuments(TRUE); 

  DisableAll();
//	pMainFrame->ShowWindow(SW_SHOWMAXIMIZED);
  pstart = pnode;
  powiz.DestroyWindow();
  powiz.Create(IDD_OBJECT_WIZARD, pMainFrame);
}


void ViewResults()
{
  char msg[256];
  FILE * f = fopen("bldres", "r");
  if (f == NULL)
    return;
  ShowResults("Project Build Results");
  
  while (fgets(msg, sizeof(msg), f))
  {
    if (msg[0] == '~')
      continue;
    msg[strlen(msg)-1] = 0;
    ProjReport(msg);
  }
  fclose(f);
}

void UnlinkAllGrammars(LPCONTROLRO pc)
{
  LPCSOGRAMMAR pg, plink;
  if (!pc || pc->lpGram == NULL)
    return;  
  plink = pc->lpGram;
  pc->lpGram = NULL;
  while(plink)
  {
    pg = plink->lpNext;
    free(plink);
    plink = pg;
  }
}

BOOL LinkGrammar(LPCONTROLRO pc, int gsindex, int gindex)
{
  LPCSOGRAMMAR pg, plink;

  if (gsindex < 0 || gindex < 0)
    return FALSE;

  pg = (LPCSOGRAMMAR) calloc(sizeof(CSOGRAMMAR), 1);
  pg->nGrammarSetOrdinal = gsindex;
  pg->nGrammarOrdinal = gindex;

  if (pc->lpGram == NULL)
  {
    pc->lpGram = pg;
    return TRUE;
  }
  plink = pc->lpGram;
  if (plink->nGrammarOrdinal == (UINT)gindex && plink->nGrammarSetOrdinal == (UINT)gsindex)
  {
    free(pg);
    return TRUE;
  }

  while(plink->lpNext)
  {
    plink = plink->lpNext;
    if (plink->nGrammarOrdinal == (UINT)gindex && plink->nGrammarSetOrdinal == (UINT)gsindex)
    {
      free(pg);
      return TRUE;
    }
  }
  plink->lpNext = pg;
  PModified = TRUE;
  return TRUE;
}

void UpdatePane (LPCSTR str)
{
	int result;
	CStatusBar     *StatusBar = ((CMainFrame *) AfxGetApp ()->m_pMainWnd)->GetStatusBar ();

	if (str[0] == 0)
		return;
	result = StatusBar->SetPaneText (0, str, TRUE);
};

BOOL FileExists(LPSTR fname)
{
  FILE * f = fopen(fname, "rb");
  if (f == NULL)
    return NULL;
  fclose(f);
  return TRUE;
}

LPCONTROLRO lastentry;

LPDIALOGRO FindMatch(LPDIALOGRO newone, LPDIALOGRO master)
{
	lastentry = NULL;
	while (master)
	{
		if (master->ControlType == newone->ControlType)
			if (atoi(pvalues[master->ControlIDOrd]) == atoi(pvalues[newone->ControlIDOrd]))
				break;
		lastentry = master;
		master = master -> lpNext;
	}
	return master;
}

LPDIALOGRO DoUpdateProjectFromRes(CString rpath, CString cname)
{
	LPDIALOGRO  OldResHead = pdreshead;
	LPDIALOGRO newrun, rt, rtn, rtt, firstdlg = NULL;
	pdreshead = NULL;
	NumReferences = NumDefines;
	CString tResPath = ResPath;
	CString tResName = ResName;
	ResPath = rpath;
	ResName = cname;
	BOOL found0 = FALSE;

	if(LoadRCFile(FALSE,rpath, cname) == FALSE)
		return NULL;

	//LoadRCFile(FALSE,rpath, cname);

	ResPath = tResPath;
	ResName = tResName;
	if (!pdreshead)
		return NULL;
	newrun = pdreshead;
	while (newrun)
	{
		rtn = newrun->lpNext;
		if (newrun->ControlType == C_DIALOG)
		{
			if (!firstdlg)
				firstdlg = newrun;
			if (atoi(pvalues[newrun->ControlIDOrd]) == 0)
				found0 = TRUE;
			rt = FindMatch(newrun, OldResHead);
			if (rt == NULL)
			{	// link in new dialog entry
				//newrun->lpNext = NULL;
				//LinkOuter(OldResHead, newrun);
			}
			else
			{	// replace existing dialog entry
				//newrun->lpNext = rt->lpNext;
				//rt->lpNext = NULL;
				newrun->lpGram = rt->lpGram;
				rt->lpGram = NULL;
				newrun->DataEntryPrompt = rt->DataEntryPrompt;
				newrun->DataType = rt->DataType;
				newrun->PhraseList = rt->PhraseList;
				newrun->NavigationPrompt = rt->NavigationPrompt;
				newrun->PreRecActions = rt->PreRecActions;

				LPCONTROLRO pnewcontrol = newrun->controlthread;
				LPCONTROLRO pon, poldcontrol = rt->controlthread;
				//if (lastentry == NULL)
				//{
				//	OldResHead = newrun;
				//}
				//else
				//	lastentry->lpNext = newrun;
				while (pnewcontrol)
				{
					pon = pnewcontrol->lpNext;
					rtt = FindMatch(pnewcontrol, poldcontrol);
					if (rtt)
					{ // use old control grammar and prompt assignments 
						pnewcontrol->lpGram = rtt->lpGram;
						pnewcontrol->DataEntryPrompt = rtt->DataEntryPrompt;
						pnewcontrol->DataType = rtt->DataType;
						pnewcontrol->PhraseList = rtt->PhraseList;
						pnewcontrol->NavigationPrompt = rtt->NavigationPrompt;
						pnewcontrol->PreRecActions = rtt->PreRecActions;
						rtt->lpGram = NULL;
					}
					pnewcontrol = pon;
				}
				//DoFree(rt);		// throw away old dialog
			}
		}
		newrun = rtn;
	}

	if (!found0)
	{
		// make sure we do not delete dialog 0 entry
		rt = OldResHead;
		rtt = NULL;
		while (rt)
		{
			rtn = rt->lpNext;
			if (atoi(pvalues[rt->ControlIDOrd]) == 0)
			{
				rt->lpNext = NULL;
				if (rtt)
					rtt->lpNext = NULL;
				LinkOuter(pdreshead, rt); // include dialog 0 entry
				break;
			}
			rtt = rt;
			rt = rtn;
		}
	}
	DoFree(OldResHead);			// Throw away old resource def
	//pdreshead = OldResHead;
	SaveProject();
	ShowProjWindow();
	return firstdlg;
}

LPDIALOGRO UpdateProjectFromRes()
{
	return DoUpdateProjectFromRes(ResPath, ResName);
}

void AddResToProject()
{
  char buf[256] = "Open AccuStudio File";

	CFileDialog dlg(TRUE, "*.rc", "*.rc", OFN_PATHMUSTEXIST, 
    "Resource files (*.rc)|*.rc|Resource files (*.rc)|*.rc|All Files (*.*) |*.*||");
  dlg.m_ofn.lpstrTitle  = (LPSTR) & buf;
	if( dlg.DoModal() == IDOK )
	{
    char fullname[256], filename[256], ext[32];
    char * p;
    
    strcpy(fullname, dlg.GetPathName());
    strcpy(filename, dlg.GetFileName());
    strcpy(ext, dlg.GetFileExt());
    p = strstr(fullname, filename);
    if (p)
      *p = 0;
		CString projpath = fullname;
		CString projname = filename;
		DoUpdateProjectFromRes(projpath, projname);
	}	  	
}
