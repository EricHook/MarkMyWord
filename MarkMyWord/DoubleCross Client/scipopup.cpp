#include "os.h"
#include "globals.h"
#include "scipopup.h"

#ifndef NOSCIPOPUPS

RGBQUAD DEFAULT_WHITE = {255, 255, 255, 0};
RGBQUAD HIGHLIGHT = {102,0,51,0};//Purple, {130,5,5,0};//Blue

#define DEFAULT_FONT		37
#define DISABLED_FONT		48
#define HIGHLIGHTED_FONT	50//49White
#define DEFAULT_ARROW		1055
#define DISABLED_ARROW		1082
#define HIGHLIGHTED_ARROW	1084//1083White
#define DEFAULT_CHECKMARK	1248
#define DISABLED_CHECKMARK_CEL	2//1248
#define HIGHLIGHTED_CHECKMARK_CEL 3//1248


/////////////////////////////////////////////////////////////////////
/////			sciPopUpMenu FUNCTION DEFINITIONS				/////
/////////////////////////////////////////////////////////////////////
sciPopUpMenu::sciPopUpMenu(int newType, Boolean modal)
	:sciPlane(modal)
{
	int i;

	SetDraggableArea(-1);

	type			= newType;
	id				= ID_EMPTY;
	selectedItemID	= -1;
	popUpButton		= NULL;
	subOwner		= NULL;

	menuRect = new sciRect();
	menuRect->plane = this;
	menuRect->Init();
	menuRect->SetInnerColor(DEFAULT_GREY);
	menuRect->SetOutlineColor(DEFAULT_GREY);
	menuRect->SetPri(1);
	
	for(i = 0; i < 6; i++)
		rects[i] = NULL;

	numItems			= 0;
	menuList = new sciList();

	mouseDown = FALSE;
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

	for(i = 0; i < 6; i++)
		if(rects[i]) {delete rects[i]; rects[i] = NULL;}

	if(menuRect) {delete menuRect; menuRect = NULL;}

	subOwner	= NULL;
}
Boolean sciPopUpMenu::HandleEvent(sciEvent &event)
{
	int i;
	sciPopUpMenu *menu;
	sciPopUpMenuItem *item;

	if(event.claimed)
		return event.claimed;
	
	menu = GetMenuEventIsOn(event.x,event.y,this);

	switch(event.type)
	{
		case KEY_PRESS:
			item = GetFirstMenu()->GetItemWithThisHotKey(tolower(event.c));
			if(item && item->IsEnabled())
			{
				item->DoClick();
				event.claimed = TRUE;
				return event.claimed;
			}
			break;

		case MOUSE_DOWN:
			if(!menu)
				GetFirstMenu()->Hide();
			else
			{
				GetFirstMenu()->mouseDown = TRUE;
				event.claimed = TRUE;
			}
			break;

		case MOUSE_MOVE:
			if(menu)
			{
				item = menu->GetItemEventIsOn(event.x,event.y,this);

				if(item && item->GetID() != ID_MENU_SEPARATOR)
				{
					if(item->IsBackgroundColor(DEFAULT_GREY))
					{
						item->owner->UnHighlightAllItemsFromHereDown(item);
						item->Highlight();
						if(item->IsEnabled() && item->delayProp)
							item->delayProp->SetScript(new DelayScript(item));
					}
				}
				else
				{
					menu->UnHighlightAllItemsFromHereDown();
				}

				event.claimed = TRUE;
			}
			else
			{
				for(i = 0; i < Size(); i++)
				{
					if(At(i)->subMenu && At(i)->subMenu->IsNotHidden())
						return event.claimed;
				}
				//moved the mouse off the menu(s)
				UnHighlightAllItemsFromHereDown();
			}
			break;
		case MOUSE_UP:
			if(menu)
			{
				if(GetFirstMenu()->mouseDown)
				{
					item = menu->GetItemEventIsOn(event.x,event.y,this);
					if(item && item->GetID() != ID_MENU_SEPARATOR)
					{
						if(!item->subMenu && item->IsEnabled())
							item->DoClick();
					}
				}
				event.claimed = TRUE;
			}
			else
			{
				if(GetFirstMenu()->mouseDown)
					GetFirstMenu()->Hide();
			}

			if(GetFirstMenu())
				GetFirstMenu()->mouseDown = FALSE;
			break;
	}

	return event.claimed;
}
sciPopUpMenuItem *sciPopUpMenu::GetItemWithThisHotKey(char c)
{
	int i;
	sciPopUpMenuItem *item = NULL;

	for(i = 0; i < Size(); i++)
	{
		if(At(i)->hotKey == c)
			return At(i);

		if(At(i)->subMenu)
		{
			item = At(i)->subMenu->GetItemWithThisHotKey(c);
			if(item)
				return item;
		}
	}

	return item;
}
sciPopUpMenu *sciPopUpMenu::GetMenuEventIsOn(int x, int y, sciPlane *lastPlane)
{
	x += lastPlane->GetX();
	y += lastPlane->GetY();

	x -= GetX();
	y -= GetY();

	if(menuRect->OnMe(x,y))
		return this;

	if(subOwner && subOwner->owner)
		return subOwner->owner->GetMenuEventIsOn(x,y,this);

	return NULL;
}
sciPopUpMenuItem *sciPopUpMenu::GetItemEventIsOn(int x, int y, sciPlane *lastPlane)
{
	int i;
	sciPopUpMenuItem *item = NULL;

	x += lastPlane->GetX();
	y += lastPlane->GetY();

	x -= GetX();
	y -= GetY();

	for(i = 0; i < Size(); i++)
	{
		if(At(i)->OnMe(x,y))
			return At(i);

		if(At(i)->subMenu)
			item = At(i)->subMenu->GetItemEventIsOn(x,y,this);
		if(item)
			return item;
	}

	return item;
}

