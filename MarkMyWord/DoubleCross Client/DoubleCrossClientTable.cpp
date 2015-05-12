#include "os.h"
#include "sci.h"
#include "globals.h"
#include "inifile.h"
#include "util.h"

#include "Client Include.h"

DoubleCrossClientTable::DoubleCrossClientTable()
{
	showdownDialog = NULL;
	definitionDialog = NULL;
	engine = NULL;
}

DoubleCrossClientTable::~DoubleCrossClientTable()
{
	if (engine) { delete engine; engine = NULL; }
}

void DoubleCrossClientTable::Init(int theIndex)
{
	if (!engine)
		engine = new DoubleCrossEngine();
	
	engine->Init(&gameState);
	
	RoomClientTable::Init(theIndex);
	
	playedTenSecondWarningSound = FALSE;
	playedOutOfTimeSound = FALSE;
	dontDisplay = false;
	enableActions = true;	
}

void DoubleCrossClientTable::InitPlayer(int seat)
{
	RoomClientTable::InitPlayer(seat);
	
	engine->InitPlayer(seat);
}

void DoubleCrossClientTable::FillTableGameValues(int *values)
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
}

void DoubleCrossClientTable::GetDescription(char *s)
{
	sprintf(s, "%d letter word minimum\n%d seconds%s",
			   (int)gameState.options.minWordSize, (int)gameState.options.timeLimit,
			   (gameState.options.gridMode && gameState.options.haveBlockingTiles) ? " Grid Mode" : "");
}

int DoubleCrossClientTable::GetSecondsRemaining()
{
	int seconds;
	
	if (gameState.gamePhase != DCGP_PLAY)
		return 0;
	
	seconds = gameState.seconds - TicksToSeconds(GetTickCount() - gameState.startTicks);
	
	if (seconds < 0)
		seconds = 0;
	
	return seconds;
}

////////////////////////////////////////////////////////////////////////////////////////////////

void DoubleCrossClientTable::HandleRESETGAME(MFS_DOUBLECROSS_RESETGAME_MESSAGE *mfs)
{
	gameState = mfs->gameState;
	enableActions = true;
}

void DoubleCrossClientTable::FinishREQUESTREPLACEPLAYER(int seat, int type)
{
	MTS_DOUBLECROSS_REPLACEPLAYER_MESSAGE mts;
	
	mts.messageType = MTS_DOUBLECROSS_REPLACEPLAYER;
	mts.table = index;
	mts.playerID = roomClient->GetMyID();
	mts.seat = GetMyPlayingIndex(FALSE);
	mts.replacingSeat = seat;
	mts.type = type;
	
	roomClient->SendMessageToServer((char *)&mts, sizeof(MTS_DOUBLECROSS_REPLACEPLAYER_MESSAGE));
}

void DoubleCrossClientTable::HandleREQUESTPLAY(MFS_DOUBLECROSS_REQUESTPLAY_MESSAGE *mfs)
{
	gameState.gamePhase = DCGP_PLAY;
	gameState.currentPlayer = mfs->who;
	
	engine->ResetForNewPlayer();
	
	gameState.startTicks = GetTickCount();
	gameState.seconds = mfs->seconds;
	
	display->commentsDisplay->ClearComments();
	display->ClearEvaluatedWords();
	display->commentsDisplay->SetDoubler(false);	
	
	if (GetMyPlayingIndex(FALSE) == mfs->who) {
		answeredMostRecentRequest = FALSE;
		game->sound1->Play(1003);
	}
	
	enableActions = true;
	playedTenSecondWarningSound = FALSE;
	playedOutOfTimeSound = FALSE;	
}

void DoubleCrossClientTable::FinishREQUESTPLAY(char playType, char tileValue, char rackSpot, DCPoint square)
{
	int i, p = GetMyPlayingIndex(TRUE);
	MTS_DOUBLECROSS_PLAY_MESSAGE mts;
	
	if (display->highlightedTile) {
		display->highlightedTile = NULL;
		//Display();
	}
	
	for (i = 0 ; i < NUM_TILE_VIEWS ; i++)
		display->tiles[i]->mouseDown = FALSE;
	
	enableActions = false;
	
	if (answeredMostRecentRequest)
		return;
	
	mts.messageType = MTS_DOUBLECROSS_PLAY;
	mts.table = index;
	mts.playerID = roomClient->GetMyID();
	mts.seat = p;
	mts.gameNumber = gameState.gameNumber;
	
	mts.playType = playType;
	mts.tileValue = tileValue;
	mts.rackSpot = rackSpot;
	mts.square = square;
	
	if (mts.playType == DCPLAY_DONE) //  || mts.playType == DCPLAY_DUMP
		answeredMostRecentRequest = TRUE;
	
	roomClient->SendMessageToServer((char *)&mts, sizeof(MTS_DOUBLECROSS_PLAY_MESSAGE));
}

void DoubleCrossClientTable::HandleEVENT(MFS_DOUBLECROSS_EVENT_MESSAGE *mfs)
{
	bool myPlay;
	int p = mfs->seat;
	
	assert(p == gameState.currentPlayer);
	
	enableActions = false;
	
	myPlay = p == GetMyPlayingIndex(FALSE);
	
	switch (mfs->eventType) {
		case DCMOVE_MOVE_TILE:
			SetReadyForMessages(FALSE);
			room0->script->SetScript(new DoubleCrossMoveTileScript(this, mfs, mfs->comment));
			break;
		case DCMOVE_WORD_MADE:
			SetReadyForMessages(FALSE);
			room0->script->SetScript(new DoubleCrossWordMadeScript(this, mfs, mfs->comment));
			break;
		case DCMOVE_TILE_STABLIZED:
			SetReadyForMessages(FALSE);
			room0->script->SetScript(new DoubleCrossTileStabilizedScript(this, mfs, mfs->comment));
			break;
		case DCMOVE_TILE_EXPLODED:
			SetReadyForMessages(FALSE);
			room0->script->SetScript(new DoubleCrossTileExplodedScript(this, mfs, mfs->comment));
			break;
 		case DCMOVE_BONUS:
			SetReadyForMessages(FALSE);
			room0->script->SetScript(new DoubleCrossBonusScript(this, mfs, mfs->comment));
			break;
 		case DCMOVE_COLORBOMB:
			SetReadyForMessages(FALSE);
			room0->script->SetScript(new DoubleCrossColorBombScript(this, mfs, mfs->comment));
			break;
 		case DCMOVE_PICKUPBOMB:
			SetReadyForMessages(FALSE);
			room0->script->SetScript(new DoubleCrossPickUpBombScript(this, mfs, mfs->comment));
			break;
 		case DCMOVE_DIREXPLOSION:
			SetReadyForMessages(FALSE);
			room0->script->SetScript(new DoubleCrossDirExplosionScript(this, mfs, mfs->comment));
			break;
 		case DCMOVE_MYSTERY:
			SetReadyForMessages(FALSE);
			room0->script->SetScript(new DoubleCrossMysteryScript(this, mfs, mfs->comment));
			break;
 		case DCMOVE_DUMP_AND_ADVANCE:
 		case DCMOVE_FILL_AND_ADVANCE:
			SetReadyForMessages(FALSE);
			room0->script->SetScript(new DoubleCrossFillRackScript(this, mfs));
			break;
 		case DCMOVE_PLACE_BOMB:
			SetReadyForMessages(FALSE);
			room0->script->SetScript(new DoubleCrossMoveTileScript(this, mfs, mfs->comment));
			break;
 		case DCMOVE_INVALID_TILE:
			display->commentsDisplay->AddComment(DCMOVE_INVALID_TILE, mfs->comment);
			break;
 		case DCMOVE_INVALID_BOMB:
			display->commentsDisplay->AddComment(DCMOVE_INVALID_BOMB, mfs->comment);
			break;
		default:
			ASSERT(_WARN_, FALSE);
			break;
	}
	
	display->highlightedTile = NULL;
}

void DoubleCrossClientTable::HandleEVENTSDONE(MFS_DOUBLECROSS_EVENTS_DONE_MESSAGE *mfs)
{
	enableActions = true;
	memset(display->crackedTiles, 0, DBLCROSS_BOARDWIDTH * DBLCROSS_BOARDHEIGHT);
	
	assert(gameState.currentStoreSize == mfs->currentStoreSize);
}

void DoubleCrossClientTable::HandleDEFINITION(MFS_DOUBLECROSS_DEFINITION_MESSAGE *mfs)
{
	definitionDialog->Show(mfs->word, mfs->definition);
}

void DoubleCrossClientTable::HandleREQUESTSHOWDOWN(MFS_DOUBLECROSS_REQUESTSHOWDOWN_MESSAGE *mfs)
{
	gameState = mfs->gameState;
	
	playedTenSecondWarningSound = TRUE;
	playedOutOfTimeSound = TRUE;
	
	answeredMostRecentRequest = FALSE;
	
	// tote up final score, subtracting unstable tiles in rack and board
	
	SetReadyForMessages(FALSE);
	room0->script->SetScript(new DoubleCrossShowdownScript(this, mfs));	
}

void DoubleCrossClientTable::FinishREQUESTSHOWDOWN()
{
	MTS_DOUBLECROSS_SHOWDOWN_MESSAGE mts;
	
	if (GetMyPlayingIndex(FALSE) >= MAX_GAME_PLAYERS)
		return;
	
	mts.messageType = MTS_DOUBLECROSS_SHOWDOWN;
	mts.table = index;
	mts.playerID = roomClient->GetMyID();
	mts.seat = GetMyPlayingIndex(TRUE);
	mts.gameNumber = gameState.gameNumber;
	
	roomClient->SendMessageToServer((char *)&mts, sizeof(MTS_DOUBLECROSS_SHOWDOWN_MESSAGE));
}

