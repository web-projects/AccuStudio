/*
Compiler modules for emitting Vaivoice BNF files
*/
#include "stdafx.h"
#include "compilerModules.h"
#include "AccuSpeech.h" 
#include "PCode.h"
//#include "\IBM\evv4.3\include\common\voc.h"
#define REC_POOL_PATH "ESR\\ViaVoice\\Vocabs\\langs\\EAK01AGE\\pools\\"

extern CString DirPath;        // Project directory path name
extern DWORD ecode;
extern void vout(CString os);
extern   int NumGrammars;
extern CString ExecDir;
extern CString ProjName; 
extern char thepath[];
extern CString path;
extern int TheNumGramSets;
extern GRAMMAR_SETS_HEADER * pGramSetsHdr;
extern BOOL SubInFile(LPSTR filename, LPSTR poriginal, LPSTR pnew);
extern void Report(LPSTR msg);
extern char * * pGrammars;
extern BOOL Launch(LPSTR lpName, BOOL ShareConsole);
extern void WaitOnProcess();
extern void * LoadFile(LPSTR pname);
extern long filelen;
extern int FindGramName(LPSTR pname);
extern BYTE * pgramparent;

void ViaVoice_OutputCommentLine(LPSTR pbody)
{
	vout(CString(pbody) + "\n");
}

void ViaVoice_StartSubexpression()
{
	vout("(");
}

void ViaVoice_StopSubexpression()
{
  vout(")");
}

void ViaVoice_OutputOrOp()
{
	vout(" | ");
}

void ViaVoice_OutputIntegerAnnotation(DWORD an)
{
	char s[256];
	sprintf(s, ": %i ", an);
	vout(s);
}

void ViaVoice_OutputRuleName(LPSTR pname)
{
  vout("<" + CString(pname) + ">");
}

void ViaVoice_StartOptionalClause()
{
}

void ViaVoice_EndOptionalClause()
{
    vout("?");
}

void ViaVoice_OutputRuleHeader(int RuleIndex)
{
	vout('{');
}

void ViaVoice_OutputRuleTrailer(int RuleIndex)
{
	char s[256];
	sprintf(s, " } -> ~s%i {1} ~e%i", RuleIndex, RuleIndex);
	vout(s);
  vout(" . \n");
}

void ViaVoice_OutputOneOrMore()
{
	vout("+");
}

void ViaVoice_OutputZeroOrMore()
{
	vout("*");
}

//extern int GetGramIndex(LPSTR pname);

void ViaVoice_OutputRuleDefinition(BOOL startrule, LPSTR pname, int g_gramindex, int RuleIndex)
{
  if (startrule)
	{
		char tt[256];
		int i = FindGramName(pname)+ 1;
		if (i == 0 && !pgramparent)
			i = g_gramindex;
		sprintf(tt, "<<%i_%s>> = ", i, pname);
    vout(tt);
	}
  else
    vout("<" + CString(pname) + "> = ");
 }

void ViaVoice_ValidateStringChar(char & thechar)
{
}

void ViaVoice_OutputQuotedString(LPSTR str)
{
	vout("\"");
	vout(str);
	vout("\"");
}

static char ibmdef[4096], curdir[256], fname[256];

#define INLINE_COMPILER 

