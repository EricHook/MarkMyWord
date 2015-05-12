#include "os.h"
#include "core.h"
#include "sci.h"
#include "globals.h"

sciPlane::sciPlane(Boolean newModal, sciPlane *newParentPlane, int width, int height, Boolean closeBox)
	{
	prevModalPlane = NULL;
	modal = newModal;
	parentPlane = newParentPlane;
	isNotHidden = FALSE;
	pApp->AddPlane(this);
	picNumber = -1;
	celNumber = 0;
	top		= 0;
	left	= 0;
	bottom	= height;
	right	= width;
	capturingEventsFeature = NULL;
	cursorX = 0;
	cursorY = 0;
	useBackgroundColor = FALSE;
	background = NULL;
	draggableArea = 20;
	mouseDown = FALSE;
	xOffset = 0;
	yOffset = 0;
	dimmingRect = NULL;
	toBeDeleted = FALSE;
	editFocus = NULL;
	isEditFocusPlane = FALSE;
	
	if (parentPlane == NULL) {
		window = new Window();
		window->MakeWindow(width, height, closeBox);
	}
	else
		window = parentPlane->window;
	}

sciPlane::~sciPlane()
	{
	// if we are a sub-plane, our parent plane and its window might already have
	// been destroyed
	if (parentPlane && pApp->planes.IndexOf(parentPlane) != -1)
		MarkAsUpdated();
	if (IsModal() && (pApp->GetModalPlane() == this))
		pApp->SetModalPlane(prevModalPlane);
	pApp->RemovePlane(this);
	DeleteFromCast(background);
	assert(cast.Size() == 0);

#if !defined(BOUNDS_CHECKER)
	while (cast.Size())
		{
		delete cast.At(0);
		}
#endif

	assert(props.Size() == 0);

#if !defined(BOUNDS_CHECKER)
	while (props.Size())
		{
		delete props.At(0);
		}
#endif

	delete background;
	
	if (parentPlane == NULL)
		delete window;
	}

void sciPlane::AddToCast(sciFeature *feature)
	{
	if (feature->plane != this)
		assert(FALSE);
	if (!cast.Contains(feature))
		{
		cast.AddToBack(feature); // features start with priority 0, so we add them to the back
		feature->cast = &cast;	 // back of the cast to start, so they'll be drawn first (lowest)
		}
	}

void sciPlane::DeleteFromCast(sciFeature *feature)
	{
	if (cast.Contains(feature))
		{
		cast.Delete(feature);
		feature->cast = NULL;
		}
	}

void sciPlane::AddToProps(sciProp *prop)
	{
	if (!props.Contains(prop))
		{
		props.AddToFront(prop);
		}
	}

void sciPlane::DeleteFromProps(sciProp *prop)
	{
	if (props.Contains(prop))
		{
		props.Delete(prop);
		}
	}

Boolean sciPlane::HandleEvent(sciEvent &event)
	{
	sciFeature *tmpFeature;
	int i;
	int tempX, tempY;

	if (event.type == MOUSE_MOVE)
		{
		cursorX = event.x;
		cursorY = event.y;
		}

	if (IsHidden())
		return FALSE;

	if (capturingEventsFeature)
		capturingEventsFeature->HandleEvent(event);
	for (i = 0; i < cast.Size(); ++i)
		{
		tmpFeature = (sciFeature*)(cast.At(i));
		tmpFeature->HandleEvent(event);
		}
	switch (event.type)
		{
		case MOUSE_DOWN:
			if (parentPlane == NULL)
				return FALSE; // real windows handle their own dragging
			
			// Remove and add this, so that it shows up on top.
			pApp->RemovePlane(this);
			pApp->AddPlane(this);
			MarkAsUpdated();
			
			if (mouseDown)
				break;
			if (event.y > draggableArea || event.y < 0)
				break;
			if (event.x > (GetWidth() - 4) || event.x < 0)
				break;
			mouseDown = TRUE;
			xOffset = event.x;
			yOffset = event.y;

			pApp->SetCapturedPlane(this);
			event.claimed = TRUE;
			break;
		case MOUSE_MOVE:
			if (mouseDown)
				{
				// Don't let the plane move off screen.
				tempX = GetX() - (xOffset - event.x);
				tempY = GetY() - (yOffset - event.y);
				if (tempX < 0)
					tempX = 0;
				if (tempY < 0)
					tempY = 0;
				if (tempX + GetWidth() > 639)
					tempX = 639 - GetWidth();
				if (tempY + GetHeight() > 479)
					tempY = 479 - GetHeight();
				/*
				// special case to avoid covering chat edit window
				if (parentPlane == pApp->pMainPlane)
					if (tempY + GetHeight() > 440)
						tempY = 440 - GetHeight();
				// end special case
				*/
				Posn(tempX, tempY);
				event.claimed = TRUE;
				}
			break;
		case MOUSE_UP:
			if (!mouseDown)
				break;
			mouseDown = FALSE;
			pApp->SetCapturedPlane(NULL);
			event.claimed = TRUE;
			break;
		}
	return FALSE;
	}

