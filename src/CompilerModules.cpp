/*
Compiler modules for emitting Vaivoice BNF files
*/

#include "stdafx.h"
#include "compilerModules.h"
#include "Nuance.h"

extern void vout(CString os);

void OutputCommentLine(LPSTR pbody)
{
	if (Recognizer == NUANCE3200)
			Nuance_OutputCommentLine(pbody);
}

void StartSubexpression()
{
	if (Recognizer == NUANCE3200)
		Nuance_StartSubexpression();
}

void StopSubexpression()
{
	if (Recognizer == NUANCE3200)
		Nuance_StopSubexpression();
}

void OutputOrOp()
{
	if (Recognizer == NUANCE3200)
		Nuance_OutputOrOp();
}

void OutputIntegerAnnotation(DWORD an)
{
	if (Recognizer == NUANCE3200)
		Nuance_OutputIntegerAnnotation(an);
}

void OutputRuleName(LPSTR pname, int index)
{
	if (Recognizer == NUANCE3200)
		Nuance_OutputRuleName(pname, index);
}

void StartOptionalClause()
{
	if (Recognizer == NUANCE3200)
		Nuance_StartOptionalClause();
}

void EndOptionalClause()
{
	if (Recognizer == NUANCE3200)
		Nuance_EndOptionalClause();
}

void OutputRuleTrailer(int RuleIndex)
{
	if (Recognizer == NUANCE3200)
		Nuance_OutputRuleTrailer(RuleIndex);
}
void OutputRuleHeader(int RuleIndex)
{
	if (Recognizer == NUANCE3200)
		Nuance_OutputRuleHeader(RuleIndex);
}

void OutputOneOrMore()
{
	if (Recognizer == NUANCE3200)
		Nuance_OutputOneOrMore();
}

void OutputZeroOrMore()
{
	if (Recognizer == NUANCE3200)
		Nuance_OutputZeroOrMore();
}

void OutputRuleDefinition(BOOL startrule, LPSTR pname, int g_gramindex, int RuleIndex)
{
	if (Recognizer == NUANCE3200)
		Nuance_OutputRuleDefinition(startrule, pname, g_gramindex, RuleIndex);
}

void ValidateStringChar(char & thechar)
{
	if (Recognizer == NUANCE3200)
		Nuance_ValidateStringChar(thechar);
}

void OutputQuotedString(LPSTR str)
{
	if (Recognizer == NUANCE3200)
		Nuance_OutputQuotedString(str);
}


BOOL CreateRecognizerFiles(int phase)
{
	if (Recognizer == NUANCE3200)
		return Nuance_CreateRecognizerFiles(phase);
	return TRUE;
}

void OutputGrammarHeader(LPSTR name)
{
	if (Recognizer == NUANCE3200)
		Nuance_OutputGrammarHeader(name);
}


