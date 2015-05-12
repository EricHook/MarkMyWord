#include "miniwin.h"
#include "DoubleCrossServer.h"

//////////////////////////////////////////////////////////////////////////////////////////
// generic functions for all room server tables

DoubleCrossServerTable::DoubleCrossServerTable()
{
	m_engine = NULL;
	
	m_wordlist = NULL;
	m_altWordList = NULL;
	m_altaltWordList = NULL;
}

DoubleCrossServerTable::~DoubleCrossServerTable()
{
	if (m_engine) { delete m_engine; m_engine = NULL; }
	
	if (m_wordlist) { delete m_wordlist; m_wordlist = NULL; }
	if (m_altWordList) { delete m_altWordList; m_altWordList = NULL; }
	if (m_altaltWordList) { delete m_altaltWordList; m_altaltWordList = NULL; }
}

void DoubleCrossServerTable::Init(int theIndex)
{
	index = theIndex;
	status = TS_EMPTY;
	joinOption = TJO_OPEN;
	watchersOption = TWO_OPEN;
	muteWatchers = FALSE;
	description[0] = 0;
	
	if (!m_engine)
		m_engine = new DoubleCrossEngine();
	
	m_engine->Init(&gameState);
	
	m_special = 0;
	
	m_nukeOrigin.x = m_nukeOrigin.y = -1;
	m_nukeType = DCBT_EMPTY;
	m_nukeColor = DBLCROSS_NONE;
	
	m_numOrphans = 0;
}

void DoubleCrossServerTable::InitPlayer(int seat)
{
	m_engine->InitPlayer(seat);
}

void DoubleCrossServerTable::InitSeat(int seat)
{
	m_engine->InitSeat(seat);
}

void DoubleCrossServerTable::PlayerSits(DoubleCrossServerPlayer *client, PlayerState *playerState)
{
	int watcherIndex, seat = client->info.seat;
	
	if (status == TS_EMPTY)
		status = TS_PENDING;
	
	if (seat < MAX_GAME_PLAYERS) {
		// add a regular player
		ASSERT(_WARN_, gameState.players[seat].playerID == PLAYER_ID_EMPTY);
		if (gameState.players[seat].playerID != PLAYER_ID_VOID) {
			gameState.players[seat].playerID = client->info.playerID;
			gameState.players[seat].status = DCPS_ACTIVE;
		}
		
		if (playerState)
			(*playerState) = gameState.players[seat];
	}
	else {
		// add a watcher
		watcherIndex = seat - MAX_GAME_PLAYERS;
		ASSERT(_WARN_, gameState.watcherIDs[watcherIndex] == PLAYER_ID_EMPTY);
		gameState.watcherIDs[watcherIndex] = client->info.playerID;
	}
}

void DoubleCrossServerTable::PlayerStands(DoubleCrossServerPlayer *client, int *newID, Boolean booted /* = FALSE */)
{
	int seat, playerID, numPlayers, numWatchers;
	
	*newID = PLAYER_ID_EMPTY;
	
	numPlayers = 0;
	for (seat = 0 ; seat < MAX_GAME_PLAYERS ; seat++)
		if (gameState.players[seat].playerID != PLAYER_ID_EMPTY && gameState.players[seat].playerID != PLAYER_ID_VOID)
			numPlayers++;
	
	numWatchers = 0;
	for (seat = 0 ; seat < MAX_GAME_WATCHERS ; seat++)
		if (gameState.watcherIDs[seat] != PLAYER_ID_EMPTY)
			numWatchers++;
	
	for (seat = 0 ; seat < MAX_GAME_PLAYERS ; seat++)
		if (gameState.players[seat].playerID == client->info.playerID)
			break;
	
	if (seat == MAX_GAME_PLAYERS) {
		for (seat = 0 ; seat < MAX_GAME_WATCHERS ; seat++)
			if (gameState.watcherIDs[seat] == client->info.playerID)
				break;
		if (seat == MAX_GAME_WATCHERS)
			{ ASSERT(_WARN_, FALSE); return; } // watcher not found at table
		
		gameState.watcherIDs[seat] = PLAYER_ID_EMPTY;
		
		if (numWatchers == 1) // player was the last watcher
			if (numPlayers == 0)
				Init(index);
		
		return;
	}
	
	// watchers have already been taken care of, so this is only for the players
	
	if (numPlayers == 1)
		{ Init(index); return; }
	
	playerID = gameState.players[seat].playerID;
	
	gameState.players[seat].playerID = PLAYER_ID_EMPTY;
	gameState.players[seat].status = DCPS_EMPTY;
	
	if (status == TS_PLAYING)
		RequestDOUBLECROSSREPLACEPLAYER(seat, playerID);
	
	if (gameState.gamePhase == DCGP_SHOWDOWN) {
		gameState.players[seat].doIt = FALSE;
		gameState.players[seat].didIt = FALSE;
	}
}

void DoubleCrossServerTable::StartGame()
{
	int i, err;
	MFS_ROOM_SIT_MESSAGE mfs;
	
	ASSERT(_WARN_, status == TS_PENDING);
	
	status = TS_PLAYING;
	gameState.gamePhase = DCGP_NOTHING;
	
	// remove the empty spots from play
	
	for (i = 0 ; i < MAX_GAME_PLAYERS ; i++)
		if (gameState.players[i].playerID == PLAYER_ID_EMPTY) {
			gameState.players[i].playerID = PLAYER_ID_VOID;
			
			mfs.messageType = MFS_ROOM_SIT;
			mfs.playerID = PLAYER_ID_VOID;
			mfs.table = index;
			mfs.seat = i;
			mfs.playerState = gameState.players[i];
			
			err = roomServer->BroadcastRoomMessage((char *)&mfs, sizeof(MFS_ROOM_SIT_MESSAGE));
			
			ASSERT(_WARN_, err == 0);
		}
}

void DoubleCrossServerTable::RestartGame()
{
	char mfs[MAX_MSG];
	int i, length, err;
	
	TRACE(_ROUTINE_ "DoubleCrossServerTable::RestartGame()\n");
	
	for (i = 0 ; i < MAX_GAME_PLAYERS ; i++) {
		gameState.players[i].doIt = FALSE;
		gameState.players[i].didIt = FALSE;
	}
	
	RequestDOUBLECROSSSHOWDOWN(mfs, &length);
	
	err = roomServer->BroadcastGameMessage(index, (char *)mfs, length);
}

int DoubleCrossServerTable::GetEmptyWatcherSeat()
{
	int seat;
	
	for (seat = 0 ; seat < MAX_GAME_WATCHERS ; seat++)
		if (gameState.watcherIDs[seat] == PLAYER_ID_EMPTY)
			return MAX_GAME_PLAYERS + seat;
	
	return -1;
}

void DoubleCrossServerTable::GetTableGameValues(int *values)
{
	char *data;
	
	data = (char *)values;
	
	data[0] = gameState.options.haveColoredBombs;
	data[1] = gameState.options.haveDirectionalBombs;
	data[2] = gameState.options.haveBonusTiles;
	data[3] = gameState.options.haveBlockingTiles;
	data[4] = gameState.options.gridMode;
	data[5] = gameState.options.haveMysteryTiles;
	data[6] = gameState.options.allowOffensiveWords;
	data[7] = gameState.options.timeLimit;
	data[8] = gameState.options.minWordSize;
}

void DoubleCrossServerTable::SetTableGameValues(int *values)
{
	char *data;
	
	data = (char *)values;
	
	gameState.options.haveColoredBombs = data[0];
	gameState.options.haveDirectionalBombs = data[1];
	gameState.options.haveBonusTiles = data[2];
	gameState.options.haveBlockingTiles = data[3];
	gameState.options.gridMode = data[4];
	gameState.options.haveMysteryTiles = data[5];
	gameState.options.allowOffensiveWords = data[6];
	gameState.options.timeLimit = data[7];
	gameState.options.minWordSize = data[8];
	
	if (gameState.options.haveColoredBombs != TRUE && gameState.options.haveColoredBombs != FALSE)
		gameState.options.haveColoredBombs = TRUE;
	
	if (gameState.options.haveDirectionalBombs != TRUE && gameState.options.haveDirectionalBombs != FALSE)
		gameState.options.haveDirectionalBombs = FALSE;
	
	if (gameState.options.haveBonusTiles != TRUE && gameState.options.haveBonusTiles != FALSE)
		gameState.options.haveBonusTiles = TRUE;
	
	if (gameState.options.haveBlockingTiles != TRUE && gameState.options.haveBlockingTiles != FALSE)
		gameState.options.haveBlockingTiles = FALSE;
	
	if (gameState.options.gridMode != TRUE && gameState.options.gridMode != FALSE)
		gameState.options.gridMode = FALSE;
	
	if (gameState.options.haveMysteryTiles != TRUE && gameState.options.haveMysteryTiles != FALSE)
		gameState.options.haveMysteryTiles = TRUE;
	
	if (gameState.options.allowOffensiveWords != TRUE && gameState.options.allowOffensiveWords != FALSE)
		gameState.options.allowOffensiveWords = FALSE;
	
	if (gameState.options.timeLimit < 30)
		gameState.options.timeLimit = 30;
	
	if (gameState.options.timeLimit > 99)
		gameState.options.timeLimit = 99;
	
	if (gameState.options.minWordSize != 3 && gameState.options.minWordSize != 4)
		gameState.options.minWordSize = 3;
}

void DoubleCrossServerTable::SetAutoResult()
{
	char s[256], s2[64], winnerNames[4][MAX_PLAYER_NAME], loserNames[4][MAX_PLAYER_NAME];
	int i, bestScore, scores[4], winners[4], losers[4], numWinners = 0, numLosers = 0;
	DoubleCrossServerPlayer *p;
	
	s[0] = 0;
	
	for (i = 0 ; i < MAX_GAME_PLAYERS ; i++) {
		winners[i] = -1;
		losers[i] = -1;
	}
	
	for (i = 0, bestScore = -1 ; i < MAX_GAME_PLAYERS ; i++)
		if (gameState.players[i].playerID != PLAYER_ID_VOID) {
			scores[i] = gameState.seats[i].score;
			if (scores[i] > bestScore)
				bestScore = scores[i];
		}
	
	for (i = 0 ; i < MAX_GAME_PLAYERS ; i++)
		if (gameState.players[i].playerID != PLAYER_ID_VOID) {
			p = roomServer->GetClientFromID(gameState.players[i].playerID);
			if (p == NULL)
				ASSERT(_WARN_, FALSE);
			
			if (scores[i] == bestScore) {
				strcpy(winnerNames[numWinners], (p == NULL) ? "???" : p->info.login);
				winners[numWinners] = i;
				numWinners++;
			}
			else {
				strcpy(loserNames[numLosers], (p == NULL) ? "???" : p->info.login);
				losers[numLosers] = i;
				numLosers++;
			}
		}
	
	ASSERT(_WARN_, numWinners >= 1);
	
	if (numWinners == 1)
		sprintf(s, "%s (%d) ", winnerNames[0], scores[winners[0]]);
	else
		for (i = 0 ; i < numWinners ; i++) {
			sprintf(s2, "%s%s (%d)%s", (i == numWinners - 1) ? "and " : "", winnerNames[i], scores[winners[i]], (i != numWinners - 1) ? ", " : " ");
			strcat(s, s2);
		}
	
	if (numLosers > 0) {
		strcat(s, "beat ");
		
		for (i = 0; i < numLosers; i++) {
			sprintf(s2, "%s%s (%d)%s", (i == numLosers - 1) ? "and " : "", loserNames[i], scores[losers[i]], (i != numLosers - 1) ? ", " : ".");
			strcat(s, s2);
		}
	}
	else {
		if (numWinners == 1)
			strcat(s, "finished a game, but had no opponents!");
		else
			strcat(s, "tied.");
	}
	
	roomServer->AddAutomaticResultsLine(index, s);
}

