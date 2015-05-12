#include "os.h"
#include "sci.h"
#include "assert.h"
#include "app.h"
#include "rc.h"
#include "game.h"

/////////////////////////////////////////////////////////////////////
/////				sciListText FUNCTION DEFINITIONS			/////
/////////////////////////////////////////////////////////////////////
sciListText::sciListText()
{
	owner = NULL;
	mouseDown = FALSE;
	selected = FALSE;
	textID = 0;
	normalFont		= 0;
	highlightedFont = 0;
}

sciListText::~sciListText()
{
}

void sciListText::SetText(char *newText)
	{
	//short maxWidth;
	
	sciText::SetText(newText);
	
	//maxWidth = CalculateFlatWidth() + 2;

	//if (owner && (owner->GetBoxView()->GetWidth() - 30) < maxWidth)
	//	maxWidth = owner->GetBoxView()->GetWidth() - 30;
	

	//SetWidth(maxWidth);
	}

Boolean sciListText::HandleEvent(sciEvent &event)
{
	if ((!sciFeature::OnMe(event) && !mouseDown) || event.claimed)
		return FALSE;

	switch (event.type)
	{
	case MOUSE_DOWN:
		if (OnMe(event.x, event.y) && event.modifiers & RIGHT_MOUSE)
			event.claimed = TRUE;

		if (OnMe(event.x, event.y) && (event.modifiers & LEFT_MOUSE) && !mouseDown) {
			mouseDown = TRUE;
			event.claimed = TRUE;
			// If not already selected, select this item and unselect any others
			if (!IsSelected()) {
				owner->SelectItem(GetID());
			}
		}
		break;

	case MOUSE_UP:
		if ((event.modifiers & LEFT_MOUSE) && mouseDown) {
			mouseDown = FALSE;
			event.claimed = TRUE;
		}
		break;
	}
	return event.claimed;
}

void sciListText::Select()
{
	selected = TRUE;
	SetFont(highlightedFont);
	SetBackgroundColor(5,5,130);//(166, 202, 240);			// Light blue (from Windows palette)
	Show();
}

void sciListText::Unselect()
{
	selected = FALSE;
	SetFont(normalFont);
	SetBackgroundColor(0,0,0);//255, 255, 255);		// White
}

Boolean sciListText::IsSelected()
{
	return(selected);
}

void sciListText::SetID(int newID)
{
	textID = newID;
}

int sciListText::GetID()
{
	return(textID);
}

void sciListText::SetOwner(sciListBox *newOwner)
{
	owner = newOwner;
}

void sciListText::SetFonts(int newNorm, int newHighlight)
{
	normalFont = newNorm;
	
	SetFont(normalFont);

	if(newHighlight != -1)
		highlightedFont = newHighlight;
	else
		highlightedFont = newNorm;
}


/////////////////////////////////////////////////////////////////////
/////				ListBoxScroll FUNCTION DEFINITIONS			/////
/////////////////////////////////////////////////////////////////////
ListBoxScroll::ListBoxScroll(sciListBox *newOwner) : sciScrollBox(GDefs::VERTICAL)
{
	owner = newOwner;
}

ListBoxScroll::~ListBoxScroll()
{
}

int ListBoxScroll::GetNumItems()
{
	return(owner->Size());
}

int ListBoxScroll::GetMaxVisibleItems()
{
	return(owner->GetMaxVisibleItems());
}

void ListBoxScroll::DoScroll(int direction, int numItems)
{
	owner->ScrollList(direction, numItems);
}

void ListBoxScroll::DoRefresh(int firstItem)
{
	owner->RefreshList(firstItem);
}


/////////////////////////////////////////////////////////////////////
/////				sciListBox FUNCTION DEFINITIONS				/////
/////////////////////////////////////////////////////////////////////
sciListBox::sciListBox()
{
	boxView				= NULL;
	newScroll			= NULL;
	listBoxItems		= NULL;

	firstVisibleItem	= 0;
	maxVisibleItems		= 0;
	numHiddenItems		= 0;

	trueBoxWidth		= 0;
	
	scrollX				= 0;
	scrollY				= 0;

	font				= 5;	// Default font is Ariel 12
	highlightFont		= 5;
	topTextX			= 0;
	topTextY			= 0;
	xOffset				= 0;
	xJustOffset			= 0;
	
	boxRect.left = 0;
	boxRect.right = 100;
	boxRect.top = 0;
	boxRect.bottom = 100;
}

