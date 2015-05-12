#include "ServerInclude.h"
#include "rismsg.h"

#include <math.h>

#define DEFAULT_SERVER_PUMP_RATE_HZ		50
#define SLOW_SERVER_PUMP_RATE_HZ		4

extern bool done;

RoomServer::RoomServer()
{
	connections = NULL;
	dbConnection = NULL;
	clients = NULL;
	tables = NULL;
	randomGenerator = NULL;
	shuttingDown = FALSE;
	dbRecords = NULL;
	gameStarts = 0;

	m_iAIRatingWins		= 0;
	m_iAIRatingLosses	= 0;
	m_iAIRatingDraws	= 0;
	m_iAIRatingPartials	= 0;
	m_iAIRating			= 0;
	m_iAIRatingDev		= 0;
}

RoomServer::~RoomServer()
{
	shuttingDown = TRUE;
	
	if (connections != NULL) {
		connections->CloseAll();
		delete connections;
	}
	if (clients)
		delete[] clients;
	if (tables)
		delete[] tables;
	if (randomGenerator)
		delete randomGenerator;
	if (dbRecords)
		delete dbRecords;
	
	roomServer = NULL;
}

int RoomServer::Init()
{
	int i, j, err;
	
	m_iAIRating	= GameHasAI() ? 1400 : 0;

	connections = new RoomServerConnections(MAX_ROOM_PLAYERS, 5);
	if (!connections)
		{ TRACE(_ROUTINE_ "RoomServer::Init(): Unable to create connections."); return -1; }

	// servers only initiate connections to the Proxy server
	// IF the proxy ever moves to a remote location we might need to increase this value
	connections->SetConnectTimeoutTicks(SecondsToTicks(15));

	err = connections->Init();
	if (err)
		{ TRACE(_ROUTINE_ "RoomServer::Init(): Unable to initialize connections (error = %d).", err); return err; }
	
	err = connections->AddListener(clientPort, CONNECTIONID_CLIENTLISTENER, CT_QUEUED, NAGLE_DELAY_OFF);
	if (err)
		{ TRACE(_ROUTINE_ "RoomServer::Init(): Unable to add listener (error = %d).", err); return err; }
	
	err = connections->AddListener(risPort, CONNECTIONID_RISLISTENER, CT_QUEUED, NAGLE_DELAY_OFF);
	if (err)
		{ TRACE(_ROUTINE_ "RoomServer::Init(): Unable to add listener (error = %d).", err); return err; }
	
	connections->EnableHeartbeats();

	#ifdef __LINUX__

		// Lets try a 50Hz pump rate which should give us a latency overhead of ~20ms.
		connections->SetSleepIntervalHz(DEFAULT_SERVER_PUMP_RATE_HZ);
		connections->EnableHeartbeatDisconnects();
	#endif
	
	DoDatabaseWork(TRUE);
	
	clients = new ROOMSERVERPLAYER[MAX_ROOM_PLAYERS];
	if (!clients)
		{ TRACE(_ROUTINE_ "RoomServer::Init(): Unable to create client records."); return -1; }
	
	for (i = 0 ; i < MAX_ROOM_PLAYERS ; i++)
		clients[i].index = i;
	
	numClients = 0;
	nextPlayerID = 0;
	
	tables = new ROOMSERVERTABLE[NUM_ROOM_TABLES];
	if (!tables)
		{ TRACE(_ROUTINE_ "RoomServer::Init(): Unable to create table records."); return -1; }
	
	for (i = 0 ; i < NUM_ROOM_TABLES ; i++) {
		tables[i].Init(i);
		for (j = 0 ; j < MAX_GAME_PLAYERS ; j++)
			tables[i].playerIndicies[j] = -1;
		for (j = 0 ; j < MAX_GAME_WATCHERS ; j++)
			tables[i].watcherIndicies[j] = -1;
		for (j = 0; j < MAX_GAME_PLAYERS; j++)
			tables[i].restartVotes[j] = FALSE;
	}
	
	dbRecords = new DatabaseRecord[MAX_ROOM_PLAYERS];
	if (!dbRecords)
		{ TRACE(_ROUTINE_ "RoomServer::Init(): Unable to create database records."); return -1; }
	
	for (i = 0 ; i < MAX_ROOM_PLAYERS ; i++) {
		dbRecords[i].login[0] = 0;
		dbRecords[i].databaseID = -1;
	}
	
	randomGenerator = new Random();
	randomGenerator->Seed((int)timeGetTime());
	
	for (i = 0 ; i < CHAT_HISTORY_LENGTH ; i++) {
		chatHistory[i].red = 0;
		chatHistory[i].green = 0;
		chatHistory[i].blue = 0;
		chatHistory[i].isPrivate = FALSE;
		chatHistory[i].labelLength = 0;
		chatHistory[i].text[0] = 0;
	}
	chatHistoryTop = 0;
	
	ReadTickerText();
	
	tournamentInformation.header.sittingEnabled = TRUE;
	tournamentInformation.header.joinEnabled = TRUE;
	tournamentInformation.header.automaticResultsEnabled = FALSE;
	tournamentInformation.header.startingBankValue = FALSE;
	for (i = 0 ; i < 5 ; i++) {
		tournamentInformation.header.checkOutPlayerIDs[i] = -1;
		tournamentInformation.text[i][0] = 0;
	}
	
#ifdef _DEBUG
#ifdef _WIN32
#ifdef HOGS_POKER
	// CreateDatabaseUsers();
#endif
#endif
#endif

	UpdateDisplay();

	return 0;
}

void RoomServer::UpdateLogs()
{
	FILE *log_file;

	log_file = GetLogFile();

	// see if its time to rotate log files
	// save one older copy of the log file
	if (log_file && log_file != stdout && ftell(log_file) >= MAX_LOG_FILE_SIZE)
		rotate_log_file();

	// - see if its time to flush the log file
//	if ()
//		fflush(log_file);
}

void RoomServer::DisconnectSlackers()
{
	int i, depth;
	
	for (i = 0 ; i < MAX_ROOM_PLAYERS ; i++)
		if (clients[i].status == CS_ACTIVE) {
			depth = clients[i].connection->GetEstimatedQueDepth();
			if (depth > MAX_CLIENT_MESSAGE_QUEUE_DEPTH) {
				TRACE(_WARNB_ "Disconnected client %s because MAX_CLIENT_MESSAGE_QUEUE_DEPTH was exceeded.\n", clients[i].info.login);
				connections->MyFlushAndClose(clients[i].connection);
			}
		}
}

int RoomServer::Tick()
{
	char message[MAX_MSG];
	int i, length, connectionType, err;
	HCONNECTION c;
	ROOMSERVERPLAYER *client;
	MTS_GAME_MESSAGE_HEADER *mts;
	MESSAGE_HEADER *mh;

	#ifdef __LINUX__

	// If there isn't anyone currently connected, slow down our pump rate to 4Hz instead of 50Hz.  If
	//  someone is then connected, speed back up again.  This should ease the CPU strain from idle rooms.n --ca
	if(0 == numClients)
		connections->SetSleepIntervalHz(SLOW_SERVER_PUMP_RATE_HZ);
	else
		connections->SetSleepIntervalHz(DEFAULT_SERVER_PUMP_RATE_HZ);
	#endif

	// cleanup any Zombie children procs (Unix-only)
	WaitForChild();
	
	// update our log files as necessary
	UpdateLogs();
	
	// call the Connection Manager
	connections->Pump();
	connections->ServerYield();
	
	DoDatabaseWork(FALSE);

	for ( ; ; ) {
		length = connections->GetMessage(message, ROOM_MSG_SIZE, &c);
		if (length == 0)
			break;
		
		if (length == -1)
			{ ASSERT(_WARN_, FALSE); return -1; }
		
		connectionType = connections->GetConnectionID(c);
		
		switch (connectionType) 
		{
			case CONNECTIONID_CLIENT:
				client = GetClientFromHCONNECTION(c);
				if (client == NULL)
					{ ASSERT(_WARN_, FALSE); break; }
				if (client->connection != c)
					{ TRACE(_WARN_ "RoomServer::Tick(): connection linked to wrong client"); break; }
				
				mh = (MESSAGE_HEADER *)message;
				
				// if (mh->messageType != MTS_ROOM_HEARTBEAT)
				TRACE(_VERBOSE_ "Received from %s: %s.\n", client->info.login, GetMessageDescription(message));
				
				switch (mh->messageType) {
					// room messages
					
					case MTS_ROOM_ENTER:
						err = HandleMTS_ROOM_ENTER(client, message, length);
						break;
					
					case MTS_ROOM_CHAT:
						err = HandleMTS_ROOM_CHAT(client, message, length);
						break;
					
					case MTS_ROOM_CHANGE_SHADOW:
						err = HandleMTS_ROOM_CHANGE_SHADOW(client, message, length);
						break;
					
					case MTS_ROOM_CHANGE_PROFILE:
						err = HandleMTS_ROOM_CHANGE_PROFILE(client, message, length);
						break;
					
					case MTS_ROOM_SIT:
						err = HandleMTS_ROOM_SIT(client, message, length);
						break;
					
					case MTS_ROOM_CHANGE_TABLE_SETTINGS:
						// allow the case where the client stands but then sends CHANGE_TABLE_SETTINGS
						// before the original stand message gets back to the client
						if (client->info.table == -1)
							break;
						err = HandleMTS_ROOM_CHANGE_TABLE_SETTINGS(client, message, length);
						break;
					
					case MTS_ROOM_START_GAME:
						// allow the case where the client stands but then sends START_GAME
						// before the original stand message gets back to the client
						if (client->info.table == -1)
							break;
						err = HandleMTS_ROOM_START_GAME(client, message, length);
						break;
					
					case MTS_ROOM_STAND:
						// allow the case where the client stands but then stands again
						// before the original stand message gets back to the client
						if (client->info.table == -1)
							break;
						err = HandleMTS_ROOM_STAND(client, message, length);
						break;
					
					case MTS_ROOM_REQUEST:
						err = HandleMTS_ROOM_REQUEST(client, message, length);
						break;
					
					case MTS_ROOM_ANSWER_REQUEST:
						err = HandleMTS_ROOM_ANSWER_REQUEST(client, message, length);
						break;
					
					case MTS_ROOM_INVITE:
						err = HandleMTS_ROOM_INVITE(client, message, length);
						break;
					
					case MTS_ROOM_RSVP:
						err = HandleMTS_ROOM_RSVP(client, message, length);
						break;
					
					case MTS_ROOM_MODERATE:
						err = HandleMTS_ROOM_MODERATE(client, message, length);
						break;
					
					case MTS_ROOM_COMPLAIN:
						err = HandleMTS_ROOM_COMPLAIN(client, message, length);
						break;
					
					case MTS_ROOM_TI_CHECK_OUT_PAGE:
						err = HandleMTS_ROOM_TI_CHECK_OUT_PAGE(client, message, length);
						break;
					
					case MTS_ROOM_TI_ADD_TO_TEXT:
						err = HandleMTS_ROOM_TI_ADD_TO_TEXT(client, message, length);
						break;
					
					case MTS_ROOM_TI_JOIN:
						err = HandleMTS_ROOM_TI_JOIN(client, message, length);
						break;
					
					case MTS_ROOM_TI_SET_BANK:
						err = HandleMTS_ROOM_TI_SET_BANK(client, message, length);
						break;
					
					case MTS_ROOM_TI_GENERATE_ASSIGNMENTS:
						err = HandleMTS_ROOM_TI_GENERATE_ASSIGNMENTS(client, message, length);
						break;
					
					case MTS_ROOM_TI_CLEAR_AUTO_RESULTS:
						err = HandleMTS_ROOM_TI_CLEAR_AUTO_RESULTS(client, message, length);
						break;
					
					case MTS_ROOM_HEARTBEAT:
						// don't need to reply
						// this message was just sent to us to show the client is still alive
						
						// TODO - However, we could keep track of the round trip time for the
						// TODO - message as a measure of the player's network performance.  This
						// TODO - value could then be used to determine an appropriate timeout
						// TODO - period for that connection.
						break;
					
					// generic game messages
					
					case MTS_GAME_STATE:
					case MTS_GAME_CHAT:
					case MTS_GAME_BOOT:
					case MTS_GAME_RESTART:
					
					// game-specific messages
					
					default:
						
						mts = (MTS_GAME_MESSAGE_HEADER *)message;
						if (mts->table == -1)
							{ ASSERT(_WARN_, FALSE); break; }
						if (mts->table != client->info.table) {
							// allow the case where the client stands but then sends a game
							// message before the stand message gets back to the client
							if (client->info.table != -1)
								ASSERT(_WARN_, FALSE);
							break;
						}
						if (mts->table < 0 || mts->table >= NUM_ROOM_TABLES)
							{ ASSERT(_WARN_, FALSE); break; }
						if (tables[mts->table].status != TS_PLAYING)
							{ ASSERT(_WARN_, FALSE); break; }
						if (mh->messageType != MTS_GAME_BOOT)
							if (mts->playerID != client->info.playerID)
								{ ASSERT(_WARN_, FALSE); break; }
						if (mh->messageType != MTS_GAME_STATE) {
							if (mh->messageType != MTS_GAME_BOOT)
								if (mts->seat == -1 || mts->seat != client->info.seat)
									{ ASSERT(_WARN_, FALSE); break; }
							if (mts->seat < 0 || mts->seat >= (MAX_GAME_PLAYERS + MAX_GAME_WATCHERS))
								{ ASSERT(_WARN_, FALSE); break; }
						}
						
						switch (mh->messageType) {
							case MTS_GAME_STATE:
								err = HandleMTS_GAME_STATE(client, message, length);
								break;
							
							case MTS_GAME_CHAT:
								err = HandleMTS_GAME_CHAT(client, message, length);
								break;
							
							case MTS_GAME_BOOT:
								err = HandleMTS_GAME_BOOT(client, message, length);
								break;
							
							case MTS_GAME_RESTART:
								err = HandleMTS_GAME_RESTART(client, message, length);
								break;

							default:
								err = tables[mts->table].HandleMessage(client, mts, length);
								break;
						}
						break;
				}
				break;
			
			case CONNECTIONID_RIS:
				TRACE(_VERBOSE_ "Received from RIS: %s.\n", GetMessageDescription(message));

				mh = (MESSAGE_HEADER *)message;
				switch (mh->messageType) {
					case MTS_RS_FIND_PLAYER:
						err = HandleMTS_RS_FIND_PLAYER(c, message, length);
						break;
					case MTS_RS_NUMPLAYERS:
						err = HandleMTS_RS_NUMPLAYERS(c, message, length);
						break;
					default:
						ASSERT(_WARN_, FALSE);
						break;
				}
				connections->MyFlushAndClose(c);
				break;
			
			case CONNECTIONID_DB:
				TRACE(_ROUTINE_ "Received from database: %s.\n", GetMessageDescription(message));

				mh = (MESSAGE_HEADER *)message;
				switch (mh->messageType) {
					case MFDB_RS_CONNECT:
						HandleMFDB_RS_CONNECT(c, message, length);
						break;
					case MFDB_RS_SET_SHADOW:
					case MFDB_RS_SET_PROFILE:
					case MFDB_RS_SET_BANK:
					case MFDB_RS_SET_GAME_RECORD:
					case MFDB_RS_DISCONNECT:
						HandleMFDB_RS_FIELDSET(c, message, length);
						break;
					case MFDB_RS_SET_BANNED:
					case MFDB_RS_SET_UNBANNED:
					case MFDB_STATUS:
						// ignore result
						break;
					default:
						ASSERT(_WARN_, FALSE);
						break;
				}
				break;
			
			default:
				ASSERT(_WARN_, FALSE);
				break;
		}
	}
	
	for (i = 0 ; i < NUM_ROOM_TABLES ; i++)
		if (tables[i].status == TS_PLAYING)
			tables[i].Tick();
	
	ClearExpiredBoots();
	
	DisconnectSlackers();
	
	return 0;
}

/////////// Messages from a Room Information Server

int RoomServer::HandleMTS_RS_FIND_PLAYER(HCONNECTION c, char *message, int length)
{
	int err;
	MTS_RS_FIND_PLAYER_MESSAGE *mts = (MTS_RS_FIND_PLAYER_MESSAGE *)message;
	MFS_RS_FIND_PLAYER_MESSAGE mfs;
	
	mfs.messageType = MFS_RS_FIND_PLAYER;
	strcpy(mfs.playerName, mts->playerName);
	mfs.wasFound = GetClientFromName(mts->playerName) != NULL;
	mfs.playerCount = numClients;
	
	TRACE(_VERBOSE_ "Sending to RIS: %s.\n", GetMessageDescription((char *)&mfs));
	
	err = connections->PostMessage(c, (char *)&mfs, sizeof(MFS_RS_FIND_PLAYER_MESSAGE));
	if (err)
		TRACE(_WARN_ "RoomServer::HandleMTS_RS_FIND_PLAYER(): connections->PostMessage() returned error %d\n", err);
	
	return err;
}

//======================================================================
// Respond to a RIS query for our player count
//======================================================================
int RoomServer::HandleMTS_RS_NUMPLAYERS(HCONNECTION c, char *message, int length)
{
	int err;
	MTS_RS_NUMPLAYERS_MESSAGE *mts = (MTS_RS_NUMPLAYERS_MESSAGE *)message;
	MFS_RS_NUMPLAYERS_MESSAGE mfs;
	
	mfs.messageType			= MFS_RS_NUMPLAYERS;
	mfs.playerCount			= numClients;
	mfs.rtt					= mts->rtt;
	mfs.gameStarts			= gameStarts;
	mfs.heartbeatBoots		= connections->GetBoots();

	mfs.warnMessages		= g_warnMessages;
	mfs.crashMessages		= g_crashMessages;
	mfs.criticalMessages	= g_criticalMessages;

	// report our status code back to the RIS
	mfs.serverStatusCode	= 0;

	if (connections->GetSleepIntervalHz() < DEFAULT_SERVER_PUMP_RATE_HZ)
		mfs.serverStatusCode |= SS_SERVER_IDLE;

	if (dbConnection != NULL)
		mfs.serverStatusCode |= SS_DB_CONNECTED;

	TRACE(_VERBOSE_ "Sending to RIS: %s.\n", GetMessageDescription((char *)&mfs));
	
	err = connections->PostMessage(c, (char *)&mfs, sizeof(MFS_RS_NUMPLAYERS_MESSAGE));
	if (err)
		TRACE(_WARN_ "RoomServer::HandleMTS_RS_NUMPLAYERS(): connections->PostMessage() returned error %d\n", err);
	
	return err;
}

/////////// Messages from the database

int RoomServer::SendRoomHeader(ROOMSERVERPLAYER *client)
{
	char *p, s[256], buffer[MAX_MSG];
	int err;
	MFS_ROOM_STATUS_HEADER_MESSAGE *mfs = (MFS_ROOM_STATUS_HEADER_MESSAGE *)buffer;
	
	mfs->messageType = MFS_ROOM_STATUS_HEADER;
	sprintf(s, "%s - %s", gameName, roomName);
	strnzcpy(mfs->roomName, s, MAX_ROOM_NAME - 1); // this name is ignored by the client
	mfs->numTables = NUM_ROOM_TABLES;
	mfs->numPlayers = CountActivePlayers();
	mfs->numChatLines = 0;
	strcpy(mfs->sponsorURL, sponsorURL);
	mfs->roomFlags = roomFlags;
	mfs->adChangeRateInSeconds = 60;
	strcpy(mfs->adTags, AD_TAGS_STRING);
	p = strstr(mfs->adTags, "room=standard"); // we assume the room= part is at the END of the tag string
	if (p)
		strcpy(&p[5], roomName);
	mfs->adTagLength = strlen(mfs->adTags) + 1;
	
	TRACE(_ROUTINE_ "Sending to %s (playerID = %d): %s.\n", client->info.login, client->info.playerID, GetMessageDescription((char *)mfs));
	
	err = connections->PostMessage(client->connection, (char *)mfs, sizeof(MFS_ROOM_STATUS_HEADER_MESSAGE) + mfs->adTagLength);
	if (err)
		TRACE(_WARN_ "RoomServer::SendRoomHeader(): connections->PostMessage() returned error %d\n", err);
	
	return err;
}

int RoomServer::SendRoomTables(ROOMSERVERPLAYER *client)
{
	char buffer[MAX_MSG];
	int i, b, err;
	MFS_ROOM_STATUS_TABLES_MESSAGE *mfs = (MFS_ROOM_STATUS_TABLES_MESSAGE *)buffer;
	
	mfs->messageType = MFS_ROOM_STATUS_TABLES;
	for (i = 0 ; i < NUM_ROOM_TABLES ; i++) {
		mfs->lastMessageOfThisType	= i == (NUM_ROOM_TABLES - 1);
		mfs->index					= i;
		mfs->status					= tables[i].status;
		mfs->joinOption				= tables[i].joinOption;
		mfs->watchersOption			= tables[i].watchersOption;
		mfs->muteWatchers			= tables[i].muteWatchers;
		mfs->ratingsFlags			= tables[i].ratingsFlags;
		mfs->ratingsSkills			= tables[i].ratingsSkills;

		tables[i].GetTableGameValues(mfs->gameValues);
		ASSERT(_WARN_, MAX_GAME_PLAYERS <= 32);
		mfs->aiBits = 0;
		for (b = 0 ; b < MAX_GAME_PLAYERS ; b++)
			if (tables[i].gameState.players[b].playerID == PLAYER_ID_AI)
				mfs->aiBits |= (1 << b);
		mfs->voidBits = 0;
		for (b = 0 ; b < MAX_GAME_PLAYERS ; b++)
			if (tables[i].gameState.players[b].playerID == PLAYER_ID_VOID)
				mfs->voidBits |= (1 << b);
		mfs->descriptionLength = strlen(tables[i].description) + 1;
		strcpy(mfs->description, tables[i].description);
		
		if (mfs->lastMessageOfThisType)
			TRACE(_ROUTINE_ "Sending to %s (playerID = %d): %s.\n", client->info.login, client->info.playerID, GetMessageDescription((char *)mfs));
		
		err = connections->PostMessage(client->connection, (char *)mfs, sizeof(MFS_ROOM_STATUS_TABLES_MESSAGE) + mfs->descriptionLength);
		if (err)
			TRACE(_WARN_ "RoomServer::SendRoomHeader(): connections->PostMessage() returned error %d\n", err);
	}
	
	return err;
}

int RoomServer::SendRoomPlayers(ROOMSERVERPLAYER *client)
{
	char buffer[MAX_MSG];
	int i, err;
	MFS_ROOM_STATUS_PLAYERS_MESSAGE *mfs = (MFS_ROOM_STATUS_PLAYERS_MESSAGE *)buffer;
	
	mfs->messageType = MFS_ROOM_STATUS_PLAYERS;
	mfs->terminator = FALSE;
	
	for (i = -1 ; i < MAX_ROOM_PLAYERS ; i++) {
		if (i == -1) {
			mfs->info = client->info; // put the player first in his own array of room players
			mfs->profileLength = strlen(client->profile) + 1;
			strcpy(mfs->profile, client->profile);
		}
		else {
			if (clients[i].status != CS_ACTIVE || clients[i].info.playerID == client->info.playerID)
				continue;
			mfs->info = clients[i].info;
			mfs->profileLength = strlen(clients[i].profile) + 1;
			strcpy(mfs->profile, clients[i].profile);
		}

		if (i == 0)
			TRACE(_ROUTINE_ "Sending to %s (playerID = %d): %s.\n", client->info.login, client->info.playerID, GetMessageDescription((char *)mfs));

		err = connections->PostMessage(client->connection, (char *)mfs, sizeof(MFS_ROOM_STATUS_PLAYERS_MESSAGE) + mfs->profileLength);
		if (err)
			TRACE(_WARN_ "RoomServer::SendRoomHeader(): connections->PostMessage() returned error %d\n", err);
	}
	
	mfs->terminator = TRUE;
	mfs->profileLength = 1;
	mfs->profile[0] = 0;

	TRACE(_ROUTINE_ "Sending to %s (playerID = %d): %s.\n", client->info.login, client->info.playerID, GetMessageDescription((char *)mfs));

	err = connections->PostMessage(client->connection, (char *)mfs, sizeof(MFS_ROOM_STATUS_PLAYERS_MESSAGE) + mfs->profileLength);
	if (err)
		TRACE(_WARN_ "RoomServer::SendRoomHeader(): connections->PostMessage() returned error %d\n", err);
	
	return err;
}

int RoomServer::SendRoomChatHistory(ROOMSERVERPLAYER *client)
{
	char buffer[MAX_MSG];
	int i, n, err;
	MFS_ROOM_STATUS_CHAT_HISTORY_MESSAGE *mfs = (MFS_ROOM_STATUS_CHAT_HISTORY_MESSAGE *)buffer;
	
	mfs->messageType = MFS_ROOM_STATUS_CHAT_HISTORY;
	
	for (i = chatHistoryTop, n = 0 ; n < CHAT_HISTORY_LENGTH ; i = (i + 1) % CHAT_HISTORY_LENGTH, n++) {
		if (chatHistory[i].text[0] == 0)
			continue;

		if(chatHistory[i].isPrivate)
			continue;
		
		mfs->terminator = FALSE;
		mfs->red = chatHistory[i].red;
		mfs->green = chatHistory[i].green;
		mfs->blue = chatHistory[i].blue;
		mfs->labelLength = chatHistory[i].labelLength;
		strcpy(mfs->text, chatHistory[i].text);
		mfs->textLength = strlen(mfs->text) + 1;
		

		if (i == 0)
			TRACE(_ROUTINE_ "Sending to %s (playerID = %d): %s.\n", client->info.login, client->info.playerID, GetMessageDescription((char *)mfs));

		
		err = connections->PostMessage(client->connection, (char *)mfs, sizeof(MFS_ROOM_STATUS_CHAT_HISTORY_MESSAGE) + mfs->textLength);
		if (err)
			TRACE(_WARN_ "RoomServer::SendRoomHeader(): connections->PostMessage() returned error %d\n", err);
	}
	
	mfs->terminator = TRUE;
	mfs->red = 0;
	mfs->green = 0;
	mfs->blue = 0;
	mfs->labelLength = 0;
	mfs->text[0] = 0;
	mfs->textLength = 0;

	TRACE(_ROUTINE_ "Sending to %s (playerID = %d): %s.\n", client->info.login, client->info.playerID, GetMessageDescription((char *)mfs));

	
	err = connections->PostMessage(client->connection, (char *)mfs, sizeof(MFS_ROOM_STATUS_CHAT_HISTORY_MESSAGE));
	if (err)
		TRACE(_WARN_ "RoomServer::SendRoomHeader(): connections->PostMessage() returned error %d\n", err);
	
	return err;
}

int RoomServer::SendRoomTournamentInformation(ROOMSERVERPLAYER *client)
{
	char *p, buffer[MAX_MSG];
	int pageNumber, i, n, err;
	MFS_ROOM_TI_CHECK_OUT_PAGE_MESSAGE mfs;
	MFS_ROOM_TI_ADD_TO_TEXT_MESSAGE *mfsTEXT = (MFS_ROOM_TI_ADD_TO_TEXT_MESSAGE *)buffer;
	
	mfs.messageType = MFS_ROOM_TI_CHECK_OUT_PAGE;
	mfs.playerID = -1;
	mfs.header = tournamentInformation.header;
	
	mfsTEXT->messageType = MFS_ROOM_TI_ADD_TO_TEXT;
	
	for (pageNumber = 0 ; pageNumber < 5 ; pageNumber++) {
		// send the header values for the page in a "fake" check-in message
		
		mfs.pageNumber = pageNumber;
		

		TRACE(_ROUTINE_ "Sending to %s (playerID = %d): %s.\n", client->info.login, client->info.playerID, GetMessageDescription((char *)&mfs));

		err = connections->PostMessage(client->connection, (char *)&mfs, sizeof(MFS_ROOM_TI_CHECK_OUT_PAGE_MESSAGE));
		if (err)
			TRACE(_WARN_ "RoomServer::SendRoomTournamentInformation(): connections->PostMessage() returned error %d\n", err);
		
		// send the text for the page
		
		mfsTEXT->pageNumber = pageNumber;
		
		p = tournamentInformation.text[pageNumber];
		
		for (i = 0 ; p[i] ; ) {
			mfsTEXT->clear = (i == 0);
			n = strlen(&p[i]);
			if (n > 255)
				n = 255;
			strnzcpy(mfsTEXT->text, &p[i], n);
			i += n;
			
			TRACE(_ROUTINE_ "Sending to %s (playerID = %d): %s.\n", client->info.login, client->info.playerID, GetMessageDescription((char *)mfsTEXT));
			
			err = connections->PostMessage(client->connection, (char *)mfsTEXT, sizeof(MFS_ROOM_TI_ADD_TO_TEXT_MESSAGE) + n + 1);
			if (err)
				TRACE(_WARN_ "RoomServer::SendRoomTournamentInformation(): connections->PostMessage() returned error %d\n", err);
		}
	}
	
	return 0;
}

int RoomServer::SendAIRatingInformation(ROOMSERVERPLAYER *client)
{
	int err;
	MFS_ROOM_PLAYER_RATING_MESSAGE mfs;
	
	mfs.messageType	= MFS_ROOM_PLAYER_RATING;
	mfs.playerID	= -1;
	mfs.wins		= roomServer->m_iAIRatingWins;
	mfs.losses		= roomServer->m_iAIRatingLosses;
	mfs.draws		= roomServer->m_iAIRatingDraws;
	mfs.quits		= roomServer->m_iAIRatingPartials;
	mfs.rating		= roomServer->m_iAIRating;
	mfs.ratingDev	= roomServer->m_iAIRatingDev;

	TRACE(_ROUTINE_ "Sending to %s (playerID = %d): %s.\n", client->info.login, client->info.playerID, GetMessageDescription((char *)&mfs));
	
	err = connections->PostMessage(client->connection, (char *)&mfs, sizeof(MFS_ROOM_PLAYER_RATING_MESSAGE));
	if (err)
		TRACE(_WARN_ "RoomServer::SendAIRatingInformation(): connections->PostMessage() returned error %d\n", err);
	
	return err;
}

