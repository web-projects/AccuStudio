//
// VBNFComp.cpp : Defines the entry point for the VBNF Compiler.
//


#include "stdafx.h"
#include <process.h>
#include <direct.h>
#include "VBNFComp.h"
#include "AccuSpeech.h"
#include "PCode.h"
#include "CompilerModules.h"
#include "project.h"
//#include "studiofile.h"
//#include "voc.h" 

#include "common.h"
#define MAXSTARTRULES		512

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__; 
#endif

#define COMPILER_VERSION    (1<<8 | 0)   // V 1.0

#define MAXNESTING    16      // nesting of Include files


typedef enum  { END_TOKEN, RULEDEF_TOKEN, ALPHA_TOKEN } tokentype;

#define MAXORDS   128

CString CCurrentFile; // The current file being compiled

Recognizer_Type Recognizer = NUANCE3200;
char * RecognizerName[] = {
	"Nuance 3200 V4+",
	"Nuance 3200",
	"Fonix"};
extern Target_Models Model;

extern UINT SaveVersion;

// THE MASTER GRAMMAR FILE
FILE * MasterFile;
MASTER_HEADER * pMasterHdr;
extern Target_Models Model;
extern Target_Languages Language; 

extern long filelen;
extern BOOL PModified;         // TRUE if the project has been modified
extern CString ExecDir;
extern CString ProjName;
extern CString DirPath;
extern BYTE * pgramparent; 
CRichEditCtrl * preport; 
FILE * ReportFile; 
char cbase[256];
PCODE_HEADER * pcode_hdr;    // Header for output ".pcd" file
static char Namechars[] = "-_."; //{ '-', '_', '.' };
extern int NumGrams;
long dummycount, dummyindex;
LPSTR * Production_Strings;
int g_gramindex;
BOOL NewLogic = FALSE;
BYTE * pgramatts;			 // Grammar attributes
extern void DoAdjustGrammars(int gs);


static void SkipLine();

BOOL Prod_Subexpression();
extern void * LoadFile(LPSTR pname);
extern int AddGramName(int i, LPSTR pname);

int NumTemplates;
TEMPLATE_REC * ptemplates;

typedef struct funcproto
{
  char * Name;
	BYTE  NumArgs;
	BYTE  ReturnResult;		// wja : 06/29/09: handles sem funcs returning results
} Function_Protocol;

typedef struct grsets_tag 
{
  int NumGrammars;
  char * * pGrammars;
} GRSETS;

int TheNumGramSets = 0;
GRAMMAR_SETS_HEADER * pGramSetsHdr = NULL;
WORD  Ord[MAXORDS];         // ordinals
GRSETS * pGrSets = NULL;
int Comperrs, TotalComperrs, AddedGrammars;
BOOL PrePrompt();
extern int FindGramName(LPSTR pname);

unsigned char token[] = { 0x00, 0x00, 0xa1, 0xef, 0xcd, 0xcf, 0x2b, 0x5c, 0x12, 0xed, 
								0x35, 0xc7, 0x89, 0x78, 0x9a, 0xa0, 0x24, 0x63, 0x00, 0x00 };

int ComputeHeaderHash(unsigned char* buf, unsigned int len)
{
	UINT ModValue = 10000001;
	UINT HASHCONSTANT = 64;
	UINT i, h = 0;
	UCHAR* p = buf;

	if (len == 0)
		return 0;

	h = ((UINT)*p++ << 18) % ModValue;
	for(i=1;i<len;i++)
		h = (HASHCONSTANT * h + *p++) % ModValue;
	return (int)(h << 6);
}

BOOL aisalpha(char cc)
{
	int ic = (int)cc & 0x00FF;
	if ((ic >= 'A') && (ic <= 'Z'))
		return TRUE;
	if ((ic >= 'a') && (ic <= 'z'))
		return TRUE;
	if (ic>= 192)			// extended latin characters
		return TRUE;
	return FALSE;
}

Function_Protocol FuncDescription[END_OP] = 
{ 
	// wja : 06/29/09: handles sem funcs returning results; new opcodes 
  {"SETFIELD", 2, FALSE},
  {"APPENDFIELD", 2, FALSE},
  {"SETFOCUS", 1, FALSE},
  {"CLEARFIELD", 1, FALSE},
  {"SAYFIELD", 1, FALSE},
  {"SAYSTRING", 1, FALSE},
  {"DEPRESS", 1, FALSE},
  {"SELECTITEM", 2, FALSE},
  {"SELECTTAB", 1, FALSE},
  {"NAVIGATE", 2, FALSE},
  {"SETRADIO", 3, FALSE},
  {"SETDATETIME", 6, FALSE},
  {"SELECTMENUITEM", 1, FALSE},
  {"ACTIVATE", 1, FALSE},
	{"SETCHECKBOX", 2, FALSE},
	{"WAVECAPTURE", 1, FALSE},
	{"PLAYWAVE", 1, FALSE}, 
	// Arithmetic operators
	{"ADD", 2, TRUE},
	{"SUB", 2, TRUE},
	{"MUL", 2, TRUE},
	{"DIV", 2, TRUE},
	{"MOD", 2, TRUE},
// Unary operators
	{"GETFIELD", 1, TRUE},
// Misc Ops
	{"SETTTSOPTIONS", 2, FALSE},
	{"SAYSTRINGEX", 2, FALSE},
	{"SETRECPARAM", 2, FALSE},
// Logic Operators
	{"IF", 1, FALSE},
	{"ELSE", 0, FALSE},
	{"END", 0, FALSE},
// Ops returning result
	{"EQ", 2, TRUE},
	{"NE", 2, TRUE},
	{"GT", 2, TRUE},
	{"LT", 2, TRUE},
	{"GTOREQ", 2, TRUE},
	{"LTOREQ", 2, TRUE},
	{"LENGTH", 1, TRUE},
	{"CASECONVERT", 2, TRUE},
	{"COMPUTEHASH", 2, TRUE},
	{"GETDATE", 2, TRUE},
// RETURN op
	{"RETURN", 1, FALSE},
// String Translation
	{"TRANSLATE", 2, TRUE},
	{"TRAINUSER", 2, FALSE},
	{"ELSEIF", 1, FALSE},
	{"WHILE", 1, FALSE},
	{"SUBSTR", 3, TRUE},
	{"JMP_", 1, FALSE},		// Stub for internal JMP_OP
	{"LOGIF_", 1, FALSE},		// Stub for internal LOGIF_OP
	{"SPRINTF", 255, TRUE},		// Variable-length sprintf
	{"GETSESSION", 0, TRUE},		// Gets the current session
	{"GETSYSINFO", 1, TRUE},
	{"FILLDYNAMICFIELD", 2, FALSE}
};

BOOL OPCodeHasID(UINT op)
{
	switch (op)
	{
		case GETFIELD_OP:
		case SETFIELD_OP: 
		case APPEND_OP: 
		case SETFOCUS_OP: 
		case CLEARFIELD_OP: 
		case SAYFIELD_OP: 
		case DEPRESS_OP: 
		case SELECTITEM_OP: 
		case SELECTTAB_OP: 
		case SETRADIO_OP: 
		case SETMENUITEM_OP: 
		case ACTIVATE_OP: 
		case SETCHECKBOX_OP: 
			return TRUE;
	}
	return FALSE;
}

LPSTR GetFuncName(UINT op)
{
	if((op < SETFIELD_OP) || (op > (END_OP)))
		return "Unknown OP_CODE";

	return FuncDescription[op - SETFIELD_OP].Name;
}

#define NUMFUNCTIONS (sizeof(FuncDescription) / sizeof(Function_Protocol))

// Volatile variables - need re-initialization
static CStudioFile lifile[MAXNESTING];
static FILE * libmfile[MAXNESTING];
static FILE * pcodefile;
static int level = 0;
static char EOD = 1;
static char * * pdefines, * * pvalues;
static int NumDefines = 0, GlobalIndex = 0, SharedGlobalIndex = 0;
char * * prules;
int * pruleatts;
int NumRules;
int ErrorCount;
char thepath[256];
int ProdIndex;
BYTE * pcode;
int pcodesize, pcodeindex, pcodemax = MAX_PCODES;
int * Productions;
static int vdelay = 0;
char * * plits;
int litindex;

//////////////////////////////////// end of initialized variables

static BOOL InHFile;
int RuleIndex;
char repm[256];

char lLineBuf[2];
char lToken[256];
char * opLineBuf = lLineBuf;
char lCurChar = ' ';
int   StartRuleIndex = -1;

int numstartrules = 0;
int prerecsemantics[MAXSTARTRULES];
int startrulexlate[MAXSTARTRULES];

void Opcode(BYTE op);
void Operand(int val);
int AddString(char * pstr);
BOOL lFind(char * pstr);
BOOL GetQuoted(void);
BOOL DoGramClause(SPEECH_OBJECTS_HEADER * psh);
BOOL GetDefField();

static STARTUPINFO TheStartup;
static PROCESS_INFORMATION TheProcess;
DWORD ecode = -100;

char * VControlNames[NUMCONTROLTYPES] = {
    "DIALOG",         // Dialog - not a control
    "MENU",           // Menu container

    "LTEXT",
    "RTEXT",
    "CTEXT",
    "ICON",
    "GROUPBOX",
    "TAB",                // "SysTabControl32"
    "CONTROL",
    "MENUITEM",

    "PUSHBUTTON",
    "DEFPUSHBUTTON",
    "CHECKBOX",           // "Button",BS_AUTOCHECKBOX
    "RADIOBUTTON",        // "Button",BS_AUTORADIOBUTTON
    "EDITTEXT",
    "EDITWAVE",
    "COMBOBOX",
    "COMBOXBOXEX",        // "ComboBoxEx32"
    "LISTBOX",
    "SCROLLBAR",
    "LISTVIEW",           // "SysListView32"
    "SPINNER",            // "msctls_updown32"
    "PROGRESS",           // "msctls_progress32"
    "SLIDER",             // "msctls_trackbar32"
    "TREEVIEW",           // "SysTreeView32"
    "RICHEDIT",           // "RICHEDIT"
    "DATETIMEPICKER",     // "SysDateTimePick32"
    "MONTHCALENDAR",      // "SysMonthCal32"
    "IPADDRESS",          // "SysIPAddress32"
    "POPUP"
};

BOOL IsFuncName(int &i)
{
	for (i=0; i < NUMFUNCTIONS; i++)
		if (FuncDescription[i].Name && strcmp(FuncDescription[i].Name, lToken) == 0)
			break;
	if ( i >= NUMFUNCTIONS)
	{
		return FALSE;
	}
	if ((pcodes) (i + SETFIELD_OP) == FORMAT_OP)
		FuncDescription[i].NumArgs = 255;		// variable argument list
	return TRUE;
}