sciListBox::~sciListBox()
{
	if (boxView)		{delete boxView; boxView = NULL;}
	if (newScroll)		{delete newScroll; newScroll = NULL;}
	if (listBoxItems) {
		sciListText *tmpListText;
		while (listBoxItems->Size()) {
			tmpListText = (sciListText *)listBoxItems->At(0);
			listBoxItems->Delete(tmpListText);
			delete tmpListText;
		}
		delete listBoxItems;
		listBoxItems = NULL;
	}
}

void sciListBox::Init(int newPrevView, int newNextView, int newScrollerView)
{
	sciFeature::Init();
	
	if (!newScroll)
		newScroll = new ListBoxScroll(this);
	newScroll->plane = plane;
	newScroll->SetViews(newPrevView, newNextView, newScrollerView);
	newScroll->Init();
	newScroll->SetPri(4);
	
	if (!listBoxItems)
		listBoxItems = new sciList();
	
	firstVisibleItem = 0;
}

Boolean sciListBox::HandleEvent(sciEvent &event)
{
	if (IsHidden() || event.claimed)
		return FALSE;

	int iSelectedItemID;

	switch (event.type) {
	case KEY_PRESS:
		if (event.modifiers == SPECIAL_KEY) {
			switch (event.c) {
			case SK_UPARROW:
				iSelectedItemID = GetSelectedItemID();
				if (iSelectedItemID > 0) {
					if (GetItemByID(iSelectedItemID - 1)->IsHidden() && listBoxItems->Size() > maxVisibleItems) {
						if (IsSelectedItemVisible()) {
							ScrollList(GDefs::REVERSE, 1);
						} else {
							if (GetFirstVisibleItemID() > iSelectedItemID) {
								RefreshList(iSelectedItemID - 1);
							} else {
								RefreshList(iSelectedItemID - maxVisibleItems);
							}
						}
					}
					SelectItem(iSelectedItemID - 1);
				}
				break;
			case SK_DOWNARROW:
				iSelectedItemID = GetSelectedItemID();
				if (iSelectedItemID < listBoxItems->Size() - 1) {
					if (GetItemByID(iSelectedItemID + 1)->IsHidden() && listBoxItems->Size() > maxVisibleItems) {
						if (IsSelectedItemVisible()) {
							ScrollList(GDefs::FORWARD, 1);
						} else {
							if (GetFirstVisibleItemID() > iSelectedItemID) {
								RefreshList(iSelectedItemID + 1);
							} else {
								RefreshList(iSelectedItemID - (maxVisibleItems - 2));
							}
						}
					}
					SelectItem(iSelectedItemID + 1);
				}
				break;
			}
		}
		break;
	}
	return(event.claimed);
}

void sciListBox::Show()
{
	// Assumes all of the following pointers exist!
	sciFeature::Show();
	if (boxView)
		boxView->Show();
	newScroll->Show();
	
	// Refresh the visible list items.
	if (IsSelectedItemVisible())
		RefreshList(firstVisibleItem);
	else
		RefreshList(GetSelectedItemID());
}

void sciListBox::Hide()
{
	// Assumes all of the following pointers exist!
	sciFeature::Hide();
	if (boxView)
		boxView->Hide();
	newScroll->Hide();
	
	// Hide everything
	for (int i = 0; i < listBoxItems->Size(); ++i)
		((sciListText *)listBoxItems->At(i))->Hide();
}

