#include "os.h"
#include "sci.h"

/* Example code:

	sciGroupBox *box;

	box = new sciGroupBox();
	box->SetOutlineColor1(22, 22, 22);
	box->SetOutlineColor2(64, 64, 64);
	box->SetInnerColor(146, 146, 146);
	box->SetPri(10);
	box->SetSize(100, 50);
	box->Posn(300, 50);
	box->SetText("This is a test.");
	box->Init();
	box->Show();

*/

sciGroupBox::sciGroupBox()
	{
	outline1 = NULL;
	outline2 = NULL;
	text = NULL;
	str = NULL;
	fontNumber = -1;
	innerColor.rgbRed = DEFAULT_GREY.rgbRed;
	innerColor.rgbGreen = DEFAULT_GREY.rgbGreen;
	innerColor.rgbBlue = DEFAULT_GREY.rgbBlue;
	outlineColor1.rgbRed = DEFAULT_LTGREY.rgbRed;
	outlineColor1.rgbGreen = DEFAULT_LTGREY.rgbGreen;
	outlineColor1.rgbBlue = DEFAULT_LTGREY.rgbBlue;
	outlineColor2.rgbRed = DEFAULT_DKGREY.rgbRed;
	outlineColor2.rgbGreen = DEFAULT_DKGREY.rgbGreen;
	outlineColor2.rgbBlue = DEFAULT_DKGREY.rgbBlue;
	}

sciGroupBox::~sciGroupBox()
	{
	if (str)
		delete[] str;
	}

void sciGroupBox::Init()
	{
	int w, h;

	sciFeature::Init();
	if (!outline1)
		outline1 = new sciRect;
	if (!outline2)
		outline2 = new sciRect;
	if (!text)
		text = new sciText;
	outline1->plane = plane;
	outline2->plane = plane;
	text->plane = plane;
	outline1->SetSize(GetWidth()-1, GetHeight()-1);
	outline1->Posn(GetX(), GetY());
	outline1->Init();
	outline2->SetSize(GetWidth()-1, GetHeight()-1);
	outline2->Posn(GetX()+1, GetY()+1);
	outline2->Init();
	text->Posn(GetX() + 10, GetY()-5);
	if (str) {
		text->SetText(str);
		w = text->CalculateFlatWidth();
		h = text->CalculateFlatHeight();
		text->SetWidth(min(w + 8, GetWidth() - 15));
		// text->SetWidth(w+8);
		text->SetHeight(h + 1);
	}
	text->SetBackgroundColor(innerColor);
	text->SetJust(JUST_HORIZONTAL | JUST_TOP);
	if (fontNumber != -1)
		text->SetFont(fontNumber);
	text->Init();
	SetInnerColor(innerColor);
	SetOutlineColor1(outlineColor1);
	SetOutlineColor2(outlineColor2);
	SetPri(GetPri());
	if (!hidden)
		Show();
	}

void sciGroupBox::SetInnerColor(BYTE red, BYTE green, BYTE blue)
	{
	innerColor.rgbRed = red;
	innerColor.rgbGreen = green;
	innerColor.rgbBlue = blue;
	if (outline2)
		outline2->SetInnerColor(innerColor);
	if (text)
		text->SetBackgroundColor(innerColor);
	}

void sciGroupBox::SetInnerColor(RGBQUAD color)
	{
	SetInnerColor(color.rgbRed, color.rgbGreen, color.rgbBlue);
	}

void sciGroupBox::SetOutlineColor1(BYTE red, BYTE green, BYTE blue)
	{
	outlineColor1.rgbRed = red;
	outlineColor1.rgbGreen = green;
	outlineColor1.rgbBlue = blue;
	if (outline1)
		{
		outline1->SetOutlineColor(outlineColor1);
		outline1->SetInnerColor(0, 0, 0);
		}
	}

void sciGroupBox::SetOutlineColor1(RGBQUAD color)
	{
	SetOutlineColor1(color.rgbRed, color.rgbGreen, color.rgbBlue);
	}

void sciGroupBox::SetOutlineColor2(BYTE red, BYTE green, BYTE blue)
	{
	outlineColor2.rgbRed = red;
	outlineColor2.rgbGreen = green;
	outlineColor2.rgbBlue = blue;
	if (outline2)
		{
		outline2->SetOutlineColor(outlineColor2);
		outline2->SetInnerColor(innerColor);
		}
	}

void sciGroupBox::SetOutlineColor2(RGBQUAD color)
	{
	SetOutlineColor2(color.rgbRed, color.rgbGreen, color.rgbBlue);
	}

void sciGroupBox::Posn(int x, int y)
	{
	sciFeature::Posn(x, y);
	}

void sciGroupBox::SetSize(int width, int height)
	{
	sciFeature::SetSize(width, height);
	}

void sciGroupBox::Show()
	{
	sciFeature::Show();
	if (outline1)
		outline1->Show();
	if (outline2)
		outline2->Show();
	if (text && str)
		text->Show();
	}

void sciGroupBox::Hide()
	{
	sciFeature::Hide();
	if (outline1)
		outline1->Hide();
	if (outline2)
		outline2->Hide();
	if (text)
		text->Hide();
	}

void sciGroupBox::SetPri(int newPri)
	{
	sciFeature::SetPri(newPri);
	if (outline1)
		outline1->SetPri(newPri+1);
	if (outline2)
		outline2->SetPri(newPri);
	if (text)
		text->SetPri(newPri+2);
	}

void sciGroupBox::SetText(char *newText)
	{
	int w, h;

	if (str)
		{
		delete[] str;
		str = NULL;
		}
	if (newText)
		{
		str = new char[strlen(newText)+1];
		strcpy(str, newText);
		if (text)
			{
			text->SetText(str);
			w = text->CalculateFlatWidth();
			h = text->CalculateFlatHeight();
			text->SetWidth(min(w + 8, GetWidth() - 15));
			// text->SetWidth(w+8);
			text->SetHeight(h + 1);
			}
		}
	}

void sciGroupBox::SetTextPri(int newPri)
	{
	if (text)
		text->SetPri(newPri);
	}

void sciGroupBox::SetFont(int newFontNumber)
	{
	fontNumber = newFontNumber;
	if (text)
		text->SetFont(fontNumber);
	}
