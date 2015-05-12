#include "ServerInclude.h"

#ifdef _WIN32
	#define WM_TRAYMSG (WM_USER + 1)
	
	HINSTANCE g_hInstance;
	static HWND hwnd;
#endif

//
ROOMSERVER *roomServer = NULL;

DWORD roomID;
int clientPort;
int risPort;
int adminPort;
int roomFlags; //info stored in the ROOM_*** defines in hwmessages.h
char roomName[MAX_ROOM_NAME];
char gameName[MAX_GAME_NAME];
char sponsorURL[MAX_ROOM_NAME];
char proxyNames[3][32];
int proxyPorts[3];

char *szLogPrefix = NULL;
bool done = false;
bool dying = false;

HINSTANCE hResourceInstance = NULL;
HINSTANCE hGameResourceInstance = NULL;

////////////////////////////////////////////////////////////////////////////////////
// global tools
////////////////////////////////////////////////////////////////////////////////////

TServerMemoryInfo ServerMemoryInfo;

//
void * CDECL operator new(size_t s)
{
	size_t *temp;

	temp = (size_t *)malloc(s + 4);
	
	if (temp == NULL)
	{
		TRACE(_WARN_ "warning: Out Of Memory!\n");
		return NULL;
	}

	*temp = s;
	ServerMemoryInfo.numBytesAllocated += s;
	if (ServerMemoryInfo.numBytesAllocated > ServerMemoryInfo.maxMemoryAllocated)
		ServerMemoryInfo.maxMemoryAllocated = ServerMemoryInfo.numBytesAllocated;
	
//	#if defined(_WIN32) && defined(_DEBUG)
//		TRACE(_ROUTINE_ "Alloc: %u\n", s);
//	#endif

	ServerMemoryInfo.newDeleteBalance++;
	return temp+1;
}

//
void CDECL operator delete(void *s)
{
	size_t *temp;

	if (s)
	{
		temp = (size_t*)s;
		--temp;

//		#if defined(_WIN32) && defined(_DEBUG)
//			TRACE(_ROUTINE_"Free: %u\n", *temp);
//		#endif

		ServerMemoryInfo.numBytesAllocated -= *temp;
		ServerMemoryInfo.newDeleteBalance--;
		free(temp);
	}
}

//
void * CDECL operator new[](size_t s)
{
	size_t *temp;

	temp = (size_t *)malloc(s + sizeof(size_t));
	
	if (temp == NULL)
	{
		TRACE(_WARN_ "warning: Out Of Memory!\n");
		return NULL;
	}

	*temp = s;
	ServerMemoryInfo.numBytesAllocated += s;
	if (ServerMemoryInfo.numBytesAllocated > ServerMemoryInfo.maxMemoryAllocated)
		ServerMemoryInfo.maxMemoryAllocated = ServerMemoryInfo.numBytesAllocated;
	
	ServerMemoryInfo.newDeleteBalance++;
	return temp+1;
}

//
void CDECL operator delete[](void *s)
{
	size_t *temp;

	if (s)
	{
		temp = (size_t*)s;
		--temp;
		ServerMemoryInfo.numBytesAllocated -= *temp;
		ServerMemoryInfo.newDeleteBalance--;
		free(temp);
	}
}

//======================================================================
// Generic Lexical analyzer class
//======================================================================
TLexer::TLexer(TTokenTable *aTokenTable)
{
	assert(aTokenTable);

	yylineno	= 0;
	numTokens	= 0;

	tokenTable	= aTokenTable;
	yylval		= NULL;
	fdDocument	= NULL;
	filename	= NULL;
	sort_function		= caseSensitive_sort;
	compare_function	= strcmp;
}

//
TLexer::~TLexer()
{
	free(filename);
	if (fdDocument)
		fclose(fdDocument);
}

//
int TLexer::GetChar()
{
	return fgetc(fdDocument);
}

//
int TLexer::UnGetChar(int c)
{
	return ungetc(c, fdDocument);
}

//
void TLexer::yyerror(const char *s)
{
	TRACE(_ROUTINE_ "%s at line %d\n", s, yylineno);
}

//
bool TLexer::Init(YYSTYPE *ayylval)
{
	yylval = ayylval;

	numTokens = CountTokens();
	sortTokens();
	return true;
}

//
void TLexer::CaseSensitive(bool onoff /*= true*/)
{
	sort_function		= (onoff) ? caseSensitive_sort : caseInSensitive_sort;
	compare_function	= (onoff) ? strcmp : stricmp;
}

