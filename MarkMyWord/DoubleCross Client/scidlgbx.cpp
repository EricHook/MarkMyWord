#include "os.h"
#include "core.h"
#include "sci.h"
#include "globals.h"

sciDialogBox::sciDialogBox(Boolean modal, sciPlane *parentPlane)
	: sciPlane(modal, parentPlane)
{
//	if (game)
//		game->HandsOff();
	titleBarText = NULL;
}

sciDialogBox::~sciDialogBox()
{
	if (titleBarText)		{delete titleBarText; titleBarText = NULL;}
}

void sciDialogBox::Posn(int posnX, int posnY, Boolean updateWindow)
{
	//posnX += pApp->pWindow->x;
	//posnY += pApp->pWindow->y;
	sciPlane::Posn(posnX, posnY, updateWindow);
}

void sciDialogBox::Center(short dX, short dY)
{
	int x, y;
	
	x = (640 - GetWidth()) / 2;
	y = (440 - GetHeight()) / 2; // 480 - 40 (for standard menu and caption bars)
	
	sciPlane::Posn(x + dX, y + dY, TRUE);
	
	if (titleBarText)
		titleBarText->SetWidth(GetWidth());
}

void sciDialogBox::SetTitleText(char *newText, int newFont, int xOffset, int yOffset)
{
	POINT textPosn = { 10, 4 };
	
	if (!titleBarText) {
		titleBarText = new sciText();
		titleBarText->plane = this;
		titleBarText->SetJust(JUST_VERTICAL | JUST_LEFT);
		titleBarText->SetHeight(20);
		titleBarText->Init();
		titleBarText->SetPri(3);
		titleBarText->Show();
	}
	titleBarText->SetFont(newFont);
	titleBarText->Posn(textPosn.x + xOffset, textPosn.y + yOffset);
	titleBarText->SetWidth(GetWidth() - 20);
	titleBarText->SetTextToWidth(newText);
}

void sciDialogBox::DrawBackground(RECT updateRect)
	{
	window->frameBuffer->DrawClosedRect(updateRect.left, updateRect.top,
		updateRect.right, updateRect.bottom, backgroundColor, ALPHA_FLAG);
	}