void sciPopUpMenu::UnHighlightAllItemsFromHereDown(sciPopUpMenuItem *exemptItem)
{
	int i;

	for(i = 0; i < Size(); i++)
	{
		if(At(i) != exemptItem && At(i)->IsBackgroundColor(HIGHLIGHT))
			At(i)->DoUnHighlight();
	}

	for(i = 0; i < Size(); i++)
	{
		if(At(i)->subMenu && At(i)->subMenu->IsNotHidden())
			At(i)->subMenu->UnHighlightAllItemsFromHereDown();
	}
}

sciPopUpMenu *sciPopUpMenu::GetFirstMenu()
{
	sciPopUpMenu *tempMenu = this;

	while(tempMenu->subOwner)
		tempMenu = tempMenu->subOwner->owner;

	return tempMenu;
}

void sciPopUpMenu::SetPopButton(sciPopUpButton *newPopUpButton)
{
	popUpButton = newPopUpButton;
	popUpButton->popUpMenu = this;
}
		
void sciPopUpMenu::Init(PopUpMenuInfo *info, BOOL subMenu)
{
	int i, tempHeight = 0;
	sciPopUpMenuItem *item;

	id			= info->id;
	itemWidth	= info->itemWidth;
	itemHeight	= info->itemHeight;
	numItems	= 0;

	if(subMenu && subOwner->owner)
		popUpButton = subOwner->owner->popUpButton;
	
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

	if(type == POPUP_MENU)
		SetSize(itemWidth + 6, tempHeight + 6);
	else
		SetSize(itemWidth + 2, tempHeight + 2);

	menuRect->SetSize(GetWidth(), GetHeight());

	if(subMenu)
		SetPri(subOwner->GetPri() + 50);
	else
		SetPri(10000);

	if(info->posn.x == -1 && info->posn.y == -1)
	{
		if(subMenu)
			Posn(subOwner->GetScreenX() + subOwner->GetWidth(), subOwner->GetScreenY());
		else
		{
			if(type != POPUP_MENU && popUpButton)
				Posn(popUpButton->GetScreenX(), popUpButton->GetScreenY() + popUpButton->GetHeight());
		}
	}
	else
		Posn(info->posn.x,info->posn.y);

	if(type == POPUP_MENU)
		DrawMenu();

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
			if(info->fontNumber > 0)
				item->normalFont = info->fontNumber;
			item->SetFont(item->normalFont);
			item->SetText(info->items[i].title);
			item->SetWidth(itemWidth);
			item->SetHeight(itemHeight);
		}
		item->Init();
		AddMenuItem(item);
		if(info->items[i].checked)
			item->CheckOn();
		if(info->items[i].subMenuID != -1)
			item->InitSubMenu(GetMenu(info->items[i].subMenuID));
	}
}
void sciPopUpMenu::DrawMenu()
{
	int i;
	POINT offsets[6]	= { {1,1},{1,1},{2,2},{2,2},{3,3},{3,3} };
	POINT sizeReduct[6] = { {-1,-1},{-2,-2},{-3,-3},{-4,-4},{-5,-5},{-6,-6} };
	RGBQUAD colors[6] = { {80, 80, 80, 0}, {255, 255, 255, 0}, {80, 80, 80, 0}, 
						{255, 255, 255, 0}, {80, 80, 80, 0}, {221, 221, 221, 0}};

	if(type != POPUP_MENU)
		return;
	
	for(i = 0; i < 6; i++)
	{
		if(!rects[i])
		{
			rects[i] = new sciRect();
			rects[i]->plane = this;
			rects[i]->SetSize(GetWidth() + sizeReduct[i].x, GetHeight() + sizeReduct[i].y);
			rects[i]->Posn(offsets[i].x, offsets[i].y);
			rects[i]->Init();
			rects[i]->SetPri(menuRect->GetPri() + 1 + i);
			rects[i]->SetInnerColor(colors[i]);
			rects[i]->SetOutlineColor(colors[i]);
		}
		else
		{
			rects[i]->Posn(offsets[i].x, offsets[i].y);
			rects[i]->SetPri(menuRect->GetPri() + 1 + i);
		}
	}
}

