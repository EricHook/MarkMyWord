#include "os.h"
#include "sci.h"
#include "globals.h"
#include "inifile.h"
#include "util.h"

#include "Client Include.h"

int DoubleCrossClient::Init()
{
	return RoomClient::Init();
}

//////////////////////////////////////////////////////////////////////////////////////////

void DoubleCrossClient::GameItemSelected(int itemID, int playerID, int seat, DoubleCrossDisplay *display)
{
	tables[display->table].FinishREQUESTREPLACEPLAYER(seat, PLAYER_ID_VOID);
}

//////////////////////////////////////////////////////////////////////////////////////////

void DoubleCrossClient::InitTablesDisplaySeat(RoomTablesDisplaySeat *seat)
{
	int angles[MAX_GAME_PLAYERS] = { RCA_LEFT, RCA_RIGHT, RCA_RIGHT, RCA_LEFT };
	
	seat->chairAngle = angles[seat->seatIndex];
	
	seat->chair = new RoomTablesDisplayChair();
	seat->chair->ParamInit(200014, 0, 0, 110);
	seat->chair->SetCel(2); // center
	switch (seat->chairAngle) {
		case RCA_LEFT: seat->chair->SetCel(3); break;
		case RCA_RIGHT: seat->chair->SetCel(1); break;
	}
	
	seat->player = new RoomTablesDisplayPlayer();
	seat->player->ParamInit(200014, 0, 0, 115);
	
	seat->nameTag = new sciView();
	seat->nameTag->ParamInit(200013, 0, 0, 200);
	
	seat->nameLabel = new sciText();
	seat->nameLabel->ParamInit(4, "", seat->nameTag->GetWidth() - 4, seat->nameTag->GetHeight(), 0, 0, 201, JUST_VERTICAL | JUST_HORIZONTAL);
}

Boolean DoubleCrossClient::PlayerInSeatFacesRight(RoomTablesDisplaySeat *seat)
{
	assert(seat->seatIndex >= 0 && seat->seatIndex < MAX_GAME_PLAYERS);
	
	return seat->seatIndex == 0 || seat->seatIndex == 3;
}

void DoubleCrossClient::PosnTablesDisplayTable(RoomTablesDisplayTable *table)
{
	int i, x, y;
	POINT p[MAX_GAME_PLAYERS] = { { 20, 2 }, { 160, 2 }, { 160, 75 }, { 20, 75 } };
	
	x = table->GetX();
	y = table->GetY();
	
	if (table->gstDisplay)
		table->gstDisplay->Posn(x, y);
	
	if (table->label)
		table->label->Posn((x > 100) ? x - 67 : x + 300, y + 14);
	
	if (table->numberView)
		table->numberView->Posn((x > 100) ? x - 102 : x + 265, y + 13);
	
	if (table->description)
		table->description->Posn((x > 100) ? x - 125 : x + 242, y + 33);
	
	if (table->watchersDisplay)
		table->watchersDisplay->Posn((x > 100) ? x - 104 : x + 263, y + 106);
	
	if (table->startButton)
		table->startButton->Posn(x + 85, y + 63);
	
	if (table->requestingApproval)
		table->requestingApproval->Posn(x + 66, y + 63);
	
	for (i = 0 ; i < MAX_GAME_PLAYERS ; i++)
		if (table->seats[i])
			table->seats[i]->Posn(x + p[i].x, y + p[i].y);
}

void DoubleCrossClient::DisplayTables(RoomTablesDisplay *tablesDisplay, int fromLineNum)
{
	int frame, a, b, aY[4] = { 78, -20, 250, 150 }, bY[4] = { 250, 150, 78, -20 };
	
	// a is the table shown in the left column
	// b is the table shown in the right column
	
	frame = fromLineNum % 4;
	
	if (frame < 2) {
		a = fromLineNum / 2;
		b = a + 1;
	}
	else {
		b = fromLineNum / 2;
		a = b + 1;
	}
	
	assert(max(a, b) < roomClient->numTables);
	
	tablesDisplay->displayTables[0].Posn(30, aY[frame]);
	tablesDisplay->displayTables[0].Show(a);
	tablesDisplay->displayTables[1].Posn(152, bY[frame]);
	tablesDisplay->displayTables[1].Show(b);
}

////////////////

void DCCheckBox::UpdateCel()
{
	sciCheckBox::UpdateCel();
	
	if (!controls)
		return;
	
	if (IsSelected())
		controls->gridMode->Enable();
	else
		controls->gridMode->Disable();
}