int RoomServer::HandleMFDB_RS_CONNECT(HCONNECTION c, char *message, int length)
{
	char buffer[MAX_MSG];
	i18nStr getS, getS2;
	int i, err;
	MFDB_RS_CONNECT_MESSAGE *mts	= (MFDB_RS_CONNECT_MESSAGE *)message;
	MFS_ROOM_ENTER_MESSAGE *mfsREM	= (MFS_ROOM_ENTER_MESSAGE *)buffer;
	MFS_ROOM_FULL_MESSAGE mfsRFM;
	ROOMSERVERPLAYER *client;
	
	if (mts->magicIndex < 0 || mts->magicIndex > MAX_ROOM_PLAYERS)
		{ ASSERT(_WARN_, FALSE); return -1; }
	
	client = &roomServer->clients[mts->magicIndex];
	
	if (mts->dbStat == DBS_NOT_FOUND) 
	{
		TRACE(_WARN_ "RoomServer::HandleMFDB_RS_CONNECT() not found user entered [%s]\n", client->info.login);
		SendDisplayStringMessage(client, GetString(29, getS), GetString(30, getS2)); // "Name Not Found", "Your user name was not found in the database.  Please try again, making sure you sign in properly."
		connections->MyFlushAndClose(client->connection);
		
		return -1;
	}

	if (client->databaseID != mts->playerID) 
	{
		TRACE(_WARN_ "RoomServer::HandleMFDB_RS_CONNECT() client->databaseID (%d) != mts->playerID (%d)\n", client->databaseID, mts->playerID);
/*
 *		for(i = 0; i < roomServer->numClients; i++)
 *		{
 *			if(roomServer->clients[i].databaseID == mts->playerID)
 *				break;
 *		}
 *
 *		if(i < roomServer->numClients)
 *			//We found mts->playerID!!!
 *			TRACE(_WARN_ "RoomServer::HandleMFDB_RS_CONNECT() Player from database (%d, %s) has status %d.\n", roomServer->clients[i].databaseID, roomServer->clients[i].info.login, roomServer->clients[i].status);
 *		else
 *			TRACE(_WARN_ "RoomServer::HandleMFDB_RS_CONNECT() Player from database (%d) is not currently in this server.\n", mts->playerID);
 *
 *		TRACE(_WARN_ "RoomServer::HandleMFDB_RS_CONNECT() DoDatabaseWork in %d ticks,  Player Count:%d, dbStatus:%d\n", GetTickCount() - roomServer->lastDatabaseWriteTickCount, roomServer->numClients, mts->dbStat);
 */
		
		//Don't kick anyone out at this point.  The odds are in favor of this client still being around, and a later
		// message from the database will match correctly.  Just don't proceed with connecting this player.
//		SendDisplayStringMessage(client, GetString(31, getS), GetString(32, getS2)); // "Access Denied", "Invalid database ID received."
//		connections->MyFlushAndClose(client->connection);
		
		return 0;
	}
	
	if (mts->dbStat == DBS_BANNED) 
	{
		TRACE(_WARN_ "RoomServer::HandleMFDB_RS_CONNECT() banned user entered [%s]\n", client->info.login);
		SendDisplayStringMessage(client, GetString(31, getS), GetString(33, getS2)); // "Access Denied", "Access to the system has been denied."
		connections->MyFlushAndClose(client->connection);
		
		return -1;
	}
	
	if (mts->dbStat != DBS_SUCCESS) 
	{
		TRACE(_WARN_ "RoomServer::HandleMFDB_RS_CONNECT() unable to connect user [%s], dbStat: %d\n", client->info.login, mts->dbStat);
		SendDisplayStringMessage(client, GetString(34, getS), GetString(35, getS2)); // "Unable to Connect", "Please try again in a few minutes."
		connections->MyFlushAndClose(client->connection);
		
		return -1;
	}
	
	if (stricmp(client->info.login, mts->login)) 
	{
		TRACE(_ROUTINE_ "RoomServer::HandleMFDB_RS_CONNECT() wrong login returned [%s vs. %s]\n", client->info.login, mts->login);

		// "Bad Login Name", "Please contact won.net for help with your account."
		SendDisplayStringMessage(client, GetString(36, getS), GetString(37, getS2)); 
		connections->MyFlushAndClose(client->connection);
		
		return -1;
	}
	
	if (client->status != CS_JOINING)
		return 0; // allow this case for when two CONNECT messages were sent to the database
				  // because of a database disconnect
	
	// if the player is not a monitor and there are fewer than 10 spots left in the room,
	// send a message back to the client saying the room is full
	
	client->info.level = mts->type;
	if (!client->IsMonitorOrSuperUser() && (numClients > (MAX_ROOM_PLAYERS - 10))) 
	{
		mfsRFM.messageType = MFS_ROOM_FULL;
		mfsRFM.numPlayers = numClients;

		TRACE(_ROUTINE_ "Sending to %s (playerID = %d): %s.\n", client->info.login, client->info.playerID, GetMessageDescription((char *)&mfsRFM));

		err = connections->PostMessage(c, (char *)&mfsRFM, sizeof(MFS_ROOM_FULL_MESSAGE));
		if (err)
			TRACE(_WARN_ "RoomServer::HandleMFDB_RS_CONNECT(): PostMessage() returned error %d", err);
		connections->MyFlushAndClose(client->connection);
		
		return -1;
	}
	
	ASSERT(_WARN_, mts->bank >= -1 && mts->bank <= 1000000000);

	if (client->dirtyFlags[DFI_SHADOW] != DF_CLEAN)
		client->info.shadow				= mts->shadow;

	if (client->dirtyFlags[DFI_BANKROLL] != DF_CLEAN)
	{
		client->info.bankRoll			= mts->bank;

		if (mts->bank < 0)
			TRACE(_WARN_ "RoomServer::HandleMFDB_RS_CONNECT(): Joining(non-ghost) player [name = %s, id = %d] bankroll is corrupted [%d], %s line: %d\n", 
				client->info.login,
				client->databaseID,
				mts->bank,
				__FILE__,
				__LINE__
			);
	}

	if (client->dirtyFlags[DFI_GAMERECORD] != DF_CLEAN)
		strnzcpy(client->gameRecord, mts->gameRecord, MAX_PLAYER_GAME_RECORD - 1);

	if (client->dirtyFlags[DFI_PROFILE] != DF_CLEAN)
		strnzcpy(client->profile, mts->profile, MAX_PLAYER_PROFILE - 1);

	strnzcpy(client->email, mts->email, MAX_EMAIL - 1);

	if(!(client->gameRecord[0] == 0 || (client->gameRecord[0] == 'r' && client->gameRecord[1] == ':')))
		TRACE(_WARN_ "Illegal game record!!!  It is %s\n", client->gameRecord);
	
	//Process the game record (first, the rating info, then make a game-specific call).
	err = ParseGameRecord(client, &client->info.wins, &client->info.losses, &client->info.draws, &client->info.quits, &client->info.rating, &client->info.ratingDev);
	if(err == 0 || err == EOF)
		InitRatings(client, &client->info.wins, &client->info.losses, &client->info.draws, &client->info.quits, &client->info.rating, &client->info.ratingDev);

	ProcessGameRecord(client);

	AddDatabaseRecord(client);
	
	// the player is now officially "here"
	
	client->status = CS_ACTIVE;
	for (i = 0 ; i < NUM_DIRTY_FLAGS ; i++)
		client->dirtyFlags[i] = DF_CLEAN;
	UpdateDisplay();
	
	// send room information to this entering player
	
	err = SendRoomHeader(client);
	// if (err) return err;
	err = SendRoomTables(client);
	// if (err) return err;
	err = SendRoomPlayers(client);
	// if (err) return err;
	err = SendRoomChatHistory(client);
	// if (err) return err;
	err = SendRoomTournamentInformation(client);
	// if (err) return err;
	err = SendAIRatingInformation(client);
	// if (err) return err;
	
	// let everyone know this player has joined
	
	mfsREM->messageType		= MFS_ROOM_ENTER;
	mfsREM->player			= client->info;
	mfsREM->numPlayers		= CountActivePlayers();
	mfsREM->profileLength	= strlen(client->profile) + 1;
	strcpy(mfsREM->profile, client->profile);
	
	return BroadcastRoomMessage((char *)mfsREM, sizeof(MFS_ROOM_ENTER_MESSAGE) + mfsREM->profileLength);
}

int RoomServer::HandleMFDB_RS_FIELDSET(HCONNECTION c, char *message, int length)
{
	int dirtyFlagIndex = -1;
	MFDB_RS_SET_SHADOW_MESSAGE *mts = (MFDB_RS_SET_SHADOW_MESSAGE *)message; // all messages have the same fields
	ROOMSERVERPLAYER *client;
	
	if (mts->magicIndex < 0 || mts->magicIndex > MAX_ROOM_PLAYERS)
		{ ASSERT(_WARN_, FALSE); return -1; }
	
	client = &roomServer->clients[mts->magicIndex];
	
	if (client->status == CS_EMPTY)
		return 0; // allow this for the case that we sent two DISCONNECT messages in a row
	
	if (client->databaseID != mts->playerID)
		{ ASSERT(_WARN_, FALSE); return -1; }
	
	/*
	if (mts->dbStat != DBS_SUCCESS) // for MFDB_RS_DISCONNECT messages, we must give up now or we could loop forever
		if (mts->messageType != MFDB_RS_DISCONNECT)
			{ ReportDatabaseError(client); return -1; }
	*/
	
	switch (mts->messageType) 
	{
	case MFDB_RS_SET_SHADOW:		dirtyFlagIndex = DFI_SHADOW; break;
	case MFDB_RS_SET_PROFILE:		dirtyFlagIndex = DFI_PROFILE; break;
	case MFDB_RS_SET_BANK:			dirtyFlagIndex = DFI_BANKROLL; break;
	case MFDB_RS_SET_GAME_RECORD:	dirtyFlagIndex = DFI_GAMERECORD; break;
	case MFDB_RS_DISCONNECT:
		if (client->status != CS_LEAVING)
			{ ASSERT(_WARN_, FALSE); return -1; }
		
		client->status = CS_EMPTY;
		dbRecords[client->index].login[0] = 0;
		roomServer->UpdateNumClients();
		UpdateDisplay();
		break;

	default:
		ASSERT(_WARN_, FALSE);
		break;
	}
	
	if (dirtyFlagIndex != -1)
		switch (client->dirtyFlags[dirtyFlagIndex]) 
	{
		case DF_CLEAN:
			ASSERT(_WARN_, FALSE);
			break;
		case DF_DIRTY:
			ASSERT(_WARN_, FALSE);
			break;
		case DF_SENDING:
			client->dirtyFlags[dirtyFlagIndex] = DF_CLEAN;
			break;
		case DF_DIRTY_WHILE_SENDING:
			client->dirtyFlags[dirtyFlagIndex] = DF_DIRTY;
			break;
		default:
			ASSERT(_WARN_, FALSE);
			break;
	}
	
	return 0;
}

/////////// Messages from a player client

int RoomServer::HandleMTS_ROOM_ENTER(ROOMSERVERPLAYER *client, char *message, int length)
{
	i18nStr getS, getS2;
	int i, j;
	MTS_ROOM_ENTER_MESSAGE *mts = (MTS_ROOM_ENTER_MESSAGE *)message;
	
	ASSERT(_WARN_, client->status == CS_JOINING);
	
	if (mts->roomID != roomID) 
	{
		TRACE(_WARN_ "Player %s (id = %d) entered with roomID %lu instead of %lu.\n",
			client->info.login, 
			(int)mts->databaseID, 
			(int)mts->roomID, 
			(int)roomID
			);
		// "Cannot Connect"
		// "Received an invalid room ID."
		SendDisplayStringMessage(client, GetString(38, getS), GetString(39, getS2));
		connections->MyFlushAndClose(client->connection);
		
		return -1;
	}
	
	// filter out a bogus ENTER message
	if ('\0' == mts->login[0])
	{
		TRACE(_WARN_ "Login name in ENTER message was empty.\n");
		// "Invalid login name"
		// "Login user name was empty.  Contact WON.net if this continues."
		SendDisplayStringMessage(client, GetString(146, getS), GetString(147, getS2));
		connections->MyFlushAndClose(client->connection);
		return -1;
	}

	// check the client version codes
	if (HIWORD(mts->globalVersion) != GLOBAL_VERSION_MAJOR || HIWORD(mts->gameVersion) != GAME_VERSION_MAJOR)
	{
		TRACE(_WARN_ "Client (user = %s) failed version check (client = %08X, server = %08X).\n", client->info.login);
		// "Incorrect client version"
		// "You have an older version of the game client.  Please email hoylecomments@won.net if this continues."
		SendDisplayStringMessage(client, GetString(148, getS), GetString(149, getS2));
		connections->MyFlushAndClose(client->connection);
		return -1;
	}

	// trigger send of CONNECT message to database
	for (i = 0 ; i < NUM_DIRTY_FLAGS ; i++)
		client->dirtyFlags[i] = DF_ENTERED;

	strnzcpy(client->info.login, mts->login, MAX_PLAYER_NAME - 1);

	// strnzcpy(client->info.nickname, mts->nickname, MAX_PLAYER_NAME - 1);
	client->info.faceInfo	= mts->faceInfo;
	client->info.level		= mts->level;

	if (client->info.level < UL_FREEBIE || client->info.level > UL_CUSTOMER)
		client->info.level = UL_FREEBIE;

	client->info.shadow = 0;
	client->info.table	= -1;
	client->info.seat	= -1;
	client->info.tournamentBankRoll = 0;
	
	InitPlayerSpecificInfo(&clients->info);
	client->databaseID	= mts->databaseID;
	client->machineID	= mts->machineID;
	// LogMachineId("HandleMTS_ROOM_ENTER()", mts->login, mts->machineID);

#if !defined (__LINUX__)
	return 0;
#endif

	// disconnect any other players with this name in the room (ghosts of this joiner, hopefully)
	for (i = 0 ; i < MAX_ROOM_PLAYERS ; i++)
		if (client != &clients[i])
			if (!stricmp(clients[i].info.login, mts->login))
				// multiple copies of michael1 (databaseID 7590) are allowed for bot testing...
				if (clients[i].databaseID != 7590) 
				{
					TRACE(_WARNB_ "Disconnected ghost %s.\n", clients[i].info.login);

					// save interesting state info from the ghost before it goes away
//					client->info.tournamentBankRoll = clients[i].info.tournamentBankRoll;
//					if (clients[i].IsInTournament())
//						client->info.flags |= UF_IN_TOURNAMENT;

					client->globallyMuted = clients[i].globallyMuted;

					for (j = 0 ; j < NUM_DIRTY_FLAGS ; j++)
					{
						if (clients[i].dirtyFlags[j] == DF_DIRTY || clients[i].dirtyFlags[j] == DF_DIRTY_WHILE_SENDING)
						{
							switch (j) 
							{
							case DFI_LEVEL:
								continue;
								break;

							case DFI_SHADOW:
								client->info.shadow = clients[i].info.shadow;
								client->dirtyFlags[j] = DF_CLEAN;
								break;

							case DFI_BANKROLL:
								// bank values should _NEVER_ be less than 0
								if (clients[i].info.bankRoll < 0)
								{
									TRACE(_WARN_ "RoomServer::SendDirtyData(): Joining(ghost) player [name = %s, id = %d] bankroll is corrupted [%d], %s line: %d\n", 
										clients[i].info.login,
										clients[i].databaseID,
										clients[i].info.bankRoll,
										__FILE__,
										__LINE__
									);
									break;
								}

								client->info.bankRoll = clients[i].info.bankRoll;
								client->dirtyFlags[j] = DF_CLEAN;
								break;

							case DFI_PROFILE:
								strnzcpy(client->profile, clients[i].profile, MAX_PLAYER_PROFILE - 1);
								client->dirtyFlags[j] = DF_CLEAN;
								break;

							case DFI_GAMERECORD:
								strnzcpy(client->gameRecord, clients[i].gameRecord, MAX_PLAYER_GAME_RECORD - 1);
								client->dirtyFlags[j] = DF_CLEAN;
								break;

							default:
								ASSERT(_WARN_, FALSE);
								continue;
							}
						}
					}
					connections->MyFlushAndClose(clients[i].connection);
				}
	return 0;
}

int RoomServer::HandleMTS_ROOM_CHAT(ROOMSERVERPLAYER *client, char *message, int length)
{
	bool sendingToNonReferee;
	char *text, mfsBuffer[MAX_MSG];
	i18nStr getS, getS2;
	int i, j, n, textLength, err, *privateIDs;
	MTS_ROOM_CHAT_MESSAGE *mts = (MTS_ROOM_CHAT_MESSAGE *)message;
	MFS_ROOM_CHAT_MESSAGE *mfs;
	HCONNECTION recipients[MAX_ROOM_PLAYERS];
	
	if (mts->playerIDFrom != client->info.playerID)
		{ ASSERT(_WARN_, FALSE); return -1; }
	
	if (client->globallyMuted)
		return 0;
	
	text = &mts->data[mts->numPrivateIDsTo * sizeof(int)];
	
	// sanity check
	if (text <= message || text >= (message + length))
		{ ASSERT(_WARN_, FALSE); return -1; }
	
	textLength = (int)((message + length) - text);
	if (textLength > (MAX_CHAT_LENGTH - 1))
		textLength = (MAX_CHAT_LENGTH - 1);
	
	// pass the chat message on to the appropriate recipients
	
	mfs					= (MFS_ROOM_CHAT_MESSAGE *)mfsBuffer;
	mfs->messageType	= MFS_ROOM_CHAT;
	mfs->playerIDFrom	= mts->playerIDFrom;
	mfs->privateIDTo	= 0;
	
	//Set the chat color based on the player type
	if (client->IsHelper()) { //BLUE
		mfs->red	= 0;
		mfs->green	= (char)12;
		mfs->blue	= (char)155;
	}
	else if(client->IsTournamentAdministrator()) { //AQUA
		mfs->red	= (char)15;
		mfs->green	= (char)104;
		mfs->blue	= (char)104;
	}
	else if (client->IsReferee()) { //GREEN
		mfs->red	= 0;
		mfs->green	= (char)100;
		mfs->blue	= 0;
	}
	else if (client->IsSuperUser()) { //BLACK
		mfs->red = 0;
		mfs->green = 0;
		mfs->blue = 0;
	}
	else if (client->IsMonitor()) { //BLACK
		mfs->red = 0;	// 255
		mfs->green = 0; // 102
		mfs->blue = 0;
	}
	else { //BLACK
		mfs->red = 0;
		mfs->green = 0;
		mfs->blue = 0;
	}

	mfs->systemBroadcast = FALSE;
	strnzcpy(mfs->text, text, textLength);
	
	if (mts->numPrivateIDsTo == 0) {
		if (mts->systemBroadcast && client->IsMonitorOrSuperUser()) {
			mfs->systemBroadcast = TRUE;
			//RED
			mfs->red = (char)255;
			mfs->green = 0;
			mfs->blue = 0;
		}
		
		if (mfs->systemBroadcast && mfs->text[0] == '<') {
			// this is a ticker text insert
			// currently we don't add it to the chat log
		}
		else {
			// add this chat line to the room's chat history
			chatHistory[chatHistoryTop].red = mfs->red;
			chatHistory[chatHistoryTop].green = mfs->green;
			chatHistory[chatHistoryTop].blue = mfs->blue;
			chatHistory[chatHistoryTop].isPrivate = FALSE;
			strcpy(chatHistory[chatHistoryTop].text, client->info.login);
			chatHistory[chatHistoryTop].labelLength = strlen(client->info.login) + 1;
			strcat(chatHistory[chatHistoryTop].text, GetString(131, getS)); // ": "
			strcat(chatHistory[chatHistoryTop].text, mfs->text);
			chatHistoryTop = (chatHistoryTop + 1) % CHAT_HISTORY_LENGTH;
		}
		
		return BroadcastRoomMessage((char *)mfs, sizeof(MFS_ROOM_CHAT_MESSAGE) + textLength);
	}
	
	//PURPLE
	mfs->red = (char)128;
	mfs->green = 0;
	mfs->blue = (char)128;
	
	n = 0;
	sendingToNonReferee = false;
	privateIDs = (int *)&mts->data;
	for (j = 0 ; j < mts->numPrivateIDsTo ; j++) {
		for (i = 0 ; i < MAX_ROOM_PLAYERS ; i++)
			if (clients[i].status == CS_ACTIVE)
				if (clients[i].info.playerID == privateIDs[j])
					break;
		if (i < MAX_ROOM_PLAYERS) {
			if (!(clients[i].IsMonitorOrReferee() || clients[i].IsTournamentAdministrator() || clients[i].IsSuperUser()))
				sendingToNonReferee = TRUE;
			if (n == 0)
				mfs->privateIDTo = clients[i].info.playerID;
			recipients[n++] = clients[i].connection;

			TRACE(_ROUTINE_ "Sending to %s (playerID = %d): %s.\n", clients[i].info.login, clients[i].info.playerID, GetMessageDescription((char *)mfs));
		}
	}
	
	if (IsTournamentRoom() && client->info.table != -1 && sendingToNonReferee)
		return SendDisplayStringMessage(client, GetString(134, getS), GetString(135, getS2)); // "No Private Chat", "From a tournament table, you can send private chat only to a tournament director or host."

	if (IsRatedRoom() && client->info.table != -1 && sendingToNonReferee)
		return SendDisplayStringMessage(client, GetString(134, getS), GetString(141, getS2)); // "No Private Chat", "From a rated table, you can send private chat only to a tournament director or host."

	if (IsLadderRoom() && client->info.table != -1 && sendingToNonReferee)
		return SendDisplayStringMessage(client, GetString(134, getS), GetString(142, getS2)); // "No Private Chat", "From a ladder table, you can send private chat only to a tournament director or host."
	
	chatHistory[chatHistoryTop].red = mfs->red;
	chatHistory[chatHistoryTop].green = mfs->green;
	chatHistory[chatHistoryTop].blue = mfs->blue;
	chatHistory[chatHistoryTop].isPrivate = TRUE;
	strcpy(chatHistory[chatHistoryTop].text, client->info.login);
	chatHistory[chatHistoryTop].labelLength = strlen(client->info.login) + 1;
	strcat(chatHistory[chatHistoryTop].text, GetString(131, getS)); // ": "
	strcat(chatHistory[chatHistoryTop].text, mfs->text);
	chatHistoryTop = (chatHistoryTop + 1) % CHAT_HISTORY_LENGTH;

	// reflect the private chat back to the sender
	
	recipients[n++] = client->connection;

	TRACE(_ROUTINE_ "Sending to %s (playerID = %d): %s.\n", client->info.login, client->info.playerID, GetMessageDescription((char *)mfs));
	
	if (n > 2)
		mfs->privateIDTo = -1; // private to group
	
	err = connections->BroadcastMessage(recipients, n, (char *)mfs, sizeof(MFS_ROOM_CHAT_MESSAGE) + textLength);
	if (err)
		TRACE(_WARN_ "RoomServer::HandleMTS_ROOM_CHAT(): connections->BroadcastMessage() returned error %d", err);
	
	return err;
}

int RoomServer::HandleMTS_ROOM_CHANGE_SHADOW(ROOMSERVERPLAYER *client, char *message, int length)
{
	int err;
	MTS_ROOM_CHANGE_SHADOW_MESSAGE *mts = (MTS_ROOM_CHANGE_SHADOW_MESSAGE *)message;
	MFS_ROOM_CHANGE_SHADOW_MESSAGE mfs;
	
	if (mts->playerID != client->info.playerID)
		{ ASSERT(_WARN_, FALSE); return -1; }
	
	client->info.shadow = mts->shadow;
	client->info.faceInfo = mts->faceInfo;
	MakeDataDirty(client, DFI_SHADOW);
	
	// tell everyone in the room about the shadow change
	
	mfs.messageType = MFS_ROOM_CHANGE_SHADOW;
	mfs.playerID = mts->playerID;
	mfs.shadow = client->info.shadow;
	mfs.faceInfo = client->info.faceInfo;
	
	err = BroadcastRoomMessage((char *)&mfs, sizeof(MFS_ROOM_CHANGE_SHADOW_MESSAGE));
	
	return err;
}

int RoomServer::HandleMTS_ROOM_CHANGE_PROFILE(ROOMSERVERPLAYER *client, char *message, int length)
{
	char buffer[MAX_MSG];
	int err;
	MTS_ROOM_CHANGE_PROFILE_MESSAGE *mts = (MTS_ROOM_CHANGE_PROFILE_MESSAGE *)message;
	MFS_ROOM_CHANGE_PROFILE_MESSAGE *mfs = (MFS_ROOM_CHANGE_PROFILE_MESSAGE *)buffer;
	
	if (mts->playerID != client->info.playerID)
		{ ASSERT(_WARN_, FALSE); return -1; }
	
	strnzcpy(client->profile, mts->profile, MAX_PLAYER_PROFILE - 1);
	MakeDataDirty(client, DFI_PROFILE);
	
	// tell everyone in the room about the profile change
	
	mfs->messageType = MFS_ROOM_CHANGE_PROFILE;
	mfs->playerID = client->info.playerID;
	mfs->profileLength = strlen(client->profile) + 1;
	strcpy(mfs->profile, client->profile);
	
	err = BroadcastRoomMessage((char *)mfs, sizeof(MFS_ROOM_CHANGE_PROFILE_MESSAGE) + mfs->profileLength);
	
	return err;
}

int RoomServer::HandleMTS_ROOM_SIT(ROOMSERVERPLAYER *client, char *message, int length)
{
	Boolean firstToSit;
	int i, n, err;
	char buffer[MAX_MSG];
	i18nStr getS, getS2;
	MTS_ROOM_SIT_MESSAGE	*mts		= (MTS_ROOM_SIT_MESSAGE *)	message;
	MTS_ROOM_STAND_MESSAGE	*mtsRSTANDM	= (MTS_ROOM_STAND_MESSAGE *)buffer;
	MFS_ROOM_SIT_MESSAGE	*mfs		= (MFS_ROOM_SIT_MESSAGE *)	buffer;
	MFS_GAME_STATE_MESSAGE	*mfsGSM		= (MFS_GAME_STATE_MESSAGE *)buffer;
	
	if (mts->playerID != client->info.playerID)
		{ ASSERT(_WARN_, FALSE); return -1; }
	
	if (mts->table < 0 || mts->table >= NUM_ROOM_TABLES)
		{ ASSERT(_WARN_, FALSE); return -1; }
	
	if (mts->seat < 0 || mts->seat > MAX_GAME_PLAYERS) 
	{ 
		// mts->seat == MAX_GAME_PLAYERS means sitting to watch
		TRACE(_WARN_ "Client %s tried to sit in invalid seat %d of table %d.\n", client->info.login, mts->seat, mts->table);
		return -1;
	}
	
	// make sure the player can sit tournament-wise
	if (IsTournamentRoom() && mts->seat < MAX_GAME_PLAYERS && !client->IsInTournament())
		return SendDisplayStringMessage(client, GetString(40, getS), GetString(41, getS2)); // "Cannot Sit", "You must be in the tournament to play in this room."
	
	if (IsTournamentRoom() && !tournamentInformation.header.sittingEnabled)
		return SendDisplayStringMessage(client, GetString(40, getS), GetString(42, getS2)); // "Cannot Sit", "Sitting has been disabled by the tournament director."
	
	// make sure the player is not on probation for this table
	
	if (client->probationTable == mts->table)
		return SendDisplayStringMessage(client, GetString(43, getS), GetString(44, getS2)); // "On Probation", "You cannot sit or watch at this table right now since you were recently booted from it."
	
	if (mts->seat == MAX_GAME_PLAYERS) { // sit to watch: find an empty watcher seat
		if (tables[mts->table].status != TS_PLAYING)
			return SendDisplayStringMessage(client, GetString(45, getS), GetString(46, getS2)); // "Cannot Watch", "There is no game in progress at this table."
		
		mts->seat = tables[mts->table].GetEmptyWatcherSeat();
		if (mts->seat == -1)
			return SendDisplayStringMessage(client, GetString(45, getS), GetString(47, getS2)); // "Cannot Watch", "There is no more room for watchers at this table."
	}
	
	// make sure the seat is empty
	
	for (i = 0, n = 0 ; i < MAX_ROOM_PLAYERS ; i++)
		if (clients[i].status == CS_ACTIVE)
			if (clients[i].info.table == mts->table) {
				if (clients[i].info.seat == mts->seat) {

					TRACE(_ROUTINE_ "Client %d tried to sit in filled seat %d of table %d.\n",
									client->info.playerID, mts->seat, mts->table);

					if (client->info.table == clients[i].info.table &&
						client->info.seat == clients[i].info.seat)
						return 0; // allow for the case the client double-clicks a seat
					
					return SendDisplayStringMessage(client, GetString(40, getS), GetString(48, getS2)); // "Cannot Sit", "There is already someone in that seat."
				}
				if (clients[i].info.seat < MAX_GAME_PLAYERS)
					n++;
			}
	
	ASSERT(_WARN_, n <= MAX_GAME_PLAYERS);

	// Make sure that the player has the skill level required to play.
	if(tables[mts->table].ratingsFlags & RF_TABLE_HAS_SKILL)
	{
		if(mts->seat < MAX_GAME_PLAYERS && !tables[mts->table].PlayersRatingMatchesTable(client->info.rating))
			return SendDisplayStringMessage(client, GetString(40, getS), GetString(140, getS2)); // "Cannot Sit" "Your rating does not match the table's required rating."
	}

	// Make sure the game allows a player to join in progress...
	
	if (n < MAX_GAME_PLAYERS && NoJoinInProgress())
		if (tables[mts->table].status == TS_PLAYING)
			return SendDisplayStringMessage(client,
											(mts->seat == MAX_GAME_PLAYERS) ? GetString(45, getS) : GetString(40, getS), // "Cannot Watch" : "Cannot Sit"
											GetString(49, getS2)); // "The game at that table has ended."
	
	// make sure the seat has not been set to void
	
	if (mts->seat < MAX_GAME_PLAYERS)
		if (tables[mts->table].gameState.players[mts->seat].playerID == PLAYER_ID_VOID)
			return SendDisplayStringMessage(client, GetString(40, getS), GetString(50, getS2)); // "Cannot Sit", "That seat is no longer available."
	
	if(tables[mts->table].status == TS_PLAYING && mts->seat < MAX_GAME_PLAYERS && !IsTournamentRoom() && 
		!(tables[mts->table].CanPlayerAffordThisTable(client)))
	{
		SendDisplayStringMessage(client, GetString(136, getS), GetString(137, getS2));
		return 0;
	}
	
	// Now, check to see if the player is already sitting somewhere else.  If so, issue a stand first.
	
	if(client->info.table != -1)
	{
		memset(mtsRSTANDM, 0, sizeof(MTS_ROOM_STAND_MESSAGE));
		mtsRSTANDM->messageType = MTS_ROOM_STAND;
		mtsRSTANDM->playerID = mts->playerID;
		mtsRSTANDM->table = client->info.table;
		mtsRSTANDM->seat = client->info.seat;

		//Set the switchingSeats parameter to true only if the player is switching seats at the same table, and not
		// if they are going to a different table.
		HandleMTS_ROOM_STAND(client, (char *)mtsRSTANDM, sizeof(MTS_ROOM_STAND_MESSAGE), client->info.table == mts->table);
	}
	
	CancelRequestOrInvitation(client);
	
	client->info.table = mts->table;
	client->info.seat = mts->seat;
	client->info.flags &= (0xFF ^ UF_READY_TO_PLAY); // no longer ready to play
	
	ClearBootRecord(client, FALSE);
	
	// add the player to the internal game state
	
	firstToSit = tables[mts->table].status == TS_EMPTY;
	
	if (tables[mts->table].status == TS_EMPTY)
		tables[mts->table].status = TS_PENDING;
	
	memset(mfs, 0, sizeof(MTS_ROOM_STAND_MESSAGE));
	tables[mts->table].PlayerSits(client, &mfs->playerState);
	if (mts->seat < MAX_GAME_PLAYERS) {
		tables[mts->table].playerIndicies[mts->seat] = client->index;
		tables[mts->table].gameState.players[mts->seat].bootCount = -1;
	}
	else
		tables[mts->table].watcherIndicies[mts->seat - MAX_GAME_PLAYERS] = client->index;
	
	// let everyone know this player has sat
	
	mfs->messageType = MFS_ROOM_SIT;
	mfs->playerID = mts->playerID;
	mfs->table = mts->table;
	mfs->seat = mts->seat;
	
	err = BroadcastRoomMessage((char *)mfs, sizeof(MFS_ROOM_SIT_MESSAGE));
	
	if (err)
		return err;
	
	// if a game is already in progress at this table...
	
	if (tables[mts->table].status == TS_PLAYING) {
		// send MFS_GAME_STATE to this player (the joiner)
		
		memset(mfsGSM, 0, sizeof(MTS_ROOM_STAND_MESSAGE));
		mfsGSM->messageType = MFS_GAME_STATE;
		mfsGSM->table = mts->table;
		mfsGSM->status = tables[mts->table].status;
		mfsGSM->gameState = tables[mts->table].gameState;
		

		TRACE(_ROUTINE_ "Sending to %s (playerID = %d): %s.\n", client->info.login, client->info.playerID, GetMessageDescription((char *)mfsGSM));
		
		err = connections->PostMessage(client->connection, (char *)mfsGSM, sizeof(MFS_GAME_STATE_MESSAGE));
		if (err) {
			TRACE(_WARN_ "RoomServer::HandleMTS_ROOM_SIT(): connections->PostMessage() returned error %d\n", err);
			
			return err;
		}
		
		SendDisplayStringMessage(client, "<SETTICKER>", tickerText, TRUE);

		if (mts->seat < MAX_GAME_PLAYERS)
			ClearRestartVotes(mts->table);
	}
	
	return 0;
}

