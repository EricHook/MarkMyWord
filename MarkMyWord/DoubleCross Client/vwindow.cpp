#include "os.h"
#include "core.h"
#include "sci.h"
#include "globals.h"
#include "Client Include.h"
extern BTMP_RES btmpResGlobalHeader;
extern BYTE btColorBitSupport;

Boolean loadingRoom = FALSE;

#define MAX_RECTS 20

class sciRECT : public sciObject
	{
	public:
		RECT rect;

		Boolean Equal(sciRECT &testRect);
		Boolean TestSect(sciRECT &testRect);
		Boolean Encloses(int x, int y);
		Boolean Encloses(sciRECT &testRect);
		Boolean EnclosedBy(sciRECT &testRect);

		int Disjoint(sciRECT &testRect, sciRECT *pResultArray);
	};

Boolean sciRECT::Equal(sciRECT &testRect)
{
	return((rect.left == testRect.rect.left) &&
		(rect.right == testRect.rect.right) &&
		(rect.top == testRect.rect.top) &&
		(rect.bottom == testRect.rect.bottom));
}

Boolean sciRECT::TestSect(sciRECT &testRect)
{
	return((rect.left < testRect.rect.right) &&
		(rect.right > testRect.rect.left) &&
		(rect.top < testRect.rect.bottom) &&
		(rect.bottom > testRect.rect.top));
}

Boolean sciRECT::Encloses(int x, int y)
{
	return((x >= rect.left) &&
		(x < rect.right) &&
		(y >= rect.top) &&
		(y < rect.bottom));
}

Boolean sciRECT::Encloses(sciRECT &testRect)
{
	return((testRect.rect.left >= rect.left) &&
		(testRect.rect.right <= rect.right) &&
		(testRect.rect.top >= rect.top) &&
		(testRect.rect.bottom <= rect.bottom));
}

Boolean sciRECT::EnclosedBy(sciRECT &testRect)
{
	return((rect.left >= testRect.rect.left) &&
		(rect.right <= testRect.rect.right) &&
		(rect.top >= testRect.rect.top) &&
		(rect.bottom <= testRect.rect.bottom));
}

int sciRECT::Disjoint(sciRECT &testRect, sciRECT *pResultArray)
{
	sciRECT rs;						// compute intersection
	rs.rect = rect;
	IntersectRect(&(rs.rect), &(rs.rect), &(testRect.rect));

	sciRECT r = *this;              // make copy of "this" rectangle
	sciRECT *pr = pResultArray;     // point at start of result array

   if (r.rect.top < rs.rect.top)    // if area above the intersection:
      {
      pr->rect.left = r.rect.left;   // then make rect of area above intersect
      pr->rect.top = r.rect.top;
      pr->rect.right = r.rect.right;
      pr->rect.bottom = rs.rect.top;
      ++pr;
      }
   if (r.rect.left < rs.rect.left)   // if area to left of intersection
      {                          // then make rect of that area
      pr->rect.left = r.rect.left;
      pr->rect.top = r.rect.top > rs.rect.top ? r.rect.top : rs.rect.top;
      pr->rect.right = rs.rect.left;
      pr->rect.bottom = r.rect.bottom < rs.rect.bottom ? r.rect.bottom : rs.rect.bottom;
      ++pr;
      }
   if (r.rect.right > rs.rect.right) // if area to right of intersection
      {                              // then make rect of that area
      pr->rect.left = rs.rect.right;
      pr->rect.top = r.rect.top > rs.rect.top ? r.rect.top : rs.rect.top;
      pr->rect.right = r.rect.right;
      pr->rect.bottom = r.rect.bottom < rs.rect.bottom ? r.rect.bottom : rs.rect.bottom;
      ++pr;
      }
   if (r.rect.bottom > rs.rect.bottom) // if area below intersection
      {
      pr->rect.left = r.rect.left;     // then make rect of that area
      pr->rect.top = rs.rect.bottom;
      pr->rect.right = r.rect.right;
      pr->rect.bottom = r.rect.bottom;
      ++pr;
      }

   return(pr - pResultArray);    // return # rectangles made
}

