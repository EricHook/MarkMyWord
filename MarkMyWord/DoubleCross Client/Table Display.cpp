#include "os.h"
#include "sci.h"
#include "globals.h"
#include "util.h"
#include "inifile.h"
#include "maketex.h"
#include <direct.h> //For save game functions...

#include "Client Include.h"

//for all those non-card games out there.
#if !defined(HIDDEN_CARD)
#define HIDDEN_CARD 0xFF
#endif

#define INFO_BAR_PRESET_Y 435
#define WATCHERS_DISPLAY_Y 450
#define INFO_BAR_DY 2

//////////////////////////////////////////////////////////////////////////////////////////

void GamePlane::AttemptClosePlane()
{
	char windowName[128], s[128], getS[256], getS2[256];
	
	if(owner->AlwaysOfferSaveGame() && owner->NeedSaveGame()) {
		if (owner->status == TS_DISPLAYED) {
			if(owner->NeedSaveGame())
			{
				owner->ShowModelessMessage(0, GetString(207, getS), GetString(208, getS2), MB_YESNO, DBP_PLAYER_LEFT_GAME);
//				owner->readyForMessages = FALSE;
				owner->answeredMostRecentRequest = FALSE;
			}
		}
	}
	else {
		GetWindowText(window->hWnd, windowName, 64);
		sprintf(s, GetString(102, getS), windowName);
		
		owner->ShowModelessMessage(0, s, GetString(103, getS), MB_YESNO, DBP_LEAVE_GAME);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////

RoomClientTable::RoomClientTable()
{
	plane = NULL;
	display = NULL;
}

RoomClientTable::~RoomClientTable()
{
	if (plane) { pApp->MarkPlaneForDeletion(plane); plane = NULL; }
	if (display) { delete display; display = NULL; }
}

void RoomClientTable::Init(int theIndex)
{
	short i;
	
	index = theIndex;
	status = TS_EMPTY;
	joinOption = TJO_OPEN;
	watchersOption = TWO_OPEN;
	muteWatchers = FALSE;
	ratingsFlags = 0;
	ratingsSkills = 0;
	m_iCardBacks = -1;
	m_iCardFaces = -1;
	description[0] = 0;

	SetReadyForMessages(FALSE);
	answeredMostRecentRequest = FALSE;
	
	for (i = 0 ; i < MAX_GAME_PLAYERS ; i++)
		InitPlayer(i);
	
	for (i = 0 ; i < MAX_GAME_WATCHERS ; i++)
		gameState.watcherIDs[i] = -1;

	for (i = 0; i < MAX_GAME_PLAYERS; i++)
		restartVotes[i] = FALSE;
}

void RoomClientTable::InitPlayer(int seat)
{
	gameState.players[seat].playerID = PLAYER_ID_EMPTY;
	gameState.players[seat].doIt = FALSE;
	gameState.players[seat].didIt = FALSE;

	gameState.players[seat].bootCount = -1;
}

void RoomClientTable::PlayerSits(ROOMCLIENTPLAYER *player, PlayerState *playerState)
{
	int watcherIndex, seat = player->info.seat;
	
	if (seat < MAX_GAME_PLAYERS) {
		// add a regular player
		assert(gameState.players[seat].playerID == PLAYER_ID_EMPTY || gameState.players[seat].playerID == PLAYER_ID_AI);
		gameState.players[seat] = *playerState;
		// the player's bank might have been changed when the player sat
		player->info.bankRoll = playerState->bankRoll;

		gameState.players[seat].bootCount = -1;
	}
	else {
		// add a watcher
		watcherIndex = seat - MAX_GAME_PLAYERS;
		assert(watcherIndex < MAX_GAME_WATCHERS);
		assert(gameState.watcherIDs[watcherIndex] == -1);
		gameState.watcherIDs[watcherIndex] = player->info.playerID;
	}
	
	if (status == TS_DISPLAYED)
		Display();
}

void RoomClientTable::PlayerStands(ROOMCLIENTPLAYER *player, int newID)
{
	i18nStr getS,getS2;
	int id, watcherIndex, seat = player->info.seat;
	ROOMCLIENTPLAYER *p;

	if (player->info.seat < MAX_GAME_PLAYERS) {	// remove a regular player
		if(AlwaysOfferSaveGame()) {
			//handled elsewhwere (in AttemptClosePlane())
			;
		}
		else if (NoJoinInProgress()) {
			if (status == TS_DISPLAYED) {
				id = roomClient->GetMyID();
				if (id != player->info.playerID) {
					p = roomClient->GetPlayerFromID(id);
					if (p->info.table == index && p->info.seat < MAX_GAME_PLAYERS) {
						assert(p->info.seat != player->info.seat);
						if (SaveGameIfNeeded())
							ShowModelessMessage(0, GetString(209, getS), GetString(210, getS2), MB_OK, DBP_PLAYER_LEFT_GAME);
						else
							ShowModelessMessage(0, GetString(211, getS), GetString(212, getS2), MB_OK, DBP_PLAYER_LEFT_GAME);
					}
				}
			}
		}

		assert(gameState.players[seat].playerID == player->info.playerID);
		InitPlayer(seat);
		gameState.players[seat].playerID = newID;

	}
	else {
		// remove a watcher
		watcherIndex = seat - MAX_GAME_PLAYERS;
		assert(watcherIndex < MAX_GAME_WATCHERS);
		assert(gameState.watcherIDs[watcherIndex] == player->info.playerID);
		gameState.watcherIDs[watcherIndex] = -1;
	}
	if (player->info.playerID != roomClient->GetMyID())
		if (status == TS_DISPLAYED)
			Display();
}

void RoomClientTable::Chat(ROOMCLIENTPLAYER *player, int sound, char *text)
{
	char cleanText[1024], s[1024];
	int seat;
	
	if (!ContainsPlayerID(player->info.playerID, &seat))
		{ assert(FALSE); return; }
	
	if (seat != player->info.seat)
		{ assert(FALSE); return; }
	
	strcpy(cleanText, text);
	if (game->IsChatFiltered())
		roomClient->dirtyWordFilter->UpdateString(cleanText, 1000);
	
	strcpy(s, player->info.login);
	strcat(s, ": ");
	strnzcpy(&s[strlen(s)], cleanText, 200);
	
	// if (sound == 0)
		display->gameChatLog->AddAChatLine(player->info.login, cleanText, 0, 0, 0); //color=BLACK
	
	if (game->GetChatDisplay() != CHAT_LOG_ONLY) {
		if (seat < MAX_GAME_PLAYERS) {
			if (!display->gameChatLog->UserIsEditing() || seat != roomClient->players[0].info.seat)
				display->characters[seat]->chatBalloon->Show(cleanText);
		}
		else
			display->watchersDisplay->chatDisplayBalloon->Show(s);
	}
	
	if (sound)
		if (game->IsChatSoundEffects())
			game->sound1->Play(sound);
}

void RoomClientTable::SetReadyForMessages(Boolean ready)
{
	readyForMessages = ready;

	if(gbShowReadyForMessages)
	{
		if(display && display->m_poReadyForMessageRect) {
			display->m_poReadyForMessageRect->Show();

			if(ready)
				display->m_poReadyForMessageRect->SetInnerColor(0, 255, 0);
			else
				display->m_poReadyForMessageRect->SetInnerColor(255, 0, 0);
		}
	}

}

void RoomClientTable::Clear()
{
	if (display)
		{ display->Hide(); delete display; display = NULL; }
	if (plane)
		{ pApp->MarkPlaneForDeletion(plane); plane = NULL; }
	
	if (room0->script->script)
		room0->script->SetScript(NULL);
	
	answeredMostRecentRequest = FALSE;
}

void RoomClientTable::Display()
{
	char s[128], getS[256];
	int b;
	RECT r;
	extern int backdrops[];
	
	if (!plane) {
		plane = new GAMEPLANE(FALSE, NULL, (ROOMCLIENTTABLE *)this);
		if (GetSystemMetrics(SM_CXSCREEN) > 640) {
			GetWindowRect(MAIN_HWND, &r);
			SetWindowPos(plane->GetWindow()->hWnd, HWND_NOTOPMOST, r.left + 20, r.top + 20, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
		}
		if (!roomClient->tables[index].CanChangeBackdrop())
			plane->SetPic(GAME_BACKGROUND_PIC);
		else {
			b = roomClient->globalIniFile->ReadValue("Backdrop", 0);
			if (b < 0 || b >= NUM_BACKDROPS)
				b = 0;
			if (b == 0 || !pApp->pResourceManager->ResourceIsAvailable(RES_TSPRITE, backdrops[b]))
				plane->SetPic(GAME_BACKGROUND_PIC);
			else
				plane->SetPic(backdrops[b]);
		}
		plane->Show();
		GetWindowText(MAIN_HWND, s, 64);
		sprintf(&s[strlen(s)], GetString(213, getS), index + 1);
		SetWindowText(plane->GetWindow()->hWnd, s);
	}
	if (!display) {
		display = new GAMEDISPLAY();
		display->Init(index);
	}
	if (roomClient->tables[index].muteWatchers) {
/*
		display->watchersDisplay->chatDisplayBalloon->SetAlpha(200);
		display->watchersDisplay->chatDisplayBalloon->sciProp::Show();
		display->watchersDisplay->chatDisplayBalloon->script->UnSetTicks();
		display->watchersDisplay->chatDisplayBalloon->text->SetText("Watchers Muted");
		display->watchersDisplay->chatDisplayBalloon->text->Show();
*/
		display->watchersDisplay->muteText->Show();
	}
	else
	{
		display->watchersDisplay->muteText->Hide();
//		display->watchersDisplay->Hide();
	}
}

void RoomClientTable::SetTickerText(char *s)
{
	if (status != TS_DISPLAYED)
		{ assert(FALSE); return; }
	
	if (!display || !display->tickerDisplay)
		{ assert(FALSE); return; }
	
	display->tickerDisplay->SetSegments(s);
}

Boolean RoomClientTable::AddTickerSegment(char *s)
{
	char *p, segment[1024];
	int n = 0;
	
	if (status != TS_DISPLAYED)
		return FALSE;
	
	if (!display || !display->tickerDisplay)
		return FALSE;
	
	if (s[0] != '<')
		return FALSE;
	
	p = strchr(s, '>');
	
	if (!p)
		return FALSE;
	
	p++;
	
	sscanf(&s[1], "%d", &n);
	
	if (n == 0)
		return FALSE;
	
	sprintf(segment, "%d -1 -1 \"%s\" -c 0 0 0 2", n, p);
	
	display->tickerDisplay->GetToken(NULL);
	display->tickerDisplay->AddSegment(segment);
	
	return TRUE;
}

void RoomClientTable::ShowModelessMessage(int viewNumber, char *text, char *caption, unsigned int uType, int thePurpose)
{
	if (display && display->gameMessageBox)
		display->gameMessageBox->Show(viewNumber, text, caption, uType, thePurpose);
}

int RoomClientTable::GetMyPlayingIndex(Boolean mustBeRealPlayer)
{
	int i, id;
	
	if (status != TS_PLAYING && status != TS_DISPLAYED)
		{ assert(FALSE); return -1; }
	
	id = roomClient->GetMyID();
	
	if (id == -1)
		{ assert(FALSE); return -1; }
	
	for (i = 0 ; i < MAX_GAME_PLAYERS ; i++)
		if (gameState.players[i].playerID == id)
			return i;
	
	if (mustBeRealPlayer)
		{ assert(FALSE); return -1; }
	
	for (i = 0 ; i < MAX_GAME_WATCHERS ; i++)
		if (gameState.watcherIDs[i] == id)
			return MAX_GAME_PLAYERS + i;
	
	assert(FALSE);
	
	return -1;
}

Boolean RoomClientTable::ContainsPlayerID(int id, int *seat)
{
	int i;
	
	if (seat)
		(*seat) = -1;
	
	for (i = 0 ; i < MAX_GAME_PLAYERS ; i++)
		if (gameState.players[i].playerID == id)
			{ if (seat) (*seat) = i; return TRUE; }
	
	for (i = 0 ; i < MAX_GAME_WATCHERS ; i++)
		if (gameState.watcherIDs[i] == id)
			{ if (seat) (*seat) = (MAX_GAME_PLAYERS + i); return TRUE; }
	
	return FALSE;
}

int RoomClientTable::GetNumPlayers()
{
	int i, n = 0;
	
	for (i = 0 ; i < MAX_GAME_PLAYERS ; i++)
		if (gameState.players[i].playerID != PLAYER_ID_EMPTY && gameState.players[i].playerID != PLAYER_ID_AI && gameState.players[i].playerID != PLAYER_ID_VOID)
			n++;
	
	return n;
}

int RoomClientTable::GetNumWatchers()
{
	int i, n = 0;
	
	for (i = 0 ; i < MAX_GAME_WATCHERS ; i++)
		if (gameState.watcherIDs[i] != -1)
			n++;
	
	return n;
}

void RoomClientTable::SaveGame(char *theFileName /*= NULL*/)
{
	int err;
	char *c, seat, fileName[MAX_PATH];
	FILE *file = NULL;

	if(theFileName)
		strcpy(fileName, theFileName);
	else
		GetSavedGameFileName(fileName);

	c = strchr(fileName, '\\');

	while (c != NULL) {
		c = strchr(c + 1, '\\');

		if (c != NULL) {
			*c = '\0';
			err = _mkdir(fileName);
			*c = '\\';
		}
	}

	if (fileName[0] != '\0')
		file = fopen(fileName, "wb");

	if (!file) {
		assert(FALSE);
		return;
	}

	seat = GetMyPlayingIndex(TRUE);

	fwrite(&seat, 1, 1, file);
	fwrite(&gameState, 1, sizeof(gameState), file);

	fclose(file);
}

void RoomClientTable::GetSavedGameFileName(char *fileName, char *theOppName /*= NULL*/)
{
	int p = GetMyPlayingIndex(FALSE);
	char *q, directory[MAX_PATH], myName[MAX_PLAYER_NAME + 20], oppName[MAX_PLAYER_NAME + 20];
	ROOMCLIENTPLAYER *me, *opp;
	
	fileName[0] = 0;
	
	if (p >= MAX_GAME_PLAYERS)
		{ assert(FALSE); return; }
	
	// note that we are assuming this is a two-player game...
	
	me = roomClient->GetPlayerFromID(gameState.players[p].playerID);
	opp = roomClient->GetPlayerFromID(gameState.players[(p == 0) ? 1 : 0].playerID);
	
	if (me == NULL || (opp == NULL && theOppName == NULL))
		{ assert(FALSE);  return; }
	
	LoginToFileName(me->info.login, myName);
	if(theOppName)
		strcpy(oppName, theOppName);
	else
		LoginToFileName(opp->info.login, oppName);
	
	strcpy(directory, wonParams.exePath);
	q = strrchr(directory, '\\');
	if (q)
		q[0] = 0;
	
	sprintf(fileName, "%s\\saved games\\%s\\%s.sav", directory, myName, oppName);
}

void RoomClientTable::LoginToFileName(char *name, char *convertedName)
{
	char c, illegals[] = "\\/:*?\"<>|";
	int i, j, n;
	
	// converts a players name (which may contain illegal characters for a
	// filename or path) into a "legal" version, which, hopefully, will be unique...
	
	for (i = 0, j = 0, n = 0 ; name[i] != '\0' && i < MAX_PLAYER_NAME ; i++) {
		c = name[i];
		if (strchr(illegals, c))
			n += (int)c;
		else
			convertedName[j++] = c;
	}
	
	convertedName[j] = '\0';
	
	if (n > 0)
		sprintf(&convertedName[j], "%d", n);
}

////////////////////////////////////////////////////////////////////////////////////////////////

TickerDisplay::TickerDisplay(GameDisplay *theGameDisplay)
{
	gameDisplay = theGameDisplay;
}

TickerDisplay::~TickerDisplay()
{
}

void TickerDisplay::Doit()
{
	if (!gameDisplay || !gameDisplay->watchersDisplay || !gameDisplay->watchersDisplay->chatDisplayBalloon)
		return;
	
	sciProp::Doit();
	
	if (gameDisplay->watchersDisplay->chatDisplayBalloon->IsNotHidden() || GetNumTexts() == 0)
		{ if (IsNotHidden()) Hide(); }
	else
		{ if (IsHidden()) Show(); }
}

////////////////////////////////////////////////////////////////////////////////////////////////

GameDisplay::GameDisplay()
{
	int i;
	
	for (i = 0 ; i < MAX_GAME_PLAYERS ; i++)
		characters[i] = NULL;
	for (i = 0 ; i < MAX_GAME_WATCHERS ; i++)
		watchers[i] = NULL;
	watchersDisplay = NULL;
	gameChatLog = NULL;
	optionsButton = NULL;
	optionsDialog = NULL;
	backdropDialog = NULL;
	cardBacksDialog = NULL;
	chatSoundDialog = NULL;

	profileDialog = NULL;
	complaintDialog = NULL;
	gameMessageBox = NULL;
	popup = NULL;
	tickerDisplay = NULL;
	exitButton = NULL;
	minimizeButton = NULL;
	sponsorshipButton = NULL;
	showLobbyButton = NULL;
	showChatButton = NULL;
	teamPlaque = NULL;
	handNumberBacking = NULL;
	handNumberText = NULL;

	m_poReadyForMessageRect = NULL;
}

GameDisplay::~GameDisplay()
{
	int i;
	
	for (i = 0 ; i < MAX_GAME_PLAYERS; i++)
		if (characters[i]) { delete characters[i]; characters[i] = NULL; }
	for (i = 0 ; i < MAX_GAME_WATCHERS ; i++)
		if (watchers[i]) { delete watchers[i]; watchers[i] = NULL; }
	if (watchersDisplay) { delete watchersDisplay; watchersDisplay = NULL; }
	if (gameChatLog)
		{ delete gameChatLog; gameChatLog = NULL; }
	if (optionsButton) { delete optionsButton; optionsButton = NULL; }
	
	if (optionsDialog) { delete optionsDialog; optionsDialog = NULL; }
	if (backdropDialog) { delete backdropDialog; backdropDialog = NULL; }
	if (cardBacksDialog) { delete cardBacksDialog; cardBacksDialog = NULL; }
	if (chatSoundDialog) { delete chatSoundDialog; chatSoundDialog = NULL; }
	if (profileDialog) { delete profileDialog; profileDialog = NULL; }
	if (gameMessageBox) { delete gameMessageBox; gameMessageBox = NULL; }
	
	if (tickerDisplay) { delete tickerDisplay; tickerDisplay = NULL; }
	if (popup) { delete popup; popup = NULL; }
	if (exitButton) { delete exitButton; exitButton = NULL; }
	if (minimizeButton) { delete minimizeButton; minimizeButton = NULL; }
	if (sponsorshipButton) { delete sponsorshipButton; sponsorshipButton = NULL; }
	if (showLobbyButton) { delete showLobbyButton; showLobbyButton = NULL; }
	if (showChatButton) { delete showChatButton; showChatButton = NULL; }
	if (teamPlaque) { delete teamPlaque; teamPlaque = NULL; }
	if (handNumberBacking) { delete handNumberBacking; handNumberBacking = NULL; }
	if (handNumberText) { delete handNumberText; handNumberText = NULL; }

	if (m_poReadyForMessageRect) { delete m_poReadyForMessageRect; m_poReadyForMessageRect = NULL; }
}

void GameDisplay::Init(int newTable)
{
	int i;
	sciPlane *plane;
	
	table = newTable;
	
	plane = roomClient->tables[table].plane;
	
	for (i = 0 ; i < MAX_GAME_PLAYERS ; i++) {
		characters[i] = new GAMECHARACTER((GAMEDISPLAY *)this);
		characters[i]->plane = plane;
		characters[i]->Init(newTable, i);
		characters[i]->SetView(200039);
		characters[i]->SetPri(500);
	}
	
	for (i = 0 ; i < MAX_GAME_WATCHERS ; i++) {
		watchers[i] = new GAMEWATCHER((GAMEDISPLAY *)this);
		watchers[i]->plane = plane;
		watchers[i]->Init(newTable, i);
		watchers[i]->SetPri(500);
	}
	
	watchersDisplay = new GameWatchersDisplay(newTable, (GAMEDISPLAY *)this);
	watchersDisplay->plane = plane;
	watchersDisplay->Init(plane);
	watchersDisplay->SetView(200003);
	watchersDisplay->SetPri(600);
	watchersDisplay->Posn(14, WATCHERS_DISPLAY_Y);
	watchersDisplay->Hide();
	
	gameChatLog = new ChatLog(plane, table, roomClient->tables[table].GetMyPlayingIndex(FALSE));
	gameChatLog->plane = plane;
	gameChatLog->SetHotkey(13);
	gameChatLog->SetPri(998);
	gameChatLog->Init();
	gameChatLog->Show();
	
	optionsButton = new GameOptionsButton((GAMEDISPLAY *)this);
	optionsButton->plane = plane;
	optionsButton->Init();
	optionsButton->SetView(200001);
	optionsButton->SetPri(700);
	optionsButton->Posn(0, 0);
	optionsButton->Show();
	
	gameMessageBox = new GameDialogBox(plane, &roomClient->tables[table]);
	
	popup = new GamePopup(plane, roomClient->tables[table].display);
	popup->Init(POPDIR_GAME);
	
	tickerDisplay = new TickerDisplay(this);
	tickerDisplay->plane = plane;
	tickerDisplay->Init(5, 2, 2);
	tickerDisplay->SetView(200002);
	// tickerDisplay->SetAlpha(200);
	tickerDisplay->SetPri(501);
	tickerDisplay->Posn(0, WATCHERS_DISPLAY_Y + INFO_BAR_DY - INFO_BAR_PRESET_Y);
	// tickerDisplay->SetSize(434 - 4, 24);
	tickerDisplay->SetSegments("2");
	tickerDisplay->Hide();
	
	exitButton = new GameExitButton((GAMEDISPLAY *)this);
	exitButton->plane = plane;
	exitButton->Init();
	exitButton->SetView(200139);
	exitButton->Posn(13, 0);
	exitButton->SetPri(700);
	exitButton->Show();
	
	minimizeButton = new GameMinimizeButton((GAMEDISPLAY *)this);
	minimizeButton->plane = plane;
	minimizeButton->Init();
	minimizeButton->SetView(200139);
	minimizeButton->SetButtonCels(4, 5, 6, 7);
	minimizeButton->Posn(13, 0);
	minimizeButton->SetPri(700);
	minimizeButton->Show();
	
	sponsorshipButton = new GameSponsorshipButton();
	sponsorshipButton->plane = plane;
	sponsorshipButton->Init();
	sponsorshipButton->SetPri(10);
	if (pApp->pResourceManager->ResourceIsAvailable(RES_TSPRITE, 290002)) {
		sponsorshipButton->SetView(290002);
		sponsorshipButton->SetButtonCels(0, 0, 0, 0);
		sponsorshipButton->Show();
	}
	else
		sponsorshipButton->Hide();
	
	showLobbyButton = new ShowLobbyButton();
	showLobbyButton->plane = plane;
	showLobbyButton->Init();
	showLobbyButton->SetView(200046);
	showLobbyButton->SetPri(501);
	showLobbyButton->Show();
	
	showChatButton = new ShowChatButton();
	showChatButton->plane = plane;
	showChatButton->Init();
	showChatButton->SetView(200051);
	showChatButton->TextPosn(0, 426);
	showChatButton->SetPri(501);
	showChatButton->Show();
	showChatButton->parent = this;
	showChatButton->Posn(0, 0);
	
	if (roomClient->tables[table].NeedsTeamPlaque()) {
		teamPlaque = new sciView();
		teamPlaque->plane = plane;
		teamPlaque->ParamInit(25205, 3, 3, 0);
		if (plane->GetPicNumber() != GAME_BACKGROUND_PIC)
			teamPlaque->Show();
	}
	
	handNumberBacking = new sciView();
	handNumberBacking->plane = plane;
	handNumberBacking->ParamInit(200013, 0, 0, 600);
	handNumberBacking->SetCel(0);
	handNumberBacking->Hide();
	
	handNumberText = new sciText();
	handNumberText->plane = plane;
	handNumberText->ParamInit(4, "", 200, 12, 0, 0, 601, JUST_LEFT | JUST_VERTICAL);
	handNumberText->Hide();

	m_poReadyForMessageRect = new sciRect();
	m_poReadyForMessageRect->plane = plane;
	m_poReadyForMessageRect->Posn(632, 426);
	m_poReadyForMessageRect->SetSize(5, 48);
	m_poReadyForMessageRect->SetInnerColor(0, 0, 255);
	m_poReadyForMessageRect->SetPri(500);
	m_poReadyForMessageRect->Init();
	m_poReadyForMessageRect->Hide();
	
	PreferencesChanged();
}

void GameDisplay::PreferencesChanged()
{
	int i, b, x, y, n;
	
	x = pApp->pIniFile->ReadValue("ChatX", 100);
	if (x < 0 || x > 640) x = 100;
	y = pApp->pIniFile->ReadValue("ChatY", 400);
	if (y < 0 || y > 640) y = 400;
	
	gameChatLog->PosnChat(x, y);
	
	n = pApp->pIniFile->ReadValue("ChatNumLines", 1);
	if (n < 0 || n > MAX_TRANSPARENT_CHAT_LINES) n = 10;  //if not transparent, stick with 10 lines
	gameChatLog->SetNumLines(n);
	
	b = roomClient->globalIniFile->ReadValue("ChatDisplay", CHAT_BALLOONS_ONLY);
	if (b < 0 || b > 2) b = 0;
	if (b == CHAT_BALLOONS_ONLY) {
		gameChatLog->Hide();
		if (gameChatLog->chatPlane)
			gameChatLog->chatPlane->Hide();
	}
	else
		gameChatLog->Show();
	
	if (b == CHAT_LOG_ONLY) {
		for (i = 0; i < MAX_GAME_PLAYERS; ++i)
			if (characters[i]->chatBalloon)
				characters[i]->chatBalloon->Hide();
		watchersDisplay->chatEditBalloon->Hide();
		watchersDisplay->chatDisplayBalloon->Hide();
	}

	if(gameChatLog->IsChatLogHidden())
	{
		showChatButton->SetButtonCels(0, 1, 2, 3);
	}
	else
	{
		showChatButton->SetButtonCels(4, 5, 6, 7);
	}
}

Boolean GameDisplay::DisplayPlayer(GameState *gameState, int seat)
{
	Boolean itIsMe = FALSE;
	char s[MAX_PLAYER_NAME], getS[256];
	ROOMCLIENTPLAYER *p;
	GAMECHARACTER *c = characters[seat];
	
	// assume seat is filled with AI or real player
	
	if (gameState->players[seat].playerID == PLAYER_ID_EMPTY || gameState->players[seat].playerID == PLAYER_ID_VOID)
		{ assert(FALSE); return FALSE; }
	
	if (gameState->players[seat].playerID == PLAYER_ID_AI) {
		strcpy(s, GetString(214, getS));
		c->SetView(200009); // Hoyle 'H'
		c->SetCel(0);
		c->SetAlpha(255);
	}
	else {
		p = roomClient->GetPlayerFromID(gameState->players[seat].playerID);
		
		if (p == NULL)
			{ assert(FALSE); strcpy(s, GetString(215, getS)); }
		else {
			strcpy(s, p->info.login);
			
			c->faceMakerView->faceInfo = p->info.faceInfo;
			
			if (pApp->pResourceManager->ResourceIsAvailable(RES_TSPRITE, roomClient->GetPlayerView(p->info.shadow))) {
				c->SetView(roomClient->GetPlayerView(p->info.shadow)); // use full art, if it's available
				c->SetCel(roomClient->GetPlayerViewCel(p->info.shadow, c->style == GCD_FACING_LEFT));
			}
			else {
				c->SetView(200039); // revert to guaranteed art
				c->SetCel((c->style == GCD_FACING_LEFT) ? 1 : 0);
			}
			
			if (c->faceMakerView->faceInfo.featureViews[0] != 0 && pApp->pResourceManager->ResourceIsAvailable(RES_TSPRITE, 1001001)) {
				c->faceMakerView->Show();
				c->SetAlpha(0);

				// Need to do this so we have a "clicking region"  Otherwise, whatever non-fm player is selected will
				// be used.  If it is a shadow, then in games like BJ clicks aren't picked up.  If it is small
				// (puppy dog), then only that area is the "clicking region"
				c->SetView(200039); // baghead
				c->SetCel((c->style == GCD_FACING_LEFT) ? 1 : 0);
			}
			else {
				c->SetAlpha(255);
				c->faceMakerView->Hide();
			}

			itIsMe = (p->info.playerID == roomClient->GetMyID());
		}
	}
	
	c->nameText->SetTextToWidth(s);
	
	return itIsMe;
}

void GameDisplay::ShowWatchersDisplay(GameState *gameState)
{
	int i, n;
	
	for (i = 0, n = 0 ; i < MAX_GAME_WATCHERS ; i++)
		if (gameState->watcherIDs[i] != -1)
			n++;
	
	if (n == 0)
		{ watchersDisplay->Hide(); return; }
	
	watchersDisplay->Show(n);
}

void GameDisplay::ShowHandNumber(int n, int x, int y)
{
	char s[256], getS[256];
	
	handNumberBacking->Posn(x, y);
	handNumberBacking->Show();
	
	sprintf(s, GetString(295, getS), n); // "Hand: "
	handNumberText->Posn(x + 8, y + 6);
	handNumberText->SetText(s);
	handNumberText->Show();
}

void GameDisplay::HideHandNumber()
{
	handNumberBacking->Hide();
	handNumberText->Hide();
}

void GameDisplay::AddPlayerPopupItems(int playerID)
{
	Boolean includeRestart = FALSE;
	i18nStr getS, getS2;
	int playerIndex, playerSeat, bootItems = 3, ratings = 0;
	ROOMCLIENTPLAYER *p;
	
	if (playerID == roomClient->GetMyID()) {
		// self
		playerSeat = roomClient->players[0].info.seat;
		if (roomClient->IsRestartAllowed() && playerSeat < MAX_GAME_PLAYERS)
			includeRestart = TRUE;

		if(roomClient->HasRatings())
			ratings = 3;

		popup->SetNumItems((includeRestart ? 11 : 9) + ratings);
		popup->playerID = playerID;
		
		popup->AddItem(0, POPITEM_DISPLAY_BOLD, roomClient->players[0].info.login);
		popup->AddItem(0, POPITEM_DELIMITER, "");
		popup->AddItem(POPID_GAME_EDIT_PROFILE, POPITEM_SELECTABLE, GetString(144, getS));
		popup->AddItem(POPID_GAME_PERSONAL_OPTIONS, POPITEM_SELECTABLE, GetString(145, getS));
		popup->AddItem(POPID_GAME_TABLE_OPTIONS, POPITEM_SELECTABLE, GetString(143, getS));
		
		if (ratings)
			popup->AddItem(POPID_GAME_RATING_OPTIONS, POPITEM_SELECTABLE, GetString(360, getS));

		popup->AddItem(0, POPITEM_DELIMITER, "");
		popup->AddItem(POPID_GAME_START_CHAT, POPITEM_SELECTABLE, GetString(216, getS));
		popup->AddItem(POPID_GAME_COPY_CHAT_LOG, POPITEM_SELECTABLE, GetString(217, getS));
		popup->AddItem(POPID_GAME_SHOW_CHATSOUNDS, POPITEM_SELECTABLE, GetString(337, getS));
		if (includeRestart) {
			popup->AddItem(0, POPITEM_DELIMITER, "");
			if (roomClient->tables[table].restartVotes[playerSeat])
				popup->AddItem(POPID_GAME_RESTART_GAME, POPITEM_SELECTABLE, GetString(218, getS));
			else
				popup->AddItem(POPID_GAME_RESTART_GAME, POPITEM_SELECTABLE, GetString(219, getS));
		}

		if (ratings)
		{
			popup->AddItem(0, POPITEM_DELIMITER, "");
			if(roomClient->players[0].info.wins + roomClient->players[0].info.losses +
				roomClient->players[0].info.draws + roomClient->players[0].info.quits)
			{
				sprintf(getS2, "%s %d", GetString(338, getS), roomClient->players[0].info.rating);
				popup->AddItem(POPID_GAME_RATINGS, POPITEM_SELECTABLE, getS2);
			}
			else
			{
				sprintf(getS2, "%s", GetString(372, getS));
				popup->AddItem(POPID_GAME_RATINGS, POPITEM_DISPLAY_ONLY, getS2);
			}
		}

	}
	else {
		// other
		if (roomClient->players[0].info.seat >= MAX_GAME_PLAYERS)
			bootItems = 0; // watchers can't boot or complain
		
		playerIndex = roomClient->GetPlayerIndexFromID(playerID);
		if (playerIndex == -1)
			{ assert(FALSE); return; }
		
		p = &roomClient->players[playerIndex];
		
		if (p->IsMonitorOrSuperUser())
			bootItems = 0; // nobody can boot or complain about moderators
		
		if(roomClient->HasRatings())
			ratings = 2;

		popup->SetNumItems(4 + bootItems + ratings);
		popup->playerID = playerID;
		
		popup->AddItem(0, POPITEM_DISPLAY_BOLD, p->info.login);
		popup->AddItem(0, POPITEM_DELIMITER, "");
		if (p->IsFriend())
			popup->AddItem(POPID_GAME_FRIEND, POPITEM_SELECTABLE, GetString(152, getS));
		else
			popup->AddItem(POPID_GAME_FRIEND, POPITEM_SELECTABLE, GetString(153, getS));
		popup->AddItem(POPID_GAME_VIEW_PROFILE, POPITEM_SELECTABLE, GetString(154, getS));
		if (bootItems) {
			popup->AddItem(0, POPITEM_DELIMITER, "");
			popup->AddItem(POPID_GAME_COMPLAINT, POPITEM_SELECTABLE, GetString(161, getS));
			popup->AddItem(POPID_GAME_BOOT, POPITEM_SELECTABLE, GetString(165, getS));
		}

		if (ratings)
		{
			popup->AddItem(0, POPITEM_DELIMITER, "");
			if(p->info.wins + p->info.losses + p->info.draws + p->info.quits)
			{
				sprintf(getS2, "%s %d", GetString(338, getS), p->info.rating);
				popup->AddItem(POPID_GAME_RATINGS, POPITEM_SELECTABLE, getS2);
			}
			else
			{
				sprintf(getS2, "%s", GetString(372, getS));
				popup->AddItem(POPID_GAME_RATINGS, POPITEM_DISPLAY_ONLY, getS2);
			}
		}

	}
}

void GameDisplay::Hide()
{
	int i;
	
	for (i = 0 ; i < MAX_GAME_PLAYERS ; i++)
		characters[i]->Hide();
	for (i = 0 ; i < MAX_GAME_WATCHERS ; i++)
		watchers[i]->Hide();
	watchersDisplay->Hide();
	gameChatLog->Hide();
	tickerDisplay->Hide();

	m_poReadyForMessageRect->Hide();
}

////////////////////////////////////////////////////////////////////////////////////////////////

Boolean GamePlaque::HandleEvent(sciEvent &event)
{
	if (roomClient->status < RCS_IN_ROOM)
		return FALSE;
	
	if (event.claimed)
		return FALSE;
	
	if (display->popup->HandleEvent(event))
		return event.claimed;
	
	if (event.type != MOUSE_DOWN || !(event.modifiers & LEFT_MOUSE))
		return FALSE;
	
	if (!sciFeature::OnMe(event))
		return FALSE;
	
	roomClient->GamePlaqueClicked(event, this, display, seat);
	
	event.claimed = TRUE;
	
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////

void GameBootDisplay::Init()
{
	sciView::Init();
	
	text = new sciText();
	text->plane = plane;
	text->ParamInit(2, "", 18, 18, 0, 0, 0, JUST_VERTICAL | JUST_HORIZONTAL);
}

void GameBootDisplay::SetPri(int newPri)
{
	sciView::SetPri(newPri);
	
	if (text)
		text->SetPri(newPri + 1);
}

void GameBootDisplay::Posn(int newX, int newY)
{
	sciView::Posn(newX, newY);
	
	if (text)
		text->Posn(newX + 20, newY + 5);
}

void GameBootDisplay::Show(int count)
{
	char s[20];
	
	if (count == -1) {
		sciView::Hide();
		
		text->Hide();
	}
	else {
		sciView::Show();
		
		sprintf(s, "%d", count);
		text->SetText(s);
		text->Show();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////

GameCharacter::GameCharacter(GAMEDISPLAY *theDisplay)
{
	faceMakerView = NULL;
	plaque = NULL;
	plaqueHighlight = NULL;
	nameText = NULL;
	scoreText = NULL;
	chatBalloon = NULL;
	bootDisplay = NULL;
	resetDisplay = NULL;
	display = theDisplay;
}

GameCharacter::~GameCharacter()
{
	if (faceMakerView) { delete faceMakerView; faceMakerView = NULL; }
	if (plaque) { delete plaque; plaque = NULL; }
	if (plaqueHighlight) { delete plaqueHighlight; plaqueHighlight = NULL; }
	if (nameText) { delete nameText; nameText = NULL; }
	if (scoreText) { delete scoreText; scoreText = NULL; }
	if (chatBalloon) { delete chatBalloon; chatBalloon = NULL; }
	if (bootDisplay) { delete bootDisplay; bootDisplay = NULL; }
	if (resetDisplay) { delete resetDisplay; resetDisplay = NULL; }
}

void GameCharacter::Init(int newTable, int newSeat, int newX, int newY, int newStyle, int balloonOrientation)
{
	i18nStr getS;
	
	sciPlane *tablePlane;
	
	assert(newSeat >= 0 && newSeat < MAX_GAME_PLAYERS);
	
	sciActor::Init();
	
	table = newTable;
	seat = newSeat;
	
	tablePlane = roomClient->tables[table].plane;
	
	Posn(newX, newY);
	style = newStyle;
	
	faceMakerView = new FaceMakerFace();
	faceMakerView->Init(tablePlane, this);
	faceMakerView->SetPri(500);
	faceMakerView->Posn(newX, newY);
	
	plaque = new GamePlaque(display, seat);
	plaque->plane = tablePlane;
	plaque->ParamInit(1046, GetX() - 11, GetY() + 80, 501);
	
	plaqueHighlight = new sciProp();
	plaqueHighlight->plane = tablePlane;
	plaqueHighlight->Init();
	plaqueHighlight->SetView(1047);
	plaqueHighlight->Posn(plaque->GetX() - 35, plaque->GetY() - 55);
	plaqueHighlight->SetPri(1000);
	
	nameText = new sciText();
	nameText->plane = tablePlane;
	nameText->ParamInit(4, "", plaque->GetWidth() - 30, 20, plaque->GetX() + 20, plaque->GetY() + 12, 502, JUST_VERTICAL | JUST_HORIZONTAL);
	
	scoreText = new sciText();
	scoreText->plane = tablePlane;
	scoreText->ParamInit(4, "", plaque->GetWidth() - 30, 20, plaque->GetX() + 20, plaque->GetY() + 29, 502, JUST_VERTICAL | JUST_HORIZONTAL);
	
	chatBalloon = new ChatBalloon(0, 0, balloonOrientation, tablePlane);
	chatBalloon->plane = tablePlane;
	chatBalloon->Init();
	chatBalloon->SetView(25159);
	chatBalloon->SetPri(600);
	
	bootDisplay = new GameBootDisplay(display, seat);
	bootDisplay->plane = tablePlane;
	bootDisplay->Init();
	bootDisplay->SetView(200038);
	bootDisplay->Posn(GetX(), GetY() + 40);
	bootDisplay->SetPri(701);
	
	resetDisplay = new sciText();
	resetDisplay->plane = tablePlane;
	resetDisplay->ParamInit(2, GetString(220, getS), 50, 30, GetX() + 50, GetY() + 50, 701, JUST_VERTICAL | JUST_HORIZONTAL);
	resetDisplay->SetBackgroundColor(128, 0, 0);
	resetDisplay->SetColorMap(0, 0, 0, 220, 220, 220);
}

void GameCharacter::Show(Boolean showAsCurrentPlayer)
{
	sciActor::Show();
	
	plaque->Show();
	nameText->Show();
	scoreText->Show();
	
	bootDisplay->Show(roomClient->tables[table].gameState.players[seat].bootCount);
	if (roomClient->tables[table].restartVotes[seat])
		resetDisplay->Show();
	else
		resetDisplay->Hide();
	
	if (showAsCurrentPlayer) {
		plaqueHighlight->Show();
		if (!plaqueHighlight->cycler) {
			plaqueHighlight->SetCycle(new sciForward());
			plaqueHighlight->cycleSpeed = (game->IsAnimations() ? 6 : 30);
		}
	}
	else {
		plaqueHighlight->SetCycle(NULL);
		plaqueHighlight->Hide();
	}
}

void GameCharacter::Hide()
{
	sciActor::Hide();
	
	faceMakerView->Hide();
	plaque->Hide();
	plaqueHighlight->Hide();
	nameText->Hide();
	scoreText->Hide();
	chatBalloon->Hide();
	bootDisplay->Show(-1);
}

Boolean GameCharacter::HandleEvent(sciEvent &event)
{
	int x;
	
	if (roomClient->status < RCS_IN_ROOM)
		return FALSE;
	
	if (event.claimed)
		return FALSE;
	
	if (display->popup->HandleEvent(event))
		return event.claimed;
	
	if (event.type != MOUSE_DOWN || !(event.modifiers & LEFT_MOUSE))
		return FALSE;
	
	if (!sciFeature::OnMe(event))
		return FALSE;
	
	if (roomClient->tables[display->table].gameState.players[seat].playerID == PLAYER_ID_AI)
		return FALSE;
	
	if (roomClient->tables[display->table].gameState.players[seat].playerID == PLAYER_ID_VOID)
		return FALSE;

	display->AddPlayerPopupItems(roomClient->tables[table].gameState.players[seat].playerID);
	
	if (style == GCD_FACING_LEFT)
		x = GetX() + 20 - display->popup->GetWidth();
	else
		x = GetX() + GetWidth();
	display->popup->Posn(x, GetY());
	
	display->popup->Show(this);
	
	event.claimed = TRUE;
	
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////

GameWatchersDisplay::GameWatchersDisplay(int newTable, GAMEDISPLAY *theDisplay)
	: sciActor()
{
	table = newTable;
	
	text = NULL;
	muteText = NULL;
	chatEditBalloon = NULL;
	chatDisplayBalloon = NULL;
	display = theDisplay;
}

GameWatchersDisplay::~GameWatchersDisplay()
{
	if (text) { delete text; text = NULL; }
	if (muteText) { delete muteText; muteText = NULL; }
	if (chatEditBalloon) { delete chatEditBalloon; chatEditBalloon = NULL; }
	if (chatDisplayBalloon) { delete chatDisplayBalloon; chatDisplayBalloon = NULL; }
}

void GameWatchersDisplay::Init(sciPlane *plane)
{
	i18nStr getS;
	
	sciActor::Init();
	
	text = new sciText();
	text->plane = plane;
	text->ParamInit(4, "", 30, 20, 0, 0, 0, JUST_VERTICAL | JUST_HORIZONTAL);
#ifndef GAME_WATCHER_NUMBER_IN_BLACK
	text->SetColorMap(0, 0, 0, 220, 220, 220);
#endif
	
	muteText = new sciText();
	muteText->plane = plane;
	muteText->ParamInit(4, GetString(205, getS), 40, 20, 0, 0, 0, JUST_VERTICAL | JUST_HORIZONTAL);
#ifndef GAME_WATCHER_NUMBER_IN_BLACK
	muteText->SetColorMap(0, 0, 0, 220, 220, 220);
#endif

	chatEditBalloon = new ChatBalloon(0, 0, CBO_NONE, plane);
	chatEditBalloon->plane = plane;
	chatEditBalloon->Init();
	chatEditBalloon->SetView(25159);
	
	chatDisplayBalloon = new ChatBalloon(0, 0, CBO_NONE, plane);
	chatDisplayBalloon->plane = plane;
	chatDisplayBalloon->Init();
	chatDisplayBalloon->SetView(200002);
	
	SetScript(new WatchersBlinkScript(this));
}

void GameWatchersDisplay::SetPri(int newPri)
{
	sciActor::SetPri(newPri);
	
	if (text)
		text->SetPri(newPri + 1);
	if (muteText)
		muteText->SetPri(newPri + 1);
	if (chatEditBalloon)
		chatEditBalloon->SetPri(newPri + 2);
	if (chatDisplayBalloon)
		chatDisplayBalloon->SetPri(newPri + 2);
}

void GameWatchersDisplay::Posn(int newX, int newY)
{
	sciActor::Posn(newX, newY);
	
	if (text)
		text->Posn(newX + 4, newY - 1);

	if (muteText)
		muteText->Posn(newX - 1, newY + 11);
	
	if (chatDisplayBalloon) {
#ifndef GAME_POSITIONS_WATCHER_CHAT_LINE
		chatDisplayBalloon->Posn(newX - 14, newY + INFO_BAR_DY - INFO_BAR_PRESET_Y);
		chatDisplayBalloon->text->Posn(newX + GetWidth() + 18, newY + 5);
#endif
		chatDisplayBalloon->text->SetJust(JUST_LEFT | JUST_VERTICAL);
		chatDisplayBalloon->text->SetWidth(chatDisplayBalloon->GetWidth() - 8);
		chatDisplayBalloon->text->SetHeight(20);
	}
	
	if (chatEditBalloon)
		chatEditBalloon->Posn((640 - chatEditBalloon->GetWidth()) / 2, (480 - chatEditBalloon->GetHeight()) / 2);
}

void GameWatchersDisplay::Show(int numWatchers)
{
	char s[20];
	
	sciActor::Show();
	
	if (text) {
		sprintf(s, "%d", numWatchers);
		text->Show();
		text->SetText(s);
	}
	if (muteText) {
		if (roomClient->tables[display->table].muteWatchers)
			muteText->Show();
		else
			muteText->Hide();
	}
}

void GameWatchersDisplay::Hide()
{
	sciActor::Hide();
	
	if (text)
		text->Hide();
	if (muteText)
		muteText->Hide();
	if (chatEditBalloon)
		chatEditBalloon->Hide();
	if (chatDisplayBalloon)
		chatDisplayBalloon->Hide();
}

Boolean GameWatchersDisplay::HandleEvent(sciEvent &event)
{
	i18nStr getS;
	int i;
	GameState *gameState;
	ROOMCLIENTPLAYER *player;
	
	if (roomClient->status < RCS_IN_ROOM)
		return FALSE;
	
	if (event.claimed)
		return FALSE;
	
	if (display->popup->HandleEvent(event))
		return event.claimed;
	
	if (event.type != MOUSE_DOWN || !(event.modifiers & LEFT_MOUSE))
		return FALSE;
	
	if (!sciFeature::OnMe(event))
		return FALSE;
	
	gameState = &roomClient->tables[table].gameState;
	
	display->popup->SetNumItems(roomClient->tables[table].GetNumWatchers() + 2);
	
	display->popup->AddItem(0, POPITEM_DISPLAY_BOLD, GetString(81, getS));
	display->popup->AddItem(0, POPITEM_DELIMITER, "");
	
	for (i = 0 ; i < MAX_GAME_WATCHERS ; i++)
		if (gameState->watcherIDs[i] != -1) {
			player = roomClient->GetPlayerFromID(gameState->watcherIDs[i]);
			if (player)
				display->popup->AddItem(POPID_GAME_WATCHERS_CLICK, POPITEM_SELECTABLE, player->info.login, player->info.playerID);
		}
	
	display->popup->Posn(GetX(), GetY() - display->popup->GetHeight());
	display->popup->Show(this);
	
	event.claimed = TRUE;
	
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////

void WatchersBlinkScript::ChangeState(int newState)
{
	sciScript::ChangeState(newState);
	
	if (owner->IsHidden()) {
		SetNextState(0);
		SetTicks(60);
		
		return;
	}
	
	switch (state) {
		case 0:
			owner->SetCel(0);
			SetTicks(120 + 60 * random (6));
			break;
		
		case 1:
			owner->SetCel(1);
			SetTicks(5);
			break;
		
		case 2:
			owner->SetCel(2);
			SetTicks(5);
			break;
		
		case 3:
			owner->SetCel(1);
			SetNextState(0);
			SetTicks(5);
			break;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////

StandardCard::StandardCard(GAMECHARACTER *newOwner, sciPlane *thePlane)
{
	int i;
	
	plane = thePlane;
	value = 0;
	SetView(220001);
	owner = newOwner;
	enabled = FALSE;
	mouseDown = FALSE;
	
	upperRank = new sciView();
	upperRank->plane = thePlane;
	upperRank->Init();
	upperRank->SetView(220001);
	
	upperSuit = new sciView();
	upperSuit->plane = thePlane;
	upperSuit->Init();
	upperSuit->SetView(220001);
	
	center = new sciView();
	center->plane = thePlane;
	center->Init();
	center->SetView(220001);

	lowerRank = new sciView();
	lowerRank->plane = thePlane;
	lowerRank->Init();
	lowerRank->SetView(220001);
	
	lowerSuit = new sciView();
	lowerSuit->plane = thePlane;
	lowerSuit->Init();
	lowerSuit->SetView(220001);

	for (i = 0 ; i < 10 ; i++) {
		pips[i] = new sciView();
		pips[i]->plane = thePlane;
		pips[i]->Init();
		pips[i]->SetView(220001);
	}
}

StandardCard::~StandardCard()
{
	int i;
	
	if (owner) 
		owner = NULL;
	
	delete upperRank;
	delete upperSuit;
	delete center;
	delete lowerRank;
	delete lowerSuit;
	
	for (i = 0 ; i < 10 ; i++)
		delete pips[i];
}

void StandardCard::GetRankAndSuit(int *rank, int *suit)
{
	unsigned char vRank, vSuit;
	
	if (value == 0xFF) {
		(*rank) = 0;
		(*suit) = 0;
		
		return;
	}
	
	// deck art is ordered spades, clubs, diamonds, hearts, with Ace first in each suit

	// rank goes from 1 to 13
	
	vRank = TranslateRank(value & 0x0Fu);
	if (vRank >= 0x0 && vRank <= 0xB)
		(*rank) = vRank + 2;
	else {
		assert(vRank == 0xC);
		(*rank) = 1;
	}
	
	// suits go from 1 to 4
	
	vSuit = TranslateSuit(value & 0xF0u);
	
	switch (vSuit) {
		case SUIT_HEARTS: (*suit) = 4; break;
		case SUIT_CLUBS: (*suit) = 2; break;
		case SUIT_DIAMONDS: (*suit) = 3; break;
		case SUIT_SPADES: (*suit) = 1; break;
		default:
			assert(FALSE);
			(*suit) = 1; break;
			break;
	}
}

void StandardCard::Show(int pri)
{
	static bool haveCD = false, checked = false;
	int i, b, pipPlace, rank, suit;
	POINT pipPosn;
	
	// card cels:
	// 0: face-up white background
	// 1: face-down blue background
	// 2-14: Black ranks A-K, right-side up
	// 15-27: Black ranks A-K, upside down
	// 28-40: Red ranks A-K, right-side up
	// 41-53: Red ranks A-K, upside down
	// 54-56: Spades suit face cards J-K
	// 57-59: Clubs suit face cards J-K
	// 60-62: Diamonds suit face cards J-K
	// 63-65: Hearts suit face cards J-K
	// 66-69: Right-side up suits: spades, clubs, diamonds, hearts
	// 70-73: Upside down suits: spades, clubs, diamonds, hearts
	// 74-77: Right-side up pips: spades, clubs, diamonds, hearts
	// 78-81: Upside down pips: spades, clubs, diamonds, hearts
	
	if (!checked) {
		haveCD = IHaveAHoyleCDProductInstalled(CUSTOMER_STRING_1, CUSTOMER_STRING_2, CUSTOMER_STRING_3) != 0;
		checked = true;
	}
	
	if (value == HIDDEN_CARD) {
		if(owner == NULL || roomClient->tables[owner->table].m_iCardBacks == -1)
		{
			b = roomClient->globalIniFile->ReadValue("CardBacks", 0);
			if (b < 0 || b >= NUM_CARDBACKS)
				b = 0;

			if(owner)
				roomClient->tables[owner->table].m_iCardBacks = b;
		}
		else
		{
			b = roomClient->tables[owner->table].m_iCardBacks;
		}

		if (b > 0 && haveCD && pApp->pResourceManager->ResourceIsAvailable(RES_TSPRITE, 200052)) {
			SetView(200052);
			SetCel(b - 1);
		}
		else {
			SetView(220001);
			SetCel(1);
		}
		upperRank->Hide();
		upperSuit->Hide();
		center->Hide();
		lowerRank->Hide();
		lowerSuit->Hide();
		for (i = 0 ; i < 10 ; i++)
       		pips[i]->Hide();
	}
	else {
		GetRankAndSuit(&rank, &suit);
		
		if(owner == NULL || roomClient->tables[owner->table].m_iCardFaces == -1)
		{
			b = roomClient->globalIniFile->ReadValue("CardFaces", 0);
			if (b < 0 || b >= 2)
				b = 0;

			if(owner)
				roomClient->tables[owner->table].m_iCardFaces = b;
		}
		else
		{
			b = roomClient->tables[owner->table].m_iCardFaces;
		}

		if (b == 1 && haveCD && pApp->pResourceManager->ResourceIsAvailable(RES_TSPRITE, 200053)) {
			SetView(200053);
			SetCel((suit - 1) * 13 + (rank - 1));
			upperRank->Hide();
			upperSuit->Hide();
			center->Hide();
			lowerRank->Hide();
			lowerSuit->Hide();
			for (i = 0 ; i < 10 ; i++)
       			pips[i]->Hide();
		}
		else {
			SetView(220001);
			SetCel(0);
			
			upperRank->SetCel(((suit <= 2) ? 2 : 28) + (rank - 1));
			upperRank->Show();
			
			upperSuit->SetCel(66 + (suit - 1));
			upperSuit->Show();
			
			if (rank > 10) {
				center->SetCel(54 + (suit - 1) * 3 + (rank - 11));
				center->Show();
			}
			else
				center->Hide();
			
			lowerRank->SetCel(((suit <= 2) ? 15 : 41) + (rank - 1));
			lowerRank->Show();
			
			lowerSuit->SetCel(70 + (suit - 1));
			lowerSuit->Show();
			
			for (i = 0 ; i < 10 ; i++) {
				if (rank > 10)
					pips[i]->Hide();
				else {
					pipPlace = GetPipPlace(i, rank);

					if (pipPlace == -1)
						pips[i]->Hide();
					else {
						pips[i]->SetCel(GetPipCel(pipPlace, suit));
						pipPosn = GetPipPosn(pipPlace);
						pips[i]->Posn(pipPosn.x, pipPosn.y);
						pips[i]->Show();
					}
				}
			}
		}
	}
	
	SetPri(pri);
	
	sciActor::Show();
}

void StandardCard::Hide()
{
	int i;
	
	sciActor::Hide();
	
	upperRank->Hide();
	upperSuit->Hide();
	center->Hide();
	lowerRank->Hide();
	lowerSuit->Hide();
	
	for (i = 0 ; i < 10 ; i++)
		pips[i]->Hide();
}

void StandardCard::Posn(int newX, int newY)
{
	int i, pipPlace, rank, suit;
	POINT pipPosn;
	
	sciActor::Posn(newX, newY);
	
	upperRank->Posn(newX, newY);
	upperSuit->Posn(newX, newY);
	center->Posn(newX, newY);
	lowerRank->Posn(newX, newY);
	lowerSuit->Posn(newX, newY);
	
	if (value == 0 || value == 0xFF)
		return;
	
	GetRankAndSuit(&rank, &suit);
	
	for (i = 0 ; i < 10 ; i++) {
		if (rank <= 10) {
			pipPlace = GetPipPlace(i, rank);

			if (pipPlace != -1) {
				pipPosn = GetPipPosn(pipPlace);
				pips[i]->Posn(pipPosn.x, pipPosn.y); 
			}
		}
	}
}

void StandardCard::SetPri(int newPri)
{
	int i;
	
	sciActor::SetPri(newPri);
	
	upperRank->SetPri(newPri + 1);
	upperSuit->SetPri(newPri + 1);
	center->SetPri(newPri + 1);
	lowerRank->SetPri(newPri + 1);
	lowerSuit->SetPri(newPri + 1);
	
	for (i = 0 ; i < 10 ; i++)
		pips[i]->SetPri(newPri + 1);
}

int StandardCard::GetPipPlace(int pipNum, int rank)
{
	//Numbers of the pip positions, from top to bottom, left to right
	static int pipPlaces[10][10] = { { 9, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
									 { 2, 16, -1, -1, -1, -1, -1, -1, -1, -1 },
									 { 2,  9, 16, -1, -1, -1, -1, -1, -1, -1 },
									 { 1,  3, 15, 17, -1, -1, -1, -1, -1, -1 },
									 { 1,  3,  9, 15, 17, -1, -1, -1, -1, -1 },
									 { 1,  3,  8, 10, 15, 17, -1, -1, -1, -1 },
									 { 1,  3,  5,  8, 10, 15, 17, -1, -1, -1 },
									 { 1,  3,  5,  8, 10, 13, 15, 17, -1, -1 },
									 { 1,  3,  6,  7,  9, 11, 12, 15, 17, -1 },
									 { 1,  3,  4,  6,  7, 11, 12, 14, 15, 17 } };
	
	assert(rank <= 10);
	
	return pipPlaces[rank - 1][pipNum];
}

int StandardCard::GetPipCel(int pipPlace, int suit)
{
	assert(pipPlace > -1);
	
	// pips 74-77 are right-side up, 78-81 are upside down
	
	return (pipPlace <= 10) ? 74 + (suit - 1) : 78 + (suit - 1);
}

POINT StandardCard::GetPipPosn(int pipPlace)
{
	POINT offset;
	
	//positions for the 17 different pip locations.
	
	static POINT pipPosns[17] = { {9, 9},   {20, 9},  {31, 9},  {20, 16}, {20, 20},
								  {9, 23},  {31, 23}, {9, 31},  {20, 31}, {31, 31},
								  {9, 40},  {31, 40}, {20, 43}, {20, 47},  {9, 54}, 
								  {20, 54}, {31, 54} };
	
	assert(pipPlace != -1);
	
	offset.x = pipPosns[pipPlace - 1].x;
	offset.y = pipPosns[pipPlace - 1].y;
	
	offset.x += GetX();
	offset.y += GetY();
	
	return offset;
}

////////////////////////////////////////////////////////////////////////////////////////////////

void GameExitButton::DoClick(BOOL netMessage)
{
	if (roomClient->status < RCS_IN_ROOM)
		return;
	
	SetState(DEFAULT_BUTTON_NORMAL);
	
	plane->AttemptClosePlane();
}

void GameMinimizeButton::DoClick(BOOL netMessage)
{
	SetState(DEFAULT_BUTTON_NORMAL);
	
	ShowWindow(plane->GetWindow()->hWnd, SW_MINIMIZE);
}

void GameSponsorshipButton::DoClick(BOOL netMessage)
{
#ifdef _DEBUG
	DebugString("browser\n");
#endif
	
	if (won && wonParams.sponsorURL[0])
		won->LaunchBrowser(wonParams.sponsorURL);
}

void ShowLobbyButton::DoClick(BOOL netMessage)
{
	SetForegroundWindow(MAIN_HWND);
}

void ShowChatButton::DoClick(BOOL netMessage)
{
	if (parent->gameChatLog->IsChatLogHidden()) {
		// turning on the chat window, and switch the option back to the correct 
		// one (log only or log+balloon)
		if (roomClient->globalIniFile->ReadValue("PreferBalloons", 1) == 1)
			roomClient->globalIniFile->WriteValue("ChatDisplay", CHAT_BALLOONS_AND_LOG);
		else
			roomClient->globalIniFile->WriteValue("ChatDisplay", CHAT_LOG_ONLY);
	}
	else {
 		//If we are hiding the chat window, we must set the balloons to be available.
		roomClient->globalIniFile->WriteValue("ChatDisplay", CHAT_BALLOONS_ONLY);
	}

	parent->PreferencesChanged();
}

////////////////////////////////////////////////////////////////////////////////////////////////

void GameOptionsButton::DoClick(BOOL netMessage)
{
	if (roomClient->status < RCS_IN_ROOM)
		return;
	
	SetState(DEFAULT_BUTTON_NORMAL);
	
	if (display) {
		if (!display->optionsDialog) {
			display->optionsDialog = new OptionsDialogBox(plane);
			display->optionsDialog->Init();
		}
		if (display->optionsDialog->IsHidden())
			display->optionsDialog->Show(roomClient->lastOptionsDialogVersion, TRUE);
	}
}

void GamePopup::ItemSelected(int itemID, int itemPlayerID, int itemX, int itemY)
{
	char *text, *data, s[512];
	i18nStr getS, getS2, getS3, getS4, getS5;
	int i;
	POINT p;
	ROOMCLIENTPLAYER *player;
	MTS_GAME_BOOT_MESSAGE mtsGBM;
	MTS_GAME_RESTART_MESSAGE mtsGRM;
	HGLOBAL handle;
	int version;
	
	switch (itemID) {
		// click on self in game
		
		case POPID_GAME_EDIT_PROFILE:
			if (!display->profileDialog) {
				display->profileDialog = new ProfileDialogBox(plane);
				display->profileDialog->Init();
			}
			display->profileDialog->Show(playerID);
			break;
		case POPID_GAME_PERSONAL_OPTIONS:
		case POPID_GAME_TABLE_OPTIONS:
		case POPID_GAME_RATING_OPTIONS:
			if (!display->optionsDialog) {
				display->optionsDialog = new OptionsDialogBox(plane);
				display->optionsDialog->Init();
			}
			if (display->optionsDialog->IsHidden())
			{
				if(itemID == POPID_GAME_PERSONAL_OPTIONS)
					version = ODBV_PERSONAL;
				else if(itemID == POPID_GAME_RATING_OPTIONS)
					version = ODBV_RATINGS;
				else
					version = ODBV_TABLE;
			
				display->optionsDialog->Show(version, TRUE);
			}
			break;
		case POPID_GAME_RESTART_GAME:
			player = roomClient->GetPlayerFromID(roomClient->GetMyID());
			mtsGRM.messageType = MTS_GAME_RESTART;
			mtsGRM.table = display->table;
			mtsGRM.playerID = roomClient->GetMyID();
			mtsGRM.seat = player->info.seat;
			mtsGRM.restartGame = !roomClient->tables[display->table].restartVotes[player->info.seat];
			
			roomClient->SendMessageToServer((char *)&mtsGRM, sizeof(MTS_GAME_RESTART_MESSAGE));
			break;
		case POPID_GAME_START_CHAT:
			display->gameChatLog->DoClick();
			break;
		case POPID_GAME_COPY_CHAT_LOG:
			if (!OpenClipboard(NULL))
				break;
			if (!EmptyClipboard()) {
				CloseClipboard();
				break;
			}
			handle = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, MAX_TRANSPARENT_CHAT_LINES * 300);
			if (!handle){
				CloseClipboard();
				break;
			}
			data = (char *)GlobalLock(handle);
			if (!data) {
				GlobalFree(handle);
				CloseClipboard();
				break;
			}
			data[0] = 0;
			for (i = 0 ; i < MAX_TRANSPARENT_CHAT_LINES ; i++) {
				text = display->gameChatLog->chatTransparentLines[i]->text->GetText();
				if (text[0] == 0)
					continue;
				strnzcpy(s, text, 255);
				strcat(s, "\r\n");
				strcat(data, s);
			}
			GlobalUnlock(handle);
			SetClipboardData(CF_TEXT, handle);
			CloseClipboard();
			break;
			
		case POPID_GAME_SHOW_CHATSOUNDS:
			if (!display->chatSoundDialog) {
				display->chatSoundDialog = new ChatSoundDialogBox(plane);
				display->chatSoundDialog->Init();
			}
			if (display->chatSoundDialog->IsHidden())
				display->chatSoundDialog->Show();

			break;

		// click on other in game
		
		case POPID_GAME_FRIEND:
			player = roomClient->GetPlayerFromID(playerID);
			if (player == NULL)
				return; // assert(FALSE) // allow this case for when player leaves while popup is up
			player->isFriend = !player->isFriend;
			roomClient->ReorderPlayer(roomClient->GetPlayerIndexFromID(playerID));
			roomClient->playersDisplay->DisplayLines();
			break;
		case POPID_GAME_VIEW_PROFILE:
			if (!display->profileDialog) {
				display->profileDialog = new ProfileDialogBox(plane);
				display->profileDialog->Init();
			}
			display->profileDialog->Show(playerID);
			break;
		case POPID_GAME_COMPLAINT:
			if (!display->complaintDialog) {
				display->complaintDialog = new ComplaintDialogBox(plane);
				display->complaintDialog->Init();
			}
			display->complaintDialog->Show(playerID, TRUE);
			break;
		case POPID_GAME_BOOT:
			player = roomClient->GetPlayerFromID(playerID);
			if (player == NULL)
				return; // assert(FALSE) // allow this case for when player leaves while popup is up
			mtsGBM.messageType = MTS_GAME_BOOT;
			mtsGBM.table = display->table;
			mtsGBM.playerID = playerID;
			mtsGBM.seat = player->info.seat;
			
			roomClient->SendMessageToServer((char *)&mtsGBM, sizeof(MTS_GAME_BOOT_MESSAGE));
			break;
		
		// click on name in game watchers popup
		
		case POPID_GAME_WATCHERS_CLICK:
			display->AddPlayerPopupItems(itemPlayerID);
			GetCursorPos(&p);
			MapWindowPoints(NULL, plane->GetWindow()->hWnd, &p, 1);
			p.y -= plane->GetWindow()->rcInner.top;
			display->popup->Posn(itemX, (p.y - GetHeight()) + 3);
			display->popup->Show(NULL);
			break;
		
		case POPID_GAME_RATINGS:
			player = roomClient->GetPlayerFromID(playerID);
			if (player == NULL)
				return; // assert(FALSE) // allow this case for when player leaves while popup is up
			
			sprintf(s, "%s %d\n %s %d\n%s %d\n%s %d\n%s %d", GetString(339, getS), player->info.wins,
				GetString(340, getS2), player->info.losses, 
				GetString(341, getS3), player->info.draws, 
				GetString(342, getS4), player->info.quits,
				GetString(338, getS5), player->info.rating);
			sprintf(getS2, GetString(343, getS), player->info.login);

			roomClient->tables[display->table].ShowModelessMessage(0, s, getS2, MB_OK);
			break;

		default:
			if (itemID >= POPID_GAME_SPECIFIC_BASE)
				roomClient->GameItemSelected(itemID, playerID, seat, display);
			else
				assert(FALSE);
			break;
	}
}
