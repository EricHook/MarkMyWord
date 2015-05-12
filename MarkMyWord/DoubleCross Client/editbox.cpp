#include "os.h"
#include "window.h"
#include "sci.h"
#include "core.h"

EditBox::EditBox()
	{
	isUp = TRUE;
	status = EB_NORMAL;
	enabled = TRUE;
	mouseDown = FALSE;
	wantReturn = FALSE;
	hwnd = NULL;
	tdOutLine = NULL;
	highLights = NULL;
	SetPri(1000);
	capturing = FALSE;
	tdShown = FALSE;
	startSel = 0;
	endSel = 0;
	realStart = -1;
	tabID = 0;
	isLastTab = FALSE;
	rgbHighLightColor.rgbRed = 60;
	rgbHighLightColor.rgbGreen = 120;
	rgbHighLightColor.rgbBlue = 200;
	timer =0;
	}

EditBox::~EditBox()
	{
	if (plane->GetEditFocus() == this)
	{
		status = EB_DELETING;
		plane->SetEditFocus(NULL);
	}
	if (caret)
		delete caret;
	if (highLights)
		delete[] highLights;
	if (tdOutLine)
		delete tdOutLine;
	}

void EditBox::DoClick(BOOL netMessage)
	{
	if (isUp)
		{
		}
	else
		{
		}
	isUp = !isUp;
	}

