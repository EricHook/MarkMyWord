#include "os.h"
#include "sci.h"
#include "bitmap.h"
#include "inifile.h"

#include "game.h"

#include "Client Include.h"

int megaHabits;
int megaBlinks;
int noHabits;
int dontStopSlots;

Game::Game()
{
	int i;
	
	nextRoomNum = ROOM_0;
	
	for(i = 0; i < MAX_NUM_MULTIPLAYERS; i++)
		InitSettings(i);
}

Game::~Game()
{
}

void Game::Doit()
{
	sciGame::Doit();
}

//for DEMO only
void Game::DemoMessages(int msgNum)
{
}

void Game::SetNextRoom(int newRoomNum)
{
	if (newRoomNum == ROOM_END) {
		QuitGame(TRUE);
		
		return;
	}
	
	sciGame::SetNextRoom(newRoomNum);
}

void Game::LoadTheRoom(int roomNum)
{
	extern Boolean loadingRoom;

	if (pApp)
		pApp->SetViewGamma(GetGamma());

	// SetViewGamma updates the dirty rect. Reset it because we're loading a room.
	MAIN_WINDOW->UnPaintAll();

	loadingRoom = TRUE;
	HandsOff();
	
	sciGame::LoadTheRoom(roomNum);
	
	switch (roomNum) {
		case ROOM_0:
		default:
			curRoom = new Room0();
			break;
		/*
		case ROOM_END:
			curRoom = new RoomEnd();
			break;
		case ROOM_INTRO:
			curRoom = new RoomIntro();
			break;
		case ROOM_CREDITS:
			curRoom = new RoomCredits();
			break;
		*/
		}
	
	curRoom->Init();
}

////////////////////////////////////////////////////////////////////////////

void Game::GetGameName(int roomNumber, char *name)
{
	if (roomNumber == -1)
		roomNumber = GetCurrentRoomNum();
	
	strcpy(name, "???");
}

BOOL Game::InAGameRoom()
{
	if(GetCurrentRoomNum() >= ROOM_FIRSTGAME &&	GetCurrentRoomNum() < ROOM_LASTGAME)
		return TRUE;
	return FALSE;
}

BOOL Game::IsCurRoom(int aRoomNum)
{
	return GetCurrentRoomNum() == aRoomNum;
}

BOOL Game::LeftAGameRoom()
{
	if(GetPreviousRoomNum() >= ROOM_FIRSTGAME && GetPreviousRoomNum() < ROOM_LASTGAME)
		return TRUE;
	return FALSE;
}

BOOL Game::QuitGame(Boolean iReallyMeanIt)
{
	i18nStr getS, getS1;
	char windowName[128], s[128];
	
	if (iReallyMeanIt) {
		sciGame::QuitGame(TRUE);
		
		return FALSE;
	}
	
	GetWindowText(MAIN_HWND, windowName, 64);
	// "Are you sure you want to leave %s?"
	sprintf(s, GetString(357, getS), windowName);
	
	if (roomClient)
		// "Quitting"
		roomClient->ShowModelessMessage(0, s, GetString(358, getS1), MB_YESNO, DBP_EXIT);
	
	return FALSE;
}

//////////////////////////////////////////////
//PLAYER SETTINGS FUNCTIONS					//
//////////////////////////////////////////////

void Game::InitSettings(int index, GameSettings *gSettings)
{
	InitCommonSettings(index, gSettings);

	InitGameSpecificSettings(index, gSettings);
}
void Game::InitCommonSettings(int index, GameSettings *gSettings)
{
	CommonSettings *cSettings;

	if (gSettings)
		cSettings = (CommonSettings*)gSettings->commonSettings;
	else
	{
		cSettings = GetPlayerSettings(index);
	}

	assert(cSettings);

	memset((char*)cSettings, 0, 3072);
	
}
void Game::InitGameSpecificSettings(int index, GameSettings *gSettings)
{
}
void Game::ClearGameSpecificStatistics(int roomNum, char *gsSettings)
{
}

void Game::ClearSettingsForRoomChange()
{
}

CommonSettings *Game::GetPlayerSettings(int index)
{
	assert(index > -1 && index < MAX_NUM_MULTIPLAYERS);

	return (CommonSettings*)&players[index].commonSettings;
}
CommonSettings *Game::GetPlayerSettings(char *name, BOOL human)
{
	if(GetPlayerIndex(name, human) != -1)
		return GetPlayerSettings(GetPlayerIndex(name, human));

	return NULL;
}
CommonSettings *Game::GetPlayerSettings(BoardCharacter *c)
{
	if(GetPlayerIndex(c) != -1)
		return GetPlayerSettings(GetPlayerIndex(c));

	return NULL;
}
char *Game::GetPlayerGameSettings(int roomNum, int index)
{
	return NULL;
}
char *Game::GetPlayerGameSettings(int roomNum, char *name, BOOL human)
{
	if(GetPlayerIndex(name,human) != -1)
		return GetPlayerGameSettings(roomNum, GetPlayerIndex(name,human));

	return NULL;
}
char *Game::GetPlayerGameSettings(int roomNum, BoardCharacter *c)
{
	if(GetPlayerIndex(c) != -1)
		return GetPlayerGameSettings(roomNum, GetPlayerIndex(c));

	return NULL;
}
int Game::GetPlayerIndex(char *name, BOOL human)
{
	int i;

	for(i = 0; i < MAX_NUM_MULTIPLAYERS; i++)
	{
		if(human && GetPlayerSettings(i)->type != CASINO_HUMAN)
			continue;

		if(!strcmp(name, GetPlayerSettings(i)->loginName))
			return i;
	}
	return -1;
}
int Game::GetPlayerIndex(BoardCharacter *c)
{
	int i;

	for(i = 0; i < MAX_NUM_MULTIPLAYERS; i++)
	{
		if(c == GetPlayerSettings(i)->character)
			return i;
	}

	return -1;
}

