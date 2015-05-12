#if !defined(_scipopupmenu)
#define _scipopupmenu

#ifndef NOSCIPOPUPS

#include "scirect.h"
#include "scibuttn.h"

////////////////////DEFINES//////////////////////
#define MAX_MENU_ITEMS		100
#define ID_EMPTY			-1
#define ID_MENU_SEPARATOR	-2
#define ID_RESERVED			50000

////////////////////ENUMS////////////////////////
enum { POPUP_MENU = 0, POPUP_DROPLIST, POPUP_SCROLL_DROPLIST };
enum { DB_DOWN = 0, DB_UP, DB_LEFT, DB_RIGHT };


////////////////////STRUCTS//////////////////////
typedef struct {
	int id;
	char *title;
	int subMenuID;
	BOOL checked;
	BOOL disabled;
} PopUpMenuItemInfo;//hotkey would have been smart to add

typedef struct {
	int id;
	int itemWidth;
	int itemHeight;
	int fontNumber;
	int dir;
	POINT posn;

	PopUpMenuItemInfo items[MAX_MENU_ITEMS];
} PopUpMenuInfo;


////////////////////CLASSES//////////////////////
class sciPopUpButton;
class sciPopUpMenuItem;
class sciPopUpMenu : public sciPlane//sciRect
{
	public:
		sciPopUpMenu(int newType = POPUP_MENU, Boolean modal = FALSE);
		~sciPopUpMenu();
		Boolean HandleEvent(sciEvent &event);
		
		sciPopUpMenu *GetFirstMenu();
		sciPopUpMenuItem *GetItemWithThisHotKey(char c);
		sciPopUpMenuItem *GetItemEventIsOn(int x, int y, sciPlane *lastPlane);
		sciPopUpMenu *GetMenuEventIsOn(int x, int y, sciPlane *lastPlane);

		void UnHighlightAllItemsFromHereDown(sciPopUpMenuItem *exemptItem = NULL);
		
		void SetPopButton(sciPopUpButton *newPopUpButton);
		void Init(PopUpMenuInfo *info, BOOL subMenu = FALSE);
		void DrawMenu();
		virtual PopUpMenuInfo *GetMenu(int menuID) {return NULL;}

		virtual BOOL CanSelectThisItem(int itemID) { return TRUE; }
		virtual void Select(sciPopUpMenuItem *selectedItem);
		void Select(int itemID);
		virtual void DoSelect(int selectedID) {}

		int GetSelectedID();
		int GetHighlightedID();
		
		virtual void Show();
		void Hide();
		virtual void JustHideThisMenu();
		void SetPri(int newPri);
		void Posn(int x, int y);
		virtual BOOL OnMe(int x, int y, sciPlane *lastPlane);
		BOOL OnMe(int x, int y);
		sciPopUpMenuItem *At(int i);
		int Size();
		sciPopUpMenuItem *GetThisItem(int itemID);

		void CheckItemOn(int itemID);
		void CheckItemOff(int itemID);
		BOOL ItemIsChecked(int itemID);

		void EnableItem(int anItemID);
		void DisableItem(int anItemID);
		BOOL IsItemEnabled(int anItemID);

		void AddMenuItem(sciPopUpMenuItem *newItem);
		int GetNewID();
		void MakeNewMenuItem(char *newText, int fontNum, int newID = -1);

		virtual sciPopUpMenu *MakeSubPopUpMenu();

		sciRect				*menuRect;
		sciPopUpButton		*popUpButton;
		sciPopUpMenuItem	*subOwner;
		sciList				*menuList;
		sciRect				*rects[6];

		BOOL mouseDown;

		int id;
		int type;
		int numItems;
		int selectedItemID;
		int itemWidth, itemHeight;
};

class sciPopUpButton : public sciButton
{
	public:
		sciPopUpButton(int newID = 0);
		~sciPopUpButton();
		virtual void DoClick(BOOL netMessage = FALSE);
		void SetTitle(char *newTitle, int newFont);
		void Show();
		void Hide();
		int GetScreenX();
		int GetScreenY();

		int id;
		sciPopUpMenu *popUpMenu;
		sciText *title;
};