//
int TLexer::CountTokens()
{
	int count = 0;
	TTokenTable *token = tokenTable;

	while(token->lexeme)
	{
		count++;
		token++;
	}

	return count;
}

// they might not be sorted originally
void TLexer::sortTokens()
{
	qsort(tokenTable, numTokens, sizeof(TTokenTable), sort_function);
}

//
int TLexer::SetFile(char *theFile)
{
	if (fdDocument)
		fclose(fdDocument);

	fdDocument = fopen(theFile, "rt");

	if (!fdDocument)
		return -1;

	filename = strdup(theFile);
	return 0;
}

//
int TLexer::GetToken()
{
	// TODO - implement
	return 0;
}

//
bool TLexer::isidval(int c)
{
	if(isalnum(c) || c == ':' || c == ',' || c == '*' || c == '/' || c == '.')
		return true;

	return false;
}

//
bool TLexer::iswhitespace(int c)
{
	return (c == ' ' || c == '\t' || c == '\n') ? true : false;
}

// skip any leading WS
int TLexer::SkipLeadingWhiteSpace()
{
	int chr;

	do
	{
		chr = GetChar();
		if (chr == '\n')
			yylineno++;
	} while (iswhitespace(chr));

	return chr;
}

//
int TLexer::backslash(int c)
{
	static char transtab[] = "b\bf\fn\nr\rt\t";

	if (c != '\\')
		return c;

	c = GetChar();
	if (islower(c) && strchr(transtab, c))
		return strchr(transtab, c)[1];

	return c;
}

//
void TLexer::comment(void)
{
	int c;

	// skip to EOL
	while ((c = GetChar()) != '\n');

	// put last character back
	UnGetChar(c);
}

//
int TLexer::follow(int expect, int ifyes, int ifno)
{
	int chr;

	chr = GetChar();
	if (chr == expect)
		return ifyes;

	UnGetChar(chr);
	return ifno;
}

//
int TLexer::getNumber()
{
	int c;
	char buf[32];
	char *bufptr = buf;

	while (isdigit((c = GetChar())) || c == '.')
		*bufptr++ = c;
	
	UnGetChar(c);
	*bufptr = '\0';
	if (strchr(buf, '.') == NULL)
	{
		yylval->ival = atoi(buf);
		return TV_INTEGER;
	}
	else
	{
		yylval->fval = atof(buf);
		return TV_FLOAT;
	}
}

//
int TLexer::getStringLiteral()
{
	int c;
	char buf[MAX_STRING_BUF];
	char *cptr = buf;

	c = GetChar();
	while (c != '"' && cptr < &buf[sizeof(buf)])
	{
		if (c == '\n' || c == EOF)
			yyerror("missing quote");

		// build up our string, translating escape chars
		*cptr++ = backslash(c);
		c = GetChar();
	}

	// make sure its asciiz
	*cptr = '\0';

	strcpy(yylval->str, buf);
	return TV_STRING;
}

//
int TLexer::getKeyword()
{
	return 0;
}

//======================================================================
// Generic Lexical analyzer routine
//======================================================================
int TLexer::yylex()
{
	int chr;
	char buf[128];
	char *pBuf = buf;
	TTokenTable *ptt;

yylex01:
	// skip any leading WS
	chr = SkipLeadingWhiteSpace();
	
	// process Unix conf style comments
	if (chr == '#')
	{
		comment();
		goto yylex01;
	}

	// handle C++ style comments
	if (chr == '/')
	{
		if (follow('/', 1, 0))
		{
			comment();
			goto yylex01;
		}
	}

	// look for a number value
	if (isdigit(chr))
	{
		UnGetChar(chr);
		return getNumber();
	}

	// look for string literals
	if (chr == '"')
	{
		return getStringLiteral();
	}

	if (isalpha(chr))
	{
		do 
		{
			*pBuf++ = ((char)chr);
		} while ((chr = GetChar()) != EOF && isidval(chr));
		
		*pBuf = 0;
		strcpy(yylval->str, buf);

		// search token table for possible match
		for (ptt = tokenTable; ptt->lexeme; ptt++)
			if (!compare_function(buf, ptt->lexeme))
			{
				return ptt->token;
			}

		return TV_STRING;
	}

	// process and return single char tokens
	switch(chr)
	{
	case EOF:
		return TV_DONE;

	default:
		return chr;
	}
}

//======================================================================
// Generic parser class
//======================================================================
TParser::TParser(TLexer *theLexer)
{
	lex = theLexer;
}

//
TParser::~TParser()
{
	// TODO - should we delete this here??
	delete lex;
}