void sciPopUpMenu::Select(sciPopUpMenuItem *selectedItem)
{
	if(CanSelectThisItem(selectedItem->id))
	{
		Select(selectedItem->GetID());
		DoSelect(GetSelectedID());
	}

	Hide();
}
void sciPopUpMenu::Select(int itemID)
{
	if(itemID > -1)
	{
		selectedItemID = itemID;
		if(popUpButton && type != POPUP_MENU)
			popUpButton->SetText(GetThisItem(GetSelectedID())->GetText());
	}
}
int sciPopUpMenu::GetSelectedID()
{
	return selectedItemID;
}
int sciPopUpMenu::GetHighlightedID()
{
	int i;

	for(i = 0; i < Size(); i++)
	{
		if(At(i)->IsBackgroundColor(HIGHLIGHT))
			return At(i)->GetID();
	}

	return -1;
}

void sciPopUpMenu::Show()
{
	int i;

	if(!numItems || !menuList->Size() || IsNotHidden())
		return;

	sciPlane::Show();

	if(type != POPUP_MENU && popUpButton)
		Posn(popUpButton->GetScreenX(), popUpButton->GetScreenY() + popUpButton->GetHeight());

	game->curRoom->SetCapturedPlane(this);

	menuRect->Show();

	for(i = 0; i < 6; i++)
	{
		if(rects[i])
			rects[i]->Show();
	}

	for(i = 0; i < Size(); i++)
		At(i)->Show();
}
void sciPopUpMenu::Hide()
{
	if(IsHidden())
		return;

	sciPlane::Hide();

	game->curRoom->SetCapturedPlane(NULL);
	
	menuRect->Hide();

	JustHideThisMenu();

	if(subOwner && subOwner->owner && subOwner->owner->IsNotHidden())
		subOwner->owner->Hide();
}
void sciPopUpMenu::JustHideThisMenu()
{
	int i;

	sciPlane::Hide();

	menuRect->Hide();

	for(i = 0; i < 6; i++)
	{
		if(rects[i])
			rects[i]->Hide();
	}
	
	for(i = 0; i < Size(); i++)
		At(i)->Hide();
}

