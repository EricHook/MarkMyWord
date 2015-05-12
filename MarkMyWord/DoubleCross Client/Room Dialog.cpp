#include "os.h"
#include "sci.h"
#include "globals.h"
#include "util.h"
#include "prf.h"
#include "unihand.h"
#include "inifile.h"

#define NOTOOLBAR
#include <commctrl.h>
#include "..\all room clients\rc.h"

#include "Client Include.h"

////////////////////////////////////////////////////////////////////////////////////////////////

void RatingCheckbox::DoClick(Boolean netMessage)
{
	int i;

	sciCheckBox::DoClick(netMessage);

	//!IsSelected() since this is done before the selected flag is updated
	if(IsSelected())
	{
		for(i = 0; i < NUM_RATING_CATAGORIES; i++)
		{
			if(m_poParentDlg->m_apoRatingLimitsCheckBox[i])
				m_poParentDlg->m_apoRatingLimitsCheckBox[i]->Disable();
			if(m_poParentDlg->m_apoRatingSkillMarkerView[i])
				m_poParentDlg->m_apoRatingSkillMarkerView[i]->SetAlpha(100);
		}
	}
	else
	{
		for(i = 0; i < NUM_RATING_CATAGORIES; i++)
		{
			if(m_poParentDlg->m_apoRatingLimitsCheckBox[i])
				m_poParentDlg->m_apoRatingLimitsCheckBox[i]->Enable();
			if(m_poParentDlg->m_apoRatingSkillMarkerView[i])
				m_poParentDlg->m_apoRatingSkillMarkerView[i]->SetAlpha(255);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////

OptionsDialogBox::OptionsDialogBox(sciPlane *parentPlane)
	: sciDialogBox(FALSE, parentPlane)
{
	int i;
	
	version = ODBV_PERSONAL;
	inGame = FALSE;
	
	for (i = 0 ; i < 2 ; i++)
		buttons[i] = NULL;
	
	for (i = 0 ; i < 3 ; i++)
		frames[i] = NULL;

	titleBar = NULL;
	titleBarText = NULL;
	panelGroup = NULL;
	personalPanelButton = NULL;
	tablePanelButton = NULL;
	ratingsPanelButton = NULL;
	
	// personal options in room or game
	
	soundEffectsCheckBox = NULL;
	offensiveChatCheckBox = NULL;
	limitAnimationsCheckBox = NULL;
	
	shadowLabel = NULL;
	shadowTypeGroup = NULL;
	for (i = 0 ; i < 2 ; i++)
		shadowTypeButtons[i] = NULL;
	shadowScroll = NULL;
	faceMakerLabel = NULL;
	faceMakerInfoButton = NULL;
	
	// personal options in room
	
	lobbyChatControlsLabel = NULL;
	lobbyChatControlsGroup = NULL;
	for (i = 0 ; i < 2 ; i++)
		lobbyChatControlsButtons[i] = NULL;
	lobbyChatPushButtonsGroup = NULL;
	for (i = 0 ; i < 3 ; i++)
		lobbyChatPushButtonsButtons[i] = NULL;
	
	invitationsLabel = NULL;
	invitationsGroup = NULL;
	for (i = 0 ; i < 3 ; i++)
		invitationsButtons[i] = NULL;
	
	// personal options in game
	
	chatLabel = NULL;
	chatGroup = NULL;
	for (i = 0 ; i < 3 ; i++)
		chatButtons[i] = NULL;
	chatSoundsCheckBox = NULL;
	
	balloonsLabel = NULL;
	balloonsGroup = NULL;
	for (i = 0 ; i < 3 ; i++)
		balloonsButtons[i] = NULL;
	fadeCheckBox = NULL;
	
	logLabel = NULL;
	logGroup = NULL;
	for (i = 0 ; i < 2 ; i++)
		logButtons[i] = NULL;
	brightnessGroup = NULL;
	for (i = 0 ; i < 2 ; i++)
		brightnessButtons[i] = NULL;
	
	backdropButton = NULL;
	cardsButton = NULL;
	
	// table options
	
	joinGameLabel = NULL;
	joinGameGroup = NULL;
	for (i = 0 ; i < 3 ; i++)
		joinGameButtons[i] = NULL;
	
	watchersLabel = NULL;
	watchersGroup = NULL;
	for (i = 0 ; i < 3 ; i++)
		watchersButtons[i] = NULL;
	muteWatchersCheckBox = NULL;
	
	tableDescriptionLabel = NULL;
	tableDescriptionText = NULL;
	
	// Ratings options
	m_poTableIsRatedCheckBox = NULL;
	m_poRequireRatingToSitCheckBox = NULL;
	for(i = 0; i < NUM_RATING_CATAGORIES; i++)
		m_apoRatingLimitsCheckBox[i] = NULL;
	
	for(i = 0; i < NUM_RATING_CATAGORIES; i++)
		m_apoRatingSkillMarkerView[i] = NULL;

	m_poTop10Title = NULL;
	m_poTop10BodyL = NULL;
	m_poTop10BodyR = NULL;
	m_poCurrentAIRating = NULL;

	m_poRatingsDescTitle = NULL;
	m_poRatingsDescBody = NULL;
	m_poFeedbackButton = NULL;


	// game-specific options
	
	gameSpecificControls = NULL;
}

OptionsDialogBox::~OptionsDialogBox()
{
	int i;
	
	for (i = 0 ; i < 2 ; i++)
		if (buttons[i]) { delete buttons[i]; buttons[i] = NULL; }
	
	for (i = 0 ; i < 3 ; i++)
		if (frames[i]) { delete frames[i]; frames[i] = NULL; }

	if (titleBar) { delete titleBar; titleBar = NULL; }
	if (titleBarText) { delete titleBarText; titleBarText = NULL; }
	if (panelGroup) { delete panelGroup; panelGroup = NULL; }
	if (personalPanelButton) { delete personalPanelButton; personalPanelButton = NULL; }
	if (tablePanelButton) { delete tablePanelButton; tablePanelButton = NULL; }
	if (ratingsPanelButton) { delete ratingsPanelButton; ratingsPanelButton = NULL; }
	
	// personal options in room or game
	
	if (soundEffectsCheckBox) { delete soundEffectsCheckBox; soundEffectsCheckBox = NULL; }
	if (offensiveChatCheckBox) { delete offensiveChatCheckBox; offensiveChatCheckBox = NULL; }
	if (limitAnimationsCheckBox) { delete limitAnimationsCheckBox; limitAnimationsCheckBox = NULL; }
	
	if (shadowLabel) { delete shadowLabel; shadowLabel = NULL; }
	if (shadowTypeGroup) { delete shadowTypeGroup; shadowTypeGroup = NULL; }
	for (i = 0 ; i < 2 ; i++)
		if (shadowTypeButtons[i]) { delete shadowTypeButtons[i]; shadowTypeButtons[i] = NULL; }
	if (shadowScroll) { delete shadowScroll; shadowScroll = NULL; }
	if (faceMakerLabel) { delete faceMakerLabel; faceMakerLabel = NULL; }
	if (faceMakerInfoButton) { delete faceMakerInfoButton; faceMakerInfoButton = NULL; }
	
	// personal options in room
	
	if (lobbyChatControlsLabel) { delete lobbyChatControlsLabel; lobbyChatControlsLabel = NULL; }
	if (lobbyChatControlsGroup) { delete lobbyChatControlsGroup; lobbyChatControlsGroup = NULL; }
	for (i = 0 ; i < 2 ; i++)
		if (lobbyChatControlsButtons[i]) { delete lobbyChatControlsButtons[i]; lobbyChatControlsButtons[i] = NULL; }
	if (lobbyChatPushButtonsGroup) { delete lobbyChatPushButtonsGroup; lobbyChatPushButtonsGroup = NULL; }
	for (i = 0 ; i < 3 ; i++)
		if (lobbyChatPushButtonsButtons[i]) { delete lobbyChatPushButtonsButtons[i]; lobbyChatPushButtonsButtons[i] = NULL; }
	
	if (invitationsLabel) { delete invitationsLabel; invitationsLabel = NULL; }
	if (invitationsGroup) { delete invitationsGroup; invitationsGroup = NULL; }
	for (i = 0 ; i < 3 ; i++)
		if (invitationsButtons[i]) { delete invitationsButtons[i]; invitationsButtons[i] = NULL; }
	
	// personal options in game
	
	if (chatLabel) { delete chatLabel; chatLabel = NULL; }
	if (chatGroup) { delete chatGroup; chatGroup = NULL; }
	for (i = 0 ; i < 3 ; i++)
		if (chatButtons[i]) { delete chatButtons[i]; chatButtons[i] = NULL; }
	if (chatSoundsCheckBox) { delete chatSoundsCheckBox; chatSoundsCheckBox = NULL; }
	
	if (balloonsLabel) { delete balloonsLabel; balloonsLabel = NULL; }
	if (balloonsGroup) { delete balloonsGroup; balloonsGroup = NULL; }
	for (i = 0 ; i < 3 ; i++)
		if (balloonsButtons[i]) { delete balloonsButtons[i]; balloonsButtons[i] = NULL; }
	if (fadeCheckBox) { delete fadeCheckBox; fadeCheckBox = NULL; }
	
	if (logLabel) { delete logLabel; logLabel = NULL; }
	if (logGroup) { delete logGroup; logGroup = NULL; }
	for (i = 0 ; i < 2 ; i++)
		if (logButtons[i]) { delete logButtons[i]; logButtons[i] = NULL; }
	if (brightnessGroup) { delete brightnessGroup; brightnessGroup = NULL; }
	brightnessGroup = NULL;
	for (i = 0 ; i < 2 ; i++)
		if (brightnessButtons[i]) { delete brightnessButtons[i]; brightnessButtons[i] = NULL; }
	
	if (backdropButton) { delete backdropButton; backdropButton= NULL; }
	if (cardsButton) { delete cardsButton; cardsButton = NULL; }
	
	// table options
	
	if (joinGameLabel) { delete joinGameLabel; joinGameLabel = NULL; }
	if (joinGameGroup) { delete joinGameGroup; joinGameGroup = NULL; }
	for (i = 0 ; i < 3 ; i++)
		if (joinGameButtons[i]) { delete joinGameButtons[i]; joinGameButtons[i] = NULL; }
	
	if (watchersLabel) { delete watchersLabel; watchersLabel = NULL; }
	if (watchersGroup) { delete watchersGroup; watchersGroup = NULL; }
	for (i = 0 ; i < 3 ; i++)
		if (watchersButtons[i]) { delete watchersButtons[i]; watchersButtons[i] = NULL; }
	if (muteWatchersCheckBox) { delete muteWatchersCheckBox; muteWatchersCheckBox = NULL; }
	
	if (tableDescriptionLabel) { delete tableDescriptionLabel; tableDescriptionLabel = NULL; }
	if (tableDescriptionText) { delete tableDescriptionText; tableDescriptionText = NULL; }
	
	// Ratings options
	if (m_poTableIsRatedCheckBox) { delete m_poTableIsRatedCheckBox; m_poTableIsRatedCheckBox = NULL; }
	if (m_poRequireRatingToSitCheckBox) { delete m_poRequireRatingToSitCheckBox; m_poRequireRatingToSitCheckBox = NULL; }
	for(i = 0; i < NUM_RATING_CATAGORIES; i++)
		if (m_apoRatingLimitsCheckBox[i]) { delete m_apoRatingLimitsCheckBox[i]; m_apoRatingLimitsCheckBox[i] = NULL; }
	for(i = 0; i < NUM_RATING_CATAGORIES; i++)
		if (m_apoRatingSkillMarkerView[i]) { delete m_apoRatingSkillMarkerView[i]; m_apoRatingSkillMarkerView[i] = NULL; }

	if (m_poTop10Title) { delete m_poTop10Title; m_poTop10Title = NULL; }
	if (m_poTop10BodyL) { delete m_poTop10BodyL; m_poTop10BodyL = NULL; }
	if (m_poTop10BodyR) { delete m_poTop10BodyR; m_poTop10BodyR = NULL; }
	if (m_poCurrentAIRating) { delete m_poCurrentAIRating; m_poCurrentAIRating = NULL; }

	if (m_poRatingsDescTitle) { delete m_poRatingsDescTitle; m_poRatingsDescTitle = NULL; }
	if (m_poRatingsDescBody) { delete m_poRatingsDescBody; m_poRatingsDescBody = NULL; }
	if (m_poFeedbackButton) { delete m_poFeedbackButton; m_poFeedbackButton = NULL; }

	// game-specific options
	
	if (gameSpecificControls) { delete gameSpecificControls; gameSpecificControls = NULL; }
}

void OptionsDialogBox::Init()
{
	i18nStr getS;
	int i;
	int colorMappings[6][3] = {{142,131,199}, {129,168,223}, {93,169,93}, {196, 174, 0}, {254, 156, 38}, {255, 0, 0}}; //V,B,G,Y,O,R
	
	SetPic(200037);
	Center();
	SetTitleText(GetString(42, getS), 32, 1, 1);
	SetDraggableArea(12);
	
	for (i = 0 ; i < 2 ; i++) {
		buttons[i] = new OptionsDialogButton(i, this);
		buttons[i]->plane = this;
		buttons[i]->SetPri(1);
		buttons[i]->SetFont(4, -1, 7);
		buttons[i]->SetView(1003);
		buttons[i]->Init();
		buttons[i]->Enable();
	}
	
	buttons[0]->SetText(GetString(87, getS));
	buttons[1]->SetText(GetString(88, getS));
	buttons[0]->Posn(20, 310);
	buttons[1]->Posn(98, 310);
	// buttons[0]->SetHotkey(13);
	buttons[1]->SetHotkey(27);
	buttons[0]->SetButtonCels(4, 5, 6, 7);

	for (i = 0 ; i < 3 ; i++) {
		frames[i] = new FourSidedRect();
		frames[i]->plane = this;
		frames[i]->Init();
		frames[i]->SetPri(1);
		frames[i]->SetSideColors(DEFAULT_LTGREY, DEFAULT_LTGREY, DEFAULT_DKGREY, DEFAULT_DKGREY);
	}
	
	frames[0]->Posn(12, 36);
	frames[0]->SetSize(183, 261);
	frames[1]->Posn(198, 36);
	frames[1]->SetSize(350, 138);
	frames[2]->Posn(198, 177);
	frames[2]->SetSize(350, 120);

	titleBar = new sciRect();
	titleBar->plane = this;
	titleBar->Init();
	titleBar->SetPri(5);
	titleBar->SetInnerColor(200, 200, 200);
	titleBar->ClearOutlineColor();
	titleBar->Posn(1, 1);
	titleBar->SetSize(561, 30);

	titleBarText = new sciText();
	titleBarText->plane = this;
	titleBarText->ParamInit(2, GetString(42, getS), 530, 27, 4, 4, 1, JUST_VERTICAL | JUST_RIGHT);
	titleBarText->SetPri(10);

	//Create the group that holds the tabs
	if(!panelGroup)
		panelGroup = new sciButtonGroup();
	panelGroup->plane = this;
	panelGroup->Init();
	panelGroup->SetPri(10);

	personalPanelButton = new OptionsDialogPanelButton(ODBV_PERSONAL, this);
	panelGroup->AddToGroup(personalPanelButton);
	personalPanelButton->plane = this;
	personalPanelButton->Init(0, 0);
	personalPanelButton->SetView(25146);
	personalPanelButton->SetText(GetString(43, getS));
	personalPanelButton->SetFont(2, -1, 7);
	personalPanelButton->SetTextOffset(40, 3);
	personalPanelButton->SetCheckBoxCels(0, 0, 2, 2, 4, 4, 6, 4);
	personalPanelButton->Posn(5, 12);
	personalPanelButton->SetPri(10);
	personalPanelButton->Enable();

	tablePanelButton = new OptionsDialogPanelButton(ODBV_TABLE, this);
	panelGroup->AddToGroup(tablePanelButton);
	tablePanelButton->plane = this;
	tablePanelButton->Init(0, 0);
	tablePanelButton->SetView(25144);
	tablePanelButton->SetText(GetString(44, getS));
	tablePanelButton->SetFont(2, -1, 7);
	tablePanelButton->SetTextOffset(36, 3);
	tablePanelButton->SetCheckBoxCels(0, 0, 2, 2, 4, 4, 6, 4);
	tablePanelButton->Posn(139, 12);
	tablePanelButton->SetPri(10);
	tablePanelButton->SetID(ODBV_TABLE);
	tablePanelButton->Disable();

	ratingsPanelButton = new OptionsDialogPanelButton(ODBV_RATINGS, this);
	panelGroup->AddToGroup(ratingsPanelButton);
	ratingsPanelButton->plane = this;
	ratingsPanelButton->Init(0, 0);
	ratingsPanelButton->SetView(25144);
	ratingsPanelButton->SetText(GetString(344, getS));
	ratingsPanelButton->SetFont(2, -1, 7);
	ratingsPanelButton->SetTextOffset(36, 3);
	ratingsPanelButton->SetCheckBoxCels(0, 0, 2, 2, 4, 4, 6, 4);
	ratingsPanelButton->Posn(242, 12);
	ratingsPanelButton->SetPri(10);
	ratingsPanelButton->SetID(ODBV_RATINGS);
	ratingsPanelButton->Disable();
	if(!roomClient->HasRatings())
		ratingsPanelButton->Hide();

	// personal options in room or game
	
	soundEffectsCheckBox = new sciCheckBox();
	soundEffectsCheckBox->plane = this;
	soundEffectsCheckBox->SetText(GetString(45, getS));
	soundEffectsCheckBox->SetFont(4, -1, 7);
	soundEffectsCheckBox->SetView(1001);
	soundEffectsCheckBox->SetTextOffset(20, 2);
	soundEffectsCheckBox->Init();
	soundEffectsCheckBox->Posn(20, 50);
	soundEffectsCheckBox->SetPri(10);
	
	offensiveChatCheckBox = new sciCheckBox();
	offensiveChatCheckBox->plane = this;
	offensiveChatCheckBox->SetText(GetString(46, getS));
	offensiveChatCheckBox->SetFont(4, -1, 7);
	offensiveChatCheckBox->SetView(1001);
	offensiveChatCheckBox->SetTextOffset(20, 2);
	offensiveChatCheckBox->Init();
	offensiveChatCheckBox->Posn(20, 75);
	offensiveChatCheckBox->SetPri(10);
	
	limitAnimationsCheckBox = new sciCheckBox();
	limitAnimationsCheckBox->plane = this;
	limitAnimationsCheckBox->SetText(GetString(47, getS));
	limitAnimationsCheckBox->SetFont(4, -1, 7);
	limitAnimationsCheckBox->SetView(1001);
	limitAnimationsCheckBox->SetTextOffset(20, 2);
	limitAnimationsCheckBox->Init();
	limitAnimationsCheckBox->Posn(20, 100);
	limitAnimationsCheckBox->SetPri(10);
	
	if (!shadowScroll)
		shadowScroll = new ShadowScroll();
	shadowScroll->plane = this;
	shadowScroll->Init(this);
	shadowScroll->Posn(50, 264);
	shadowScroll->SetSize(106 ,16);
	shadowScroll->SetPri(13);
	
	shadowLabel = new sciText();
	shadowLabel->plane = this;
	shadowLabel->ParamInit(2, GetString(48, getS), 150, 20, 50, 102, 1, JUST_VERTICAL | JUST_LEFT);
	
	shadowTypeGroup = new sciButtonGroup();
	shadowTypeGroup->plane = this;
	shadowTypeGroup->Init();
	shadowTypeGroup->SetPri(10);
	
	for (i = 0 ; i < 2 ; i++) {
		shadowTypeButtons[i] = new ShadowRadioButton(shadowScroll, i);
		shadowTypeGroup->AddToGroup(shadowTypeButtons[i]);
		shadowTypeButtons[i]->plane = this;
		shadowTypeButtons[i]->Init(0, 0);
		shadowTypeButtons[i]->SetView(1063);
		shadowTypeButtons[i]->SetText("");
		shadowTypeButtons[i]->SetFont(4, -1, 7);
		shadowTypeButtons[i]->SetTextOffset(20);
		shadowTypeButtons[i]->Posn(60, 102 + 25 + i * 20);
		shadowTypeButtons[i]->SetPri(10);
		shadowTypeButtons[i]->Enable();
	}
	
	shadowTypeButtons[0]->SetText(GetString(49, getS));
	shadowTypeButtons[1]->SetText(GetString(50, getS));
	
	faceMakerLabel = new sciText();
	faceMakerLabel->plane = this;
	faceMakerLabel->ParamInit(4, "", 170, 16, 20, 282, 1, JUST_VERTICAL | JUST_HORIZONTAL);
	
	faceMakerInfoButton = new OptionsDialogButton(10, this);
	faceMakerInfoButton->plane = this;
	faceMakerInfoButton->SetPri(1);
	faceMakerInfoButton->SetView(200013);
	faceMakerInfoButton->SetButtonCels(0, 2, 0, 2);
	faceMakerInfoButton->Posn(134, 102 + 25 + 15);
	faceMakerInfoButton->SetFont(4, -1, 7);
	faceMakerInfoButton->SetText(GetString(51, getS)); // "Info..."
	faceMakerInfoButton->TextPosn(6, 5);
	faceMakerInfoButton->Init();
	faceMakerInfoButton->Enable();
	
	// personal options in room
	
	lobbyChatControlsLabel = new sciText();
	lobbyChatControlsLabel->plane = this;
	lobbyChatControlsLabel->ParamInit(2, GetString(52, getS), 175, 20, 215, 45, 10, JUST_VERTICAL | JUST_LEFT);
	
	lobbyChatControlsGroup = new sciButtonGroup();
	lobbyChatControlsGroup->plane = this;
	lobbyChatControlsGroup->Init();
	lobbyChatControlsGroup->SetPri(10);
	
	for (i = 0 ; i < 2 ; i++) {
		lobbyChatControlsButtons[i] = new sciRadioButton(i);
		lobbyChatControlsGroup->AddToGroup(lobbyChatControlsButtons[i]);
		lobbyChatControlsButtons[i]->plane = this;
		lobbyChatControlsButtons[i]->Init(0, 0);
		lobbyChatControlsButtons[i]->SetView(1063);
		lobbyChatControlsButtons[i]->SetText("");
		lobbyChatControlsButtons[i]->SetFont(4, -1, 7);
		lobbyChatControlsButtons[i]->SetTextOffset(20);
		lobbyChatControlsButtons[i]->Posn(215, 45 + 30 + i * 20);
		lobbyChatControlsButtons[i]->SetPri(10);
		lobbyChatControlsButtons[i]->Enable();
	}
	
	lobbyChatControlsButtons[0]->SetText(GetString(53, getS));
	lobbyChatControlsButtons[1]->SetText(GetString(54, getS));
	
	lobbyChatPushButtonsGroup = new sciButtonGroup();
	lobbyChatPushButtonsGroup->plane = this;
	lobbyChatPushButtonsGroup->Init();
	lobbyChatPushButtonsGroup->SetPri(10);
	
	for (i = 0 ; i < 3 ; i++) {
		lobbyChatPushButtonsButtons[i] = new sciRadioButton(i);
		lobbyChatPushButtonsGroup->AddToGroup(lobbyChatPushButtonsButtons[i]);
		lobbyChatPushButtonsButtons[i]->plane = this;
		lobbyChatPushButtonsButtons[i]->Init(0, 0);
		lobbyChatPushButtonsButtons[i]->SetView(1063);
		lobbyChatPushButtonsButtons[i]->SetText("");
		lobbyChatPushButtonsButtons[i]->SetFont(4, -1, 7);
		lobbyChatPushButtonsButtons[i]->SetTextOffset(20);
		lobbyChatPushButtonsButtons[i]->Posn(215 + 38, 45 + 30 + (i + 2) * 20);
		lobbyChatPushButtonsButtons[i]->SetPri(10);
		lobbyChatPushButtonsButtons[i]->Enable();
	}
	
	lobbyChatPushButtonsButtons[0]->SetText(GetString(55, getS));
	lobbyChatPushButtonsButtons[1]->SetText(GetString(56, getS));
	lobbyChatPushButtonsButtons[2]->SetText(GetString(57, getS));
	
	invitationsLabel = new sciText();
	invitationsLabel->plane = this;
	invitationsLabel->ParamInit(2, GetString(58, getS), 175, 20, 215, 193, 10, JUST_VERTICAL | JUST_LEFT);
	
	invitationsGroup = new sciButtonGroup();
	invitationsGroup->plane = this;
	invitationsGroup->Init();
	invitationsGroup->SetPri(10);
	
	for (i = 0 ; i < 3 ; i++) {
		invitationsButtons[i] = new sciRadioButton(i);
		invitationsGroup->AddToGroup(invitationsButtons[i]);
		invitationsButtons[i]->plane = this;
		invitationsButtons[i]->Init(0, 0);
		invitationsButtons[i]->SetView(1063);
		invitationsButtons[i]->SetText("");
		invitationsButtons[i]->SetFont(4, -1, 7);
		invitationsButtons[i]->SetTextOffset(20);
		invitationsButtons[i]->Posn(215, 218 + i * 20);
		invitationsButtons[i]->SetPri(10);
		invitationsButtons[i]->Enable();
	}
	
	invitationsButtons[0]->SetText(GetString(59, getS));
	invitationsButtons[1]->SetText(GetString(60, getS));
	invitationsButtons[2]->SetText(GetString(61, getS));
	
	// personal options in game
	
	chatLabel = new sciText();
	chatLabel->plane = this;
	chatLabel->ParamInit(2, GetString(62, getS), 100, 20, 215, 50, 10, JUST_VERTICAL | JUST_LEFT);
	
	chatGroup = new sciButtonGroup();
	chatGroup->plane = this;
	chatGroup->Init();
	chatGroup->SetPri(10);
	
	for (i = 0 ; i < 3 ; i++) {
		chatButtons[i] = new sciRadioButton(i);
		chatGroup->AddToGroup(chatButtons[i]);
		chatButtons[i]->plane = this;
		chatButtons[i]->SetFont(4, -1, 7);
		chatButtons[i]->SetView(1063);
		chatButtons[i]->SetText("");
		chatButtons[i]->SetTextOffset(20);
		chatButtons[i]->Init(0, 0);
		chatButtons[i]->Posn(215, 75 + i * 20);
		chatButtons[i]->SetPri(10);
		chatButtons[i]->Enable();
	}
	
	chatButtons[0]->SetText(GetString(63, getS));
	chatButtons[1]->SetText(GetString(64, getS));
	chatButtons[2]->SetText(GetString(65, getS));
	
	chatSoundsCheckBox = new sciCheckBox();
	chatSoundsCheckBox->plane = this;
	chatSoundsCheckBox->SetText(GetString(66, getS));
	chatSoundsCheckBox->SetFont(4, -1, 7);
	chatSoundsCheckBox->SetView(1001);
	chatSoundsCheckBox->SetTextOffset(20, 2);
	chatSoundsCheckBox->Init();
	chatSoundsCheckBox->Posn(215, 150);
	chatSoundsCheckBox->SetPri(10);
	
	balloonsLabel = new sciText();
	balloonsLabel->plane = this;
	balloonsLabel->ParamInit(2, GetString(67, getS), 100, 20, 340, 50, 10, JUST_VERTICAL | JUST_LEFT);
	
	balloonsGroup = new sciButtonGroup();
	balloonsGroup->plane = this;
	balloonsGroup->Init();
	balloonsGroup->SetPri(10);
	
	for (i = 0 ; i < 3 ; i++) {
		balloonsButtons[i] = new sciRadioButton(i);
		balloonsGroup->AddToGroup(balloonsButtons[i]);
		balloonsButtons[i]->plane = this;
		balloonsButtons[i]->SetFont(4, -1, 7);
		balloonsButtons[i]->SetView(1063);
		balloonsButtons[i]->SetText("");
		balloonsButtons[i]->SetTextOffset(20);
		balloonsButtons[i]->Init(0, 0);
		balloonsButtons[i]->Posn(340, 75 + i * 20);
		balloonsButtons[i]->SetPri(10);
		balloonsButtons[i]->Enable();
	}
	
	balloonsButtons[0]->SetText(GetString(68, getS));
	balloonsButtons[1]->SetText(GetString(69, getS));
	balloonsButtons[2]->SetText(GetString(70, getS));
	
	fadeCheckBox = new sciCheckBox();
	fadeCheckBox->plane = this;
	fadeCheckBox->SetText(GetString(71, getS));
	fadeCheckBox->SetFont(4, -1, 7);
	fadeCheckBox->SetView(1001);
	fadeCheckBox->SetTextOffset(20, 2);
	fadeCheckBox->Init();
	fadeCheckBox->Posn(340, 150);
	fadeCheckBox->SetPri(10);
	
	logLabel = new sciText();
	logLabel->plane = this;
	logLabel->ParamInit(2, GetString(72, getS), 100, 20, 426, 50, 10, JUST_VERTICAL | JUST_LEFT);
	
	logGroup = new sciButtonGroup();
	logGroup->plane = this;
	logGroup->Init();
	logGroup->SetPri(10);
	
	for (i = 0 ; i < 2 ; i++) {
		logButtons[i] = new OptionsDialogLoggingRB(i, this);
		logGroup->AddToGroup(logButtons[i]);
		logButtons[i]->plane = this;
		logButtons[i]->SetFont(4, -1, 7);
		logButtons[i]->SetView(1063);
		logButtons[i]->SetText("");
		logButtons[i]->SetTextOffset(20);
		logButtons[i]->Init(0, 0);
		logButtons[i]->Posn(426, 75 + i * 20);
		logButtons[i]->SetPri(10);
		logButtons[i]->Enable();
	}
	
	logButtons[0]->SetText(GetString(73, getS));
	logButtons[1]->SetText(GetString(74, getS));
	
	brightnessGroup = new sciButtonGroup();
	brightnessGroup->plane = this;
	brightnessGroup->Init();
	brightnessGroup->SetPri(10);
	
	for (i = 0 ; i < 2 ; i++) {
		brightnessButtons[i] = new OptionsDialogLoggingRB(i, this);
		brightnessGroup->AddToGroup(brightnessButtons[i]);
		brightnessButtons[i]->plane = this;
		brightnessButtons[i]->SetFont(4, -1, 7);
		brightnessButtons[i]->SetView(1063);
		brightnessButtons[i]->SetText("");
		brightnessButtons[i]->SetTextOffset(20);
		brightnessButtons[i]->Init(0, 0);
		brightnessButtons[i]->Posn(426 + 25, 75 + (i + 2) * 20);
		brightnessButtons[i]->SetPri(10);
		brightnessButtons[i]->Enable();
	}
	
	brightnessButtons[0]->SetText(GetString(75, getS));
	brightnessButtons[1]->SetText(GetString(76, getS));
	
	backdropButton = new OptionsDialogButton(20, this);
	backdropButton->plane = this;
	backdropButton->SetPri(1);
	backdropButton->SetView(1003);
	backdropButton->Posn(485, 310);
	backdropButton->SetFont(4, -1, 7);
	backdropButton->SetText(GetString(242, getS)); // "Backdrop"
	backdropButton->Init();
	backdropButton->Enable();
	
	cardsButton = new OptionsDialogButton(30, this);
	cardsButton->plane = this;
	cardsButton->SetPri(1);
	cardsButton->SetView(1003);
	cardsButton->Posn(405, 310);
	cardsButton->SetFont(4, -1, 7);
	cardsButton->SetText(GetString(245, getS)); // "Cards"
	cardsButton->Init();
	cardsButton->Enable();
	
	// table options
	
	joinGameLabel = new sciText();
	joinGameLabel->plane = this;
	joinGameLabel->ParamInit(2, GetString(77, getS), 100, 20, 208, 50, 10, JUST_VERTICAL | JUST_LEFT);
	
	joinGameGroup = new sciButtonGroup();
	joinGameGroup->plane = this;
	joinGameGroup->Init();
	joinGameGroup->SetPri(10);
	
	for (i = 0 ; i < 3 ; i++) {
		joinGameButtons[i] = new sciRadioButton(i);
		joinGameGroup->AddToGroup(joinGameButtons[i]);
		joinGameButtons[i]->plane = this;
		joinGameButtons[i]->SetFont(4, -1, 7);
		joinGameButtons[i]->SetView(1063);
		joinGameButtons[i]->SetText("");
		joinGameButtons[i]->SetTextOffset(20);
		joinGameButtons[i]->Init(0, 0);
		joinGameButtons[i]->Posn(208, 75 + i * 20);
		joinGameButtons[i]->SetPri(10);
		joinGameButtons[i]->Enable();
	}
	
	joinGameButtons[0]->SetText(GetString(78, getS));
	joinGameButtons[1]->SetText(GetString(79, getS));
	joinGameButtons[2]->SetText(GetString(80, getS));
	
	watchersLabel = new sciText();
	watchersLabel->plane = this;
	watchersLabel->ParamInit(2, GetString(81, getS), 100, 20, 312, 50, 10, JUST_VERTICAL | JUST_LEFT);
	
	watchersGroup = new sciButtonGroup();
	watchersGroup->plane = this;
	watchersGroup->Init();
	watchersGroup->SetPri(10);
	
	for (i = 0 ; i < 3 ; i++) {
		watchersButtons[i] = new sciRadioButton(i);
		watchersGroup->AddToGroup(watchersButtons[i]);
		watchersButtons[i]->plane = this;
		watchersButtons[i]->SetFont(4, -1, 7);
		watchersButtons[i]->SetView(1063);
		watchersButtons[i]->SetText("");
		watchersButtons[i]->SetTextOffset(20);
		watchersButtons[i]->Init(0, 0);
		watchersButtons[i]->Posn(312, 75 + i * 20);
		watchersButtons[i]->SetPri(10);
		watchersButtons[i]->Enable();
	}
	
	watchersButtons[0]->SetText(GetString(78, getS));
	watchersButtons[1]->SetText(GetString(79, getS));
	watchersButtons[2]->SetText(GetString(80, getS));
	
	muteWatchersCheckBox = new sciCheckBox();
	muteWatchersCheckBox->plane = this;
	muteWatchersCheckBox->SetText(GetString(82, getS));
	muteWatchersCheckBox->SetFont(4, -1, 7);
	muteWatchersCheckBox->SetView(1001);
	muteWatchersCheckBox->SetTextOffset(20, 2);
	muteWatchersCheckBox->Init();
	muteWatchersCheckBox->Posn(312, 138);
	muteWatchersCheckBox->SetPri(10);
	
	tableDescriptionLabel = new sciText();
	tableDescriptionLabel->plane = this;
	tableDescriptionLabel->ParamInit(2, GetString(83, getS), 150, 20, 208, 188, 10, JUST_VERTICAL | JUST_LEFT); //"Table Description
	
	tableDescriptionText = new EditBox();
	tableDescriptionText->plane = this;
	tableDescriptionText->ParamInit(4, GetString(373, getS), 202, 54, 208, 225, 10, JUST_TOP | JUST_LEFT); //"description"
	tableDescriptionText->Set3DOutLine(TRUE);		

	// ratings options
	m_poTableIsRatedCheckBox = new sciCheckBox();
	m_poTableIsRatedCheckBox->plane = this;
	m_poTableIsRatedCheckBox->SetText(GetString(345, getS));
	m_poTableIsRatedCheckBox->SetFont(4, -1, 7);
	m_poTableIsRatedCheckBox->SetView(1001);
	m_poTableIsRatedCheckBox->SetTextOffset(20, 2);
	m_poTableIsRatedCheckBox->Init();
	m_poTableIsRatedCheckBox->Posn(18, 50);
	m_poTableIsRatedCheckBox->SetPri(10);
	m_poTableIsRatedCheckBox->Hide();
	m_poTableIsRatedCheckBox->Unselect();

	m_poRequireRatingToSitCheckBox = new RatingCheckbox(this);
	m_poRequireRatingToSitCheckBox->plane = this;
	m_poRequireRatingToSitCheckBox->SetText(GetString(346, getS));
	m_poRequireRatingToSitCheckBox->SetFont(4, -1, 7);
	m_poRequireRatingToSitCheckBox->SetView(1001);
	m_poRequireRatingToSitCheckBox->SetTextOffset(20, 2);
	m_poRequireRatingToSitCheckBox->Init();
	m_poRequireRatingToSitCheckBox->Posn(18, 70);
	m_poRequireRatingToSitCheckBox->SetPri(10);
	m_poRequireRatingToSitCheckBox->Hide();
	m_poRequireRatingToSitCheckBox->Unselect();
	
	for(i = 0; i < NUM_RATING_CATAGORIES; i++)
	{
		m_apoRatingLimitsCheckBox[i] = new sciCheckBox();
		m_apoRatingLimitsCheckBox[i]->plane = this;
		m_apoRatingLimitsCheckBox[i]->SetText(GetString(347 + i, getS));
		m_apoRatingLimitsCheckBox[i]->SetFont(4, -1, 7);
		m_apoRatingLimitsCheckBox[i]->SetView(1001);
		m_apoRatingLimitsCheckBox[i]->SetTextOffset(32, 2);
		m_apoRatingLimitsCheckBox[i]->Init();
		m_apoRatingLimitsCheckBox[i]->Posn(37, 90 + i * 20);
		m_apoRatingLimitsCheckBox[i]->SetPri(10);
		m_apoRatingLimitsCheckBox[i]->Hide();
		m_apoRatingLimitsCheckBox[i]->Disable();
		m_apoRatingLimitsCheckBox[i]->Unselect();
	}

	for(i = 0; i < NUM_RATING_CATAGORIES; i++)
	{
		m_apoRatingSkillMarkerView[i] = new sciView();
		m_apoRatingSkillMarkerView[i]->plane = this;
		m_apoRatingSkillMarkerView[i]->SetView(220201);
		m_apoRatingSkillMarkerView[i]->SetCel(1);
		m_apoRatingSkillMarkerView[i]->Posn(55, 94 + i * 20);
		m_apoRatingSkillMarkerView[i]->SetPri(10);
		m_apoRatingSkillMarkerView[i]->Init();
		m_apoRatingSkillMarkerView[i]->SetColorMap(0, 0, 0, colorMappings[i][0], colorMappings[i][1], colorMappings[i][2]);
		m_apoRatingSkillMarkerView[i]->SetAlpha(100);
		m_apoRatingSkillMarkerView[i]->Hide();
	}

	m_poTop10Title = new sciText();
	m_poTop10Title->plane = this;
	m_poTop10Title->ParamInit(2, GetString(362, getS), 200, 16, 205, 40, 1, JUST_TOP | JUST_LEFT);
	m_poTop10Title->SetPri(10);

	m_poTop10BodyL = new sciText();
	m_poTop10BodyL->plane = this;
	m_poTop10BodyL->ParamInit(4, "", 150, 115, 205, 60, 1, JUST_TOP | JUST_LEFT);
	m_poTop10BodyL->SetPri(10);

	m_poTop10BodyR = new sciText();
	m_poTop10BodyR->plane = this;
	m_poTop10BodyR->ParamInit(4, "", 150, 115, 364, 60, 1, JUST_TOP | JUST_LEFT);
	m_poTop10BodyR->SetPri(10);

	m_poCurrentAIRating = new sciText();
	m_poCurrentAIRating->plane = this;
	m_poCurrentAIRating->ParamInit(4, "", 200, 16, 205, 155, 1, JUST_TOP | JUST_LEFT);
	m_poCurrentAIRating->SetPri(10);

	m_poRatingsDescTitle = new sciText();
	m_poRatingsDescTitle->plane = this;
	m_poRatingsDescTitle->ParamInit(2, GetString(363, getS), 200, 16, 205, 180, 1, JUST_TOP | JUST_LEFT);
	m_poRatingsDescTitle->SetPri(10);

	m_poRatingsDescBody = new sciText();
	m_poRatingsDescBody->plane = this;
	m_poRatingsDescBody->ParamInit(4, GetString(364, getS), 300, 100, 205, 200, 1, JUST_TOP | JUST_LEFT);
	m_poRatingsDescBody->SetPri(10);

	m_poFeedbackButton = new OptionsDialogButton(40, this);
	m_poFeedbackButton->plane = this;
	m_poFeedbackButton->SetPri(10);
	m_poFeedbackButton->SetView(1003);
	m_poFeedbackButton->Posn(475, 265);
	m_poFeedbackButton->SetFont(4, -1, 7);
	m_poFeedbackButton->SetText(GetString(367, getS)); // "Feedback..."
	m_poFeedbackButton->Init();
	m_poFeedbackButton->Enable();

	// game-specific options
	
	gameSpecificControls = new GAMESPECIFICCONTROLS();
	gameSpecificControls->Init(this);
}

void OptionsDialogBox::Show(int theVersion, Boolean theInGame)
{
	Boolean gameHasStarted = FALSE, watcher = FALSE;
	i18nStr getS;
	char acTemp[512];
	int i, b, table;
	PlayerNameRatings arTop10[10];

	version = theVersion;
	inGame = theInGame;
	
	table = roomClient->players[0].info.table;
	
	if (version == ODBV_TABLE && table == -1)
		version = ODBV_PERSONAL;
	
	roomClient->lastOptionsDialogVersion = version;
	
	if (roomClient->players[0].info.seat >= MAX_GAME_PLAYERS)
		watcher = TRUE;
	
	if (table != -1)
		if (roomClient->tables[table].status == TS_PLAYING || roomClient->tables[table].status == TS_DISPLAYED)
			gameHasStarted = TRUE;
	
	if (IsHidden()) {
		sciDialogBox::Show();
		
		b = roomClient->globalIniFile->ReadValue("SoundEffects", 1);
		if (b < 0 || b > 1) b = 1;
		if (b == 1)
			soundEffectsCheckBox->Select();
		else
			soundEffectsCheckBox->Unselect();
		
		b = roomClient->globalIniFile->ReadValue("OffensiveChat", 1);
		if (b < 0 || b > 1) b = 1;
		if (b == 1)
			offensiveChatCheckBox->Select();
		else
			offensiveChatCheckBox->Unselect();
		
		b = roomClient->globalIniFile->ReadValue("LimitAnimations", 0);
		if (b < 0 || b > 1) b = 0;
		if (b == 1)
			limitAnimationsCheckBox->Select();
		else
			limitAnimationsCheckBox->Unselect();
		
		b = roomClient->globalIniFile->ReadValue("ChatControls2", CHAT_CONTROLS_PUSH_BUTTONS);
		if (b < 0 || b > 1) b = CHAT_CONTROLS_PUSH_BUTTONS;
		lobbyChatControlsGroup->SelectButtonByID(b);
		
		b = roomClient->globalIniFile->ReadValue("ChatPushButtons", CHAT_PUSH_BUTTONS_PUBLIC);
		if (b < 0 || b > 2) b = CHAT_PUSH_BUTTONS_PUBLIC;
		lobbyChatPushButtonsGroup->SelectButtonByID(b);
		
		b = roomClient->globalIniFile->ReadValue("Invitations", INVITATIONS_ANYBODY);
		if (b < 0 || b > 2) b = INVITATIONS_ANYBODY;
		invitationsGroup->SelectButtonByID(b);
		
		b = roomClient->globalIniFile->ReadValue("ChatDisplay", CHAT_BALLOONS_ONLY);
		if (b < 0 || b > 2) b = CHAT_BALLOONS_ONLY;
		chatGroup->SelectButtonByID(b);
		
		b = roomClient->globalIniFile->ReadValue("ChatSounds", 1);
		if (b < 0 || b > 1) b = 1;
		if (b == 1)
			chatSoundsCheckBox->Select();
		else
			chatSoundsCheckBox->Unselect();
		
		b = roomClient->globalIniFile->ReadValue("BalloonSpeed", CHAT_BALLOONS_MEDIUM);
		if (b < 0 || b > 2) b = CHAT_BALLOONS_MEDIUM;
		balloonsGroup->SelectButtonByID(b);
		
		b = roomClient->globalIniFile->ReadValue("BalloonFade", 0);
		if (b < 0 || b > 1) b = 0;
		if (b == 1)
			fadeCheckBox->Select();
		else
			fadeCheckBox->Unselect();
		
		b = pApp->pIniFile->ReadValue("LogWindow", CHAT_LOG_FLOATING);
		if (b < 0 || b > 1) b = CHAT_LOG_FLOATING;
		logGroup->SelectButtonByID(b);
		
		b = pApp->pIniFile->ReadValue("LogBrightness", CHAT_LOG_DARK);
		if (b < 0 || b > 1) b = CHAT_LOG_DARK;
		brightnessGroup->SelectButtonByID(b);
		
		if (IHaveAHoyleCDProductInstalled(CUSTOMER_STRING_1, CUSTOMER_STRING_2, CUSTOMER_STRING_3))
			numFaceFiles = roomClient->CountFaceFiles(NULL);
		else
			numFaceFiles = 0;
		if (numFaceFiles != 0 && roomClient->globalIniFile->ReadValue("UseFaceFile", 0) == 1) {
			shadowTypeGroup->SelectButtonByID(1); // facemaker
			shadowTypeButtons[1]->GotSelected();
		}
		else {
			shadowTypeGroup->SelectButtonByID(0); // normal
			shadowTypeButtons[0]->GotSelected();
		}
		
		if (table != -1) {
			b = roomClient->tables[table].joinOption;
			if (b < TJO_OPEN || b > TJO_BY_INVITATION) b = TJO_OPEN;
			joinGameGroup->SelectButtonByID(b);
			
			b = roomClient->tables[table].watchersOption;
			if (b < TWO_OPEN || b > TWO_BY_INVITATION) b = TWO_OPEN;
			watchersGroup->SelectButtonByID(b);
			
			if (roomClient->tables[table].muteWatchers)
				muteWatchersCheckBox->Select();
			else
				muteWatchersCheckBox->Unselect();
			
			tableDescriptionText->SetText(roomClient->tables[table].description);
		
			if(roomClient->tables[table].ratingsFlags & RF_TABLE_RATED)
				m_poTableIsRatedCheckBox->Select();
			else
				m_poTableIsRatedCheckBox->Unselect();

			if(gameHasStarted)
				m_poTableIsRatedCheckBox->Disable();

			if(roomClient->tables[table].ratingsFlags & RF_TABLE_HAS_SKILL && !watcher)
			{
				m_poRequireRatingToSitCheckBox->Select();
				for(i = 0; i < NUM_RATING_CATAGORIES; i++)
				{
					m_apoRatingLimitsCheckBox[i]->Enable();
					m_apoRatingSkillMarkerView[i]->SetAlpha(255);
				}
			}
//			else
//				m_poRequireRatingToSitCheckBox->Hide();

			for(i = 0; i < NUM_RATING_CATAGORIES; i++)
			{
				if(roomClient->tables[table].ratingsSkills & (1 << i))
					m_apoRatingLimitsCheckBox[i]->Select();
				else
					m_apoRatingLimitsCheckBox[i]->Unselect();
			}
		}
		
		gameSpecificControls->PrepareToShow(table, version, gameHasStarted, inGame, watcher);
		panelGroup->SelectButtonByID(version);

	}
	
	////////////////
	
	soundEffectsCheckBox->Hide();
	offensiveChatCheckBox->Hide();
	limitAnimationsCheckBox->Hide();
	shadowLabel->Hide();
	shadowTypeGroup->Hide();
	shadowScroll->Hide();
	faceMakerLabel->Hide();
	faceMakerInfoButton->Hide();
	
	lobbyChatControlsLabel->Hide();
	lobbyChatControlsGroup->Hide();
	lobbyChatPushButtonsGroup->Hide();
	invitationsLabel->Hide();
	invitationsGroup->Hide();
	
	chatLabel->Hide();
	chatGroup->Hide();
	chatSoundsCheckBox->Hide();
	
	balloonsLabel->Hide();
	balloonsGroup->Hide();
	fadeCheckBox->Hide();
	
	logLabel->Hide();
	logGroup->Hide();
	brightnessGroup->Hide();
	
	backdropButton->Hide();
	cardsButton->Hide();
	
	joinGameLabel->Hide();
	joinGameGroup->Hide();
	
	watchersLabel->Hide();
	watchersGroup->Hide();
	muteWatchersCheckBox->Hide();
	
	tableDescriptionLabel->Hide();
	tableDescriptionText->Hide();
	
	// Ratings options
	m_poTableIsRatedCheckBox->Hide();
	m_poRequireRatingToSitCheckBox->Hide();
	for(i = 0; i < NUM_RATING_CATAGORIES; i++)
		m_apoRatingLimitsCheckBox[i]->Hide();
	for(i = 0; i < NUM_RATING_CATAGORIES; i++)
		m_apoRatingSkillMarkerView[i]->Hide();

	m_poTop10Title->Hide();
	m_poTop10BodyL->Hide();
	m_poTop10BodyR->Hide();
	m_poCurrentAIRating->Hide();

	m_poRatingsDescTitle->Hide();
	m_poRatingsDescBody->Hide();
	m_poFeedbackButton->Hide();

	for (i = 0 ; i < 2 ; i++)
		buttons[i]->Show();
	
	for (i = 0 ; i < 3 ; i++)
		frames[i]->Show();

	titleBar->Show();
	titleBarText->Show();
	
	if (table == -1)
		tablePanelButton->Disable();
	else
		tablePanelButton->Enable();

	panelGroup->Show();

	if(roomClient->HasRatings())
		ratingsPanelButton->Enable();
	else
	{
		ratingsPanelButton->Disable();
		ratingsPanelButton->Hide();
	}

	////////////////
	
	if (version == ODBV_PERSONAL) {
		SetTitleText(GetString(84, getS), 32, 1, 1);
		
		soundEffectsCheckBox->Show();
		offensiveChatCheckBox->Show();
		// limitAnimationsCheckBox->Show();
		shadowScroll->Show();
		shadowLabel->Show();
		shadowTypeGroup->Show();
		faceMakerInfoButton->Show();
		shadowTypeGroup->SelectButtonByID(0); // normal
		shadowTypeButtons[0]->GotSelected();
		shadowTypeButtons[1]->Disable();
		faceMakerLabel->Hide();
		if (numFaceFiles != 0) {
			shadowTypeButtons[1]->Enable();
			if (roomClient->globalIniFile->ReadValue("UseFaceFile", 0) == 1) {
				shadowTypeGroup->SelectButtonByID(1); // facemaker
				shadowTypeButtons[1]->GotSelected();
				faceMakerLabel->Show();
			}
		}
		if (inGame) {
			chatLabel->Show();
			chatGroup->Show();
			chatSoundsCheckBox->Show();
			
			balloonsLabel->Show();
			balloonsGroup->Show();
			// fadeCheckBox->Show();
			
			logLabel->Show();
			logGroup->Show();
			brightnessGroup->Show();
			
			if (table != -1) {
				if (roomClient->tables[table].CanChangeBackdrop())
					backdropButton->Show();
				if (roomClient->tables[table].CanChangeCardBacks())
					cardsButton->Show();
			}
		}
		else {
			lobbyChatControlsLabel->Show();
			lobbyChatControlsGroup->Show();
			lobbyChatPushButtonsGroup->Show();
			invitationsLabel->Show();
			invitationsGroup->Show();
		}
	}
	
	if (version == ODBV_TABLE) {
		SetTitleText(GetString(85, getS), 32, 1, 1);
		
		joinGameLabel->Show();
		joinGameGroup->Show();
		if (watcher)
			joinGameGroup->DisableAll();
		else
			joinGameGroup->EnableAll();
		
		watchersLabel->Show();
		watchersGroup->Show();
		if (watcher)
			watchersGroup->DisableAll();
		else
			watchersGroup->EnableAll();
		muteWatchersCheckBox->Show();
		if (watcher)
			muteWatchersCheckBox->Disable();
		else
			muteWatchersCheckBox->Enable();
		
		tableDescriptionLabel->Show();
		tableDescriptionText->Show();
		if (watcher)
			tableDescriptionText->Disable();
		else
			tableDescriptionText->Enable();

	}

		// Ratings options
	if (version == ODBV_RATINGS) {
		SetTitleText(GetString(356, getS), 32, 1, 1);

		if (table == -1 || watcher)
		{
			m_poTableIsRatedCheckBox->Disable();
			m_poRequireRatingToSitCheckBox->Disable();
			for(i = 0; i < NUM_RATING_CATAGORIES; i++)
			{
				m_apoRatingLimitsCheckBox[i]->Disable();
				m_apoRatingSkillMarkerView[i]->SetAlpha(100);
			}
		}
		else
		{
			if(roomClient->IsRatedRoom() || gameHasStarted) // Players cannot unrate a game in rated room or if already started
				m_poTableIsRatedCheckBox->Disable();  
			else
				m_poTableIsRatedCheckBox->Enable();

			m_poRequireRatingToSitCheckBox->Enable();
			if(roomClient->tables[table].ratingsFlags & RF_TABLE_HAS_SKILL)
			{
				for(i = 0; i < NUM_RATING_CATAGORIES; i++)
				{
					m_apoRatingLimitsCheckBox[i]->Enable();
					m_apoRatingSkillMarkerView[i]->SetAlpha(255);
				}
			}
		}

		m_poTableIsRatedCheckBox->Show();
		m_poRequireRatingToSitCheckBox->Show();
		for(i = 0; i < NUM_RATING_CATAGORIES; i++)
			m_apoRatingLimitsCheckBox[i]->Show();
		for(i = 0; i < NUM_RATING_CATAGORIES; i++)
			m_apoRatingSkillMarkerView[i]->Show();

		m_poTop10Title->Show();
		roomClient->GetTop10Players(arTop10);

		acTemp[0] = 0;
		for(i = 0; i < 5; i++)
		{
			if(arTop10[i].acName[0])
				sprintf(acTemp + strlen(acTemp), GetString(365, getS), i+1, arTop10[i].acName, arTop10[i].sRating);
			else
				sprintf(acTemp + strlen(acTemp), GetString(366, getS), i+1);

		}

		m_poTop10BodyL->SetText(acTemp);
		m_poTop10BodyL->Show();

		acTemp[0] = 0;
		for( ; i < 10; i++)
		{
			if(arTop10[i].acName[0])
				sprintf(acTemp + strlen(acTemp), GetString(365, getS), i+1, arTop10[i].acName, arTop10[i].sRating);
			else
				sprintf(acTemp + strlen(acTemp), GetString(366, getS), i+1);

		}

		m_poTop10BodyR->SetText(acTemp);
		m_poTop10BodyR->Show();

		if(roomClient->m_iAIRating)
		{
			sprintf(acTemp, GetString(371, getS), roomClient->m_iAIRating);
			m_poCurrentAIRating->SetText(acTemp);
			m_poCurrentAIRating->Show();
		}

		m_poRatingsDescTitle->Show();
		m_poRatingsDescBody->Show();
		m_poFeedbackButton->Show();
	}
	
	////////////////
	
	gameSpecificControls->Show(table, version, gameHasStarted, inGame, watcher);
}

Boolean OptionsDialogPanelButton::HandleEvent(sciEvent &event)
{
	int version;
	
	if (!enabled || hidden || event.claimed)
		return sciRadioButton::HandleEvent(event);
	
	sciRadioButton::HandleEvent(event);
	
	version = parent->panelGroup->GetSelectedButtonID();
	if (version != parent->version) {
		parent->version = version;
		parent->Show(parent->version, parent->inGame);
	}
	
	return event.claimed;
}

Boolean OptionsDialogLoggingRB::HandleEvent(sciEvent &event)
{
	int i = 0;

	switch(event.type)
	{
	case MOUSE_UP:
		if ((!sciFeature::OnMe(event) && !mouseDown) || event.claimed)
			break;
		if ((event.modifiers & LEFT_MOUSE) && mouseDown)
		{
			if (OnMe(event))
			{
				//This means that we are currently logging balloons only.  However, 
				// since the player just changed a log window setting, set to use both
				// the log window and the balloons.
				if(parent->chatGroup->GetSelectedButtonID() == 0)
				{
					parent->chatGroup->SelectButtonByID(2);
				}
			}
		}
	}

	return sciRadioButton::HandleEvent(event);
}

void OptionsDialogButton::DoClick(BOOL netMessage /*= FALSE*/)
{
	Boolean changed;
	char s[64], oldFaceFileName[MAX_PATH], buffer[1024], getS[256];
	int table, oldUseFaceFile, i;
	char newRatingsFlags, newRatingsSkills;
	// TickerDisplay *tickerDisplay;
	GAMEDISPLAY *display;
	MTS_ROOM_CHANGE_TABLE_SETTINGS_MESSAGE *mts = (MTS_ROOM_CHANGE_TABLE_SETTINGS_MESSAGE *)buffer;
	
	SetState(DEFAULT_BUTTON_NORMAL);
	
	if (roomClient->status < RCS_IN_ROOM)
		return;
	
	if (id == 0) { // OK
		roomClient->globalIniFile->WriteValue("SoundEffects", parent->soundEffectsCheckBox->IsSelected() ? 1 : 0);
		roomClient->globalIniFile->WriteValue("OffensiveChat", parent->offensiveChatCheckBox->IsSelected() ? 1 : 0);
		roomClient->globalIniFile->WriteValue("LimitAnimations", parent->limitAnimationsCheckBox->IsSelected() ? 1 : 0);
		roomClient->globalIniFile->WriteValue("ChatControls2", parent->lobbyChatControlsGroup->GetSelectedButtonID());
		roomClient->globalIniFile->WriteValue("ChatPushButtons", parent->lobbyChatPushButtonsGroup->GetSelectedButtonID());
		roomClient->globalIniFile->WriteValue("Invitations", parent->invitationsGroup->GetSelectedButtonID());
		roomClient->globalIniFile->WriteValue("ChatDisplay", parent->chatGroup->GetSelectedButtonID());
		roomClient->globalIniFile->WriteValue("ChatSounds", parent->chatSoundsCheckBox->IsSelected() ? 1 : 0);
		roomClient->globalIniFile->WriteValue("BalloonSpeed", parent->balloonsGroup->GetSelectedButtonID());
		roomClient->globalIniFile->WriteValue("BalloonFade", parent->fadeCheckBox->IsSelected() ? 1 : 0);
		roomClient->globalIniFile->WriteValue("PreferBalloons", 
			parent->chatGroup->GetSelectedButtonID() == CHAT_LOG_ONLY ? 0 : 1);
		oldUseFaceFile = roomClient->globalIniFile->ReadValue("UseFaceFile", 0);
		roomClient->globalIniFile->WriteValue("UseFaceFile", parent->shadowTypeGroup->GetSelectedButtonID());
		pApp->pIniFile->WriteValue("LogWindow", parent->logGroup->GetSelectedButtonID());
		pApp->pIniFile->WriteValue("LogBrightness", parent->brightnessGroup->GetSelectedButtonID());
		
		if (parent->shadowTypeGroup->GetSelectedButtonID() == 1) {
			roomClient->globalIniFile->ReadString("FaceFileName", "", oldFaceFileName, MAX_PATH - 1);
			roomClient->globalIniFile->WriteString("FaceFileName", parent->faceMakerLabel->GetText());
			if (oldUseFaceFile != 1 || strcmp(oldFaceFileName, parent->faceMakerLabel->GetText())) {
				sprintf(s, "changeshadow %d", roomClient->players[0].info.shadow);
				roomClient->DoChatCommand(NULL, s);
			}
		}
		else
			if (oldUseFaceFile != 0 || parent->shadowScroll->GetSelectedItemID() != roomClient->players[0].info.shadow) {
				sprintf(s, "changeshadow %d", parent->shadowScroll->GetSelectedItemID());
				roomClient->DoChatCommand(NULL, s);
			}
		
		// send changed table settings
		
		table = roomClient->players[0].info.table;
		
		if (table != -1 && roomClient->players[0].info.seat < MAX_GAME_PLAYERS) { // watchers don't get to change options
			
			/*
			tickerDisplay = roomClient->tables[table].display->tickerDisplay;
			if (game->IsAnimations()) {
				tickerDisplay->increment = 1;
				tickerDisplay->speed = 1;
			}
			else {
				tickerDisplay->increment = 60;
				tickerDisplay->speed = 120;
			}
			*/
			
			changed = roomClient->tables[table].joinOption != parent->joinGameGroup->GetSelectedButtonID() ||
					  roomClient->tables[table].watchersOption != parent->watchersGroup->GetSelectedButtonID() ||
					  roomClient->tables[table].muteWatchers != parent->muteWatchersCheckBox->IsSelected() ||
					  strcmp(roomClient->tables[table].description, parent->tableDescriptionText->GetText());

			newRatingsFlags = 0;
			if (parent->m_poTableIsRatedCheckBox->IsSelected())
				newRatingsFlags |= RF_TABLE_RATED;
			if (parent->m_poRequireRatingToSitCheckBox->IsSelected())
				newRatingsFlags |= RF_TABLE_HAS_SKILL;

			newRatingsSkills = 0;
			for(i = 0; i < NUM_RATING_CATAGORIES; i++)
				newRatingsSkills |= parent->m_apoRatingLimitsCheckBox[i]->IsSelected() << i;

			// There aren't any checked, so make sure to turn off that the table has skill requirements
			if(newRatingsSkills == 0)
				newRatingsFlags &= ~RF_TABLE_HAS_SKILL;
			
			changed |= (roomClient->tables[table].ratingsFlags != newRatingsFlags);
			changed |= (roomClient->tables[table].ratingsSkills != newRatingsSkills);

			changed |= parent->gameSpecificControls->DoClickOK(table, mts->gameValues);
			
			if (changed) {
				mts->messageType = MTS_ROOM_CHANGE_TABLE_SETTINGS;
				mts->table = table;
				mts->joinOption = parent->joinGameGroup->GetSelectedButtonID();
				mts->watchersOption = parent->watchersGroup->GetSelectedButtonID();
				mts->muteWatchers = parent->muteWatchersCheckBox->IsSelected();
				mts->ratingsFlags = newRatingsFlags;
				mts->ratingsSkills = newRatingsSkills;
				strnzcpy(mts->description, parent->tableDescriptionText->GetText(), MAX_TABLE_DESCRIPTION - 1);
				mts->descriptionLength = strlen(mts->description) + 1;
				
				roomClient->SendMessageToServer((char *)mts, sizeof(MTS_ROOM_CHANGE_TABLE_SETTINGS_MESSAGE) + mts->descriptionLength);
			}
		}
		
		roomClient->PreferencesChanged(); // react to personal preference changes
		
		parent->Hide();
	}
	
	if (id == 1) // Cancel
		parent->Hide();
	
	if (id == 2) // toggle
		parent->Show((parent->version == ODBV_PERSONAL) ? ODBV_TABLE : ODBV_PERSONAL, parent->inGame);
	
	if (id == 10 && won) // face maker info
		won->LaunchBrowser(GetString(86, getS));
	
	if (id == 20) { // backdrop
		table = roomClient->players[0].info.table;
		if (table != -1) {
			display = roomClient->tables[table].display;
			if (!display->backdropDialog) {
				display->backdropDialog = new BackdropDialogBox(plane, &roomClient->tables[table]);
				display->backdropDialog->Init();
			}
			if (display->backdropDialog->IsHidden())
				display->backdropDialog->Show();
		}
	}
	
	if (id == 30) { // card backs
		table = roomClient->players[0].info.table;
		if (table != -1) {
			display = roomClient->tables[table].display;
			if (!display->cardBacksDialog) {
				display->cardBacksDialog = new CardBacksDialogBox(plane, &roomClient->tables[table]);
				display->cardBacksDialog->Init();
			}
			if (display->cardBacksDialog->IsHidden())
				display->cardBacksDialog->Show();
		}
	}

	if (id == 40) // Feedback... (for ratings)
	{
		if (!roomClient->complaintDialog) {
			roomClient->complaintDialog = new ComplaintDialogBox(pApp->pMainPlane);
			roomClient->complaintDialog->Init();
		}
		roomClient->complaintDialog->Show(-2, FALSE);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////

ShadowScroll::ShadowScroll() : sciScrollBox(GDefs::HORIZONTAL)
{
	normalView = NULL;
	faceMakerView = NULL;
	frame = NULL;
	selectedItemID = 0;
	parent = NULL;
}

ShadowScroll::~ShadowScroll()
{
	if (normalView) { delete normalView; normalView = NULL; }
	if (faceMakerView) { delete faceMakerView; faceMakerView = NULL; }
	if (frame) { delete frame; frame = NULL; }
}

void ShadowScroll::Init(OptionsDialogBox *theParent)
{
	sciScrollBox::Init();
	
	parent = theParent;
	
	Enable();
	
	if (!normalView)
		normalView = new sciView();
	normalView->plane = plane;
	normalView->SetPri(15);
	normalView->SetView(200039);
	normalView->Posn(55, 176);
	normalView->Init();
	
	if (!faceMakerView)
		faceMakerView = new FaceMakerFace();
	faceMakerView->Init(plane, NULL);
	faceMakerView->SetPri(15);
	faceMakerView->Posn(55, 176);
	
	if (!frame)
		frame = new sciView();
	frame->plane = plane;
	frame->SetPri(13);
	frame->SetView(25256);
	frame->Posn(50, 168);
	frame->Init();
}

void ShadowScroll::Show()
{
	sciScrollBox::Show();
	
	if (InFaceMakerMode()) {
		faceMakerView->Show();
		normalView->Hide();
	}
	else {
		if (pApp->pResourceManager->ResourceIsAvailable(RES_TSPRITE, roomClient->GetPlayerView(selectedItemID)))
			normalView->SetView(roomClient->GetPlayerView(selectedItemID)); // use full art, if it's available
		else
			normalView->SetView(200039); // revert to guaranteed art
		normalView->Show();
		faceMakerView->Hide();
	}
	
	frame->Show();
}

void ShadowScroll::Hide()
{
	sciScrollBox::Hide();
	
	normalView->Hide();
	faceMakerView->Hide();
	frame->Hide();
}

void ShadowScroll::SelectItem(int newItemID)
{
	if (normalView->GetView() == 200039 && pApp->pResourceManager->ResourceIsAvailable(RES_TSPRITE, roomClient->GetPlayerView(selectedItemID)))
		normalView->SetView(roomClient->GetPlayerView(selectedItemID)); // use full art, if it's available
	
	if(!pApp->pResourceManager->ResourceIsAvailable(RES_TSPRITE, roomClient->GetPlayerView(selectedItemID))) {
		normalView->SetView(200039);
	}

	if (normalView->GetView() == 200039)
		newItemID = 0; // revert to guaranteed art
	
	selectedItemID = newItemID;
	DoRefresh(selectedItemID);
}

int ShadowScroll::GetSelectedItemID()
{
	if (normalView->GetView() == 200039 && pApp->pResourceManager->ResourceIsAvailable(RES_TSPRITE, roomClient->GetPlayerView(selectedItemID)))
		normalView->SetView(roomClient->GetPlayerView(selectedItemID)); // use full art, if it's available
	
	if(!pApp->pResourceManager->ResourceIsAvailable(RES_TSPRITE, roomClient->GetPlayerView(selectedItemID))) {
		normalView->SetView(200039);
		selectedItemID = 0;
	}

	return selectedItemID;
}

int ShadowScroll::GetNumItems()
{
	if (normalView->GetView() == 200039 && pApp->pResourceManager->ResourceIsAvailable(RES_TSPRITE, roomClient->GetPlayerView(selectedItemID)))
		normalView->SetView(roomClient->GetPlayerView(selectedItemID)); // use full art, if it's available
	
	if (InFaceMakerMode())
		return parent->numFaceFiles;

	if(!pApp->pResourceManager->ResourceIsAvailable(RES_TSPRITE, roomClient->GetPlayerView(selectedItemID))) {
		normalView->SetView(200039);
		normalView->SetCel(0);
	}
	
	if (normalView->GetView() == 200039)
		return 1; // guarenteed art


	return roomClient->GetNumShadows(); // number of faces in all the standard face views
}

int ShadowScroll::GetMaxVisibleItems()
{
	return 1;
}

void ShadowScroll::DoScroll(int direction, int numItems)
{
	if (direction == GDefs::REVERSE) {
		if (selectedItemID - numItems < 0)
			selectedItemID = 0;
		else
			selectedItemID -= numItems;
	}
	else {
		if (selectedItemID + numItems > (GetNumItems() - 1))
			selectedItemID = (GetNumItems() - 1);
		else
			selectedItemID += numItems;
	}
	assert(selectedItemID >= 0 && selectedItemID <= (GetNumItems() - 1));
	DoRefresh(selectedItemID);
}

void ShadowScroll::DoRefresh(int firstItem)
{
	char fileName[MAX_PATH];
	
	selectedItemID = firstItem;
	
	if (InFaceMakerMode() && roomClient->LoadNthFaceFile(selectedItemID, &faceMakerView->faceInfo, fileName)) {
		faceMakerView->Show();
		parent->faceMakerLabel->Show();
		parent->faceMakerLabel->SetText(fileName);
		normalView->Hide();
	}
	else {
		if(!pApp->pResourceManager->ResourceIsAvailable(RES_TSPRITE, roomClient->GetPlayerView(selectedItemID))) {
			normalView->SetView(200039);
			normalView->SetCel(0);
		}
		else {
			normalView->SetView(roomClient->GetPlayerView(selectedItemID));
			normalView->SetCel(roomClient->GetPlayerViewCel(selectedItemID, FALSE));
		}
		normalView->Show();
		faceMakerView->Hide();
		parent->faceMakerLabel->Hide();
	}
	frame->Show();
	
	SnapThumbnail(selectedItemID);
}

Boolean ShadowScroll::InFaceMakerMode()
{
	return parent &&
		   parent->shadowTypeGroup->GetSelectedButtonID() == 1 &&
		   pApp->pResourceManager->ResourceIsAvailable(RES_TSPRITE, 1001001);
}

void ShadowRadioButton::GotSelected()
{
	char fileName[MAX_PATH];
	int item;
	
	if (buttonID == 1) {
		roomClient->globalIniFile->ReadString("FaceFileName", "", fileName, MAX_PATH - 1);
		item = roomClient->CountFaceFiles(fileName);
		if (item == -1)
			item = 0;
	}
	else
		item = roomClient->players[0].info.shadow;
	
	scroller->SelectItem(item);
}

////////////////////////////////////////////////////////////////////////////////////////////////

ProfileDialogBox::ProfileDialogBox(sciPlane *parentPlane)
	: sciDialogBox(FALSE, parentPlane)
{
	int i;
	
	playerID = -1;
	
	for (i = 0 ; i < 2 ; i++)
		buttons[i] = NULL;
	
	shadow			= NULL;
	icon			= NULL;
	ageLabel		= NULL;
	age				= NULL;
	sexLabel		= NULL;
	sex				= NULL;
	cityLabel		= NULL;
	city			= NULL;
	profile			= NULL;
	faceMakerView	= NULL;
}

ProfileDialogBox::~ProfileDialogBox()
{
	int i;
	
	for (i = 0 ; i < 2 ; i++)
		if (buttons[i]) { delete buttons[i]; buttons[i] = NULL; }
	
	if (shadow) { delete shadow; shadow = NULL; }
	if (icon) { delete icon; icon = NULL; }
	if (ageLabel) { delete ageLabel; ageLabel = NULL; }
	if (age) { delete age; age = NULL; }
	if (sexLabel) { delete sexLabel; sexLabel = NULL; }
	if (sex) { delete sex; sex = NULL; }
	if (cityLabel) { delete cityLabel; cityLabel = NULL; }
	if (city) { delete city; city = NULL; }
	if (profile) { delete profile; profile = NULL; }
	if (faceMakerView) { delete faceMakerView; faceMakerView = NULL; }
}

void ProfileDialogBox::Init()
{
	i18nStr getS;
	int i;
	
	SetPic(200028);
	Center();
	SetTitleText(GetString(92, getS), 32, 1, 1);
	SetDraggableArea(25);
	
	for (i = 0 ; i < 2 ; i++) {
		buttons[i] = new ProfileDialogButton(i, this);
		buttons[i]->plane = this;
		buttons[i]->SetPri(1);
		buttons[i]->SetFont(4, -1, 7);
		buttons[i]->SetView(1003);
		buttons[i]->Init();
		buttons[i]->Enable();
	}
	
	buttons[0]->SetText(GetString(87, getS));
	buttons[1]->SetText(GetString(88, getS));
	buttons[0]->Posn(20, 154);
	buttons[1]->Posn(98, 154);
	// buttons[0]->SetHotkey(13);
	buttons[1]->SetHotkey(27);
	buttons[0]->SetButtonCels(4, 5, 6, 7);
	
	shadow = new sciView();
	shadow->plane = this;
	shadow->ParamInit(200040, 14, 41, 1);

	faceMakerView = new FaceMakerFace();
	faceMakerView->Init(this, NULL);
	faceMakerView->SetPri(1);
	faceMakerView->Posn(14, 41);
	
	// the room face is scaled down from the game face
	faceMakerView->SetHalfSize(true, ST_BILINEAR);
	
	icon = new sciView();
	icon->plane = this;
	icon->ParamInit(200017, 60, 45, 1);
	
	ageLabel = new sciText();
	ageLabel->plane = this;
	ageLabel->ParamInit(4, GetString(93, getS), 40, 16, 20, 86, 1, JUST_TOP | JUST_LEFT);
	
	age = new EditBox();
	age->plane = this;
	age->SetTabID(0);
	age->ParamInit(4, "", 40, 16, 47, 86, 1, JUST_TOP | JUST_LEFT);
	age->Set3DOutLine(TRUE);
	
	sexLabel = new sciText();
	sexLabel->plane = this;
	sexLabel->ParamInit(4, GetString(94, getS), 40, 16, 20, 105, 1, JUST_TOP | JUST_LEFT);
	
	sex = new EditBox();
	sex->plane = this;
	sex->SetTabID(1);
	sex->ParamInit(4, "", 40, 16, 47, 105, 1, JUST_TOP | JUST_LEFT);
	sex->Set3DOutLine(TRUE);
	
	cityLabel = new sciText();
	cityLabel->plane = this;
	cityLabel->ParamInit(4, GetString(95, getS), 40, 16, 20, 124, 1, JUST_TOP | JUST_LEFT);
	
	city = new EditBox();
	city->plane = this;
	city->SetTabID(2);
	city->ParamInit(4, "", 270, 16, 47, 124, 1, JUST_TOP | JUST_LEFT);
	city->Set3DOutLine(TRUE);
	
	profile = new EditBox();
	profile->plane = this;
	profile->SetTabID(3);
	profile->SetLastTab(TRUE);
	profile->ParamInit(4, "profile", 227, 76, 90, 45, 1, JUST_TOP | JUST_LEFT);
	profile->Set3DOutLine(TRUE);
	profile->WantReturn(TRUE);
}

void ProfileDialogBox::ExtractProfileField(char *key, char *profile, char *s)
{
	char fullKey[20], *p;
	int i;
	
	sprintf(fullKey, "%s:`", key);
	
	s[0] = 0;
	p = strstr(profile, fullKey);
	if (p != NULL) {
		p += strlen(fullKey);
		for (i = 0 ; p[0] && p[0] != '`' && i < (MAX_PLAYER_PROFILE - 1) ; p++, i++)
			s[i] = p[0];
		s[i] = 0;
	}
}

void ProfileDialogBox::StripSpecialQuote(char *s)
{
	int i;
	
	for (i = 0 ; s[i] ; i++)
		if (s[i] == '`')
			s[i] = '\'';
}

void ProfileDialogBox::Show(Boolean thePlayerID)
{
	char s[MAX_PLAYER_PROFILE], getS[256];
	int i;
	ROOMCLIENTPLAYER *p;
	
	playerID = thePlayerID;
	
	p = roomClient->GetPlayerFromID(playerID);
	
	if (p == NULL)
		{ assert(FALSE); return; }
	
	sprintf(s, GetString(133, getS), p->info.login);
	SetTitleText(s, 32, 1, 1);
	
	sciDialogBox::Show();
	
	////////////////
	
	for (i = 0 ; i < 2 ; i++)
		buttons[i]->Show();
	
	faceMakerView->faceInfo = p->info.faceInfo;
	
	/* THIS IS NO LONGER CORRECT BECAUSE INDIVIDUAL FEATURES ARE NO LONGER SCALED
	// scale facemaker face piece offsets
	for (i = 0 ; i < FM_MAX_FEATURES ; i++) {
		faceMakerView->faceInfo.xOffsets[i] /= 2;
		faceMakerView->faceInfo.yOffsets[i] /= 2;
	}
	*/
	
	if (faceMakerView->faceInfo.featureViews[0] != 0 && pApp->pResourceManager->ResourceIsAvailable(RES_TSPRITE, 1001001)) {
		faceMakerView->Show();
		shadow->SetAlpha(0);
	}
	else {
		shadow->SetAlpha(255);
		faceMakerView->Hide();
	}

	if (pApp->pResourceManager->ResourceIsAvailable(RES_TSPRITE, roomClient->GetPlayerView(p->info.shadow))) {
		shadow->SetView(roomClient->GetPlayerView(p->info.shadow)); // use full art, if it's available
		shadow->SetCel(roomClient->GetPlayerViewCel(p->info.shadow, FALSE));
	}
	else {
		shadow->SetView(200039); // revert to guaranteed art
		shadow->SetCel(0);
	}
	shadow->SetHalfSize(true, ST_BILINEAR);
	shadow->Show();
	
	icon->SetCel(p->GetIconCel());
	icon->Show();
	
	ageLabel->Show();
	ExtractProfileField("age", p->profile, s);
	age->SetText(s);
	age->Show();
	
	sexLabel->Show();
	ExtractProfileField("sex", p->profile, s);
	sex->SetText(s);
	sex->Show();
	
	cityLabel->Show();
	ExtractProfileField("city", p->profile, s);
	city->SetText(s);
	city->Show();
	
	ExtractProfileField("text", p->profile, s);
	
	if (playerID == roomClient->GetMyID()) {
		profile->SetText(s);
		profile->Show();
		
		age->Enable();
		sex->Enable();
		city->Enable();
		profile->Enable();
	}
	else {
		if (game->IsChatFiltered())
			roomClient->dirtyWordFilter->UpdateString(s, MAX_TABLE_DESCRIPTION - 1);
		profile->SetText(s);
		profile->Show();
		
		age->Disable();
		sex->Disable();
		city->Disable();
		profile->Disable();
	}
}

void ProfileDialogButton::DoClick(BOOL netMessage)
{
	char s[256], buffer[1024];
	MTS_ROOM_CHANGE_PROFILE_MESSAGE *mts = (MTS_ROOM_CHANGE_PROFILE_MESSAGE *)buffer;
	
	SetState(DEFAULT_BUTTON_NORMAL);
	
	if (roomClient->status < RCS_IN_ROOM)
		return;
	
	if (id == 0 && parent->playerID == roomClient->GetMyID()) { // OK
		mts->profile[0] = 0;
		
		strnzcpy(s, parent->age->GetText(), 19);
		parent->StripSpecialQuote(s);
		sprintf(&mts->profile[strlen(mts->profile)], "age:`%s`", s);
		
		strnzcpy(s, parent->sex->GetText(), 19);
		parent->StripSpecialQuote(s);
		sprintf(&mts->profile[strlen(mts->profile)], "sex:`%s`", s);
		
		strnzcpy(s, parent->city->GetText(), 127);
		parent->StripSpecialQuote(s);
		sprintf(&mts->profile[strlen(mts->profile)], "city:`%s`", s);
		
		strnzcpy(s, parent->profile->GetText(), 255);
		parent->StripSpecialQuote(s);
		sprintf(&mts->profile[strlen(mts->profile)], "text:`%s`", s);
		
		if (strcmp(mts->profile, roomClient->players[0].profile)) {
			// send changed profile
			mts->messageType = MTS_ROOM_CHANGE_PROFILE;
			mts->playerID = parent->playerID;
			mts->profileLength = strlen(mts->profile) + 1;
			
			roomClient->SendMessageToServer((char *)mts, sizeof(MTS_ROOM_CHANGE_PROFILE_MESSAGE) + mts->profileLength);
		}
	}
	
	parent->Hide();
}

////////////////////////////////////////////////////////////////////////////////////////////////

ComplaintDialogBox::ComplaintDialogBox(sciPlane *parentPlane)
	: sciDialogBox(FALSE, parentPlane)
{
	int i;
	
	playerID = -1;
	
	for (i = 0 ; i < 3 ; i++)
		buttons[i] = NULL;
	
	complaint = NULL;
}

ComplaintDialogBox::~ComplaintDialogBox()
{
	int i;
	
	for (i = 0 ; i < 3 ; i++)
		if (buttons[i]) { delete buttons[i]; buttons[i] = NULL; }
	
	if (complaint) { delete complaint; complaint = NULL; }
}

void ComplaintDialogBox::Init()
{
	i18nStr getS;
	int i;
	
	SetPic(200028);
	Center();
	SetTitleText(GetString(96, getS), 32, 1, 1);
	SetDraggableArea(25);
	
	for (i = 0 ; i < 3 ; i++) {
		buttons[i] = new ComplaintDialogButton(i, this);
		buttons[i]->plane = this;
		buttons[i]->SetPri(1);
		buttons[i]->SetFont(4, -1, 7);
		buttons[i]->SetView(1003);
		buttons[i]->Init();
		buttons[i]->Enable();
	}
	
	buttons[0]->SetText(GetString(87, getS));
	buttons[1]->SetText(GetString(88, getS));
	buttons[2]->SetText(GetString(91, getS));
	buttons[0]->Posn(20, 154);
	buttons[1]->Posn(98, 154);
	buttons[2]->Posn(250, 154);
	// buttons[0]->SetHotkey(13);
	buttons[1]->SetHotkey(27);
	buttons[0]->SetButtonCels(4, 5, 6, 7);
	
	complaint = new EditBox();
	complaint->plane = this;
	complaint->ParamInit(4, "", 297, 91, 20, 45, 1, JUST_TOP | JUST_LEFT);
	complaint->Set3DOutLine(TRUE);
	complaint->WantReturn(TRUE);
}

void ComplaintDialogBox::Show(Boolean thePlayerID, Boolean newInGame)
{
	char s[256];
	i18nStr getS;
	int i;
	ROOMCLIENTPLAYER *p;
	
	playerID = thePlayerID;
	inGame = newInGame;
	
	p = roomClient->GetPlayerFromID(playerID);
	
	if (p == NULL && !(playerID == -1 || playerID == -2))
		{ assert(FALSE); return; }
	
	if (playerID == -1) {
		complaint->SetText(GetString(99, getS));
		sprintf(s, GetString(100, getS));
	}
	else if (playerID == -2) { //ratings feedback
		complaint->SetText(GetString(368, getS));
		GetString(369, s);
	}
	else {
		complaint->SetText(GetString(97, getS));
		sprintf(s, GetString(98, getS), p->info.login);
	}
		
	SetTitleText(s, 32, 1, 1);
	
	sciDialogBox::Show();
	
	buttons[0]->Disable();
	
	for (i = 0 ; i < 3 ; i++)
		buttons[i]->Show();
	
	complaint->Show();
	complaint->Disable();
}

void ComplaintDialogButton::DoClick(BOOL netMessage)
{
	char buffer[1024];
	ROOMCLIENTPLAYER *p;
	MTS_ROOM_COMPLAIN_MESSAGE *mts = (MTS_ROOM_COMPLAIN_MESSAGE *)buffer;
	
	SetState(DEFAULT_BUTTON_NORMAL);
	
	if (roomClient->status < RCS_IN_ROOM)
		return;
	
	if (id == 2) { // Sure
		Hide();
		parent->buttons[0]->Enable(); // OK
		parent->complaint->SetText("");
		parent->complaint->Enable();
		parent->complaint->SetFocus();
		
		return;
	}
	
	if (id == 0) { // OK
		mts->message[0] = 0;
		mts->login[0] = 0;
		mts->inGame = parent->inGame;
		strnzcpy(mts->message, parent->complaint->GetText(), MAX_COMPLAINT_LENGTH - 1);
		mts->messageType = MTS_ROOM_COMPLAIN;
		if (!(parent->playerID == -1 || parent->playerID == -2)) {
			p = roomClient->GetPlayerFromID(parent->playerID);
			if (p)
				strnzcpy(mts->login, p->info.login, MAX_PLAYER_NAME - 1);
		}

		if(parent->playerID == -2)
			mts->complaintType = RATINGS_COMPLAINT;
		else
			mts->complaintType = 0;

		mts->messageLength = strlen(mts->message) + 1;
		
		roomClient->SendMessageToServer((char *)mts, sizeof(MTS_ROOM_COMPLAIN_MESSAGE) + mts->messageLength);
	}
	
	parent->Hide();
}

///////////////////////////////////////////////////////////////

ClientDialogBox::ClientDialogBox(sciPlane *parentPlane)
	: sciDialogBox(FALSE, parentPlane)
{
	bodyText = NULL;
	buttons[0] = NULL;
	buttons[1] = NULL;
	
	purpose = DBP_UNKNOWN;
}

ClientDialogBox::~ClientDialogBox()
{
	if (bodyText) { delete bodyText; bodyText = NULL; }
	if (buttons[0]) { delete buttons[0]; buttons[0] = NULL; }
	if (buttons[1]) { delete buttons[1]; buttons[1] = NULL; }
}

void ClientDialogBox::Show(int viewNumber, char *text, char *caption, unsigned int uType, int thePurpose)
{
	i18nStr getS;
	int i, x1, x2;
	
	purpose = thePurpose;
	
	if (viewNumber == 0)
		viewNumber = 1085;
	
	SetPic(viewNumber);
	Center();
	sciDialogBox::Show();
	
	for (i = 0 ; i < 2 ; i++)
		if (!buttons[i]) {
			buttons[i] = new ClientDialogButton();
			buttons[i]->plane = this;
			buttons[i]->parent = this;
			buttons[i]->SetFont(4, -1, 7);
			buttons[i]->SetView(1003);
			if (i == 0)
				buttons[i]->SetButtonCels(4, 5, 6, 7); // default
			buttons[i]->Init();
			buttons[i]->SetPri(999);
			buttons[i]->Enable();
		}
	
	buttons[0]->SetText(GetString(87, getS));
	// buttons[0]->SetHotkey(13);
	//buttons[0]->SetHotkey2(' ');
	buttons[0]->id = IDOK;
	
	if (uType == MB_OK) {
		x1 = (GetWidth() - buttons[0]->GetWidth()) / 2;
		x2 = 0;
		
		buttons[0]->Show();
		buttons[1]->Hide();
	}
	else {
		if (uType == MB_YESNO) {
			buttons[0]->SetText(GetString(89, getS));
			// buttons[0]->SetHotkey(' ');
			// buttons[0]->SetHotkey(13);
			buttons[0]->id = IDYES;
			
			buttons[1]->SetText(GetString(90, getS));
			// buttons[1]->SetHotkey(27);
			// buttons[1]->SetHotkey2('n');
			buttons[1]->id = IDNO;
		}
		
		if (uType == MB_OKCANCEL) {
			buttons[1]->SetText(GetString(88, getS));
			// buttons[1]->SetHotkey(27);
			// buttons[1]->SetHotkey2('c');
			buttons[1]->id = IDCANCEL;
			buttons[1]->Show();
		}
		
		x1 = (GetWidth() - (buttons[0]->GetWidth() * 2) - 10) / 2;
		x2 = (GetWidth() / 2) + 5;
		
		buttons[0]->Show();
		buttons[1]->Show();
	}
	
	buttons[0]->Posn(x1 - 4, (GetHeight() - buttons[0]->GetHeight()) - 35);
	buttons[1]->Posn(x2 - 4, (GetHeight() - buttons[1]->GetHeight()) - 35);
	
	SetTitleText(caption, 32);
	
	if (!bodyText) {
		bodyText = new sciText();
		bodyText->plane = this;
	}
	bodyText->SetJust(JUST_VERTICAL | JUST_HORIZONTAL);
	bodyText->Posn(20, 36);
	bodyText->SetWidth(GetWidth() - 60);
	bodyText->SetHeight(GetHeight() - 105);
	bodyText->SetFont(4);
	bodyText->SetPri(998);
	bodyText->Init();
	bodyText->Show();
	bodyText->SetText(text);
}

void ClientDialogButton::DoClick(BOOL netMessage)
{
	SetState(DEFAULT_BUTTON_NORMAL);
	plane->Hide();
	
	parent->DoClick(id);
}

void RoomDialogBox::DoClick(int buttonID)
{
	i18nStr getS;
	
	switch (purpose) {
		case DBP_UNKNOWN: break;
		case DBP_LEAVE_GAME:
			if (buttonID == IDYES)
				roomClient->DoChatCommand(NULL, "stand");
			break;
		case DBP_EXIT:
			if (buttonID == IDYES)
				game->SetNextRoom(ROOM_END);
			break;
		default:
			assert(FALSE);
			break;
	}
	
	if (roomClient->status < RCS_IN_ROOM && buttonID != IDNO) {
		game->SetNextRoom(ROOM_END);
		// for now, we'll do this by actually checking the dialog text, since there is no
		// other flag that we've been banned
		if (strstr(bodyText->GetText(), GetString(101, getS)))
			if (won)
				won->Kill();
	}
}

void GameDialogBox::DoClick(int buttonID)
{
	i18nStr getS;
	MTS_GAME_RESTART_MESSAGE mtsGRM;
	
	if (roomClient->status < RCS_IN_ROOM)
		return;
	
	switch (purpose) {
		case DBP_UNKNOWN: break;
		case DBP_LEAVE_GAME:
			if (buttonID == IDYES)
				roomClient->DoChatCommand(NULL, "stand");
			break;
		case DBP_PLAYER_LEFT_GAME:
			//If the button was YES, then we are to save and keep going.
			if(buttonID == IDYES)
				owner->SaveGameIfNeeded();

			if(buttonID == IDYES || buttonID == IDNO)
			{
				char windowName[128], s[128];
				GetWindowText(owner->plane->GetWindow()->hWnd, windowName, 64);
				sprintf(s, GetString(102, getS), windowName);
				
				owner->ShowModelessMessage(0, s, GetString(103, getS), MB_YESNO, DBP_LEAVE_GAME);

//				owner->readyForMessages = TRUE;
				owner->answeredMostRecentRequest = TRUE;
			}

			//If it was OK, then the message box was to simply save and exit.
			if(buttonID == IDOK)
				roomClient->DoChatCommand(NULL, "stand");
			break;
		case DBP_RESTART_GAME:
			mtsGRM.messageType = MTS_GAME_RESTART;
			mtsGRM.table = owner->index;
			mtsGRM.playerID = roomClient->GetMyID();
			mtsGRM.seat = owner->GetMyPlayingIndex(TRUE);
			mtsGRM.restartGame = (buttonID == IDYES);
			
			roomClient->SendMessageToServer((char *)&mtsGRM, sizeof(MTS_GAME_RESTART_MESSAGE));
			break;
		default:
			assert(FALSE);
			break;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////

RSVPPlane::RSVPPlane()
	: sciPlane(FALSE, NULL, 240, 100)
{
	int i;
	
	version = RSVP_REQUEST;
	
	text = NULL;
	
	for (i = 0 ; i < 2 ; i++)
		buttons[i] = NULL;
}

RSVPPlane::~RSVPPlane()
{
	int i;
	
	if (text) { delete text; text = NULL; }
	
	for (i = 0 ; i < 2 ; i++)
		if (buttons[i]) { delete buttons[i]; buttons[i] = NULL; }
}

void RSVPPlane::Init()
{
	i18nStr getS;
	int i;
	
	SetPic(200035);
	window->SetTitle(GetString(104, getS));
	SetWindowPos(window->hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	
	for (i = 0 ; i < 2 ; i++) {
		buttons[i] = new RSVPButton(i, this);
		buttons[i]->plane = this;
		buttons[i]->SetPri(1);
		buttons[i]->SetFont(4, -1, 7);
		buttons[i]->SetView(1003);
		buttons[i]->Init();
		buttons[i]->Enable();
	}
	
	buttons[0]->SetText(GetString(89, getS));
	buttons[1]->SetText(GetString(90, getS));
	buttons[0]->Posn(52, 68);
	buttons[1]->Posn(140, 68);
	
	text = new sciText();
	text->plane = this;
	text->ParamInit(4, GetString(105, getS), 225, 50, 10, 10, 1, JUST_VERTICAL | JUST_HORIZONTAL);
}

void RSVPPlane::Show(int theVersion)
{
	char s[512];
	i18nStr getS;
	ROOMCLIENTPLAYER *p;
	
	version = theVersion;
	
	switch (version) {
		case RSVP_REQUEST:
			if (roomClient->requestedToWatch)
				window->SetTitle(GetString(106, getS));
			else
				window->SetTitle(GetString(107, getS));
			buttons[0]->SetText(GetString(108, getS));
			buttons[1]->SetText(GetString(109, getS));
			p = roomClient->GetPlayerFromID(roomClient->requesterPlayerID);
			if (p == NULL)
				{ assert(FALSE); return; }
			if (roomClient->requestedToWatch)
				sprintf(s, GetString(110, getS), p->info.login);
			else
				sprintf(s, GetString(111, getS), p->info.login);
			text->SetText(s);
			break;
		case RSVP_INVITE:
			if (roomClient->invitedToWatch)
				window->SetTitle(GetString(112, getS));
			else
				window->SetTitle(GetString(113, getS));
			buttons[0]->SetText(GetString(114, getS));
			buttons[1]->SetText(GetString(115, getS));
			p = roomClient->GetPlayerFromID(roomClient->inviterPlayerID);
			if (p == NULL) { // inviter is no longer around ... that's OK
				if (roomClient->invitedToWatch)
					sprintf(s, GetString(116, getS), roomClient->inviterTable + 1);
				else
					sprintf(s, GetString(117, getS), roomClient->inviterTable + 1);
			}
			else {
				if (roomClient->invitedToWatch)
					sprintf(s, GetString(118, getS), p->info.login, roomClient->inviterTable + 1);
				else
					sprintf(s, GetString(119, getS), p->info.login, roomClient->inviterTable + 1);
			}
			text->SetText(s);
			break;
		default:
			assert(FALSE);
			
			return;
	}
	
	buttons[0]->Show();
	buttons[1]->Show();
	text->Show();
	
	sciPlane::Show();
	
	SetForegroundWindow(window->hWnd);
}

void RSVPButton::DoClick(BOOL netMessage)
{
	MTS_ROOM_ANSWER_REQUEST_MESSAGE mts;
	MTS_ROOM_RSVP_MESSAGE mtsRSVP;
	BOOL toSetForgroundWindow = FALSE;
	
	SetState(DEFAULT_BUTTON_NORMAL);
	
	if (roomClient->status < RCS_IN_ROOM)
		return;
	
	switch (parent->version) {
		case RSVP_REQUEST:
			mts.messageType = MTS_ROOM_ANSWER_REQUEST;
			mts.playerID = roomClient->requesterPlayerID;
			mts.table = roomClient->players[0].info.table;
			mts.approved = (id == 0);
			
			roomClient->SendMessageToServer((char *)&mts, sizeof(MTS_ROOM_ANSWER_REQUEST_MESSAGE));
			
			roomClient->requesterPlayerID = -1;
			roomClient->requestedToWatch = FALSE;
			if(!(roomClient->tables[mts.table].status == TS_DISPLAYED))
				toSetForgroundWindow = TRUE;
			else
				toSetForgroundWindow = FALSE;
			break;
		case RSVP_INVITE:
			mtsRSVP.messageType = MTS_ROOM_RSVP;
			mtsRSVP.accepted = (id == 0);
			
			roomClient->SendMessageToServer((char *)&mtsRSVP, sizeof(MTS_ROOM_RSVP_MESSAGE));
			
			roomClient->inviterTable = -1;
			roomClient->inviterPlayerID = -1;
			roomClient->invitedToWatch = FALSE;
			//The only way I can be invited is if I am not in a game, so we won't be switching windows.
			toSetForgroundWindow = TRUE;
			break;
		default:
			assert(FALSE);
			
			return;
	}
	
	parent->Hide();
	if(toSetForgroundWindow)
		SetForegroundWindow(MAIN_HWND);
}

////////////////////////////////////////////////////////////////////////////////////////////////

InstructionsDialogBox::InstructionsDialogBox(sciPlane *parentPlane)
		: ClientDialogBox(parentPlane)
{
	showOnStartup = NULL;
}

InstructionsDialogBox::~InstructionsDialogBox()
{
	if (showOnStartup) { delete showOnStartup; showOnStartup = NULL; }
}

void InstructionsDialogBox::Show(Boolean startup)
{
	Boolean showOnStartupValue = TRUE;
	i18nStr getS;
	
#ifdef NO_HW_ROOM_INTERFACE
	return;
#endif
	
	showOnStartupValue = roomClient->globalIniFile->ReadValue("ShowStartUpInfo", TRUE);
	
	if (startup && !showOnStartupValue)
		return;
	
	ClientDialogBox::Show(200028, GetString(120, getS), "", MB_OK, DBP_GETTING_STARTED);
	SetTitleText(GetString(121, getS), 32, 1, 1);
	
	if (showOnStartup == NULL) {
		showOnStartup = new sciCheckBox();
		showOnStartup->plane = this;
		showOnStartup->SetText(GetString(122, getS));
		showOnStartup->SetFont(4, -1, 7);
		showOnStartup->SetView(1001);
		showOnStartup->SetTextOffset(20, 2);
		showOnStartup->Init();
		showOnStartup->Posn(130, 158);
		showOnStartup->SetPri(10);
	}
	
	if (showOnStartupValue)
   		showOnStartup->Select();
	else
		showOnStartup->Unselect();
	
	showOnStartup->Show();
	
	buttons[0]->Posn(20, buttons[0]->GetY() - 4);
}

void InstructionsDialogBox::DoClick(int buttonID)
{
	Hide();
	
	roomClient->globalIniFile->WriteValue("ShowStartUpInfo", showOnStartup->IsSelected());
}

////////////////////////////////////////////////////////////////////////////////////////////////

int backdrops[NUM_BACKDROPS] = {
		0, // "Standard"
		4033, // "Blue Gravel"
		4012, // "Blue Marbled Paper"
		4047, // "Brown Gravel"
		4006, // "Cityscape"
		4002, // "Cubist"
		4004, // "Green Felt"
		4010, // "Green Marbled Paper"
		4008, // "Knots"
		4007, // "Marble H"
		4045, // "Mars"
		4013, // "Metal"
		4014, // "Music and Violin"
		4001, // "Nouveau"
		4009, // "Pool of Leaves"
		4015, // "Red Felt"
		4011, // "Red Marbled Paper"
		4016, // "Rust"
		4003, // "Sandy Beach"
		4053, // "Sierra Logo"
		4038, // "Sky"
		4054, // "Sojourner"
		4019, // "Squares"
		4046, // "Steel Plate"
		4020, // "Thatch"
		4043, // "Weird Rocks"
		4005, // "Wild Horses"
		4021 // "Zen"
	};

BackdropDialogBox::BackdropDialogBox(sciPlane *parentPlane, ROOMCLIENTTABLE *theOwner)
		: sciDialogBox(FALSE, parentPlane)
{
	int i;
	
	owner = theOwner;
	
	for (i = 0 ; i < 2 ; i++)
		buttons[i] = NULL;
	
	choicesGroup = NULL;
	for (i = 0 ; i < NUM_BACKDROPS ; i++)
		choices[i] = NULL;
}

BackdropDialogBox::~BackdropDialogBox()
{
	int i;
	
	for (i = 0 ; i < 2 ; i++)
		if (buttons[i]) { delete buttons[i]; buttons[i] = NULL; }
	
	if (choicesGroup) { delete choicesGroup; choicesGroup = NULL; }
	for (i = 0 ; i < NUM_BACKDROPS ; i++)
		if (choices[i]) { delete choices[i]; choices[i] = NULL; }
}

void BackdropDialogBox::Init()
{
	i18nStr getS;
	int i;
	
	SetPic(200037);
	Center();
	
	SetTitleText(GetString(242, getS), 32, 1, 1); // Backdrop
	
	for (i = 0 ; i < 2 ; i++) {
		buttons[i] = new BackdropDialogButton(i, this);
		buttons[i]->plane = this;
		buttons[i]->Init();
		buttons[i]->SetFont(4, -1, 7);
		buttons[i]->SetView(1003);
		buttons[i]->SetPri(10);
	}
	
	buttons[0]->SetButtonCels(4, 5, 6, 7);
	buttons[1]->SetHotkey(27);
	
	buttons[0]->SetText(GetString(87, getS)); // "OK"
	buttons[1]->SetText(GetString(88, getS)); // "Cancel"
	
	buttons[0]->Posn(20, 310);
	buttons[1]->Posn(98, 310);
	
	choicesGroup = new sciButtonGroup();
	choicesGroup->plane = this;
	choicesGroup->Init();
	choicesGroup->SetPri(10);
	
	for (i = 0 ; i < NUM_BACKDROPS ; i++) {
		choices[i] = new sciRadioButton(i);
		choicesGroup->AddToGroup(choices[i]);
		choices[i]->plane = this;
		choices[i]->SetFont(4, -1, 7);
		choices[i]->SetView(1063);
		choices[i]->SetText(GetString((i == 0) ? 243 : (245 + i), getS));
		choices[i]->SetTextOffset(20);
		choices[i]->Init(0, 0);
		choices[i]->Posn(30 + (i / 10) * 200, 50 + (i % 10) * 20);
		choices[i]->SetPri(10);
		choices[i]->Enable();
	}
}

void BackdropDialogBox::Show()
{
	bool available[NUM_BACKDROPS];
	char path[MAX_PATH];
	int i, b;
	FILE *f;
	
	sciDialogBox::Show();
	
	// see if any of the optional backdrops are available either in the WON resources or
	// on an inserted CD
	
	available[0] = true;
	
	for (i = 1 ; i < NUM_BACKDROPS ; i++) {
		available[i] = false;
		if (pApp->pResourceManager->ResourceIsAvailable(RES_TSPRITE, backdrops[i])) {
			available[i] = true;
			continue;
		}
	}
	
	// if we can find one of the needed CD PRFs, we know we have access to all the backdrops
	// NOTE: this starts scanning on d:, and progresses through z:
	
	strcpy(path, "c:\\common2o.prf"); // the '98 and '99 Card CDs
	for (f = NULL ; f == NULL && path[0] < 'z' ; ) {
		path[0]++;
		f = fopen(path, "rb");
	}
	
	if (!f) {
		strcpy(path, "c:\\common.prf"); // the '97 Card CD
		for (f = NULL ; f == NULL && path[0] < 'z' ; ) {
			path[0]++;
			f = fopen(path, "rb");
		}
	}
	
	if (f) {
		fclose(f);
		for (i = 1 ; i < NUM_BACKDROPS ; i++)
			available[i] = true;
	}
	
	// show the buttons
	
	for (i = 0 ; i < 2 ; i++)
		buttons[i]->Show();
	
	for (i = 0 ; i < NUM_BACKDROPS ; i++) {
		choices[i]->Show();
		choices[i]->Unselect();
		if (available[i])
			choices[i]->Enable();
		else
			choices[i]->Disable();
	}
	
	b = roomClient->globalIniFile->ReadValue("Backdrop", 0);
	if (b < 0 || b >= NUM_BACKDROPS)
		b = 0;
	if (available[b])
		choices[b]->Select();
	else
		choices[0]->Select();
}

void BackdropDialogButton::DoClick(BOOL netMessage)
{
	int b, length;
	char path[MAX_PATH];
	FILE *f;
	PRF *prf;
	UniHandle *handle;
	ROOMCLIENTTABLE *table;
	
	plane->Hide();
	
	if (id != 0) // OK
		return;
	
	b = ((BackdropDialogBox *)parent)->choicesGroup->GetSelectedButtonID();
	roomClient->globalIniFile->WriteValue("Backdrop", b);
	
	table = ((BackdropDialogBox *)parent)->owner;
	
	if (b != 0) {
		if (pApp->pResourceManager->ResourceIsAvailable(RES_TSPRITE, backdrops[b]))
			; // the backdrop is already on the HD
		else {
			// copy the backdrop from the CD
			// NOTE: this starts scanning on d:, and progresses through z:

			strcpy(path, "c:\\common2o.prf"); // the '98 and '99 Card CDs
			for (f = NULL ; f == NULL && path[0] < 'z' ; ) {
				path[0]++;
				f = fopen(path, "rb");
			}
			
			if (!f) {
				strcpy(path, "c:\\common.prf"); // the '97 Card CD
				for (f = NULL ; f == NULL && path[0] < 'z' ; ) {
					path[0]++;
					f = fopen(path, "rb");
				}
			}
			
			if (f) {
				fclose(f);
				
				prf = new PRF();
				prf->Open(path, "rb");;
				prf->Load(MAKEC4ID('T', 'E', 'X', ' '), backdrops[b], &handle, &length);
				if (handle != NULL) {
					// write the file to the art directory
					sprintf(path, "%s\\art\\%d.tex", wonParams.wonPath, backdrops[b]);
					f = fopen(path, "wb");
					if (f) {
						fwrite(handle->Lock(), length, 1, f);
						handle->Unlock();
						fclose(f);
					}
					delete handle;
				}
			}
		}
	}
	
	if (b == 0 || !pApp->pResourceManager->ResourceIsAvailable(RES_TSPRITE, backdrops[b])) {
		table->plane->SetPic(GAME_BACKGROUND_PIC);
		if (table->display->teamPlaque)
			table->display->teamPlaque->Hide();
	}
	else {
		table->plane->SetPic(backdrops[b]);
		if (table->display->teamPlaque)
			table->display->teamPlaque->Show();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////

/*
	0, // "Standard"
	1, // "Hoyle Red"
	2, // "Mice"
	3, // "Carmen's Hat"
	4, // "Cubist Cafe"
	5, // "Airplanes"
	6, // "Moo"
	7, // "Flowers"
	8, // "Blue Liquid Goo"
	9, // "Smokey"
	10, // "Ancient Pattern"
	11, // "Stained Glass"
	12, // "SchprocketWerks"
	13, // "Train"
	14, // "Automobiles"
	15, // "Squares"
	16, // "Checkers"
	17, // "Camouflage"
*/

CardBacksDialogBox::CardBacksDialogBox(sciPlane *parentPlane, ROOMCLIENTTABLE *theOwner)
		: sciDialogBox(FALSE, parentPlane)
{
	int i;
	
	owner = theOwner;
	
	for (i = 0 ; i < 2 ; i++)
		buttons[i] = NULL;
	
	cardBacksLabel = NULL;
	choicesGroup = NULL;
	for (i = 0 ; i < NUM_CARDBACKS ; i++)
		choices[i] = NULL;
	
	cardFaceLabel = NULL;
	cardFaceGroup = NULL;
	for (i = 0 ; i < 2 ; i++)
		cardFaces[i] = NULL;
}

CardBacksDialogBox::~CardBacksDialogBox()
{
	int i;
	
	for (i = 0 ; i < 2 ; i++)
		if (buttons[i]) { delete buttons[i]; buttons[i] = NULL; }
	
	if (cardBacksLabel) { delete cardBacksLabel; cardBacksLabel = NULL; }
	if (choicesGroup) { delete choicesGroup; choicesGroup = NULL; }
	for (i = 0 ; i < NUM_CARDBACKS ; i++)
		if (choices[i]) { delete choices[i]; choices[i] = NULL; }
	
	if (cardFaceLabel) { delete cardFaceLabel; cardFaceLabel = NULL; }
	if (cardFaceGroup) { delete cardFaceGroup; cardFaceGroup = NULL; }
	for (i = 0 ; i < 2 ; i++)
		if (cardFaces[i]) { delete cardFaces[i]; cardFaces[i] = NULL; }
}

void CardBacksDialogBox::Init()
{
	i18nStr getS;
	int i;
	
	SetPic(200037);
	Center();
	
	SetTitleText(GetString(245, getS), 32, 1, 1); // Card Backs
	
	for (i = 0 ; i < 2 ; i++) {
		buttons[i] = new CardBacksDialogButton(i, this);
		buttons[i]->plane = this;
		buttons[i]->Init();
		buttons[i]->SetFont(4, -1, 7);
		buttons[i]->SetView(1003);
		buttons[i]->SetPri(10);
	}
	
	buttons[0]->SetButtonCels(4, 5, 6, 7);
	buttons[1]->SetHotkey(27);
	
	buttons[0]->SetText(GetString(87, getS)); // "OK"
	buttons[1]->SetText(GetString(88, getS)); // "Cancel"
	
	buttons[0]->Posn(20, 310);
	buttons[1]->Posn(98, 310);
	
	cardBacksLabel = new sciText();
	cardBacksLabel->plane = this;
	cardBacksLabel->ParamInit(2, GetString(273, getS), 175, 20, 30, 50, 10, JUST_VERTICAL | JUST_LEFT);
	
	choicesGroup = new sciButtonGroup();
	choicesGroup->plane = this;
	choicesGroup->Init();
	choicesGroup->SetPri(10);
	
	for (i = 0 ; i < NUM_CARDBACKS ; i++) {
		choices[i] = new sciRadioButton(i);
		choicesGroup->AddToGroup(choices[i]);
		choices[i]->plane = this;
		choices[i]->SetFont(4, -1, 7);
		choices[i]->SetView(1063);
		choices[i]->SetText(GetString((i == 0) ? 243 : (273 + i), getS));
		choices[i]->SetTextOffset(20);
		choices[i]->Init(0, 0);
		choices[i]->Posn(30 + (i / 9) * 150, 80 + (i % 9) * 20);
		choices[i]->SetPri(10);
		choices[i]->Enable();
	}
	
	cardFaceLabel = new sciText();
	cardFaceLabel->plane = this;
	cardFaceLabel->ParamInit(2, GetString(291, getS), 175, 20, 400, 50, 10, JUST_VERTICAL | JUST_LEFT);
	
	cardFaceGroup = new sciButtonGroup();
	cardFaceGroup->plane = this;
	cardFaceGroup->Init();
	cardFaceGroup->SetPri(10);
	
	for (i = 0 ; i < 2 ; i++) {
		cardFaces[i] = new sciRadioButton(i);
		cardFaceGroup->AddToGroup(cardFaces[i]);
		cardFaces[i]->plane = this;
		cardFaces[i]->SetFont(4, -1, 7);
		cardFaces[i]->SetView(1063);
		cardFaces[i]->SetText(GetString((i == 0) ? 243 : 292, getS));
		cardFaces[i]->SetTextOffset(20);
		cardFaces[i]->Init(0, 0);
		cardFaces[i]->Posn(400, 80 + i * 20);
		cardFaces[i]->SetPri(10);
		cardFaces[i]->Enable();
	}
	
}

void CardBacksDialogBox::Show()
{
	bool artAvailable;
	int i, b;
	
	sciDialogBox::Show();
	
	for (i = 0 ; i < 2 ; i++)
		buttons[i]->Show();
	
	cardFaceLabel->Show();
	cardBacksLabel->Show();
	
	// show the card back options
	
	artAvailable = pApp->pResourceManager->ResourceIsAvailable(RES_TSPRITE, 200052) != 0;
	if (!IHaveAHoyleCDProductInstalled(CUSTOMER_STRING_1, CUSTOMER_STRING_2, CUSTOMER_STRING_3))
		artAvailable = FALSE;
	
	b = roomClient->globalIniFile->ReadValue("CardBacks", 0);
	if (b < 0 || b >= NUM_CARDBACKS)
		b = 0;
	if (b > 0 && !artAvailable)
		b = 0;

	owner->m_iCardBacks = b;
	
	for (i = 0 ; i < NUM_CARDBACKS ; i++) {
		choices[i]->Show();
		choices[i]->Unselect();
		if (i == 0 || artAvailable)
			choices[i]->Enable();
		else
			choices[i]->Disable();
	}
	
	choices[b]->Select();
	
	// show the card face options
	
	artAvailable = pApp->pResourceManager->ResourceIsAvailable(RES_TSPRITE, 200053) != 0;
	if (!IHaveAHoyleCDProductInstalled(CUSTOMER_STRING_1, CUSTOMER_STRING_2, CUSTOMER_STRING_3))
		artAvailable = FALSE;
	
	b = roomClient->globalIniFile->ReadValue("CardFaces", 0);
	if (b < 0 || b >= 2)
		b = 0;
	if (b > 0 && !artAvailable)
		b = 0;

	owner->m_iCardFaces = b;
	
	for (i = 0 ; i < 2 ; i++) {
		cardFaces[i]->Show();
		cardFaces[i]->Unselect();
		if (i == 0 || artAvailable)
			cardFaces[i]->Enable();
		else
			cardFaces[i]->Disable();
	}
	
	cardFaces[b]->Select();
}

void CardBacksDialogButton::DoClick(BOOL netMessage)
{
	int b;
	
	plane->Hide();
	
	if (id != 0) // OK
		return;
	
	b = ((CardBacksDialogBox *)parent)->choicesGroup->GetSelectedButtonID();
	roomClient->globalIniFile->WriteValue("CardBacks", b);
	parent->owner->m_iCardBacks = b;
	
	b = ((CardBacksDialogBox *)parent)->cardFaceGroup->GetSelectedButtonID();
	roomClient->globalIniFile->WriteValue("CardFaces", b);
	parent->owner->m_iCardFaces = b;
	
	((CardBacksDialogBox *)parent)->owner->Display();
}

////////////////////////////////////////////////////////////////////////////////////////////////

const char gacKeyPressed[NUM_CHATSOUNDS] = { '1','2','3','4','5','6','7','8','9','0','Q','W','E','R','T','Y','U','I','O','P'};
const int gaiChatSounds[NUM_CHATSOUNDS] = 
	{ 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220};

ChatSoundDialogBox::ChatSoundDialogBox(sciPlane *parentPlane)
		: sciDialogBox(FALSE, parentPlane)
{
	int i;
	
	for (i = 0 ; i < 3 ; i++)
		buttons[i] = NULL;
	
	choicesGroup = NULL;
	for (i = 0 ; i < NUM_CHATSOUNDS ; i++)
		choices[i] = NULL;
}

ChatSoundDialogBox::~ChatSoundDialogBox()
{
	int i;
	
	for (i = 0 ; i < 3 ; i++)
		if (buttons[i]) { delete buttons[i]; buttons[i] = NULL; }
	
	if (choicesGroup) { delete choicesGroup; choicesGroup = NULL; }
	for (i = 0 ; i < NUM_CHATSOUNDS ; i++)
		if (choices[i]) { delete choices[i]; choices[i] = NULL; }
}

void ChatSoundDialogBox::Init()
{
	i18nStr getS;
	int i;
	
	SetPic(200037);
	Center();
	
	SetTitleText(GetString(298, getS), 32, 1, 1); // Chat Sounds
	
	for (i = 0 ; i < 3 ; i++) {
		buttons[i] = new ChatSoundDialogButton(i, this);
		buttons[i]->plane = this;
		buttons[i]->Init();
		buttons[i]->SetFont(4, -1, 7);
		buttons[i]->SetView(1003);
		buttons[i]->SetPri(10);
	}
	
	buttons[0]->SetButtonCels(4, 5, 6, 7);
	buttons[1]->SetHotkey(27);
	
	buttons[0]->SetText(GetString(335, getS)); 
	buttons[1]->SetText(GetString(88, getS)); // "Cancel"
	buttons[2]->SetText(GetString(336, getS));
	
	buttons[0]->Posn(20, 310);
	buttons[1]->Posn(98, 310);
	buttons[2]->Posn(475, 310);
	
	choicesGroup = new sciButtonGroup();
	choicesGroup->plane = this;
	choicesGroup->Init();
	choicesGroup->SetPri(10);
	
	for (i = 0 ; i < NUM_CHATSOUNDS ; i++) {
		choices[i] = new sciRadioButton(i);
		choicesGroup->AddToGroup(choices[i]);
		choices[i]->plane = this;
		choices[i]->SetFont(4, -1, 7);
		choices[i]->SetView(1063);
		choices[i]->SetText(GetString((299 + i), getS));
		choices[i]->SetTextOffset(20);
		choices[i]->Init(0, 0);
		choices[i]->Posn(20 + (i / 12) * 180, 50 + (i % 12) * 20);
		choices[i]->SetPri(10);
		choices[i]->Enable();
	}
}

void ChatSoundDialogBox::Show()
{
	int i;
	
	sciDialogBox::Show();
	
	// show the buttons
	
	for (i = 0 ; i < 3 ; i++)
		buttons[i]->Show();
	
	for (i = 0 ; i < NUM_CHATSOUNDS ; i++) {
		choices[i]->Show();
		choices[i]->Unselect();
		choices[i]->Enable();
	}
	
	choices[0]->Select();
}

void ChatSoundDialogButton::DoClick(BOOL netMessage)
{
	int b;
		
	if (id == 1) // Cancel
	{
		plane->Hide();
		return;
	}
	
	b = ((ChatSoundDialogBox *)parent)->choicesGroup->GetSelectedButtonID();

	if(id == 0) // OK
	{
		roomClient->ChatSoundKeyPressed(gacKeyPressed[b]);
		plane->Hide();
	}

	if(id == 2) // Sample
	{
		game->sound1->Play(gaiChatSounds[b]);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////

void ShowHideTIControls(HWND hDlg)
{
	char s[256], getS[256];
	int i, j, selected, playerID;
	HWND tabControl, editor, checkButton, joinButton;
	ROOMCLIENTPLAYER *p;
	static int firsts[] = { IDC_TC_EDITOR, IDC_TR_EDITOR, IDC_TT_EDITOR, IDC_TA_EDITOR, IDC_TO_EDITOR },
			   lasts[] = { IDC_TC_CHECK_OUT_IN, IDC_TR_CHECK_OUT_IN, IDC_TT_CHECK_OUT_IN, IDC_TA_CHECK_OUT_IN, IDC_TO_CHECK_OUT_IN },
			   otherTDControls[] = { IDC_TC_ENABLE_JOIN, IDC_TC_ENABLE_SITTING, IDC_TC_STARTING_BANK, IDC_TC_BOX, IDC_TC_LABEL,
									 IDC_TT_PLAYERS_PER_TABLE, IDC_TT_GENERATE_ASSIGNMENTS, IDC_TT_BOX, IDC_TT_LABEL1, IDC_TT_LABEL2,
									 IDC_TA_CLEAR, IDC_TA_ENABLE, IDC_TA_BOX, 0 },
			   textBoxes[5] = { IDC_TC_DESCRIPTION, IDC_TR_RULES, IDC_TT_ASSIGNMENTS, IDC_TA_RESULTS, IDC_TO_RESULTS },
			   protectedEditBoxes[5] = { IDC_TC_DESCRIPTION, IDC_TR_RULES, IDC_TT_ASSIGNMENTS, 0, IDC_TO_RESULTS },
			   protectedControls[5][4] = {
					{ IDC_TC_ENABLE_JOIN, IDC_TC_ENABLE_SITTING, IDC_TC_STARTING_BANK, 0 },
					{ 0, 0, 0, 0 },
					{ IDC_TT_PLAYERS_PER_TABLE, IDC_TT_GENERATE_ASSIGNMENTS, 0, 0 },
					{ IDC_TA_ENABLE, IDC_TA_CLEAR, 0, 0 },
					{ 0, 0, 0, 0 } };
	
	tabControl = GetDlgItem(hDlg, IDC_TAB);
	selected = TabCtrl_GetCurSel(tabControl);
	
	for (i = 0 ; i <= 5 ; i++)
		for (j = firsts[i] ; j <= lasts[i] ; j++)
			ShowWindow(GetDlgItem(hDlg, j), (i == selected) ? SW_SHOW : SW_HIDE);
	
	// if we are not a TD or a host, hide the TD controls
	
	if (!(roomClient->players[0].IsMonitorOrReferee() || roomClient->players[0].IsSuperUser() || 
		roomClient->players[0].IsTournamentAdministrator())) {
		for (i = 0 ; i <= 5 ; i++) {
			ShowWindow(GetDlgItem(hDlg, firsts[i]), SW_HIDE);
			ShowWindow(GetDlgItem(hDlg, lasts[i]), SW_HIDE);
		}
		for (i = 0 ; otherTDControls[i] ; i++)
			ShowWindow(GetDlgItem(hDlg, otherTDControls[i]), SW_HIDE);
	}
	else // show checkout status and enable TD controls appropriately for each page
		for (i = 0 ; i < 5 ; i++) {
			for (j = 0 ; j < 4 ; j++)
				if (protectedControls[i][j])
					EnableWindow(GetDlgItem(hDlg, protectedControls[i][j]), FALSE);
			if (protectedEditBoxes[i])
				SendMessage(GetDlgItem(hDlg, protectedEditBoxes[i]), EM_SETREADONLY, TRUE, 0);
			playerID = roomClient->tournamentInformation.header.checkOutPlayerIDs[i];
			editor = GetDlgItem(hDlg, firsts[i]);
			checkButton = GetDlgItem(hDlg, lasts[i]);
			if (playerID == -1) {
				SetWindowText(editor, "");
				SetWindowText(checkButton, GetString(123, getS));
				EnableWindow(checkButton, TRUE);
			}
			else {
				p = roomClient->GetPlayerFromID(playerID);
				if (p == NULL)
					assert(FALSE);
				else {
					sprintf(s, GetString(125, getS), p->info.login);
					SetWindowText(editor, s);
					if (playerID == roomClient->GetMyID()) {
						for (j = 0 ; j < 5 ; j++)
							if (protectedControls[i][j])
								EnableWindow(GetDlgItem(hDlg, protectedControls[i][j]), TRUE);
						if (protectedEditBoxes[i])
							SendMessage(GetDlgItem(hDlg, protectedEditBoxes[i]), EM_SETREADONLY, FALSE, 0);
						SetWindowText(checkButton, GetString(124, getS));
						EnableWindow(checkButton, TRUE);
					}
					else {
						SetWindowText(checkButton, GetString(123, getS));
						EnableWindow(checkButton, FALSE);
					}
				}
			}
		}
	
	// show the join button appropriately
	
	joinButton = GetDlgItem(hDlg, IDC_TC_JOIN);
	
	if (roomClient->players[0].IsInTournament()) {
		SetWindowText(joinButton, GetString(126, getS));
		EnableWindow(joinButton, TRUE);
	}
	else {
		SetWindowText(joinButton, GetString(127, getS));
		EnableWindow(joinButton, roomClient->tournamentInformation.header.joinEnabled);
	}
	
	// set the variable values according the tournamentInformation structure
	// but never change a control that I have checked out
	
	if (roomClient->tournamentInformation.header.checkOutPlayerIDs[0] != roomClient->GetMyID()) {
		SendDlgItemMessage(hDlg, IDC_TC_ENABLE_JOIN, BM_SETCHECK,
						   roomClient->tournamentInformation.header.joinEnabled ? BST_CHECKED : BST_UNCHECKED, 0);
		SendDlgItemMessage(hDlg, IDC_TC_ENABLE_SITTING, BM_SETCHECK,
						   roomClient->tournamentInformation.header.sittingEnabled ? BST_CHECKED : BST_UNCHECKED, 0);
		SetDlgItemInt(hDlg, IDC_TC_STARTING_BANK, roomClient->tournamentInformation.header.startingBankValue, FALSE);
	}
	
	if (roomClient->tournamentInformation.header.checkOutPlayerIDs[3] != roomClient->GetMyID())
		SendDlgItemMessage(hDlg, IDC_TA_ENABLE, BM_SETCHECK,
						   roomClient->tournamentInformation.header.automaticResultsEnabled ? BST_CHECKED : BST_UNCHECKED, 0);
	
	// update the text of each page
	// but never change the text of a page that I have checked out
	// but automatic results are an exception, since I can never be editing them
	
	for (i = 0 ; i < 5 ; i++) {
		if (i != TI_PAGE_AUTOMATIC)
			if (roomClient->tournamentInformation.header.checkOutPlayerIDs[i] == roomClient->GetMyID())
				continue;
		
		SetDlgItemText(hDlg, textBoxes[i], roomClient->tournamentInformation.text[i]);
	}
}

void SendChangedTIText(HWND hDlg, int pageNumber)
{
	char *p, buffer[1024];
	int i, n;
	MTS_ROOM_TI_ADD_TO_TEXT_MESSAGE *mts = (MTS_ROOM_TI_ADD_TO_TEXT_MESSAGE *)buffer;
	static int textBoxes[5] = { IDC_TC_DESCRIPTION, IDC_TR_RULES, IDC_TT_ASSIGNMENTS, 0, IDC_TO_RESULTS };
	
	GetDlgItemText(hDlg, textBoxes[pageNumber], roomClient->tournamentInformation.text[pageNumber], MAX_TOURNAMENT_INFO - 1);
	
	mts->messageType = MTS_ROOM_TI_ADD_TO_TEXT;
	mts->playerID = roomClient->GetMyID();
	mts->pageNumber = pageNumber;
	
	p = roomClient->tournamentInformation.text[pageNumber];
	
	for (i = 0 ; p[i] ; ) {
		mts->clear = (i == 0);
		n = strlen(&p[i]);
		if (n > 255)
			n = 255;
		strnzcpy(mts->text, &p[i], n);
		i += n;
		
		roomClient->SendMessageToServer((char *)mts, sizeof(MTS_ROOM_TI_ADD_TO_TEXT_MESSAGE) + n + 1);
	}
}

BOOL CALLBACK TournamentInformationDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	i18nStr getS;
	int i;
	HWND tabControl;
    TC_ITEM tabItem;
	LPNMHDR notificationHeader;
	MTS_ROOM_TI_CHECK_OUT_PAGE_MESSAGE mtsTICOPM;
	MTS_ROOM_TI_JOIN_MESSAGE mtsTIJM;
	MTS_ROOM_TI_GENERATE_ASSIGNMENTS_MESSAGE mtsTIGAM;
	MTS_ROOM_TI_CLEAR_AUTO_RESULTS_MESSAGE mtsTICARM;
	static int checkButtons[] = { IDC_TC_CHECK_OUT_IN, IDC_TR_CHECK_OUT_IN, IDC_TT_CHECK_OUT_IN, IDC_TA_CHECK_OUT_IN, IDC_TO_CHECK_OUT_IN };
	
	tabControl = GetDlgItem(hDlg, IDC_TAB);
	
	switch(uMsg) {
		case WM_INITDIALOG:
			InitCommonControls();
			
			for (i = 0 ; i < 5 ; i++) {
				tabItem.mask = TCIF_TEXT;
				tabItem.iImage = -1;
				tabItem.pszText = GetString(128 + i, getS);
				TabCtrl_InsertItem(tabControl, i, &tabItem);
			}
			
			TabCtrl_SetCurSel(tabControl, 0);
			ShowHideTIControls(hDlg);
			
			return TRUE;
		
		case WM_CLOSE:
			ShowWindow(hDlg, SW_HIDE);
			return TRUE;
		
		case WM_NOTIFY:
			notificationHeader = (LPNMHDR)lParam;
			
			switch (notificationHeader->code) {
				case TCN_SELCHANGE:
					ShowHideTIControls(hDlg);
					break;
			}
			break;
		
		case WM_COMMAND:
			if (HIWORD(wParam) == BN_CLICKED) {
				for (i = 0 ; i < 5 ; i++)
					if (checkButtons[i] == LOWORD(wParam)) {
						mtsTICOPM.messageType = MTS_ROOM_TI_CHECK_OUT_PAGE;
						mtsTICOPM.pageNumber = i;
						// am I checking the page in or out?
						if (roomClient->tournamentInformation.header.checkOutPlayerIDs[i] == -1)
							mtsTICOPM.playerID = roomClient->GetMyID();
						else {
							// checking it in; set the appropriate values
							mtsTICOPM.playerID = -1;
							switch (mtsTICOPM.pageNumber) {
								case TI_PAGE_CURRENT:
									mtsTICOPM.header.joinEnabled = 
										SendDlgItemMessage(hDlg, IDC_TC_ENABLE_JOIN, BM_GETCHECK, 0, 0) == BST_CHECKED;
									mtsTICOPM.header.sittingEnabled =
										SendDlgItemMessage(hDlg, IDC_TC_ENABLE_SITTING, BM_GETCHECK, 0, 0) == BST_CHECKED;
									mtsTICOPM.header.startingBankValue =
										GetDlgItemInt(hDlg, IDC_TC_STARTING_BANK, NULL, FALSE);
									break;
								case TI_PAGE_RULES:
									break;
								case TI_PAGE_ASSIGNMENTS:
									break;
								case TI_PAGE_AUTOMATIC:
									mtsTICOPM.header.automaticResultsEnabled =
										SendDlgItemMessage(hDlg, IDC_TA_ENABLE, BM_GETCHECK, 0, 0) == BST_CHECKED;
									break;
								case TI_PAGE_OFFICIAL:
									break;
							}
							
							if (mtsTICOPM.pageNumber != TI_PAGE_AUTOMATIC)
								SendChangedTIText(hDlg, mtsTICOPM.pageNumber);
						}
						
						roomClient->SendMessageToServer((char *)&mtsTICOPM, sizeof(MTS_ROOM_TI_CHECK_OUT_PAGE_MESSAGE));
						
						break;
					}
				
				switch (LOWORD(wParam)) {
					case IDC_TC_JOIN:
						mtsTIJM.messageType = MTS_ROOM_TI_JOIN;
						mtsTIJM.playerID = roomClient->GetMyID();
						mtsTIJM.joining = !roomClient->players[0].IsInTournament();
						roomClient->SendMessageToServer((char *)&mtsTIJM, sizeof(MTS_ROOM_TI_JOIN_MESSAGE));
						break;
					
					case IDC_TC_OTHER:
						if (won)
							won->LaunchBrowser(GetString(134, getS));
						break;
					
					case IDC_TT_GENERATE_ASSIGNMENTS:
						mtsTIGAM.messageType = MTS_ROOM_TI_GENERATE_ASSIGNMENTS;
						mtsTIGAM.playersPerTable =
							GetDlgItemInt(hDlg, IDC_TT_PLAYERS_PER_TABLE, NULL, FALSE);
						roomClient->SendMessageToServer((char *)&mtsTIGAM, sizeof(MTS_ROOM_TI_GENERATE_ASSIGNMENTS_MESSAGE));
						break;
					case IDC_TA_CLEAR:
						mtsTICARM.messageType = MTS_ROOM_TI_CLEAR_AUTO_RESULTS;
						roomClient->SendMessageToServer((char *)&mtsTICARM, sizeof(MTS_ROOM_TI_CLEAR_AUTO_RESULTS_MESSAGE));
						break;
				}
				
				return TRUE;
			}
			
			break;
		
		default:
			return FALSE;
	}
	
	return FALSE;
}

TournamentDialogBox::TournamentDialogBox()
{
	dialog = CreateDialog(hResourceInstance, MAKEINTRESOURCE(IDD_TOURNAMENT_INFORMATION), NULL, TournamentInformationDlgProc);
	CenterHWND(dialog, 0, 0);
}

TournamentDialogBox::~TournamentDialogBox()
{
	DestroyWindow(dialog);
}

void TournamentDialogBox::Show()
{
	ShowWindow(dialog, SW_SHOWNORMAL);
	SetForegroundWindow(dialog);
}

void TournamentDialogBox::Hide()
{
	ShowWindow(dialog, SW_HIDE);
}

////////////////////////////////////////////////////////////////////////////////////////////////

PopupMenu::PopupMenu(sciPlane *thePlane)
{
	plane = thePlane;
	
	direction = POPDIR_ROOM;
	totalItems = 0;
	numItems = 0;
	playerID = -1;
	
	top = NULL;
	items = NULL;
	bottom = NULL;
}

PopupMenu::~PopupMenu()
{
	if (top) { delete top; top = NULL; }
	if (items) { delete[] items; items = NULL; }
	if (bottom) { delete bottom; bottom = NULL; }
}

void PopupMenu::Init(int theDirection)
{
	sciFeature::Init();
	
	direction = theDirection;
	viewNum = (direction == POPDIR_GAME) ? 200005 : 200006; // assume narrow
	playerID = -1;
	
	top = new sciView();
	top->plane = plane;
	top->SetPri(2000);
	top->SetView(viewNum);
	top->SetCel(0);
	top->Init();
	
	bottom = new sciView();
	bottom->plane = plane;
	bottom->SetPri(2000);
	bottom->SetView(viewNum);
	bottom->SetCel(2);
	bottom->Init();
}

int PopupMenu::GetWidth()
{
	return top->GetWidth();
}

int PopupMenu::GetHeight()
{
	int i, h;
	
	h = 0;
	
	h += top->GetHeight();
	for (i = 0 ; i < numItems ; i++)
		h += items[i].GetHeight();
	h += bottom->GetHeight();
	
	return h;
}

void PopupMenu::Posn(int x, int y)
{
	int i, dY;
	
	dY = 0;
	
	if (y < 0)
		y = 0;
	
	top->Posn(x, y);
	dY += top->GetHeight();
	for (i = 0 ; i < numItems ; i++) {
		items[i].Posn(x, y + dY);
		dY += items[i].GetHeight();
	}
	bottom->Posn(x, y + dY);
}

void PopupMenu::Show(sciFeature *theKeepShownFeature)
{
	int i;
	
	keepShownFeature = theKeepShownFeature;
	
	top->Show();
	for (i = 0 ; i < numItems ; i++)
		items[i].Show();
	bottom->Show();
}

void PopupMenu::Hide()
{
	int i;
	
	top->Hide();
	for (i = 0 ; i < numItems ; i++)
		items[i].Hide();
	bottom->Hide();
}

void PopupMenu::SetNumItems(int n)
{
	int i;
	
	if (items)
		delete[] items;
	
	totalItems = n;
	numItems = 0;
	
	playerID = -1;
	seat = -1;
	
	viewNum = (direction == POPDIR_GAME) ? 200005 : 200006; // assume narrow
	top->SetView(viewNum);
	top->SetCel(0);
	bottom->SetView(viewNum);
	bottom->SetCel(2);
	
	items = new PopupItem[totalItems];
	
	for (i = 0 ; i < totalItems ; i++)
		items[i].Init(this);
}

void PopupMenu::AddItem(int id, int state, char *text, int thePlayerID)
{
	int i;
	
	if (numItems >= totalItems)
		{ assert(FALSE); return; }
	
	items[numItems].SetID(id);
	items[numItems].SetState(state);
	items[numItems].SetText(text);
	items[numItems].playerID = thePlayerID;
	
	if (state == POPITEM_DELIMITER)
		items[numItems].SetCel(3);
	
	if (strcmp(items[numItems].text->GetText(), text))
		if (viewNum != 200023 && viewNum != 200024) {
			// not all of the text could fit; make the menu wide
			viewNum = (direction == POPDIR_GAME) ? 200023 : 200024;
			for (i = 0 ; i < totalItems ; i++) {
				items[i].SetView(viewNum);
				items[i].SetCel((items[i].state == POPITEM_DELIMITER) ? 3 : 1);
				items[i].text->SetWidth(items[i].GetWidth() - 20);
				items[i].highlight->SetView(200027);
			}
			top->SetView(viewNum);
			top->SetCel(0);
			bottom->SetView(viewNum);
			bottom->SetCel(2);
			items[numItems].SetText(text);
		}
	
	numItems++;
}

Boolean PopupMenu::HandleEvent(sciEvent &event)
{
	Boolean onAnItem = FALSE, onASelectedItem = FALSE, changed = FALSE;
	int i;
	POINT p;
	RECT r;
	
	if (roomClient->status < RCS_IN_ROOM)
		return FALSE;
	
	if (event.claimed)
		return FALSE;
	
	if (event.plane != plane)
		return FALSE;
	
	if (top->IsHidden())
		return FALSE;
	
	for (i = 0 ; i < numItems ; i++)
		if (items[i].sciFeature::OnMe(event)) {
			onAnItem = TRUE;
			if (items[i].state == POPITEM_SELECTED)
				onASelectedItem = TRUE;
			break;
		}
	
	// allow for a fudge factor of 50 pixels around each side of the menu
	
	p.x = event.x;
	p.y = event.y;
	SetRect(&r, top->GetX() - 50, top->GetY() - 50, top->GetX() + top->GetWidth() + 50, bottom->GetY() + bottom->GetHeight() + 50);
	
	if (i == numItems && !top->sciFeature::OnMe(event) && !bottom->sciFeature::OnMe(event))
		if (!PtInRect(&r, p))
			if (!keepShownFeature || !keepShownFeature->sciFeature::OnMe(event) || event.type == MOUSE_DOWN)
				{ Hide(); return FALSE; }
	
	event.claimed = TRUE;
	
	switch (event.type) {
		case MOUSE_MOVE:
			for (i = 0 ; i < numItems ; i++)
				if (items[i].state == POPITEM_SELECTED) {
					if (items[i].sciFeature::OnMe(event))
						return TRUE;
					
					items[i].SetState(POPITEM_SELECTABLE);
					changed = TRUE;
				}
			
			for (i = 0 ; i < numItems ; i++)
				if (items[i].state == POPITEM_SELECTABLE)
					if (items[i].sciFeature::OnMe(event)) {
						items[i].SetState(POPITEM_SELECTED);
						changed = TRUE;
						break;
					}
			
			if (changed)
				Show(keepShownFeature);
			break;
		
		case MOUSE_UP:
			if (!(event.modifiers & LEFT_MOUSE))
				break;
			if (!onASelectedItem) {
				if (onAnItem || top->sciFeature::OnMe(event) || bottom->sciFeature::OnMe(event))
					break;
				if (keepShownFeature && keepShownFeature->OnMe(event))
					break;
			}
			Hide();
			for (i = 0 ; i < numItems ; i++)
				if (items[i].state == POPITEM_SELECTED) {
					ItemSelected(items[i].id, items[i].playerID, items[i].GetX(), items[i].GetY());
					break;
				}
			break;
		
		case KEY_PRESS:
			if (event.c == 27) // escape
				Hide();
			break;
	}
	
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////

PopupItem::PopupItem()
{
	state = POPITEM_SELECTABLE;
	text = NULL;
	highlight = NULL;
	owner = NULL;
	playerID = -1;
}

PopupItem::~PopupItem()
{
	if (text) { delete text; text = NULL; }
	if (highlight) { delete highlight; highlight = NULL; }
}

void PopupItem::Init(PopupMenu *theOwner)
{
	owner = theOwner;
	
	SetView(owner->viewNum);
	SetCel(1);
	plane = owner->plane;
	SetPri(2000);
	
	sciView::Init();
	
	text = new sciText();
	text->plane = owner->plane;
	text->ParamInit(4, "", GetWidth() - 20, GetHeight(), 0, 0, 2001, JUST_TOP | JUST_LEFT);
	
	highlight = new sciView();
	highlight->plane = owner->plane;
	highlight->ParamInit(200026, 0, 0, 2002);
}

void PopupItem::SetID(int theID)
{
	id = theID;
}

void PopupItem::SetState(int theState)
{
	state = theState;
}

void PopupItem::SetText(char *s)
{
	text->SetTextToWidth(s);
}

void PopupItem::Posn(int newX, int newY)
{
	sciView::Posn(newX, newY);
	
	if (highlight)
		highlight->Posn(newX + 3, newY);
	
	if (text)
		text->Posn(newX + 10, newY + 1);
}

void PopupItem::Show()
{
	sciView::Show();
	
	highlight->Hide();
	
	switch (state) {
		case POPITEM_SELECTED:
			highlight->Show();
			text->SetFont(4);
			break;
			// FALL THROUGH
		case POPITEM_SELECTABLE:
		case POPITEM_DISPLAY_ONLY:
			text->SetFont(4);
			break;
		case POPITEM_DELIMITER:
			return;
		case POPITEM_DISPLAY_BOLD:
			text->SetFont(2);
			break;
	}
	
	text->Show();
}

void PopupItem::Hide()
{
	sciView::Hide();
	
	highlight->Hide();
	text->Hide();
}
