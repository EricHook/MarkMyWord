#include "os.h"
#include "globals.h"
#include "scidrpbx.h"
/*
RGBQUAD DEFAULT_BLACK = {0, 0, 0, 0};
RGBQUAD DEFAULT_WHITE = {255, 255, 255, 0};
RGBQUAD HIGHLIGHT = {102,0,51,0};//{130,5,5,0};//Blue

#define DEFAULT_FONT		37
#define DISABLED_FONT		48
#define HIGHLIGHTED_FONT	50//49White
#define DEFAULT_ARROW		1055
#define DISABLED_ARROW		1082
#define HIGHLIGHTED_ARROW	1084//1083White

/////////////////////////////////////////////////////////////////////
/////				sciDropBox FUNCTION DEFINITIONS				/////
/////////////////////////////////////////////////////////////////////
sciDropBox::sciDropBox(int newID, int newType)
{
	id			= newID;
	type		= newType;
	popUpMenu	= NULL;
	title		= NULL;
	SetTextJust(JUST_LEFT | JUST_VERTICAL);
	SetJustOffsets(5, 0);
}
sciDropBox::~sciDropBox()
{
	if(popUpMenu)	{delete popUpMenu; popUpMenu = NULL;}
	if(title)		{delete title; title = NULL;}
}

void sciDropBox::Show()
{
	sciButton::Show();

	if(title)
		title->Show();
}
void sciDropBox::Hide()
{
	sciButton::Hide();

	if(title)
		title->Hide();

	if(popUpMenu)
		popUpMenu->Hide();
}
void sciDropBox::SetTextToWidth(char *newText, int newWidth)
{
	SetText(newText);

	if(newWidth != -1)
	{
		//text->SetWidth(newWidth);
		//text->SetTextToWidth(newText);
	}
}
void sciDropBox::SetTitle(char *newTitle, int newFont)
{
	if(!title)
		title = new sciText();
	title->plane = plane;
	title->SetText(newTitle);
	title->SetFont(newFont);
	title->Init();
	title->Posn(GetX() + 4, GetY() - 14);
	title->SetPri(GetPri());
}
void sciDropBox::DoClick(BOOL netMessage)
{
	if(popUpMenu)
	{
		if(popUpMenu->IsHidden())
			popUpMenu->Show();
		else
			popUpMenu->Hide();
	}
}
int sciDropBox::SelectByText(char *text)
{
	int i;

	if(popUpMenu)
	{
		for(i = 0; i < popUpMenu->menuList->Size(); i++)
		{
			if(!strcmp(((sciPopUpMenuItem*)popUpMenu->menuList->At(i))->GetText(), text))
			{
				Select(((sciPopUpMenuItem*)popUpMenu->menuList->At(i))->GetID());
				return i;
			}
		}
	}

	return -1;
}
int sciDropBox::GetSelectedID()
{
	if(popUpMenu)
		return popUpMenu->GetSelectedID();
	return -1;
}
sciPopUpMenuItem *sciDropBox::GetThisItem(int itemID)
{
	if(popUpMenu)
		return popUpMenu->GetThisItem(itemID);
	return NULL;
}
void sciDropBox::RemoveItemWithThisString(char *itemString)
{
	if(popUpMenu)
		popUpMenu->RemoveItemByString(itemString);
}
void sciDropBox::Select(int itemID)
{
	if(popUpMenu)
		popUpMenu->Select(itemID);
}
BOOL sciDropBox::IsPopped()
{
	return (popUpMenu && popUpMenu->IsNotHidden());
}
BOOL sciDropBox::CanSelectThisItem(int itemID)
{
	if(popUpMenu->IsItemEnabled(itemID) &&
		(popUpMenu->GetSelectedID() != itemID || type == DB_POPUPMENU))
		return TRUE;
	return FALSE;
}
BOOL sciDropBox::IsSelectedItemVisible()
{
	if(popUpMenu)
		return popUpMenu->IsSelectedItemVisible();
	return FALSE;
}
void sciDropBox::InitMenu(PopUpMenuInfo *info, char reverseMenuItems)
{
	if(!popUpMenu)
		popUpMenu = new sciPopUpMenu(this);
	popUpMenu->Init(info, FALSE, reverseMenuItems);
}
void sciDropBox::InitComboBox(PopUpMenuInfo *info, int newWidth, int newHeight)
{
	if(!popUpMenu)
		popUpMenu = new sciPopUpMenu(this);
	popUpMenu->Init(info,newWidth,newHeight);
}

void sciDropBox::EnableItem(int anItemID)
{
	if(popUpMenu)
		popUpMenu->EnableItem(anItemID);
}
void sciDropBox::DisableItem(int anItemID)
{
	if(popUpMenu)
		popUpMenu->DisableItem(anItemID);
}
BOOL sciDropBox::IsItemEnabled(int anItemID)
{
	if(popUpMenu)
		return(popUpMenu->IsItemEnabled(anItemID));
	return FALSE;
}

/////////////////////////////////////////////////////////////////////
/////				ComboBoxScroll FUNCTION DEFINITIONS			/////
/////////////////////////////////////////////////////////////////////
ComboBoxScroll::ComboBoxScroll(sciPopUpMenu *newOwner) : sciScrollBox(GDefs::VERTICAL)
{
	owner = newOwner;
}

ComboBoxScroll::~ComboBoxScroll()
{
}
BOOL ComboBoxScroll::OnMe(int x, int y)
{
	if(sciScrollBox::OnMe(x,y))
		return TRUE;
	if(previousButton && previousButton->OnMe(x,y))
		return TRUE;
	if(nextButton && nextButton->OnMe(x,y))
		return TRUE;
	if(scroller && scroller->OnMe(x,y))
		return TRUE;
	return FALSE;
}
int ComboBoxScroll::GetNumItems()
{
	return(owner->menuList->Size());
}

int ComboBoxScroll::GetMaxVisibleItems()
{
	return(owner->maxVisibleItems);
}

void ComboBoxScroll::DoScroll(int direction, int numItems)
{
	owner->ScrollMenuItems(direction, numItems);
}

void ComboBoxScroll::DoRefresh(int firstItem)
{
	owner->RefreshList(firstItem);
}


/////////////////////////////////////////////////////////////////////
/////			sciPopUpMenu FUNCTION DEFINITIONS				/////
/////////////////////////////////////////////////////////////////////
sciPopUpMenu::sciPopUpMenu(sciDropBox *newOwner)
{
	int i;

	owner			= newOwner;
	plane			= owner->plane;
	selectedItemID	= -1;
	direction		= DB_DOWN;
	subOwner		= NULL;
	newScroll		= NULL;
	comboBox		= FALSE;

	if(owner->type == DB_POPUPMENU)
	{
		SetInnerColor(DEFAULT_GREY);
		SetOutlineColor(DEFAULT_GREY);
	}
	else
	{
		SetInnerColor(DEFAULT_GREY);
		SetOutlineColor(DEFAULT_BLACK);
	}

	for(i = 0; i < 6; i++)
		rects[i] = NULL;

	numItems			= 0;
	maxVisibleItems		= 0;
	firstVisibleItem	= 0;
	menuList = new sciList();
}

sciPopUpMenu::~sciPopUpMenu()
{
	int i;

	if(menuList)
	{
		sciPopUpMenuItem *tmpItem;
		while (menuList->Size()) {
			tmpItem = (sciPopUpMenuItem *)menuList->At(0);
			menuList->Delete(tmpItem);
			delete tmpItem;
		}
		delete menuList;
		menuList = NULL;
	}

	if(newScroll) {delete newScroll; newScroll = NULL;}

	for(i = 0; i < 6; i++)
		if(rects[i]) {delete rects[i]; rects[i] = NULL;}

	owner		= NULL;
	subOwner	= NULL;
}

void sciPopUpMenu::DrawMenu()
{
	int i;
	POINT offsets[6]	= { {1,1},{1,1},{2,2},{2,2},{3,3},{3,3} };
	POINT sizeReduct[6] = { {-1,-1},{-2,-2},{-3,-3},{-4,-4},{-5,-5},{-6,-6} };
	RGBQUAD colors[6] = { {80, 80, 80, 0}, {255, 255, 255, 0}, {80, 80, 80, 0}, 
						{255, 255, 255, 0}, {80, 80, 80, 0}, {221, 221, 221, 0}};
	
	for(i = 0; i < 6; i++)
	{
		if(!rects[i])
		{
			rects[i] = new sciRect();
			rects[i]->plane = plane;
			rects[i]->SetSize(GetWidth() + sizeReduct[i].x, GetHeight() + sizeReduct[i].y);
			rects[i]->Posn(GetX() + offsets[i].x, GetY() + offsets[i].y);
			rects[i]->Init();
			rects[i]->SetPri(GetPri() + 1 + i);
			rects[i]->SetInnerColor(colors[i]);
			rects[i]->SetOutlineColor(colors[i]);
		}
	}

//You'll need to have a window/dialog box larger than 6 pixels to start with and 
//3 colors: a main dialog color (1), a much darker color (2--not black), 
//and a much lighter color (3--white or almost white).

//With (0,0) being top left base coordinate where x = final width and y = final height, 

//� draw a rectangle using color 1 and the size you want 
//(I suggest beefing up whatever you already have by 6 pixels both ways to fit the same number of stuff in the existing dialogs) 
//� draw a rectangle using color 2 at (1,1) where the size is x - 1 pixel by y - 1 pixel
//� draw a rectangle using color 3 at (1,1) where the size is x - 2 pixels by y - 2 pixels
//� draw a rectangle using color 2 at (2,2) where the size is x - 3 pixels by y - 3 pixels
//� draw a rectangle using color 3 at (2,2) where the size is x - 4 pixels by y - 4 pixels
//� draw a rectangle using color 2 at (3,3) where the size is x - 5 pixels by y - 5 pixels
//� draw a rectangle using color 1 at (3,3) where the size is x - 6 pixels by y - 6 pixels
}

void sciPopUpMenu::Init(PopUpMenuInfo *info, BOOL subMenu, char reverseOrder)
{
	int i, tempY, tempHeight = 0;
	sciPopUpMenuItem *item;

	itemWidth	= info->itemWidth;
	itemHeight	= info->itemHeight;
	numItems	= 0;
	
	for(i = 0; i < MAX_MENU_ITEMS; i++)
	{
		if(info->items[i].id != -1)
		{
			numItems++;
			if(info->items[i].id == ID_MENU_SEPARATOR)
				tempHeight += itemHeight / 3;
			else
				tempHeight += itemHeight;
		}
		else
			break;
	}

	sciRect::Init();

	if(owner->type == DB_POPUPMENU)
		SetSize(itemWidth + 6, tempHeight + 6);//(itemHeight * numItems) + 6);
	else
		SetSize(itemWidth + 2, tempHeight + 2);//(itemHeight * numItems) + 2);

	if(subMenu)
		SetPri(subOwner->GetPri() + 50);//subOwner->owner->numItems + 1);
	else
		SetPri(owner->GetPri() + 1);

	if(subMenu)
		direction = subOwner->owner->direction;
	else
		direction = info->dir;

	if(info->posn.x == -1 && info->posn.y == -1)
	{
		if(subMenu)
		{
			if(direction == DB_DOWN)//info->dir == DB_DOWN)
				Posn(subOwner->GetX() + subOwner->GetWidth(), subOwner->GetY());
			else
			{
				if(subOwner->GetY() + GetHeight() < 480)
					Posn(subOwner->GetX() + subOwner->GetWidth(), subOwner->GetY());
				else
				{
					tempY = subOwner->GetY() + GetHeight() - 480;
					tempY = subOwner->GetY() - tempY;
					Posn(subOwner->GetX() + subOwner->GetWidth(),
						tempY);//subOwner->GetY() - ((numItems - 1) * subOwner->GetHeight()));
				}
			}
		}
		else
		{
			if(direction == DB_DOWN)
				Posn(owner->GetX(), owner->GetY() + owner->GetHeight());
			else
				Posn(owner->GetX(), owner->GetY() - (numItems * itemHeight));
		}
	}
	else
		Posn(info->posn.x,info->posn.y);

	if(owner->type == DB_POPUPMENU)
		DrawMenu();

	int itemIndex;
	for(i = 0; i < numItems; i++)
	{
		if(reverseOrder)
			itemIndex = numItems - 1 - i;
		else
			itemIndex = i;

		item = new sciPopUpMenuItem(info->items[itemIndex].id, this);
		item->plane = plane;
		//see if this item is a separator
		if(info->items[itemIndex].id == ID_MENU_SEPARATOR)
		{
			item->SetWidth(itemWidth);
			item->SetHeight(itemHeight / 3);
			item->DrawSeparator();
		}
		else
		{
			item->normalFont = info->fontNumber;
			item->SetFont(info->fontNumber);
			item->SetText(info->items[itemIndex].title);
			item->SetWidth(itemWidth);
			item->SetHeight(itemHeight);
		}
		item->Init();
		AddMenuItem(item);
		if(info->items[itemIndex].selected)
			Select(itemIndex);
		if(info->items[itemIndex].subMenuID != -1)
			item->InitSubMenu(owner->GetMenu(info->items[itemIndex].subMenuID));
	}
}

void sciPopUpMenu::Init(PopUpMenuInfo *info, int newWidth, int newHeight)
{
	int i;
	sciPopUpMenuItem *item;

	itemWidth	= info->itemWidth;
	itemHeight	= info->itemHeight;
	numItems	= 0;
	comboBox	= TRUE;
	
	for(i = 0; i < MAX_MENU_ITEMS; i++)
	{
		if(info->items[i].id != -1)
			numItems++;
		else
			break;
	}

	sciRect::Init();
	SetSize(newWidth, newHeight);
	SetPri(owner->GetPri() + 10);

	if(info->dir == DB_DOWN)
		Posn(owner->GetX(), owner->GetY() + owner->GetHeight());
	else
		Posn(owner->GetX(), owner->GetY() - (numItems * itemHeight));

	if (!newScroll) {
		newScroll = new ComboBoxScroll(this);
		newScroll->plane = plane;
		newScroll->Init();
		newScroll->SetViews(1036,1035,1037);
		newScroll->SetPri(GetPri() + 100);
	}
	newScroll->Posn(GetX() + newWidth, GetY());
	newScroll->SetSize(16, newHeight - 1);
	newScroll->Hide();

	for(i = 0; i < numItems; i++)
	{
		item = new sciPopUpMenuItem(info->items[i].id, this);
		item->plane = plane;
		if(info->items[i].id == ID_MENU_SEPARATOR)
		{
			item->SetWidth(itemWidth);
			item->SetHeight(itemHeight / 3);
			item->DrawSeparator();
		}
		else
		{
			item->normalFont = info->fontNumber;
			item->SetFont(info->fontNumber);
			item->SetText(info->items[i].title);
			item->SetWidth(itemWidth);
			item->SetHeight(itemHeight);
		}
		item->Init();
		AddMenuItem(item);
		if(info->items[i].selected)
			Select(i);
		if(info->items[i].subMenuID != -1)
			item->InitSubMenu(owner->GetMenu(info->items[i].subMenuID));
	}
}

void sciPopUpMenu::Select(sciPopUpMenuItem *selectedItem)
{
	if(owner->CanSelectThisItem(selectedItem->id))
	{
		Select(selectedItem->GetID());
		owner->DoSelect(selectedItemID);
	}

	Hide();
}
void sciPopUpMenu::Select(int itemID)
{
	if(itemID > -1)
	{
		selectedItemID = itemID;
		if(owner->type == DB_DROPLIST)
			owner->SetTextToWidth(GetSelectedItemText(), owner->GetWidth() - 20);
	}
}
int sciPopUpMenu::GetSelectedID()
{
	return selectedItemID;
}
int sciPopUpMenu::GetIDOfItemInListWithIndex(int index)
{
	if(menuList->Size() > index)
		return ((sciPopUpMenuItem *)menuList->At(index))->GetID();

	return -1;
}
BOOL sciPopUpMenu::IsSelectedItemVisible()
{
	int i;
	for(i = 0; i < menuList->Size(); ++i) 
	{
		if (((sciPopUpMenuItem *)menuList->At(i))->GetID() == GetSelectedID()) 
		{
			return(i >= firstVisibleItem && i < firstVisibleItem + maxVisibleItems);
		}
	}

	return FALSE;
}
sciPopUpMenuItem *sciPopUpMenu::GetSelectedItem()
{
	int i;

	for(i = 0; i < menuList->Size(); i++)
	{
		if(((sciPopUpMenuItem *)menuList->At(i))->GetID() == GetSelectedID())
			return (sciPopUpMenuItem *)menuList->At(i);
	}
	return NULL;
}
sciPopUpMenuItem *sciPopUpMenu::GetThisItem(int itemID)
{
	int i;
	sciPopUpMenuItem *item;

	for(i = 0; i < menuList->Size(); i++)
	{
		if(((sciPopUpMenuItem *)menuList->At(i))->GetID() == itemID)
			return (sciPopUpMenuItem *)menuList->At(i);
		if(((sciPopUpMenuItem *)menuList->At(i))->subMenu)
		{
			item = ((sciPopUpMenuItem *)menuList->At(i))->subMenu->GetThisItem(itemID);
			if(item)
				return item;
		}
	}

	return NULL;
}
char *sciPopUpMenu::GetSelectedItemText()
{
	int i;

	for(i = 0; i < menuList->Size(); i++)
	{
		if(((sciPopUpMenuItem *)menuList->At(i))->GetID() == GetSelectedID())
			return ((sciPopUpMenuItem *)menuList->At(i))->GetText();
	}
	return "";
}
char *sciPopUpMenu::GetTextOfItemWithID(int itemID)
{
	int i;

	for(i = 0; i < menuList->Size(); i++)
	{
		if(((sciPopUpMenuItem *)menuList->At(i))->GetID() == itemID)
			return ((sciPopUpMenuItem *)menuList->At(i))->GetText();
	}
	return "";
}
char *sciPopUpMenu::GetTextOfItemInListWithIndex(int index)
{
	if(menuList->Size() > index)
		return ((sciPopUpMenuItem *)menuList->At(index))->GetText();
	
	return "";
}
int sciPopUpMenu::GetHighlightedID()
{
	int i;

	for(i = 0; i < menuList->Size(); i++)
	{
		if(((sciPopUpMenuItem *)menuList->At(i))->IsBackgroundColor(HIGHLIGHT))
			return ((sciPopUpMenuItem *)menuList->At(i))->GetID();
	}

	return -1;
}
void sciPopUpMenu::Posn(int x, int y)
{
	sciRect::Posn(x,y);
	
	int i, tempY = GetY() + 1;

	if(menuList)
	{
		for(i = 0; i < menuList->Size(); i++)
		{
			if(menuList->At(i))
			{
				((sciPopUpMenuItem*)menuList->At(i))->Posn(x + 1, tempY);
				tempY += ((sciPopUpMenuItem*)menuList->At(i))->GetHeight();
			}
		}
	}
}
BOOL sciPopUpMenu::OnMe(int x, int y)
{
	int i;

	if(sciRect::OnMe(x,y))
		return TRUE;

	if(newScroll && newScroll->OnMe(x,y))
		return TRUE;

	for(i = 0; i < menuList->Size(); i++)
	{
		if(((sciPopUpMenuItem*)menuList->At(i))->OnMe(x,y))
			return TRUE;
	}
	
	return FALSE;
}
void sciPopUpMenu::SetPri(int newPri)
{
	sciRect::SetPri(newPri++);
	
	int i;

	for(i = 0; i < menuList->Size(); i++)
	{
		if(menuList->At(i))
			((sciPopUpMenuItem*)menuList->At(i))->SetPri(newPri++);
	}
}
void sciPopUpMenu::Show()
{
	int i;

	if(!numItems || !menuList->Size())
		return;

	sciRect::Show();
	if(rects[0])
	{
		for(i = 0; i < 6; i++)
			rects[i]->Show();
	}

	if(newScroll)
	{
		newScroll->Show();
		if(menuList->Size() > maxVisibleItems)
			newScroll->Enable();
	}

	if(comboBox)
	{
		if (owner->IsSelectedItemVisible()) {
			RefreshList(firstVisibleItem);
		} else {
			RefreshList(GetSelectedID());
		}
	}
	else
	{
		if(menuList)
		{
			for(i = 0; i < menuList->Size(); i++)
				((sciPopUpMenuItem*)menuList->At(i))->Show();
			return;
		}
	}
}
void sciPopUpMenu::Hide()
{
	JustHideThisMenu();

	if(subOwner && subOwner->owner && subOwner->owner->IsNotHidden())
		subOwner->owner->Hide();
}
void sciPopUpMenu::JustHideThisMenu()
{
	int i;

	sciRect::Hide();

	if(newScroll)
		newScroll->Hide();

	if(rects[0])
	{
		for(i = 0; i < 6; i++)
			rects[i]->Hide();
	}
	
	if(menuList)
	{
		for(i = 0; i < menuList->Size(); i++)
			((sciPopUpMenuItem*)menuList->At(i))->Hide();
	}
}

void sciPopUpMenu::EnableItem(int anItemID)
{
	int i;
	sciPopUpMenuItem *menuItem;
	for(i = 0; i < menuList->Size(); i++)
	{
		menuItem = (sciPopUpMenuItem*)menuList->At(i);
		if(menuItem->GetID() == anItemID)
		{
			menuItem->Enable();
			return;
		}
		if(menuItem->subMenu)
			menuItem->subMenu->EnableItem(anItemID);
	}
}
void sciPopUpMenu::DisableItem(int anItemID)
{
	int i;
	sciPopUpMenuItem *menuItem;
	for(i = 0; i < menuList->Size(); i++)
	{
		menuItem = (sciPopUpMenuItem*)menuList->At(i);
		if(menuItem->GetID() == anItemID)
		{
			menuItem->Disable();
			return;
		}
		if(menuItem->subMenu)
			menuItem->subMenu->DisableItem(anItemID);
	}
}
BOOL sciPopUpMenu::IsItemEnabled(int anItemID)
{
	int i;
	BOOL isEnabled = FALSE;
	sciPopUpMenuItem *menuItem;
	for(i = 0; i < menuList->Size(); i++)
	{
		menuItem = (sciPopUpMenuItem*)menuList->At(i);
		if(menuItem->GetID() == anItemID)
			return menuItem->IsEnabled();
		if(menuItem->subMenu)
		{
			isEnabled = menuItem->subMenu->IsItemEnabled(anItemID);
			if(isEnabled)
				return TRUE;
		}
	}
	return FALSE;
}

void sciPopUpMenu::SetMaxVisibleItems(int newMaxItems)
{
	maxVisibleItems = newMaxItems;
}
void sciPopUpMenu::AddMenuItem(sciPopUpMenuItem *newItem)
{
	int i, itemOffset = 0;

	for(i = 0; i < menuList->Size(); i++)
		itemOffset += ((sciPopUpMenuItem*)menuList->At(i))->GetHeight();

	if(owner->type == DB_POPUPMENU)
	{
		newItem->Posn(GetX() + 3, GetY() + 3 + itemOffset);
	}
	else
	{
		newItem->Posn(GetX() + 1, GetY() + 1 + itemOffset);
	}
	
	menuList->AddToBack(newItem);
	
	if(owner->type == DB_POPUPMENU)
		newItem->SetPri(GetPri() + menuList->Size() + 8);
	else
		newItem->SetPri(GetPri() + menuList->Size());
}
int sciPopUpMenu::GetNewID()
{
	int i;

	for(i = menuList->Size() - 1; i > -1; i--)
	{
		if(((sciPopUpMenuItem*)menuList->At(i))->GetID() >= ID_RESERVED)
			return ((sciPopUpMenuItem*)menuList->At(i))->GetID() + 1;
	}

	return ID_RESERVED;
}
void sciPopUpMenu::MakeNewMenuItem(char *newText, int fontNum, int newID)
{
	sciPopUpMenuItem *item;
	int i, tempHeight = 0;
	
	item = new sciPopUpMenuItem((newID == -1) ? GetNewID() : newID, this);
	item->plane = plane;
	if(item->GetID() == ID_MENU_SEPARATOR)
	{
		item->SetWidth(itemWidth);
		if(comboBox)
			item->SetHeight(itemHeight);
		else
			item->SetHeight(itemHeight / 3);
		item->DrawSeparator();
	}
	else
	{
		item->normalFont = fontNum;
		item->SetFont(item->normalFont);
		item->SetText(newText);
		item->SetWidth(itemWidth);
		item->SetHeight(itemHeight);
	}
	item->Init();
	AddMenuItem(item);

	numItems++;

	for(i = 0; i < menuList->Size(); i++)
		tempHeight += ((sciPopUpMenuItem *)menuList->At(i))->GetHeight();
	
	if(!comboBox)
	{
		if(owner->type == DB_POPUPMENU)
			SetSize(itemWidth + 6, tempHeight + 6);//(itemHeight * numItems) + 6);
		else
			SetSize(itemWidth + 2, tempHeight + 2);

		//SetSize(itemWidth + 2, tempHeight + 2);//(itemHeight * numItems) + 2);
	}
}

void sciPopUpMenu::RemoveItemByString(char *itemString)
{
	int i;
	sciPopUpMenuItem *tmpItem;
	int removedItemID = -1;
	int newID = ID_RESERVED;

	for(i = 0; i < menuList->Size(); i++)
	{
		tmpItem = (sciPopUpMenuItem *)menuList->At(i);
		if(!strcmp(itemString, tmpItem->GetText())) 
		{
			removedItemID = tmpItem->GetID();
			tmpItem->Hide();
			menuList->Delete(tmpItem);
			delete tmpItem;
			break;
		}
	}

	numItems = menuList->Size();

	// Reset all IDs that were given through dynamic addition to the menu list
	for(i = 0; i < menuList->Size(); i++)
	{
		if(((sciPopUpMenuItem *)menuList->At(i))->GetID() >= ID_RESERVED)
			((sciPopUpMenuItem *)menuList->At(i))->id = newID++;
	}

	if(comboBox && newScroll && maxVisibleItems >= numItems)
		newScroll->Disable();

	// Finally, refresh the list
	if(comboBox)
		RefreshList(firstVisibleItem);
}
void sciPopUpMenu::RemoveAllItems()
{
	sciPopUpMenuItem *tempItem;//tmpListText;
	
	if (!menuList)
		return;
	
	while (menuList->Size()) {
		tempItem = (sciPopUpMenuItem *)menuList->At(0);
		tempItem->Hide();
		menuList->Delete(tempItem);
		delete tempItem;
	}
	
	firstVisibleItem = 0;
	
	if(comboBox)
		RefreshList(firstVisibleItem);
}

void sciPopUpMenu::ScrollMenuItems(int direction, int numItemsToScroll)
{
	if (direction == GDefs::REVERSE) {
		RefreshList(firstVisibleItem - numItemsToScroll);
	} else if (direction == GDefs::FORWARD) {
		RefreshList(firstVisibleItem + numItemsToScroll);
	} else {
		assert(FALSE);
	}
}
void sciPopUpMenu::RefreshList(int firstItem)
{
	int iSize = menuList->Size();

	if(!comboBox)
		return;

	if (iSize) 
	{
		if (firstItem > iSize - maxVisibleItems) {
			firstItem = iSize - maxVisibleItems;
		}

		if (firstItem < 0) {
			firstVisibleItem = 0;
		} else {
			firstVisibleItem = firstItem;
		}

		int tempY = GetY() + 1;
		sciPopUpMenuItem *tmpText = (sciPopUpMenuItem *)menuList->At(0);
		//int itemHeight = tmpText->GetMinHeight();
		//itemHeight = 15;
		for (int i = 0; i < iSize; ++i) {
			tmpText = (sciPopUpMenuItem *)menuList->At(i);
			if (i < firstVisibleItem || i >= firstVisibleItem + maxVisibleItems) {
				if (tmpText->IsNotHidden()) {
					tmpText->Hide();
				}
			} else {
				tmpText->Posn(GetX() + 1, tempY);
				tempY += itemHeight;
				if(IsNotHidden())
					tmpText->Show();
			}
		}
	}

	newScroll->SnapThumbnail(firstVisibleItem);
}



/////////////////////////////////////////////////////////////////////
/////			sciPopoidMenu FUNCTION DEFINITIONS				/////
/////////////////////////////////////////////////////////////////////
sciPopoidMenu::sciPopoidMenu()
{
	int i;

	selectedItemID	= -1;
	subOwner		= NULL;

	SetInnerColor(DEFAULT_GREY);
	SetOutlineColor(DEFAULT_GREY);
	
	for(i = 0; i < 6; i++)
		rects[i] = NULL;

	numItems			= 0;
	menuList = new sciList();
}
sciPopoidMenu::~sciPopoidMenu()
{
	int i;

	if(menuList)
	{
		sciPopUpMenuItem *tmpItem;
		while (menuList->Size()) {
			tmpItem = (sciPopUpMenuItem *)menuList->At(0);
			menuList->Delete(tmpItem);
			delete tmpItem;
		}
		delete menuList;
		menuList = NULL;
	}

	for(i = 0; i < 6; i++)
		if(rects[i]) {delete rects[i]; rects[i] = NULL;}

	subOwner	= NULL;
}
		
void sciPopoidMenu::Init(PopUpMenuInfo *info, BOOL subMenu)
{
	int i, tempY, tempHeight = 0;
	sciPopUpMenuItem *item;

	itemWidth	= info->itemWidth;
	itemHeight	= info->itemHeight;
	numItems	= 0;
	
	for(i = 0; i < MAX_MENU_ITEMS; i++)
	{
		if(info->items[i].id != -1)
		{
			numItems++;
			if(info->items[i].id == ID_MENU_SEPARATOR)
				tempHeight += itemHeight / 3;
			else
				tempHeight += itemHeight;
		}
		else
			break;
	}

	sciRect::Init();

	SetSize(itemWidth + 6, tempHeight + 6);//(itemHeight * numItems) + 6);

	if(subMenu)
		SetPri(subOwner->GetPri() + 50);//subOwner->owner->numItems + 1);
	else
		SetPri(10000);

	if(info->posn.x != -1 || info->posn.y != -1)
		Posn(info->posn.x,info->posn.y);

	for(i = 0; i < numItems; i++)
	{
		item = new sciPopUpMenuItem(info->items[i].id, this);
		//see if this item is a separator
		if(info->items[i].id == ID_MENU_SEPARATOR)
		{
			item->SetWidth(itemWidth);
			item->SetHeight(itemHeight / 3);
			item->DrawSeparator();
		}
		else
		{
			item->normalFont = info->fontNumber;
			item->SetFont(info->fontNumber);
			item->SetText(info->items[i].title);
			item->SetWidth(itemWidth);
			item->SetHeight(itemHeight);
		}
		item->Init();
		AddMenuItem(item);
		if(info->items[i].selected)
			Select(i);
		if(info->items[i].subMenuID != -1)
			item->InitSubMenu(GetMenu(info->items[i].subMenuID));
	}
}
void sciPopoidMenu::DrawMenu()
{
	int i;
	POINT offsets[6]	= { {1,1},{1,1},{2,2},{2,2},{3,3},{3,3} };
	POINT sizeReduct[6] = { {-1,-1},{-2,-2},{-3,-3},{-4,-4},{-5,-5},{-6,-6} };
	RGBQUAD colors[6] = { {80, 80, 80, 0}, {255, 255, 255, 0}, {80, 80, 80, 0}, 
						{255, 255, 255, 0}, {80, 80, 80, 0}, {221, 221, 221, 0}};
	
	for(i = 0; i < 6; i++)
	{
		if(!rects[i])
		{
			rects[i] = new sciRect();
			rects[i]->plane = plane;
			rects[i]->SetSize(GetWidth() + sizeReduct[i].x, GetHeight() + sizeReduct[i].y);
			rects[i]->Posn(GetX() + offsets[i].x, GetY() + offsets[i].y);
			rects[i]->Init();
			rects[i]->SetPri(GetPri() + 1 + i);
			rects[i]->SetInnerColor(colors[i]);
			rects[i]->SetOutlineColor(colors[i]);
		}
		else
		{
			rects[i]->Posn(GetX() + offsets[i].x, GetY() + offsets[i].y);
			rects[i]->SetPri(GetPri() + 1 + i);
		}
	}
}

void sciPopoidMenu::Select(sciPopUpMenuItem *selectedItem)
{
	Hide();

	if(CanSelectThisItem(selectedItem->id))
	{
		Select(selectedItem->GetID());
		DoSelect(GetSelectedID());
	}
}
void sciPopoidMenu::Select(int itemID)
{
	if(itemID > -1)
		selectedItemID = itemID;
}
int sciPopoidMenu::GetSelectedID()
{
	return selectedItemID;
}

void sciPopoidMenu::Posn(int x, int y)
{
	int i, tempY;
	POINT offsets[6] = { {1,1},{1,1},{2,2},{2,2},{3,3},{3,3} };
	
	sciRect::Posn(x,y);
	
	for(i = 0; i < 6; i++)
	{
		if(rects[i])
			rects[i]->Posn(GetX() + offsets[i].x, GetY() + offsets[i].y);
	}

	tempY = GetY() + 3;

	if(menuList)
	{
		for(i = 0; i < menuList->Size(); i++)
		{
			if(menuList->At(i))
			{
				((sciPopUpMenuItem*)menuList->At(i))->Posn(x + 3, tempY);
				tempY += ((sciPopUpMenuItem*)menuList->At(i))->GetHeight();
			}
		}
	}
}
BOOL sciPopoidMenu::OnMe(int x, int y)
{
	int i;

	if(sciRect::OnMe(x,y))
		return TRUE;

	for(i = 0; i < menuList->Size(); i++)
	{
		if(((sciPopUpMenuItem*)menuList->At(i))->OnMe(x,y))
			return TRUE;
	}
	
	return FALSE;
}
void sciPopoidMenu::SetPri(int newPri)
{
	int i;

	sciRect::SetPri(newPri++);

	for(i = 0; i < 6; i++)
	{
		if(rects[i])
			rects[i]->SetPri(GetPri() + 1 + i);
	}

	for(i = 0; i < menuList->Size(); i++)
	{
		if(menuList->At(i))
			((sciPopUpMenuItem*)menuList->At(i))->SetPri(newPri++);
	}
}
void sciPopoidMenu::Show()
{
	int i;

	if(!numItems || !menuList->Size())
		return;

	sciRect::Show();
	if(rects[0])
	{
		for(i = 0; i < 6; i++)
			rects[i]->Show();
	}

	if(menuList)
	{
		for(i = 0; i < menuList->Size(); i++)
			((sciPopUpMenuItem*)menuList->At(i))->Show();
	}
}
void sciPopoidMenu::Hide()
{
	JustHideThisMenu();

	if(subOwner && subOwner->owner && subOwner->owner->IsNotHidden())
		subOwner->owner->Hide();
}
void sciPopoidMenu::JustHideThisMenu()
{
	int i;

	sciRect::Hide();

	if(rects[0])
	{
		for(i = 0; i < 6; i++)
			rects[i]->Hide();
	}
	
	if(menuList)
	{
		for(i = 0; i < menuList->Size(); i++)
			((sciPopUpMenuItem*)menuList->At(i))->Hide();
	}
}

void sciPopoidMenu::EnableItem(int anItemID)
{
	int i;
	sciPopUpMenuItem *menuItem;
	for(i = 0; i < menuList->Size(); i++)
	{
		menuItem = (sciPopUpMenuItem*)menuList->At(i);
		if(menuItem->GetID() == anItemID)
		{
			menuItem->Enable();
			return;
		}
		if(menuItem->subMenu)
			menuItem->subMenu->EnableItem(anItemID);
	}
}
void sciPopoidMenu::DisableItem(int anItemID)
{
	int i;
	sciPopUpMenuItem *menuItem;
	for(i = 0; i < menuList->Size(); i++)
	{
		menuItem = (sciPopUpMenuItem*)menuList->At(i);
		if(menuItem->GetID() == anItemID)
		{
			menuItem->Disable();
			return;
		}
		if(menuItem->subMenu)
			menuItem->subMenu->DisableItem(anItemID);
	}
}
BOOL sciPopoidMenu::IsItemEnabled(int anItemID)
{
	int i;
	BOOL isEnabled = FALSE;
	sciPopUpMenuItem *menuItem;
	for(i = 0; i < menuList->Size(); i++)
	{
		menuItem = (sciPopUpMenuItem*)menuList->At(i);
		if(menuItem->GetID() == anItemID)
			return menuItem->IsEnabled();
		if(menuItem->subMenu)
		{
			isEnabled = menuItem->subMenu->IsItemEnabled(anItemID);
			if(isEnabled)
				return TRUE;
		}
	}
	return FALSE;
}

void sciPopoidMenu::AddMenuItem(sciPopUpMenuItem *newItem)
{
	int i, itemOffset = 0;

	for(i = 0; i < menuList->Size(); i++)
		itemOffset += ((sciPopUpMenuItem*)menuList->At(i))->GetHeight();

	newItem->Posn(GetX() + 3, GetY() + 3 + itemOffset);
	
	menuList->AddToBack(newItem);
	
	newItem->SetPri(GetPri() + menuList->Size() + 8);
}
int sciPopoidMenu::GetNewID()
{
	int i;

	for(i = menuList->Size() - 1; i > -1; i--)
	{
		if(((sciPopUpMenuItem*)menuList->At(i))->GetID() >= ID_RESERVED)
			return ((sciPopUpMenuItem*)menuList->At(i))->GetID() + 1;
	}

	return ID_RESERVED;
}
void sciPopoidMenu::MakeNewMenuItem(char *newText, int fontNum, int newID)
{
	sciPopUpMenuItem *item;
	int i, tempHeight = 0;
	
	item = new sciPopUpMenuItem((newID == -1) ? GetNewID() : newID, this);

	if(item->GetID() == ID_MENU_SEPARATOR)
	{
		item->SetWidth(itemWidth);
		item->SetHeight(itemHeight / 3);
		item->DrawSeparator();
	}
	else
	{
		item->normalFont = fontNum;
		item->SetFont(item->normalFont);
		item->SetText(newText);
		item->SetWidth(itemWidth);
		item->SetHeight(itemHeight);
	}
	item->Init();
	AddMenuItem(item);

	numItems++;

	for(i = 0; i < menuList->Size(); i++)
		tempHeight += ((sciPopUpMenuItem *)menuList->At(i))->GetHeight();
	
	SetSize(itemWidth + 6, tempHeight + 6);//(itemHeight * numItems) + 6);
}


/////////////////////////////////////////////////////////////////////
/////			sciPopUpMenuItem FUNCTION DEFINITIONS			/////
/////////////////////////////////////////////////////////////////////
sciPopUpMenuItem::sciPopUpMenuItem(int newID, sciPopoidMenu *newOwner)
{
	int i;

	owner			= NULL;
	popoidOwner		= newOwner;
	plane			= newOwner->plane;
	id				= newID;
	subMenu			= NULL;
	subMenuArrow	= NULL;
	mouseDown		= FALSE;
	enabled			= TRUE;
	counter			= 0;
	delayProp		= NULL;

	for(i = 0; i < 2; i++)
		separatorLine[i] = NULL;

	normalFont		= DEFAULT_FONT;
	disabledFont	= DISABLED_FONT;
	highlightedFont	= HIGHLIGHTED_FONT;

	if(!delayProp)
	{
		delayProp = new sciProp();
		delayProp->SetView(0);
		delayProp->Init();
		delayProp->Show();
	}

	SetJustOffsets(5, 0);
	SetJust(JUST_LEFT | JUST_VERTICAL);
	SetBackgroundColor(DEFAULT_GREY);
}
sciPopUpMenuItem::sciPopUpMenuItem(int newID, sciPopUpMenu *newOwner)
{
	int i;

	owner			= newOwner;
	popoidOwner		= NULL;
	plane			= owner->plane;
	id				= newID;
	subMenu			= NULL;
	subMenuArrow	= NULL;
	mouseDown		= FALSE;
	enabled			= TRUE;
	counter			= 0;
	delayProp		= NULL;

	for(i = 0; i < 2; i++)
		separatorLine[i] = NULL;

	normalFont		= DEFAULT_FONT;
	disabledFont	= DISABLED_FONT;
	highlightedFont	= HIGHLIGHTED_FONT;

	if(!delayProp)
	{
		delayProp = new sciProp();
		delayProp->SetView(0);
		delayProp->Init();
		delayProp->Show();
	}

	SetJustOffsets(5, 0);
	SetJust(JUST_LEFT | JUST_VERTICAL);
	SetBackgroundColor(DEFAULT_GREY);
}
sciPopUpMenuItem::~sciPopUpMenuItem()
{
	int i;

	owner = NULL;
	popoidOwner = NULL;
	if(subMenu) {subMenu->Hide(); delete subMenu; subMenu = NULL;}
	if(subMenuArrow) {subMenuArrow->Hide(); delete subMenuArrow; subMenuArrow = NULL;}
	if(delayProp) {delete delayProp; delayProp = NULL;}

	for(i = 0; i < 2; i++)
		if(separatorLine[i]) {delete separatorLine[i]; separatorLine[i] = NULL;}
}
void sciPopUpMenuItem::InitSubMenu(PopUpMenuInfo *info)
{
	int x, y;

	if(!subMenu)
		subMenu = new sciPopUpMenu(owner->owner);
	subMenu->subOwner = this;
	subMenu->Init(info, TRUE);

	if(!subMenuArrow)
		subMenuArrow = new sciView();
	subMenuArrow->plane = plane;
	subMenuArrow->SetView(DEFAULT_ARROW);
	subMenuArrow->Init();
	subMenuArrow->SetPri(GetPri() + 1);
	x = GetX() + GetWidth() - 8;
	y = GetY() + (GetHeight() - subMenuArrow->GetHeight()) / 2;
	subMenuArrow->Posn(x, y);
	subMenuArrow->Hide();
}
BOOL sciPopUpMenuItem::OnMe(int x, int y)
{
	if(sciFeature::OnMe(x,y))
		return TRUE;

	if(subMenu && subMenu->OnMe(x,y))
		return TRUE;

	return FALSE;
}
void sciPopUpMenuItem::Show()
{
	int i;

	sciText::Show();

	if(subMenuArrow)
		subMenuArrow->Show();

	for(i = 0; i < 2; i++)
	{
		if(separatorLine[i])
			separatorLine[i]->Show();
	}
}
void sciPopUpMenuItem::Hide()
{
	int i;

	sciText::Hide();

	if(!IsBackgroundColor(DEFAULT_GREY))
		SetBackgroundColor(DEFAULT_GREY);
	
	if(GetFontNumber() == highlightedFont)
		SetFont(normalFont);
	
	if(subMenuArrow &&
		subMenuArrow->GetView() == HIGHLIGHTED_ARROW)
		subMenuArrow->SetView(DEFAULT_ARROW);

	if(subMenu)
		subMenu->Hide();

	if(subMenuArrow)
		subMenuArrow->Hide();

	for(i = 0; i < 2; i++)
	{
		if(separatorLine[i])
			separatorLine[i]->Hide();
	}
}
int sciPopUpMenuItem::GetID()
{
	return id;
}
void sciPopUpMenuItem::Posn(int x, int y)
{
	int i;

	sciText::Posn(x,y);

	for(i = 0 ; i < 2; i++)
	{
		if(separatorLine[i])
			separatorLine[i]->Posn(GetX() + 1, GetY() + i + GetHeight() / 2 - 1);
	}
}
void sciPopUpMenuItem::SetPri(int newPri)
{
	int i;

	sciText::SetPri(newPri);

	for(i = 0; i < 2; i++)
	{
		if(separatorLine[i])
			separatorLine[i]->SetPri(GetPri() + 1 + i);
	}
}
void sciPopUpMenuItem::DrawSeparator()
{
	int i;

	for(i = 0; i < 2; i++)
	{
		if(!separatorLine[i])
		{
			separatorLine[i] = new sciRect();
			separatorLine[i]->plane = plane;
			separatorLine[i]->SetSize(GetWidth() - 2, 1);
			separatorLine[i]->Posn(GetX() + 1, GetY() + i + GetHeight() / 2 - 1);
			separatorLine[i]->Init();
			separatorLine[i]->SetPri(GetPri() + i + 1);
			if(i == 0)
			{
				separatorLine[i]->SetInnerColor(DEFAULT_WHITE);
				separatorLine[i]->SetOutlineColor(DEFAULT_WHITE);
			}
			else
			{
				separatorLine[i]->SetInnerColor(DEFAULT_DKGREY);
				separatorLine[i]->SetOutlineColor(DEFAULT_DKGREY);
			}
		}
	}
}

Boolean sciPopUpMenuItem::HandleEvent(sciEvent &event)
{
	if(IsHidden())
		return event.claimed;

	switch(event.type)
	{
		case MOUSE_DOWN:
			if(OnMe(event.x,event.y) && !mouseDown && !event.claimed)
			{
				mouseDown = TRUE;
				event.claimed = TRUE;
			}
			break;
		case MOUSE_UP:
			if(OnMe(event.x,event.y) && IsBackgroundColor(HIGHLIGHT) && !event.claimed)
			{
				mouseDown = FALSE;
				event.claimed = TRUE;
				if(!subMenu)
					DoClick();
			}
			break;
		case MOUSE_MOVE:
			if(OnMe(event.x,event.y))
			{
				if(sciFeature::OnMe(event.x,event.y))
				{
					if(IsBackgroundColor(DEFAULT_GREY))
					{
						DoHighlight();
						if(delayProp)
							delayProp->SetScript(new DelayScript(this));
					}
				}
				event.claimed = TRUE;
			}
			else
			{
				if(IsBackgroundColor(HIGHLIGHT) && (!subMenu || subMenu->IsHidden()))
					DoUnHighlight();
				if(delayProp)
					delayProp->SetScript(NULL);
			}
			break;
	}

	return event.claimed;
}
void sciPopUpMenuItem::DoHighlight()
{
	int i, ownerSize = owner ? owner->menuList->Size() : popoidOwner->menuList->Size();
	sciPopUpMenuItem *item;


	if(GetID() != ID_MENU_SEPARATOR)
	{
		SetBackgroundColor(HIGHLIGHT);

		if(IsEnabled())
		{
			SetFont(highlightedFont);
			if(subMenuArrow)
				subMenuArrow->SetView(HIGHLIGHTED_ARROW);
		}
		if(owner && owner->subOwner)
		{
			owner->subOwner->SetBackgroundColor(HIGHLIGHT);
			owner->subOwner->subMenuArrow->SetView(HIGHLIGHTED_ARROW);
			owner->subOwner->SetFont(owner->subOwner->highlightedFont);
		}
	}

	for(i = 0; i < ownerSize; i++)
	{
		if(owner)
			item = (sciPopUpMenuItem*)owner->menuList->At(i);
		else
			item = (sciPopUpMenuItem*)popoidOwner->menuList->At(i);
		if(GetID() == item->GetID())
			continue;
		if(item->IsBackgroundColor(HIGHLIGHT))
			item->DoUnHighlight();
	}
}

void sciPopUpMenuItem::DoUnHighlight()
{
	if(!IsBackgroundColor(DEFAULT_GREY))
	{
		SetBackgroundColor(DEFAULT_GREY);
		if(IsEnabled())
		{
			SetFont(normalFont);
			if(subMenuArrow)
				subMenuArrow->SetView(DEFAULT_ARROW);
		}
	}
}
void sciPopUpMenuItem::ShowSubMenu()
{
	int i, ownerSize = owner ? owner->menuList->Size() : popoidOwner->menuList->Size();
	sciPopUpMenuItem *item;

	for(i = 0; i < ownerSize; i++)
	{
		if(owner)
			item = (sciPopUpMenuItem*)owner->menuList->At(i);
		else
			item = (sciPopUpMenuItem*)popoidOwner->menuList->At(i);

		if(GetID() == item->GetID())
			continue;
		if(item->subMenu && item->subMenu->IsNotHidden())
			item->subMenu->JustHideThisMenu();
	}
	
	if(subMenu)
		subMenu->Show();
	delayProp->SetScript(NULL);
}
void sciPopUpMenuItem::HideSubMenu()
{
	if(subMenu)
		subMenu->JustHideThisMenu();
	delayProp->SetScript(NULL);
}
void sciPopUpMenuItem::DoClick()
{
	if((owner || popoidOwner) && !subMenu && GetID() != ID_MENU_SEPARATOR && IsEnabled())
	{
		if(owner)
			owner->Select(this);
		else
			popoidOwner->Select(this);
	}
}
BOOL sciPopUpMenuItem::IsBackgroundColor(RGBQUAD color)
{
	if(backgroundColor.rgbRed == color.rgbRed &&
		backgroundColor.rgbGreen == color.rgbGreen &&
		backgroundColor.rgbBlue == color.rgbBlue)
		return TRUE;
		
	return FALSE;
}
void sciPopUpMenuItem::Enable()
{
	if(!enabled)
	{
		enabled = TRUE;
		SetFont(normalFont);
	}
}
void sciPopUpMenuItem::Disable()
{
	if(enabled)
	{
		enabled = FALSE;
		SetFont(disabledFont);
	}
}
BOOL sciPopUpMenuItem::IsEnabled()
{
	return enabled;
}


DelayScript::DelayScript(sciPopUpMenuItem *aMenuItem, sciScript *newCaller)
	: sciScript(newCaller)
{
	menuItem = aMenuItem;
}
void DelayScript::ChangeState(int newState)
{
	sciScript::ChangeState(newState);

	switch (state)
	{
		case 0:
			SetNextState(1);
			SetTicks(30);
			break;
		case 1:
			menuItem->ShowSubMenu();
			break;
	}
}
*/