void sciListBox::RefreshList(int firstItem)
{
	int iSize = listBoxItems->Size();

	if (iSize) {
		if (firstItem > iSize - maxVisibleItems) {
			firstItem = iSize - maxVisibleItems;
		}

		if (firstItem < 0) {
			firstVisibleItem = 0;
		} else {
			firstVisibleItem = firstItem;
		}

		sciListText *tmpText = (sciListText *)listBoxItems->At(0);
		int itemHeight = tmpText->GetMinHeight();
		itemHeight = 15;
		for (int i = 0; i < iSize; ++i) {
			tmpText = (sciListText *)listBoxItems->At(i);
			if (i < firstVisibleItem || i >= firstVisibleItem + maxVisibleItems) {
				if (tmpText->IsNotHidden()) {
					tmpText->Hide();
				}
			} else {
				tmpText->Posn(topTextX, topTextY + (i - firstVisibleItem) * itemHeight);
				//tmpText->Posn(boxView->GetX() + 7,
				// boxView->GetY() + 5 + (i - firstVisibleItem) * itemHeight);
				tmpText->Show();
			}
		}
	}

	newScroll->SnapThumbnail(firstVisibleItem);
}

void sciListBox::ScrollList(int direction, int numItemsToScroll)
{
	if (direction == GDefs::REVERSE) {
		RefreshList(firstVisibleItem - numItemsToScroll);
	} else if (direction == GDefs::FORWARD) {
		RefreshList(firstVisibleItem + numItemsToScroll);
	} else {
		assert(FALSE);
	}
}

void sciListBox::SetFont(int newFont)
{
	int i;

	font = newFont;
	sciListText *tmpListText;
	for (i = 0; i < listBoxItems->Size(); ++i) {
		tmpListText = (sciListText *)listBoxItems->At(i);
		tmpListText->SetFont(font);
	}
}
void sciListBox::SetTopTextPosn(int newX, int newY)
{
	topTextX = newX;
	topTextY = newY;
}
void sciListBox::SetFonts(int newNorm, int newHighlight)
{
	int i;

	font = newNorm;
	if(newHighlight != -1)
		highlightFont = newHighlight;
	else
		highlightFont = newNorm;

	sciListText *tmpListText;
	for (i = 0; i < listBoxItems->Size(); ++i) {
		tmpListText = (sciListText *)listBoxItems->At(i);
		tmpListText->SetFonts(newNorm, newHighlight);
	}
}

int sciListBox::Size()
{
	return(listBoxItems->Size());
}

void sciListBox::SetBoxRect(int left, int top, int right, int bottom)
{
	boxRect.left = left;
	boxRect.top = top;
	boxRect.right = right;
	boxRect.bottom = bottom;
	
	SetupScroller();
}

void sciListBox::SetBoxView(int newView)
{
	if (!inited)
		Init();
	
	if (!boxView) {
		boxView = new sciView();
		assert(boxView);
		boxView->plane = plane;
		boxView->SetPri(3);
		boxView->Init();
	}
	
	boxView->SetView(newView);
	
	SetupScroller();
}

void sciListBox::SetupScroller()
{
	int boxX, boxY, boxWidth, boxHeight;
	
	if (!newScroll) {
		newScroll = new ListBoxScroll(this);
		newScroll->plane = plane;
		newScroll->Init();
		newScroll->SetPri(3);
	}
	
	if (boxView) {
		boxX = boxView->GetX();
		boxY = boxView->GetY();
		boxWidth = boxView->GetWidth();
		boxHeight = boxView->GetHeight();
	}
	else {
		boxX = boxRect.left;
		boxX = boxRect.top;
		boxWidth = boxRect.right - boxRect.left;
		boxHeight = boxRect.bottom - boxRect.top;
	}
	
	newScroll->Posn(boxX + boxWidth - newScroll->nextButton->GetWidth(), boxY + 2);
	newScroll->SetSize(16, boxHeight - 6);

	maxVisibleItems = boxHeight / 15;
	if (boxHeight % 15 <= 7)
		maxVisibleItems--;
}

sciView *sciListBox::GetBoxView()
{
	assert(boxView);
	
	return boxView;
}