//
bool TParser::Init()
{
	if (!lex->Init(&yylval))
		return false;
	
	return true;
}

//
void TParser::expected(int token)
{
	char s[256];

	sprintf(s, "Error, expected to see token %d in stream but saw %d instead.\n", token, lookahead);
	lex->yyerror(s);
}

//
void TParser::match(int token)
{
	if (lookahead == token)
		lookahead = lex->yylex();
	else
		expected(token);
}

//
int TParser::DoToken(int token)
{
	return 0;
}

//
int TParser::Parse(char *filename)
{
	int rv;

	assert(filename);

	rv = lex->SetFile(filename);
	if (rv != 0)
		return rv;

	// TODO - implement
	lookahead = lex->yylex();
	
	while(lookahead != TV_DONE)
	{
		DoToken(lookahead);
	}

	return 0;
}

//======================================================================
// For Win32 compatibility
// Note, you MUST remove this line to use native Win32 string table API calls
// FIXME!
//======================================================================
#ifdef LoadString
#	undef LoadString
#	define LoadString linuxLoadString
#endif

//
TTokenTable tokenTable[] = {
	{"STRINGTABLE",		TV_STRINGTABLE},
	{"DISCARDABLE",		TV_DISCARDABLE},
	{"BEGIN",			TV_BEGIN},
	{"END",				TV_END},

	{NULL,				TV_DONE}
};

//======================================================================
//
//======================================================================
TStringTable::TStringTable()
{
	nStrings = 0;
	memset(strings, 0, sizeof(strings));
}

//======================================================================
//
//======================================================================
TStringTable::~TStringTable()
{
	int id;

	for (id = 0; id < MAX_STRINGS_IN_TABLE; id++)
	{
		if (strings[id])
		{
			free(strings[id]);
			strings[id] = NULL;
		}
	}
}

//======================================================================
// Load up a string resource
//======================================================================
HINSTANCE linuxLoadLibrary(char *filename)
{
	char stringFile[256];
	TStringTable *pst;

	assert(filename);

	if (!filename)
		return NULL;

	// TODO - check for '.ext' in filename
	sprintf(stringFile, "%s.rcl", filename);

	pst = new TStringTable;
	if (!pst)
		return NULL;

	TStringParser parser(new TLexer(tokenTable), pst);
	parser.Init();
	parser.Parse(stringFile);

	return (HINSTANCE)pst;
}

//======================================================================
// Free a string resource
//======================================================================
BOOL linuxFreeLibrary(HINSTANCE hInstance)
{
	delete (TStringTable *)hInstance;
	return TRUE;
}

//===============================================================================
// hInstance	- ignored for Linux (handle of module containing string resource)
// uID			- resource identifier 
// lpBuffer		- address of buffer for resource 
// nBufferMax	- size of buffer 
//===============================================================================
int linuxLoadString(HINSTANCE hInstance, DWORD uID, char *lpBuffer, int nBufferMax)
{
	TStringTable *pst = (TStringTable *)hInstance;
	
	assert(hInstance);

	// make sure hmodule is valid, and react the way Win32 does if not
	if (!pst)
		return 0;

	// validate the params
	if (uID < 0 || uID > MAX_STRINGS_IN_TABLE)
	{
		strnzcpy(lpBuffer, "Invalid id!", nBufferMax - 1);
		return 0;
	}

	// look it up and copy it into the user buffer
	if (pst->strings[uID])
		strnzcpy(lpBuffer, pst->strings[uID], nBufferMax - 1);
	else
		strnzcpy(lpBuffer, "No string in table!", nBufferMax - 1);

	return strlen(lpBuffer);
}

//======================================================================
// Dumps out all of the strings in the given string table
//======================================================================
void DumpStrings(HINSTANCE hInstance)
{
	TStringTable *pst = (TStringTable *)hInstance;
	char s[256];

	if (!hInstance)
		return;

	for (int i = 1; i <= pst->nStrings; i++)
	{
		LoadString(hInstance, i, s, sizeof(s) - 1);
		TRACE(_ROUTINE_ "%d \"%s\"\n", i, s);
	}
}

//======================================================================
//
//======================================================================
char *GetString(int id, char *s, int maxLength /* = MAX_I18N_STRING */)
{
	int bytes;
	
	s[0] = 0;
	
	if (!hResourceInstance)
		{ assert(FALSE); return s; }
	
	bytes = LoadString(hResourceInstance, id, s, maxLength - 1);
	
	return s;
}

