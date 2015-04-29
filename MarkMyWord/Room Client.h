#ifndef _roomclient
#define _roomclient

#include "winos.h"
#include <winsock.h>

#include "os.h"
#include "sci.h"
#include "globals.h"
#include "chat.h"
#include "resman.h"
#include "dirtyword.h"
#include "gifview.h"
#include "snmachineid.h"
#include "tickertext.h"

#include "cmMsg.h"
#include "cmConn.h"
#include "cmConmgr.h"

#include "wonplaydll.h"

#define HOYLE_CHARACTER_WIDTH	100
#define HOYLE_CHARACTER_HEIGHT	84

#define IS_A_CASINO_GAME		((wonParams.roomID & 0x80000000) != 0)

using namespace WONplayDLL;

#define PLAYER_LINE_HEIGHT		16
#define NUM_VISIBLE_PLAYER_LINES 13

#define NUM_BACKDROPS			28 // including the "standard" backdrop
#define NUM_CARDBACKS			18 // including the "standard" card back
#define NUM_CHATSOUNDS			20
#define NUM_SHADOWS				76

enum { RCS_UNCONNECTED = 0,
	   RCS_RECEIVING_ROOM_HEADER,
	   RCS_RECEIVING_ROOM_TABLES,
	   RCS_RECEIVING_ROOM_PLAYERS,
	   RCS_RECEIVING_ROOM_CHAT_HISTORY,
	   RCS_IN_ROOM,
	   RCS_QUITTING }; // room connection status

enum { GCD_FACING_LEFT = 0, GCD_FACING_RIGHT }; // game character directions

enum { RBID_WON = 0, RBID_HOYLE, RBID_OPTIONS,
	   RBID_SPONSORSHIP, RBID_EXIT, RBID_MINIMIZE,
	   RBID_CHAT_PUBLIC, RBID_CHAT_PRIVATE, RBID_CHAT_BROADCAST,
	   RBID_TOURNAMENT_INFORMATION, RBID_SHOW_GAME }; // room button IDs

enum { CSBID_ROOM_CHAT = 0, CSBID_PLAYERS, CSBID_TABLES, CSBID_GAME_CHAT }; // client scroll box IDs

enum { PDL_EMPTY = 0, PDL_PLAYER, PDL_FRIENDS_DIVIDER, PDL_MONITORS_DIVIDER, PDL_DIVIDER,
	   PDL_TDS_DIVIDER, PDL_TPLAYERS_DIVIDER, PDL_TWATCHERS_DIVIDER }; // player display line types

enum { RCA_LEFT = 0, RCA_RIGHT, RCA_CENTER }; // room chair angles

enum { ODBV_PERSONAL = 0, ODBV_TABLE, ODBV_RATINGS}; // options dialog box version

enum { RSVP_REQUEST = 0, RSVP_INVITE }; // RSVP dialog box version

enum { INVITATIONS_ANYBODY = 0, INVITATIONS_NOBODY, INVITATIONS_FRIENDS }; // personal invitations options

enum { CHAT_CONTROLS_RADIO_BUTTONS = 0, CHAT_CONTROLS_PUSH_BUTTONS }; // personal chat controls options

enum { CHAT_PUSH_BUTTONS_PUBLIC = 0, CHAT_PUSH_BUTTONS_PRIVATE, CHAT_PUSH_BUTTONS_PREVIOUS }; // personal chat push buttons options

enum { AT_NONE = 0, AT_JPEG, AT_GIF }; // ad types

enum { SUIT_HEARTS = 0x10, SUIT_CLUBS = 0x20, SUIT_DIAMONDS = 0x30, SUIT_SPADES = 0x40 }; // suits

enum {
	// click on self in game
	POPID_GAME_EDIT_PROFILE = 1000,
	POPID_GAME_PERSONAL_OPTIONS,
	POPID_GAME_TABLE_OPTIONS,
	POPID_GAME_RATING_OPTIONS,
	POPID_GAME_RESTART_GAME,
	POPID_GAME_START_CHAT,
	POPID_GAME_COPY_CHAT_LOG,
	POPID_GAME_SHOW_CHATSOUNDS,
	POPID_GAME_RATINGS, //This is for either clicking on self or others in room
	
	// click on other in game
	POPID_GAME_FRIEND = 1100,
	POPID_GAME_VIEW_PROFILE,
	POPID_GAME_COMPLAINT,
	POPID_GAME_BOOT,
	
	// click on name in game watchers popup
	POPID_GAME_WATCHERS_CLICK = 1200,
	
	// click on won logo in room
	POPID_ROOM_GOTO_WONNET = 2000,
	POPID_ROOM_GOTO_SIERRACOM,
	POPID_ROOM_MEMBERSHIP_POLICY,
	POPID_ROOM_COMPLAINT,
	
	// click on Hoyle logo in room
	POPID_ROOM_GOTO_HOYLECOM = 2100,
	POPID_ROOM_ABOUT_GAME,
	POPID_ROOM_GETTING_STARTED,
	
	// click on self in room
	POPID_ROOM_LEAVE_TABLE = 2200,
	POPID_ROOM_TABLE_OPTIONS,
	POPID_ROOM_RATING_OPTIONS,
	POPID_ROOM_EDIT_PROFILE,
	POPID_ROOM_PERSONAL_OPTIONS,
	POPID_ROOM_BAN_OFFLINE, // moderators only
	POPID_ROOM_UNBAN, // moderators only
	
	// click on other in room
	POPID_ROOM_PRIVATE_CHAT = 2300,
	POPID_ROOM_FRIEND,
	POPID_ROOM_VIEW_PROFILE,
	POPID_ROOM_INVITE_TO_PLAY,
	POPID_ROOM_INVITE_TO_WATCH,
	POPID_ROOM_TOURNAMENT_JOIN,
	POPID_ROOM_TOURNAMENT_BANK,
	POPID_ROOM_MUTE,
	POPID_ROOM_PLAYER_COMPLAINT,
	POPID_ROOM_RATINGS, //This is for either clicking on self or others in room
	POPID_ROOM_GET_IP, // moderators only
	POPID_ROOM_GLOBAL_MUTE, // moderators only
	POPID_ROOM_GLOBAL_UNMUTE, // moderators only
	POPID_ROOM_BOOT, // moderators only
	POPID_ROOM_BAN_ONLINE, // moderators only
	
	// click on name in room watchers popup
	POPID_ROOM_WATCHERS_CLICK = 2400,
	
	// click on chair in room
	POPID_ROOM_JOIN = 2500,
	POPID_ROOM_WATCH,
	
	// click on chat log in room
	POPID_ROOM_COPY_CHAT_LOG = 2600,
	
	// (right) click on chat edit text in room
	POPID_ROOM_PASTE_CHAT = 2700,
	
	// click on chat log in chat plane
	POPID_CHAT_COPY_CHAT_LOG = 3000,
	
