#if !defined(_scidropbox)
#define _scidropbox

#include "sciscrol.h"
/*
////////////////////DEFINES//////////////////////
#define MAX_MENU_ITEMS		100
#define ID_EMPTY			-1
#define ID_MENU_SEPARATOR	-2
#define ID_RESERVED			50000


////////////////////ENUMS////////////////////////
enum { DB_POPUPMENU = 0, DB_DROPLIST };
enum { DB_DOWN = 0, DB_UP, DB_LEFT, DB_RIGHT };


////////////////////STRUCTS//////////////////////
typedef struct {
	int id;
	char *title;
	int subMenuID;
	BOOL selected;
	BOOL disabled;
} PopUpMenuItemInfo;

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

class ComboBoxScroll;
class sciPopUpMenu;
class sciPopUpMenuItem;
class sciDropBox : public sciButton
{
	public:
		sciDropBox(int newID = 0, int newType = DB_DROPLIST);
		~sciDropBox();

		void Show();
		void Hide();
		void SetTextToWidth(char *newText, int newWidth = -1);
		void SetTitle(char *newTitle, int newFont);
		void DoClick(BOOL netMessage = FALSE);
		void Select(int itemID);
		int SelectByText(char *text);
		int GetSelectedID();
		sciPopUpMenuItem *GetThisItem(int itemID);
		void RemoveItemWithThisString(char *itemString);
		BOOL IsPopped();
		virtual BOOL CanSelectThisItem(int itemID);
		virtual BOOL IsSelectedItemVisible();

		void InitMenu(PopUpMenuInfo *info, char reverseMenuItems = 0);
		void InitComboBox(PopUpMenuInfo *info, int newWidth, int newHeight);
		
		virtual PopUpMenuInfo *GetMenu(int menuID) {return NULL;}
		virtual void DoSelect(int selectedID) {}

		void EnableItem(int anItemID);
		void DisableItem(int anItemID);
		BOOL IsItemEnabled(int anItemID);

		sciPopUpMenu *popUpMenu;
		sciText	*title;

		int id;
		int type;
};

class ComboBoxScroll : public sciScrollBox
{
	public:
		ComboBoxScroll(sciPopUpMenu *newOwner);
		~ComboBoxScroll();

		BOOL OnMe(int x, int y);

		int GetNumItems();
		int GetMaxVisibleItems();

		void DoScroll(int direction, int numItems);
		void DoRefresh(int firstItem);

	protected:
		sciPopUpMenu *owner;
};

class sciPopoidMenu : public sciRect
{
	public:
		sciPopoidMenu();
		~sciPopoidMenu();
		
		void Init(PopUpMenuInfo *info, BOOL subMenu = FALSE);
		void DrawMenu();
		virtual PopUpMenuInfo *GetMenu(int menuID) {return NULL;}

		virtual BOOL CanSelectThisItem(int itemID) { return TRUE; }
		virtual void Select(sciPopUpMenuItem *selectedItem);
		void Select(int itemID);
		virtual void DoSelect(int selectedID) {}

		int GetSelectedID();
		
		void Show();
		void Hide();
		void JustHideThisMenu();
		void SetPri(int newPri);
		void Posn(int x, int y);
		BOOL OnMe(int x, int y);

		void EnableItem(int anItemID);
		void DisableItem(int anItemID);
		BOOL IsItemEnabled(int anItemID);

		void AddMenuItem(sciPopUpMenuItem *newItem);
		int GetNewID();
		void MakeNewMenuItem(char *newText, int fontNum, int newID = -1);

		sciPopUpMenuItem	*subOwner;
		sciList				*menuList;
		sciRect				*rects[6];

		int numItems;
		int selectedItemID;
		int itemWidth, itemHeight;
};

class sciPopUpMenu : public sciRect
{
	public:
		sciPopUpMenu(sciDropBox *newOwner);
		~sciPopUpMenu();
		void Init(PopUpMenuInfo *info, BOOL subMenu = FALSE, char reverseOrder = 0);
		void Init(PopUpMenuInfo *info, int newWidth, int newHeight);
		void DrawMenu();

		virtual void Select(sciPopUpMenuItem *selectedItem);
		void Select(int itemID);
		int GetSelectedID();
		int GetIDOfItemInListWithIndex(int index);
		BOOL IsSelectedItemVisible();
		sciPopUpMenuItem *GetSelectedItem();
		sciPopUpMenuItem *GetThisItem(int itemID);
		char *GetSelectedItemText();
		char *GetTextOfItemWithID(int itemID);
		char *GetTextOfItemInListWithIndex(int index);
		int GetHighlightedID();

		void Show();
		void Hide();
		void JustHideThisMenu();
		void SetPri(int newPri);
		void Posn(int x, int y);
		BOOL OnMe(int x, int y);

		void EnableItem(int anItemID);
		void DisableItem(int anItemID);
		BOOL IsItemEnabled(int anItemID);

		void SetMaxVisibleItems(int newMaxItems);
		void AddMenuItem(sciPopUpMenuItem *newItem);
		int GetNewID();
		void MakeNewMenuItem(char *newText, int fontNum, int newID = -1);
		void RemoveItemByString(char *itemString);
		void RemoveAllItems();

		void ScrollMenuItems(int direction, int numItemsToScroll);
		void RefreshList(int firstItem);

		sciDropBox			*owner;
		sciPopUpMenuItem	*subOwner;
		sciList				*menuList;
		ComboBoxScroll		*newScroll;
		sciRect				*rects[6];

		int numItems;
		int maxVisibleItems;
		int firstVisibleItem;
		int selectedItemID;
		int direction;
		int itemWidth, itemHeight;
		BOOL comboBox;
};

class sciPopUpMenuItem : public sciText
{
	public:
		sciPopUpMenuItem(int newID, sciPopoidMenu *newOwner);
		sciPopUpMenuItem(int newID, sciPopUpMenu *newOwner);
		~sciPopUpMenuItem();
		
		void InitSubMenu(PopUpMenuInfo *info);
		Boolean HandleEvent(sciEvent &event);
		BOOL OnMe(int x, int y);
		void Hide();
		void Show();
		int GetID();
		void Posn(int x, int y);
		void SetPri(int newPri);
		void DrawSeparator();

		virtual void DoHighlight();
		virtual void DoUnHighlight();
		virtual void DoClick();
		BOOL IsBackgroundColor(RGBQUAD color);
		void ShowSubMenu();
		void HideSubMenu();

		void Enable();
		void Disable();
		BOOL IsEnabled();

		sciPopoidMenu	*popoidOwner;
		sciPopUpMenu	*owner;
		sciPopUpMenu	*subMenu;
		sciView			*subMenuArrow;
		sciProp			*delayProp;

		sciRect *separatorLine[2];

		int normalFont;
		int disabledFont;
		int highlightedFont;

		int id;
		int counter;
		BOOL mouseDown;
		BOOL enabled;
};

class DelayScript : public sciScript
{
	public:
		DelayScript(sciPopUpMenuItem *aMenuItem, sciScript *newCaller = NULL);
		void ChangeState(int newState);

		sciPopUpMenuItem *menuItem;
};
*/
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

#endif //_scidropbox