//======================================================================
//
//======================================================================
char *GetGameString(int id, char *s, int maxLength /* = MAX_I18N_STRING */)
{
	int bytes;
	
	s[0] = 0;
	
	if (!hGameResourceInstance)
		{ assert(FALSE); return s; }
	
	bytes = LoadString(hGameResourceInstance, id, s, maxLength - 1);
	
	return s;
}

//======================================================================
// This method is called on each token in the input stream
//======================================================================
int TStringParser::DoToken(int token)
{
	int id;

	// Note: comments are handled automatically by the lexical analyzer

	// Here we simply deal with the tokens that we expect to see in the
	// parse stream.
	switch(lookahead)
	{
	case TV_STRINGTABLE:
		TRACE(_VERBOSE_ "%s ", yylval.str);
		match(lookahead);
		
		if (lookahead == TV_DISCARDABLE)
		{
			TRACE(_VERBOSE_ "%s", yylval.str);
			match(TV_DISCARDABLE);
		}

		TRACE(_VERBOSE_ "\n%s\n", yylval.str);
		match(TV_BEGIN);

		// process all entries in this table
		while(lookahead == TV_INTEGER)
		{
			id = yylval.ival;
			TRACE(_VERBOSE_ "%d\t", id);
			match(TV_INTEGER);

			// verify id, then read string and copy it into the string table
			ASSERT(_CRASH_, id >= 0 && id < MAX_STRINGS_IN_TABLE);
			ASSERT(_CRASH_, pStringTable->strings[id] == NULL);

			pStringTable->strings[id] = strdup(yylval.str);
			pStringTable->nStrings++;

			TRACE(_VERBOSE_ "\"%s\"\n", pStringTable->strings[id]);

			match(TV_STRING);
		}

		TRACE(_VERBOSE_ "%s\n", yylval.str);
		match(TV_END);
		return 0;
		break;

	default:
		ASSERT(_WARN_, false);
		lookahead = TV_DONE;
	}

	// give the parent parser a chance to process
	return TParser::DoToken(token);
}

//
void initlog()
{
	char s[256];
	
	// give ourselves a couple of extra FDs since we don't need these streams
	
	fclose(stderr);
	fclose(stdin);
	fclose(stdout);
	
	// start using our regular log file
	
	sprintf(s, "%s.log", szLogPrefix);
	SetLogFile(s, "at");
	
	// start using our warnings log file
	
	sprintf(s, "%s.warn.log", szLogPrefix);
	SetWarningsFile(s, "at");
	
	// start using our boot warnings log file
	
	sprintf(s, "%s.bootwarn.log", szLogPrefix);
	SetBootWarningsFile(s, "at");
	
#ifdef __LINUX__
	char szPidDir[256];
	FILE *fptr;
	pid_t pid;
	
	// create our pid file
	pid = getpid();
	sprintf(s, "%s/%s%d.pid", GetString(28, szPidDir), szLogPrefix, (int)clientPort);
	fptr = fopen(s, "wt");
	if (fptr) {
		fprintf(fptr, "%d\n", pid);
		fclose(fptr);
	}
	else
		assert(false);
#endif
}

void rotate_log_file()
{
	FILE *fptr;
	char logfile[256], oldlog[256];
	
	// determine our log file name
	
	sprintf(logfile, "%s.log", szLogPrefix);
	sprintf(oldlog, "%s-old.log", szLogPrefix);
	
	fptr = GetLogFile();
	
	if (fptr != NULL) {
		fflush(fptr);
		fclose(fptr);
		
		// make sure we try to save the old log file
		
		rename(logfile, oldlog);
	}
	
	// (re-)open our log file
	
	SetLogFile(logfile, "at");
}

//
void rotate_warn_log()
{
	FILE *fptr;
	char logfile[256];
	
	// determine our warn log file name
	sprintf(logfile, "%s.warn.log", szLogPrefix);

	fptr = GetWarningsFile();
	if (fptr)
	{
		fflush(fptr);
		fclose(fptr);
	}
	
	// (re-)open our warn log file
	SetWarningsFile(logfile, "at");
	
	// determine our boot warn log file name
	sprintf(logfile, "%s.bootwarn.log", szLogPrefix);
	
	fptr = GetBootWarningsFile();
	if (fptr)
	{
		fflush(fptr);
		fclose(fptr);
	}
	
	// (re-)open our boot warn log file
	SetBootWarningsFile(logfile, "at");
}

