#include "os.h"
#include "sci.h"
#include "globals.h"
#include "util.h"
#include "inifile.h"
#include "maketex.h"

#include "..\all room clients\rc.h"

#include "Client Include.h"

ROOMCLIENT *roomClient = NULL;
Boolean gbShowReadyForMessages;

RoomClient::RoomClient()
{
	int i;
	
	connections = NULL;
	players = NULL;
	tables = NULL;
	server = 0;
	status = RCS_UNCONNECTED;
	
	roomOverlay = NULL;
	roomMessageBox = NULL;
	
	wonButton = NULL;
	hoyleButton = NULL;
	optionsButton = NULL;
	playersInRoomLabel = NULL;
	optionsDialog = NULL;
	profileDialog = NULL;
	complaintDialog = NULL;
	instructionsDialog = NULL;
	tournamentDialog = NULL;
	popup = NULL;
	exitButton = NULL;
	minimizeButton = NULL;
	sponsorshipButton = NULL;
	tournamentInformationButton = NULL;
	showGameButton = NULL;
	
	chatGroup = NULL;
	for (i = 0 ; i < 3 ; i++) {
		chatRadioButtons[i] = NULL;
		chatPushButtons[i] = NULL;
	}
	privateChatCount = NULL;
	roomChatEditBox = NULL;
	chatDisplay = NULL;
	commandLineMode = FALSE;
	dirtyWordFilter = NULL;
	
	playersDisplay = NULL;
	tablesDisplay = NULL;
	
	chatScrollBox = NULL;
	playersScrollBox = NULL;
	tablesScrollBox = NULL;
	
	for (i = 0 ; i < 3 ; i++)
		playerListHeadings[i] = NULL;
	playerListRect = NULL;
	
	currentAdInfo = NULL;
	gifAd = NULL;
	jpegAd = NULL;
	gifGameAd = NULL;
	jpegGameAd = NULL;
	gameAdPlane = NULL;
	
	rsvpPlane = NULL;
	
	statusText = NULL;
	diagnostics = NULL;
	
	globalIniFile = NULL;
	
	roomFlags = 0;
	tournamentInformation.header.sittingEnabled = TRUE;
	tournamentInformation.header.joinEnabled = TRUE;
	tournamentInformation.header.automaticResultsEnabled = FALSE;
	tournamentInformation.header.startingBankValue = FALSE;
	for (i = 0 ; i < 5 ; i++) {
		tournamentInformation.header.checkOutPlayerIDs[i] = -1;
		tournamentInformation.text[i][0] = 0;
	}
	
	m_iAIRatingWins		= 0;
	m_iAIRatingLosses	= 0;
	m_iAIRatingDraws	= 0;
	m_iAIRatingPartials	= 0;
	m_iAIRating			= 0;
	m_iAIRatingDev		= 0;
}

RoomClient::~RoomClient()
{
	int i;
	QueuedMessage *qm;
	
	status = RCS_QUITTING;
	SetCurrentAd(NULL);
	
	if (connections != NULL) {
		connections->CloseAll();
		delete connections;
	}
	if (players) delete[] players;
	if (tables) delete[] tables;
	
	if (roomOverlay) { delete roomOverlay; roomOverlay = NULL; }
	if (wonButton) { delete wonButton; wonButton = NULL; }
	if (hoyleButton) { delete hoyleButton; hoyleButton = NULL; }
	if (optionsButton) { delete optionsButton; optionsButton = NULL; }
	if (playersInRoomLabel) { delete playersInRoomLabel; playersInRoomLabel = NULL; }
	
	if (optionsDialog) { delete optionsDialog; optionsDialog = NULL; }
	if (profileDialog) { delete profileDialog; profileDialog = NULL; }
	if (instructionsDialog) { delete instructionsDialog; instructionsDialog = NULL; }
	if (tournamentDialog) { delete tournamentDialog; tournamentDialog = NULL; }
	if (roomMessageBox) { delete roomMessageBox; roomMessageBox = NULL; }
	
	if (popup) { delete popup; popup = NULL; }
	if (exitButton) { delete exitButton; exitButton = NULL; }
	if (minimizeButton) { delete minimizeButton; minimizeButton = NULL; }
	if (sponsorshipButton) { delete sponsorshipButton; sponsorshipButton = NULL; }
	if (tournamentInformationButton) { delete tournamentInformationButton; tournamentInformationButton = NULL; }
	if (showGameButton) { delete showGameButton; showGameButton = NULL; }
	
	if (chatGroup) { delete chatGroup; chatGroup = NULL; }
	for (i = 0 ; i < 3 ; i++) {
		if (chatRadioButtons[i]) { delete chatRadioButtons[i]; chatRadioButtons[i] = NULL; }
		if (chatPushButtons[i]) { delete chatPushButtons[i]; chatPushButtons[i] = NULL; }
	}
	if (privateChatCount) { delete privateChatCount; privateChatCount = NULL; }
	if (chatDisplay) { delete chatDisplay; chatDisplay = NULL; }
	if (dirtyWordFilter) { delete dirtyWordFilter; dirtyWordFilter = NULL; }
	
	if (roomChatEditBox) { delete roomChatEditBox; roomChatEditBox = NULL; }
	
	if (playersDisplay) { delete playersDisplay; playersDisplay = NULL; }
	if (tablesDisplay) { delete tablesDisplay; tablesDisplay = NULL; }
	
	if (chatScrollBox) { delete chatScrollBox; chatScrollBox = NULL; }
	if (playersScrollBox) { delete playersScrollBox; playersScrollBox = NULL; }
	if (tablesScrollBox) { delete tablesScrollBox; tablesScrollBox = NULL; }
	
	for (i = 0 ; i < 3 ; i++)
		if (playerListHeadings[i]) { delete playerListHeadings[i]; playerListHeadings[i] = NULL; }
	if (playerListRect) { delete playerListRect; playerListRect = NULL; }
	
	if (currentAdInfo) { delete currentAdInfo; currentAdInfo = NULL; }
	
	if (rsvpPlane) { delete rsvpPlane; rsvpPlane = NULL; }
	
	if (statusText) { delete statusText; statusText = NULL; }
	if (diagnostics) { delete diagnostics; diagnostics = NULL; }
	
	if (globalIniFile) { delete globalIniFile; globalIniFile = NULL; }
	
	for (i = 0 ; i < queue.Size() ; i++) {
		qm = (QueuedMessage *)queue.At(i);
		delete[] qm->message;
		delete qm;
	}
	if (room0->script)
		room0->script->SetScript(NULL);
	
	roomClient = NULL;
}