int RoomServer::HandleMTS_ROOM_CHANGE_TABLE_SETTINGS(ROOMSERVERPLAYER *client, char *message, int length)
{
	char buffer[MAX_MSG];
	int err;
	MTS_ROOM_CHANGE_TABLE_SETTINGS_MESSAGE *mts = (MTS_ROOM_CHANGE_TABLE_SETTINGS_MESSAGE *)message;
	MFS_ROOM_CHANGE_TABLE_SETTINGS_MESSAGE *mfs = (MFS_ROOM_CHANGE_TABLE_SETTINGS_MESSAGE *)buffer;
	
	if (mts->table < 0 || mts->table >= NUM_ROOM_TABLES)
		{ ASSERT(_WARN_, FALSE); return -1; }
	
	// sanity check: make sure we see the player at this table
	
	if (client->info.table != mts->table)
		{ ASSERT(_WARN_, FALSE); return -1; }
	
	if (mts->joinOption >= TJO_OPEN && mts->joinOption <= TJO_BY_INVITATION)
		tables[mts->table].joinOption = mts->joinOption;
	
	if (mts->watchersOption >= TWO_OPEN && mts->watchersOption <= TWO_BY_INVITATION)
		tables[mts->table].watchersOption = mts->watchersOption;
	
	tables[mts->table].muteWatchers = mts->muteWatchers ? TRUE : FALSE;
	
	tables[mts->table].ratingsFlags = mts->ratingsFlags;
	tables[mts->table].ratingsSkills = mts->ratingsSkills;

	tables[mts->table].SetTableGameValues(mts->gameValues);
	
	strnzcpy(tables[mts->table].description, mts->description, MAX_TABLE_DESCRIPTION - 1);
	
	// let everyone know the table settings have changed
	
	mfs->messageType		= MFS_ROOM_CHANGE_TABLE_SETTINGS;
	mfs->table				= mts->table;
	mfs->joinOption			= tables[mts->table].joinOption;
	mfs->watchersOption		= tables[mts->table].watchersOption;
	mfs->muteWatchers		= tables[mts->table].muteWatchers;
	mfs->ratingsFlags		= tables[mts->table].ratingsFlags;
	mfs->ratingsSkills		= tables[mts->table].ratingsSkills;
	tables[mts->table].GetTableGameValues(mfs->gameValues);
	strcpy(mfs->description, tables[mts->table].description);
	mfs->descriptionLength	= strlen(mfs->description) + 1;
	
	err = BroadcastRoomMessage((char *)mfs, sizeof(MFS_ROOM_CHANGE_TABLE_SETTINGS_MESSAGE) + mfs->descriptionLength);
	
	return err;
}

int RoomServer::HandleMTS_ROOM_START_GAME(ROOMSERVERPLAYER *client, char *message, int length)
{
	MTS_ROOM_START_GAME_MESSAGE *mts = (MTS_ROOM_START_GAME_MESSAGE *)message;
	
	if (mts->playerID != client->info.playerID)
		{ ASSERT(_WARN_, FALSE); return -1; }
	
	if (mts->table < 0 || mts->table >= NUM_ROOM_TABLES)
		{ ASSERT(_WARN_, FALSE); return -1; }
	
	if (mts->seat < 0 || mts->seat >= MAX_GAME_PLAYERS)
		{ ASSERT(_WARN_, FALSE); return -1; }
	
	// sanity check: make sure we see the player in this seat
	
	if (client->info.table != mts->table || client->info.seat != mts->seat)
	{
		TRACE(_WARN_ "client->info.table = %d, mts->table = %d, client->info.seat= %d, mts->seat = %d\n", 
			client->info.table,
			mts->table,
			client->info.seat,
			mts->seat
			);
		ASSERT(_WARN_, FALSE); return -1;
	}
	
	// the game might have already been started by someone else
	
	if (tables[mts->table].status != TS_PENDING)
		return 0;
	
	client->info.flags |= UF_READY_TO_PLAY; // ready to play
	
	return TablePlayerChanged(client, TPC_READY);
}

int RoomServer::HandleMTS_ROOM_STAND(ROOMSERVERPLAYER *client, char *message, int length, Boolean switchingSeats/* = FALSE*/)
{
	MTS_ROOM_STAND_MESSAGE *mts = (MTS_ROOM_STAND_MESSAGE *)message;
	
	if (mts->playerID != client->info.playerID)
		{ ASSERT(_WARN_, FALSE); return -1; }
	
	if (mts->table < 0 || mts->table >= NUM_ROOM_TABLES)
		{ ASSERT(_WARN_, FALSE); return -1; }
	
	if (mts->seat < 0 || mts->seat >= (MAX_GAME_PLAYERS + MAX_GAME_WATCHERS))
		{ ASSERT(_WARN_, FALSE); return -1; }
	
	// sanity check: make sure we see the player in this seat
	
	if (client->info.table != mts->table || client->info.seat != mts->seat)
		{ ASSERT(_WARN_, FALSE); return -1; }
	
	return TablePlayerChanged(client, switchingSeats ? TPC_SWITCHINGSEATS : TPC_LEFT);
}

int RoomServer::HandleMTS_ROOM_REQUEST(ROOMSERVERPLAYER *client, char *message, int length)
{
	Boolean correctOption;
	i18nStr getS, getS2;
	int i, err, approverSeat, approverIndex;
	MTS_ROOM_REQUEST_MESSAGE *mts = (MTS_ROOM_REQUEST_MESSAGE *)message;
	MFS_ROOM_REQUEST_MESSAGE mfs;
	ROOMSERVERPLAYER *approver, *oldApprover;
	HCONNECTION recipients[2];
	
	if (mts->playerID != client->info.playerID)
		{ ASSERT(_WARN_, FALSE); return -1; }
	
	if (mts->table < 0 || mts->table >= NUM_ROOM_TABLES)
		{ ASSERT(_WARN_, FALSE); return -1; }
	
	if (mts->seat < 0 || mts->seat > MAX_GAME_PLAYERS)
		{ ASSERT(_WARN_, FALSE); return -1; }
	
	// make sure the table has BY_APPROVAL as its join or watch option, as appropriate
	
	if (mts->seat == MAX_GAME_PLAYERS) // ask to watch
		correctOption = tables[mts->table].watchersOption == TWO_BY_APPROVAL;
	else // ask to join
		correctOption = tables[mts->table].joinOption == TJO_BY_APPROVAL;
	
	if (!correctOption)
		return 0; // allow this case for when the option changes after the MTS_ROOM_REQUEST has been sent
	

	// make sure the player is not already seated
	
	if (!(client->info.table == mts->table) && (client->info.seat < MAX_GAME_PLAYERS) && 
		(client->info.table != -1 || client->info.seat != -1)) {

		// allow for the case the player clicks a seat and then makes a request before the
		// client receives the MFS_SIT_MESSAGE
		TRACE(_ROUTINE_ "Client %d requested to sit in seat %d of table %d but was "
					"already in seat %d of table %d.\n",
					client->info.playerID, mts->seat, mts->table,
					client->info.seat, client->info.table);
		
		return -1;
	}

	// Make sure the player wasn't recently booted from the table...

	if (client->probationTable == mts->table)
		return SendDisplayStringMessage(client, GetString(43, getS), GetString(44, getS2)); // "On Probation", "You cannot sit or watch at this table right now since you were recently booted from it."

	// if requesting to watch, make sure the table is playing and find the first empty watcher seat

	if (mts->seat == MAX_GAME_PLAYERS) {
		if (tables[mts->table].status != TS_PLAYING)
			return SendDisplayStringMessage(client, GetString(45, getS), GetString(46, getS2)); // "Cannot Watch", "There is no game in progress at this table."
		
		mts->seat = tables[mts->table].GetEmptyWatcherSeat();
		if (mts->seat == -1)
			return SendDisplayStringMessage(client, GetString(45, getS), GetString(47, getS)); // "Cannot Watch", "There is no more room for watchers at this table."
	}
	
	// make sure the seat is empty
	
	for (i = 0 ; i < MAX_ROOM_PLAYERS ; i++)
		if (clients[i].status == CS_ACTIVE)
			if (clients[i].info.table == mts->table && clients[i].info.seat == mts->seat) {

				TRACE(_WARN_ "Client %d requested to sit in filled seat %d of table %d.\n",
							client->info.playerID, mts->table, mts->seat);
				
				return SendDisplayStringMessage(client, GetString(51, getS), GetString(52, getS2)); // "Invalid Request", "There is already someone in that seat."
			}
	
	// if the requester has a different pending request, cancel it
	
	if (client->requestingTable != -1) {
		// but if it's for the same table (impatient requester), ignore it
		if (client->requestingTable == mts->table)
			return 0;
		
		oldApprover = GetClientFromID(client->approvingPlayerID);
		ASSERT(_WARN_, oldApprover && oldApprover->requesterPlayerID == client->info.playerID);
		if (oldApprover && oldApprover->requesterPlayerID == client->info.playerID) {
			mfs.messageType = MFS_ROOM_REQUEST;
			mfs.playerID = client->info.playerID;
			mfs.table = client->requestingTable;
			mfs.seat = client->requestingSeat;
			mfs.cancel = TRUE;
			
			TRACE(_ROUTINE_ "Sending to %s (playerID = %d): %s.\n", oldApprover->info.login, oldApprover->info.playerID, GetMessageDescription((char *)&mfs));
			
			err = connections->PostMessage(oldApprover->connection, (char *)&mfs, sizeof(MFS_ROOM_REQUEST_MESSAGE));
			if (err)
				TRACE(_WARN_ "RoomServer::HandleMTS_ROOM_REQUEST(): connections->PostMessage() returned error %d\n", err);
			
			oldApprover->requesterPlayerID = -1;
		}
	}
	
	// determine which player at the table should be the approver
	
	approverSeat = GetPreferredApproverSeat(mts->seat);
	approverIndex = tables[mts->table].playerIndicies[approverSeat];
	if (approverIndex == -1 || clients[approverIndex].requesterPlayerID != -1)
		for (approverSeat = 0 ; approverSeat < MAX_GAME_PLAYERS ; approverSeat++) {
			approverIndex = tables[mts->table].playerIndicies[approverSeat];
			if (approverIndex != -1 && clients[approverIndex].requesterPlayerID == -1)
				break;
		}
	
	if (approverSeat == MAX_GAME_PLAYERS)
		return SendDisplayStringMessage(client, GetString(53, getS), GetString(54, getS2)); // "Cannot Approve Request", "Nobody is available to approve your request."
	
	// make sure the approver is valid
	
	approver = &clients[approverIndex];
	if (approver->status != CS_ACTIVE || approver->info.playerID != tables[mts->table].gameState.players[approverSeat].playerID)
		{ ASSERT(_WARN_, FALSE); return -1; }
	
	mfs.messageType = MFS_ROOM_REQUEST;
	mfs.playerID = mts->playerID;
	mfs.table = mts->table;
	mfs.seat = mts->seat;
	mfs.cancel = FALSE;
	
	recipients[0] = client->connection;
	recipients[1] = approver->connection;
	
	err = connections->BroadcastMessage(recipients, 2, (char *)&mfs, sizeof(MFS_ROOM_REQUEST_MESSAGE));
	if (err)
		TRACE(_WARN_ "RoomServer::HandleMTS_ROOM_REQUEST(): connections->BroadcastMessage() returned error %d", err);
	else {
		// set the appropriate fields in the requester
		
		client->requestingTable = mts->table;
		client->requestingSeat = mts->seat;
		client->approvingPlayerID = approver->info.playerID;
		
		// set the appropriate fields in the approver
		
		approver->requesterPlayerID = client->info.playerID;
		approver->requestedSeat = mts->seat;
	}
	
	return err;
}

int RoomServer::HandleMTS_ROOM_ANSWER_REQUEST(ROOMSERVERPLAYER *client, char *message, int length)
{
	int seat, emptySeat, start, end, err,
		requestingTable, requestingSeat, approvingPlayerID, requesterPlayerID, requestedSeat;
	MTS_ROOM_ANSWER_REQUEST_MESSAGE *mts = (MTS_ROOM_ANSWER_REQUEST_MESSAGE *)message;
	MFS_ROOM_DISAPPROVE_REQUEST_MESSAGE mfs;
	MTS_ROOM_SIT_MESSAGE mtsSIT;
	ROOMSERVERPLAYER *requester;
	
	// make sure the approver points to a valid requester
	
	requester = GetClientFromID(client->requesterPlayerID);
	if (!requester)
		{ ASSERT(_WARN_, FALSE); client->requesterPlayerID = -1; return -1; }
	
	// make sure the requester points back to this approver
	
	if (requester->approvingPlayerID != client->info.playerID)
		{ ASSERT(_WARN_, FALSE); client->requesterPlayerID = -1; return -1; }
	
	requestingTable = requester->requestingTable;
	requestingSeat = requester->requestingSeat;
	approvingPlayerID  = requester->approvingPlayerID;
	requesterPlayerID = client->requesterPlayerID;
	requestedSeat = client->requestedSeat;
	
	requester->requestingTable = -1;
	requester->requestingSeat = -1;
	requester->approvingPlayerID = -1;
	client->requesterPlayerID = -1;
	client->requestedSeat = -1;
	
	// make sure the approver is at the right table
	
	if (client->info.table == -1 || client->info.table != requestingTable)
		{ ASSERT(_WARN_, FALSE); return -1; }
	
	// if the request was denied, we can handle that right away
	
	if (!mts->approved) {
		mfs.messageType = MFS_ROOM_DISAPPROVE_REQUEST;
		mfs.playerID = requester->info.playerID;
		mfs.table = requestingTable;
		mfs.seat = requestingSeat;
		mfs.notice = 1; // "You're request has been denied."
		
		TRACE(_ROUTINE_ "Sending to %s (playerID = %d): %s.\n", requester->info.login, requester->info.playerID, GetMessageDescription((char *)&mfs));
		
		err = connections->PostMessage(requester->connection, (char *)&mfs, sizeof(MFS_ROOM_DISAPPROVE_REQUEST_MESSAGE));
		if (err)
			TRACE(_WARN_ "RoomServer::HandleMTS_ROOM_ANSWER_REQUEST(): connections->PostMessage() returned error %d\n", err);
		
		return err;
	}
	
	// make sure the two agree on the seat
	
	if (requestedSeat != requestingSeat)
		ASSERT(_WARN_, FALSE);
	
	// if the seat that was requested has already been filled (by another accepted request)
	// we should put the player in any seat of the same type (if there is one free)
	
	seat = requestedSeat;
	if (seat < MAX_GAME_PLAYERS) {
		start = 0;
		end = MAX_GAME_PLAYERS;
	}
	else {
		start = MAX_GAME_PLAYERS;
		end = MAX_GAME_PLAYERS + MAX_GAME_WATCHERS;
	}
	if (tables[requestingTable].playerIndicies[seat] != -1) { // the seat is already filled
		// find first seat without a real player in it
		
		for (seat = start ; seat < end ; seat++)
			if (tables[requester->requestingTable].gameState.players[seat].playerID != PLAYER_ID_VOID) //Void seats are FORBIDDEN!!!!
				if (tables[requester->requestingTable].playerIndicies[seat] == -1)
					break;
		
		// if we chose an AI seat, choose an empty seat instead if one is available
		
		if (start == 0 && seat != end)
			if (tables[requester->requestingTable].gameState.players[seat].playerID == PLAYER_ID_AI)
				for (emptySeat = 0 ; emptySeat < MAX_GAME_PLAYERS ; emptySeat++)
					if (tables[requester->requestingTable].gameState.players[emptySeat].playerID == PLAYER_ID_EMPTY) {
						seat = emptySeat;
						break;
					}
	}
	
	if (seat == end) { // couldn't find a free seat
		mfs.messageType = MFS_ROOM_DISAPPROVE_REQUEST;
		mfs.playerID = requester->info.playerID;
		mfs.table = requestingTable;
		mfs.seat = requestingSeat;
		mfs.notice = 2; // "Nobody is available to approve your request."
		
		TRACE(_ROUTINE_ "Sending to %s (playerID = %d): %s.\n", requester->info.login, requester->info.playerID, GetMessageDescription((char *)&mfs));
		
		err = connections->PostMessage(requester->connection, (char *)&mfs, sizeof(MFS_ROOM_DISAPPROVE_REQUEST_MESSAGE));
		if (err)
			TRACE(_WARN_ "RoomServer::HandleMTS_ROOM_ANSWER_REQUEST(): connections->PostMessage() returned error %d\n", err);
		
		return err;
	}
	
	// attempt the actual sit
	
	mtsSIT.messageType = MTS_ROOM_SIT;
	mtsSIT.playerID = requester->info.playerID;
	mtsSIT.table = requestingTable;
	mtsSIT.seat = seat;
	
	return HandleMTS_ROOM_SIT(requester, (char *)&mtsSIT, sizeof(MTS_ROOM_SIT_MESSAGE));
}

int RoomServer::HandleMTS_ROOM_INVITE(ROOMSERVERPLAYER *client, char *message, int length)
{
	i18nStr getS, getS2;
	int i, err;
	MTS_ROOM_INVITE_MESSAGE *mts = (MTS_ROOM_INVITE_MESSAGE *)message;
	MFS_ROOM_INVITE_MESSAGE mfs;
	ROOMSERVERPLAYER *invitee;
	
	if (mts->table < 0 || mts->table >= NUM_ROOM_TABLES)
		{ ASSERT(_WARN_, FALSE); return -1; }
	
	// make sure the invitee is present and not already seated
	
	invitee = GetClientFromID(mts->inviteePlayerID);
	if (invitee == NULL)
		return SendDisplayStringMessage(client, GetString(55, getS), GetString(56, getS2)); // "RSVP", "The player you invited is no longer available."
	
	if (invitee->info.table == mts->table)
		return 0;
	
	// An invitee can be invited somewhere if they are only watching a game.

	if (invitee->info.table != -1 && invitee->info.seat < MAX_GAME_PLAYERS)
		return SendDisplayStringMessage(client, GetString(55, getS), GetString(57, getS2)); // "RSVP", "The player you invited is already seated at another table."
	
	// make sure the invitee is not already being invited
	
	if (invitee->inviterTable != -1)
		return SendDisplayStringMessage(client, GetString(55, getS), GetString(58, getS2)); // "RSVP", "The player you invited is currently responding to another invitation."
	
	// make sure there is an empty seat of the given type
	
	if (mts->toWatch) {
		for (i = 0 ; i < MAX_GAME_WATCHERS ; i++)
			if (tables[mts->table].gameState.watcherIDs[i] == PLAYER_ID_EMPTY)
				break;
		if (i == MAX_GAME_WATCHERS)
			return SendDisplayStringMessage(client, GetString(55, getS), GetString(59, getS2)); // "RSVP", "Your invitation was canceled because there are no free watcher seats at your table."
	}
	else {
		for (i = 0 ; i < MAX_GAME_PLAYERS ; i++)
			if (tables[mts->table].gameState.players[i].playerID == PLAYER_ID_EMPTY || tables[mts->table].gameState.players[i].playerID == PLAYER_ID_AI)
				break;
		if (i == MAX_GAME_PLAYERS)
			return SendDisplayStringMessage(client, GetString(55, getS), GetString(60, getS2)); // "RSVP", "Your invitation was canceled because there are no free seats at your table."
	}
	
	mfs.messageType = MFS_ROOM_INVITE;
	mfs.table = mts->table;
	mfs.inviterPlayerID = client->info.playerID;
	mfs.toWatch = mts->toWatch;
	
	TRACE(_ROUTINE_ "Sending to %s (playerID = %d): %s.\n", invitee->info.login, invitee->info.playerID, GetMessageDescription((char *)&mfs));
	
	err = connections->PostMessage(invitee->connection, (char *)&mfs, sizeof(MFS_ROOM_INVITE_MESSAGE));
	if (err)
		TRACE(_WARN_ "RoomServer::HandleMTS_ROOM_INVITE(): connections->PostMessage() returned error %d\n", err);
	else {
		invitee->inviterTable = mts->table;
		invitee->inviterPlayerID = client->info.playerID;
		invitee->invitedToWatch = mts->toWatch;
	}
	
	return err;
}

int RoomServer::HandleMTS_ROOM_RSVP(ROOMSERVERPLAYER *client, char *message, int length)
{
	Boolean invitedToWatch;
	i18nStr getS, getS2;
	int i, inviterTable, inviterPlayerID, seat, emptySeat;
	MTS_ROOM_RSVP_MESSAGE *mts = (MTS_ROOM_RSVP_MESSAGE *)message;
	MTS_ROOM_SIT_MESSAGE mtsSIT;
	ROOMSERVERPLAYER *inviter;
	
	inviterTable = client->inviterTable;
	inviterPlayerID = client->inviterPlayerID;
	invitedToWatch = client->invitedToWatch;
	
	client->inviterTable = -1;
	client->inviterPlayerID = -1;
	client->invitedToWatch = FALSE;
	
	// make sure we think the client has been invited
	
	if (inviterTable == -1) {
		TRACE(_WARN_ "Client %d accepted an invitation at table %d but was not invited.\n",
					client->info.playerID, inviterTable);
		
		return -1;
	}
	
	// make sure the client is not already playing somewhere
	
	if (client->info.table != -1 && client->info.seat < MAX_GAME_PLAYERS) {
		// allow for the case the player clicks a seat and then accepts an invitation before the
		// client receives the MFS_SIT_MESSAGE
		TRACE(_ROUTINE_ "Client %d accepted an invitation at table %d but was already seated at table %d.\n",
					client->info.playerID, invitedToWatch, client->info.table);
		
		return -1;
	}
	
	if (inviterPlayerID == -1) {
		ASSERT(_WARN_, FALSE);
		inviter = NULL;
	}
	else
		inviter = GetClientFromID(inviterPlayerID);
	
	// if the invitation was declined, we can handle that right away
	
	if (!mts->accepted) {
		if (inviter == NULL)
			return 0; // the inviter left the room before the RSVP messsage was received
		
		if (inviter->info.table == inviterTable)
			return SendDisplayStringMessage(inviter, GetString(55, getS), GetString(61, getS2)); // "RSVP", "Your invitation was declined."
		
		return 0; // the inviter left the table before the RSVP messsage was received
	}
	
	// If the player is watching somewhere, they first need to stand.
	if (client->info.table != -1 && client->info.seat >= MAX_GAME_PLAYERS) {
		TablePlayerChanged(client, TPC_LEFT);
	}

	// find the best seat to sit in
	
	if (invitedToWatch) {
		for (i = 0 ; i < MAX_GAME_WATCHERS ; i++)
			if (tables[inviterTable].gameState.watcherIDs[i] == PLAYER_ID_EMPTY)
				break;
		if (i == MAX_GAME_WATCHERS)
			return SendDisplayStringMessage(client, GetString(55, getS), GetString(62, getS2)); // "RSVP", "The invitation was canceled because there are no free watcher seats at the table."
		
		seat = MAX_GAME_PLAYERS;
	}
	else {
		seat = -1;
		
		if (inviter && inviter->info.table == inviterTable)
			seat = GetPreferredInvitationSeat(inviter->info.seat);
		
		if (tables[inviterTable].gameState.players[seat].playerID == PLAYER_ID_VOID)
			seat = -1;

		// an empty seat is always better than an AI seat...
		
		if (seat != -1 && tables[inviterTable].playerIndicies[seat] == -1 && tables[inviterTable].gameState.players[seat].playerID == PLAYER_ID_AI)
			for (emptySeat = 0 ; emptySeat < MAX_GAME_PLAYERS ; emptySeat++)
				if (tables[inviterTable].gameState.players[emptySeat].playerID == PLAYER_ID_EMPTY) {
					seat = emptySeat;
					break;
				}
		
		if (seat == -1 || tables[inviterTable].playerIndicies[seat] != -1) {
			// find first empty seat
			
			for (seat = 0 ; seat < MAX_GAME_PLAYERS ; seat++)
				if (tables[inviterTable].gameState.players[seat].playerID == PLAYER_ID_EMPTY)
					break;
			
			// else first AI seat
			
			if (seat == MAX_GAME_PLAYERS)
				for (seat = 0 ; seat < MAX_GAME_PLAYERS ; seat++)
					if (tables[inviterTable].gameState.players[seat].playerID == PLAYER_ID_AI)
						break;
		}
		
		if (seat == MAX_GAME_PLAYERS)
			return SendDisplayStringMessage(client, GetString(55, getS), GetString(63, getS2)); // "RSVP", "The invitation was canceled because there are no free seats at the table."
	}
	
	// attempt the actual sit
	
	mtsSIT.messageType = MTS_ROOM_SIT;
	mtsSIT.playerID = client->info.playerID;
	mtsSIT.table = inviterTable;
	mtsSIT.seat = seat;
	
	return HandleMTS_ROOM_SIT(client, (char *)&mtsSIT, sizeof(MTS_ROOM_SIT_MESSAGE));
}