Boolean EditBox::HandleEvent(sciEvent &event)
	{
	char textBuffer[256], oldText[256], update = FALSE, selected = FALSE, mouseMessage = FALSE;
	int i, localStart, localEnd, oldStart, oldEnd, oldCaret, lastChar;
	unsigned int tempSel;
	// DWORD tickCount;
	LPARAM lParam;
	WPARAM wParam;
	sciFeature *tmpFeature;
	// MSG dummyMSG;
	
	if (!enabled || event.claimed)
		return FALSE;
	
	wParam = (WPARAM)&localStart;
	lParam = (LPARAM)&localEnd;
	SendMessage(hwnd, EM_GETSEL, wParam, lParam);
	if (localStart != localEnd)
		selected = TRUE;
	else
		realStart = localStart;
	
	if (event.type == KEY_DOWN)
		if (event.wparam == VK_DELETE)
			event.type = KEY_PRESS;
	
	switch (event.type) {
		case KEY_PRESS:
 			if (IsEnabled() && IsNotHidden() && event.c != 27 && !(event.c == 13 && !wantReturn))
				{
				if (isUp)
					{
						if (event.c == 0x09)
						{
							if (HaveFocus())
							{
								for (i = 0; i < cast->Size(); ++i)
								{
									tmpFeature = (sciFeature*)(cast->At(i));
									if (tmpFeature != this && tmpFeature->HandleEvent(event) && event.claimed)
									{
										break;
									}
								}
							}
							else
							{
								if (GetKeyState(VK_SHIFT)&0xf000)
								{
									if ((plane->GetEditFocus()->GetTabID() == tabID + 1) ||	(plane->GetEditFocus()->GetTabID() == 0 && IsLastTab()))
									{
										SetFocus();
										event.claimed = TRUE;
										break;
									}
								}
								else if ((plane->GetEditFocus()->GetTabID() == tabID - 1) ||	(plane->GetEditFocus()->IsLastTab() && tabID == 0))
								{
									SetFocus();
									event.claimed = TRUE;
									break;
								}
							}
							break;
						}
						else if (HaveFocus())
						{
							// up and down arrows
							if (event.message == 0x100 && (event.wparam == 0x26 || event.wparam == 0x28))
							{
								FindNewCaretIndex(event.wparam == 0x26);
								if (GetKeyState(VK_SHIFT)&0xf000)
								{
									startSel = realStart;
								}
								else
								{
									startSel = caretIndex;
								}
								endSel = caretIndex;
								SendMessage(hwnd, EM_SETSEL, startSel, endSel);
								event.claimed = TRUE;
								update = TRUE;
								endFlag--;
								if (endFlag < 0)
									endFlag = FALSE;
							}
							// home and end
							else if (event.message == 0x100 && (event.wparam == 0x23 || event.wparam == 0x24) && !(GetKeyState(VK_CONTROL)&0xf000))
							{
								HomeOrEnd(event.wparam == 0x23);
								if (GetKeyState(VK_SHIFT)&0xf000)
								{
									startSel = realStart;
								}
								else
								{
									startSel = caretIndex;
								}
								endSel = caretIndex;
								SendMessage(hwnd, EM_SETSEL, startSel, endSel);
								event.claimed = TRUE;
								update = TRUE;
							}
							else
							{
								oldStart = startSel;
								oldEnd = endSel;
								oldCaret = caretIndex;
								strcpy(oldText, text ? (char *)text : "");
								SendMessage(hwnd, event.message, event.wparam, event.lparam);
								wParam = (WPARAM)&startSel;
								lParam = (LPARAM)&endSel;
								SendMessage(hwnd, EM_GETSEL, wParam, lParam);
								if (realStart == (int)endSel)
								{
									tempSel = endSel;
									endSel = startSel;
									startSel = tempSel;
								}
								caretIndex = endSel;
								event.claimed = TRUE;
								// DebugString("Got key press\n");
								endFlag = FALSE;
								update = TRUE;
							}
						}
					}
				}
			break;
		
		case MOUSE_DOWN:
			if(sciFeature::OnMe(event)) {
				// DebugString("Got mouse down\n");
				SetFocus();
				if (text)
				{
					MouseMove(event);
					startSel = caretIndex;
					realStart = caretIndex;
					endSel = startSel;
					capturing = TRUE;
					plane->CaptureEvents(this);
					event.claimed = TRUE;
					mouseMessage = TRUE;
					SendMessage(hwnd, EM_SETSEL, -1, 0);
					endFlag = FALSE;
					update = TRUE;
				}
			}
			break;
		
		case MOUSE_MOVE:
			if (capturing) {
				MouseMove(event);			
				endSel = caretIndex;
				SendMessage(hwnd, EM_SETSEL, startSel, endSel);
				// DebugString("Got mouse move\n");
				event.claimed = TRUE;
				update = TRUE;
				mouseMessage = TRUE;
			}
			break;
		
		case MOUSE_UP:
			// DebugString("Got mouse up\n");
			if (capturing) {
				event.claimed = TRUE;
				capturing = FALSE;
				plane->CaptureEvents(NULL);
				update = TRUE;
				mouseMessage = TRUE;
			}
			break;
	}
	
	/*
	tickCount = GetTickCount();
	
	if (timer > 0 && (tickCount >= timer))
		update = TRUE;
	*/
	
	if (update) {
		// since this update is expensive, and since there is likely to be another
		// edit key event in the queue, let's check and if so let's wait to update
		// when that next key is processed; but we can't rely on only that, so we'll set
		// a timer and update the edit box when the timer goes off no matter what
		
		/*
		if (PeekMessage(&dummyMSG, MAIN_HWND, WM_KEYFIRST, WM_KEYLAST, FALSE)) {
			timer = tickCount + 2000;
			
			return event.claimed;
		}
		*/
		
		SendMessage(hwnd, WM_GETTEXT, 255, (LPARAM)&textBuffer[0]);
		if (mouseMessage)
			SendMessage(hwnd, EM_SETSEL, startSel, endSel);
		SetText(textBuffer);
		if (!mouseMessage) {
			lastChar = GetLastRenderableChar();
			if (lastChar < (int)(text ? strlen((char *)text) - 1 : 0)) {
				startSel = oldStart;
				endSel = oldEnd;
				caretIndex = oldCaret;
				SetText(oldText);
				SendMessage(hwnd, WM_SETTEXT, 0, (LPARAM)text);
				SendMessage(hwnd, EM_SETSEL, startSel, endSel);
			}
		}
		// sprintf(textBuffer, "Startsel: %d, Endsel: %d\n", startSel, endSel);
		// DebugString(textBuffer);
		MarkAsUpdated();
		
		timer = 0;
	}
	
	return event.claimed;
}

