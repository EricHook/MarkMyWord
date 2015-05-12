#include "os.h"
#include "core.h"
#include "sci.h"

#define RENDER8BITFONTS

void ClipSomeRects(RECT &rcDest, RECT &rcDestClippingRect,
				RECT &rcSource, RECT &rcSourceClippingRect);

sciText::sciText()
	{
	fontNumber = -1;
	text = NULL;
	customLineSpacing = -1;
	backgroundColor.rgbRed = 0;
	backgroundColor.rgbGreen = 0;
	backgroundColor.rgbBlue = 0;
	sprite = NULL;
	alpha = 255;
	justX = 0;
	justY = 0;
	justEndX = 0;
	justEndY = 0;
	endFlag = FALSE;
	caret = NULL;
	SetWidth(128);
	SetHeight(64);
	SetJust(JUST_LEFT | JUST_TOP);
	SetFont(2);
#ifdef _DEBUG
	static Boolean firstTime = TRUE, textInBlue = FALSE;
	
	if (firstTime)
		textInBlue = pApp->pIniFile->ReadValue("TextInBlue", FALSE);
	
	firstTime = FALSE;
	if (textInBlue)
		SetBackgroundColor(166, 202, 240);
#endif

	caretsOn = FALSE;
	caretIndex = 0;
	colorMapping = NULL;
	HideCaret();
	}

sciText::~sciText()
	{
	if (text)
		delete[] text;
	if (colorMapping)
	{
		delete [] colorMapping->mappings;
		delete colorMapping;
	}
	if (sprite)
		pApp->pResourceManager->FreeResource(sprite);
	}

void sciText::ParamInit(int newFont, char *newText, int newWidth, int newHeight, int newX, int newY, 
			int newPri, int newJust)
{
	SetFont(newFont);
	SetText(newText);
	sciFeature::ParamInit(newX,newY,newPri);
	SetWidth(newWidth);
	SetHeight(newHeight);
	SetJust(newJust);
}

void sciText::SetColorMap(BYTE rFrom, BYTE gFrom, BYTE bFrom, BYTE rTo, BYTE gTo, BYTE bTo)
{
	int i;
	COLORMAP *newMappings;
	
	if (!colorMapping) {
		colorMapping = new COLORMAPPING;
		colorMapping->numMaps = 0;
		colorMapping->mappings = NULL;
	}
	
	// check if we need to remap a map that's already in this mapping
	
	if (colorMapping->mappings)
		for (i = 0 ; i < colorMapping->numMaps ; i++)
			if (rFrom == colorMapping->mappings[i].fromColor.r)
				if (gFrom == colorMapping->mappings[i].fromColor.g)
					if (bFrom == colorMapping->mappings[i].fromColor.b) {
						colorMapping->mappings[i].toColor.r = rTo;
						colorMapping->mappings[i].toColor.g = gTo;
						colorMapping->mappings[i].toColor.b = bTo;
						
						return;
					}
	
	colorMapping->numMaps++;
	
	newMappings = new COLORMAP[colorMapping->numMaps];
	
	for (i = 0 ; i < (colorMapping->numMaps - 1) ; i++)
		newMappings[i] = colorMapping->mappings[i];
	
	newMappings[i].fromColor.r = rFrom;
	newMappings[i].fromColor.g = gFrom;
	newMappings[i].fromColor.b = bFrom;
	newMappings[i].toColor.r = rTo;
	newMappings[i].toColor.g = gTo;
	newMappings[i].toColor.b = bTo;
	
	if (colorMapping->mappings)
		delete [] colorMapping->mappings;
	
	colorMapping->mappings = newMappings;
}

COLORMAPPING *sciText::GetColorMapping()
{
	return colorMapping;
}

void sciText::CopyColorMapping(COLORMAPPING *sourceMap)
{
	int i;

	if (!sourceMap)
	{
		if (colorMapping)
		{
			delete colorMapping->mappings;
			delete colorMapping;
			colorMapping = NULL;
		}
		return;
	}

	if (!colorMapping)
	{
		colorMapping = new COLORMAPPING;
		colorMapping->numMaps = 0;
		colorMapping->mappings = NULL;
	}
	else
	{
		delete [] colorMapping->mappings;
	}

	colorMapping->numMaps = sourceMap->numMaps;
	colorMapping->mappings = new COLORMAP[colorMapping->numMaps];

	for (i = 0; i < colorMapping->numMaps; i++)
	{
		colorMapping->mappings[i] = sourceMap->mappings[i];
	}
}