class sciPopUpMenuItem : public sciText
{
	public:
		sciPopUpMenuItem(int newID, sciPopUpMenu *newOwner);
		~sciPopUpMenuItem();
		
		void InitSubMenu(PopUpMenuInfo *info);
		void Hide();
		void Show();
		int GetID();
		void Posn(int x, int y);
		int GetScreenX();
		int GetScreenY();
		void SetPri(int newPri);
		void DrawSeparator();

		void Highlight();
		virtual void DoUnHighlight();
		virtual void DoClick();
		BOOL IsBackgroundColor(RGBQUAD color);
		void ShowSubMenu();
		void HideSubMenu();

		void SetHotKey(char c);

		void CheckOn();
		void CheckOff();

		void Enable();
		void Disable();
		BOOL IsEnabled();

		sciPopUpMenu	*owner;
		sciPopUpMenu	*subMenu;
		sciView			*subMenuArrow;
		sciView			*checkMark;
		sciProp			*delayProp;

		sciRect *separatorLine[2];

		int normalFont;
		int disabledFont;
		int highlightedFont;

		int id;
		int counter;
		BOOL mouseDown;
		BOOL enabled;
		char hotKey;
};

class DelayScript : public sciScript
{
	public:
		DelayScript(sciPopUpMenuItem *aMenuItem, sciScript *newCaller = NULL);
		void ChangeState(int newState);

		sciPopUpMenuItem *menuItem;
};

/*
Examples:
//1 Element Menu
PopUpMenuInfo menu[] = { 
	{id, elementWidth, elementHeight, fontNum, dirToPop(up/down), {xPos,yPos},
		{{elementID, "Element Text", subMenuID, isSelected, isDisabled},{-1,"",-1,0,0}}}
};

//PopUp Menu with 2 Elements
PopUpMenuInfo menu[] = {
	{1,90,14,37,0,{90,419},
		{{0,"Controls", -1,0,0},{1,"Players",-1,0,0},{-1,"",-1,0,0}}}
};


//PopUp Menu with 9 Elements, each with a subMenu 
PopUpMenuInfo menu[] = {
	{0,90,14,37,0,{32,321}, 
		{{0,"Video Poker", 100,0,0},{1,"Slots", 100,0,0},
		{2,"Roulette",100,0,0},{3,"Poker",100,0,0},
		{4,"Paigow Poker",100,0,0},{5,"Keno",100,0,0},
		{6,"Horse Racing",100,0,0},{7,"Craps",100,0,0},
		{8,"Blackjack",100,0,0},{-1,"",-1,0,0}}},

	{100,90,14,37,0,{-1,-1}, 
		{{0,"$5 Min.", -1,0,0},{1,"$25 Min.", -1,0,0},{2,"$100 Min.",-1,0,0},{3,"Tournament",-1,0,0},
		{-1,"",-1,0,0}}}

	//Termination Menu
	{-1,0,0,0,0,{-1,-1},{{0,"",-1,0,0}}}
};


class ADropBox : public sciDropBox
{
    public:
		ADropBox(int newID);
		~ADropBox();
		
		void DoSelect(int selectedID);
		PopUpMenuInfo *GetMenu(int menuID);
		PopUpMenuInfo *GetMenuInfo();

		void DoOneThing();
		void DoAnotherThing();
};

ADropBox::ADropBox(int newID)
	:sciDropBox(newID)
{
}
ADropBox::~ADropBox()
{
}
void ADropBox::DoSelect(int selectedID)
{
	switch(id)
	{
		case 0:
			DoOneThing();
			break;
		case 1:
			DoAnotherThing();
			break;
	}
}
PopUpMenuInfo *ADropBox::GetMenu(int menuID)
{
	int i = 0;

	while(menu[i].id != -1)
	{
		if(menu[i].id == menuID)
			return &menu[i];
		i++;
	}

	return NULL;
}
PopUpMenuInfo *ADropBox::GetMenuInfo()
{
	if(id == 1)
		return GetMenu(0);

	if(id == 2)
		return GetMenu(4);

	return NULL;
}
*/

#endif

#endif //_scipopupmenu