BOOL SubInFile(LPSTR filename, LPSTR poriginal, LPSTR pnew)
{
  BOOL res = FALSE;
  LPSTR prun, pdat;
  FILE * pin = fopen(filename, "rb");
  if (pin == NULL)
    return FALSE;
  fseek(pin, 0, SEEK_END);
  long len = ftell(pin);
  fseek(pin, 0, SEEK_SET);
  prun = pdat = (LPSTR)malloc(len);
  fread(pdat, 1, len, pin);
  while (TRUE)
  {
    prun = strstr(pdat, poriginal);
    if (prun == NULL)
      break;
    ULONG poff = (ULONG) prun - (ULONG) pdat; 
    res = TRUE;
    int flen = strlen(poriginal);
    memcpy(prun, prun+flen, len-(poff+flen));
    len = len - flen;
    flen = strlen(pnew);
    pdat = (LPSTR)realloc(pdat, len + flen);
    memmove(pdat+poff+flen, pdat+poff, len-poff);
    len += flen;
    memcpy(pdat+poff, pnew, flen);
  }

  fclose(pin);;
  if (res)
  {
    pin = fopen(filename, "wb");    
    fwrite(pdat, 1, len, pin);
    fclose(pin);
  }
  free(pdat);
  return res;
}

void Report(LPSTR msg)
{
  if (preport)
  {
    int te = preport->GetTextLength()-1;
    preport->SetSel(te,te);
    preport->ReplaceSel(msg);
//    preport->ReplaceSel("\n");
  }
	if (ReportFile)
		fprintf(ReportFile, msg);
}

void WaitOnProcess()
{
	WaitForInputIdle(TheProcess.hProcess, INFINITE);
  while (TRUE)
  {
    GetExitCodeProcess(TheProcess.hProcess,&ecode);
    if (ecode != STILL_ACTIVE)
      break;
    Sleep(300);
  }
}

BOOL Launch(LPSTR lpName, BOOL ShareConsole)
{
  int cflag = 0;
	BOOL success;
  cflag = CREATE_NEW_CONSOLE;
	TheStartup.cb = sizeof(STARTUPINFO);
  if (ShareConsole == FALSE)
//    cflag = CREATE_NEW_CONSOLE;
    cflag = DETACHED_PROCESS;
	success = CreateProcess(
		NULL, 
		lpName,
		NULL,
		NULL,
		FALSE,
		NORMAL_PRIORITY_CLASS | CREATE_NEW_PROCESS_GROUP | cflag,
		NULL,
		NULL,
		&TheStartup,
		&TheProcess
	);

	return success;

};

void vout(CString os)
{
  if (libmfile[level])
    fprintf(libmfile[level], os.GetBuffer(0));  
}

void StartDelay(void)
{
  vdelay = 1;
}

void StopDelay()
{
  if (vdelay == 2)
    vdelay = 3;
  else
    vdelay = 0;
}

static void * ReAlloc(void * p, int size)
{
  if (p == NULL)
    return calloc(1, size);
  return realloc(p, size);
}


BOOL SubExpression(void);

void Indent()
{
  if (level != (MAXNESTING - 2))
   for (int i=0; i <= level; i++)
     Report(" ");
}


static char TestChar()
{
  if (vdelay == 3)
  {
    vout("\n");
    vdelay = 0;
  }

loop:
  while ( level >= 0 && (!opLineBuf || *opLineBuf == 0))
  {
		if (lifile[level].m_pStream == 0)
			break;
//    if (lifile[level].ReadString(lLineBufStr) == 0)
    if (lifile[level].GetData() == 0)
    {
			BOOL intemplate = lifile[level].IsInTemplate();
      lifile[level].Close();
      if (libmfile[level] && libmfile[level] != libmfile[0])
        fclose(libmfile[level]);
      level--;
      InHFile = FALSE;
			if (intemplate)
			{
				--opLineBuf;
				return lCurChar;
			}
      if (level >= 0 && lifile[level].m_pStream == NULL)
        return (lCurChar = EOD);
//      Indent();
//      printf("\n");
    }
    else
    {
      if (vdelay == 0)
        vout("\n");
      else
        vdelay = 2;
			opLineBuf =  lifile[level].GetStringPtr();
      lifile[level].lLineNumber++;
    }
  }
  if (level >= 0)
  {
    if ((opLineBuf[0] == '/' && opLineBuf[1] == '/') || (opLineBuf[0] == '<' && opLineBuf[1] == '!'))   // ignore comments
    {
      OutputCommentLine(opLineBuf);
      *opLineBuf = 0;
      goto loop;
    }
		lCurChar = *opLineBuf;
    return (lCurChar);
  }
  else
    return (lCurChar = EOD);
}

static char GetChar()
{
  char ch = TestChar();
  if (lCurChar != EOD)
		if (*opLineBuf)
		{
			opLineBuf++;
			++lifile[level].CurCol;
		}
  return lCurChar;
}


static BOOL WhiteSpace(char ch)
{
  if (ch == ' ' || ch == '\t' || ch == '\n')
	{
		if (ch == '\n')
		{
			++lifile[level].lLineNumber;
			lifile[level].CurCol = 0;
		}
    return TRUE;
	}
  return FALSE;
}

static void SkipWhiteSpace()
{
  if (WhiteSpace(lCurChar) == FALSE)
    return;
  vout(" ");
  GetChar();
  while (WhiteSpace(lCurChar))
  {
    GetChar();
  }
}


static BOOL Alpha()
{

  if (aisalpha(lCurChar) | isdigit(lCurChar) | (lCurChar == '_')) 
  {
    int i=0;
    while(lCurChar && (aisalpha(lCurChar) || isdigit(lCurChar) || strchr(Namechars, lCurChar)))
    {
      lToken[i++] = lCurChar;
      lCurChar = *opLineBuf++;
			++lifile[level].CurCol;
    }
    lToken[i] = 0;
    if (lCurChar == 0)
    { 
      lCurChar = ' ';
      --opLineBuf;
    }
    return TRUE;
  }
  return FALSE;
}

static tokentype GetToken()
{

  if ( lCurChar == EOD)
    return END_TOKEN;

  while(WhiteSpace(lCurChar))
    lCurChar = GetChar();

  if (Alpha())
  {
    return ALPHA_TOKEN;
  }

  switch (lCurChar)
  {
  case '=':        // rule definition
    return RULEDEF_TOKEN;

  default:
    return END_TOKEN;
  }
  return END_TOKEN;
}

static void Error(char * er)
{
  char msg[256];
  long colno;

  Indent();
  if (level >= 0)
	{
		if (lifile[level].IsInTemplate())
			colno = lifile[level].CurCol;
		else
			colno = ((long)opLineBuf  - (long)lifile[level].GetStringPtr())+1;
    sprintf(msg, "***** %s : Line %i at %i\n", er, lifile[level].lLineNumber, colno);
	}
  else
    sprintf(msg, "***** %s\n", er);
  Report(msg);
  *opLineBuf = 0;
  lCurChar = ' ';
  ErrorCount++;
}

void LError(char * er)
{
  char ch = lCurChar, ch1 = *opLineBuf;
  Error(er);
  lCurChar = ch;
  *opLineBuf = ch1;
}

static BOOL GetAlpha()
{
  tokentype tt = GetToken();
  if (tt == ALPHA_TOKEN)
    return TRUE;
  Error("Missing Name String");
  return FALSE;
}

int GetNum(DWORD * pMask)
{
  int i=0;

  *pMask = ENUMMASK;
  if (lCurChar == '\'')
  {
    GetChar();
    i = lCurChar;
    GetChar();
    lFind("'");
    *pMask = CENUMMASK;
    return i;
  }
  while( isdigit(lCurChar) )
  {
    lToken[i++] = lCurChar;
    lCurChar = *opLineBuf++;
		++lifile[level].CurCol;
  }
  lToken[i] = 0;
  if (lCurChar == 0)
  {
    lCurChar = ' ';
    --opLineBuf;
  }
  if (i == 0)
  {
    Error("Missing numeric value");
    return -1;
  }
  i = atoi(lToken);
  return i;
}



BOOL DoInclude(CString & fname)
{
  if (fname.Find(".h") > 0) // special processing for ".h" files
  {
    if( lifile[level+1].Open(fname, CFile::modeRead) == 0)
    {
      return FALSE;
    }
    libmfile[level+1] = NULL;
    InHFile = TRUE;
  }
  else
  { // normal include file
    if( lifile[level+1].Open(fname + ".abnf", CFile::modeRead) == 0)
    {
      return FALSE;
    }

    libmfile[level+1] = libmfile[0];  // no include file for now
  }
  Indent();
  sprintf(repm,"Including %s\n", lToken);
  Report(repm);
  level++;
  lifile[level].lLineNumber = 0;
  *opLineBuf = 0;      // start fetch from include file
  lCurChar = ' ';      // pretend whitespace to cause skip
  return TRUE;
}


int FindDefine()
{
  for (int i = 0; i < NumDefines; i++)
    if (strcmp(pdefines[i], lToken) == 0)
      return i;
  return -1;
}

extern LONG InclVal(LPSTR lToken);

static BOOL Translate()
{
  int i;
  
  if (isdigit(lToken[0]))
    return TRUE;
  i = FindDefine();
  if (i >= 0)
  {
    strcpy(lToken, pvalues[i]);
		if (lToken[0] == '0' && lToken[1] == 'x')
		{
			i = InclVal(lToken);
			sprintf(lToken, "%i", i);
		}
    return TRUE;
  }
  return FALSE;
}


static void AddDefine(char * pname, char * pvalue)
{
  pdefines = (char * *)ReAlloc(pdefines, (NumDefines+1)*sizeof(LPSTR));
  pdefines[NumDefines] = (char *)malloc(strlen(pname) + 1);
  strcpy(pdefines[NumDefines], pname);
  pvalues = (char * *)ReAlloc(pvalues, (NumDefines+1)*sizeof(LPSTR));
  pvalues[NumDefines] = (char *)malloc(strlen(pvalue) + 1);
  strcpy(pvalues[NumDefines++], pvalue);
}

int FindTemplate()
{
  for (int i = 0; i < NumTemplates; i++)
		if (strcmp(ptemplates[i].pTemplateName, lToken) == 0)
      return i;
  return -1;
}

