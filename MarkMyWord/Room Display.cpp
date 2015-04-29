#include "os.h"
#include "sci.h"
#include "globals.h"
#include "util.h"
#include "inifile.h"
#include "resman.h"
#include "maketex.h"

#include "Client Include.h"

#define SCIVIEW_SCALING

////////////////////////////////////////////////////////////////////////////////////////////////

//
Boolean RoomOverlayView::IsInTableArea(sciEvent &event)
{
	if (event.x > 13 && event.y > 77 && event.x < 397 && event.y < 294)
	{
		if (event.z > 0)
			roomClient->tablesScrollBox->DoScroll(GDefs::REVERSE, (event.modifiers & CTRL_DOWN) ? MAX_ROOM_TABLES: 2 * event.z/120);
		else
			roomClient->tablesScrollBox->DoScroll(GDefs::FORWARD, (event.modifiers & CTRL_DOWN) ? MAX_ROOM_TABLES: 2 * abs(event.z/120));
		event.claimed = TRUE;
	}

	return event.claimed;
}

//
Boolean RoomOverlayView::IsInPlayerListArea(sciEvent &event)
{
	if (event.x > 434 && event.y > 100 && event.x < 630 && event.y < 320)
	{
		if (event.z > 0)
			roomClient->playersScrollBox->DoScroll(GDefs::REVERSE, 2 * event.z/120);
		else
			roomClient->playersScrollBox->DoScroll(GDefs::FORWARD, 2 * abs(event.z/120));
		event.claimed = TRUE;
	}

	return event.claimed;
}

//
Boolean RoomOverlayView::IsInChatArea(sciEvent &event)
{
	if (event.x > 15 && event.y > 334 && event.x < 514 && event.y < 438)
	{
		if (event.z > 0)
			roomClient->chatScrollBox->DoScroll(GDefs::REVERSE, 2 * event.z/120);
		else
			roomClient->chatScrollBox->DoScroll(GDefs::FORWARD, 2 * abs(event.z/120));
		event.claimed = TRUE;
	}

	return event.claimed;
}

//
Boolean RoomOverlayView::HandleEvent(sciEvent &event)
{
	if (event.claimed)
		return FALSE;
	
	if (roomClient->status != RCS_IN_ROOM)
		return FALSE;
	
	if (event.type != MOUSE_WHEEL)
		return FALSE;
	
	(IsInTableArea(event) || IsInPlayerListArea(event) || IsInChatArea(event));
	
	return event.claimed;
}


void RoomClient::ShowStatus()
{
	char s[256];
	i18nStr getS;
	
#ifdef NO_HW_ROOM_INTERFACE
	return;
#endif
	
	switch (status) {
		case RCS_UNCONNECTED: strcpy(s, GetString(135, getS)); break;
		case RCS_RECEIVING_ROOM_HEADER: strcpy(s, GetString(136, getS)); break;
		case RCS_RECEIVING_ROOM_TABLES: strcpy(s, GetString(137, getS)); break;
		case RCS_RECEIVING_ROOM_PLAYERS: strcpy(s, GetString(138, getS)); break;
		case RCS_RECEIVING_ROOM_CHAT_HISTORY: strcpy(s, GetString(139, getS)); break;
		case RCS_IN_ROOM: strcpy(s, GetString(140, getS)); break;
	}
	
	if (status == RCS_IN_ROOM)
		statusText->Hide();
	else {
		statusText->SetText(s);
		statusText->Show();
	}
	
	if (status >= RCS_RECEIVING_ROOM_PLAYERS)
		tablesScrollBox->DoRefresh(tablesScrollBox->GetFirstVisibleItem());
	
	if (status >= RCS_RECEIVING_ROOM_CHAT_HISTORY) {
		playersScrollBox->DoRefresh(playersScrollBox->GetFirstVisibleItem());
		if (numPlayers == 1)
			strcpy(s, GetString(204, getS));
		else
			sprintf(s, GetString(203, getS), numPlayers);
		playersInRoomLabel->SetText(s);
		UpdateChatButtons(NULL);
	}
	
	if (status == RCS_IN_ROOM &&
		players[0].info.table != -1 &&
		tables[players[0].info.table].status == TS_DISPLAYED)
		showGameButton->Enable();
	else
		showGameButton->Disable();
	
	/*
	for (i = 0 ; i < numPlayers ; i++) {
		if (players[i].info.table != -1)
			switch (tables[players[i].info.table].status) {
				case TS_EMPTY: strcpy(tableStatus, "TS_EMPTY"); break;
				case TS_PENDING: strcpy(tableStatus, "TS_PENDING"); break;
				case TS_PLAYING: strcpy(tableStatus, "TS_PLAYING"); break;
				case TS_DISPLAYED: strcpy(tableStatus, "TS_DISPLAYED"); break;
				default:
					strcpy(tableStatus, "ERROR");
					break;
			}
		else
			strcpy(tableStatus, "STANDING");
		sprintf(p, "player: %s, ID: %d, table: %d, seat: %d [%s]\n",
					players[i].info.login,
					players[i].info.playerID,
					players[i].info.table,
					players[i].info.seat,
					tableStatus);
		strcat(s, p);
	}
	*/
}

////////////////////////////////////////////////////////////////////////////////////////////////

void RoomClient::ShowDiagnostics()
{
	char s[256];
	int b, br, bs, mr, ms;
	DWORD ticks, bytesRead, bytesSent, messagesRead, messagesSent;
	static Boolean doNotShow, didInit = FALSE;
	static DWORD lastTicks = 0,
				 lastBytesRead = 0, lastBytesSent = 0, lastMessagesRead = 0, lastMessagesSent = 0;
	
#ifdef NO_HW_ROOM_INTERFACE
	return;
#endif
	
	if (!connections)
		return;
	
	if (status != RCS_IN_ROOM)
		return;
	
	if (!didInit) {
		b = pApp->pIniFile->ReadValue("ShowTransferRate", 0);
		if (b == 0)
			doNotShow = TRUE;
		else
			diagnostics->Show();
		didInit = TRUE;
	}
	
	if (doNotShow)
		return;
	
	ticks = GetTickCount();
	
	if (ticks < (lastTicks + 1000))
		return;
	
	lastTicks = ticks;
	
	connections->GetTransferStats(&bytesRead, &bytesSent, &messagesRead, &messagesSent);
	
	br = bytesRead - lastBytesRead;
	bs = bytesSent - lastBytesSent;
	mr = messagesRead - lastMessagesRead;
	ms = messagesSent - lastMessagesSent;
	
	lastBytesRead = bytesRead;
	lastBytesSent = bytesSent;
	lastMessagesRead = messagesRead;
	lastMessagesSent = messagesSent;
	
	sprintf(s, "br = %d, bs = %d, mr = %d, ms = %d\nbrT = %d, bsT = %d, mrT = %d, msT = %d",
			   br, bs, mr, ms,
			   (int)bytesRead, (int)bytesSent, (int)messagesRead, (int)messagesSent);
	
	diagnostics->SetText(s);
}

////////////////////////////////////////////////////////////////////////////////////////////////

int RoomClient::GetPlayerView(int playerNumber)
{
	if(playerNumber < 46)
		return 200119; // original player art (#'s 0-45)
	else if(playerNumber < 75)
		return 200054; // new player art from Fall '99 release (#'s 46-74)
	else if(playerNumber < 76)
		return 200055; // Tux the Linux penguin (# 75)
	else
		return 200039;  // revert to guaranteed art
}

int RoomClient::GetNumShadows()
{
	int iNumItems = 0;

	if(pApp->pResourceManager->ResourceIsAvailable(RES_TSPRITE, roomClient->GetPlayerView(0))) {
		iNumItems += 46;
	}

	if(pApp->pResourceManager->ResourceIsAvailable(RES_TSPRITE, roomClient->GetPlayerView(46))) {
		iNumItems += 29;
	}

	if(pApp->pResourceManager->ResourceIsAvailable(RES_TSPRITE, roomClient->GetPlayerView(75))) {
		iNumItems += 1;
	}

	return iNumItems;
}


int RoomClient::GetPlayerViewCel(int playerNumber, Boolean facingWest)
{
	static int views[NUM_SHADOWS][2] = {
		// east, west
		{ 0, 0 }, // male shadow
		{ 1, 1 }, // female shadow
		{ 2, 3 }, // bart
		{ 4, 4 }, // rick
		{ 5, 6 }, // carlos
		{ 7, 7 }, // texan
		{ 8, 9 }, // white haired man
		{ 10, 11 }, // jasper
		{ 12, 13 }, // leo
		{ 14, 14 }, // goof
		{ 15, 16 }, // anna
		{ 17, 17 }, // movie star
		{ 18, 18 }, // trucker babe
		{ 19, 19 }, // robin
		{ 20, 20 }, // elayne
		{ 21, 21 }, // rebecca
		{ 22, 23 }, // ethel
		{ 24, 25 }, // kathryn
		{ 26, 27 }, // black woman with turtleneck
		{ 28, 28 }, // readhead girl
		{ 29, 29 }, // beatnik chick
		{ 30, 31 }, // woman
		{ 32, 33 }, // frank
		{ 34, 35 }, // deano
		{ 36, 36 }, // sammy
		{ 37, 37 }, // elvis
		{ 38, 39 }, // charro
		{ 40, 41 }, // showgirl
		{ 42, 42 }, // deniro
		{ 43, 43 }, // arnold
		{ 44, 44 }, // marylin
		{ 45, 45 }, // roswell
		{ 46, 46 }, // joker
		{ 47, 47 }, // gargoyle
		{ 48, 49 }, // dragon
		{ 50, 50 }, // marvin
		{ 51, 51 }, // burro
		{ 52, 53 }, // aligator
		{ 54, 54 }, // lion
		{ 55, 56 }, // harley
		{ 57, 57 }, // teddy bear
		{ 58, 58 }, // puppy
		{ 59, 59 }, // vegetables
		{ 60, 61 }, // knight
		{ 62, 63 }, // bag head
		{ 64, 65 },  // diver
		{ 0, 0 }, // Glamorous Woman
		{ 1, 1 }, // Glamorous Man
		{ 2, 2 }, // Ganster
		{ 3, 3 }, // Astronaut
		{ 4, 4 }, // Sumo wrestler
		{ 5, 5 }, // Doctor
		{ 6, 6 }, // Mountie
		{ 7, 7 }, // Chef
		{ 8, 8 }, // Cyclist
		{ 9, 9 }, // Medieval King
		{ 10, 10 }, // Medieval Queen
		{ 11, 11 }, // Hawaiian King
		{ 12, 12 }, // Egyptian queen
		{ 13, 13 }, // Genie
		{ 14, 14 }, // Unicorn
		{ 15, 15 }, // Elf
		{ 16, 16 }, // Witch
		{ 17, 17 }, // Ogre
		{ 18, 18 }, // Vampire
		{ 19, 19 }, // Frankenstein
		{ 20, 20 }, // Skeleton
		{ 21, 21 }, // Clown
		{ 22, 22 }, // Chili Peppers
		{ 23, 23 }, // Corn & Potato
		{ 24, 24 }, // Octopus
		{ 25, 25 }, // Pug
		{ 26, 26 }, // Cats
		{ 27, 27 }, // Bulldog
		{ 28, 28 }, // Bunny
		{ 0, 0} }; // Tux the Linux penguin
	
	if (playerNumber < 0 || playerNumber >= NUM_SHADOWS)
		playerNumber = 0;
	
	return views[playerNumber][facingWest ? 1 : 0];
}

////////////////////////////////////////////////////////////////////////////////////////////////

