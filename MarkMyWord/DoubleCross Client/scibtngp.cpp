#include "os.h"
#include "scibtngp.h"
#include "assert.h"

sciRadioButton::sciRadioButton(int newID, Boolean newSelected)
 : sciCheckBox(newSelected)
{
	buttonID = newID;
	buttonGroupList = NULL;
}

sciRadioButton::~sciRadioButton()
{
}

Boolean sciRadioButton::HandleEvent(sciEvent &event)
{
	if (!enabled || hidden || event.claimed)
		return sciCheckBox::HandleEvent(event);

	Boolean bWasSelected = IsSelected();
	
	sciCheckBox::HandleEvent(event);

	//if selected button is clicked on we don't want to unselect it
	if(bWasSelected && !IsSelected())
	{
		Select();
		UpdateCel();
	}

	//if another button was selected we need to unselect all others
	sciRadioButton *radioButton;
	if(!bWasSelected && IsSelected())
	{
		for(int i = 0; i < buttonGroupList->Size(); i++)
		{
			radioButton = ((sciRadioButton *)(buttonGroupList->At(i)));
			if(radioButton != this && radioButton->IsSelected())
			{
				radioButton->Unselect();
				radioButton->UpdateCel();
			}
		}
		
		GotSelected();
	}
	
	return(event.claimed);
}

int sciRadioButton::GetID()
{
	return(buttonID);
}

void sciRadioButton::SetID(int newID)
{
	buttonID = newID;
}

