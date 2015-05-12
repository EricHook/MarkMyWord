#ifndef _chat
#define _chat

#include "game.h"
#include "scibuttn.h"
#include "sciscrip.h"
#include "scitext.h"
#include "editbox.h"

#define MAX_PLAYER_NAME 32 // duplicated here because of include circularity

#define CHAT_LINE_HEIGHT 14
#define NUM_VISIBLE_SCROLLING_CHAT_LINES 7
#define MAX_TRANSPARENT_CHAT_LINES 25

#define CHAT_PLANE_SHIFT_X (-8)
#define CHAT_PLANE_SHIFT_Y (-307)

enum { CBO_NORTHEAST = 0, CBO_EAST, CBO_SOUTHEAST, CBO_SOUTHWEST, CBO_WEST, CBO_NORTHWEST, CBO_NONE };

enum { CHAT_BALLOONS_ONLY = 0, CHAT_LOG_ONLY, CHAT_BALLOONS_AND_LOG };
enum { CHAT_BALLOONS_SLOW = 0, CHAT_BALLOONS_MEDIUM, CHAT_BALLOONS_FAST };
enum { CHAT_LOG_FLOATING = 0, CHAT_LOG_TRANSPARENT };
enum { CHAT_LOG_DARK = 0, CHAT_LOG_BRIGHT };

class ChatLog;
class ClientScrollBox;
class PopupMenu;
class ChatPopup;

////////////////////////

class ChatDisplayLine : public sciObject
{
public:
	ChatDisplayLine();
	~ChatDisplayLine();
	void Init(sciPlane *plane);
	void SetFont(int newLabelFont, int newFont);
	void SetHeight(int newHeight);
	void Posn(int x, int y);
	void SetText(char *newLabel, char *newText, char red, char green, char blue);
	void Show();
	
	sciText *label;
	sciText *text;
};

struct ChatLine
{
	char label[MAX_PLAYER_NAME + 20]; // "Private from :"
	char red;
	char green;
	char blue;
	char text[256];
};

class ChatScrollingDisplay : public sciRect
{
public:
	ChatScrollingDisplay();
	~ChatScrollingDisplay();
	void Init(sciPlane *thePlane, ClientScrollBox *theScrollBox,
			  PopupMenu *thePopup, int thePopID, int newX, int newY);
	void AddAChatLine(char *label, char *text, char red, char green, char blue);
	void DisplayLines(int fromLineNum = -1);
	Boolean HandleEvent(sciEvent &event);
	void CopyToClipboard();
	
	ChatDisplayLine lengthChecker;
	ChatDisplayLine displayLines[NUM_VISIBLE_SCROLLING_CHAT_LINES];
	ChatLine lines[256];
	
	int numLines;
	ClientScrollBox *scrollBox;
	PopupMenu *popup;
	int popID;
};

class ChatPlaneEditBox : public EditBox
{
public:
	ChatPlaneEditBox(ChatLog *theOwner) { owner = theOwner; }
	~ChatPlaneEditBox() { }
	
	Boolean HandleEvent(sciEvent &event);
	
	ChatLog *owner;
};

class ChatPlane : public sciPlane
{
public:
	ChatPlane(Boolean newModal, sciPlane *newParentPlane, ChatLog *theOwner);
	~ChatPlane();
	void AttemptClosePlane();
	
	sciView *overlay;
	ClientScrollBox *chatScrollBox;
	ChatScrollingDisplay *chatDisplay;
	ChatPlaneEditBox *editBox;
	ChatPopup *popup;
	
	ChatLog *owner;
};

////////////////////////

class ChatBalloon : public sciProp
{
public:
	ChatBalloon(int x, int y, int orientation, sciPlane *newPlane);
	~ChatBalloon();
	Boolean HandleEvent(sciEvent &event);
	void Posn(int newX, int newY);
	void Show(char *s);
	void Hide();
	
	sciText *text;
	BYTE alpha;
	BYTE dAlpha;
	BYTE count;
	// char queue[5][256];
};

class ChatTransparentLine : public sciView
{
public:
	ChatTransparentLine(sciPlane *newPlane, int newTable);
	~ChatTransparentLine();
	void ShowChatLine();
	void HideChatLine();
	void PosnChatLine(int x, int y);
	Boolean HandleEvent(sciEvent &event);
	
	sciText *text;
	int table;
};

class ChatEditText : public sciText
{
public:
	ChatEditText();
	~ChatEditText();
	void Init();
	void PositionCaret();
	void SetText(char *newText);
	
	sciProp *caret;
};

typedef struct {
	int x;
	int y;
	int numLines;
} ChatSettings;

class ChatDragBar : public sciText
{
public:
	ChatDragBar(ChatLog *theChatLog);
	Boolean HandleEvent(sciEvent &event);
	
protected:
	ChatLog *chatLog;
	Boolean mouseDown;
};

class ChatSizeLine : public sciText
{
public:
	ChatSizeLine(ChatLog *theChatLog);
	Boolean HandleEvent(sciEvent &event);
	
	Boolean mouseDown;
	ChatLog *chatLog;
};

class ChatEditBox : public EditBox
{
public:
	ChatEditBox(ChatLog *theChatLog);
	void SetHotkey(char c);
	Boolean HandleEvent(sciEvent &event);
	
	char hotkey;
	ChatLog *chatLog;
};

class ChatLog : public sciButton
{
	friend class ChatDragBar;
	friend class ChatSizeLine;
	friend class ChatPlane;
	
public:
	ChatLog(sciPlane *plane, int newTable, int newSeat);
	~ChatLog();
	void Show();
	void Hide();
	void PosnChat(int x, int y);
	void SetNumLines(int n);
	void DoClick(BOOL netMessage = FALSE);
	Boolean HandleEvent(sciEvent &event);
	void AddAChatLine(char *label, char *str, char red, char green, char blue);
	void RemoveAChatLine();
	Boolean UserIsEditing() { return isUp; }
	void ReadChatSettings();
	void WriteChatSettings();
	ChatSizeLine *GetSizeLine() { return sizeLine; }
	void SetTextHotkey(char c) { text->SetHotkey(c); }
	
	Boolean IsChatLogHidden() { return isHidden; }
	
	ChatSettings chatSettings;
	// sciSound *chatSounds[NUM_CHAT_SOUNDS];
	
	ChatTransparentLine *chatTransparentLines[MAX_TRANSPARENT_CHAT_LINES];
	
	ChatPlane *chatPlane;
	
	int table;
	int seat;
	
protected:
	Boolean isUp;
	Boolean isHidden;
	int numLines;
	int nextLine;
	sciView *topPart;
	ChatEditBox *text;
	sciText *prompt;
	sciView *bottomPart;
	ChatDragBar *dragBar;
	ChatSizeLine *sizeLine;
};

class ChatScript : public sciScript
{
public:
	void ChangeState(int newState);
};

class ChatBalloonScript : public sciScript
{
public:
	void ChangeState(int newState);
};

#endif // _chat
