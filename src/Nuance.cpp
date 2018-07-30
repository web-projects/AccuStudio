

#include "stdafx.h"
#include "compilerModules.h"
#include "AccuSpeech.h" 
#include "pcode.h"
#include "common.h"

#define RegVangardVoice "Software\\VANGARD VOICE"

extern MASTER_HEADER * pMasterHdr;
extern FILE * MasterFile;
extern Target_Models Model;
extern Target_Languages Language;

extern void vout(CString os);
extern void Report(LPSTR msg);
extern GRAMMAR_SETS_HEADER * pGramSetsHdr;
extern int g_gramindex, TotalComperrs;
extern CString ExecDir;

LPSTR LangCodes[Num_Lang] = {
	"enu",			// american english
	"spm",			// Mexican Spanish 
	"frf",			// French
	"ged",			// German
	"frc",			// French Canadian
	"ptp",			// Portuguese
	"ptb",			// Brazilian Portuguese
	"iti",			// Italian
	"eng",			// British English
	"spe",			// Spanish
	//"ena",			// Australian
	//"plp",			// Polish
	"grk",      // Greek
};

LPSTR LangNames[Num_Lang] = {
	"American English",			 
	"Mexican Spanish",			 
	"French",								 
	"German",								 
	"French Canadian",			 
	"Portuguese",			 
	"Brazilian Portuguese",			 
	"Italian",			 
	"British English",			 
	"Spanish",			 
	//"Australian",	
	//"Polish"
	"Greek",
};

LPSTR Models[Num_Model] = {	"acmod3200_car_enu_f16_full_lew2.dat", 
										"acmod3200_car_enu_f16_compact_lew2.dat", 
										"acmod3200_car_enu_f16_ultracompact_lew2.dat",
										NULL,
										"acmod3200_mobile_enu_f8_compact_lew2.dat",
										"acmod3200_mobile_enu_f8_ultracompact_lew2.dat"
};

LPSTR Model_Nickname[Num_Model] = {	"Full 16 Khz", 
										"Compact 16 Khz", 
										"Ultra-Compact 16 Khz",
										NULL,
										"Compact Mobile 8 Khz",
										"Ultra-Compact Mobile 8 Khz"
};

//BOOL BnfSpec = FALSE;

void Nuance_OutputCommentLine(LPSTR pbody)
{
	vout((CString) "//" + CString(pbody) + "\n");
};

void Nuance_StartSubexpression()
{
	vout("(");
};

void Nuance_StopSubexpression()
{
  vout(")");
};

void Nuance_OutputOrOp()
{
	vout(" | ");
};

void Nuance_OutputIntegerAnnotation(DWORD an)
{
	char s[256];
	sprintf(s, " !id(%i) ", an);
	vout(s);
};

extern int * pruleatts;

void Nuance_OutputRuleName(LPSTR pname, int index)
{
	char t[256];
	/*
	BOOL dyn = pruleatts[index] & (ISDYNAMIC | ISSTARTRULE);
	if (strcmp(pname, "NULL") && !dyn)
		sprintf(t, "<%i_%s>", g_gramindex, pname);
	else
	*/
		sprintf(t, "<%s>", pname);
  vout(t);
};

void Nuance_StartOptionalClause()
{
	vout("[");
};

void Nuance_EndOptionalClause()
{
	vout("]");
};

void Nuance_OutputRuleTrailer(int RuleIndex)
{
	char s[256];
	BOOL isdyn = pruleatts[RuleIndex] & ISDYNAMIC;
	if (isdyn == 0)
	{
#ifdef _v3code
		sprintf(s, " ) n_w !id(%i)", RULEEND | RuleIndex);
#else
		sprintf(s, " ) !action(\"~e%i\")", RuleIndex);
#endif
		vout(s);
	}
  vout(" ; \n");
};

void Nuance_OutputRuleHeader(int RuleIndex)
{
	BOOL isdyn = pruleatts[RuleIndex] & ISDYNAMIC;
	if (isdyn == 0)
		vout("(");
}

void Nuance_OutputOneOrMore()
{
	vout("+");
};

