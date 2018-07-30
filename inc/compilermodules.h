
#define ISSTARTRULE					1
#define ISDEFINED						2
#define ISMASTERSTARTRULE		4
#define ISDYNAMIC						8			// Grammar has a dynamic rule list

typedef enum  { NUANCE3200V4, NUANCE3200, FONICS } Recognizer_Type;
extern Recognizer_Type Recognizer;

void OutputGrammarHeader(LPSTR name);
void OutputCommentLine(LPSTR pbody);
void StartSubexpression();
void StopSubexpression();
void OutputOrOp();
void OutputIntegerAnnotation(DWORD an);
void OutputRuleName(LPSTR pname, int index);
void StartOptionalClause();
void EndOptionalClause();
void OutputRuleTrailer(int RuleIndex);
void OutputRuleHeader(int RuleIndex);
void OutputOneOrMore();
void OutputZeroOrMore();
void OutputRuleDefinition(BOOL startrule,LPSTR pname,int g_gramindex, int RuleIndex);
void ValidateStringChar(char & thechar);
void OutputQuotedString(LPSTR str);

BOOL CreateRecognizerFiles(int phase);