void sciText::SetWidth(int newWidth)
	{
	MarkAsUpdated();
	width = newWidth;
	sciRect::SetWidth(width);
	MarkAsUpdated();
	}

void sciText::SetHeight(int newHeight)
	{
	MarkAsUpdated();
	height = newHeight;
	sciRect::SetHeight(height);
	MarkAsUpdated();
	}

void sciText::SetFont(int newFont)
	{
	Resource *resource;
	FONT *pFont;
	int i;

	if (fontNumber == newFont)
		return;

	// Get the font properties.
	resource = pApp->pResourceManager->LoadResource(RES_FONT, newFont);
	if ((!resource) || (!resource->GetData()))
		{
		newFont = 2;
		resource = pApp->pResourceManager->LoadResource(RES_FONT, newFont);
		}
	fontNumber = newFont;
	pFont = (FONT *)(resource->GetData());
	if (pFont)
		{
		fontResource.bitmapID = pFont->bitmapID;
		fontResource.shadowWidth = pFont->shadowWidth;
		fontResource.shadowHeight = pFont->shadowHeight;
		fontResource.characterSpacing = pFont->characterSpacing;
		fontResource.lineSpacing = pFont->lineSpacing;
		fontResource.numChars = pFont->numChars;
		for (i = 0; i < fontResource.numChars; ++i)
			{
			fontResource.theChars[i] = pFont->theChars[i];
			}
		}
	else
		{
		fontResource.bitmapID = 0;
		fontResource.shadowWidth = 0;
		fontResource.shadowHeight = 0;
		fontResource.characterSpacing = 0;
		fontResource.lineSpacing = 0;
		fontResource.numChars = 0;
		for (i = 0; i < fontResource.numChars; ++i)
			{
			fontResource.theChars[i].top = 0;
			fontResource.theChars[i].left = 0;
			fontResource.theChars[i].bottom = 0;
			fontResource.theChars[i].right = 0;
			}
		}
	pApp->pResourceManager->FreeResource(resource);

	if (sprite)
		pApp->pResourceManager->FreeResource(sprite);
	sprite = (TSprite *)pApp->pResourceManager->LoadResource(RES_TSPRITE, fontResource.bitmapID);

	if(customLineSpacing != -1)
		fontResource.lineSpacing = customLineSpacing;

	MarkAsUpdated();
	}

void sciText::SetText(char *newText)
	{
	int strLen;

	if (!strcmp(newText,""))
		ResetCaretIndex();

	if (text && newText && !strcmp((char *)text, newText))
		return;
	if (text)
		delete[] text;
	if (newText)
		{
		strLen = strlen(newText);
		text = (unsigned char*) new char[strLen + 1];
		strcpy((char*)text, newText);
		}
	MarkAsUpdated();
	}

void sciText::SetTextToWidth(char *newText)
{
	char s[256];
	
	SetText(newText);
	if (strlen(newText) >= 255)
		return;
	
	strcpy(s, newText);
	while (s[0] && CalculateFlatWidth() >= width) {
		s[strlen(s) - 1] = 0;
		SetText(s);
	}
}

char* sciText::GetText()
	{

	if (text)
		return (char*)text;
	else
		return "";
	}

void sciText::SetBackgroundColor(BYTE red, BYTE green, BYTE blue)
	{
	SetInnerColor(red, green, blue);
	backgroundColor.rgbRed = red;
	backgroundColor.rgbGreen = green;
	backgroundColor.rgbBlue = blue;
	MarkAsUpdated();
	}

void sciText::SetBackgroundColor(RGBQUAD color)
	{
	SetBackgroundColor(color.rgbRed, color.rgbGreen, color.rgbBlue);
	}