static void AddTemplate(LPSTR pname, LPSTR pvalue, LPSTR filename, int Line, int Col)
{
  ptemplates = (TEMPLATE_REC * )ReAlloc(ptemplates, (NumTemplates+1)*sizeof(TEMPLATE_REC));
	ptemplates[NumTemplates].ColNum = Col;
	ptemplates[NumTemplates].LineNum = Line;
	ptemplates[NumTemplates].pTemplateName = strdup(pname);
	ptemplates[NumTemplates].pTemplateValue = strdup(pvalue);
	ptemplates[NumTemplates++].pTemplateFile = strdup(filename);
}

static void DoTemplate()
{
  char tname[256];
  if (GetAlpha() == FALSE)
  {
    Error("Invalid 'template' syntax");
    return;
  }
  int index = FindTemplate();
  if (index >= 0)
	{
    Error("Multiply defined Template");
		return;
	}
	strcpy(tname, lToken);
  SkipWhiteSpace();
	if (lCurChar != '{')
	{
		Error("Missing '{'");
		return;
	}
	int Line = lifile[level].lLineNumber;
	int rLine = Line;
	int Col = ((DWORD)opLineBuf-(DWORD)lifile[level].Template.GetBuffer(0));
	CString templateval;
	while (TRUE)
	{
		GetChar();
		if (lCurChar == '}')
			break;
		if (lCurChar == EOD)
			break;
		templateval += lCurChar;
		if (lifile[level].lLineNumber > rLine)
		{
			rLine++;
			templateval += '\n';
		}
	}
	if (lCurChar == EOD)
	{
		Error("Unbounded Template, Missing '}'");
		return;
	}
	AddTemplate(tname, templateval.GetBuffer(0), CCurrentFile.GetBuffer(0), Line, Col);
	GetChar();
}

static BOOL DoGlobalDef(BOOL Shared)
{
  char defname[256];
	char vals[32];
	int gindex = GlobalIndex, index;
	DWORD val = 0x80000001;

	if (Shared)
	{ 
		gindex = SharedGlobalIndex;
		val = 0xC0000001;
	}
	while(TRUE)
	{
	  if (GetAlpha() == FALSE)
			return FALSE;
		SkipWhiteSpace();
		strcpy(defname, lToken);
		index = FindDefine();
	  if (index >= 0)
		{
			Error("Multiply defined symbol");
			return FALSE;
		}
		sprintf(vals, "0x%8X", val + gindex++);
		AddDefine(defname, vals);
		SkipWhiteSpace();
		if (lCurChar != ',')
			break;
		GetChar();
		SkipWhiteSpace();
	}
	if (Shared)
		SharedGlobalIndex = gindex;
	else
		GlobalIndex = gindex;

	return TRUE;
}

static void DoDefine()
{
  char defname[256];
  int i = 0;

  if (GetAlpha() == FALSE)
    return;
  strcpy(defname, lToken);
  int index = FindDefine();

  SkipWhiteSpace();
  if (GetDefField() == FALSE)
  {
    Error("Invalid 'define' value");
    return;
  }
  if (index >= 0)
    Error("Multiply defined symbol");
  AddDefine(defname, lToken);
}

void CheckRules()
{
  int i;
  char err[256];
	BOOL foundmaster = FALSE;
  for (i=0; i < NumRules; i++)
	{
    if ( (pruleatts[i] & ISDEFINED) == 0 && strcmp(prules[i] ,"VOID"))
    {
      sprintf(err, "Missing rule definition for <%s>", prules[i]);
      Error(err);
    }
    if (pruleatts[i] & ISMASTERSTARTRULE)
			foundmaster = TRUE;
	}
	if (!foundmaster)
	{
    sprintf(err, "Missing Start declaration for Grammar <%s>", cbase);
    Error(err);
	}
}


int FindRule()
{
  for (int i = 0; i < NumRules; i++)
    if (strcmp(prules[i], lToken) == 0)
      return i;
  return -1;
}

int AddRule()
{
  prules = (char * *)ReAlloc(prules, (NumRules+1)*sizeof(LPSTR));
  prules[NumRules] = (char *)malloc(strlen(lToken) + 1);
  strcpy(prules[NumRules], lToken);
  pruleatts = (int *)ReAlloc(pruleatts, (NumRules+1)*sizeof(int));
  pruleatts[NumRules] = 0;
  NumRules++;
  return NumRules-1;
}

static BOOL lFind(char * pstr)
{
  while (*pstr && *pstr == lCurChar)
  {
    pstr++;
    GetChar();
  }

  if (*pstr)
  {
    char err[256];
    sprintf(err, "Missing construct: %s", pstr);
    Error(err);
    return FALSE;
  }
  return TRUE;
}

int GetRuleName()
{
  int index;

  SkipWhiteSpace();
  if (lFind("<") == FALSE)
    return -1;
  if (GetAlpha() == FALSE)
    return -1;
  if (lFind(">") == FALSE)
    return -1;
  index = FindRule();
  if (index < 0)
    index = AddRule();
  return index;
}

void DoExport()
{
	char t[256];
	if (GetRuleName() < 0)
		return;
	sprintf(t,"!export <%s>;\n", lToken);
	vout(t);
  SkipWhiteSpace();
}

void DoImport()
{
	char t[256];
	int i = GetRuleName();
	if (i < 0)
		return;
	pruleatts[i] |= ISDEFINED;
	sprintf(t,"!import <%s>", lToken);
	vout(t);
	vout(";\n");
  SkipWhiteSpace();
}

void DoDynamic()
{
  int index = GetRuleName();
  if (pruleatts[index] & ISDEFINED)
    Error("A 'dynamic' declaration must preceed its rule definition");
  pruleatts[index] |= ISDYNAMIC;
}

int grambase;


void DoStart()
{
  int index = GetRuleName();
	int gramindex = FindGramName(lToken);
  if ( gramindex < 0)
	{
		char rm[256];
		gramindex = AddGramName(0, lToken);
		pgramparent[gramindex] = grambase;
		sprintf(rm," ********  Grammar %s has been added.\n", lToken);

		// Add in a reference to the grammar from enclosing dialog
		{
			LPDIALOGRO pbase = pdreshead;
			LPCSOGRAMMAR lpGram;
			CString ts;
			while (pbase)
			{
				ts = pdefines[pbase->ControlIDOrd];
				lpGram = pbase->lpGram;
				if (lpGram && lpGram->nGrammarOrdinal == (grambase-1))
				{// we are in the correct dialog grammar
					LPDIALOGRO pObj = pbase->controlthread;
					while (pObj)
					{
						ts = pdefines[pObj->ControlIDOrd] + (CString) "_Grm";
						if (strstr(&lToken[1], ts.GetBuffer(0)))
						{
							LinkGrammar(pObj, 0, gramindex);
							ShowProjWindow();
							break;
						}
						pObj = pObj->lpNext;
					}
					break;
				}
				pbase = pbase->lpNext;
			}
		}
		Report(rm);
		++AddedGrammars;
		PModified = TRUE;
	}
  if (pruleatts[index] & ISDEFINED)
    Error("A 'start' declaration must preceed its rule definition");
  if (pruleatts[index] & ISSTARTRULE)
    Error("Multiple 'start' declaration for the same rule");
  pruleatts[index] |= ISSTARTRULE;
	pgramatts[gramindex] = 1;
	if (strcmp(cbase,lToken)== 0)
	{
		pruleatts[index] |= ISMASTERSTARTRULE;
		grambase = gramindex + 1;
	}
	prerecsemantics[gramindex] = -1;
	if (PrePrompt())
	{
		prerecsemantics[gramindex] = StartRuleIndex;
	}
	StartRuleIndex = -1;
	startrulexlate[numstartrules++] = gramindex;
	if (numstartrules >= MAXSTARTRULES)
	{
		char tt[256];
		numstartrules--;
		sprintf(tt, "There are too many Grammars(%i allowed)", MAXSTARTRULES);
		Error(tt); 
	}
}

BOOL GetDefField()
{
  int i=0;
	BOOL hex = FALSE;

  lToken[0] = 0;
  if (lCurChar == EOD)
    return FALSE;
  while(lCurChar && !WhiteSpace(lCurChar))
  {
		if (hex)
			if (!isdigit(lCurChar)&& (lCurChar < 'A' || lCurChar > 'F') && (lCurChar < 'a' || lCurChar > 'f'))
				break;
    lToken[i++] = lCurChar;
    lCurChar = *opLineBuf++;
		if (i == 2 && lToken[0] == '0' && lToken[1] == 'x')
			hex = TRUE;
		++lifile[level].CurCol;
  }
  lToken[i] = 0;
  if (lCurChar == 0)
  {
    lCurChar = ' ';
    --opLineBuf;
  }
  return i;
}

BOOL GetLiteral()
{
static char lNamechars[] = "-_"; //{ '-', '_' };
//  char TermChars[] = ";)|](=,";
  int i=0;
  if (lCurChar == EOD)
    return FALSE;
  while(lCurChar && (aisalpha(lCurChar) || isdigit(lCurChar) || strchr(lNamechars, lCurChar)))
//  while( lCurChar && (strchr(TermChars,lCurChar) == NULL) && (WhiteSpace(lCurChar) == FALSE) )
  {
		ValidateStringChar(lCurChar);
    lToken[i++] = lCurChar;
    vout(CString(lCurChar));
    lCurChar = *opLineBuf++;
		++lifile[level].CurCol;
  }
  lToken[i] = 0;
  if (lCurChar == 0)
  {
    lCurChar = ' ';
    --opLineBuf;
  }
  return i;
}

BOOL GetQuoted()
{
  int i=0;

  GetChar();
  while (*opLineBuf)
  {
    if (lCurChar == '"')
      break;
		// Check for hex character
		if (lCurChar == '\\' && *opLineBuf == 'x')
		{
			int k, r = 0;
			for (k=1; k < 3; k++)
			{
				if (isdigit(opLineBuf[k]))
					r = (r<<4) | (opLineBuf[k] - '0');
				else if (opLineBuf[k] >= 'A' && opLineBuf[k] <= 'F')
					r = (r<<4) | (opLineBuf[k] - 'A' + 10);
				else if (opLineBuf[k] >= 'a' && opLineBuf[k] <= 'f')
					r = (r<<4) | (opLineBuf[k] - 'a' + 10);
			}
			opLineBuf += 3;
			lCurChar = r;
		}
		else if (lCurChar == '\\' && *opLineBuf == 'n')
		{
			lCurChar = '\n';
			opLineBuf += 1;
		}
		else if (lCurChar == '\\' && *opLineBuf == 't')
		{
			lCurChar = '\t';
			opLineBuf += 1;
		};
	
    lToken[i++] = lCurChar;
    lCurChar = *opLineBuf++;
		++lifile[level].CurCol;
  }
  if (lCurChar != '"')
  {
    Error("Invalid quoted string");
    return FALSE;
  }
  lToken[i] = 0;
  lCurChar = ' ';
  return TRUE;
}

