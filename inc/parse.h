#define MAXNESTING    16      // nesting of Include files
typedef enum  { END_TOKEN, RULEDEF_TOKEN, ALPHA_TOKEN } tokentype;

extern char Token[];
extern char CurChar;
extern char EOD;
extern BOOL InHFile;
extern char LineBuf[];
extern char * pLineBuf;

BOOL OpenParser(CString Path, CString fname);
BOOL WhiteSpace(char ch);
void SkipWhiteSpace();
tokentype GetToken();
BOOL Alpha();
int GetNum();
char GetChar();
BOOL GetAlpha();
BOOL DoInclude();
BOOL GetQuoted(char ch);
BOOL Translate();
BOOL Find(char * pstr);
void SkipLine();
void CloseParser();
BOOL OpenInclude(CString fname);