void sciText::GetNextLine(int i, int *start, int *end, int *lineWidth)
	{
	int characterWidth, newWidth, lastSafeEnd, lastSafeWidth = 0;
	int charIdx;
		
	if (i > 0)
		while (text[i] == ' ')
			i++;
	
	*start = i;
	*end = i + 1;
	*lineWidth = 0;
	
	for ( ; text[i] ; i++)
		{
		charIdx = GetCharIdx(text[i]);
		characterWidth = 
			(fontResource.theChars[charIdx].right - fontResource.theChars[charIdx].left)
			+ 1 + fontResource.shadowWidth;
		newWidth = (*lineWidth) + characterWidth -
			fontResource.shadowWidth + fontResource.characterSpacing;
		if (text[i] == ' ')
			{
			lastSafeEnd = (*end);
			lastSafeWidth = (*lineWidth);
			}
		if (text[i] == '\n') {
			*end = i + 1;
			break;
		}
		if (newWidth > width - justX - justEndX)
			{
			if (lastSafeWidth)
				{
				*end = lastSafeEnd;
				*lineWidth = lastSafeWidth;
				}
			break;
			}
		*end = i + 1;
		*lineWidth = newWidth;
		}
	}

void sciText::CalculateSize(unsigned char *theText, int *theWidth, int *theHeight)
	{
	int i, n, start, end, lineWidth;
	
	*theWidth = 0;
	for (i = 0, n = 0 ; theText[i] ; i = end)
		{
		GetNextLine(i, &start, &end, &lineWidth);
		if (!theText[start])
			break;
		if (lineWidth > *theWidth)
			*theWidth = lineWidth;
		n++;
		}

	// Min height should be 1.
	if (n == 0)
		n = 1;

	*theHeight = n * fontResource.lineSpacing;
	*theWidth -= fontResource.shadowWidth;
	}

int sciText::CountLines()
	{
	int i, n, start, end, lineWidth;
	
	for (i = 0, n = 0 ; text[i] ; i = end)
		{
		GetNextLine(i, &start, &end, &lineWidth);
		if (!text[start])
			break;
		n++;
		}
	
	return n;
	}

int sciText::CalculateFlatWidth()
	{
	unsigned char c;
	int i, characterWidth, theWidth = 0;
	
	for ( i = 0 ; text[i] ; i++)
		{
		c = (text[i] == '\n') ? ' ' : text[i];
		c = GetCharIdx(c);
		characterWidth = fontResource.theChars[c].right + 1 + 
			fontResource.shadowWidth -
			fontResource.theChars[c].left;
		theWidth += characterWidth -
						fontResource.shadowWidth + fontResource.characterSpacing;
		}
	
	return theWidth;
	}

int sciText::CalculateFlatHeight()
	{
		return fontResource.lineSpacing;
	}


int sciText::GetFirstLineBreak()
	{
		int start, end, width;
		
		GetNextLine(0, &start, &end, &width);
		
		return end;
	}

//Sets the size of its fields(width,height) based upon the length of the longest line within 
//the 'text' string field. 
void sciText::AutoSize() {
	if (text) {
		int sizeWidth, sizeHeight;
		//temporarily expand width/height
		width = 640;
		height= 480;
		CalculateSize(text, &sizeWidth, &sizeHeight);
		sizeWidth += 2;	//add a little padding
		SetWidth(sizeWidth);
		int nTop = max(fontResource.theChars['y'].top, fontResource.theChars['Y'].top);
		int nBot = max(fontResource.theChars['y'].bottom, fontResource.theChars['Y'].bottom);
		sizeHeight += (int)((nBot - nTop) * .3);	//add a little padding
		SetHeight(sizeHeight);
	}
}

void sciText::SetJust(int newJust)
	{
	just = newJust;
	MarkAsUpdated();
	}

void sciText::SetJustOffsets(int x, int y)
	{
	justX = x;
	justY = y;
	MarkAsUpdated();
	}

void sciText::SetJustEndOffsets(int x, int y)
	{
	justEndX = x;
	justEndY = y;
	MarkAsUpdated();
	}

void sciText::SetLineSpacing(int newLineSpacing)
	{
	customLineSpacing = newLineSpacing;
	//Render(updateRect);
	}

void sciText::Init()
	{
	sciRect::Init();
	MarkAsUpdated();
	}

int sciText::GetMinWidth()
	{
	int w, h;

	CalculateSize(text, &w, &h);
	return w;
	}

int sciText::GetMinHeight()
	{
	int w, h;

	CalculateSize(text, &w, &h);
	return h;
	}

extern BYTE maxAlpha;