void Nuance_OutputZeroOrMore()
{
	vout("*");
};

void Nuance_OutputRuleDefinition(BOOL startrule,LPSTR pname, int lg_gramindex, int RuleIndex)
{
	char tt[256];
  if (startrule)
	{
		sprintf(tt, "!start <%s> ;\n", pname);
    vout(tt);
#ifdef _v3code
		sprintf(tt, "<%s> : n_w !id(%i)", pname, RULESTART | RuleIndex);
#else
		sprintf(tt, "<%s> : !action(\"~s%i\")", pname, RuleIndex);
#endif
		vout(tt);
		return;
	}
	if (pruleatts[RuleIndex] & ISDYNAMIC)
	{
		sprintf(tt, "!modifiable <%s> ;\n", pname);
		vout(tt);
		sprintf(tt, "<%s> : ", pname);
	}
	else
//		sprintf(tt, "<%i_%s> : !action(\"~s%i\")", g_gramindex, pname, RuleIndex);
#ifdef _v3code
		sprintf(tt, "<%s> : n_w !id(%i)", pname, RULESTART | RuleIndex);
#else
		sprintf(tt, "<%s> : !action(\"~s%i\")", pname, RuleIndex);
#endif
	vout(tt);
};

void Nuance_ValidateStringChar(char & thechar) 
{
};

void Nuance_OutputQuotedString(LPSTR str)
{
	vout("\"");
	vout(str);
	vout("\"");
};

// SpeechAPI Include
#include "facapi.h"

// Audioin Includes
#include "audioin.h"    // Used for: Audioin API component use
#include "audioinerr.h" // Used for: Audioin API component use error codes

//#include "asr_func.h"
//#include "events.h"

extern char thepath[];
extern void * LoadFile(LPSTR pname);
extern long filelen;
extern   int NumGrammars;
extern BYTE * pgramparent;
extern char * * pGrammars;

LHS_ERROR Compiler_Callback(LHS_TCHAR * perr,  LHS_USERDATA User)
{
	char es[256];
	wcstombs((LPSTR)&es, perr, sizeof(es));
	Report(es);
	return  LHS_OK ;
}

HKEY            hKeySF = NULL;

BOOL OpenKey(void)
{
  int rc;
	char            TempBuffer[512];
  if (hKeySF == NULL)
  {
	  strcpy (TempBuffer, RegVangardVoice);
	  rc = RegOpenKeyEx (
					     HKEY_LOCAL_MACHINE,	// name of open key
					     TempBuffer,	// subkey to open
					     0,		// reserved
					     KEY_ALL_ACCESS,	// security access desired
					     &hKeySF	// handle for the opened key
		  );
	  if (rc != ERROR_SUCCESS)
		  return FALSE;
  }
  return TRUE;
}


BOOL GetDefinedString(LPSTR pname, char * pr)
{
  int rc;
	DWORD           sizeOfDataVal,dwValueType;
	char            TempBuffer[512];
  if (OpenKey() == FALSE)
    return FALSE;
	sizeOfDataVal = sizeof(TempBuffer);
	rc = RegQueryValueEx (
						  hKeySF,	// name of open key
						  pname,	// value name to query
						  0,	// reserved
						  &dwValueType,	// value type
						  (unsigned char *) pr,	// value data
						  &sizeOfDataVal	// value data size
		);
	if (rc != ERROR_SUCCESS)
	  return FALSE;
  return TRUE;
}

int GetModelByName(LPSTR pmodel)
{
	int nmodel;
	for (nmodel = Full_Model; nmodel < Num_Model; nmodel++)
		if (Model_Nickname[nmodel])
			if (strcmp(pmodel, Model_Nickname[nmodel]) == 0)
				return nmodel;
	return -1;
}

int GetLangByName(LPSTR pLang)
{
	int lang;
	for (lang = English_Lang; lang < Num_Lang; lang++)
		if (strcmp(pLang, LangNames[lang]) == 0)
			return lang;
	return -1;
}