	// game-specific items
	POPID_GAME_SPECIFIC_BASE = 10000,
}; // popup menu IDs

typedef struct {
	char acName[MAX_PLAYER_NAME];
	short sRating;
} PlayerNameRatings;

//////////////////

// dialog box purpose
enum { DBP_UNKNOWN = 0, DBP_GETTING_STARTED, DBP_LEAVE_GAME, DBP_EXIT,
		DBP_PLAYER_LEFT_GAME, DBP_RESTART_GAME };

class ClientDialogButton;

class ClientDialogBox : public sciDialogBox
{
public:
	ClientDialogBox(sciPlane *parentPlane);
	~ClientDialogBox();
	void Show(int viewNumber, char *text, char *caption, unsigned int uType, int thePurpose);
	virtual void DoClick(int buttonID) { }
	
	int purpose;
	sciText *bodyText;
	ClientDialogButton *buttons[2];
};

class ClientDialogButton : public sciButton
{
public:
	ClientDialogButton() : sciButton() { }
	~ClientDialogButton() { }
	void DoClick(BOOL netMessage = FALSE);
	
	int id;
	ClientDialogBox *parent;
};

//////////////////

class BackdropDialogButton;

class BackdropDialogBox : public sciDialogBox
{
public:
	BackdropDialogBox(sciPlane *parentPlane, ROOMCLIENTTABLE *theOwner);
	~BackdropDialogBox();
	void Init();
	void Show();
	
	sciButtonGroup *choicesGroup;
	sciRadioButton *choices[NUM_BACKDROPS];
	BackdropDialogButton *buttons[2];
	
	ROOMCLIENTTABLE *owner;
};

class BackdropDialogButton : public sciButton
{
public:
	BackdropDialogButton(int theID, BackdropDialogBox *theParent) : sciButton() { id = theID; parent = theParent; }
	~BackdropDialogButton() { }
	void DoClick(BOOL netMessage = FALSE);
	
	int id;
	BackdropDialogBox *parent;
};

class CardBacksDialogButton;

class CardBacksDialogBox : public sciDialogBox
{
public:
	CardBacksDialogBox(sciPlane *parentPlane, ROOMCLIENTTABLE *theOwner);
	~CardBacksDialogBox();
	void Init();
	void Show();
	
	sciText *cardBacksLabel;
	sciButtonGroup *choicesGroup;
	sciRadioButton *choices[NUM_CARDBACKS];
	
	sciText *cardFaceLabel;
	sciButtonGroup *cardFaceGroup;
	sciRadioButton *cardFaces[2];
	
	CardBacksDialogButton *buttons[2];
	
	ROOMCLIENTTABLE *owner;
};

class CardBacksDialogButton : public sciButton
{
public:
	CardBacksDialogButton(int theID, CardBacksDialogBox *theParent) : sciButton() { id = theID; parent = theParent; }
	~CardBacksDialogButton() { }
	void DoClick(BOOL netMessage = FALSE);
	
	int id;
	CardBacksDialogBox *parent;
};

//////////////////

class ChatSoundDialogButton;

class ChatSoundDialogBox : public sciDialogBox
{
public:
	ChatSoundDialogBox(sciPlane *parentPlane);
	~ChatSoundDialogBox();
	void Init();
	void Show();
	
	sciButtonGroup *choicesGroup;
	sciRadioButton *choices[NUM_CHATSOUNDS];
	ChatSoundDialogButton *buttons[3];
};

class ChatSoundDialogButton : public sciButton
{
public:
	ChatSoundDialogButton(int theID, ChatSoundDialogBox *theParent) : sciButton() { id = theID; parent = theParent; }
	~ChatSoundDialogButton() { }
	void DoClick(BOOL netMessage = FALSE);
	
	int id;
	ChatSoundDialogBox *parent;
};

//////////////////

class OptionsDialogButton;
class OptionsDialogPanelButton;
class OptionsDialogBox;
class OptionsDialogLoggingRB;
class FaceMakerFace;

class ShadowScroll : public sciScrollBox
{
public:
	ShadowScroll();
	~ShadowScroll();
	
	void Init(OptionsDialogBox *theParent);
	void Show();
	void Hide();
	void SelectItem(int newItemID);
	int GetSelectedItemID();
	int GetNumItems();
	int GetMaxVisibleItems();
	void DoScroll(int direction, int numItems);
	void DoRefresh(int firstItem);
	Boolean InFaceMakerMode();
	
	sciView *normalView;
	FaceMakerFace *faceMakerView;
	sciView *frame;
	int selectedItemID;
	OptionsDialogBox *parent;
};

class ShadowRadioButton : public sciRadioButton
{
public:
	ShadowRadioButton(ShadowScroll *theScroller, int newID)
		: sciRadioButton(newID) { scroller = theScroller; }
	~ShadowRadioButton() { }
	
	void GotSelected();
	
	ShadowScroll *scroller;
};

class RatingCheckbox : public sciCheckBox
{
public:
	RatingCheckbox(OptionsDialogBox *poParent) { m_poParentDlg = poParent; }
	void DoClick(Boolean netMessage);

private:
	OptionsDialogBox *m_poParentDlg;
};

class OptionsDialogBox : public sciDialogBox
{
public:
	OptionsDialogBox(sciPlane *parentPlane);
	~OptionsDialogBox();
	void Init();
	void Show(int theVersion, Boolean theInGame);
	
	int version;
	Boolean inGame;
	
	OptionsDialogButton *buttons[2];
	FourSidedRect *frames[3];
	sciRect *titleBar;
	sciText *titleBarText;
	sciButtonGroup *panelGroup;
	OptionsDialogPanelButton *personalPanelButton;
	OptionsDialogPanelButton *tablePanelButton;
	OptionsDialogPanelButton *ratingsPanelButton;
	
	// personal options in room or game
	
	int numFaceFiles;
	
	sciCheckBox *soundEffectsCheckBox;
	sciCheckBox *offensiveChatCheckBox;
	sciCheckBox *limitAnimationsCheckBox;
	
	sciText *shadowLabel;
	sciButtonGroup *shadowTypeGroup;
	ShadowRadioButton *shadowTypeButtons[2];
	ShadowScroll *shadowScroll;
	sciText *faceMakerLabel;
	OptionsDialogButton *faceMakerInfoButton; //ID:10
	
	// personal options in room
	
	sciText *lobbyChatControlsLabel;
	sciButtonGroup *lobbyChatControlsGroup;
	sciRadioButton *lobbyChatControlsButtons[2];
	sciButtonGroup *lobbyChatPushButtonsGroup;
	sciRadioButton *lobbyChatPushButtonsButtons[3];
	
	sciText *invitationsLabel;
	sciButtonGroup *invitationsGroup;
	sciRadioButton *invitationsButtons[3];
	
	// personal options in game
	
	sciText *chatLabel;
	sciButtonGroup *chatGroup;
	sciRadioButton *chatButtons[3];
	sciCheckBox *chatSoundsCheckBox;
	
