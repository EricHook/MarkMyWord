#include "os.h"
#include "window.h"
#include "sci.h"
#include "core.h"
#include "tickertext.h"

void ClipSomeRects(RECT &rcDest, RECT &rcDestClippingRect,
				RECT &rcSource, RECT &rcSourceClippingRect);

TickerText::TickerText()
{
	texts = NULL;
	numTexts = 0;
}

TickerText::~TickerText()
{
	int i;
	
	for (i = 0; i < numTexts; i++)
		delete texts[i];
	
	if (texts)
		delete [] texts;
}

void TickerText::Init(int theVOffset, int theLeftInset, int theRightInset)
{
	sciProp::Init();
	
	vOffset = theVOffset;
	leftInset = theLeftInset;
	rightInset = theRightInset;
	
	speed = 5;
	currentSegment = 0;
	
	SetScript(new TickerScript(this));
	
	// SetSegments("1 -1 -1 -1 String0 -c 200 0 200 4 -1 10 40 String1 -c 0 200 200 4 -1 -1 -1 String2 -c 100 100 30 4 4 -1 -1 \"Delete Me\" 4");
}

void TickerText::SetPri(int newPri)
{
	int i;
	
	sciProp::SetPri(newPri);
	
	for (i = 0 ; i < numTexts ; i++)
		texts[i]->SetPri(newPri + 1);
}

void TickerText::Show()
{
	int i, leftX, rightX, textX, textY;
	
	sciProp::Show();
	
	for (i = 0 ; i < numTexts ; i++)
		texts[i]->Hide();
	
	if (numTexts > 0 && currentSegment >= 0 && currentSegment < numTexts) {
		leftX = GetX() + GetBitmapX() + leftInset;
		rightX = leftX + GetWidth() - rightInset;
		textX = leftX + ((rightX - leftX) - texts[currentSegment]->GetWidth()) / 2;
		textY = GetBitmapY() + vOffset + GetY();
		if (texts[currentSegment]->GetFontNumber() == 2)
			textY += 1;
		texts[currentSegment]->Posn(textX, textY);
		texts[currentSegment]->Show();
	}
}

void TickerText::Hide()
{
	int i;
	
	sciProp::Hide();
	
	for (i = 0 ; i < numTexts ; i++)
		texts[i]->Hide();
}

/////////////////////////////////////////////////////////////////////////////////////////

void TickerText::SetSegments(char *segments)
{
	char *token;
	int i, n;
	
	if (texts) {
		for (i = 0 ; i < numTexts ; i++)
			delete texts[i];
		
		delete [] texts;
	}
	
	texts = NULL;
	numTexts = 0;
	
	GetToken(NULL);
	token = GetToken(segments); // increment (unused)
	if (!token)
		return;
	
	token = GetToken(segments); // speed (unused)
	if (!token)
		return;
	
	token = GetToken(segments);
	if (!token)
		return;
	
	sscanf(token, "%d", &n);
	
	for ( ; n > 0 ; n--)
		AddSegment(segments);
}

//======================================================================
// Format of ticker text is:
// maxLoops offtime ontime text -c r g b font
//======================================================================
bool TickerText::AddSegment(char *segment)
{
	bool gotColor = false;
	char szText[256], *token;
	int i, n, offTime, onTime, maxLoops, red, green, blue, font;
	TickerSegment *newText, **newTexts;
	
	token = GetToken(segment);
	if (!token)
		return false;
	sscanf(token, "%d", &maxLoops);
	
	token = GetToken(segment);
	if (!token)
		return false;
	sscanf(token, "%d", &offTime);
	
	token = GetToken(segment);
	if (!token)
		return false;
	sscanf(token, "%d", &onTime);
	
	token = GetToken(segment);
	if (!token)
		return false;
	strcpy(szText, token);
	
	token = GetToken(segment);
	if (!token)
		return false;
	
	if (!stricmp(token, "-c")) {
		token = GetToken(segment);
		if (!token)
			return false;
		sscanf(token, "%d", &red);		
		
		token = GetToken(segment);
		if (!token)
			return false;
		sscanf(token, "%d", &green);
		
		token = GetToken(segment);
		if (!token)
			return false;
		sscanf(token, "%d", &blue);
		
		token = GetToken(segment);
		if (!token)
			return false;
		
		gotColor = true;
	}
	
	sscanf(token, "%d", &font);
	
	numTexts++;
	newText = new TickerSegment();
	newTexts = new TickerSegment*[numTexts];
	for (i = 0 ; i < numTexts - 1 ; i++)
		newTexts[i] = texts[i];
	n = numTexts - 1;
	newTexts[n] = newText;
	if (texts)
		delete [] texts;
	texts = newTexts;
	
	texts[n]->SetMaxLoops(maxLoops);
	texts[n]->SetFont(font);
	
	/*
	if (onTime != -1 && offTime != -1) {
		prop = new sciProp;
		prop->plane = plane;
		prop->Init();
		prop->SetScript(new TickerTextScript(texts[n], offTime, onTime));
		texts[n]->SetProp(prop);
	}
	*/
	
	texts[n]->SetText(szText);
	
	texts[n]->SetPri(GetPri());
	texts[n]->plane = plane;
	texts[n]->Init();
	if (gotColor)
		texts[i]->SetColorMap(0, 0, 0, (BYTE)red, (BYTE)green, (BYTE)blue);
	texts[n]->AutoSize();
	texts[n]->SetWidth(texts[n]->CalculateFlatWidth());
	
	/*
	offset = texts[0]->GetX();
	
	for (i = 0 ; i < n ; i++)
		offset += texts[i]->GetWidth();
	
	texts[n]->Posn(offset, GetY() + GetBitmapY() + vOffset);
	*/
	
	Posn(GetX(), GetY());
	
	Show();
	
	return true;
}