int DoEnum()
{
  BOOL res = TRUE;
  int val, ord = 0;
  ULONG Mask = ENUMMASK;

  if (lFind("#(") == FALSE)
    return FALSE;
	StartSubexpression();
  while (res)
  {
    SkipWhiteSpace();
    if (lCurChar == ')')
      break;
    if (ord)
			OutputOrOp();
    while (res && (lCurChar != ',') && (lCurChar != ')'))   
    {
      val = (ord<<16) | Mask | RuleIndex;
      StartDelay();
      if (lCurChar == '\"')
      {
        res = GetQuoted();
        SkipWhiteSpace();
        if (lCurChar == '=')
        {
          GetChar();
          SkipWhiteSpace();
          ord = GetNum(&Mask);
          val = (ord<<16) | Mask | RuleIndex;
        }
        SkipWhiteSpace();
        if (lCurChar != ',' && lCurChar != ')')
          val = IGNOREMASK | 0xffff;      //completely ignore
				OutputQuotedString(lToken);
      }
      else
      {
        res = GetLiteral();
        SkipWhiteSpace();
        if (lCurChar == '=')
        {
          GetChar();
          SkipWhiteSpace();
          ord = GetNum(&Mask);
          val = (ord<<16) | Mask | RuleIndex;
        }
        SkipWhiteSpace();
        if (lCurChar != ',' && lCurChar != ')')
          val = IGNOREMASK | 0xffff;      //completely ignore
//        sprintf(os, ": %i ", val);
      }
      StopDelay();
			OutputIntegerAnnotation(val);
//      vout(os);
    };
    ord++;

    SkipWhiteSpace();
    if (lCurChar != ',')
      break;
    GetChar();
  }
  StopSubexpression();
  return lFind(")");
}


int Operand()
{
  if (lCurChar == '<')
  {
    int index = GetRuleName();
    if (index < 0)
      return FALSE;
    OutputRuleName(lToken, index);
    return TRUE;
  }
  else if (lCurChar == '(')
  {
    GetChar();
    StartSubexpression();
    SubExpression();
    StopSubexpression();
    return lFind(")");
  }
  else if (lCurChar == '[')
  {
    GetChar();
		StartOptionalClause();
    StartSubexpression();
    SubExpression();
    StopSubexpression();
		EndOptionalClause();
    return lFind("]");
  }
  else if (lCurChar == '"')
  {
    StartDelay();
    BOOL res = GetQuoted();
		OutputQuotedString(lToken);
//    vout(":");
    return 2;
  }
  else if (lCurChar == '#')      // enumerated productions
  {
    return DoEnum();
  }
  else
  {
    StartDelay();
    if (GetLiteral() == FALSE)
      return FALSE;
//    vout(":");
    return 2;
  }
  return FALSE;
}

BOOL ParseMathOp()
{
	int i,j;
	pcodes pc;

	BOOL isfunc = IsFuncName(i);
  //for (i=0; i < NUMFUNCTIONS; i++)
  //  if (FuncDescription[i].Name && strcmp(FuncDescription[i].Name, lToken) == 0)
  //    break;
  if ( !isfunc) 
  {
    return FALSE; // wja : 06/29/09: handles sem funcs returning results
  }
  if (FuncDescription[i].ReturnResult == FALSE) 
  {
		Error("Function call must return a Result");
    return FALSE; // wja : 06/29/09: handles sem funcs returning results
  }
	SkipWhiteSpace();
  if (lFind("(") == FALSE)
		return FALSE;
	pc = (pcodes) (i + SETFIELD_OP);
  for (j=0; j < FuncDescription[i].NumArgs; j++)
  {
    BOOL arg = Prod_Subexpression();
		if (!arg)
		{
			Error("Invalid Parameter Syntax");
			return FALSE;
		}
    if (j < FuncDescription[i].NumArgs-1)
			if (pc == FORMAT_OP && lCurChar != ',')
				break;
			else
				lFind(",");
  }
	if (pc == FORMAT_OP)
		FuncDescription[i].NumArgs = j+1;
  SkipWhiteSpace();
  lFind(")");
  Opcode(pc);
	return TRUE;
}

void IncDepth(int &depth)
{
	depth++;
	if (depth >= 2)
	{
		Opcode(CONCAT_OP);
		depth--;
	}
}

BOOL Prod_Subexpression()
{
  int index;
  BOOL t, hasarg = FALSE, depth = 0;

  while (TRUE)
  {
    SkipWhiteSpace();
    if (lCurChar == '<')
    {
      if ((index = GetRuleName()) < 0)
        return FALSE;
			hasarg = TRUE;
      Opcode(PUSH_OP);
      Operand(-(index+1));
			IncDepth(depth);
    }
    else if (lCurChar == '"')
    {
      if (GetQuoted() == FALSE)
        return FALSE;
 			hasarg = TRUE;
			index = AddString(lToken);
      Opcode(PUSH_OP);
      Operand(index);
			IncDepth(depth);
    }
    else if (lCurChar == ')' || lCurChar == ',')
      break;
    else
    { // must be a literal
      if (GetAlpha() == FALSE)
        return FALSE;

			if (ParseMathOp())
			{
				hasarg = TRUE;
				IncDepth(depth);
			}
			else
			{
				t = Translate();
				//if (strcmp(lToken, "_data_") == 0)
				//	index = 0x3fff;
				//else if (strcmp(lToken, "_score_") == 0)
				//	index = 0x3ffe;
				//else
				{
					index = AddString(lToken);
					if (t == FALSE)
					{
						char t[256];
						sprintf(t,"Undefined Name: %s", lToken); 
						LError(t);
						return FALSE;
					}
				}
				hasarg = TRUE;
				Opcode(PUSH_OP);
				Operand(index);
				IncDepth(depth);
			}
    }
    SkipWhiteSpace();
    if (lCurChar != '+')
      break;
    GetChar();
  }
  return hasarg;
}


void Alloc(int size)
{
  pcodesize += size;
	if (pcode == NULL)
		pcode = (BYTE *) calloc(1, pcodemax);
	if (pcode == NULL)
	{
		pcodemax = 100;
		pcode = (BYTE *) calloc(1, pcodemax);
		pcodesize = 0;
		Error("Cannot allocate p-code memory");
	}
	if (pcodesize >= pcodemax)
	{
		Error("Code limit size exceeded");
		pcodesize = 0;
		pcodeindex = 0;
	}
//  pcode = (BYTE *) ReAlloc(pcode, pcodesize+10);
}

void Opcode(BYTE op)
{
  Alloc(1);
  pcode[pcodeindex++] = op;
}

void Operand(int val)
{
  Alloc(sizeof(int));
  memcpy(&pcode[pcodeindex], &val, sizeof(int));
  pcodeindex += sizeof(int);
}

int AddString(char * pstr)
{
  int i, comp = 1;
  for (i=0; i < litindex; i++)
  {
    comp = strcmp(pstr, plits[i]);
    if (comp == 0)
      break;
  }
  if (comp == 0)
    return i;
  plits = (char **) ReAlloc(plits, (litindex+1) * sizeof(char *));
  memmove(plits + i + 1, plits + i, (litindex - i) * sizeof(char *));
  plits[i] = (char *) calloc(1, strlen(pstr)+1);
  strcpy(plits[i], pstr);
  ++litindex;
  return i;
}


void FillLogicRef(LPDWORD phead, int poff)
{
	LPDWORD rph = phead, pdest;
	while (*rph)
	{
		pdest = (LPDWORD)&pcode[*rph];
		*rph = *pdest;
		*pdest = poff;
	}
}

void AddLogicRef(DWORD * phead, int poff)
{
	DWORD oldlink = *phead;
	LPDWORD pdat = (LPDWORD)&pcode[poff];
	*pdat = oldlink;
	*phead = poff;
}

BOOL ParseSemArgs(int i)
{
	int j;
	pcodes pc = (pcodes) (i + SETFIELD_OP);
	for (j=0; j < FuncDescription[i].NumArgs; j++)
	{
		BOOL arg = Prod_Subexpression();
		if (!arg)
		{
			Error("Invalid Argument Syntax");
			break;
		}
		if (j < FuncDescription[i].NumArgs-1)
			if (pc == FORMAT_OP && lCurChar != ',')
				break;
			else
				lFind(",");
	}
	SkipWhiteSpace();
	if (pc == FORMAT_OP)
		FuncDescription[i].NumArgs = j+1;
	return lFind(")");
}

