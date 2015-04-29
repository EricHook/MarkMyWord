#ifndef _doublecrossclient
#define _doublecrossclient

#include "DoubleCrossMessage.h"

#include "Room Client.h"
#include "DoubleCrossEngine.h"

#define ENTIRE_TABLE_OFFSET 0

#define NUM_TILE_VIEWS (DBLCROSS_BOARDWIDTH * DBLCROSS_BOARDHEIGHT + MAX_GAME_PLAYERS * (DBLCROSS_RACKSIZE + DBLCROSS_BOMBRACKSIZE))

#define	DBLCROSS_BRDLEFT			139
#define	DBLCROSS_BRDTOP				42
#define DBLCROSS_SQSIZE				24
#define DBLCROSS_SCOREDIGITWIDTH	15

class DoubleCrossEngine;
class DoubleCrossTile;

enum { DCBID_PASS = 0, DCBID_NEWLETTERS }; // button IDs

//////////////////

class DoubleCrossControls;

class DCCheckBox : public sciCheckBox
{
public:
	DCCheckBox::DCCheckBox() { controls = NULL; }
	void UpdateCel();
	
	DoubleCrossControls *controls;
};

class DoubleCrossControls : public sciObject
{
public:
	DoubleCrossControls();
	~DoubleCrossControls();
	
	void Init(sciPlane *thePlane);
	void PrepareToShow(int table, int version, Boolean gameHasStarted, Boolean inGame, Boolean watcher);
	void Show(int table, int version, Boolean gameHasStarted, Boolean inGame, Boolean watcher);
	Boolean DoClickOK(int table, int *values);
	
	DCCheckBox *MakeCheckBox(sciPlane *plane, char *title, int x, int y);
	void CheckCheckBox(DCCheckBox *c, Boolean selected);
	
	// personal settings
	
	DCCheckBox *verbose;
	
	// table settings
	
	sciText *gameOptionsLabel;
	DCCheckBox *haveColoredBombs;
	DCCheckBox *haveDirectionalBombs;
	DCCheckBox *haveBonusTiles;
	DCCheckBox *haveBlockingTiles;
	DCCheckBox *gridMode;
	DCCheckBox *haveMysteryTiles;
	DCCheckBox *allowOffensiveWords;
	sciText *secondsLabel;
	EditBox *secondsEditBox;
	sciText *minWordSizeTitle;
	sciButtonGroup *minWordSizeGroup;
	sciRadioButton *minWordSize[2];
	
	sciPlane *plane;
};

//////////////////

class DoubleCrossTableDisplay : public sciObject
{
public:
	DoubleCrossTableDisplay();
	~DoubleCrossTableDisplay();
	
	void Init();
	void Posn(int newX, int newY);
	void Show(int theTable);
	
	sciView *centerpiece;
};

//////////////////

class DoubleCrossEventScript : public sciScript
{
public:
	DoubleCrossEventScript(DoubleCrossClientTable *theTable, MFS_DOUBLECROSS_EVENT_MESSAGE *theMFS, char *theComment = NULL);
	~DoubleCrossEventScript();
	
	char comment[256];
	MFS_DOUBLECROSS_EVENT_MESSAGE mfs;
	
	DoubleCrossClientTable *table;
};

class DoubleCrossMoveTileScript : public DoubleCrossEventScript
{
public:
	DoubleCrossMoveTileScript(DoubleCrossClientTable *theTable, MFS_DOUBLECROSS_EVENT_MESSAGE *theMFS, char *theComment = NULL)
		: DoubleCrossEventScript(theTable, theMFS, theComment) { }
	
	void ChangeState(int newState);
};

class DoubleCrossWordMadeScript : public DoubleCrossEventScript
{
public:
	DoubleCrossWordMadeScript(DoubleCrossClientTable *theTable, MFS_DOUBLECROSS_EVENT_MESSAGE *theMFS, char *theComment = NULL)
		: DoubleCrossEventScript(theTable, theMFS, theComment) { }
	