void sciText::RenderCaret(int leftPosition, int topPosition, RECT *clippedUpdateRect, int lineWidth)
{
	RECT sourceRect, destRect;
	RECT r1, r2, r3, r4;

	if(caretIndex == 0)
	{
		if (just & JUST_LEFT)
			leftPosition = 0;
		if (just & JUST_RIGHT)
			leftPosition = width - lineWidth;
		if (just & JUST_HORIZONTAL)
			leftPosition = (width - lineWidth) / 2;
		leftPosition += justX - 1;
	}

	if(caretIndex != 0)
		leftPosition--;

	if (caretShown && caretsOn && HaveFocus())
	{
		sourceRect.top = fontResource.theChars[0x7C].top;
		sourceRect.left = fontResource.theChars[0x7C].left;
		sourceRect.bottom = fontResource.theChars[0x7C].bottom + 1 + fontResource.shadowHeight;
		sourceRect.right = fontResource.theChars[0x7C].right + 1 + fontResource.shadowWidth;
		destRect.left = leftPosition;
		destRect.top = topPosition;
		destRect.right = leftPosition + (sourceRect.right - sourceRect.left);
		destRect.bottom = destRect.top + (sourceRect.bottom - sourceRect.top);

		OffsetRect(&destRect, left, top);

		// Do some clipping.
		r1 = destRect;
		r2 = *clippedUpdateRect;
		r3 = sourceRect;
		r4 = sourceRect;

		// Use r* because ClipSomeRects changes the rects passed in.
		// We need source and dest unchanged.
		ClipSomeRects(r1, r2, r3, r4);

		// Copy Pixels
		if (!IsRectEmpty(&r1))
			{
			maxAlpha = alpha;
			OffsetRect(&r1, plane->GetX(), plane->GetY());
			if (sprite->GetNumCels(0) == 1)
				sprite->CopyPixels(0, 0, plane->GetWindow()->frameBuffer, r1, r3, colorMapping);
			else
			{
				OffsetRect(&r3, -1 * r3.left, -1 * r3.top);
				sprite->CopyPixels(0, 0x7C - 32, plane->GetWindow()->frameBuffer, r1, r3, colorMapping);
			}
			maxAlpha = 255;
			}
	}
}

BOOL sciText::HaveFocus()
{
	sciPlane *loopPlane = plane;
	sciPlane *nextPlane;
	HWND hWnd;
	int i;
	
	if (IsHidden() || plane->IsHidden())
		return FALSE;
	
	while (loopPlane->GetParentPlane())
	{
		loopPlane = loopPlane->GetParentPlane();
	}
	
	hWnd = loopPlane->GetWindow()->hWnd;

	// if this plane is not on the active window, we cannot have the focus
	if (GetActiveWindow() != hWnd)
		return FALSE;

	// if the highest plane has the focus, and we are not that plane, we cannot
	// have the focus
	if (loopPlane->IsEditFocusPlane() && loopPlane != plane)
		return FALSE;

	// if the highest plane does not have the focus, and we are that plane, 
	// we cannot have the focus
	if (!loopPlane->IsEditFocusPlane())
	{
		if (loopPlane == plane)
			return FALSE;
		
		for (i = pApp->planes.Size() - 1 ; i >= 0 ; i--)
		{
			nextPlane = (sciPlane*)(pApp->planes.At(i));
			if (nextPlane->GetWindow() != loopPlane->GetWindow())
				continue;
			if (plane != nextPlane)
				return FALSE;
			break;
		}
	}

	return (plane->GetEditFocus() == this);	
}

void sciText::MoveCaret(int mouseX, int mouseY)
{
	int i, j;
	int topPosition = 0, leftPosition = 0, rightPosition = 0;
	int start, end, lineWidth = 0;
	int start2, end2, lineWidth2 = 0;
	int sizeWidth, sizeHeight;
	unsigned char charIdx;

	if (!text || !text[0])
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
		GetNextLine(end, &start2, &end2, &lineWidth2);

		if (!text[start])
			break;

		if (text[start2] && (mouseY < topPosition || mouseY >= topPosition + fontResource.lineSpacing))
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
			if (/* mouseX >= leftPosition && */ mouseX <= rightPosition)
			{
				if(mouseX < (leftPosition + rightPosition) / 2)
					caretIndex = j;
				else
					caretIndex = j + 1;
				return;
			}
			else
			{
				if(!text[j + 1] && mouseX > rightPosition)
				{
					caretIndex = j + 1;
					return;
				}
			}
			leftPosition += (fontResource.theChars[charIdx].right + 1 + 
					fontResource.shadowWidth - fontResource.theChars[charIdx].left) -
					fontResource.shadowWidth + fontResource.characterSpacing;
		}
		
		topPosition += fontResource.lineSpacing;
	}
}