DoubleCrossControls::DoubleCrossControls()
{
	int i;
	
	// personal
	
	verbose = NULL;
	
	// table
	
	gameOptionsLabel = NULL;
	haveColoredBombs = NULL;
	haveDirectionalBombs = NULL;
	haveBonusTiles = NULL;
	haveBlockingTiles = NULL;
	gridMode = NULL;
	haveMysteryTiles = NULL;
	allowOffensiveWords = NULL;
	secondsLabel = NULL;
	secondsEditBox = NULL;
	minWordSizeTitle = NULL;
	minWordSizeGroup = NULL;
	for (i = 0 ; i < 2 ; i++)
		minWordSize[i] = NULL;
}

DoubleCrossControls::~DoubleCrossControls()
{
	int i;
	
	// personal
	
	if (verbose) { delete verbose; verbose = NULL; }
	
	// table
	
	if (gameOptionsLabel) { delete gameOptionsLabel; gameOptionsLabel = NULL; }
	if (haveColoredBombs) { delete haveColoredBombs; haveColoredBombs = NULL; }
	if (haveDirectionalBombs) { delete haveDirectionalBombs; haveDirectionalBombs = NULL; }
	if (haveBonusTiles) { delete haveBonusTiles; haveBonusTiles = NULL; }
	if (haveBlockingTiles) { delete haveBlockingTiles; haveBlockingTiles = NULL; }
	if (gridMode) { delete gridMode; gridMode = NULL; }
	if (haveMysteryTiles) { delete haveMysteryTiles; haveMysteryTiles = NULL; }
	if (allowOffensiveWords) { delete allowOffensiveWords; allowOffensiveWords = NULL; }
	if (secondsLabel) { delete secondsLabel; secondsLabel = NULL; }
	if (secondsEditBox) { delete secondsEditBox; secondsEditBox = NULL; }
	if (minWordSizeTitle) { delete minWordSizeTitle; minWordSizeTitle = NULL; }
	if (minWordSizeGroup) { delete minWordSizeGroup; minWordSizeGroup = NULL; }
	for (i = 0 ; i < 2 ; i++)
		if (minWordSize[i]) { delete minWordSize[i]; minWordSize[i] = NULL; }
}

DCCheckBox *DoubleCrossControls::MakeCheckBox(sciPlane *plane, char *title, int x, int y)
{
	DCCheckBox *c;
	
	c = new DCCheckBox();
	c->plane = plane;
	c->SetFont(4, -1, 7);
	c->SetView(1001);
	c->SetText(title);
	c->SetTextOffset(20, 2);
	c->Init(0, 0);
	c->Posn(x, y);
	c->SetPri(10);
	
	return c;
}

void DoubleCrossControls::CheckCheckBox(DCCheckBox *c, Boolean selected)
{
	if (selected)
		c->Select();
	else
		c->Unselect();
}

void DoubleCrossControls::Init(sciPlane *thePlane)
{
	int i;
	
	plane = thePlane;
	
	// personal...
	
	verbose = MakeCheckBox(plane, "Explain all plays", 208, 192);
	
	// table
	
	gameOptionsLabel = new sciText();
	gameOptionsLabel->plane = plane;
	gameOptionsLabel->ParamInit(2, "Game Options", 150, 20, 27, 50, 10, JUST_VERTICAL | JUST_LEFT);
	
	haveColoredBombs = MakeCheckBox(plane, "Colored bombs", 25, 75);
	haveDirectionalBombs = MakeCheckBox(plane, "Directional bombs", 25, 75 + 20);
	haveBonusTiles = MakeCheckBox(plane, "Bonus spaces", 25, 75 + 20 * 2);
	haveBlockingTiles = MakeCheckBox(plane, "Blocking spaces", 25, 75 + 20 * 3);
	haveBlockingTiles->controls = this;
	gridMode = MakeCheckBox(plane, "Grid mode", 45, 75 + 20 * 4);
	haveMysteryTiles = MakeCheckBox(plane, "Mystery spaces", 25, 75 + 20 * 5);
	allowOffensiveWords = MakeCheckBox(plane, "Allow offensive words", 25, 75 + 20 * 6);
	
	secondsLabel = new sciText();
	secondsLabel->plane = plane;
	secondsLabel->ParamInit(4, "Seconds to play:", 120, 20, 28, 225, 10, JUST_VERTICAL | JUST_LEFT);
	
	secondsEditBox = new EditBox();
	secondsEditBox->plane = plane;
	secondsEditBox->ParamInit(4, "120", 30, 16, 116, 226, 10, JUST_LEFT | JUST_TOP);
	secondsEditBox->SetMaxLength(4);
	secondsEditBox->Set3DOutLine(TRUE);
	
	minWordSizeTitle = new sciText();
	minWordSizeTitle->plane = plane;
	minWordSizeTitle->ParamInit(2, "Minimum Word Size", 150, 20, 27, 255, 10, JUST_VERTICAL | JUST_LEFT);
	
	minWordSizeGroup = new sciButtonGroup();
	minWordSizeGroup->plane = plane;
	minWordSizeGroup->Init();
	minWordSizeGroup->SetPri(10);
	
	for (i = 0 ; i < 2 ; i++) {
		minWordSize[i] = new sciRadioButton(i);
		minWordSizeGroup->AddToGroup(minWordSize[i]);
		minWordSize[i]->plane = plane;
		minWordSize[i]->SetFont(4, -1, 7);
		minWordSize[i]->SetView(1063);
		minWordSize[i]->SetText("");
		minWordSize[i]->SetTextOffset(20);
		minWordSize[i]->Init(0, 0);
		minWordSize[i]->Posn(25 + 15 + i * 50, 275);
		minWordSize[i]->SetPri(10);
	}
	
	minWordSize[0]->SetText("3");
	minWordSize[1]->SetText("4");
}