void sciPlane::Doit()
	{
	sciProp *tmpProp;
	int i;

	for (i = 0; i < props.Size(); ++i)
		{
		tmpProp = (sciProp*)(props.At(i));
		if (tmpProp)
			tmpProp->Doit();
		}
	}

void sciPlane::Render(RECT &updateRect)
	{
	sciFeature *tmpFeature;
	int i;

	if (IsHidden())
		return;
	if (useBackgroundColor)
		DrawBackground(updateRect);
	if (background)
		background->Render(updateRect);
	for (i = cast.Size() - 1; i >= 0; --i)
		{
		tmpFeature = (sciFeature*)(cast.At(i));
		if (tmpFeature && tmpFeature->IsNotHidden())
			{
			tmpFeature->Render(updateRect);
			}
		}
	}

void sciPlane::Show()
	{
	if (!MAIN_WINDOW->InFullScreen())
		window->Show();
	// Remove and add this plane (if it's not a real window), so that it shows up on top.
	if (parentPlane != NULL)
		pApp->RemovePlane(this);
	if (parentPlane != NULL)
		pApp->AddPlane(this);
	
	if (IsModal() && IsHidden())
		{
		/*if (!dimmingRect)
			{
			dimmingRect = new sciRect();
			dimmingRect->plane = (sciPlane*)game->curRoom->planes.At(game->curRoom->planes.Size() - 2);
			if (dimmingRect->plane)
				dimmingRect->SetSize(dimmingRect->plane->GetWidth(), dimmingRect->plane->GetHeight());
			else
				dimmingRect->SetSize(640, 480);
			dimmingRect->SetAlpha(127);
			dimmingRect->SetInnerColor(1, 1, 1);
			dimmingRect->SetPri(99999);
			dimmingRect->Show();
			dimmingRect->Init();
			}*/
		if (prevModalPlane == NULL)
			prevModalPlane = pApp->GetModalPlane();
		pApp->SetModalPlane(this);
		window->SetNormalCursor(0);
		}
	isNotHidden = TRUE;

	// Update the window.
	MarkAsUpdated();
	}

void sciPlane::Hide()
	{
	if (IsNotHidden() && (pApp->GetCapturedPlane() == this))
		pApp->SetCapturedPlane(NULL);
	
	if (IsModal() && IsNotHidden())
		{
		/*if (dimmingRect)
			{
			dimmingRect->Hide();
			delete dimmingRect;
			dimmingRect = NULL;
			}*/
		if (pApp->GetModalPlane() == this)
			pApp->SetModalPlane(prevModalPlane);
		prevModalPlane = NULL;
		}
	if (parentPlane == NULL)
		window->Hide();
	
	isNotHidden = FALSE;

	// Update the window.
	MarkAsUpdated();
	}

void sciPlane::SetPic(int newPicNumber, int newCelNumber)
	{
	if (picNumber != newPicNumber || celNumber != newCelNumber)
		{
		picNumber = newPicNumber;
		celNumber = newCelNumber;
		LoadPic();
		}
	}

int sciPlane::GetPicNumber()
	{
		return picNumber;
	}

void sciPlane::LoadPic()
	{
	MarkAsUpdated();

	if (background == NULL)
		{
		background = new sciView();
		background->Show();
		background->plane = this;
		background->Init();
		background->SetLoop(0);
		background->SetCel(0);
		}
	background->SetView(picNumber);
	background->SetLoop(0);
	background->SetCel(celNumber);
	if (background->GetBitmapX() || background->GetBitmapY())
		{
		background->Posn(-background->GetBitmapX(), -background->GetBitmapY());
		left = background->GetBitmapX();
		top = background->GetBitmapY();
		right = background->GetBitmapX() + background->GetWidth();
		bottom = background->GetBitmapY() + background->GetHeight();
		}
	else
		{
		right = left + background->GetWidth();
		bottom = top + background->GetHeight();
		}

	MarkAsUpdated();
	DeleteFromCast(background);
	}

