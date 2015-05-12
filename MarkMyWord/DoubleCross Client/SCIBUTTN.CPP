#include "os.h"

#include "app.h"
#include "sci.h"
#include "game.h"

extern Application *pApp;
extern Game *game;

class sciButtonRepeatScript : public sciScript
	{
	public:
		Boolean rightClicking;

		sciButtonRepeatScript(Boolean newRightClicking) : sciScript(NULL)
			{
			rightClicking = newRightClicking;
			}

		void ChangeState(int newState)
			{
			sciScript::ChangeState(newState);
			switch (state)
				{
				case 0: //Setup
					SetTicks(((sciButton*)client)->startDelay);
					break;
				case 1:
					SetTicks(((sciButton*)client)->repeatDelay);
					--state;
					if (((sciButton*)client)->IsEnabled() &&
							((sciButton*)client)->IsNotHidden())
					{
						if (rightClicking)
							((sciButton*)client)->DoRightClick();
						else
							((sciButton*)client)->DoClick();
					}
					break;
				}
			}
	};

sciButton::sciButton()
{
	state		= DEFAULT_BUTTON_NORMAL;
	text		= NULL;
	SetButtonDefaultCels();
	mouseDown	= FALSE;
	enabled		= TRUE;
	hotkey		= 0;
	hotkey2		= 0;
	rightHotkey = 0;
	normalFont		= 2;
	hilightedFont	= 2;
	disabledFont	= 2;
	justification = JUST_HORIZONTAL | JUST_VERTICAL;
	startDelay	= -1;
	repeatDelay = -1;
	justX		= 0;
	justY		= 0;
	textXOffset = 0;
	textYOffset = 0;

}

sciButton::~sciButton()
{
	if(text) {delete text; text = NULL;}
}

void sciButton::Init(int newWidth, int newHeight)
{
	sciProp::Init();
	if(text)
	{
		if(!newWidth)
			newWidth = GetWidth();
		if(!newHeight)
			newHeight = GetHeight();
		SetWidth(newWidth);
		SetHeight(newHeight);
		text->SetPri(GetPri() + 1);
		textArea = GetRectNoAlpha();
		text->SetWidth(textArea.right - textArea.left + 1);
		text->SetHeight(textArea.bottom - textArea.top + 1);
		text->Posn(GetX() + textArea.left + textXOffset, GetY() + textArea.top + textYOffset);
		text->Init();
	}
}

void sciButton::ParamInit2(int newFont, char *newText, int newView, int newX, int newY, int newPri)
{
	sciView::ParamInit(newView, newX, newY, newPri);
	SetFont(newFont);
	SetText(newText);
}
void sciButton::SetButtonCels(int newNormal, int newDepressed, int newDisabled, int newHilighted)
{
	normal = newNormal;
	depressed = newDepressed;
	disabled = newDisabled;
	hilighted = newHilighted;
	UpdateCel();
}

void sciButton::SetButtonDefaultCels()
{
	normal = DEFAULT_BUTTON_NORMAL;
	depressed = DEFAULT_BUTTON_DEPRESSED;
	disabled = DEFAULT_BUTTON_DISABLED;
	hilighted = DEFAULT_BUTTON_HILIGHTED;
	UpdateCel();
}