int RoomClient::Init()
{
	char path[MAX_PATH], getS[256], getS2[256];
	int i, err;
	HCONNECTION c;
	static int ids[3] = { RBID_CHAT_PUBLIC, RBID_CHAT_PRIVATE, RBID_CHAT_BROADCAST };
	
	/*
	HINSTANCE makeTex;
	lpfnJpeg2TexFunc jpeg2Tex;
	
	makeTex = LoadLibrary("maketex");
	jpeg2Tex = (lpfnJpeg2TexFunc)GetProcAddress(makeTex, "Jpeg2Tex");
	jpeg2Tex("ad2.jpg", "2.tex");
	jpeg2Tex("ad3.jpg", "3.tex");
	jpeg2Tex("ad4.jpg", "4.tex");
	FreeLibrary(makeTex);
	*/
	
	if (!connections) {
		connections = new RoomClientConnections(1, 0);
		if (!connections)
			{ ReportError(GetString(3, getS)); game->QuitGame(TRUE); return -1; }
		err = connections->Init();
		if (err)
			{ ReportError(GetString(4, getS), err); game->QuitGame(TRUE); return err; }
		connections->SetConnectTimeoutTicks(SecondsToTicks(2 * 60));
	}
	
	c = connections->RequestConnection(wonParams.serverLocation, wonParams.serverPort, 1000);
	if (c == NULL)
		{ ReportError(GetString(5, getS)); game->QuitGame(TRUE); return err; }
	
	// turn off TCP Nagle delay
	c->UseNagleDelay(false);

	connections->TurnOffReadSelects();
	
	holdTicks = 0;
	numHoldTicks = 0;
	numPlayers = 0;
	numPlayersPromised = 0;
	numTables = 0;
	gbShowReadyForMessages = FALSE;
	
	players = new ROOMCLIENTPLAYER[MAX_ROOM_PLAYERS];
	if (!players)
		{ ReportError(GetString(6, getS)); game->QuitGame(TRUE); return -1; }
	for (i = 0 ; i < MAX_ROOM_PLAYERS ; i++) {
		players[i].profile[0] = 0;
		// players[i].gameRecord[0] = 0;
	}
	
	tables = new ROOMCLIENTTABLE[MAX_ROOM_TABLES];
	if (!tables)
		{ ReportError(GetString(7, getS)); game->QuitGame(TRUE); return -1; }
	for (i = 0 ; i < MAX_ROOM_TABLES ; i++)
		tables[i].Init(i);
	
#ifdef NO_HW_ROOM_INTERFACE
	return 0;
#endif
	
	sprintf(path, "%s\\hoyle.ini", wonParams.wonPath);
	globalIniFile = new IniFile(path, "Settings");
	gbShowReadyForMessages = globalIniFile->ReadValue("ShowSetReadyForMessages", 0);
	
	roomOverlay = new RoomOverlayView();
	roomOverlay->ParamInit(ROOM_OVERLAY_PIC, 0, 0, 500);
	roomOverlay->Show();
	
	wonButton = new RoomButton(RBID_WON);
	wonButton->Init();
	wonButton->SetView(200008);
	wonButton->SetPri(501);
	wonButton->Show();
	
	hoyleButton = new RoomButton(RBID_HOYLE);
	hoyleButton->Init();
	hoyleButton->SetView(200009);
	hoyleButton->Posn(546, -21);
	hoyleButton->SetPri(501);
	hoyleButton->Show();
	
	optionsButton = new RoomButton(RBID_OPTIONS);
	optionsButton->Init();
	optionsButton->SetView(200007);
	optionsButton->SetPri(501);
	optionsButton->Show();
	
	exitButton = new RoomButton(RBID_EXIT);
	exitButton->Init();
	exitButton->SetView(200138);
	exitButton->SetPri(501);
	exitButton->Show();
	
	minimizeButton = new RoomButton(RBID_MINIMIZE);
	minimizeButton->Init();
	minimizeButton->SetView(200138);
	minimizeButton->SetButtonCels(4, 5, 6, 7);
	minimizeButton->SetPri(501);
	minimizeButton->Show();
	
	sponsorshipButton = new RoomButton(RBID_SPONSORSHIP);
	sponsorshipButton->Init();
	sponsorshipButton->SetPri(501);
	if (pApp->pResourceManager->ResourceIsAvailable(RES_TSPRITE, 290001)) {
		sponsorshipButton->SetView(290001);
		sponsorshipButton->SetButtonCels(0, 0, 0, 0);
		sponsorshipButton->Show();
	}
	else
		sponsorshipButton->Hide();
	
	tournamentInformationButton = new RoomButton(RBID_TOURNAMENT_INFORMATION);
	tournamentInformationButton->UseColorOfPoint(TRUE);
	tournamentInformationButton->Init();
	tournamentInformationButton->SetView(200048);
	tournamentInformationButton->SetPri(501);
	tournamentInformationButton->Show();
	
	showGameButton = new RoomButton(RBID_SHOW_GAME);
	showGameButton->Init();
	showGameButton->SetView(200047);
	showGameButton->SetPri(501);
	showGameButton->Show();
	
	statusText = new sciText();
	statusText->Init();
	statusText->SetBackgroundColor(255, 255, 255);
	// statusText->SetLineSpacing(12);
	statusText->SetFont(2);
	statusText->SetJust(JUST_VERTICAL | JUST_HORIZONTAL);
	statusText->SetText("");
	statusText->SetWidth(300);
	statusText->SetHeight(40);
	statusText->SetPri(501);
	statusText->Posn(320 - 150, 10);
	
	ShowStatus();
	
	diagnostics = new sciText();
	diagnostics->Init();
	diagnostics->SetBackgroundColor(255, 255, 255);
	diagnostics->SetLineSpacing(12);
	diagnostics->SetFont(4);
	diagnostics->SetJust(JUST_VERTICAL | JUST_LEFT);
	diagnostics->SetText("");
	diagnostics->SetWidth(412);
	diagnostics->SetHeight(26);
	diagnostics->SetPri(1000);
	diagnostics->Posn(98, 10);
	diagnostics->Hide();
	
	// let the user see that something is going on...
	MAIN_WINDOW->ForceUpdate();
	
	playersInRoomLabel = new sciText();
	playersInRoomLabel->ParamInit(2, "", 75, 20, 558, 56, 501, JUST_VERTICAL | JUST_HORIZONTAL);
	// playersInRoomLabel->SetBackgroundColor(0, 128, 128);
	// playersInRoomLabel->SetAlpha(128);
	playersInRoomLabel->Show();
	
	popup = new RoomPopup(pApp->pMainPlane);
	popup->Init(POPDIR_ROOM);
	
	chatGroup = new sciButtonGroup();
	chatGroup->Init();
	chatGroup->SetPri(501);
	
	for (i = 0 ; i < 3 ; i++) {
//		chatButtonWells[i] = new sciView();
//		chatButtonWells[i]->ParamInit(ROOM_RADIO_WELL_PIC, 6, (i == 0) ? 417 : 431, 501);
//		chatButtonWells[i]->Show();
		
		chatRadioButtons[i] = new RoomRadioButton(i);
		chatGroup->AddToGroup(chatRadioButtons[i]);
		chatRadioButtons[i]->SetFont(2);
		chatRadioButtons[i]->SetView(200025);
		chatRadioButtons[i]->Init();
		chatRadioButtons[i]->SetTextOffset(19, 4);
		chatRadioButtons[i]->Posn(6, (i == 0) ? 417 : 431);
		chatRadioButtons[i]->SetPri(502);
		chatRadioButtons[i]->SetText((i == 0) ? GetString(9, getS) : GetString(10, getS2));
		chatRadioButtons[i]->Enable();
		
		chatPushButtons[i] = new RoomButton(ids[i]);
		chatPushButtons[i]->SetFont(2);
		chatPushButtons[i]->SetView(200013);
		chatPushButtons[i]->Init();
		chatPushButtons[i]->SetButtonCels(0, 2, 0, 2);
		chatPushButtons[i]->Posn(5, (i == 0) ? 416 : 430);
		chatPushButtons[i]->SetPri(502);
		chatPushButtons[i]->TextPosn(6, 4);
		chatPushButtons[i]->SetText((i == 0) ? GetString(9, getS) : GetString(10, getS2));
		chatPushButtons[i]->Enable();
	}
	
//	chatButtonWells[2]->Posn(6, 1000);
	chatRadioButtons[2]->Posn(6, 1000);
	chatRadioButtons[2]->SetText("");
	chatPushButtons[2]->Posn(6, 1000);
	chatPushButtons[2]->SetText("");
	
	chatRadioButtons[0]->Select();
	chatRadioButtons[1]->Disable();
	chatPushButtons[1]->Disable();
	
	privateChatCount = new sciText();
	privateChatCount->ParamInit(2, "", 75, 20, 65, 432, 502, JUST_VERTICAL | JUST_LEFT);
	
	roomChatEditBox = new RoomChatEditBox();
	roomChatEditBox->SetLineSpacing(14);
	roomChatEditBox->Init();
	roomChatEditBox->SetPri(501);
	roomChatEditBox->Posn(101, 427);
	roomChatEditBox->SetWidth(410);
	roomChatEditBox->SetHeight(38);
	roomChatEditBox->SetFont(4);
	roomChatEditBox->Show();
	roomChatEditBox->SetFocus();
	
	sprintf(path, "%s\\thank.you", wonParams.wonPath);
	dirtyWordFilter = new CDirtyWordFilter(path);
	
	chatScrollBox = new ClientScrollBox();
	chatScrollBox->Init(pApp->pMainPlane, CSBID_ROOM_CHAT);
	chatScrollBox->Show();
	
	playersScrollBox = new ClientScrollBox();
	playersScrollBox->Init(pApp->pMainPlane, CSBID_PLAYERS);
	playersScrollBox->Show();
	
	tablesScrollBox = new ClientScrollBox();
	tablesScrollBox->Init(pApp->pMainPlane, CSBID_TABLES);
	tablesScrollBox->Show();
	
	chatDisplay = new ChatScrollingDisplay();
	chatDisplay->Init(pApp->pMainPlane, chatScrollBox, popup, POPID_ROOM_COPY_CHAT_LOG, 12, 311);
	
	playersDisplay = new RoomPlayersDisplay();
	playersDisplay->Init();
	
	tablesDisplay = new RoomTablesDisplay();
	tablesDisplay->Init();
	
	for (i = 0 ; i < 3 ; i++) {
		playerListHeadings[i] = new sciView();
		playerListHeadings[i]->ParamInit(200019, 0, 0, 501);
		playerListHeadings[i]->SetCel(i);
		if (i != 1 || HasRankings())
			playerListHeadings[i]->Show();
	}
	
	playerListRect = new sciRect();
	playerListRect->Posn(431, 78);
	playerListRect->SetSize(177, 216);
	playerListRect->Show();
	
	currentAdType = AT_NONE;
	gameAdTable = -1;
	currentAdInfo = new WONadInfo();
	
	rsvpPlane = new RSVPPlane();
	rsvpPlane->Init();
	rsvpPlane->Hide();
	// rsvpPlane->Show(RSVP_REQUEST);
	
	requestingTable = -1;
	requesterPlayerID = -1;
	requestedToWatch = FALSE;
	
	inviterTable = -1;
	inviterPlayerID = -1;
	invitedToWatch = FALSE;
	
	lastOptionsDialogVersion = ODBV_PERSONAL;
	
	roomMessageBox = new RoomDialogBox(pApp->pMainPlane, (ROOMCLIENT *)this);
	roomMessageBox->Hide();
	
	instructionsDialog = new InstructionsDialogBox(pApp->pMainPlane);
	instructionsDialog->Hide();
	
	return 0;
}

