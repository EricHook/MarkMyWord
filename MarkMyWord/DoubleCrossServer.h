#ifndef _doublecrossserver
#define _doublecrossserver

#include "DoubleCrossMessage.h"

#include "RoomServer.h"

#include "DoubleCrossEngine.h"
#include "wgdict.h"

#ifdef _WIN32
	#define DICTIONARY_PATH "h:\\resources\\dictionary\\" 
#else
	#define DICTIONARY_PATH "../dictionary/" 
#endif

#define DCPOINTS_DEFUSE			16
#define DCPOINTS_GETCOLORBOMB	8
#define DCPOINTS_GETNESWBOMB	4
#define DCPOINTS_GETDIRBOMB		2

#define DBLCROSS_SEEDSIZE 5
#define	DBLCROSS_NUMMYSTERYTILES 6

class DblCrossPartWord
{
public:
	DblCrossPartWord();
	~DblCrossPartWord();
	
	int GetLength();
	
	// data
	DCPoint m_start;
	DCPoint m_finish;
	char m_word[DBLCROSS_BOARDWIDTH + 1];
	DblCrossDirection m_direction;
};

class DblCrossPartWordList
{
public:
	DblCrossPartWordList(DoubleCrossServerTable *table);
	~DblCrossPartWordList();
	
	void Reset();
	int Size();
	DblCrossPartWord *Add(DCPoint start, DCPoint finish, char *word, DblCrossDirection direction);
	DblCrossPartWord *At(int index);
	
	bool CreateWordList(DCPoint square, DblCrossDirection whichDir);
	bool GetLongestWord(DCPoint square, DCPoint step, DblCrossPartWord *word);
	void AddWordsToList(DblCrossPartWord *word, int startOffset, DCPoint step);
	DblCrossPartWord *FindSmallestPartWord(DblCrossDirection dir);
	
	// data
	DblCrossPartWord words[50];
	int m_nextWord;
	DoubleCrossServerTable *m_table;
};

class DoubleCrossServerPlayer : public RoomServerPlayer
{
public:
	DoubleCrossServerPlayer() { }
	~DoubleCrossServerPlayer() { }
};

class DoubleCrossServerTable : public RoomServerTable
{
public:
	DoubleCrossServerTable();
	~DoubleCrossServerTable();
	void Init(int theIndex);
	void InitPlayer(int seat);
	void InitSeat(int seat);
	void PlayerSits(DoubleCrossServerPlayer *client, PlayerState *playerState);
	void PlayerStands(DoubleCrossServerPlayer *client, int *newID, Boolean booted = FALSE);
	void StartGame();
	void RestartGame();
	int GetEmptyWatcherSeat();
	void GetTableGameValues(int *values);
	void SetTableGameValues(int *values);
	void SetAutoResult();
	
	////////
	
	int GetNextDOUBLECROSSPlayer(int p);
	void ResetDOUBLECROSSGAME();
	bool GameIsOver();
	void PossiblySuggestReset();
	
	void SetupGame();
	void CreateTiles();
	void SetupSeedWord();
	bool FindAndPurgeSeedWord(char *word);
	void AddSeedWordToBoard(char *word, DCPoint pt, DblCrossDirection dir);
	void CreateSpecialTiles();
	bool IsSquareGoodForSpecialTile(DCPoint square, int type);
	
	void PlaceBomb(int seat, char bombType, char rackSpot, DCPoint square, MFS_DOUBLECROSS_EVENT_MESSAGE *mfs);
	void NukeLine(DblCrossDirection dir, DCPoint square, bool bothDirs, MFS_DOUBLECROSS_EVENT_MESSAGE *mfs);
	bool PlaceTile(int seat, char letter, char rackSpot, DCPoint square, MFS_DOUBLECROSS_EVENT_MESSAGE *mfs);
	bool ProcessMove(int seat, char letter, char rackSpot, DCPoint square, MFS_DOUBLECROSS_EVENT_MESSAGE *mfs);
	void UpdateBoard(int seat, DblCrossPartWord *wordH, DblCrossPartWord *wordV,
					 DCPoint square, int rackSpot, MFS_DOUBLECROSS_EVENT_MESSAGE *mfs);
	void DoMysteryTile(int seat, DCPoint square, MFS_DOUBLECROSS_EVENT_MESSAGE *mfs);
	void StabilizeWord(DCPoint square, DblCrossPartWord *word, bool twoWords, int twoWordPoints, MFS_DOUBLECROSS_EVENT_MESSAGE *mfs);
	void StabilizeTile(DCPoint square, int points, MFS_DOUBLECROSS_EVENT_MESSAGE *mfs);
	void VerifyLine(DblCrossPartWord *word, DblCrossDirection dir, MFS_DOUBLECROSS_EVENT_MESSAGE *mfs);
	void ExplodeTiles(DCPoint square, DCPoint step, DCPoint stepR,
					  bool topLevel, bool recurse, bool bothDirs,
					  int specialTileUnderneath,
					  MFS_DOUBLECROSS_EVENT_MESSAGE *mfs);
	void RecheckForWords(MFS_DOUBLECROSS_EVENT_MESSAGE *mfs);
	void ExplodeColorTiles(int seat, DCPoint square, bool defuse, DblCrossColors bombColor, MFS_DOUBLECROSS_EVENT_MESSAGE *mfs);
	