void sciText::Render(RECT &updateRect)
	{
	int leftPosition, topPosition;
	int oldLeft, oldTop, oldWidth;
	int i, j = 0, start, end, lineWidth = 0;
	int sizeWidth, sizeHeight;
	int caretLineWidth = 0;
	RECT sourceRect, destRect, clippedUpdateRect;
	RECT r1, r2, r3, r4;
	static unsigned char chars[10] = {0x8e, 0xa1, 0xa5, 0xa8, 0xa9, 0xaa, 0xd2, 0xd3, 0xd4, 0xd5};
	int celNo, charIdx;

	int caretLeftPos = 0, caretTopPos = 0;
	
	if (!text || !inited || !sprite || IsHidden())
		return;
	
	sciRect::Render(updateRect);
	
	// Clip to updateRect.
	SetRect(&clippedUpdateRect, left, top, right, bottom);
	IntersectRect(&clippedUpdateRect, &clippedUpdateRect, &updateRect);
	if (IsRectEmpty(&clippedUpdateRect))
		return;
	
	// Justify vertical
	CalculateSize(text, &sizeWidth, &sizeHeight);
	if (just & JUST_TOP)
		topPosition = 0;
	if (just & JUST_BOTTOM)
		topPosition = height - sizeHeight;
	if (just & JUST_VERTICAL)
		topPosition = (height - sizeHeight) / 2;
	topPosition += justY;
	caretTopPos = topPosition;
	
	destRect.left = 0;
	destRect.top = 0;
	destRect.right = GetWidth();
	destRect.bottom = GetHeight();
	//sprite->Erase(destRect, backgroundColor);


	for (i = 0 ; text[i] ; i = end)
	{
		GetNextLine(i, &start, &end, &lineWidth);

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
		
		// deal with trailing whitespace in previous line
		while (j < start)
		{
			if(j == caretIndex)
			{
				caretLeftPos = oldLeft;
				caretTopPos = oldTop;
				caretLineWidth = oldWidth;
			}
			j++;
		}

		for (j = start ; j < end ; j++)
		{
			charIdx = GetCharIdx(text[j]);
			sourceRect.top = fontResource.theChars[charIdx].top;
			sourceRect.left = fontResource.theChars[charIdx].left;
			sourceRect.bottom = fontResource.theChars[charIdx].bottom + 1 +
				fontResource.shadowHeight;
			sourceRect.right = fontResource.theChars[charIdx].right + 1 + 
				fontResource.shadowWidth;

			if (sprite->GetNumCels(0) > 1)
				OffsetRect(&sourceRect, -1 * sourceRect.left, -1 * sourceRect.top);

			destRect.left = leftPosition;
			destRect.top = topPosition;
			destRect.right = leftPosition + (sourceRect.right - sourceRect.left);
			destRect.bottom = destRect.top + (sourceRect.bottom - sourceRect.top);

			OffsetRect(&destRect, left, top);

			// Do some clipping.
			r1 = destRect;
			r2 = clippedUpdateRect;
			r3 = sourceRect;
			r4 = sourceRect;

			// Use r* because ClipSomeRects changes the rects passed in.
			// We need source and dest unchanged.
			ClipSomeRects(r1, r2, r3, r4);

			// Copy Pixels
			if (!IsRectEmpty(&r1))
				{
				maxAlpha = alpha;
				OffsetRect(&r1, plane->GetX(), plane->GetY());
				if (sprite->GetNumCels(0) == 1)
					sprite->CopyPixels(0, 0, plane->GetWindow()->frameBuffer, r1, r3, colorMapping);
				else
				{
					celNo = charIdx - 32;
					if (celNo < 0)
						celNo = 0;
					sprite->CopyPixels(0, celNo, plane->GetWindow()->frameBuffer, r1, r3, colorMapping);
				}
				maxAlpha = 255;
				}
			
			// if this is the caret index, and we didn't already assign the positions
			if(j == caretIndex && !(endFlag && j == start))
			{
				caretLeftPos = leftPosition+1;
				caretTopPos = topPosition;
				caretLineWidth = lineWidth;
			}

			leftPosition += (sourceRect.right - sourceRect.left) -
				fontResource.shadowWidth + fontResource.characterSpacing;

			oldLeft = leftPosition+1;
			oldTop = topPosition;
			oldWidth = lineWidth;
		}
		if (j == caretIndex && endFlag)
		{
			caretLeftPos = leftPosition+1;
			caretTopPos = topPosition;
			caretLineWidth = lineWidth;
		}
		topPosition += fontResource.lineSpacing;
	}

	// if the caret is at the very end
	if (j == caretIndex)
	{
		if (j != 0)
		{
			caretLeftPos = leftPosition+1;
			if (text[j-1] == '\n' && topPosition + fontResource.lineSpacing > height)
			{
				text[j-2] = 0;
				text[j-1] = 0;
				SetText((char*)text);
			}
			if (text[j-1] == '\n')
			{
				caretTopPos = topPosition;
				if (just & JUST_LEFT)
					caretLeftPos = 0;
				if (just & JUST_RIGHT)
					caretLeftPos = width - lineWidth;
				if (just & JUST_HORIZONTAL)
					caretLeftPos = (width - lineWidth) / 2;
				caretLeftPos += justX+1;
			}
			else
			{
				caretTopPos = topPosition - fontResource.lineSpacing;
				caretLineWidth = oldWidth;
			}
		}
	}

	RenderCaret(caretLeftPos, caretTopPos, &clippedUpdateRect, caretLineWidth);

	sourceRect.top = GetY();
	sourceRect.left = GetX();
	sourceRect.bottom = sourceRect.top + GetHeight();
	sourceRect.right = sourceRect.left + GetWidth();
	
	top = sourceRect.top;
	left = sourceRect.left;
	bottom = sourceRect.bottom;
	right = sourceRect.right;
	}