int RoomClient::Tick()
{
	char message[MAX_MSG];
	int length, i, err;
	HCONNECTION c;
	MFS_GAME_MESSAGE_HEADER *mfs;
	MTS_ROOM_HEARTBEAT_MESSAGE mtsRHM;
	MESSAGE_HEADER *mh;
	// QueuedMessage *qm;
	
	connections->Pump();
	// connections->ServerYield();
	
	ToggleAds();
	
	ShowDiagnostics();
	
	if (server == 0)
		return 0;
	
	// let's avoid the whole issue of queuing messages by simply not processing any
	// messages while any animation is going on
	for (i = 0 ; i < numTables ; i++)
		if (tables[i].status == TS_DISPLAYED && !tables[i].readyForMessages)
			return 0;

	// If the client has asked us to hold messages, don't process any...
	if (HoldingMessages())
		return 0;
	
	for ( ; ; ) {
		length = 0;
		
		/*
		// try to process a queued message for a table that is now ready for messages
		
		for (i = 0 ; i < queue.Size() ; i++) {
			qm = (QueuedMessage *)queue.At(i);
			for (j = 0 ; j < numTables ; j++)
				if (j == qm->table && tables[j].readyForMessages) {
					length = qm->length;
					memcpy(message, qm->message, length);
					queue.Delete(qm);
					delete[] qm->message;
					delete qm;
					i = queue.Size();
					break;
				}
		}
		*/
		
		// otherwise process the next incoming message
		if (length == 0) {
			length = connections->GetMessage(message, 1000, &c);
			if (length > 0 && c != server)
				{ assert(FALSE); return -1; }
		}
		
		if (length == 0)
			return 0;
		
		if (length == -1)
			{ assert(FALSE); return -1; }
		
		mh = (MESSAGE_HEADER *)message;
		
		if (mh->messageType != MFS_ROOM_DISPLAY_STRING && mh->messageType != MFS_ROOM_HEARTBEAT) {
			if ((status == RCS_RECEIVING_ROOM_HEADER) != (mh->messageType == MFS_ROOM_STATUS_HEADER))
				{ assert(FALSE); return -1; }
			if ((status == RCS_RECEIVING_ROOM_TABLES) != (mh->messageType == MFS_ROOM_STATUS_TABLES))
				{ assert(FALSE); return -1; }
			if ((status == RCS_RECEIVING_ROOM_PLAYERS) != (mh->messageType == MFS_ROOM_STATUS_PLAYERS))
				{ assert(FALSE); return -1; }
			if ((status == RCS_RECEIVING_ROOM_CHAT_HISTORY) != (mh->messageType == MFS_ROOM_STATUS_CHAT_HISTORY))
				{ assert(FALSE); return -1; }
			
			if (numPlayers == 0 && status >= RCS_IN_ROOM)
				{ assert(FALSE); return -1; }
			
			if (status < RCS_IN_ROOM &&
				(mh->messageType < MFS_ROOM_STATUS_HEADER || mh->messageType > MFS_ROOM_STATUS_CHAT_HISTORY))
				{ assert(FALSE); return -1; }
		}
		
		err = 0;
		
		switch (mh->messageType) {
			
			// room messages
			
			case MFS_ROOM_HEARTBEAT:
				mtsRHM.messageType = MTS_ROOM_HEARTBEAT;
				mtsRHM.rtt = ((MFS_ROOM_HEARTBEAT_MESSAGE*)mh)->rtt;
				err = SendMessageToServer((char *)&mtsRHM, sizeof(MTS_ROOM_HEARTBEAT_MESSAGE));
				break;
			
			case MFS_ROOM_FULL:
				err = HandleMFS_ROOM_FULL(message, length);
				break;
			
			case MFS_ROOM_STATUS_HEADER:
				err = HandleMFS_ROOM_STATUS_HEADER(message, length);
				break;
			
			case MFS_ROOM_STATUS_TABLES:
				err = HandleMFS_ROOM_STATUS_TABLES(message, length);
				break;
			
			case MFS_ROOM_STATUS_PLAYERS:
				err = HandleMFS_ROOM_STATUS_PLAYERS(message, length);
				break;
			
			case MFS_ROOM_STATUS_CHAT_HISTORY:
				err = HandleMFS_ROOM_STATUS_CHAT_HISTORY(message, length);
				break;
			
			case MFS_ROOM_ENTER:
				err = HandleMFS_ROOM_ENTER(message, length);
				break;
			
			case MFS_ROOM_CHAT:
				err = HandleMFS_ROOM_CHAT(message, length);
				break;
			
			case MFS_ROOM_SIT:
				err = HandleMFS_ROOM_SIT(message, length);
				break;
			
			case MFS_ROOM_CHANGE_TABLE_SETTINGS:
				err = HandleMFS_ROOM_CHANGE_TABLE_SETTINGS(message, length);
				break;
			
			case MFS_ROOM_START_GAME:
				err = HandleMFS_ROOM_START_GAME(message, length);
				break;
			
			case MFS_ROOM_STAND:
				err = HandleMFS_ROOM_STAND(message, length);
				break;
			
			case MFS_ROOM_END_GAME:
				err = HandleMFS_ROOM_END_GAME(message, length);
				break;
			
			case MFS_ROOM_EXIT:
				err = HandleMFS_ROOM_EXIT(message, length);
				break;
			
			case MFS_ROOM_DISPLAY_STRING:
				err = HandleMFS_ROOM_DISPLAY_STRING(message, length);
				break;
			
			case MFS_ROOM_CHANGE_SHADOW:
				err = HandleMFS_ROOM_CHANGE_SHADOW(message, length);
				break;
			
			case MFS_ROOM_CHANGE_PROFILE:
				err = HandleMFS_ROOM_CHANGE_PROFILE(message, length);
				break;
			
			case MFS_ROOM_REQUEST:
				err = HandleMFS_ROOM_REQUEST(message, length);
				break;
			
			case MFS_ROOM_DISAPPROVE_REQUEST:
				err = HandleMFS_ROOM_DISAPPROVE_REQUEST(message, length);
				break;
			
			case MFS_ROOM_INVITE:
				err = HandleMFS_ROOM_INVITE(message, length);
				break;
			
			case MFS_ROOM_TI_CHECK_OUT_PAGE:
				err = HandleMFS_ROOM_TI_CHECK_OUT_PAGE(message, length);
				break;
			
			case MFS_ROOM_TI_ADD_TO_TEXT:
				err = HandleMFS_ROOM_TI_ADD_TO_TEXT(message, length);
				break;
			
			case MFS_ROOM_TI_JOIN:
				err = HandleMFS_ROOM_TI_JOIN(message, length);
				break;
			
			case MFS_ROOM_TI_SET_BANK:
				err = HandleMFS_ROOM_TI_SET_BANK(message, length);
				break;

			case MFS_ROOM_PLAYER_RATING:
				err = HandleMFS_ROOM_PLAYER_RATING(message, length);
				break;
			
			// game messages
			
			case MFS_GAME_STATE:
				err = HandleMFS_GAME_STATE(message, length);
				break;
			
			case MFS_GAME_CHAT:
				err = HandleMFS_GAME_CHAT(message, length);
				break;
			
			case MFS_GAME_BOOT:
				err = HandleMFS_GAME_BOOT(message, length);
				break;
			
			case MFS_GAME_RESTART:
				err = HandleMFS_GAME_RESTART(message, length);
				break;

			case MFS_GAME_DISPLAY_STRING:
				err = HandleMFS_GAME_DISPLAY_STRING(message, length);
				break;
			
			// assume it's a game-specific message
			
			default:
				mfs = (MFS_GAME_MESSAGE_HEADER *)message;
				if (mfs->table == -1)
					{ assert(FALSE); return -1; }
				if (mfs->table < 0 || mfs->table >= numTables)
					{ assert(FALSE); return -1; }
				if (tables[mfs->table].status != TS_DISPLAYED)
					{ assert(FALSE); return -1; }
				if (!tables[mfs->table].ContainsPlayerID(GetMyID(), NULL))
					{ assert(FALSE); return -1; }
				
				/*
				if (tables[mfs->table].status == TS_DISPLAYED && !tables[mfs->table].readyForMessages)
					{ QueueMessage(mfs->table, message, length); return 0; }
				*/
				
				err = tables[mfs->table].HandleMessage(mfs);
				break;
		}
		
		// keep the game up to date while a string of messages is being processed
		game->UpdateTime();
		
		if (err)
			return err;
		
		// if processing this message caused us no longer to be ready for messages,
		// we need to break out so the animation can finish before we try to get more messages
		for (i = 0 ; i < numTables ; i++)
			if (tables[i].status == TS_DISPLAYED && !tables[i].readyForMessages)
				return 0;

		// if the client has asked us to hold messages, don't process any
		if (HoldingMessages())
			return 0;
	}
	
	return 0;
}

void RoomClient::QueueMessage(int table, char *message, int length)
{
	QueuedMessage *qm;
	
	qm = new QueuedMessage();
	qm->table = table;
	qm->message = new char[length];
	memcpy(qm->message, message, length);
	qm->length = length;
	
	queue.AddToBack(qm);
}

int RoomClient::HandleMFS_ROOM_FULL(char *message, int length)
{
	char getS[256], getS2[256];
	MFS_ROOM_FULL_MESSAGE *mfs = (MFS_ROOM_FULL_MESSAGE *)message;
	
	ShowModelessMessage(0, GetString(13, getS), GetString(14, getS2), MB_OK);
	
	return 0;
}

int RoomClient::HandleMFS_ROOM_STATUS_HEADER(char *message, int length)
{
	MFS_ROOM_STATUS_HEADER_MESSAGE *mfs = (MFS_ROOM_STATUS_HEADER_MESSAGE *)message;
	
	// SetWindowText(MAIN_HWND, mfs->roomName); // get the name from WONplay
	numTables = mfs->numTables;
	numPlayersPromised = mfs->numPlayers;
	strcpy(wonParams.sponsorURL, mfs->sponsorURL);
	roomFlags = mfs->roomFlags;
	wonParams.adChangeRateInSeconds = mfs->adChangeRateInSeconds;
	strcpy(wonParams.adTags, mfs->adTags);
	
	status = RCS_RECEIVING_ROOM_TABLES;
	ShowStatus();
	
	return 0;
}

int RoomClient::HandleMFS_ROOM_STATUS_TABLES(char *message, int length)
{
	int i, b;
	MFS_ROOM_STATUS_TABLES_MESSAGE *mfs = (MFS_ROOM_STATUS_TABLES_MESSAGE *)message;
	
	i = mfs->index;
	if (i < 0 || i >= numTables)
		{ assert(FALSE); return -1; }
	
	tables[i].status = mfs->status;
	tables[i].joinOption = mfs->joinOption;
	tables[i].watchersOption = mfs->watchersOption;
	tables[i].muteWatchers = mfs->muteWatchers;
	tables[i].ratingsFlags = mfs->ratingsFlags;
	tables[i].ratingsSkills = mfs->ratingsSkills;
	tables[i].FillTableGameValues(mfs->gameValues);
	strcpy(tables[i].description, mfs->description);
	
	assert(MAX_GAME_PLAYERS <= 32);
	
	for (b = 0 ; b < MAX_GAME_PLAYERS ; b++)
		if (mfs->aiBits & (1 << b))
			tables[i].gameState.players[b].playerID = PLAYER_ID_AI;
	
	for (b = 0 ; b < MAX_GAME_PLAYERS ; b++)
		if (mfs->voidBits & (1 << b))
			tables[i].gameState.players[b].playerID = PLAYER_ID_VOID;

	if (mfs->lastMessageOfThisType) {
		status = RCS_RECEIVING_ROOM_PLAYERS;
		ShowStatus();
	}
	
	return 0;
}

int RoomClient::HandleMFS_ROOM_STATUS_PLAYERS(char *message, int length)
{
	int table, seat;
	MFS_ROOM_STATUS_PLAYERS_MESSAGE *mfs = (MFS_ROOM_STATUS_PLAYERS_MESSAGE *)message;
	
	if (mfs->terminator) {
		assert(numPlayers == numPlayersPromised);
		ModifyInitialPlayerSettings();
		status = RCS_RECEIVING_ROOM_CHAT_HISTORY;
		ShowStatus();
		
		return 0;
	}
	
	if (numPlayers == MAX_ROOM_PLAYERS)
		{ assert(FALSE); return -1; }
	players[numPlayers].info = mfs->info;
	strcpy(players[numPlayers].profile, mfs->profile);
	
	// if the player is sitting at a table, record that in the tables array
	
	table = players[numPlayers].info.table;
	seat = players[numPlayers].info.seat;
	if (table != -1) {
		assert(table >= 0 && table < numTables);
		if (table >= 0 && table < numTables) {
			assert(seat >= 0 && seat < (MAX_GAME_PLAYERS + MAX_GAME_WATCHERS));
			if (seat >= 0 && seat < MAX_GAME_PLAYERS) {
				assert(tables[table].gameState.players[seat].playerID == PLAYER_ID_EMPTY);
				tables[table].gameState.players[seat].playerID = players[numPlayers].info.playerID;
			}
			else
				tables[table].gameState.watcherIDs[seat - MAX_GAME_PLAYERS] = players[numPlayers].info.playerID;
		}
	}
	
	numPlayers++;
	
	ReorderPlayer(numPlayers - 1, TRUE); // put this new player in the proper sorted place in the array
	
	return 0;
}

int RoomClient::HandleMFS_ROOM_STATUS_CHAT_HISTORY(char *message, int length)
{
	MFS_ROOM_STATUS_CHAT_HISTORY_MESSAGE *mfs = (MFS_ROOM_STATUS_CHAT_HISTORY_MESSAGE *)message;
	WONroomFileDownload wonRoomFileDownload;
	
	if (mfs->terminator) {
		status = RCS_IN_ROOM;
		instructionsDialog->Show(TRUE);
		SetForegroundWindow(pApp->pMainPlane->GetWindow()->hWnd);
		
		if (won) {
			// tell WONplay it's OK to start downloading background files
			wonRoomFileDownload.nStatus = DWNLOAD_ON;
			won->SetRoomFileDownload(&wonRoomFileDownload);
		}
		
#ifdef _DEBUG
		WONadInfo wonAdInfo;
		
		strcpy(wonAdInfo.sAdPath, "c:\\program files\\won\\wonplay\\adcache\\pulling.gft");
		strcpy(wonAdInfo.sClickURL, "www.buddig.com");
		
		roomClient->SetCurrentAd(&wonAdInfo);
#endif
		
		ShowStatus();
		
		return 0;
	}
	
#ifdef NO_HW_ROOM_INTERFACE
	return 0;
#endif
	
	if (mfs->labelLength > 0) {
		mfs->text[mfs->labelLength] = 0;
		chatDisplay->AddAChatLine(mfs->text, &mfs->text[mfs->labelLength + 1], mfs->red, mfs->green, mfs->blue);
	}
	else
		chatDisplay->AddAChatLine("", mfs->text, mfs->red, mfs->green, mfs->blue);
	
	return 0;
}