//////////////////////////////////////////////////////////////////////////////////////////
// game-specific utility functions similar to functions in other games

int DoubleCrossServerTable::GetNextDOUBLECROSSPlayer(int p)
{
	int i;
	
	for (i = 0 ; i < (MAX_GAME_PLAYERS + 1) ; i++) {
		p++;
		if (p >= MAX_GAME_PLAYERS)
			p = 0;
		
		if (gameState.players[p].playerID != PLAYER_ID_VOID)
			return p;
	}
	
	ASSERT(_WARN_, FALSE);
	
	return p;
}

void DoubleCrossServerTable::ResetDOUBLECROSSGAME()
{
	int i;
	MFS_DOUBLECROSS_RESETGAME_MESSAGE mfs;
	
	gameState.gameNumber++;
	m_engine->ResetForNewGame();
	
	for (i = 0 ; i < MAX_GAME_PLAYERS ; i++) {
		m_passedWithoutPlayingOnPreviousTurn[i] = FALSE;
		m_hasPlayedSinceLastDump[i] = TRUE;
	}
	
	m_special = -1;
	m_numOrphans = 0;
	
	SetupGame();
	
	mfs.messageType = MFS_DOUBLECROSS_RESETGAME;
	mfs.table = index;
	mfs.gameState = gameState;
	
	roomServer->BroadcastGameMessage(index, (char *)&mfs, sizeof(MFS_DOUBLECROSS_RESETGAME_MESSAGE));
}

bool DoubleCrossServerTable::GameIsOver()
{
	if (gameState.currentPlayer == -1)
		return false;
	
	// if it's a player's turn and he has no tiles and the store is empty
	
	if (m_engine->IsRackEmpty(gameState.currentPlayer) && gameState.currentStoreSize == 0)
		return true;
	
	return false;
}

void DoubleCrossServerTable::PossiblySuggestReset()
{
	int i;
	
	for (i = 0 ; i < MAX_GAME_PLAYERS ; i++)
		if (gameState.players[i].status == DCPS_ACTIVE)
			if (!m_passedWithoutPlayingOnPreviousTurn[i])
				break;
	
	if (i == MAX_GAME_PLAYERS)
		roomServer->BroadcastGameStringMessage(index, "Reset Game?", "All players passed on the previous round.  If you wish to end the game, click your player and choose Reset Game.");
}

//////////////////////////////////////////////////////////////////////////////////////////
// setup the DoubleCross game board

void DoubleCrossServerTable::SetupGame()
{
	int i;
	
	m_numOrphans = 0;
	
	// Set up wordlist
	// Set up alternate wordlists, which are used for checking crosswords
	// without disrupting the data in the original wordlist
	
	if (!m_wordlist)
		m_wordlist = new DblCrossPartWordList(this);
	if (!m_altWordList)
		m_altWordList = new DblCrossPartWordList(this);
	if (!m_altaltWordList)
		m_altaltWordList = new DblCrossPartWordList(this);
	
	CreateTiles();
	
	SetupSeedWord();
	
	for (i = 0 ; i < MAX_GAME_PLAYERS ; i++)
		if (gameState.players[i].status == DCPS_ACTIVE)
			FillRack(i);
	
	CreateSpecialTiles();
}

void DoubleCrossServerTable::CreateTiles()
{
	char ind2, val;
	int i, j;
	static char tileStore[DBLCROSS_TILESTORESIZE + 1] = {
			"AAAAAAAAAABBBCCCDDDDDEEEEEEEEEEEEEFFGGGGHHHIIIIIIIIIIJKKLLLL"
			"MMMMNNNNNNOOOOOOOOOPPPQRRRRRRRSSSSSSTTTTTTTUUUUVVWWXYYYZ" };
	
	memcpy(m_tileStore, tileStore, DBLCROSS_TILESTORESIZE);
	
	// shuffle
	
	roomServer->randomGenerator->Seed((int)timeGetTime());
	for (j = 0 ; j < 4 ; j++)
		for (i = 0 ; i < DBLCROSS_TILESTORESIZE ; i++) {
			ind2 = roomServer->randomGenerator->GetRandom(DBLCROSS_TILESTORESIZE);
			val = m_tileStore[i];
			m_tileStore[i] = m_tileStore[ind2];
			m_tileStore[ind2] = val;		
		}
	
	gameState.currentStoreSize = DBLCROSS_TILESTORESIZE;
}

void DoubleCrossServerTable::SetupSeedWord()
{
	bool found = false;
	char *word, s[256];
	
	while (!found) {
		word = roomServer->dictionary->GetRandomWord(5, 5, WGDICT_ISMEDIUM);
		strcpy(s, word);
		strupr(s);
		found = FindAndPurgeSeedWord(s); // Find tiles for this word and remove from the tile Store
	}
	
	if (roomServer->randomGenerator->GetRandom(2) == 1)
		AddSeedWordToBoard(s, m_engine->MakeDCPoint(5, 7), DBLCROSS_HORIZONTAL);
	else
		AddSeedWordToBoard(s, m_engine->MakeDCPoint(7, 5), DBLCROSS_VERTICAL);
}

bool DoubleCrossServerTable::FindAndPurgeSeedWord(char *word)
{
	char *p, tileStore[DBLCROSS_TILESTORESIZE];
	int i, j, length;
	
	// find the word in the store, eliminating the tiles
	
	length = strlen(word);
	memcpy(tileStore, m_tileStore, DBLCROSS_TILESTORESIZE);
	
	for (i = 0 ; i < length ; i++) {
		// Find the letter in the store
		p = strchr(tileStore, word[i]);
		if (!p)
			return false;
		
		*p = '-';
	}
	
	// shift these tiles to the (used) start of the tileStore
	
	strcpy(m_tileStore, word);
	for (i = 0, j = length ; i < DBLCROSS_TILESTORESIZE ; i++)
		if (tileStore[i] != '-')
			m_tileStore[j++] = tileStore[i];
	
	gameState.currentStoreSize -= length;
	
	return true;
}

void DoubleCrossServerTable::AddSeedWordToBoard(char *word, DCPoint pt, DblCrossDirection dir)
{
	int i;
	DCPoint square;
	static DCPoint delta[] = { { 0, 1 }, { 1, 0 } };
	
	for (square.x = pt.x, square.y = pt.y, i = 0;
		 i < (signed)strlen(word) && square.x < DBLCROSS_BOARDWIDTH && square.y < DBLCROSS_BOARDHEIGHT;
		 i++, square.x += delta[dir - 1].x, square.y += delta[dir - 1].y) {
		m_engine->PlayLetterOnSquare(DBLCROSS_SEED_PLAYER, word[i], square);
		m_engine->StabilizeSquare(square, 0);
	}
}

void DoubleCrossServerTable::CreateSpecialTiles()
{
	bool gotTile;
	char type = 0;
	int i, value = 0;
	DCPoint	square;
	static int mysVal[] = { 10, 15, 25, 35, 0, DCMYS_CURBONUS, -10, -25, DCMYS_DOUBLER, 
							DCMYS_FILLRACK, DCMYS_BLACKHOLE, DCMYS_BLACKHOLE, DCMYS_BLACKHOLE, 
							DCMYS_REDBOMB, DCMYS_YELLOWBOMB, DCMYS_BLUEBOMB, DCMYS_GREENBOMB, 
							DCMYS_NSBOMB, DCMYS_EWBOMB, DCMYS_NESWBOMB };
	static DCPoint corners[] = { { 0, 0 },
								 { DBLCROSS_BOARDWIDTH - 1, 0 },
								 { DBLCROSS_BOARDWIDTH - 1, DBLCROSS_BOARDHEIGHT - 1 },
								 { 0, DBLCROSS_BOARDHEIGHT - 1 } };
	
	if (gameState.options.haveBlockingTiles && gameState.options.gridMode)
		for (square.x = 2 ; square.x <= 12 ; square.x += 2)
			for (square.y = 2 ; square.y <= 12 ; square.y += 2)
				m_engine->SetSquareSpecial(square, DCSS_BLOCK);
	
	for (i = 0 ; i < DBLCROSS_NUMSPECTILES ; i++) {
		// pick a special tile of the appropriate kind
		
		gotTile = false;
		
		// colored bombs
		if (i < 4) {
			if (gameState.options.haveColoredBombs) {
				// Create color bombs, one per player
				if (gameState.players[i].status != DCPS_ACTIVE)
					continue;
				type = (char)(DCBT_RED + (gameState.seats[i].color - 1));
				gotTile = true;
			}
		} 
		
		// directional bombs (2 vertical, 2 horizontal, 1 unidirectional)
		else if (i < 9) {
			if (gameState.options.haveDirectionalBombs) {
				type = (char)(DCBT_NORTH_SOUTH + ((i - 4) / 2));
				gotTile = true;
			}
		}
		
		// blocks (4 of them)
		else if (i < 13) {
			if (gameState.options.haveBlockingTiles && !gameState.options.gridMode) {
				type = DCSS_BLOCK;
				gotTile = true;
			}
		} 
		
		// bonus
		else if (i < 21) {
			if (gameState.options.haveBonusTiles) {
				type = DCSS_BONUS;
				gotTile = true;
			}
		} 
		
		// mystery (6 of them)
		else if (gameState.options.haveMysteryTiles) {
			type = DCSS_MYSTERY;
			
			// loop until we pick a good mystery value
			for ( ; ; ) {
				if (i == 21)
					value = 8;	//DCMYS_DOUBLER index in mysVal
				else if (i == 24)
					value = 9;	// DCMYS_FILLRACK index in mysVal
				else
					value = roomServer->randomGenerator->GetRandom(20);
				
				// make it hard to get a colored bomb
				if (gameState.options.haveColoredBombs && mysVal[value] >= DCMYS_REDBOMB
					&& mysVal[value] <= DCMYS_GREENBOMB) {
					if (roomServer->randomGenerator->GetRandom(3) != 1)
						continue;
				}
				
				// Debug code
				// value = DCMYS_NSBOMB;
				
				// don't choose a type that is not enabled
				if ((!gameState.options.haveColoredBombs && mysVal[value] >= DCMYS_REDBOMB
						&& mysVal[value] <= DCMYS_GREENBOMB) ||
					(!gameState.options.haveDirectionalBombs && mysVal[value] >= DCMYS_NSBOMB
						&& mysVal[value] <= DCMYS_NESWBOMB) ||
					(!gameState.options.haveBlockingTiles && mysVal[value] == DCMYS_BLACKHOLE))
					continue;
				
				// don't choose a colored bomb is the given player color is not present
				if (gameState.options.haveColoredBombs &&  mysVal[value] >= DCMYS_REDBOMB
						&& mysVal[value] <= DCMYS_GREENBOMB && 
						gameState.players[mysVal[value] - DCMYS_REDBOMB].status != DCPS_ACTIVE)
					continue;
				
				break;
			}
			
			gotTile = true;			
		}
		
		// place the chosen tile on the board, moving it a bit if necessary
		
		if (gotTile) {
			// colored bombs go in corners
			if (i < 4) {
				square.x = corners[i].x;
				square.y = corners[i].y;
			}
			
			// first tile of a given type
			else if (i == 4 || i == 9 || i == 13 || i == 21) {
				do {
					square.x = roomServer->randomGenerator->GetRandom(DBLCROSS_BOARDWIDTH);
					square.y = roomServer->randomGenerator->GetRandom(DBLCROSS_BOARDHEIGHT);
				} while (!IsSquareGoodForSpecialTile(square, type));
			}
			else {
				do {				
					square.x += roomServer->randomGenerator->GetRandom(DBLCROSS_BOARDWIDTH / 3) + 3;
					square.y += roomServer->randomGenerator->GetRandom(DBLCROSS_BOARDHEIGHT / 3) + 3;
					if (square.x >= DBLCROSS_BOARDWIDTH)
						square.x -= DBLCROSS_BOARDWIDTH;
					if (square.y >= DBLCROSS_BOARDHEIGHT)
						square.y -= DBLCROSS_BOARDHEIGHT;			
				} while (!IsSquareGoodForSpecialTile(square, type));
			}
			
			m_engine->SetSquareSpecial(square, type);
			if (type == DCSS_MYSTERY) {
				m_mysteryPoints[i - 21] = square;
				m_mysteryValues[i - 21] = mysVal[value];
			}
		}
	}
}