	void ChangeState(int newState);
};

class DoubleCrossTileStabilizedScript : public DoubleCrossEventScript
{
public:
	DoubleCrossTileStabilizedScript(DoubleCrossClientTable *theTable, MFS_DOUBLECROSS_EVENT_MESSAGE *theMFS, char *theComment = NULL)
		: DoubleCrossEventScript(theTable, theMFS, theComment) { }
	
	void ChangeState(int newState);
};

class DoubleCrossTileExplodedScript : public DoubleCrossEventScript
{
public:
	DoubleCrossTileExplodedScript(DoubleCrossClientTable *theTable, MFS_DOUBLECROSS_EVENT_MESSAGE *theMFS, char *theComment = NULL)
		: DoubleCrossEventScript(theTable, theMFS, theComment) { }
	
	void ChangeState(int newState);
	
	DoubleCrossTile *tile;
	sciView *explosion;
};

class DoubleCrossBonusScript : public DoubleCrossEventScript
{
public:
	DoubleCrossBonusScript(DoubleCrossClientTable *theTable, MFS_DOUBLECROSS_EVENT_MESSAGE *theMFS, char *theComment = NULL)
		: DoubleCrossEventScript(theTable, theMFS, theComment) { }
	
	void ChangeState(int newState);
	
	DoubleCrossTile *tile;
	int bonus;
};

class DoubleCrossColorBombScript : public DoubleCrossEventScript
{
public:
	DoubleCrossColorBombScript(DoubleCrossClientTable *theTable, MFS_DOUBLECROSS_EVENT_MESSAGE *theMFS, char *theComment = NULL)
		: DoubleCrossEventScript(theTable, theMFS, theComment) { }
	
	void ChangeState(int newState);
	void CrackTile(DCPoint square);
	
	sciView *shrapnelViews[DBLCROSS_BOARDWIDTH + DBLCROSS_BOARDHEIGHT - 1];
	
	int factor;	
};

class DoubleCrossPickUpBombScript : public DoubleCrossEventScript
{
public:
	DoubleCrossPickUpBombScript(DoubleCrossClientTable *theTable, MFS_DOUBLECROSS_EVENT_MESSAGE *theMFS, char *theComment = NULL,
		DoubleCrossTile *theBomb = NULL);
	
	void ChangeState(int newState);
	
	DoubleCrossTile *bomb;
};

class DoubleCrossDirExplosionScript : public DoubleCrossEventScript
{
public:
	DoubleCrossDirExplosionScript(DoubleCrossClientTable *theTable, MFS_DOUBLECROSS_EVENT_MESSAGE *theMFS, char *theComment = NULL)
		: DoubleCrossEventScript(theTable, theMFS, theComment) { }
	
	void ChangeState(int newState);
	int CountList(int ind);
	bool ShiftList(int whichList);

	
	sciView *shrapnelViews[4][5];	
	sciView *explosion;
	DoubleCrossTile *bomb;
	
	bool isVert;
	bool isHorz;
	int factor;
	int doneDir[4];
};

class DoubleCrossMysteryScript : public DoubleCrossEventScript
{
public:
	DoubleCrossMysteryScript(DoubleCrossClientTable *theTable, MFS_DOUBLECROSS_EVENT_MESSAGE *theMFS, char *theComment = NULL)
		: DoubleCrossEventScript(theTable, theMFS, theComment) { }
	
	void ChangeState(int newState);
	void CrackAllTiles(int owner);
	void PlayMysteryTune(int mysteryValue);
	
	DoubleCrossTile *mysteryTile;	
	sciView *blackHole;
	sciView *explosion;
};

class DoubleCrossFillRackScript : public DoubleCrossEventScript
{
public:
	DoubleCrossFillRackScript(DoubleCrossClientTable *theTable, MFS_DOUBLECROSS_EVENT_MESSAGE *theMFS, char *theComment = NULL)
		: DoubleCrossEventScript(theTable, theMFS, theComment) { }
	
	void ChangeState(int newState);
};

