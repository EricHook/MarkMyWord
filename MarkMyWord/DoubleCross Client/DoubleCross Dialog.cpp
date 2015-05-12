#include "os.h"
#include "sci.h"
#include "globals.h"
#include "util.h"

#include "Client Include.h"

DoubleCrossShowdownDialog::DoubleCrossShowdownDialog(sciPlane *parentPlane, DoubleCrossClientTable *theTable)
	: sciDialogBox(FALSE, parentPlane)
{
	int i;
	
	for (i = 0 ; i < MAX_GAME_PLAYERS ; i++)
		lines[i] = NULL;
	
	okButton = NULL;
	
	table = theTable;
}

DoubleCrossShowdownDialog::~DoubleCrossShowdownDialog()
{
	int i;
	
	if (okButton) { delete okButton; okButton = NULL; }
	
	for (i = 0 ; i < MAX_GAME_PLAYERS ; i++)
		if (lines[i]) { delete lines[i]; lines[i] = NULL; }
	
	table = NULL;
}

void DoubleCrossShowdownDialog::Init()
{
	int i, x1;
	
	SetPic(1085);
	Center();
	
	for (i = 0; i < MAX_GAME_PLAYERS; i++) {
		if (!lines[i])
			lines[i] = new sciText();
		lines[i]->plane = this;
		lines[i]->ParamInit(4, "", 197, 16, 14, 48 + (18 * i), 10);
		lines[i]->Hide();
	}
	
	if (!okButton)
		okButton = new DoubleCrossShowdownOKButton(this);
	okButton->plane = this;
	okButton->Init();
	okButton->SetText("OK");
	okButton->SetFont(4, -1, 7);
	okButton->SetView(1003);
	okButton->SetButtonCels(4, 5, 6, 7);
	x1 = ((GetWidth() - okButton->GetWidth()) / 2);
	okButton->Posn(x1 - 6, (GetHeight() - okButton->GetHeight()) - 35);
	okButton->SetPri(10);
	// okButton->SetHotkey(13);
	okButton->Enable();
}

void DoubleCrossShowdownDialog::Show()
{
	char s[64], s2[16], names[MAX_GAME_PLAYERS][MAX_PLAYER_NAME];
	int i, j, temp, scores[MAX_GAME_PLAYERS], order[MAX_GAME_PLAYERS];
	
	sciDialogBox::Show();
	
	GetNames(names);
	GetScores(scores);
	
	for (i = 0 ; i < MAX_GAME_PLAYERS ; i++)
		order[i] = i;
	
	for (i = 0 ; i < (MAX_GAME_PLAYERS - 1) ; i++)
		for (j = i + 1 ; j < MAX_GAME_PLAYERS ; j++)
			if (scores[order[j]] > scores[order[i]]) {
				temp = order[i];
				order[i] = order[j];
				order[j] = temp;
			}
	
	for (i = 0 ; i < MAX_GAME_PLAYERS ; i++) {
		if (scores[order[i]] == -1)
			{ lines[i]->Hide(); continue; }
		
		if (scores[order[i]] == scores[order[0]])
			lines[i]->SetFont(2);
		else
			lines[i]->SetFont(4);
		
		strcpy(s, names[order[i]]);
		lines[i]->SetTextToWidth(s);
		
		while (lines[i]->CalculateFlatWidth() > lines[i]->GetWidth() - 30) {
			s[strlen(s) - 1] = 0;
			lines[i]->SetTextToWidth(s);
		}
		
		sprintf(s2, ": %d", scores[order[i]]);
		strcat(s, s2);
		lines[i]->SetTextToWidth(s);
		assert(lines[i]->CalculateFlatWidth() < lines[i]->GetWidth());
		lines[i]->Show();
	}
	
	okButton->Show();
	
	if (scores[order[0]] > scores[order[1]])
		sprintf(s, "%s Wins!", names[order[0]]);
	else
		sprintf(s, "It's a Tie!", names[order[0]]);
	
	SetTitleText(s, 32, 1, 0);
}

void DoubleCrossShowdownDialog::Hide()
{
	int i;
	
	sciDialogBox::Hide();
	
	for (i = 0 ; i < MAX_GAME_PLAYERS ; i++)
		lines[i]->Hide();
	
	okButton->Hide();
}

