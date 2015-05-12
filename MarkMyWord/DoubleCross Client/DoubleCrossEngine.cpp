#include "miniwin.h"
#include "DoubleCrossMessage.h"
#include "DoubleCrossEngine.h"

#ifdef WIN32
#ifndef ASSERT
	#define ASSERT(x, y) assert(y)
#endif
#ifndef _WARN_
	#define _WARN_ 0
#endif
#endif

DoubleCrossEngine::DoubleCrossEngine()
{
}

DoubleCrossEngine::~DoubleCrossEngine()
{
}

void DoubleCrossEngine::Init(GameState *theGameState)
{
	int i;
	
	gameState = theGameState;
	
	gameState->options.haveColoredBombs = TRUE;
	gameState->options.haveDirectionalBombs = TRUE;
	gameState->options.haveBonusTiles = TRUE;
	gameState->options.haveBlockingTiles = FALSE;
	gameState->options.gridMode = FALSE;
	gameState->options.haveMysteryTiles = TRUE;
	gameState->options.allowOffensiveWords = FALSE;
	gameState->options.timeLimit = 60;
	gameState->options.minWordSize = DBLCROSS_DEFMINWORDSIZE;
	
	gameState->gameNumber = 0;
	
	ResetForNewGame();
	
	for (i = 0 ; i < MAX_GAME_PLAYERS ; i++)
		InitPlayer(i);
	
	for (i = 0 ; i < MAX_GAME_WATCHERS ; i++)
		gameState->watcherIDs[i] = -1;
}

void DoubleCrossEngine::InitPlayer(int seat)
{
	gameState->players[seat].playerID = PLAYER_ID_EMPTY;
	gameState->players[seat].bootCount = -1;
	gameState->players[seat].status = DCPS_EMPTY;
	gameState->players[seat].doIt = FALSE;
	gameState->players[seat].didIt = FALSE;
}

void DoubleCrossEngine::InitSeat(int seat)
{
	int i;
	
	gameState->seats[seat].color = (DblCrossColors)(seat + 1);
	gameState->seats[seat].score = 0;
	for (i = 0 ; i < DBLCROSS_RACKSIZE ; i++)
		gameState->seats[seat].rack[i] = '-';
	for (i = 0 ; i < DBLCROSS_BOMBRACKSIZE ; i++)
		gameState->seats[seat].bombs[i] = '-';
	gameState->seats[seat].canDump = TRUE;
}

void DoubleCrossEngine::ResetForNewGame()
{
	int i, x, y;
	
	gameState->gamePhase = DCGP_NOTHING;
	gameState->currentPlayer = -1;
	
	for (i = 0 ; i < MAX_GAME_PLAYERS ; i++)
		InitSeat(i);
	
	gameState->currentStoreSize = DBLCROSS_TILESTORESIZE;
	gameState->bonusIndex = 0;
	
	for (x = 0 ; x < DBLCROSS_BOARDWIDTH ; x++)
		for (y = 0 ; y < DBLCROSS_BOARDHEIGHT ; y++)
			gameState->board[y][x] = 0;
	
	memset(gameState->stable, 0, sizeof(gameState->stable));
	
	ResetForNewPlayer();
}

void DoubleCrossEngine::ResetForNewPlayer()
{
	int i;
	
	gameState->doublerFactor = 1;
	
	gameState->seconds = 0;
	gameState->startTicks = 0;
	
	for (i = 0 ; i < MAX_GAME_PLAYERS ; i++)
		if (gameState->currentStoreSize > 0)
			gameState->seats[i].canDump = TRUE;
		else
			gameState->seats[i].canDump = FALSE;
}

//////////////////////////////////////////////////////////////////////////////////
	
int DoubleCrossEngine::GetNthBonusValue(int n)
{
	static int values[8] = { 5, 10, 15, 25, 35, 50, 70, 95 };
	
	if (n < 0 || n > 7)
		{ ASSERT(_WARN_, FALSE); return 5; }
	
	return values[n];
}

bool DoubleCrossEngine::IsSquareEmpty(DCPoint square)
{
	return gameState->board[square.y][square.x] == 0;
}

int DoubleCrossEngine::GetSquareOwner(DCPoint square)
{
	unsigned char c;
	
	c = gameState->board[square.y][square.x];
	
	c = c >> 5;
	
	if (c == 0 || c >= 6)
		return -1;
	
	return c - 1;
}

char DoubleCrossEngine::GetSquareLetter(DCPoint square)
{
	unsigned char c;
	
	if (GetSquareOwner(square) == -1)
		{ ASSERT(_WARN_, FALSE); return 'A'; }
	
	c = gameState->board[square.y][square.x];
	
	c &= 0x1F;
	
	if (c == 0 || c > 26)
		{ ASSERT(_WARN_, FALSE); return 'A'; }
	
	return 'A' + (c - 1);
}

int DoubleCrossEngine::GetSquareSpecial(DCPoint square)
{
	unsigned char c;
	
	if (IsSquareEmpty(square) || GetSquareOwner(square) != -1)
		{ /* ASSERT(_WARN_, FALSE); */ return DCBT_EMPTY; }
	
	c = gameState->board[square.y][square.x];
	
	c &= 0x1F;
	
	if (c == 0 || c > DCSS_BLOCK)
		{ ASSERT(_WARN_, FALSE); return DCBT_EMPTY; }
	
	return c;
}
	