	sciText *balloonsLabel;
	sciButtonGroup *balloonsGroup;
	sciRadioButton *balloonsButtons[3];
	sciCheckBox *fadeCheckBox;
	
	sciText *logLabel;
	sciButtonGroup *logGroup;
	sciRadioButton *logButtons[2];
	sciButtonGroup *brightnessGroup;
	sciRadioButton *brightnessButtons[2];
	
	OptionsDialogButton *backdropButton; //ID:20
	OptionsDialogButton *cardsButton;	//ID:30
	
	// table options
	
	sciText *joinGameLabel;
	sciButtonGroup *joinGameGroup;
	sciRadioButton *joinGameButtons[3];
	
	sciText *watchersLabel;
	sciButtonGroup *watchersGroup;
	sciRadioButton *watchersButtons[3];
	sciCheckBox *muteWatchersCheckBox;
	
	sciText *tableDescriptionLabel;
	EditBox *tableDescriptionText;

	// Ratings options
	sciCheckBox *m_poTableIsRatedCheckBox;
	RatingCheckbox *m_poRequireRatingToSitCheckBox;
	sciCheckBox *m_apoRatingLimitsCheckBox[NUM_RATING_CATAGORIES];
	sciView *m_apoRatingSkillMarkerView[NUM_RATING_CATAGORIES];
	
	sciText *m_poTop10Title;
	sciText *m_poTop10BodyL;
	sciText *m_poTop10BodyR;
	sciText *m_poCurrentAIRating;

	sciText *m_poRatingsDescTitle;
	sciText *m_poRatingsDescBody;
	OptionsDialogButton *m_poFeedbackButton; //ID:40

	// game-specific options
	GAMESPECIFICCONTROLS *gameSpecificControls;
};

class OptionsDialogButton : public sciButton
{
public:
	OptionsDialogButton(int theID, OptionsDialogBox *theParent)
		: sciButton() { id = theID; parent = theParent; }
	~OptionsDialogButton() { }
	void DoClick(BOOL netMessage = FALSE);
	
	int id;
	OptionsDialogBox *parent;
};

class OptionsDialogPanelButton : public sciRadioButton
{
public:
	OptionsDialogPanelButton(int theID, OptionsDialogBox *theParent)
		: sciRadioButton(theID) { parent = theParent; }
	~OptionsDialogPanelButton() { }
	Boolean HandleEvent(sciEvent &event);

	OptionsDialogBox *parent;
};

class OptionsDialogLoggingRB : public sciRadioButton
{
public:
	OptionsDialogLoggingRB(int theID, OptionsDialogBox *theParent)
		: sciRadioButton(theID) { parent = theParent; }
	~OptionsDialogLoggingRB() { }
	Boolean HandleEvent(sciEvent &event);
//	void DoClick(BOOL netMessage = FALSE);
	
	OptionsDialogBox *parent;
};

class ProfileDialogButton;

class ProfileDialogBox : public sciDialogBox
{
public:
	ProfileDialogBox(sciPlane *parentPlane);
	~ProfileDialogBox();
	void Init();
	void ExtractProfileField(char *key, char *profile, char *s);
	void StripSpecialQuote(char *s);
	void Show(Boolean thePlayerID);
	
	int playerID;
	
	ProfileDialogButton *buttons[2];
	
	sciView *shadow;
	sciView *icon;
	
	sciText *ageLabel;
	EditBox *age;
	
	sciText *sexLabel;
	EditBox *sex;
	
	sciText *cityLabel;
	EditBox *city;
	
	EditBox *profile;

	FaceMakerFace *faceMakerView;
};

class ProfileDialogButton : public sciButton
{
public:
	ProfileDialogButton(int theID, ProfileDialogBox *theParent)
		: sciButton() { id = theID; parent = theParent; }
	~ProfileDialogButton() { }
	void DoClick(BOOL netMessage = FALSE);
	
	int id;
	ProfileDialogBox *parent;
};

//////////////////

class ComplaintDialogButton;

class ComplaintDialogBox : public sciDialogBox
{
public:
	ComplaintDialogBox(sciPlane *parentPlane);
	~ComplaintDialogBox();
	void Init();
	void StripSpecialQuote(char *s);
	void Show(Boolean thePlayerID, Boolean inRoom);
	
	int playerID;
	char inGame;
	
	ComplaintDialogButton *buttons[3];
	
	EditBox *complaint;
};

class ComplaintDialogButton : public sciButton
{
public:
	ComplaintDialogButton(int theID, ComplaintDialogBox *theParent)
		: sciButton() { id = theID; parent = theParent; }
	~ComplaintDialogButton() { }
	void DoClick(BOOL netMessage = FALSE);
	
	int id;
	ComplaintDialogBox *parent;
};

//////////////////

void ShowHideTIControls(HWND hDlg);

class TournamentDialogBox : public sciObject
{
public:
	TournamentDialogBox();
	~TournamentDialogBox();
	void Show();
	void Hide();
	
	HWND dialog;
};

//////////////////

class ClientScrollBox : public sciScrollBox
{
public:
	ClientScrollBox() : sciScrollBox(GDefs::VERTICAL) { }
	~ClientScrollBox() { }
	
	void Init(sciPlane *thePlane, int theID);
	
	int GetNumItems();
	int GetMaxVisibleItems();
	void DoScroll(int direction, int numItems);
	void DoRefresh(int firstItem);
	
	int id;
};

//////////////////

enum { POPDIR_ROOM = 0, POPDIR_GAME }; // popup shadow directions

enum { POPWIDTH_NORMAL = 0, POPWIDTH_WIDE }; // popup widths

enum { POPITEM_SELECTABLE = 0,
	   POPITEM_SELECTED,
	   POPITEM_DELIMITER,
	   POPITEM_DISPLAY_ONLY,
	   POPITEM_DISPLAY_BOLD }; // popup item states

class PopupMenu;

class PopupItem : public sciView
{
public:
	PopupItem();
	~PopupItem();
	
	void Init(PopupMenu *theOwner);
	void SetID(int theID);
	void SetState(int theState);
	void SetText(char *s);
	void Posn(int newX, int newY);
	void Show();
	void Hide();
	
	int id;
	int state;
	int playerID;
	int seat;
	sciText *text;
	sciView *highlight;
	PopupMenu *owner;
};

class PopupMenu : public sciFeature
{
public:
	PopupMenu(sciPlane *thePlane);
	~PopupMenu();
	
	void Init(int theDirection);
	int GetWidth();
	int GetHeight();
	void Posn(int x, int y);
	void Show(sciFeature *theKeepShownFeature);
	void Hide();
	
	void SetNumItems(int n);
	void AddItem(int id, int state, char *text, int thePlayerID = 0);
	
	Boolean HandleEvent(sciEvent &event);
	virtual void ItemSelected(int itemID, int itemPlayerID, int itemX, int itemY) { }
	