class DoubleCrossShowdownScript : public sciScript
{
public:
	DoubleCrossShowdownScript(DoubleCrossClientTable *theTable, MFS_DOUBLECROSS_REQUESTSHOWDOWN_MESSAGE *theMfs);

	void ChangeState(int newState);
	
	MFS_DOUBLECROSS_REQUESTSHOWDOWN_MESSAGE *mfs;
	
	DoubleCrossClientTable *table;
	
	DCPoint curSpot;
	int curRackSpot;
	int curPlayer;
	DoubleCrossTile *curTile;	
};

//////////////////

class DoubleCrossShowdownOKButton;

class DoubleCrossShowdownDialog : public sciDialogBox
{
public:
	DoubleCrossShowdownDialog(sciPlane *parentPlane, DoubleCrossClientTable *theTable);
	~DoubleCrossShowdownDialog();
	
	void Init();
	void Show();
	void Hide();
	
	void GetNames(char names[MAX_GAME_PLAYERS][MAX_PLAYER_NAME]);
	void GetScores(int scores[MAX_GAME_PLAYERS]);
	
	sciText *lines[MAX_GAME_PLAYERS];
	DoubleCrossShowdownOKButton *okButton;
	
	DoubleCrossClientTable *table;
};

class DoubleCrossShowdownOKButton : public sciButton
{
public:
	DoubleCrossShowdownOKButton(DoubleCrossShowdownDialog *theDialog) { dialog = theDialog; }
	~DoubleCrossShowdownOKButton() { }
	
	void DoClick(Boolean netMessage = FALSE);
	
	DoubleCrossShowdownDialog *dialog;
};

class DoubleCrossDefinitionOKButton;

class DoubleCrossDefinitionDialog : public sciDialogBox
{
public:
	DoubleCrossDefinitionDialog(sciPlane *parentPlane);
	~DoubleCrossDefinitionDialog();
	
	void Init();
	void Show(char *word, char *definition);
	void Hide();
	
	sciText *text;
	sciText *credits;
	DoubleCrossDefinitionOKButton *okButton;
};

class DoubleCrossDefinitionOKButton : public sciButton
{
public:
	DoubleCrossDefinitionOKButton(DoubleCrossDefinitionDialog *theDialog) { dialog = theDialog; }
	~DoubleCrossDefinitionOKButton() { }
	
	void DoClick(Boolean netMessage = FALSE);
	
	DoubleCrossDefinitionDialog *dialog;
};

//////////////////

class DoubleCrossTile : public sciActor
{
public:
	DoubleCrossTile(DoubleCrossClientTable *theTable, int theIndex);
	~DoubleCrossTile();
	void Init();
	void Show(int theOwner, char theTileValue, int theRackSpot, bool theStable, int theCracked, DCPoint theBoardSpot, bool theHighlight = false);
	void ShowBonus(int bonus, BYTE alpha);
	void Hide();
	void Posn(int newX, int newY);
	void SetPri(int newPri);
	
	bool InRack() { return rackSpot >= 0; }
	
	Boolean HandleEvent(sciEvent &event);
	
	int owner;
	char tileValue;
	int rackSpot;
	DCPoint boardSpot;
	bool stable;
	int cracked;
	
	int xOffset; // used for dragging the tile
	int yOffset;
	
	Boolean mouseDown;
	int index;
	sciView *letterView;
	sciView *crackView;
	
	sciView *bonusViews[3];
	
	DoubleCrossClientTable *table;
};

class DoubleCrossCommentsDisplay : public sciFeature
{
public:
	DoubleCrossCommentsDisplay(DoubleCrossClientTable *theTable);
	~DoubleCrossCommentsDisplay();
	
	void Init();
	void Show();
	void Hide();
	
	void AddComment(int eventType, char *line, int mysteryType = 0);
	
	void SetDoubler(bool isActive);
	void ClearComments();
	void SetLine(int which, char *text, int priority);
	short GetEventPriority(int eventType, int mysteryType);
	
	// data
	
