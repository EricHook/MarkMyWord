#include "os.h"

#include "app.h"
#include "sci.h"
#include "game.h"

extern Application *pApp;
extern Game *game;

sciCheckBox::sciCheckBox(Boolean newSelected)
{
	normalSelected = 0;
	depressedSelected = 0;
	disabledSelected = 0;
	hilightedSelected = 0;
	normalNonSelected = 0;
	depressedNonSelected = 0;
	disabledNonSelected = 0;
	hilightedNonSelected = 0;

	normal = normalSelected;
	depressed = depressedSelected;
	disabled = disabledSelected;
	hilighted = hilightedSelected;
	normal = normalNonSelected;
	depressed = depressedNonSelected;
	disabled = disabledNonSelected;
	hilighted = hilightedNonSelected;

	if (newSelected) {
		Select();
	} else {
		Unselect();
	}
	justification = JUST_LEFT | JUST_VERTICAL;
	textXOffset = 0;
	textYOffset = 0;
	SetCheckBoxDefaultCels();
	UpdateCel();
}

sciCheckBox::~sciCheckBox()
{
}

void sciCheckBox::Init(int newWidth, int newHeight)
{
	sciProp::Init();
	
	if (text) {
		text->SetPri(GetPri() + 1);
		text->Posn(GetX(), GetY());
		text->Init();
		if (newWidth != 0)
			text->SetWidth(newWidth);
		if (newHeight != 0)
			text->SetHeight(newHeight);
	}
}

void sciCheckBox::SetView(int newView)
{
	sciProp::SetView(newView);
}

Boolean sciCheckBox::HandleEvent(sciEvent &event)
{
	if (!enabled || hidden || event.claimed)
		return sciButton::HandleEvent(event);

	Boolean bMouseWasDown = mouseDown;

	sciButton::HandleEvent(event);

	if (event.type == MOUSE_UP && (sciFeature::OnMe(event) || mouseDown)) {
		if ((event.modifiers & RIGHT_MOUSE) && bMouseWasDown && sciFeature::OnMe(event)) {
			if (selected) {
				Unselect();
			} else {
				Select();
			}
		}
		if ((event.modifiers & LEFT_MOUSE) && bMouseWasDown && sciFeature::OnMe(event)) {
			if (selected) {
				Unselect();
			} else {
				Select();
			}
		}
	}

	return(event.claimed);//(sciButton::HandleEvent(event));
}

void sciCheckBox::Select()
{
	selected = TRUE;
	UpdateCel();
}

void sciCheckBox::Unselect()
{
	selected = FALSE;
	UpdateCel();
}

Boolean sciCheckBox::IsSelected()
{
	return(selected);
}

void sciCheckBox::UpdateCel(void)
{
	if (selected) {
		normal = normalSelected;
		depressed = depressedSelected;
		disabled = disabledSelected;
		hilighted = hilightedSelected;
	} else {
		normal = normalNonSelected;
		depressed = depressedNonSelected;
		disabled = disabledNonSelected;
		hilighted = hilightedNonSelected;
	}
	sciButton::UpdateCel();
}

void sciCheckBox::SetCheckBoxCels(int newNormal, int newDepressed, int newDisabled,
 int newHilighted, int newNormalSelected, int newDepressedSelected,
 int newDisabledSelected, int newHilightedSelected)
{
	normalNonSelected = newNormal;
	depressedNonSelected = newDepressed;
	disabledNonSelected = newDisabled;
	hilightedNonSelected = newHilighted;
	normalSelected = newNormalSelected;
	depressedSelected = newDepressedSelected;
	disabledSelected = newDisabledSelected;
	hilightedSelected = newHilightedSelected;

	UpdateCel();
}

void sciCheckBox::SetCheckBoxDefaultCels()
{
	normalNonSelected = DEFAULT_BUTTON_NORMAL;
	depressedNonSelected = DEFAULT_BUTTON_DEPRESSED;
	disabledNonSelected = DEFAULT_BUTTON_DISABLED;
	hilightedNonSelected = DEFAULT_BUTTON_HILIGHTED;
	normalSelected = DEFAULT_BUTTON_NORMAL_SELECTED;
	depressedSelected = DEFAULT_BUTTON_DEPRESSED_SELECTED;
	disabledSelected = DEFAULT_BUTTON_DISABLED_SELECTED;
	hilightedSelected = DEFAULT_BUTTON_HILIGHTED_SELECTED;

	UpdateCel();
}

void sciCheckBox::SetText(char *newText)
{
	sciButton::SetText(newText);
	
	text->SetWidth(text->CalculateFlatWidth());
	text->SetHeight(text->CalculateFlatHeight());
}

void sciCheckBox::SetFont(int newFont, int newHiFont, int newDisabledFont)
{
	sciButton::SetFont(newFont, newHiFont, newDisabledFont);
	
	if (text) {
		text->SetFont(newFont);
		text->SetWidth(text->CalculateFlatWidth());
		text->SetHeight(text->CalculateFlatHeight());
	}
}

void sciCheckBox::SetTextOffset(int newXOffset, int newYOffset)
{
	textXOffset = newXOffset;
	textYOffset = newYOffset;
	//sciButton::UpdateText();
}

void sciCheckBox::Posn(int x, int y)
{
	sciButton::Posn(x,y);
	if(text)
	{
		text->Posn(x + textXOffset, y + textYOffset);
	}
}

Boolean sciCheckBox::OnMe(sciEvent &event)
{
	return OnMe(event.x, event.y);
}

Boolean sciCheckBox::OnMe(int x, int y)
{
	if (text && text->OnMe(x, y))
		return TRUE;
	
	return sciButton::OnMe(x, y);
}