void EditBox::SetFocus()
{
	sciPlane *loopPlane = plane;

	PositionCaret();

	if (plane)
	{
		plane->SetEditFocus(this);
	
		while (loopPlane->GetParentPlane())
		{
			loopPlane = loopPlane->GetParentPlane();
		}
		if (loopPlane == plane)
			plane->SetEditFocusPlane(TRUE);
		else
			loopPlane->SetEditFocusPlane(FALSE);
	}
}

void EditBox::LoseFocus()
{
	startSel = endSel = realStart = caretIndex = 0;
	endFlag = FALSE;
	SendMessage(hwnd, EM_SETSEL, -1, 0);
	SetText((char *)text);
	MarkAsUpdated();
}

int EditBox::GetTabID()
{
	return tabID;
}

void EditBox::SetTabID(int newTabID)
{
	tabID = newTabID;
}

Boolean EditBox::IsLastTab()
{
	return isLastTab;	
}

void EditBox::SetLastTab(Boolean newIsLastTab)
{
	isLastTab = newIsLastTab;
}

void EditBox::WantReturn(Boolean want)
{
	wantReturn = want;
}

void EditBox::MouseMove(sciEvent &event)
{
	int x, y;
	int sizeWidth, sizeHeight;
	int leftPosition, topPosition;

	CalculateSize(text, &sizeWidth, &sizeHeight);
	y = event.y;

	// Justify vertical
	if (just & JUST_TOP)
		topPosition = 0;
	if (just & JUST_BOTTOM)
		topPosition = height - sizeHeight;
	if (just & JUST_VERTICAL)
		topPosition = (height - sizeHeight) / 2;
	topPosition += justY;

	if (y < GetY())
		y = GetY();
	if (y > GetY() + topPosition + sizeHeight)
		y = GetY() + topPosition + sizeHeight;

	CalcWidth(&sizeWidth, y - GetY());
	x = event.x;

	if (just & JUST_LEFT)
		leftPosition = 0;
	if (just & JUST_RIGHT)
		leftPosition = width - sizeWidth;
	if (just & JUST_HORIZONTAL)
		leftPosition = (width - sizeWidth) / 2;
	leftPosition += justX;

	if (x < GetX())
		x = GetX();
	if (x > GetX() + sizeWidth)
		x = GetX() + sizeWidth;

	MoveCaret(x - GetX(), y - GetY());
}

int EditBox::GetLastRenderableChar()
{
	int topPosition;
	int i, start, end, lineWidth = 0;
	int sizeWidth, sizeHeight;

	if (!text || !inited || !sprite)
		return 0;
	
	// Justify vertical
	CalculateSize(text, &sizeWidth, &sizeHeight);
	if (just & JUST_TOP)
		topPosition = 0;
	if (just & JUST_BOTTOM)
		topPosition = height - sizeHeight;
	if (just & JUST_VERTICAL)
		topPosition = (height - sizeHeight) / 2;
	topPosition += justY;
	
	for (i = 0 ; text[i] ; i = end)
	{
		GetNextLine(i, &start, &end, &lineWidth);

		if (!text[start])
			break;

		topPosition += fontResource.lineSpacing;

		if (topPosition + fontResource.lineSpacing > height)
		{
			// DebugString("Truncating text\n");
			
			return end - 1;
		}
	}
	
	return (text ? strlen((char*)text) : 0);
}

