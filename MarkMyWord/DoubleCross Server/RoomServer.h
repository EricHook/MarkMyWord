#ifndef _roomserver
#define _roomserver

#ifdef _WIN32
#	include "winos.h"
#	include <winsock.h>
#endif

#include "cmMsg.h"
#include "cmConn.h"
#include "cmConMgr.h"
#include "random.h"
#include "DBMessages.h"
#include "sendmail.h"

extern DWORD roomID;
extern int clientPort;
extern int risPort;
extern int adminPort;
extern char roomName[MAX_ROOM_NAME];
extern char gameName[MAX_GAME_NAME];
extern char sponsorURL[MAX_ROOM_NAME];
extern int roomFlags; //info stored in the ROOM_*** defines in hwmessages.h
extern char proxyNames[3][32];
extern int proxyPorts[3];
extern int CreateDatabaseUsers();

void GetServerStatusString(char *s);
void UpdateDisplay();

#define CHAT_HISTORY_LENGTH 40
#define DATABASE_TICKS (SecondsToTicks(60 * 5))
#define DATABASE_RECONNECT_TICKS (SecondsToTicks(20))
#define DATABASE_WARNING_TICKS (SecondsToTicks(60 * 2))
#define MAX_DATABASE_TRIES 3
#define BOOT_EXPIRATION_TICKS (SecondsToTicks(60))
#define PROBATION_TICKS (SecondsToTicks(60))
#define MAX_CLIENT_MESSAGE_QUEUE_DEPTH 10000

#define IS_A_CASINO_GAME ((roomID & FLAG_MONEY_GAME) != 0)

enum { CONNECTIONID_CLIENTLISTENER = 1, CONNECTIONID_CLIENT = 1001,
	   CONNECTIONID_RISLISTENER = 2, CONNECTIONID_RIS = 1002,
	   CONNECTIONID_DBCONNECTOR = 3, CONNECTIONID_DB = 1003 }; // connection types

enum { CS_EMPTY = 0, CS_JOINING, CS_ACTIVE, CS_LEAVING }; // table player changed types

enum { TPC_READY = 1, TPC_LEFT, TPC_BOOTED, TPC_SWITCHINGSEATS, TPC_REMOVED_NOTENOUGHMONEY }; // table player changed types

enum { DF_EMPTY = -3,
	   DF_ENTERED = -2,
	   DF_REQUESTED = -1,
	   DF_CLEAN = 0, DF_DIRTY, DF_SENDING, DF_DIRTY_WHILE_SENDING }; // dirty flag types

enum { DFI_LEVEL = 0,
	   DFI_SHADOW,
	   DFI_BANKROLL,
	   DFI_PROFILE,
	   DFI_GAMERECORD,
	   NUM_DIRTY_FLAGS }; // dirty flag indicies

enum { BV_NO_VOTE = 0, BV_VOTED_YES, BV_CAN_VOTE }; // boot vote states

enum { RRG_WIN, RRG_LOSS, RRG_DRAW, RRG_QUIT, RRG_FORFEIT }; //ratings reason for end of game (how did the winner win)

enum { RRU_SHOWDOWN, RRU_NOTHING, RRU_RESET, RRU_DROPPEDCONNECTION, RRU_LEFTTABLE, RRU_BOOT, RRU_MODERATORBOOT, RRU_MODERATORBAN}; //ratings reason for updating ratings

typedef struct {
	char red;
	char green;
	char blue;
	char isPrivate;
	char labelLength;
	char text[MAX_CHAT_LENGTH];
} ChatHistoryLine;

typedef struct {
	char login[MAX_PLAYER_NAME];
	int databaseID;
} DatabaseRecord;

class RoomServerPlayer
{
public:
	RoomServerPlayer() : machineID(dwPC1) { connection = NULL; status = CS_EMPTY; }
	~RoomServerPlayer() { }
	
	Boolean IsCustomer()			{ return (info.level & UL_CUSTOMER) != 0; }

