
void Nuance_OutputCommentLine(LPSTR pbody);
void Nuance_StartSubexpression();
void Nuance_StopSubexpression();
void Nuance_OutputOrOp();
void Nuance_OutputIntegerAnnotation(DWORD an);
void Nuance_OutputRuleName(LPSTR pname, int index);
void Nuance_StartOptionalClause();
void Nuance_EndOptionalClause();
void Nuance_OutputRuleTrailer(int RuleIndex);
void Nuance_OutputRuleHeader(int RuleIndex);
void Nuance_OutputOneOrMore();
void Nuance_OutputZeroOrMore();
void Nuance_OutputRuleDefinition(BOOL startrule,LPSTR pname, int g_gramindex, int RuleIndex);
void Nuance_ValidateStringChar(char & thechar);
void Nuance_OutputQuotedString(LPSTR str);
BOOL Nuance_CreateRecognizerFiles(int phase);
void Nuance_OutputGrammarHeader(LPSTR name);