int DoubleCrossClientTable::HandleMessage(MFS_GAME_MESSAGE_HEADER *mfs)
{
	assert(mfs->table == index);
	
	switch (mfs->messageType) {
		case MFS_DOUBLECROSS_RESETGAME:
			HandleRESETGAME((MFS_DOUBLECROSS_RESETGAME_MESSAGE *)mfs);
			break;
		case MFS_DOUBLECROSS_REQUESTPLAY:
			HandleREQUESTPLAY((MFS_DOUBLECROSS_REQUESTPLAY_MESSAGE *)mfs);
			break;
		case MFS_DOUBLECROSS_EVENT:
			HandleEVENT((MFS_DOUBLECROSS_EVENT_MESSAGE *)mfs);
			break;
		case MFS_DOUBLECROSS_EVENTS_DONE:
			HandleEVENTSDONE((MFS_DOUBLECROSS_EVENTS_DONE_MESSAGE *)mfs);
			break;
		case MFS_DOUBLECROSS_DEFINITION:
			HandleDEFINITION((MFS_DOUBLECROSS_DEFINITION_MESSAGE *)mfs);
			break;
		case MFS_DOUBLECROSS_REQUESTSHOWDOWN:
			HandleREQUESTSHOWDOWN((MFS_DOUBLECROSS_REQUESTSHOWDOWN_MESSAGE *)mfs);
			break;
		default:
			assert(FALSE);
			break;
	}
	
	if (!room0->script->script && !dontDisplay) // don't display when we are about to animate
		Display();
	
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////

void DoubleCrossClientTable::Clear()
{
	RoomClientTable::Clear();
	
	playedTenSecondWarningSound = TRUE;
	playedOutOfTimeSound = TRUE;
	
	showdownDialog = NULL;
	definitionDialog = NULL;
	
	enableActions = true;
}

void DoubleCrossClientTable::Display()
{
	int i, p;
	
	#define INFO_BAR_PRESET_Y 435
	#define WATCHERS_DISPLAY_Y 450
	#define INFO_BAR_DY 2
	
	RoomClientTable::Display();
	
	display->showChatButton->Posn(0, 21);
	display->showLobbyButton->Posn(0, 21);
	
	if (!showdownDialog) {
		showdownDialog = new DoubleCrossShowdownDialog(plane, this);
		showdownDialog->Init();
		showdownDialog->Hide();
	}
	
	if (!definitionDialog) {
		definitionDialog = new DoubleCrossDefinitionDialog(plane);
		definitionDialog->Init();
		definitionDialog->Hide();
	}
	
	for (i = 0 ; i < MAX_GAME_PLAYERS ; i++)
		display->DisplayPlayer(&gameState, i);
	
	p = GetMyPlayingIndex(FALSE);
	
	display->DisplayElements(this, p == gameState.currentPlayer, p < MAX_GAME_PLAYERS && gameState.seats[p].canDump);
	
	display->ShowWatchersDisplay(&gameState);
	
	display->watchersDisplay->Posn(278, 418);
	
	display->watchersDisplay->chatDisplayBalloon->Posn(0, WATCHERS_DISPLAY_Y + INFO_BAR_DY - INFO_BAR_PRESET_Y);
	display->watchersDisplay->chatDisplayBalloon->text->Posn(14 + display->watchersDisplay->GetWidth() + 18, WATCHERS_DISPLAY_Y + 5);
	
	// this was here so that the user would see the screen change for every play that happens
	// but now we think it's better, if many plays happen in quick succession, to just let
	// them happen as quickly as they can
	
	// plane->GetWindow()->ForceUpdate();
}

void DoubleCrossClientTable::DisplayForNewGameState()
{
	Display();
	enableActions = true;
}

POINT DoubleCrossClientTable::RackToScreenCoordinates(int seat, int rackIndex, bool bomb)
{
	POINT p;
	static POINT upperLeft[] = { { 45, 172 }, { 520, 172 }, { 520, 366 }, { 45, 366 } };
	static POINT tileOffsets[] = { { 0, 0 }, { 26, 0 }, { 26 * 2, 0 },
								 { 26 * 2, 26 }, { 26, 26 }, { 0, 26 } };
	static POINT bombUpperLeft[] = { { 19, 172 }, { 598, 172 }, { 598, 366 }, { 19, 366 } };
	static POINT bombOffsets[] = { { 0, 0 }, { 0, 26 } };
	
	if (seat < 0 || seat > MAX_GAME_PLAYERS)
		{ assert(false); seat = 0; }
	
	if (bomb) {
		if (!gameState.options.haveDirectionalBombs)
			assert(false);
		if (rackIndex < 0 || rackIndex > 1)
			{ assert(false); rackIndex = 0; }
		
		p.x = bombUpperLeft[seat].x + bombOffsets[rackIndex].x;
		p.y = bombUpperLeft[seat].y + bombOffsets[rackIndex].y;
	}
	else {
		if (rackIndex < 0 || rackIndex >= DBLCROSS_RACKSIZE)
			{ assert(false); rackIndex = 0; }
		
		p.x = upperLeft[seat].x + tileOffsets[rackIndex].x;
		p.y = upperLeft[seat].y + tileOffsets[rackIndex].y;
	}
	
	return p;
}

POINT DoubleCrossClientTable::BoardToScreenCoordinates(DCPoint square)
{
	POINT p;
	
	if (!engine->SquareIsValid(square)) {
		assert(false);
		square.x = 0;
		square.y = 0;
	}
	
	p.x = DBLCROSS_BRDLEFT + square.x * DBLCROSS_SQSIZE;
	p.y = DBLCROSS_BRDTOP + square.y * DBLCROSS_SQSIZE;
	
	return p;
}

bool DoubleCrossClientTable::ScreenToBoardCoordinates(POINT p, DCPoint *square)
{
	if (p.x < DBLCROSS_BRDLEFT || p.y < DBLCROSS_BRDTOP ||
		p.x >= (DBLCROSS_BRDLEFT + DBLCROSS_BOARDWIDTH * DBLCROSS_SQSIZE) ||
		p.y >= (DBLCROSS_BRDTOP + DBLCROSS_BOARDHEIGHT * DBLCROSS_SQSIZE))
		return false;
	
	square->x = (p.x - DBLCROSS_BRDLEFT) / DBLCROSS_SQSIZE;
	square->y = (p.y - DBLCROSS_BRDTOP) / DBLCROSS_SQSIZE;
	
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////

DoubleCrossDisplay::DoubleCrossDisplay()
{
	int i, j;
	
	for (i = 0 ; i < NUM_TILE_VIEWS ; i++)
		tiles[i] = NULL;
	
	timerDisplay = NULL;
	passButton = NULL;
	newLettersButton = NULL;
	commentsDisplay = NULL;
	statusWordSize = NULL;
	statusNumTiles = NULL;
	boardDisplay = NULL;
	
	for (i = 0 ; i < 2 ; i++)
		for (j = 0 ; j < DBLCROSS_BOARDWIDTH ; j++)
			evaluated[i][j] = NULL;
	
	highlightedTile = NULL;
}

DoubleCrossDisplay::~DoubleCrossDisplay()
{
	int i, j;
	
	for (i = 0 ; i < NUM_TILE_VIEWS ; i++)
		if (tiles[i]) { delete tiles[i]; tiles[i] = NULL; }
	
	if (timerDisplay) { delete timerDisplay; timerDisplay = NULL; }
	if (passButton) { delete passButton; passButton = NULL; }
	if (newLettersButton) { delete newLettersButton; newLettersButton = NULL; }
	if (commentsDisplay) { delete commentsDisplay; commentsDisplay = NULL; }
	if (statusWordSize) { delete statusWordSize; statusWordSize = NULL; }
	if (statusNumTiles) { delete statusNumTiles; statusNumTiles = NULL; }
	if (boardDisplay) { delete boardDisplay; boardDisplay = NULL; }
	
	for (i = 0 ; i < 2 ; i++)
		for (j = 0 ; j < DBLCROSS_BOARDWIDTH ; j++)
			if (evaluated[i][j]) { delete evaluated[i][j]; evaluated[i][j] = NULL; }
}

void DoubleCrossDisplay::Init(int newTable)
{
	int i, j;
	sciPlane *plane;
	DoubleCrossClientTable *t;
	
	GameDisplay::Init(newTable);
	
	t = &roomClient->tables[table];
	plane = t->plane;
	
	for (i = 0 ; i < NUM_TILE_VIEWS ; i++) {
		tiles[i] = new DoubleCrossTile(t, i);
		tiles[i]->plane = plane;
		tiles[i]->ParamInit(30034, 0, 0, 10);
	}
	
	timerDisplay = new DoubleCrossTimerDisplay(t);
	timerDisplay->plane = plane;
	timerDisplay->Init();
	timerDisplay->SetView(30026);
	timerDisplay->SetPri(1);
	
	passButton = new DoubleCrossButton(t, DCBID_PASS);
	passButton->plane = plane;
	passButton->Init();
	passButton->SetView(30031);
	// passButton->Posn(35, 0);
	passButton->SetPri(1);
	
	newLettersButton = new DoubleCrossButton(t, DCBID_NEWLETTERS);
	newLettersButton->plane = plane;
	newLettersButton->Init();
	newLettersButton->SetView(30032);
	// newLettersButton->Posn(-35, 0);
	newLettersButton->SetPri(1);
	
	statusWordSize = new DoubleCrossScoreDisplay();
	statusWordSize->plane = plane;
	statusWordSize->Init();
	statusWordSize->Posn(36, 3);
	statusWordSize->SetWidth(DBLCROSS_SCOREDIGITWIDTH);
	
	statusNumTiles = new DoubleCrossScoreDisplay();
	statusNumTiles->plane = plane;
	statusNumTiles->Init();
	statusNumTiles->Posn(185, 2);
	statusNumTiles->SetWidth(DBLCROSS_SCOREDIGITWIDTH * 3);
	
	boardDisplay = new DoubleCrossBoardDisplay(t);
	boardDisplay->plane = plane;
	boardDisplay->Init();
	
	commentsDisplay = new DoubleCrossCommentsDisplay(t);
	commentsDisplay->plane = plane;
	commentsDisplay->Init();
	
	for (i = 0 ; i < 2 ; i++)
		for (j = 0 ; j < DBLCROSS_BOARDWIDTH ; j++) {
			evaluated[i][j] = new sciView();
			evaluated[i][j]->plane = plane;
			evaluated[i][j]->ParamInit(30061, 0, 0, 100);
		}

	memset(crackedTiles, 0, DBLCROSS_BOARDWIDTH * DBLCROSS_BOARDHEIGHT);
}

Boolean DoubleCrossDisplay::DisplayPlayer(GameState *gameState, int seat)
{
	DoubleCrossCharacter *c = characters[seat];
	
	if (gameState->players[seat].playerID == PLAYER_ID_VOID) {
		assert(gameState->players[seat].status == DCPS_EMPTY);
		c->Hide();
		
		return FALSE;
	}
	
	if (gameState->players[seat].playerID == PLAYER_ID_EMPTY) {
		c->nameText->SetFont(4);
		c->nameText->SetTextToWidth("EMPTY");
		c->scoreText->SetText("");
		c->Show(gameState->currentPlayer == seat);
		c->Hide(TRUE);
		
		return FALSE;
	}
	
	c->nameText->SetFont(2);
	
	GameDisplay::DisplayPlayer(gameState, seat);
	
	c->Show(gameState->currentPlayer == seat);
	c->scoreDisplay->Show(gameState->seats[seat].score);
	
	return FALSE;
}

void DoubleCrossDisplay::DisplayElements(DoubleCrossClientTable *t, bool myTurn, bool canDump)
{
	ShowTiles(t);
	// timerDisplay->Show(0); // the timer shows itself on the Doit()
	ShowButtons(t, myTurn, canDump);
	statusWordSize->Show(t->gameState.options.minWordSize);
	statusNumTiles->Show(t->gameState.currentStoreSize);
	commentsDisplay->Show();
}

void DoubleCrossDisplay::Hide()
{
	int i;
	
	GameDisplay::Hide();
	
	for (i = 0 ; i < NUM_TILE_VIEWS ; i++)
		tiles[i]->Hide();
	
	timerDisplay->Hide();
	passButton->Hide();
	newLettersButton->Hide();
	statusWordSize->Hide();
	statusNumTiles->Hide();
	commentsDisplay->Hide();
}

void DoubleCrossDisplay::ShowTiles(DoubleCrossClientTable *t)
{
	int i, j, owner, n = 0;
	DCPoint spot;
	
	// use as many tiles as necessary to display the board
	
	for (spot.x = 0 ; spot.x < DBLCROSS_BOARDWIDTH ; spot.x++)
		for (spot.y = 0 ; spot.y < DBLCROSS_BOARDHEIGHT ; spot.y++) {
			if (t->engine->IsSquareEmpty(spot))
				continue;
			tiles[n]->SetPri(10);
			owner = t->engine->GetSquareOwner(spot);
			if (owner == -1)
				tiles[n++]->Show(owner, t->engine->GetSquareSpecial(spot), -1, false, 0, spot);
			else
				tiles[n++]->Show(owner, t->engine->GetSquareLetter(spot), -1, t->engine->IsSquareStable(spot), GetTileCrackedState(spot), spot);
		}
	
	// use as many tiles as necessary to display the racks of all non-void seats
	
	for (i = 0 ; i < MAX_GAME_PLAYERS ; i++)
		if (t->gameState.players[i].playerID != PLAYER_ID_VOID) {
			for (j = 0 ; j < DBLCROSS_RACKSIZE ; j++)
				if (t->gameState.seats[i].rack[j] != '-') {
					if (tiles[n] == highlightedTile || tiles[n]->mouseDown) {
						n++;
						continue;
					}
					tiles[n]->SetPri(10);
					tiles[n++]->Show(i, t->gameState.seats[i].rack[j], j, false, 0, spot);
				}
			
			for (j = 0 ; j < DBLCROSS_BOMBRACKSIZE ; j++)
				if (t->gameState.seats[i].bombs[j] != '-') {
					if (tiles[n] == highlightedTile || tiles[n]->mouseDown) {
						n++;
						continue;
					}
					tiles[n]->SetPri(10);
					tiles[n++]->Show(i, t->gameState.seats[i].bombs[j], j, false, 0, spot);
				}
		}
	
	// hide the remaining tiles
	
	for ( ; n < NUM_TILE_VIEWS ; n++)
		tiles[n]->Hide();
}

void DoubleCrossDisplay::ShowButtons(DoubleCrossClientTable *t, bool myTurn, bool canDump)
{
	passButton->Show();
	newLettersButton->Show();
	
	if (!myTurn || t->GetSecondsRemaining() <= 0 || !t->enableActions) {
		passButton->Disable();
		newLettersButton->Disable();
	}
	else {
		passButton->Enable();
		if (canDump)
			newLettersButton->Enable();
		else
			newLettersButton->Disable();
	}
}

DoubleCrossTile *DoubleCrossDisplay::GetTileByBoardSpot(DCPoint spot)
{
	int i;
	
	for (i = 0 ; i < NUM_TILE_VIEWS ; i++) {
		if (tiles[i]->IsHidden())
			continue;
		if (tiles[i]->InRack())
			continue;
		if (tiles[i]->boardSpot.x == spot.x && tiles[i]->boardSpot.y == spot.y)
			return tiles[i];
	}
	
	return NULL;
}

DoubleCrossTile *DoubleCrossDisplay::GetTileByRackSpot(int rackSpot, int owner)
{
	int i;
	
	for (i = 0 ; i < NUM_TILE_VIEWS ; i++) {
		if (tiles[i]->IsHidden())
			continue;
		if (!tiles[i]->InRack())
			continue;
		if (tiles[i]->rackSpot == rackSpot && tiles[i]->owner == owner)
			return tiles[i];
	}
	
	return NULL;	
}

int DoubleCrossDisplay::GetTileCrackedState(DCPoint square)
{
	return crackedTiles[square.y][square.x];
}

void DoubleCrossDisplay::CrackTile(int crackCel, DCPoint square)
{
	crackedTiles[square.y][square.x] = crackCel;
}

void DoubleCrossDisplay::CircleEvaluatedWords(DoubleCrossClientTable *t, DCPoint square)
{
	bool foundStable, stable;
	int ind, i;
	DCPoint pt, ends[2], step[] = { { 0 , -1 }, { 0 , 1 }, { -1 , 0 }, { 1 , 0 } };
	POINT spot;

	// This function highlights, vertically and horizontally,
	// the word fragments that must be legal in order for this
	// to be a legal DoubleCross move.  Note that it does NOT
	// tell you whether the fragments actually ARE legal, just
	// which ones need to be legal.  It's up to you to decide
	// if you want to make the move based on this info.
	
	// Is the stuff for this square already displayed currently?
	if (square.x == evalOrigin.x && square.y == evalOrigin.y)
		return;

	// Can't play here if a tile is already here or if we are in limbo
	if (!t->engine->SquareIsLegal(square)) {
		if (evalOrigin.x >= 0)
			ClearEvaluatedWords();
		evalOrigin.x = evalOrigin.y = -1;
		return;
	}

	evalOrigin = square;
	ClearEvaluatedWords();
	
	// find the limits of the evaluated segment(s)
	
	for (i = 0 ; i < 4 ; i++) {
		foundStable = false;
		for (pt.x = square.x + step[i].x, pt.y = square.y + step[i].y ; 
				pt.x >= 0 && pt.x < DBLCROSS_BOARDWIDTH && pt.y >= 0 
				&& pt.y < DBLCROSS_BOARDHEIGHT ; 
				pt.x += step[i].x, pt.y += step[i].y) {
			if (t->engine->GetSquareOwner(pt) == -1)
				break;
			
			stable = t->engine->IsSquareStable(pt);
			if (foundStable && !stable)
				break;

			if (stable)
				foundStable = true;			
		}
		ends[i % 2].x = pt.x - step[i].x;
		ends[i % 2].y = pt.y - step[i].y;

		if (i % 2 == 1) {
			// do we have a circle in this direction?
			if (ends[0].x == ends[1].x && ends[0].y == ends[1].y)
				continue;

			// yes... draw the circles
			for (ind = 0, pt = ends[0] ; 
					pt.x <= ends[1].x && pt.y <= ends[1].y;
					pt.x += step[i].x, pt.y += step[i].y, ind++) {
				
				spot = t->BoardToScreenCoordinates(pt);
				evaluated[i/2][ind]->Posn(spot.x, spot.y);
				if (ind == 0)
					evaluated[i/2][ind]->SetCel((i/2)*3);
				else if (pt.x == ends[1].x && pt.y == ends[1].y)
					evaluated[i/2][ind]->SetCel((i/2)*3 + 2);
				else
					evaluated[i/2][ind]->SetCel((i/2)*3 + 1);

				evaluated[i/2][ind]->Show();
			}
		}
	}	
}

void DoubleCrossDisplay::ClearEvaluatedWords()
{
	int i, j;

	if (evalOrigin.x == -1)
		return;

	for (i = 0 ; i < 2 ; i++)
		for (j = 0 ; evaluated[i][j] && j < DBLCROSS_BOARDHEIGHT ; j++)
			evaluated[i][j]->Hide();
}

////////////////////////////////////////////////////////////////////////////////////////////////

DoubleCrossCharacter::DoubleCrossCharacter(DoubleCrossDisplay *theDisplay) : GameCharacter(theDisplay)
{
	scoreDisplay = NULL;
	tvScreen = NULL;
	tvOverlay = NULL;
}

DoubleCrossCharacter::~DoubleCrossCharacter()
{
	if (scoreDisplay) { delete scoreDisplay; scoreDisplay = NULL; }
	if (tvScreen) { delete tvScreen; tvScreen = NULL; }
	if (tvOverlay) { delete tvOverlay; tvOverlay = NULL; }
}

void DoubleCrossCharacter::Init(int newTable, int newSeat)
{
	static int directions[MAX_GAME_PLAYERS] = { GCD_FACING_RIGHT, GCD_FACING_LEFT, GCD_FACING_LEFT, GCD_FACING_RIGHT };
	static POINT p[MAX_GAME_PLAYERS] = { { 20, 40 }, { 522, 40 }, { 522, 234 }, { 20, 234 } };
	
	GameCharacter::Init(newTable, newSeat, p[newSeat].x, p[newSeat].y + ENTIRE_TABLE_OFFSET, directions[newSeat], CBO_NONE);
	
	scoreDisplay = new DoubleCrossScoreDisplay();
	scoreDisplay->plane = plane;
	scoreDisplay->Init();
	scoreDisplay->Posn(p[newSeat].x - 5, p[newSeat].y + 92 + 18);
	scoreDisplay->SetWidth(110);
	
	tvScreen = new DoubleCrossTV(display, newSeat);
	tvScreen->plane = plane;
	tvScreen->ParamInit(220502, p[newSeat].x - 20, p[newSeat].y, 10);
	
	tvOverlay = new sciProp();
	tvOverlay->plane = plane;
	tvOverlay->ParamInit(220503, p[newSeat].x - 20, p[newSeat].y, 550);
	
	nameText->SetFont(2);
	nameText->Posn(p[newSeat].x - 5, p[newSeat].y + 92);
	nameText->SetWidth(110);
	nameText->SetHeight(16);
	nameText->SetJust(JUST_HORIZONTAL | JUST_VERTICAL);
	// nameText->SetBackgroundColor(100, 100, 100);
	
	if (newSeat == 0 || newSeat == 3)
		chatBalloon->Posn(p[newSeat].x + 60, p[newSeat].y - 25);
	else
		chatBalloon->Posn(p[newSeat].x - 200, p[newSeat].y - 25);
	
	Hide();
}

void DoubleCrossCharacter::Show(Boolean showAsCurrentPlayer)
{
	// GameCharacter::Show(showAsCurrentPlayer);
	
	bootDisplay->Show(roomClient->tables[table].gameState.players[seat].bootCount);
	if (roomClient->tables[table].restartVotes[seat])
		resetDisplay->Show();
	else
		resetDisplay->Hide();
	
	sciActor::Show();
	
	plaque->Hide();
	nameText->Show();
	scoreText->Hide();
	plaqueHighlight->Hide();
	
	tvScreen->Show();
	tvOverlay->Show();
	
	if (showAsCurrentPlayer) {
		tvScreen->SetCycle(NULL);
		tvOverlay->SetCycle(NULL);
		if (!tvScreen->cycler) {
			tvScreen->SetCycle(new sciToggle(seat * 2, seat * 2 + 1));
			tvScreen->cycleSpeed = 20;
		}
		if (!tvOverlay->cycler) {
			tvOverlay->SetCycle(new sciToggle(seat * 2, seat * 2 + 1));
			tvOverlay->cycleSpeed = 20;
		}
	}
	else {
		tvScreen->SetCel(seat * 2);
		tvOverlay->SetCel(seat * 2);
		tvScreen->SetCycle(NULL);
		tvOverlay->SetCycle(NULL);
	}
}

void DoubleCrossCharacter::Hide(Boolean onlyShadow /* = FALSE */)
{
	if (onlyShadow) {
		sciActor::Hide();
		faceMakerView->Hide();
		
		return;
	}
	
	GameCharacter::Hide();
	
	scoreDisplay->Hide();
	tvScreen->Hide();
	tvOverlay->Hide();
}

////////////////////////////////////////////////////////////////////////////////////////////////

DoubleCrossTimerDisplay::DoubleCrossTimerDisplay(DoubleCrossClientTable *theTable)
{
	int i;
	
	table = theTable;
	
	for (i = 0 ; i < 2 ; i++)
		digits[i] = NULL;
}

DoubleCrossTimerDisplay::~DoubleCrossTimerDisplay()
{
	int i;
	
	for (i = 0 ; i < 2 ; i++)
		if (digits[i]) { delete digits[i]; digits[i] = NULL; }
}

void DoubleCrossTimerDisplay::Init()
{
	int i;
	
	sciProp::Init();
	
	for (i = 0 ; i < 2 ; i++) {
		digits[i] = new sciView();
		digits[i]->plane = plane;
		digits[i]->ParamInit(30028, 304 + i * 15, 9, 2);
	}
}

void DoubleCrossTimerDisplay::Show(int seconds)
{
	int i;
	
	if (seconds <= 10 && !table->playedTenSecondWarningSound) {
		game->sound1->Play(1001);
		table->playedTenSecondWarningSound = TRUE;
	}
	
	if (seconds == 0 && !table->playedOutOfTimeSound) {
		game->sound1->Play(1002);
		table->playedOutOfTimeSound = TRUE;
		table->Display();
	}
	
	digits[0]->SetCel(seconds / 10);
	digits[1]->SetCel(seconds % 10);
	
	for (i = 0 ; i < 2 ; i++)
		digits[i]->Show();
	
	if (table->gameState.currentPlayer >= 0 && table->gameState.currentPlayer < MAX_GAME_PLAYERS)
		SetCel(table->gameState.currentPlayer);
	else
		SetCel(0);
	
	sciProp::Show();
}

void DoubleCrossTimerDisplay::Hide()
{
	int i;
	
	sciProp::Hide();
	
	for (i = 0 ; i < 2 ; i++)
		digits[i]->Hide();
}

void DoubleCrossTimerDisplay::Doit()
{
	int n;
	
	n = table->GetSecondsRemaining();
	
	Show(n);
	
	// if it's my turn and I'm out of time, pass
	
	if (table->gameState.gamePhase == DCGP_PLAY && table->gameState.currentPlayer == table->GetMyPlayingIndex(FALSE))
		if (n <= 0 && !table->answeredMostRecentRequest)
			table->FinishREQUESTPLAY(DCPLAY_DONE, 0, 0, table->engine->MakeDCPoint(0, 0));
}

////////////////////////////////////////////////////////////////////////////////////////////////

DoubleCrossButton::DoubleCrossButton(DoubleCrossClientTable *theTable, int theID)
{
	table = theTable;
	
	id = theID;
}

void DoubleCrossButton::DoClick(Boolean netMessage)
{
	// char c;
	// int i, j, a, b, n;
	
	if (table->GetMyPlayingIndex(FALSE) != table->gameState.currentPlayer)
		return;
	
	if (table->answeredMostRecentRequest)
		return;
	
	switch (id) {
		case DCBID_PASS:			
			// setting value to 1 when pressing button to pass triggers pass sound
			table->FinishREQUESTPLAY(DCPLAY_DONE, 1, 0, table->engine->MakeDCPoint(0, 0));
			break;
		case DCBID_NEWLETTERS:
			if (table->gameState.seats[table->gameState.currentPlayer].canDump)
				table->FinishREQUESTPLAY(DCPLAY_DUMP, 0, 0, table->engine->MakeDCPoint(0, 0));
			break;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////

DoubleCrossCommentsDisplay::DoubleCrossCommentsDisplay(DoubleCrossClientTable *theTable)
{
	table = theTable;
	lines[0] = lines[1] = NULL;
	priority[0] = priority[1] = 0;
	specState = false;
	bombState = false;
	doubler = NULL;
}

DoubleCrossCommentsDisplay::~DoubleCrossCommentsDisplay()
{
	int i;
	
	for (i = 0 ; i < 2 ; i++)
		if (lines[i])
			{ delete lines[i]; lines[i] = NULL; }
		
	if (doubler) 
		{ delete doubler; doubler = NULL; }
}

void DoubleCrossCommentsDisplay::Init()
{
	int i;
	POINT pos[] = { {376, 1}, {376, 16} };
	
	for (i = 0 ; i < 2 ; i++) {
		if (!lines[i]) {
			lines[i] = new sciText;
			lines[i]->plane = table->plane;
			lines[i]->ParamInit(4, "", 244, 15, 
				pos[i].x, pos[i].y, 5, 5);
			lines[i]->SetJust(JUST_VERTICAL | JUST_HORIZONTAL);
		}
	}

	if (!doubler) {
		doubler = new sciView;
		doubler->plane = table->plane;
		doubler->ParamInit(30060, 0, 0, 5);
	}
}

void DoubleCrossCommentsDisplay::Show()
{
	if (priority[0])
		lines[0]->Show();
	if (priority[1])
		lines[1]->Show();
}

void DoubleCrossCommentsDisplay::Hide()
{
	if (priority[0])
		lines[0]->Hide();
	if (priority[1])
		lines[1]->Hide();
}

void DoubleCrossCommentsDisplay::AddComment(int eventType, char *line, int mysteryType)
{
	int ind, myPriority;

	// This function determines what gets shown in the comments.  Not
	// all comments are created equal; depending on what the move value
	// is, we decide whether we want to show the comment or not

	// Ignore blank lines
	if (!line || (line && line[0] == 0))
		return;

	// Check verbose option, which winnows out most comments
	if (roomClient->globalIniFile->ReadValue("DC_VERBOSE", 1) != 1) {
		if (eventType != -1 && eventType != DCMOVE_COLORBOMB && 
				eventType != DCMOVE_MYSTERY )
			return;
	}

	// For now, throw out extraneous comments
	ind = (priority[0]) ? (priority[1] ? -1 : 1) : 0;

	myPriority = GetEventPriority(eventType, mysteryType);

	// During an explosion, we always keep the bomb exploding message on top
	// line, no matter the priority of other messages
	if (eventType == DCMOVE_DIREXPLOSION || eventType == DCMOVE_COLORBOMB) {
		bombState = true;
		Hide();

		priority[0] = myPriority;
		lines[0]->SetText(line);
		Show();
		return;
	}

	if (bombState) {
		if ( !priority[1] || myPriority >= priority[1])
			SetLine(1, line, myPriority);		
	}
	else if (ind < 0) {
		// We need to replace a current displayed line if possible
		if (myPriority >= priority[0]) {
			SetLine(1, lines[0]->GetText(), priority[0]);
			SetLine(0, line, myPriority);			
		}
		else if (myPriority >= priority[1]) {
			SetLine(1, line, myPriority);			
		}
	}
	else if (ind == 1) {
		// Do we move the current line to the top spot?  Yes if priority
		// is greater
		if (myPriority >= priority[0]) {
			ind = 0;
			SetLine(1, lines[0]->GetText(), priority[0]);			
		}
		SetLine(ind, line, myPriority);		
	}
	else
		SetLine(0, line, myPriority);	

	Show();	
}

void DoubleCrossCommentsDisplay::ClearComments()
{
	Hide();
	
	priority[0] = priority[1] = 0;
	bombState = false;	
}

void DoubleCrossCommentsDisplay::SetLine(int which, char *text, int linePriority)
{
	lines[which]->SetText(text);
	priority[which] = linePriority;
}

short DoubleCrossCommentsDisplay::GetEventPriority(int eventType, int mysteryType)
{
	static int priority[] = {0, /* DCMOVE_MOVE_TILE */ 1, /* DCMOVE_WORD_MADE */ 5,
		/* DCMOVE_TILE_STABLIZED */ 1, /* DCMOVE_TILE_EXPLODED */ 6,
		/* DCMOVE_BONUS */ 3, /* DCMOVE_COLORBOMB */ 7,
		/* DCMOVE_PICKUPBOMB */ 3, /* DCMOVE_DIREXPLOSION */ 7,
		/* DCMOVE_MYSTERY */ 4, /* DCMOVE_DUMP_AND_ADVANCE */ 1, 
		/* DCMOVE_FILL_AND_ADVANCE */ 1, /* DCMOVE_PLACE_BOMB */ 1,
		/* DCMOVE_INVALID_TILE */ 0, /* DCMOVE_INVALID_BOMB*/ 0};
	static int mysteryPriority[] = { /* Bonus */ 3, /* DCMYS_DOUBLER */ 6, 
		/* DCMYS_FILLRACK */ 6, /* DCMYS_BLACKHOLE */ 6, /* DCMYS_REDBOMB */ 7, 
		/* DCMYS_YELLOWBOMB */ 7, /* DCMYS_BLUEBOMB */ 7, /* DCMYS_GREENBOMB */ 7,
		/* DCMYS_NSBOMB */ 3, /* DCMYS_EWBOMB */ 3, /* DCMYS_NESWBOMB */ 3 };
	
	if (eventType < 0)
		return 6;
	
	if (eventType == DCMOVE_MYSTERY) {
		if (mysteryType < DCMYS_DOUBLER)
			mysteryType = 0;
		else
			mysteryType -= DCMYS_CURBONUS;
		
		return  mysteryPriority[mysteryType];		
	}

	return priority[eventType];
}

void DoubleCrossCommentsDisplay::SetDoubler(bool isActive)
{
	if (specState == isActive)
		return;
	
	specState = isActive;
	if (specState)
		doubler->Show();
	else 
		doubler->Hide();
	
	Show();
}

////////////////////////////////////////////////////////////////////////////////////////////////

DoubleCrossScoreDisplay::DoubleCrossScoreDisplay()
{
	int i;
	
	for (i = 0 ; i < 4 ; i++)
		digits[i] = NULL;
}

DoubleCrossScoreDisplay::~DoubleCrossScoreDisplay()
{
	int i;
	
	for (i = 0 ; i < 4 ; i++)
		if (digits[i]) { delete digits[i]; digits[i] = NULL; }
}

void DoubleCrossScoreDisplay::Init()
{
	int i;
	
	for (i = 0 ; i < 4 ; i++) {
		digits[i] = new sciView();
		digits[i]->plane = plane;
		digits[i]->ParamInit(30028, 0, 0, 10);
	}
}

void DoubleCrossScoreDisplay::Show(int score)
{
	char s[20];
	int i, numDigits, offset;
	
	// TODO: put in sounds for positive/negative scores?
/*	if ( points >= 0 ) {
		if ( m_player == dblCrossRoom->m_display->m_curPlayer )
			dblCrossRoom->sound2->Play(DCSOUND_SCOREPLUS);
		else
			dblCrossRoom->sound2->Play(DCSOUND_OPPSCOREPLUS);
	}
	else {
		if ( m_player == dblCrossRoom->m_display->m_curPlayer )
			dblCrossRoom->sound2->Play(DCSOUND_SCOREMINUS);
		else
			dblCrossRoom->sound2->Play(DCSOUND_OPPSCOREMINUS);
	}
*/
	if (digits[0] == NULL)
		return;
	
	Hide();
	
	sprintf(s, "%d", score);
	
	numDigits = strlen(s);
	
	offset = (GetWidth() - numDigits * DBLCROSS_SCOREDIGITWIDTH) / 2;
	
	for (i = 0 ; i < numDigits ; i++) {
		digits[i]->SetCel(s[i] - '0');
		digits[i]->Posn(GetX() + offset + i * DBLCROSS_SCOREDIGITWIDTH, GetY());
		digits[i]->Show();
	}
}

void DoubleCrossScoreDisplay::Hide()
{
	int i;
	
	if (digits[0] == NULL)
		return;
	
	for (i = 0 ; i < 4 ; i++)
		digits[i]->Hide();
}

////////////////////////////////////////////////////////////////////////////////////////////////

DoubleCrossTile::DoubleCrossTile(DoubleCrossClientTable *theTable, int theIndex)
{
	int i;
	
	table = theTable;
	index = theIndex;
	
	owner = 0;
	tileValue = 'A';
	rackSpot = 0;
	stable = false;
	cracked = false;
	mouseDown = false;
	
	letterView = NULL;
	crackView = NULL;
	
	for (i = 0 ; i < 3 ; i++)
		bonusViews[i] = NULL;
}

DoubleCrossTile::~DoubleCrossTile()
{
	int i;
	
	if (letterView) { delete letterView; letterView = NULL; }
	if (crackView) { delete crackView; crackView = NULL; }
	for (i = 0 ; i < 3 ; i++)
		if (bonusViews[i]) { delete bonusViews[i]; bonusViews[i] = NULL; }
}

void DoubleCrossTile::Init()
{
	int i;
	
	sciActor::Init();
	
	letterView = new sciView();
	letterView->plane = plane;
	letterView->ParamInit(30002, 0, 0, 0);
	
	crackView = new sciView();
	crackView->plane = plane;
	crackView->ParamInit(30062, 0, 0, 25);
	
	for (i = 0 ; i < 3 ; i++) {
		bonusViews[i] = new sciView();
		bonusViews[i]->plane = plane;
		bonusViews[i]->ParamInit(30042, 0, 0, 0);
	}
}

void DoubleCrossTile::Show(int theOwner, char theTileValue, int theRackSpot, bool theStable, int theCracked, DCPoint theBoardSpot, bool theHighlight)
{
	int view, bonus;
	POINT p;
	static int specialViews[] = { 0, 30047, 30048, 30049, 30043, 30044, 30045, 30046,
								  0, 0, 0, 0, 30038, 30039, 30055, 30041 };
	owner = theOwner;
	tileValue = theTileValue;
	rackSpot = theRackSpot;
	stable = theStable;
	cracked = theCracked;
	boardSpot = theBoardSpot;
	
	Hide();
	
	if (rackSpot != -1)
		p = table->RackToScreenCoordinates(owner, rackSpot, tileValue <= DCSS_BOMB);
	else
		p = table->BoardToScreenCoordinates(boardSpot);
	
	Posn(p.x, p.y);
	
	if (tileValue < 'A') { // special
		SetView((this == table->display->highlightedTile) ? 30037 : 30036);
		if (tileValue <= 0 || tileValue > DCSS_FILLRACK) {
			assert(false);
			view = 0;
		}
		else
			view = specialViews[tileValue];
		
		if (view != 0) {
			letterView->SetView(view);
			letterView->Show();
		}
		
		if (tileValue == DCSS_BONUS) {
			bonus = table->engine->GetNthBonusValue(table->gameState.bonusIndex);
			bonusViews[0]->SetCel(10); // the '+'
			bonusViews[0]->Show();
			if (bonus > 9) {
				bonusViews[1]->SetCel(bonus / 10);
				bonusViews[1]->Show();
				bonusViews[2]->SetCel(bonus % 10);
				bonusViews[2]->Show();
			}
			else {
				bonusViews[1]->SetCel(bonus);
				bonusViews[1]->Show();
			}
		}
	}
	else {
		if (stable) {
			SetView(30035);
			if (owner == DBLCROSS_SEED_PLAYER)
				letterView->SetView(30005);
			else
				letterView->SetView(30006 + owner);
		}
		else {
			SetView(30034);
			SetCel((theOwner * 2) + ((this == table->display->highlightedTile || theHighlight) ? 1 : 0));
			letterView->SetView(30002);
			
			if (cracked) {
				assert(rackSpot == -1);
				crackView->SetCel(cracked - 1);
				crackView->Show();
			}
		}
		letterView->SetCel(tileValue - 'A');
		letterView->Show();
	}
	
	sciActor::Show();
}

void DoubleCrossTile::ShowBonus(int bonus, BYTE alpha)
{
	int i;
	
	bonusViews[0]->SetCel((bonus < 0) ? 11 : 10); // '-' : '+'
	bonusViews[0]->Show();
	
	bonus = abs(bonus);

	if (bonus > 9) {
		bonusViews[1]->SetCel(bonus / 10);
		bonusViews[1]->Show();
		bonusViews[2]->SetCel(bonus % 10);
		bonusViews[2]->Show();
	}
	else {
		bonusViews[1]->SetCel(bonus);
		bonusViews[1]->Show();
	}
	
	for (i = 0 ; i < 3 ; i++)
		bonusViews[i]->SetAlpha(alpha);
}

void DoubleCrossTile::Hide()
{
	int i;
	
	sciActor::Hide();
	
	if (letterView)
		letterView->Hide();
	if (crackView)
		crackView->Hide();
	for (i = 0 ; i < 3 ; i++)
		if (bonusViews[i])
			bonusViews[i]->Hide();
	
	mouseDown = false;
}

void DoubleCrossTile::Posn(int newX, int newY)
{
	sciActor::Posn(newX, newY);
	
	if (letterView) {
		if (tileValue <= DCSS_BOMB)
			letterView->Posn(newX + 1, newY - 2);
		else
			letterView->Posn(newX, newY);
	}
	if (crackView)
		crackView->Posn(newX, newY);
	
	if (bonusViews[0])
		bonusViews[0]->Posn(newX + 1, newY + GetHeight() / 3);
	if (bonusViews[1])
		bonusViews[1]->Posn(newX + bonusViews[0]->GetWidth() - 1, newY + GetHeight() / 3);
	if (bonusViews[2])
		bonusViews[2]->Posn(newX + bonusViews[0]->GetWidth() * 2 - 3, newY + GetHeight() / 3);
}

void DoubleCrossTile::SetPri(int newPri)
{
	int i;
	
	sciActor::SetPri(newPri);
	
	if (letterView)
		letterView->SetPri(newPri + 1);
	if (crackView)
		letterView->SetPri(newPri + 2);
	for (i = 0 ; i < 3 ; i++)
		if (bonusViews[i])
			bonusViews[i]->SetPri(newPri + 3);
}

Boolean DoubleCrossTile::HandleEvent(sciEvent &event)
{
	POINT p, pRack;
	DCPoint toSquare;
	MTS_DOUBLECROSS_DEFINITION_MESSAGE mts;
	
	if (event.claimed)
		return FALSE;
	
	if (table->display->popup->HandleEvent(event))
		return event.claimed;
	
	if (!table->enableActions)
		return event.claimed;
	
	if (event.type == MOUSE_DOWN && sciFeature::OnMe(event) && table->readyForMessages)
		if (!InRack() && table->engine->GetSquareSpecial(boardSpot) == DCBT_EMPTY) {
			event.claimed = TRUE;
			
			mts.messageType = MTS_DOUBLECROSS_DEFINITION;
			mts.table = table->index;
			mts.playerID = roomClient->GetMyID();
			mts.seat = table->GetMyPlayingIndex(FALSE);
			mts.gameNumber = table->gameState.gameNumber;
			
			mts.square = boardSpot;
			mts.preferHorizontal = (event.modifiers & LEFT_MOUSE) != 0;
			
			roomClient->SendMessageToServer((char *)&mts, sizeof(MTS_DOUBLECROSS_DEFINITION_MESSAGE));
			
			return event.claimed;
		}
	
	if (table->GetMyPlayingIndex(FALSE) != table->gameState.currentPlayer)
		return event.claimed;
	
	switch (event.type) {
		case MOUSE_DOWN:
			if (!sciFeature::OnMe(event))
				return FALSE;
			if (!InRack())
				return FALSE;
			if (table->GetSecondsRemaining() <= 0)
				return FALSE;
			if (table->answeredMostRecentRequest)
				return FALSE;
			if (this == table->display->highlightedTile)
				return FALSE; // mouse up will clear the highlight
			if (owner != table->GetMyPlayingIndex(FALSE))
				return FALSE;
			if ((event.modifiers & LEFT_MOUSE) && !mouseDown) {
				if (table->display->highlightedTile) {
					table->display->highlightedTile = NULL;
					table->Display();
				}
				// table->SetReadyForMessages(FALSE);
				event.claimed = TRUE;
				mouseDown = TRUE;
				xOffset = event.x - GetX();
				yOffset = event.y - GetY();
				SetPri(600);
			}
			break;
		case MOUSE_MOVE:
			if (mouseDown) {
				Posn(event.x - xOffset, event.y - yOffset);
				p.x = event.x;
				p.y = event.y;
			
				// show selection for tiles to be evaluated

				if (table->ScreenToBoardCoordinates(p, &toSquare)) {
					if (tileValue > DCSS_BOMB  && table->enableActions) {
						table->display->CircleEvaluatedWords(table, toSquare);
					}
				}
				else
					table->display->ClearEvaluatedWords();
			}
			break;

		case MOUSE_UP:
			table->display->ClearEvaluatedWords();
			table->display->evalOrigin.x = table->display->evalOrigin.y = -1;
			if ((event.modifiers & LEFT_MOUSE) && mouseDown) {
				mouseDown = FALSE;
				event.claimed = TRUE;
				// SetPri(10); // priority will get reset when Display() is called
				// table->SetReadyForMessages(TRUE);
				if (table->GetSecondsRemaining() <= 0)
					{ table->Display(); break; }
				if (table->answeredMostRecentRequest)
					{ table->Display(); break; }
				
				p.x = event.x;
				p.y = event.y;
				
				if (!table->ScreenToBoardCoordinates(p, &toSquare)) {
					pRack = table->RackToScreenCoordinates(table->gameState.currentPlayer, rackSpot, tileValue <= DCSS_BOMB);
					
					if (abs(pRack.x - (p.x - xOffset)) > 2 || abs(pRack.y - (p.y - yOffset)) > 2) {
						table->display->highlightedTile = NULL;
						table->Display();
					}
					else {
						if (table->display->highlightedTile == NULL) {
							table->display->highlightedTile = this;
							SetPri(600);
							Show(owner, tileValue, rackSpot, stable, cracked, boardSpot);
						}
						else {
							table->display->highlightedTile = NULL;
							table->Display();
						}
					}
					
					break;
				}
				
				if (tileValue >= 'A' && !table->engine->SquareIsLegal(toSquare)) {
					table->display->commentsDisplay->ClearComments();
					
					if (table->engine->GetSquareSpecial(toSquare) == DCSS_BLOCK)
						table->display->commentsDisplay->AddComment(-1, 
							"Can't Play on Block Square.");
					else
						table->display->commentsDisplay->AddComment(-1, 
							"Illegal Move! Tile must connect to stable tile.");
					
					table->display->highlightedTile = NULL;
					table->Display();
					
					break;
				}
				
				/*
				if (table->ScreenCoordinatesToRackIndex(p, &rackIndex)) {
					toSquare.x = -1;
					toSquare.y = rackIndex;
				}
				else
					if (!table->ScreenToBoardCoordinates(p, &toSquare))
						{ table->Display(); break; }
				*/
				
				if (tileValue <= DCBT_NESW)
					table->FinishREQUESTPLAY(DCPLAY_PLACE_BOMB, tileValue, rackSpot, toSquare);
				else
					table->FinishREQUESTPLAY(DCPLAY_PLACE_TILE, tileValue, rackSpot, toSquare);
			}
			break;
	}
	
	return event.claimed;
}

////////////////////////////////////////////////////////////////////////////////////////////////

DoubleCrossBoardDisplay::DoubleCrossBoardDisplay(DoubleCrossClientTable *theTable)
{
	table = theTable;
}

DoubleCrossBoardDisplay::~DoubleCrossBoardDisplay()
{
}

Boolean DoubleCrossBoardDisplay::HandleEvent(sciEvent &event)
{
	DCPoint square;
	POINT p;
	DoubleCrossTile *tile;
	
	if (event.claimed)
		return FALSE;
	
	if (table->display->popup->HandleEvent(event))
		return event.claimed;
	
	if (table->GetMyPlayingIndex(FALSE) != table->gameState.currentPlayer)
		return event.claimed;
	
	if (!table->enableActions)
		return event.claimed;
	
	if (table->display->highlightedTile == NULL)
		return event.claimed;
	
	tile = table->display->highlightedTile;
	
	switch (event.type) {
		case KEY_DOWN:
			if (event.c == 27) {
				table->display->highlightedTile = NULL;
				table->Display();
			}
			break;
		
		case MOUSE_MOVE:
			tile->Posn(event.x - tile->xOffset, event.y - tile->yOffset);
			p.x = event.x;
			p.y = event.y;
			
			// show selection for tiles to be evaluated

			if (table->ScreenToBoardCoordinates(p, &square)) {
				if (table->display->highlightedTile && table->display->highlightedTile->tileValue > DCSS_BOMB  &&
					table->enableActions) {
					table->display->CircleEvaluatedWords(table, square);
				}
			}
			else
				table->display->ClearEvaluatedWords();
			break;
		
		case MOUSE_UP:
			tile->mouseDown = TRUE;
			tile->HandleEvent(event);
			break;
	}
	
	return event.claimed;
}

////////////////////////////////////////////////////////////////////////////////////////////////

DoubleCrossTV::DoubleCrossTV(DoubleCrossDisplay *theDisplay, int theSeat)
{
	display = theDisplay;
	seat = theSeat;
}

DoubleCrossTV::~DoubleCrossTV()
{
}

Boolean DoubleCrossTV::HandleEvent(sciEvent &event)
{
	if (display->popup->HandleEvent(event))
		return event.claimed;
	
	if (event.type != MOUSE_UP || !sciFeature::OnMe(event))
		return FALSE;
	
	if (roomClient->tables[display->table].GetMyPlayingIndex(FALSE) >= MAX_GAME_PLAYERS)
		return FALSE;
	
	if (roomClient->tables[display->table].gameState.players[seat].playerID == PLAYER_ID_EMPTY) {
		event.claimed = TRUE;
		
		display->popup->SetNumItems(1);
		display->popup->seat = seat;
		display->popup->AddItem(POPID_GAME_SPECIFIC_BASE, POPITEM_SELECTABLE, "Remove From Game");
		display->popup->Posn(GetX(), GetY() + GetHeight());
		display->popup->Show(this);
	}
	
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////

DoubleCrossEventScript::DoubleCrossEventScript(DoubleCrossClientTable *theTable, MFS_DOUBLECROSS_EVENT_MESSAGE *theMFS, char *theComment)
{
	table = theTable;
	mfs = (*theMFS);
	if (theComment)
		strnzcpy(comment, theComment, 255);
	else
		comment[0] = 0;
}

DoubleCrossEventScript::~DoubleCrossEventScript()
{
	table = NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////

void DoubleCrossMoveTileScript::ChangeState(int newState)
{
	int i;
	POINT p;
	DoubleCrossTile *tile;
	
	sciScript::ChangeState(newState);
	
	switch (state) {
		case 0:
			SetNextState(1);
			table->display->commentsDisplay->ClearComments();
			table->display->commentsDisplay->AddComment(DCMOVE_MOVE_TILE, comment);
			
			if (mfs.eventType == DCMOVE_MOVE_TILE) {
				table->gameState.seats[mfs.who].rack[mfs.rackSpot] = '-';
				table->engine->PlayLetterOnSquare(mfs.who, mfs.tileValue, mfs.start);
			}
			else
				table->gameState.seats[mfs.who].bombs[mfs.rackSpot] = '-';			
			
			// if it's my play, just do a display to show the tile in its new position
			
			if (mfs.seat == table->GetMyPlayingIndex(FALSE) && mfs.eventType == DCMOVE_MOVE_TILE) {
				SetTicks(0);
				break;
			}
			
			// otherwise, animate the move
			
			// find the tile to be moved
			
			for (i = 0 ; i < NUM_TILE_VIEWS ; i++) {
				tile = table->display->tiles[i];
				if (tile->IsHidden())
					continue;
				if (!tile->InRack())
					continue;
				if (tile->tileValue != mfs.tileValue)
					continue;
				if (tile->owner == mfs.who && tile->rackSpot == mfs.rackSpot)
					break;
			}
			
			if (i == NUM_TILE_VIEWS) {
				assert(false);
				SetTicks(0);
				break;
			}
			
			p = table->BoardToScreenCoordinates(mfs.start);
			
			if (mfs.eventType == DCMOVE_MOVE_TILE || (mfs.eventType == DCMOVE_PLACE_BOMB && mfs.seat != table->GetMyPlayingIndex(FALSE))) {
				tile->SetPri(600);
				tile->moveSpeed = 4;
				tile->SetMotion(new sciMoveTo(this, p.x, p.y));
			}
			else {
				// position bomb over current square
				tile->Posn(p.x, p.y);
				SetTicks(0);
			}
			break;
		
		case 1:
			game->sound1->Play(1006);
			
			table->SetReadyForMessages(TRUE);
			if (mfs.eventType == DCMOVE_MOVE_TILE) 
				table->Display();
			else
				table->dontDisplay = true;
			((sciScript *)client)->SetScript(NULL);
			break;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////

void DoubleCrossWordMadeScript::ChangeState(int newState)
{
	DCPoint step, spot;
	DoubleCrossTile *tile;
	
	sciScript::ChangeState(newState);
	
	switch (state) {
		case 0:
			table->gameState.seats[mfs.who].score = mfs.score;
			table->display->commentsDisplay->AddComment(DCMOVE_WORD_MADE, comment);
			
			if (mfs.start.x == mfs.finish.x)
				step = table->engine->MakeDCPoint(0, 1);
			else
				step = table->engine->MakeDCPoint(1, 0);
			
			for (spot = mfs.start ;
				 spot.x <= mfs.finish.x && spot.y <= mfs.finish.y ;
				 spot.x += step.x, spot.y += step.y) {
				tile = table->display->GetTileByBoardSpot(spot);
				if (tile == NULL)
					{ assert(false); continue; }
				tile->SetView(30034);
				tile->SetCel((mfs.who * 2) + 1);
			}
			
			game->sound1->Play(1008);
			SetNextState(1);
			SetTicks(25);
			break;
		
		case 1:
			table->Display();
			SetNextState(2);
			SetTicks(25);
			break;
		
		case 2:
			table->SetReadyForMessages(TRUE);
			((sciScript *)client)->SetScript(NULL);
			break;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////

void DoubleCrossTileStabilizedScript::ChangeState(int newState)
{
	sciScript::ChangeState(newState);
	
	switch (state) {
		case 0:
			table->engine->StabilizeSquare(mfs.start, 0);
			table->gameState.seats[mfs.who].score = mfs.score;
			table->display->commentsDisplay->AddComment(DCMOVE_TILE_STABLIZED, comment);
			table->Display();
			
			game->sound1->Play(1007);
			SetNextState(1);
			SetTicks(20);
		
		case 1:
			table->SetReadyForMessages(TRUE);
			table->Display();
			((sciScript *)client)->SetScript(NULL);
			break;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////

void DoubleCrossTileExplodedScript::ChangeState(int newState)
{
	sciScript::ChangeState(newState);
	
	switch (state) {
		case 0:
			tile = table->display->GetTileByBoardSpot(mfs.start);
			if (tile == NULL)
				{ assert(false); SetNextState(10); break; }
			
			explosion = new sciView();
			explosion->plane = table->plane;
			explosion->ParamInit(30064, tile->GetX() - 12, tile->GetY() - 19, 40);
			
			table->display->commentsDisplay->AddComment(DCMOVE_TILE_EXPLODED, comment);
			table->Display();
			
			SetNextState(1);
			SetTicks(0);
			break;
		
		case 1: // small smoke puff
			explosion->SetCel(0);
			explosion->Show();
			
			SetNextState(2);
			SetTicks(10);
			break;
		
		case 2: // big smoke puff
			explosion->SetCel(1);
			game->sound1->Play(1009);
			
			SetNextState(3);
			SetTicks(10);
			break;
		
		case 3: // going...
			tile->Hide();
			table->engine->ClearSquare(mfs.start, 0);
			explosion->SetCel(2);
			
			SetNextState(4);
			SetTicks(10);
			break;
		
		case 4: // going...
			explosion->SetCel(3);
			
			SetNextState(5);
			SetTicks(10);
			break;
		
		case 5: // gone...
			explosion->SetCel(4);
			
			SetNextState(6);
			SetTicks(8);
			break;
		
		case 6: // possibly replace special tile underneath
			if (mfs.tileValue != -1)
				table->engine->SetSquareSpecial(mfs.start, mfs.tileValue);
			
			delete explosion;
			explosion = NULL;
			
			table->gameState.seats[mfs.who].score = mfs.score;
			
			SetNextState(10);
			SetTicks(0);
			break;
		
		case 10:
			table->SetReadyForMessages(TRUE);
			table->Display();
			((sciScript *)client)->SetScript(NULL);
			break;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////

void DoubleCrossBonusScript::ChangeState(int newState)
{
	BYTE alpha;
	
	sciScript::ChangeState(newState);
	
	switch (state) {
		case 0:
			tile = table->display->GetTileByBoardSpot(mfs.start);
			if (tile == NULL)
				{ assert(false); SetNextState(2); break; }
			
			table->display->commentsDisplay->AddComment(DCMOVE_BONUS, comment);
			
			if (mfs.eventType == DCMOVE_MYSTERY && mfs.mysteryValue != DCMYS_CURBONUS)
				bonus = mfs.mysteryValue;
			else
				bonus = table->engine->GetNthBonusValue(table->gameState.bonusIndex);
			
			tile->ShowBonus(bonus, 0);
			
			if (bonus > 0) 
				game->sound1->Play(1014);
			else if (bonus == 0)
				game->sound1->Play(1015);
			else
				game->sound1->Play(1016);
			
			SetNextState(1);
			SetTicks(0);
			break;
		
		case 1: // fade in
			alpha = tile->bonusViews[0]->GetAlpha();
			
			if (alpha == 250) {
				tile->ShowBonus(bonus, 255);
				SetNextState(2);
				SetTicks(25);
				break;
			}
			
			tile->ShowBonus(bonus, alpha + 50);
			SetNextState(1);
			SetTicks(4);
			break;
		
		case 2:
			if (mfs.eventType == DCMOVE_BONUS)
				table->gameState.bonusIndex = mfs.rackSpot;

			table->gameState.seats[mfs.who].score = mfs.score;
			
			table->SetReadyForMessages(TRUE);
			table->Display();
			((sciScript *)client)->SetScript(NULL);
			break;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////

void DoubleCrossColorBombScript::ChangeState(int newState)
{
	int i;
	POINT p;
	DCPoint spot, step[] = { { 1 , 1 }, { -1 , 1 }, { -1 , -1 }, { 1 , -1 } };
	DCPoint corners[] = { { 0 , 0 }, 
						{ DBLCROSS_BOARDWIDTH - 1, 0 }, 
						{DBLCROSS_BOARDWIDTH - 1, DBLCROSS_BOARDHEIGHT - 1 }, 
						{ 0, DBLCROSS_BOARDHEIGHT - 1 } };
	
	sciScript::ChangeState(newState);
	
	switch (state) {
		case 0:	// Initialize explosion
			table->display->commentsDisplay->AddComment(DCMOVE_COLORBOMB, comment);
			table->gameState.seats[mfs.seat].score = mfs.score;

			// defuse the bomb if it is the same color as current player

			if (mfs.tileValue == 1) {
				game->sound1->Play(1023);
				SetNextState(4);
				SetTicks(0);
				break;
			}

			game->sound1->Play(1021);

			if (mfs.who < 0 || mfs.who > 3) {
				assert(false);
				break;
			}
			
			for (i = 0 ; i < DBLCROSS_BOARDWIDTH + DBLCROSS_BOARDHEIGHT - 1; i++) {
				shrapnelViews[i] = new sciView;
				shrapnelViews[i]->plane = table->plane;
				shrapnelViews[i]->ParamInit(30051, 0, 0, 30);				
			}
			
			factor = 0; 

			SetNextState(1);
			SetTicks(0);
			
			break;
			
		case 1: // Initial step for shrapnel
			factor++;

			// initial stage contains three new pieces of shrapnel; every other stage
			// will add two new pieces
			
			spot = table->engine->MakeDCPoint(corners[mfs.who].x + step[mfs.who].x, corners[mfs.who].y);
			p = table->BoardToScreenCoordinates(spot);
			CrackTile(spot);
			shrapnelViews[0]->Posn(p.x, p.y);
			shrapnelViews[0]->SetCel(random(5));
			shrapnelViews[0]->Show();

			spot = table->engine->MakeDCPoint(corners[mfs.who].x, corners[mfs.who].y + step[mfs.who].y);
			p = table->BoardToScreenCoordinates(spot);
			CrackTile(spot);
			shrapnelViews[1]->Posn(p.x, p.y);
			shrapnelViews[1]->SetCel(random(5));
			shrapnelViews[1]->Show();

			spot = table->engine->MakeDCPoint(corners[mfs.who].x + step[mfs.who].x, corners[mfs.who].y + step[mfs.who].y);
			p = table->BoardToScreenCoordinates(spot);
			CrackTile(spot);
			shrapnelViews[2]->Posn(p.x, p.y);
			shrapnelViews[2]->SetCel(random(5));
			shrapnelViews[2]->Show();

			SetNextState(2);
			SetTicks(4);
			
			break;

		case 2: // animate shrapnel

			// move each tile according to step, cracking tiles as necessary

			factor++;
			if (factor >= DBLCROSS_BOARDWIDTH) {
				SetNextState(3);
				SetTicks(4);
				break;
			}

			for (i = 0 ; 
					i < DBLCROSS_BOARDWIDTH + DBLCROSS_BOARDHEIGHT && shrapnelViews[i]->GetX() > 0 ; 
					i++) {
				// get current shrapnel location on board
				p.x = shrapnelViews[i]->GetX();
				p.y = shrapnelViews[i]->GetY();
				table->ScreenToBoardCoordinates(p, &spot);
				
				spot.x += step[mfs.who].x;
				spot.y += step[mfs.who].y;
				CrackTile(spot);
				p = table->BoardToScreenCoordinates(spot);
				
				shrapnelViews[i]->Posn(p.x, p.y);
				shrapnelViews[i]->SetCel(random(5));
				shrapnelViews[i]->Show();
			}

			// Add two new tiles and crack tiles
			spot = table->engine->MakeDCPoint(corners[mfs.who].x + step[mfs.who].x * factor, corners[mfs.who].y);
			p = table->BoardToScreenCoordinates(spot);
			CrackTile(spot);
			shrapnelViews[i]->Posn(p.x, p.y);
			shrapnelViews[i]->SetCel(random(5));
			shrapnelViews[i]->Show();

			spot = table->engine->MakeDCPoint(corners[mfs.who].x, corners[mfs.who].y + step[mfs.who].y * factor);
			p = table->BoardToScreenCoordinates(spot);
			CrackTile(spot);
			shrapnelViews[i + 1]->Posn(p.x, p.y);
			shrapnelViews[i + 1]->SetCel(random(5));
			shrapnelViews[i + 1]->Show();

			SetNextState(2);
			SetTicks(4);
			
			break;

		case 3: // cleanup
			for ( i = 0 ; i < DBLCROSS_BOARDWIDTH + DBLCROSS_BOARDHEIGHT - 1 ; i++) {
				delete shrapnelViews[i];
				shrapnelViews[i] = NULL;
			}

			SetNextState(4);
			SetTicks(0);
			
			break;

		case 4: // done
			table->SetReadyForMessages(TRUE);
			table->Display();
			((sciScript *)client)->SetScript(NULL);
			break;
	}
}

void DoubleCrossColorBombScript::CrackTile(DCPoint square)
{
	int cel;
	DoubleCrossTile *tile;
	
	// crack unstable tile under the bomb (if any)
	if (table->engine->GetSquareOwner(square) == mfs.who && !table->engine->IsSquareStable(square)) {
		tile = table->display->GetTileByBoardSpot(square);
		if (!tile) {
			assert(false);
			SetTicks(0);
			return;
		}
		cel = random(5) + 1;
		tile->Show(tile->owner, tile->tileValue, -1, false, cel, square);
		table->display->CrackTile(cel, square);
	}
}
			
////////////////////////////////////////////////////////////////////////////////////////////////

DoubleCrossPickUpBombScript::DoubleCrossPickUpBombScript(DoubleCrossClientTable *theTable, MFS_DOUBLECROSS_EVENT_MESSAGE *theMFS, char *theComment, DoubleCrossTile *theBomb)
		: DoubleCrossEventScript(theTable, theMFS, theComment) 
{ 
	bomb = theBomb;
}


void DoubleCrossPickUpBombScript::ChangeState(int newState)
{
	POINT p;
	
	sciScript::ChangeState(newState);
	
	switch (state) {
		case 0:
			SetNextState(1);
			
			table->gameState.seats[mfs.who].score = mfs.score;
			table->display->commentsDisplay->AddComment(DCMOVE_PICKUPBOMB, comment);
			memcpy(table->gameState.seats[mfs.who].bombs, mfs.rack, DBLCROSS_BOMBRACKSIZE);
			
			if (mfs.rackSpot == DBLCROSS_BOMBRACKSIZE) {
				SetTicks(0);
				break;
			}
			
			p = table->RackToScreenCoordinates(mfs.who, mfs.rackSpot, true);
			
			if (!bomb) {
				bomb = new DoubleCrossTile(table, -1);
				bomb->plane = table->plane;
				bomb->ParamInit(30034, 0, 0, 10);
				bomb->Show(mfs.who, mfs.rack[mfs.rackSpot], -1, false, false, mfs.start);
			}
			
			bomb->SetPri(600);
			bomb->moveSpeed = 4;
			bomb->SetMotion(new sciMoveTo(this, p.x, p.y));
			break;
		
		case 1:
			delete bomb;
			table->SetReadyForMessages(TRUE);
			table->Display();
			((sciScript *)client)->SetScript(NULL);
			break;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////

void DoubleCrossDirExplosionScript::ChangeState(int newState)
{
	int count, i, j, cel;
	POINT p;
	DCPoint spot, step[] = { { -1 , 0 }, { 1 , 0 }, { 0 , -1 }, { 0 , 1 } };
	DoubleCrossTile *tile;
	
	sciScript::ChangeState(newState);
	
	switch (state) {
		case 0:	// Get bomb to detonate
			// Had to set this when placing tile to prevent it from moving back to rack
			// on next Display();
			table->dontDisplay = false;
			
			SetNextState(1);
			table->display->commentsDisplay->AddComment(DCMOVE_DIREXPLOSION, comment);

			// find the bomb; it should be at the spot where it will blow up
			
			for (i = 0 ; i < NUM_TILE_VIEWS ; i++) {
				bomb = table->display->tiles[i];
				if (bomb->IsHidden())
					continue;
				if (!bomb->InRack())
					continue;
				if (bomb->tileValue > DCBT_NESW)
					continue;
				
				// only one bomb owned by a player can be on the board; the one we want
				
				p.x = bomb->GetX();
				p.y = bomb->GetY();
				if (bomb->owner == mfs.who && table->ScreenToBoardCoordinates(p, &spot))
					break;
			}
			
			if (i == NUM_TILE_VIEWS) {
				assert(false);
				SetTicks(0);
				break;
			}
			
			SetTicks(0);
			break;

		case 1: // small explosion
			SetNextState(2);
			SetTicks(7);
				
			// create an explosion object
			explosion = new sciView();
			explosion->plane = table->plane;
			explosion->ParamInit(30053, bomb->GetX() - 12, bomb->GetY() - 19, bomb->GetPri() + 1);
			explosion->Show();

			game->sound1->Play(1020);

			// crack unstable tile under the bomb (if any)
			if (table->engine->GetSquareOwner(mfs.start) != -1 && !table->engine->IsSquareStable(mfs.start)) {
				tile = table->display->GetTileByBoardSpot(mfs.start);
				if (!tile) {
					assert(false);
					SetTicks(0);
					break;
				}
				cel = random(5) + 1;
				tile->Show(tile->owner, tile->tileValue, -1, false, cel, mfs.start);
				table->display->CrackTile(cel, mfs.start);
			}
			
			break;

		case 2: // large explosion
			SetNextState(3);
			SetTicks(7);

			explosion->SetCel(1);
			bomb->SetAlpha(128);
			bomb->Show(mfs.who, bomb->tileValue, -1, false, 0, mfs.start);
			explosion->Show();

			break;

		case 3: // cleanup bomb, set up shrapnel effect
			SetNextState(4);
			SetTicks(7);

			// cleanup bomb, explosion
			bomb->SetAlpha(255);
			bomb->Hide();
			explosion->Hide();
			delete explosion;

			// initialize shrapnel info
			isVert = (mfs.tileValue == DCBT_NORTH_SOUTH || mfs.tileValue == DCBT_NESW);
			isHorz = (mfs.tileValue == DCBT_EAST_WEST || mfs.tileValue ==  DCBT_NESW);

			factor = 0;

			for (i = 0 ; i < 4 ; i++) {
				if (i < 2 && isHorz || i >= 2 && isVert) {
					doneDir[i] = 0;
					for (j = 0 ; j < 5 ; j++) {
						shrapnelViews[i][j] = new sciView;
						shrapnelViews[i][j]->plane = table->plane;
						shrapnelViews[i][j]->ParamInit(30051, 0, 0, 30);
					}
					if (mfs.start.x == 0 && i == 0)
						doneDir[i] = 2;
					if (mfs.start.x == DBLCROSS_BOARDWIDTH - 1 && i == 1)
						doneDir[i] = 2;
					if (mfs.start.y == 0 && i == 2)
						doneDir[i] = 2;
					if (mfs.start.y == DBLCROSS_BOARDHEIGHT - 1  && i == 3)
						doneDir[i] = 2;
				}
				else {
					doneDir[i] = 2;
					for (j = 0 ; j < 5 ; j++) 
						shrapnelViews[i][j] = NULL;
				}
			}
			
			break;
			
		case 4: // shrapnel effect
			if (doneDir[0] == 2 && doneDir[1] == 2 && doneDir[2] == 2 && doneDir[3] == 2) {
				// clean up; 
				for (i = 0 ; i < 4 ; i++) {
					for (j = 0 ; j < 5 ; j++) {
						if (shrapnelViews[i][j]) {
							delete shrapnelViews[i][j];
							shrapnelViews[i][j] = NULL;
						}
					}
				}

				SetNextState(5);
				SetTicks(0);
				break;
			}
			
			// Extend explosion one tile out in each direction
			factor++;

			for (i = 0 ; i < 4 ; i++) {
				// do we explode in this direction?
				if ((i < 2 && isHorz || i >= 2 && isVert) && doneDir[i] < 2) {
					spot.y = mfs.start.y + step[i].y*factor;
					spot.x = mfs.start.x + step[i].x*factor;
					
					// at the edge?
					if (spot.x < 0 || spot.x >= DBLCROSS_BOARDWIDTH || spot.y < 0
						|| spot.y >= DBLCROSS_BOARDHEIGHT) {
						doneDir[i] = 1;
					}
					else
						p = table->BoardToScreenCoordinates(spot);
								
					count = CountList(i);
						
					// if we've reached an edge, then only delete shrapnel
					if (doneDir[i] == 1) {
						if (ShiftList(i))
							doneDir[i] = 2;
						continue;
					}
					else {
						if (count < 5) {
							// Add shrapnel to list
							for (j = 0 ; j < count ; j++) {
								shrapnelViews[i][j]->SetCel(random(5));
								shrapnelViews[i][j]->Show();
							}
							shrapnelViews[i][count]->Posn(p.x, p.y);
							shrapnelViews[i][count]->SetCel(random(5));
							shrapnelViews[i][count]->Show();
						}
						else {
							// if we've reached the limit, delete the first in list
							ShiftList(i);
							shrapnelViews[i][4]->Posn(p.x, p.y);
							shrapnelViews[i][4]->SetCel(random(5));
							shrapnelViews[i][4]->Show();
						}
					}
					
					// crack the tile (if any)
					if (table->engine->GetSquareOwner(spot) != -1 && !table->engine->IsSquareStable(spot)) {
						tile = table->display->GetTileByBoardSpot(spot);
						if (!tile) {
							assert(false);
							SetTicks(0);
							break;
						}													 
						cel = random(5) + 1;
						tile->Show(tile->owner, tile->tileValue, -1, false, cel, spot);
						table->display->CrackTile(cel, spot);
					}					
				}
			}

			// draw new shrapnel
			SetNextState(4);
			SetTicks(7);
			
			break;

		case 5: // done
			table->SetReadyForMessages(TRUE);
			table->Display();
			((sciScript *)client)->SetScript(NULL);
			break;
	}
}

int DoubleCrossDirExplosionScript::CountList(int ind)
{
	int i;
	
	// return the number of active shrapnel pieces in this list

	assert(ind >= 0 && ind <= 3);

	for (i = 0 ; i < 5; i++) {
		if (shrapnelViews[ind][i]->GetX() == 0)
			break;
	}

	return i;
}

bool DoubleCrossDirExplosionScript::ShiftList(int ind)
{
	int i;
	sciView *spare;

	assert(ind >= 0 && ind <= 3);
	
	// shift everything in list down one, removing entry at spot 0
	
	spare = shrapnelViews[ind][0];

	for (i = 0 ; i < 4 ; i++) { 
		shrapnelViews[ind][i] = shrapnelViews[ind][i + 1];
		if (shrapnelViews[ind][i]->GetX() != 0) {
			shrapnelViews[ind][i]->SetCel(random(5));
			shrapnelViews[ind][i]->Show();
		}
		else
			shrapnelViews[ind][i]->Hide();
	}

	spare->Posn(0, 0);
	shrapnelViews[ind][4] = spare;
	shrapnelViews[ind][4]->Hide();

	return shrapnelViews[ind][0]->GetX() == 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////

void DoubleCrossMysteryScript::ChangeState(int newState)
{
	BYTE alpha;
	char mysteryConvert[] = { DCSS_DOUBLER, DCSS_FILLRACK, 0,  
							DCBT_RED, DCBT_YELLOW, DCBT_BLUE, DCBT_GREEN,
							DCBT_NORTH_SOUTH, DCBT_EAST_WEST, DCBT_NESW };	
	int cel;
	POINT p;
	DoubleCrossTile *tile;
	
	sciScript::ChangeState(newState);

    // BONUS: do bonusScript
	// CURBONUS: like BONUS, except figure out what bonus points should be
	// FILLRACK: do animation, then FillRackScript
	// DOUBLER: do animation, then set Comments, state up for doubler
	// DIR BOMBS: do animation, then pickup animation
	// COLORED BOMBS: do animation, then localized explosion, crack all tiles
	// BLACKHOLE: do animation.  C'est tout
	
	switch (state) {
		case 0: // init
			if (mfs.mysteryValue <= DCMYS_CURBONUS) {
				// Do the animation in the BonusScript for this one
				((sciScript *)client)->SetScript(new DoubleCrossBonusScript(table, &mfs, comment));
				break;
			}

			if (mfs.mysteryValue == DCMYS_BLACKHOLE) {
				blackHole = new sciView();
				blackHole->plane = table->plane;
				p = table->BoardToScreenCoordinates(mfs.start);

				blackHole->ParamInit(30050, p.x - 4, p.y - 12, 30);
				blackHole->SetCel(0);
				blackHole->Hide();
			}
			else {
				mysteryTile = new DoubleCrossTile(table, -1);
				mysteryTile->plane = table->plane;
				mysteryTile->ParamInit(30036, mfs.start.x, mfs.start.y, 30);
				mysteryTile->SetCel(0);
				//mysteryTile->Show(-1, mysteryConvert[mfs.mysteryValue - DCMYS_DOUBLER], -1, false, 0, mfs.start);
				mysteryTile->Hide();
				mysteryTile->SetAlpha(0);
			}
			
			PlayMysteryTune(mfs.mysteryValue);
			table->display->commentsDisplay->AddComment(DCMOVE_MYSTERY, comment, mfs.mysteryValue);
			
			SetNextState(1);
			SetTicks(0);

			break;

		case 1: // show mystery revelation animation (except for bonuses)
			if (mfs.mysteryValue == DCMYS_BLACKHOLE) {
				blackHole->Show();
				cel = blackHole->GetCel();
				cel++;
				
				tile = table->display->GetTileByBoardSpot(mfs.start);
				
				if (cel == 5) {
					// done
					tile->SetAlpha(255);
					tile->letterView->SetAlpha(255);
					tile->Hide();
					
					SetNextState(2);
					SetTicks(0);
					break;
				}				
				
				blackHole->SetCel(cel);
				blackHole->Show();

				// fade the tile the blackhole is over
				tile->SetAlpha(128);
				tile->letterView->SetAlpha(128);
				tile->Show(tile->owner, tile->tileValue, -1, tile->stable, false, tile->boardSpot);
				
				SetNextState(1);
				SetTicks(14);				
			}
			else {
				alpha = mysteryTile->GetAlpha();
			
				if (alpha >= 250) {
					SetNextState(2);
					SetTicks(25);
					break;
				}
			
				mysteryTile->Show(-1, mysteryConvert[mfs.mysteryValue - DCMYS_DOUBLER], -1, false, 0, mfs.start);
				mysteryTile->SetAlpha(alpha + 50);
				if (mysteryTile->letterView)
					mysteryTile->letterView->SetAlpha(alpha + 50);
				
				SetNextState(1);
				SetTicks(4);
			}		
			
			break;

		case 2: // cleanup; figure out what to do next
			explosion = NULL;
			switch (mfs.mysteryValue) {
				case DCMYS_DOUBLER:
					delete mysteryTile;
					table->gameState.doublerFactor = 2;
					table->display->commentsDisplay->SetDoubler(true);
					SetNextState(10);
					SetTicks(0);

					break;

				case DCMYS_FILLRACK:
					delete mysteryTile;
					((sciScript *)client)->SetScript(new DoubleCrossFillRackScript(table, &mfs, comment));
					
					break;

				case DCMYS_BLACKHOLE: // the next event will change players
					delete blackHole;
					
					table->engine->ClearSquare(mfs.start, 0);
					table->gameState.seats[mfs.who].score = mfs.score;

					SetNextState(10);
					SetTicks(0);
					break;

				case DCMYS_REDBOMB:
				case DCMYS_YELLOWBOMB:
				case DCMYS_BLUEBOMB:
				case DCMYS_GREENBOMB:
					if (table->gameState.currentPlayer == mfs.mysteryValue - DCMYS_REDBOMB) {
						SetNextState(10);
						delete mysteryTile;
						game->sound1->Play(1023);
					}
					else
						SetNextState(3);
					SetTicks(0);

					break;

				case DCMYS_NSBOMB:
				case DCMYS_EWBOMB:
				case DCMYS_NESWBOMB:
					((sciScript *)client)->SetScript(new DoubleCrossPickUpBombScript(table, &mfs, "", mysteryTile));
					break;

				default:
					assert(false);
					break;
			}
			break;

		case 3: // Colored bomb animation Init
			explosion = new sciView();
			explosion->plane = table->plane;
			explosion->ParamInit(30053, mysteryTile->GetX() - 12, mysteryTile->GetY() - 19, 40);
			
			table->gameState.seats[mfs.seat].score = mfs.score;

			SetNextState(4);
			SetTicks(0);
			break;
			
		case 4: // small color bomb explosion
			explosion->SetCel(0);
			explosion->Show();
			
			SetNextState(5);
			SetTicks(10);
			
			break;

		case 5: //large color bomb explosion
			explosion->SetCel(1);
			mysteryTile->SetAlpha(128);
			mysteryTile->Show(-1, mysteryTile->tileValue, -1, false, 0, mfs.start);
			explosion->Show();

			game->sound1->Play(1020);

			SetNextState(6);
			SetTicks(10);

			break;

		case 6: // crack tiles, clean up explosion
			if (explosion)
				delete explosion;
			delete mysteryTile;

			CrackAllTiles(mfs.mysteryValue - DCMYS_REDBOMB);
			
			SetNextState(10);
			SetTicks(14);

			break;

		case 10: // done
			table->SetReadyForMessages(TRUE);
			table->Display();
			((sciScript *)client)->SetScript(NULL);
			break;
	}
}

void DoubleCrossMysteryScript::CrackAllTiles(int owner)
{
	DCPoint p;
	int cel;
	DoubleCrossTile *tile;
	
	// crack all the unstable tiles owned by owner
	for (p.y = 0 ; p.y < DBLCROSS_BOARDHEIGHT ; p.y++) {
		for (p.x = 0 ; p.x < DBLCROSS_BOARDWIDTH ; p.x++) {
			if (table->engine->GetSquareOwner(p) == owner && !table->engine->IsSquareStable(p)) {
				tile = table->display->GetTileByBoardSpot(p);
				if (!tile) {
					assert(false);
					return;
				}
				cel = random(5) + 1;
				tile->Show(tile->owner, tile->tileValue, -1, false, cel, p);
				table->display->CrackTile(cel, p);
			}
		}
	}
}

void DoubleCrossMysteryScript::PlayMysteryTune(int mysteryValue)
{
	int sound;
	
	if (mysteryValue == DCMYS_DOUBLER)
		sound = 4018;
	else if (mysteryValue == DCMYS_FILLRACK)
		sound = 1022;
	else if (mysteryValue == DCMYS_BLACKHOLE)
		sound = 1018;
	else
		sound = 1019;

	game->sound1->Play(sound);
}

////////////////////////////////////////////////////////////////////////////////////////////////

void DoubleCrossFillRackScript::ChangeState(int newState)
{
	bool compressed = true, foundBlank = false;
	char a, b;
	int i, n, count = 0;
	
	sciScript::ChangeState(newState);
	
	switch (state) {
		case 0:
			if (mfs.eventType == DCMOVE_FILL_AND_ADVANCE || mfs.eventType == DCMOVE_MYSTERY) {
				if (mfs.eventType == DCMOVE_FILL_AND_ADVANCE && mfs.tileValue)
					game->sound1->Play(1000);
				for (i = 0 ; i < DBLCROSS_RACKSIZE ; i++)
					if (mfs.rack[i] != '-')
						count++;
								
				n = table->engine->CountLettersInRack(mfs.who);
				
				if (count == n) { // no new tiles
					SetNextState(3);
					SetTicks(0);
					break;
				}
				
				// kill the letters from the new rack that we already have in the old rack
				for (i = DBLCROSS_RACKSIZE - n ; i < DBLCROSS_RACKSIZE ; i++)
					mfs.rack[i] = '-';
			}			
			
			SetNextState((mfs.eventType == DCMOVE_DUMP_AND_ADVANCE) ? 1 : 2);
			SetTicks(45);
			break;
		
		case 1: // dump
			for (i = 0 ; i < DBLCROSS_RACKSIZE ; i++)
				if (table->gameState.seats[mfs.seat].rack[i] != '-')
					break;
			
			if (i == DBLCROSS_RACKSIZE) {
				SetNextState(2);
				SetTicks(0);
				break;
			}
			
			game->sound1->Play(4016);
			table->gameState.currentStoreSize++;
			table->gameState.seats[mfs.seat].rack[i] = '-';
			table->Display();
			SetNextState(1);
			SetTicks(15);
			break;
		
		case 2: // cycle in new tiles
			// compress existing tiles one iteration
			for (i = DBLCROSS_RACKSIZE - 1 ; i > 0  ; i--) {
				a = table->gameState.seats[mfs.seat].rack[i];
				b = table->gameState.seats[mfs.seat].rack[i - 1];
				if (a == '-' && b != '-') {
					table->gameState.seats[mfs.seat].rack[i] = b;
					table->gameState.seats[mfs.seat].rack[i - 1] = '-';
				}
			}
			
			// put last new tile in first rack spot
			for (i = DBLCROSS_RACKSIZE - 1 ; i >= 0  ; i--)
				if (mfs.rack[i] != '-') {
					table->gameState.seats[mfs.seat].rack[0] = mfs.rack[i];
					mfs.rack[i] = '-';
					table->gameState.currentStoreSize--;
					break;
				}
			
			// see if we're compressed
			for (i = DBLCROSS_RACKSIZE - 1 ; i >= 0  ; i--) {
				if (table->gameState.seats[mfs.seat].rack[i] == '-')
					foundBlank = true;
				else // found a letter
					if (foundBlank) {
						compressed = false;
						break;
					}
			}
			
			game->sound1->Play(4016);
			table->Display();
			
			if (i < 0 && compressed) {
				SetNextState(3);
				SetTicks(0);
				break;
			}
			else {
				SetNextState(2);
				SetTicks(15);
			}
			break;
		
		case 3: // done with animation
			table->SetReadyForMessages(TRUE);
			table->Display();
			((sciScript *)client)->SetScript(NULL);
			break;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////

DoubleCrossShowdownScript::DoubleCrossShowdownScript(DoubleCrossClientTable *theTable, MFS_DOUBLECROSS_REQUESTSHOWDOWN_MESSAGE *theMfs)
{
	table = theTable;
	mfs = theMfs;
}

void DoubleCrossShowdownScript::ChangeState(int newState)
{
	bool done;
	DCPoint spot;
	
	// Go through each player.  Highlight each unstable tile on the
	// board and deduct a point for each.  Then highlight each tile in
	// your hand and deduct 4 points for it
	
	sciScript::ChangeState(newState);
	
	switch (state) {
		case 0: // init
			curPlayer = -1;
			
			table->display->commentsDisplay->AddComment(-1, "Subtracting points for unstable tiles.");
			
			SetNextState(4);
			SetTicks(0);

			break;

		case 1: // tote up tiles for each player on board
			for (done = false, spot.y = curSpot.y ; spot.y < DBLCROSS_BOARDHEIGHT && !done ; spot.y++) {
				for (spot.x = curSpot.x ; spot.x < DBLCROSS_BOARDWIDTH && !done ; spot.x++) {
					if (table->engine->GetSquareOwner(spot) == curPlayer && !table->engine->IsSquareStable(spot)) {
						
						// turn off highlight on last tile (if any)
						
						if (curTile)
							table->Display();
						
						if (table->engine->gameState->seats[curPlayer].score == 0) {
							// no more to do; skip to next player
							SetNextState(4);
							SetTicks(0);
							return;
						}

						curTile = table->display->GetTileByBoardSpot(spot);
						if (!curTile) {
							assert(false);
							return;
						}
						
						// curTile->Show(curTile->owner, curTile->tileValue, -1, false, false, curTile->boardSpot, true);
						curTile->Hide();

						game->sound1->Play(1006);

						table->engine->gameState->seats[curPlayer].score--;
						spot.x++;
						curSpot = spot;
								
						done = true;								
					}
				}
				
				if (!done)
					curSpot.x = 0;
				else
					break;
			}
			
			if (spot.y == DBLCROSS_BOARDHEIGHT) {
				SetNextState(2);
				table->Display();
			}
			else
				SetNextState(1);

			SetTicks(8);
			
			break;

		case 2: // tote up tiles in player's rack
					
			// find a tile in rack to highlight

			while (curRackSpot < DBLCROSS_RACKSIZE && table->engine->gameState->seats[curPlayer].rack[curRackSpot] == '-')
				curRackSpot++;
			
			if (curRackSpot == DBLCROSS_RACKSIZE) {
				// done with this player
				SetNextState(4);
				SetTicks(8);
				break;
			}

			curTile = table->display->GetTileByRackSpot(curRackSpot, curPlayer);
			if (!curTile) {
				assert(false);
				return;
			}

			// curTile->Show(curTile->owner, curTile->tileValue, curRackSpot, false, false, curTile->boardSpot, true);
			curTile->Hide();

			game->sound1->Play(1006);
						
			if (table->engine->gameState->seats[curPlayer].score == 0) {
				// no more to do; skip to next player
				SetNextState(4);
				SetTicks(0);
				break;
			}

			table->engine->gameState->seats[curPlayer].score += DCPOINTS_EXPLODE;
			if (table->engine->gameState->seats[curPlayer].score < 0)
				table->engine->gameState->seats[curPlayer].score = 0;
			
			SetNextState(3);
			SetTicks(8);
			
			break;

		case 3: // reshow display
			table->Display();
			curRackSpot++;

			SetNextState(2);
			SetTicks(8);

			break;

		case 4: // go to next player (if any)
			curSpot.x = curSpot.y = 0;
			curTile = NULL;
			curRackSpot = 0;

			while ((table->engine->gameState->players[++curPlayer].playerID == PLAYER_ID_VOID || 
				table->engine->gameState->players[curPlayer].playerID == PLAYER_ID_EMPTY) &&
				curPlayer < MAX_GAME_PLAYERS)
				;

			if (curPlayer == MAX_GAME_PLAYERS) {
				SetNextState(5);
				SetTicks(0);
			}
			else {
				SetNextState(1);
				SetTicks(0);
			}
			
			break;

		case 5: // show showdown dialog, clean up script
			table->SetReadyForMessages(TRUE);
			table->Display();
			table->showdownDialog->Show();
			
			((sciScript *)client)->SetScript(NULL);
			break;
	}
}