BOOL ParseProduction(BOOL dooutput)
{
#define maxlogdepth  32
	typedef struct LOGBLOCK {
		BYTE opcode;
		DWORD blockstart;
		DWORD AlternativeOff;
		DWORD EndOff;
		BOOL  haselse;
	} LOGIC_BLOCK;
	LOGIC_BLOCK pLogic[maxlogdepth] = {0} ;
	int LogicDepth = 0;
  int depth = 0, i,j, index, spcodeindex = pcodeindex;
  int * pint;
  BOOL tt;

  Productions = (int *) ReAlloc(Productions, (ProdIndex+1) * sizeof(int));
  Productions[ProdIndex] = pcodeindex;

  while (TRUE)
  {
    SkipWhiteSpace();

    if (lCurChar == ')' || lCurChar == ']' || lCurChar == '[' || lCurChar == ';' || lCurChar == '|')
      break;
    else if (lCurChar == '<')
    {
      index = GetRuleName();
      if (index < 0)
        return FALSE;
      Opcode(PUSH_OP);
      Operand(-(index+1));
			IncDepth(depth);
    }
    else if (lCurChar == '"')
    {
      if (GetQuoted() == FALSE)
        return FALSE;
      index = AddString(lToken);
      Opcode(PUSH_OP);
      Operand(index);
			IncDepth(depth);
    }
    else
    { // must be a function call or literal
      if (GetAlpha() == FALSE)
        return FALSE;
      tt = Translate();
			SkipWhiteSpace();
			BOOL isfunc = IsFuncName(i);
      if (lCurChar != '(')   // must be a literal
      {
				if (isfunc)
				{
					Error("Missing Function Parameter(s)");
					return(FALSE);
				}
        //if (strcmp(lToken, "_data_") == 0)
        //  index = 0x3fff;
        //else if (strcmp(lToken, "_score_") == 0)
        //  index = 0x3ffe;
        //else
        {
          index = AddString(lToken);
        };
        Opcode(PUSH_OP);
        Operand(index);
				IncDepth(depth);
      }
			else
			{
				// Process semantic function call
				GetChar();
				// See if we have a template definition
				index = FindTemplate();
				if (index >= 0)
				{
					int lev;
					CString arg, fulltemplate;
					char repm[256];
					int argc = 0;
					Indent();
					sprintf(repm,"Expanding %s\n", lToken);
					Report(repm);
					fulltemplate = (CString)ptemplates[index].pTemplateValue;
					while (lCurChar != ')')  // Parse any arguments
					{
						argc++;
						arg.Empty();
						SkipWhiteSpace();
						lev = 0;
						while(lev || (lCurChar != ')' && lCurChar != ','))
						{
							if (lCurChar == '(')
								lev++;
							else if (lCurChar == ')')
								lev--;
							if (lCurChar == '\"')
							{
								if (GetQuoted() == FALSE)
									return(FALSE);
								arg += (CString)'"' + lToken + (CString)'"';
							}
							else
							{
								arg += lCurChar;
								GetChar();
								if (lCurChar == EOD)
									goto tend;
							}
						}
						sprintf(repm, " %i", argc);
						repm[0] = '%';
						fulltemplate.Replace(repm, arg);
						SkipWhiteSpace();
						if (lCurChar != ',')
							break;
						GetChar();
					}
tend:
					if (!lFind(")"))
						return FALSE;
					//GetChar();
					//Indent();
					level++;
					libmfile[level] = libmfile[0];
					lifile[level].lLineNumber = 0;
					lifile[level].CurCol = 0;
					lifile[level].SetTemplate(fulltemplate);
					opLineBuf = lifile[level].GetStringPtr();
					lCurChar = ' ';      // pretend whitespace to cause skip
					lifile[level].m_pStream = lifile[level-1].m_pStream;
					continue;
				}
				//for (i=0; i < NUMFUNCTIONS; i++)
				//	if (FuncDescription[i].Name && strcmp(FuncDescription[i].Name, lToken) == 0)
				//		break;
				//if ( i >= NUMFUNCTIONS)
				if (!isfunc)
				{
					Error("Undefined Function Reference");
					return FALSE;
				}
				BOOL retres = FuncDescription[i].ReturnResult;
				int argcount = FuncDescription[i].NumArgs;
				if (NewLogic)
				{
					BYTE opcode = i + SETFIELD_OP;
					if (opcode == IF_OP)
						opcode = LOGIF_OP;		// use new logical IF operator
					BYTE theopcode = opcode;
					// Check for Logic operator
					switch (opcode)
					{
						case ELSEIF_OP:
							if (LogicDepth == 0)
							{
								Error("ELSEIF outside an IF");
								return FALSE;
							}
							if (pLogic[LogicDepth-1].haselse)
							{
								Error("ELSEIF preceeded by ELSE");
								return FALSE;
							}
							Opcode(JMP_OP);	// Issue a jump to IF end
							Operand(0);		// ouput a jump to alternative or end
							AddLogicRef(&pLogic[LogicDepth-1].EndOff, pcodeindex-4);
							FillLogicRef(&pLogic[LogicDepth-1].AlternativeOff, pcodeindex - spcodeindex);
							opcode = LOGIF_OP;			// Acts like another IF ...
							break;
						case WHILE_OP:
							opcode = LOGIF_OP;  // treat WHILE like an IF
						case LOGIF_OP:
							pLogic[LogicDepth].opcode = theopcode;  // save block op code
							pLogic[LogicDepth].blockstart = pcodeindex;
							pLogic[LogicDepth].AlternativeOff = pLogic[LogicDepth].EndOff = pLogic[LogicDepth].haselse = 0;
							++LogicDepth;
							break;
						case ENDIF_OP:
							if (LogicDepth <= 0)
							{
								Error("END does not terminate a block");
								return FALSE;
							}
							if (pLogic[LogicDepth-1].opcode == WHILE_OP)
							{
								Opcode(JMP_OP);	// Issue a jump to start of While
								Operand(pLogic[LogicDepth-1].blockstart - spcodeindex);		
							}
							if (pLogic[LogicDepth-1].EndOff)
								FillLogicRef(&pLogic[LogicDepth-1].EndOff, pcodeindex - spcodeindex);
							if (pLogic[LogicDepth-1].AlternativeOff)
								FillLogicRef(&pLogic[LogicDepth-1].AlternativeOff, pcodeindex - spcodeindex);
							--LogicDepth;
							opcode = 0;		// inhibit code
							break;
						case ELSE_OP:
							if (LogicDepth == 0)
							{
								Error("ELSE outside an IF");
								return FALSE;
							}
							if (pLogic[LogicDepth-1].haselse)
							{
								Error("Multiple ELSE clauses");
								return FALSE;
							}
							pLogic[LogicDepth-1].haselse = TRUE;
							Opcode(JMP_OP);	// Issue a jump to IF end
							Operand(0);		// ouput a jump to alternative or end
							AddLogicRef(&pLogic[LogicDepth-1].EndOff, pcodeindex-4);
							FillLogicRef(&pLogic[LogicDepth-1].AlternativeOff, pcodeindex - spcodeindex);
							pLogic[LogicDepth-1].AlternativeOff = 0;
							opcode = 0;
							break;
						default:
							break;
					}
					ParseSemArgs(i);
					if (opcode)
						Opcode(opcode);

					// Check for Logic op post-processing
					switch (theopcode)
					{
						case WHILE_OP:
						case ELSEIF_OP:
						case LOGIF_OP:
							pLogic[LogicDepth-1].AlternativeOff = pcodeindex;
							Operand(0);		// ouput a jump to alternative or end
							argcount = retres = 0;
							break;
						default:
							break;
					}
				}
				else  // not NewLogic
				{
					ParseSemArgs(i);
					Opcode(i + SETFIELD_OP);
				}

				if (retres)
				{// unary or binary operator
					IncDepth(depth);
				}
				else if (argcount)
				{
					Opcode(POP_OP);
					Opcode(argcount);
				};
			}
    }

    SkipWhiteSpace();
    if (lCurChar != '+')
      break;
    GetChar();
  }
	if (LogicDepth)
		Error("Missing END clause");

	//while (LogicDepth > 0)  // Take care of missing END's
	//{
	//	if (pLogic[LogicDepth-1].EndOff)
	//		FillLogicRef(&pLogic[LogicDepth-1].EndOff, pcodeindex - spcodeindex);
	//	if (pLogic[LogicDepth-1].AlternativeOff)
	//		FillLogicRef(&pLogic[LogicDepth-1].AlternativeOff, pcodeindex - spcodeindex);
	//	--LogicDepth;
	//}
  if (depth)    // issue replace op if an operand result is hanging
  {
    Opcode(REP_OP);
    depth = 0;
  }
  Opcode(END_OP);

  if (dooutput && pcodeindex == (spcodeindex+7))  // we have a single PUSH of string
  {
    pint = (int *) &pcode[spcodeindex+1];
    if ( (i= *pint) >= 0)
    {
      j = (i<<16) | RuleIndex;
      j |= STRINGMASK;
			OutputIntegerAnnotation(j);
//      sprintf(t, " %i ", j);   // simple production is a string pointer only
//      vout(t);
      StopDelay();
      pcodeindex -= 7;
      pcodesize -= 7;
      return TRUE;
    }
  }
  j = (ProdIndex << 16) | RuleIndex;
  if (dooutput)
	{
		OutputIntegerAnnotation(j);
//		sprintf(t, " %i ", j);   // output (production -- ruleid) for annotation
//    vout(t);
	}
  StopDelay();
  ++ProdIndex;
  return j;
}

BOOL SubExpression()
{
  while (lCurChar != EOD)
  {
    SkipWhiteSpace();
    if (lCurChar == '|')
    {
      OutputOrOp();
			GetChar();
      continue;
    }
    else if (lCurChar == ')')
      break;

    else if (lCurChar == ']')
      break;

    else if (lCurChar == ';')
    {
			int i;
      if (level >= 0)
			{
				OutputRuleTrailer(RuleIndex);
				for (i=0; i < dummyindex; i++)
				{
					vout("\n");
					vout(Production_Strings[i]);
					free(Production_Strings[i]);
					vout("\n");
				}
				free(Production_Strings);

			}
      GetChar();
      break;
    }
		if (level < 0)
			return FALSE;
		long lastpos = ftell(libmfile[level]);
    int lastoperand = Operand();
    SkipWhiteSpace();
    if (lastoperand == 0)
      return FALSE;
    if (lCurChar != '=' && lastoperand == 2)   // terminal with no production - ignore
    {
			if (lCurChar == '#')		// Self-defining terminal production
			{
				OutputIntegerAnnotation(AUTOPRODUCTION | RuleIndex);
				GetChar();
			}
			else
				OutputIntegerAnnotation(IGNOREMASK | RuleIndex);
      StopDelay();
    }

    if (lCurChar == '=')
    {
      if (lFind("=>") == FALSE)
        return FALSE;

      if (lastoperand == 2)
        ParseProduction(TRUE);
      else // if ( (pruleatts[RuleIndex] & ISSTARTRULE) == FALSE)
      {
				LPSTR pp;
				int j = ParseProduction(FALSE);
				if (level < 0)
					return FALSE;
				long curpos = ftell(libmfile[level]);
				int len = curpos - lastpos;
				fseek(libmfile[level], lastpos, SEEK_SET);
				pp = (LPSTR)calloc(1,len+10);
				fread(pp, 1, len, libmfile[level]);
				fseek(libmfile[level], lastpos, SEEK_SET);
				len -= 2;
				pp[len] = 0;
				if (Recognizer == NUANCE3200)
				{
					char lpp[128];
					vout(pp);
	
					#ifdef _v3code
							sprintf(lpp, " n_w !id(%i)", RULESTART | j);
					#else
							sprintf(lpp," !action(\"~%i\")", j);
					#endif					
					vout(lpp);
				}
				else
					OutputIntegerAnnotation(j);
				free(pp);
			}
			/*
      else
      {
        StartRuleIndex = ProdIndex;
        ParseProduction(FALSE);
      }
			*/
    }
    else if (lCurChar == '+')
    {
			OutputOneOrMore();
      GetChar();
    }
    else if (lCurChar == '*')
    {
			OutputZeroOrMore();
      GetChar();
    }
  }
  return TRUE;
}

BOOL PrePrompt()
{
	int j;
  SkipWhiteSpace();
	if (lCurChar != '=')
		return FALSE;
	GetChar();
	if (lCurChar != '>')
		return FALSE;
	GetChar();
	Production_Strings = (LPSTR *) calloc(1, 100*sizeof(LPSTR));
	if (StartRuleIndex != -1)
	{
		Error("Multiple Prompt Rules are not allowed");
		return FALSE;
	}
	StartRuleIndex = ProdIndex;
	j = ParseProduction(FALSE); 
	if (j == FALSE && RuleIndex)
		return FALSE;
	if (lFind(";") == FALSE)
		return FALSE;
	return TRUE;
}

