// TTS Control parameters

#define TTS_VOLUME			2	// 1 .. 100 (80)
#define TTS_SPEECHRATE		3	// 50 .. 400 :  % rate of speech (100)
#define TTS_PITCH			4	// 1 .. 100:   Pitch  (50)	
#define TTS_WAITFACTOR		5	// 0 .. 9:  Sentence delay  (5)
#define TTS_READMODE		6	// 1 .. 4 (1)
	#define TTS_SEN_BY_SEN		1
	#define TTS_CHAR_BY_CHAR		2
	#define TTS_WORD_BY_WORD		3
#define TTS_SETVOICE			100		// Sets the TTS Voice to named parameter
#define TTS_SETLANGUAGE			101		// Defines the Language
#define TTS_SETDICTIONARY		102		// Loads in the named Dictionary
#define TTS_Disabled		255  // Speech off(1) or On(0)