int RoomServer::HandleMTS_ROOM_MODERATE(ROOMSERVERPLAYER *client, char *message, int length)
{
	char s[MAX_MSG];
	char subject[256];
	i18nStr getS, getS2, getS3;
	char *peer1 = NULL;
	int err;
	int iBufSize = 16000;
	char *buf;
	ROOMSERVERPLAYER *p;
	MTS_ROOM_MODERATE_MESSAGE *mts = (MTS_ROOM_MODERATE_MESSAGE *)message;
	MTDB_RS_SET_BANNED_MESSAGE mtdbBAN;
	MTDB_RS_SET_UNBANNED_MESSAGE mtdbUNBAN;
	
	if (!client->IsMonitorOrSuperUser()) 
	{
		TRACE(_WARN_ "Got monitor message from a non-monitor: %s.\n", client->info.login);
		return -1;
	}
	
	p = GetClientFromName(mts->login);
	if (p == NULL && mts->function != MODERATE_BAN_OFFLINE && mts->function != MODERATE_UNBAN)
		return SendDisplayStringMessage(client, GetString(64, getS), GetString(65, getS2)); // "Moderate Error", "The name you entered is not the name of a present player.  Please check spelling and capitalization."
	
	switch (mts->function) 
	{
	case MODERATE_GET_IP:
		peer1 = p->connection->GetPeerName();
		sprintf(s, GetString(66, getS), mts->login, peer1 ? peer1 : "???"); // "Player: %s\nIP Adress: %s"
		
		return SendDisplayStringMessage(client, GetString(67, getS), s); // "IP Address"
	
	case MODERATE_GLOBAL_MUTE:
		p->globallyMuted = TRUE;
		
		return SendDisplayStringMessage(p, GetString(68, getS), GetString(69, getS2)); // "Global Mute", "You have been globally muted by a WON host.  Please be more respectful."
	
	case MODERATE_GLOBAL_UNMUTE:
		p->globallyMuted = FALSE;
		
		return SendDisplayStringMessage(p, GetString(68, getS), GetString(70, getS2)); // "Global Mute", "You have been globally un-muted by a WON host.  Please be respectful."
	
	case MODERATE_BOOT:
		buf = new char[iBufSize + 10];
		if (buf == NULL)
			{ ASSERT(_WARN_, FALSE); buf = s; iBufSize = MAX_MSG;}

		peer1 = p->connection->GetPeerName();
		sprintf(buf, GetString(71, getS), // "%s [e-mail: %s] was booted from the room by %s [e-mail: %s]\n"
				   p->info.login, peer1 ? peer1 : "???", p->email[0] ? p->email : "???",
				   client->info.login, client->email[0] ? client->email : "???");

		AddChatToBuffer(buf, iBufSize - strlen(buf));

		SendMail(GetString(72, getS), GetString(74, getS2), GetString(73, getS3), buf); // "roomserver@won.net", "sysop@won.net", "Booted"

		if(buf != s) //this means we had enough memory to allocate 16000 bytes for buf.
			delete [] buf;
		
		err = SendDisplayStringMessage(p, GetString(73, getS), GetString(76, getS)); // "Booted", "You have been removed from the room by a WON host.  Please be more respectful."

		if(p->info.table != -1 && p->info.seat < MAX_GAME_PLAYERS)
		{
			tables[p->info.table].UpdateRatings(p->info.seat, tables[p->info.table].GetNumPlayersAtTable(), RRG_LOSS, RRU_MODERATORBOOT, IsPartnershipGame());
		}
		connections->MyFlushAndClose(p->connection);
		
		return err;
	
	case MODERATE_BAN_ONLINE:
		buf = new char[16000 + 10];
		if (buf == NULL)
			{ ASSERT(_WARN_, FALSE); buf = s; iBufSize = MAX_MSG;}

		peer1 = p->connection->GetPeerName();
		sprintf(buf, GetString(77, getS), // "%s [IP: %s, e-mail: %s] was banned (online) by %s [e-mail: %s]\n"
				   p->info.login, peer1 ? peer1 : "???", p->email[0] ? p->email : "???",
				   client->info.login, client->email[0] ? client->email : "???");

		AddChatToBuffer(buf, iBufSize - strlen(buf));

		sprintf(subject, GetString(139, getS), p->info.login, GetString(78, getS2), client->info.login); //"%s was %s by %s", bannee, "Banned Online", bannor
		SendMail(GetString(72, getS), GetString(74, getS2), subject, buf); // "roomserver@won.net", "sysop@won.net"
		
		if(buf != s) //this means we had enough memory to allocate 16000 bytes for buf.
			delete [] buf;
		
		SendDisplayStringMessage(p, GetString(79, getS), GetString(80, getS2)); // "Banned", "You have been banned from the system by a WON host.  Please be more respectful."

		if(p->info.table != -1 && p->info.seat < MAX_GAME_PLAYERS)
		{
			tables[p->info.table].UpdateRatings(p->info.seat, tables[p->info.table].GetNumPlayersAtTable(), RRG_LOSS, RRU_MODERATORBAN, IsPartnershipGame());
		}

		connections->MyFlushAndClose(p->connection);
		
		if (dbConnection == NULL)
			return SendDisplayStringMessage(client, GetString(81, getS), GetString(82, getS2)); // "Ban", "The ban could not be done because the database was not available.  The player was booted."
		
		// send database ban message
		
		mtdbBAN.messageType = MTDB_RS_SET_BANNED;
		strcpy(mtdbBAN.login, p->info.login);
		strupr(mtdbBAN.login);
		mtdbBAN.OfflineBanFlag = 0;
		mtdbBAN.playerMachineID = p->machineID;
		// LogMachineId("MODERATE_BAN_ONLINE", mtdbBAN.login, mtdbBAN.playerMachineID);
		strcpy(mtdbBAN.moderatorName, client->info.login);
		strcpy(mtdbBAN.gameName, gameName);
		strcpy(mtdbBAN.roomName, roomName);
		
		TRACE(_ROUTINE_ "Sending to DATABASE: %s.\n", GetMessageDescription((char *)&mtdbBAN));
		
		err = connections->PostMessage(dbConnection, (char *)&mtdbBAN, sizeof(MTDB_RS_SET_BANNED_MESSAGE));
		if (err)
			TRACE(_WARN_ "RoomServer::HandleMTS_ROOM_MODERATE(): connections->PostMessage() returned error %d\n", err);
		
		return err;
	
	case MODERATE_BAN_OFFLINE:
		buf = new char[16000 + 10];
		if (buf == NULL)
			{ ASSERT(_WARN_, FALSE); buf = s; iBufSize = MAX_MSG;}

		sprintf(buf, GetString(83, getS), // "%s was banned (offline) by %s [e-mail: %s]\n"
				   mts->login, client->info.login, client->email[0] ? client->email : "???");

		AddChatToBuffer(buf, iBufSize - strlen(buf));

		sprintf(subject, GetString(139, getS), mts->login, GetString(84, getS2), client->info.login); //"%s was %s by %s", bannee, "Banned Offline", bannor
		SendMail(GetString(72, getS), GetString(74, getS2), subject, buf); // "roomserver@won.net", "sysop@won.net"
		
		if(buf != s) //this means we had enough memory to allocate 16000 bytes for buf.
			delete [] buf;
		
		if (dbConnection == NULL)
			return SendDisplayStringMessage(client, GetString(81, getS), GetString(85, getS2)); // "Ban", "The ban could not be done because the database was not available."
		
		// send database ban message
		
		mtdbBAN.messageType = MTDB_RS_SET_BANNED;
		strcpy(mtdbBAN.login, mts->login);
		strupr(mtdbBAN.login);
		mtdbBAN.OfflineBanFlag = 1;
		// mtdbBAN.playerMachineID = p->machineID;
		strcpy(mtdbBAN.moderatorName, client->info.login);
		strcpy(mtdbBAN.gameName, gameName);
		strcpy(mtdbBAN.roomName, roomName);
		
		TRACE(_ROUTINE_ "Sending to DATABASE: %s.\n", GetMessageDescription((char *)&mtdbBAN));
		
		err = connections->PostMessage(dbConnection, (char *)&mtdbBAN, sizeof(MTDB_RS_SET_BANNED_MESSAGE));
		if (err)
			TRACE(_WARN_ "RoomServer::HandleMTS_ROOM_MODERATE(): connections->PostMessage() returned error %d\n", err);
		
		return 0;
	
	case MODERATE_UNBAN:
		sprintf(s, GetString(86, getS), // "%s was un-banned by %s [e-mail: %s]\n"
				   mts->login, client->info.login, client->email[0] ? client->email : "???");
		SendMail(GetString(72, getS), GetString(74, getS2), GetString(87, getS3), s); // "roomserver@won.net", "sysop@won.net", "Un-Ban"
		
		if (dbConnection == NULL)
			return SendDisplayStringMessage(client, GetString(87, getS), GetString(88, getS2)); // "Un-Ban", "The un-ban could not be done because the database was not available."
		
		// send database unban message
		mtdbUNBAN.messageType = MTDB_RS_SET_UNBANNED;
		strcpy(mtdbUNBAN.login, mts->login);
		strupr(mtdbUNBAN.login);
		
		TRACE(_ROUTINE_ "Sending to DATABASE: %s.\n", GetMessageDescription((char *)&mtdbUNBAN));
		
		err = connections->PostMessage(dbConnection, (char *)&mtdbUNBAN, sizeof(MTDB_RS_SET_UNBANNED_MESSAGE));
		if (err)
			TRACE(_WARN_ "RoomServer::HandleMTS_ROOM_MODERATE(): connections->PostMessage() returned error %d\n", err);
		
		return 0;
	
	default:
		ASSERT(_WARN_, FALSE);
		
		return 0;
	}
}

int RoomServer::HandleMTS_ROOM_COMPLAIN(ROOMSERVERPLAYER *client, char *message, int length)
{
	char szSubject[512], *peer1, *peer2, *szBody;
	i18nStr getS, getS2, getS3, getS4, getS5, getS6, getS7, szTo;
	int returnValue, copyAmount, spaceRemaining;
	ROOMSERVERPLAYER *p;
	MTS_ROOM_COMPLAIN_MESSAGE *mts = (MTS_ROOM_COMPLAIN_MESSAGE *)message;
	const int iBufSize = 16000;
	
	if (mts->login[0]) {
		p = GetClientFromName(mts->login);
		if (p == NULL)
			{ ASSERT(_WARN_, FALSE); return -1; }
	}
	else
		p = NULL;
	
	szBody = new char[iBufSize + 10];
	if (szBody == NULL)
		{ ASSERT(_WARN_, FALSE); return -1; }

	*szBody = '\0';

	if (mts->complaintType == RATINGS_COMPLAINT)
	{
		GetString(143, szSubject); // "Ratings Feedback"
		
		sprintf(szBody, GetString(145, getS), 
				   // "Feedback from %s (email: %s) about game %s, room %s:\n\n"

				   client->info.login, client->email[0] ? client->email : "???", 
				   gameName, roomName); 

		spaceRemaining = iBufSize - strlen(szBody);
		copyAmount = mts->messageLength;
		if (copyAmount > 1000)
			copyAmount = 1000;
		if (copyAmount > spaceRemaining)
			copyAmount = spaceRemaining;
		
		strnzcpy(&szBody[strlen(szBody)], mts->message, copyAmount);

		GetString(144, szTo); // "hoylecomments@won.net"
	}

	if (mts->complaintType != RATINGS_COMPLAINT)
	{
		peer1 = client->connection->GetPeerName();
		peer2 = p ? p->connection->GetPeerName() : NULL;
		
		sprintf(szBody, GetString(89, getS),
				   // "Complaint from %s (%s) [IP: %s, e-mail: %s]\n"
				   // "about %s (%s) [IP: %s, e-mail: %s]\n\n"
				   // "%s - %s [%s]\n\n"
				   // "Text:\n\n"
				   
				   client->info.login,
				   client->IsMonitorOrSuperUser() ? GetString(90, getS2) : GetString(91, getS2), // "moderator" : "user"
				   peer1 ? peer1 : "???",
				   client->email[0] ? client->email : "???",
				   
				   mts->login[0] ? mts->login : GetString(132, getS7), // "SYSTEM"
				   (mts->login[0] && p) ? (p->IsMonitorOrSuperUser() ? GetString(90, getS3) : GetString(91, getS3)) : GetString(92, getS3), // "moderator" : "user" : "N/A"
				   (mts->login[0] && p) ? (peer2 ? peer2 : "???") : GetString(92, getS4), // "N/A"
				   (mts->login[0] && p) ? (p->email[0] ? p->email : "???") : GetString(92, getS5), // "N/A"
				   
				   gameName, roomName, mts->inGame ? GetString(93, getS6) : GetString(94, getS6)); // "In Game" : "In Room"
		
		spaceRemaining = iBufSize - strlen(szBody);
		copyAmount = mts->messageLength;
		if (copyAmount > 1000)
			copyAmount = 1000;
		if (copyAmount > spaceRemaining)
			copyAmount = spaceRemaining;
		
		strnzcpy(&szBody[strlen(szBody)], mts->message, copyAmount);
		spaceRemaining -= copyAmount;
		
		if (!mts->inGame) {
			AddChatToBuffer(szBody, spaceRemaining);
		}
		
		sprintf(szSubject, GetString(96, getS), mts->login[0] ? mts->login : GetString(97, getS2)); // "complaint about %s", "SYSTEM"

		if(mts->login[0])
			GetString(74, szTo); // "sysop@won.net"
		else
			GetString(75, szTo); // "hoylecomments@won.net"
	}

	// szTo contains who the message is going to
	// szSubject contains the subject
	// szBody contains the body text
	returnValue = SendMail(GetString(72, getS), szTo, szSubject, szBody); // "roomserver@won.net"
	
	delete [] szBody;
	
	return returnValue;
}

////////////////

int RoomServer::HandleMTS_ROOM_TI_CHECK_OUT_PAGE(ROOMSERVERPLAYER *client, char *message, int length)
{
	i18nStr getS, getS2;
	int err, playerID;
	ROOMSERVERPLAYER *p;
	MTS_ROOM_TI_CHECK_OUT_PAGE_MESSAGE *mts = (MTS_ROOM_TI_CHECK_OUT_PAGE_MESSAGE *)message;
	MFS_ROOM_TI_CHECK_OUT_PAGE_MESSAGE mfs;
	
	if (mts->playerID != -1)
		if (mts->playerID != client->info.playerID)
			{ ASSERT(_WARN_, FALSE); return -1; }
	
	if (!(client->IsMonitorOrReferee() || client->IsTournamentAdministrator() || client->IsSuperUser()))
		{ ASSERT(_WARN_, FALSE); return -1; }
	
	if (mts->pageNumber < 0 || mts->pageNumber > 4)
		{ ASSERT(_WARN_, FALSE); return -1; }
	
	if (mts->playerID == -1) 
	{
		// checking page back in
		ASSERT(_WARN_, tournamentInformation.header.checkOutPlayerIDs[mts->pageNumber] == client->info.playerID);

		if (tournamentInformation.header.checkOutPlayerIDs[mts->pageNumber] == client->info.playerID)
			switch (mts->pageNumber) 
			{
			case TI_PAGE_CURRENT:
				tournamentInformation.header.sittingEnabled = mts->header.sittingEnabled;
				tournamentInformation.header.joinEnabled = mts->header.joinEnabled;
				tournamentInformation.header.startingBankValue = mts->header.startingBankValue;
				if (!tournamentInformation.header.sittingEnabled)
					RemovePlayersFromTables();
				break;
			case TI_PAGE_RULES:
				break;
			case TI_PAGE_ASSIGNMENTS:
				break;
			case TI_PAGE_AUTOMATIC:
				tournamentInformation.header.automaticResultsEnabled = mts->header.automaticResultsEnabled;
				break;
			case TI_PAGE_OFFICIAL:
				break;
			default:
				ASSERT(_WARN_, FALSE);
			}

		tournamentInformation.header.checkOutPlayerIDs[mts->pageNumber] = -1;
	}
	else 
	{
		playerID = tournamentInformation.header.checkOutPlayerIDs[mts->pageNumber];
		if (playerID != -1) 
		{
			// sanity check: make sure the player who owns the page is still here
			p = GetClientFromID(playerID);
			if (p == NULL)
				ASSERT(_WARN_, FALSE); // fall through to do the checkout
			else
				return SendDisplayStringMessage(client, GetString(98, getS), GetString(99, getS2)); // "Page In Use", "This page is already checked out."
		}
		
		tournamentInformation.header.checkOutPlayerIDs[mts->pageNumber] = mts->playerID;
	}
	
	// tell everyone in the room about the check out/in
	
	mfs.messageType = MFS_ROOM_TI_CHECK_OUT_PAGE;
	mfs.playerID = tournamentInformation.header.checkOutPlayerIDs[mts->pageNumber];
	mfs.pageNumber = mts->pageNumber;
	mfs.header = tournamentInformation.header;
	
	err = BroadcastRoomMessage((char *)&mfs, sizeof(MFS_ROOM_TI_CHECK_OUT_PAGE_MESSAGE));
	
	return err;
}

int RoomServer::HandleMTS_ROOM_TI_ADD_TO_TEXT(ROOMSERVERPLAYER *client, char *message, int length)
{
	char buffer[MAX_MSG];
	int err;
	MTS_ROOM_TI_ADD_TO_TEXT_MESSAGE *mts = (MTS_ROOM_TI_ADD_TO_TEXT_MESSAGE *)message;
	MFS_ROOM_TI_ADD_TO_TEXT_MESSAGE *mfs = (MFS_ROOM_TI_ADD_TO_TEXT_MESSAGE *)buffer;
	
	if (mts->playerID != client->info.playerID)
		{ ASSERT(_WARN_, FALSE); return -1; }
	
	if (!(client->IsMonitorOrReferee() || client->IsTournamentAdministrator() || client->IsSuperUser()))
		{ ASSERT(_WARN_, FALSE); return -1; }
	
	if (mts->pageNumber < 0 || mts->pageNumber > 4)
		{ ASSERT(_WARN_, FALSE); return -1; }
	
	if (tournamentInformation.header.checkOutPlayerIDs[mts->pageNumber] != client->info.playerID)
		{ ASSERT(_WARN_, FALSE); return -1; }
	
	// clear the text if requested
	
	if (mts->clear)
		tournamentInformation.text[mts->pageNumber][0] = 0;
	
	// add the text to the server's copy
	
	if ((strlen(mts->text) + strlen(tournamentInformation.text[mts->pageNumber])) < MAX_TOURNAMENT_INFO)
		strcat(tournamentInformation.text[mts->pageNumber], mts->text);
	
	// tell everyone in the room about the check out/in
	
	mfs->messageType = MFS_ROOM_TI_ADD_TO_TEXT;
	mfs->pageNumber = mts->pageNumber;
	mfs->clear = mts->clear;
	strnzcpy(mfs->text, mts->text, 255);
	
	err = BroadcastRoomMessage((char *)mfs, sizeof(MFS_ROOM_TI_ADD_TO_TEXT_MESSAGE) + strlen(mfs->text) + 1);
	
	return err;
}

int RoomServer::HandleMTS_ROOM_TI_JOIN(ROOMSERVERPLAYER *client, char *message, int length)
{
	i18nStr getS, getS2;
	int err;
	MTS_ROOM_TI_JOIN_MESSAGE *mts = (MTS_ROOM_TI_JOIN_MESSAGE *)message;
	MFS_ROOM_TI_JOIN_MESSAGE mfs;
	ROOMSERVERPLAYER *p;
	
	p = GetClientFromID(mts->playerID);
	if (p == NULL)
		return 0; // allow this case for when the player leaves before the join is processed
	
	if (!(client->IsMonitorOrReferee() || client->IsTournamentAdministrator() || client->IsSuperUser()))
	{
		if (mts->playerID != client->info.playerID)
			{ ASSERT(_WARN_, FALSE); return -1; }
		if (mts->joining && !tournamentInformation.header.joinEnabled)
			return SendDisplayStringMessage(client, GetString(100, getS), GetString(101, getS2)); // "Cannot Join Tournament", "Joining is currently disabled for this tournament."
	}
	
	if (mts->joining && p->info.table != -1)
		return SendDisplayStringMessage(client, GetString(100, getS), GetString(102, getS2)); // "Cannot Join Tournament", "You cannot join the tournament while you are at a table."
	
	if (mts->joining) {
		p->info.flags |= UF_IN_TOURNAMENT; // joined the tournament
		p->info.tournamentBankRoll = tournamentInformation.header.startingBankValue;
	}
	else {
		p->info.flags &= (0xFF ^ UF_IN_TOURNAMENT); // unjoined the tournament
		if (p->info.table != -1 && p->info.seat < MAX_GAME_PLAYERS) {
			TablePlayerChanged(p, TPC_LEFT);
			SendDisplayStringMessage(p, GetString(103, getS), GetString(104, getS2)); // "Tournament Players Only", "You have been removed from your table because you have been removed from the tournament."
		}
	}
	
	// tell everyone in the room about the join
	
	mfs.messageType = MFS_ROOM_TI_JOIN;
	mfs.playerID = mts->playerID;
	mfs.joining = mts->joining;
	mfs.startingBankValue = tournamentInformation.header.startingBankValue;
	
	err = BroadcastRoomMessage((char *)&mfs, sizeof(MFS_ROOM_TI_JOIN_MESSAGE));
	
	return err;
}

int RoomServer::HandleMTS_ROOM_TI_SET_BANK(ROOMSERVERPLAYER *client, char *message, int length)
{
	i18nStr getS, getS2;
	int err;
	MTS_ROOM_TI_SET_BANK_MESSAGE *mts = (MTS_ROOM_TI_SET_BANK_MESSAGE *)message;
	MFS_ROOM_TI_SET_BANK_MESSAGE mfs;
	ROOMSERVERPLAYER *p;
	
	if (!(client->IsMonitorOrReferee() || client->IsTournamentAdministrator() || client->IsSuperUser()))
		{ ASSERT(_WARN_, FALSE); return -1; }
	
	p = GetClientFromID(mts->playerID);
	if (p == NULL)
		return 0; // allow this case for when the player leaves before the join is processed
	
	if (!p->IsInTournament())
		return SendDisplayStringMessage(client, GetString(103, getS), GetString(105, getS2)); // "Tournament Players Only", "You can only set the tournament bank of players who have joined the tournament."
	
	if (p->info.table != -1)
		return SendDisplayStringMessage(client, GetString(106, getS), GetString(107, getS2)); // "Cannot Set Bank", "You cannot set the tournament bank of a player who is at a table."
	
	if (mts->bankValue < 0)
		mts->bankValue = 0;
	
	if (mts->bankValue > 1000000000)
		mts->bankValue = 1000000000;
	
	p->info.tournamentBankRoll = mts->bankValue;
	
	// tell everyone in the room about the bank set
	
	mfs.messageType = MFS_ROOM_TI_SET_BANK;
	mfs.playerID = mts->playerID;
	mfs.bankValue = mts->bankValue;
	
	err = BroadcastRoomMessage((char *)&mfs, sizeof(MFS_ROOM_TI_SET_BANK_MESSAGE));
	
	return err;
}

int RoomServer::HandleMTS_ROOM_TI_GENERATE_ASSIGNMENTS(ROOMSERVERPLAYER *client, char *message, int length)
{
	char buffer[MAX_MSG];
	i18nStr getS, getS2;
	int i, j, k, n, err, index, playerIndicies[MAX_ROOM_PLAYERS];
	MTS_ROOM_TI_GENERATE_ASSIGNMENTS_MESSAGE	*mts		= (MTS_ROOM_TI_GENERATE_ASSIGNMENTS_MESSAGE *)message;
	MFS_ROOM_TI_ADD_TO_TEXT_MESSAGE				*mfsTEXT	= (MFS_ROOM_TI_ADD_TO_TEXT_MESSAGE *)buffer;
	
	if (!(client->IsMonitorOrReferee() || client->IsTournamentAdministrator() || client->IsSuperUser()))
		{ ASSERT(_WARN_, FALSE); return -1; }
	
	if (mts->playersPerTable < 1 || mts->playersPerTable > MAX_GAME_PLAYERS)
		return SendDisplayStringMessage(client, GetString(108, getS), GetString(109, getS2)); // "Invalid Generate Request", "The specified number of players per table is invalid."
	
	mfsTEXT->messageType = MFS_ROOM_TI_ADD_TO_TEXT;
	mfsTEXT->pageNumber = TI_PAGE_ASSIGNMENTS;
	
	for (i = 0, n = 0 ; i < MAX_ROOM_PLAYERS ; i++)
		if (clients[i].status == CS_ACTIVE)
			if (clients[i].IsInTournament())
				playerIndicies[n++] = i;
	
	if (n == 0)
		return SendDisplayStringMessage(client, GetString(108, getS), GetString(110, getS2)); // "Invalid Generate Request", "Nobody has joined the tournament yet."
	
	for (i = 0 ; i < 5 ; i++)
		for (j = 0 ; j < n ; j++) {
			k = randomGenerator->GetRandom() % n;
			index = playerIndicies[j];
			playerIndicies[j] = playerIndicies[k];
			playerIndicies[k] = index;
		}
	
	for (i = 0 ; i < n ; ) {
		mfsTEXT->clear = (i == 0);
		sprintf(mfsTEXT->text, GetString(133, getS), (i == 0) ? "" : "\r\n\r\n", (int)(i / mts->playersPerTable + 1)); // "%sTable %d:"
		for (j = 0 ; j < mts->playersPerTable && (i + j) < n ; j++)
			sprintf(&mfsTEXT->text[strlen(mfsTEXT->text)], "%s%s", (j == 0) ? " " : ", ", clients[playerIndicies[i + j]].info.login);
		i += j;
		
		TRACE(_ROUTINE_ "Sending to %s (playerID = %d): %s.\n", client->info.login, client->info.playerID, GetMessageDescription((char *)mfsTEXT));
		
		err = connections->PostMessage(client->connection, (char *)mfsTEXT, sizeof(MFS_ROOM_TI_ADD_TO_TEXT_MESSAGE) + strlen(mfsTEXT->text) + 1);
		if (err)
			TRACE(_WARN_ "RoomServer::HandleMTS_ROOM_TI_GENERATE_ASSIGNMENTS(): connections->PostMessage() returned error %d\n", err);
	}
	
	return 0;
}

int RoomServer::HandleMTS_ROOM_TI_CLEAR_AUTO_RESULTS(ROOMSERVERPLAYER *client, char *message, int length)
{
	int err;
	MFS_ROOM_TI_ADD_TO_TEXT_MESSAGE mfsTEXT;
	
	if (!(client->IsMonitorOrReferee() || client->IsTournamentAdministrator() || client->IsSuperUser()))
		{ ASSERT(_WARN_, FALSE); return -1; }
	
	tournamentInformation.text[TI_PAGE_AUTOMATIC][0] = 0;
	
	mfsTEXT.messageType = MFS_ROOM_TI_ADD_TO_TEXT;
	mfsTEXT.pageNumber = TI_PAGE_AUTOMATIC;
	mfsTEXT.clear = TRUE;
	mfsTEXT.text[0] = 0;
	
	err = BroadcastRoomMessage((char *)&mfsTEXT, sizeof(MTS_ROOM_TI_ADD_TO_TEXT_MESSAGE));
	
	return err;
}

////////////////

int RoomServer::HandleMTS_GAME_STATE(ROOMSERVERPLAYER *client, char *message, int length)
{
	int err;
	MTS_GAME_STATE_MESSAGE *mts = (MTS_GAME_STATE_MESSAGE *)message;
	MFS_GAME_STATE_MESSAGE mfs;
	
	mfs.messageType = MFS_GAME_STATE;
	mfs.table = mts->table;
	mfs.gameState = tables[mts->table].gameState;
	
	TRACE(_ROUTINE_ "Sending to %s (playerID = %d): %s.\n", client->info.login, client->info.playerID, GetMessageDescription((char *)&mfs));
	
	err = connections->PostMessage(client->connection, (char *)&mfs, sizeof(MTS_GAME_STATE_MESSAGE));
	if (err) {
		TRACE(_WARN_ "RoomServer::HandleMTS_GAME_STATE_MESSAGE(): connections->PostMessage() returned error %d\n", err);
		
		return err;
	}
	
	return SendDisplayStringMessage(client, "<SETTICKER>", tickerText, TRUE);
}

int RoomServer::HandleMTS_GAME_CHAT(ROOMSERVERPLAYER *client, char *message, int length)
{
	char data[1024];
	int n, textLength, err;
	MTS_GAME_CHAT_MESSAGE *mts = (MTS_GAME_CHAT_MESSAGE *)message;
	MFS_GAME_CHAT_MESSAGE *mfs;
	
	// ignore chat from muted watchers, unless they are monitors
	
	if (client->info.seat >= MAX_GAME_PLAYERS && tables[client->info.table].muteWatchers)
		if (!client->IsMonitorOrSuperUser())
			return 0;
	
	mfs = (MFS_GAME_CHAT_MESSAGE *)data;
	
	if (mts->flags & GCM_SOUND_KEY) 
	{
		n = mts->sound;
		switch (n) 
		{
		case '1': mfs->sound = 201; GetString(8, mfs->text); break; // "<applause>"
		case '2': mfs->sound = 202; GetString(9, mfs->text); break; // "<Wow!>"
		case '3': mfs->sound = 203; GetString(10, mfs->text); break; // "<Oh, gnarly!>"
		case '4': mfs->sound = 204; GetString(11, mfs->text); break; // "<Whatever!>"
		case '5': mfs->sound = 205; GetString(12, mfs->text); break; // "<Inconceivable!>"
		case '6': mfs->sound = 206; GetString(13, mfs->text); break; // "<Awwwww!>"
		case '7': mfs->sound = 207; GetString(14, mfs->text); break; // "<Rough!>"
		case '8': mfs->sound = 208; GetString(15, mfs->text); break; // "<pant...pant...pant>"
		case '9': mfs->sound = 209; GetString(16, mfs->text); break; // "<Ahem...>"
		case '0': mfs->sound = 210; GetString(17, mfs->text); break; // "<Are you still there?>"
		case 'Q': mfs->sound = 211; GetString(18, mfs->text); break; // "<yay yay yay yay yay>"
		case 'W': mfs->sound = 212; GetString(19, mfs->text); break; // "<hee hee hee ha ha>"
		case 'E': mfs->sound = 213; GetString(20, mfs->text); break; // "<Excuse me!>"
		case 'R': mfs->sound = 214; GetString(21, mfs->text); break; // "<Pardon me!>"
		case 'T': mfs->sound = 215; GetString(22, mfs->text); break; // "<Hey!>"
		case 'Y': mfs->sound = 216; GetString(23, mfs->text); break; // "<Squawk!>"
		case 'U': mfs->sound = 217; GetString(24, mfs->text); break; // "<You go, girl!>"
		case 'I': mfs->sound = 218; GetString(25, mfs->text); break; // "<I am afraid I goofed.>"
		case 'O': mfs->sound = 219; GetString(26, mfs->text); break; // "<Are we having fun yet?>"
		case 'P': mfs->sound = 220; GetString(27, mfs->text); break; // "<Nah Nah Nah Nah Nah Nah>"
		default:
			return 0;
		}
		textLength = strlen(mfs->text + 1);
	}
	else {
		mfs->sound = 0;
		textLength = length - (int)(mts->text - message);
		if (textLength > 255)
			textLength = 255;
		strnzcpy(mfs->text, mts->text, textLength);
	}
	
	// pass the chat message on to all players in the game
	
	mfs->messageType	= MFS_GAME_CHAT;
	mfs->playerID		= mts->playerID;
	mfs->table			= mts->table;
	mfs->seat			= mts->seat;
	mfs->flags			= mts->flags;
	
	err = BroadcastGameMessage(client->info.table, (char *)mfs, sizeof(MFS_GAME_CHAT_MESSAGE) + textLength);
	
	return err;
}