void DoubleCrossControls::PrepareToShow(int table, int version, Boolean gameHasStarted, Boolean inGame, Boolean watcher)
{
	char s[256];
	GameState *gameState;
	
	if (table == -1)
		return;
	
	if (roomClient->players[0].info.table != table)
		{ assert(FALSE); return; }
	
	gameState = &roomClient->tables[table].gameState;
	
	// personal options
	
	CheckCheckBox(verbose, roomClient->globalIniFile->ReadValue("DC_VERBOSE", 1) == 1);
	
	// table options
	
	CheckCheckBox(haveColoredBombs, gameState->options.haveColoredBombs);
	CheckCheckBox(haveDirectionalBombs, gameState->options.haveDirectionalBombs);
	CheckCheckBox(haveBonusTiles, gameState->options.haveBonusTiles);
	CheckCheckBox(haveBlockingTiles, gameState->options.haveBlockingTiles);
	CheckCheckBox(gridMode, gameState->options.gridMode);
	CheckCheckBox(haveMysteryTiles, gameState->options.haveMysteryTiles);
	CheckCheckBox(allowOffensiveWords, gameState->options.allowOffensiveWords);
	
	sprintf(s, "%d", gameState->options.timeLimit);
	secondsEditBox->SetText(s);
	
	minWordSizeGroup->SelectButtonByID((gameState->options.minWordSize == 4) ? 1 : 0);
}

void DoubleCrossControls::Show(int table, int version, Boolean gameHasStarted, Boolean inGame, Boolean watcher)
{
	verbose->Hide();
	gameOptionsLabel->Hide();
	haveColoredBombs->Hide();
	haveDirectionalBombs->Hide();
	haveBonusTiles->Hide();
	haveBlockingTiles->Hide();
	gridMode->Hide();
	haveMysteryTiles->Hide();
	allowOffensiveWords->Hide();
	secondsLabel->Hide();
	secondsEditBox->Hide();
	minWordSizeTitle->Hide();
	minWordSizeGroup->Hide();
	
	if (table == -1)
		return;
	
	// personal options
	
	if (version == ODBV_PERSONAL && inGame)
		verbose->Show();
	
	// table options
	
	if (version == ODBV_TABLE && table != -1) {
		gameOptionsLabel->Show();
		haveColoredBombs->Show();
		haveDirectionalBombs->Show();
		haveBonusTiles->Show();
		haveBlockingTiles->Show();
		gridMode->Show();
		haveMysteryTiles->Show();
		allowOffensiveWords->Show();
		secondsLabel->Show();
		secondsEditBox->Show();
		minWordSizeTitle->Show();
		minWordSizeGroup->Show();
		
		if (gameHasStarted || watcher) {
			haveColoredBombs->Disable();
			haveDirectionalBombs->Disable();
			haveBonusTiles->Disable();
			haveBlockingTiles->Disable();
			gridMode->Disable();
			haveMysteryTiles->Disable();
			allowOffensiveWords->Disable();
			secondsEditBox->Disable();
			secondsEditBox->SetColorMap(0, 0, 0, 128, 128, 128);
			minWordSizeGroup->DisableAll();
		}
		else {
			haveColoredBombs->Enable();
			haveDirectionalBombs->Enable();
			haveBonusTiles->Enable();
			haveBlockingTiles->Enable();
			// gridMode->Enable();
			haveMysteryTiles->Enable();
			allowOffensiveWords->Enable();
			secondsEditBox->Enable();
			secondsEditBox->SetColorMap(0, 0, 0, 0, 0, 0);
			minWordSizeGroup->EnableAll();
		}
	}
}