void sciPopUpMenu::SetPri(int newPri)
{
	int i;

	menuRect->SetPri(newPri++);

	for(i = 0; i < 6; i++)
	{
		if(rects[i])
			rects[i]->SetPri(menuRect->GetPri() + 1 + i);
	}

	for(i = 0; i < Size(); i++)
		At(i)->SetPri(newPri++);
}
void sciPopUpMenu::Posn(int x, int y)
{
	int i, tempY, typeOffset = (type == POPUP_MENU) ? 3 : 1;
	POINT offsets[6] = { {1,1},{1,1},{2,2},{2,2},{3,3},{3,3} };

	if(x < 0) x = 0;
	if(x + GetWidth() > 639)
		x = 639 - GetWidth();
	if(y < 0) y = 0;
	if(y + GetHeight() > 479)
		y = 479 - GetHeight();

	sciPlane::Posn(x,y);

	menuRect->Posn(0,0);
	
	for(i = 0; i < 6; i++)
	{
		if(rects[i])
			rects[i]->Posn(offsets[i].x, offsets[i].y);
	}

	tempY = typeOffset;

	for(i = 0; i < Size(); i++)
	{
		At(i)->Posn(typeOffset, tempY);
		tempY += At(i)->GetHeight();
	}
}
BOOL sciPopUpMenu::OnMe(int x, int y, sciPlane *lastPlane)
{
	x += lastPlane->GetX();
	y += lastPlane->GetY();

	x -= GetX();
	y -= GetY();

	return OnMe(x,y);
}
BOOL sciPopUpMenu::OnMe(int x, int y)
{
	int i;

	if(menuRect->OnMe(x,y))
		return TRUE;

	for(i = 0; i < 6; i++)
	{
		if(rects[i] && rects[i]->OnMe(x,y))
			return TRUE;
	}

	for(i = 0; i < Size(); i++)
	{
		if(At(i)->OnMe(x,y))
			return TRUE;
	}
	
	return FALSE;
}
sciPopUpMenuItem *sciPopUpMenu::At(int i)
{
	if(menuList && i < menuList->Size())
		return (sciPopUpMenuItem *)menuList->At(i);
	return NULL;
}
int sciPopUpMenu::Size()
{
	if(menuList)
		return menuList->Size();
	return 0;
}
sciPopUpMenuItem *sciPopUpMenu::GetThisItem(int itemID)
{
	int i;
	sciPopUpMenuItem *item;

	for(i = 0; i < Size(); i++)
	{
		if(At(i)->GetID() == itemID)
			return At(i);
		if(At(i)->subMenu)
		{
			item = At(i)->subMenu->GetThisItem(itemID);
			if(item)
				return item;
		}
	}

	return NULL;
}

void sciPopUpMenu::CheckItemOn(int itemID)
{
	sciPopUpMenuItem *menuItem = GetThisItem(itemID);
	if(menuItem)
		menuItem->CheckOn();
}
void sciPopUpMenu::CheckItemOff(int itemID)
{
	sciPopUpMenuItem *menuItem = GetThisItem(itemID);
	if(menuItem)
		menuItem->CheckOff();
}
BOOL sciPopUpMenu::ItemIsChecked(int itemID)
{
	sciPopUpMenuItem *menuItem = GetThisItem(itemID);
	if(menuItem)
		return menuItem->checkMark != NULL;
	return FALSE;
}