int RoomClient::HandleMFS_ROOM_ENTER(char *message, int length)
{
	char getS[32];
	int i;
	MFS_ROOM_ENTER_MESSAGE *mfs = (MFS_ROOM_ENTER_MESSAGE *)message;
	
	// check if this player is us
	
	if (mfs->player.playerID == GetMyID()) {
		players[0].info = mfs->player;
		strcpy(players[0].profile, mfs->profile);
		if (players[0].IsMonitorOrSuperUser()) {
//			chatButtonWells[2]->Posn(6, 455);
			chatRadioButtons[2]->Posn(6, 455);
			chatRadioButtons[2]->SetText(GetString(11, getS, 32));
			chatPushButtons[2]->Posn(6, 455);
			chatPushButtons[2]->SetText(GetString(12, getS, 32));
		}
		ReorderPlayer(0, TRUE); // put ourselves in the proper sorted place in the array
		
		return 0; // we are already here
	}
	
	// make sure the player is not already here (an error)
	
	for (i = 1 ; i < numPlayers ; i++)
		if (players[i].info.playerID == mfs->player.playerID)
			{ assert(FALSE); return -1; }
	
	if (numPlayers == MAX_ROOM_PLAYERS)
		{ assert(FALSE); return -1; }
	
	players[numPlayers].info = mfs->player;
	strcpy(players[numPlayers].profile, mfs->profile);
	
	numPlayers++;
	
	assert(numPlayers == mfs->numPlayers);
	
	ReorderPlayer(numPlayers - 1, TRUE); // put this new player in the proper sorted place in the array
	
	ShowStatus();
	
	return 0;
}

int RoomClient::HandleMFS_ROOM_CHAT(char *message, int length)
{
	char label[MAX_PLAYER_NAME + 20], getS[256];
	int table;
	ROOMCLIENTPLAYER *p, *q;
	MFS_ROOM_CHAT_MESSAGE *mfs = (MFS_ROOM_CHAT_MESSAGE *)message;
	
	if (mfs->playerIDFrom != -1) { // -1 = from system
		p = GetPlayerFromID(mfs->playerIDFrom);
		if (p == NULL)
			{ assert(FALSE); return -1; }
		if (p->IsMuted() && !p->IsMonitorOrSuperUser())
			return 0;
	}
	
#ifdef NO_HW_ROOM_INTERFACE
	return 0;
#endif
	
	if (mfs->systemBroadcast || mfs->playerIDFrom == -1) {
		if (mfs->text[0] == '<') {
			if (players[0].info.table != -1)
				tables[players[0].info.table].AddTickerSegment(mfs->text);
			
			return 0;
		}
		
		game->sound1->Play(103);
		
		// if we are in a game, show a dialog box so we're sure not to miss the message
		
		table = players[0].info.table;
		if (table != -1 && tables[table].status == TS_DISPLAYED)
			tables[table].ShowModelessMessage(0, mfs->text, GetString(15, getS), MB_OK);
		
		if (mfs->playerIDFrom == -1)
			sprintf(label, "%s:", GetString(15, getS));
		else
			sprintf(label, "%s:", p->info.login);
	}
	else {
		if (mfs->privateIDTo != 0) {
			if (mfs->playerIDFrom == GetMyID()) {
				if (mfs->privateIDTo == -1)
					strcpy(label, GetString(16, getS));
				else {
					q = GetPlayerFromID(mfs->privateIDTo);
					if (q == NULL)
						strcpy(label, GetString(17, getS));
					else
						sprintf(label, GetString(18, getS), q->info.login);
				}
			}
			else {
				game->sound1->Play(102);
				if (mfs->privateIDTo != -1 && mfs->privateIDTo != GetMyID())
					{ assert(FALSE); return -1; }
				sprintf(label, GetString(19, getS), p->info.login);
			}
		}
		else
			sprintf(label, "%s:", p->info.login);
	}
	
	chatDisplay->AddAChatLine(label, mfs->text, mfs->red, mfs->green, mfs->blue);
	
	return 0;
}

int RoomClient::HandleMFS_ROOM_SIT(char *message, int length)
{
	// Boolean firstToSit = FALSE;
	int i;
	MFS_ROOM_SIT_MESSAGE *mfs = (MFS_ROOM_SIT_MESSAGE *)message;
	ROOMCLIENTPLAYER *p;
	GameState *gameState;
	
	if (mfs->playerID == PLAYER_ID_AI || mfs->playerID == PLAYER_ID_VOID) {
		assert(tables[mfs->table].status != TS_EMPTY);
		
		if (mfs->seat < MAX_GAME_PLAYERS) {
			gameState = &tables[mfs->table].gameState;
			
			assert(gameState->players[mfs->seat].playerID == PLAYER_ID_EMPTY);
			
			gameState->players[mfs->seat] = mfs->playerState;
		}
		else
			assert(FALSE); // AI's and voids can't watch
		
		if (tables[mfs->table].status == TS_DISPLAYED)
			tables[mfs->table].Display();
	}
	else {
		p = GetPlayerFromID(mfs->playerID);
		if (p == NULL)
			{ assert(FALSE); return -1; }
		
		if (p->info.table != -1 || p->info.seat != -1)
			assert(FALSE);
		
		for (i = 0 ; i < numPlayers ; i++)
			if (players[i].info.table == mfs->table && players[i].info.seat == mfs->seat)
				assert(FALSE);
		
		if (tables[mfs->table].status == TS_EMPTY) {
			tables[mfs->table].status = TS_PENDING;
			// firstToSit = TRUE;
		}
		
		p->info.table = mfs->table;
		p->info.seat = mfs->seat;
		if (tables[mfs->table].status == TS_PLAYING || tables[mfs->table].status == TS_DISPLAYED)
			p->info.flags |= UF_READY_TO_PLAY; // ready to play
		else
			p->info.flags &= (0xFF ^ UF_READY_TO_PLAY); // not ready to play
		
		tables[mfs->table].PlayerSits(p, &mfs->playerState);
	}
	
	if (mfs->playerID == GetMyID() && tablesDisplay)
		tablesDisplay->BringTableIntoFullView(mfs->table);
	
	UpdateRequestOrInvitation();
	
	ShowStatus();
	
	/*
	if (firstToSit && mfs->playerID == GetMyID()) {
		if (!optionsDialog) {
			optionsDialog = new OptionsDialogBox(pApp->pMainPlane);
			optionsDialog->Init();
		}
		optionsDialog->Show(ODBV_TABLE, FALSE);
	}
	*/
	
	return 0;
}

int RoomClient::HandleMFS_ROOM_CHANGE_TABLE_SETTINGS(char *message, int length)
{
	MFS_ROOM_CHANGE_TABLE_SETTINGS_MESSAGE *mfs = (MFS_ROOM_CHANGE_TABLE_SETTINGS_MESSAGE *)message;
	
	if (mfs->table < 0 || mfs->table >= numTables)
		{ assert(FALSE); return -1; }
	
	tables[mfs->table].joinOption = mfs->joinOption;
	tables[mfs->table].watchersOption = mfs->watchersOption;
	tables[mfs->table].muteWatchers = mfs->muteWatchers;
	tables[mfs->table].ratingsFlags = mfs->ratingsFlags;
	tables[mfs->table].ratingsSkills = mfs->ratingsSkills;
	tables[mfs->table].FillTableGameValues(mfs->gameValues);
	strcpy(tables[mfs->table].description, mfs->description);
	
	if (tables[mfs->table].status == TS_DISPLAYED)
		tables[mfs->table].Display();
	
	UpdateRequestOrInvitation();
	
	ShowStatus();
	
	return 0;
}

int RoomClient::HandleMFS_ROOM_START_GAME(char *message, int length)
{
	MFS_ROOM_START_GAME_MESSAGE *mfs = (MFS_ROOM_START_GAME_MESSAGE *)message;
	ROOMCLIENTPLAYER *p;
	
	if (mfs->table < 0 || mfs->table >= numTables)
		{ assert(FALSE); return -1; }
	
	if (tables[mfs->table].status == TS_PLAYING)
		{ assert(FALSE); return -1; }
	
	if (tables[mfs->table].status == TS_EMPTY)
		assert(FALSE);
	
	if (mfs->playerID != -1) {
		p = GetPlayerFromID(mfs->playerID);
		if (p == NULL)
			{ assert(FALSE); return -1; }
		
		assert(p->info.table == mfs->table && p->info.seat == mfs->seat);
		
		p->info.flags |= UF_READY_TO_PLAY; // ready to play
	}
	
	if (mfs->startGame)
		tables[mfs->table].status = TS_PLAYING;
	
	ShowStatus();
	
	return 0;
}

int RoomClient::HandleMFS_ROOM_STAND(char *message, int length)
{
	MFS_ROOM_STAND_MESSAGE *mfs = (MFS_ROOM_STAND_MESSAGE *)message;
	ROOMCLIENTPLAYER *p;
	
	p = GetPlayerFromID(mfs->playerID);
	if (p == NULL)
		{ assert(FALSE); return -1; }
	
	assert(p->info.table == mfs->table && p->info.seat == mfs->seat);
	
	if (mfs->table < 0 || mfs->table >= numTables)
		{ assert(FALSE); return -1; }
	
	if (tables[mfs->table].status == TS_EMPTY)
		{ assert(FALSE); return -1; }
	
	tables[mfs->table].PlayerStands(p, mfs->newID);
	
	p->info.table = -1;
	p->info.seat = -1;
	
	if (mfs->playerID == GetMyID()) { // we are standing
		tables[mfs->table].Clear();
		if (tables[mfs->table].status == TS_DISPLAYED)
			tables[mfs->table].status = TS_PLAYING;
	}
	
	UpdateRequestOrInvitation();
	
	ShowStatus();
	
	return 0;
}

int RoomClient::HandleMFS_ROOM_END_GAME(char *message, int length)
{
	int i;
	MFS_ROOM_END_GAME_MESSAGE *mfs = (MFS_ROOM_END_GAME_MESSAGE *)message;
	
	if (mfs->table < 0 || mfs->table >= numTables)
		{ assert(FALSE); return -1; }
	
	if (tables[mfs->table].status == TS_EMPTY)
		{ assert(FALSE); return -1; }
	
	if (tables[mfs->table].status == TS_DISPLAYED)
		tables[mfs->table].Clear();
	
	tables[mfs->table].Init(mfs->table);
	
	tables[mfs->table].joinOption = mfs->joinOption;
	tables[mfs->table].watchersOption = mfs->watchersOption;
	tables[mfs->table].muteWatchers = mfs->muteWatchers;
	tables[mfs->table].ratingsFlags = mfs->ratingsFlags;
	tables[mfs->table].ratingsSkills = mfs->ratingsSkills;
	tables[mfs->table].FillTableGameValues(mfs->gameValues);
	strcpy(tables[mfs->table].description, mfs->description);
	
	for (i = 0 ; i < numPlayers ; i++)
		if (players[i].info.table == mfs->table) {
			players[i].info.table = -1;
			players[i].info.seat = -1;
		}
	
	tables[mfs->table].status = TS_EMPTY;
	
	UpdateRequestOrInvitation();
	
	ShowStatus();
	
	return 0;
}