void RoomClient::SetCurrentAd(WONadInfo *wonAdInfo)
{
	char toPath[MAX_PATH];
	int err;
	
#ifdef NO_HW_ROOM_INTERFACE
	return;
#endif
	
	if (status != RCS_IN_ROOM && status != RCS_QUITTING)
		return;
	
	if (gifAd) { gifAd->SetView(0, "", TRUE); delete gifAd; gifAd = NULL; }
	if (jpegAd) { jpegAd->SetView(0); delete jpegAd; jpegAd = NULL; }
	if (gifGameAd) {
		gifGameAd->SetView(0, "", TRUE);
		if (gameAdPlane)
			gameAdPlane->DeleteFromCast(gifGameAd);
		delete gifGameAd;
		gifGameAd = NULL;
	}
	if (jpegGameAd) {
		jpegGameAd->SetView(0);
		if (gameAdPlane)
			gameAdPlane->DeleteFromCast(jpegGameAd);
		delete jpegGameAd;
		jpegGameAd = NULL;
	}
	
	pApp->pResourceManager->FreeResource(RES_TSPRITE, 500000);
	pApp->pResourceManager->PurgeResource(RES_TSPRITE, 500000);
	
	if (wonAdInfo == NULL) {
		if (currentAdInfo) { delete currentAdInfo; currentAdInfo = NULL; }
		currentAdType = AT_NONE;
		gameAdPlane = NULL;
		
		return;
	}
	
	if (status != RCS_IN_ROOM)
		return;
	
	(*currentAdInfo) = (*wonAdInfo);
	currentAdType = AT_NONE;
	if (strstr(currentAdInfo->sAdPath, ".jpt"))
		currentAdType = AT_JPEG;
	if (strstr(currentAdInfo->sAdPath, ".gft"))
		currentAdType = AT_GIF;
	if (currentAdType == AT_NONE)
		{ assert(FALSE); return; }
	
	sprintf(toPath, "%s\\art\\500000.tex", wonParams.wonPath);
	if (CopyFile(currentAdInfo->sAdPath, toPath, FALSE) == FALSE) {
//		assert(FALSE);
		err = GetLastError();
		
#ifndef _DEBUG
		return;
#endif
	}
	
	if (currentAdType == AT_JPEG) {
		jpegAd = new JPEGAd();
		jpegAd->Init();
		jpegAd->Posn(86, 2);
		jpegAd->SetPri(501);
		jpegAd->SetView(500000);
		jpegAd->Show();
		if (gameAdPlane) {
			jpegGameAd = new JPEGAd();
			jpegGameAd->plane = gameAdPlane;
			jpegGameAd->Init();
			jpegGameAd->Posn(gameAdX, gameAdY);
			jpegGameAd->SetPri(gameAdPri);
			jpegGameAd->SetView(500000);
			jpegGameAd->Show();
		}
	}
	else {
		gifAd = new GIFAd();
		gifAd->Init();
		gifAd->Posn(86, 2);
		gifAd->SetPri(501);
		gifAd->SetView(500000, toPath, TRUE);
		gifAd->Show();
		if (gameAdPlane) {
			gifGameAd = new GIFAd();
			gifGameAd->plane = gameAdPlane;
			gifGameAd->Init();
			gifGameAd->Posn(gameAdX, gameAdY);
			gifGameAd->SetPri(gameAdPri);
			gifGameAd->SetView(500000, toPath, TRUE);
			gifGameAd->Show();
		}
	}
}

void RoomClient::SetGameAd(sciPlane *plane, int x, int y, int priority, int table)
{
#ifdef NO_HW_ROOM_INTERFACE
	return;
#endif
	
	if (status != RCS_IN_ROOM)
		return;
	
	if (plane == NULL) {
		if (gifGameAd) {
			if (gameAdPlane)
				gameAdPlane->DeleteFromCast(gifGameAd);
			delete gifGameAd;
			gifGameAd = NULL;
		}
		if (jpegGameAd) {
			if (gameAdPlane)
				gameAdPlane->DeleteFromCast(jpegGameAd);
			delete jpegGameAd;
			jpegGameAd = NULL;
		}
	}
	
	gameAdPlane = plane;
	
	if (status != RCS_IN_ROOM)
		{ if (gameAdPlane != NULL) assert(FALSE); return; }
	
	gameAdX = x;
	gameAdY = y;
	gameAdPri = priority;
	gameAdTable = table;
	
	if (gameAdPlane != NULL)
		if (jpegAd || gifAd)
			SetCurrentAd(currentAdInfo);
}

void RoomClient::ToggleAds()
{
	WONadFileDownload wonAdFileDownload;
	static bool adsOn = false;
	static DWORD timeAdsTurnedOff = 0;
	
	if (status != RCS_IN_ROOM || !won)
		return;
	
	if (adsOn) {
		if (MAIN_HWND != GetActiveWindow()) {
			adsOn = false;
			wonAdFileDownload.nStatus = DWNLOAD_OFF;
			strcpy(wonAdFileDownload.sAdTags, wonParams.adTags);
			wonAdFileDownload.sAdFolder[0] = 0;
			wonAdFileDownload.nAdFrequencySeconds = wonParams.adChangeRateInSeconds;
			won->SetAdFileDownload(&wonAdFileDownload);
			timeAdsTurnedOff = GetTickCount();
		}
		
		return;
	}
	
	// ad downloads are off; turn then on if the main window has come forward
	// and if they were turned off more than a minute ago
	
	if (MAIN_HWND == GetActiveWindow())
		if (GetTickCount() > (timeAdsTurnedOff + SecondsToTicks(60))) {
			adsOn = true;
			wonAdFileDownload.nStatus = DWNLOAD_ON;
			strcpy(wonAdFileDownload.sAdTags, wonParams.adTags);
			wonAdFileDownload.sAdFolder[0] = 0;
			wonAdFileDownload.nAdFrequencySeconds = wonParams.adChangeRateInSeconds;
			won->SetAdFileDownload(&wonAdFileDownload);
		}
}

Boolean RoomClient::MouseIsOverAd()
{
	int table;
	
	if (status != RCS_IN_ROOM)
		return FALSE;
	
	if (!players)
		return FALSE;
	
	if (pApp->MouseIsOverFeatureInPlane(pApp->pMainPlane, sponsorshipButton))
		return TRUE;
	
	table = players[0].info.table;
	
	if (table != -1 && tables[table].status == TS_DISPLAYED)
		if (pApp->MouseIsOverFeatureInPlane(tables[table].plane,
											tables[table].display->sponsorshipButton))
			return TRUE;
	
	switch (currentAdType) {
		case AT_JPEG:
			if (jpegAd && pApp->MouseIsOverFeatureInPlane(pApp->pMainPlane, jpegAd))
				return TRUE;
			if (gameAdPlane && jpegGameAd && pApp->MouseIsOverFeatureInPlane(gameAdPlane, jpegGameAd))
				return TRUE;
			break;
		
		case AT_GIF:
			if (gifAd && pApp->MouseIsOverFeatureInPlane(pApp->pMainPlane, gifAd))
				return TRUE;
			if (gameAdPlane && gifGameAd && pApp->MouseIsOverFeatureInPlane(gameAdPlane, gifGameAd))
				return TRUE;
			break;
	}
	
	return FALSE;
}

Boolean RoomClient::MouseIsOverChatSizeBar()
{
	int table;
	
	if (status != RCS_IN_ROOM)
		return FALSE;
	
	if (!players)
		return FALSE;
	
	table = players[0].info.table;
	
	if (table == -1)
		return FALSE;
	
	if (tables[table].status != TS_DISPLAYED)
		return FALSE;
	
	return pApp->MouseIsOverFeatureInPlane(tables[table].plane,
										   tables[table].display->gameChatLog->GetSizeLine());
}

Boolean JPEGAd::HandleEvent(sciEvent &event)
{
	if (event.claimed)
		return FALSE;
	
	if (roomClient->status != RCS_IN_ROOM)
		return FALSE;
	
	if (roomClient->popup->HandleEvent(event))
		return event.claimed;
	
	if (!sciFeature::OnMe(event))
		return FALSE;
	
	if (event.type != MOUSE_DOWN || !(event.modifiers & LEFT_MOUSE))
		return FALSE;
	
	event.claimed = TRUE;
	
#ifdef _DEBUG
	DebugString("browser\n");
#endif
	
	if (won && roomClient->currentAdInfo)
		won->LaunchBrowser(roomClient->currentAdInfo->sClickURL);
	
	return TRUE;
}