int RoomServer::HandleMTS_GAME_BOOT(ROOMSERVERPLAYER *client, char *message, int length)
{
	Boolean sendChat = FALSE, sendBoot = FALSE, sendStand = FALSE;
	char data[MAX_MSG], text[256];
	i18nStr getS, getS2;
	int i, n, table, seat, vote, textLength, err = 0;
	MTS_GAME_BOOT_MESSAGE *mts		= (MTS_GAME_BOOT_MESSAGE *)message;
	MFS_GAME_CHAT_MESSAGE *mfsGCM	= (MFS_GAME_CHAT_MESSAGE *)data;
	MFS_GAME_BOOT_MESSAGE mfsGBM;
	ROOMSERVERPLAYER *bootee;
	
	table = mts->table;
	seat = mts->seat;
	
	bootee = GetClientFromID(mts->playerID);
	if (bootee == NULL)
		return 0; // allow for the case that the player leaves before the boot message arrives
	
	// watcher
	
	if (seat >= MAX_GAME_PLAYERS) {
		// make sure the player to be booted is really there
		
		if (tables[table].watcherIndicies[seat - MAX_GAME_PLAYERS] == -1)
			{ ASSERT(_WARN_, FALSE); return -1; }
		if (tables[table].gameState.watcherIDs[seat - MAX_GAME_PLAYERS] != mts->playerID)
			{ ASSERT(_WARN_, FALSE); return -1; }
		
		sendChat = TRUE;
		sprintf(text, GetString(111, getS), bootee->info.login); // "I booted %s, who was a watcher."
		sendStand = TRUE;
	}
	
	// player
	
	if (seat < MAX_GAME_PLAYERS) {
		// make sure the player to be booted is really there
		
		if (tables[table].playerIndicies[seat] == -1)
			return 0; // allow for the case the player leaves the seat before the boot message arrives
		if (tables[table].gameState.players[seat].playerID != mts->playerID)
			return 0; // allow for the case the player leaves the seat before the boot message arrives
		
		if (bootee->votesNeededToBoot == -1) {
			// start of boot process
			
			n = CountPlayersAtTableFast(table, FALSE);
			
			if (n < 3)
				return SendDisplayStringMessage(client, GetString(112, getS), GetString(113, getS2), TRUE); // "Cannot Boot", "There must be at least three players to boot."
			
			sendChat = TRUE;
			sprintf(text, GetString(114, getS), bootee->info.login, bootee->info.login); // "I vote to boot %s.  Click %s and select Boot to confirm."
			
			bootee->votesNeededToBoot = n - 1; // everyone but the bootee himself must vote yes
			tables[table].gameState.players[seat].bootCount = 1;
			bootee->lastVoteTicks = GetTickCount();
			for (i = 0 ; i < MAX_GAME_PLAYERS ; i++) {
				vote = BV_NO_VOTE;
				if (i == client->info.seat)
					vote = BV_VOTED_YES;
				else
					if (IsHumanPlayer(table, i))
						vote = BV_CAN_VOTE;
				bootee->votes[i] = vote;
			}
			sendBoot = TRUE;
		}
		else {
			// another vote to boot
			
			if (bootee->votes[client->info.seat] == BV_VOTED_YES)
				return SendDisplayStringMessage(client, GetString(115, getS), GetString(116, getS2), TRUE); // "Cannot Vote Again", "You already voted to boot this player."
			
			if (bootee->votes[client->info.seat] == BV_NO_VOTE)
				return SendDisplayStringMessage(client, GetString(115, getS), GetString(117, getS), TRUE); // "Cannot Vote", "You are not eligible to vote to boot this player."
			
			sendChat = TRUE;
			sprintf(text, GetString(118, getS), bootee->info.login); // "I vote to boot %s."
			
			tables[table].gameState.players[seat].bootCount++;
			bootee->lastVoteTicks = GetTickCount();
			bootee->votes[client->info.seat] = BV_VOTED_YES;
			sendBoot = TRUE;
			
			if (tables[table].gameState.players[seat].bootCount >= bootee->votesNeededToBoot)
				sendStand = TRUE;
		}
	}
	
	// send messages
	
	if (sendChat) {
		mfsGCM->messageType = MFS_GAME_CHAT;
		mfsGCM->playerID = client->info.playerID;
		mfsGCM->table = mts->table;
		mfsGCM->seat = mts->seat;
		mfsGCM->flags = 0;
		strcpy(mfsGCM->text, text);
		textLength = strlen(text + 1);
		
		err = BroadcastGameMessage(client->info.table, (char *)mfsGCM, sizeof(MFS_GAME_CHAT_MESSAGE) + textLength);
	}
	
	if (sendBoot) {
		mfsGBM.messageType = MFS_GAME_BOOT;
		mfsGBM.playerID = bootee->info.playerID;
		mfsGBM.table = mts->table;
		mfsGBM.seat = mts->seat;
		mfsGBM.bootCount = tables[table].gameState.players[seat].bootCount;
		
		err = BroadcastGameMessage(client->info.table, (char *)&mfsGBM, sizeof(MFS_GAME_BOOT_MESSAGE));
	}
	
	if (sendStand) {
		TablePlayerChanged(bootee, TPC_BOOTED);
		
		bootee->probationTable = mts->table;
		bootee->probationTicks = GetTickCount();
		
		SendDisplayStringMessage(bootee, GetString(119, getS), GetString(120, getS2)); // "Booted", "You have been removed from the game by the other players."
	}
	
	return err;
}

int RoomServer::HandleMTS_GAME_RESTART(ROOMSERVERPLAYER *client, char *message, int length)
{
	i18nStr getS, getS2;
	int i, n, table, seat, err = 0;
	MTS_GAME_RESTART_MESSAGE *mts = (MTS_GAME_RESTART_MESSAGE *)message;
	MFS_GAME_RESTART_MESSAGE mfs;
	
	table = mts->table;
	seat = mts->seat;
	
	if (!IsRestartAllowed())
		{ ASSERT(_WARN_, FALSE); return -1; }

	// watchers cannot restart the game...
	if (seat >= MAX_GAME_PLAYERS)
		{ ASSERT(_WARN_, FALSE); return -1; }
	
	// handle player's vote
	tables[table].restartVotes[seat] = mts->restartGame;

	for (i = 0, n = 0; i < MAX_GAME_PLAYERS; i++)
		if (tables[table].restartVotes[i] || !IsHumanPlayer(table, i))
			n++;
	
	// send messages
	
	mfs.messageType = MFS_GAME_RESTART;
	mfs.table = table;
	for (i = 0; i < MAX_GAME_PLAYERS; i++)
		mfs.restartVotes[i] = tables[table].restartVotes[i];
	
	err = BroadcastGameMessage(client->info.table, (char *)&mfs, sizeof(MFS_GAME_RESTART_MESSAGE));
	
	if (n == MAX_GAME_PLAYERS) {
		if (ShowRestartMessage())
			BroadcastGameStringMessage(table, GetString(121, getS), GetString(122, getS2)); // "Reset Game", "The game has been reset."
		tables[table].RestartGame();
		ClearRestartVotes(table);
	}
	
	return err;
}

/////////// utility functions

//======================================================================
// This is called at startup and every 5 minutes in DoDatabaseWork()
//======================================================================
void RoomServer::ReadTickerText()
{
	int n;
	size_t size;
	FILE *f;
	
	tickerText[0] = 0;
	
	f = fopen("ticker.txt", "r");
	
	if (f == NULL)
		return;
	
	for (n = 0 ; n < 500 ; n++) {
		size = fread(&tickerText[n], 1, 1, f);
		if (size == 0)
			break;
	}
	
	tickerText[n] = 0;
	
	fclose(f);
}

void RoomServer::ClearBootRecord(ROOMSERVERPLAYER *client, Boolean clearProbation)
{
	int i;
	
	client->votesNeededToBoot = -1;
	client->lastVoteTicks = 0;
	for (i = 0 ; i < MAX_GAME_PLAYERS ; i++)
		client->votes[i] = BV_NO_VOTE;
	
	if (clearProbation) {
		client->probationTable = -1;
		client->probationTicks = 0;
	}

	if ((client->info.table >= 0 && client->info.table < NUM_ROOM_TABLES) &&
		(client->info.seat >= 0 && client->info.seat < MAX_GAME_PLAYERS))
		tables[client->info.table].gameState.players[client->info.seat].bootCount = -1;
}

void RoomServer::ClearExpiredBoots()
{
	int i, err;
	DWORD tickCount;
	MFS_GAME_BOOT_MESSAGE mfsGBM;
	
	tickCount = GetTickCount();
	
	for (i = 0 ; i < MAX_ROOM_PLAYERS ; i++)
		if (clients[i].status == CS_ACTIVE) {
			if (clients[i].votesNeededToBoot != -1)
				if (tickCount > (clients[i].lastVoteTicks + BOOT_EXPIRATION_TICKS)) {
					ClearBootRecord(&clients[i], FALSE);
					
					mfsGBM.messageType = MFS_GAME_BOOT;
					mfsGBM.playerID = clients[i].info.playerID;
					mfsGBM.table = clients[i].info.table;
					mfsGBM.seat = clients[i].info.seat;
					mfsGBM.bootCount = -1;
					
					err = BroadcastGameMessage(clients[i].info.table, (char *)&mfsGBM, sizeof(MFS_GAME_BOOT_MESSAGE));
				}
			if (clients[i].probationTable != -1)
				if (tickCount > (clients[i].probationTicks + PROBATION_TICKS)) {
					clients[i].probationTable = -1;
					clients[i].probationTicks = 0;
				}
		}
}

void RoomServer::ClearRestartVotes(int tableIndex)
{
	int i;
	MFS_GAME_RESTART_MESSAGE mfs;

	for (i = 0 ; i < MAX_GAME_PLAYERS ; i++)
		tables[tableIndex].restartVotes[i] = FALSE;

	if (tables[tableIndex].status != TS_PLAYING)
		return;

	if (!IsRestartAllowed())
		return;

	mfs.messageType = MFS_GAME_RESTART;
	mfs.table = tableIndex;
	for (i = 0; i < MAX_GAME_PLAYERS; i++)
		mfs.restartVotes[i] = tables[tableIndex].restartVotes[i];
	
	BroadcastGameMessage(tableIndex, (char *)&mfs, sizeof(MFS_GAME_RESTART_MESSAGE));
}

void RoomServer::GameStartSanityCheck(int table)
{
	int i, n, playerIndex;
	ROOMSERVERPLAYER *c;
	
	n = 0;
	
	for (i = 0 ; i < MAX_GAME_PLAYERS ; i++)
		if (IsHumanPlayer(table, i)) {
			n++;
			playerIndex = tables[table].playerIndicies[i];
			if (playerIndex < 0 || playerIndex >= MAX_ROOM_PLAYERS)
				{ ASSERT(_WARN_, FALSE); return; }
			c = &clients[playerIndex];
			if (c->status != CS_ACTIVE)
				{ ASSERT(_WARN_, FALSE); return; }
			if (c->info.table != table)
				{ ASSERT(_WARN_, FALSE); return; }
			if (c->info.seat != i)
				{ ASSERT(_WARN_, FALSE); return; }
		}
	
	for (i = 0 ; i < MAX_GAME_WATCHERS ; i++)
		if (tables[table].gameState.watcherIDs[i] != PLAYER_ID_EMPTY) {
			n++;
			playerIndex = tables[table].watcherIndicies[i];
			if (playerIndex < 0 || playerIndex >= MAX_ROOM_PLAYERS)
				{ ASSERT(_WARN_, FALSE); return; }
			c = &clients[playerIndex];
			if (c->status != CS_ACTIVE)
				{ ASSERT(_WARN_, FALSE); return; }
			if (c->info.table != table)
				{ ASSERT(_WARN_, FALSE); return; }
			if (c->info.seat != (MAX_GAME_PLAYERS + i))
				{ ASSERT(_WARN_, FALSE); return; }
		}
	
	if (CountPlayersAtTable(table, TRUE) != n)
		ASSERT(_WARN_, FALSE);
}

int RoomServer::CancelRequestOrInvitation(ROOMSERVERPLAYER *client)
{
	i18nStr getS, getS2;
	ROOMSERVERPLAYER *approver, *inviter;
	
	if (client->approvingPlayerID != -1) {
		approver = GetClientFromID(client->approvingPlayerID);
		if (!approver || approver->requesterPlayerID != client->info.playerID)
			ASSERT(_WARN_, FALSE);
		else {
			approver->requesterPlayerID = -1;
			approver->requestedSeat = -1;
		}
	}
	
	if (client->inviterTable != -1) {
		inviter = GetClientFromID(client->inviterPlayerID);
		if (inviter && inviter->info.table == client->inviterTable)
			return SendDisplayStringMessage(inviter, GetString(55, getS), GetString(123, getS2)); // "RSVP", "Your invitation was canceled."
	}
	
	client->requestingTable = -1;
	client->requestingSeat = -1;
	client->approvingPlayerID = -1;
	
	client->inviterTable = -1;
	client->inviterPlayerID = -1;
	client->invitedToWatch = FALSE;
	
	return 0;
}

int RoomServer::TransferApproverResponsibility(ROOMSERVERPLAYER *client)
{
	int requesterPlayerID, requestedSeat, approverSeat, approverIndex, table, err;
	MFS_ROOM_REQUEST_MESSAGE mfs;
	MFS_ROOM_DISAPPROVE_REQUEST_MESSAGE mfsRDR;
	ROOMSERVERPLAYER *requester, *newApprover;
	
	if (client->requesterPlayerID == -1)
		return 0;
	
	requesterPlayerID = client->requesterPlayerID;
	requestedSeat = client->requestedSeat;
	
	client->requesterPlayerID = -1;
	client->requestedSeat = -1;
	
	requester = GetClientFromID(requesterPlayerID);
	if (!requester)
		{ ASSERT(_WARN_, FALSE); return -1; }
	
	table = requester->requestingTable;
	if (table == -1)
		{ ASSERT(_WARN_, FALSE); return -1; }
	
	// try to find someone else at this table to be the approver
	
	approverSeat = GetPreferredApproverSeat(requester->requestingSeat);
	approverIndex = tables[table].playerIndicies[approverSeat];
	if (approverIndex == -1 || approverIndex == client->index || clients[approverIndex].requesterPlayerID != -1)
		for (approverSeat = 0 ; approverSeat < MAX_GAME_PLAYERS ; approverSeat++) {
			approverIndex = tables[table].playerIndicies[approverSeat];
			if (approverIndex != -1 && approverIndex != client->index && clients[approverIndex].requesterPlayerID == -1)
				break;
		}
	
	if (approverSeat == MAX_GAME_PLAYERS) { // couldn't find another approver
		requester->requestingTable = -1;
		requester->requestingSeat = -1;
		requester->approvingPlayerID = -1;
		
		mfsRDR.messageType = MFS_ROOM_DISAPPROVE_REQUEST;
		mfsRDR.playerID = requester->info.playerID;
		mfsRDR.table = requester->requestingTable;
		mfsRDR.seat = requester->requestingSeat;
		mfsRDR.notice = 2; // "Nobody is available to approve your request."
		
		TRACE(_ROUTINE_ "Sending to %s (playerID = %d): %s.\n", requester->info.login, requester->info.playerID, GetMessageDescription((char *)&mfsRDR));
		
		err = connections->PostMessage(requester->connection, (char *)&mfsRDR, sizeof(MFS_ROOM_DISAPPROVE_REQUEST_MESSAGE));
		if (err)
			TRACE(_WARN_ "RoomServer::TransferApproverResponsibility(): connections->PostMessage() returned error %d\n", err);
		
		return err;
	}
	
	newApprover = &clients[approverIndex];
	
	ASSERT(_WARN_, newApprover->requesterPlayerID == -1);
	
	newApprover->requesterPlayerID = requesterPlayerID;
	newApprover->requestedSeat = requestedSeat;
	
	mfs.messageType = MFS_ROOM_REQUEST;
	mfs.playerID = requesterPlayerID;
	mfs.table = newApprover->info.table;
	mfs.seat = requestedSeat;
	mfs.cancel = FALSE;
	
	TRACE(_ROUTINE_ "Sending to %s (playerID = %d): %s.\n", newApprover->info.login, newApprover->info.playerID, GetMessageDescription((char *)&mfs));
	
	err = connections->PostMessage(newApprover->connection, (char *)&mfs, sizeof(MFS_ROOM_REQUEST_MESSAGE));
	if (err)
		TRACE(_WARN_ "RoomServer::TransferApproverResponsibility(): connections->PostMessage() returned error %d\n", err);
	
	return err;
}

int RoomServer::RelinquishBootVotes(ROOMSERVERPLAYER *client)
{
	i18nStr getS, getS2;
	int i, n, index, table, seat, bootCount;
	MFS_GAME_BOOT_MESSAGE mfsGBM;
	ROOMSERVERPLAYER *bootee;
	
	table = client->info.table;
	seat = client->info.seat;
	
	ClearBootRecord(client, FALSE);
	tables[table].gameState.players[seat].bootCount = -1;
	
	// for other players at the table who were waiting for this player to vote to boot them,
	// adjust their bootCount and boot if appropriate
	
	n = CountPlayersAtTableFast(table, FALSE);
	
	for (i = 0 ; i < MAX_GAME_PLAYERS ; i++) {
		index = tables[table].playerIndicies[i];
		if (index == -1)
			continue;
		bootee = &clients[index];
		
		if (bootee->votesNeededToBoot == -1)
			continue;
		
		if (n <= 3) // if client leaving means we have 2 or fewer players, remove all bootCounts...
			ClearBootRecord(bootee, FALSE);
		
		/*
		if (bootee->votes[seat] == BV_NO_VOTE)
			continue;
		
		if (bootee->votes[seat] == BV_VOTED_YES) // lower the bootCount by 1 if the player had voted
			tables[table].gameState.players[i].bootCount--;
		else {
			ASSERT(_WARN_, bootee->votes[seat] == BV_CAN_VOTE);
			bootee->votesNeededToBoot--; // otherwise, decrement the votes needed to boot
		}
		*/
		
		if (bootee->votesNeededToBoot != -1 && bootee->votes[seat] == BV_CAN_VOTE) {
			bootee->votesNeededToBoot--;
			
			bootCount = tables[table].gameState.players[i].bootCount;
			
			if (bootCount != -1 && bootCount >= bootee->votesNeededToBoot) {
				TablePlayerChanged(bootee, TPC_BOOTED);
				
				SendDisplayStringMessage(bootee, GetString(119, getS), GetString(120, getS2)); // "Booted", "You have been removed from the game by the other players."
			}
		}
		
		bootee->votes[seat] = BV_NO_VOTE;
		
		mfsGBM.messageType = MFS_GAME_BOOT;
		mfsGBM.playerID = bootee->info.playerID;
		mfsGBM.table = bootee->info.table;
		mfsGBM.seat = bootee->info.seat;
		mfsGBM.bootCount = tables[table].gameState.players[i].bootCount;
		
		BroadcastGameMessage(table, (char *)&mfsGBM, sizeof(MFS_GAME_BOOT_MESSAGE));
	}
	
	return 0;
}

int RoomServer::TablePlayerChanged(ROOMSERVERPLAYER *client, int status)
{
	Boolean everyoneIsReady;
	char buffer[MAX_MSG];
	int i, n, err, playerID, table, seat, newID;
	MFS_ROOM_STAND_MESSAGE mfsRSM;
	MFS_ROOM_END_GAME_MESSAGE *mfsREGM = (MFS_ROOM_END_GAME_MESSAGE *)buffer;
	MFS_GAME_STATE_MESSAGE mfsGSM;
	MFS_ROOM_START_GAME_MESSAGE mfsRSGM;
	i18nStr getS, getS2;
	
	playerID = client->info.playerID;
	table = client->info.table;
	seat = client->info.seat;
	
	//////////////////
	
	if (status == TPC_LEFT || status == TPC_BOOTED || status == TPC_SWITCHINGSEATS || status == TPC_REMOVED_NOTENOUGHMONEY) {
		if (seat < MAX_GAME_PLAYERS) {
			TransferApproverResponsibility(client);
			RelinquishBootVotes(client);
		}
		
		if((status == TPC_LEFT || status == TPC_BOOTED) && tables[table].status == TS_PLAYING && tables[table].PenaltyForLeavingNow(client->info.seat))
		{
			if(tables[table].GetNumPlayersAtTable() >= 2 && client->info.seat < MAX_GAME_PLAYERS) // watchers shouldn't get penalized
				tables[table].UpdateRatings(client->info.seat, 1, RRG_QUIT, RRU_LEFTTABLE, FALSE);
		}
		
		//First off, save the table options so they can be restored if necessary.
		//I'm using mfsREGM since it already exits and has all the proper fields.
		if(status == TPC_SWITCHINGSEATS)
		{
			mfsREGM->joinOption		= tables[table].joinOption;
			mfsREGM->watchersOption	= tables[table].watchersOption;
			mfsREGM->muteWatchers	= tables[table].muteWatchers;
			mfsREGM->ratingsFlags	= tables[table].ratingsFlags;
			mfsREGM->ratingsSkills	= tables[table].ratingsSkills;
			tables[table].GetTableGameValues(mfsREGM->gameValues);
			strcpy(mfsREGM->description, tables[table].description);
		}

		// this call to PlayerStands() has to come before the assignments just below
		// for the case when the player is booted and we need to broadcast a table message
		tables[table].PlayerStands(client, &newID, status == TPC_BOOTED);

		if(status == TPC_SWITCHINGSEATS)
		{
			//Traces are here to help track down a Pinochle bug.
			TRACE(_ROUTINE_ "Switching seats...\n");
			tables[table].joinOption		= mfsREGM->joinOption;
			tables[table].watchersOption	= mfsREGM->watchersOption;
			tables[table].muteWatchers		= mfsREGM->muteWatchers;
			tables[table].ratingsFlags		= mfsREGM->ratingsFlags;
			tables[table].ratingsSkills		= mfsREGM->ratingsSkills;
			tables[table].SetTableGameValues(mfsREGM->gameValues);
			strcpy(tables[table].description, mfsREGM->description);
			TRACE(_ROUTINE_ "Switched seats...\n");
		}

		client->info.table = -1;
		client->info.seat = -1;
		client->info.flags &= (0xFF ^ UF_READY_TO_PLAY); // no longer ready to play
		
		if (seat < MAX_GAME_PLAYERS)
			tables[table].playerIndicies[seat] = -1;
		else
			tables[table].watcherIndicies[seat - MAX_GAME_PLAYERS] = -1;
		
		mfsRSM.messageType = MFS_ROOM_STAND;
		mfsRSM.playerID = playerID;
		mfsRSM.table = table;
		mfsRSM.seat = seat;
		mfsRSM.newID = newID;
		BroadcastRoomMessage((char *)&mfsRSM, sizeof(MFS_ROOM_STAND_MESSAGE));
		
		if (seat < MAX_GAME_PLAYERS)
			ClearRestartVotes(table);

		if (tables[table].status == TS_EMPTY) {
			// the table was re-inited by PlayerStands()
			
			for (i = 0 ; i < MAX_ROOM_PLAYERS ; i++)
				if (clients[i].status == CS_ACTIVE)
					if (clients[i].info.table == table) {
						clients[i].info.table = -1;
						clients[i].info.seat = -1;
					}
			mfsREGM->messageType	= MFS_ROOM_END_GAME;
			mfsREGM->table			= table;
			mfsREGM->joinOption		= tables[table].joinOption;
			mfsREGM->watchersOption	= tables[table].watchersOption;
			mfsREGM->muteWatchers	= tables[table].muteWatchers;
			mfsREGM->ratingsFlags	= tables[table].ratingsFlags;
			mfsREGM->ratingsSkills	= tables[table].ratingsSkills;
			tables[table].GetTableGameValues(mfsREGM->gameValues);
			strcpy(mfsREGM->description, tables[table].description);
			mfsREGM->descriptionLength = strlen(mfsREGM->description) + 1;
			err = BroadcastRoomMessage((char *)mfsREGM, sizeof(MFS_ROOM_END_GAME_MESSAGE) + mfsREGM->descriptionLength);
			
			return err;
		}
		
		//If here becuase of not enough money, we don't have to continue in this function.
		if(status != TPC_REMOVED_NOTENOUGHMONEY)
		{
			// fall through to TPC_READY for the case that this player leaving means the
			// game can start
			
			playerID = -1;
			status = TPC_READY;
		}
	}
	
	//////////////////
	
	if (status == TPC_READY) {
		everyoneIsReady = TRUE;
		n = 0;
		for (i = 0 ; i < MAX_ROOM_PLAYERS ; i++)
			if (clients[i].status == CS_ACTIVE)
				if (clients[i].info.table == table && clients[i].info.seat < MAX_GAME_PLAYERS) {
					if (!(clients[i].info.flags & UF_READY_TO_PLAY))
						everyoneIsReady = FALSE;
					else
						n++;
				}
		
		if (tables[table].status == TS_PENDING) {
			if (n > 0 && (everyoneIsReady || playerID != -1)) {
				// tell everyone in the room that the player is ready and/or that the game has started
				
				mfsRSGM.messageType = MFS_ROOM_START_GAME;
				mfsRSGM.playerID = playerID;
				mfsRSGM.table = table;
				mfsRSGM.seat = seat;
				mfsRSGM.startGame = everyoneIsReady && n >= MINIMUM_PLAYERS_TO_START_GAME;
				
				err = BroadcastRoomMessage((char *)&mfsRSGM, sizeof(MFS_ROOM_START_GAME_MESSAGE));
				
				if (err)
					return err;
			}
			
			if (n >= MINIMUM_PLAYERS_TO_START_GAME && everyoneIsReady) {
				// set up the game state and send an MFS_GAME_STATE_MESSAGE to all players
				
				//Everyone's allowed anytime in a tournament room
				if(!IsTournamentRoom())
				{
					for(i = 0; i < MAX_GAME_PLAYERS; i++)
					{
						if(clients[i].status != CS_EMPTY && (clients[i].info.flags & UF_READY_TO_PLAY) && 
							clients[i].info.table == table &&
							!(tables[table].CanPlayerAffordThisTable(&clients[i])))
						{
							//They don't have enough money to stay at this table
							SendDisplayStringMessage(&clients[i], GetString(136, getS), GetString(137, getS2));
							TablePlayerChanged(&clients[i], TPC_REMOVED_NOTENOUGHMONEY);
						}

						//Once the table is empty (if it gets there), don't process anymore start-game stuff!!!
						if(tables[table].status == TS_EMPTY)
							return 0;
					}
				}

				tables[table].StartGame();
				GameStartSanityCheck(table);
				
				mfsGSM.messageType = MFS_GAME_STATE;
				mfsGSM.table = table;
				mfsGSM.status = TS_PLAYING;
				mfsGSM.gameState = tables[table].gameState;
				
				err = BroadcastGameMessage(table, (char *)&mfsGSM, sizeof(MFS_GAME_STATE_MESSAGE));
				
				if (err)
					return err;
				
				err = BroadcastGameStringMessage(table, "<SETTICKER>", tickerText);
				
				if (err)
					return err;
			}
		}
	}
	
	////////////////
	
	return 0;
}

int RoomServer::RelinquishTIPages(ROOMSERVERPLAYER *client)
{
	int i, err;
	MFS_ROOM_TI_CHECK_OUT_PAGE_MESSAGE mfs;
	
	for (i = 0 ; i < 5 ; i++)
		if (tournamentInformation.header.checkOutPlayerIDs[i] == client->info.playerID) {
			tournamentInformation.header.checkOutPlayerIDs[i] = -1;
			
			// tell everyone in the room about the check in
			
			mfs.messageType = MFS_ROOM_TI_CHECK_OUT_PAGE;
			mfs.playerID = -1;
			mfs.pageNumber = i;
			mfs.header = tournamentInformation.header;
			
			err = BroadcastRoomMessage((char *)&mfs, sizeof(MFS_ROOM_TI_CHECK_OUT_PAGE_MESSAGE));
		}
	
	return 0;
}

void RoomServer::RemovePlayersFromTables()
{
	i18nStr getS, getS2;
	int i;
	
	for (i = 0 ; i < MAX_ROOM_PLAYERS ; i++)
		if (clients[i].status == CS_ACTIVE)
			if (clients[i].info.table != -1) {
				TablePlayerChanged(&clients[i], TPC_LEFT);
				SendDisplayStringMessage(&clients[i], GetString(103, getS), GetString(124, getS2)); // "Tournament Players Only", "You have been removed from your table because only people who have joined the tournament can play now."
			}
}

int RoomServer::AddAutomaticResultsLine(int tableNumber, char *s)
{
	char buffer[MAX_MSG];
	i18nStr getS;
	int err;
	MFS_ROOM_TI_ADD_TO_TEXT_MESSAGE *mfsTEXT = (MFS_ROOM_TI_ADD_TO_TEXT_MESSAGE *)buffer;
	
	if (!tournamentInformation.header.automaticResultsEnabled)
		return 0;
	
	mfsTEXT->messageType = MFS_ROOM_TI_ADD_TO_TEXT;
	mfsTEXT->pageNumber = TI_PAGE_AUTOMATIC;
	mfsTEXT->clear = FALSE;
	
	if (tableNumber == -1)
		mfsTEXT->text[0] = 0;
	else
		sprintf(mfsTEXT->text, GetString(125, getS), tableNumber + 1); // "Table %d: "
	strnzcpy(&mfsTEXT->text[strlen(mfsTEXT->text)], s, 768);
	strcat(mfsTEXT->text, "\r\n\r\n");
	
	// add the text to the server's copy
	
	if ((strlen(mfsTEXT->text) + strlen(tournamentInformation.text[TI_PAGE_AUTOMATIC])) < MAX_TOURNAMENT_INFO)
		strcat(tournamentInformation.text[TI_PAGE_AUTOMATIC], mfsTEXT->text);
	
	// tell everyone about the added line
	
	err = BroadcastRoomMessage((char *)mfsTEXT, sizeof(MTS_ROOM_TI_ADD_TO_TEXT_MESSAGE) + strlen(mfsTEXT->text) + 1);
	
	return err;
}

int RoomServer::TournamentPlayerLeftRoom(ROOMSERVERPLAYER *client)
{
	char bank[64];
	i18nStr s, getS;
	
	if (!IS_A_CASINO_GAME)
		return 0; // not a casino room
	
	MoneyToString(client->info.tournamentBankRoll, bank);
	sprintf(s, GetString(126, getS), client->info.login, bank); // "%s left with tournament bank = %s"
	
	return AddAutomaticResultsLine(-1, s);
}

