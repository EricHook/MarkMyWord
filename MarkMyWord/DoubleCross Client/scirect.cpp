#include "os.h"
#include "core.h"
#include "sci.h"

sciRect::sciRect()
	{
	innerColor = FALSE;
	outlineColor = FALSE;
	alpha = 255;
	transferMode = ALPHA_FLAG;
	rgbInnerColor.rgbRed = 0;
	rgbInnerColor.rgbGreen = 0;
	rgbInnerColor.rgbBlue = 0;
	rgbInnerColor.rgbReserved = 0;
	rgbOutlineColor.rgbRed = 0;
	rgbOutlineColor.rgbGreen = 0;
	rgbOutlineColor.rgbBlue = 0;
	rgbOutlineColor.rgbReserved = 0;
	}

sciRect::~sciRect()
	{
	}

void sciRect::Render(RECT &updateRect)
	{
	int y;
	RECT clippedUpdateRect;
	TEX_PIXEL tmpPixel;
	RGBA rgba;
	
	if (IsHidden())
		return;
	SetRect(&clippedUpdateRect, left, top, right, bottom);
	IntersectRect(&clippedUpdateRect, &clippedUpdateRect, &updateRect);

	// Inner color
	tmpPixel = RGBQUADtoTEX_PIXEL(rgbInnerColor);
	rgba.r = rgbInnerColor.rgbRed;
	rgba.g = rgbInnerColor.rgbGreen;
	rgba.b = rgbInnerColor.rgbBlue;
	rgba.a = GetAlpha();
	if (innerColor)
		{
		for (y = clippedUpdateRect.top; y < clippedUpdateRect.bottom; ++y)
			{
			plane->DrawHLine(clippedUpdateRect.left,
				clippedUpdateRect.right-1, y, rgba, GetTransferMode());
			}
		}

	tmpPixel = RGBQUADtoTEX_PIXEL(rgbOutlineColor);
	rgba.r = rgbOutlineColor.rgbRed;
	rgba.g = rgbOutlineColor.rgbGreen;
	rgba.b = rgbOutlineColor.rgbBlue;
	rgba.a = GetAlpha();
	if (outlineColor)
		{
		// Top outline
		if (clippedUpdateRect.top == top)
			{
			plane->DrawHLine(clippedUpdateRect.left,
				clippedUpdateRect.right-1, top, rgba, GetTransferMode());
			}

		// Bottom outline
		if ((clippedUpdateRect.bottom-1) == (bottom-1))
			{
			plane->DrawHLine(clippedUpdateRect.left,
				clippedUpdateRect.right-1, bottom-1, rgba, GetTransferMode());
			}

		// Left outline
		if (clippedUpdateRect.left == left)
			{
			plane->DrawVLine(left,
				clippedUpdateRect.top, clippedUpdateRect.bottom-1, rgba, GetTransferMode());
			}

		// Right outline
		if ((clippedUpdateRect.right-1) == (right-1))
			{
			plane->DrawVLine((right-1),
				clippedUpdateRect.top, clippedUpdateRect.bottom-1, rgba, GetTransferMode());
			}
		}
	}

void sciRect::SetInnerColor(BYTE red, BYTE green, BYTE blue)
	{
	RGBQUAD rgbQuad;

	rgbQuad.rgbRed = red;
	rgbQuad.rgbGreen = green;
	rgbQuad.rgbBlue = blue;
	SetInnerColor(rgbQuad);
	}

void sciRect::SetOutlineColor(BYTE red, BYTE green, BYTE blue)
	{
	RGBQUAD rgbQuad;

	rgbQuad.rgbRed = red;
	rgbQuad.rgbGreen = green;
	rgbQuad.rgbBlue = blue;
	SetOutlineColor(rgbQuad);
	}

void sciRect::SetInnerColor(RGBQUAD color)
	{
	if ((color.rgbRed == 0) && (color.rgbGreen == 0) && (color.rgbBlue == 0))
		innerColor = FALSE;
	else
		innerColor = TRUE;
	rgbInnerColor = color;
	MAIN_WINDOW->pCurrentPalette->DoGammaCorrection(&rgbInnerColor);
	MarkAsUpdated();
	}

void sciRect::SetOutlineColor(RGBQUAD color)
	{
//	if ((color.rgbRed == 0) && (color.rgbGreen == 0) && (color.rgbBlue == 0))
//		outlineColor = FALSE;
//	else
		outlineColor = TRUE;
	rgbOutlineColor = color;
	MAIN_WINDOW->pCurrentPalette->DoGammaCorrection(&rgbOutlineColor);
	MarkAsUpdated();
	}