Boolean GIFAd::HandleEvent(sciEvent &event)
{
	if (event.claimed)
		return FALSE;
	
	if (roomClient->status != RCS_IN_ROOM)
		return FALSE;
	
	if (roomClient->popup->HandleEvent(event))
		return event.claimed;
	
	if (!sciFeature::OnMe(event))
		return FALSE;
	
	if (event.type != MOUSE_DOWN || !(event.modifiers & LEFT_MOUSE))
		return FALSE;
	
	event.claimed = TRUE;
	
#ifdef _DEBUG
	DebugString("browser\n");
#endif
	
	if (won && roomClient->currentAdInfo)
		won->LaunchBrowser(roomClient->currentAdInfo->sClickURL);
	
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////

Boolean RoomClient::AddPlayerPopupItems(int playerIndex)
{
	char s[256];
	i18nStr getS, getS2;
	int table, leaveTable = 0, tableOptions = 0, inviteToPlay = 0,
		inviteToWatch = 0, muteAndComplain = 0, boot = 0, addToTournament = 0, ratings = 0;
	
	if (playerIndex < 0 || playerIndex >= numPlayers)
		return FALSE;
	
	if (playerIndex == 0) { // click on self
		if (ControlKeyDown())
			return FALSE;
		
		if (players[playerIndex].info.table != -1) {
			leaveTable = 2;
			if (players[playerIndex].info.seat < MAX_GAME_PLAYERS)
				tableOptions = 1;
		}
		
		if (IsTournamentRoom() && (players[0].IsMonitorOrReferee() || players[0].IsSuperUser() || players[0].IsTournamentAdministrator()))
			addToTournament = IS_A_CASINO_GAME ? 3 : 2;
		
		if (players[0].IsMonitorOrSuperUser())
			boot = 3;

		if (HasRatings())
			ratings = 4;
		
		popup->SetNumItems(6 + leaveTable + tableOptions + addToTournament + boot + ratings);
		popup->playerID = players[0].info.playerID;
		popup->table = players[0].info.table;
		popup->seat = players[0].info.seat;
		
		popup->AddItem(0, POPITEM_DISPLAY_BOLD, players[playerIndex].info.login);
		if (leaveTable) {
			popup->AddItem(0, POPITEM_DELIMITER, "");
			popup->AddItem(POPID_ROOM_LEAVE_TABLE, POPITEM_SELECTABLE,
						   (players[playerIndex].info.seat >= MAX_GAME_PLAYERS) ? GetString(141, getS) : GetString(142, getS2));
		}
		if (tableOptions)
			popup->AddItem(POPID_ROOM_TABLE_OPTIONS, POPITEM_SELECTABLE, GetString(143, getS));
		popup->AddItem(0, POPITEM_DELIMITER, "");

		if (ratings)
		{
			popup->AddItem(POPID_ROOM_RATING_OPTIONS, POPITEM_SELECTABLE, GetString(360, getS));
			popup->AddItem(0, POPITEM_DELIMITER, "");
		}

		popup->AddItem(POPID_ROOM_EDIT_PROFILE, POPITEM_SELECTABLE, GetString(144, getS));
		popup->AddItem(POPID_ROOM_PERSONAL_OPTIONS, POPITEM_SELECTABLE, GetString(145, getS));
		if (addToTournament) {
			popup->AddItem(0, POPITEM_DELIMITER, "");
			if (players[playerIndex].IsInTournament())
				popup->AddItem(POPID_ROOM_TOURNAMENT_JOIN, POPITEM_SELECTABLE, GetString(146, getS));
			else
				popup->AddItem(POPID_ROOM_TOURNAMENT_JOIN, POPITEM_SELECTABLE, GetString(147, getS));
			if (addToTournament == 3) {
				MoneyToString(players[playerIndex].info.tournamentBankRoll, s);
				popup->AddItem(POPID_ROOM_TOURNAMENT_BANK, POPITEM_SELECTABLE, s);
			}
		}

		if (ratings)
		{
			popup->AddItem(0, POPITEM_DELIMITER, "");
			if(players[playerIndex].info.wins + players[playerIndex].info.losses +
				players[playerIndex].info.draws + players[playerIndex].info.quits)
			{
				sprintf(getS2, "%s %d", GetString(338, getS), players[playerIndex].info.rating);
				popup->AddItem(POPID_ROOM_RATINGS, POPITEM_SELECTABLE, getS2);
			}
			else
			{
				sprintf(getS2, "%s", GetString(372, getS));
				popup->AddItem(POPID_ROOM_RATINGS, POPITEM_DISPLAY_ONLY, getS2);
			}
		}
		
		if (boot) {
			popup->AddItem(0, POPITEM_DELIMITER, "");
			popup->AddItem(POPID_ROOM_BAN_OFFLINE, POPITEM_SELECTABLE, GetString(148, getS));
			popup->AddItem(POPID_ROOM_UNBAN, POPITEM_SELECTABLE, GetString(149, getS));
		}
	}
	else { // click on other
		table = players[0].info.table;
		if (table != -1)
		{
			if (players[0].info.seat < MAX_GAME_PLAYERS) {
				// if I am a player, I can invite others as long as they are either not at a table, or are
				//  at a table, but only watching.
				if (tables[table].GetNumPlayers() < MAX_GAME_PLAYERS &&
					(players[playerIndex].info.table == -1 || 
					(players[playerIndex].info.table != -1 && players[playerIndex].info.seat >= MAX_GAME_PLAYERS)))
				{
					inviteToPlay = 1;
				}
				if (players[playerIndex].info.table == -1 && 
					(tables[table].status == TS_PLAYING || tables[table].status == TS_DISPLAYED))
				{
					//Only players not at a table at all are invited to watch.
					if (tables[table].GetNumWatchers() < MAX_GAME_WATCHERS)
						inviteToWatch = 1;
				}
			}
		}
		
		if (!players[playerIndex].IsMonitorOrSuperUser())
			muteAndComplain = 3;
		
		if (IsTournamentRoom() && (players[0].IsMonitorOrReferee() || players[0].IsSuperUser() || players[0].IsTournamentAdministrator()))
			addToTournament = IS_A_CASINO_GAME ? 3 : 2;
		
		if (players[0].IsMonitorOrSuperUser())
			boot = 6;

		if(HasRatings())
			ratings = 2;
		
		tableOptions = players[playerIndex].info.table != -1;
		
		popup->SetNumItems(5 + inviteToPlay + inviteToWatch + addToTournament + muteAndComplain + boot + ratings);
		popup->playerID = players[playerIndex].info.playerID;
		popup->table = players[playerIndex].info.table;
		popup->seat = players[playerIndex].info.seat;
		
		popup->AddItem(0, POPITEM_DISPLAY_BOLD, players[playerIndex].info.login);
		popup->AddItem(0, POPITEM_DELIMITER, "");
		if (players[playerIndex].isSelected)
			popup->AddItem(POPID_ROOM_PRIVATE_CHAT, POPITEM_SELECTABLE, GetString(150, getS));
		else
			popup->AddItem(POPID_ROOM_PRIVATE_CHAT, POPITEM_SELECTABLE, GetString(151, getS));
		if (players[playerIndex].IsFriend())
			popup->AddItem(POPID_ROOM_FRIEND, POPITEM_SELECTABLE, GetString(152, getS));
		else
			popup->AddItem(POPID_ROOM_FRIEND, POPITEM_SELECTABLE, GetString(153, getS));
		popup->AddItem(POPID_ROOM_VIEW_PROFILE, POPITEM_SELECTABLE, GetString(154, getS));
		if (inviteToPlay)
			popup->AddItem(POPID_ROOM_INVITE_TO_PLAY, POPITEM_SELECTABLE, GetString(155, getS));
		if (inviteToWatch)
			popup->AddItem(POPID_ROOM_INVITE_TO_WATCH, POPITEM_SELECTABLE, GetString(156, getS));

		if (ratings)
		{
			popup->AddItem(0, POPITEM_DELIMITER, "");
			if(players[playerIndex].info.wins + players[playerIndex].info.losses +
				players[playerIndex].info.draws + players[playerIndex].info.quits)
			{
				sprintf(getS2, "%s %d", GetString(338, getS), players[playerIndex].info.rating);
				popup->AddItem(POPID_ROOM_RATINGS, POPITEM_SELECTABLE, getS2);
			}
			else
			{
				sprintf(getS2, "%s", GetString(372, getS));
				popup->AddItem(POPID_ROOM_RATINGS, POPITEM_DISPLAY_ONLY, getS2);
			}
		}
		
		if (addToTournament) {
			popup->AddItem(0, POPITEM_DELIMITER, "");
			if (players[playerIndex].IsInTournament())
				popup->AddItem(POPID_ROOM_TOURNAMENT_JOIN, POPITEM_SELECTABLE, GetString(157, getS));
			else
				popup->AddItem(POPID_ROOM_TOURNAMENT_JOIN, POPITEM_SELECTABLE, GetString(158, getS));
			if (addToTournament == 3) {
				MoneyToString(players[playerIndex].info.tournamentBankRoll, s);
				popup->AddItem(POPID_ROOM_TOURNAMENT_BANK, POPITEM_SELECTABLE, s);
			}
		}
		if (muteAndComplain) {
			popup->AddItem(0, POPITEM_DELIMITER, "");
			if (players[playerIndex].IsMuted())
				popup->AddItem(POPID_ROOM_MUTE, POPITEM_SELECTABLE, GetString(159, getS));
			else
				popup->AddItem(POPID_ROOM_MUTE, POPITEM_SELECTABLE, GetString(160, getS));
			popup->AddItem(POPID_ROOM_PLAYER_COMPLAINT, POPITEM_SELECTABLE, GetString(161, getS));
			if (boot) {
				popup->AddItem(0, POPITEM_DELIMITER, "");
				popup->AddItem(POPID_ROOM_GET_IP, POPITEM_SELECTABLE, GetString(162, getS));
				popup->AddItem(POPID_ROOM_GLOBAL_MUTE, POPITEM_SELECTABLE, GetString(163, getS));
				popup->AddItem(POPID_ROOM_GLOBAL_UNMUTE, POPITEM_SELECTABLE, GetString(164, getS));
				popup->AddItem(POPID_ROOM_BOOT, POPITEM_SELECTABLE, GetString(165, getS));
				popup->AddItem(POPID_ROOM_BAN_ONLINE, POPITEM_SELECTABLE, GetString(166, getS));
			}
		}
		
		if (ControlKeyDown())
			{ popup->ItemSelected(POPID_ROOM_PRIVATE_CHAT, 0, 0, 0); return FALSE; }
	}
	
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////

Boolean RoomClient::ClickedInTablesArea(sciEvent &event)
{
	return event.x > 13 && event.y > 77 && event.x < 397 && event.y < 294;
}

//////////////////////////////////////////////////////////////////////////////////////////

Boolean RoomChatEditBox::HandleEvent(sciEvent &event)
{
	char buf[300], getS[256];
	int defaultPushButton;
	
	if (event.claimed || roomClient->status != RCS_IN_ROOM)
		return FALSE;
	
	if (roomClient->popup->HandleEvent(event))
		return event.claimed;
	
	switch (event.type) {
		case MOUSE_DOWN:
			if (event.modifiers & RIGHT_MOUSE) {
				roomClient->popup->SetNumItems(1);
				roomClient->popup->AddItem(POPID_ROOM_PASTE_CHAT, POPITEM_SELECTABLE, GetString(297, getS));
				roomClient->popup->Posn(event.x, event.y);
				roomClient->popup->Show(this);
			}
			else
				return EditBox::HandleEvent(event);
			break;
		case KEY_PRESS:
			if (event.c == 13) {
				// if the chat push buttons are shown, click one of them
				if (roomClient->chatPushButtons[0]->IsNotHidden()) {
					defaultPushButton = roomClient->globalIniFile->ReadValue("ChatPushButtons", CHAT_PUSH_BUTTONS_PUBLIC);
					switch (defaultPushButton) {
						case CHAT_PUSH_BUTTONS_PUBLIC:
							roomClient->chatPushButtons[0]->DoClick();
							event.claimed = TRUE;
							
							return TRUE;
						
						case CHAT_PUSH_BUTTONS_PRIVATE:
							roomClient->chatPushButtons[1]->DoClick();
							event.claimed = TRUE;
							
							return TRUE;
					}
				}
				
				strcpy(buf, "rchat ");
				strnzcpy(&buf[strlen(buf)], (char *)text, 255);
				roomClient->DoChatCommand(NULL, buf);
				event.claimed = TRUE;
				SetText("");
			}
			else
				return EditBox::HandleEvent(event);
			break;
		
		default:
			return EditBox::HandleEvent(event);
	}
	
	return event.claimed;
}

////////////////////////////////////////////////////////////////////////////////////

Boolean RoomPlayersDisplayLineRect::HandleEvent(sciEvent &event)
{
	int playerIndex;
	
	if (roomClient->status < RCS_IN_ROOM)
		return FALSE;
	
	if (event.claimed)
		return FALSE;
	
	if (roomClient->popup->HandleEvent(event))
		return event.claimed;
	
	if (event.type != MOUSE_DOWN || !(event.modifiers & LEFT_MOUSE))
		return FALSE;
	
	if (!sciFeature::OnMe(event))
		return FALSE;
	
	event.claimed = TRUE;
	
	playerIndex = roomClient->playersDisplay->displayLines[index].playerIndex;
	
	if (!roomClient->AddPlayerPopupItems(playerIndex))
		return TRUE;
	
	roomClient->popup->Posn(GetX() - roomClient->popup->GetWidth(), GetY());
	roomClient->popup->Show(roomClient->playerListRect);
	
	return TRUE;
}

//////////

RoomPlayersDisplayLine::RoomPlayersDisplayLine()
{
	area = NULL;
	icon = NULL;
	name = NULL;
	tournamentMarker = NULL;
	ranking = NULL;
	tableStatus = NULL;
	tableNumber = NULL;
	selectedOverlay = NULL;
	mutedOverlay = NULL;
	divider = NULL;
}

RoomPlayersDisplayLine::~RoomPlayersDisplayLine()
{
	if (area) { delete area; area = NULL; }
	if (icon) { delete icon; icon = NULL; }
	if (name) { delete name; name = NULL; }
	if (tournamentMarker) { delete tournamentMarker; tournamentMarker = NULL; }
	if (ranking) { delete ranking; ranking = NULL; }
	if (tableStatus) { delete tableStatus; tableStatus = NULL; }
	if (tableNumber) { delete tableNumber; tableNumber = NULL; }
	if (selectedOverlay) { delete selectedOverlay; selectedOverlay = NULL; }
	if (mutedOverlay) { delete mutedOverlay; mutedOverlay = NULL; }
	if (divider) { delete divider; divider = NULL; }
}

void RoomPlayersDisplayLine::Init(int index)
{
	i18nStr getS;
	int width;
	
	type = PDL_EMPTY;
	playerIndex = -1;
	
	area = new RoomPlayersDisplayLineRect(index);
	area->Init();
	area->SetSize(176, 16);
	
	icon = new sciView();
	icon->ParamInit(200017, 0, 0, 501);
	
	name = new sciText();
	width = roomClient->HasRankings() ? 105 : 123;
	name->ParamInit(4, "", width, PLAYER_LINE_HEIGHT, 0, 0, 501, JUST_VERTICAL | JUST_LEFT);
	
	tournamentMarker = new sciText();
	tournamentMarker->ParamInit(2, GetString(167, getS), 10, PLAYER_LINE_HEIGHT, 0, 0, 501, JUST_VERTICAL | JUST_HORIZONTAL);
	tournamentMarker->SetColorMap(0, 0, 0, 128, 0, 0);
	
	ranking = new sciView();
	ranking->ParamInit(200017, 0, 0, 501);
	
	tableStatus = new sciView();
	tableStatus->ParamInit(200020, 0, 0, 501);
	
	tableNumber = new sciText(); // x = 578
	tableNumber->ParamInit(2, "", 28, PLAYER_LINE_HEIGHT, 0, 0, 501, JUST_VERTICAL | JUST_HORIZONTAL);
	
	selectedOverlay = new sciView();
	selectedOverlay->ParamInit(200021, 0, 0, 502);
	
	mutedOverlay = new sciView();
	mutedOverlay->ParamInit(200022, 0, 0, 502);
	
	divider = new sciView();
	divider->ParamInit(200018, 0, 0, 501);
}

void RoomPlayersDisplayLine::SetTypeAndPlayerIndex(int theType, int thePlayerIndex)
{
	type = theType;
	playerIndex = thePlayerIndex;
}

void RoomPlayersDisplayLine::Posn(int x, int y)
{
	area->Posn(x, y);
	icon->Posn(x, y);
	name->Posn(x + 25, y + 2);
	tournamentMarker->Posn(x + 25, y + 1);
	ranking->Posn(x, y);
	tableStatus->Posn(x, y);
	tableNumber->Posn(x + 149, y + 1);
	selectedOverlay->Posn(x + 1, y);
	mutedOverlay->Posn(x, y);
	divider->Posn(x + 1, y + 4);
}

void RoomPlayersDisplayLine::Show()
{
	char s[32];
	ROOMCLIENTPLAYER *player;
	
	area->Hide();
	icon->Hide();
	name->SetText("");
	name->Hide();
	tournamentMarker->Hide();
	ranking->Hide();
	tableStatus->Hide();
	tableNumber->SetText("");
	tableNumber->Hide();
	selectedOverlay->Hide();
	mutedOverlay->Hide();
	divider->Hide();
	
	switch (type) {
		case PDL_EMPTY:
			break;
		case PDL_PLAYER:
			area->Show();
			player = &roomClient->players[playerIndex];
			icon->SetCel(player->GetIconCel());
			icon->Show();
			if (roomClient->IsTournamentRoom()) {
				name->SetWidth(102);
				name->Posn(area->GetX() + 36, area->GetY() + 2);
			}
			name->SetTextToWidth(player->info.login);
			name->Show();
			if (player->IsInTournament())
				tournamentMarker->Show();
			if (roomClient->HasRankings()) {
				// TODO: set ranking view
			}
			else
				ranking->Hide();
			if (player->info.table == -1) {
				tableStatus->SetCel((player->info.invitedTable == -1) ? 0 : 1);
				tableStatus->Show();
			}
			else {
				sprintf(s, "%d", player->info.table + 1);
				tableNumber->SetText(s);
				tableNumber->Show();
			}
			if (player->info.playerID != roomClient->GetMyID()) {
				if (player->isSelected)
					selectedOverlay->Show();
				if (player->IsMuted())
					mutedOverlay->Show();
			}
			break;
		case PDL_FRIENDS_DIVIDER:
			divider->SetCel(1);
			divider->Show();
			break;
		case PDL_MONITORS_DIVIDER:
			divider->SetCel(0);
			divider->Show();
			break;
		case PDL_DIVIDER:
			divider->SetCel(2);
			divider->Show();
			break;
		case PDL_TDS_DIVIDER:
			divider->SetCel(3);
			divider->Show();
			break;
		case PDL_TPLAYERS_DIVIDER:
			divider->SetCel(4);
			divider->Show();
			break;
		case PDL_TWATCHERS_DIVIDER:
			divider->SetCel(4);
			divider->Show();
			break;
	}
}

///////////

void RoomPlayersDisplay::Init()
{
	int i;
	
	for (i = 0 ; i < NUM_VISIBLE_PLAYER_LINES ; i++) {
		displayLines[i].Init(i);
		displayLines[i].Posn(431, 87 + i * PLAYER_LINE_HEIGHT);
	}
}

int RoomPlayersDisplay::GetNumLines()
{
	int i, friends = 0, monitors = 0, tournamentDirectors = 0, others = 0;
	
	// we want to return numPlayers plus some number of divider lines
	
	// if any player is a friend, add one line for the -FRIENDS- divider
	// if any player is a monitor (and not a friend), add one line for the the -MONITORS- divider
	// if any player is a tournament director (and not a friend or a monitor), add one line for the the -TOURNAMENT DIRECTORS- divider
	// if there is any other player, add one line for the --------- divider
	
	for (i = 1 ; i < roomClient->numPlayers ; i++) {
		if (roomClient->players[i].IsFriend())
			friends = 1;
		else {
			if (roomClient->players[i].IsMonitor())
				monitors = 1;
			else {
				if (roomClient->players[i].IsRefereeOrTA())
					tournamentDirectors = 1;
				else
					others = 1;
			}
		}
	}
	
	return roomClient->numPlayers + friends + monitors + tournamentDirectors + others;
}

void RoomPlayersDisplay::DisplayLines(int fromLineNum)
{
	Boolean didFriends = FALSE, didMonitors = FALSE, didTDs = FALSE, didOthers = FALSE;
	int i, totalLines, playersIndex = 0, displayLine = 0;
	
	totalLines = GetNumLines();
	
	if (fromLineNum == -1) {
		fromLineNum = roomClient->playersScrollBox->GetFirstVisibleItem();
		if ((totalLines - fromLineNum) < NUM_VISIBLE_PLAYER_LINES)
			fromLineNum = totalLines - NUM_VISIBLE_PLAYER_LINES;
		if (fromLineNum < 0)
			fromLineNum = 0;
	}
	
	// we assume the players list is sorted as follows:
	// player at this computer
	// friends, sorted alphabetically
	// monitors (who are not also friends), sorted alphabetically
	// tournament directors (who are not also friends or monitors), sorted alphabetically
	// regular players, sorted alphabetically
	// muted players (whose friends flag is ignored), sorted alphabetically
	
	for (i = 0 ; i < totalLines && displayLine < NUM_VISIBLE_PLAYER_LINES ; i++) {
		// player at this computer
		if (i == 0) {
			if (i >= fromLineNum)
				displayLines[displayLine++].SetTypeAndPlayerIndex(PDL_PLAYER, playersIndex);
			playersIndex++;
			continue;
		}
		
		// friends
		if (roomClient->players[playersIndex].IsFriend()) {
			if (!didFriends) {
				if (i >= fromLineNum)
					displayLines[displayLine++].SetTypeAndPlayerIndex(PDL_FRIENDS_DIVIDER, -1);
				didFriends = TRUE;
				continue;
			}
			if (i >= fromLineNum)
				displayLines[displayLine++].SetTypeAndPlayerIndex(PDL_PLAYER, playersIndex);
			playersIndex++;
			continue;
		}
		
		// monitors
		if (roomClient->players[playersIndex].IsMonitor()) {
			if (!didMonitors) {
				if (i >= fromLineNum)
					displayLines[displayLine++].SetTypeAndPlayerIndex(PDL_MONITORS_DIVIDER, -1);
				didMonitors = TRUE;
				continue;
			}
			if (i >= fromLineNum)
				displayLines[displayLine++].SetTypeAndPlayerIndex(PDL_PLAYER, playersIndex);
			playersIndex++;
			continue;
		}
		
		// tournament directors
		if (roomClient->players[playersIndex].IsRefereeOrTA()) {
			if (!didTDs) {
				if (i >= fromLineNum)
					displayLines[displayLine++].SetTypeAndPlayerIndex(PDL_TDS_DIVIDER, -1);
				didTDs = TRUE;
				continue;
			}
			if (i >= fromLineNum)
				displayLines[displayLine++].SetTypeAndPlayerIndex(PDL_PLAYER, playersIndex);
			playersIndex++;
			continue;
		}
		
		// other players
		if (!didOthers) {
			if (i >= fromLineNum)
				displayLines[displayLine++].SetTypeAndPlayerIndex(PDL_DIVIDER, -1);
			didOthers = TRUE;
			continue;
		}
		if (i >= fromLineNum)
			displayLines[displayLine++].SetTypeAndPlayerIndex(PDL_PLAYER, playersIndex);
		playersIndex++;
	}
	
	for ( ; displayLine < NUM_VISIBLE_PLAYER_LINES ; displayLine++)
		displayLines[displayLine].SetTypeAndPlayerIndex(PDL_EMPTY, -1);
	
	for (i = 0 ; i < NUM_VISIBLE_PLAYER_LINES ; i++)
		displayLines[i].Show();
	
	if (totalLines > NUM_VISIBLE_PLAYER_LINES)
		roomClient->playersScrollBox->Enable();
	else
		roomClient->playersScrollBox->Disable();
}

////////////////////////////////////////////////////////////////////////////////////

Boolean RoomTablesDisplayPlayer::HandleEvent(sciEvent &event)
{
	if (roomClient->status < RCS_IN_ROOM)
		return FALSE;
	
	if (event.claimed)
		return FALSE;
	
	if (roomClient->popup->HandleEvent(event))
		return event.claimed;
	
	if (event.type != MOUSE_DOWN || !(event.modifiers & LEFT_MOUSE))
		return FALSE;
	
	if (!roomClient->ClickedInTablesArea(event))
		return FALSE;
	
	if (!IsOnMe(event.x, event.y))
		return FALSE;
	
	if (playerID == PLAYER_ID_AI)
		return FALSE;
	
	event.claimed = TRUE;
	
	if (!roomClient->AddPlayerPopupItems(roomClient->GetPlayerIndexFromID(playerID)))
		return TRUE;
	
	roomClient->popup->Posn(GetX() + 40, GetY() + 20);
	roomClient->popup->Show(this);
	
	return TRUE;
}

Boolean RoomTablesDisplayChair::HandleEvent(sciEvent &event)
{
	return HandleEventEx(event, FALSE);
}

Boolean RoomTablesDisplayChair::HandleEventEx(sciEvent &event, Boolean clickedOnName)
{
	Boolean atATable = FALSE, atThisTable = FALSE, watching = FALSE, gameHasStarted = FALSE;
	Boolean inThisChair = FALSE, currentlyInAGame = FALSE;
	char s[256];
	i18nStr getS, getS2;
	int joinOption, watchersOption, join = 0, watch = 0, i, x, y;
	
	if (roomClient->status < RCS_IN_ROOM)
		return FALSE;
	
	if (event.claimed)
		return FALSE;
	
	if (roomClient->popup->HandleEvent(event))
		return event.claimed;
	
	if (event.type != MOUSE_DOWN || !(event.modifiers & LEFT_MOUSE))
		return FALSE;
	
	if (!roomClient->ClickedInTablesArea(event))
		return FALSE;
	
	if (!(clickedOnName || sciFeature::OnMe(event)))
		return FALSE;
	
	event.claimed = TRUE;
	
	atATable = roomClient->players[0].info.table != -1;
	atThisTable = roomClient->players[0].info.table == parent->tableIndex;
	inThisChair = roomClient->players[0].info.seat == parent->seatIndex;
	watching = atATable && roomClient->players[0].info.seat >= MAX_GAME_PLAYERS;
	gameHasStarted = roomClient->tables[parent->tableIndex].status == TS_PLAYING ||
					 roomClient->tables[parent->tableIndex].status == TS_DISPLAYED;
	currentlyInAGame = atATable && !watching && (roomClient->tables[roomClient->players[0].info.table].status == TS_PLAYING ||
					   roomClient->tables[roomClient->players[0].info.table].status == TS_DISPLAYED);
	joinOption = roomClient->tables[parent->tableIndex].joinOption;
	watchersOption = roomClient->tables[parent->tableIndex].watchersOption;
	
	
	//If the player is currently sitting at a table that is not the current table, and the current tables requires 
	// permission to join, display a dialog box first saying that they must leave their current table.  This only
	// goes if they are seated at a table, not just a watcher.
	if(atATable && !atThisTable && joinOption == TJO_BY_APPROVAL && !watching)
	{
		sprintf(s, GetString(168, getS), roomClient->players[0].info.table + 1);
		roomClient->ShowModelessMessage(0, s, GetString(26, getS), MB_OK);
		
		return TRUE;
	}

	if(atThisTable && inThisChair)
	{
		roomClient->AddPlayerPopupItems(roomClient->GetPlayerIndexFromID(roomClient->GetMyID()));
		if(clickedOnName)
		{
			x = parent->nameTagClick->GetX(); y = parent->nameTagClick->GetY();
		}
		else
		{
			x = GetX(); y = GetY();
		}
		roomClient->popup->Posn(x + 40, y + 20);
		if(!ControlKeyDown()) //add to private chat by default if CTRL key down
			if(clickedOnName)
				roomClient->popup->Show(parent->nameTagClick);
			else
				roomClient->popup->Show(this);
		return TRUE;
	}
	//Now, check all the other players to see if this seat is taken
	for (i = 0 ; i < roomClient->numPlayers ; i++)
	{
		if (roomClient->players[i].info.table == parent->tableIndex && roomClient->players[i].info.seat == parent->seatIndex)
		{
			roomClient->AddPlayerPopupItems(i);
			if(clickedOnName)
			{
				x = parent->nameTagClick->GetX(); y = parent->nameTagClick->GetY();
			}
			else
			{
				x = GetX(); y = GetY();
			}

			roomClient->popup->Posn(x + 40, y + 20);
			
			if(!ControlKeyDown()) //add to private chat by default if CTRL key down
			{
				if(clickedOnName)
					roomClient->popup->Show(parent->nameTagClick);
				else
					roomClient->popup->Show(this);
			}

			return TRUE;
		}
	}

	//Once I have started playing a game, I cannot leave without explicitly doing so.
	if(currentlyInAGame)
	{
		sprintf(s, GetString(169, getS), roomClient->players[0].info.table + 1);
		roomClient->ShowModelessMessage(0, s, GetString(26, getS), MB_OK);
		return TRUE;
	}

	if ((joinOption != TJO_BY_INVITATION || roomClient->players[0].IsMonitorOrSuperUser()) && !watching)
		join = 1;
	//If this table is by invitation only, but I'm already at the table (not as a watcher), let me move around.
	if(joinOption == TJO_BY_INVITATION && atThisTable && !watching)
		join = 1;
	//If I'm currently watching, and needed special permission to watch, then I can sit.
	if (atThisTable && ((watching && watchersOption != TWO_OPEN) || (watching && joinOption == TJO_OPEN)))
		join = 1;
	else
		if (gameHasStarted) {
			if (watchersOption != TWO_BY_INVITATION || roomClient->players[0].IsMonitorOrSuperUser())
				watch = 1;
			if (roomClient->tables[parent->tableIndex].GetNumWatchers() >= MAX_GAME_WATCHERS)
				watch = 0;
		}
	
	if (!join && !watch)
		return TRUE;
	
	roomClient->popup->SetNumItems(join + watch);
	roomClient->popup->table = parent->tableIndex;
	roomClient->popup->seat = parent->seatIndex;
	
	if (join)
	{
		if(atThisTable)
			roomClient->popup->AddItem(POPID_ROOM_JOIN, POPITEM_SELECTABLE, GetString(170, getS));
		else
		{
			switch (joinOption) {
				case TJO_OPEN:
				case TJO_BY_INVITATION: // this case for moderators
					roomClient->popup->AddItem(POPID_ROOM_JOIN, POPITEM_SELECTABLE,
											   (roomClient->tables[parent->tableIndex].status == TS_PLAYING) ? GetString(171, getS) : GetString(170, getS2));
					break;
				case TJO_BY_APPROVAL:
					if(roomClient->players[0].IsMonitorOrSuperUser()) //monitors and superusers get to always sit.
						roomClient->popup->AddItem(POPID_ROOM_JOIN, POPITEM_SELECTABLE,
												   (roomClient->tables[parent->tableIndex].status == TS_PLAYING) ? GetString(171, getS) : GetString(170, getS2));
					else
						roomClient->popup->AddItem(POPID_ROOM_JOIN, POPITEM_SELECTABLE, GetString(172, getS));
					break;
			}
		}
	}
	if (watching && !join)
		roomClient->popup->AddItem(POPID_ROOM_LEAVE_TABLE, POPITEM_SELECTABLE, GetString(173, getS));
	else
		if (watch)
			switch (watchersOption) {
				case TWO_OPEN:
				case TWO_BY_INVITATION: // this case for moderators
					roomClient->popup->AddItem(POPID_ROOM_WATCH, POPITEM_SELECTABLE, GetString(174, getS));
					break;
				case TWO_BY_APPROVAL:
					if(roomClient->players[0].IsMonitorOrSuperUser()) //monitors and superusers get to always watch.
						roomClient->popup->AddItem(POPID_ROOM_WATCH, POPITEM_SELECTABLE, GetString(174, getS));
					else
						roomClient->popup->AddItem(POPID_ROOM_WATCH, POPITEM_SELECTABLE, GetString(175, getS));
					break;
			}
	
	// special case: skip popup step when sitting at an open table
	
	if (join && (joinOption == TJO_OPEN || atThisTable))
		roomClient->popup->ItemSelected(POPID_ROOM_JOIN, 0, 0, 0);
	else {
		if(clickedOnName)
		{
			x = parent->nameTagClick->GetX(); y = parent->nameTagClick->GetY();
		}
		else
		{
			x = GetX(); y = GetY();
		}
		roomClient->popup->Posn(x + 40, y + 20);
		if(!ControlKeyDown()) //add to private chat by default if CTRL key down
			if(clickedOnName)
				roomClient->popup->Show(parent->nameTagClick);
			else
				roomClient->popup->Show(this);
	}
	
	return TRUE;
}

Boolean RoomTablesDisplayTable::HandleEvent2(sciEvent &event, sciFeature *clickedObject, int theTableIndex)
{
	Boolean atATable = FALSE, atThisTable = FALSE, watching = FALSE, gameHasStarted = FALSE;
	char s[256];
	i18nStr getS;
	int watchersOption, watch = 0, tableOptions = 0, divider = 0, ratings = 0;
	
	if (roomClient->status < RCS_IN_ROOM)
		return FALSE;
	
	if (event.claimed)
		return FALSE;
	
	if (roomClient->popup->HandleEvent(event))
		return event.claimed;
	
	if (event.type != MOUSE_DOWN || !(event.modifiers & LEFT_MOUSE))
		return FALSE;
	
	if (!roomClient->ClickedInTablesArea(event))
		return FALSE;
	
	if (!clickedObject->OnMe(event))
		return FALSE;
	
	event.claimed = TRUE;
	
	atATable = roomClient->players[0].info.table != -1;
	atThisTable = roomClient->players[0].info.table == theTableIndex;
	watching = atATable && roomClient->players[0].info.seat >= MAX_GAME_PLAYERS;
	gameHasStarted = roomClient->tables[theTableIndex].status == TS_PLAYING ||
					 roomClient->tables[theTableIndex].status == TS_DISPLAYED;
	
	if (atATable && !atThisTable) {
		if (watching)
			sprintf(s, GetString(176, getS), roomClient->players[0].info.table + 1);
		else
			sprintf(s, GetString(177, getS), roomClient->players[0].info.table + 1);
		roomClient->ShowModelessMessage(0, s, GetString(178, getS), MB_OK);
		
		return TRUE;
	}
	
	watchersOption = roomClient->tables[theTableIndex].watchersOption;
	
	if (watching)
		watch = 1;
	else
		if (!atThisTable && gameHasStarted) {
			if (watchersOption != TWO_BY_INVITATION)
				watch = 1;
			if (roomClient->players[0].IsMonitorOrSuperUser())
				watch = 1;
		if (roomClient->tables[theTableIndex].GetNumWatchers() >= MAX_GAME_WATCHERS)
				watch = 0;
		}
	
	if (atThisTable && !watching)
		tableOptions = 1; // TableOptions...
	
	if (!watch && !tableOptions)
		return TRUE;
	
	if (watch && tableOptions)
		divider = 1;

	if(roomClient->HasRatings() && (tableOptions || watching)) //Ratings Optioins... only valid if somehow at table
		ratings = 2;
	
	roomClient->popup->SetNumItems(watch + divider + tableOptions + ratings);
	roomClient->popup->table = theTableIndex;
	roomClient->popup->seat = -1;
	
	if (watch) {
		if (watching)
			roomClient->popup->AddItem(POPID_ROOM_LEAVE_TABLE, POPITEM_SELECTABLE, GetString(173, getS));
		else if(roomClient->players[0].IsMonitorOrSuperUser())
			roomClient->popup->AddItem(POPID_ROOM_WATCH, POPITEM_SELECTABLE, GetString(174, getS));
		else
			switch (watchersOption) {
				case TWO_OPEN:
					roomClient->popup->AddItem(POPID_ROOM_WATCH, POPITEM_SELECTABLE, GetString(174, getS));
					break;
				case TWO_BY_APPROVAL:
					roomClient->popup->AddItem(POPID_ROOM_WATCH, POPITEM_SELECTABLE, GetString(175, getS));
					break;
			}
	}
	if (divider)
		roomClient->popup->AddItem(0, POPITEM_DELIMITER, "");
	if (tableOptions)
		roomClient->popup->AddItem(POPID_ROOM_TABLE_OPTIONS, POPITEM_SELECTABLE, GetString(143, getS));
	if (ratings)
	{
		roomClient->popup->AddItem(0, POPITEM_DELIMITER, "");
		roomClient->popup->AddItem(POPID_ROOM_RATING_OPTIONS, POPITEM_SELECTABLE, GetString(360, getS));
	}
	roomClient->popup->Posn(event.x, event.y);
	roomClient->popup->Show(clickedObject);
	
	return TRUE;
}

Boolean RoomTablesDisplayTable::HandleEvent(sciEvent &event)
{
	return HandleEvent2(event, this, tableIndex);
}

Boolean RoomTablesDisplayDescription::HandleEvent(sciEvent &event)
{
	if (displayTable)
		return displayTable->HandleEvent2(event, this, tableIndex);
	
	return FALSE;
}

Boolean RoomTablesDisplayWatchers::HandleEvent(sciEvent &event)
{
	Boolean watchersMuted, atATable = FALSE, atThisTable = FALSE, watching = FALSE, gameHasStarted = FALSE;
	i18nStr getS, getS2;
	int i, n, watchersOption, watch = 0, delimiter = 0;
	GameState *gameState;
	ROOMCLIENTPLAYER *player;
	
	if (roomClient->status < RCS_IN_ROOM)
		return FALSE;
	
	if (event.claimed)
		return FALSE;
	
	if (roomClient->popup->HandleEvent(event))
		return event.claimed;
	
	if (event.type != MOUSE_DOWN || !(event.modifiers & LEFT_MOUSE))
		return FALSE;
	
	if (!roomClient->ClickedInTablesArea(event))
		return FALSE;
	
	if (!sciFeature::OnMe(event))
		return FALSE;
	
	if (tableIndex == -1)
		return FALSE;
	
	event.claimed = TRUE;
	
	atATable = roomClient->players[0].info.table != -1;
	atThisTable = roomClient->players[0].info.table == tableIndex;
	watching = atThisTable && roomClient->players[0].info.seat >= MAX_GAME_PLAYERS;
	watchersOption = roomClient->tables[tableIndex].watchersOption;
	watchersMuted = roomClient->tables[tableIndex].muteWatchers;
	n = roomClient->tables[tableIndex].GetNumWatchers();
	gameHasStarted = roomClient->tables[tableIndex].status == TS_PLAYING ||
					 roomClient->tables[tableIndex].status == TS_DISPLAYED;
	
	if (watching)
		watch = 1;
	else
		if (!atATable && gameHasStarted) {
			if (watchersOption != TWO_BY_INVITATION)
				watch = 1;
			if (roomClient->players[0].IsMonitorOrSuperUser())
				watch = 1;
			if (n >= MAX_GAME_WATCHERS)
				watch = 0;
		}
	
	if ((n + watch) == 0)
		return TRUE;
	
	if (watch && n > 0)
		delimiter = 1;
	
	roomClient->popup->SetNumItems(2 + watch + delimiter + n);
	roomClient->popup->table = tableIndex;
	roomClient->popup->seat = -1;
	
	roomClient->popup->AddItem(0, POPITEM_DISPLAY_BOLD, watchersMuted ? GetString(179, getS) : GetString(180, getS2));
	roomClient->popup->AddItem(0, POPITEM_DELIMITER, "");
	
	if (watch) {
		if (watching)
			roomClient->popup->AddItem(POPID_ROOM_LEAVE_TABLE, POPITEM_SELECTABLE, GetString(181, getS));
		else if(roomClient->players[0].IsMonitorOrSuperUser())
			roomClient->popup->AddItem(POPID_ROOM_WATCH, POPITEM_SELECTABLE, GetString(182, getS));
		else
			switch (watchersOption) {
				case TWO_OPEN:
					roomClient->popup->AddItem(POPID_ROOM_WATCH, POPITEM_SELECTABLE, GetString(182, getS));
					break;
				case TWO_BY_APPROVAL:
					roomClient->popup->AddItem(POPID_ROOM_WATCH, POPITEM_SELECTABLE, GetString(183, getS));
					break;
			}
	}
	if (delimiter)
		roomClient->popup->AddItem(0, POPITEM_DELIMITER, "");
	
	gameState = &roomClient->tables[tableIndex].gameState;
	
	for (i = 0 ; i < MAX_GAME_WATCHERS ; i++)
		if (gameState->watcherIDs[i] != -1) {
			player = roomClient->GetPlayerFromID(gameState->watcherIDs[i]);
			if (player)
				roomClient->popup->AddItem(POPID_ROOM_WATCHERS_CLICK, POPITEM_SELECTABLE, player->info.login, player->info.playerID);
		}
	
	roomClient->popup->Posn(GetX(), GetY() - roomClient->popup->GetHeight());
	roomClient->popup->Show(this);
	
	return TRUE;
}

Boolean RoomTablesDisplayStartButton::HandleEvent(sciEvent &event)
{
	if (roomClient->status < RCS_IN_ROOM)
		return FALSE;
	
	if (event.claimed)
		return FALSE;
	
	if (roomClient->popup->HandleEvent(event))
		return event.claimed;
	
	if (event.type == MOUSE_DOWN && (event.modifiers & LEFT_MOUSE))
		if (!roomClient->ClickedInTablesArea(event))
			return FALSE;
	
	return sciButton::HandleEvent(event);
}

///////////////////////////////////////////////////////////////////////////////////////////////

RoomTablesDisplayPlayer::RoomTablesDisplayPlayer()
{
	playerID = -1;
	
	faceMakerView = NULL;
}

RoomTablesDisplayPlayer::~RoomTablesDisplayPlayer() 
{ 
	if (faceMakerView) 
		{ delete faceMakerView; faceMakerView = NULL;}
}

bool RoomTablesDisplayPlayer::IsOnMe(int eventX, int eventY)
{
	if (!sciFeature::OnMe(eventX, eventY))
		return FALSE;
	
	return (eventX - x) < (HOYLE_CHARACTER_WIDTH / 2) && (eventY - y) < (HOYLE_CHARACTER_HEIGHT / 2);
}

void RoomTablesDisplayPlayer::Show(int thePlayerID)
{
	playerID = thePlayerID;
	
	sciView::Show();
}

////////////////////////////////////////////////////////////////////////////////////////////////

RoomTablesDisplaySeat::RoomTablesDisplaySeat()
{
	tableIndex = -1;
	seatIndex = -1;
	behindTable = TRUE;
	chairAngle = RCA_CENTER;
	
	chair = NULL;
	player = NULL;
//	player->faceMakerView = NULL;
	nameTag = NULL;
	nameLabel = NULL;
	nameTagClick = NULL;
}

RoomTablesDisplaySeat::~RoomTablesDisplaySeat()
{
	if (chair) { delete chair; chair = NULL; }
	if (player) { delete player; player = NULL; }
	if (nameTag) { delete nameTag; nameTag = NULL; }
	if (nameLabel) { delete nameLabel; nameLabel = NULL; }
	if (nameTagClick) { delete nameTagClick; nameTagClick = NULL; }
}

void RoomTablesDisplaySeat::Init(int theSeatIndex)
{
	seatIndex = theSeatIndex;
	
	if (seatIndex < 0 || seatIndex >= MAX_GAME_PLAYERS) {
		assert(FALSE);
		seatIndex = 0;
	}
	
	roomClient->InitTablesDisplaySeat(this);

	// the room face is scaled down from the game face
	player->faceMakerView = new FaceMakerFace();
	player->faceMakerView->Init(pApp->pMainPlane, NULL);
	player->faceMakerView->SetPri(115);
	player->faceMakerView->Posn(0, 0);
	player->faceMakerView->SetHalfSize(true, ST_BILINEAR);

	chair->parent = this;
	nameTag->SetPri(110);
	nameLabel->SetPri(110);
	nameTagClick = new NameTagClickPad(this);
	nameTagClick->plane = pApp->pMainPlane;
	nameTagClick->SetPri(101);
	nameTagClick->SetView(200013);
	nameTagClick->Init();
}

Boolean NameTagClickPad::HandleEvent(sciEvent &event)
{
	if (!sciFeature::OnMe(event))
		return FALSE;

	return parent->chair->HandleEventEx(event, TRUE);
}

Boolean RoomTablesDisplaySeat::PlayerInSeatFacesRight()
{
	return roomClient->PlayerInSeatFacesRight(this);
}

void RoomTablesDisplaySeat::Posn(int x, int y)
{
	chair->Posn(x, y + 1);
	player->Posn(x, y + 1);
	player->faceMakerView->Posn(x, y + 1);

	nameTag->Posn(x - 6, y + 38);
	nameLabel->Posn((x - 6) + 7, y + 38 + 4);
	nameTagClick->Posn(x - 6, y + 38);
}

void RoomTablesDisplaySeat::Show(int theTableIndex)
{
	i18nStr getS;
	int playerID, joinOption;
	ROOMCLIENTPLAYER *p;
	GameState *gameState;
	
	tableIndex = theTableIndex;
	
	gameState = &roomClient->tables[tableIndex].gameState;
	joinOption = roomClient->tables[tableIndex].joinOption;
	playerID = gameState->players[seatIndex].playerID;
	nameTagClick->Show();

	if (playerID == PLAYER_ID_VOID) {
		chair->Hide();
		player->Hide();
		player->faceMakerView->Hide();
		nameTag->Hide();
		nameLabel->Hide();
		nameTagClick->Hide();
		return;
	}
	else if (playerID == PLAYER_ID_EMPTY || playerID == PLAYER_ID_AI) {
		chair->Show(tableIndex, seatIndex);
		if (playerID == PLAYER_ID_AI) {
			player->SetView(200014);
			player->SetCel(0);
			player->SetAlpha(255);
			player->Show(playerID);
			player->SetHalfSize(false);
		}
		else
		{
			player->Hide();
		}
			
		player->faceMakerView->Hide();

		switch (joinOption) {
			case TJO_OPEN: nameLabel->SetTextToWidth(GetString(184, getS)); break;
			case TJO_BY_APPROVAL: nameLabel->SetTextToWidth(GetString(185, getS)); break;
			default:
			case TJO_BY_INVITATION: nameLabel->SetTextToWidth(GetString(186, getS)); break;
		}
		nameTag->SetCel(0);
	}
	else {
		chair->Hide();
		p = roomClient->GetPlayerFromID(playerID);
		if (p == NULL)
			{ assert(FALSE); return; }

#if defined(SCIVIEW_SCALING)
		player->faceMakerView->faceInfo = p->info.faceInfo;
		
		/* THIS IS NO LONGER CORRECT BECAUSE INDIVIDUAL FEATURES ARE NO LONGER SCALED
		// scale facemaker face piece offsets
		for (i = 0 ; i < FM_MAX_FEATURES ; i++) {
			player->faceMakerView->faceInfo.xOffsets[i] /= 2;
			player->faceMakerView->faceInfo.yOffsets[i] /= 2;
		}
		*/
		
		if (player->faceMakerView->faceInfo.featureViews[0] != 0 && pApp->pResourceManager->ResourceIsAvailable(RES_TSPRITE, 1001001)) {
			player->faceMakerView->Show();
			player->SetAlpha(0);
		}
		else {
			player->SetAlpha(255);
			player->faceMakerView->Hide();
		}

		if (pApp->pResourceManager->ResourceIsAvailable(RES_TSPRITE, roomClient->GetPlayerView(p->info.shadow))) {
			player->SetView(roomClient->GetPlayerView(p->info.shadow));
			player->SetCel(roomClient->GetPlayerViewCel(p->info.shadow, !PlayerInSeatFacesRight()));
		}
		else {
			player->SetView(200039); // revert to guaranteed art
			player->SetCel(PlayerInSeatFacesRight() ? 0 : 1);
		}
		player->SetHalfSize(true, ST_BILINEAR);
#else
		if (pApp->pResourceManager->ResourceIsAvailable(RES_TSPRITE, 200015)) {
			player->SetView(200015); // use full art, if it's available
			player->SetCel(roomClient->GetPlayerViewCel(p->info.shadow, !PlayerInSeatFacesRight()));
		}
		else {
			player->SetView(200040); // revert to guaranteed art
			player->SetCel(PlayerInSeatFacesRight() ? 0 : 1);
		}
#endif

		player->Show(playerID);
		nameLabel->SetTextToWidth(p->info.login);
		nameTag->SetCel((p->info.flags & UF_READY_TO_PLAY) ? 1 : 0);
	}
	
	if (roomClient->tables[tableIndex].status == TS_PLAYING ||
		roomClient->tables[tableIndex].status == TS_DISPLAYED)
		nameTag->SetCel(2);
	
	nameTag->Show();
	nameLabel->Show();
	nameTagClick->Show();
}

////////////////////////////////////////////////////////////////////////////////////////////////

RoomTablesDisplayWatchers::RoomTablesDisplayWatchers()
{
	tableIndex = -1;
	
	text = NULL;
	muteText = NULL;
}

RoomTablesDisplayWatchers::~RoomTablesDisplayWatchers()
{
	if (text) { delete text; text = NULL; }
	if (muteText) { delete muteText; muteText = NULL; }
}

void RoomTablesDisplayWatchers::Init()
{
	i18nStr getS;
	
	sciView::Init();
	
	text = new sciText();
	text->ParamInit(4, "", 30, 20, 0, 0, 0, JUST_VERTICAL | JUST_HORIZONTAL);
#ifndef ROOM_WATCHER_NUMBER_IN_BLACK
	text->SetColorMap(0, 0, 0, 220, 220, 220);
#endif
	muteText = new sciText();
	muteText->ParamInit(4, GetString(205, getS), 40, 20, 0, 0, 0, JUST_VERTICAL | JUST_HORIZONTAL);
#ifndef ROOM_WATCHER_NUMBER_IN_BLACK
	muteText->SetColorMap(0, 0, 0, 220, 220, 220);
#endif
}

void RoomTablesDisplayWatchers::SetPri(int newPri)
{
	sciView::SetPri(newPri);
	
	if (text)
		text->SetPri(newPri + 1);
	if (muteText)
		muteText->SetPri(newPri + 1);
}

void RoomTablesDisplayWatchers::Posn(int newX, int newY)
{
	sciView::Posn(newX, newY);
	
	if (text)
		text->Posn(newX - 1, newY - 1);
	if (muteText)
		muteText->Posn(newX - 6, newY + 11);
}

void RoomTablesDisplayWatchers::Show(int theTableIndex)
{
	char s[20];
	int n;
	
	tableIndex = theTableIndex;
	
	if (roomClient->tables[tableIndex].status != TS_PLAYING &&
		roomClient->tables[tableIndex].status != TS_DISPLAYED) {
		sciView::Hide();
		if (text)
			text->Hide();
		if (muteText)
			muteText->Hide();
		
		return;
	}
	
	sciView::Show();
	
	if (text) {
		n = roomClient->tables[tableIndex].GetNumWatchers();
		if (n == 0)
			text->Hide();
		else {
			sprintf(s, "%d", n);
			text->Show();
			text->SetText(s);
			if (roomClient->tables[tableIndex].muteWatchers)
				muteText->Show();
			else
				muteText->Hide();
		}
	}
}

void RoomTablesDisplayWatchers::Hide()
{
	sciView::Hide();
	
	if (text)
		text->Hide();
	if (muteText)
		muteText->Hide();
}

////////////////////////////////////////////////////////////////////////////////////////////////

RoomTablesDisplayDescription::RoomTablesDisplayDescription()
{
	tableIndex = -1;
	
	text1 = NULL;
	text2 = NULL;
	displayTable = NULL;
}

RoomTablesDisplayDescription::~RoomTablesDisplayDescription()
{
	if (text1) { delete text1; text1 = NULL; }
	if (text2) { delete text2; text2 = NULL; }
}

void RoomTablesDisplayDescription::Init()
{
	sciRect::Init();
	
	SetWidth(116);
	SetHeight(70);
	
	SetInnerColor(255, 248, 185);
	SetAlpha(128);
	
	text1 = new sciText();
	text1->SetLineSpacing(12);
	text1->ParamInit(4, "", 112, 30, 0, 0, 0, JUST_VERTICAL | JUST_HORIZONTAL);
	
	text2 = new sciText();
	text2->SetLineSpacing(12);
	text2->ParamInit(4, "", 112, 40, 0, 0, 0, JUST_VERTICAL | JUST_HORIZONTAL);
}

void RoomTablesDisplayDescription::SetPri(int newPri)
{
	sciRect::SetPri(newPri);
	
	if (text1)
		text1->SetPri(newPri + 1);
	if (text2)
		text2->SetPri(newPri + 1);
}

void RoomTablesDisplayDescription::Posn(int newX, int newY)
{
	sciRect::Posn(newX, newY);
	
	if (text1)
		text1->Posn(newX + 2, newY);
	if (text2)
		text2->Posn(newX + 2, newY + 30);
}

void RoomTablesDisplayDescription::Show(int theTableIndex, RoomTablesDisplayTable *theDisplayTable)
{
	char s[MAX_TABLE_DESCRIPTION];
	
	s[0] = '\0';

	tableIndex = theTableIndex;
	displayTable = theDisplayTable;
	
	if (roomClient->tables[tableIndex].status == TS_EMPTY) {
		sciRect::Hide();
		if (text1)
			text1->Hide();
		if (text2)
			text2->Hide();
		
		return;
	}
	
	sciRect::Show();
	
	if (text1) {
		roomClient->tables[tableIndex].GetDescription(s);
		text1->SetText(s);
		text1->Show();
	}
	
	if (text2) {
		strnzcpy(s, roomClient->tables[tableIndex].description, MAX_TABLE_DESCRIPTION - 1);
		if (game->IsChatFiltered())
			roomClient->dirtyWordFilter->UpdateString(s, MAX_TABLE_DESCRIPTION - 1);
		text2->SetText(s);
		while (text2->CountLines() > 3 && s[0]) {
			s[strlen(s) - 1] = 0;
			text2->SetText(s);
		}
		text2->Show();
	}
}

void RoomTablesDisplayDescription::Hide()
{
	sciRect::Hide();
	
	if (text1)
		text1->Hide();
	if (text2)
		text2->Hide();
	
	displayTable = NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////

void RoomTablesDisplayStartButton::DoClick(BOOL netMessage)
{
	if (roomClient->status < RCS_IN_ROOM)
		return;
	
	Hide();
	SetState(DEFAULT_BUTTON_NORMAL);
	
	roomClient->DoChatCommand(NULL, "start");
}

////////////////////////////////////////////////////////////////////////////////////////////////

RoomTablesDisplayTable::RoomTablesDisplayTable()
{
	int i;
	
	tableIndex = -1;
	
	label = NULL;
	numberView = NULL;
	gstDisplay = NULL;
	description = NULL;
	startButton = NULL;
	requestingApproval = NULL;
	watchersDisplay = NULL;
	
	for (i =  0 ; i < MAX_GAME_PLAYERS ; i++)
		seats[i] = NULL;

	for(i = 0; i < NUM_RATING_CATAGORIES; i++)
		m_apoRatingSkillMarkerView[i] = NULL;
}

RoomTablesDisplayTable::~RoomTablesDisplayTable()
{
	int i;
	
	if (label) { delete label; label = NULL; }
	if (numberView) { delete numberView; numberView = NULL; }
	if (gstDisplay) { delete gstDisplay; gstDisplay = NULL; }
	if (description) { delete description; description = NULL; }
	if (startButton) { delete startButton; startButton = NULL; }
	if (requestingApproval) { delete requestingApproval; requestingApproval = NULL; }
	if (watchersDisplay) { delete watchersDisplay; watchersDisplay = NULL; }
	
	for (i =  0 ; i < MAX_GAME_PLAYERS ; i++)
		if (seats[i]) { delete seats[i]; seats[i] = NULL; }

	for(i = 0; i < NUM_RATING_CATAGORIES; i++)
		if (m_apoRatingSkillMarkerView[i]) { delete m_apoRatingSkillMarkerView[i]; m_apoRatingSkillMarkerView[i] = NULL; }
}

void RoomTablesDisplayTable::Init()
{
	i18nStr getS;
	int i;
	int colorMappings[6][3] = {{142,131,199}, {129,168,223}, {93,169,93}, {196, 174, 0}, {254, 156, 38}, {255, 0, 0}}; //V,B,G,Y,O,R
	
	sciView::Init();
	if (!roomClient->TablesHaveAlpha())
		UseColorOfPoint(TRUE);
	
	label = new sciText();
	label->ParamInit(2, "", 64, 16, 0, 0, 102, JUST_HORIZONTAL | JUST_VERTICAL);

	numberView = new sciView();
	numberView->ParamInit(200030, 0, 0, 101);

	gstDisplay = new GAMESPECIFICTABLEDISPLAY();
	gstDisplay->Init();

	description = new RoomTablesDisplayDescription();
	description->Init();
	description->SetPri(101);
	
	startButton = new RoomTablesDisplayStartButton();
	startButton->ParamInit2(4, "", 200043, 0, 0, 150);
	
	requestingApproval = new sciText();
	requestingApproval->SetLineSpacing(12);
	requestingApproval->ParamInit(2, GetString(187, getS), 100, 32, 0, 0, 102, JUST_HORIZONTAL | JUST_VERTICAL);
	requestingApproval->SetBackgroundColor(255, 248, 185);
	
	watchersDisplay = new RoomTablesDisplayWatchers();
	watchersDisplay->ParamInit(200004, 0, 0, 101);
	
	for (i =  0 ; i < MAX_GAME_PLAYERS ; i++) {
		seats[i] = new RoomTablesDisplaySeat();
		seats[i]->Init(i);
	}

	for(i = 0; i < NUM_RATING_CATAGORIES; i++)
	{
		m_apoRatingSkillMarkerView[i] = new sciView();
		m_apoRatingSkillMarkerView[i]->SetView(220201);
		m_apoRatingSkillMarkerView[i]->SetCel(1);
		m_apoRatingSkillMarkerView[i]->SetPri(101);
		m_apoRatingSkillMarkerView[i]->Init();
		m_apoRatingSkillMarkerView[i]->SetColorMap(0, 0, 0, colorMappings[i][0], colorMappings[i][1], colorMappings[i][2]);
		m_apoRatingSkillMarkerView[i]->Hide();
	}

}

void RoomTablesDisplayTable::Posn(int newX, int newY)
{
	sciView::Posn(newX, newY);

	roomClient->PosnTablesDisplayTable(this);
}

void RoomTablesDisplayTable::Show(int theTableIndex)
{
	char s[256];
	int i;

	tableIndex = theTableIndex;
	
	sciView::Show();
	
	if(roomClient->tables[tableIndex].ratingsFlags & RF_TABLE_RATED)
	{
		if (numberView) 
			numberView->SetView(200056);
	}
	else
	{
		if (numberView) 
			numberView->SetView(200030);
	}

	sprintf(s, "%d", tableIndex + 1);

	if (label) {
		label->SetText(s);
#ifndef ROOM_TABLE_NUMBER_IN_BLACK
		label->SetColorMap(0, 0, 0, 255, 255, 255);
#endif
		label->Show();
	}
	
	if (numberView)
		numberView->Show();
	
	if (roomClient->status < RCS_IN_ROOM)
		return;
	
	if (gstDisplay)
		gstDisplay->Show(tableIndex);

	if (roomClient->DimInactiveTable()) {
		if (roomClient->tables[tableIndex].status == TS_PLAYING || roomClient->tables[tableIndex].status == TS_DISPLAYED)
			SetAlpha(255);
		else
			SetAlpha(80);
	}
	
	if (description)
		description->Show(tableIndex, this);
	
	if (startButton) {
		startButton->Hide();
		if (tableIndex == roomClient->players[0].info.table)
			if (roomClient->players[0].info.seat < MAX_GAME_PLAYERS)
				if (!(roomClient->players[0].info.flags & UF_READY_TO_PLAY))
					startButton->Show();
	}
	
	if (requestingApproval) {
		requestingApproval->Hide();
		if (tableIndex == roomClient->requestingTable)
			if (roomClient->players[0].info.table == -1)
				requestingApproval->Show();
	}
	
	if (watchersDisplay)
		watchersDisplay->Show(tableIndex);
	
	for (i = 0 ; i < MAX_GAME_PLAYERS ; i++)
		if (seats[i])
			seats[i]->Show(tableIndex);

	for(i = 0; i < NUM_RATING_CATAGORIES; i++)
		if((roomClient->tables[tableIndex].ratingsFlags & RF_TABLE_HAS_SKILL) && 
			(roomClient->tables[tableIndex].ratingsSkills & (1 << i)))
			m_apoRatingSkillMarkerView[i]->Show();
		else
			m_apoRatingSkillMarkerView[i]->Hide();
}

///////////

void RoomTablesDisplay::Init()
{
	int i;
	
	for (i = 0 ; i < MAX_VISIBLE_TABLES ; i++)
		displayTables[i].ParamInit(ROOM_TABLE_PIC, 0, 0, 100);
}

void RoomTablesDisplay::DisplayTables(int fromLineNum)
{
	if (roomClient->numTables == 0)
		return;
	
	if (roomClient->tablesScrollBox->GetNumItems() > roomClient->tablesScrollBox->GetMaxVisibleItems())
		roomClient->tablesScrollBox->Enable();
	
	if (fromLineNum == -1)
		fromLineNum = roomClient->tablesScrollBox->GetFirstVisibleItem();
	
	roomClient->DisplayTables(this, fromLineNum);
}

void RoomTablesDisplay::BringTableIntoFullView(int table)
{
	int currentFrame, minFrame, maxFrame;
	
	// consider each "frame" an item
	
	currentFrame = roomClient->tablesScrollBox->GetFirstVisibleItem();
	
#if (NUM_TABLES_PER_LINE == 2)
	minFrame = (table & 0xFFFE) - 1;
#else
	minFrame = table * 2 - 1;
#endif
	maxFrame = minFrame + 1;
	
	if (currentFrame < minFrame)
		currentFrame = minFrame;
	
	if (currentFrame > maxFrame)
		currentFrame = maxFrame;
	
	if (currentFrame < 0)
		currentFrame = 0;
	
	if (currentFrame >= roomClient->tablesScrollBox->GetNumItems())
		currentFrame = roomClient->tablesScrollBox->GetNumItems() - 1;
	
	roomClient->tablesScrollBox->DoRefresh(currentFrame);
}

////////////////////////////////////////////////////////////////////////////////////

void ClientScrollBox::Init(sciPlane *thePlane, int theID)
{
	plane = thePlane;
	
	sciScrollBox::Init();
	
	id = theID;
	
	SetPri(501);
	SetViews(200010, 200011, 200012);
	SetScrollerOffsets(2, -1, 1);
	SetAlpha(0); // hide the rectangle
	Enable(); // buttons must start enabled before they can be disabled
	Disable(); // nothing in the lists yet
	
	switch (id) {
		case CSBID_ROOM_CHAT:
			scroller->SetCel(1);
			SetSize(14, 97);
			Posn(495, 314);
			break;
		case CSBID_PLAYERS:
			SetSize(14, 213);
			Posn(611, 79);
			break;
		case CSBID_TABLES:
			SetSize(14, 213);
			Posn(401, 79);
			break;
		case CSBID_GAME_CHAT:
			scroller->SetCel(1);
			SetSize(14, 97);
			Posn(495 + CHAT_PLANE_SHIFT_X, 314 + CHAT_PLANE_SHIFT_Y);
			break;
	}
	
	previousButton->SetRepeat(30, 6);
	nextButton->SetRepeat(30, 6);
}
		
int ClientScrollBox::GetNumItems()
{
	ChatPlane *chatPlane;
	
	switch (id) {
		case CSBID_ROOM_CHAT:
			return roomClient->chatDisplay->numLines + 1; // one blank line at the end
		case CSBID_PLAYERS:
			return roomClient->playersDisplay->GetNumLines();
		case CSBID_TABLES:
			// consider each "frame" an item
			
#if (NUM_TABLES_PER_LINE == 2)
			return max(1, (roomClient->numTables - 2));
#else
			return max(1, (roomClient->numTables - 1) * 2);
#endif
		case CSBID_GAME_CHAT:
			chatPlane = (ChatPlane *)plane;
			
			return chatPlane->chatDisplay->numLines + 1; // one blank line at the end
	}
	
	assert(FALSE);
	
	return 0;
}

int ClientScrollBox::GetMaxVisibleItems()
{
	switch (id) {
		case CSBID_ROOM_CHAT: return NUM_VISIBLE_SCROLLING_CHAT_LINES;
		case CSBID_PLAYERS: return NUM_VISIBLE_PLAYER_LINES;
		case CSBID_TABLES: return 1; // 1 "frame" in the scrolling list of tables
		case CSBID_GAME_CHAT: return NUM_VISIBLE_SCROLLING_CHAT_LINES;
	}
	
	assert(FALSE);
	
	return 1;
}

void ClientScrollBox::DoScroll(int direction, int numItems)
{
	// don't try to scroll if not possible
	if (GetNumItems() <= GetMaxVisibleItems())
		return;

	sciScrollBox::DoScroll(direction, numItems);
}

void ClientScrollBox::DoRefresh(int firstItem)
{
	ChatPlane *chatPlane;
	
	sciScrollBox::DoRefresh(firstItem);
	
	switch (id) {
		case CSBID_ROOM_CHAT:
			roomClient->chatDisplay->DisplayLines(firstItem);
			break;
		case CSBID_PLAYERS:
			roomClient->playersDisplay->DisplayLines();
			break;
		case CSBID_TABLES:
			roomClient->tablesDisplay->DisplayTables();
			break;
		case CSBID_GAME_CHAT:
			chatPlane = (ChatPlane *)plane;
			chatPlane->chatDisplay->DisplayLines(firstItem);
			break;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////

int RoomClientPlayer::GetIconCel()
{
	if (IsRalph())
		return 8;
	
	if (IsHelper())
		return 7;
	
	if (IsTournamentAdministrator())
		return 6;
	
	if (IsReferee())
		return 5;
	
	if (IsSuperUser())
		return 4;

	if (IsMonitor())
		return 2;
	
	if (info.playerID == roomClient->GetMyID())
		return IsCustomer() ? 1 : 0;
	
	return IsCustomer() ? 4 : 3;
}

//////////////////////////////////////////////////////////////////////////////////////////

RoomRadioButton::RoomRadioButton(int newID)
	: sciRadioButton(newID, FALSE)
{
	well = NULL;
}

RoomRadioButton::~RoomRadioButton()
{
	if (well) { delete well; well = NULL; }
}

void RoomRadioButton::Init()
{
	sciRadioButton::Init(0, 0);
	
	well = new sciView();
	well->ParamInit(200049, 0, 0, 501);
}

void RoomRadioButton::Show()
{
	sciRadioButton::Show();
	
	if (well)
		well->Show();
}

void RoomRadioButton::Hide()
{
	sciRadioButton::Hide();
	
	if (well)
		well->Hide();
}

void RoomRadioButton::Posn(int x, int y)
{
	sciRadioButton::Posn(x, y);
	
	if (well)
		well->Posn(x, y);
}

void RoomRadioButton::SetPri(int newPri)
{
	sciRadioButton::SetPri(newPri + 1);
	
	if (well)
		well->SetPri(newPri);
}

//////////////////////////////////////////////////////////////////////////////////////////

void RoomButton::DoClick(BOOL netMessage)
{
	char buf[300];
	i18nStr getS;
	int chatType, table;
	sciEvent event;
	
	if (roomClient->status < RCS_IN_ROOM && id != RBID_EXIT)
		return;
	
	SetState(DEFAULT_BUTTON_NORMAL);
	
	switch (id) {
		case RBID_WON:
			roomClient->popup->SetNumItems(5);
			roomClient->popup->AddItem(POPID_ROOM_GOTO_WONNET, POPITEM_SELECTABLE, GetString(188, getS));
			roomClient->popup->AddItem(POPID_ROOM_GOTO_SIERRACOM, POPITEM_SELECTABLE, GetString(189, getS));
			roomClient->popup->AddItem(0, POPITEM_DELIMITER, "");
			roomClient->popup->AddItem(POPID_ROOM_MEMBERSHIP_POLICY, POPITEM_SELECTABLE, GetString(190, getS));
			roomClient->popup->AddItem(POPID_ROOM_COMPLAINT, POPITEM_SELECTABLE, GetString(161, getS));
			roomClient->popup->Posn(68, 18);
			roomClient->popup->Show(this);
			break;
		case RBID_HOYLE:
#ifdef _DEBUG
			if (ControlKeyDown() && ShiftKeyDown()) {
				if (roomClient->commandLineMode) {
					roomClient->commandLineMode = FALSE;
					roomClient->hoyleButton->Posn(0, 0);
				}
				else {
					roomClient->commandLineMode = TRUE;
					roomClient->hoyleButton->Posn(0, 15);
				}
				break;
			}
#endif
			roomClient->popup->SetNumItems(4);
			roomClient->popup->AddItem(POPID_ROOM_GETTING_STARTED, POPITEM_SELECTABLE, GetString(191, getS));
			roomClient->popup->AddItem(0, POPITEM_DELIMITER, "");
			roomClient->popup->AddItem(POPID_ROOM_GOTO_HOYLECOM, POPITEM_SELECTABLE, GetString(192, getS));
			roomClient->popup->AddItem(POPID_ROOM_ABOUT_GAME, POPITEM_SELECTABLE, GetGameString(3, getS));
			roomClient->popup->Posn(568 - roomClient->popup->GetWidth(), 18);
			roomClient->popup->Show(this);
			break;
		case RBID_OPTIONS:
			if (!roomClient->optionsDialog) {
				roomClient->optionsDialog = new OptionsDialogBox(pApp->pMainPlane);
				roomClient->optionsDialog->Init();
			}
			if (roomClient->optionsDialog->IsHidden())
				roomClient->optionsDialog->Show(roomClient->lastOptionsDialogVersion, FALSE);
			break;
		case RBID_EXIT:
			game->QuitGame(FALSE);
			break;
		case RBID_MINIMIZE:
			ShowWindow(plane->GetWindow()->hWnd, SW_MINIMIZE);
			break;
		case RBID_SPONSORSHIP:
#ifdef _DEBUG
			DebugString("browser\n");
#endif
			if (won && wonParams.sponsorURL[0])
				won->LaunchBrowser(wonParams.sponsorURL);
			break;
		case RBID_CHAT_PUBLIC:
		case RBID_CHAT_PRIVATE:
		case RBID_CHAT_BROADCAST:
			switch (id) {
				case RBID_CHAT_PUBLIC: chatType = 0; break;
				case RBID_CHAT_PRIVATE: chatType = 1; break;
				case RBID_CHAT_BROADCAST: chatType = 2; break;
			}
			roomClient->chatGroup->SelectButtonByID(chatType);
			strcpy(buf, "rchat ");
			strnzcpy(&buf[strlen(buf)], (char *)roomClient->roomChatEditBox->GetText(), 255);
			roomClient->DoChatCommand(NULL, buf);
			roomClient->roomChatEditBox->SetText("");
			break;
		case RBID_TOURNAMENT_INFORMATION:
			if (!roomClient->IsTournamentRoom()) {
				if (won)
					won->LaunchBrowser(GetString(134, getS));
				break;
			}
			if (!roomClient->tournamentDialog)
				roomClient->tournamentDialog = new TournamentDialogBox();
			roomClient->tournamentDialog->Show();
			break;
		case RBID_SHOW_GAME:
			table = roomClient->players[0].info.table;
			if (roomClient->tables[table].status == TS_DISPLAYED && roomClient->tables[table].plane != NULL)
				SetForegroundWindow(roomClient->tables[table].plane->GetWindow()->hWnd);
			break;
		default:
			assert(FALSE);
			break;
	}
}

////////////////////////////////////////////////////////////////////////////////////

void RoomPopup::ItemSelected(int itemID, int itemPlayerID, int itemX, int itemY)
{
	char s[512], bankS[64], login[MAX_PLAYER_NAME + 5], *data;
	i18nStr getS, getS2, getS3, getS4, getS5;
	int i, j;
	sciEvent event;
	ROOMCLIENTPLAYER *player;
	MTS_ROOM_MODERATE_MESSAGE mts;
	MTS_ROOM_TI_JOIN_MESSAGE mtsTIJM;
	MTS_ROOM_TI_SET_BANK_MESSAGE mtsTISBM;
	HGLOBAL handle;
	int version;
	
	if (itemID >= POPID_ROOM_PRIVATE_CHAT && itemID <= POPID_ROOM_BAN_ONLINE) {
		if (playerID == -1)
			{ assert(FALSE); return; }
		
		player = roomClient->GetPlayerFromID(playerID);
		if (player == NULL)
			return; // assert(FALSE) // allow this case for when player leaves while popup is up
	}
	
	switch (itemID) {
		// click on won logo in room
		case POPID_ROOM_GOTO_WONNET:
			if (won)
				won->LaunchBrowser(GetString(193, getS));
			break;
		case POPID_ROOM_GOTO_SIERRACOM:
			if (won)
				won->LaunchBrowser(GetString(194, getS));
			break;
		case POPID_ROOM_MEMBERSHIP_POLICY:
			if (won)
				won->LaunchBrowser(GetString(195, getS));
			break;
		case POPID_ROOM_COMPLAINT:
			if (!roomClient->complaintDialog) {
				roomClient->complaintDialog = new ComplaintDialogBox(pApp->pMainPlane);
				roomClient->complaintDialog->Init();
			}
			roomClient->complaintDialog->Show(playerID, FALSE);
			break;
		
		// click on hoyle logo in room
		case POPID_ROOM_GOTO_HOYLECOM:
			if (won)
				won->LaunchBrowser(GetString(196, getS));
			break;
		case POPID_ROOM_ABOUT_GAME:
			if (won)
				won->LaunchBrowser(GetGameString(4, getS));
			break;
		case POPID_ROOM_GETTING_STARTED:
			roomClient->instructionsDialog->Show(FALSE);
			break;
		
		// click on self in room
		case POPID_ROOM_LEAVE_TABLE:
			if (roomClient->tables[table].status == TS_DISPLAYED) {
				sprintf(s, GetString(197, getS), table + 1);
				roomClient->ShowModelessMessage(0, s, GetString(103, getS), MB_YESNO, DBP_LEAVE_GAME);
			}
			else
				roomClient->DoChatCommand(NULL, "stand");
			break;
		case POPID_ROOM_PERSONAL_OPTIONS:
		case POPID_ROOM_TABLE_OPTIONS:
		case POPID_ROOM_RATING_OPTIONS:
			if (!roomClient->optionsDialog) {
				roomClient->optionsDialog = new OptionsDialogBox(pApp->pMainPlane);
				roomClient->optionsDialog->Init();
			}
			if (roomClient->optionsDialog->IsHidden())
			{
				if(itemID == POPID_ROOM_PERSONAL_OPTIONS)
					version = ODBV_PERSONAL;
				else if(itemID == POPID_ROOM_RATING_OPTIONS)
					version = ODBV_RATINGS;
				else
					version = ODBV_TABLE;

				roomClient->optionsDialog->Show(version, FALSE);
			}
			break;
		case POPID_ROOM_EDIT_PROFILE:
			if (!roomClient->profileDialog) {
				roomClient->profileDialog = new ProfileDialogBox(pApp->pMainPlane);
				roomClient->profileDialog->Init();
			}
			roomClient->profileDialog->Show(playerID);
			break;
		
		// click on other in room
		case POPID_ROOM_PRIVATE_CHAT:
			player->isSelected = !player->isSelected;
			roomClient->UpdateChatButtons(player);
			break;
		case POPID_ROOM_FRIEND:
			player->isFriend = !player->isFriend;
			if (player->isFriend)
				roomClient->AddFriend(player->info.login);
			else
				roomClient->DeleteFriend(player->info.login);
			/*
			if (won) {
				if (player->isFriend)
					won->AddFriend(player->info.login);
				else
					won->DeleteFriend(player->info.login);
			}
			*/
			break;
		case POPID_ROOM_VIEW_PROFILE:
			if (!roomClient->profileDialog) {
				roomClient->profileDialog = new ProfileDialogBox(pApp->pMainPlane);
				roomClient->profileDialog->Init();
			}
			roomClient->profileDialog->Show(playerID);
			break;
		case POPID_ROOM_INVITE_TO_PLAY:
			sprintf(s, "invite %d play", playerID);
			roomClient->DoChatCommand(NULL, s);
			break;
		case POPID_ROOM_INVITE_TO_WATCH:
			sprintf(s, "invite %d watch", playerID);
			roomClient->DoChatCommand(NULL, s);
			break;
		case POPID_ROOM_TOURNAMENT_JOIN:
			if (!player->IsInTournament() && IS_A_CASINO_GAME) {
				sprintf(s, GetString(198, getS), player->info.login);
				GetStringDialog(s, bankS, 63);
			}
			mtsTIJM.messageType = MTS_ROOM_TI_JOIN;
			mtsTIJM.playerID = playerID;
			mtsTIJM.joining = !player->IsInTournament();
			roomClient->SendMessageToServer((char *)&mtsTIJM, sizeof(MTS_ROOM_TI_JOIN_MESSAGE));
			if (player->IsInTournament())
				break;
			if (!IS_A_CASINO_GAME)
				break;
			// FALL THROUGH to set bank of newly joined player
		case POPID_ROOM_TOURNAMENT_BANK:
			if (itemID == POPID_ROOM_TOURNAMENT_BANK) {
				sprintf(s, GetString(199, getS), player->info.login);
				GetStringDialog(s, bankS, 63);
			}
			for (i = 0, j =0 ; bankS[i] ; i++)
				if (bankS[i] >= '0' && bankS[i] <= '9')
					s[j++] = bankS[i];
			s[j] = 0;
			if (s[0]) {
				mtsTISBM.messageType = MTS_ROOM_TI_SET_BANK;
				mtsTISBM.playerID = playerID;
				mtsTISBM.bankValue = atoi(s);
				
				roomClient->SendMessageToServer((char *)&mtsTISBM, sizeof(MTS_ROOM_TI_SET_BANK_MESSAGE));
			}
			/*
			mtsTIJ.messageType = MTS_ROOM_TI_JOIN;
			mtsTIJ.playerID = playerID;
			mtsTIJ.joining = !player->IsInTournament();
			roomClient->SendMessageToServer((char *)&mtsTIJ, sizeof(MTS_ROOM_TI_JOIN_MESSAGE));
			*/
			break;
		case POPID_ROOM_MUTE:
			player->isMuted = !player->isMuted;
			if (player->isMuted)
				roomClient->AddFoe(player->info.login);
			else
				roomClient->DeleteFoe(player->info.login);
			/*
			if (won) {
				if (player->isMuted)
					won->AddFoe(player->info.login);
				else
					won->DeleteFoe(player->info.login);
			}
			*/
			break;
		case POPID_ROOM_PLAYER_COMPLAINT:
			if (!roomClient->complaintDialog) {
				roomClient->complaintDialog = new ComplaintDialogBox(pApp->pMainPlane);
				roomClient->complaintDialog->Init();
			}
			roomClient->complaintDialog->Show(player->info.playerID, FALSE);
			break;
		case POPID_ROOM_GET_IP:
		case POPID_ROOM_GLOBAL_MUTE:
		case POPID_ROOM_GLOBAL_UNMUTE:
		case POPID_ROOM_BOOT:
		case POPID_ROOM_BAN_ONLINE:
			mts.messageType = MTS_ROOM_MODERATE;
			mts.playerID = player->info.playerID;
			strcpy(mts.login, player->info.login);
			mts.function = 0;
			s[0] = 0;
			switch (itemID) {
				case POPID_ROOM_GET_IP: mts.function = MODERATE_GET_IP; strcpy(s, GetString(206, getS)); break;
				case POPID_ROOM_GLOBAL_MUTE: mts.function = MODERATE_GLOBAL_MUTE; strcpy(s, GetString(163, getS)); break;
				case POPID_ROOM_GLOBAL_UNMUTE: mts.function = MODERATE_GLOBAL_UNMUTE; strcpy(s, GetString(164, getS)); break;
				case POPID_ROOM_BOOT: mts.function = MODERATE_BOOT; strcpy(s, GetString(165, getS)); break;
				case POPID_ROOM_BAN_ONLINE: mts.function = MODERATE_BAN_ONLINE; strcpy(s, GetString(166, getS)); break;
			}
			if (MessageBoxA(NULL, GetString(200, getS), s, MB_YESNO | MB_ICONQUESTION | MB_TASKMODAL) == IDYES)
				roomClient->SendMessageToServer((char *)&mts, sizeof(MTS_ROOM_MODERATE_MESSAGE));
			break;
		case POPID_ROOM_BAN_OFFLINE:
		case POPID_ROOM_UNBAN:
			switch (itemID) {
				case POPID_ROOM_BAN_OFFLINE:
					strcpy(s, GetString(201, getS));
					mts.function = MODERATE_BAN_OFFLINE;
					break;
				case POPID_ROOM_UNBAN:
					strcpy(s, GetString(202, getS));
					mts.function = MODERATE_UNBAN;
					break;
			}
			GetStringDialog(s, login, MAX_PLAYER_NAME);
			if (!login[0] || login[0] == ' ')
				break;
			mts.messageType = MTS_ROOM_MODERATE;
			mts.playerID = -1;
			strcpy(mts.login, login);
			roomClient->SendMessageToServer((char *)&mts, sizeof(MTS_ROOM_MODERATE_MESSAGE));
			break;
		
		// click on name in room watchers popup
		case POPID_ROOM_WATCHERS_CLICK:
			if (!roomClient->AddPlayerPopupItems(roomClient->GetPlayerIndexFromID(itemPlayerID)))
				break;
			
			Posn(itemX, itemY);
			Show(NULL);
			break;
		
		// click on chair in room
		case POPID_ROOM_JOIN:
			sprintf(s, "sit %d %d", table, seat);
			roomClient->DoChatCommand(NULL, s);
			break;
		case POPID_ROOM_WATCH:
			sprintf(s, "watch %d", table);
			roomClient->DoChatCommand(NULL, s);
			break;
		
		// click on chat log in room
		case POPID_ROOM_COPY_CHAT_LOG:
			if (!OpenClipboard(NULL))
				break;
			if (!EmptyClipboard()) {
				CloseClipboard();
				break;
			}
			handle = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, roomClient->chatDisplay->numLines * 512);
			if (!handle) {
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
			for (i = 0 ; i < roomClient->chatDisplay->numLines ; i++) {
				sprintf(s, "%s %s\r\n", roomClient->chatDisplay->lines[i].label, roomClient->chatDisplay->lines[i].text);
				strcat(data, s);
			}
			GlobalUnlock(handle);
			SetClipboardData(CF_TEXT, handle);
			CloseClipboard();
			break;
		
		// (right) click on chat edit text in room
		case POPID_ROOM_PASTE_CHAT:
			// simulate a control-V
			event.claimed = FALSE;
			event.c = 22; // control-V
			event.type = KEY_PRESS;
			event.modifiers = CTRL_DOWN;
			event.message = WM_CHAR;
			event.lparam = 0; // 0x002F0001
			event.wparam = 22; // control-V
			roomClient->roomChatEditBox->HandleEvent(event);
			break;

		case POPID_ROOM_RATINGS:
			sprintf(s, "%s %d\n %s %d\n%s %d\n%s %d\n%s %d", GetString(339, getS), player->info.wins,
				GetString(340, getS2), player->info.losses, 
				GetString(341, getS3), player->info.draws, 
				GetString(342, getS4), player->info.quits,
				GetString(338, getS5), player->info.rating);
			sprintf(getS2, GetString(343, getS), player->info.login);

			roomClient->ShowModelessMessage(0, s, getS2, MB_OK);
			break;
	}
	
	if (itemID >= POPID_ROOM_PRIVATE_CHAT && itemID <= POPID_ROOM_BAN_ONLINE) {
		roomClient->ReorderPlayer(roomClient->GetPlayerIndexFromID(playerID));
		roomClient->playersDisplay->DisplayLines();
	}
}