void sciRadioButton::SetButtonGroupList(sciList *newButtonList)
{
	buttonGroupList = newButtonList;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

sciButtonGroup::sciButtonGroup()
{
	buttonList = NULL;
}
sciButtonGroup::~sciButtonGroup()
{
	if (buttonList) {
		buttonList->Release();
		delete buttonList;
		buttonList = NULL;
	}
}
Boolean sciButtonGroup::AddToGroup(sciRadioButton *newButton)
{
	if(!buttonList)
		buttonList = new sciList();

	if(!buttonList->Contains(newButton))
	{
		buttonList->AddToBack(newButton);
		newButton->SetButtonGroupList(buttonList);
		newButton->plane = plane;
	}
	return FALSE;
}

sciRadioButton *sciButtonGroup::GetSelectedButton(void)
{
	for(int i = 0; i < buttonList->Size(); i++)
	{
		if(((sciRadioButton *)(buttonList->At(i)))->IsSelected())
			return((sciRadioButton *)(buttonList->At(i)));
	}
	return NULL;
}

int sciButtonGroup::GetSelectedButtonID(void)
{
	for(int i = 0; i < buttonList->Size(); i++)
	{
		if(((sciRadioButton *)(buttonList->At(i)))->IsSelected())
			return(((sciRadioButton *)buttonList->At(i))->GetID());
	}
	return -1;
}

void sciButtonGroup::SelectButtonByID(int newID)
{
	// Select specified button and unselect others.  If ID is invalid,
	// everything gets unselected.

	for(int i = 0; i < buttonList->Size(); i++)
	{
		if(((sciRadioButton *)buttonList->At(i))->GetID() == newID) {
			((sciRadioButton *)buttonList->At(i))->Select();
			((sciRadioButton *)buttonList->At(i))->DoClick();
		} else {
			((sciRadioButton *)buttonList->At(i))->Unselect();
		}
	}
}

void sciButtonGroup::EnableAll()
{
	for(int i = 0; i < buttonList->Size(); i++)
		((sciRadioButton *)buttonList->At(i))->Enable();
}

void sciButtonGroup::DisableAll()
{
	for(int i = 0; i < buttonList->Size(); i++)
		((sciRadioButton *)buttonList->At(i))->Disable();
}

void sciButtonGroup::UnselectAll()
{
	for(int i = 0; i < buttonList->Size(); i++)
		((sciRadioButton *)buttonList->At(i))->Unselect();
}

void sciButtonGroup::Show()
{
	sciView::Show();
	for(int i = 0; i < buttonList->Size(); i++)
		((sciRadioButton *)(buttonList->At(i)))->Show();
}

void sciButtonGroup::Hide()
{
	for(int i = 0; i < buttonList->Size(); i++)
		((sciRadioButton *)(buttonList->At(i)))->Hide();
	sciView::Hide();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

sciTab::sciTab(int newID, int newLocation, int newSubGroupID, Boolean newSelected)
 : sciRadioButton(newID, newSelected)
{
	justification = JUST_HORIZONTAL | JUST_VERTICAL;
	featureList = NULL;
	xPri = 0;
	subGroupID = newSubGroupID;
	location = newLocation;
}

sciTab::~sciTab()
{
	if (featureList) {
		featureList->Release();
		delete featureList;
		featureList = NULL;
	}
}

void sciTab::Init()
{
	sciRadioButton::Init();
	
	if (text) {
		text->SetWidth(GetWidth());
		text->SetHeight(GetHeight());
	}
	
	SetCheckBoxCels(0, 0, 1, 0, 4, 4, 5, 4);
}

void sciTab::SetView(int newView)
{
	sciButton::SetView(newView);
}

void sciTab::SetXPri(short newXPri)
{
	xPri = newXPri;
	
	SetPri(GetY() + xPri);
}

short sciTab::GetXPri()
{
	return xPri;
}

void sciTab::AddToTab(sciFeature *newFeature)
{
	if(!featureList)
		featureList = new sciList();

	if(!featureList->Contains(newFeature))
	{
		featureList->AddToBack(newFeature);
		newFeature->plane = plane;
	}
}

void sciTab::RemoveFromTab(sciFeature *feature)
{
	if (featureList)
		featureList->Delete(feature);
}

void sciTab::Select()
{
	sciRadioButton::Select();
	SetFont(owner->bold, owner->boldHighlighted);//2, 3);
	Show();
	if (featureList) {
		for (int i = 0; i < featureList->Size(); ++i) {
			((sciFeature *)featureList->At(i))->Show();
		}
	}

	owner->BringRowToBottom(this);
	SetPri(TABGROUP_SELECTEDPRI);
}

void sciTab::Unselect()
{
	sciRadioButton::Unselect();
	SetFont(owner->normal, owner->normalHighlighted);//(5, 6);
	Show();
	if (featureList) {
		for (int i = 0; i < featureList->Size(); ++i) {
			((sciFeature *)featureList->At(i))->Hide();
		}
	}
}

int sciTab::GetSubGroupID()
{
	return(subGroupID);
}

int sciTab::GetLocation()
{
	return(location);
}

void sciTab::SetOwner(sciTabGroup *newOwner)
{
	owner = newOwner;
}

void sciTab::UpdateCel()
{
	sciCheckBox::UpdateCel();
	
	if (selected)
		if (state == DEFAULT_BUTTON_HILIGHTED || state == DEFAULT_BUTTON_DEPRESSED)
			if (text)
				text->SetFont(normalFont);
}

void sciTab::SetText(char *newText)
{
	sciButton::SetText(newText);
}

void sciTab::SetFont(int newFont, int newHiFont, int newDisabledFont)
{
	sciButton::SetFont(newFont, newHiFont, newDisabledFont);
}

Boolean sciTab::HandleEvent(sciEvent &event)
{
	if (!enabled || hidden || event.claimed)
		return sciRadioButton::HandleEvent(event);
	
	if (event.type == MOUSE_DOWN && sciFeature::OnMe(event)) {
		sciRadioButton::HandleEvent(event);
		event.type = MOUSE_UP;
		event.claimed = FALSE;
		
		return sciRadioButton::HandleEvent(event);
	}
	
	return sciRadioButton::HandleEvent(event);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

sciTabGroup::sciTabGroup()
{
	// numRows = 1;
}

sciTabGroup::~sciTabGroup()
{
}

Boolean sciTabGroup::AddToGroup(sciTab *newTab)
{
	Boolean bReturn = sciButtonGroup::AddToGroup(newTab);
	newTab->SetOwner(this);
	return(bReturn);
}

void sciTabGroup::SetTabFonts(int newNormal, int newNormalHighlighted, int newBold, int newBoldHighlighted)
{
	normal				= newNormal;
	normalHighlighted	= newNormalHighlighted;
	bold				= newBold;
	boldHighlighted		= newBoldHighlighted;
}

void sciTabGroup::BringRowToBottom(sciTab *member)
{
	short i, y, maxY = 0;
	sciTab *tab;
	
	for (i = 0; i < buttonList->Size(); i++) {
		tab = (sciTab *)buttonList->At(i);
		if (tab->GetY() > maxY)
			maxY = tab->GetY();
	}
	
	if (member->GetY() == maxY) {
		for (i = 0; i < buttonList->Size(); i++) {
			tab = (sciTab *)buttonList->At(i);
			tab->SetPri(tab->GetY() + tab->GetXPri());
		}
		
		return; // member's row is already at the bottom
	}
	
	y = member->GetY();
	
	for (i = 0; i < buttonList->Size(); i++) {
		tab = (sciTab *)buttonList->At(i);
		if (tab->GetY() == y)
			tab->Posn(tab->GetX(), maxY);
		else
			if (tab->GetY() == maxY)
				tab->Posn(tab->GetX(), y);
		tab->SetPri(tab->GetY() + tab->GetXPri());
	}
}