//
void init_strings()
{
	char s[256];
	char *p;

	strcpy(s, SERVER_APP_NAME);
	p = strchr(s, ' ');
	if (p)
		p[0] = 0;

	strlwr(s);

	if (!hResourceInstance)
		hResourceInstance		= linuxLoadLibrary("server");
	else
		ASSERT(_WARN_, false);

	if (!hGameResourceInstance)
		hGameResourceInstance	= linuxLoadLibrary(s);
	else
		ASSERT(_WARN_, false);
}

//
void preflight()
{
	MTS_ROOM_ENTER_MESSAGE mts;

	TRACE(_ROUTINE_ "sizeof(GameState) = %d\n", sizeof(GameState));
	TRACE(_ROUTINE_ "sizeof(MTS_ROOM_ENTER_MESSAGE) = %d\n", sizeof(MTS_ROOM_ENTER_MESSAGE));
	TRACE(_ROUTINE_ "offset of login = %d\n", ((char*)&mts.login - (char*)&mts));
	TRACE(_ROUTINE_ "offset of faceInfo = %d\n", ((char*)&mts.faceInfo - (char*)&mts));
	TRACE(_ROUTINE_ "offset of roomID = %d\n", ((char*)&mts.roomID - (char*)&mts));
	TRACE(_ROUTINE_ "offset of MachineID = %d\n", ((char*)&mts.machineID - (char*)&mts));
//	TRACE(_ROUTINE_ "sizeof(MTDB_RIS_CREATE_ACCOUNT_MESSAGE) = %d\n", sizeof(MTDB_RIS_CREATE_ACCOUNT_MESSAGE));
//	TRACE(_ROUTINE_ "offset of MachineID = %d\n", ((char*)&mtsc.playerMachineID - (char*)&mtsc));

	if (hResourceInstance)
		DumpStrings(hResourceInstance);
	
	if (hGameResourceInstance)
		DumpStrings(hGameResourceInstance);
}

//======================================================================
// TODO - this could be a call to a RoomServer factory if/when we
// TODO - choose to implement that particular feature
//======================================================================
bool CreateServerObject()
{
	assert(!roomServer);

	roomServer = new ROOMSERVER();
	if (!roomServer)
	{
		TRACE(_ROUTINE_ "Failed to create Server object.\n");
		return false;
	}

	if (roomServer->Init())
	{
		TRACE(_ROUTINE_ "Server Init failed.\n");

		delete roomServer;
		roomServer = NULL;
		return false;
	}

	return true;
}

Boolean ParseCommandLine(char *s)
{
	char *p, key[16];
	int i, j, successes = 0;
	
	p = strstr(s, "roomid:");
	if (p != NULL) {
		sscanf(&p[strlen("roomid:")], "%lx", &roomID);
		successes++;
	}
	
	p = strstr(s, "clientport:");
	if (p != NULL) {
		sscanf(&p[strlen("clientport:")], "%d", &clientPort);
		successes++;
	}
	
	p = strstr(s, "risport:");
	if (p != NULL) {
		sscanf(&p[strlen("risport:")], "%d", &risPort);
		successes++;
	}
	
	p = strstr(s, "adminport:");
	if (p != NULL) {
		sscanf(&p[strlen("adminPort:")], "%d", &adminPort);
		successes++;
	}
	
	p = strstr(s, "roomname:\"");
	if (p != NULL) {
		p += strlen("roomname:\"");
		for (i = 0 ; p[0] && p[0] != '"' && i < (MAX_ROOM_NAME - 1) ; p++, i++)
			roomName[i] = p[0];
		roomName[i] = 0;
		successes++;
	}
	
	p = strstr(s, "gamename:\"");
	if (p != NULL) {
		p += strlen("gamename:\"");
		for (i = 0 ; p[0] && p[0] != '"' && i < (MAX_GAME_NAME - 1) ; p++, i++)
			gameName[i] = p[0];
		gameName[i] = 0;
		successes++;
	}
	
	for (i = 0 ; i < 3 ; i++) {
		sprintf(key, "proxy%d:", i + 1);
		p = strstr(s, key);
		if (p != NULL) {
			p += strlen(key);
			for (j = 0 ; p[0] && p[0] != ':' && j < (32 - 1) ; p++, j++)
				proxyNames[i][j] = p[0];
			proxyNames[i][j] = 0;
			sscanf(&p[1], "%d", &proxyPorts[i]);
			successes++;
		}
	}
	
	if (successes != 9) {
		TRACE(_ROUTINE_ "usage: server roomid:xxxxx clientport:xxxx risport:xxxx adminport:xxxx roomname:\"Room Name\" gamename:\"Game Name\" proxy1:machine1.com:xxxx proxy2:machine2.com:xxxx proxy3:machine3.com:xxxx\n");
		
		return FALSE;
	}
	
	sponsorURL[0] = 0;
	p = strstr(s, "sponsorurl:\"");
	if (p != NULL) {
		p += strlen("sponsorurl:\"");
		for (i = 0 ; p[0] && p[0] != '"' && i < (MAX_ROOM_NAME - 1) ; p++, i++)
			sponsorURL[i] = p[0];
		sponsorURL[i] = 0;
	}
	
	roomFlags = 0;
	p = strstr(s, "-tournament");
	if (p != NULL)
		roomFlags |= ROOM_IS_TOURNAMENT_ROOM;

	p = strstr(s, "-rated");
	if (p != NULL)
		roomFlags |= ROOM_IS_RATED_ROOM;
	
	p = strstr(s, "-ladder");
	if (p != NULL)
		roomFlags |= ROOM_IS_LADDER_ROOM;

	return TRUE;
}