void RoomServer::AddChatToBuffer(char *pcStartOfBuffer, unsigned int uiSpaceLeftInBuffer)
{
	char line[512];
	i18nStr getS;
	int i, n;

	// add on room-chat text
	strcpy(line, GetString(95, getS));
	if (uiSpaceLeftInBuffer >= strlen(line)) {
		strcat(pcStartOfBuffer, line);
		uiSpaceLeftInBuffer -= strlen(line);
	}
	
	i = chatHistoryTop - 1;
	if (i == -1)
		i = CHAT_HISTORY_LENGTH - 1;
	
	for (n = 0 ; n < CHAT_HISTORY_LENGTH ; n++) {
		if (chatHistory[i].text[0] == 0)
			continue;
		
		if(chatHistory[i].isPrivate)
			sprintf(line, "%d: (private from:)%s\n", (int)(CHAT_HISTORY_LENGTH - n), chatHistory[i].text);
		else
			sprintf(line, "%d: %s\n", (int)(CHAT_HISTORY_LENGTH - n), chatHistory[i].text);
		
		if (uiSpaceLeftInBuffer <= strlen(line))
			return;
		
		strcat(pcStartOfBuffer, line);
		uiSpaceLeftInBuffer -= strlen(line);
		
		i = i - 1;
		if (i == -1)
			i = CHAT_HISTORY_LENGTH - 1;
	}
}

///////////

int RoomServer::BroadcastRoomMessage(char *message, int length)
{	
	int i, n, err;
	HCONNECTION recipients[MAX_ROOM_PLAYERS];
	char messageDescription[512];
	
	if (numClients == 0)
		return 0;
	
	n = 0;
	
	strcpy(messageDescription, GetMessageDescription(message));
	TRACE(_ROUTINE_ "Sending %s to all active players.\n", messageDescription);

	for (i = 0 ; i < MAX_ROOM_PLAYERS ; i++)
		if (clients[i].status == CS_ACTIVE)
		{
			recipients[n++] = clients[i].connection;
		}
	
	if (n == 0)
		return 0;
	
	err = connections->BroadcastMessage(recipients, n, message, length);
	if (err)
		TRACE(_WARN_ "RoomServer::BroadcaseRoomMessage(): connections->BroadcastMessage() returned error %d", err);
	
	return err;
}

int RoomServer::BroadcastGameMessage(int table, char *message, int length)
{	
	char messageDescription[512];
	int i, n, playerIndex, err;
	HCONNECTION recipients[MAX_ROOM_PLAYERS];
	ROOMSERVERPLAYER *client;
	
	n = 0;
	
	strcpy(messageDescription, GetMessageDescription(message));
	
	TRACE(_ROUTINE_ "Sending to player(s) at table %d: %s.\n", table, messageDescription);
	
	for (i = 0 ; i < MAX_GAME_PLAYERS ; i++)
		if (IsHumanPlayer(table, i)) 
		{
			playerIndex = tables[table].playerIndicies[i];

			if (playerIndex < 0 || playerIndex >= MAX_ROOM_PLAYERS)
				{ ASSERT(_WARN_, FALSE); continue; }

			client = &clients[playerIndex];

			if (client->status != CS_ACTIVE)
			{ 
				TRACE(_WARN_ "client status was not CS_ACTIVE (%d)\n", client->status);
				continue;
			}

			if (client->info.table != table)
			{
				ASSERT(_WARN_, FALSE); continue;
			}

			if (client->info.seat != i)
				{ ASSERT(_WARN_, FALSE); continue; }

			recipients[n++] = client->connection;
//			TRACE(_ROUTINE_ "Sending to player %s (playerID = %d) [table %d]: %s.\n", client->info.login, client->info.playerID, table, messageDescription);
		}
	
	for (i = 0 ; i < MAX_GAME_WATCHERS ; i++)
		if (tables[table].gameState.watcherIDs[i] != PLAYER_ID_EMPTY) {
			playerIndex = tables[table].watcherIndicies[i];
			if (playerIndex < 0 || playerIndex >= MAX_ROOM_PLAYERS)
				{ ASSERT(_WARN_, FALSE); continue; }
			client = &clients[playerIndex];
			if (client->status != CS_ACTIVE)
				{ ASSERT(_WARN_, FALSE); continue; }
			if (client->info.table != table)
				{ ASSERT(_WARN_, FALSE); continue; }
			if (client->info.seat != (i + MAX_GAME_PLAYERS))
				{ ASSERT(_WARN_, FALSE); continue; }

			recipients[n++] = client->connection;
//			TRACE(_ROUTINE_ "Sending to watcher %s (playerID = %d) [table %d]: %s.\n", client->info.login, client->info.playerID, table, messageDescription);
		}
	
	if (n == 0)
		return 0;
	
	err = connections->BroadcastMessage(recipients, n, message, length);
	if (err)
		TRACE(_WARN_ "RoomServer::BroadcaseGameMessage(): connections->BroadcastMessage() returned error %d", err);
	
	return err;
}

int RoomServer::BroadcastGameStringMessage(int table, char *title, char *text)
{
	char buffer[MAX_MSG];
	MFS_GAME_DISPLAY_STRING_MESSAGE *mfsGDSM = (MFS_GAME_DISPLAY_STRING_MESSAGE *)buffer;
	
	mfsGDSM->messageType = MFS_GAME_DISPLAY_STRING;
	mfsGDSM->table = table;
	strcpy(mfsGDSM->title, title);
	mfsGDSM->textLength = strlen(text) + 1;
	strcpy(mfsGDSM->text, text);
	
	return BroadcastGameMessage(table, (char *)mfsGDSM, sizeof(MFS_GAME_DISPLAY_STRING_MESSAGE) + mfsGDSM->textLength);
}

int RoomServer::SendDisplayStringMessage(ROOMSERVERPLAYER *client, char *title, char *text, Boolean gameMessage /* = FALSE */)
{
	char buffer[MAX_MSG];
	int err;
	MFS_ROOM_DISPLAY_STRING_MESSAGE *mfsRDSM = (MFS_ROOM_DISPLAY_STRING_MESSAGE *)buffer;
	MFS_GAME_DISPLAY_STRING_MESSAGE *mfsGDSM = (MFS_GAME_DISPLAY_STRING_MESSAGE *)buffer;
	
	if (gameMessage) {
		mfsGDSM->messageType = MFS_GAME_DISPLAY_STRING;
		mfsGDSM->table = client->info.table;
		strcpy(mfsGDSM->title, title);
		mfsGDSM->textLength = strlen(text) + 1;
		strcpy(mfsGDSM->text, text);
		
		TRACE(_ROUTINE_ "Sending to %s (playerID = %d): %s.\n", client->info.login, client->info.playerID, GetMessageDescription((char *)mfsGDSM));
		err = connections->PostMessage(client->connection, (char *)mfsGDSM, sizeof(MFS_GAME_DISPLAY_STRING_MESSAGE) + mfsGDSM->textLength);
	}
	else {
		mfsRDSM->messageType = MFS_ROOM_DISPLAY_STRING;
		strcpy(mfsRDSM->title, title);
		mfsRDSM->textLength = strlen(text) + 1;
		strcpy(mfsRDSM->text, text);
		
		TRACE(_ROUTINE_ "Sending to %s (playerID = %d): %s.\n", client->info.login, client->info.playerID, GetMessageDescription((char *)mfsRDSM));
		
		err = connections->PostMessage(client->connection, (char *)mfsRDSM, sizeof(MFS_ROOM_DISPLAY_STRING_MESSAGE) + mfsRDSM->textLength);
	}
	
	if (err) {
		TRACE(_WARN_ "RoomServer::SendDisplayStringMessage(): connections->PostMessage() returned error %d\n", err);

		return err;
	}

	return 0;
}

int RoomServer::GenerateNextPlayerID()
{
	int i;
	
	for ( ; ; ) {
		nextPlayerID++;
		if (nextPlayerID > 2000000000)
			nextPlayerID = 1;
		
		for (i = 0 ; i < MAX_ROOM_PLAYERS ; i++)
			if (clients[i].status != CS_EMPTY)
				if (clients[i].info.playerID == nextPlayerID)	// BCHECK - info is unitialized memory
					break;
		
		if (i == MAX_ROOM_PLAYERS)
			return nextPlayerID;
	}
}

ROOMSERVERPLAYER *RoomServer::GetClientFromHCONNECTION(HCONNECTION c)
{
	return (ROOMSERVERPLAYER *)connections->GetConnectionData(c);
	
	/*
	int i;
	
	for (i = 0 ; i < MAX_ROOM_PLAYERS ; i++)
		if (clients[i].status != CS_EMPTY && clients[i].status != CS_LEAVING)
			if (clients[i].connection == c)
				return &clients[i];
	
	return NULL;
	*/
}

ROOMSERVERPLAYER *RoomServer::GetClientFromID(int id)
{
	int i;
	
	// TODO: if the clients are indexed by ID, we can do a binary search here
	
	for (i = 0 ; i < MAX_ROOM_PLAYERS ; i++)
		if (clients[i].status != CS_EMPTY)
			if (clients[i].info.playerID == id)
				return &clients[i];
	
	return NULL;
}

ROOMSERVERPLAYER *RoomServer::GetClientFromName(char *name)
{
	int i;
	
	// TODO: if the clients are indexed by name, we can do a binary search here
	
	for (i = 0 ; i < MAX_ROOM_PLAYERS ; i++)
		if (clients[i].status != CS_EMPTY)
			if (!stricmp(clients[i].info.login, name))
				return &clients[i];
	
	return NULL;
}

void RoomServer::UpdateNumClients()
{
	int i;
	
	// NOTE: ideally, we should not have to recount the number of non-empty clients every time
	// the number of clients changes; but there were bugs (having to do with the database
	// connection going down) where the numClients was going negative; so we put in this
	// function, which is only called when players join and leave; so the cost isn't very
	// high anyway
	
	numClients = 0;
	
	for (i = 0 ; i < MAX_ROOM_PLAYERS ; i++)
		if (clients[i].status != CS_EMPTY)
			numClients++;
}

int RoomServer::CountActivePlayers()
{
	int i, n;
	
	// TODO: we could keep this count statically
	
	n = 0;
	for (i = 0 ; i < MAX_ROOM_PLAYERS ; i++)
		if (clients[i].status == CS_ACTIVE)
			n++;
	
	return n;
}

int RoomServer::CountPlayersAtTable(int table, Boolean includeWatchers)
{
	int i, n;
	
	// TODO: we could keep this count statically
	
	n = 0;
	
	for (i = 0 ; i < MAX_ROOM_PLAYERS ; i++)
		if (clients[i].status == CS_ACTIVE)
			if (clients[i].info.table == table) {
				if (includeWatchers)
					n++;
				else
					if (clients[i].info.seat < MAX_GAME_PLAYERS)
						n++;
			}
	
	return n;
}

int RoomServer::CountPlayersAtTableFast(int table, Boolean includeWatchers)
{
	int i, n;
	
	n = 0;
	
	for (i = 0 ; i < MAX_GAME_PLAYERS ; i++)
		if (IsHumanPlayer(table, i))
			n++;
	
	if (includeWatchers)
		for (i = 0 ; i < MAX_GAME_WATCHERS ; i++)
			if (tables[table].gameState.watcherIDs[i] != PLAYER_ID_EMPTY)
				n++;
	
	return n;
}

Boolean RoomServer::IsHumanPlayer(int table, int seat)
{
	int playerID;

	if (seat >= MAX_GAME_PLAYERS) 
		{ ASSERT(_WARN_, FALSE); return FALSE; }

	playerID = tables[table].gameState.players[seat].playerID;
	
	return (playerID != PLAYER_ID_EMPTY && playerID != PLAYER_ID_AI && playerID != PLAYER_ID_VOID);
}