	int direction;
	int viewNum;
	int numItems;
	int totalItems;
	
	// information passed via the popup
	int playerID;
	int table;
	int seat;
	
	sciView *top;
	PopupItem *items;
	sciView *bottom;
	sciFeature *keepShownFeature;
};

//////////////////

class RoomPopup : public PopupMenu
{
public:
	RoomPopup(sciPlane *thePlane) : PopupMenu(thePlane) { }
	~RoomPopup() { }
	void ItemSelected(int itemID, int itemPlayerID, int itemX, int itemY);
	void Posn(int x, int y) { PopupMenu::Posn(x, y > 250 ? y-20 : y);} 
};

class GamePopup : public PopupMenu
{
public:
	GamePopup(sciPlane *thePlane, GAMEDISPLAY *theDisplay) : PopupMenu(thePlane) { display = theDisplay; }
	~GamePopup() { }
	void ItemSelected(int itemID, int itemPlayerID, int itemX, int itemY);
	void Posn(int x, int y) { PopupMenu::Posn(x, y > 250 ? y-20 : y);} 
	
	GAMEDISPLAY *display;
};

class ChatPopup : public PopupMenu
{
public:
	ChatPopup(sciPlane *thePlane) : PopupMenu(thePlane) { }
	~ChatPopup() { }
	void ItemSelected(int itemID, int itemPlayerID, int itemX, int itemY);
};


//////////////////

enum { FM_TORSO = 0, FM_CLOTHING, FM_HEAD, FM_LEYE, FM_REYE, FM_HAIR,
	   FM_NOSE, FM_MOUTH, FM_LEYEBROW, FM_REYEBROW, FM_FACIALHAIR, FM_GLASSES };

class GameCharacter;

typedef struct {
	int featureViews[FM_MAX_FEATURES];
	char skinTone;
	char xOffsets[FM_MAX_FEATURES];
	char yOffsets[FM_MAX_FEATURES];
} FaceFile;

class FaceMakerSubView : public sciProp
{
	public:
		FaceMakerSubView() { }
		~FaceMakerSubView() { }
		void Render(RECT &updateRect);
		void FMRender(DWORD *pixelData, int xOffset, int yOffset);
};

class FaceMakerFace : public sciFeature
{
	public:
		FaceMakerFace();
		~FaceMakerFace();
		void Init(sciPlane *thePlane, GameCharacter *newOwner);
		void Posn(int newX, int newY);
		void SetPri(int newPri);
		void Show();
		void ShowFeature(int feature);
		void Hide();
		void Render(RECT &updateRect);
		int GetFeatureNumPieces(int feature);
		int GetFeatureFirstViewIndex(int feature);
		int GetFeatureIndexFromViewIndex(int viewIndex);
		
		void SetHalfSize(bool onoff, SCALE_TYPE st = ST_DISCARD);
		
		bool halfSize;
		FaceInfo faceInfo;
		FaceMakerSubView *subViews[FM_MAX_VIEWS];
};

//////////////////

class GamePlaque : public sciView
{
public:
	GamePlaque(GAMEDISPLAY *theDisplay, int theSeat) { display = theDisplay; seat = theSeat; }
	Boolean HandleEvent(sciEvent &event);
	
	int seat;
	GAMEDISPLAY *display;
};

class GameBootDisplay : public sciView
{
public:
	GameBootDisplay(GAMEDISPLAY *theDisplay, int theSeat) { display = theDisplay; seat = theSeat; text = NULL; }
	~GameBootDisplay() { if (text) { delete text; text = NULL; } }
	void Init();
	void SetPri(int newPri);
	void Posn(int newX, int newY);
	void Show(int count);
	
	int seat;
	sciText *text;
	GAMEDISPLAY *display;
};

class GameCharacter : public sciActor
{
public:
	GameCharacter(GAMEDISPLAY *theDisplay);
	~GameCharacter();
	virtual void Init(int newTable, int newSeat, int newX, int newY, int newStyle, int balloonOrientation);
	virtual void Show(Boolean showAsCurrentPlayer);
	virtual void Hide();
	virtual Boolean HandleEvent(sciEvent &event);
	
	int table;
	int seat;
	int style;
	FaceMakerFace *faceMakerView;
	GamePlaque *plaque;
	sciProp *plaqueHighlight;
	sciText *nameText;
	sciText *scoreText;
	ChatBalloon *chatBalloon;
	GameBootDisplay *bootDisplay;
	sciText *resetDisplay;
	GAMEDISPLAY *display;
};

class GameWatcher : public sciActor
{
public:
	GameWatcher(GAMEDISPLAY *theDisplay) { display = theDisplay; }
	~GameWatcher() { }
	void Init(int newTable, int newSeat) { table = newTable, seat = newSeat; }
	
	int table;
	int seat;
	GAMEDISPLAY *display;
};

class GameWatchersDisplay : public sciActor
{
public:
	GameWatchersDisplay(int newTable, GAMEDISPLAY *theDisplay);
	~GameWatchersDisplay();
	void Init(sciPlane *plane);
	void SetPri(int newPri);
	void Posn(int newX, int newY);
	void Show(int numWatchers);
	void Hide();
	
	Boolean HandleEvent(sciEvent &event);
	
	int table;
	sciText *text;
	sciText *muteText;
	ChatBalloon *chatEditBalloon;
	ChatBalloon *chatDisplayBalloon;
	GAMEDISPLAY *display;
};

class WatchersBlinkScript : public sciScript
{
public:
	WatchersBlinkScript(GameWatchersDisplay *theOwner) { owner = theOwner; }
	~WatchersBlinkScript() { }
	
	void ChangeState(int newState);
	
	GameWatchersDisplay *owner;
};

class StandardCard : public sciActor
{
public:
	StandardCard(GAMECHARACTER *newOwner, sciPlane *thePlane);
	~StandardCard();
	
	virtual unsigned char TranslateRank(unsigned char rank) { return rank; }
	virtual unsigned char TranslateSuit(unsigned char suit) { return suit; }
	virtual void GetRankAndSuit(int *rank, int *suit);
	virtual void Show(int pri);
	virtual void Hide();
	virtual void Posn(int newX, int newY);
	virtual void SetPri(int newPri);
	
	GAMECHARACTER *owner;
	unsigned char value; // 0 = empty, 0xFF = hidden
	Boolean enabled;
	Boolean mouseDown;
	
private:
	int GetPipPlace(int pipNum, int rank);
	int GetPipCel(int pipPlace, int suit);
	POINT GetPipPosn(int pipPlace);
	
	sciView *upperRank;
	sciView *upperSuit;
	sciView *center;
	sciView *lowerRank;
	sciView *lowerSuit;
	sciView *pips[10];
};