BOOL Game::SaveSettings(GameSettings *gSettings, char *fileName)
{
	return FALSE;
}
BOOL Game::GetSettings(GameSettings *gSettings, char *fileName)
{
	return FALSE;
}

void Game::CopySettingsIfSamePlayer(int index1, int index2, GameSettings *tempPlayers)
{
}
BOOL Game::SavePlayerSettings(int index)
{
	return FALSE;
}
BOOL Game::SavePlayerSettings(char *name)
{
	return FALSE;
}
BOOL Game::SavePlayerSettings(BoardCharacter *c)
{
	return FALSE;
}
BOOL Game::IsSignedIn(char *name, BOOL human)
{
	return(GetPlayerIndex(name,human) != -1);
}
void Game::GetFileName(char *name, char *fileName)
{
	strcpy(fileName, "");
}

//////////////////////////////////////////////
// HOST'S PREFERENCES						//
//////////////////////////////////////////////

BOOL Game::IsAnimations()
{
	return FALSE;
	
	/*
	BOOL b;
	
	b = roomClient->globalIniFile->ReadValue("LimitAnimations", 0);
	if (b < 0 || b > 1)
		b = 0;
	
	return !b;
	*/
}

BOOL Game::IsBackgroundMusic()
{
	return FALSE;
}

BOOL Game::IsSoundEffects()
{
	BOOL b;
	
	b = roomClient->globalIniFile->ReadValue("SoundEffects", 1);
	if (b < 0 || b > 1)
		b = 1;
	
	return b;
}

BOOL Game::IsChatSoundEffects()
{
	BOOL b;
	
	b = roomClient->globalIniFile->ReadValue("ChatSounds", 1);
	if (b < 0 || b > 1)
		b = 1;
	
	return b;
}

BOOL Game::IsChatFiltered()
{
	BOOL b;
	
	b = roomClient->globalIniFile->ReadValue("OffensiveChat", 1);
	if (b < 0 || b > 1)
		b = 1;
	
	return b;
}

BOOL Game::IsTutorialMode()
{
	return FALSE;
}

BOOL Game::IsPracticeMode()
{
	return FALSE;
}

int Game::GetChatDisplay()
{
	int d;
	
	d = roomClient->globalIniFile->ReadValue("ChatDisplay", CHAT_BALLOONS_ONLY);
	if (d < 0 || d > 2)
		d = CHAT_BALLOONS_ONLY;
	
	return d;
}

int Game::GetMoveSpeed()
{
	return 0;
}

int Game::GetAttitude()
{
	return 0;
}

int Game::GetSpeed(int option)
{
	return 0;
}

double Game::GetGamma()
{
	return GetPlayerSettings()->gamma;
}

int Game::GetNumOpponents()
{
	return 0;
}

int Game::GetGender()
{
	return 32;
}

void Game::SaveBank(char *name, BOOL human, CasinoMoney &aBank, BOOL forKeno, BOOL forTourn, BOOL forLoan)
{
}
void Game::GetBank(char *name, BOOL human, CasinoMoney &aBank)
{
}
void Game::SaveStats(int index, CasinoMoney &oldBank, CasinoMoney &newBank, int gameRoom)
{
}

BOOL Game::ResetCasinoPlayer(char *name, BOOL forceReset)
{
	return TRUE;
}

int Game::GetLoanAmount(char *name)
{
	return 0;
}
void Game::SetMinTableBet(int newMin)
{
	tableMin = newMin;
}
short Game::GetMinTableBet()
{
	return (tableMin == 0) ? 100 : tableMin;
}
void Game::GetTableLimits(int *min, int *max)
{
	*min = 5;
	*max = 1000;
}

CasinoMoney Game::GetInitialBank()
{
	CasinoMoney m = { 0, 0 };
	
	return m;
}

BOOL Game::OverCreditLimit(int index)
{
	return FALSE;
}

void Game::GiveLoan(int index)
{
}

BOOL Game::RepayLoan(int index, BOOL forceRepay)
{
	return FALSE;
}

BOOL Game::AwardNextCreditCard(int index)
{
	return TRUE;
}

//////////////////////////////////////////////
//CHARACTER INFORMATION						//
//////////////////////////////////////////////

int Game::GetPlayerViewNumber(int playerNumber, int playerSize, Boolean facingWest)
{
	return 70068;
}

BOOL Game::IsMale(int gender)
{
	return FALSE;
}

BOOL Game::IsFemale(int gender)
{
	return FALSE;
}

int Game::GetPlayerViewCel(int playerNumber, int playerSize, Boolean facingWest)
{
	return 0;
}

void Game::GetCharacterNameByNumber(int characterNumber, char *name)
{
	name[0] = 0;
}

int Game::GetCharacterNumberByName(char *name)
{
	return -1;
}

BoardCharacter *Game::FindCharacter(int n)
{
	return NULL;
}

void Game::PlayerCounts(int whichGame, int *max, int *min, int *maxHumans)
{
}

int Game::GetNumSeatsPlayerIsIn(char *name)
{
	return 0;
}
int Game::GetNumDifferentHumansPlaying()
{
	return 0;
}
int Game::GetNumPlayers(BOOL humans)
{
	return 0;
}

void Game::ClearAllButHost()
{
}

BOOL Game::TooManyLogins(int newRoomNum)
{
	return FALSE;
}

BOOL Game::HumansOnlyGame(int roomNum)
{
	return FALSE;
}

void Game::CheckFillScreenMenuItem(int n)
{
}