void DoubleCrossEngine::PlayLetterOnSquare(int seat, char letter, DCPoint square)
{
	unsigned char c, player;
	
	if (GetSquareOwner(square) != -1)
		{ ASSERT(_WARN_, FALSE); return; }
	
	c = (letter - 'A') + 1;
	
	player = seat + 1;
	player = player << 5;
	c |= player;
	
	gameState->board[square.y][square.x] = c;
	
	if (seat >= 0 && seat < MAX_GAME_PLAYERS)
		gameState->seats[seat].canDump = FALSE;
}

void DoubleCrossEngine::SetSquareSpecial(DCPoint square, char value)
{
	gameState->board[square.y][square.x] = DCBF_SPECIAL << 5;
	gameState->board[square.y][square.x] |= value;
}

void DoubleCrossEngine::StabilizeSquare(DCPoint square, short score)
{
	int owner, x, y;
	
	owner = GetSquareOwner(square);
	
	if (owner == -1)
		{ ASSERT(_WARN_, FALSE); return; }
	
	x = square.x;
	y = square.y;
	gameState->stable[(y * DBLCROSS_BOARDHEIGHT + x) / 8][0] |= (0x01 << (7 - ((y * DBLCROSS_BOARDHEIGHT + x) % 8)));
	
	if (owner != DBLCROSS_SEED_PLAYER)
		gameState->seats[owner].score += score;
}

bool DoubleCrossEngine::IsSquareStable(DCPoint square)
{
	short value;
	int owner, x, y;
	
	owner = GetSquareOwner(square);
	
	if (owner == -1)
		{ /* ASSERT(_WARN_, FALSE); */ return false; }
	
	x = square.x;
	y = square.y;
	value = (short) ( ((char)(gameState->stable[(y * DBLCROSS_BOARDHEIGHT + x) / 8][0])
					>> (7 - ((y * DBLCROSS_BOARDHEIGHT + x) % 8))) & 0x01 );
	
	return value != 0;
}

void DoubleCrossEngine::ClearSquare(DCPoint square, short score)
{
	int owner;
	
	owner = GetSquareOwner(square);
	
	if (owner == -1 || owner == DBLCROSS_SEED_PLAYER)
		{ ASSERT(_WARN_, FALSE); return; }
	
	ASSERT(_WARN_, !IsSquareStable(square));
	
	gameState->board[square.y][square.x] = 0;
	
	gameState->seats[owner].score += score;
	if (gameState->seats[owner].score < 0)
		gameState->seats[owner].score = 0;
}

int DoubleCrossEngine::CountLettersInRack(int seat)
{
	int i, n;
	
	for (i = 0, n = 0 ; i < DBLCROSS_RACKSIZE ; i++)
		if (gameState->seats[seat].rack[i] != '-')
			n++;
	
	return n;
}

bool DoubleCrossEngine::IsRackEmpty(int seat)
{
	return CountLettersInRack(seat) == 0;
}

bool DoubleCrossEngine::IsRackFull(int seat)
{
	return CountLettersInRack(seat) == DBLCROSS_RACKSIZE;
}

bool DoubleCrossEngine::SquareIsValid(DCPoint square)
{
	return square.x >= 0 && square.x < DBLCROSS_BOARDWIDTH &&
		   square.y >= 0 && square.y < DBLCROSS_BOARDHEIGHT;
}
	
bool DoubleCrossEngine::SquareIsLegal(DCPoint square)
{
	int i;
	DCPoint spot;
	static DCPoint dirSearch[] = { {-1 , 0 }, { 1, 0 }, { 0, -1 }, { 0, 1 } };
	
	// The rules for playing a tile to a square are as follows:
	
	// - There must not be a tile already in the square (special tiles
	//   such as bombs are allowed here)
	// - There must not be a block tile on the space
	// - The square must touch a tile
	// - There must be a line drawable between the square, through any 
	//	 number of tiles to a stable tile
	
	if (!IsSquareEmpty(square)) {
		if (GetSquareOwner(square) != -1)
			return false;
		if (GetSquareSpecial(square) == DCSS_BLOCK)
			return false;
	}
	
	for (i = 0 ; i < 4 ; i++)
		for (spot.x = square.x + dirSearch[i].x,
			 spot.y = square.y + dirSearch[i].y ;
			 
			 spot.x >= 0 && spot.x < DBLCROSS_BOARDWIDTH && spot.y >= 0
			 && spot.y < DBLCROSS_BOARDHEIGHT ; 
			 
			 spot.x += dirSearch[i].x, spot.y += dirSearch[i].y) {
			if (GetSquareOwner(spot) == -1)
				break; // give up in this direction
			if (IsSquareStable(spot))
				return true;
		}
	
	return false;
}

DCPoint DoubleCrossEngine::MakeDCPoint(int x, int y)
{
	DCPoint p;
	
	p.x = (char)x;
	p.y = (char)y;
	
	return p;
}
