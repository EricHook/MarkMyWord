#include "os.h"
#include "rc.h"
#include "sci.h"
#include "globals.h"
#include "inifile.h"

#include "Client Include.h"

#include "chat.h"

#define CHAT_FONT 2
// #define CHAT_TIMEOUT 30
// #define NUM_CHAT_SOUNDS 10 // number of silly sounds that can be playing at the same time

////////////////////////////////////////////////////////////////////////////////////////////////

ChatDisplayLine::ChatDisplayLine()
{
	label = NULL;
	text = NULL;
}

ChatDisplayLine::~ChatDisplayLine()
{
	if (label) { delete label; label = NULL; }
	if (text) { delete text; text = NULL; }
}

void ChatDisplayLine::Init(sciPlane *plane)
{
	label = new sciText();
	label->plane = plane;
	label->ParamInit(2, "", 50, CHAT_LINE_HEIGHT, 0, 0, 501, JUST_VERTICAL | JUST_LEFT);
	// label->SetBackgroundColor(0, 128, 0);
	
	text = new sciText();
	text->plane = plane;
	text->ParamInit(2, "", 50, CHAT_LINE_HEIGHT, 0, 0, 501, JUST_VERTICAL | JUST_LEFT);
}

void ChatDisplayLine::SetFont(int newLabelFont, int newFont)
{
	label->SetFont(newLabelFont);
	text->SetFont(newFont);
}

void ChatDisplayLine::SetHeight(int newHeight)
{
	label->SetHeight(newHeight);
	text->SetHeight(newHeight);
}

void ChatDisplayLine::Posn(int x, int y)
{
	label->Posn(x, y - 1);
	text->Posn(x + label->GetWidth() + 3, y);
}

void ChatDisplayLine::SetText(char *newLabel, char *newText, char red, char green, char blue)
{
	label->SetWidth(500);
	label->SetText(newLabel);
	label->SetWidth(label->CalculateFlatWidth() + 3);
	text->Posn(label->GetX() + label->GetWidth() + 3, text->GetY());
	text->SetWidth(490 - text->GetX());
	text->SetText(newText);

	text->SetColorMap(0, 0, 0, red, green, blue);
	
//	switch (color) {
//		case CHATCOLOR_RED:
//			text->SetColorMap(0, 0, 0, 255, 0, 0);
//			break;
//		case CHATCOLOR_GREEN:
//			text->SetColorMap(0, 0, 0, 0, 100, 0);
//			break;
//		case CHATCOLOR_PURPLE:
//			text->SetColorMap(0, 0, 0, 128, 0, 128);
//			break;
//		default:
//			text->SetColorMap(0, 0, 0, 0, 0, 0);
//			break;
//	}
}

void ChatDisplayLine::Show()
{
	label->Show();
	text->Show();
}

///////////

ChatScrollingDisplay::ChatScrollingDisplay()
{
	numLines = 0;
}

ChatScrollingDisplay::~ChatScrollingDisplay()
{
}

void ChatScrollingDisplay::Init(sciPlane *thePlane, ClientScrollBox *theScrollBox,
								PopupMenu *thePopup, int thePopID,
								int newX, int newY)
{
	int i;
	
	plane = thePlane;
	scrollBox = theScrollBox;
	popup = thePopup;
	popID = thePopID;
	
	sciRect::Init();
	
	SetPri(1);
	Posn(newX, newY);
	SetSize(478, 100);
	Show();
	
	lengthChecker.Init(plane);
	lengthChecker.SetText("", "", 0, 0, 0); //color=BLACK
	lengthChecker.SetFont(2, 4);
	lengthChecker.Posn(16, 0);
	lengthChecker.SetHeight(CHAT_LINE_HEIGHT);
	
	for (i = 0 ; i < NUM_VISIBLE_SCROLLING_CHAT_LINES ; i++) {
		displayLines[i].Init(plane);
		displayLines[i].SetText("", "", 0, 0, 0); //color=BLACK
		displayLines[i].SetFont(2, 4);
		displayLines[i].Posn(newX + 4, newY + 3 + i * CHAT_LINE_HEIGHT);
		displayLines[i].SetHeight(CHAT_LINE_HEIGHT);
	}
	
	for (i = 0 ; i < 256 ; i++) {
		lines[i].label[0] = 0;
		//BLACK
		lines[i].red = 0;
		lines[i].green = 0;
		lines[i].blue = 0;
		lines[i].text[0] = 0;
	}
}