	Boolean IsMonitor()				{ return (info.level & UL_MODERATOR); }
	Boolean IsSuperUser()			{ return (info.level & UL_SUPERUSER); }
	Boolean IsMonitorOrSuperUser()	{ return IsMonitor() || IsSuperUser(); }

	Boolean IsReferee()				{ return (info.level & UL_REFEREE) != 0; }
	Boolean IsMonitorOrReferee()	{ return IsMonitor() || IsReferee(); }

	Boolean IsTournamentAdministrator() 
									{ return (info.level & UL_TADMIN) != 0; }
	Boolean IsRefereeOrTA()			{ return IsReferee() || IsTournamentAdministrator(); }

	Boolean IsHelper()				{ return (info.level & UL_HELPER) != 0; }
	Boolean IsRalph()				{ return (info.level & UL_RALPH) != 0; }

	Boolean IsInTournament() { return (info.flags & UF_IN_TOURNAMENT) != 0; }

	short index;
	char status;
	HCONNECTION connection;
	PlayerInfo info;
	int databaseID;
	SNMachineID machineID;
	char email[MAX_EMAIL];
	char profile[MAX_PLAYER_PROFILE];
	char gameRecord[MAX_PLAYER_GAME_RECORD];
	char dirtyFlags[NUM_DIRTY_FLAGS];
	
	// requests
	
	int requestingTable; // the table I've requested to play at (-1 = no request)
	int requestingSeat; // the seat I've requested to sit in
	int approvingPlayerID; // the player who will approve or deny my request
	
	int requesterPlayerID; // the player who has asked to join my table (-1 = no requester)
	int requestedSeat; // the seat the player wants to sit in
	
	// invitations
	
	int inviterTable; // the table I've been invited to play at (-1 = no invitation)
	int inviterPlayerID; // the player who invited me to play or watch at that table
	int invitedToWatch; // TRUE if I've been invited to watch, FALSE if I've been invited to play
	
	// global mute
	
	Boolean globallyMuted;
	
	// booting
	
	int votesNeededToBoot; // -1 = not being booted
	DWORD lastVoteTicks;
	char votes[MAX_GAME_PLAYERS];
	
	int probationTable; // -1 = not on probation
	DWORD probationTicks;
};

class RoomServerTable
{
public:
	RoomServerTable() { }
	virtual ~RoomServerTable() { }
	void Init(int theIndex);
	virtual void InitPlayer(int seat) = 0;
	virtual void PlayerSits(ROOMSERVERPLAYER *client, PlayerState *playerState) = 0;
	virtual void PlayerStands(ROOMSERVERPLAYER *client, int *newID, Boolean booted) = 0;
	virtual void StartGame() = 0;
	virtual void RestartGame() { }
	virtual int GetEmptyWatcherSeat() = 0;
	virtual void GetTableGameValues(int *values) = 0;
	virtual void SetTableGameValues(int *values) = 0;
	
	virtual int HandleMessage(ROOMSERVERPLAYER *client, MTS_GAME_MESSAGE_HEADER *mts, int length) = 0;
	virtual void Tick() = 0;
	
	virtual bool CanPlayerAffordThisTable(ROOMSERVERPLAYER *client) { return TRUE; }

	//Used by ratings
	virtual bool PenaltyForLeavingNow(char seat) { return FALSE; } // Has a new game started so a leaving player is leaving in middle of game?
	virtual int GetNumPlayersAtTable() { return MAX_GAME_PLAYERS; }

	void CommonEndOfGame();			//common to all games
	virtual void EndOfGame() { };	//specific to each game

	Boolean PlayersRatingMatchesTable(short sRating);
	
	// winningSeat		-- seat of the winner (or the first seat for the partnership's win)
	// iNumPlayers		-- the number of non-empty seats (includes AI players)
	// resultOfGame		-- RRG_* enums.
	// reasonForUpdate	-- RRU_* enums.  Almost always use RRU_SHOWDOWN
	// bPartnershipGame -- Is the game that is calling UpdateRatings a partnership game or not?
	void UpdateRatings(int winningSeat, int iNumPlayers, char resultOfGame, char reasonForUpdate, Boolean bPartnersGame);