BOOL FindLanguage(Target_Languages nlang, int nmodel)
{
	char data[256];
//	LPSTR lp;

	if (GetDefinedString("Path", data))
	{
		strcat(data, "AccuSpeech_Runtime\\");
	}
	else
		strcpy(data, ExecDir.GetBuffer(0));

	if (!Models[nmodel])
		return FALSE;
	strcat(data, Models[nmodel]);
	LPSTR p = strstr(data, LangCodes[English_Lang]);
	if (!p)
		return FALSE;
	memcpy(p, LangCodes[nlang],3);
	void * pmodel = LoadFile(data);
	if (pmodel)
	{
		free(pmodel);
		return TRUE;
	}
	return FALSE;
}

int SpeechErrHandler ( LHS_ERROR Err, LPSTR title) 
{
  char szBuf[1024];
  LHS_TCHAR *szString;
  
  if( LHS_OK==Err )
    return 0;
  else 
	{
    szString = (LHS_TCHAR *)lhs_asrErrGetString (Err);
		wcstombs((LPSTR)&szBuf, szString, sizeof(szBuf));
		sprintf(szBuf,"%s: %S", title, szString);
		Report(szBuf);
		return 1;
  }
}

BOOL Nuance_CreateRecognizerFiles(int phase)
{
	int i;
// 	FILE * ibmdeffile;
	void * p, * pmodel;
	LPBYTE op;
	char data[256];
	char repm[256];
	short * gwp = NULL;
	long numgramwords = 0; 
	short *owp;
	void * pGrmBuff;
  LHS_ERROR      nErr = LHS_OK;         // Error returned by an API function.
  LHS_HSPEECH          hSpeech;
  ASR_HG2P       hG2P;                  // Handle to the G2P object.
  ASR_HLEX       hLex;                  // Handle to the Lexicon object.
	ASR_HGRM			hGrm[1024];
	ULONG GramSize;
//	FILE * gramfile;
	int NumGrms = 0;

	if (phase == 0)
	{
		//BnfSpec = FALSE;
		return TRUE;
	}

	if (GetDefinedString("Path", data))
	{
		strcat(data, "AccuSpeech_Runtime\\");
	}
	else
		strcpy(data, ExecDir.GetBuffer(0));
	strcat(data, Models[Model]);
	LPSTR lp = strstr(data, LangCodes[English_Lang]);
	if (lp)
		memcpy(lp, LangCodes[Language], 3);
	pmodel = LoadFile(data);
	if (pmodel == NULL)
	{
		sprintf(repm, "Missing Nuance Model File: %s\n", data);
		Report(repm);
		return FALSE;
	}
	
	sprintf(repm, "Using the Model: %s\n", data);
	Report(repm);
	
  nErr = lhs_Initialize( NULL, pmodel, filelen, &hSpeech );
	if (SpeechErrHandler(nErr, "lhs_Initialize"))
		return FALSE;
  nErr = lhs_asrCreateG2P (hSpeech, &hG2P);
	if (SpeechErrHandler(nErr, "lhs_asrCreateG2P"))
		return FALSE;
  nErr = lhs_asrCreateLex (hSpeech, &hLex);
	if (SpeechErrHandler(nErr, "lhs_asrCreateLex"))
		return FALSE;
  nErr = lhs_asrLexSetG2P (hLex, hG2P);
	if (SpeechErrHandler(nErr, "lhs_asrLexSetG2P"))
		return FALSE;
	fwrite(pMasterHdr, 1, sizeof(MASTER_HEADER), MasterFile);
	fgetpos(MasterFile, (fpos_t *) &pMasterHdr->PrecompGramOffset);

	pMasterHdr->PrecompGramOffset++;

	/*
	sprintf(data, "%s%s.bin", thepath, pGramSetsHdr->GramSetNames[0]);
	gramfile = fopen(data, "wb");
	if (!gramfile)
	{
		Report("Existing Bin File is read-only\n");
		return FALSE;  
	}
	*/

	for (i=0; i < NumGrammars; i++)
	{
		if (pgramparent[i])
			continue;
		sprintf(data, "%s%s.bnf", thepath, pGrammars[i]);
		p = LoadFile(data);
		if (p == NULL)
			break;
		owp = gwp = (short *)calloc(1, filelen*sizeof(short));
		op = (LPBYTE)p;
		for (int ii=0; ii < filelen; ii++)
			*owp++ = *op++;
		numgramwords = filelen;
		free(p);

		nErr = lhs_asrCreateGrmFromText ( hSpeech,
																		hLex,
																		gwp, numgramwords*2,
																	(		
																			ASR_OPT_OPTIMIZE | 
																			ASR_OPT_NO_NT_IN_MODIFICATIONS |
																			ASR_OPT_REMOVE_PHONETIC_INFO 
																	), 
																		(ASR_CBGRMERR)&Compiler_Callback, 0,
																		&hGrm[NumGrms] );
		if (nErr)
		{
			Report("\nNuance Grammar Compiler errors\n");
			TotalComperrs++;
		}
		else
		{
			nErr = lhs_asrGrmGetBinaryBuffer(hGrm[NumGrms], &pGrmBuff, &GramSize);
			fwrite(&GramSize, 1, sizeof(ULONG), MasterFile);
			fwrite(pGrmBuff, 1, GramSize, MasterFile);
			nErr = lhs_asrGrmFreeBinaryBuffer(hGrm[NumGrms], pGrmBuff);
			NumGrms++;
			nErr = lhs_asrGrmClose(&hGrm[NumGrms]);
		}
		free(gwp);
	}
	GramSize = 0;
	fwrite(&GramSize, 1, sizeof(ULONG), MasterFile);
//	fclose(gramfile);
/*
	if (!nErr)	
	{
		ASR_HCTX thecontext;
		LPVOID pmem;
		DWORD size, total = 0;
		int i;
		nErr = lhs_asrCreateCtxFromGrammarHandles(hSpeech, (const ASR_HGRM *)&hGrm[0], NumGrms, 
			ASR_CTX_OPTIMIZE , 
			NULL, 0, &thecontext);
		if (nErr)
		{
			LHS_TCHAR * therr = (LHS_TCHAR *) lhs_asrErrGetString(nErr);
			sprintf(data, "\n%S\n", therr);
			Report(data);
		}
		nErr = lhs_asrCtxGetBinaryBuffer(thecontext, LHS_FALSE, LHS_FALSE, &pmem, &size);
		nErr = lhs_asrCtxFreeBinaryBuffer(thecontext, pmem);
		nErr = lhs_asrCtxClose(&thecontext);
		for (i=0; i < NumGrms; i++)
		{
			nErr = lhs_asrCreateCtxFromGrammarHandles(hSpeech, (const ASR_HGRM *)&hGrm[i], 1, 
				ASR_CTX_OPTIMIZE , 
				NULL, 0, &thecontext);
			if (nErr)
			{
				LHS_TCHAR * therr = (LHS_TCHAR *) lhs_asrErrGetString(nErr);
				sprintf(data, "\n%S\n", therr);
				Report(data);
			}
			nErr = lhs_asrCtxGetBinaryBuffer(thecontext, LHS_FALSE, LHS_FALSE, &pmem, &size);
			total += size;
			nErr = lhs_asrCtxFreeBinaryBuffer(thecontext, pmem);
			nErr = lhs_asrCtxClose(&thecontext);
		}
		total = total;
	}
	*/

  nErr = lhs_asrLexClearG2P (hLex);
  nErr = lhs_asrLexClose (&hLex);
  nErr = lhs_asrG2PClose (&hG2P);	
  nErr = lhs_UnInitialize( &hSpeech );
	free(pmodel);
	return TRUE;
};

void Nuance_OutputGrammarHeader(LPSTR name)
{
	char t[256];
	//if (!BnfSpec)
	{
#ifndef _v3code
		vout("#BNF+EM V1.0;\n");
#else
		vout("#BNF+EM V1.1;\n");
#endif
		sprintf(t, "!grammar \"%s\";\n", name);
		vout(t);
	}
	//BnfSpec = TRUE;
#ifdef _v3code
	vout("!pronounce n_w \"##\";\n");
#endif
}