void sciPopUpMenu::EnableItem(int anItemID)
{
	int i;
	sciPopUpMenuItem *menuItem;
	for(i = 0; i < Size(); i++)
	{
		menuItem = At(i);
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
	for(i = 0; i < Size(); i++)
	{
		menuItem = At(i);
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
	for(i = 0; i < Size(); i++)
	{
		menuItem = At(i);
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

void sciPopUpMenu::AddMenuItem(sciPopUpMenuItem *newItem)
{
	int i, itemOffset = 0;
	int typeOffset = (type == POPUP_MENU) ? 3 : 1;

	for(i = 0; i < Size(); i++)
		itemOffset += At(i)->GetHeight();

	newItem->Posn(typeOffset, typeOffset + itemOffset);
	
	menuList->AddToBack(newItem);
	
	newItem->SetPri(menuRect->GetPri() + menuList->Size() + 8);
}
int sciPopUpMenu::GetNewID()
{
	int i;

	for(i = Size() - 1; i > -1; i--)
	{
		if(At(i)->GetID() >= ID_RESERVED)
			return At(i)->GetID() + 1;
	}

	return ID_RESERVED;
}
void sciPopUpMenu::MakeNewMenuItem(char *newText, int fontNum, int newID)
{
	sciPopUpMenuItem *item;
	int i, tempHeight = 0;
	int typeOffset = (type == POPUP_MENU) ? 6 : 2;
	
	item = new sciPopUpMenuItem((newID == -1) ? GetNewID() : newID, this);

	if(item->GetID() == ID_MENU_SEPARATOR)
	{
		item->SetWidth(itemWidth);
		item->SetHeight(itemHeight / 3);
		item->DrawSeparator();
	}
	else
	{
		if(fontNum > 0)
			item->normalFont = fontNum;
		item->SetFont(item->normalFont);
		item->SetText(newText);
		item->SetWidth(itemWidth);
		item->SetHeight(itemHeight);
	}
	item->Init();
	AddMenuItem(item);

	numItems++;

	for(i = 0; i < Size(); i++)
		tempHeight += At(i)->GetHeight();
	
	if(type != POPUP_SCROLL_DROPLIST)
		SetSize(itemWidth + typeOffset, tempHeight + typeOffset);
}
sciPopUpMenu *sciPopUpMenu::MakeSubPopUpMenu()
{
	return(new sciPopUpMenu(type));
}

/////////////////////////////////////////////////////////////////////
/////			sciPopUpButton FUNCTION DEFINITIONS				/////
/////////////////////////////////////////////////////////////////////
sciPopUpButton::sciPopUpButton(int newID)
{
	id			= newID;
	popUpMenu	= NULL;
	title		= NULL;

	SetTextJust(JUST_LEFT | JUST_VERTICAL);
	SetJustOffsets(5, 0);
}
sciPopUpButton::~sciPopUpButton()
{
	if(title) {delete title; title = NULL;}
}

void sciPopUpButton::DoClick(BOOL netMessage)
{
	if(popUpMenu)
	{
		if(popUpMenu->IsHidden())
			popUpMenu->Show();
		else
			popUpMenu->Hide();
	}
}
void sciPopUpButton::SetTitle(char *newTitle, int newFont)
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
void sciPopUpButton::Show()
{
	sciButton::Show();

	if(title)
		title->Show();
}
void sciPopUpButton::Hide()
{
	sciButton::Hide();

	if(title)
		title->Hide();
}
int sciPopUpButton::GetScreenX()
{
	return plane->GetX() + GetX();
}
int sciPopUpButton::GetScreenY()
{
	return plane->GetY() + GetY();
}

/////////////////////////////////////////////////////////////////////
/////			sciPopUpMenuItem FUNCTION DEFINITIONS			/////
/////////////////////////////////////////////////////////////////////
sciPopUpMenuItem::sciPopUpMenuItem(int newID, sciPopUpMenu *newOwner)
{
	int i;

	owner			= newOwner;
	plane			= newOwner;
	id				= newID;
	subMenu			= NULL;
	subMenuArrow	= NULL;
	checkMark		= NULL;
	mouseDown		= FALSE;
	enabled			= TRUE;
	hotKey			= 0;
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

	SetJustOffsets(10, 0);
	SetJust(JUST_LEFT | JUST_VERTICAL);
	SetBackgroundColor(DEFAULT_GREY);
}

sciPopUpMenuItem::~sciPopUpMenuItem()
{
	int i;

	owner = NULL;
	if(subMenu) {subMenu->Hide(); delete subMenu; subMenu = NULL;}
	if(subMenuArrow) {subMenuArrow->Hide(); delete subMenuArrow; subMenuArrow = NULL;}
	if(checkMark) {checkMark->Hide(); delete checkMark; checkMark = NULL;}
	if(delayProp) {delete delayProp; delayProp = NULL;}

	for(i = 0; i < 2; i++)
		if(separatorLine[i]) {delete separatorLine[i]; separatorLine[i] = NULL;}
}

void sciPopUpMenuItem::InitSubMenu(PopUpMenuInfo *info)
{
	int x, y;

	if(!subMenu)
		subMenu = owner->MakeSubPopUpMenu();
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

void sciPopUpMenuItem::Show()
{
	int i;

	sciText::Show();

	if(checkMark)
		checkMark->Show();

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

	if(checkMark)
		checkMark->Hide();
	
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
int sciPopUpMenuItem::GetScreenX()
{
	return plane->GetX() + GetX();
}
int sciPopUpMenuItem::GetScreenY()
{
	return plane->GetY() + GetY();
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

void sciPopUpMenuItem::Highlight()
{
	int i;

	if(GetID() != ID_MENU_SEPARATOR)
	{
		SetBackgroundColor(HIGHLIGHT);

		if(IsEnabled())
		{
			SetFont(highlightedFont);
			if(subMenuArrow)
				subMenuArrow->SetView(HIGHLIGHTED_ARROW);
			if(checkMark)
				checkMark->SetCel(HIGHLIGHTED_CHECKMARK_CEL);
		}

		if(owner && owner->subOwner)
		{
			for(i = 0; i < owner->subOwner->owner->Size(); i++)
			{
				if(owner->subOwner->owner->At(i)->IsBackgroundColor(HIGHLIGHT))
					owner->subOwner->owner->At(i)->DoUnHighlight();
			}
			owner->subOwner->Highlight();
		}
	}
}

void sciPopUpMenuItem::DoUnHighlight()
{
	SetBackgroundColor(DEFAULT_GREY);
	if(IsEnabled())
	{
		SetFont(normalFont);
		if(subMenuArrow)
			subMenuArrow->SetView(DEFAULT_ARROW);
		if(checkMark)
			checkMark->SetCel(0);
	}
	if(delayProp)
		delayProp->SetScript(NULL);
}
void sciPopUpMenuItem::ShowSubMenu()
{
	int i;
	sciPopUpMenuItem *item;

	for(i = 0; i < owner->menuList->Size(); i++)
	{
		item = (sciPopUpMenuItem*)owner->menuList->At(i);
		
		if(GetID() == item->GetID())
			continue;
		
		if(item->subMenu && item->subMenu->IsNotHidden())
			item->subMenu->JustHideThisMenu();
	}
	
	if(subMenu && IsBackgroundColor(HIGHLIGHT))
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
	if(owner && !subMenu && GetID() != ID_MENU_SEPARATOR && IsEnabled())
		owner->Select(this);
}
BOOL sciPopUpMenuItem::IsBackgroundColor(RGBQUAD color)
{
	if(backgroundColor.rgbRed == color.rgbRed &&
		backgroundColor.rgbGreen == color.rgbGreen &&
		backgroundColor.rgbBlue == color.rgbBlue)
		return TRUE;
		
	return FALSE;
}
void sciPopUpMenuItem::SetHotKey(char c)
{
	hotKey = tolower(c);
}
void sciPopUpMenuItem::CheckOn()
{
	if(!checkMark)
	{
		checkMark = new sciView();
		checkMark->plane = plane;
		checkMark->SetView(DEFAULT_CHECKMARK);
		checkMark->Init();
		checkMark->Posn(GetX() + 1,GetY() + 5);
		checkMark->SetPri(GetPri() + 1);
	}
	if(IsNotHidden())
		checkMark->Show();
}
void sciPopUpMenuItem::CheckOff()
{
	if(checkMark)
	{
		checkMark->Hide();
		delete checkMark;
		checkMark = NULL;
	}
}
void sciPopUpMenuItem::Enable()
{
	if(!enabled)
	{
		enabled = TRUE;
		SetFont(normalFont);
		if(subMenuArrow)
			subMenuArrow->SetView(DEFAULT_ARROW);
		if(checkMark)
			checkMark->SetCel(0);
	}
}
void sciPopUpMenuItem::Disable()
{
	if(enabled)
	{
		enabled = FALSE;
		SetFont(disabledFont);
		if(subMenuArrow)
			subMenuArrow->SetView(DISABLED_ARROW);
		if(checkMark)
			checkMark->SetCel(DISABLED_CHECKMARK_CEL);
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

#endif
