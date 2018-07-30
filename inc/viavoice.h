
void ViaVoice_OutputCommentLine(LPSTR pbody);
void ViaVoice_StartSubexpression();
void ViaVoice_StopSubexpression();
void ViaVoice_OutputOrOp();
void ViaVoice_OutputIntegerAnnotation(DWORD an);
void ViaVoice_OutputRuleName(LPSTR pname);
void ViaVoice_StartOptionalClause();
void ViaVoice_EndOptionalClause();
void ViaVoice_OutputRuleTrailer(int RuleIndex);
void ViaVoice_OutputRuleHeader(int RuleIndex);
void ViaVoice_OutputOneOrMore();
void ViaVoice_OutputZeroOrMore();
void ViaVoice_OutputRuleDefinition(BOOL startrule,LPSTR pname, int g_gramindex, int RuleIndex);
void ViaVoice_ValidateStringChar(char & thechar);
void ViaVoice_OutputQuotedString(LPSTR str);
BOOL ViaVoice_CreateRecognizerFiles(int phase);
void ViaVoice_OutputGrammarHeader(LPSTR name);