	int index;
	int status;
	int joinOption;
	int watchersOption;
	Boolean muteWatchers;
	char ratingsFlags;
	char ratingsSkills;
	char description[MAX_TABLE_DESCRIPTION];
	GameState gameState;
	int playerIndicies[MAX_GAME_PLAYERS];
	int watcherIndicies[MAX_GAME_WATCHERS];

	//Restart game
	char restartVotes[MAX_GAME_PLAYERS];
};

class RoomServerConnections : public TConnectionMgr
{
public:
	RoomServerConnections(int maxConnections, int maxListeners = 0);
	~RoomServerConnections();
	
	bool UsesAdminPort() const { return true; }
	short GetAdminPort() const { return adminPort; }
	void ProcessAdminRequest(HCONNECTION c);
	
	// callback methods
	void ConnectionCompleted(HCONNECTION c);
	void ClientConnected(HCONNECTION c);
	void ConnectionClosed(HCONNECTION c);
	void RequestHeartbeat(HCONNECTION c);
	void ProcessUnquedMessage(HCONNECTION c, char *buf, int length);
	void ProcessRawMessage(HCONNECTION c, char *buf, int length);
	const char *GetPlayerNameFromConnection(HCONNECTION c) const;
	
	int MyFlushAndClose(HCONNECTION c);
};

class RoomServer
{
public:
	RoomServer();
	virtual ~RoomServer();
	int Init();
	void UpdateLogs();
	void DisconnectSlackers();
	int Tick();
	
	int SendRoomHeader(ROOMSERVERPLAYER *client);
	int SendRoomTables(ROOMSERVERPLAYER *client);
	int SendRoomPlayers(ROOMSERVERPLAYER *client);
	int SendRoomChatHistory(ROOMSERVERPLAYER *client);
	int SendRoomTournamentInformation(ROOMSERVERPLAYER *client);
	int SendAIRatingInformation(ROOMSERVERPLAYER *client);
	
	int HandleMTS_RS_FIND_PLAYER(HCONNECTION c, char *message, int length);
	int HandleMTS_RS_NUMPLAYERS(HCONNECTION c, char *message, int length);
	
	int HandleMFDB_RS_CONNECT(HCONNECTION c, char *message, int length);
	int HandleMFDB_RS_FIELDSET(HCONNECTION c, char *message, int length);
	