void ChatScrollingDisplay::AddAChatLine(char *label, char *text, char red, char green, char blue)
{
	char cleanText[256];
	int i, n, end;
	POINT p;
	sciEvent event;
	
	strnzcpy(cleanText, text, 255);
	if (game->IsChatFiltered())
		roomClient->dirtyWordFilter->UpdateString(cleanText, 255);
	
	if (numLines == 256) {
		for (i = 0 ; i < 255 ; i++)
			lines[i] = lines[i + 1];
		numLines = 255;
	}
	
	lengthChecker.SetText(label, cleanText, 0, 0, 0); //color=BLACK
	end = lengthChecker.text->GetFirstLineBreak();
	strcpy(lines[numLines].label, label);
	strnzcpy(lines[numLines].text, cleanText, end);
	lines[numLines].red = red;
	lines[numLines].green = green;
	lines[numLines].blue = blue;
	
	numLines++;
	
	if (numLines >= NUM_VISIBLE_SCROLLING_CHAT_LINES) { // one blank line at the end
		if (numLines == NUM_VISIBLE_SCROLLING_CHAT_LINES)
			scrollBox->Enable();
		n = (numLines + 1) - NUM_VISIBLE_SCROLLING_CHAT_LINES;
	}
	else
		n = 0;
	
	// don't scroll the chat log to show the line if the user is looking at another line
	// (we say the user is looking at another line if the user has the mouse down on the scroll
	// thumb, or is scrolled to somewhere other than the bottom and has the mouse somewhere on
	// the chat log)
	
	if (scrollBox->scroller->TrackingMouse())
		; // scrollBox->sciScrollBox::DoRefresh(n);
	else {
		GetCursorPos(&p);
		MapWindowPoints(NULL, plane->GetWindow()->hWnd, &p, 1);
		p.x -= plane->GetWindow()->rcInner.left;
		p.y -= plane->GetWindow()->rcInner.top;
		event.x = p.x;
		event.y = p.y;
		if (roomClient->status != RCS_IN_ROOM || scrollBox->AtBottom(1) || (!OnMe(event) && !scrollBox->OnMe(event)))
			scrollBox->DoRefresh(n);
	}
	
	if (end < strlen(cleanText))
		AddAChatLine("", &cleanText[end], red, green, blue);
}

void ChatScrollingDisplay::DisplayLines(int fromLineNum)
{
	int i;
	
	if (fromLineNum == -1) {
		if (numLines > NUM_VISIBLE_SCROLLING_CHAT_LINES)
			fromLineNum = (numLines + 1) - NUM_VISIBLE_SCROLLING_CHAT_LINES; // one blank line at the end
		else
			fromLineNum = 0;
	}
	
	for (i = 0 ; i < NUM_VISIBLE_SCROLLING_CHAT_LINES ; i++) {
		if (fromLineNum < numLines) {
			displayLines[i].SetText(lines[fromLineNum].label, lines[fromLineNum].text, lines[fromLineNum].red, lines[fromLineNum].green, lines[fromLineNum].blue);
			displayLines[i].Show();
			fromLineNum++;
		}
		else
			displayLines[i].SetText("", "", 0, 0, 0);
	}
}

Boolean ChatScrollingDisplay::HandleEvent(sciEvent &event)
{
	char getS[256];
	
	if (roomClient->status < RCS_IN_ROOM)
		return FALSE;
	
	if (event.claimed)
		return FALSE;
	
	if (popup->HandleEvent(event))
		return event.claimed;
	
	if (event.type != MOUSE_DOWN) // left or right click is OK
		return FALSE;
	
	if (!sciFeature::OnMe(event))
		return FALSE;
	
	popup->SetNumItems(1);
	popup->AddItem(popID, POPITEM_SELECTABLE, GetString(296, getS)); // "Copy Chat Log"
	popup->Posn(event.x, event.y);
	popup->Show(this);
	
	return TRUE;
}

void ChatScrollingDisplay::CopyToClipboard()
{
	char *data, s[512];
	int i;
	HGLOBAL handle;
	
	if (!OpenClipboard(NULL))
		return;
	
	if (!EmptyClipboard())
		{ CloseClipboard(); return; }
	
	handle = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, numLines * 512);
	if (!handle)
		{ CloseClipboard(); return; }
	
	data = (char *)GlobalLock(handle);
	if (!data)
		{ GlobalFree(handle); CloseClipboard(); return; }
	
	data[0] = 0;
	for (i = 0 ; i < numLines ; i++) {
		sprintf(s, "%s %s\r\n", lines[i].label, lines[i].text);
		strcat(data, s);
	}
	
	GlobalUnlock(handle);
	SetClipboardData(CF_TEXT, handle);
	CloseClipboard();
}

////////////////////////////////////////////////////////////////////////////////////