	sciText	*lines[2];
	bool	specState;		// if true, we are showing special info
	bool	bombState;		// true if a bomb is detonating
	int     priority[2];	// The priority levels of each line
	sciView *doubler;		// comes on when doubler is active
	
	DoubleCrossClientTable *table;
};

class DoubleCrossScoreDisplay : public sciFeature
{
public:
	DoubleCrossScoreDisplay();
	~DoubleCrossScoreDisplay();
	
	void Init();
	void Show(int score);
	void Hide();
	
	// data
	
	sciView *digits[4];
};

class DoubleCrossTimerDisplay : public sciProp
{
public:
	DoubleCrossTimerDisplay(DoubleCrossClientTable *theTable);
	~DoubleCrossTimerDisplay();
	
	void Init();
	void Show(int seconds);
	void Hide();
	void Doit();
	
	sciView *digits[2];
	
	DoubleCrossClientTable *table;
};

class DoubleCrossButton : public sciButton
{
public:
	DoubleCrossButton(DoubleCrossClientTable *theTable, int theID);
	~DoubleCrossButton() { }
	
	void DoClick(Boolean netMessage = FALSE);
	
	int id;
	
	DoubleCrossClientTable *table;
};

class DoubleCrossBoardDisplay : public sciFeature
{
public:
	DoubleCrossBoardDisplay(DoubleCrossClientTable *theTable);
	~DoubleCrossBoardDisplay();
	
	Boolean HandleEvent(sciEvent &event);
	
	DoubleCrossClientTable *table;
};

class DoubleCrossDisplay : public GameDisplay
{
public:
	DoubleCrossDisplay();
	~DoubleCrossDisplay();
	void Init(int newTable);
	Boolean DisplayPlayer(GameState *gameState, int seat);
	void DisplayElements(DoubleCrossClientTable *t, bool myTurn, bool canDump);
	void Hide();
	void ShowTiles(DoubleCrossClientTable *t);
	void ShowButtons(DoubleCrossClientTable *t, bool myTurn, bool canDump);
	DoubleCrossTile *GetTileByBoardSpot(DCPoint spot);
	DoubleCrossTile *GetTileByRackSpot(int rackSpot, int owner);
	int GetTileCrackedState(DCPoint square);
	void CrackTile(int crackCel, DCPoint square);
	void CircleEvaluatedWords(DoubleCrossClientTable *t, DCPoint square);
	void ClearEvaluatedWords();
	
	DoubleCrossTile *tiles[NUM_TILE_VIEWS];
	DoubleCrossTimerDisplay *timerDisplay;
	DoubleCrossButton *passButton;
	DoubleCrossButton *newLettersButton;
	DoubleCrossCommentsDisplay *commentsDisplay;
	DoubleCrossScoreDisplay *statusWordSize;
	DoubleCrossScoreDisplay *statusNumTiles;
	DoubleCrossBoardDisplay *boardDisplay;
	
	sciView *evaluated[2][DBLCROSS_BOARDWIDTH];
	DCPoint evalOrigin;
	DoubleCrossTile *highlightedTile;

	char crackedTiles[DBLCROSS_BOARDHEIGHT][DBLCROSS_BOARDWIDTH];
};

class DoubleCrossTV : public sciProp
{
public:
	DoubleCrossTV(DoubleCrossDisplay *theDisplay, int theSeat);
	~DoubleCrossTV();
	
	Boolean HandleEvent(sciEvent &event);
	
	int seat;
	DoubleCrossDisplay *display;
};

class DoubleCrossCharacter : public GameCharacter
{
public:
	DoubleCrossCharacter(DoubleCrossDisplay *theDisplay);
	~DoubleCrossCharacter();
	void Init(int newTable, int newSeat);
	void Show(Boolean showAsCurrentPlayer);
	void Hide(Boolean onlyShadow = FALSE);
	
	DoubleCrossScoreDisplay *scoreDisplay;
	DoubleCrossTV *tvScreen;
	sciProp *tvOverlay;
};