int RoomClient::HandleMFS_ROOM_EXIT(char *message, int length)
{
	int i;
	MFS_ROOM_EXIT_MESSAGE *mfs = (MFS_ROOM_EXIT_MESSAGE *)message;
	
	// we should never see ourselves exit
	
	if (mfs->player.playerID == GetMyID())
		{ assert(FALSE); return -1; }
	
	// make sure the player is here
	
	for (i = 1 ; i < numPlayers ; i++)
		if (players[i].info.playerID == mfs->player.playerID)
			break;
	
	if (i == numPlayers)
		{ assert(FALSE); return -1; }
	
	for ( ; i < (numPlayers - 1) ; i++)
		players[i] = players[i + 1];
	
	numPlayers--;
	
	assert(numPlayers == mfs->numPlayers);
	
	UpdateRequestOrInvitation();
	
	UpdateChatButtons(NULL);
	
	ShowStatus();
	
	return 0;
}

int RoomClient::HandleMFS_ROOM_DISPLAY_STRING(char *message, int length)
{
	MFS_ROOM_DISPLAY_STRING_MESSAGE *mfs = (MFS_ROOM_DISPLAY_STRING_MESSAGE *)message;
	
	ShowModelessMessage(0, mfs->text, mfs->title, MB_OK);
	
	return 0;
}

int RoomClient::HandleMFS_ROOM_CHANGE_SHADOW(char *message, int length)
{
	MFS_ROOM_CHANGE_SHADOW_MESSAGE *mfs = (MFS_ROOM_CHANGE_SHADOW_MESSAGE *)message;
	ROOMCLIENTPLAYER *p;
	
	p = GetPlayerFromID(mfs->playerID);
	if (p == NULL)
		{ assert(FALSE); return -1; }
	
	p->info.shadow = mfs->shadow;
	p->info.faceInfo = mfs->faceInfo;
	
	if (p->info.table != -1 && p->info.seat < MAX_GAME_PLAYERS)
		if (tables[p->info.table].status == TS_DISPLAYED)
			tables[p->info.table].Display();
	
	ShowStatus();
	
	return 0;
}

int RoomClient::HandleMFS_ROOM_CHANGE_PROFILE(char *message, int length)
{
	MFS_ROOM_CHANGE_PROFILE_MESSAGE *mfs = (MFS_ROOM_CHANGE_PROFILE_MESSAGE *)message;
	ROOMCLIENTPLAYER *p;
	
	p = GetPlayerFromID(mfs->playerID);
	if (p == NULL)
		{ assert(FALSE); return -1; }
	
	strnzcpy(p->profile, mfs->profile, MAX_PLAYER_PROFILE - 1);
	
	return 0;
}

int RoomClient::HandleMFS_ROOM_REQUEST(char *message, int length)
{
	MFS_ROOM_REQUEST_MESSAGE *mfs = (MFS_ROOM_REQUEST_MESSAGE *)message;
	
	if (mfs->playerID == GetMyID()) {
		// I am the requester
		
		assert(!mfs->cancel);
		// assert(requestingTable == -1); // allow one invitation after another
		assert(requesterPlayerID == -1);
		
		requestingTable = mfs->table;
		
		ShowStatus();
		
		return 0;
	}
	
	// I am the approver
	
	assert(players[0].info.table != -1);
	
	if (mfs->cancel) {
		assert(requesterPlayerID == mfs->playerID);
		requesterPlayerID = -1;
		requestedToWatch = -1;
		rsvpPlane->Hide();
	}
	else {
		assert(requestingTable == -1);
		assert(requesterPlayerID == -1);
		game->sound1->Play(100);
		requesterPlayerID = mfs->playerID;
		requestedToWatch = mfs->seat >= MAX_GAME_PLAYERS;
		rsvpPlane->Show(RSVP_REQUEST);
	}
	
	return 0;
}

int RoomClient::HandleMFS_ROOM_DISAPPROVE_REQUEST(char *message, int length)
{
	char getS[256], getS2[256];
	MFS_ROOM_DISAPPROVE_REQUEST_MESSAGE *mfs = (MFS_ROOM_DISAPPROVE_REQUEST_MESSAGE *)message;
	
	assert(requestingTable != -1);
	assert(requestingTable == mfs->table);
	
	requestingTable = -1;
	
	ShowStatus();
	
	if (mfs->notice == 1)
		ShowModelessMessage(0, GetString(20, getS), GetString(21, getS2), MB_OK);
	else
		ShowModelessMessage(0, GetString(22, getS), GetString(21, getS2), MB_OK);
	
	return 0;
}

int RoomClient::HandleMFS_ROOM_INVITE(char *message, int length)
{
	int invitationsOption;
	ROOMCLIENTPLAYER *p;
	MFS_ROOM_INVITE_MESSAGE *mfs = (MFS_ROOM_INVITE_MESSAGE *)message;
	MTS_ROOM_RSVP_MESSAGE mts;

	assert(inviterTable == -1);
	assert(inviterPlayerID == -1);

	p = GetPlayerFromID(mfs->inviterPlayerID);

	if (p == NULL) 
		{ assert(FALSE); return -1; }

	invitationsOption = globalIniFile->ReadValue("Invitations", INVITATIONS_ANYBODY);
	if (invitationsOption == INVITATIONS_NOBODY || (invitationsOption == INVITATIONS_FRIENDS && !p->IsFriend())) {
		mts.messageType = MTS_ROOM_RSVP;
		mts.accepted = FALSE;

		return roomClient->SendMessageToServer((char *)&mts, sizeof(MTS_ROOM_ANSWER_REQUEST_MESSAGE));
	}

	game->sound1->Play(101);
	
	inviterTable = mfs->table;
	inviterPlayerID = mfs->inviterPlayerID;
	invitedToWatch = mfs->toWatch;
	
	rsvpPlane->Show(RSVP_INVITE);
	
	return 0;
}

int RoomClient::HandleMFS_ROOM_TI_CHECK_OUT_PAGE(char *message, int length)
{
	MFS_ROOM_TI_CHECK_OUT_PAGE_MESSAGE *mfs = (MFS_ROOM_TI_CHECK_OUT_PAGE_MESSAGE *)message;
	
	// note that this could be a "fake" check-in where the server is giving us
	// the header information (mfs->playerID == -1), even though a player actually has
	// the file checked out (mfs->header.checkOutPlayerIDs[mfs->pageNumber])
	
	tournamentInformation.header.checkOutPlayerIDs[mfs->pageNumber] = mfs->header.checkOutPlayerIDs[mfs->pageNumber];
	
	if (mfs->playerID == -1)
		switch (mfs->pageNumber) {
			case TI_PAGE_CURRENT:
				tournamentInformation.header.sittingEnabled = mfs->header.sittingEnabled;
				tournamentInformation.header.joinEnabled = mfs->header.joinEnabled;
				tournamentInformation.header.startingBankValue = mfs->header.startingBankValue;
				break;
			case TI_PAGE_RULES:
				break;
			case TI_PAGE_ASSIGNMENTS:
				break;
			case TI_PAGE_AUTOMATIC:
				tournamentInformation.header.automaticResultsEnabled = mfs->header.automaticResultsEnabled;
				break;
			case TI_PAGE_OFFICIAL:
				break;
		}
	
	if (tournamentDialog)
		ShowHideTIControls(tournamentDialog->dialog);
	
	return 0;
}

int RoomClient::HandleMFS_ROOM_TI_ADD_TO_TEXT(char *message, int length)
{
	MFS_ROOM_TI_ADD_TO_TEXT_MESSAGE *mfs = (MFS_ROOM_TI_ADD_TO_TEXT_MESSAGE *)message;
	
	// clear the text if requested
	
	if (mfs->clear)
		tournamentInformation.text[mfs->pageNumber][0] = 0;
	
	// add the text
	
	if ((strlen(mfs->text) + strlen(tournamentInformation.text[mfs->pageNumber])) < MAX_TOURNAMENT_INFO)
		strcat(tournamentInformation.text[mfs->pageNumber], mfs->text);
	
	if (tournamentDialog)
		ShowHideTIControls(tournamentDialog->dialog);
	
	// the TI_PAGE_ASSIGNMENTS page is special because even though we may have it checked
	// out we want the text to update when this message is received
	
	if (mfs->pageNumber == TI_PAGE_ASSIGNMENTS && tournamentDialog && tournamentDialog->dialog)
		SetDlgItemText(tournamentDialog->dialog, IDC_TT_ASSIGNMENTS, roomClient->tournamentInformation.text[TI_PAGE_ASSIGNMENTS]);
	
	return 0;
}

int RoomClient::HandleMFS_ROOM_TI_JOIN(char *message, int length)
{
	MFS_ROOM_TI_JOIN_MESSAGE *mfs = (MFS_ROOM_TI_JOIN_MESSAGE *)message;
	ROOMCLIENTPLAYER *p;
	
	p = GetPlayerFromID(mfs->playerID);
	if (p == NULL)
		{ assert(FALSE); return -1; }
	
	if (mfs->joining) {
		p->info.flags |= UF_IN_TOURNAMENT; // joined the tournament
		p->info.tournamentBankRoll = mfs->startingBankValue;
	}
	else
		p->info.flags &= (0xFF ^ UF_IN_TOURNAMENT); // unjoined the tournament
	
	ShowStatus();
	
	if (mfs->playerID == GetMyID() && tournamentDialog)
		ShowHideTIControls(tournamentDialog->dialog);
	
	return 0;
}

int RoomClient::HandleMFS_ROOM_TI_SET_BANK(char *message, int length)
{
	MFS_ROOM_TI_SET_BANK_MESSAGE *mfs = (MFS_ROOM_TI_SET_BANK_MESSAGE *)message;
	ROOMCLIENTPLAYER *p;
	
	p = GetPlayerFromID(mfs->playerID);
	if (p == NULL)
		{ assert(FALSE); return -1; }
	
	p->info.tournamentBankRoll = mfs->bankValue;
	
	return 0;
}

int RoomClient::HandleMFS_ROOM_PLAYER_RATING(char *message, int length)
{
	MFS_ROOM_PLAYER_RATING_MESSAGE *mfs = (MFS_ROOM_PLAYER_RATING_MESSAGE *)message;
	ROOMCLIENTPLAYER *p;

	if(mfs->playerID == -1) //AI's results come back as -1
	{
		m_iAIRatingWins		= mfs->wins;
		m_iAIRatingLosses	= mfs->losses;
		m_iAIRatingDraws	= mfs->draws;
		m_iAIRatingPartials	= mfs->quits;
		m_iAIRating			= mfs->rating;
		m_iAIRatingDev		= mfs->ratingDev;
	}
	else
	{
		p = GetPlayerFromID(mfs->playerID);
		
		p->info.wins		= mfs->wins;
		p->info.losses		= mfs->losses;
		p->info.draws		= mfs->draws;
		p->info.quits		= mfs->quits;
		p->info.rating		= mfs->rating;
		p->info.ratingDev	= mfs->ratingDev;
	}
	
	return 0;
}

