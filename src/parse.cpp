#include "stdafx.h"
#include "parse.h"
#include "project.h"
#include "common.h"

int vdelay = 0;
BOOL IsSysInclude;
CStdioFile ifile[MAXNESTING];
FILE * ibmfile[MAXNESTING];
int LineNumber[MAXNESTING];
int plevel = 0;
char EOD = 1;
BOOL InHFile;
static char Namechars[] = { '-', '_', '.' };

CString LineBufStr;
char LineBuf[256];
char Token[256];
char * pLineBuf = LineBuf;
char CurChar = ' ';
CString path;
extern CString ExecDir;
extern BOOL aisalpha(char cc);

void CloseParser()
{
  if (ifile[0].m_pStream)
    ifile[0].Close();
}

char TestChar()
{

loop:
  while ( plevel >= 0 && (*pLineBuf == 0))
  {
		if (ifile[plevel].m_pStream == 0)
			break;
    if (ifile[plevel].ReadString(LineBufStr) == 0)
    {
      ifile[plevel].Close();
      if (ibmfile[plevel] && ibmfile[plevel] != ibmfile[0])
        fclose(ibmfile[plevel]);
      plevel--;
      InHFile = FALSE;
      if (plevel >= 0 && ifile[plevel].m_pStream == NULL)
        return (CurChar = EOD);
//      Indent();
//      printf("\n");
    }
    else
    {
      strcpy(LineBuf, LineBufStr.GetBuffer(0));
      pLineBuf = LineBuf;
      LineNumber[plevel]++;
    }
  }
  if (plevel >= 0)
  {
    if (pLineBuf[0] == '/' && pLineBuf[1] == '/')   // ignore comments
    {
      *pLineBuf = 0;
      goto loop;
    }
    return (CurChar = *pLineBuf);
  }
  else
    return (CurChar = EOD);
}

char GetChar()
{
  char ch = TestChar();
  if (CurChar != EOD)
    pLineBuf++;
  return CurChar;
}


BOOL WhiteSpace(char ch)
{
  if (ch == ' ' || ch == '\t' || ch == '\n')
    return TRUE;
  return FALSE;
}

void SkipWhiteSpace()
{
  if (WhiteSpace(CurChar) == FALSE)
    return;
  GetChar();
  while (WhiteSpace(CurChar))
  {
    GetChar();
  }
}


BOOL GetAlpha()
{
  tokentype tt = GetToken();
  if (tt == ALPHA_TOKEN)
    return TRUE;
  return FALSE;
}

int GetNum()
{
  int i=0;

  while( isdigit(CurChar) )
  {
    Token[i++] = CurChar;
    CurChar = *pLineBuf++;
  }
  Token[i] = 0;
  if (CurChar == 0)
  {
    CurChar = ' ';
    --pLineBuf;
  }
  if (i == 0)
  {
    return -1;
  }
  i = atoi(Token);
  return i;
}


BOOL Alpha()
{
  if (aisalpha(CurChar) | isdigit(CurChar) | (CurChar == '_'))
  {
    int i=0;
    while(CurChar && (aisalpha(CurChar) || isdigit(CurChar) || strchr(Namechars, CurChar)))
    {
      Token[i++] = CurChar;
      CurChar = *pLineBuf++;
    }
    Token[i] = 0;
    if (CurChar == 0)
    { 
      CurChar = ' ';
      --pLineBuf;
    }
    return TRUE;
  }
  return FALSE;
}

tokentype GetToken()
{

  if ( CurChar == EOD)
    return END_TOKEN;

  while(WhiteSpace(CurChar))
    CurChar = GetChar();

  if (Alpha())
  {
    return ALPHA_TOKEN;
  }

  switch (CurChar)
  {
  case '=':        // rule definition
    return RULEDEF_TOKEN;

  default:
    return END_TOKEN;
  }
  return END_TOKEN;
}

BOOL OpenParser(CString Path, CString fname)
{
  path = Path;
  if( ifile[0].Open(Path + fname, CFile::modeRead) == 0)
    return FALSE;
  CurChar = ' ';
  pLineBuf = LineBuf;
  *pLineBuf = 0;
  return TRUE;
}

BOOL OpenInclude(CString fname)
{
  if (fname.Find(".h") > 0) // special processing for ".h" files
  {
    if( ifile[plevel+1].Open(fname, CFile::modeRead) == 0)
    {
      return FALSE;
    }
    ibmfile[plevel+1] = NULL;
    InHFile = TRUE;
  }
  else
  { // normal include file
    if( ifile[plevel+1].Open(fname + ".abnf", CFile::modeRead) == 0)
    {
      return FALSE;
    }

    ibmfile[plevel+1] = ibmfile[0];  // no include file for now
  }
  plevel++;
  LineNumber[plevel] = 0;
  *pLineBuf = 0;      // start fetch from include file
  CurChar = ' ';      // pretend whitespace to cause skip
  return TRUE;
}

BOOL DoInclude()
{

  SkipWhiteSpace();
  if (CurChar != '\"' && CurChar != '<')
    return FALSE;

  if (GetQuoted(CurChar) == FALSE)
    return FALSE;

  if (OpenInclude(ResPath + Token))
	{
		IsSysInclude = FALSE;
    return TRUE;
	}
  if (OpenInclude(ExecDir + Token))
  {
    IsSysInclude = TRUE;
    return TRUE;
  }
  return FALSE;
}

BOOL GetQuoted(char ch)
{
  int i=0;

  if (CurChar != ch)
    return FALSE;

  GetChar();
  while (*pLineBuf)
  {
    if (CurChar == ch)
      break;
    Token[i++] = CurChar;
    CurChar = *pLineBuf++;
  }
  if (CurChar != ch)
  {
    return FALSE;
  }
  Token[i] = 0;
  CurChar = ' ';
  return TRUE;
}

BOOL Find(char * pstr)
{
  while (*pstr && *pstr == CurChar)
  {
    pstr++;
    GetChar();
  }

  if (*pstr)
  {
    return FALSE;
  }
  return TRUE;
}

void SkipLine()
{
   *pLineBuf = 0;    // skip current line
   CurChar = ' ';
}