void EditBox::FindNewCaretIndex(char isUp)
{
	int leftPosition, topPosition = 0;
	int i, j = 0, start, end, start2, end2, lineWidth = 0, lineWidth2;
	RECT sourceRect;
	int caretLeftPos = 0, caretTopPos = 0, nextLeftPos;
	char foundCaret = FALSE;
	char done = FALSE;
	unsigned char charIdx;
	
	if (!text || !inited)
		return;

	for (i = 0 ; text[i] && !done; i = end)
	{
		GetNextLine(i, &start, &end, &lineWidth);
		GetNextLine(end, &start2, &end2, &lineWidth2);

		if (!text[start])
			break;

		// if we haven't found the caret yet, and it isn't in this line
		if (!foundCaret && (caretIndex > end || (caretIndex == end && !endFlag && end == start2 && (text[start2] || text[end-1] == 10))))
		{
			j = end;
			topPosition += fontResource.lineSpacing;
			continue;
		}

		// Justify horizontal
		if (just & JUST_LEFT)
			leftPosition = 0;
		if (just & JUST_RIGHT)
			leftPosition = width - lineWidth;
		if (just & JUST_HORIZONTAL)
			leftPosition = (width - lineWidth) / 2;
		leftPosition += justX;
		
		for (j = start ; j < end+1 ; j++)
		{
			charIdx = GetCharIdx(text[j]);
			sourceRect.left = fontResource.theChars[charIdx].left;
			sourceRect.right = fontResource.theChars[charIdx].right + 1 + 
				fontResource.shadowWidth;

			nextLeftPos = leftPosition + (sourceRect.right - sourceRect.left) -
				fontResource.shadowWidth + fontResource.characterSpacing;

			if (foundCaret && nextLeftPos > caretLeftPos)
			{
				caretIndex = j;
				if (j == end/* && endFlag*/)
					endFlag = 2;
				done = TRUE;
				break;
			}
			if (j == caretIndex && !(endFlag && j == start))
			{
				caretLeftPos = leftPosition;
				caretTopPos = topPosition;
				foundCaret = TRUE;
				j = end;
				if (isUp)
					done = TRUE;
				break;
			}
			leftPosition = nextLeftPos;
		}
		if (!done)
		{
			topPosition += fontResource.lineSpacing;
			// if we manage to get two lines past the caret, because 
			// the first line after was not wide enough, then set the caret to
			// the last char of the previous line
			if (topPosition == caretTopPos + 2*fontResource.lineSpacing)
			{
				caretIndex = end;
				endFlag++;
				done = TRUE;
			}
			// if we didn't find the caret and we are at the end, and the line
			// ended with a crlf, we want to put the caret at the start of the next
			// line
			if (!text[start2] && text[end-1] != 10)
			{
				done = TRUE;
			}
		}
	}
	if (isUp)
	{
		// if we didn't find the caret, it is after a final crlf, on a line with no text
		if (!foundCaret)
		{
			caretTopPos = topPosition;
		}
		if (caretTopPos != 0)
		{
			done = FALSE;
			j = 0;
			topPosition = 0;
		}

		for (i = 0 ; text[i] && !done; i = end)
		{
			GetNextLine(i, &start, &end, &lineWidth);

			// if this line is more than one line higher than the caret
			if (caretTopPos - topPosition > fontResource.lineSpacing)
			{
				topPosition += fontResource.lineSpacing;
				continue;
			}

			// Justify horizontal
			if (just & JUST_LEFT)
				leftPosition = 0;
			if (just & JUST_RIGHT)
				leftPosition = width - lineWidth;
			if (just & JUST_HORIZONTAL)
				leftPosition = (width - lineWidth) / 2;
			leftPosition += justX;
			
			for (j = start ; j < end+1 ; j++)
			{
				charIdx = GetCharIdx(text[j]);
				sourceRect.left = fontResource.theChars[charIdx].left;
				sourceRect.right = fontResource.theChars[charIdx].right + 1 + 
					fontResource.shadowWidth;

				nextLeftPos = leftPosition + (sourceRect.right - sourceRect.left) -
					fontResource.shadowWidth + fontResource.characterSpacing;

				if (nextLeftPos > caretLeftPos && topPosition == caretTopPos - fontResource.lineSpacing)
				{
					caretIndex = j;
					if (j == end /*&& endFlag*/)
						endFlag = 2;
					done = TRUE;
					break;
				}

				leftPosition = nextLeftPos;
			}
			if (!done)
			{
				topPosition += fontResource.lineSpacing;
				// if we didn't find a place, and we are back on the caret line
				if (topPosition == caretTopPos)
				{
					caretIndex = end;
					endFlag = 2;
					done = TRUE;
				}
			}
		}
	}

	if (!done)
		caretIndex = end;
	
	return;
}