void sciPlane::Posn(int posnX, int posnY, BOOL updateWindow)
	{
	int width, height;

	MarkAsUpdated();

	width = right - left;
	height = bottom - top;
	left = posnX;
	top = posnY;
	right = left + width;
	bottom = top + height;

	if (updateWindow)
		{
		}

	MarkAsUpdated();
	}

int sciPlane::GetX()
	{
	return left;
	}

int sciPlane::GetY()
	{
	return top;
	}

int sciPlane::GetWidth()
	{
	return right - left;
	}

int sciPlane::GetHeight()
	{
	return bottom - top;
	}

Boolean sciPlane::IsModal()
	{
	return modal;
	}

void sciPlane::SetDraggableArea(int newDraggableArea)
	{
	draggableArea = newDraggableArea;
	}

void sciPlane::CaptureEvents(sciFeature *feature)
	{
	capturingEventsFeature = feature;
	if (capturingEventsFeature)
		pApp->SetCapturedPlane(this);
	else
		pApp->SetCapturedPlane(NULL);
	}

BOOL sciPlane::IsHidden()
	{
	return !isNotHidden;
	}

BOOL sciPlane::IsNotHidden()
	{
	return isNotHidden;
	}

int sciPlane::GetCursorX()
	{
	return cursorX;
	}

int sciPlane::GetCursorY()
	{
	return cursorY;
	}

void sciPlane::SetSize(int w, int h)
	{
	right = left + w;
	bottom = top + h;
	MarkAsUpdated();
	}

void sciPlane::SetBackgroundColor(RGBA color)
	{
	useBackgroundColor = TRUE;
	backgroundColor = color;
	backgroundColor.a = 255;
	}

void sciPlane::UnsetBackgroundColor()
	{
	useBackgroundColor = FALSE;
	}

void sciPlane::DrawBackground(RECT updateRect)
	{
	window->frameBuffer->DrawClosedRect(updateRect.left, updateRect.top,
		updateRect.right, updateRect.bottom, backgroundColor, ALPHA_FLAG);
	}

void sciPlane::MarkAsUpdated(const RECT &rect)
	{
	RECT r;

	r = rect;
	OffsetRect(&r, GetX(), GetY());
	window->Paint(r);
	}

void sciPlane::DrawPixel(int x, int y, RGBA color, int transferMode)
	{
	window->frameBuffer->DrawPixel(x + GetX(), y + GetY(), color, transferMode);
	}

void sciPlane::DrawHLine(int x1, int x2, int y, RGBA color, int transferMode)
	{
	window->frameBuffer->DrawHLine(x1 + GetX(), x2 + GetX(), y + GetY(), color, transferMode);
	}

void sciPlane::DrawVLine(int x, int y1, int y2, RGBA color, int transferMode)
	{
	window->frameBuffer->DrawVLine(x + GetX(), y1 + GetY(), y2 + GetY(), color, transferMode);
	}

void sciPlane::DrawClosedRect(int x1, int y1, int x2, int y2, RGBA color, int transferMode)
	{
	window->frameBuffer->DrawClosedRect(x1 + GetX(), y1 + GetY(), x2 + GetX(), y2 + GetY(), color, transferMode);
	}

void sciPlane::MarkAsUpdated()
	{
	RECT r;

	SetRect(&r, left, top, right, bottom);
	window->Paint(r);
	}

void sciPlane::Delete()
	{
	Hide();
	toBeDeleted = TRUE;
	}

BOOL sciPlane::IsToBeDeleted()
	{
	return toBeDeleted;
	}

void sciPlane::SetViewGamma(double gamma)
	{
	sciView *tmpView;
	int i;

	if (background)
		background->SetGamma(gamma);
	for (i = 0; i < cast.Size(); ++i)
		{
		tmpView = (sciView *)(cast.At(i)); // dynamic_cast<sciView *>
		if (tmpView)
			tmpView->SetGamma(gamma);
		}
	}

void sciPlane::AttemptClosePlane()
{
	pApp->MarkPlaneForDeletion(this);
}

EditBox *sciPlane::GetEditFocus()
{
	return editFocus;
}

EditBox *sciPlane::SetEditFocus(EditBox *editBox)
{
	EditBox *oldFocus = editFocus;

	if (oldFocus && oldFocus != editBox && oldFocus->status != EB_DELETING)
		oldFocus->LoseFocus();

	editFocus = editBox;

	return oldFocus;
}