Boolean sciButton::HandleEvent(sciEvent &event)
{
	if (!enabled || hidden)
		return FALSE;
	switch (event.type)
		{
		case MOUSE_DOWN:
			if ((!sciFeature::OnMe(event) && !mouseDown) || event.claimed)
				break;
			if ((event.modifiers & RIGHT_MOUSE) && !mouseDown)
			{
				mouseDown = TRUE;
				plane->CaptureEvents(this);
				SetState(DEFAULT_BUTTON_DEPRESSED);
				event.claimed = TRUE;
				if (startDelay != -1)
					SetScript(new sciButtonRepeatScript(TRUE));
			}
			if ((event.modifiers & LEFT_MOUSE) && !mouseDown)
			{
				mouseDown = TRUE;
				plane->CaptureEvents(this);
				SetState(DEFAULT_BUTTON_DEPRESSED);
				event.claimed = TRUE;
				if (startDelay != -1)
					SetScript(new sciButtonRepeatScript(FALSE));
			}
			break;
		case MOUSE_MOVE:
			if (mouseDown)
			{
				event.claimed = TRUE;
				if (!OnMe(event.x, event.y))
				{
					SetState(DEFAULT_BUTTON_NORMAL);
					SetScript(NULL);
				}
				else
					SetState(DEFAULT_BUTTON_DEPRESSED);
			}
			else
			{
				if (OnMe(event.x, event.y))
				{
					if (event.claimed)
						{
						SetState(DEFAULT_BUTTON_NORMAL);
						}
					else
						{
						SetState(DEFAULT_BUTTON_HILIGHTED);
						 event.claimed = TRUE; // commented out by MAK // uncommented out by PAH
						//MAK so that the chat window can be dragged over a character changer button
						//PAH just because 
						}
				}
				else
					SetState(DEFAULT_BUTTON_NORMAL);
			}
			break;
		case MOUSE_UP:
			if ((!sciFeature::OnMe(event) && !mouseDown) || event.claimed)
				break;
			SetScript(NULL);
			if ((event.modifiers & LEFT_MOUSE) && mouseDown)
			{
				plane->CaptureEvents(NULL);
				if (OnMe(event.x, event.y))
				{
					SetState(DEFAULT_BUTTON_HILIGHTED);
					DoClick();
				}
				else
				{
					SetState(DEFAULT_BUTTON_NORMAL);
				}
				mouseDown = FALSE;
				event.claimed = TRUE;
			}

			if ((event.modifiers & RIGHT_MOUSE) && mouseDown)
			{
				plane->CaptureEvents(NULL);
				if (OnMe(event.x, event.y))
				{
					SetState(DEFAULT_BUTTON_HILIGHTED);
					DoRightClick();
				}
				else
				{
					SetState(DEFAULT_BUTTON_NORMAL);
				}
				mouseDown = FALSE;
				event.claimed = TRUE;
			}
			break;
		case KEY_PRESS:
			if (event.claimed)
				break;
			if ((tolower(event.c) == hotkey) || (tolower(event.c) == hotkey2))
			{
				if (tolower(event.c) == hotkey)
					if (hkDeferToEditing && plane->GetWindow()->EditCursorIsFlashingInWindow())
						break;
				if (tolower(event.c) == hotkey2)
					if (hk2DeferToEditing && plane->GetWindow()->EditCursorIsFlashingInWindow())
						break;
				SetState(DEFAULT_BUTTON_DEPRESSED);
				plane->GetWindow()->ForceUpdate();
				Sleep(100);
				SetState(DEFAULT_BUTTON_NORMAL);
				DoClick();
				event.claimed = TRUE;
			}
			else if (tolower(event.c) == rightHotkey)
			{
				if (rhkDeferToEditing && plane->GetWindow()->EditCursorIsFlashingInWindow())
					break;
				SetState(DEFAULT_BUTTON_DEPRESSED);
				plane->GetWindow()->ForceUpdate();
				Sleep(100);
				SetState(DEFAULT_BUTTON_NORMAL);
				DoRightClick();
				event.claimed = TRUE;
			}
			break;
		}
	return event.claimed;
}

void sciButton::Enable()
{
	if (!enabled)
		{
		enabled = TRUE;
		mouseDown = FALSE;
		if (OnMe(plane->GetCursorX(), plane->GetCursorY()))
			SetState(DEFAULT_BUTTON_HILIGHTED);
		else
			SetState(DEFAULT_BUTTON_NORMAL);
		}
}

void sciButton::Disable()
{
	if (enabled)
		{
		enabled = FALSE;
		mouseDown = FALSE;
		SetScript(NULL);
		SetState(DEFAULT_BUTTON_DISABLED);
		}
}

Boolean sciButton::IsEnabled()
{
	return enabled;
}

void sciButton::DoClick(BOOL netMessage)
{
//	if (sigs)
//		sigs->Send(MENU_MESSAGE, (char *)&message, sizeof(message));
}

void sciButton::SetHotkey(char c, bool deferToEditing)
{
	hotkey = tolower(c);
	hkDeferToEditing = deferToEditing;
}

void sciButton::SetHotkey2(char c, bool deferToEditing)
{
	hotkey2 = tolower(c);
	hk2DeferToEditing = deferToEditing;
}

void sciButton::SetRightHotkey(char c, bool deferToEditing)
{
	rightHotkey = c;
	rhkDeferToEditing = deferToEditing;
}

void sciButton::SetState(int newState)
{
	if(state != newState)
	{
		state = newState;
		UpdateCel();
	}
}

int sciButton::GetState(void)
{
	return state;
}