BOOL RuleDef()
{
	BOOL res = TRUE;
  RuleIndex = GetRuleName();
  if (RuleIndex < 0)
		return FALSE;
  if (pruleatts[RuleIndex] & ISDEFINED)
	{
		CString ts = (CString)"Multiply-Defined Rule : " + lToken;
    Error(ts.GetBuffer(0));
	}
  pruleatts[RuleIndex] |= ISDEFINED;
  if (pruleatts[RuleIndex] & ISSTARTRULE)
		OutputRuleDefinition(TRUE, lToken, g_gramindex, RuleIndex);
  else
		OutputRuleDefinition(FALSE, lToken, g_gramindex, RuleIndex); 
  SkipWhiteSpace();
  if (lFind("="))
	{
		OutputRuleHeader(RuleIndex);
		dummyindex = 0;
		Production_Strings = (LPSTR *) calloc(1, 100*sizeof(LPSTR));
		res = SubExpression();
	}
  return res;
}

static void SkipLine()
{
  *opLineBuf = 0;    // skip current line
  lCurChar = ' ';
}

static BOOL GlobalDeclaration()
{
	BOOL shared = FALSE;
	if (strcmp(lToken, "shared") == 0)
	{
		shared = TRUE;
		SkipWhiteSpace();
		if (GetAlpha() == FALSE)
			return FALSE;
	}
	if (strcmp(lToken, "string"))
		return FALSE;
	SkipWhiteSpace();
	return DoGlobalDef(shared);
}

static BOOL Declarative()
{
  BOOL Result = TRUE;
  char LibDir[] = "RuleLib\\";

  if (lCurChar != '#')
    return FALSE;
  GetChar();
  if (GetAlpha() == FALSE)
    return FALSE;
	if ( (strcmp(lToken, "shared") == 0) || (strcmp(lToken, "string") == 0))
		return GlobalDeclaration();
  if (strcmp(lToken, "include") == 0)
  {
    SkipWhiteSpace();
    if (lCurChar == '\"')
    {
      LibDir[0] = 0;
      if (GetQuoted() == FALSE) 
      {
ierr:
        Error("invalid #include syntax");
        SkipLine();
        return FALSE;
      }
    }
    else if (lCurChar == '<')
    {
      GetChar();
      if (GetAlpha() == FALSE)
        goto ierr;
      if (lCurChar != '>')
        goto ierr;
      lCurChar = ' ';
    };         

    if (DoInclude((CString)thepath + (CString)lToken) == FALSE)
      if (DoInclude(ExecDir + (CString)LibDir + (CString)lToken) == FALSE)
      {
        goto ierr;
      }
    Result = 2;
  }
  else if (strcmp(lToken, "define") == 0)
    DoDefine();
  else if (strcmp(lToken, "template") == 0)
    DoTemplate();
  else if (strcmp(lToken, "start") == 0)
    DoStart();
  else if (strcmp(lToken, "dynamic") == 0)
    DoDynamic();
	else if (strcmp(lToken, "import") == 0)
	{
    DoImport();		// output directive
	}
	else if (strcmp(lToken, "export") == 0)
	{
    DoExport();		// output directive
	}
  else if (InHFile)    // ignore other commands in a h file
  {
    SkipLine();		
  }
  else
	{
		char t[256];
		sprintf(t, "Invalid Declaration: %s!", lToken);
    Error(t);
	}
  return Result;
}

void FreeTemplates()
{
	if (NumTemplates)
	{
		for (int i=0; i < NumTemplates; i++)
		{
			free(ptemplates[i].pTemplateFile);
			free(ptemplates[i].pTemplateName);
			free(ptemplates[i].pTemplateValue);
		}
		free(ptemplates);
	};
  ptemplates = NULL;
	NumTemplates = 0;
}

int Compile(int gramindex, char * bfile)
{
  char bnffile[256];
  LPSTR ptr;
  int i;
	
	pcodesize = pcodeindex = 0;
	pcodemax = MAX_PCODES;
//	Opcode(END_OP);
	g_gramindex = gramindex;
	dummycount = 0;
  ErrorCount = 0;
  StartRuleIndex = -1;

  GetFullPathName(bfile, sizeof(bnffile),thepath, &ptr);

  strcpy(cbase, ptr);
	grambase = FindGramName(ptr) + 1;
  *ptr = 0;

  sprintf(bnffile, "%s%s.abnf", thepath, cbase);

  //if(MakeFileWritable(bnffile) == FALSE)
  //{
  //  char t[256];
  //  sprintf(t, "Unable to open ABNF File: %s\n", bfile);
  //  LError(t);
  //  return 1;
  //}

	CCurrentFile = cbase;
  if( lifile[level].Open(bnffile, CFile::modeRead) == 0)
  {
    char t[256];
    sprintf(t, "Unable to open ABNF File: %s\n", bfile);
    LError(t);
    return 1;
  }

  sprintf(repm,"Compiling (#%i) %s\n", gramindex-1, cbase);
  Report(repm);

	sprintf(bnffile, "%s%s.bnf", thepath, cbase);

  libmfile[0] = fopen(bnffile, "w+");
  if( libmfile[0] == 0)
  {
    char t[256];
    sprintf(t, "Unable to create BNF File: %s\n", bnffile);    
    LError(t);
    return 1;
  }

  sprintf(bnffile, "%s%s.vpc", thepath, cbase);

  pcodefile = fopen(bnffile, "wb");
  if( pcodefile == 0)
  {
    char t[256];
    sprintf(t, "Unable to create P-Code File: %s\n", bnffile);
    LError(t);
    return 1;
  }
	OutputGrammarHeader(cbase);
  lCurChar = ' ';
  *opLineBuf = 0;
  while (TRUE)
  {
    SkipWhiteSpace();
    if (lCurChar == EOD)
      break;
    if (Declarative())
      continue;
    if (RuleDef())
      continue;
    Error("Unrecognizable Construct");
  }
  CheckRules();

  fclose(libmfile[0]);
//  lifile[0].Close();

  int val = sizeof(PCODE_HEADER) + (ProdIndex-1) * sizeof(int);
  pcode_hdr = (PCODE_HEADER *) calloc(1, val);
	pcode_hdr->EndOpValue = END_OP;
  pcode_hdr->Version = COMPILER_VERSION;
  pcode_hdr->NumProductions = ProdIndex;
  pcode_hdr->NumRules = NumRules;
  pcode_hdr->StartRuleIndex = StartRuleIndex;

  for (i = 0; i < ProdIndex; i++)
  {
    pcode_hdr->ProdOffsets[i] = val + Productions[i];
  }
  
  while ((val + pcodeindex) & 3)     // make pcode count long-bounded
    ++pcodeindex;
  pcode_hdr->StringsOffset = val + pcodeindex;
  fwrite(pcode_hdr, 1, val, pcodefile);
  fwrite(pcode, 1, pcodeindex, pcodefile);

  val = pcode_hdr->StringsOffset + (litindex * sizeof(int));  // first string offset

  for (i=0; i < litindex; i++)      // output all the string offsets
  {
    fwrite(&val, 1, sizeof(int), pcodefile);
    val += (strlen(plits[i])+1);
  }

  for (i=0; i < litindex; i++)      // now output the strings themselves
  {
    fwrite(plits[i], 1, strlen(plits[i])+1, pcodefile);
  }
  fflush(pcodefile);
  fclose(pcodefile);

  free(pcode_hdr);
  free(pcode);
  pcode = NULL;
  pcode_hdr = NULL;
  //memset(lLineNumber, 0, sizeof(lLineNumber));
  level = 0;
	lifile[level].lLineNumber = 0;
  EOD = 1;
  for (i=0; i < NumDefines; i++)
  {
    free(pdefines[i]);
    free(pvalues[i]);
  }
  pdefines = pvalues = NULL;

	//FreeTemplates();
  for (i=0; i < NumRules; i++)
  {
    free(prules[i]);
  }
  prules = NULL;
  free(pruleatts);
  pruleatts = NULL;

  NumRules = NumDefines = ProdIndex = GlobalIndex = SharedGlobalIndex = 0;
//  pcodesize = pcodeindex = 0;
  free(Productions);
  Productions = NULL;

  return ErrorCount;
}

BOOL DoGramClause(SPEECH_OBJECTS_HEADER * psh)
{
  int k,j, i;

  strupr(lToken);
  if (strcmp(lToken, "GRAMMARS") != 0)
    return FALSE;       // no Grammar clause

  SkipWhiteSpace();
  lFind("(");
  psh->numgrammars = j = 0;
  while (lCurChar)
  {
    ++psh->numgrammars;
    SkipWhiteSpace();
    if (GetAlpha() == FALSE)      // Grammar Set Name
      break;
    for (i=0; i < TheNumGramSets; i++)
      if (strcmp(pGramSetsHdr->GramSetNames[i], lToken) == 0)
        break;
    if ( i >= TheNumGramSets)
    {
      Error("Undefined Grammar Set Name");
      break;
    }
    Ord[j++] = i;
    lFind(":");
    SkipWhiteSpace();
    if (GetAlpha() == FALSE)
      break;

    for (k=0; k < pGrSets[i].NumGrammars; k++)
      if (strcmp(pGrSets[i].pGrammars[k], lToken) == 0)
        break;
    if ( k >= pGrSets[i].NumGrammars)
    {
      Error("Undefined Grammar");
      break;
    }
    Ord[j++] = k;

    SkipWhiteSpace();
    if (lCurChar != ',')
      break;
    GetChar();
  }
  SkipWhiteSpace();
  lFind(")");
  return TRUE;
}

char * * pGrammars = NULL;
int NumGrammars = 0;