	int HandleMTS_ROOM_ENTER(ROOMSERVERPLAYER *client, char *message, int length);
	int HandleMTS_ROOM_CHAT(ROOMSERVERPLAYER *client, char *message, int length);
	int HandleMTS_ROOM_CHANGE_SHADOW(ROOMSERVERPLAYER *client, char *message, int length);
	int HandleMTS_ROOM_CHANGE_PROFILE(ROOMSERVERPLAYER *client, char *message, int length);
	virtual int HandleMTS_ROOM_SIT(ROOMSERVERPLAYER *client, char *message, int length);
	int HandleMTS_ROOM_CHANGE_TABLE_SETTINGS(ROOMSERVERPLAYER *client, char *message, int length);
	int HandleMTS_ROOM_START_GAME(ROOMSERVERPLAYER *client, char *message, int length);
	int HandleMTS_ROOM_STAND(ROOMSERVERPLAYER *client, char *message, int length, Boolean switchingSeats = FALSE);
	int HandleMTS_ROOM_REQUEST(ROOMSERVERPLAYER *client, char *message, int length);
	int HandleMTS_ROOM_ANSWER_REQUEST(ROOMSERVERPLAYER *client, char *message, int length);
	int HandleMTS_ROOM_INVITE(ROOMSERVERPLAYER *client, char *message, int length);
	int HandleMTS_ROOM_RSVP(ROOMSERVERPLAYER *client, char *message, int length);
	int HandleMTS_ROOM_MODERATE(ROOMSERVERPLAYER *client, char *message, int length);
	int HandleMTS_ROOM_COMPLAIN(ROOMSERVERPLAYER *client, char *message, int length);
	int HandleMTS_ROOM_TI_CHECK_OUT_PAGE(ROOMSERVERPLAYER *client, char *message, int length);
	int HandleMTS_ROOM_TI_ADD_TO_TEXT(ROOMSERVERPLAYER *client, char *message, int length);
	int HandleMTS_ROOM_TI_JOIN(ROOMSERVERPLAYER *client, char *message, int length);
	int HandleMTS_ROOM_TI_SET_BANK(ROOMSERVERPLAYER *client, char *message, int length);
	int HandleMTS_ROOM_TI_GENERATE_ASSIGNMENTS(ROOMSERVERPLAYER *client, char *message, int length);
	int HandleMTS_ROOM_TI_CLEAR_AUTO_RESULTS(ROOMSERVERPLAYER *client, char *message, int length);
	int HandleMTS_ROOM_EXIT(ROOMSERVERPLAYER *client, char *message, int length);
	int HandleMTS_GAME_STATE(ROOMSERVERPLAYER *client, char *message, int length);
	int HandleMTS_GAME_CHAT(ROOMSERVERPLAYER *client, char *message, int length);
	int HandleMTS_GAME_BOOT(ROOMSERVERPLAYER *client, char *message, int length);
	int HandleMTS_GAME_RESTART(ROOMSERVERPLAYER *client, char *message, int length);
	
	void ReadTickerText();
	void ClearBootRecord(ROOMSERVERPLAYER *client, Boolean clearProbation);
	void ClearExpiredBoots();
	void ClearRestartVotes(int tableIndex);
	void GameStartSanityCheck(int table);
	int CancelRequestOrInvitation(ROOMSERVERPLAYER *client);
	int TransferApproverResponsibility(ROOMSERVERPLAYER *client);
	int RelinquishBootVotes(ROOMSERVERPLAYER *client);
	virtual int TablePlayerChanged(ROOMSERVERPLAYER *client, int status);
	int RelinquishTIPages(ROOMSERVERPLAYER *client);
	void RemovePlayersFromTables();
	int AddAutomaticResultsLine(int tableNumber, char *s);
	int TournamentPlayerLeftRoom(ROOMSERVERPLAYER *client);
	
	void AddChatToBuffer(char *pcStartOfBuffer, unsigned int uiSpaceLeftInBuffer);

	int BroadcastRoomMessage(char *message, int length);
	int BroadcastGameMessage(int table, char *message, int length);
	int BroadcastGameStringMessage(int table, char *title, char *text);
	int SendDisplayStringMessage(ROOMSERVERPLAYER *client, char *title, char *text, Boolean gameMessage = FALSE);
	int GenerateNextPlayerID();
	ROOMSERVERPLAYER *GetClientFromHCONNECTION(HCONNECTION c);
	ROOMSERVERPLAYER *GetClientFromID(int id);
	ROOMSERVERPLAYER *GetClientFromName(char *name);
	void UpdateNumClients();
	int CountActivePlayers();
	int CountPlayersAtTable(int table, Boolean includeWatchers);
	int CountPlayersAtTableFast(int table, Boolean includeWatchers);
	Boolean IsHumanPlayer(int table, int seat);
	void LogMachineId(char *heading, char *login, SNMachineID &playerMachineID);
	
	void DoDatabaseWork(Boolean init);
	void SendDirtyData(ROOMSERVERPLAYER *client);
	void MakeDataDirty(ROOMSERVERPLAYER *client, int dirtyFlagIndex);
	int ReportDatabaseError(ROOMSERVERPLAYER *client);
	void AddDatabaseRecord(ROOMSERVERPLAYER *client);
	bool CheckDatabaseRecord(ROOMSERVERPLAYER *client);
	