void sciListBox::SetBoxPosn(int x, int y)
{
	if (!boxView)
		{ assert(FALSE); return; }
	
	if (!inited)
		Init();
	assert(boxView);
	boxView->Posn(x, y);
	
	if (!newScroll) {
		newScroll = new ListBoxScroll(this);
		newScroll->plane = plane;
		newScroll->Init();
		newScroll->SetPri(3);
		//newScroll->Posn(boxView->GetX() + boxView->GetWidth() - 18, boxView->GetY() + 2);
		//newScroll->Posn(boxView->GetX() + boxView->GetWidth() - newScroll->nextButton->GetWidth(), 
		//		boxView->GetY() + 2);
	}
	//newScroll->Posn(boxView->GetX() + boxView->GetWidth() - 18, boxView->GetY() + 2);
	//newScroll->Posn(boxView->GetX() + boxView->GetWidth() - newScroll->nextButton->GetWidth() - 5, 
	//			boxView->GetY() + 2);
}
void sciListBox::SetScrollPosn(int x, int y)
{
	if (newScroll)
		newScroll->Posn(x, y);
}
void sciListBox::SetScrollHeight(int newHeight)
{
	if (newScroll)
		newScroll->SetSize(newScroll->nextButton->GetWidth(), newHeight);
}
void sciListBox::SetTrueBoxWidth(int newWidth)
{
	int i;

	trueBoxWidth = newWidth;

	sciListText *tmpListText;
	for (i = 0; i < listBoxItems->Size(); ++i) {
		tmpListText = (sciListText *)listBoxItems->At(i);
		tmpListText->SetWidth(trueBoxWidth);
	}
}
void sciListBox::SetTextJustX(int newJustX)
{
	int i;

	xJustOffset = newJustX;

	sciListText *tmpListText;
	for (i = 0; i < listBoxItems->Size(); ++i) {
		tmpListText = (sciListText *)listBoxItems->At(i);
		tmpListText->SetJustOffsets(xJustOffset,0);
	}
}
int sciListBox::GetMaxVisibleItems()
{
	return(maxVisibleItems);
}

Boolean sciListBox::SelectItem(int itemID)
{
	sciListText *tmpListText;
	for (int i = 0; i < listBoxItems->Size(); ++i) {
		tmpListText = (sciListText *)listBoxItems->At(i);
		if (tmpListText->GetID() == itemID) {
			if (!tmpListText->IsSelected()) {
				tmpListText->Select();
			}
		} else if (tmpListText->IsSelected()) {
			tmpListText->Unselect();
		}
	}

	if (!IsSelectedItemVisible()) {
		if (GetFirstVisibleItemID() > itemID) {
			RefreshList(itemID);
		} else {
			RefreshList(itemID - (maxVisibleItems - 1));
		}
	}

	DoItemSelected(itemID);
	return(TRUE);
}

Boolean sciListBox::SelectItemByText(char *text, Boolean caseInsensitive)
{
	sciListText *tmpListText;
	int result, itemID = -1;
	
	for (int i = 0; i < listBoxItems->Size(); ++i) {
		tmpListText = (sciListText *)listBoxItems->At(i);
		if (caseInsensitive)
			result = stricmp(tmpListText->GetText(), text);
		else
			result = strcmp(tmpListText->GetText(), text);
		if (!result) {
			itemID = tmpListText->GetID();
			if (!tmpListText->IsSelected()) {
				tmpListText->Select();
			}
		} else if (tmpListText->IsSelected()) {
			tmpListText->Unselect();
		}
	}

	if (itemID == -1) {
		itemID = 0;
		((sciListText *)listBoxItems->At(itemID))->Select();
	}

	if (!IsSelectedItemVisible()) {
		if (GetFirstVisibleItemID() > itemID) {
			RefreshList(itemID);
		} else {
			RefreshList(itemID - (maxVisibleItems - 1));
		}
	}

	DoItemSelected(itemID);
	return(TRUE);
}

void sciListBox::UnselectAll()
{
	sciListText *tmpListText;
	for (int i = 0; i < listBoxItems->Size(); ++i) {
		tmpListText = (sciListText *)listBoxItems->At(i);
		if (tmpListText->IsSelected()) {
			tmpListText->Unselect();
		}
	}
}

int sciListBox::GetSelectedItemID()
{
	for (int i = 0; i < listBoxItems->Size(); ++i) {
		if (((sciListText *)listBoxItems->At(i))->IsSelected()) {
			return(((sciListText *)listBoxItems->At(i))->GetID());
		}
	}

	return(-1);
}

