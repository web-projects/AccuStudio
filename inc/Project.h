
#ifndef  PROJECT_INCLUDE
#define PROJECT_INCLUDE

#include "stdafx.h"

#define OEM_ID_DEMO				0
#define OEM_ID_VANGARD			1
#define OEM_ID_WAVELINK			2

#define PROJECT_TYPE_NONE 0
#define PROJECT_TYPE_WIN 1
#define PROJECT_TYPE_TE 2
#define PROJECT_TYPE_WEB 3

//#define VERSION   0x0421
#define NumMRU  6         // Number of MRU files if File Menu 

// Project settings 

typedef enum  { 
	AWL_VOICEACTIVATION_FLAG = 1,
} Proj_Settings;
#define PROJ_REC_MSK		0x1e

typedef struct tagProjHeader
{
  WORD    version;              // Project Version
	WORD		ControlSize;					// size of CONTROLO structure
  UINT    DefineOff;            // Offset to Defines list
  UINT    ValuesOff;            // Offset to Values list
  UINT    NamesOff;             // Offset to Names list
  UINT    GramSetOff;           // Offset to Grammar Set list
  UINT    GramOff;              // Offset to Grammarlist
  UINT    GramListOff;          // Offset to Grammar thread List
  UINT    ObjOff;               // Start of the Resource Objects
  UINT    StringsOff;           // Start of string table
  char    ClientPath[126];      // This unused space used to be = Client Execution Path Name
	BYTE		OEMID;		// This is a filler for proper structure alignment
	BYTE		ProjectType;
  char    ResName[124];         // This unused space used to be = Name of the resource file
	DWORD		ProjSettings;
  char    ResPath[124];         // This unused space used to be = Path to Resource file (*.rc)
	BYTE		NewLogic;							// Using new logic code
	BYTE		Junk1;
	BYTE		LangID;
	BYTE		ModelID;
}PROJHEADER, * LPPROJHEADER;

// Grammar thread attached to a speech object
typedef struct tagCSOGRAMMAR
{
	struct tagCSOGRAMMAR * lpNext;
	UINT nGrammarSetOrdinal;
	UINT nGrammarOrdinal;
} CSOGRAMMAR, * LPCSOGRAMMAR;

typedef struct OldtagControlResObject
{
  UINT ControlType;
	struct tagControlResObject * lpNext;    // next object
  tagControlResObject * controlthread;    // for Dialogs and Menus
	int ControlIDOrd;
  UINT Coords[4];
  UINT Style, NotStyle;
  int Caption;
  int ClassName;
  LPCSOGRAMMAR lpGram;                    // grammars associated with object
} OLDCONTROLRO;

typedef struct Oldv1tagControlResObject
{
  UINT ControlType;
	struct tagControlResObject * lpNext;    // next object
  tagControlResObject * controlthread;    // for Dialogs and Menus
	int ControlIDOrd;
  UINT Coords[4];
  UINT Style, NotStyle;
  int Caption;
  int ClassName;
  LPCSOGRAMMAR lpGram;                    // grammars associated with object

	int DataEntryPrompt;
	int DataType;
	int PhraseList;
	// Caution: to not change the declarations above
} OLDV1CONTROLRO;


typedef struct tagControlResObject
{
  UINT ControlType;
	struct tagControlResObject * lpNext;    // next object
  tagControlResObject * controlthread;    // for Dialogs and Menus
	int ControlIDOrd;
  UINT Coords[4];
  UINT Style, NotStyle;
  int Caption;
  int ClassName;
  LPCSOGRAMMAR lpGram;

	int DataEntryPrompt;
	int DataType;
	int PhraseList;
	// Caution: to not change the declarations above
	int NavigationPrompt;
	int PreRecActions;  // Includes Grammar attributes: 0x80000000 = Dynamic
} CONTROLRO, * LPCONTROLRO, ** LPPCONTROLRO;

typedef LPCONTROLRO LPDIALOGRO;
typedef CONTROLRO DIALOGRO;

// Grammar Thread

typedef struct tagGramThread
{
  UINT ID;					// Grammar name ID
  tagGramThread * next;    
} GRAMTHREAD, * LPGRAMTHREAD;


// Grammar Set Thread

typedef struct tagGramSetThread
{
  UINT ID;        // Grammar set name ID
  tagGramSetThread * next; 
  LPGRAMTHREAD  pgrams;   // grammar thread
} GRAMSETTHREAD, * LPGRAMSETTHREAD;

typedef struct tagControlDef
{
  LPSTR ControlName;    // Name String
  LPSTR ControlClass;   // Window Class
  UINT wtype;           // Control Window type
} ControlDef, * lpControlDef;

extern unsigned char gOEMID;

extern CString ProjName;       // The project name
extern CString DirPath;        // Project directory path name
//extern CString ClientPath;     // Client Exectution Path Name
extern CString ResName;        // Name of the resource file
extern CString ResPath;        // Path to Resource file (*.rc)

extern BOOL PModified;         // TRUE if the project has been modified
extern char * * pnames;        //  For Captions, class names, etc..
extern int NumNames;

extern char * * pgramsetnames;   // Grammar Set Names
extern int NumGramSets;

extern char * * pgramnames;   // Grammar Names
extern int NumGrams;
extern DWORD ProjSettings;			// Project settings
extern char * * pdefines, * * pvalues;      // For #defines
extern int NumDefines;

extern LPDIALOGRO pdreshead;   // head node for res dialog
extern LPDIALOGRO pdres;       // running dialog res ptr

extern LPGRAMSETTHREAD lpGramSetThread;   // Grammar set thread

extern BOOL   buildinprogress;
extern BOOL gProjectLimited;
extern BOOL gProjectLoadFailed;
extern BOOL gAppLoading;
extern int gProjectType;

void Disable(UINT id);
void Enable(UINT id);
BOOL IsDisabled(UINT id);
void NewProject(void);
BOOL DoNewProject();
void CloseProject();
void SaveProject();
void OpenProject();
BOOL DoOpenProject();
int GetGramSetIndex(LPSTR pname);
BOOL AddGramSetName(int i);
int AddGramName(int i, LPSTR pname);
void AddGrammarFiles();
void AddGrammarset();
BOOL ShowProjWindow();
int BuildProject(BOOL makedef, CString DirPath, CString ProjName);	
void BuildFromDef();
void GrammarWizard(LPDIALOGRO pnode);
void TEGrammarWizard(LPDIALOGRO pnode);
void ObjectWizard(LPDIALOGRO pnode);
void GrammarExerciser(LPDIALOGRO pnode);
void BuildLib();
BOOL IsSpeechObj(LPCONTROLRO pc);
BOOL IsDataObj(LPCONTROLRO pc);
BOOL IsListObj(LPCONTROLRO pc);
void ViewResults();
BOOL LinkGrammar(LPCONTROLRO pc, int gsindex, int gindex);
void RestoreAll();
void DisableAll();
void UpdatePane (LPCSTR str);
void UnlinkAllGrammars(LPCONTROLRO pc);
BOOL FileExists(LPSTR fname);
LPDIALOGRO UpdateProjectFromRes();
void AddResToProject();
BOOL MakeFileWritable(CString infile);
#endif