bool DoubleCrossServerTable::IsSquareGoodForSpecialTile(DCPoint square, int type)
{
	int i;
	DCPoint	spot;
	static DCPoint delta[] = { { 0, 1 }, { 1, 0 }, { -1, 0 }, { 0, -1 } };
	
	// we don't want to place a special tile in a spot if its immediate
	// neighbor in any direction is the same type of block
	
	if (!m_engine->IsSquareEmpty(square))
		return false;
	
	for (i = 0 ; i < 4 ; i++) {
		spot.x = square.x + delta[i].x;
		spot.y = square.y + delta[i].y;
		if (spot.x < 0)
			continue;
		if (spot.y < 0)
			continue;
		if (spot.x >= DBLCROSS_BOARDWIDTH)
			continue;
		if (spot.y >= DBLCROSS_BOARDHEIGHT)
			continue;
		if (m_engine->IsSquareEmpty(spot))
			continue;
		if (m_engine->GetSquareSpecial(spot) == type ||
			m_engine->GetSquareOwner(spot) != -1)
			return false;
	}
	
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// handle the various events

void DoubleCrossServerTable::PlaceBomb(int seat, char bombType, char rackSpot, DCPoint square, MFS_DOUBLECROSS_EVENT_MESSAGE *mfs)
{
	static char bombDirs[][20] = { "Vertical", "Horizontal", "Vertical/Horizontal" };
	
	// Place bomb, destroy appropriate tiles
	
	if (bombType < DCBT_NORTH_SOUTH || bombType > DCBT_NESW)
		{ ASSERT(_WARN_, FALSE); return; }
	
	if (rackSpot < 0 || rackSpot >= DBLCROSS_BOMBRACKSIZE)
		{ ASSERT(_WARN_, FALSE); return; }
	
	if (gameState.seats[seat].bombs[rackSpot] != bombType)
		{ ASSERT(_WARN_, FALSE); return; }
	
	gameState.seats[seat].bombs[rackSpot] = '-';
	
	ClearBombData();
	
	// Put in bomb move first
	mfs->who = seat;
	mfs->eventType = DCMOVE_PLACE_BOMB;
	mfs->tileValue = bombType;
	mfs->rackSpot = rackSpot;
	mfs->start = square;
	mfs->finish = square;
	mfs->comment[0] = 0;
	
	roomServer->BroadcastGameMessage(index, (char *)mfs, sizeof(MFS_DOUBLECROSS_EVENT_MESSAGE) + strlen(mfs->comment) + 1);
	
	// nuke appropriate tiles
	
	// First show the explosion effects
	mfs->who = seat;
	mfs->eventType = DCMOVE_DIREXPLOSION;
	mfs->tileValue = bombType;
	mfs->start = square;
	mfs->finish = square;
	sprintf(mfs->comment, "%s Bomb Played.", bombDirs[bombType - DCBT_NORTH_SOUTH]);
	
	roomServer->BroadcastGameMessage(index, (char *)mfs, sizeof(MFS_DOUBLECROSS_EVENT_MESSAGE) + strlen(mfs->comment) + 1);
	
	m_nukeOrigin = square;
	m_nukeType = bombType;
	
	strcpy(mfs->comment, "");

	// Now figure out which ones to destroy
	if (bombType == DCBT_EAST_WEST || bombType == DCBT_NESW)
		NukeLine(DBLCROSS_HORIZONTAL, square, bombType == DCBT_NESW, mfs);
	
	if (bombType == DCBT_NORTH_SOUTH || bombType == DCBT_NESW)
		NukeLine(DBLCROSS_VERTICAL, square, bombType == DCBT_NESW, mfs);
}

void DoubleCrossServerTable::NukeLine(DblCrossDirection dir, DCPoint square, bool bothDirs, MFS_DOUBLECROSS_EVENT_MESSAGE *mfs)
{
	DCPoint spot, step, stepR;
	
	// Destroy all the unstable tiles and orphans in the given direction
	// (Result of placing non-colored bomb)
	
	if (dir == DBLCROSS_VERTICAL) {
		step = m_engine->MakeDCPoint(0, 1);
		stepR = m_engine->MakeDCPoint(1, 0);
	}
	else {
		step = m_engine->MakeDCPoint(1, 0);
		stepR = m_engine->MakeDCPoint(0, 1);
	}
	
	// Check left/up first, nuke anything unstable in your path
	
	for (spot = square ; spot.x >= 0 && spot.y >= 0 ; spot.x -= step.x, spot.y -= step.y)
		if (m_engine->GetSquareOwner(spot) != -1)
			if (!m_engine->IsSquareStable(spot)) {
				ExplodeTiles(spot, stepR, step, true, true, false, -1, mfs);
				
				// Now recheck that we don't have more words to create/points to give
				RecheckForWords(mfs);
			}
	
	// Now check right/down
	
	for (spot.x = square.x + step.x, spot.y = square.y + step.y ;
		 spot.x < DBLCROSS_BOARDWIDTH && spot.y < DBLCROSS_BOARDHEIGHT ;
		 spot.x += step.x, spot.y += step.y)
		if (m_engine->GetSquareOwner(spot) != -1)
			if (!m_engine->IsSquareStable(spot)) {
				ExplodeTiles(spot, stepR, step, true, true, false, -1, mfs);
				
				// Now recheck that we don't have more words to create/points to give
				RecheckForWords(mfs);
			}
}

bool DoubleCrossServerTable::PlaceTile(int seat, char letter, char rackSpot, DCPoint square, MFS_DOUBLECROSS_EVENT_MESSAGE *mfs)
{
	bool continueTurn;
	
	// return false if this ends the player's turn
	
	// to start, just put it in the board, and move the tile appropriately from the rack
	
	if (letter < 'A' || letter > 'Z')
		{ ASSERT(_WARN_, FALSE); return false; }
	
	if (rackSpot < 0 || rackSpot >= DBLCROSS_RACKSIZE)
		{ ASSERT(_WARN_, FALSE); return false; }
	
	if (gameState.seats[seat].rack[rackSpot] != letter)
		{ ASSERT(_WARN_, FALSE); return false; }
	
	if (m_engine->GetSquareOwner(square) != -1)
		{ ASSERT(_WARN_, FALSE); return false; }
	
	gameState.seats[seat].rack[rackSpot] = '-';
	m_special = m_engine->GetSquareSpecial(square);
	if (m_special == DCBT_EMPTY)
		m_special = -1;
	
	m_engine->PlayLetterOnSquare(seat, letter, square);
	
	// check if play is OK (legal partial word) and create its consequences
	
	continueTurn = ProcessMove(seat, letter, rackSpot, square, mfs);
	
	return continueTurn && !m_engine->IsRackEmpty(seat);
}

bool DoubleCrossServerTable::ProcessMove(int seat, char letter, char rackSpot, DCPoint square, MFS_DOUBLECROSS_EVENT_MESSAGE *mfs)
{
	bool result;
	int mysteryValue;
	DblCrossColors color, bombColor;
	DblCrossPartWord *wordV = NULL, *wordH = NULL;
	
	// return false if this ends the player's turn
	
	ClearBombData();
	
	// here we assume that the board has eliminated moves that are otherwise illegal
	
	// first, check if this is a legal move (forms a legal partial word in all directions)
	
	m_wordlist->Reset();
	if (!m_wordlist->CreateWordList(square, DBLCROSS_NODIR))
		return false;
	
	result = ShortestPartialsDefined(square, m_wordlist);
	
	///////////////////////////// send the tile move event
	
	mfs->who = seat;
	mfs->eventType = DCMOVE_MOVE_TILE;
	mfs->tileValue = letter;
	mfs->rackSpot = rackSpot;
	mfs->start = square;
	mfs->finish = square;
	mfs->mysteryValue = DCMYS_EMPTY;
	mfs->score = 0;
	
	if (result)
		CreatePartWordComment(mfs->comment, result, NULL);
	else
		mfs->comment[0] = 0;
	
	roomServer->BroadcastGameMessage(index, (char *)mfs, sizeof(MFS_DOUBLECROSS_EVENT_MESSAGE) + strlen(mfs->comment) + 1);
	
	if (!result) {
		// Explode this tile - note that the second and third params are not used
		// if the 5th param is false
		CreatePartWordComment(mfs->comment, false, NULL);
		ExplodeTiles(square, square, square, true, false, false, m_special, mfs);
		
		return false;
	}
	
	/////////////////////////////
	// SPECIAL CASES:
	// Check if we have hit a black hole or set off a colored bomb.  Need to do this here
	// because these (could) explode even the tile we are playing.
	
	if (m_special >= 0) {
		if (m_special == DCSS_MYSTERY)
			mysteryValue = GetMysteryValue(square);
		else
			mysteryValue = 0;
		
		// colored bomb (or colored bomb under a mystery tile)
		if ((m_special >= DCBT_RED && m_special <= DCBT_GREEN) ||
				(m_special == DCSS_MYSTERY && mysteryValue >= DCMYS_REDBOMB && mysteryValue <= DCMYS_GREENBOMB)) {
			color = gameState.seats[seat].color;
			if (mysteryValue)
				bombColor = (DblCrossColors)((mysteryValue - (int)DCMYS_REDBOMB) + 1);
			else
				bombColor = (DblCrossColors)((m_special - (int)DCBT_RED) + 1);
			
			// score it
			if (color != bombColor) {
				gameState.seats[seat].score += DCPOINTS_GETCOLORBOMB * gameState.doublerFactor;
				m_nukeColor = bombColor;
			}
			else
				gameState.seats[seat].score += DCPOINTS_DEFUSE * gameState.doublerFactor;
			
			ExplodeColorTiles(seat, square, color == bombColor, bombColor, mfs);
			
			// did the played tile get destroyed?
			if (m_engine->IsSquareEmpty(square))
				return true; // losing your tile because you hit another player's colored bomb does NOT end your turn
		}
		
		// black hole under a mystery tile
		if (m_special == DCSS_MYSTERY && mysteryValue == DCMYS_BLACKHOLE) {
			m_engine->ClearSquare(square, DCPOINTS_EXPLODE * gameState.doublerFactor);
			
			// add mystery animation to moves list
			mfs->who = seat;
			mfs->eventType = DCMOVE_MYSTERY;
			mfs->tileValue = letter;
			mfs->rackSpot = rackSpot;
			mfs->start = square;
			mfs->finish = square;
			mfs->score = gameState.seats[seat].score;
			mfs->mysteryValue = DCMYS_BLACKHOLE;
			strcpy(mfs->comment, "Black Hole: Lose your tile and your turn!");
			
			roomServer->BroadcastGameMessage(index, (char *)mfs, sizeof(MFS_DOUBLECROSS_EVENT_MESSAGE) + strlen(mfs->comment) + 1);
			
			return false;
		}
	}
	
	///////////////////////////// check if there are any tiles to stabilize
	
	wordH = FindLongestLegalWord(DBLCROSS_HORIZONTAL, m_wordlist);
	wordV = FindLongestLegalWord(DBLCROSS_VERTICAL, m_wordlist);
	UpdateBoard(seat, wordH, wordV, square, rackSpot, mfs);
	
	/*
	// Flag that we have a tile involved in two words
	moves->m_twoWordTile.x = state->twoWordTile.x;
	moves->m_twoWordTile.y = state->twoWordTile.y;
	state->twoWordTile.x = state->twoWordTile.y = -1;
	
	// Points for a tile involved in two words must include pts for both words
	// Even though it only gets stabilized once
	if ( hword && vword && moves->m_twoWordTile.x != -1 ) {
		moves->m_specialPoints = hword->GetLength() - m_server->m_minWordSize + 1
				+ vword->GetLength() - m_server->m_minWordSize + 1;
	}
	else {
		moves->m_specialPoints = 0;
	}
	*/
	
	return true;
}

void DoubleCrossServerTable::UpdateBoard(int seat, DblCrossPartWord *wordH, DblCrossPartWord *wordV,
										 DCPoint square, int rackSpot, MFS_DOUBLECROSS_EVENT_MESSAGE *mfs)
{
	int i, bonus, points;
	DblCrossPartWord *word;
	static char bombDirs[][20] = { "Vertical", "Horizontal", "Vertical/Horizontal" };
	
	// Figure out which tiles stabilize, score, explode, etc, as a result
	// of made words
	
	/* Check out the results of placing this tile here and creating a legal
	   word.  Send each event to the clients.  Among the things to check:
		
		- determine which tiles stabilize in all directions
		- determine which tiles explode in all directions
		- score stabilizing tiles for owner of tile
		- score word for current player
	*/
	
	// first, evaluate any special tiles we may have landed on...
	
	if (m_special >= 0) {
		switch (m_special) {
			case DCSS_BONUS:
				bonus = m_engine->GetNthBonusValue(gameState.bonusIndex);
				gameState.seats[seat].score += bonus * gameState.doublerFactor;
				
				sprintf(mfs->comment, "Get %d point%s%s.",
									  bonus,
									  (bonus == 1) ? "" : "s",
									  (gameState.doublerFactor == 2) ? " X 2 (doubler active!)" : "");
				gameState.bonusIndex++;
				
				mfs->who = seat;
				mfs->eventType = DCMOVE_BONUS;
				mfs->rackSpot = gameState.bonusIndex;
				mfs->start = square;
				mfs->finish = square;
				mfs->score = gameState.seats[seat].score;
				
				roomServer->BroadcastGameMessage(index, (char *)mfs, sizeof(MFS_DOUBLECROSS_EVENT_MESSAGE) + strlen(mfs->comment) + 1);
				
				break;
			case DCSS_MYSTERY:
				// handles all Mystery tile stuff except colored bombs (see below)
				DoMysteryTile(seat, square, mfs);
				break;
			default:
				// handle all the bombs (but we already did colored bombs)
				
				if (m_special >= DCBT_NORTH_SOUTH && m_special <= DCBT_NESW) {
					for (i = 0 ; i < DBLCROSS_BOMBRACKSIZE ; i++)
						if (gameState.seats[seat].bombs[i] == '-' ) {
							gameState.seats[seat].bombs[i] = m_special;
							sprintf(mfs->comment, "%s Bomb:  Play at any time!", bombDirs[m_special - DCBT_NORTH_SOUTH]);
							break;
						}
					
					if (i == DBLCROSS_BOMBRACKSIZE)
						strcpy(mfs->comment, "Two bomb limit!");
					
					// score points for getting the bomb
					points = (m_special == DCBT_NESW) ? DCPOINTS_GETNESWBOMB : DCPOINTS_GETDIRBOMB;
					gameState.seats[seat].score += points * gameState.doublerFactor;
					
					mfs->who = seat;
					mfs->eventType = DCMOVE_PICKUPBOMB;
					
					mfs->start = square;
					mfs->finish = square;
					
					mfs->rackSpot = i;
					memcpy(mfs->rack, gameState.seats[seat].bombs, DBLCROSS_BOMBRACKSIZE);
					
					mfs->score = gameState.seats[seat].score;
					
					roomServer->BroadcastGameMessage(index, (char *)mfs, sizeof(MFS_DOUBLECROSS_EVENT_MESSAGE) + strlen(mfs->comment) + 1);
				}
				break;
		}
	}
	
	// Second, check every tile in the word(s), ignoring stabilized tiles, which
	// don't score or change.  Stabilize tiles if they are only part
	// of complete words.  Then check those lines to see if other tiles in
	// that line need to explode...
	
	for (i = 0 ; i < 2 ; i++) {
		word = i ? wordV : wordH;
		if (!word)
			continue;
		
		// send word made event
		
		mfs->who = seat;
		mfs->eventType = DCMOVE_WORD_MADE;
		mfs->start = word->m_start;
		mfs->finish = word->m_finish;
		
		sprintf(mfs->comment, "Made Word: %s.", word->m_word);
		
		// score is affected by size of word being stabilized
		points = word->GetLength();
		gameState.seats[seat].score += points * gameState.doublerFactor;
		
		mfs->score = gameState.seats[seat].score;
		
		roomServer->BroadcastGameMessage(index, (char *)mfs, sizeof(MFS_DOUBLECROSS_EVENT_MESSAGE) + strlen(mfs->comment) + 1);
		
		// If we have two words made on the same tile, calculate the points
		
		if (wordH && wordV && i == 0)
			points = wordV->GetLength() + wordH->GetLength();
		
		// Check tiles for stabilization 
		StabilizeWord(square, word, wordH && wordV && i == 0, points, mfs);
	}
}

void DoubleCrossServerTable::DoMysteryTile(int seat, DCPoint square, MFS_DOUBLECROSS_EVENT_MESSAGE *mfs)
{
	int i, mysteryValue, points;
	static char bombDirs[][20] = { "Vertical", "Horizontal", "Vertical/Horizontal" };
	
	mysteryValue = GetMysteryValue(square);
	
	if (mysteryValue <= DCMYS_CURBONUS) {
		// Bonus points
		if (mysteryValue == DCMYS_CURBONUS)
			points = m_engine->GetNthBonusValue(gameState.bonusIndex);
		else
			points = mysteryValue;
		
		gameState.seats[seat].score += points * gameState.doublerFactor;
		if (gameState.seats[seat].score < 0)
			gameState.seats[seat].score = 0;
		
		if (points < 0) {
			if (gameState.doublerFactor == 2)
				sprintf(mfs->comment, "Lose %d points X 2 (doubler active!).", abs(points));
			else
				sprintf(mfs->comment, "Lose %d points.", abs(points));	
		}
		else {
			if (!points)
				sprintf(mfs->comment, "Get 0 points!!!! Way to Go!");	
			else if (gameState.doublerFactor == 2)
				sprintf(mfs->comment, "Get %d point%s X 2 (doubler active!).", points, (points == 1) ? "" : "s");
			else
				sprintf(mfs->comment, "Get %d point%s.", points, (points == 1) ? "" : "s");
		}
	}
	else if (mysteryValue == DCMYS_DOUBLER) {
		// doubler
		gameState.doublerFactor = 2;
		strcpy(mfs->comment, "All point values this turn are doubled!");
	}
	else if (mysteryValue == DCMYS_FILLRACK) {
		// fill rack
		if (FillRack(seat))
			strcpy(mfs->comment, "Fill Rack: Refill your rack!");
		else
			strcpy(mfs->comment, "Fill Rack: No more tiles left!");
		memcpy(mfs->rack, gameState.seats[seat].rack, DBLCROSS_RACKSIZE);
	}
	else if (mysteryValue >= DCMYS_NSBOMB && mysteryValue <= DCMYS_NESWBOMB) {
		for (i = 0 ; i < DBLCROSS_BOMBRACKSIZE ; i++)
			if (gameState.seats[seat].bombs[i] == '-' ) {
				gameState.seats[seat].bombs[i] = DCBT_NORTH_SOUTH + (mysteryValue - DCMYS_NSBOMB);
				sprintf(mfs->comment, "%s Bomb:  Play at any time!", bombDirs[mysteryValue - DCMYS_NSBOMB]);
				break;
			}
		
		if (i == DBLCROSS_BOMBRACKSIZE)
			strcpy(mfs->comment, "Two bomb limit!");
		
		// score points for getting the bomb
		points = (mysteryValue == DCMYS_NESWBOMB) ? DCPOINTS_GETNESWBOMB : DCPOINTS_GETDIRBOMB;
		gameState.seats[seat].score += points * gameState.doublerFactor;
		
		mfs->rackSpot = i;
		memcpy(mfs->rack, gameState.seats[seat].bombs, DBLCROSS_BOMBRACKSIZE);
	}
	else
		return; // don't send a message for colored bombs
	
	// send the mystery event message
	
	mfs->who = seat;
	mfs->eventType = DCMOVE_MYSTERY;
	
	mfs->start = square;
	mfs->finish = square;
	
	mfs->score = gameState.seats[seat].score;
	mfs->mysteryValue = mysteryValue;
	
	roomServer->BroadcastGameMessage(index, (char *)mfs, sizeof(MFS_DOUBLECROSS_EVENT_MESSAGE) + strlen(mfs->comment) + 1);
}

void DoubleCrossServerTable::StabilizeWord(DCPoint square, DblCrossPartWord *word, bool twoWords, int twoWordPoints, MFS_DOUBLECROSS_EVENT_MESSAGE *mfs)
{
	int d;
	DblCrossDirection oppDir;
	DCPoint spot;
	DblCrossPartWord *subWord;
	static DCPoint dirs[] = { { 1, 0 }, { 0, 1 } };
	
	if (word->m_direction == DBLCROSS_VERTICAL) {
		d = 1;
		oppDir = DBLCROSS_HORIZONTAL;
	}
	else {
		d = 0;
		oppDir = DBLCROSS_VERTICAL;
	}
	
	// Try to Stabilize all the tiles in the word, and verify the line(s)
	// and sublines thereafter
	
	for (spot = word->m_start ;
		 spot.x <= word->m_finish.x && spot.y <= word->m_finish.y ;
		 spot.x += dirs[d].x, spot.y += dirs[d].y) {
		if (m_engine->GetSquareOwner(spot) == -1)
			{ ASSERT(_WARN_, false); break; }
		
		if (m_engine->IsSquareStable(spot))
			continue;
		
		if (twoWords && spot.x == square.x && spot.y == square.y) {
			// If there are two words off this tile, it's guaranteed to be stable
			StabilizeTile(square, twoWordPoints, mfs);
		}
		else {
			// Check if the other word on this tile is complete (if any)
			m_altaltWordList->Reset();
			m_altaltWordList->CreateWordList(spot, oppDir);
			
			// Check if there is a legal word; if so, we can stabilize this tile
			subWord = FindLongestLegalWord(oppDir, m_altaltWordList);
			if (m_altaltWordList->Size() == 0 || subWord) {
				StabilizeTile(spot, word->GetLength(), mfs);
				
				if (subWord) {
					// make sure the line with the new stabilized tile still works
					VerifyLine(subWord, oppDir, mfs);
				}					
			}
		}
	}
	
	// Now explode tiles on either side of the completed word(s) that don't form
	// legitimate partial words...
	
	VerifyLine(word, word->m_direction, mfs);
}

void DoubleCrossServerTable::StabilizeTile(DCPoint square, int points, MFS_DOUBLECROSS_EVENT_MESSAGE *mfs)
{
	int owner;
	
	// stabilize the specified tile, and send the event to the client
	
	owner = m_engine->GetSquareOwner(square);
	
	if (owner == -1 || owner == DBLCROSS_SEED_PLAYER)
		{ ASSERT(_WARN_, false); return; }
	
	m_engine->StabilizeSquare(square, points * gameState.doublerFactor);
	
	// send the stabilize tile event
	
	mfs->who = owner;
	mfs->eventType = DCMOVE_TILE_STABLIZED;
	mfs->start = square;
	mfs->finish = square;
	mfs->score = gameState.seats[owner].score;
	sprintf(mfs->comment, "Stabilizing: %c.", m_engine->GetSquareLetter(square));
	
	roomServer->BroadcastGameMessage(index, (char *)mfs, sizeof(MFS_DOUBLECROSS_EVENT_MESSAGE) + strlen(mfs->comment) + 1);
}

void DoubleCrossServerTable::VerifyLine(DblCrossPartWord *word, DblCrossDirection dir, MFS_DOUBLECROSS_EVENT_MESSAGE *mfs)
{
	int d, oppDir;
	DCPoint spot;
	static DCPoint dirs[] = { { 1, 0 }, { 0, 1 } };
	
	// Verify that tiles around a completed word are still legal
	
	if (word->m_direction == DBLCROSS_VERTICAL) {
		d = 1;
		oppDir = 0;
	}
	else {
		d = 0;
		oppDir = 1;
	}
	
	// First delete the ones to the left/top of the word that don't work any longer
	for (spot.x = word->m_start.x - dirs[d].x, spot.y = word->m_start.y - dirs[d].y ;
		 spot.x >= 0 && spot.y >= 0 && m_engine->GetSquareOwner(spot) != -1 ;
		 spot.x -= dirs[d].x, spot.y -= dirs[d].y) {
		
		if (m_engine->IsSquareStable(spot))
			continue;
		
		if (!ShortestPartialsDefined(spot, NULL)) {
			// explode unstable letters - note that this function cleans up
			// the orphan tiles as well
			
			CreatePartWordComment(mfs->comment, false, m_altWordList);
			ExplodeTiles(spot, dirs[oppDir], dirs[d], true, true, true, -1, mfs);
			
			RecheckForWords(mfs);
		}
	}
	
	// Now check the letters to the right/bottom of the word for legitimacy
	for (spot.x = word->m_finish.x + dirs[d].x, spot.y = word->m_finish.y + dirs[d].y ;
		 spot.x < DBLCROSS_BOARDWIDTH && spot.y < DBLCROSS_BOARDHEIGHT && m_engine->GetSquareOwner(spot) != -1 ;
		 spot.x += dirs[d].x, spot.y += dirs[d].y) {
		
		if (m_engine->IsSquareStable(spot))
			continue;
		
		if (!ShortestPartialsDefined(spot, NULL)) {
			// explode unstable letters - note that this function cleans up
			// the orphan tiles as well
			
			CreatePartWordComment(mfs->comment, false, m_altWordList);
			ExplodeTiles(spot, dirs[oppDir], dirs[d], true, true, true, -1, mfs);
			
			RecheckForWords(mfs);
		}
	}
}

void DoubleCrossServerTable::ExplodeTiles(DCPoint square, DCPoint step, DCPoint stepR,
										  bool topLevel, bool recurse, bool bothDirs,
										  int specialTileUnderneath,
										  MFS_DOUBLECROSS_EVENT_MESSAGE *mfs)
{
	bool orphan = false;
	int i, limit, owner;
	DCPoint spot;
	
	// explode tiles and any orphan tiles not attached to stable tiles
	// as a result of the initial explosion
	
	// explode this tile, subtracting points
	
	owner = m_engine->GetSquareOwner(square);
	
	if (owner == -1)
		{ ASSERT(_WARN_, false); return; }
	
	if (topLevel)
		m_numOrphans = 0;
	
	if (specialTileUnderneath != -1)
		m_engine->SetSquareSpecial(square, (char)specialTileUnderneath);
	else
		m_engine->ClearSquare(square, DCPOINTS_EXPLODE * gameState.doublerFactor);
	
	// send the explode event
	
	mfs->who = owner;
	mfs->eventType = DCMOVE_TILE_EXPLODED;
	mfs->tileValue = (char)specialTileUnderneath;
	mfs->start = square;
	mfs->finish = square;
	mfs->score = gameState.seats[owner].score;
	// comment has already been filled
	
	roomServer->BroadcastGameMessage(index, (char *)mfs, sizeof(MFS_DOUBLECROSS_EVENT_MESSAGE) + strlen(mfs->comment) + 1);
	
	if (!recurse)
		return;
	
	AddOrphan(topLevel, square);
	
	// Loop in dirs in both direction to find orphaned tiles
	for (i = 0, limit = (bothDirs) ? 2 : 1 ; i < limit ; i++) {
		if (i > 0) {
			// Now do the opposite direction as well (used in colored bombs and non-bomb
			// routines only, not in directional bomb routine)
			spot = step;
			step = stepR;
			stepR = spot;
		}
		
		// look to the left/top of the placed tile
		
		for (orphan = false, spot.x = square.x - step.x, spot.y = square.y - step.y ;
			 spot.x >= 0 && spot.y >= 0 ;
			 spot.x -= step.x, spot.y -= step.y) {
			if (m_engine->IsSquareStable(spot)) {
				orphan = false;
				break;
			}
			
			if (m_engine->GetSquareOwner(spot) == -1)
				break;
			
			orphan = true;
		}
		
		// delete orphans to the left/top of the placed tile
		
		if (orphan) {
			// delete all these orphans
			spot.x = square.x - step.x;
			spot.y = square.y - step.y;
			
			while (spot.x >= 0 && spot.y >= 0 && m_engine->GetSquareOwner(spot) != -1) {
				// Recurse this
				if (IsOrphan(spot, stepR)) {
					// don't send comment for each orphan we eliminate
					if (m_numOrphans < 2)
						strcpy(mfs->comment, "Removing orphaned tiles.");
					else
						strcpy(mfs->comment, "");
					
					ExplodeTiles(spot, stepR, step, false, true, false, -1, mfs);
				}
				spot.x -= step.x;
				spot.y -= step.y;
			}
		}
		
		// look to the right/bottom of the placed tile
		
		for (orphan = false, spot.x = square.x + step.x, spot.y = square.y + step.y ;
			 spot.x < DBLCROSS_BOARDWIDTH && spot.y < DBLCROSS_BOARDHEIGHT ;
			 spot.x += step.x, spot.y += step.y ) {
			if (m_engine->IsSquareStable(spot)) {
				orphan = false;
				break;
			}
			
			if (m_engine->GetSquareOwner(spot) == -1)
				break;
			
			orphan = true;
		}
		
		// delete orphans to the right/bottom of the placed tile
		
		if (orphan) {
			// delete all these orphans
			spot.x = square.x + step.x;
			spot.y = square.y + step.y;
			
			while (spot.x < DBLCROSS_BOARDWIDTH && spot.y < DBLCROSS_BOARDHEIGHT && m_engine->GetSquareOwner(spot) != -1) {
				// Recurse this
				if (IsOrphan(spot, stepR)) {
					// don't send comment for each orphan we eliminate
					if (m_numOrphans < 2)
						strcpy(mfs->comment, "Removing orphaned tiles.");
					else
						strcpy(mfs->comment, "");

					ExplodeTiles(spot, stepR, step, false, true, false, -1, mfs);
				}
				spot.x += step.x;
				spot.y += step.y;
			}
		}
	}
}

void DoubleCrossServerTable::RecheckForWords(MFS_DOUBLECROSS_EVENT_MESSAGE *mfs)
{
	int i, orph, points;
	DCPoint square, spot;
	DblCrossPartWord *wordV = NULL, *wordH = NULL;
	static DCPoint dirs[] = { { -1, 0 }, { 0, -1 }, { 1, 0 }, { 0, 1 } };
	
	// Recheck that our exploded tiles haven't indirectly created new words
	// Check all the orphan tiles for possible stabilization near them
	
	for (orph = 0 ; orph < m_numOrphans ; orph++) {
		square = m_orphans[orph];
		
		for (i = 0 ; i < 4 ; i++) {
			
			spot.x = square.x + dirs[i].x;
			spot.y = square.y + dirs[i].y;
			
			if (spot.y < 0 || spot.y >= DBLCROSS_BOARDHEIGHT ||
				spot.x < 0 || spot.x >= DBLCROSS_BOARDWIDTH)
				continue;
			
			if (m_engine->GetSquareOwner(spot) == -1 || m_engine->IsSquareStable(spot))
				continue;
			
			if (IsMarkedForDestruction(spot))
				continue;
			
			m_altWordList->Reset();
			m_altWordList->CreateWordList(spot, DBLCROSS_NODIR);
			
			wordH = FindLongestLegalWord(DBLCROSS_HORIZONTAL, m_altWordList);
			wordV = FindLongestLegalWord(DBLCROSS_VERTICAL, m_altWordList);
			
			// If we have two words made on the same tile, calculate the points
			
			points = 0;
			if (wordH && wordV)
				points = wordV->GetLength() + wordH->GetLength();
			
			if (wordH)
				StabilizeWord(spot, wordH, wordH && wordV, points, mfs);
			
			if (wordV)
				StabilizeWord(spot, wordV, false, 0, mfs);
		}
	}
	
	m_numOrphans = 0;
}

void DoubleCrossServerTable::ExplodeColorTiles(int seat, DCPoint square, bool defuse, DblCrossColors bombColor, MFS_DOUBLECROSS_EVENT_MESSAGE *mfs)
{
	char colour[10];
	int j, x, y, owner, dest, start, cornNum, xLim, yLim;
	DCPoint spot;
	static char bombCols[][10] = { "Red", "Yellow", "Blue", "Green" };
	static DCPoint recurseDirs[] = { { 0 , 1 }, { 1 , 0 }  };
	static DCPoint dirs[] = { { 1 , 1 }, { -1 , 1 }, { -1, -1 }, { 1, -1 } };
	static DCPoint corners[] = { { 0 , 0 },
								 { DBLCROSS_BOARDWIDTH - 1 , 0 },
								 { DBLCROSS_BOARDWIDTH - 1 , DBLCROSS_BOARDHEIGHT - 1 },
								 { 0, DBLCROSS_BOARDHEIGHT - 1 } };
	
	// Explode all unstable tiles of the given color on the board
	
	// The explosion radiates out from the corner square is in to 
	// the opposite side of the board.  Any unstable tiles we intercept that
	// are unstable and of the right color are wiped out
	
	// Which player is color?
	
	owner = bombColor - 1;
	strcpy(colour, bombCols[owner]);
	strlwr(colour);
	
	if (defuse)
		sprintf(mfs->comment, "%s player defuses %s bomb.", bombCols[owner], colour);
	else			
		sprintf(mfs->comment, "%s Bomb: Destroys unstable %s tiles.", bombCols[owner], colour);
	
	if (m_special == DCSS_MYSTERY) {
		// Add a Move for the colored bomb explosion from current location
		mfs->who = owner;
		mfs->eventType = DCMOVE_MYSTERY;
		mfs->start = square;
		mfs->finish = square;
		
		mfs->tileValue = defuse;
		mfs->score = gameState.seats[seat].score;
		mfs->mysteryValue = owner + DCMYS_REDBOMB;
		
		roomServer->BroadcastGameMessage(index, (char *)mfs, sizeof(MFS_DOUBLECROSS_EVENT_MESSAGE) + strlen(mfs->comment) + 1);
		
		// We need to set this to the proper corner for this explosion...
		spot = corners[owner];
	}
	else {
		// Add a Move for the colored bomb explosion
		mfs->who = owner;
		mfs->eventType = DCMOVE_COLORBOMB;
		mfs->start = square;
		mfs->finish = square;
	
		mfs->tileValue = defuse;
		mfs->score = gameState.seats[seat].score;
		
		roomServer->BroadcastGameMessage(index, (char *)mfs, sizeof(MFS_DOUBLECROSS_EVENT_MESSAGE) + strlen(mfs->comment) + 1);
		
		spot = square;
	}

	strcpy(mfs->comment, "");
	
	if (defuse)
		return;
	
	// find corner the explosion starts from
	for (cornNum = 0 ; cornNum < 4 ; cornNum++) {
		if (corners[cornNum].x == spot.x && corners[cornNum].y == spot.y)
			break;
	}

	// There are only four corners! ERROR!
	if (cornNum == 4)
		{ ASSERT(_WARN_, false); return; }		
	
	yLim = (cornNum < 2) ? 2 : DBLCROSS_BOARDHEIGHT - 3;
	dest = (cornNum == 1 || cornNum == 2) ? DBLCROSS_BOARDHEIGHT : -1;
	start = (cornNum < 2) ? 0 : DBLCROSS_BOARDHEIGHT - 1;
	
	for (xLim = spot.x + dirs[cornNum].x, j = 0; j < DBLCROSS_BOARDHEIGHT - 1; 
			j++, xLim += dirs[cornNum].x, yLim += dirs[cornNum].y) {
		
		// First search vertically
		for (y = start ; y != yLim ; y += dirs[cornNum].y)
			if (m_engine->GetSquareOwner(m_engine->MakeDCPoint(xLim, y)) == owner)
				if (!m_engine->IsSquareStable(m_engine->MakeDCPoint(xLim, y))) {
					ExplodeTiles(m_engine->MakeDCPoint(xLim, y), recurseDirs[0], recurseDirs[1], true, true, true, -1, mfs);
					
					// Now recheck that we don't have more words to create/points to give
					RecheckForWords(mfs);
				}
		
		// Now search horizontally
		y -= dirs[cornNum].y;
		for (x = xLim - dirs[cornNum].x ; x != dest ; x -= dirs[cornNum].x)
			if (m_engine->GetSquareOwner(m_engine->MakeDCPoint(x, y)) == owner)
				if (!m_engine->IsSquareStable(m_engine->MakeDCPoint(x, y))) {
					ExplodeTiles(m_engine->MakeDCPoint(x, y), recurseDirs[0], recurseDirs[1], true, true, true, -1, mfs);
					
					// Now recheck that we don't have more words to create/points to give
					RecheckForWords(mfs);
				}						
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
// helper functions for DoubleCross

void DoubleCrossServerTable::ExchangeRack(int seat)
{
	char temp;
	int n, i, j, numFound, p, rackPositions[DBLCROSS_RACKSIZE], positions[DBLCROSS_RACKSIZE];
	
	if (gameState.currentStoreSize <= 0)
		{ ASSERT(_WARN_, FALSE); return; }
	
	if (m_engine->CountLettersInRack(seat) != DBLCROSS_RACKSIZE)
		{ ASSERT(_WARN_, FALSE); return; }
	
	for (i = 0 ; i < DBLCROSS_RACKSIZE ; i++)
		rackPositions[i] = i;
	
	for (j = 0 ; j < 5 ; j++)
		for (i = 0 ; i < DBLCROSS_RACKSIZE ; i++) {
			p = roomServer->randomGenerator->GetRandom(DBLCROSS_RACKSIZE);
			n = rackPositions[p];
			rackPositions[p] = rackPositions[i];
			rackPositions[i] = n;
		}
	
	n = DBLCROSS_RACKSIZE;
	if (n > gameState.currentStoreSize)
		n = gameState.currentStoreSize;
	
	// choose n unique positions
	
	for (i = 0 ; i < DBLCROSS_RACKSIZE ; i++)
		positions[i] = -1;
	
	for (numFound = 0 ; numFound < n ; ) {
		p = (DBLCROSS_TILESTORESIZE - 1) - roomServer->randomGenerator->GetRandom(gameState.currentStoreSize);
		assert(p < DBLCROSS_TILESTORESIZE);
		for (i = 0 ; i < DBLCROSS_RACKSIZE ; i++)
			if (p == positions[i])
				break;
		if (i == DBLCROSS_RACKSIZE)
			positions[numFound++] = p;
	}
	
	// move a letter from the chosen positions to temp
	// move a letter from the player's rack into the positions
	// move the temp rack into the player's rack
	
	for (i = 0 ; i < n ; i++) {
		temp = m_tileStore[positions[i]];
		assert(temp >= 'A' && temp <= 'Z');
		m_tileStore[positions[i]] = gameState.seats[seat].rack[rackPositions[i]];
		gameState.seats[seat].rack[rackPositions[i]] = temp;
	}
	
	// debug - ERF
	assert(&gameState == m_engine->gameState);
}

bool DoubleCrossServerTable::FillRack(int seat)
{
	char letter;
	int next, i;
	
	if (gameState.currentStoreSize <= 0)
		return false;
	
	// first pack the rack (shift empty spaces to right)
	for (next = i = DBLCROSS_RACKSIZE - 1 ; i >= 0 ; i--) {
		if (gameState.seats[seat].rack[i] == '-')
			continue;
		
		if (next > i) {
			gameState.seats[seat].rack[next] = gameState.seats[seat].rack[i];
			gameState.seats[seat].rack[i] = '-';
		}
		next--;
	}
	
	// Now fill 'er up
	for (i = DBLCROSS_RACKSIZE - 1 ; i >= 0 && gameState.currentStoreSize > 0 ; i--)
		if (gameState.seats[seat].rack[i] == '-') {
			letter = m_tileStore[DBLCROSS_TILESTORESIZE - gameState.currentStoreSize];
			gameState.currentStoreSize--;
			gameState.seats[seat].rack[i] = letter;
		}
	
	/*
	if ( m_endGame >= 0 ) {
		// Now we are in the final stage, where every player has one more turn to 
		// finish.  Has everyone had their last chance to go?
		if ( m_endGame == m_gameData->curPlayer )
			m_state = DBLCROSS_ENDOFGAME;
	}
	else if ( (next == DBLCROSS_RACKSIZE - 1) && m_gameData->gotten[playerNum] == 0 ) {
		// We have an empty rack, and there are no more tiles
		m_endGame = playerNum;		
	}
	*/
	
	return true;
}

void DoubleCrossServerTable::ClearBombData()
{
	m_nukeType = DCBT_EMPTY;
	m_nukeOrigin.x = m_nukeOrigin.y = -1;
	m_nukeColor = DBLCROSS_NONE;
}

bool DoubleCrossServerTable::ShortestPartialsDefined(DCPoint square, DblCrossPartWordList *wList)
{
	bool found = false;
	DblCrossPartWord *word;
	DblCrossPartWordList *list;
	
	// for the given tile / word list, we need to check that the smallest partial words
	// in each direction are in the dictionary
	
	// Create a list if one is not already provided
	if (!wList) {
		list = m_altWordList;
		list->Reset();
		list->CreateWordList(square, DBLCROSS_NODIR);
	}
	else
		list = wList;
	
	word = list->FindSmallestPartWord(DBLCROSS_HORIZONTAL);
	if (word) {
		found = true;
		if (!roomServer->dictionary->IsLegalPartialWord(word->m_word, 0, gameState.options.allowOffensiveWords ? true : false))
			return false;
	}
	
	word = list->FindSmallestPartWord(DBLCROSS_VERTICAL);
	if (word) {
		found = true;
		if (!roomServer->dictionary->IsLegalPartialWord(word->m_word, 0, gameState.options.allowOffensiveWords ? true : false))
			return false;
	}
	
	if (!found)
		ASSERT(_WARN_, false);
	
	return found;
}

void DoubleCrossServerTable::CreatePartWordComment(char *comment, bool isLegal, DblCrossPartWordList *ls)
{
	int i;
	char wds[2][DBLCROSS_BOARDWIDTH + 1];
	DblCrossPartWord *word;
	DblCrossPartWordList *list;
	
	ASSERT(_WARN_, comment);
	
	if (ls)
		list = ls;
	else
		list = m_wordlist;
	
	for (i = 0 ; i < 2 ; i++) {
		strcpy(wds[i], "");
		word = list->FindSmallestPartWord(i ? DBLCROSS_VERTICAL : DBLCROSS_HORIZONTAL);
		if (word && isLegal == roomServer->dictionary->IsLegalPartialWord(word->m_word, 0, gameState.options.allowOffensiveWords ? true : false))
			strcpy(wds[i], word->m_word);
	}
	
	if (wds[0][0] && wds[1][0])
		sprintf(comment, "%s, %s are %slegal partial words.", wds[0], wds[1], (isLegal) ? "" : "not ");
	else if (wds[0][0] || wds[1][0])
		sprintf(comment, "%s is %sa legal partial word.", (wds[0][0]) ? wds[0] : wds[1], (isLegal) ? "" : "not ");
	else
		strcpy(comment, "");
}

int DoubleCrossServerTable::GetMysteryValue(DCPoint square)
{
	int i;
	
	if (!gameState.options.haveMysteryTiles)
		return DCMYS_EMPTY;
	
	for (i = 0 ; i < DBLCROSS_NUMMYSTERYTILES ; i++)
		if (m_mysteryPoints[i].x == square.x && m_mysteryPoints[i].y == square.y)
			return m_mysteryValues[i];
	
	ASSERT(_WARN_, false);
	
	return DCMYS_EMPTY;
}

DblCrossPartWord *DoubleCrossServerTable::FindLongestLegalWord(DblCrossDirection dir, DblCrossPartWordList *ls)
{
	int i, k, wLen, largest = 0, ind = -1;
	DblCrossPartWord *word;
	DCPoint pt;
	
	// locate the longest legal (in the dictionary) word in the given direction
	
	for (i = 0 ; i < ls->Size() ; i++) {
		word = ls->At(i);
		if (word->m_direction != dir)
			continue;
		
		if (roomServer->dictionary->IsLegalWord(word->m_word, gameState.options.minWordSize, gameState.options.allowOffensiveWords ? true : false)) {
			// Right now, if two words of equal size can be made, we
			// arbitrarily choose one of them.  Should we handle this
			// differently? - ERF
			if (largest < (signed)strlen(word->m_word)) {
				// Make sure that prospective word does not contain a tile marked
				// for destruction
				wLen = (word->m_direction == DBLCROSS_HORIZONTAL)
					? word->m_finish.x - word->m_start.x + 1
					: word->m_finish.y - word->m_start.y + 1;
				for (k = 0 ; k < wLen ; k++) {
					pt.x = (word->m_direction == DBLCROSS_HORIZONTAL) ? word->m_start.x + k
						: word->m_start.x;
					pt.y = (word->m_direction == DBLCROSS_VERTICAL) ? word->m_start.y + k
						: word->m_start.y;
					if (IsMarkedForDestruction(pt)) {
						break;
					}
				}
				if (wLen == k) {
					largest = strlen(word->m_word);
					ind = i;
				}
			}
		}
	}
	
	return (ind == -1) ? NULL : ls->At(ind);
}

bool DoubleCrossServerTable::IsMarkedForDestruction(DCPoint square)
{
	int owner;
	
	if (m_nukeColor != DBLCROSS_NONE) {
		// we've set off a color bomb
		owner = m_engine->GetSquareOwner(square);
		if (owner == -1)
			{ ASSERT(_WARN_, false); }
		if (owner == DBLCROSS_SEED_PLAYER)
			return false;
		if ((owner + 1) == m_nukeColor && !m_engine->IsSquareStable(square))
			return true;
		
		return false;
	}
	
	if (m_nukeOrigin.x == -1)
		return false;
	
	if ((m_nukeType == DCBT_EAST_WEST || m_nukeType == DCBT_NESW) &&
			m_nukeOrigin.y == square.y && !m_engine->IsSquareStable(square))
		return true;
	
	if ((m_nukeType == DCBT_NORTH_SOUTH || m_nukeType == DCBT_NESW) &&
			m_nukeOrigin.x == square.x && !m_engine->IsSquareStable(square))
		return true;
	
	return false;
}

void DoubleCrossServerTable::AddOrphan(bool topLevel, DCPoint square)
{
	if (topLevel) {
		m_numOrphans = 1;
		m_orphans[0] = square;
	}
	else {
		if (m_numOrphans == DBLCROSS_BOARDWIDTH * DBLCROSS_BOARDHEIGHT)
			{ ASSERT(_WARN_, false); return; }
		m_orphans[m_numOrphans++] = square;
	}
}

bool DoubleCrossServerTable::IsOrphan(DCPoint square, DCPoint step)
{
	DCPoint spot;
	
	// Check if the tile is an orphan in provided direction
	
	// check in one direction
	
	for (spot.x = square.x - step.x, spot.y = square.y - step.y ;
		 spot.x >= 0 && spot.y >= 0 ;
		 spot.x -= step.x, spot.y -= step.y) {
		if (m_engine->IsSquareStable(spot))
			return false;
		
		if (m_engine->GetSquareOwner(spot) == -1)
			break;
	}
	
	// check in the other direction
	
	for (spot.x = square.x + step.x, spot.y = square.y + step.y ; 
		 spot.x < DBLCROSS_BOARDWIDTH && spot.y < DBLCROSS_BOARDHEIGHT ;
		 spot.x += step.x, spot.y += step.y) {
		if (m_engine->IsSquareStable(spot))
			return false;
		
		if (m_engine->GetSquareOwner(spot) == -1)
			break;
	}
	
	return true;
}

void DoubleCrossServerTable::TallyFinalScores()
{
	int i, owner;
	DCPoint square;
	
	// end of game: lose points for remaining tiles
	
	// tote up all the negative points in each person's hands
	
	for (i = 0 ; i < MAX_GAME_PLAYERS ; i++)
		if (gameState.players[i].status == DCPS_ACTIVE)
			gameState.seats[i].score += m_engine->CountLettersInRack(i) * DCPOINTS_EXPLODE;
	
	// Now tote up all the negative points remaining on the board
	
	for (square.y = 0 ; square.y < DBLCROSS_BOARDHEIGHT ; square.y++)
		for (square.x = 0 ; square.x < DBLCROSS_BOARDWIDTH ; square.x++) {
			owner = m_engine->GetSquareOwner(square);
			if (owner != -1 && !m_engine->IsSquareStable(square))
				gameState.seats[i].score--;
		}
	
	for (i = 0 ; i < MAX_GAME_PLAYERS ; i++)
		if (gameState.seats[i].score < 0)
			gameState.seats[i].score = 0;
}

int DoubleCrossServerTable::GetWordForDefinition(DCPoint square, DblCrossDirection direction, char *word)
{
	bool haveStart;
	int i = 0, length, maxLength, ind;
	DblCrossPartWord *wordPtr;
	DblCrossPartWordList wordList(this);
	
	// create word list for the tile
	wordList.Reset();
	if (!wordList.CreateWordList(square, direction))
		return 0;

	// get definition for longest word starting at this spot in this direction

	for (haveStart = false, maxLength = i = 0, ind = -1 ; ; i++) {
		wordPtr = wordList.At(i);
		if (wordPtr->m_direction != direction)
			break;
		length = wordPtr->GetLength();
		if (length > maxLength && length >= gameState.options.minWordSize) {
			// Prefer words that start at clicked square
			if (haveStart && wordPtr->m_start.x != square.x)
				continue;

			// is it a legal word?
			if (roomServer->dictionary->IsLegalWord(wordPtr->m_word, gameState.options.minWordSize, gameState.options.allowOffensiveWords ? true : false)) {
				if (!haveStart && wordPtr->m_start.x == square.x)
					haveStart = true;
				maxLength = length;
				ind = i;
			}
		}
	}

	if (ind == -1)
		return 0;

	wordPtr = wordList.At(ind);
	strcpy(word, wordPtr->m_word);
	
	return maxLength;
}

//////////////////////////////////////////////////////////////////////////////////

void DoubleCrossServerTable::HandleDOUBLECROSSREPLACEPLAYER(MTS_DOUBLECROSS_REPLACEPLAYER_MESSAGE *mts)
{
	int p;
	MFS_ROOM_SIT_MESSAGE mfs;
	
	p = mts->replacingSeat;
	
	if (gameState.players[p].playerID != PLAYER_ID_EMPTY)
		return;
	
	InitPlayer(p);
	gameState.players[p].playerID = PLAYER_ID_VOID;
	
	mfs.messageType = MFS_ROOM_SIT;
	mfs.playerID = PLAYER_ID_VOID;
	mfs.table = index;
	mfs.seat = p;
	mfs.playerState = gameState.players[p];
	
	roomServer->BroadcastRoomMessage((char *)&mfs, sizeof(MFS_ROOM_SIT_MESSAGE));
}

void DoubleCrossServerTable::SendEventsDone()
{
	MFS_DOUBLECROSS_EVENTS_DONE_MESSAGE mfs;
	
	mfs.messageType = MFS_DOUBLECROSS_EVENTS_DONE;
	mfs.table = index;
	mfs.currentStoreSize = gameState.currentStoreSize;
	
	roomServer->BroadcastGameMessage(index, (char *)&mfs, sizeof(MFS_DOUBLECROSS_EVENTS_DONE_MESSAGE));
}

void DoubleCrossServerTable::HandleDOUBLECROSSPLAY(DoubleCrossServerPlayer *client, MTS_DOUBLECROSS_PLAY_MESSAGE *mts)
{
	bool late = false, offBoard = false, illegal = false;
	char buffer[MAX_CM_MSG];
	int length, err = 0;
	MFS_DOUBLECROSS_EVENT_MESSAGE *mfs = (MFS_DOUBLECROSS_EVENT_MESSAGE *)buffer;
	
	if (gameState.gameNumber != mts->gameNumber)
		return;
	
	TRACE(_ROUTINE_ "HandleDOUBLECROSSPLAY()\n");
	
	if (mts->playType != DCPLAY_DONE && mts->playType != DCPLAY_DUMP &&
		mts->playType != DCPLAY_PLACE_TILE && mts->playType != DCPLAY_PLACE_BOMB)
		{ ASSERT(_WARN_, FALSE); return; }
	
	mfs->messageType = MFS_DOUBLECROSS_EVENT;
	mfs->table = mts->table;
	mfs->playerID = mts->playerID;
	mfs->seat = mts->seat;
	
	if ((GetTickCount() - gameState.startTicks) > (DWORD)SecondsToTicks(gameState.seconds))
		late = true;
	if (gameState.currentPlayer != mfs->seat)
		late = true;
	if (gameState.gamePhase != DCGP_PLAY)
		late = true;
	if (gameState.players[mts->seat].didIt)
		late = true;
	
	if (mts->playType == DCPLAY_PLACE_TILE || mts->playType == DCPLAY_PLACE_BOMB)
		offBoard = !m_engine->SquareIsValid(mts->square);
	
	if (mts->playType == DCPLAY_PLACE_TILE)
		illegal = !m_engine->SquareIsLegal(mts->square);
	
	if (late || offBoard || illegal) {
		switch (mts->playType) {
			case DCPLAY_DONE:
				if (gameState.currentPlayer != mfs->seat || gameState.gamePhase != DCGP_PLAY || gameState.players[mts->seat].didIt)
					{ /*ASSERT(_WARN_, FALSE); */return; }
				
				// allow normal late DCPLAY_DONE messages to be processed normally below
				
				break;
			
			case DCPLAY_DUMP:
				roomServer->SendDisplayStringMessage(client, "Too Late", "Your New Letters play was ignored.", TRUE);
				
				return;
			
			case DCPLAY_PLACE_TILE:
			case DCPLAY_PLACE_BOMB:
				mfs->who = mts->seat;
				mfs->eventType = (mts->playType == DCPLAY_PLACE_TILE) ? DCMOVE_INVALID_TILE : DCMOVE_INVALID_BOMB;
				mfs->tileValue = mts->tileValue;
				mfs->rackSpot = mfs->rackSpot;
				mfs->start = mts->square;
				mfs->finish = mts->square;
				strcpy(mfs->comment, "Illegal play.");
				
				length = sizeof(MFS_DOUBLECROSS_EVENT_MESSAGE) + strlen(mfs->comment) + 1;
				err = roomServer->connections->PostMessage(client->connection, (char *)mfs, length);
				
				if (err)
					TRACE(_WARN_ "DoubleCrossServerTable::HandleDOUBLECROSSPLAY(): connections->PostMessage() returned error %d\n", err);
				
				return;
		}
	}
	
	switch (mts->playType) {
		case DCPLAY_PLACE_TILE:
			m_hasPlayedSinceLastDump[mts->seat] = TRUE;
			if (PlaceTile(mts->seat, mts->tileValue, mts->rackSpot, mts->square, mfs))
				break;
			mts->tileValue = 0;
			// FALL THROUGH TO END THE TURN
		case DCPLAY_DONE:
			gameState.players[mts->seat].didIt = TRUE;
			m_passedWithoutPlayingOnPreviousTurn[mts->seat] = gameState.seats[mts->seat].canDump ? TRUE : FALSE;
			FillRack(mts->seat);
			
			mfs->who = mts->seat;
			mfs->eventType = DCMOVE_FILL_AND_ADVANCE;
			// the tileValue tells you how pass occurred; 1 = pass button pressed, otherwise 0
			mfs->tileValue = mts->tileValue;
			memcpy(mfs->rack, gameState.seats[mts->seat].rack, DBLCROSS_RACKSIZE);
			mfs->comment[0] = 0;
			length = sizeof(MFS_DOUBLECROSS_EVENT_MESSAGE) + 1;
			roomServer->BroadcastGameMessage(index, (char *)mfs, length);
			break;
		case DCPLAY_DUMP:
			if (!gameState.seats[mts->seat].canDump)
				{ roomServer->SendDisplayStringMessage(client, "Cannot Exchange", "You cannot get new letters at this time.", TRUE); return; }
			
			if (gameState.currentStoreSize <= 0)
				{ roomServer->SendDisplayStringMessage(client, "Cannot Exchange", "There are no more letters available.", TRUE); return; }
			
			if (!m_hasPlayedSinceLastDump[mts->seat])
				{ roomServer->SendDisplayStringMessage(client, "Cannot Exchange", "You cannot exchange letters on two turns in a row (passing without playing doesn't count as a turn).", TRUE); return; }
			
			if (!m_engine->IsRackFull(mts->seat))
				{ roomServer->SendDisplayStringMessage(client, "Cannot Exchange", "You can only exchange letters when your rack is full.", TRUE); return; }
			
			gameState.players[mts->seat].didIt = TRUE;
			m_passedWithoutPlayingOnPreviousTurn[mts->seat] = FALSE;
			m_hasPlayedSinceLastDump[mts->seat] = FALSE;
			ExchangeRack(mts->seat);
			
			mfs->who = mts->seat;
			mfs->eventType = DCMOVE_DUMP_AND_ADVANCE;
			memcpy(mfs->rack, gameState.seats[mts->seat].rack, DBLCROSS_RACKSIZE);
			mfs->comment[0] = 0;
			length = sizeof(MFS_DOUBLECROSS_EVENT_MESSAGE) + 1;
			
			// debug - ERF
			assert(&gameState == m_engine->gameState);
			
			roomServer->BroadcastGameMessage(index, (char *)mfs, length);
			break;
		case DCPLAY_PLACE_BOMB:
			PlaceBomb(mts->seat, mts->tileValue, mts->rackSpot, mts->square, mfs);
			m_hasPlayedSinceLastDump[mts->seat] = TRUE;
			break;
		default:
			ASSERT(_WARN_, FALSE);
			break;
	}
}

void DoubleCrossServerTable::HandleDOUBLECROSSDEFINITION(DoubleCrossServerPlayer *client, MTS_DOUBLECROSS_DEFINITION_MESSAGE *mts)
{
	char buffer[1024], wordH[DBLCROSS_BOARDWIDTH + 1], wordV[DBLCROSS_BOARDHEIGHT + 1];
	int lengthH, lengthV, length, err;
	MFS_DOUBLECROSS_DEFINITION_MESSAGE *mfs = (MFS_DOUBLECROSS_DEFINITION_MESSAGE *)buffer;
	
	if (mts->gameNumber != gameState.gameNumber)
		return;
	
	mfs->messageType = MFS_DOUBLECROSS_DEFINITION;
	mfs->table = mts->table;
	
	if (mts->square.x < 0 || mts->square.x >= DBLCROSS_BOARDWIDTH || mts->square.y < 0 || mts->square.y >= DBLCROSS_BOARDHEIGHT)
		{ ASSERT(_WARN_, false); return; }
	
	lengthH = GetWordForDefinition(mts->square, DBLCROSS_HORIZONTAL, wordH);
	lengthV = GetWordForDefinition(mts->square, DBLCROSS_VERTICAL, wordV);
	
	if (!lengthH && !lengthV)
		return;
	
	// if there is only one word using this tile (horizontal or vertical), send it
	
	if (!lengthH || !lengthV)
		strcpy(mfs->word, lengthH ? wordH : wordV);
	else
		strcpy(mfs->word, mts->preferHorizontal ? wordH : wordV);
	
	roomServer->dictionary->definitions->FindDefinition(mfs->word, mfs->definition);

#ifdef _DEBUG
	TRACE(_ROUTINE_ "Sending to %s (playerID = %d): %s.\n", client->info.login, client->info.playerID, roomServer->GetMessageDescription((char *)mfs));
#endif
	
	length = sizeof(MFS_DOUBLECROSS_DEFINITION_MESSAGE) + strlen(mfs->definition) + 1;
	err = roomServer->connections->PostMessage(client->connection, (char *)mfs, length);
	
	if (err)
		TRACE(_WARN_ "DoubleCrossServerTable::HandleDOUBLECROSSDEFINITION(): connections->PostMessage() returned error %d\n", err);		
}

void DoubleCrossServerTable::HandleDOUBLECROSSSHOWDOWN(MTS_DOUBLECROSS_SHOWDOWN_MESSAGE *mts)
{
	if (gameState.gameNumber == mts->gameNumber) {
		ASSERT(_CRITICAL_, mts->seat < MAX_GAME_PLAYERS);
		gameState.players[mts->seat].didIt = TRUE;
	}
}

int DoubleCrossServerTable::HandleMessage(DoubleCrossServerPlayer *client, MTS_GAME_MESSAGE_HEADER *mts, int length)
{
	switch (mts->messageType) {
		case MTS_DOUBLECROSS_REPLACEPLAYER:
			HandleDOUBLECROSSREPLACEPLAYER((MTS_DOUBLECROSS_REPLACEPLAYER_MESSAGE *)mts);
			break;
		case MTS_DOUBLECROSS_PLAY:
			HandleDOUBLECROSSPLAY(client, (MTS_DOUBLECROSS_PLAY_MESSAGE *)mts);
			SendEventsDone();
			break;
		case MTS_DOUBLECROSS_DEFINITION:
			HandleDOUBLECROSSDEFINITION(client, (MTS_DOUBLECROSS_DEFINITION_MESSAGE *)mts);
			break;
		case MTS_DOUBLECROSS_SHOWDOWN:
			HandleDOUBLECROSSSHOWDOWN((MTS_DOUBLECROSS_SHOWDOWN_MESSAGE *)mts);
			break;
		default:
			ASSERT(_WARN_, FALSE);
			
			return -1;
	}
	
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////

void DoubleCrossServerTable::RequestDOUBLECROSSREPLACEPLAYER(int seat, int playerID)
{
	char s[256];
	int i, n;
	DoubleCrossServerPlayer *p, *leaver;
	
	leaver = roomServer->GetClientFromID(playerID);
	
	if (leaver == NULL)
		{ ASSERT(_WARN_, FALSE); return; }
	
	// send the appropriate player a message telling them what to do
	
	n = roomServer->GetPreferredApproverSeat(seat);
	
	// if the player in seat n is not active, look for an active player
	
	if (gameState.players[n].status != DCPS_ACTIVE)
		for (i = 0 ; i < MAX_GAME_PLAYERS ; i++)
			if (gameState.players[i].status == DCPS_ACTIVE) {
				n = i;
				break;
			}
	
	// if there are no active players, we have a problem
	
	if (gameState.players[n].status != DCPS_ACTIVE)
		{ ASSERT(_WARN_, FALSE); return; }
	
	p = roomServer->GetClientFromID(gameState.players[n].playerID);
	if (p == NULL) 
		{ ASSERT(_WARN_, FALSE); return; }
	
	sprintf(s, "%s has left the game.  Click on the nameplate to remove this player from the game, or invite another person to join your game.", leaver->info.login);
	
	roomServer->SendDisplayStringMessage(p, "Replace Player", s, TRUE);
}

Boolean DoubleCrossServerTable::RequestDOUBLECROSSPLAY(char *m, int *length)
{
	int i;
	MFS_DOUBLECROSS_REQUESTPLAY_MESSAGE *mfs;
	
	TRACE(_ROUTINE_ "RequestDOUBLECROSSPLAY()\n");
	
	if (gameState.currentPlayer == -1) { // pick random player to start
		i = 0;
		do {
			gameState.currentPlayer = roomServer->randomGenerator->GetRandom(MAX_GAME_PLAYERS);
		}
		while (gameState.players[gameState.currentPlayer].status != DCPS_ACTIVE && i < 100);
	}
	
	gameState.currentPlayer = GetNextDOUBLECROSSPlayer(gameState.currentPlayer);
	
	if (GameIsOver())
		return false;
	
	PossiblySuggestReset();
	
	gameState.gamePhase = DCGP_PLAY;
	
	m_engine->ResetForNewPlayer();
	
	gameState.players[gameState.currentPlayer].doIt = TRUE;
	
	gameState.startTicks = GetTickCount();
	gameState.seconds = gameState.options.timeLimit + 5; // grace period
	
	// prepare the message
	
	mfs = (MFS_DOUBLECROSS_REQUESTPLAY_MESSAGE *) m;
	
	mfs->messageType = MFS_DOUBLECROSS_REQUESTPLAY;
	mfs->table = index;
	mfs->who = gameState.currentPlayer;
	mfs->seconds = gameState.options.timeLimit;
	
	(*length) = sizeof(MFS_DOUBLECROSS_REQUESTPLAY_MESSAGE);
	
	return TRUE;
}

void DoubleCrossServerTable::RequestDOUBLECROSSSHOWDOWN(char *m, int *length)
{
	int i;
	MFS_DOUBLECROSS_REQUESTSHOWDOWN_MESSAGE *mfs;
	
	TRACE(_ROUTINE_ "RequestDOUBLECROSSSHOWDOWN()\n");
	
	gameState.gamePhase = DCGP_SHOWDOWN;
	gameState.currentPlayer = -1;
	
	SetAutoResult();
	
	for (i = 0 ; i < MAX_GAME_PLAYERS ; i++)
		if (gameState.players[i].status == DCPS_ACTIVE)
			gameState.players[i].doIt = TRUE;
	
	// prepare the message
	
	mfs = (MFS_DOUBLECROSS_REQUESTSHOWDOWN_MESSAGE *)m;
	
	mfs->messageType = MFS_DOUBLECROSS_REQUESTSHOWDOWN;
	mfs->table = index;
	mfs->gameState = gameState;
	
	(*length) = sizeof(MFS_DOUBLECROSS_REQUESTSHOWDOWN_MESSAGE);
}

void DoubleCrossServerTable::Tick()
{
	char mfs[MAX_MSG];
	int i, length, err;
	
	// if we are still waiting for someone to do something, keep waiting
	
	for (i = 0 ; i < MAX_GAME_PLAYERS ; i++)
		if (gameState.players[i].doIt)
			if (!gameState.players[i].didIt)
				return;
	
	// if a seat is empty, it must be filled before we proceed
	
	for (i = 0; i < MAX_GAME_PLAYERS; i++)
		if (gameState.players[i].playerID == PLAYER_ID_EMPTY)
			return;
	
	// otherwise, we advance to the next turn
	
	TRACE(_ROUTINE_ "TickDOUBLECROSS() advancing to the next turn\n");
	
	for (i = 0 ; i < MAX_GAME_PLAYERS ; i++) {
		gameState.players[i].doIt = FALSE;
		gameState.players[i].didIt = FALSE;
	}
	
	switch (gameState.gamePhase) {
		case DCGP_NOTHING:
		case DCGP_SHOWDOWN:
			roomServer->gameStarts++;
			ResetDOUBLECROSSGAME();
			RequestDOUBLECROSSPLAY(mfs, &length);
			break;
		case DCGP_PLAY:
			if (!RequestDOUBLECROSSPLAY(mfs, &length))
				RequestDOUBLECROSSSHOWDOWN(mfs, &length);
			break;
		default:
			ASSERT(_WARN_, FALSE);
			break;
	}
	
	err = roomServer->BroadcastGameMessage(index, (char *)mfs, length);
}