int VBNFComp(LPSTR projname, FILE * pout, CRichEditCtrl * poutct)
{
  
  char * * files = NULL;
//  FILE * deffile;
	char bnffile[256], base[256];
  char fname[1024], curdir[256];
  LPSTR ptr;
  BOOL use = FALSE;
  int i, j=0;
  char * p;
  BIN_HEADER * binhdr = NULL;
  SPEECH_OBJECTS_HEADER sh;
  FILE * pcdfile;
  FILE * binfile;
	FILE * inifile = NULL;
	int NumINIFiles = 0;
	pgramatts = (BYTE *) calloc(NumGrams * sizeof(BYTE), 1);

  ULONG Offset, size, hsize;
	time_t ct;

  char ExecDir[256];
	numstartrules = 0;
  pGramSetsHdr = NULL;

	FreeTemplates();
  ReportFile = pout;
  preport = poutct;

  GetCurrentDirectory(256, (LPSTR)ExecDir);
  strcat(ExecDir,"\\"); 

	GetFullPathName(projname, sizeof(bnffile),thepath, &ptr);

  strcpy(base, ptr);
  *ptr = 0;

	sprintf(fname, "%s%s.vbin", thepath, base);

	if(MakeFileWritable(fname) == FALSE)
	{
		sprintf(repm, "Unable to create VBIN file: %s\n", fname);
		Report(repm);
		goto cleanup;
	}

	// Create the master BIN file
  sprintf(bnffile, "%s%s.tbin", thepath, base);
	MasterFile = fopen(bnffile, "wb");
  if (MasterFile == NULL)
  {
    sprintf(repm, "Unable to create Grammar file: %s\n", bnffile);
    Report(repm);
    goto cleanup;
  }
	pMasterHdr = (MASTER_HEADER *)calloc(1, sizeof(MASTER_HEADER));
	pMasterHdr->Version = SaveVersion;
	pMasterHdr->LangID = Language;
	pMasterHdr->ModelID = Model;
	pMasterHdr->Oem = gOEMID;
	pMasterHdr->VAPType = gProjectType;

  sprintf(bnffile, "%s%s.vdef", thepath, base);

  lCurChar = ' ';
  *opLineBuf = 0;
  level = MAXNESTING - 2;

  if( lifile[level].Open(bnffile, CFile::modeRead) == 0)
  {
    sprintf(repm,"Unable to open Project DEF File: %s\n", bnffile);
    Report(repm);
    goto cleanup;
  }

//	Report("Vangard AccuSPEECH Studio Project Builder\n");
	//Report(repm);

	sprintf(repm, "Target Recognizer:  %s\n\n", RecognizerName[(int)Recognizer]);
  Report(repm);
  while(TRUE)     // parse the grammar sets, one by one
  {
    SkipWhiteSpace();
    if (Declarative())
      continue;

		if (lCurChar == EOD)
			break;		// no more VDEF data

    if (GetAlpha() == FALSE)
      break;
    strupr(lToken);

    if (strcmp(lToken, "GRAMMARSET") != 0)
      break;

    if (GetAlpha() == FALSE)
      break;
    p = lToken;
    sprintf(repm,"Processing Grammar Set %s:\n\n", p);
    Report(repm);

    pGramSetsHdr = (GRAMMAR_SETS_HEADER *) realloc(pGramSetsHdr, sizeof(GRAMMAR_SETS_HEADER) +
        (TheNumGramSets+1)* strlen(p));
    strcpy(pGramSetsHdr->GramSetNames[TheNumGramSets], p);

    pGrSets = (GRSETS *) realloc(pGrSets, (TheNumGramSets + 1) * sizeof(GRSETS));

		if (CreateRecognizerFiles(0) == FALSE)
			goto cleanup;

    SkipWhiteSpace();
    lFind("(");

    NumGrammars = j = 0;      // start grammars at zero
    pGrammars = NULL;         // saved grammar names for this grammar set

    while (TRUE)
    {
      if (lCurChar == ')')
        break;
      if (GetAlpha() == FALSE)
        break;
      
      // add in a grammar name, with duplicates allowed
      pGrammars = (char * *) realloc(pGrammars, (NumGrammars + 1) * sizeof(char *));
      pGrammars[NumGrammars] = (char *) calloc(1, strlen(lToken)+1);
      strcpy(pGrammars[NumGrammars++], lToken);

			strcpy(bnffile, thepath);
      strcat(bnffile, lToken);
      j++;
      files = (char * *) realloc(files, sizeof(char *) * j);
      files[j-1] = (char *) calloc(1, strlen(bnffile) + 1);
      strcpy(files[j-1], bnffile);

			if (!pgramparent || pgramparent[j-1] == 0)
			{
				//int oln = lLineNumber[level];
				level = 0;
				char ch = lCurChar;
				char * tpch = opLineBuf;
				strcpy(fname, opLineBuf);
				Comperrs += Compile(j, bnffile);
				level = MAXNESTING - 2;
				//lLineNumber[level] = oln;
				opLineBuf = tpch;
				strcpy(opLineBuf, fname);
				lCurChar = ch;
			};

      SkipWhiteSpace();
      if (lCurChar != ',')
        break;
      GetChar();
      SkipWhiteSpace();
    }
    lFind(")");
    SkipWhiteSpace();
    lFind(";");

		if (NumGrammars == 0)
    {
      Report("\nThere are no Grammars defined\n");
      Comperrs += 1;
    }
    TotalComperrs += Comperrs;
    if (Comperrs)
    {
      sprintf(repm,"\nErrors Encountered : %i\n\n", Comperrs); 
      Report(repm);

      Comperrs = 0;
      goto done;
    }

    ErrorCount = 0;
    Report("\nCompiling Target Grammars:\n\n");

		if (CreateRecognizerFiles(1) == FALSE) 
			goto cleanup;

  // Now generate the ".vbin" tables
    SetCurrentDirectory(curdir);

    if (binhdr)
      free(binhdr);
    binhdr = (BIN_HEADER *) calloc(1, sizeof(DWORD)*j + sizeof(BIN_HEADER));
    binhdr->NumGrammars = j;

		// Generate the project INI file
    sprintf(bnffile, "%s%s.ini", thepath, ProjName);

	if(MakeFileWritable(bnffile) == FALSE)
    {
      sprintf(repm, "Unable to create INI file: %s\n", bnffile);
      Report(repm);
      goto cleanup;
    }

		inifile = fopen(bnffile, "w");
    if (inifile == NULL)
    {
      sprintf(repm, "Unable to create INI file: %s\n", bnffile);
      Report(repm);
      goto cleanup;
    }
		fprintf(inifile, "[AccuSPEECH]\n");

    sprintf(bnffile, "%s%s.vbin", thepath, pGramSetsHdr->GramSetNames[TheNumGramSets]);
		ct = time(NULL);
		fprintf(inifile, "BuildID=%i\n", ct);
		fprintf(inifile, "CfgFileName=%s.vbin\n", ProjName);
		fprintf(inifile, "FILE000=%s.vbin\n", ProjName);
		if (!MasterFile)
			goto cleanup;
		fgetpos(MasterFile, (fpos_t *) &pMasterHdr->PcodeBinOffset);

		pMasterHdr->PcodeBinOffset++;

    binfile = fopen(bnffile, "wb");
    if (binfile == NULL)
    {
      sprintf(repm, "Unable to create BIN file: %s\n", bnffile);
      Report(repm);
      goto cleanup;
    }
    hsize = sizeof(BIN_HEADER) + (j-1)*sizeof(ULONG);
    Offset = hsize;
    fwrite(binhdr, 1, hsize, binfile);
    for (i=0; i < j; i++)
    {
			if (pgramparent && pgramparent[i])
			{
				// Grammar is contained in parent grammar file - use its offset
				binhdr->Offsets[i] = binhdr->Offsets[pgramparent[i]-1];
				continue;
			}
      sprintf(bnffile, "%s.vpc", files[i]); 
      pcdfile = fopen(bnffile, "rb");
      if (pcdfile == NULL)
      {
        sprintf(repm,"Unable to open pcd file: %s\n", bnffile);
        Report(repm);
        goto cleanup;
      }
      fseek(pcdfile, 0, SEEK_END);
      size = ftell(pcdfile);
      fseek(pcdfile, 0, SEEK_SET);
      p = (char *) calloc(1, size);
      fread(p, 1, size, pcdfile);
      fwrite(p, 1, size, binfile);
      free(p);
      fclose(pcdfile);
      binhdr->Offsets[i] = Offset;

      while ((Offset + size) & 3)     // make pcode count long-bounded
      {
        fwrite(p, 1, 1, binfile);
        ++size;
      }
      Offset += size;
    }
    fseek(binfile, 0, SEEK_SET);
    fwrite(binhdr, 1, hsize, binfile);
    fseek(binfile, 0, SEEK_END);
    fclose(binfile);
done:
    pGrSets[TheNumGramSets].NumGrammars = NumGrammars;
    pGrSets[TheNumGramSets].pGrammars = pGrammars;
    ++TheNumGramSets;
  };    // end of Grammar Sets loop

  if (binhdr)
    free(binhdr);
  binhdr = NULL;

	if (TotalComperrs)		// Done if there were errors
		goto cleanup;

	// Now concat the pcode bin file to the master
  sprintf(bnffile, "%s%s.vbin", thepath, pGramSetsHdr->GramSetNames[0]);
	LPSTR pbin = (LPSTR) LoadFile(bnffile);
	if (pbin)
	{
		fwrite(pbin, 1, filelen, MasterFile);
		free(pbin);
	}
	DeleteFile(bnffile);

// Process Speech Object Commands
// Create the ".sbin" file

  Report("\nProcessing Speech Object Definitions:\n"); 

	if (!MasterFile)
		goto cleanup;
	fgetpos(MasterFile, (fpos_t *) &pMasterHdr->SpeechObjData); 

	/*
  sprintf(bnffile, "%s%s.sbin", thepath, base);
  deffile = fopen(bnffile, "wb");
  if (deffile == NULL)
  {
    sprintf(repm,"Unable to create sbin file: %s\n", bnffile);
    goto cleanup;
  }
	*/

	//if (inifile)
	//	fprintf(inifile, "FILE%03i=%s.sbin\n", NumINIFiles++, base);

  while (lCurChar && lCurChar != EOD)
  {
		if (lifile[level].m_pStream == 0)
			break;
    memset(&sh, 0, sizeof(sh));
    SkipWhiteSpace();
    strupr(lToken);
    strcpy(fname, lToken);
    if (GetAlpha() == FALSE)
      break;
    if (Translate() == FALSE)      // resource ID number
		{
			char t[256];
			sprintf(t,"Undefined Name: %s", lToken); 
      LError(t);
		}
    sh.ResID = atoi(lToken);
    SkipWhiteSpace();

    if (lCurChar == ',')
    {
      GetChar();
      SkipWhiteSpace();
      if (GetAlpha() == FALSE)
      {
        Error("Missing 'Grammars' or 'Type' clause");
        break;
      }
    }

    if (strcmp(fname, "DIALOG") == 0)
    {
      DoGramClause(&sh);
      SkipWhiteSpace();
      lFind(";");
      sh.ctype = C_DIALOG;
    }
    else if (strcmp(fname, "CONTROL") == 0)
    {
      if (DoGramClause(&sh))
      {
        SkipWhiteSpace();
        lFind(",");
        if (GetAlpha() == FALSE)
          break;
      }
      strupr(lToken);
      if (strcmp(lToken, "TYPE") != 0)
      {
        Error("Missing TYPE clause");
        break;
      }

      SkipWhiteSpace();
      if (GetAlpha() == FALSE)
        break;
      for (i=0; i < NUMCONTROLTYPES; i++)
        if (strcmp(VControlNames[i], lToken) == 0)
          break;
      if ( i >= NUMCONTROLTYPES)
      {
				char ts[256];
				sprintf(ts, "Invalid Control Type : %s", lToken);
        Error(ts);
        break;
      }
      sh.ctype = (VCONTROLTYPE) i;
      SkipWhiteSpace();
      lFind(";");
    }
    fwrite(&sh, 1,sizeof(SPEECH_OBJECTS_HEADER), MasterFile);
    fwrite(Ord, 1, 2*sh.numgrammars * sizeof(WORD), MasterFile);
    SkipWhiteSpace();
    if (lCurChar == EOD)
      break;
    if (GetAlpha() == FALSE)
      break;
  }
//  fclose(deffile);

  Report("\nProject Build completed\n");
//  lifile[level].Close();

	fgetpos(MasterFile, (fpos_t *) &pMasterHdr->RuleXlatData);

	/*
  sprintf(bnffile, "%s%s.vbin", thepath, base);
  deffile = fopen(bnffile, "wb");
  if (deffile == NULL)
  {
    sprintf(repm,"Unable to create vbin file: %s\n", bnffile);
    Report(repm);
    goto cleanup;
  }
	*/

	//if (inifile)
	//	fprintf(inifile, "FILE%03i=%s.vbin\n", NumINIFiles++, base);
  if (pGramSetsHdr)
  {
		int ii;
    pGramSetsHdr->NumGramSets = TheNumGramSets;
//    fwrite(pGramSetsHdr, 1, sizeof(GRAMMAR_SETS_HEADER) + (TheNumGramSets-1) * MAXGRAMSETNAME, MasterFile);
// Now output the pre-rec semantics, if any
		fwrite(&numstartrules, 1, sizeof(int), MasterFile);
		for (ii=0; ii < numstartrules; ii++)
			fwrite(&prerecsemantics[ii], 1, sizeof(int), MasterFile);
		for (ii=0; ii < numstartrules; ii++)
			fwrite(&startrulexlate[ii], 1, sizeof(int), MasterFile);
  }
//  fclose(deffile);


  // cleanup all files

  
  SetCurrentDirectory(thepath); 

  TotalComperrs += ErrorCount;
	sprintf(repm,"\nNumber of Grammars defined : %i\n", numstartrules);
	Report(repm);
  if (TotalComperrs)
    sprintf(repm,"\nTotal Errors Encountered: %i\n", TotalComperrs);
  else
    sprintf(repm,"\nNo Errors Encountered\n");
  Report(repm);

cleanup:

  SetCurrentDirectory(thepath); 

  system("del *.vpc");
//  system("del *.BE");
	system("del *.bnf");
//  system("del *.fsg");
//	system("del *.fst");
//  system("del *.LE");
//  system("del resbldvocab"); 


  /*
  sprintf(bnffile, "cleanup.bat %s", thepath);
  rc = Launch(bnffile, FALSE);
  WaitOnProcess();
  */
	if (AddedGrammars)
	{
				MessageBox(NULL, "Grammars have been added. A re-build is required!","Added Grammars", MB_OK);
				Report("\n*** Grammars have been added. A re-build is required!\n");
				TotalComperrs += 1;
	}
		if (NumGrams != numstartrules)
		{ // check for missing grammar
			int n;
			TotalComperrs += 1;
			MessageBox(NULL, "Grammars have been deleted. A re-build is required!","Deleted Grammars", MB_OK);
			Report("\n*** The following Grammars are missing and have been deleted from the project:\n\n  ");
			n = 0;
			while (n < NumGrams)
			{
				if (pgramatts[n] == 0)
				{
					int old = NumGrams;
					Report(pgramnames[n]);
					// now delete the grammar
					if (pgramparent[n])
						DoAdjustGrammars(n);		// Control grammar inside a dialog grammar
					else  // Dialog Grammar - manual delete
					{
						Report("  Manual delete of this Grammar is required");
					}
					Report("\n  ");
					if (old != NumGrams)		// Grammers were deleted?
					{
						if(n < (old-1))
							memcpy(pgramatts+n, pgramatts+n+1, (old-n-1)*sizeof(BYTE));
						n = -1;
					}
				};
				n++;
			}
			Report("\n*** A re-build is required!\n");
			ShowProjWindow();
			PModified = TRUE;
		}

	free(pgramatts);
	pgramatts = NULL;
	if (MasterFile)
	{
		fpos_t lo;
		fgetpos(MasterFile, (fpos_t *) &lo);
		pMasterHdr->LastOffset = (ULONG)lo;
		fseek(MasterFile, 0, SEEK_SET);
		pMasterHdr->hash = ComputeHeaderHash((unsigned char *)pMasterHdr, sizeof(MASTER_HEADER));
		
		i = (pMasterHdr->hash >> 24) & 0xFF;
		token[0] = (unsigned char)i;
		i = (pMasterHdr->hash >> 16) & 0xFF;
		token[1] = (unsigned char)i;
		i = (pMasterHdr->hash >> 8) & 0xFF;
		token[18] = (unsigned char)i;
		i = (pMasterHdr->hash) & 0xFF;
		token[19] = (unsigned char)i;

		pMasterHdr->hash = ComputeHeaderHash((unsigned char*)&token, 20);

		fwrite(pMasterHdr, 1, sizeof(MASTER_HEADER), MasterFile);
		free(pMasterHdr);
		fclose(MasterFile);
		sprintf(bnffile, "%s%s.tbin", thepath, base);
		sprintf(fname, "%s%s.vbin", thepath, base);

		if (TotalComperrs)
		{
			i = DeleteFile(bnffile);
		}
		else
		{
			i = DeleteFile(fname);
			i = rename(bnffile, fname);
		}
	}

  while (lCurChar && lCurChar != EOD)     // make sure all input files are closed
    GetChar();

  SetCurrentDirectory(ExecDir);

  if (litindex)
  {
    for (i=0; i < litindex; i++)
    {
      free(plits[i]);
    }
    free(plits);
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
  }
  if (NumRules)
  {
    for (i=0; i < NumRules; i++)
    {
      free(prules[i]);
    }
    free(prules);
    free(pruleatts);
  }

  plits = NULL;
  pdefines =  NULL;
  pvalues =  NULL;
  prules = NULL;
  pruleatts = NULL;
  litindex = level = NumDefines = NumRules = ErrorCount = vdelay = InHFile = 0;
	GlobalIndex = SharedGlobalIndex = 0;
	free(pGramSetsHdr);
  pGramSetsHdr = NULL;

  for (i=0; i < TheNumGramSets; i++)
    free(pGrSets[i].pGrammars);
  free(pGrSets);
  pGrSets = NULL;

  i = TotalComperrs;


  AddedGrammars = TotalComperrs = Comperrs = TheNumGramSets = 0;

	if (inifile)
	{
		fprintf(inifile, "NumberFiles=1\n");
		fclose(inifile);
	}
  if (PModified)
    SaveProject();
	return i;
}