int RoomClient::HandleMFS_GAME_STATE(char *message, int length)
{
	void *data;
	MFS_GAME_STATE_MESSAGE *mfs = (MFS_GAME_STATE_MESSAGE *)message;
	
	// I only care about states for the table I'm playing at (or watching at)
	
	if (mfs->table != players[0].info.table)
		{ assert(FALSE); return -1; }
	
	assert(tables[mfs->table].status == TS_PLAYING || tables[mfs->table].status == TS_DISPLAYED);
	assert(mfs->status == TS_PLAYING);
	
	tables[mfs->table].status = TS_DISPLAYED;

#if !defined(SHOW_ALL_CARDS)
	ClearSecretGameStateInformation(players[0].info.seat, &mfs->gameState);
#endif
	
	data = tables[mfs->table].GetPersistentGameStateData();
	tables[mfs->table].gameState = mfs->gameState;
	tables[mfs->table].SetPersistentGameStateData(data);
	
	tables[mfs->table].Clear();
	tables[mfs->table].SetReadyForMessages(TRUE);
	tables[mfs->table].DisplayForNewGameState();
	
	ShowStatus();
	
	return 0;
}

int RoomClient::HandleMFS_GAME_CHAT(char *message, int length)
{
	MFS_GAME_CHAT_MESSAGE *mfs = (MFS_GAME_CHAT_MESSAGE *)message;
	ROOMCLIENTPLAYER *p;
	
	// I only care about chat for the table I'm playing at (or watching at)
	
	if (mfs->table != players[0].info.table)
		{ assert(FALSE); return -1; }
	
	if (tables[mfs->table].status != TS_DISPLAYED)
		{ assert(FALSE); return -1; }
	
	p = GetPlayerFromID(mfs->playerID);
	if (p == NULL)
		{ assert(FALSE); return -1; }
	
	if (p->info.table != mfs->table)
		{ assert(FALSE); return -1; }
	
	tables[mfs->table].Chat(p, mfs->sound, mfs->text);
	
	return 0;
}

int RoomClient::HandleMFS_GAME_BOOT(char *message, int length)
{
	MFS_GAME_BOOT_MESSAGE *mfs = (MFS_GAME_BOOT_MESSAGE *)message;
	ROOMCLIENTPLAYER *p;
	
	// I only care about boot messages for the table I'm playing at (or watching at)
	
	if (mfs->table != players[0].info.table)
		{ assert(FALSE); return -1; }
	
	if (tables[mfs->table].status != TS_DISPLAYED)
		{ assert(FALSE); return -1; }
	
	p = GetPlayerFromID(mfs->playerID);
	if (p == NULL)
		{ assert(FALSE); return -1; }
	
	if (p->info.table != mfs->table)
		{ assert(FALSE); return -1; }
	
	tables[mfs->table].gameState.players[mfs->seat].bootCount = mfs->bootCount;
	
	tables[mfs->table].Display();
	
	return 0;
}

int RoomClient::HandleMFS_GAME_RESTART(char *message, int length)
{
	int i, n, m, voteSeat;
	char s[256], getS[256];
	MFS_GAME_RESTART_MESSAGE *mfs = (MFS_GAME_RESTART_MESSAGE *)message;
	ROOMCLIENTPLAYER *p = NULL;
	
	// I only care about messages for the table I'm playing at
	
	if (mfs->table != players[0].info.table)
		{ assert(FALSE); return -1; }
	
	if (tables[mfs->table].status != TS_DISPLAYED)
		{ assert(FALSE); return -1; }

	if (!IsRestartAllowed())
		{ assert(FALSE); return -1; }
	
	for (i = 0, n = 0, m = 0; i < MAX_GAME_PLAYERS; i++) {
		if (tables[mfs->table].restartVotes[i])
			n++;

		tables[mfs->table].restartVotes[i] = mfs->restartVotes[i];

		if (tables[mfs->table].restartVotes[i]) {
			m++;
			voteSeat = i;
		}
	}

	//If one person has voted to restart the game...
	if (n == 0 && m == 1 && players[0].info.seat < MAX_GAME_PLAYERS) { //Watchers don't get the dialog...
		if (voteSeat != players[0].info.seat) { //Don't show the dialog for the player who just voted...
			//Show dialog
			assert(voteSeat >= 0 && voteSeat < MAX_GAME_PLAYERS);
			p = GetPlayerFromID(tables[mfs->table].gameState.players[voteSeat].playerID);

			if (p == NULL)
				{ assert(FALSE); return -1; }

			sprintf(s, GetString(23, getS), p->info.login);
			tables[mfs->table].ShowModelessMessage(0, s, GetString(24, getS), MB_YESNO, DBP_RESTART_GAME);
		}
	}
	
	tables[mfs->table].Display();
	
	return 0;
}

int RoomClient::HandleMFS_GAME_DISPLAY_STRING(char *message, int length)
{
	MFS_GAME_DISPLAY_STRING_MESSAGE *mfs = (MFS_GAME_DISPLAY_STRING_MESSAGE *)message;
	
	// I only care about strings for the table I'm playing at (or watching at)
	
	if (mfs->table != players[0].info.table)
		{ assert(FALSE); return -1; }
	
	if (tables[mfs->table].status != TS_DISPLAYED)
		{ assert(FALSE); return -1; }
	
	if (!strcmp(mfs->title, "<SETTICKER>"))
		tables[mfs->table].SetTickerText(mfs->text);
	else
		tables[mfs->table].ShowModelessMessage(0, mfs->text, mfs->title, MB_OK);
	
	return 0;
}

void RoomClient::UpdateRequestOrInvitation()
{
	ROOMCLIENTPLAYER *p;
	
	// this function is called after something happens that could change
	// our status as an requester, approver, inviter, or invitee
	
	// we don't want to show requests/invitations that are no longer possible
	
	// if I am seated, I can't have my requestingTable set
	
	if (players[0].info.table != -1)
		requestingTable = -1;
	
	// if I am standing or a watcher, I can't be an approver
	
	if (players[0].info.table == -1 || players[0].info.seat >= MAX_GAME_PLAYERS) {
		requesterPlayerID = -1;
		requestedToWatch = FALSE;
		
		if (rsvpPlane && rsvpPlane->IsNotHidden() && rsvpPlane->version == RSVP_REQUEST)
			rsvpPlane->Hide();
	}
	
	// I can't be approving someone who is seated or who is not present
	
	if (requesterPlayerID != -1) {
		p = GetPlayerFromID(requesterPlayerID);
		if (p == NULL || p->info.table != -1) {
			requesterPlayerID = -1;
			requestedToWatch = FALSE;
			
			if (rsvpPlane && rsvpPlane->IsNotHidden() && rsvpPlane->version == RSVP_REQUEST)
				rsvpPlane->Hide();
		}
	}
	
	// if I am seated, I can't be invited
	
	if (players[0].info.table != -1) {
		inviterTable = -1;
		inviterPlayerID = -1;
	}
}

Boolean RoomClient::PlayerComesLaterInList(ROOMCLIENTPLAYER *a, ROOMCLIENTPLAYER *b)
{
	char *p, *q;
	
	// the players list is sorted as follows:
	// player at this computer
	// friends, sorted alphabetically
	// monitors (who are not also friends), sorted alphabetically
	// tournament directors and tournament admin's (who are not also friends or monitors), sorted alphabetically
	// regular players, sorted alphabetically
	// muted players (whose friends flag is ignored), sorted alphabetically
	
	if (a == b) return FALSE;
	
	if (a == &players[0]) return FALSE;
	if (b == &players[0]) return TRUE;
	
	for (p = a->info.login ; p[0] && !isalnum(p[0]) ; p++) ;
	if (!p[0])
		p = a->info.login;
	
	for (q = b->info.login ; q[0] && !isalnum(q[0]) ; q++) ;
	if (!q[0])
		q = b->info.login;
	
	if (a->IsFriend() && b->IsFriend())
		return lstrcmpi(p, q) > 0;
	
	if (a->IsFriend()) return FALSE;
	if (b->IsFriend()) return TRUE;
	
	if (a->IsMonitor() && b->IsMonitor())
		return lstrcmpi(p, q) > 0;
	
	if (a->IsMonitor()) return FALSE;
	if (b->IsMonitor()) return TRUE;
	
	if (a->IsRefereeOrTA() && b->IsRefereeOrTA())
		return lstrcmpi(p, q) > 0;
	
	if (a->IsRefereeOrTA()) return FALSE;
	if (b->IsRefereeOrTA()) return TRUE;
	
	if (a->IsMuted() && b->IsMuted())
		return lstrcmpi(p, q) > 0;
	
	if (a->IsMuted()) return TRUE;
	if (b->IsMuted()) return FALSE;
	
	return lstrcmpi(p, q) > 0;
}

void RoomClient::ReorderPlayer(int n, Boolean initialize)
{
	// char *s;
	int i, j;
	// WonErr wonErr;
	ROOMCLIENTPLAYER player;
	
	if (n < 0 || n >= numPlayers)
		return;
	
	if (initialize && n != 0 /* && won */) {
		// mark the player as a friend or muted, if appropriate
		
		players[n].isFriend = IsPlayerFriend(players[n].info.login);
		players[n].isMuted = IsPlayerMuted(players[n].info.login);
		players[n].isSelected = FALSE;
		
		/*
		s = new char[nWON_PLAYER_NAME * nWON_MAX_FRIENDS + 1];
		if (s) {
			wonErr = won->GetFriendList(s, NULL);
			if (WONSUCCESS(wonErr))
				if (strizzmember(s, players[n].info.login))
					players[n].isFriend = TRUE;
			delete [] s;
		}
		
		s = new char[nWON_PLAYER_NAME * nWON_MAX_FOES + 1];
		if (s) {
			wonErr = won->GetFoeList(s, NULL);
			if (WONSUCCESS(wonErr))
				if (strizzmember(s, players[n].info.login))
					players[n].isMuted = TRUE;
			delete [] s;
		}
		*/
	}
	
	if (n == 0)
		return; // we are always in the correct place
	
	// the players list is assumed to be in order, except for player n
	
	// remove the player from the list
	
	player = players[n];
	
	for (i = n ; i < (numPlayers - 1) ; i++)
		players[i] = players[i + 1];
	
	// insert the player in the right place in the list
	
	for (i = 1 ; i < (numPlayers - 1) ; i++)
		if (PlayerComesLaterInList(&players[i], &player))
			break;
	
	for (j = numPlayers - 1 ; j > i ; j--)
		players[j] = players[j - 1];
	
	players[i] = player;
}

// This sorts in descending order (larger numbers come first).
int ComparePlayersByRating(const void *elem1, const void *elem2)
{
	if((*((ROOMCLIENTPLAYER **)elem1))->info.rating < (*((ROOMCLIENTPLAYER **)elem2))->info.rating)
		return 1;
	else if((*((ROOMCLIENTPLAYER **)elem1))->info.rating > (*((ROOMCLIENTPLAYER **)elem2))->info.rating)
		return -1;
	else
		return 0;
}