void sciButton::UpdateCel()
{
	switch (state)
	{
		case DEFAULT_BUTTON_NORMAL:
			SetCel(normal);
			if(text)
				text->SetFont(normalFont);
			break;
		case DEFAULT_BUTTON_DEPRESSED:
			SetCel(depressed);
			if(text)
				text->SetFont(hilightedFont);
			break;
		case DEFAULT_BUTTON_DISABLED:
			SetCel(disabled);
			if(text)
				text->SetFont(disabledFont);
			break;
		case DEFAULT_BUTTON_HILIGHTED:
			SetCel(hilighted);
			if(text)
				text->SetFont(hilightedFont);
			break;
	}
}

void sciButton::DoRightClick()
{
}

void sciButton::SetRepeat(int newStartDelay, int newRepeatDelay)
{
	startDelay = newStartDelay;
	repeatDelay = newRepeatDelay;
}

void sciButton::SetText(char *newText)
{
	if (text == NULL) {
		text = new sciText();
		text->plane = plane;
		text->Init();
		text->SetText(newText);
		text->SetFont(normalFont);
		text->SetPri(GetPri() + 1);
		textArea = GetRectNoAlpha();
		text->SetWidth(textArea.right - textArea.left + 1);
		text->SetHeight(textArea.bottom - textArea.top + 1);
		text->Posn(GetX() + textArea.left + textXOffset, GetY() + textArea.top + textYOffset);
		text->SetJust(justification);
		text->SetJustOffsets(justX, justY);
		if(IsNotHidden())
			text->Show();
	}
	else
	{
		text->SetText(newText);
	}

	// TODO:  More stuff, e.g. resizing button rect.
}

char *sciButton::GetText()
{
	if(text)
		return text->GetText();
	
	return "";
}

void sciButton::SetLineSpacing(int newLineSpacing)
{
	if (text)
		text->SetLineSpacing(newLineSpacing);
}

void sciButton::SetFont(int newFont, int newHiFont, int newDisabledFont)
{
	normalFont = newFont;

	if (newHiFont != -1)
		hilightedFont = newHiFont;
	else
		hilightedFont = newFont;

	if (newDisabledFont != -1)
		disabledFont = newDisabledFont;
	else
		disabledFont = normalFont;

	if (text)
		text->SetFont(newFont);
}

void sciButton::ToggleFont(Boolean bHilighted)
{
	if (bHilighted)
		text->SetFont(hilightedFont);
	else
		text->SetFont(normalFont);
}

void sciButton::SetView(int newView)
{
	sciProp::SetView(newView);
	if(text) {
		textArea = GetRectNoAlpha();
		text->SetWidth(textArea.right - textArea.left + 1);
		text->SetHeight(textArea.bottom - textArea.top + 1);
		text->Posn(GetX() + textArea.left + textXOffset, GetY() + textArea.top + textYOffset);
	}
}
void sciButton::SetPri(int newPri)
{
	sciProp::SetPri(newPri);
	if (text)
		text->SetPri(newPri + 1);
}

void sciButton::Posn(int x, int y)
{
	sciProp::Posn(x, y);
	
	if (text)
		text->Posn(x + textArea.left + textXOffset, y + textArea.top + textYOffset);
}

void sciButton::TextPosn(int newTextXOffset, int newTextYOffset)
{
	textXOffset = newTextXOffset;
	textYOffset = newTextYOffset;
	
	Posn(GetX(), GetY());
}

int sciButton::GetTextFontNumber()
{
	if (text)
		return text->GetFontNumber();

	return -1;
}

void sciButton::SetTextJust(int newJust)
{
	justification = newJust;
	if (text)
		text->SetJust(newJust);
}

void sciButton::SetJustOffsets(int x, int y)
	{
	justX = x;
	justY = y;
	if (text)
		text->SetJustOffsets(justX, justY);
	}

void sciButton::SetAlpha(BYTE newAlpha)
{
	sciProp::SetAlpha(newAlpha);

	if (text)
		text->SetAlpha(newAlpha);
}

void sciButton::Show()
{
	//SetState(DEFAULT_BUTTON_NORMAL); // MAK 
	//Setting that state did bad things to Paul's buttons
	//he suggests you find another way to make buttons not show up blue
	//in hidden dialogs  //PAH
	sciProp::Show();
	if (text)
		text->Show();
}

void sciButton::Hide()
{
	mouseDown = FALSE;
	SetScript(NULL);
	sciProp::Hide();
	if (text)
		text->Hide();
}