class GameOptionsButton : public sciButton
{
public:
	GameOptionsButton(GAMEDISPLAY *theDisplay) : sciButton() { display = theDisplay; }
	~GameOptionsButton() { }
	void DoClick(BOOL netMessage = FALSE);
	
	GAMEDISPLAY *display;
};

class GameExitButton : public sciButton
{
public:
	GameExitButton(GAMEDISPLAY *theDisplay) : sciButton() { display = theDisplay; }
	~GameExitButton() { }
	void DoClick(BOOL netMessage = FALSE);
	
	GAMEDISPLAY *display;
};

class GameMinimizeButton : public sciButton
{
public:
	GameMinimizeButton(GAMEDISPLAY *theDisplay) : sciButton() { display = theDisplay; }
	~GameMinimizeButton() { }
	void DoClick(BOOL netMessage = FALSE);
	
	GAMEDISPLAY *display;
};

class GameSponsorshipButton : public sciButton
{
public:
	GameSponsorshipButton() { }
	~GameSponsorshipButton() { }
	
	void DoClick(BOOL netMessage = FALSE);
};

class GameDisplay;

class ShowLobbyButton : public sciButton
{
public:
	ShowLobbyButton() { }
	~ShowLobbyButton() { }
	
	void DoClick(BOOL netMessage = FALSE);
};

class ShowChatButton : public sciButton
{
public:
	ShowChatButton() { }
	~ShowChatButton() { }
	
	void DoClick(BOOL netMessage = FALSE);

	GameDisplay *parent;
};

class GameDialogBox : public ClientDialogBox
{
public:
	GameDialogBox(sciPlane *parentPlane, ROOMCLIENTTABLE *theOwner)
		: ClientDialogBox(parentPlane) { owner = theOwner; }
	void DoClick(int buttonID);
	
	ROOMCLIENTTABLE *owner;
};

class TickerDisplay : public TickerText
{
public:
	TickerDisplay(GameDisplay *theGameDisplay);
	~TickerDisplay();
	
	void Doit();
	
	GameDisplay *gameDisplay;
};

class GameDisplay : public sciObject
{
public:
	GameDisplay();
	~GameDisplay();
	virtual void Init(int newTable);
	virtual void PreferencesChanged();
	virtual Boolean DisplayPlayer(GameState *gameState, int seat);
	virtual void ShowWatchersDisplay(GameState *gameState);
	virtual void ShowHandNumber(int n, int x, int y);
	virtual void HideHandNumber();
	virtual void AddPlayerPopupItems(int playerID);
	virtual void Hide();
	
	int table;
	GAMECHARACTER *characters[MAX_GAME_PLAYERS];
	GAMEWATCHER *watchers[MAX_GAME_WATCHERS];
	GameWatchersDisplay *watchersDisplay;
	ChatLog *gameChatLog;
	TickerDisplay *tickerDisplay;
	GameOptionsButton *optionsButton;
	OptionsDialogBox *optionsDialog;
	BackdropDialogBox *backdropDialog;
	CardBacksDialogBox *cardBacksDialog;
	ChatSoundDialogBox *chatSoundDialog;
	ProfileDialogBox *profileDialog;
	ComplaintDialogBox *complaintDialog;
	GameDialogBox *gameMessageBox;
	GamePopup *popup;
	GameExitButton *exitButton;
	GameMinimizeButton *minimizeButton;
	GameSponsorshipButton *sponsorshipButton;
	ShowLobbyButton *showLobbyButton;
	ShowChatButton *showChatButton;
	sciView *teamPlaque;
	sciView *handNumberBacking;
	sciText *handNumberText;

	sciRect *m_poReadyForMessageRect;
};

class GamePlane : public sciPlane
{
public:
	GamePlane(Boolean newModal, sciPlane *newParentPlane, ROOMCLIENTTABLE *theOwner)
		: sciPlane(newModal, newParentPlane) { owner = theOwner; }
	~GamePlane() { }
	virtual void AttemptClosePlane();
	
	ROOMCLIENTTABLE *owner;
};

class RoomClientTable : public sciObject
{
public:
	RoomClientTable();
	~RoomClientTable();
	virtual void Init(int theIndex);
	virtual void InitPlayer(int seat);
	virtual void FillTableGameValues(int *values) = 0;
	virtual void GetDescription(char *s) = 0;
	virtual void PlayerSits(ROOMCLIENTPLAYER *player, PlayerState *playerState);
	virtual void PlayerStands(ROOMCLIENTPLAYER *player, int newID);
	virtual void Chat(ROOMCLIENTPLAYER *player, int sound, char *text);
	void SetReadyForMessages(Boolean ready);
	
	virtual int HandleMessage(MFS_GAME_MESSAGE_HEADER *mfs) = 0;
	
	virtual void Clear();
	virtual void Display();
	virtual void DisplayForNewGameState() = 0;
	virtual void SetTickerText(char *s);
	virtual Boolean AddTickerSegment(char *s);
	
	virtual void ShowModelessMessage(int viewNumber, char *text, char *caption, unsigned int uType, int purpose = DBP_UNKNOWN);
	
	virtual int GetMyPlayingIndex(Boolean mustBeRealPlayer);
	virtual Boolean ContainsPlayerID(int id, int *seat);
	virtual int GetNumPlayers();
	virtual int GetNumWatchers();
	
	virtual Boolean CanChangeBackdrop() { return FALSE; }
	virtual Boolean NeedsTeamPlaque() { return FALSE; }
	virtual Boolean CanChangeCardBacks() { return FALSE; }
	
	// Note: This function is used to determine if a game requires saving.
	// It is assumed that all games that need saving are 2-player, no join
	// in progress games...
	virtual Boolean NoJoinInProgress() { return FALSE; }
	virtual Boolean NeedSaveGame() { return SaveGameIfNeeded(); }  //can be used to split out test logic of SaveGameIfNeeded()
	virtual Boolean SaveGameIfNeeded() { return FALSE; }
	
	// Note: AlwaysOfferSaveGame() is multi-purposed.  The first use is to allow the player the _choice_
	// as to whether or not to save the game (don't always save).  Also, by using this function, the
	// game can be saved even if the leaving player is the last one at the table.  Finally, it also
	// allows a NoJoinInProgress game that can be saved.
	virtual Boolean AlwaysOfferSaveGame() { return FALSE; }

	virtual void *GetPersistentGameStateData() { return NULL; }
	virtual void SetPersistentGameStateData(void *data) { }
	
	void SaveGame(char *theFileName = NULL); //provide a name to override for 1 person games (i.e. word games, see CW)
	void GetSavedGameFileName(char *fileName, char *theOppName = NULL);  //theOppName is used for 1 person games (word games)
	void LoginToFileName(char *name, char *convertedName);
	
	// restart game
	char restartVotes[MAX_GAME_PLAYERS];