	virtual void ProcessGameRecord(ROOMSERVERPLAYER *client) {}
	
	virtual void InitPlayerSpecificInfo(PlayerInfo *playerInfo) = 0;
	virtual void FillGameRecord(ROOMSERVERPLAYER *client, int *grLength, char *gameRecord) = 0;
	virtual int GetPreferredApproverSeat(int requestedSeat) = 0;
	virtual int GetPreferredInvitationSeat(int inviterSeat) = 0;
	virtual Boolean NoJoinInProgress() { return FALSE; }
	virtual Boolean IsRestartAllowed() { return FALSE; }
	virtual bool ShowRestartMessage() { return true; }
	
	char *GetMessageDescription(char *message);
	virtual void GetGameMessageDescription(int messageType, char *message, char *description) = 0;
	
	// Rated games methods
	virtual void CalculateNewSinglesRating(int winningSeat, int iNumPlayers, char resultOfGame, char reasonForUpdate, 
		int iWins[], int iLosses[], int iDraws[], int iPartials[], int iRatings[], int iRatingDevs[], const GameState * gameState);

	virtual void CalculateNewPartnersRating(int winningSeat, int iNumPlayers, char resultOfGame, char reasonForUpdate, 
		int iWins[], int iLosses[], int iDraws[], int iPartials[], int iRatings[], int iRatingDevs[], const GameState * gameState);
	
	void InitRatings(ROOMSERVERPLAYER *p, int *piWins, int *piLosses, int *piDraws, int *piPartials, int *piRating, int *piRatingDev);
	
	virtual void SetRatingForAI(int *piWins, int *piLosses, int *piDraws, int *piPartials, int *piRating, int *piRatingDev);
	virtual void UpdateRatingForAI(int result, int ratingChange, int ratingDevChange);
	
	int ParseGameRecord(ROOMSERVERPLAYER *p, int *piWins, int *piLosses, int *piDraws, int *piPartials, int *piRating, int *piRatingDev);
	void UpdateGameRecord(ROOMSERVERPLAYER *p, int iWins, int iLosses, int iDraws, int iPartials, int iRating, int iRatingDev);

	Boolean IsRatedGame(int iTable) { return ((roomFlags & ROOM_IS_RATED_ROOM) || TableIsRatedGame(iTable)); }
	virtual Boolean TableIsRatedGame(int iTable);
	virtual bool IsPartnershipGame() { return FALSE; }

	Boolean IsTournamentRoom() { return (roomFlags & ROOM_IS_TOURNAMENT_ROOM); }
	Boolean IsRatedRoom() { return (roomFlags & ROOM_IS_RATED_ROOM); }
	Boolean IsLadderRoom() { return (roomFlags & ROOM_IS_LADDER_ROOM); }

	virtual Boolean GameHasAI() { return FALSE; }

	// Public data...
	int numClients;
	int nextPlayerID;
	ROOMSERVERPLAYER *clients;
	DatabaseRecord *dbRecords; // a redundant copy of the login/databaseID pair for safety
	RoomServerConnections *connections;
	HCONNECTION dbConnection;
	int databaseTries;
	int lastDatabaseConnectAttempt;
	int lastDatabaseWriteTickCount;
	int lastDatabaseWarning;
	ROOMSERVERTABLE *tables;
	Random *randomGenerator;
	ChatHistoryLine chatHistory[CHAT_HISTORY_LENGTH];
	int chatHistoryTop;
	char tickerText[512];
	TournamentInformation tournamentInformation;
	Boolean shuttingDown;

	int m_iAIRatingWins;
	int m_iAIRatingLosses;
	int m_iAIRatingDraws;
	int m_iAIRatingPartials;
	int m_iAIRating;
	int m_iAIRatingDev;
	
	DWORD gameStarts;
};

extern ROOMSERVER *roomServer;

void rotate_log_file();

#endif
