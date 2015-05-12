#include "os.h"
#include "globals.h"
#include "scidplst.h"

RGBQUAD DEFAULT_BLACK = {0, 0, 0, 0};

#ifndef NOSCIPOPUPS

/////////////////////////////////////////////////////////////////////
/////			sciDropList FUNCTION DEFINITIONS				/////
/////////////////////////////////////////////////////////////////////
sciDropList::sciDropList(int newType, Boolean modal)
	:sciPopUpMenu(newType, modal)
{
	menuRect->SetInnerColor(DEFAULT_GREY);
	menuRect->SetOutlineColor(DEFAULT_BLACK);
}

sciDropList::~sciDropList()
{
}

int sciDropList::GetIDOfItemInListWithIndex(int index)
{
	if(Size() > index)
		return At(index)->GetID();

	return -1;
}
sciPopUpMenuItem *sciDropList::GetSelectedItem()
{
	int i;

	for(i = 0; i < Size(); i++)
	{
		if(At(i)->GetID() == GetSelectedID())
			return At(i);
	}
	return NULL;
}
BOOL sciDropList::CanSelectThisItem(int itemID)
{
	if(IsItemEnabled(itemID) &&	(GetSelectedID() != itemID || type == POPUP_MENU))
		return TRUE;
	return FALSE;
}

char *sciDropList::GetSelectedItemText()
{
	return GetTextOfItemWithID(GetSelectedID());
}
char *sciDropList::GetTextOfItemWithID(int itemID)
{
	int i;

	for(i = 0; i < Size(); i++)
	{
		if(At(i)->GetID() == itemID)
			return At(i)->GetText();
	}
	return "";
}
char *sciDropList::GetTextOfItemInListWithIndex(int index)
{
	if(Size() > index)
		return At(index)->GetText();
	
	return "";
}

void sciDropList::RemoveItemByString(char *itemString)
{
	int i;
	sciPopUpMenuItem *tempItem;
	int removedItemID = -1;
	int newID = ID_RESERVED;

	for(i = 0; i < Size(); i++)
	{
		tempItem = At(i);
		if(!strcmp(itemString, tempItem->GetText())) 
		{
			removedItemID = tempItem->GetID();
			tempItem->Hide();
			menuList->Delete(tempItem);
			delete tempItem;
			break;
		}
	}

	numItems = Size();

	// Reset all IDs that were given through dynamic addition to the menu list
	for(i = 0; i < Size(); i++)
	{
		if(At(i)->GetID() >= ID_RESERVED)
			At(i)->id = newID++;
	}
}
void sciDropList::RemoveAllItems()
{
	sciPopUpMenuItem *tempItem;
	
	if (!menuList)
		return;
	
	while(Size()) 
	{
		tempItem = At(0);
		tempItem->Hide();
		menuList->Delete(tempItem);
		delete tempItem;
	}
}