VWindow::VWindow()
	{
	frameBuffer = NULL;
	x = 0;
	y = 0;
	SetRect(&dirtyRect, 0, 0, 0, 0);
	SetRect(&rcOuter, 0, 0, 0, 0);
	SetRect(&rcInner, 0, 0, 0, 0);
	isNotHidden = FALSE;
	pCurrentPalette = new Palette();
	title = NULL;
	}

VWindow::~VWindow()
	{
	UnPaintAll();

	delete pCurrentPalette;
	delete frameBuffer;
	delete[] title;
	}

void VWindow::MakeWindow()
	{
	pCurrentPalette->LoadResource(1002);
	}

void VWindow::Show()
	{
	isNotHidden = TRUE;
	}

void VWindow::Hide()
	{
	isNotHidden = FALSE;
	}

WORD VWindow::Width()
	{
	return (WORD)(rcInner.right - rcInner.left);
	}

WORD VWindow::Height()
	{
	return (WORD)(rcInner.bottom - rcInner.top);
	}

void VWindow::Paint(RECT &rcPaint)
	{
	int i, j;
	RECT tempRect;
	sciRECT *newRect;
	sciRECT *rect1, *rect2;

	if (IsRectEmpty(&rcPaint))
		return;
	if (isNotHidden == FALSE)
		return;
	tempRect = rcPaint;
	UnionRect(&dirtyRect, &dirtyRect, &tempRect);

	// Add the new rect to the list.
	// If the new rect overlaps any old rects, union them together.
	newRect = new sciRECT;
	newRect->rect = rcPaint;
	updateRects.AddToBack(newRect);

	if (updateRects.Size() > MAX_RECTS)
		{
		// Too many rects to quickly handle.
		// Collapse to rcInner.
		while (updateRects.Size())
			{
			rect1 = (sciRECT*)updateRects.At(0);
			updateRects.Delete(rect1);
			delete rect1;
			}
		newRect = new sciRECT;
		newRect->rect = rcInner;
		OffsetRect(&(newRect->rect), -rcInner.left, -rcInner.top);
		updateRects.AddToBack(newRect);
		return;
		}

	for (i = 0; i <= (updateRects.Size()-2); ++i)
		{
		for (j = i+1; j <= (updateRects.Size()-1); ++j)
			{
			rect1 = (sciRECT*)updateRects.At(i);
			rect2 = (sciRECT*)updateRects.At(j);
			IntersectRect(&tempRect, &(rect1->rect), &(rect2->rect));
			if (!IsRectEmpty(&tempRect))
				{
				UnionRect(&tempRect, &(rect1->rect), &(rect2->rect));
				rect1->rect = tempRect;
				updateRects.Delete(rect2);
				delete rect2;

				// Start over the for loops.
				i = 0;
				j = i;
				continue;
				}
			}
		}
	}

void VWindow::OffscreenUpdate(RECT &updateRect)
	{
	sciPlane *plane, *parentPlane = NULL;
	int i;
	RECT r;
	char foundFocus = FALSE;

	for (i = 0 ; i < pApp->planes.Size() ; i++)
		{
		plane = (sciPlane *)pApp->planes.At(i);
		if (plane->GetWindow() == this)
			{
			if (plane->GetParentPlane() == NULL)
				{
				parentPlane = plane;
				}
			r = updateRect;
			OffsetRect(&r, -plane->GetX(), -plane->GetY());
			plane->Render(r);
			if (plane->IsNotHidden() && plane->GetEditFocus() && plane->GetEditFocus()->IsNotHidden() && plane != parentPlane)
				foundFocus = TRUE;
			}
		}
	if (!foundFocus && parentPlane)
		parentPlane->SetEditFocusPlane(TRUE);
	}

void VWindow::OffscreenErase(RECT &rect)
	{
	//background->CopyPixels(0, 0, frameBuffer, rect, rect);
	}