	int index;
	int status;
	int joinOption;
	int watchersOption;
	Boolean muteWatchers;
	char ratingsFlags;
	char ratingsSkills;
	int m_iCardBacks;
	int m_iCardFaces;
	char description[MAX_TABLE_DESCRIPTION];
	Boolean readyForMessages;
	GameState gameState;
	Boolean answeredMostRecentRequest;
	GAMEDISPLAY *display;
	GAMEPLANE *plane;
};

class RoomClientPlayer : public sciObject
{
public:
	RoomClientPlayer() { isFriend = FALSE; isSelected = FALSE; isMuted = FALSE; }
	~RoomClientPlayer() { }

	Boolean IsFriend()				{ return isFriend && !isMuted; }
	Boolean IsMuted()				{ return isMuted && !IsMonitor(); }

	Boolean IsCustomer()			{ return (info.level & UL_CUSTOMER) != 0; }

	Boolean IsMonitor()				{ return (info.level & UL_MODERATOR); }
	Boolean IsSuperUser()			{ return (info.level & UL_SUPERUSER); }
	Boolean IsMonitorOrSuperUser()	{ return IsMonitor() || IsSuperUser(); }

	Boolean IsReferee()				{ return (info.level & UL_REFEREE) != 0; }
	Boolean IsMonitorOrReferee()	{ return IsMonitor() || IsReferee(); }

	Boolean IsTournamentAdministrator() 
									{ return (info.level & UL_TADMIN) != 0; }
	Boolean IsRefereeOrTA()			{ return IsReferee() || IsTournamentAdministrator(); }

	Boolean IsHelper()				{ return (info.level & UL_HELPER) != 0; }
	Boolean IsRalph()				{ return (info.level & UL_RALPH) != 0; }

	Boolean IsInTournament() { return (info.flags & UF_IN_TOURNAMENT) != 0; }
	int GetIconCel();
	
	PlayerInfo info;
	char profile[MAX_PLAYER_PROFILE];
	// char gameRecord[MAX_PLAYER_GAME_RECORD];
	
	Boolean isFriend;
	Boolean isSelected;
	Boolean isMuted;
};

class RoomClientConnections : public TConnectionMgr
{
public:
	RoomClientConnections(int maxConnections, int maxListeners = 0);
	~RoomClientConnections();
	
	bool UsesAdminPort() const { return false; }
	
	// callback methods
	virtual void ConnectionCompleted(HCONNECTION c);
	virtual void ClientConnected(HCONNECTION c);
	virtual void ConnectionClosed(HCONNECTION c);
	virtual void RequestHeartbeat(HCONNECTION c);
	virtual void ProcessUnquedMessage(HCONNECTION c, char *buf, int length) { }
	virtual void ProcessRawMessage(HCONNECTION c, char *buf, int length) { }
	
	virtual bool PreProcessMessage(HCONNECTION c, const void *message, int length);

	virtual bool RoomClientConnections::IsValidAdminHost(IPAddr ipaddr);
};

class QueuedMessage : public sciObject
{
public:
	int table;
	char *message;
	int length;
};

class RoomDialogBox : public ClientDialogBox
{
public:
	RoomDialogBox(sciPlane *parentPlane, ROOMCLIENT *theOwner)
		: ClientDialogBox(parentPlane) { owner = theOwner; }
	void DoClick(int buttonID);
	
	ROOMCLIENT *owner;
};

class RoomButton : public sciButton
{
public:
	RoomButton(int theID) : sciButton() { id = theID; }
	~RoomButton() { }
	void DoClick(BOOL netMessage = FALSE);
	
	int id;
};

class RoomRadioButton : public sciRadioButton
{
public:
	RoomRadioButton(int newID);
	~RoomRadioButton();
	void Init();
	void Show();
	void Hide();
	void Posn(int x, int y);
	void SetPri(int newPri);
	
	sciView *well;
};

class RoomPlayersDisplayLineRect : public sciRect
{
public:
	RoomPlayersDisplayLineRect(int theIndex) { index = theIndex; };
	~RoomPlayersDisplayLineRect() { };
	
	Boolean HandleEvent(sciEvent &event);
	
	int index;
};

class RoomPlayersDisplayLine : public sciObject
{
public:
	RoomPlayersDisplayLine();
	~RoomPlayersDisplayLine();
	void Init(int index);
	void SetTypeAndPlayerIndex(int theType, int thePlayerIndex);
	void Posn(int x, int y);
	void Show();
	
	int type;
	int playerIndex;
	RoomPlayersDisplayLineRect *area;
	sciView *icon;
	sciText *name;
	sciText *tournamentMarker;
	sciView *ranking;
	sciView *tableStatus;
	sciText *tableNumber;
	sciView *selectedOverlay;
	sciView *mutedOverlay;
	sciView *divider;
};

class RoomPlayersDisplay : public sciObject
{
public:
	RoomPlayersDisplay() { }
	~RoomPlayersDisplay() { }
	void Init();
	int GetNumLines();
	void DisplayLines(int fromLineNum = -1);
	
	RoomPlayersDisplayLine displayLines[NUM_VISIBLE_PLAYER_LINES];
};

class RoomTablesDisplayPlayer : public sciView
{
public:
	RoomTablesDisplayPlayer();
	~RoomTablesDisplayPlayer();
	
	void Show(int thePlayerID);
	Boolean HandleEvent(sciEvent &event);
	bool IsOnMe(int eventX, int eventY);
	
	FaceMakerFace *faceMakerView;

	int playerID;
};

class NameTagClickPad;
class RoomTablesDisplayChair;

class RoomTablesDisplaySeat : public sciObject
{
public:
	RoomTablesDisplaySeat();
	~RoomTablesDisplaySeat();
	void Init(int theSeatIndex);
	Boolean PlayerInSeatFacesRight();
	void Posn(int x, int y);
	void Show(int theTableIndex);
	
	int tableIndex;
	int seatIndex;
	Boolean behindTable;
	int chairAngle;
	RoomTablesDisplayChair *chair;
	RoomTablesDisplayPlayer *player;
	sciView *nameTag;
	sciText *nameLabel;
	NameTagClickPad *nameTagClick;
};

class RoomTablesDisplayChair : public sciView
{
public:
	RoomTablesDisplayChair() { table = -1; };
	~RoomTablesDisplayChair() { };
	void Show(int theTable, int theSeat) { table = theTable; seat = theSeat; sciView::Show(); }
	Boolean HandleEvent(sciEvent &event);
	Boolean HandleEventEx(sciEvent &event, Boolean clickedOnName);
	
	int table;
	int seat;
	RoomTablesDisplaySeat *parent;
};

class NameTagClickPad : public sciView
{
public:
	NameTagClickPad(RoomTablesDisplaySeat *theParent) { parent = theParent; }
	~NameTagClickPad() { }
	Boolean HandleEvent(sciEvent &event);

	RoomTablesDisplaySeat *parent;
};