void RoomServer::LogMachineId(char *heading, char *login, SNMachineID &playerMachineID)
{
	char msg[256], s[3];
	ULONG i;
	const unsigned char *midptr = playerMachineID.GetMachineIDPtr();
	
	msg[0] = 0;
	for (i = 0 ; i < SizeOfPC1 ; i++) {
		sprintf(s, "%02X", midptr[i]);
		strcat(msg, s);
	}
	
#ifndef WIN32
	TRACE(_WARNB_ "%s login = %s MachineID = %s\n", heading, login, msg);
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
	
void RoomServer::DoDatabaseWork(Boolean init)
{
	int i, j, r, err;
	// MFS_ROOM_DISPLAY_STRING_MESSAGE *mfsRDSM = (MFS_ROOM_DISPLAY_STRING_MESSAGE *)buffer;
	MTDB_RS_CONNECT_MESSAGE mfsRCM;
	DWORD theTickCount;

	if (init) {
		lastDatabaseConnectAttempt = 0;
		lastDatabaseWriteTickCount = 0;
		lastDatabaseWarning = 0;
		databaseTries = 0;
		
		return;
	}

	// no need to call this multiple times within same method...mds 9/18/99
	theTickCount = GetTickCount();

	// no need to do any database work if we don't have any non-empty clients.  Furthermore, don't
	//  even bother to reconnect to the database if this is true.
	if(0 == numClients)
		return;
	
	// if we are not connected to the database, try to connect to it
		// but if we have tried to connect to it too many times, clear out all leaving players,
		// disconnect all joining players,
		// send warnings to all active players, and reset databaseTries
	
	if (dbConnection == NULL && !done) {
		// don't reconnect to the database too quickly or we might not have a fair
		// chance at connecting
		
		if ((theTickCount - lastDatabaseConnectAttempt) < DATABASE_RECONNECT_TICKS)
			return;
		
		lastDatabaseConnectAttempt = theTickCount;
		
		if (databaseTries == MAX_DATABASE_TRIES) {
			for (i = 0 ; i < MAX_ROOM_PLAYERS ; i++) {
				if (clients[i].status == CS_LEAVING) {
					clients[i].status = CS_EMPTY;
					dbRecords[i].login[0] = 0;
					UpdateNumClients();
					UpdateDisplay();
				}
				if (clients[i].status == CS_JOINING) {
					connections->MyFlushAndClose(clients[i].connection);
					clients[i].status = CS_EMPTY;
					dbRecords[i].login[0] = 0;
					UpdateNumClients();
					UpdateDisplay();
				}
			}
			
			if ((theTickCount - lastDatabaseWarning) >= DATABASE_WARNING_TICKS) {
				/*
				mfsRDSM->messageType = MFS_ROOM_DISPLAY_STRING;
				strcpy(mfsRDSM->title, GetString(127, getS)); // "Database Error"
				strcpy(mfsRDSM->text, GetString(128, getS)); // "Changes to your record during this game session may be lost."
				mfsRDSM->textLength = strlen(mfsRDSM->text) + 1;
				BroadcastRoomMessage((char *)mfsRDSM, sizeof(MFS_ROOM_DISPLAY_STRING_MESSAGE) + mfsRDSM->textLength);
				*/
				TRACE(_WARN_ "RoomServer::DoDatabaseWork(): DATABASE IS DOWN\n");
				lastDatabaseWarning = theTickCount;
			}
			
			databaseTries = 0;
		}
		
		r = randomGenerator->GetRandom() % 3;
		dbConnection = connections->RequestConnection(proxyNames[r], proxyPorts[r], CONNECTIONID_DBCONNECTOR);
		databaseTries++;
		
		return;
	}
	
	// if we are trying to connect to the database, wait until the result comes in
	
	if (connections->GetConnectionStatus(dbConnection) != CS_CONNECTED)
		return;
	
	databaseTries = 0;
	
	dbConnection->SetHeartbeat(FALSE);
	
	// we are connected:
		// request data for entering players as apporpriate
		// send writes for leaving players as apporpriate
		// if the timer is up, send writes for dirty data
	
	for (i = 0 ; i < MAX_ROOM_PLAYERS ; i++) 
	{
		if (clients[i].status == CS_JOINING && clients[i].dirtyFlags[0] == DF_ENTERED)
		{
			if (clients[i].index != i)
			{
				TRACE(_WARN_ "RoomServer::DoDatabaseWork(): bad client index\n");
				clients[i].index = i;
			}

			mfsRCM.messageType = MTDB_RS_CONNECT;
			mfsRCM.playerID = clients[i].databaseID;
			mfsRCM.magicIndex = i;
			mfsRCM.type = clients[i].info.level;
			mfsRCM.roomID = roomID;
			strcpy(mfsRCM.gameName, gameName);
			strcpy(mfsRCM.roomName, roomName);
			mfsRCM.userCount = numClients;

			TRACE(_ROUTINE_ "Sending to DATABASE: %s.\n", GetMessageDescription((char *)&mfsRCM));

			err = connections->PostMessage(dbConnection, (char *)&mfsRCM, sizeof(MTDB_RS_CONNECT_MESSAGE));
			if (err == 0)
				for (j = 0 ; j < NUM_DIRTY_FLAGS ; j++)
					clients[i].dirtyFlags[j] = DF_REQUESTED;
			else
				TRACE(_WARN_ "RoomServer::DoDatabaseWork(): PostMessage(MTDB_RS_CONNECT_MESSAGE) returned error %d", err);
		}
		if (clients[i].status == CS_LEAVING)
			SendDirtyData(&clients[i]);
	}
	
	if ((theTickCount - lastDatabaseWriteTickCount) < DATABASE_TICKS)
		return;
	
	lastDatabaseWriteTickCount = theTickCount;
	
	for (i = 0 ; i < MAX_ROOM_PLAYERS ; i++)
		if (clients[i].status == CS_ACTIVE)
			SendDirtyData(&clients[i]);
	
	ReadTickerText();
}

//======================================================================
//
//======================================================================
void RoomServer::SendDirtyData(ROOMSERVERPLAYER *client)
{
	char *m, buffer[MAX_CM_MSG];
	int i, size, err;
	MTDB_RS_SET_SHADOW_MESSAGE		mfsRSSM;
	MTDB_RS_SET_BANK_MESSAGE		mfsRSBM;
	MTDB_RS_SET_PROFILE_MESSAGE		*mfsRSPM;
	MTDB_RS_SET_GAME_RECORD_MESSAGE *mfsRSGRM;
	MTDB_RS_DISCONNECT_MESSAGE		*mfsRDM;
	
	if (!CheckDatabaseRecord(client)) 
	{
		TRACE(_WARN_ "Invalid database ID\n");
		
		if (client->status == CS_LEAVING) 
		{
			TRACE(_WARN_ "Invalid database ID: disconnecting leaver\n");
			client->status = CS_EMPTY;
			dbRecords[client->index].login[0] = 0;
			UpdateNumClients();
			UpdateDisplay();
		}
		
		return;
	}
	
	if (client->status == CS_LEAVING) 
	{
		if (client->dirtyFlags[DFI_LEVEL] == DF_DIRTY) 
		{
			// send PROFILE here; if the PROFILE message gets lost with the DISCONNECT message,
			// both will be resent; if somehow the PROFILE message gets lost but the DISCONNECT
			// message is processed without error, oh well
			
			mfsRSPM = (MTDB_RS_SET_PROFILE_MESSAGE *)buffer;

			mfsRSPM->messageType	= MTDB_RS_SET_PROFILE;
			mfsRSPM->magicIndex		= client->index;
			mfsRSPM->playerID		= client->databaseID;
			mfsRSPM->profileLength	= strlen(client->profile) + 1;

			strnzcpy(mfsRSPM->profile, client->profile, MAX_PLAYER_PROFILE - 1);

			TRACE(_ROUTINE_ "Sending to DATABASE: %s.\n", GetMessageDescription((char *)mfsRSPM));

			err = connections->PostMessage(dbConnection, (char *)mfsRSPM, sizeof(MTDB_RS_SET_PROFILE_MESSAGE) + mfsRSPM->profileLength);
			
			mfsRDM = (MTDB_RS_DISCONNECT_MESSAGE *)buffer;

			// TODO: distinguish between active and passive logouts
			mfsRDM->active		= TRUE;
			mfsRDM->messageType = MTDB_RS_DISCONNECT;
			mfsRDM->playerID	= client->databaseID;
			mfsRDM->magicIndex	= client->index;
			mfsRDM->roomID		= roomID;
			mfsRDM->userCount	= numClients;
			mfsRDM->shadow		= client->info.shadow;
			mfsRDM->bank		= client->info.bankRoll;

			strnzcpy(mfsRDM->gameName, gameName, MAX_GAME_NAME - 1);
			strnzcpy(mfsRDM->roomName, roomName, MAX_ROOM_NAME - 1);
			
			// bank values should _NEVER_ be less than 0
			if (client->info.bankRoll < 0)
				TRACE(_WARN_ "RoomServer::SendDirtyData(): Leaving player [name = %s, id = %d] bankroll is corrupted [%d], %s line: %d\n", 
					client->info.login,
					client->databaseID,
					client->info.bankRoll,
					__FILE__,
					__LINE__
				);

			strcpy(mfsRDM->gameRecord, client->gameRecord);
			mfsRDM->grLength = strlen(mfsRDM->gameRecord) + 1;
//			FillGameRecord(client, &mfsRDM->grLength, mfsRDM->gameRecord);
			
			if(atoi(&mfsRDM->gameRecord[4]) != client->info.wins)
				TRACE(_WARN_ "atoi(&mfsRDM->gameRecord[4]) (%d) != client->info.wins (%d)\n", atoi(&mfsRDM->gameRecord[4]), client->info.wins);

			TRACE(_ROUTINE_ "Sending to DATABASE: %s.\n", GetMessageDescription((char *)mfsRDM));

			err = connections->PostMessage(dbConnection, (char *)mfsRDM, sizeof(MTDB_RS_DISCONNECT_MESSAGE) + mfsRDM->grLength);
			if (err == 0)
				for (i = 0 ; i < NUM_DIRTY_FLAGS ; i++)
					client->dirtyFlags[i] = DF_SENDING;
			else
				TRACE(_WARN_ "RoomServer::SendDirtyData(): PostMessage() returned error %d\n", err);
		}
		
		return;
	}
	
	// client state is implied as CS_ACTIVE though we don't test for it...
	for (i = 0 ; i < NUM_DIRTY_FLAGS ; i++)
		if (client->dirtyFlags[i] == DF_DIRTY) 
		{
			switch (i) 
			{
			case DFI_LEVEL:
				// do nothing!
				continue;
				break;

			case DFI_SHADOW:
				mfsRSSM.messageType = MTDB_RS_SET_SHADOW;
				mfsRSSM.playerID = client->databaseID;
				mfsRSSM.magicIndex = client->index;
				mfsRSSM.shadow = client->info.shadow;
				m = (char *)&mfsRSSM;
				size = sizeof(MTDB_RS_SET_SHADOW_MESSAGE);
				break;

			case DFI_BANKROLL:
				mfsRSBM.messageType		= MTDB_RS_SET_BANK;
				mfsRSBM.playerID		= client->databaseID;
				mfsRSBM.magicIndex		= client->index;
				mfsRSBM.bank			= client->info.bankRoll;
				m = (char *)&mfsRSBM;
				size = sizeof(MTDB_RS_SET_BANK_MESSAGE);
				
				if (client->info.bankRoll < 0)
					TRACE(_WARN_ "RoomServer::SendDirtyData(): player [name = %s, id = %d] bankroll is corrupted [%d], %s line: %d\n", 
						client->info.login,
						client->databaseID,
						client->info.bankRoll,
						__FILE__,
						__LINE__
					);
				break;

			case DFI_PROFILE:
				mfsRSPM = (MTDB_RS_SET_PROFILE_MESSAGE *)buffer;
				mfsRSPM->messageType = MTDB_RS_SET_PROFILE;
				mfsRSPM->playerID = client->databaseID;
				mfsRSPM->magicIndex = client->index;
				mfsRSPM->profileLength = strlen(client->profile) + 1;
				strnzcpy(mfsRSPM->profile, client->profile, MAX_PLAYER_PROFILE - 1);
				m = (char *)mfsRSPM;
				size = sizeof(MTDB_RS_SET_PROFILE_MESSAGE) + mfsRSPM->profileLength;
				break;

			case DFI_GAMERECORD:
				mfsRSGRM = (MTDB_RS_SET_GAME_RECORD_MESSAGE *)buffer;
				mfsRSGRM->messageType = MTDB_RS_SET_GAME_RECORD;
				mfsRSGRM->playerID = client->databaseID;
				mfsRSGRM->magicIndex = client->index;
				strcpy(mfsRSGRM->gameName, gameName);
				strcpy(mfsRSGRM->gameRecord, client->gameRecord);
				mfsRSGRM->grLength = strlen(mfsRSGRM->gameRecord) + 1;
	
				if(atoi(&mfsRSGRM->gameRecord[4]) != client->info.wins)
					TRACE(_WARN_ "atoi(&mfsRSGRM->gameRecord[4]) (%d) != client->info.wins (%d)\n", atoi(&mfsRSGRM->gameRecord[4]), client->info.wins);

//					FillGameRecord(client, &mfsRSGRM->grLength, mfsRSGRM->gameRecord);
				m = (char *)mfsRSGRM;
				size = sizeof(MTDB_RS_SET_GAME_RECORD_MESSAGE) + mfsRSGRM->grLength;
				break;

			default:
				ASSERT(_WARN_, FALSE);
				continue;
			}

			TRACE(_ROUTINE_ "Sending to DATABASE: %s.\n", GetMessageDescription(m));

			err = connections->PostMessage(dbConnection, m, size);
			if (err == 0)
				client->dirtyFlags[i] = DF_SENDING;
			else
				TRACE(_WARN_ "RoomServer::SendDirtyData(): PostMessage(%d) returned error %d", i, err);
		}
}

void RoomServer::MakeDataDirty(ROOMSERVERPLAYER *client, int dirtyFlagIndex)
{
	switch (client->dirtyFlags[dirtyFlagIndex]) 
	{
	case DF_CLEAN: client->dirtyFlags[dirtyFlagIndex] = DF_DIRTY; break;
	case DF_DIRTY: break;
	case DF_SENDING: client->dirtyFlags[dirtyFlagIndex] = DF_DIRTY_WHILE_SENDING; break;
	case DF_DIRTY_WHILE_SENDING: break;
	default:
		ASSERT(_WARN_, FALSE);
		break;
	}
}

int RoomServer::ReportDatabaseError(ROOMSERVERPLAYER *client)
{
	i18nStr getS, getS2;
	int err = 0;
	
	TRACE(_ROUTINE_ "RoomServer::ReportDatabaseError() [status = %d]\n", (int)client->status);
	
	if (client->status == CS_JOINING)
		err = SendDisplayStringMessage(client, GetString(127, getS), GetString(129, getS2)); // "Database Error", "Unable to connect to server.  Please try again in a few minutes."
	else
		if (client->status != CS_LEAVING)
			err = SendDisplayStringMessage(client, GetString(127, getS), GetString(130, getS2)); // "Database Error", "Due to a database error, changes to information about your player in the last few minutes may have been lost."
	
	if (client->status == CS_JOINING)
		connections->MyFlushAndClose(client->connection);
	
	return err;
}

void RoomServer::AddDatabaseRecord(ROOMSERVERPLAYER *client)
{
	// make sure we're not overwriting someone else's database record
	ASSERT(_WARN_, dbRecords[client->index].login[0] == 0);
	
	strcpy(dbRecords[client->index].login, client->info.login);
	dbRecords[client->index].databaseID = client->databaseID;
}

bool RoomServer::CheckDatabaseRecord(ROOMSERVERPLAYER *client)
{
	int i;
	
	i = client->index;
	
	if (!strcmp(dbRecords[i].login, client->info.login)) {
		if (dbRecords[i].databaseID != client->databaseID) {
			TRACE(_WARN_ "For client %s: dbRecords[%d].databaseID (%d) != client->databaseID (%d).\n",
						  client->info.login, i, dbRecords[i].databaseID, client->databaseID);
			
			return false;
		}
	}
	else {
		TRACE(_WARN_ "No match for client %s in database ID table.\n", client->info.login);
		
		return false;
	}
	
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void RoomServerTable::Init(int theIndex)
{
	index = theIndex;
	status = TS_EMPTY;
	joinOption = TJO_OPEN;
	watchersOption = TWO_OPEN;
	muteWatchers = FALSE;
	description[0] = 0;

	ratingsFlags = roomServer->IsRatedRoom() ? RF_TABLE_RATED : 0;
	ratingsSkills = 0;
}

void RoomServerTable::CommonEndOfGame()
{
	//Update game starts.
	roomServer->gameStarts++;

	//Do game-specific end of game stuff, as well as ratings.
	EndOfGame();
}

Boolean RoomServerTable::PlayersRatingMatchesTable(short sRating)
{
	short saaRatingCatagories[NUM_RATING_CATAGORIES][2] = {{0, 1400}, {1401, 1600}, {1601, 1800}, {1801, 2000}, {2001, 2300}, {2300, 15000}};
	int i;

	for(i = 0; i < NUM_RATING_CATAGORIES; i++)
	{
		if(ratingsSkills & (1 << i))
		{
			if(sRating >= saaRatingCatagories[i][0] && sRating <= saaRatingCatagories[i][1])
				return TRUE;
		}
	}

	return FALSE;
}

void RoomServerTable::UpdateRatings(int winningSeat, int iNumPlayers, char resultOfGame, char reasonForUpdate,
									Boolean bPartnersGame)
{
	int iWins[MAX_GAME_PLAYERS], iLosses[MAX_GAME_PLAYERS], iDraws[MAX_GAME_PLAYERS];
	int iPartials[MAX_GAME_PLAYERS], iRating[MAX_GAME_PLAYERS], iRatingDev[MAX_GAME_PLAYERS];
	int iOriginalAIStats[6]; //wins, losses, draws, partials, rating, rating dev.
	int i, iRet;
	bool bAIRatingChanged = FALSE;
	MFS_ROOM_PLAYER_RATING_MESSAGE mfsRPR;

	// If we somehow get here, but the game really wasn't rated, don't update anyones ratings!!!
	if(!roomServer->IsRatedGame(index))
		return;

	roomServer->SetRatingForAI(&iOriginalAIStats[0], &iOriginalAIStats[1], &iOriginalAIStats[2], &iOriginalAIStats[3], &iOriginalAIStats[4], &iOriginalAIStats[5]);

	for(i = 0; i < MAX_GAME_PLAYERS; i++)
	{
		if(-1 == playerIndicies[i])
		{
			if(!roomServer->IsHumanPlayer(index, i))
				roomServer->SetRatingForAI(&iWins[i], &iLosses[i], &iDraws[i], &iPartials[i], &iRating[i],
				&iRatingDev[i]);

			continue;
		}

		iRet = roomServer->ParseGameRecord(&roomServer->clients[playerIndicies[i]], &iWins[i], &iLosses[i],
			&iDraws[i], &iPartials[i], &iRating[i], &iRatingDev[i]);
		if(iRet == 0 || iRet == EOF)
			roomServer->InitRatings(&roomServer->clients[playerIndicies[i]], &iWins[i], &iLosses[i], 
			&iDraws[i], &iPartials[i], &iRating[i], &iRatingDev[i]);
	}

	// If the result is a quit, just update the winning seat (the person quitting)
	if(reasonForUpdate == RRU_LEFTTABLE)
	{
		iPartials[winningSeat]++;
	}
	else
	{
		//determine each player's new rating
		if(bPartnersGame)
			roomServer->CalculateNewPartnersRating(winningSeat, iNumPlayers, resultOfGame, reasonForUpdate,
			iWins, iLosses, iDraws, iPartials, iRating, iRatingDev, &gameState);
		else
			roomServer->CalculateNewSinglesRating(winningSeat, iNumPlayers, resultOfGame, reasonForUpdate,
			iWins, iLosses, iDraws, iPartials, iRating, iRatingDev, &gameState);
	}

	//Finally, write out each player's info.
	for(i = 0; i < MAX_GAME_PLAYERS; i++)
	{
		if(-1 == playerIndicies[i])
		{
			if(!roomServer->IsHumanPlayer(index, i))
			{
				if(iOriginalAIStats[0] != iWins[i])
					iRet = RRG_WIN;
				else if (iOriginalAIStats[1] != iLosses[i])
					iRet = RRG_LOSS;
				else if (iOriginalAIStats[2] != iDraws[i])
					iRet = RRG_DRAW;
				else
					iRet = RRG_QUIT;

				roomServer->UpdateRatingForAI(iRet, iRating[i] - iOriginalAIStats[4], iRatingDev[i] - iOriginalAIStats[5]);

				bAIRatingChanged = TRUE;
			}

			continue;
		}

		roomServer->UpdateGameRecord(&roomServer->clients[playerIndicies[i]], iWins[i], iLosses[i], iDraws[i],
			iPartials[i], iRating[i], iRatingDev[i]);
		
		mfsRPR.messageType	= MFS_ROOM_PLAYER_RATING;
		mfsRPR.playerID		= roomServer->clients[playerIndicies[i]].info.playerID;
		mfsRPR.wins			= iWins[i];
		mfsRPR.losses		= iLosses[i];
		mfsRPR.draws		= iDraws[i];
		mfsRPR.quits		= iPartials[i];
		mfsRPR.rating		= iRating[i];
		mfsRPR.ratingDev	= iRatingDev[i];
		
		roomServer->BroadcastRoomMessage((char *)&mfsRPR, sizeof(MFS_ROOM_PLAYER_RATING_MESSAGE));
	}

	if(bAIRatingChanged)
	{
		mfsRPR.messageType	= MFS_ROOM_PLAYER_RATING;
		mfsRPR.playerID		= -1;
		mfsRPR.wins			= roomServer->m_iAIRatingWins;
		mfsRPR.losses		= roomServer->m_iAIRatingLosses;
		mfsRPR.draws		= roomServer->m_iAIRatingDraws;
		mfsRPR.quits		= roomServer->m_iAIRatingPartials;
		mfsRPR.rating		= roomServer->m_iAIRating;
		mfsRPR.ratingDev	= roomServer->m_iAIRatingDev;
		
		roomServer->BroadcastRoomMessage((char *)&mfsRPR, sizeof(MFS_ROOM_PLAYER_RATING_MESSAGE));
	}
	return;
}

void RoomServer::CalculateNewSinglesRating(int winningSeat, int iNumPlayers, char resultOfGame, char reasonForUpdate,
	int iWins[], int iLosses[], int iDraws[], int iPartials[], int iRatings[], int iRatingDevs[], const GameState * gameState)
{
	float WeWinner, WeLoser, K;
	float multiplier;
	float fDiffRatings;
	int averageOpponentsRating;
	int i;

	if (iNumPlayers < 2)
		return;

	averageOpponentsRating = 0;
	i = (winningSeat + 1) % MAX_GAME_PLAYERS;
	while(i != winningSeat)
	{
		averageOpponentsRating += iRatings[i];
		i++;
		i = i % MAX_GAME_PLAYERS;
	}

	//average rating of losers
	averageOpponentsRating /= (iNumPlayers - 1);

	// rating of winner - rating of loser
	fDiffRatings = iRatings[winningSeat] - averageOpponentsRating;

	WeWinner = 1.0 / (pow(10.0, -fDiffRatings/400.0) + 1.0);
	WeLoser = 1.0 - WeWinner;

	for(i = 0; i < MAX_GAME_PLAYERS; i++)
	{
		if(iRatings[i] > 2400)
			K = 20.0;
		else if(iWins[i] + iLosses[i] + iDraws[i] > 30)
			K = 30.0;
		else
			K = 50.0;

		if(i == winningSeat)
		{
			if(resultOfGame == RRG_WIN)
			{
				multiplier = 1.0 - WeWinner;
				iWins[i]++;
			}
			else if(resultOfGame == RRG_DRAW)
			{
				multiplier = 0.5 - WeWinner;
				iDraws[i]++;
			}
			else
			{
				// The player in the winners seat actually is a loser (moderator-forced)
				multiplier = 0.0 - WeWinner;
				iLosses[i]++;
			}
		}
		else
		{
			if(resultOfGame == RRG_WIN) {	//This means you are the loser
				multiplier = 0.0 - WeLoser;
				iLosses[i]++;
			}
			else if(resultOfGame == RRG_DRAW) {
				multiplier = 0.5 - WeLoser;
				iDraws[i]++;
			}
			else
			{
				// The player in the winners seat actually is a loser (moderator-forced),
				//  so everyone else shouldn't change.
				multiplier = 1.0 - WeLoser;
				K = 0;
			}
		}

		iRatings[i] += (int)(K * (multiplier));

		if (iPartials[i] && 0 == ((iWins[i] + iLosses[i] + iDraws[i]) % 10) && iPartials[i])
			iPartials[i]--;
	}
}

void RoomServer::CalculateNewPartnersRating(int winningSeat, int iNumPlayers, char resultOfGame, char reasonForUpdate, 
	int iWins[], int iLosses[], int iDraws[], int iPartials[], int iRatings[], int iRatingDevs[], const GameState * gameState)
{
	float WeWinner, WeLoser, K;
	int iTeamWRat, iTeamLRat;
	int i;
	float multiplier;
	float fDiffRatings;
	int iWinners[MAX_GAME_PLAYERS];
	int iLosers[MAX_GAME_PLAYERS];

	for (i = 0; i < MAX_GAME_PLAYERS / 2; i++)
	{
		iWinners[i] = (winningSeat + (i * 2)) % MAX_GAME_PLAYERS;
	}

	for (i = 0; i < MAX_GAME_PLAYERS / 2; i++)
	{
		iLosers[i] = ((winningSeat + 1) + (i * 2)) % MAX_GAME_PLAYERS;
	}

	iTeamWRat = (iRatings[iWinners[0]] + iRatings[iWinners[1]]) / 2;
	iTeamLRat = (iRatings[iLosers[0]] + iRatings[iLosers[1]]) / 2;

	// rating of winner - rating of loser
	fDiffRatings = iTeamWRat - iTeamLRat;

	WeWinner = 1.0 / (pow(10.0, -fDiffRatings/400.0) + 1.0);
	WeLoser = 1.0 - WeWinner;

	for(i = 0; i < MAX_GAME_PLAYERS; i++)
	{
		if(iRatings[i] > 2400)
			K = 20.0;
		else if(iWins[i] + iLosses[i] + iDraws[i] > 30)
			K = 30.0;
		else
			K = 50.0;

		if(i == winningSeat || i == (winningSeat + 2))
		{
			if(resultOfGame == RRG_WIN)
			{
				multiplier = 1.0 - WeWinner;
				iWins[i]++;
			}
			else if(resultOfGame == RRG_DRAW)
			{
				multiplier = 0.5 - WeWinner;
				iDraws[i]++;
			}
			else
			{
				// The player in the winners seat actually is a loser (moderator-forced)
				multiplier = 0.0 - WeWinner;
				iLosses[i]++;
			}
		}
		else
		{
			if(resultOfGame == RRG_WIN) {	//This means you are the loser
				multiplier = 0.0 - WeLoser;
				iLosses[i]++;
			}
			else if(resultOfGame == RRG_DRAW) {
				multiplier = 0.5 - WeLoser;
				iDraws[i]++;
			}
			else
			{
				// The player in the winners seat actually is a loser (moderator-forced),
				//  so everyone else shouldn't change.
				multiplier = 1.0 - WeLoser;
				K = 0;
			}
		}

		iRatings[i] += (int)(K * (multiplier));

		if (iPartials[i] && 0 == ((iWins[i] + iLosses[i] + iDraws[i]) % 10))
			iPartials[i]--;
	}
}

void RoomServer::InitRatings(ROOMSERVERPLAYER *p, int *piWins, int *piLosses, int *piDraws, int *piPartials, int *piRating, int *piRatingDev)
{
	*piWins		 = 0;
	*piLosses	 = 0;
	*piDraws	 = 0;
	*piPartials  = 0;
	*piRating	 = 1500;
	*piRatingDev = 0;

	strcpy(p->gameRecord, "r:W:0,L:0,D:0,Q:0,RA:1500,RD:0:r");
}

void RoomServer::SetRatingForAI(int *piWins, int *piLosses, int *piDraws, int *piPartials, int *piRating, int *piRatingDev)
{
	*piWins		 = m_iAIRatingWins;
	*piLosses	 = m_iAIRatingLosses;
	*piDraws	 = m_iAIRatingDraws;
	*piPartials  = m_iAIRatingPartials;
	*piRating	 = m_iAIRating;
	*piRatingDev = m_iAIRatingDev;

	//time to reset
	if(GameHasAI() && (m_iAIRating < 1000))
	{
		TRACE(_WARN_ "AI Rating below 1000!!!  W:%d, L:%d, D:%d, Q:%d, RA:%d, RD:%d\n", m_iAIRatingWins,
			m_iAIRatingLosses, m_iAIRatingDraws, m_iAIRatingPartials, m_iAIRating, m_iAIRatingDev);

		m_iAIRating			= 1000;
	}
}

void RoomServer::UpdateRatingForAI(int result, int ratingChange, int ratingDevChange)
{
	if (RRG_WIN == result)
		m_iAIRatingWins++;
	else if (RRG_LOSS == result)
		m_iAIRatingLosses++;
	else if (RRG_DRAW == result)
		m_iAIRatingDraws++;
	else
		m_iAIRatingPartials++;

	m_iAIRating += ratingChange;		// ratingChange can be negative if a loss
	m_iAIRatingDev += ratingDevChange;	// ratingDevChange can be negative if a loss

	if ((m_iAIRatingWins + m_iAIRatingLosses + m_iAIRatingDraws + m_iAIRatingPartials) % 100)
	{
		TRACE(_ROUTINE_ "AI Rating now at:  W:%d, L:%d, D:%d, Q:%d, RA:%d, RD:%d\n", m_iAIRatingWins,
			m_iAIRatingLosses, m_iAIRatingDraws, m_iAIRatingPartials, m_iAIRating, m_iAIRatingDev);
	}
}

int RoomServer::ParseGameRecord(ROOMSERVERPLAYER *p, int *piWins, int *piLosses, int *piDraws, int *piPartials, int *piRating, int *piRatingDev)
{
	char *pc, *pc1;

	pc = strstr(p->gameRecord, "r:");
	if(!pc)
	{
		//no ratings record
		return 0;
	}

	pc1 = strstr(pc, "W:");
	if(pc1)
		*piWins = atoi(pc1 + 2);
	else
		*piWins = 0;

	if(!(*piWins >= 0 && *piWins < 5000))
	{
		TRACE(_WARN_ "Invalid # of wins for player %s\n", p->info.login);
		return 0;
	}

	pc1 = strstr(pc, "L:");
	if(pc1)
		*piLosses = atoi(pc1 + 2);
	else
		*piLosses = 0;

	pc1 = strstr(pc, "D:");
	if(pc1)
		*piDraws = atoi(pc1 + 2);
	else
		*piDraws = 0;

	pc1 = strstr(pc, "Q:");
	if(pc1)
		*piPartials = atoi(pc1 + 2);
	else
		*piPartials = 0;

	pc1 = strstr(pc, "RA:");
	if(pc1)
		*piRating = atoi(pc1 + 3);
	else
		*piRating = 1500;
	
	pc1 = strstr(pc, "RD:");
	if(pc1)
		*piRatingDev = atoi(pc1 + 3);
	else
		*piRatingDev = 0;

	if(!(*piRatingDev == 0))
	{
		TRACE(_WARN_ "Invalid value for RatingDev for player %s\n", p->info.login);
		return 0;
	}

	return 6;
}

void RoomServer::UpdateGameRecord(ROOMSERVERPLAYER *p, int iWins, int iLosses, int iDraws, int iPartials, int iRating, int iRatingDev)
{
	char str1[MAX_PLAYER_GAME_RECORD], localGameRecord[MAX_PLAYER_GAME_RECORD];
	char *pcStart = NULL, *pcEnd = NULL;

	if(!(iWins >= 0 && iWins < 5000))
		TRACE(_WARN_ "iWins messed up for player %s.\n", p->info.login);

	strnzcpy(localGameRecord, p->gameRecord, sizeof(localGameRecord) - 1);

	pcStart = strstr(p->gameRecord, "r:");
	if(!pcStart)
	{
		pcStart = p->gameRecord;
	}
	else
	{
		//only do this if we indeed did find a ratings segment in the game record.
		pcEnd = strstr(pcStart, ":r");
		if(pcEnd)
			pcEnd += 2; //advance past ":r"

		*pcStart = 0;
		if(pcEnd && *pcEnd)
		{
			strcat(p->gameRecord, pcEnd);
		}
	}
	
	sprintf(str1, "r:W:%d,L:%d,D:%d,Q:%d,RA:%d,RD:%d:r", iWins, iLosses, iDraws, iPartials, iRating, iRatingDev);

	ASSERT(_WARN_, atoi(&str1[4]) == iWins);
	
	if(strlen(p->gameRecord) + strlen(str1) > MAX_PLAYER_GAME_RECORD)
	{
		TRACE(_WARN_ "Game record isn't large enough.  It contains \"%s\", and \"%s\" not added.\n", p->gameRecord, str1);
		
		strcpy(p->gameRecord, localGameRecord);

		return;
	}

	strcat(p->gameRecord, str1);

	p->info.wins		= iWins;
	p->info.losses		= iLosses;
	p->info.draws		= iDraws;
	p->info.quits		= iPartials;
	p->info.rating		= iRating;
	p->info.ratingDev	= iRatingDev;

	MakeDataDirty(p, DFI_GAMERECORD);

	return;
}

Boolean RoomServer::TableIsRatedGame(int iTable) 
{ 
	return (tables[iTable].ratingsFlags & RF_TABLE_RATED); 
}

///////////////////////////////////////////////////////////////////////////////////////////////////

RoomServerConnections::RoomServerConnections(int maxConnections, int maxListeners)
	: TConnectionMgr(maxConnections, maxListeners)
{
}

RoomServerConnections::~RoomServerConnections()
{
}

void RoomServerConnections::ConnectionCompleted(HCONNECTION c)
{
	if (c->GetID() != CONNECTIONID_DBCONNECTOR)
		{ ASSERT(_WARN_, FALSE); return; }
	
	ASSERT(_WARN_, c == roomServer->dbConnection);
	
	if (GetConnectionStatus(c) != CS_CONNECTED)
		{ roomServer->dbConnection = NULL; return; }
	
	// turn off the NAGLE delay
	c->UseNagleDelay(NAGLE_DELAY_OFF);

	SetConnectionID(c, CONNECTIONID_DB);
	
	UpdateDisplay();
}

void RoomServerConnections::ClientConnected(HCONNECTION c)
{
	int i, index, err;
	MFS_ROOM_FULL_MESSAGE mfs;
	
	switch (GetConnectionID(c)) 
	{
		case CONNECTIONID_CLIENTLISTENER:
			// look for an empty spot for this new player
			
			for (index = 0 ; index < MAX_ROOM_PLAYERS ; index++)
			{
				if (roomServer->clients[index].status == CS_EMPTY)
//				{
//					if (roomServer->clients[index].connection != NULL)
//						TRACE(_WARN_ "Found empty player record with non-null connection.\n");
//					else
						break;
//				}
			}
			
			if (index == MAX_ROOM_PLAYERS) 
			{
				// send a message back to the client saying the room is full
				mfs.messageType = MFS_ROOM_FULL;
				mfs.numPlayers = roomServer->numClients;

				TRACE(_ROUTINE_ "Sending to <joiner>: %s.\n", roomServer->GetMessageDescription((char *)&mfs));
				
				err = PostMessage(c, (char *)&mfs, sizeof(MFS_ROOM_FULL_MESSAGE));
				if (err)
					TRACE(_WARN_ "RoomServerConnections::ClientConnected(): PostMessage() returned error %d\n", err);

				MyFlushAndClose(c);
				break;
			}
			
			if (roomServer->clients[index].index != index) 
			{
				TRACE(_WARN_ "RoomServerConnections::ClientConnected(): bad client index\n");
				roomServer->clients[index].index = index;
			}
			
			SetConnectionData(c, (LPVOID)&roomServer->clients[index]);
			SetConnectionID(c, CONNECTIONID_CLIENT);
			roomServer->clients[index].connection = c;
			roomServer->clients[index].status = CS_JOINING;
			roomServer->clients[index].info.playerID = 0;
			roomServer->clients[index].info.playerID = roomServer->GenerateNextPlayerID();
			roomServer->clients[index].info.login[0] = 0;
			roomServer->clients[index].info.level = 0;
			roomServer->clients[index].info.shadow = 0;
			roomServer->clients[index].info.table = -1;
			roomServer->clients[index].info.seat = -1;
			roomServer->clients[index].info.invitedTable = -1;
			roomServer->clients[index].info.invitedSeat = -1;
			roomServer->clients[index].info.flags = 0;
			roomServer->InitPlayerSpecificInfo(&roomServer->clients[index].info);
			roomServer->clients[index].databaseID = -1;
			roomServer->clients[index].email[0] = 0;
			roomServer->clients[index].profile[0] = 0;
			roomServer->clients[index].gameRecord[0] = 0;
			for (i = 0 ; i < NUM_DIRTY_FLAGS ; i++)
				roomServer->clients[index].dirtyFlags[i] = DF_EMPTY;
			roomServer->clients[index].requestingTable = -1;
			roomServer->clients[index].requestingSeat = -1;
			roomServer->clients[index].approvingPlayerID = -1;
			roomServer->clients[index].requesterPlayerID = -1;
			roomServer->clients[index].requestedSeat = -1;
			roomServer->clients[index].inviterTable = -1;
			roomServer->clients[index].inviterPlayerID = -1;
			roomServer->clients[index].invitedToWatch = FALSE;
			roomServer->clients[index].globallyMuted = FALSE;
			roomServer->ClearBootRecord(&roomServer->clients[index], TRUE);
			
			roomServer->UpdateNumClients();
			break;
		case CONNECTIONID_RISLISTENER:
			SetConnectionID(c, CONNECTIONID_RIS);
			c->SetHeartbeat(FALSE);
			break;
		default:
			ASSERT(_WARN_, FALSE);
			break;
	}
	
	UpdateDisplay();
}

void RoomServerConnections::ConnectionClosed(HCONNECTION c)
{
	int i, j, oldStatus;
	MFS_ROOM_EXIT_MESSAGE mfsRE;
	ROOMSERVERPLAYER *client;
	
	if (IsAListener(c))
		return;
	
	if (GetConnectionID(c) == CONNECTIONID_RIS)
		{ UpdateDisplay(); return; }
	
	if (GetConnectionID(c) == CONNECTIONID_DB || GetConnectionID(c) == CONNECTIONID_DBCONNECTOR) 
	{
		roomServer->dbConnection = NULL;
		// since we lost the database connection, we have to assume that all of the
		// pending database writes are not going to be completed
		
		for (i = 0 ; i < MAX_ROOM_PLAYERS ; i++) 
		{
			client = &roomServer->clients[i];
			if (client->status != CS_EMPTY)
				for (j = 0 ; j < NUM_DIRTY_FLAGS ; j++) 
				{
					// Joiners roll-back
					if (client->dirtyFlags[j] == DF_REQUESTED)
						client->dirtyFlags[j] = DF_ENTERED;

					// Leavers or in-game roll-back
					if (client->dirtyFlags[j] == DF_SENDING || client->dirtyFlags[j] == DF_DIRTY_WHILE_SENDING)
						client->dirtyFlags[j] = DF_DIRTY;
				}
		}
		
		UpdateDisplay();
		
		return;
	}
	
	client = roomServer->GetClientFromHCONNECTION(c);
	SetConnectionData(c, NULL);
	
	if (roomServer->shuttingDown)
		return;
	
	if (client == NULL)
		{ TRACE(_WARN_ "RoomServerConnections::ConnectionClosed(): connection not found"); return; }

	if (client->connection != c)
		{ TRACE(_WARN_ "RoomServerConnections::ConnectionClosed(): connection linked to wrong client"); return; }
	
	oldStatus = client->status;
	
	// mark the player as leaving (must do before TablePlayerChanged() so we don't broadcast to this player)
	
	client->status = CS_LEAVING;
	
	if (oldStatus == CS_JOINING) 
	{
		client->status = CS_EMPTY;
		roomServer->dbRecords[client->index].login[0] = 0;
		roomServer->UpdateNumClients();
	}
	else
	{
		// mark all player data as needing database write
		for (i = 0 ; i < NUM_DIRTY_FLAGS ; i++)
			client->dirtyFlags[i] = DF_DIRTY;

		// if the player is at a table, tell the table the player left
		
		if (client->info.table != -1)
			roomServer->TablePlayerChanged(client, TPC_LEFT);
		
		roomServer->CancelRequestOrInvitation(client);
		roomServer->RelinquishTIPages(client);
		
		// tell everyone the player has left
		
		mfsRE.messageType	= MFS_ROOM_EXIT;
		mfsRE.player		= client->info;
		mfsRE.numPlayers	= roomServer->CountActivePlayers();
		roomServer->BroadcastRoomMessage((char *)&mfsRE, sizeof(MFS_ROOM_EXIT_MESSAGE));
		
		// if the player is at a table, tell the table the player left
		
		if (roomServer->IsTournamentRoom() && client->IsInTournament())
			roomServer->TournamentPlayerLeftRoom(client);
		
		// wait until we can write the player's data to the database before clearing the record
	}
	
	UpdateDisplay();
}

//======================================================================
// The Connection Manager decided it's time to send this connection a
// heartbeat message.
//======================================================================

void RoomServerConnections::RequestHeartbeat(HCONNECTION c)
{
	MFS_ROOM_HEARTBEAT_MESSAGE mfs;
	ROOMSERVERPLAYER *client;
	
	if (GetConnectionID(c) != CONNECTIONID_CLIENT)
		{ ASSERT(_WARN_, FALSE); return; }
	
	client = roomServer->GetClientFromHCONNECTION(c);
	
	mfs.messageType = MFS_ROOM_HEARTBEAT;

	mfs.rtt = timeGetTime();
	
	TRACE(_VERBOSE_ "Sending to %s (playerID = %d): %s.\n", (client ? client->info.login : "???"), (client ? client->info.playerID : -1), roomServer->GetMessageDescription((char *)&mfs));
	
	PostMessage(c, (char *)&mfs, sizeof(MFS_ROOM_HEARTBEAT_MESSAGE));
}

void RoomServerConnections::ProcessUnquedMessage(HCONNECTION c, char *buf, int length)
{
}

void RoomServerConnections::ProcessRawMessage(HCONNECTION c, char *buf, int length)
{
}

void RoomServerConnections::ProcessAdminRequest(HCONNECTION c)
{
	char adminMsg[256];
	i18nStr getS;
	
	// "<html><head><title>%s</title><META HTTP-EQUIV=\"Refresh\" CONTENT=\"300\" ></head><body background=http://128.11.20.135/images/qs_rock.jpg>\n"	
	GetString(138, getS);
	sprintf(adminMsg, getS, SERVER_APP_NAME);
	PostMessage(c, adminMsg, strlen(adminMsg));

	GetServerStatusString(adminMsg);
	PostMessage(c, adminMsg, strlen(adminMsg));
	
	sprintf(adminMsg, "<p>gameStarts = %lu, mem = %d bytes, maxmem = %d bytes, balance = %d<p>Ratings: wins = %d, losses = %d, draws = %d, rating = %d", 
		roomServer->gameStarts,
		ServerMemoryInfo.numBytesAllocated,
		ServerMemoryInfo.maxMemoryAllocated,
		ServerMemoryInfo.newDeleteBalance,
		roomServer->m_iAIRatingWins,
		roomServer->m_iAIRatingLosses,
		roomServer->m_iAIRatingDraws,
		roomServer->m_iAIRating
		);

	PostMessage(c, adminMsg, (msglen_t)strlen(adminMsg));

	sprintf(adminMsg, "<p>warns = %lu, crashes = %lu, criticals = %lu",
		g_warnMessages,
		g_crashMessages,
		g_criticalMessages
		);

	PostMessage(c, adminMsg, (msglen_t)strlen(adminMsg));

	// chain to the CM so it can add its admin data
	TConnectionMgr::ProcessAdminRequest(c);
}

const char *RoomServerConnections::GetPlayerNameFromConnection(HCONNECTION c) const
{
	ROOMSERVERPLAYER *player;
	
	player = roomServer->GetClientFromHCONNECTION(c);
	if (player)
		return player->info.login;
	
	return "???"; // <player not found>
}

int RoomServerConnections::MyFlushAndClose(HCONNECTION c)
{
	if (c->GetStatus() == CS_CONNECTED)
		FlushAndClose(c);
	
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////

char *RoomServer::GetMessageDescription(char *message)
{
	MESSAGE_HEADER *mh;
	
	MFS_ROOM_FULL_MESSAGE *mfsRFM;
	MFS_ROOM_DISPLAY_STRING_MESSAGE *mfsRDSM;
	MTS_ROOM_ENTER_MESSAGE *mtsREM;
	MFS_ROOM_STATUS_HEADER_MESSAGE *mfsRSHM;
	MFS_ROOM_STATUS_TABLES_MESSAGE *mfsRSTM;
	MFS_ROOM_STATUS_PLAYERS_MESSAGE *mfsRSPM;
	MFS_ROOM_STATUS_CHAT_HISTORY_MESSAGE *mfsRSCHM;
	MFS_ROOM_ENTER_MESSAGE *mfsREM;
	MTS_ROOM_CHAT_MESSAGE *mtsRCM;
	MFS_ROOM_CHAT_MESSAGE *mfsRCM;
	MTS_ROOM_CHANGE_SHADOW_MESSAGE *mtsRCSM;
	MFS_ROOM_CHANGE_SHADOW_MESSAGE *mfsRCSM;
	MTS_ROOM_CHANGE_PROFILE_MESSAGE *mtsRCPM;
	MFS_ROOM_CHANGE_PROFILE_MESSAGE *mfsRCPM;
	MTS_ROOM_SIT_MESSAGE *mtsRSM;
	MFS_ROOM_SIT_MESSAGE *mfsRSM;
	MTS_ROOM_CHANGE_TABLE_SETTINGS_MESSAGE *mtsRCTSM;
	MFS_ROOM_CHANGE_TABLE_SETTINGS_MESSAGE *mfsRCTSM;
	MTS_ROOM_START_GAME_MESSAGE *mtsRSGM;
	MFS_ROOM_START_GAME_MESSAGE *mfsRSGM;
	MTS_ROOM_STAND_MESSAGE *mtsRSTANDM;
	MFS_ROOM_STAND_MESSAGE *mfsRSTANDM;
	MFS_ROOM_END_GAME_MESSAGE *mfsREGM;
	MTS_ROOM_MODERATE_MESSAGE *mtsRMM;
	MTS_ROOM_COMPLAIN_MESSAGE *mtsRCOMPLAINM;
	MFS_ROOM_EXIT_MESSAGE *mfsREXM;
	
	MTS_ROOM_REQUEST_MESSAGE *mtsRRM;
	MFS_ROOM_REQUEST_MESSAGE *mfsRRM;
	MTS_ROOM_ANSWER_REQUEST_MESSAGE *mtsRARM;
	MFS_ROOM_DISAPPROVE_REQUEST_MESSAGE *mfsRDRM;
	MTS_ROOM_INVITE_MESSAGE *mtsRIM;
	MFS_ROOM_INVITE_MESSAGE *mfsRIM;
	MTS_ROOM_RSVP_MESSAGE *mtsRRSVPM;
	
	MTS_ROOM_TI_CHECK_OUT_PAGE_MESSAGE *mtsTICOPM;
	MFS_ROOM_TI_CHECK_OUT_PAGE_MESSAGE *mfsTICOPM;
	MTS_ROOM_TI_ADD_TO_TEXT_MESSAGE *mtsTIATM;
	MFS_ROOM_TI_ADD_TO_TEXT_MESSAGE *mfsTIATM;
	MTS_ROOM_TI_JOIN_MESSAGE *mtsTIJM;
	MFS_ROOM_TI_JOIN_MESSAGE *mfsTIJM;
	MTS_ROOM_TI_SET_BANK_MESSAGE *mtsTISBM;
	MFS_ROOM_TI_SET_BANK_MESSAGE *mfsTISBM;
	MTS_ROOM_TI_GENERATE_ASSIGNMENTS_MESSAGE *mtsTIGAM;
	MTS_ROOM_TI_CLEAR_AUTO_RESULTS_MESSAGE *mtsTICARM;
	
	MFS_ROOM_PLAYER_RATING_MESSAGE *mfsRPR;

	MTS_GAME_STATE_MESSAGE *mtsGSM;
	MFS_GAME_STATE_MESSAGE *mfsGSM;
	MTS_GAME_CHAT_MESSAGE *mtsGCM;
	MFS_GAME_CHAT_MESSAGE *mfsGCM;
	MFS_GAME_DISPLAY_STRING_MESSAGE *mfsGDSM;
	MTS_GAME_BOOT_MESSAGE *mtsGBM;
	MFS_GAME_BOOT_MESSAGE *mfsGBM;
	MTS_GAME_RESTART_MESSAGE *mtsGRM;
	MFS_GAME_RESTART_MESSAGE *mfsGRM;
	
	MFS_RIS_ERROR_MESSAGE *mfsRISEM;
	MTS_RS_FIND_PLAYER_MESSAGE *mtsRFPM;
	MFS_RS_FIND_PLAYER_MESSAGE *mfsRFPM;
	MTS_RS_NUMPLAYERS_MESSAGE *mtsRNPM;
	MFS_RS_NUMPLAYERS_MESSAGE *mfsRNPM;
	
	MFDB_STATUS_MESSAGE *mfdbSTATUS;
	MTDB_RS_CONNECT_MESSAGE *mtdbRCM;
	MFDB_RS_CONNECT_MESSAGE *mfdbRCM;
	MTDB_RS_SET_SHADOW_MESSAGE *mtdbRSSM;
	MFDB_RS_SET_SHADOW_MESSAGE *mfdbRSSM;
	MTDB_RS_SET_BANK_MESSAGE *mtdbRSBM;
	MFDB_RS_SET_BANK_MESSAGE *mfdbRSBM;
	MTDB_RS_SET_PROFILE_MESSAGE *mtdbRSPM;
	MFDB_RS_SET_PROFILE_MESSAGE *mfdbRSPM;
	MTDB_RS_SET_GAME_RECORD_MESSAGE *mtdbRSGRM;
	MFDB_RS_SET_GAME_RECORD_MESSAGE *mfdbRSGRM;
	MTDB_RS_SET_BANNED_MESSAGE *mtdbRSBANNEDM;
	MFDB_RS_SET_BANNED_MESSAGE *mfdbRSBANNEDM;
	MTDB_RS_SET_UNBANNED_MESSAGE *mtdbRSUNBANNEDM;
	MFDB_RS_SET_UNBANNED_MESSAGE *mfdbRSUNBANNEDM;
	MTDB_RS_DISCONNECT_MESSAGE *mtdbRDM;
	MFDB_RS_DISCONNECT_MESSAGE *mfdbRDM;
	
	static char messageDescription[2048];
	
	mh = (MESSAGE_HEADER *)message;
	
	switch (mh->messageType) {
		// messages to/from room information servers
		
		case MFS_RIS_ERROR:
			mfsRISEM = (MFS_RIS_ERROR_MESSAGE *)message;
			sprintf(messageDescription, "MTS_RS_FIND_PLAYER, originalMessageType = %d, errCode = %d, errText = %s",
										(int)mfsRISEM->originalMessageType, (int)mfsRISEM->errCode, mfsRISEM->errText);
			break;
		case MTS_RS_FIND_PLAYER:
			mtsRFPM = (MTS_RS_FIND_PLAYER_MESSAGE *)message;
			sprintf(messageDescription, "MTS_RS_FIND_PLAYER, playerName = %s", mtsRFPM->playerName);
			break;
		case MFS_RS_FIND_PLAYER:
			mfsRFPM = (MFS_RS_FIND_PLAYER_MESSAGE *)message;
			sprintf(messageDescription, "MFS_RS_FIND_PLAYER, playerName = %s, wasFound = %d, playerCount = %d",
										mfsRFPM->playerName, (int)mfsRFPM->wasFound, mfsRFPM->playerCount);
			break;
		case MTS_RS_NUMPLAYERS:
			mtsRNPM = (MTS_RS_NUMPLAYERS_MESSAGE *)message;
			sprintf(messageDescription, "MTS_RS_NUMPLAYERS");
			break;
		case MFS_RS_NUMPLAYERS:
			mfsRNPM = (MFS_RS_NUMPLAYERS_MESSAGE *)message;
			sprintf(messageDescription, "MFS_RS_NUMPLAYERS, playerCount = %d", mfsRNPM->playerCount);
			break;
		
		// messages to/from the database
		
		case MFDB_STATUS:
			mfdbSTATUS = (MFDB_STATUS_MESSAGE *)message;
			sprintf(messageDescription, "MFDB_STATUS, dbStat = %d", mfdbSTATUS->dbStat);
			break;
		case MTDB_RS_CONNECT:
			mtdbRCM = (MTDB_RS_CONNECT_MESSAGE *)message;
			sprintf(messageDescription, "MTDB_RS_CONNECT, playerID = %d, magicIndex = %d, type = %d, roomID = %lu",
										mtdbRCM->playerID, mtdbRCM->magicIndex, mtdbRCM->type, mtdbRCM->roomID);
			break;
		case MFDB_RS_CONNECT:
			mfdbRCM = (MFDB_RS_CONNECT_MESSAGE *)message;
			sprintf(messageDescription, "MFDB_RS_CONNECT, dbStat = %d, magicIndex = %d, playerID = %d, type = %d",
										(int)mfdbRCM->dbStat, mfdbRCM->magicIndex, mfdbRCM->playerID, mfdbRCM->type);
			break;
		case MTDB_RS_SET_SHADOW:
			mtdbRSSM = (MTDB_RS_SET_SHADOW_MESSAGE *)message;
			sprintf(messageDescription, "MTDB_RS_SET_SHADOW, playerID = %d, magicIndex = %d, shadow = %d",
										mtdbRSSM->playerID, mtdbRSSM->magicIndex, mtdbRSSM->shadow);
			break;
		case MFDB_RS_SET_SHADOW:
			mfdbRSSM = (MFDB_RS_SET_SHADOW_MESSAGE *)message;
			sprintf(messageDescription, "MFDB_RS_SET_SHADOW, dbStat = %d, magicIndex = %d, playerID = %d",
										(int)mfdbRSSM->dbStat, mfdbRSSM->magicIndex, mfdbRSSM->playerID);
			break;
		case MTDB_RS_SET_BANK:
			mtdbRSBM = (MTDB_RS_SET_BANK_MESSAGE *)message;
			sprintf(messageDescription, "MTDB_RS_SET_BANK, playerID = %d, magicIndex = %d, bank = %d",
										mtdbRSBM->playerID, mtdbRSBM->magicIndex, mtdbRSBM->bank);
			break;
		case MFDB_RS_SET_BANK:
			mfdbRSBM = (MFDB_RS_SET_BANK_MESSAGE *)message;
			sprintf(messageDescription, "MFDB_RS_SET_BANK, dbStat = %d, magicIndex = %d, playerID = %d",
										(int)mfdbRSBM->dbStat, mfdbRSBM->magicIndex, mfdbRSBM->playerID);
			break;
		case MTDB_RS_SET_PROFILE:
			mtdbRSPM = (MTDB_RS_SET_PROFILE_MESSAGE *)message;
			sprintf(messageDescription, "MTDB_RS_SET_PROFILE, playerID = %d, magicIndex = %d, profileLength = %d, profile = %s",
										mtdbRSPM->playerID, mtdbRSPM->magicIndex, mtdbRSPM->profileLength, mtdbRSPM->profile);
			break;
		case MFDB_RS_SET_PROFILE:
			mfdbRSPM = (MFDB_RS_SET_PROFILE_MESSAGE *)message;
			sprintf(messageDescription, "MFDB_RS_SET_PROFILE, dbStat = %d, magicIndex = %d, playerID = %d",
										(int)mfdbRSPM->dbStat, mfdbRSPM->magicIndex, mfdbRSPM->playerID);
			break;
		case MTDB_RS_SET_GAME_RECORD:
			mtdbRSGRM = (MTDB_RS_SET_GAME_RECORD_MESSAGE *)message;
			sprintf(messageDescription, "MTDB_RS_SET_GAME_RECORD, playerID = %d, magicIndex = %d, grLength = %d, gameRecord = %s",
										mtdbRSGRM->playerID, mtdbRSGRM->magicIndex, mtdbRSGRM->grLength, mtdbRSGRM->gameRecord);
			break;
		case MFDB_RS_SET_GAME_RECORD:
			mfdbRSGRM = (MFDB_RS_SET_GAME_RECORD_MESSAGE *)message;
			sprintf(messageDescription, "MFDB_RS_SET_GAME_RECORD, dbStat = %d, magicIndex = %d, playerID = %d",
										(int)mfdbRSGRM->dbStat, mfdbRSGRM->magicIndex, mfdbRSGRM->playerID);
			break;
		case MTDB_RS_SET_BANNED:
			mtdbRSBANNEDM = (MTDB_RS_SET_BANNED_MESSAGE *)message;
			sprintf(messageDescription, "MTDB_RS_SET_BANNED, login = %s, OfflineBanFlag = %d, moderatorName = %s",
										mtdbRSBANNEDM->login, (int)mtdbRSBANNEDM->OfflineBanFlag, mtdbRSBANNEDM->moderatorName);
			break;
		case MFDB_RS_SET_BANNED:
			mfdbRSBANNEDM = (MFDB_RS_SET_BANNED_MESSAGE *)message;
			sprintf(messageDescription, "MFDB_RS_SET_BANNED, dbStat = %d", (int)mfdbRSBANNEDM->dbStat);
			break;
		case MTDB_RS_SET_UNBANNED:
			mtdbRSUNBANNEDM = (MTDB_RS_SET_UNBANNED_MESSAGE *)message;
			sprintf(messageDescription, "MTDB_RS_SET_UNBANNED, login = %s", mtdbRSUNBANNEDM->login);
			break;
		case MFDB_RS_SET_UNBANNED:
			mfdbRSUNBANNEDM = (MFDB_RS_SET_UNBANNED_MESSAGE *)message;
			sprintf(messageDescription, "MFDB_RS_SET_UNBANNED, dbStat = %d", (int)mfdbRSUNBANNEDM->dbStat);
			break;
		case MTDB_RS_DISCONNECT:
			mtdbRDM = (MTDB_RS_DISCONNECT_MESSAGE *)message;
			sprintf(messageDescription, "MTDB_RS_DISCONNECT, playerID = %d, magicIndex = %d, roomID = %lu",
										mtdbRDM->playerID, mtdbRDM->magicIndex, mtdbRDM->roomID);
			break;
		case MFDB_RS_DISCONNECT:
			mfdbRDM = (MFDB_RS_DISCONNECT_MESSAGE *)message;
			sprintf(messageDescription, "MFDB_RS_DISCONNECT, dbStat = %d, magicIndex = %d, playerID = %d",
										(int)mfdbRDM->dbStat, mfdbRDM->magicIndex, mfdbRDM->playerID);
			break;
		
		// messages to/from players
		
		case MFS_ROOM_HEARTBEAT:
			sprintf(messageDescription, "MFS_ROOM_HEARTBEAT");
			break;
		
		case MTS_ROOM_HEARTBEAT:
			sprintf(messageDescription, "MTS_ROOM_HEARTBEAT");
			break;
		
		case MFS_ROOM_FULL:
			mfsRFM = (MFS_ROOM_FULL_MESSAGE *)message;
			sprintf(messageDescription, "MFS_ROOM_FULL, numPlayers = %d", mfsRFM->numPlayers);
			break;
		
		case MFS_ROOM_DISPLAY_STRING:
			mfsRDSM = (MFS_ROOM_DISPLAY_STRING_MESSAGE *)message;
			sprintf(messageDescription, "MFS_ROOM_DISPLAY_STRING, title = %s, text = %s",
										mfsRDSM->title, mfsRDSM->text);
			break;
		
		case MTS_ROOM_ENTER:
			mtsREM = (MTS_ROOM_ENTER_MESSAGE *)message;
			sprintf(messageDescription, "MTS_ROOM_ENTER, login = %s, level = %d, databaseID = %d, roomID = %lu",
										mtsREM->login, (int)mtsREM->level, (int)mtsREM->databaseID, mtsREM->roomID);
			break;
		case MFS_ROOM_STATUS_HEADER:
			mfsRSHM = (MFS_ROOM_STATUS_HEADER_MESSAGE *)message;
			sprintf(messageDescription, "MFS_ROOM_STATUS_HEADER, roomName = %s, numTables = %d, numPlayers = %d, numChatLines = %d, adTags = %s",
										mfsRSHM->roomName, (int)mfsRSHM->numTables, (int)mfsRSHM->numPlayers, (int)mfsRSHM->numChatLines, mfsRSHM->adTags);
			break;
		case MFS_ROOM_STATUS_TABLES:
			mfsRSTM = (MFS_ROOM_STATUS_TABLES_MESSAGE *)message;
			sprintf(messageDescription, "MFS_ROOM_STATUS_TABLES, lastMessageOfThisType = %d",
										(int)mfsRSTM->lastMessageOfThisType);
			break;
		case MFS_ROOM_STATUS_PLAYERS:
			mfsRSPM = (MFS_ROOM_STATUS_PLAYERS_MESSAGE *)message;
			sprintf(messageDescription, "MFS_ROOM_STATUS_PLAYERS, terminator = %d",
										(int)mfsRSPM->terminator);
			break;
		case MFS_ROOM_STATUS_CHAT_HISTORY:
			mfsRSCHM = (MFS_ROOM_STATUS_CHAT_HISTORY_MESSAGE *)message;
			sprintf(messageDescription, "MFS_ROOM_STATUS_CHAT_HISTORY, terminator = %d",
										(int)mfsRSCHM->terminator);
			break;
		case MFS_ROOM_ENTER:
			mfsREM = (MFS_ROOM_ENTER_MESSAGE *)message;
			sprintf(messageDescription, "MFS_ROOM_ENTER, playerID = %d, login = %s, level = %d, shadow = %d, table = %d, seat = %d, numPlayers = %d",
										(int)mfsREM->player.playerID,
										mfsREM->player.login,
										(int)mfsREM->player.level,
										(int)mfsREM->player.shadow,
										(int)mfsREM->player.table,
										(int)mfsREM->player.seat,
										(int)mfsREM->numPlayers);
			break;
		case MTS_ROOM_CHAT:
			mtsRCM = (MTS_ROOM_CHAT_MESSAGE *)message;
			sprintf(messageDescription, "MTS_ROOM_CHAT, playerIDFrom = %d, numPrivateIDsTo = %d",
										(int)mtsRCM->playerIDFrom, (int)mtsRCM->numPrivateIDsTo);
			break;
		case MFS_ROOM_CHAT:
			mfsRCM = (MFS_ROOM_CHAT_MESSAGE *)message;
			sprintf(messageDescription, "MFS_ROOM_CHAT, playerIDFrom = %d, privateIDTo = %d, text = %s",
										(int)mfsRCM->playerIDFrom, (int)mfsRCM->privateIDTo, mfsRCM->text);
			break;
		case MTS_ROOM_CHANGE_SHADOW:
			mtsRCSM = (MTS_ROOM_CHANGE_SHADOW_MESSAGE *)message;
			sprintf(messageDescription, "MTS_ROOM_CHANGE_SHADOW, playerID = %d, shadow = %d",
										(int)mtsRCSM->playerID, (int)mtsRCSM->shadow);
			break;
		case MFS_ROOM_CHANGE_SHADOW:
			mfsRCSM = (MFS_ROOM_CHANGE_SHADOW_MESSAGE *)message;
			sprintf(messageDescription, "MFS_ROOM_CHANGE_SHADOW, playerID = %d, shadow = %d",
										(int)mfsRCSM->playerID, (int)mfsRCSM->shadow);
			break;
		case MTS_ROOM_CHANGE_PROFILE:
			mtsRCPM = (MTS_ROOM_CHANGE_PROFILE_MESSAGE *)message;
			sprintf(messageDescription, "MTS_ROOM_CHANGE_PROFILE, playerID = %d", (int)mtsRCPM->playerID);
			break;
		case MFS_ROOM_CHANGE_PROFILE:
			mfsRCPM = (MFS_ROOM_CHANGE_PROFILE_MESSAGE *)message;
			sprintf(messageDescription, "MFS_ROOM_CHANGE_PROFILE, playerID = %d", (int)mfsRCPM->playerID);
			break;
		case MTS_ROOM_SIT:
			mtsRSM = (MTS_ROOM_SIT_MESSAGE *)message;
			sprintf(messageDescription, "MTS_ROOM_SIT, playerID = %d, table = %d, seat = %d",
										(int)mtsRSM->playerID, (int)mtsRSM->table, (int)mtsRSM->seat);
			break;
		case MFS_ROOM_SIT:
			mfsRSM = (MFS_ROOM_SIT_MESSAGE *)message;
			sprintf(messageDescription, "MFS_ROOM_SIT, playerID = %d, table = %d, seat = %d",
										(int)mfsRSM->playerID, (int)mfsRSM->table, (int)mfsRSM->seat);
			break;
		case MTS_ROOM_CHANGE_TABLE_SETTINGS:
			mtsRCTSM = (MTS_ROOM_CHANGE_TABLE_SETTINGS_MESSAGE *)message;
			sprintf(messageDescription, "MTS_ROOM_CHANGE_TABLE_SETTINGS, table = %d, joinOption = %d, watchersOption = %d, muteWatchers = %d, description = %s",
										(int)mtsRCTSM->table, (int)mtsRCTSM->joinOption, (int)mtsRCTSM->watchersOption, (int)mtsRCTSM->muteWatchers, mtsRCTSM->description);
			break;
		case MFS_ROOM_CHANGE_TABLE_SETTINGS:
			mfsRCTSM = (MFS_ROOM_CHANGE_TABLE_SETTINGS_MESSAGE *)message;
			sprintf(messageDescription, "MTS_ROOM_CHANGE_TABLE_SETTINGS, table = %d, joinOption = %d, watchersOption = %d, muteWatchers = %d, description = %s",
										(int)mfsRCTSM->table, (int)mfsRCTSM->joinOption, (int)mfsRCTSM->watchersOption, (int)mfsRCTSM->muteWatchers, mfsRCTSM->description);
			break;
		case MTS_ROOM_START_GAME:
			mtsRSGM = (MTS_ROOM_START_GAME_MESSAGE *)message;
			sprintf(messageDescription, "MTS_ROOM_START_GAME, playerID = %d, table = %d, seat = %d",
										(int)mtsRSGM->playerID, (int)mtsRSGM->table, (int)mtsRSGM->seat);
			break;
		case MFS_ROOM_START_GAME:
			mfsRSGM = (MFS_ROOM_START_GAME_MESSAGE *)message;
			sprintf(messageDescription, "MFS_ROOM_START_GAME, playerID = %d, table = %d, seat = %d, startGame = %d",
										(int)mfsRSGM->playerID, (int)mfsRSGM->table, (int)mfsRSGM->seat, (int)mfsRSGM->startGame);
			break;
		case MTS_ROOM_STAND:
			mtsRSTANDM = (MTS_ROOM_STAND_MESSAGE *)message;
			sprintf(messageDescription, "MTS_ROOM_STAND, playerID = %d, table = %d, seat = %d",
										(int)mtsRSTANDM->playerID, (int)mtsRSTANDM->table, (int)mtsRSTANDM->seat);
			break;
		case MFS_ROOM_STAND:
			mfsRSTANDM = (MFS_ROOM_STAND_MESSAGE *)message;
			sprintf(messageDescription, "MFS_ROOM_STAND, playerID = %d, table = %d, seat = %d",
										(int)mfsRSTANDM->playerID, (int)mfsRSTANDM->table, (int)mfsRSTANDM->seat);
			break;
		case MFS_ROOM_END_GAME:
			mfsREGM = (MFS_ROOM_END_GAME_MESSAGE *)message;
			sprintf(messageDescription, "MFS_ROOM_END_GAME, table = %d", (int)mfsREGM->table);
			break;
		case MTS_ROOM_MODERATE:
			mtsRMM = (MTS_ROOM_MODERATE_MESSAGE *)message;
			sprintf(messageDescription, "MTS_ROOM_MODERATE, playerID = %d, login = %s, function = %d",
										(int)mtsRMM->playerID, mtsRMM->login, (int)mtsRMM->function);
			break;
		case MTS_ROOM_COMPLAIN:
			mtsRCOMPLAINM = (MTS_ROOM_COMPLAIN_MESSAGE *)message;
			sprintf(messageDescription, "MTS_ROOM_COMPLAIN, login = %s, inGame = %d",
										mtsRCOMPLAINM->login, (int)mtsRCOMPLAINM->inGame);
			break;
		case MFS_ROOM_EXIT:
			mfsREXM = (MFS_ROOM_EXIT_MESSAGE *)message;
			sprintf(messageDescription, "MFS_ROOM_EXIT, playerID = %d, login = %s, level = %d, shadow = %d, table = %d, seat = %d, numPlayers = %d",
										(int)mfsREXM->player.playerID,
										mfsREXM->player.login,
										(int)mfsREXM->player.level,
										(int)mfsREXM->player.shadow,
										(int)mfsREXM->player.table,
										(int)mfsREXM->player.seat,
										(int)mfsREXM->numPlayers);
			break;
		
		// request/invite messages to/from players
		
		case MTS_ROOM_REQUEST:
			mtsRRM = (MTS_ROOM_REQUEST_MESSAGE *)message;
			sprintf(messageDescription, "MTS_ROOM_REQUEST, playerID = %d, table = %d, seat = %d",
										(int)mtsRRM->playerID, (int)mtsRRM->table, (int)mtsRRM->seat);
			break;
		case MFS_ROOM_REQUEST:
			mfsRRM = (MFS_ROOM_REQUEST_MESSAGE *)message;
			sprintf(messageDescription, "MFS_ROOM_REQUEST, playerID = %d, table = %d, seat = %d, cancel = %d",
										(int)mfsRRM->playerID, (int)mfsRRM->table, (int)mfsRRM->seat, (int)mfsRRM->cancel);
			break;
		case MTS_ROOM_ANSWER_REQUEST:
			mtsRARM = (MTS_ROOM_ANSWER_REQUEST_MESSAGE *)message;
			sprintf(messageDescription, "MTS_ROOM_ANSWER_REQUEST, playerID = %d, table = %d, approved = %d",
										(int)mtsRARM->playerID, (int)mtsRARM->table, (int)mtsRARM->approved);
			break;
		case MFS_ROOM_DISAPPROVE_REQUEST:
			mfsRDRM = (MFS_ROOM_DISAPPROVE_REQUEST_MESSAGE *)message;
			sprintf(messageDescription, "MFS_ROOM_DISAPPROVE_REQUEST, playerID = %d, table = %d, seat = %d, notice = %d",
										(int)mfsRDRM->playerID, (int)mfsRDRM->table, (int)mfsRDRM->seat, (int)mfsRDRM->notice);
			break;
		
		case MTS_ROOM_INVITE:
			mtsRIM = (MTS_ROOM_INVITE_MESSAGE *)message;
			sprintf(messageDescription, "MTS_ROOM_INVITE, table = %d, inviteePlayerID = %d, toWatch = %d",
										(int)mtsRIM->table, (int)mtsRIM->inviteePlayerID, (int)mtsRIM->toWatch);
			break;
		case MFS_ROOM_INVITE:
			mfsRIM = (MFS_ROOM_INVITE_MESSAGE *)message;
			sprintf(messageDescription, "MFS_ROOM_INVITE, table = %d, inviterPlayerID = %d, toWatch = %d",
										(int)mfsRIM->table, (int)mfsRIM->inviterPlayerID, (int)mfsRIM->toWatch);
			break;
		case MTS_ROOM_RSVP:
			mtsRRSVPM = (MTS_ROOM_RSVP_MESSAGE *)message;
			sprintf(messageDescription, "MTS_ROOM_RSVP, accepted = %d", (int)mtsRRSVPM->accepted);
			break;
		
		// tournament messages
		
		case MTS_ROOM_TI_CHECK_OUT_PAGE:
			mtsTICOPM = (MTS_ROOM_TI_CHECK_OUT_PAGE_MESSAGE *)message;
			sprintf(messageDescription, "MTS_ROOM_TI_CHECK_OUT_PAGE, playerID = %d, pageNumber = %d",
										(int)mtsTICOPM->playerID, (int)mtsTICOPM->pageNumber);
			break;
		
		case MFS_ROOM_TI_CHECK_OUT_PAGE:
			mfsTICOPM = (MFS_ROOM_TI_CHECK_OUT_PAGE_MESSAGE *)message;
			sprintf(messageDescription, "MFS_ROOM_TI_CHECK_OUT_PAGE, playerID = %d, pageNumber = %d",
										(int)mfsTICOPM->playerID, (int)mfsTICOPM->pageNumber);
			break;
		
		case MTS_ROOM_TI_ADD_TO_TEXT:
			mtsTIATM = (MTS_ROOM_TI_ADD_TO_TEXT_MESSAGE *)message;
			sprintf(messageDescription, "MTS_ROOM_TI_ADD_TO_TEXT, playerID = %d, pageNumber = %d, clear = %d, text = %s",
										(int)mtsTIATM->playerID, (int)mtsTIATM->pageNumber, (int)mtsTIATM->clear, mtsTIATM->text);
			break;
		
		case MFS_ROOM_TI_ADD_TO_TEXT:
			mfsTIATM = (MFS_ROOM_TI_ADD_TO_TEXT_MESSAGE *)message;
			sprintf(messageDescription, "MFS_ROOM_TI_ADD_TO_TEXT, pageNumber = %d, clear = %d, text = %s",
										(int)mfsTIATM->pageNumber, (int)mfsTIATM->clear, mfsTIATM->text);
			break;
		
		case MTS_ROOM_TI_JOIN:
			mtsTIJM = (MTS_ROOM_TI_JOIN_MESSAGE *)message;
			sprintf(messageDescription, "MTS_ROOM_TI_JOIN, playerID = %d, joining = %d",
					(int)mtsTIJM->playerID, (int)mtsTIJM->joining);
			break;
		
		case MFS_ROOM_TI_JOIN:
			mfsTIJM = (MFS_ROOM_TI_JOIN_MESSAGE *)message;
			sprintf(messageDescription, "MFS_ROOM_TI_JOIN, playerID = %d, joining = %d, startingBankValue = %d",
					(int)mfsTIJM->playerID, (int)mfsTIJM->joining, (int)mfsTIJM->startingBankValue);
			break;
		
		case MTS_ROOM_TI_SET_BANK:
			mtsTISBM = (MTS_ROOM_TI_SET_BANK_MESSAGE *)message;
			sprintf(messageDescription, "MTS_ROOM_TI_SET_BANK, playerID = %d, bankValue = %d",
					(int)mtsTISBM->playerID, (int)mtsTISBM->bankValue);
			break;
		
		case MFS_ROOM_TI_SET_BANK:
			mfsTISBM = (MFS_ROOM_TI_SET_BANK_MESSAGE *)message;
			sprintf(messageDescription, "MFS_ROOM_TI_SET_BANK, playerID = %d, bankValue = %d",
					(int)mfsTISBM->playerID, (int)mfsTISBM->bankValue);
			break;
		
		case MTS_ROOM_TI_GENERATE_ASSIGNMENTS:
			mtsTIGAM = (MTS_ROOM_TI_GENERATE_ASSIGNMENTS_MESSAGE *)message;
			sprintf(messageDescription, "MTS_ROOM_TI_GENERATE_ASSIGNMENTS, playersPerTable = %d",
					(int)mtsTIGAM->playersPerTable);
			break;
		
		case MTS_ROOM_TI_CLEAR_AUTO_RESULTS:
			mtsTICARM = (MTS_ROOM_TI_CLEAR_AUTO_RESULTS_MESSAGE *)message;
			sprintf(messageDescription, "MTS_ROOM_TI_CLEAR_AUTO_RESULTS");
			break;
		
		// ratings messages
		case MFS_ROOM_PLAYER_RATING:
			mfsRPR = (MFS_ROOM_PLAYER_RATING_MESSAGE *)message;
			sprintf(messageDescription, "MTS_ROOM_PLAYER_RATING");
			break;

		// game-generic messages to/from players
		
		case MTS_GAME_STATE:
			mtsGSM = (MTS_GAME_STATE_MESSAGE *)message;
			sprintf(messageDescription, "MTS_GAME_STATE, playerID = %d, table = %d",
										(int)mtsGSM->playerID, (int)mtsGSM->table);
			break;
		case MFS_GAME_STATE:
			mfsGSM = (MFS_GAME_STATE_MESSAGE *)message;
			sprintf(messageDescription, "MFS_GAME_STATE, table = %d, status = %d, gamePhase = %d",
										(int)mfsGSM->table, (int)mfsGSM->status, (int)mfsGSM->gameState.gamePhase);
			break;
		case MTS_GAME_CHAT:
			mtsGCM = (MTS_GAME_CHAT_MESSAGE *)message;
			sprintf(messageDescription, "MTS_GAME_CHAT, playerID = %d, table = %d, seat = %d, flags = %d, text = %s",
										(int)mtsGCM->playerID, (int)mtsGCM->table, (int)mtsGCM->seat, (int)mtsGCM->flags, mtsGCM->text);
			break;
		case MFS_GAME_CHAT:
			mfsGCM = (MFS_GAME_CHAT_MESSAGE *)message;
			sprintf(messageDescription, "MFS_GAME_CHAT, playerID = %d, table = %d, seat = %d, flags = %d, text = %s",
										(int)mfsGCM->playerID, (int)mfsGCM->table, (int)mfsGCM->seat, (int)mfsGCM->flags, mfsGCM->text);
			break;
		case MFS_GAME_DISPLAY_STRING:
			mfsGDSM = (MFS_GAME_DISPLAY_STRING_MESSAGE *)message;
			sprintf(messageDescription, "MFS_GAME_DISPLAY_STRING, table = %d, title = %s, text = %s",
										(int)mfsGDSM->table, mfsGDSM->title, mfsGDSM->text);
			break;
		case MTS_GAME_BOOT:
			mtsGBM = (MTS_GAME_BOOT_MESSAGE *)message;
			sprintf(messageDescription, "MTS_GAME_BOOT, table = %d, playerID = %d, seat = %d",
										(int)mtsGBM->table, (int)mtsGBM->playerID, (int)mtsGBM->seat);
			break;
		case MFS_GAME_BOOT:
			mfsGBM = (MFS_GAME_BOOT_MESSAGE *)message;
			sprintf(messageDescription, "MFS_GAME_BOOT, table = %d, playerID = %d, seat = %d, bootCount = %d",
										(int)mfsGBM->table, (int)mfsGBM->playerID, (int)mfsGBM->seat, (int)mfsGBM->bootCount);
			break;
		case MTS_GAME_RESTART:
			mtsGRM = (MTS_GAME_RESTART_MESSAGE *)message;
			sprintf(messageDescription, "MTS_GAME_RESTART, table = %d, playerID = %d, seat = %d",
										(int)mtsGRM->table, (int)mtsGRM->playerID, (int)mtsGRM->seat);
			break;
		case MFS_GAME_RESTART:
			mfsGRM = (MFS_GAME_RESTART_MESSAGE *)message;
			sprintf(messageDescription, "MFS_GAME_RESTART, table = %d", (int)mfsGRM->table);
			break;
		
		// game-specific messages to/from players
		
		default:
			GetGameMessageDescription(mh->messageType, message, messageDescription);
			break;
	}
	
	return messageDescription;
}