void VWindow::PaintAll()
	{
	RECT rcSource;

	rcSource = rcInner;
	OffsetRect(&rcSource, -rcInner.left, -rcInner.top);
	Paint(rcSource);
	}

void VWindow::UnPaintAll()
	{
	RECT rcSource;
	sciRECT *rect1;

	while (updateRects.Size())
		{
		rect1 = (sciRECT*)updateRects.At(0);
		rcSource = rect1->rect;
		updateRects.Delete(rect1);
		delete rect1;
		}
	SetRect(&dirtyRect, 0, 0, 0, 0);
	}

void VWindow::TruePaint(RECT &rcPaint)
	{
	RECT rcSource;
	sciRECT *rect1;
	RGBA color = { 255, 0, 0, 128 };
	
	while (updateRects.Size())
		{
		rect1 = (sciRECT*)updateRects.At(0);
		rcSource = rect1->rect;
		OffscreenErase(rcSource);
		OffscreenUpdate(rcSource);
		
#ifdef _DEBUG
		if (ControlKeyDown() && ShiftKeyDown())
			frameBuffer->DrawClosedRect(rcSource.left, rcSource.top, rcSource.right, rcSource.bottom, color, ALPHA_FLAG);
#endif
		
		updateRects.Delete(rect1);
		delete rect1;
		}

	CopyToScreen(rcPaint);
	
	if (loadingRoom && game)
		{
		loadingRoom = FALSE;
		game->HandsOn();
		}
	
	/*
	if (roomClient && roomClient->roomChatEditWindow && this == MAIN_WINDOW)
		{
		SetWindowPos(roomClient->roomChatEditWindow,
					 NULL,
					 rcInner.left + 101, rcInner.top + 428,
					 0, 0,
					 SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
		
		if (GetActiveWindow() == MAIN_HWND)
			SetFocus(roomClient->roomChatEditWindow);
		}
	*/
	}

void VWindow::TruePaintDirty()
	{
	if (!IsRectEmpty(&dirtyRect))
		{
		TruePaint(dirtyRect);
		SetRect(&dirtyRect, 0, 0, 0, 0);
		}
	}

void VWindow::ForceUpdate()
	{
	RECT rcSource;

////////////
	#ifdef _DEBUG
	//	MessageBeep(MB_OK);
	#endif
////////////

	rcSource = rcInner;
	OffsetRect(&rcSource, -rcInner.left, -rcInner.top);
	TruePaint(rcSource);
	}

BOOL VWindow::IsHidden()
	{
	return !isNotHidden;
	}

BOOL VWindow::IsNotHidden()
	{
	return isNotHidden;
	}

void VWindow::SetSize(int width, int height)
	{
	rcInner.right = rcInner.left + width;
	rcInner.bottom = rcInner.top + height;

	// Recreate the work bitmap.
	delete frameBuffer;
#if defined(FLIP_DIB)
	frameBuffer = new FrameBuffer(rcInner.right - rcInner.left, rcInner.bottom - rcInner.top, sizeof(TEX_PIXEL), FALSE);
#else
	frameBuffer = new FrameBuffer(rcInner.right - rcInner.left, rcInner.bottom - rcInner.top, sizeof(TEX_PIXEL), TRUE);
#endif

	PaintAll();
	}

void VWindow::SetTitle(char *newTitle)
	{
	delete[] title;
	title = new char[strlen(newTitle)+1];
	strcpy(title, newTitle);
	}

Boolean VWindow::EditCursorIsFlashingInWindow()
{
	char foundFocus = FALSE;
	int i;
	sciPlane *plane;
	
	for (i = 0 ; i < pApp->planes.Size() ; i++) {
		plane = (sciPlane *)pApp->planes.At(i);
		if (plane->GetWindow() != this)
			continue;
		if (plane->IsNotHidden() && plane->GetEditFocus() && plane->GetEditFocus()->IsNotHidden())
			return TRUE;
	}
	
	return FALSE;
}