unsigned char sciText::GetCharIdx(unsigned char theChar)
{
	if (theChar < 0x7F)
		return theChar;
	
#ifdef RENDER8BITFONTS
	if (theChar < 0xA0)
		return 0;
	
	return 0x7F + (theChar - 0xA0);
#else
	static unsigned char chars[10] = { 0x8e, 0xa1, 0xa5, 0xa8, 0xa9, 0xaa, 0xd2, 0xd3, 0xd4, 0xd5 };
	int specialChar;
	
	for (specialChar = 0 ; specialChar < 10 ; specialChar++)
		if (theChar == chars[specialChar])
			return 0x7F + specialChar;
	
	return 0;
#endif
}

void sciText::SetAlpha(BYTE newAlpha)
	{
	RECT rcWindow;
	SetRect(&rcWindow, left, top, right, bottom);

	if (inited && IsNotHidden() && alpha != newAlpha )
		{
		sciRect::SetAlpha(newAlpha);

		alpha = newAlpha;
		plane->GetWindow()->Paint(rcWindow);
		}
	}

void sciText::ShowCaret()
	{
	caretShown = TRUE;
	}

void sciText::HideCaret()
	{
	caretShown = FALSE;
	caretsOn = FALSE;
	}

void sciText::DrawCaret(BOOL on)
	{
	if (caretShown)
		{
		caretsOn = on;

		if (HaveFocus())
			MarkAsUpdated();
		}
	}

#define CARET_SPEED 30

CaretScript::CaretScript(sciText *newT) : sciScript()
	{
	t = newT;
	}

void CaretScript::ChangeState(int newState)
	{
	sciScript::ChangeState(newState);
	switch (state)
		{
		case 0:
			// Show caret.
			//((sciProp*)client)->Show();
			t->DrawCaret(1);
			SetTicks(CARET_SPEED);
			SetNextState(1);
			break;
		case 1:
			// Hide caret.
			//((sciProp*)client)->Hide();
			t->DrawCaret(0);
			SetTicks(CARET_SPEED);
			SetNextState(0);
			break;
		}
	}