extern void ShowResults(LPSTR Title);
extern CRichEditCtrl * poutct;

void DisplayOpcodes()
{
	CString reps;
	int i,j;
	ShowResults("Semantic Functions");
  ReportFile = NULL;
  preport = poutct;
	Report("The following Semantic Function calls are available:\n\n");
	for (i=0; i < NUMFUNCTIONS; i++)
	{
		reps = " ";
		if (FuncDescription[i].Name)
		{
			if (FuncDescription[i].ReturnResult)
				reps = "*";
			reps += (CString)FuncDescription[i].Name + "(";
			for (j=0; j < FuncDescription[i].NumArgs; j++)
			{
				if (j)
					reps += ",";
				reps += "arg";
			}
			reps += ")\n";
			Report(reps.GetBuffer(0));
		}
	}
	Report("\n*  -- Indicates operation returns a result.\n");
}

// StudioFile.cpp : implementation file
//

//#include "stdafx.h"
//#include "AccuStudio.h"
//#include "StudioFile.h"


// CStudioFile

IMPLEMENT_DYNCREATE(CStudioFile, CStudioFile)


CStudioFile::CStudioFile()
	: CStdioFile()
{
	InTemplate = FALSE;
}

CStudioFile::~CStudioFile()
{
}

extern char * opLineBuf;
extern char lCurChar;

void CStudioFile::SetTemplate(CString& rString)
{
	olCurChar = lCurChar;
	lopLineBuf = opLineBuf;
	Template = rString;
	InTemplate = 1;
}

BOOL CStudioFile::IsInTemplate(void)
{
	return InTemplate;
}


void CStudioFile::Close()
{
	if (!InTemplate)
		CStdioFile::Close();
	else
		Template.Empty();
	InTemplate = 0;
}

void CStudioFile::DoDataExchange(CDataExchange* pDX)
{
	DoDataExchange(pDX);
}

LPSTR CStudioFile::GetStringPtr(void)
{
	return Template.GetBuffer(0);
}

BOOL CStudioFile::GetData(void)
{
	BOOL rc;
	if (InTemplate)
	{
		lCurChar = olCurChar;
		opLineBuf = lopLineBuf;
		return FALSE;
	}
	rc = ReadString(Template);
	if (rc == FALSE)
	{
		opLineBuf = Template.GetBuffer(0);
		*opLineBuf = 0;
	}
	return rc;
}	


BOOL CStudioFile::ReadString(CString& rString)
{
	if (!InTemplate)
		return CStdioFile::ReadString(rString);
	return TRUE;
}


BOOL CStudioFile::Open(LPCTSTR lpszFileName, UINT nOpenFlags,
		CFileException* pError)
{
	if (!InTemplate)
		return CStdioFile::Open(lpszFileName, nOpenFlags, pError);
	return TRUE;
}

BEGIN_MESSAGE_MAP(CStudioFile, CStudioFile)
END_MESSAGE_MAP()


// CStudioFile diagnostics

#ifdef _DEBUG
void CStudioFile::AssertValid() const
{
	CStdioFile::AssertValid();
}

#ifndef _WIN32_WCE
void CStudioFile::Dump(CDumpContext& dc) const
{
	CStdioFile::Dump(dc);
}
#endif
#endif //_DEBUG


// CStudioFile message handlers