	void ExchangeRack(int seat);
	bool FillRack(int seat);
	void DumpRack(int seat);
	void ClearBombData();
	bool ShortestPartialsDefined(DCPoint square, DblCrossPartWordList *wList);
	void CreatePartWordComment(char *comment, bool isLegal, DblCrossPartWordList *ls);
	int GetMysteryValue(DCPoint square);
	DblCrossPartWord *FindLongestLegalWord(DblCrossDirection dir, DblCrossPartWordList *ls);
	bool IsMarkedForDestruction(DCPoint square);
	void AddOrphan(bool topLevel, DCPoint square);
	bool IsOrphan(DCPoint square, DCPoint step);
	void TallyFinalScores();
	int GetWordForDefinition(DCPoint square, DblCrossDirection direction, char *word);
	
	////////
	
	void HandleDOUBLECROSSREPLACEPLAYER(MTS_DOUBLECROSS_REPLACEPLAYER_MESSAGE *mts);
	void SendEventsDone();
	void HandleDOUBLECROSSPLAY(DoubleCrossServerPlayer *client, MTS_DOUBLECROSS_PLAY_MESSAGE *mts);
	void HandleDOUBLECROSSDEFINITION(DoubleCrossServerPlayer *client, MTS_DOUBLECROSS_DEFINITION_MESSAGE *mts);
	void HandleDOUBLECROSSSHOWDOWN(MTS_DOUBLECROSS_SHOWDOWN_MESSAGE *mts);
	int HandleMessage(DoubleCrossServerPlayer *client, MTS_GAME_MESSAGE_HEADER *mts, int length);
	
	void RequestDOUBLECROSSREPLACEPLAYER(int seat, int playerID);
	Boolean RequestDOUBLECROSSPLAY(char *m, int *length);
	void RequestDOUBLECROSSSHOWDOWN(char *m, int *length);
	void Tick();
	
	/////////////////
	
	// Data
	
	bool			m_passedWithoutPlayingOnPreviousTurn[MAX_GAME_PLAYERS];
	bool			m_hasPlayedSinceLastDump[MAX_GAME_PLAYERS];
	
	char	m_special;		// If there is a special tile on current
							// space, put the index to it in state->specTiles here
	
	DCPoint			m_mysteryPoints[DBLCROSS_NUMMYSTERYTILES];
	int				m_mysteryValues[DBLCROSS_NUMMYSTERYTILES];
	
	char			m_tileStore[DBLCROSS_TILESTORESIZE];
	
	DoubleCrossEngine *m_engine;
	
	// computational temps
	
	int				m_nukeType;
	DblCrossColors	m_nukeColor;
	DCPoint			m_nukeOrigin;
	
	int				m_numOrphans; // used to keep track of orphans
	DCPoint			m_orphans[DBLCROSS_BOARDWIDTH * DBLCROSS_BOARDHEIGHT]; // array of orphans
	
	// We have three of these because, while checking in one direction,
	// we might have to check words off a given letter in another direction,
	// without deleting the original wordlist.  On occasion, you even need
	// three recursions (for instance, a vertical explosion destroys a tile,
	// which frees up a tile to its left to stabilize because of a vertical
	// word, so we want to check horizontally on that tile to make sure it
	// doesn't have a horizontal partial word still)  Whew!  That's a mouthful...
	
	DblCrossPartWordList *m_wordlist;
	DblCrossPartWordList *m_altWordList;
	DblCrossPartWordList *m_altaltWordList;
};

class DoubleCrossServer : public RoomServer
{
public:
	DoubleCrossServer();
	~DoubleCrossServer();
	
	void InitPlayerSpecificInfo(PlayerInfo *playerInfo);
	void FillGameRecord(DoubleCrossServerPlayer *client, int *grLength, char *gameRecord);
	int GetPreferredApproverSeat(int requestedSeat);
	int GetPreferredInvitationSeat(int inviterSeat);
	Boolean IsRestartAllowed() { return TRUE; }
	virtual bool ShowRestartMessage() { return false; }
	
#ifdef _DEBUG
	void GetGameMessageDescription(int messageType, char *message, char *description);
#endif
	
	WGDict *dictionary;
};

extern DoubleCrossServer *doublecrossServer;

#endif // _doublecrossserver