void EditBox::HomeOrEnd(char isEnd)
{
	int i, j = 0, start, end, start2, end2, lineWidth = 0;
	int oldEnd = 0, oldStart = 0;
	
	if (endFlag && isEnd)
		return;

	if (!text || !inited)
		return;

	for (i = 0 ; text[i] ; i = end)
	{
		GetNextLine(i, &start, &end, &lineWidth);
		GetNextLine(end, &start2, &end2, &lineWidth);

		if (!text[start])
			break;

		if (caretIndex >= start && (caretIndex < end || 
			((caretIndex == end) && (endFlag || (end != start2 || !text[start2])))))
		{
			if (isEnd)
				caretIndex = end;
			else
				caretIndex = start;
			break;
		}
		if (caretIndex < start && caretIndex > oldEnd)
		{
			if (isEnd)
				caretIndex = oldEnd;
			else
				caretIndex = oldStart;
			break;
		}
		oldEnd = end;
		oldStart = start;
	}
	endFlag = isEnd;
	return;
}

void EditBox::CalcWidth(int *maxLineWidth, int mouseY)
{
	int i, j;
	int topPosition = 0, leftPosition = 0, rightPosition = 0;
	int start, end, lineWidth = 0;
	int sizeWidth, sizeHeight;
	unsigned char charIdx;

	if (!text[0])
		return;

	CalculateSize(text, &sizeWidth, &sizeHeight);

	// Justify vertical
	if (just & JUST_TOP)
		topPosition = 0;
	if (just & JUST_BOTTOM)
		topPosition = height - sizeHeight;
	if (just & JUST_VERTICAL)
		topPosition = (height - sizeHeight) / 2;
	topPosition += justY;
	

	for (i = 0 ; text[i] ; i = end)
	{
		GetNextLine(i, &start, &end, &lineWidth);

		if (!text[start])
			break;

		if(mouseY < topPosition || mouseY > topPosition + fontResource.lineSpacing)
		{
			i = end + 1;
			topPosition += fontResource.lineSpacing;
			continue;
		}

		// Justify horizontal
		if (just & JUST_LEFT)
			leftPosition = 0;
		if (just & JUST_RIGHT)
			leftPosition = width - lineWidth;
		if (just & JUST_HORIZONTAL)
			leftPosition = (width - lineWidth) / 2;
		leftPosition += justX;
		
		for (j = start ; j < end ; j++)
		{
			charIdx = GetCharIdx(text[j]);
			rightPosition = leftPosition + fontResource.theChars[charIdx].right + 1 + 
				fontResource.shadowWidth - fontResource.theChars[charIdx].left;
			leftPosition += (fontResource.theChars[charIdx].right + 1 + 
					fontResource.shadowWidth - fontResource.theChars[charIdx].left) -
					fontResource.shadowWidth + fontResource.characterSpacing;
		}
		
		topPosition += fontResource.lineSpacing;
	}
	*maxLineWidth = leftPosition;
}

void EditBox::Render(RECT &updateRect)
{
	sciText::Render(updateRect);

	if (IsHidden())
		return;
	
	// we need to do this because the render may truncate some of the text
	SendMessage(hwnd, WM_SETTEXT, 256, (LPARAM)&text[0]);
	SendMessage(hwnd, EM_SETSEL, startSel, endSel);
	if (caretIndex > (int)(text ? strlen((char *)text) : 0))
		caretIndex = (int)(text ? strlen((char *)text) : 0);
}

void EditBox::Enable()
{
	if(!enabled)
	{
		enabled = TRUE;
		if(caret)
		{
			ShowCaret();
			MarkAsUpdated();
			caret->SetScript(new CaretScript(this));
			caret->script->SetTicks(0);
		}
	}
}
void EditBox::Disable()
{
	if(enabled)
	{
		enabled = FALSE;
		if(caret)
		{
			HideCaret();
			MarkAsUpdated();
			caret->SetScript(NULL);
		}
	}
}