void GetServerStatusString(char *s)
{
	int i, d, e = 0, j = 0, a = 0, l = 0, db = 0, totalQ = 0, maxQ = 0;
	
	for (i = 0 ; i < MAX_ROOM_PLAYERS ; i++) {
		switch (roomServer->clients[i].status) {
			case CS_EMPTY: e++; break;
			case CS_JOINING: j++; break;
			case CS_ACTIVE:a++; break;
			case CS_LEAVING: l++; break;
		}
		if (roomServer->clients[i].status == CS_ACTIVE || roomServer->clients[i].status == CS_LEAVING) {
			d = roomServer->clients[i].connection->GetEstimatedQueDepth();
			totalQ += d;
			if (d > maxQ)
				maxQ = d;
		}
	}
	if (roomServer->dbConnection != NULL)
		db++;
	
	sprintf(s, "%s e = %d, j = %d, a = %d, l = %d, db = %d, nc = %d, totalQ = %d, maxQ = %d, rn = \"%s\", gn = \"%s\"",
			   SERVER_APP_NAME, e, j, a, l, db, roomServer->numClients, totalQ, maxQ, roomName, gameName);
}

void UpdateDisplay()
{
#ifdef _WIN32
	char s[256];
	
	if (!IsWindow(hwnd))
		return;
	
	GetServerStatusString(s);
	SetWindowText(hwnd, s);
#endif
}

#if defined(_WIN32) && !defined(CONSOLE_APP)

////////////////////////////////////////////////////////////////////////////////////
// Windows message handlers
////////////////////////////////////////////////////////////////////////////////////

static void OnSize(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	int cxSize;
	int cySize;
	
	cxSize = LOWORD(lParam);
	cySize = HIWORD(lParam);
}

static void OnTrayMessage(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	if ((UINT) lParam == WM_LBUTTONDBLCLK) 
		ShowWindow(hwnd, SW_RESTORE);
}

static void OnDestroy(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	linuxFreeLibrary(hResourceInstance);
	hResourceInstance = NULL;

	linuxFreeLibrary(hGameResourceInstance);
	hGameResourceInstance = NULL;

	PostQuitMessage(0);
}

static void OnPaint(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	hdc = BeginPaint(hwnd, &ps);
	
	// TODO - update window

	EndPaint(hwnd, &ps);
}

static void OnCreate(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	BEGIN_MSG_MAP;
		HANDLE_MSG(WM_CREATE,		OnCreate);
		HANDLE_MSG(WM_SIZE,			OnSize);
		HANDLE_MSG(WM_PAINT,		OnPaint);
		HANDLE_MSG(WM_DESTROY, 		OnDestroy);
		HANDLE_MSG(WM_TRAYMSG,		OnTrayMessage);
	END_MSG_MAP;
}