BOOL ViaVoice_CreateRecognizerFiles(int phase)
{
#ifndef INLINE_COMPILER
	int rc;
	int i;
 	FILE * ibmdeffile;

	if (phase == 0)
	{
		// make sure we have a pool file for this project
		sprintf(ibmdef, "%s%s%s.ppl", 
			ExecDir, REC_POOL_PATH, ProjName);
		ibmdeffile = fopen(ibmdef, "rb"); 
		if (ibmdeffile == NULL)
		{
			CopyFile("RuleLib\\vanwords.ppl", ibmdef, FALSE);
		}
		else
			fclose(ibmdeffile);

		sprintf(ibmdef, "%svocabs.def", thepath);      // create the vocabs.def file
		ibmdeffile = fopen(ibmdef, "w");
		fprintf(ibmdeffile, "inputfiles= temp_voc.le:%i\n", (TheNumGramSets+1) * 1000);
		fprintf(ibmdeffile, "outputname=%s.bin\n", pGramSetsHdr->GramSetNames[TheNumGramSets]);
		fclose(ibmdeffile);
		sprintf(ibmdef, "%s%s.def", thepath, pGramSetsHdr->GramSetNames[TheNumGramSets]);
	//    sprintf(bnffile, "MakeIbmDef.bat %s", ibmdef);

		CopyFile("ibmtemplate.def", ibmdef, FALSE);
		SubInFile(ibmdef, "%1", ExecDir.GetBuffer(0));
		SubInFile(ibmdef, "%projname", ProjName.GetBuffer(0));
		return TRUE;
	}

  ibmdeffile = fopen(ibmdef, "a+");
  if( (ibmdeffile == 0))
  {
		char repm[256];
    sprintf(repm,"Unable to open IBM Project DEF File: %s\n", ibmdef);
    Report(repm);
    return FALSE;
  }
	for (i=0; i < NumGrammars; i++)
		fprintf(ibmdeffile, "%i_%s.bnf\n", i+1, pGrammars[i]);
	fclose(ibmdeffile);


  // First run the IBM Compiler
  GetCurrentDirectory(256, curdir);
  sprintf(ibmdef, "%s.def", pGramSetsHdr->GramSetNames[TheNumGramSets]);

  /*
  SetCurrentDirectory(thepath);
  sprintf(fname, "%sbldvocab.exe %s 1> resbldvocab 2> nul", ExecDir, ibmdef);
  */
  
  sprintf(fname, "bldvocab.bat %s %s %s", thepath, ibmdef, ExecDir);

  rc = Launch(fname,TRUE);
	if (rc == FALSE)
	{
		Report("Unable to launch IBM BNF Compiler\n");
		return FALSE;
	}
	WaitOnProcess();
  

  SetCurrentDirectory(curdir);
  if (ecode)      // errors - must be missing words ...
  {

    DeleteFile("newwords");
    // Now run our analysis tool to find missing words
    sprintf(fname, "GenBaseForm.exe %sresbldvocab", thepath);
    rc = Launch(fname, TRUE);
	  if (rc == FALSE)
	  {
		  Report("Unable to launch GenBaseForm analysis tool\n");
		  return FALSE;
	  }
	  WaitOnProcess();
    if (ecode < 0)
    {
		  Report("Unable to load 'resbldvocab' file\n"); 
		  return FALSE;
    }
    if (ecode == 0)
    {
      char name[256];
      sprintf(name, "%sresbldvocab", thepath);
		  Report("IBM Vocabulary Build Errors:\n*****\n");
      FILE * inf = fopen(name, "r");
      if (inf == NULL)
      {
		    Report("Error Opening resbldvocab File\n"); 
      }
      while (inf)
      {
        if (fgets(name, sizeof(name),inf) == NULL)
          break;
        Report(name);
      }
      Report("*****\n");
      fclose(inf);
		  return FALSE;
    }
//      if (ecode == 1)   // missing words found
    {
      char data[256];

      CopyFile("vanwordstemplate.def", "vanwords.def", FALSE);
      SubInFile("vanwords.def", "%exedir", ExecDir.GetBuffer(0));
      SubInFile("vanwords.def", "%projname", ProjName.GetBuffer(0));
      SubInFile("vanwords.def", "%projdir", DirPath.GetBuffer(0));
      FILE * inf = fopen("newwords", "r");
      sprintf(data, "%s%s.pbsp", DirPath, ProjName); 
      FILE * outf = fopen(data, "a+");
//        FILE * outf = fopen(data, "w+");
      if (outf == NULL)
        outf = fopen("vanwords.pbsp", "w");

      if (inf == NULL || outf == NULL)
      {
		    Report("Error Opening Missing Words File\n"); 
		    return FALSE;
      }

      Report("\nMissing vocabulary words have been added:\n\n");
      fprintf(outf, "\n");
      while (TRUE)
      {
        if (fgets(data, sizeof(data),inf) == NULL)
          break;
        Report(data);
        fprintf(outf, data);
      }
      fclose(inf);
      fclose(outf);
      Report("\n");

      rc = Launch("bldwords.exe vanwords.def",TRUE);
	    if (rc == FALSE)
	    {
		    Report("Unable to launch IBM BldWords Utility\n"); 
		    return FALSE;
	    }
	    WaitOnProcess();
      if (ecode)
      {
		    Report("BldWords Utility Error\n"); 
		    return FALSE;
      }

      sprintf(fname, "bldvocab.bat %s %s %s", thepath, ibmdef, ExecDir);
      rc = Launch(fname,TRUE);
	    if (rc == FALSE)
	    {
		    Report("Unable to launch IBM BNF Compiler\n");
		    return FALSE;
	    }
	    WaitOnProcess();
      if (ecode)
      {
		    Report("Error compiling generated IBM BNF\n"); 
		    return FALSE;
      }
    }
  }
//    fprintf(rep,"\n\nBuilding ViaVoice BIN File: \n\n");
  SetCurrentDirectory(thepath);
  sprintf(fname, "%sbldrom.exe vocabs.def", ExecDir);
  rc = Launch(fname, FALSE);
	if (rc == FALSE)
	{
		Report("Unable to launch IBM bldrom utility\n");
		return FALSE;
	}
	WaitOnProcess(); 
  if (ecode)
  {
		Report("bldrom Utility Error\n"); 
		return FALSE;
  }

	//  New Inline - Compiler code
 
#else // not INLINE_COMPILER
/*
	int i;
 	FILE * ibmdeffile;
	if (phase == 0)
		return TRUE;

	void * p;
	ecode = OpenCompiler();
	if (ecode)
	{
		sprintf(ibmdef, "Compiler Initialization Error: %i\n", ecode);
		Report(ibmdef);
		return FALSE;
	}
	while (TRUE)
	{
		for (i=0; i < NumGrammars; i++)
		{
			char data[256];
			if (pgramparent && pgramparent[i])
				continue;
			sprintf(data, "%s%i_%s.bnf", thepath, i+1, pGrammars[i]);
			p = LoadFile(data);
			ecode = AddCompilerFile((LPSTR)p, filelen); 
			if (ecode)
			{
				sprintf(ibmdef, "ViaVoice Compiler Syntax Error: %i\n", ecode);
				Report(ibmdef);
				return FALSE;
			}
			free(p);
		}
		ecode = DoCompilation(thepath, ProjName.GetBuffer(0), ibmdef);
		if (ecode != -1)
			break;
		LPSTR pp, q;
		q = pp = ibmdef;
		Report("Missing Vocabulary words have been added:\n\n");
		while (*pp && q)
		{
			q = strchr((LPSTR)pp, '\n');
			if (q)
				*q = 0;
			Report("\t");
			Report(pp);
			Report("\n");
			if (q)
				pp = q+1;
		}
		Report("\n");
	}
	if (ecode)
	{
		sprintf(ibmdef, "Internal IBM Compiler error: %i\n", ecode);
		Report(ibmdef);
		return FALSE;
	}
	sprintf(ibmdef, "%stemp_voc.le", DirPath);      
	ecode = CloseCompiler(ibmdef);
	if (ecode)
	{
		sprintf(ibmdef, "Problem closing IBM Compiler: %i\n", ecode);
		Report(ibmdef);
		return FALSE;
	}
	sprintf(ibmdef, "%svocabs.def", DirPath);      // create the vocabs.def file
	ibmdeffile = fopen(ibmdef, "w");
	fprintf(ibmdeffile, "inputfiles= temp_voc.le:%i\n", (TheNumGramSets+1) * 1000);
	fprintf(ibmdeffile, "outputname=%s.bin\n", pGramSetsHdr->GramSetNames[TheNumGramSets]);
	fclose(ibmdeffile);
	SetCurrentDirectory(DirPath);
  sprintf(fname, "%sbldrom.exe vocabs.def", ExecDir);
  int rc = Launch(fname, FALSE);
	if (rc == FALSE)
	{
		Report("Unable to launch IBM bldrom utility\n");
		return FALSE;
	}
	WaitOnProcess(); 
  if (ecode)
  {
		Report("bldrom Utility Error\n"); 
		return FALSE;
  }
	*/
#endif
	return TRUE;
}

void ViaVoice_OutputGrammarHeader(LPSTR name)
{
}