void sciRect::ClearOutlineColor()
	{
	outlineColor = FALSE;
	}

void sciRect::Posn(int x, int y)
	{
	MarkAsUpdated();
	sciFeature::Posn(x, y);
	MarkAsUpdated();
	}

void sciRect::SetSize(int width, int height)
	{
	MarkAsUpdated();
	sciFeature::SetSize(width, height);
	MarkAsUpdated();
	}

void sciRect::Posn(const RECT &r)
{
	MarkAsUpdated();
	sciFeature::Posn(r.left, r.top);
	sciFeature::SetSize(r.right - r.left, r.bottom - r.top);
	MarkAsUpdated();
}

void sciRect::Hide()
	{
	if (IsNotHidden())
		{
		sciFeature::Hide();
		if (inited)
			{
			MarkAsUpdated();
			}
		}
	}

void sciRect::Show()
	{
	if (IsHidden())
		{
		sciFeature::Show();
		if (inited)
			{
			MarkAsUpdated();
			}
		}
	}

void sciRect::SetAlpha(BYTE newAlpha)
	{
	RECT rcWindow;

	alpha = newAlpha;
	if (inited && IsNotHidden())
		{
		SetRect(&rcWindow, left, top, right, bottom);
		plane->MarkAsUpdated(rcWindow);
		}
	}

BYTE sciRect::GetAlpha()
	{
	return alpha;
	}

void sciRect::SetTransferMode(int mode)
	{
	transferMode = mode;
	}

int sciRect::GetTransferMode()
	{
	return transferMode;
	}

////////////////////////////////////////////////////////////////////////////////////////////////////

FourSidedRect::FourSidedRect()
{
	int i;
	
	for (i = 0 ; i < 4 ; i++)
		sides[i] = NULL;
}

FourSidedRect::~FourSidedRect()
{
	int i;
	
	for (i = 0 ; i < 4 ; i++)
		if (sides[i]) { delete sides[i]; sides[i] = NULL; }
}

void FourSidedRect::Init()
{
	int i;
	
	sciFeature::Init();
	
	for (i = 0 ; i < 4 ; i++) {
		sides[i] = new sciRect();
		sides[i]->plane = plane;
		sides[i]->Init();
	}
}

void FourSidedRect::SetSize(int width, int height)
{
	sciFeature::SetSize(width, height);
	
	if (IsNotHidden())
		Show();
}

void FourSidedRect::Posn(int x, int y)
{
	sciFeature::Posn(x, y);
	
	SetSize(GetWidth(), GetHeight());
	
	if (IsNotHidden())
		Show();
}

void FourSidedRect::SetPri(int newPri)
{
	int i;
	
	sciFeature::SetPri(newPri);
	
	for (i = 0 ; i < 4 ; i++)
		if (sides[i])
			sides[i]->SetPri(newPri + 2);
	
	if (IsNotHidden())
		Show();
}

void FourSidedRect::Show()
{
	int i;
	
	sciFeature::Show();
	
	if (sides[0]) {
		sides[0]->Posn(left, top);
		sides[0]->SetSize(1, bottom - top);
		sides[1]->Posn(left, top);
		sides[1]->SetSize(right - left, 1);
		sides[2]->Posn(right, top);
		sides[2]->SetSize(1, bottom - top);
		sides[3]->Posn(left, bottom);
		sides[3]->SetSize(right - left, 1);
		
		for (i = 0 ; i < 4 ; i++)
			sides[i]->Show();
	}
}

void FourSidedRect::Hide()
{
	int i;
	
	sciFeature::Hide();
	
	for (i = 0 ; i < 4 ; i++)
		if (sides[i])
			sides[i]->Hide();
}