Boolean ChatPlaneEditBox::HandleEvent(sciEvent &event)
{
	if (event.claimed || roomClient->status != RCS_IN_ROOM)
		return FALSE;
	
	switch (event.type) {
		case KEY_PRESS:
			if (event.c == 13) {
				roomClient->DoChatCommand(owner, GetText());
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

ChatPlane::ChatPlane(Boolean newModal, sciPlane *newParentPlane, ChatLog *theOwner)
		: sciPlane(newModal, newParentPlane, 508, 157, TRUE)
{
	i18nStr getS;
	RECT r;
	
	owner = theOwner;
	
	SetPic(ROOM_BACKGROUND_PIC);
	
	overlay = new sciView();
	overlay->plane = this;
	overlay->ParamInit(ROOM_OVERLAY_PIC, CHAT_PLANE_SHIFT_X, CHAT_PLANE_SHIFT_Y, 10);
	overlay->Show();
	
	chatScrollBox = new ClientScrollBox();
	chatScrollBox->Init(this, CSBID_GAME_CHAT);
	chatScrollBox->Show();
	
	editBox = new ChatPlaneEditBox(owner);
	editBox->plane = this;
	editBox->Init();
	editBox->SetPri(501);
	editBox->Posn(101 + CHAT_PLANE_SHIFT_X, 427 + CHAT_PLANE_SHIFT_Y);
	editBox->SetWidth(410);
	editBox->SetHeight(16);
	editBox->SetFont(4);
	editBox->Show();
	editBox->SetFocus();
	
	popup = new ChatPopup(this);
	popup->Init(POPDIR_GAME);
	
	chatDisplay = new ChatScrollingDisplay();
	chatDisplay->Init(this, chatScrollBox, popup, POPID_CHAT_COPY_CHAT_LOG, 12 + CHAT_PLANE_SHIFT_X, 311 + CHAT_PLANE_SHIFT_Y);
	
	GetWindowRect(owner->plane->GetWindow()->hWnd, &r);
	
	SetWindowPos(GetWindow()->hWnd, HWND_TOPMOST, r.left + 20, r.top + 20, 0, 0, SWP_NOSIZE);
	SetWindowText(GetWindow()->hWnd, GetString(222, getS));
	Show();
}

ChatPlane::~ChatPlane()
{
	if (overlay) { delete overlay; overlay = NULL; }
	if (chatScrollBox) { delete chatScrollBox; chatScrollBox = NULL; }
	if (chatDisplay) { delete chatDisplay; chatDisplay = NULL; }
	if (editBox) { delete editBox; editBox = NULL; }
	if (popup) { delete popup; popup = NULL; }
}

void ChatPlane::AttemptClosePlane()
{
	Hide();
	
	//Since we are hiding the chat window, we need to switch over to balloons
	roomClient->globalIniFile->WriteValue("ChatDisplay", CHAT_BALLOONS_ONLY);
	roomClient->tables[owner->table].display->showChatButton->SetButtonCels(0, 1, 2, 3);
	owner->isHidden = TRUE;
}

////////////////////////////////////////////////////////////////////////////////////

ChatBalloon::ChatBalloon(int x, int y, int orientation, sciPlane *newPlane)
{
	static POINT offsets[] = { { 54, 60 }, { 42, 54 }, { 55, 48 }, { 67, 47 }, { 76, 55 }, { 67, 60 }, { 54, 47 } };
	
	// enum { CBO_NORTHEAST = 0, CBO_EAST, CBO_SOUTHEAST, CBO_SOUTHWEST, CBO_WEST, CBO_NORTHWEST, CBO_NONE };
	
	UseColorOfPoint(TRUE);
	
	/*
	for (i = 0 ; i < 5 ; i++)
		queue[i][0] = 0;
	*/
	
	text = new sciText();
	text->plane = newPlane;
	text->Init();
	text->SetFont(2);
	text->SetWidth(124);
	text->SetHeight(53 + 20);
	text->SetJust(JUST_HORIZONTAL | JUST_VERTICAL);
	text->Posn(offsets[orientation].x, offsets[orientation].y - 10);
	// text->SetBackgroundColor(9);
	text->SetPri(997);
	text->Hide();
	
	SetScript(new ChatBalloonScript());
	script->UnSetTicks();
}

ChatBalloon::~ChatBalloon()
{
	if (text) { delete text; text = NULL; }
}

Boolean ChatBalloon::HandleEvent(sciEvent &event)
{
	if (event.claimed)
		return FALSE;
	
	// when the user is editing in the balloon, the balloon's text itself is empty
	// we'll use this as a flag that clicking on the balloon should not make it disappear
	
	if ((*text->GetText()) == 0)
		return FALSE;
	
	if (event.type == MOUSE_DOWN && sciFeature::OnMe(event)) {
		event.claimed = TRUE;
		script->SetNextState(2);
		script->SetTicks(0);
	}
	
	return event.claimed;
}

void ChatBalloon::Posn(int newX, int newY)
{
	int dX, dY;
	
	dX = text->GetX() - x;
	dY = text->GetY() - y;
	
	sciProp::Posn(newX, newY);
	
	text->Posn(newX + dX, newY + dY);
}

void ChatBalloon::Show(char *s)
{
	/*
	// if the balloon is already shown, queue the message for when it's hidden
	
	if (sciProp::IsNotHidden()) {
		for (i = 0 ; i < 5 ; i++)
			if (queue[i][0] == 0)
				{ strnzcpy(queue[i], s, 255); return; }
		
		// if the queue has become full, we are way behind the times, so we'll just clear
		// the queue to catch up
		
		for (i = 0 ; i < 5 ; i++)
			queue[i][0] = 0;
		
		strnzcpy(queue[0], s, 255);
		
		return;
	}
	*/
	
	sciProp::Show();
	
	text->SetText(s);
	text->Show();
	
	script->ChangeState(0);
}

void ChatBalloon::Hide()
{
	sciProp::Hide();
	
	text->Hide();
}

////////////////////////////////////////////////////////////////////////////////////

ChatTransparentLine::ChatTransparentLine(sciPlane *newPlane, int newTable)
{
	text = new sciText();
	text->plane = newPlane;
	text->SetFont(CHAT_FONT);
	text->SetJust(JUST_LEFT | JUST_VERTICAL);
	text->SetText("");
	text->SetPri(999);
	text->Init();
	
	table = newTable;
}

ChatTransparentLine::~ChatTransparentLine()
{
	if (text) { delete text; text = NULL; }
}

void ChatTransparentLine::ShowChatLine()
{
	int brightness;
	
	Show();
	brightness = pApp->pIniFile->ReadValue("LogBrightness", CHAT_LOG_DARK);
	if (brightness == CHAT_LOG_BRIGHT)
		text->SetColorMap(0, 0, 0, 250, 250, 250);
	else
		text->SetColorMap(0, 0, 0, 0, 0, 0);
	text->Show();
}

void ChatTransparentLine::HideChatLine()
{
	Hide();
	text->Hide();
}

void ChatTransparentLine::PosnChatLine(int x, int y)
{
	Posn(x, y);
	text->SetWidth(GetWidth() - 20);
	text->SetHeight(GetHeight());
	text->Posn(x + 10, y);
}

Boolean ChatTransparentLine::HandleEvent(sciEvent &event)
{
	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////

ChatEditText::ChatEditText()
{
	caret = NULL;
}

ChatEditText::~ChatEditText()
{
	if (caret) { delete caret; caret = NULL; }
}

void ChatEditText::Init()
{
	sciText::Init();
	
	caret = new sciProp();
	caret->plane = plane;
	caret->SetView(2);
	PositionCaret();
	caret->Init();
	caret->SetPri(1001);
}

void ChatEditText::PositionCaret()
{
	if (!caret)
		return;
	
	if (caret->script) {
		caret->script->SetNextState(0);
		caret->script->SetTicks(0);
	}
}

void ChatEditText::SetText(char *newText)
{
	sciText::SetText(newText);
	
	PositionCaret();
}

////////////////////////////////////////////////////////////////////////////////////

ChatLog::ChatLog(sciPlane *newPlane, int newTable, int newSeat)
{
	i18nStr getS;
	int i;
	
	plane = newPlane;
	
	table = newTable;
	seat = newSeat;
	
	isUp = FALSE;
	isHidden = FALSE;
	
	numLines = 0;
	nextLine = 0; // not used
	
	topPart = new sciView();
	topPart->plane = newPlane;
	topPart->SetView(25059);
	topPart->SetAlpha(64);
	topPart->SetPri(998);
	topPart->Init();
	
	for (i = 0 ; i < MAX_TRANSPARENT_CHAT_LINES ; i++) {
		chatTransparentLines[i] = new ChatTransparentLine(newPlane, table);
		chatTransparentLines[i]->plane = newPlane;
		chatTransparentLines[i]->SetView(25058);
		chatTransparentLines[i]->SetAlpha(64);
		chatTransparentLines[i]->SetPri(998);
		chatTransparentLines[i]->Init();
	}
	
	text = new ChatEditBox(this);
	text->plane = newPlane;
	//text->SetFocus();
	text->SetFont(CHAT_FONT);
	text->SetJust(JUST_LEFT | JUST_VERTICAL);
	text->SetText("");
	text->SetPri(999);
	text->Init();
	
	prompt = new sciText();
	prompt->plane = newPlane;
	prompt->SetPri(999);
	prompt->Init();
	prompt->SetText(GetString(223, getS));

	bottomPart = new sciView();
	bottomPart->plane = newPlane;
	bottomPart->SetView(25057);
	bottomPart->SetAlpha(64);
	bottomPart->SetPri(998);
	bottomPart->Init();
	
	dragBar = new ChatDragBar(this);
	dragBar->plane = newPlane;
	dragBar->SetPri(2); // above the character changer buttons
	dragBar->SetText("");
	dragBar->Init();
	// dragBar->SetBackgroundColor(9);
	
	sizeLine = new ChatSizeLine(this);
	sizeLine->plane = newPlane;
	sizeLine->SetPri(2); // above the character changer buttons
	sizeLine->SetText("");
	sizeLine->Init();
	// sizeLine->SetBackgroundColor(9);
	
	Posn(0, 0); // the chat button itself has no view
	SetScript(new ChatScript());
	
	ReadChatSettings();
	
	/*
	for (i = 0 ; i < NUM_CHAT_SOUNDS ; i++)
		chatSounds[i] = new sciSound();
	*/
	
	chatPlane = NULL;
}

ChatLog::~ChatLog()
{
	int i;
	
	if (topPart) { delete topPart; topPart = NULL; }
	for (i = 0 ; i < MAX_TRANSPARENT_CHAT_LINES ; i++)
		if (chatTransparentLines[i]) { delete chatTransparentLines[i]; chatTransparentLines[i] = NULL; }
	if (text) { delete text; text = NULL; }
	if (bottomPart) { delete bottomPart; bottomPart = NULL; }
	if (prompt) { delete prompt; prompt = NULL; }
	if (dragBar) { delete dragBar; dragBar = NULL; }
	if (sizeLine) { delete sizeLine; sizeLine = NULL; }
	/*
	for (i = 0 ; i < NUM_CHAT_SOUNDS ; i++)
		if (chatSounds[i]) { delete chatSounds[i]; chatSounds[i] = NULL; }
	*/
	if (chatPlane)
		{ pApp->MarkPlaneForDeletion(chatPlane); chatPlane = NULL; }
}

void ChatLog::Show()
{
	int i, b, numHidden;
	
	sciButton::Show();
	
	b = pApp->pIniFile->ReadValue("LogWindow", CHAT_LOG_FLOATING);
	if (b < 0 || b > 1) b = CHAT_LOG_FLOATING;
	
	if (b == CHAT_LOG_TRANSPARENT) {
		if (chatPlane)
			chatPlane->Hide();
		
		topPart->Show();
		numHidden = MAX_TRANSPARENT_CHAT_LINES - numLines;
		for (i = 0 ; i < numHidden ; i++)
			chatTransparentLines[i]->HideChatLine();
		for ( ; i < MAX_TRANSPARENT_CHAT_LINES ; i++)
			chatTransparentLines[i]->ShowChatLine();
		//text->Show();
		bottomPart->Show();
		dragBar->Show();
		sizeLine->Show();
	}
	else {
		Hide();
		if (game->GetChatDisplay() != CHAT_BALLOONS_ONLY) {
			if (chatPlane == NULL)
				chatPlane = new ChatPlane(FALSE, NULL, this);
			chatPlane->Show();
		}
	}

	isHidden = FALSE;
}

void ChatLog::Hide()
{
	short i;
	
	// sciButton::Hide(); // the button is always shown, but it has no view
	
	topPart->Hide();
	for (i = 0 ; i < MAX_TRANSPARENT_CHAT_LINES ; i++)
		chatTransparentLines[i]->HideChatLine();
	text->Hide();
	bottomPart->Hide();
	dragBar->Hide();
	sizeLine->Hide();
	prompt->Hide();
	
	if (isUp) {
		isUp = FALSE;
		text->SetText("");
	}

	isHidden = TRUE;
}

void ChatLog::PosnChat(int x, int y)
{
	int i, theHeight, numHidden;
	
	theHeight = topPart->GetHeight() + bottomPart->GetHeight() + numLines * chatTransparentLines[0]->GetHeight();
	
	if (x < 0 || x > 30000)
		x = 0;
	if (y < 0 || y > 30000)
		y = 0;
	if (x > (640 - topPart->GetWidth()))
		x = 640 - topPart->GetWidth();
	if (y > (480 - theHeight))
		y = 480 - theHeight;
	
	chatSettings.x = x;
	chatSettings.y = y;
	
	topPart->Posn(x, y);
	dragBar->SetWidth(topPart->GetWidth());
	dragBar->SetHeight(topPart->GetHeight() - 4);
	dragBar->Posn(x, y);
	y += topPart->GetHeight();
	
	numHidden = MAX_TRANSPARENT_CHAT_LINES - numLines;
	for (i = numHidden ; i < MAX_TRANSPARENT_CHAT_LINES ; i++) {
		chatTransparentLines[i]->PosnChatLine(x, y);
		y += chatTransparentLines[i]->GetHeight();
	}
	
	bottomPart->Posn(x, y);
	prompt->Posn(x+12, y+12);
	
	sizeLine->SetWidth(topPart->GetWidth());
	sizeLine->SetHeight(4);
	sizeLine->Posn(x, y + bottomPart->GetHeight() - 4);
	
	if (!(isUp && game->GetChatDisplay() == CHAT_BALLOONS_ONLY)) {
		text->Posn(x + 13 + prompt->CalculateFlatWidth(), y + 7);
		text->SetWidth(bottomPart->GetWidth() - 48);
		text->SetHeight(bottomPart->GetHeight() - 13);
	}
	
/*	if (isUp && game->GetChatDisplay() != CHAT_BALLOONS_ONLY) {
		//text->caret->Hide();
		text->HideCaret();
		text->PositionCaret();
		//text->caret->Show();
		text->ShowCaret();
	}*/
}

void ChatLog::SetNumLines(int n)
{
	if (n < 1)
		n = 1;
	
	if (n > MAX_TRANSPARENT_CHAT_LINES)
		n = MAX_TRANSPARENT_CHAT_LINES;
	
	numLines = n;
	chatSettings.numLines = n;
	
	PosnChat(topPart->GetX(), topPart->GetY());
	
	if (IsHidden())
		Hide();
	else
		Show();
}

void ChatLog::DoClick(BOOL netMessage)
{
	Boolean justBalloon;
	char textBuffer[1000];
	int brightness, n = 0, offset = 0;
	ChatBalloon *balloon;
	static Boolean upForJustBallon = FALSE;
	static short oldX, oldY, oldWidth, oldHeight;
	
	// I can't chat if I'm a watcher and watchers are muted, unless I'm a monitor
	
	if (roomClient->tables[table].muteWatchers && seat >= MAX_GAME_PLAYERS)
		if (!roomClient->players[0].IsMonitorOrSuperUser())
			return;
	
	justBalloon = table != -1 && game->GetChatDisplay() == CHAT_BALLOONS_ONLY;
	
	if (isUp) { // send the line
		if (upForJustBallon) {
			upForJustBallon = FALSE;
			text->Posn(oldX, oldY);
			text->SetWidth(oldWidth);
			text->SetHeight(oldHeight);
			text->SetJust(JUST_LEFT | JUST_VERTICAL);
			if (seat < MAX_GAME_PLAYERS) // player
				balloon = roomClient->tables[table].display->characters[seat]->chatBalloon;
			else // watcher
				balloon = roomClient->tables[table].display->watchersDisplay->chatEditBalloon;
			offset = 0;
			balloon->Hide();
			// continue showing balloon's chat history, if there is one
			balloon->script->SetNextState(2);
			balloon->script->SetTicks(0);
		}
		if (strlen(text->GetText() + offset)) {
			sprintf(textBuffer, "%s", text->GetText() + offset);
			roomClient->DoChatCommand(this, textBuffer);
		}
		text->HideCaret();
		text->caret->SetScript(NULL);
		text->SetText("");
		text->Hide();
		prompt->Hide();
	}
	else { // start edit box
		upForJustBallon = FALSE;
		text->SetColorMap(0, 0, 0, 0, 0, 0);
		if (justBalloon) {
			upForJustBallon = TRUE;
			oldX = text->GetX();
			oldY = text->GetY();
			oldWidth = text->GetWidth();
			oldHeight = text->GetHeight();
			if (seat < MAX_GAME_PLAYERS) // player
				balloon = roomClient->tables[table].display->characters[seat]->chatBalloon;
			else // watcher
				balloon = roomClient->tables[table].display->watchersDisplay->chatEditBalloon;
			balloon->sciProp::Show();
			balloon->SetAlpha(200);
			balloon->script->UnSetTicks();
			balloon->text->SetText("");
			text->Posn(balloon->text->GetX(), balloon->text->GetY());
			text->SetWidth(balloon->text->GetWidth());
			text->SetHeight(balloon->text->GetHeight());
			text->SetText("");
			text->SetJust(JUST_HORIZONTAL | JUST_VERTICAL);
		}
		else {
			if (chatPlane && chatPlane->IsNotHidden()) {
				SetForegroundWindow(chatPlane->GetWindow()->hWnd);
				
				return;
			}
			
			//text->SetText("Chat: ");
			brightness = pApp->pIniFile->ReadValue("LogBrightness", CHAT_LOG_DARK);
			if (brightness == CHAT_LOG_BRIGHT) {
				text->SetColorMap(0, 0, 0, 250, 250, 250);
				prompt->SetColorMap(0, 0, 0, 250, 250, 250);
			}
			else
				prompt->SetColorMap(0, 0, 0, 0, 0, 0);
			prompt->Show();
		}
		text->SetFocus();
		text->Show();
		text->PositionCaret();
		//text->caret->Show();
		text->ShowCaret();
		text->caret->SetScript(new CaretScript(text));
		text->caret->script->SetTicks(0);
	}
	
	isUp = !isUp;
}

Boolean ChatLog::HandleEvent(sciEvent &event)
{
	if (!enabled)
		return FALSE;
	
	if (event.claimed)
		return FALSE;
	
	return event.claimed;
}

void ChatLog::AddAChatLine(char *label, char *str, char red, char green, char blue)
{
	char *line, s[1024];
	int i, numHidden;
	
	if (chatPlane) {
		sprintf(s, "%s:", label);
		chatPlane->chatDisplay->AddAChatLine(s, str, red, green, blue);
	}
	
	// put the string in the first blank line (such that all lower lines are also blank)
	
	numHidden = MAX_TRANSPARENT_CHAT_LINES - numLines;
	
	for (i = MAX_TRANSPARENT_CHAT_LINES - 1 ; i >= numHidden ; i--) {
		line = chatTransparentLines[i]->text->GetText();
		if (line[0])
			break;
	}
	
	i++;
	
	// if there are no blank spots, scroll the lines up
	
	if (i == MAX_TRANSPARENT_CHAT_LINES) {
		// scroll the lines up
		for (i = 0 ; i < (MAX_TRANSPARENT_CHAT_LINES - 1) ; i++)
			chatTransparentLines[i]->text->SetText(chatTransparentLines[i + 1]->text->GetText());
		i = MAX_TRANSPARENT_CHAT_LINES - 1;
	}
	
	sprintf(s, "%s: %s", label, str);
	
	chatTransparentLines[i]->text->SetText(s);
	while (chatTransparentLines[i]->text->CalculateFlatWidth() >= (topPart->GetWidth() - 20)) {
		s[strlen(s) - 1] = 0;
		chatTransparentLines[i]->text->SetText(s);
	}
}

void ChatLog::RemoveAChatLine()
{
	int i;
	
	// scroll the lines up
	for (i = 0 ; i < (MAX_TRANSPARENT_CHAT_LINES - 1) ; i++)
		chatTransparentLines[i]->text->SetText(chatTransparentLines[i + 1]->text->GetText());
	chatTransparentLines[MAX_TRANSPARENT_CHAT_LINES - 1]->text->SetText("");
}

void ChatLog::ReadChatSettings()
{
	if (table == -1) {
		chatSettings.x = 100;
		chatSettings.y = 100;
		chatSettings.numLines = 10;
		
		return;
	}
	
	chatSettings.x = pApp->pIniFile->ReadValue("ChatX", 100);
	if (chatSettings.x < 0 || chatSettings.x > 600) chatSettings.x = 100;
	
	chatSettings.y = pApp->pIniFile->ReadValue("ChatY", 400);
	if (chatSettings.y < 0 || chatSettings.y > 440) chatSettings.y = 400;
	
	chatSettings.numLines = pApp->pIniFile->ReadValue("ChatNumLines", 1);
	if (chatSettings.numLines < 1 || chatSettings.numLines > MAX_TRANSPARENT_CHAT_LINES) chatSettings.numLines = 1;
}

void ChatLog::WriteChatSettings()
{
	if (table == -1)
		return; // the room chat log
	pApp->pIniFile->WriteValue("ChatX", chatSettings.x);
	pApp->pIniFile->WriteValue("ChatY", chatSettings.y);
	pApp->pIniFile->WriteValue("ChatNumLines", chatSettings.numLines);
}

////////////////////////////////////////////////////////////////////////////////////

ChatDragBar::ChatDragBar(ChatLog *theChatLog)
{
	mouseDown = FALSE;
	chatLog = theChatLog;
}

Boolean ChatDragBar::HandleEvent(sciEvent &event)
{
	static short startX, startY, dX, dY;
	
	if (event.claimed || pApp->handsOff)
		return FALSE;
	
	switch (event.type) {
		case MOUSE_DOWN:
			if (!sciFeature::OnMe(event))
				return FALSE;
			if ((event.modifiers & LEFT_MOUSE) && !mouseDown) {
				plane->CaptureEvents(this);
				startX = GetX();
				startY = GetY();
				dX = event.x - startX;
				dY = event.y - startY;
				event.claimed = TRUE;
				mouseDown = TRUE;
			}
			break;
		case MOUSE_MOVE:
			if (!mouseDown)
				break;
			event.claimed = TRUE;
			chatLog->PosnChat(event.x - dX, event.y - dY);
			break;
		case MOUSE_UP:
			if ((event.modifiers & LEFT_MOUSE) && mouseDown) {
				plane->CaptureEvents(NULL);
				event.claimed = TRUE;
				chatLog->WriteChatSettings();
			}
			mouseDown = FALSE;
			break;
	}
	
	return event.claimed;
}

////////////////////////////////////////////////////////////////////////////////////

ChatEditBox::ChatEditBox(ChatLog *theChatLog)
{
	chatLog = theChatLog;
	
	hotkey = 13;
}

void ChatEditBox::SetHotkey(char c)
{
	hotkey = c;
}

Boolean ChatEditBox::HandleEvent(sciEvent &event)
{
	if (event.claimed || pApp->handsOff)
		return FALSE;
	
	switch (event.type) {
		case KEY_PRESS:
			if (event.c == hotkey) {
				chatLog->DoClick();
				event.claimed = TRUE;
				break;
			}
			if (event.c == 27 && chatLog->UserIsEditing()) {
				SetText("");
				chatLog->DoClick();
				event.claimed = TRUE;
				break;
			}
		default:
			return EditBox::HandleEvent(event);
			break;
	}
	
	return event.claimed;
}

////////////////////////////////////////////////////////////////////////////////////

ChatSizeLine::ChatSizeLine(ChatLog *theChatLog)
{
	mouseDown = FALSE;
	chatLog = theChatLog;
}

Boolean ChatSizeLine::HandleEvent(sciEvent &event)
{
	int n, top, bottom;
	static short dY;
	
	if (event.claimed || pApp->handsOff)
		return FALSE;
	
	switch (event.type) {
		case MOUSE_DOWN:
			if (!sciFeature::OnMe(event))
				return FALSE;
			if ((event.modifiers & LEFT_MOUSE) && !mouseDown) {
				// SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZENS)));
				dY = event.y - chatLog->bottomPart->GetY();
				event.claimed = TRUE;
				mouseDown = TRUE;
			}
			break;
		case MOUSE_MOVE:
			if (event.x < 0 || event.x > 640 || event.y < 0 || event.y > 480)
				break;
			if (GetActiveWindow() != plane->GetWindow()->hWnd)
				break;
			if (!mouseDown) {
				// SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(sciFeature::OnMe(event) ? IDC_SIZENS : IDC_ARROW)));
				break;
			}
			// SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZENS)));
			// we need to see if the number of lines needs to be changed
			if (event.y > 10000)
				break; // bogus value
			top = chatLog->topPart->GetY() + chatLog->topPart->GetHeight();
			bottom = event.y - dY;
			n = (bottom - top) / chatLog->chatTransparentLines[0]->GetHeight();
			if (n != chatLog->numLines)
				chatLog->SetNumLines(n);
			break;
		case MOUSE_UP:
			if ((event.modifiers & LEFT_MOUSE) && mouseDown)
				event.claimed = TRUE;
			mouseDown = FALSE;
			chatLog->WriteChatSettings();
			break;
	}
	
	return event.claimed;
}

////////////////////////////////////////////////////////////////////////////////////

void ChatScript::ChangeState(int newState)
{
	sciScript::ChangeState(newState);
	
	switch (state) {
		case 0:
			// SetSeconds(((ChatLog *)client)->chatSettings.chatLogSpeed);
			break;
		case 1:
			// ((ChatLog *)client)->RemoveAChatLine();
			// SetNextState(1);
			// SetSeconds(((ChatLog *)client)->chatSettings.chatLogSpeed);
			break;
	}
}

void ChatBalloonScript::ChangeState(int newState)
{
	int b, speed, fade;
	ChatBalloon *balloon = (ChatBalloon *)client;
	
	sciScript::ChangeState(newState);
	
	b = roomClient->globalIniFile->ReadValue("BalloonSpeed", CHAT_BALLOONS_MEDIUM);
	if (b < 0 || b > 2) b = CHAT_BALLOONS_MEDIUM;
	switch (b) {
		case CHAT_BALLOONS_SLOW: speed = 10; break;
		default:
		case CHAT_BALLOONS_MEDIUM: speed = 5; break;
		case CHAT_BALLOONS_FAST: speed = 2; break;
	}
	
	fade = FALSE; // roomClient->globalIniFile->ReadValue("BalloonFade", 1);
	if (fade < 0 || fade > 1) fade = 0;
	
	switch (state) {
		case 0:
			balloon->alpha = 200;
			balloon->SetAlpha(balloon->alpha);
			balloon->count = (BYTE)speed;
			balloon->dAlpha = 150 / balloon->count;
			SetSeconds(1);
			break;
		case 1:
			if (fade) {
				balloon->alpha -= balloon->dAlpha;
				balloon->SetAlpha(balloon->alpha);
			}
			if (balloon->count > 1) {
				balloon->count--;
				SetNextState(1);
			}
			else
				SetNextState(2);
			SetSeconds(1);
			break;
		case 2:
			balloon->Hide();
			SetNextState(3);
			SetTicks(0);
			break;
		case 3:
			/*
			if (balloon->queue[0][0] != 0) {
				balloon->sciProp::Show();
				balloon->text->SetText(balloon->queue[0]);
				balloon->text->Show();
				for (i = 0 ; i < 4 ; i++)
					strcpy(balloon->queue[i], balloon->queue[i + 1]);
				SetNextState(0);
				SetTicks(0);
			}
			*/
			break;
	}
}

////////////////////////////////////////////////////////////////////////////////////

void ChatPopup::ItemSelected(int itemID, int itemPlayerID, int itemX, int itemY)
{
	ChatPlane *chatPlane;
	
	switch (itemID) {
		// click on chat log in chat plane
		
		case POPID_CHAT_COPY_CHAT_LOG:
			chatPlane = (ChatPlane *)plane;
			chatPlane->chatDisplay->CopyToClipboard();
			break;
		
		default:
			assert(FALSE);
			break;
	}
}