void RoomClient::GetTop10Players(PlayerNameRatings *prTop10)
{
	ROOMCLIENTPLAYER *apoSortedPlayers[MAX_ROOM_PLAYERS];
	int i;
	int iNumInList = 0;

	memset(prTop10, 0, sizeof(*prTop10) * 10);

	for(i = 0; i < numPlayers; i++)
	{
		apoSortedPlayers[i] = &players[i];
	}

	qsort(apoSortedPlayers, numPlayers, sizeof(apoSortedPlayers[0]), ComparePlayersByRating);

	for(i = 0; i < MAX_ROOM_PLAYERS; i++)
	{
		if(i >= numPlayers)
			continue;

		if(0 == (apoSortedPlayers[i]->info.wins + apoSortedPlayers[i]->info.losses +
			apoSortedPlayers[i]->info.draws + apoSortedPlayers[i]->info.quits))
			continue;

		strcpy(prTop10[iNumInList].acName, apoSortedPlayers[i]->info.login);
		prTop10[iNumInList].sRating = apoSortedPlayers[i]->info.rating;
		iNumInList++;

		if(iNumInList >= 10)
			break;
	}
}

void RoomClient::ShowModelessMessage(int viewNumber, char *text, char *caption, unsigned int uType, int purpose)
{
#ifdef NO_HW_ROOM_INTERFACE
	return;
#endif
	
	roomMessageBox->Show(viewNumber, text, caption, uType, purpose);
}

int RoomClient::SendMessageToServer(char *message, int length)
{
	char getS[256];
	int err;
	
	assert(server != 0);
	if (server == 0)
		return -1;
	
	err = connections->PostMessage(server, message, length);
	if (err)
		ReportError(GetString(8, getS), err);
	
	return err;
}

ROOMCLIENTPLAYER *RoomClient::GetPlayerFromID(int playerID)
{
	int i;
	
	for (i = 0 ; i < numPlayers ; i++)
		if (players[i].info.playerID == playerID)
			return &players[i];
	
	return NULL;
}

int RoomClient::GetPlayerIndexFromID(int playerID)
{
	int i;
	
	for (i = 0 ; i < numPlayers ; i++)
		if (players[i].info.playerID == playerID)
			return i;
	
	return -1;
}

int RoomClient::GetMyID()
{
	if (numPlayers <= 0)
		return -1;
	
	return players[0].info.playerID;
}

void RoomClient::ChatSoundKeyPressed(char c)
{
	MTS_GAME_CHAT_MESSAGE mtsGCM;
	static DWORD lastTicks = 0;
	
	if (players[0].info.table == -1 || tables[players[0].info.table].status != TS_DISPLAYED)
		return;
	
	if (GetTickCount() < (lastTicks + 2000))
		return;
	
	lastTicks = GetTickCount();
	
	mtsGCM.messageType = MTS_GAME_CHAT;
	mtsGCM.playerID = GetMyID();
	mtsGCM.table = players[0].info.table;
	mtsGCM.seat = players[0].info.seat;
	mtsGCM.flags = GCM_SOUND_KEY;
	mtsGCM.sound = c;
	mtsGCM.text[0] = 0;
	SendMessageToServer((char *)&mtsGCM, sizeof(MTS_GAME_CHAT_MESSAGE));
}

void RoomClient::HoldMessages(int numTicks)
{
	assert(numTicks > 0);

	holdTicks = game->GetTime();
	numHoldTicks = numTicks;
}

Boolean RoomClient::HoldingMessages()
{
	if (numHoldTicks == 0)
		return FALSE;

	if (game->GetTime() - holdTicks < numHoldTicks)
		return TRUE;

	numHoldTicks = 0;
	holdTicks = 0;
	return FALSE;
}

void RoomClient::PreferencesChanged()
{
	int i;
	
	for (i = 0 ; i < numTables ; i++)
		if (tables[i].status == TS_DISPLAYED && tables[i].display)
		{
			tables[i].display->PreferencesChanged();
			tables[i].Display();
		}
	
	ShowStatus();
}

void RoomClient::UpdateChatButtons(ROOMCLIENTPLAYER *player)
{
	char s[256];
	i18nStr getS;
	int i, n, chatControls;
	
	for (i = 1, n = 0 ; i < numPlayers ; i++)
		if (roomClient->players[i].isSelected)
			n++;
	
	if (n == 0) {
		chatGroup->SelectButtonByID(0);
		chatRadioButtons[1]->SetText(GetString(10, getS));
		chatRadioButtons[1]->Disable();
		chatPushButtons[1]->Disable();
		chatPushButtons[1]->SetAlpha(170);
		privateChatCount->SetText("");
	}
	else {
		// "Private (%d)"
		sprintf(s, GetString(370, getS), n);
		chatRadioButtons[1]->SetText(s);
		chatRadioButtons[1]->Enable();
		chatPushButtons[1]->Enable();
		chatPushButtons[1]->SetAlpha(255);
		sprintf(s, "(%d)", n);
		privateChatCount->SetText(s);
	}
	
	if (player && player->isSelected)
		chatGroup->SelectButtonByID(1);
	
	chatControls = globalIniFile->ReadValue("ChatControls2", CHAT_CONTROLS_PUSH_BUTTONS);
	
	if (chatControls == CHAT_CONTROLS_RADIO_BUTTONS) {
		chatGroup->Show();
		for (i = 0 ; i < 3 ; i++)
			chatPushButtons[i]->Hide();
		privateChatCount->Hide();
	}
	else {
		chatGroup->Hide();
		for (i = 0 ; i < 3 ; i++)
			chatPushButtons[i]->Show();
		privateChatCount->Show();
	}
}

////////////////

Boolean RoomClient::IsPlayerInINIGroup(char *login, char *group)
{
	char name[MAX_PLAYER_NAME + 1], s[256];
	int i;
	
	for (i = 0 ; i < 256 ; i++) {
		sprintf(s, "%s%d", group, i);
		globalIniFile->ReadString(s, "", name, MAX_PLAYER_NAME);
		if (!lstrcmpi(name, login))
			return TRUE;
	}
	
	return FALSE;
}

void RoomClient::AddPlayerToINIGroup(char *login, char *group)
{
	char name[MAX_PLAYER_NAME + 1], s[256];
	int i;
	
	for (i = 0 ; i < 256 ; i++) {
		sprintf(s, "%s%d", group, i);
		globalIniFile->ReadString(s, "", name, MAX_PLAYER_NAME);
		if (name[0] == 0) {
			globalIniFile->WriteString(s, login);
			break;
		}
	}
}

void RoomClient::DeletePlayerFromINIGroup(char *login, char *group)
{
	char name[MAX_PLAYER_NAME + 1], s[256];
	int i;
	
	for (i = 0 ; i < 256 ; i++) {
		sprintf(s, "%s%d", group, i);
		globalIniFile->ReadString(s, "", name, MAX_PLAYER_NAME);
		if (!lstrcmpi(name, login)) {
			globalIniFile->WriteString(s, NULL);
			break;
		}
	}
}

Boolean RoomClient::IsPlayerFriend(char *login) { return IsPlayerInINIGroup(login, "F"); }
Boolean RoomClient::IsPlayerMuted(char *login) { return IsPlayerInINIGroup(login, "M"); }

void RoomClient::AddFriend(char *login) { AddPlayerToINIGroup(login, "F"); }
void RoomClient::AddFoe(char *login) { AddPlayerToINIGroup(login, "M"); }

void RoomClient::DeleteFriend(char *login) { DeletePlayerFromINIGroup(login, "F"); }
void RoomClient::DeleteFoe(char *login) { DeletePlayerFromINIGroup(login, "M"); }

////////////////