bool TickerText::DeleteSegment(int index)
{
	int i, offset;
	
	if (index >= numTexts || index < 0)
		return false;
	
	offset = texts[index]->GetWidth();
	
	delete texts[index];
	
	for (i = index ; i < (numTexts - 1) ; i++)
		texts[i] = texts[i + 1];
	
	numTexts--;
	
	return true;
}

char *TickerText::GetToken(char *s)
{
	BOOL quote = FALSE, escape = FALSE;
	int q = 0;
	static char token[256];
	static char *p = NULL;
	
	if (s == NULL)
		{ p = NULL; return p; }
	
	if (p == NULL)
		p = s;
	
	// skip white space
	
	while (strchr(" \t\n\r", *p)) {
		if (*p == '\n' || *p == '\r' || *p == 0)
			{ p = NULL; return NULL; }
		p++;
	}
	
	if (*p == 0)
		{ p = NULL; return NULL; }
	
	if (*p == '"') {
		quote = TRUE;
		p++;
	}
	
	if (*p == '\\') {
		escape = TRUE;
		p++;
	}
	
	// while we have non-whitespace (or non-quotes), collect the token
	while ((escape || !(!quote && strchr(" \t=:", *p)) && !(quote && *p == '"')) && (*p != '\n'))
	{
		if (escape)
		{
			escape = FALSE;
			token[q++] = *p++;
		}
		else
		{
			if (*p == '\\')
			{
				escape = TRUE;
				p++;
			}
			else
			{
				escape = FALSE;
				token[q++] = *p++;
			}
		}
	}
	
	if (*p == '\n')
	{
		if (quote)
		{
			// DebugString("Newline in quoted string.\n");
			quote = FALSE;
		}
		p = NULL;
	}
	if (quote && (*p == '"'))
	{
		p++;
		quote = FALSE;
	}
	
	token[q] = 0;
	
	return token;
}

/////////////////////////////////////////////////////////////////////////////////////////

void TickerText::Cycle()
{
	if (numTexts == 0 || IsHidden())
		return;
	
	// check if the current segment has expired
	
	if (texts[currentSegment]->GetMaxLoops() != -1) {
		texts[currentSegment]->IncrNumLoops();
		if (texts[currentSegment]->GetNumLoops() >= texts[currentSegment]->GetMaxLoops()) {
			DeleteSegment(currentSegment);
			currentSegment--;
		}
	}
	
	currentSegment++;
	if (currentSegment >= numTexts)
		currentSegment = 0;
	
	if (numTexts == 0)
		{ Hide(); return; }
	
	Show();
}

/////////////////////////////////////////////////////////////////////////////////////////

TickerSegment::TickerSegment()
{
	SetPri(1000);
	maxLoops = numLoops = 0;
}

TickerSegment::~TickerSegment()
{
}

void TickerSegment::SetMaxLoops(int newMax)
{
	maxLoops = newMax;
}

void TickerSegment::IncrNumLoops()
{
	numLoops++;
}

int TickerSegment::GetNumLoops()
{
	return numLoops;
}

int TickerSegment::GetMaxLoops()
{
	return maxLoops;
}

/////////////////////////////////////////////////////////////////////////////////////////

TickerScript::TickerScript(TickerText *newT) : sciScript()
{
	t = newT;
}

void TickerScript::ChangeState(int newState)
{
	SetSeconds(t->speed);
	t->Cycle();
}