class RoomTablesDisplayWatchers : public sciView
{
public:
	RoomTablesDisplayWatchers();
	~RoomTablesDisplayWatchers();
	void Init();
	void SetPri(int newPri);
	void Posn(int newX, int newY);
	void Show(int theTableIndex);
	void Hide();
	Boolean HandleEvent(sciEvent &event);
	
	int tableIndex;
	sciText *text;
	sciText *muteText;
};

class RoomTablesDisplayStartButton : public sciButton
{
public:
	RoomTablesDisplayStartButton() { }
	~RoomTablesDisplayStartButton() { }
	void DoClick(BOOL netMessage = FALSE);
	Boolean HandleEvent(sciEvent &event);
};

class RoomTablesDisplayTable;

class RoomTablesDisplayDescription : public sciRect
{
public:
	RoomTablesDisplayDescription();
	~RoomTablesDisplayDescription();
	void Init();
	void SetPri(int newPri);
	void Posn(int newX, int newY);
	void Show(int theTableIndex, RoomTablesDisplayTable *theDisplayTable);
	void Hide();
	Boolean HandleEvent(sciEvent &event);
	
	int tableIndex;
	sciText *text1;
	sciText *text2;
	RoomTablesDisplayTable *displayTable;
};

class RoomTablesDisplayTable : public sciView
{
public:
	RoomTablesDisplayTable();
	~RoomTablesDisplayTable();
	void Init();
	void Posn(int newX, int newY);
	void Show(int theTableIndex);
	Boolean HandleEvent2(sciEvent &event, sciFeature *clickedObject, int theTableIndex);
	Boolean HandleEvent(sciEvent &event);
	
	int tableIndex;
	sciText *label;
	sciView *numberView;
	sciText *requestingApproval;
	GAMESPECIFICTABLEDISPLAY *gstDisplay;
	RoomTablesDisplayDescription *description;
	RoomTablesDisplayStartButton *startButton;
	RoomTablesDisplayWatchers *watchersDisplay;
	RoomTablesDisplaySeat *seats[MAX_GAME_PLAYERS];
	sciView *m_apoRatingSkillMarkerView[NUM_RATING_CATAGORIES];
};

class RoomTablesDisplay : public sciObject
{
public:
	RoomTablesDisplay() { }
	~RoomTablesDisplay() { }
	void Init();
	void DisplayTables(int fromLineNum = -1);
	void BringTableIntoFullView(int table);
	
	RoomTablesDisplayTable displayTables[MAX_VISIBLE_TABLES];
};

class RSVPButton;

class RSVPPlane : public sciPlane
{
public:
	RSVPPlane();
	~RSVPPlane();
	void Init();
	void Show(int theVersion);
	
	int version;
	
	sciText *text;
	RSVPButton *buttons[2];
};

class RSVPButton : public sciButton
{
public:
	RSVPButton(int theID, RSVPPlane *theParent)
		: sciButton() { id = theID; parent = theParent; }
	~RSVPButton() { }
	void DoClick(BOOL netMessage = FALSE);
	
	int id;
	RSVPPlane *parent;
};

class InstructionsDialogBox : public ClientDialogBox
{
public:
	InstructionsDialogBox(sciPlane *parentPlane);
	~InstructionsDialogBox();
	void Show(Boolean startup);
	void DoClick(int buttonID);
	
	sciCheckBox *showOnStartup;
};

class RoomChatEditBox : public EditBox
{
public:
	RoomChatEditBox() { }
	~RoomChatEditBox() { }
	
	Boolean HandleEvent(sciEvent &event);
};

class JPEGAd : public sciView
{
public:
	JPEGAd() { }
	~JPEGAd() { }
	
	Boolean HandleEvent(sciEvent &event);
};

class GIFAd : public GIFView
{
public:
	GIFAd() { }
	~GIFAd() { }
	
	Boolean HandleEvent(sciEvent &event);
};

class RoomOverlayView : public sciView
{
public:
	RoomOverlayView() : sciView() {}
	~RoomOverlayView() {}

	Boolean HandleEvent(sciEvent &event);
	
	Boolean IsInTableArea(sciEvent &event);
	Boolean IsInPlayerListArea(sciEvent &event);
	Boolean IsInChatArea(sciEvent &event);

};

class RoomClient : public sciObject
{
public:
	RoomClient();
	~RoomClient();
	virtual int Init();
	virtual int Tick();
	
	void QueueMessage(int table, char *message, int length);
	
	virtual int HandleMFS_ROOM_FULL(char *message, int length);
	virtual int HandleMFS_ROOM_STATUS_HEADER(char *message, int length);
	virtual int HandleMFS_ROOM_STATUS_TABLES(char *message, int length);
	virtual int HandleMFS_ROOM_STATUS_PLAYERS(char *message, int length);
	virtual int HandleMFS_ROOM_STATUS_CHAT_HISTORY(char *message, int length);
	virtual int HandleMFS_ROOM_ENTER(char *message, int length);
	virtual int HandleMFS_ROOM_CHAT(char *message, int length);
	virtual int HandleMFS_ROOM_SIT(char *message, int length);
	virtual int HandleMFS_ROOM_CHANGE_TABLE_SETTINGS(char *message, int length);
	virtual int HandleMFS_ROOM_START_GAME(char *message, int length);
	virtual int HandleMFS_ROOM_STAND(char *message, int length);
	virtual int HandleMFS_ROOM_END_GAME(char *message, int length);
	virtual int HandleMFS_ROOM_EXIT(char *message, int length);
	virtual int HandleMFS_ROOM_DISPLAY_STRING(char *message, int length);
	virtual int HandleMFS_ROOM_CHANGE_SHADOW(char *message, int length);
	virtual int HandleMFS_ROOM_CHANGE_PROFILE(char *message, int length);
	virtual int HandleMFS_ROOM_REQUEST(char *message, int length);
	virtual int HandleMFS_ROOM_DISAPPROVE_REQUEST(char *message, int length);
	virtual int HandleMFS_ROOM_INVITE(char *message, int length);
	virtual int HandleMFS_ROOM_TI_CHECK_OUT_PAGE(char *message, int length);
	virtual int HandleMFS_ROOM_TI_ADD_TO_TEXT(char *message, int length);
	virtual int HandleMFS_ROOM_TI_JOIN(char *message, int length);
	virtual int HandleMFS_ROOM_TI_SET_BANK(char *message, int length);
	virtual int HandleMFS_ROOM_PLAYER_RATING(char *message, int length);

	virtual int HandleMFS_GAME_STATE(char *message, int length);
	virtual int HandleMFS_GAME_CHAT(char *message, int length);
	virtual int HandleMFS_GAME_BOOT(char *message, int length);
	virtual int HandleMFS_GAME_RESTART(char *message, int length);
	virtual int HandleMFS_GAME_DISPLAY_STRING(char *message, int length);
	
	void UpdateRequestOrInvitation();
	Boolean PlayerComesLaterInList(ROOMCLIENTPLAYER *a, ROOMCLIENTPLAYER *b);
	void ReorderPlayer(int n, Boolean initialize = FALSE);
	void GetTop10Players(PlayerNameRatings *prTop10);
	