sciListText *sciListBox::GetSelectedItem()
{
	for (int i = 0; i < listBoxItems->Size(); ++i) {
		if (((sciListText *)listBoxItems->At(i))->IsSelected()) {
			return((sciListText *)listBoxItems->At(i));
		}
	}

	return(NULL);
}

void sciListBox::DoItemSelected(int itemID)
{
}

Boolean sciListBox::IsSelectedItemVisible()
{
	for (int i = 0; i < listBoxItems->Size(); ++i) {
		if (((sciListText *)listBoxItems->At(i))->IsSelected()) {
			return(i >= firstVisibleItem && i < firstVisibleItem + maxVisibleItems);
		}
	}

	return(FALSE);
}

int sciListBox::AddItem(sciListText *item)
{
	// Returns the ID number assigned to this item.
	int itemID = listBoxItems->Size();

	item->SetID(itemID);
	listBoxItems->AddToBack(item);
	item->SetOwner(this);
	item->plane = plane;
	item->SetFonts(font, highlightFont);
	item->SetWidth(trueBoxWidth);
	item->SetJustOffsets(xJustOffset,0);
	//item->SetFont(font);

	// Set number of hidden items based on max number of visible
	// items and number of items in the list.
	if (itemID >= maxVisibleItems) {
		numHiddenItems++;
		if (numHiddenItems == 1) {
			newScroll->Enable();
		}
	}
	
	return(itemID);
}

void sciListBox::RemoveItemByString(char *itemString, Boolean caseInsensitive)
{
	int i;
	sciListText *tmpListText;
	int result, removedItemID = -1;
	
	if(listBoxItems)
	{
		for(i = 0; i < listBoxItems->Size(); i++)
		{
			tmpListText = (sciListText *)listBoxItems->At(i);
			if (caseInsensitive)
				result = stricmp(itemString, tmpListText->GetText());
			else
				result = strcmp(itemString, tmpListText->GetText());
			if(result == 0) {
				removedItemID = tmpListText->GetID();
				tmpListText->Hide();
				listBoxItems->Delete(tmpListText);
				delete tmpListText;
				break;
			}
		}

		// Reset all IDs.
		for(i = 0; i < listBoxItems->Size(); i++)
		{
			((sciListText *)listBoxItems->At(i))->SetID(i);
		}

		// Set number of hidden items based on max number of visible
		// items and number of items in the list.
		if (listBoxItems->Size() >= maxVisibleItems) {
			numHiddenItems--;
			if (!numHiddenItems) {
				newScroll->Disable();
			}
		}

		// Select the item with the ID of the deleted item (or the last
		// item if the deleted one was previously the last item).
		if (removedItemID != -1 && listBoxItems->Size()) {
			if (listBoxItems->Size() == removedItemID) {
				SelectItem(removedItemID - 1);
			} else {
				SelectItem(removedItemID);
			}
		}

		// Finally, refresh the list
		RefreshList(firstVisibleItem);
	}
}

void sciListBox::RemoveItemByID(int itemID)
{
	int i;
	sciListText *tmpListText;
	int removedItemID = -1;

	if(listBoxItems)
	{
		for(i = 0; i < listBoxItems->Size(); i++)
		{
			tmpListText = (sciListText *)listBoxItems->At(i);
			if(tmpListText->GetID() == itemID) {
				removedItemID = itemID;
				tmpListText->Hide();
				listBoxItems->Delete(tmpListText);
				delete tmpListText;
				break;
			}
		}

		// Reset all IDs.
		for(i = 0; i < listBoxItems->Size(); i++)
		{
			((sciListText *)listBoxItems->At(i))->SetID(i);
		}

		// Set number of hidden items based on max number of visible
		// items and number of items in the list.
		if (listBoxItems->Size() >= maxVisibleItems) {
			numHiddenItems--;
			if (!numHiddenItems) {
				newScroll->Disable();
			}
		}

		// Select the item with the ID of the deleted item (or the last
		// item if the deleted one was previously the last item).
		if (removedItemID != -1 && listBoxItems->Size()) {
			if (listBoxItems->Size() == removedItemID) {
				SelectItem(removedItemID - 1);
			} else {
				SelectItem(removedItemID);
			}
		}

		// Finally, refresh the list
		RefreshList(firstVisibleItem);
	}
}