class DoubleCrossWatcher : public GameWatcher
{
public:
	DoubleCrossWatcher(DoubleCrossDisplay *theDisplay) : GameWatcher(theDisplay) { }
	~DoubleCrossWatcher() { }
};

class DoubleCrossWatchersDisplay : public GameWatchersDisplay
{
public:
	DoubleCrossWatchersDisplay(int newTable, DoubleCrossDisplay *theDisplay) : GameWatchersDisplay(newTable, theDisplay) { }
	~DoubleCrossWatchersDisplay() { }
};

class DoubleCrossGamePlane : public GamePlane
{
public:
	DoubleCrossGamePlane(Boolean newModal, sciPlane *newParentPlane, DoubleCrossClientTable *newOwner) : GamePlane(newModal, newParentPlane, newOwner) { }
	~DoubleCrossGamePlane() { }
};

class DoubleCrossClientTable : public RoomClientTable
{
public:
	DoubleCrossClientTable();
	~DoubleCrossClientTable();
	void Init(int theIndex);
	void InitPlayer(int seat);
	void FillTableGameValues(int *values);
	void GetDescription(char *s);
	
	int GetSecondsRemaining();
	
	void HandleRESETGAME(MFS_DOUBLECROSS_RESETGAME_MESSAGE *mfs);
	void FinishREQUESTREPLACEPLAYER(int seat, int type);
	void HandleREQUESTPLAY(MFS_DOUBLECROSS_REQUESTPLAY_MESSAGE *mfs);
	void FinishREQUESTPLAY(char playType, char tileValue, char rackSpot, DCPoint square);
	void HandleEVENT(MFS_DOUBLECROSS_EVENT_MESSAGE *mfs);
	void HandleEVENTSDONE(MFS_DOUBLECROSS_EVENTS_DONE_MESSAGE *mfs);
	void HandleDEFINITION(MFS_DOUBLECROSS_DEFINITION_MESSAGE *mfs);
	void HandleREQUESTSHOWDOWN(MFS_DOUBLECROSS_REQUESTSHOWDOWN_MESSAGE *mfs);
	void FinishREQUESTSHOWDOWN();
	int HandleMessage(MFS_GAME_MESSAGE_HEADER *mfs);
	
	void Clear();
	void Display();
	void DisplayForNewGameState();
	
	POINT RackToScreenCoordinates(int seat, int rackIndex, bool bomb);
	POINT BoardToScreenCoordinates(DCPoint square);
	bool ScreenToBoardCoordinates(POINT p, DCPoint *square);
	
	Boolean playedTenSecondWarningSound;
	Boolean playedOutOfTimeSound;
	DoubleCrossEngine *engine;
	DoubleCrossShowdownDialog *showdownDialog;
	DoubleCrossDefinitionDialog *definitionDialog;
	
	bool enableActions;
	bool dontDisplay;
};

class DoubleCrossClientPlayer : public RoomClientPlayer
{
public:
	DoubleCrossClientPlayer() { }
	~DoubleCrossClientPlayer() { }
};

class DoubleCrossClient : public RoomClient
{
public:
	DoubleCrossClient() : RoomClient() { }
	~DoubleCrossClient() { }
	int Init();
	
	void ModifyInitialPlayerSettings() { }
	void ClearSecretGameStateInformation(int mySeat, GameState *gameState) { }
	Boolean HasRankings() { return FALSE; }
	Boolean IsRestartAllowed() { return TRUE; }
	
	void GameItemSelected(int itemID, int playerID, int seat, DoubleCrossDisplay *display);
	
	void InitTablesDisplaySeat(RoomTablesDisplaySeat *seat);
	Boolean PlayerInSeatFacesRight(RoomTablesDisplaySeat *seat);
	void PosnTablesDisplayTable(RoomTablesDisplayTable *table);
	void DisplayTables(RoomTablesDisplay *tablesDisplay, int fromLineNum);
};

extern DoubleCrossClient *doublecrossClient;

#endif //_doublecrossclient