Boolean EditBox::IsEnabled()
	{
	return enabled;
	}

void EditBox::Posn(int posnX, int posnY)
	{
	sciText::Posn(posnX, posnY);
	if (tdOutLine)
		tdOutLine->Posn(posnX, posnY);
	PositionCaret();
	}

void EditBox::Show()
	{
	sciText::Show();
	
	HighLight();

	if (tdOutLine && tdShown)
	{
		tdOutLine->SetSize(GetWidth(), GetHeight());
		tdOutLine->Show();
	}
	
	if(caret && enabled)
		{
		ShowCaret();
		caret->SetScript(new CaretScript(this));
		caret->script->SetTicks(0);
		}
	}

void EditBox::Hide()
	{
	int i;

	sciText::Hide();
	
	for (i = 0; i < 16; i++)
	{
		highLights[i].Hide();
	}
	
	if (tdOutLine)
	{
		tdOutLine->Hide();
	}

	if(caret)
		{
		caret->SetScript(NULL);
		HideCaret();
		}
	}

void EditBox::SetSize(int width, int height)
{
	sciText::SetSize(width, height);
	if (tdOutLine)
		tdOutLine->SetSize(width, height);
}

void EditBox::SetPri(int newPri)
{
	sciText::SetPri(newPri);
	if (tdOutLine)
		tdOutLine->SetPri(newPri+1);
}

void EditBox::Init()
{
	int i;
	RGBQUAD grey, white;
	
	sciText::Init();
	caret = new sciProp();
	caret->plane = plane;
	caret->SetView(2);
	PositionCaret();
	caret->Init();
	caret->SetPri(1001);
	SetJustOffsets(3, 1);
	SetJustEndOffsets(3, 0);
	highLights = new sciRect[16];
	hwnd = CreateWindow("EDIT", "",
			  WS_CHILD | WS_DISABLED | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE | ES_WANTRETURN | ES_LEFT | ES_NOHIDESEL,
			  0, 0, 35, 64,
			  MAIN_HWND,
			  (HMENU)1001,
			  hInstance,
			  NULL);
	for (i = 0; i < 16; i++)
	{
		highLights[i].plane = plane;
		highLights[i].Init();
	}
	
	tdOutLine = new FourSidedRect();
	tdOutLine->plane = plane;
	tdOutLine->Init();
	grey.rgbRed = grey.rgbGreen = grey.rgbBlue = 120;
	white.rgbRed = white.rgbGreen = white.rgbBlue = 255;
	tdOutLine->SetSideColors(grey, grey, white, white);

	SetText("");
}

void EditBox::PositionCaret()
	{
	if (caret)
		{
		if (caret->script)
			{
			caret->script->SetNextState(0);
			caret->script->SetTicks(0);
			}
		}
	}
void EditBox::SetCaretView(int newView)
	{
	if(caret)
		{
		caret->SetView(newView);
		PositionCaret();
		}
	}
void EditBox::SetMaxLength(int l)
	{
	maxLength = l;
	}

void EditBox::SetText(char *newText)
{
	sciText::SetText(newText);

	PositionCaret();

	HighLight();
}

void EditBox::Set3DOutLine(Boolean show)
{
	tdShown = show;
}