	Boolean IsTournamentRoom() { return (roomFlags & ROOM_IS_TOURNAMENT_ROOM); }
	Boolean IsRatedRoom() { return (roomFlags & ROOM_IS_RATED_ROOM); }

	virtual void ShowModelessMessage(int viewNumber, char *text, char *caption, unsigned int uType, int purpose = DBP_UNKNOWN);
	virtual Boolean IsRestartAllowed() { return FALSE; }
	virtual Boolean HasRatings() { return FALSE; } // does this mean the same thing as HasRankings()??????
	virtual Boolean DimInactiveTable() { return FALSE; }
	virtual Boolean TablesHaveAlpha() { return FALSE; }
	
	int SendMessageToServer(char *message, int length);
	ROOMCLIENTPLAYER *GetPlayerFromID(int playerID);
	int GetPlayerIndexFromID(int playerID);
	int GetMyID();
	void ChatSoundKeyPressed(char c);
	void HoldMessages(int numTicks);
	Boolean HoldingMessages();
	void UpdateChatButtons(ROOMCLIENTPLAYER *player);
	
	virtual void InitTablesDisplaySeat(RoomTablesDisplaySeat *seat) = 0;
	virtual Boolean PlayerInSeatFacesRight(RoomTablesDisplaySeat *seat) = 0;
	virtual void PosnTablesDisplayTable(RoomTablesDisplayTable *table) = 0;
	virtual void DisplayTables(RoomTablesDisplay *tablesDisplay, int fromLineNum) = 0;
	
	virtual void ModifyInitialPlayerSettings() = 0;
	virtual void ClearSecretGameStateInformation(int mySeat, GameState *gameState) = 0;
	virtual Boolean HasRankings() = 0;
	virtual void GamePlaqueClicked(sciEvent &event, GamePlaque *plaque, GAMEDISPLAY *display, int seat) { }
	virtual void GameItemSelected(int itemID, int playerID, int seat, GAMEDISPLAY *display) { }
	virtual void PreferencesChanged();
	virtual void DoChatCommand(ChatLog *chatLog, char *c);
	
	int GetPlayerView(int playerNumber);
	int GetNumShadows();
	int GetPlayerViewCel(int playerNumber, Boolean facingWest);
	void GetMyFace(FaceInfo *faceInfo, char *path);
	int ReadAndSortFaceFiles();
	int CountFaceFiles(char *fileNameToFind);
	bool LoadNthFaceFile(int n, FaceInfo *faceInfo, char *fileName);
	bool ReadFaceFile(char *path, FaceInfo *faceInfo);
	Boolean AddPlayerPopupItems(int playerIndex);
	Boolean ClickedInTablesArea(sciEvent &event);
	void ShowStatus();
	void ShowDiagnostics();
	
	Boolean IsPlayerInINIGroup(char *login, char *group);
	void AddPlayerToINIGroup(char *login, char *group);
	void DeletePlayerFromINIGroup(char *login, char *group);
	Boolean IsPlayerFriend(char *login);
	Boolean IsPlayerMuted(char *login);
	void AddFriend(char *login);
	void AddFoe(char *login);
	void DeleteFriend(char *login);
	void DeleteFoe(char *login);
	
	void SetCurrentAd(WONadInfo *wonAdInfo);
	void SetGameAd(sciPlane *plane, int x, int y, int priority, int table);
	void ToggleAds();
	Boolean MouseIsOverAd();
	Boolean MouseIsOverChatSizeBar();
	
	sciList queue;
	
	RoomOverlayView *roomOverlay;

	RoomButton *wonButton;
	RoomButton *hoyleButton;
	RoomButton *optionsButton;
	RoomButton *exitButton;
	RoomButton *minimizeButton;
	RoomButton *sponsorshipButton;
	RoomButton *tournamentInformationButton;
	RoomButton *showGameButton;
	sciText *playersInRoomLabel;
	OptionsDialogBox *optionsDialog;
	ProfileDialogBox *profileDialog;
	ComplaintDialogBox *complaintDialog;
	InstructionsDialogBox *instructionsDialog;
	TournamentDialogBox *tournamentDialog;
	RoomDialogBox *roomMessageBox;
	RoomPopup *popup;
	int lastOptionsDialogVersion;
	
	sciButtonGroup *chatGroup;
	RoomRadioButton *chatRadioButtons[3];
	RoomButton *chatPushButtons[3];
	sciText *privateChatCount;
//	sciView *chatButtonWells[3];
	RoomChatEditBox *roomChatEditBox;
	Boolean commandLineMode;
	CDirtyWordFilter *dirtyWordFilter;
	
	ChatScrollingDisplay *chatDisplay;
	RoomPlayersDisplay *playersDisplay;
	RoomTablesDisplay *tablesDisplay;
	
	ClientScrollBox *chatScrollBox;
	ClientScrollBox *playersScrollBox;
	ClientScrollBox *tablesScrollBox;
	
	sciView *playerListHeadings[3];
	sciRect *playerListRect;
	
	int currentAdType;
	WONadInfo *currentAdInfo;
	GIFAd *gifAd;
	JPEGAd *jpegAd;
	GIFAd *gifGameAd;
	JPEGAd *jpegGameAd;
	sciPlane *gameAdPlane;
	int gameAdX;
	int gameAdY;
	int gameAdPri;
	int gameAdTable;
	
	RSVPPlane *rsvpPlane;
	
	int status;
	sciText *statusText;
	
	int roomFlags; // stored in the ROOM_*** #defines in hwmessages.h
	TournamentInformation tournamentInformation;
	
	sciText *diagnostics;
	
	IniFile *globalIniFile;
	
	int holdTicks;
	int numHoldTicks;
	
	// requests/invitations
	
	int requestingTable; // the table I've requested to play at (-1 = no request)
	
	int requesterPlayerID; // the player who has asked to join my table
	int requestedToWatch; // TRUE is the player asked to watch, FALSE if the player asked to play
	
	int inviterTable; // the table I've been invited to play at (-1 = no invitation)
	int inviterPlayerID; // the player who invited me
	Boolean invitedToWatch; // TRUE if I've been invited to watch; FALSE if I've been invited to play
	
	// core
	
	int numPlayersPromised;
	int numPlayers;
	int numTables;
	HCONNECTION server;
	RoomClientConnections *connections;
	ROOMCLIENTPLAYER *players;
	ROOMCLIENTTABLE *tables;

	// Server AI rating info.
	int m_iAIRatingWins;
	int m_iAIRatingLosses;
	int m_iAIRatingDraws;
	int m_iAIRatingPartials;
	int m_iAIRating;
	int m_iAIRatingDev;
};

extern ROOMCLIENT *roomClient;
extern Boolean gbShowReadyForMessages;
#endif
