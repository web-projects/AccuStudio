// TTS Control parameter ID's

#define TTS_VOLUME														2	// 1 .. 100 (80)
#define TTS_SPEECHRATE												3	// 50 .. 400 :  % rate of speech (100)
#define TTS_PITCH															4	// 1 .. 100:   Pitch  (50)	
#define TTS_WAITFACTOR												5	// 0 .. 9:  Sentence delay  (5)
#define TTS_READMODE													6	// 1 .. 4 (1)
	#define TTS_SEN_BY_SEN												1
	#define TTS_CHAR_BY_CHAR											2
	#define TTS_WORD_BY_WORD											3

// Recognition Control Parameter ID's

#define	REC_FXPARAM_SENSITIVITY								2        
#define	REC_FXPARAM_ABSOLUTE_THRESHOLD				3
#define	REC_FXPARAM_MINSPEECH									4				
#define REC_FXPARAM_TS												6					
#define	REC_FXPARAM_TIMEOUT_SILENCE						9    
#define	REC_FXPARAM_TIMEOUT_SPEECH						10		
#define REC_CTXPARAM_ACCURACY									1002					
#define	REC_PARAM_SECONDPASS_ENABLE						1005			
#define	REC_CTXPARAM_TS												1001              
#define	REC_REJECTION_THRESH									10000			
#define	REC_RESETPARAMS												10001			