/////////////////////////////////////////////////////////////////////
/////			sciScrollDropList FUNCTION DEFINITIONS			/////
/////////////////////////////////////////////////////////////////////
sciScrollDropList::sciScrollDropList(int newType, Boolean modal)
	:sciDropList(newType, modal)
{
	scroll				= NULL;
	maxVisibleItems		= 0;
	firstVisibleItem	= 0;
}
sciScrollDropList::~sciScrollDropList()
{
	if(scroll) {delete scroll; scroll = NULL;}
}
void sciScrollDropList::Init(PopUpMenuInfo *info, int newWidth, int newHeight)
{
	int i;
	sciPopUpMenuItem *item;

	itemWidth	= info->itemWidth;
	itemHeight	= info->itemHeight;
	numItems	= 0;
	
	for(i = 0; i < MAX_MENU_ITEMS; i++)
	{
		if(info->items[i].id != -1)
			numItems++;
		else
			break;
	}

	//sciRect::Init();
	SetSize(newWidth, newHeight);
	SetPri(popUpButton->GetPri() + 10);

	Posn(popUpButton->GetScreenX(), popUpButton->GetScreenY() + popUpButton->GetHeight());

	if (!scroll) 
	{
		scroll = new ComboBoxScroll(this);
		scroll->plane = this;//plane;
		scroll->Init();
		scroll->SetViews(1036,1035,1037);
		scroll->SetPri(100);//GetPri() + 100);
	}
	scroll->Posn(newWidth, GetY());
	scroll->SetSize(16, newHeight - 1);
	scroll->Hide();

	for(i = 0; i < numItems; i++)
	{
		item = new sciPopUpMenuItem(info->items[i].id, this);
		item->plane = this;//plane;
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
		
		if(info->items[i].checked)
			item->CheckOn();

		if(info->items[i].subMenuID != -1)
			item->InitSubMenu(GetMenu(info->items[i].subMenuID));
	}
}
void sciScrollDropList::SetMaxVisibleItems(int newMaxItems)
{
	maxVisibleItems = newMaxItems;
}
BOOL sciScrollDropList::IsSelectedItemVisible()
{
	int i;
	for(i = 0; i < Size(); ++i) 
	{
		if (At(i)->GetID() == GetSelectedID()) 
		{
			return(i >= firstVisibleItem && i < firstVisibleItem + maxVisibleItems);
		}
	}

	return FALSE;
}
void sciScrollDropList::ScrollMenuItems(int direction, int numItemsToScroll)
{
	if (direction == GDefs::REVERSE) {
		RefreshList(firstVisibleItem - numItemsToScroll);
	} else if (direction == GDefs::FORWARD) {
		RefreshList(firstVisibleItem + numItemsToScroll);
	} else {
		assert(FALSE);
	}
}
void sciScrollDropList::RefreshList(int firstItem)
{
	int i, tempY, iSize = Size();
	sciPopUpMenuItem *tempItem;

	if(iSize) 
	{
		if (firstItem > iSize - maxVisibleItems)
			firstItem = iSize - maxVisibleItems;

		if (firstItem < 0)
			firstVisibleItem = 0;
		else
			firstVisibleItem = firstItem;

		tempY = GetY() + 1;
		
		for (i = 0; i < iSize; i++) 
		{
			tempItem = At(i);
			if (i < firstVisibleItem || i >= firstVisibleItem + maxVisibleItems)
			{
				if (tempItem->IsNotHidden())
					tempItem->Hide();
			}
			else 
			{
				tempItem->Posn(GetX() + 1, tempY);
				tempY += itemHeight;
				if(IsNotHidden())
					tempItem->Show();
			}
		}
	}

	scroll->SnapThumbnail(firstVisibleItem);
}

void sciScrollDropList::Show()
{
	sciDropList::Show();

	if(scroll)
	{
		scroll->Show();
		if(Size() > maxVisibleItems)
			scroll->Enable();
	}

	if (IsSelectedItemVisible())
		RefreshList(firstVisibleItem);
	else
		RefreshList(GetSelectedID());
}
void sciScrollDropList::JustHideThisMenu()
{
	sciDropList::JustHideThisMenu();

	if(scroll)
		scroll->Hide();
}
BOOL sciScrollDropList::OnMe(int x, int y, sciPlane *lastPlane)
{
	if(sciDropList::OnMe(x,y,lastPlane))
		return TRUE;

	if(scroll && scroll->OnMe(x,y))
		return TRUE;

	return FALSE;
}

void sciScrollDropList::RemoveItemByString(char *itemString)
{
	sciDropList::RemoveItemByString(itemString);

	if(scroll && maxVisibleItems >= numItems)
		scroll->Disable();

	RefreshList(firstVisibleItem);
}
void sciScrollDropList::RemoveAllItems()
{
	sciDropList::RemoveAllItems();

	firstVisibleItem = 0;
	RefreshList(firstVisibleItem);
}


/////////////////////////////////////////////////////////////////////
/////				ComboBoxScroll FUNCTION DEFINITIONS			/////
/////////////////////////////////////////////////////////////////////
ComboBoxScroll::ComboBoxScroll(sciScrollDropList *newOwner) : sciScrollBox(GDefs::VERTICAL)
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
	return(owner->Size());
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

#endif