void FourSidedRect::SetSideColors(RGBQUAD leftC, RGBQUAD topC, RGBQUAD rightC, RGBQUAD bottomC)
{
	sides[0]->SetOutlineColor(leftC);
	sides[1]->SetOutlineColor(topC);
	sides[2]->SetOutlineColor(rightC);
	sides[3]->SetOutlineColor(bottomC);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

EmbossedRect::EmbossedRect()
{
	frame1 = NULL;
	frame2 = NULL;
	filler = NULL;

	frame1Color = DEFAULT_LTGREY; // default is raised
	frame2Color = DEFAULT_DKGREY;
	fillerColor = DEFAULT_GREY;
}

EmbossedRect::~EmbossedRect()
{
	if (frame1) { delete frame1; frame1 = NULL; }
	if (frame2) { delete frame2; frame2 = NULL; }
	if (filler) { delete filler; filler = NULL; }
}

void EmbossedRect::Init()
{
	int x, y, width, height;

	sciFeature::Init();

	x = GetX();
	y = GetY();
	width = GetWidth();
	height = GetHeight();

	if (!frame1)
		frame1 = new sciRect();
	if (!frame2)
		frame2 = new sciRect();
	if (!filler)
		filler = new sciRect();

	frame1->plane = plane;
	frame2->plane = plane;
	filler->plane = plane;
	
	frame1->SetPri(GetPri() + 2);
	frame1->SetSize(width, height);
	frame1->Posn(x, y);
	frame1->Init();
	
	frame2->SetPri(GetPri() + 1);
	frame2->SetSize(width + 1, height + 1);
	frame2->Posn(x - 1, y);
	frame2->Init();
	
	filler->SetPri(GetPri() + 3);
	filler->SetSize(width - 1, height - 1);
	filler->Posn(x, y + 1);
	filler->Init();

	SetFrame1Color(frame1Color);
	SetFrame2Color(frame2Color);
	SetFillerColor(fillerColor);
}

void EmbossedRect::Posn(int x, int y)
{
	sciFeature::Posn(x, y);

	if (inited) {
		frame1->Posn(x, y);
		frame2->Posn(x - 1, y);
		filler->Posn(x, y + 1);
	}
}

void EmbossedRect::SetSize(int width, int height)
{
	sciFeature::SetSize(width, height);

	if (inited) {
		frame1->SetSize(width, height);
		frame2->SetSize(width + 1, height + 1);
		filler->SetSize(width - 1, height - 1);
	}
}

void EmbossedRect::SetPri(int newPri)
{
	sciFeature::SetPri(newPri);

	if (inited) {  //If the rect hasn't been inited, these don't exist...
		frame1->SetPri(newPri + 2); 
		frame2->SetPri(newPri + 1);
		filler->SetPri(newPri + 3);
	}
}

void EmbossedRect::Show(Boolean showFiller)
{
	sciFeature::Show();

	frame1->Show();
	frame2->Show();
	if (showFiller)
		filler->Show();
	else
		filler->Hide();
}

void EmbossedRect::Hide()
{
	sciFeature::Hide();
	
	frame1->Hide();
	frame2->Hide();
	filler->Hide();
}

void EmbossedRect::SetFrame1Color(BYTE red, BYTE green, BYTE blue)
{
	frame1Color.rgbRed = red;
	frame1Color.rgbGreen = green;
	frame1Color.rgbBlue = blue;

	SetFrame1Color(frame1Color);
}

void EmbossedRect::SetFrame2Color(BYTE red, BYTE green, BYTE blue)
{
	frame2Color.rgbRed = red;
	frame2Color.rgbGreen = green;
	frame2Color.rgbBlue = blue;

	SetFrame2Color(frame2Color);
}

void EmbossedRect::SetFillerColor(BYTE red, BYTE green, BYTE blue)
{
	fillerColor.rgbRed = red;
	fillerColor.rgbGreen = green;
	fillerColor.rgbBlue = blue;

	SetFillerColor(fillerColor);
}

void EmbossedRect::SetFrame1Color(RGBQUAD color)
{
	frame1Color.rgbRed = color.rgbRed;
	frame1Color.rgbGreen = color.rgbGreen;
	frame1Color.rgbBlue = color.rgbBlue;

	if (inited)
		frame1->SetOutlineColor(color);
}

void EmbossedRect::SetFrame2Color(RGBQUAD color)
{
	frame2Color.rgbRed = color.rgbRed;
	frame2Color.rgbGreen = color.rgbGreen;
	frame2Color.rgbBlue = color.rgbBlue;

	if (inited)
		frame2->SetOutlineColor(color);
}

void EmbossedRect::SetFillerColor(RGBQUAD color)
{
	fillerColor.rgbRed = color.rgbRed;
	fillerColor.rgbGreen = color.rgbGreen;
	fillerColor.rgbBlue = color.rgbBlue;

	if (inited) {
		filler->SetOutlineColor(color);
		filler->SetInnerColor(color);
	}
}