//
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, PSTR szCmdLine, int iCmdShow)
{
	MSG msg;
	WNDCLASSEX wndclass;
	
	g_hInstance = hInstance;
	
	if (!ParseCommandLine(szCmdLine))
		return -1;
	
	wndclass.cbSize			= sizeof(WNDCLASSEX);
	wndclass.style			= CS_HREDRAW | CS_VREDRAW | CS_BYTEALIGNCLIENT;
	wndclass.lpfnWndProc	= WndProc;
	wndclass.cbClsExtra		= 0;
	wndclass.cbWndExtra		= 0;
	wndclass.hInstance		= g_hInstance;
	wndclass.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground	= (HBRUSH) GetStockObject(BLACK_BRUSH);
	wndclass.lpszMenuName	= NULL;
	wndclass.lpszClassName	= SERVER_APP_NAME;
	wndclass.hIconSm		= LoadIcon(NULL, IDI_APPLICATION);
	
	if (!RegisterClassEx(&wndclass))
		return -1;
	
	hwnd = CreateWindowEx
	(
		0,
		SERVER_APP_NAME,
		SERVER_APP_NAME,
		WS_OVERLAPPEDWINDOW,
		50,
		50,
		630,
		50,
		NULL,
		NULL,
		hInstance,
		NULL
	);
	
	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);
	
	// create and set the roomServer global object pointer
	if (!CreateServerObject())
		return -1;
	
	init_strings();
	preflight();

	do
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
			roomServer->Tick();
	} while (msg.message != WM_QUIT);
	
	delete roomServer;
	
	// dump out the memory 
	TRACE(_ROUTINE_ "memory allocated: %d bytes\nnew/del balance: %d bytes\nmax memory: %d bytes\n",
		ServerMemoryInfo.numBytesAllocated,
		ServerMemoryInfo.newDeleteBalance,
		ServerMemoryInfo.maxMemoryAllocated
		);
	return msg.wParam;
}

#else

#include <signal.h>

typedef struct {
	char message[128];
	DWORD ticks;
	bool given;
} DeathStage;

DeathStage deathStages[5];

//======================================================================
// setup a signal handler for kill commands
//======================================================================
void sigterm(int signal)
{
	TRACE(_WARN_ "sigterm() signal received.\n");
	
	done = true;
}

//======================================================================
// setup a signal handler for alarms (kill slowly)
//======================================================================
void sigalarm(int signal)
{
	i18nStr getS;
	int i;
	DWORD tickCount;
	
	TRACE(_WARN_ "sigalarm() signal received.\n");
	
	dying = true;
	
	strcpy(deathStages[0].message, GetString(1, getS)); // "This room will be shut down within 5 minutes."
	strcpy(deathStages[1].message, GetString(2, getS)); // "This room will be shut down within 1 minute."
	strcpy(deathStages[2].message, GetString(3, getS)); // "This room will be shut down within 30 seconds."
	strcpy(deathStages[3].message, GetString(4, getS)); // "This room will be shut down within 15 seconds.")
	strcpy(deathStages[4].message, GetString(5, getS)); // "This room has been shut down by the system administrator."
	
	tickCount = GetTickCount();
	
	deathStages[0].ticks = tickCount;
	deathStages[1].ticks = deathStages[0].ticks + 1000 * 60 * 4;
	deathStages[2].ticks = deathStages[1].ticks + 1000 * 30;
	deathStages[3].ticks = deathStages[2].ticks + 1000 * 15;
	deathStages[4].ticks = deathStages[3].ticks + 1000 * 15;
	
	for (i = 0 ; i < (int)ARRAY_SIZE(deathStages) ; i++)
		deathStages[i].given = FALSE;
}

//======================================================================
// setup a signal handler for int commands
//======================================================================
void sigint(int signal)
{
	TRACE(_WARN_ "Server: received INT signal\n");
	
	rotate_log_file();
}

//======================================================================
// setup a signal handler for kill -HUP commands
//======================================================================
void sighup(int signal)
{
	TRACE(_WARN_ "Server: received HUP signal\n");

	rotate_log_file();
	rotate_warn_log();

	ASSERT(_WARN_, hResourceInstance);
	ASSERT(_WARN_, hGameResourceInstance);

	if (hResourceInstance)
	{
		FreeLibrary(hResourceInstance);
		hResourceInstance = NULL;
	}

	if (hGameResourceInstance)
	{
		FreeLibrary(hGameResourceInstance);
		hGameResourceInstance = NULL;
	}

	init_strings();
}