void EditBox::HighLight()
{
	int leftPosition, topPosition;
	int i, j = 0, start, end, lineWidth = 0;
	int sizeWidth, sizeHeight;
	RECT sourceRect, destRect;
	int hiX = 0, hiY = 0, hiWidth = 0;
	int lineNo = 0;
	char on = 0;
	int extraSpace = 0;
	unsigned int realStart, realEnd;
	unsigned char charIdx;

	if (!text || !inited)
		return;

	if (startSel > endSel)
	{
		realStart = endSel;
		realEnd = startSel;
	}
	else
	{
		realEnd = endSel;
		realStart = startSel;
	}

	// Justify vertical
	CalculateSize(text, &sizeWidth, &sizeHeight);
	if (just & JUST_TOP)
		topPosition = 0;
	if (just & JUST_BOTTOM)
		topPosition = height - sizeHeight;
	if (just & JUST_VERTICAL)
		topPosition = (height - sizeHeight) / 2;
	topPosition += justY;
	
	destRect.left = 0;
	destRect.top = 0;
	destRect.right = GetWidth();
	destRect.bottom = GetHeight();

	for (i = 0 ; text[i] ; i = end)
	{
		GetNextLine(i, &start, &end, &lineWidth);

		// deal with trailing whitespace on last line
		while (j < start)
		{
			if (on) 
			{
				if (j == (int)realEnd)
				{
					on = FALSE;
				}
			}
			else if (j == (int)realStart && j != (int)realEnd)
			{
				on = TRUE;
			}
			j++;
		}
		
		if (!text[start])
			break;

		// Justify horizontal
		if (just & JUST_LEFT)
			leftPosition = 0;
		if (just & JUST_RIGHT)
			leftPosition = width - lineWidth;
		if (just & JUST_HORIZONTAL)
			leftPosition = (width - lineWidth) / 2;
		leftPosition += justX;
		
		for (j = start ; j < end ; j++)
		{
			charIdx = GetCharIdx(text[j]);
			sourceRect.top = fontResource.theChars[charIdx].top;
			sourceRect.left = fontResource.theChars[charIdx].left;
			sourceRect.bottom = fontResource.theChars[charIdx].bottom + 1 +
				fontResource.shadowHeight;
			sourceRect.right = fontResource.theChars[charIdx].right + 1 + 
				fontResource.shadowWidth;
			destRect.left = leftPosition;
			destRect.top = topPosition;
			destRect.right = leftPosition + (sourceRect.right - sourceRect.left);
			destRect.bottom = destRect.top + (sourceRect.bottom - sourceRect.top);

			OffsetRect(&destRect, left, top);

			if (j == (int)realStart || (on && j == start))
			{
				on = TRUE;
				hiX = destRect.left;
				hiY = destRect.top + 1;
				hiY -= extraSpace;
			}
			if (on) 
			{
				if (j == (int)realEnd)
				{
					hiWidth = destRect.left - hiX;
					on = FALSE;
				}
				else if (j == end - 1)
				{
					hiWidth = destRect.right + 1 - hiX;
				}
			}
	
			leftPosition += (sourceRect.right - sourceRect.left) -
				fontResource.shadowWidth + fontResource.characterSpacing;

		}
		if (hiWidth)
		{
			highLights[lineNo].SetPri(GetPri()-1);
			highLights[lineNo].SetInnerColor(rgbHighLightColor);
			highLights[lineNo].Posn(hiX, hiY);
			highLights[lineNo].SetSize(hiWidth, fontResource.theChars['j'].bottom - fontResource.theChars['j'].top + extraSpace);
			highLights[lineNo].Show();
		}
		else
			highLights[lineNo].Hide();
	
		lineNo++;
		extraSpace = fontResource.lineSpacing - (fontResource.theChars['j'].bottom - fontResource.theChars['j'].top);
		hiX = hiY = hiWidth = 0;
	
		topPosition += fontResource.lineSpacing;
	}
	for (i = lineNo; i < 16; i++)
	{
		highLights[i].Hide();
	}

}

void EditBox::SetHighLightColor(BYTE red, BYTE green, BYTE blue)
	{
	RGBQUAD rgbQuad;

	rgbQuad.rgbRed = red;
	rgbQuad.rgbGreen = green;
	rgbQuad.rgbBlue = blue;
	SetHighLightColor(rgbQuad);
	}

void EditBox::SetHighLightColor(RGBQUAD color)
	{
	if ((color.rgbRed == 0) && (color.rgbGreen == 0) && (color.rgbBlue == 0))
		innerColor = FALSE;
	else
		innerColor = TRUE;
	rgbHighLightColor = color;
	MAIN_WINDOW->pCurrentPalette->DoGammaCorrection(&rgbHighLightColor);
	MarkAsUpdated();
	}