Boolean DoubleCrossControls::DoClickOK(int table, int *values)
{
	Boolean changed = FALSE;
	char s[256], *data;
	int seat, newTimeLimit, newMinWordSize;
	GameState *gameState;
	
	seat = roomClient->players[0].info.seat;
	
	if (seat >= MAX_GAME_PLAYERS)
		return FALSE; // watchers can't change settings
	
	// personal options
	
	roomClient->globalIniFile->WriteValue("DC_VERBOSE", verbose->IsSelected());
	
	// table options
	
	gameState = &roomClient->tables[table].gameState;
	
	data = (char *)values;
	
	if (haveColoredBombs->IsSelected() != gameState->options.haveColoredBombs)
		changed = TRUE;
	data[0] = haveColoredBombs->IsSelected();
	
	if (haveDirectionalBombs->IsSelected() != gameState->options.haveDirectionalBombs)
		changed = TRUE;
	data[1] = haveDirectionalBombs->IsSelected();
	
	if (haveBonusTiles->IsSelected() != gameState->options.haveBonusTiles)
		changed = TRUE;
	data[2] = haveBonusTiles->IsSelected();
	
	if (haveBlockingTiles->IsSelected() != gameState->options.haveBlockingTiles)
		changed = TRUE;
	data[3] = haveBlockingTiles->IsSelected();
	
	if ((haveBlockingTiles->IsSelected() && gridMode->IsSelected()) != gameState->options.gridMode)
		changed = TRUE;
	data[4] = haveBlockingTiles->IsSelected() && gridMode->IsSelected();
	
	if (haveMysteryTiles->IsSelected() != gameState->options.haveMysteryTiles)
		changed = TRUE;
	data[5] = haveMysteryTiles->IsSelected();
	
	if (allowOffensiveWords->IsSelected() != gameState->options.allowOffensiveWords)
		changed = TRUE;
	data[6] = allowOffensiveWords->IsSelected();
	
	strcpy(s, secondsEditBox->GetText());
	newTimeLimit = atoi(s);
	
	if (newTimeLimit < 30)
		newTimeLimit = 30;
	
	if (newTimeLimit > 99)
		newTimeLimit = 99;
	
	data[7] = newTimeLimit;
	
	if (newTimeLimit != gameState->options.timeLimit)
		changed = TRUE;
	
	newMinWordSize = (minWordSizeGroup->GetSelectedButtonID() == 0) ? 3 : 4;
	data[8] = newMinWordSize;
	
	if (newMinWordSize != gameState->options.minWordSize)
		changed = TRUE;
	
	return changed;
}

////////////////////////////////////////////////////////////////////////////////////////////////

DoubleCrossTableDisplay::DoubleCrossTableDisplay()
{
	centerpiece = NULL;
}

DoubleCrossTableDisplay::~DoubleCrossTableDisplay()
{
	if (centerpiece) { delete centerpiece; centerpiece = NULL; }
}

void DoubleCrossTableDisplay::Init()
{
	if (!centerpiece)
		centerpiece = new sciView();
	
	centerpiece->ParamInit(200200, 0, 0, 101);
}

void DoubleCrossTableDisplay::Posn(int newX, int newY)
{
	if (centerpiece)
		centerpiece->Posn(newX, newY);
}

void DoubleCrossTableDisplay::Show(int theTable)
{
	GameState *gameState;
	
	if (theTable < 0 || theTable >= NUM_ROOM_TABLES)
		{ assert(FALSE); return; }
	
	gameState = &roomClient->tables[theTable].gameState;
	
	if (roomClient->tables[theTable].status == TS_PLAYING || roomClient->tables[theTable].status == TS_DISPLAYED)
		centerpiece->SetView(200190);
	else
		centerpiece->SetView(200191);
	
	centerpiece->Show();
}