void RoomClient::DoChatCommand(ChatLog *chatLog, char *c)
{
	char mts[1024], command[32], path[MAX_PATH], getS[256], getS2[256];
	int i, j, n, table, seat, shadow, playerID;
	MTS_ROOM_CHAT_MESSAGE *mtsRCM;
	MTS_ROOM_SIT_MESSAGE *mtsRSITM;
	MTS_ROOM_START_GAME_MESSAGE *mtsRSGM;
	MTS_ROOM_STAND_MESSAGE *mtsRSTANDM;
	MTS_GAME_CHAT_MESSAGE *mtsGCM;
	MTS_ROOM_CHANGE_SHADOW_MESSAGE *mtsCSM;
	MTS_ROOM_REQUEST_MESSAGE *mtsRRM;
	MTS_ROOM_INVITE_MESSAGE *mtsRIM;
	
	if (chatLog != NULL) { // game chat
		if (chatLog->table == -1 || tables[chatLog->table].status != TS_DISPLAYED)
			{ assert(FALSE); return; }
		
		for (i = 0 ; c[i] == ' ' || c[i] == '\t' ; i++) ;
		if (c[i] == 0)
			return; // don't send empty chat lines
		
		mtsGCM = (MTS_GAME_CHAT_MESSAGE *)mts;
		mtsGCM->messageType = MTS_GAME_CHAT;
		mtsGCM->playerID = GetMyID();
		mtsGCM->table = chatLog->table;
		mtsGCM->seat = chatLog->seat;
		mtsGCM->flags = GCM_NORMAL;
		mtsGCM->sound = 0;
		strcpy(mtsGCM->text, c);
		SendMessageToServer(mts, sizeof(MTS_GAME_CHAT_MESSAGE) + strlen(mtsGCM->text));
		
		return;
	}
	
	if (!strncmp(c, "rchat", 5)) { // room chat
		for (i = 6 ; c[i] == ' ' || c[i] == '\t' ; i++) ;
		if (c[i] == 0)
			return; // don't send empty chat lines
		mtsRCM = (MTS_ROOM_CHAT_MESSAGE *)mts;
		mtsRCM->messageType = MTS_ROOM_CHAT;
		mtsRCM->playerIDFrom = GetMyID();
		
		// send private chats to up to 100 players
		n = 0;
		if (chatRadioButtons[1]->IsSelected()) // private
			for (j = 1, n = 0 ; j < numPlayers && n < 100 ; j++)
				if (players[j].isSelected) {
					*(int *)(mtsRCM->data + n * sizeof(int)) = players[j].info.playerID;
					n++;
				}
		
		mtsRCM->numPrivateIDsTo = n;
		if (n == 0 && chatRadioButtons[2]->IsSelected() && players[0].IsMonitorOrSuperUser())
			mtsRCM->systemBroadcast = TRUE;
		else
			mtsRCM->systemBroadcast = FALSE;
		strcpy(mtsRCM->data + n * sizeof(int), &c[i]);
		SendMessageToServer(mts, sizeof(MTS_ROOM_CHAT_MESSAGE) + n * sizeof(int) + strlen(&c[i]) + 1);
	}
	
	if (!strncmp(c, "changeshadow", 12)) {
		mtsCSM = (MTS_ROOM_CHANGE_SHADOW_MESSAGE *)mts;
		sscanf(c, "%s %d", command, &shadow);
		mtsCSM->messageType = MTS_ROOM_CHANGE_SHADOW;
		mtsCSM->playerID = GetMyID();
		mtsCSM->shadow = shadow;
		GetMyFace(&mtsCSM->faceInfo, path);
		SendMessageToServer(mts, sizeof(MTS_ROOM_CHANGE_SHADOW_MESSAGE));
	}
	
	if (!strncmp(c, "sit", 3)) { // sit at table to play
		sscanf(c, "%s %d %d", command, &table, &seat);
		
		for (i = 0 ; i < numPlayers ; i++)
			if (players[i].info.table == table && players[i].info.seat == seat)
				{ ShowModelessMessage(0, GetString(25, getS), GetString(26, getS2), MB_OK); return; }
		
		//If the table is by approval AND the player isn't the monitor AND the player currently 
		// isn't playing at the table, then prompt to get approval.
		if (tables[table].joinOption == TJO_BY_APPROVAL && !players[0].IsMonitorOrSuperUser() &&
			!(players[0].info.table == table && players[0].info.seat < MAX_GAME_PLAYERS)) {
			if (players[0].info.table != -1 && players[0].info.seat < MAX_GAME_PLAYERS)
				{ ShowModelessMessage(0, GetString(27, getS), GetString(26, getS2), MB_OK); return; }

			mtsRRM = (MTS_ROOM_REQUEST_MESSAGE *)mts;
			mtsRRM->messageType = MTS_ROOM_REQUEST;
			mtsRRM->playerID = GetMyID();
			mtsRRM->table = table;
			mtsRRM->seat = seat;
			SendMessageToServer(mts, sizeof(MTS_ROOM_REQUEST_MESSAGE));
		}
		else {
			mtsRSITM = (MTS_ROOM_SIT_MESSAGE *)mts;
			mtsRSITM->messageType = MTS_ROOM_SIT;
			mtsRSITM->playerID = GetMyID();
			mtsRSITM->table = table;
			mtsRSITM->seat = seat;
			SendMessageToServer(mts, sizeof(MTS_ROOM_SIT_MESSAGE));
		}
	}
	
	if (!strncmp(c, "watch", 5)) { // sit at table to watch
		if (players[0].info.table != -1)
			{ ShowModelessMessage(0, GetString(27, getS), GetString(26, getS2), MB_OK); return; }
		
		sscanf(c, "%s %d", command, &table);
		
		if (tables[table].watchersOption == TWO_BY_APPROVAL && !players[0].IsMonitorOrSuperUser()) {
			mtsRRM = (MTS_ROOM_REQUEST_MESSAGE *)mts;
			mtsRRM->messageType = MTS_ROOM_REQUEST;
			mtsRRM->playerID = GetMyID();
			mtsRRM->table = table;
			mtsRRM->seat = MAX_GAME_PLAYERS;
			SendMessageToServer(mts, sizeof(MTS_ROOM_REQUEST_MESSAGE));
		}
		else {
			mtsRSITM = (MTS_ROOM_SIT_MESSAGE *)mts;
			mtsRSITM->messageType = MTS_ROOM_SIT;
			mtsRSITM->playerID = GetMyID();
			mtsRSITM->table = table;
			mtsRSITM->seat = MAX_GAME_PLAYERS;
			SendMessageToServer(mts, sizeof(MTS_ROOM_SIT_MESSAGE));
		}
	}
	
	if (!strncmp(c, "start", 5)) { // start the game
		if (players[0].info.table == -1)
			{ ShowModelessMessage(0, GetString(28, getS), GetString(29, getS2), MB_OK); return; }
		
		if (players[0].info.seat >= MAX_GAME_PLAYERS)
			{ ShowModelessMessage(0, GetString(30, getS), GetString(29, getS2), MB_OK); return; }
		
		if (tables[players[0].info.table].status != TS_PENDING)
			{ ShowModelessMessage(0, GetString(31, getS), GetString(29, getS2), MB_OK); return; }
		
		mtsRSGM = (MTS_ROOM_START_GAME_MESSAGE *)mts;
		mtsRSGM->messageType = MTS_ROOM_START_GAME;
		mtsRSGM->playerID = GetMyID();
		mtsRSGM->table = players[0].info.table;
		mtsRSGM->seat = players[0].info.seat;
		SendMessageToServer(mts, sizeof(MTS_ROOM_START_GAME_MESSAGE));
	}
	
	if (!strncmp(c, "invite", 6)) { // invite another player
		if (players[0].info.table == -1)
			{ ShowModelessMessage(0, GetString(32, getS), GetString(33, getS2), MB_OK); return; }
		
		sscanf(c, "%s %d", command, &playerID);
		
		mtsRIM = (MTS_ROOM_INVITE_MESSAGE *)mts;
		mtsRIM->messageType = MTS_ROOM_INVITE;
		mtsRIM->table = players[0].info.table;
		mtsRIM->inviteePlayerID = playerID;
		mtsRIM->toWatch = strstr(c, "watch") ? TRUE : FALSE;
		SendMessageToServer(mts, sizeof(MTS_ROOM_REQUEST_MESSAGE));
	}
	
	if (!strncmp(c, "gchat", 5)) { // game chat
		if (players[0].info.table == -1 || tables[players[0].info.table].status != TS_DISPLAYED)
			{ ShowModelessMessage(0, GetString(34, getS), GetString(35, getS2), MB_OK); return; }
		
		mtsGCM = (MTS_GAME_CHAT_MESSAGE *)mts;
		mtsGCM->messageType = MTS_GAME_CHAT;
		mtsGCM->playerID = GetMyID();
		mtsGCM->table = players[0].info.table;
		mtsGCM->seat = players[0].info.seat;
		mtsGCM->flags = 0;
		strcpy(mtsGCM->text, &c[6]);
		SendMessageToServer(mts, sizeof(MTS_GAME_CHAT_MESSAGE) + strlen(mtsGCM->text));
	}
	
	if (!strncmp(c, "stand", 5)) { // leave the game
		if (players[0].info.table == -1)
			{ ShowModelessMessage(0, GetString(36, getS), GetString(37, getS2), MB_OK); return; }
		
		mtsRSTANDM = (MTS_ROOM_STAND_MESSAGE *)mts;
		mtsRSTANDM->messageType = MTS_ROOM_STAND;
		mtsRSTANDM->playerID = GetMyID();
		mtsRSTANDM->table = players[0].info.table;
		mtsRSTANDM->seat = players[0].info.seat;
		SendMessageToServer(mts, sizeof(MTS_ROOM_STAND_MESSAGE));
	}
}

//////////////////////////////////////////////////////////////////////////////////////////

RoomClientConnections::RoomClientConnections(int nclients, int nlisteners) :
	TConnectionMgr(nclients, nlisteners)
{
}

RoomClientConnections::~RoomClientConnections()
{
}

//======================================================================
// We are going to process some messages here rather than let them go
// on the message queue.
//======================================================================
bool RoomClientConnections::PreProcessMessage(HCONNECTION c, const void *message, int length)
{ 
	MTS_ROOM_HEARTBEAT_MESSAGE mtsRHM;
	MESSAGE_HEADER *mh;
	int err;

	mh = (MESSAGE_HEADER *)message;
	switch (mh->messageType)
	{
	case MFS_ROOM_HEARTBEAT:
		mtsRHM.messageType = MTS_ROOM_HEARTBEAT;
		mtsRHM.rtt = ((MFS_ROOM_HEARTBEAT_MESSAGE*)mh)->rtt;
		err = roomClient->SendMessageToServer((char *)&mtsRHM, sizeof(MTS_ROOM_HEARTBEAT_MESSAGE));
		break;

	case MFS_GAME_CHAT:
		err = roomClient->HandleMFS_GAME_CHAT((char *)message, length);
		break;

	case MFS_ROOM_CHAT:
		err = roomClient->HandleMFS_ROOM_CHAT((char *)message, length);
		break;
			
	default:
		return false;
	}

	return true;
}

void RoomClientConnections::ConnectionCompleted(HCONNECTION c)
{
	char path[MAX_PATH], getS[256], getS2[256];
	MTS_ROOM_ENTER_MESSAGE mts;
	
	assert(c->GetID() == 1000);
	
	if (GetConnectionStatus(c) != CS_CONNECTED) {
		roomClient->ShowModelessMessage(0, GetString(38, getS), GetString(39, getS2), MB_OK);
		roomClient->server = NULL;
		roomClient->ShowStatus();
		
		return;
	}
	
	roomClient->server = c;
	
//	roomClient->ShowStatus();
	
	mts.messageType = MTS_ROOM_ENTER;
	strcpy(mts.login, wonParams.userName);
	// strcpy(mts.nickname, wonParams.userName);
	roomClient->GetMyFace(&mts.faceInfo, path);
	mts.level = IHaveAHoyleCDProductInstalled(CUSTOMER_STRING_1, CUSTOMER_STRING_2, CUSTOMER_STRING_3) ? UL_CUSTOMER : UL_FREEBIE;
#ifdef _DEBUG		
	/*
	if (strchr(userName, 'H') != NULL)
		mts.level |= UL_MODERATOR;
	*/
#endif
	mts.databaseID = wonParams.databaseID;
	mts.roomID = wonParams.roomID;
	if (mts.roomID == 0)
		{ MessageBoxA(NULL, GetString(294, getS), (char *)szAppName, MB_OK | MB_ICONSTOP | MB_TASKMODAL); game->QuitGame(TRUE); return; }
	mts.machineID = wonParams.myMachineID;
	mts.globalVersion = MAKE_GLOBAL_VERSION(GLOBAL_VERSION_MAJOR, GLOBAL_VERSION_MINOR);
	mts.gameVersion = MAKE_GAME_VERSION(GAME_VERSION_MAJOR, GAME_VERSION_MAJOR);
	mts.platformType = PT_WIN32;
	
	roomClient->SendMessageToServer((char *)&mts, sizeof(MTS_ROOM_ENTER_MESSAGE));
	
	roomClient->status = RCS_RECEIVING_ROOM_HEADER;
	roomClient->ShowStatus();
}

void RoomClientConnections::ClientConnected(HCONNECTION c)
{
	assert(FALSE);
}

void RoomClientConnections::ConnectionClosed(HCONNECTION c)
{
	char getS[256], getS2[256];
	
	DebugString("Connection [%d] closed.\n", GetConnectionID(c));
	
	if (roomClient->status != RCS_QUITTING)
		if (!roomClient->roomMessageBox || roomClient->roomMessageBox->IsHidden()) {
			if (roomClient->status == RCS_IN_ROOM)
				roomClient->ShowModelessMessage(0, GetString(40, getS), GetString(41, getS2), MB_OK);
			else
				roomClient->ShowModelessMessage(0, GetString(38, getS), GetString(41, getS2), MB_OK);
		}
	roomClient->server = NULL;
	roomClient->status = RCS_UNCONNECTED;
}

void RoomClientConnections::RequestHeartbeat(HCONNECTION c)
{
	DebugString("Request Heartbeat [%d].\n", GetConnectionID(c));
}

bool RoomClientConnections::IsValidAdminHost(IPAddr ipaddr)
{
	return true;
}