void sciListBox::RemoveAllItems()
{
	sciListText *tmpListText;
	
	if (!listBoxItems)
		return;
	
	while (listBoxItems->Size()) {
		tmpListText = (sciListText *)listBoxItems->At(0);
		tmpListText->Hide();
		listBoxItems->Delete(tmpListText);
		delete tmpListText;
	}
	
	firstVisibleItem = 0;
	numHiddenItems = 0;
	
	// Finally, refresh the list
	RefreshList(firstVisibleItem);
}

Boolean sciListBox::IsTextInList(char *newText, Boolean caseInsensitive)
{
	int result;
	
	if(listBoxItems)
	{
		for(int i = 0; i < listBoxItems->Size(); i++)
		{
			if (caseInsensitive)
				result = stricmp(newText, ((sciListText *)listBoxItems->At(i))->GetText());
			else
				result = strcmp(newText, ((sciListText *)listBoxItems->At(i))->GetText());
			if(result == 0)
				return TRUE;
		}
	}
	return FALSE;
}

char *sciListBox::GetSelectedText()
{
	if(GetSelectedItemID() == -1)
		return "";

	return GetTextByID(GetSelectedItemID());
}

char *sciListBox::GetTextByID(int itemID)
{
	sciListText *tmpListText;
	if(listBoxItems)
	{
		for(int i = 0; i < listBoxItems->Size(); i++)
		{
			tmpListText = (sciListText *)listBoxItems->At(i);
			if (tmpListText->GetID() == itemID) 
				return(tmpListText->GetText());
		}
	}
	return "";
}

int sciListBox::GetIDByText(char *text, Boolean caseInsensitive)
{
	int result;
	sciListText *tmpListText;
	
	if(listBoxItems)
	{
		for(int i = 0; i < listBoxItems->Size(); i++)
		{
			tmpListText = (sciListText *)listBoxItems->At(i);
			if (caseInsensitive)
				result = stricmp(tmpListText->GetText(), text);
			else
				result = strcmp(tmpListText->GetText(), text);
			if(!result)
				return(tmpListText->GetID());
		}
	}
	return -1;
}

sciListText *sciListBox::GetItemByID(int itemID)
{
	sciListText *tmpListText;
	if(listBoxItems)
	{
		for(int i = 0; i < listBoxItems->Size(); i++)
		{
			tmpListText = (sciListText *)listBoxItems->At(i);
			if (tmpListText->GetID() == itemID) 
				return(tmpListText);
		}
	}
	return(NULL);
}

sciListText *sciListBox::GetItemByText(char *text, Boolean caseInsensitive)
{
	int result;
	sciListText *tmpListText;
	
	if(listBoxItems)
	{
		for(int i = 0; i < listBoxItems->Size(); i++)
		{
			tmpListText = (sciListText *)listBoxItems->At(i);
			if (caseInsensitive)
				result = stricmp(tmpListText->GetText(), text);
			else
				result = strcmp(tmpListText->GetText(), text);
			if(!result)
				return(tmpListText);
		}
	}
	return(NULL);
}

int sciListBox::GetFirstVisibleItemID()
{
	sciListText *tmpListText;
	if(listBoxItems)
	{
		for(int i = 0; i < listBoxItems->Size(); i++)
		{
			tmpListText = (sciListText *)listBoxItems->At(i);
			if(tmpListText->IsNotHidden())
				return(tmpListText->GetID());
		}
	}
	return(NULL);
}

sciListText *sciListBox::GetFirstVisibleItem()
{
	sciListText *tmpListText;
	if(listBoxItems)
	{
		for(int i = 0; i < listBoxItems->Size(); i++)
		{
			tmpListText = (sciListText *)listBoxItems->At(i);
			if(tmpListText->IsNotHidden())
				return(tmpListText);
		}
	}
	return(NULL);
}

void sciListBox::Sort(int (*compare)(sciObject *, sciObject *))
{
	int i;
	
	listBoxItems->Sort(compare);
	
	// Reset all IDs.
	for(i = 0 ; i < listBoxItems->Size() ; i++)
		((sciListText *)listBoxItems->At(i))->SetID(i);
	
	RefreshList(0);
}