void DoubleCrossShowdownDialog::GetNames(char names[MAX_GAME_PLAYERS][MAX_PLAYER_NAME])
{
	int i;
	DoubleCrossClientPlayer *p;
	
	for (i = 0 ; i < MAX_GAME_PLAYERS ; i++) {
		if (table->gameState.players[i].playerID == PLAYER_ID_VOID || table->gameState.players[i].playerID == PLAYER_ID_EMPTY) {
			assert(table->gameState.players[i].status == DCPS_EMPTY);
			strcpy(names[i], "EMPTY");
		}
		else {
			assert(table->gameState.players[i].playerID >= 0);
			p = roomClient->GetPlayerFromID(table->gameState.players[i].playerID);
			
			if (p == NULL)
				{ assert(FALSE); strcpy(names[i], "???"); }
			else
				strcpy(names[i], p->info.login);
		}
	}
}

void DoubleCrossShowdownDialog::GetScores(int scores[MAX_GAME_PLAYERS])
{
	int i;
	
	for (i = 0; i < MAX_GAME_PLAYERS; i++) {
		if (table->gameState.players[i].playerID == PLAYER_ID_VOID || table->gameState.players[i].playerID == PLAYER_ID_EMPTY) {
			assert(table->gameState.players[i].status == DCPS_EMPTY);
			scores[i] = -1;
		}
		else {
			assert(table->gameState.players[i].playerID >= 0);
			scores[i] = table->gameState.seats[i].score;
		}
	}
}

/////

void DoubleCrossShowdownOKButton::DoClick(Boolean netMessage /* = FALSE */)
{
	SetState(DEFAULT_BUTTON_NORMAL);
	
	dialog->Hide();
	
	dialog->table->FinishREQUESTSHOWDOWN();
}

////////////////////////////////////////////////////////////////////////////////////////////////

DoubleCrossDefinitionDialog::DoubleCrossDefinitionDialog(sciPlane *parentPlane)
	: sciDialogBox(FALSE, parentPlane)
{
	text = NULL;
	credits = NULL;
	okButton = NULL;
}

DoubleCrossDefinitionDialog::~DoubleCrossDefinitionDialog()
{
	if (text) { delete text; text = NULL; }
	if (credits) { delete credits; credits = NULL; }
	if (okButton) { delete okButton; okButton = NULL; }
}

void DoubleCrossDefinitionDialog::Init()
{
	SetPic(200028);
	Center();
	
	text = new sciText();
	text->plane = this;
	text->SetLineSpacing(12);
	text->ParamInit(4, "", GetWidth() - 58, GetHeight() - 122, 18, 42, 10, JUST_TOP | JUST_LEFT);
	// text->SetBackgroundColor(100, 100, 100);
	
	credits = new sciText();
	credits->plane = this;
	credits->SetLineSpacing(12);
	credits->ParamInit(4, "", 210, 32, 105, 150, 10, JUST_TOP | JUST_LEFT);
	credits->SetText("Some definitions based on WordNet 1.6 Copyright © 1997 by Princeton University");
	// credits->SetBackgroundColor(100, 100, 100);
	
	okButton = new DoubleCrossDefinitionOKButton(this);
	okButton->plane = this;
	okButton->Init();
	okButton->SetText("OK");
	okButton->SetFont(4, -1, 7);
	okButton->SetView(1003);
	okButton->SetButtonCels(4, 5, 6, 7);
	okButton->Posn(20, (GetHeight() - okButton->GetHeight()) - 39);
	okButton->SetPri(10);
	// okButton->SetHotkey(13);
	okButton->Enable();
}

void DoubleCrossDefinitionDialog::Show(char *word, char *definition)
{
	char s[256];
	// int i;
	
	/*
	for (i = 0 ; mfs->definition[i] ; i++)
		if (mfs->definition[i] == '\r' || mfs->definition[i] == '\n')
			mfs->definition[i] = ' ';
	*/
	
	sciDialogBox::Show();
	
	sprintf(s, "Definition: %s", word);
	SetTitleText(s, 32, 1, 1);
	
	text->SetText(definition);
	
	text->Show();
	credits->Show();
	okButton->Show();
}

void DoubleCrossDefinitionDialog::Hide()
{
	sciDialogBox::Hide();
	
	text->Hide();
	credits->Hide();
	okButton->Hide();
}

/////

void DoubleCrossDefinitionOKButton::DoClick(Boolean netMessage /* = FALSE */)
{
	SetState(DEFAULT_BUTTON_NORMAL);
	
	dialog->Hide();
}