void CheckDeathStage()
{
	char mfsBuffer[MAX_MSG];
	i18nStr getS;
	int i, textLength;
	DWORD tickCount;
	MFS_ROOM_CHAT_MESSAGE *mfs;
	
	tickCount = GetTickCount();
	
	for (i = 0 ; i < (int)ARRAY_SIZE(deathStages) ; i++) {
		if (deathStages[i].given)
			continue;

		if (tickCount < deathStages[i].ticks)
			break;
		
		deathStages[i].given = TRUE;
		
		mfs = (MFS_ROOM_CHAT_MESSAGE *)mfsBuffer;
		mfs->messageType		= MFS_ROOM_CHAT;
		mfs->playerIDFrom		= -1;
		mfs->privateIDTo		= 0;
		mfs->systemBroadcast	= TRUE;
		mfs->red				= 255;
		mfs->green				= 0;
		mfs->blue				= 0;

		strcpy(mfs->text, deathStages[i].message);
		textLength = strlen(deathStages[i].message) + 1;
		
		// add this chat line to the room's chat history
		roomServer->chatHistory[roomServer->chatHistoryTop].red = 255;
		roomServer->chatHistory[roomServer->chatHistoryTop].green = 0;
		roomServer->chatHistory[roomServer->chatHistoryTop].blue = 0;

		roomServer->chatHistory[roomServer->chatHistoryTop].labelLength = strlen(GetString(6, getS)); // "SYSTEM:"
		strcpy(roomServer->chatHistory[roomServer->chatHistoryTop].text, GetString(7, getS)); // "SYSTEM: "
		strcat(roomServer->chatHistory[roomServer->chatHistoryTop].text, mfs->text);
		roomServer->chatHistoryTop = (roomServer->chatHistoryTop + 1) % CHAT_HISTORY_LENGTH;
		
		roomServer->BroadcastRoomMessage((char *)mfs, sizeof(MFS_ROOM_CHAT_MESSAGE) + textLength);
		
		if (i == ARRAY_SIZE(deathStages) - 1)
			done = true;
		else
			roomServer->lastDatabaseWriteTickCount = 0; // force a write of all dirty database data
		
		break;
	}
}

int main(int argc, char *argv[])
{
	char commandLine[1024] = "";
	int i;
	i18nStr szPidDir, getS;
	pid_t pid;
	char s[256];
	struct passwd *pwd = NULL;
	
	szLogPrefix = strrchr(argv[0], '/');
	if (!szLogPrefix)
		szLogPrefix = argv[0];
	else
		szLogPrefix++;
	
	// install our signal handlers
	if (signal(SIGTERM, sigterm) == SIG_ERR)
		TRACE("unable to install TERM signal handler.\n");
	
	if (signal(SIGALRM, sigalarm) == SIG_ERR)
		TRACE("unable to install ALRM signal handler.\n");
	
	if (signal(SIGHUP, sighup) == SIG_ERR)
		TRACE(_ROUTINE_ "unable to install SIGHUP signal handler.\n");
	
	if (signal(SIGINT, sigint) == SIG_ERR)
		TRACE("unable to install INT signal handler.\n");
	
	for (i = 0 ; i < argc ; i++)
	{
		strcat(commandLine, argv[i]);
		strcat(commandLine, " ");
	}
	
	if (!ParseCommandLine(commandLine))
		return -1;
	
	// create and set the roomServer global object pointer
	if (!CreateServerObject())
		return -1;
	
	init_strings();

	/* if user defined then secure as user given */
	if (GetString(150, getS))
	{
		pwd = getpwnam(getS);
		if (pwd == (struct passwd *)NULL)
		{
   			fprintf(stderr, "httpd: unable to find user %s\n", getS);
  			return(-1);
		}
	}

/*
	if (Chroot != NULL) 
	{
		if (chroot(Chroot))
		{
			fprintf(stderr, "httpd: unable to chroot\n");
			return(-1);
		}
		
		if (chdir("/")) 
		{
   			fprintf(stderr, "httpd: unable to chdir\n");
   			return(-1);
		}
	}
*/

	if (getS)
		if (setgid(pwd->pw_gid) || setuid(pwd->pw_uid))
		{
   			fprintf(stderr, "httpd: unable to set user\n");
   			return(-1);
		}

	initlog();
	preflight();

	while (!done) {
		roomServer->Tick();
		if (dying)
			CheckDeathStage();
	}
	
	// we are trying to quit but we want to wait for all connections to close gracefully
	// before we shut down...
	
	roomServer->connections->CloseAll();
	while(roomServer->connections->GetClientCount())
		roomServer->Tick();
	
	delete roomServer;
	
	// clean-up our pid file
	pid = getpid();
	sprintf(s, "%s/%s%d.pid", GetString(28, szPidDir), szLogPrefix, (int)clientPort);
	remove(s);

	FreeLibrary(hResourceInstance);
	FreeLibrary(hGameResourceInstance);

	// dump out the memory 
	TRACE(_ROUTINE_ "memory leaked: %d bytes\nnew/del balance: %d\nmax memory: %d bytes\n",
		ServerMemoryInfo.numBytesAllocated,
		ServerMemoryInfo.newDeleteBalance,
		ServerMemoryInfo.maxMemoryAllocated
		);
	
	return 0;
}

#endif
