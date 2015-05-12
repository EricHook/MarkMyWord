#include "miniwin.h"
#include "version.h"

// this is used in the RoomServer::Tick() calls to TConnectionManager::GetMessage()
#define ROOM_MSG_SIZE	1000

//
struct TServerMemoryInfo
{
	int maxMemoryAllocated;
	int newDeleteBalance;
	int numBytesAllocated;
	
	TServerMemoryInfo()
	{
		maxMemoryAllocated	= 0;
		newDeleteBalance	= 0;
		numBytesAllocated	= 0;
	}
};

extern TServerMemoryInfo ServerMemoryInfo;

//======================================================================
// Generic parser classes
//======================================================================
//
#define MAX_STRING_BUF	256

// predefined token values
enum {
	TV_ERROR = 256,
	TV_DONE,
	TV_INTEGER,
	TV_FLOAT,
	TV_STRING,
	TV_USER
};

//
struct TTokenTable
{
	char *lexeme;
	int token;
};

//
union YYSTYPE
{
	char str[MAX_STRING_BUF];
	int  ival;
	double fval;
	TTokenTable *ptt;
};

//
class TParser;

class TLexer
{
protected:
//	friend class TParser;

	FILE *fdDocument;
	char *filename;

	YYSTYPE *yylval;
	TTokenTable *tokenTable;
	int numTokens;

	int yylineno;

	int (*sort_function)(const void*, const void*);
	int (*compare_function)(const char*, const char*);

	// they might not be sorted originally
	void sortTokens();
	int CountTokens();
	void CaseSensitive(bool onoff = true);

	static int caseSensitive_sort(const void *a, const void *b)
	{
		return strcmp(((TTokenTable *)a)->lexeme, ((TTokenTable *)b)->lexeme);
	}

	static int caseInSensitive_sort(const void *a, const void *b)
	{
		return stricmp(((TTokenTable *)a)->lexeme, ((TTokenTable *)b)->lexeme);
	}

public:
	TLexer(TTokenTable *atokenTable);
	virtual ~TLexer();

	int SetFile(char *theFile);
	const char *GetFile() const { return filename; }
	
	// methods to help with lexical processing
	// yylex() will use these to find tokens
	int SkipLeadingWhiteSpace();
	int follow(int expect, int ifyes, int ifno);
	int backslash(int c);
	void comment(void);
	int getNumber();
	int getStringLiteral();
	int getKeyword();

	int GetChar();
	int UnGetChar(int c);

	// functions that may typically be overridden
	virtual bool Init(YYSTYPE *yylval);
	virtual int GetToken();
	virtual int yylex();
	virtual void yyerror(const char *s);
	virtual bool isidval(int c);
	virtual bool iswhitespace(int c);
};

//
class TParser
{
protected:
	TLexer *lex;
	YYSTYPE yylval;
	int lookahead;

public:
	TParser(TLexer *theLexer);
	virtual ~TParser();

	virtual bool Init();
	virtual int Parse(char *filename);
	virtual int DoToken(int token);

	void expected(int token);
	void match(int token);
};

//======================================================================
// String Table resource management
//======================================================================

//
#define MAX_STRINGS_IN_TABLE	1024

//#define MAX_I18N_STRING			256
//typedef char i18nStr[MAX_I18N_STRING];

//
struct TStringTable
{
	int nStrings;
	char *strings[MAX_STRINGS_IN_TABLE];

	TStringTable();
	~TStringTable();
};

#ifdef __LINUX__
	typedef TStringTable *HINSTANCE;
#endif

HINSTANCE linuxLoadLibrary(char *filename);
BOOL linuxFreeLibrary(HINSTANCE hInstance);
int linuxLoadString(HINSTANCE hInstance, DWORD uID, char *lpBuffer, int nBufferMax);
char *GetString(int id, char *s, int maxLength = MAX_I18N_STRING);
char *GetGameString(int id, char *s, int maxLength = MAX_I18N_STRING);
void DumpStrings(HINSTANCE hInstance);

// used by the StringParser class
enum TTokenValues {
	TV_STRINGTABLE = TV_USER,
	TV_DISCARDABLE,
	TV_BEGIN,
	TV_END,
	TV_LAST
};

//
extern HINSTANCE hResourceInstance;
extern HINSTANCE hGameResourceInstance;

//
class TStringParser : public TParser
{
protected:
	TStringTable *pStringTable;

public:
	TStringParser(TLexer *theLexer, TStringTable *pst) : TParser(theLexer)
	{
		pStringTable = pst;
	}

	virtual ~TStringParser() {}

	int DoToken(int token);
};

//======================================================================
// Game server specific includes follow
//======================================================================

#ifdef HOGS_BACKGAMMON
	#include "BackgammonServer.h"
#endif

#ifdef HOGS_BLACKJACK
	#include "BlackjackServer.h"
#endif

#ifdef HOGS_BRIDGE
	#include "BridgeServer.h"
#endif

#ifdef HOGS_CHECKERS
	#include "CheckersServer.h"
#endif

#ifdef HOGS_CHESS
	#include "ChessServer.h"
#endif

#ifdef HOGS_CRAPS
	#include "CrapsServer.h"
#endif

#ifdef HOGS_CRIBBAGE
	#include "CribbageServer.h"
#endif

#ifdef HOGS_CROSSWORDS
	#include "CrosswordsServer.h"
#endif

#ifdef HOGS_DOMINOES
	#include "DominoesServer.h"
#endif

#ifdef HOGS_DOUBLECROSS
	#include "DoubleCrossServer.h"
#endif

#ifdef HOGS_EUCHRE
	#include "EuchreServer.h"
#endif

#ifdef HOGS_GIN
	#include "GinServer.h"
#endif

#ifdef HOGS_HANGMAN
	#include "HangmanServer.h"
#endif

#ifdef HOGS_HEARTS
	#include "HeartsServer.h"
#endif

#ifdef HOGS_PACHISI
	#include "PachisiServer.h"
#endif

#ifdef HOGS_PAIGOW
	#include "PaigowServer.h"
#endif

#ifdef HOGS_PINOCHLE
	#include "PinochleServer.h"
#endif

#ifdef HOGS_POKER
	#include "PokerServer.h"
#endif

#ifdef HOGS_REVERSI
	#include "ReversiServer.h"
#endif

#ifdef HOGS_ROULETTE
	#include "RouletteServer.h"
#endif

#ifdef HOGS_SPADES
	#include "SpadesServer.h"
#endif

#ifdef HOGS_WORDOX
	#include "WordoxServer.h"
#endif

#ifdef HOGS_WORDSEARCH
	#include "WordsearchServer.h"
#endif

#ifdef HOGS_WORDYACHT
	#include "WordYachtServer.h"
#endif

#ifdef HOGS_YACHT
	#include "YachtServer.h"
#endif
