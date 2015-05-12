#include "os.h"
#include "core.h"
#include "sci.h"

sciView::sciView()
	{
	view = 0;
	loop = 0;
	cel = 0;
	sprite = NULL;
	alpha = 255;
	lastCel = 0;
	lastLoop = 0;
	UseColorOfPoint(FALSE);
	x = 0;
	y = 0;
	SetRect(&rcWindow, 0, 0, 0, 0);
	transferMode = ALPHA_FLAG;

	displayHalfSize = FALSE;
	scaleType = ST_NONE;
	
	colorMapping = NULL;
	
	// Someone else has to delete resourceManager;
	resourceManager = pApp->pResourceManager;
	}

sciView::~sciView()
	{
	if (sprite)
		resourceManager->FreeResource(sprite);

	if (colorMapping)
		{
		delete [] colorMapping->mappings;
		delete colorMapping;
		}
	
	// Someone else has to delete resourceManager;
	}

void sciView::Init()
	{
	sciFeature::Init();
	if (sprite)
		resourceManager->FreeResource(sprite);
	sprite = NULL;
	if (view)
		{
		sprite = (TSprite*)resourceManager->LoadResource(RES_TSPRITE, view);
		if (sprite)
			{
			lastLoop = sprite->GetNumLoops() - 1;
			lastCel = sprite->GetNumCels(0) - 1;
			}
		if (cel > LastCel())
			cel = LastCel();
		if (cel < 0)
			cel = 0;
		if (loop > LastLoop())
			loop = LastLoop();
		if (loop < 0)
			loop = 0;

		// Paint the old frame area.
		plane->MarkAsUpdated(rcWindow);

		// Get the new frame area.
		Posn(GetX(), GetY());

		// Paint the new frame area.
		plane->MarkAsUpdated(rcWindow);
		}
	}
void sciView::ParamInit(int newView, int newX, int newY, int newPri)
	{
	SetView(newView);
	sciFeature::ParamInit(newX,newY,newPri);
	}
void sciView::Posn(int posnX, int posnY)
	{
	if (inited && IsNotHidden())
		{
		plane->MarkAsUpdated(rcWindow);
		}
	x = posnX;
	y = posnY;
	if (sprite)
		{
		SetSize(sprite->GetWidth(loop, cel), sprite->GetHeight(loop, cel));
		if (displayHalfSize)
			sciFeature::Posn(x + sprite->GetX(loop, cel) / 2, y + sprite->GetY(loop, cel) / 2);
		else
			sciFeature::Posn(x + sprite->GetX(loop, cel), y + sprite->GetY(loop, cel));
		}
	else
		{
		sciFeature::Posn(x, y);
		}

	SetRect(&rcWindow, left, top, right, bottom);
	if (inited && IsNotHidden())
		{
		plane->MarkAsUpdated(rcWindow);
		}
	}

void sciView::SetView(int newView)
	{
	if (view != newView)
		{
		view = newView;
		if (inited)
			{
			plane->MarkAsUpdated(rcWindow);
			sciView::Init();
			if (IsNotHidden())
				{
				plane->MarkAsUpdated(rcWindow);
				}
			}
		}
	else
		{
		view = newView;
		}
	}

void sciView::SetLoop(int newLoop)
	{
	if (loop != newLoop)
		{
		if (inited)
			{
			if (newLoop > LastLoop())
				newLoop = LastLoop();
			if (newLoop < 0)
				newLoop = 0;
			if (loop != newLoop)
				plane->MarkAsUpdated(rcWindow);
			}
		}
	loop = newLoop;
	if (sprite)
		{
		lastCel = sprite->GetNumCels(loop) - 1;
		}
	SetCel(cel);
	}

void sciView::SetCel(int newCel)
	{
	if (cel != newCel)
		{
		if (inited)
			{
			if (newCel > LastCel())
				newCel = LastCel();
			if (newCel < 0)
				newCel = 0;
			if (cel != newCel)
				{
				cel = newCel;

				// Get the new frame area.
				// This will paint old and new area.
				// It will also update all the pertinent variables.
				Posn(GetX(), GetY());
				}
			}
		else
			{
			cel = newCel;
			}
		}
	}

int sciView::GetCel()
	{
	return cel;
	}

int sciView::GetLoop()
	{
	return loop;
	}

int sciView::GetView()
	{
	return view;
	}

void sciView::Hide()
	{
	if (!hidden)
		{
		sciFeature::Hide();
		if (inited)
			{
			plane->MarkAsUpdated(rcWindow);
			}
		}
	}

void sciView::Show()
	{
	if (hidden)
		{
		sciFeature::Show();
		if (inited)
			{
			plane->MarkAsUpdated(rcWindow);
			}
		}
	}

int sciView::LastCel()
	{
	return lastCel;
	}

int sciView::LastLoop()
	{
	return lastLoop;
	}

extern BYTE maxAlpha;

void sciView::Render(RECT &updateRect)
{
	int halfX, halfY;
	RECT my;
	RECT clippedUpdateRect;

	clippedUpdateRect = rcWindow;
	IntersectRect(&clippedUpdateRect, &clippedUpdateRect, &updateRect);
	if (IsRectEmpty(&clippedUpdateRect))
		return;

	if (sprite && plane && IsNotHidden())
	{
		if (displayHalfSize)
		{
			// clip to our smaller rectangle
			halfX = rcWindow.left	+ (rcWindow.right - rcWindow.left) / 2;
			halfY = rcWindow.top	+ (rcWindow.bottom - rcWindow.top) / 2;
			if (clippedUpdateRect.left > halfX)
				clippedUpdateRect.left = halfX;

			if (clippedUpdateRect.right > halfX)
				clippedUpdateRect.right = halfX;

			if (clippedUpdateRect.top > halfY)
				clippedUpdateRect.top = halfY;

			if (clippedUpdateRect.bottom > halfY)
				clippedUpdateRect.bottom = halfY;
			
			// figure out which part of my normal-size bitmap gets mapped into the
			// shrunken clippedUpdateRect
			my.left		= (clippedUpdateRect.left - rcWindow.left) * 2;
			my.top		= (clippedUpdateRect.top - rcWindow.top) * 2;
			my.right	= my.left + (clippedUpdateRect.right - clippedUpdateRect.left) * 2;
			my.bottom	= my.top + (clippedUpdateRect.bottom - clippedUpdateRect.top) * 2;

			OffsetRect(&clippedUpdateRect, plane->GetX(), plane->GetY());
			maxAlpha = alpha;

			if (maxAlpha > 0)
				sprite->CopyScaledPixels(loop, cel, plane->GetWindow()->frameBuffer, clippedUpdateRect, my, scaleType);

			maxAlpha = 255;
		
		}
		else
		{
			my.left		= clippedUpdateRect.left - rcWindow.left;
			my.top		= clippedUpdateRect.top - rcWindow.top;
			my.right	= my.left + (clippedUpdateRect.right - clippedUpdateRect.left);
			my.bottom	= my.top + (clippedUpdateRect.bottom - clippedUpdateRect.top);

			OffsetRect(&clippedUpdateRect, plane->GetX(), plane->GetY());
			maxAlpha = alpha;
			if (maxAlpha > 0)
				sprite->CopyPixels(loop, cel, plane->GetWindow()->frameBuffer, clippedUpdateRect, my, colorMapping);
			maxAlpha = 255;
		}
	}
}

void sciView::SetAlpha(BYTE newAlpha)
	{
	alpha = newAlpha;
	if (inited && IsNotHidden())
		{
		plane->MarkAsUpdated(rcWindow);
		}
	}

BYTE sciView::GetAlpha()
	{
	return alpha;
	}

Boolean sciView::OnMe(int onMeX, int onMeY)
	{
	if (sciFeature::OnMe(onMeX, onMeY) &&
		(!sprite || !useColorOfPoint || sprite->ColorOfPoint(loop, cel, onMeX - rcWindow.left, onMeY - rcWindow.top))
		)
		return (TRUE);
	else
		return (FALSE);
	}

void sciView::UseColorOfPoint(BOOL b)
	{
	useColorOfPoint = b;
	}

int sciView::GetX()
	{
	return x;
	}

int sciView::GetY()
	{
	return y;
	}

int sciView::GetBitmapX()
	{
	if (sprite)
		{
			if (displayHalfSize)
				return sprite->GetX(loop, cel) / 2;
			else
				return sprite->GetX(loop, cel);
		}
	else
		return 0;
	}

int sciView::GetBitmapY()
	{
	if (sprite)
		{
		if (displayHalfSize)
			return sprite->GetY(loop, cel) / 2;
		else
			return sprite->GetY(loop, cel);
		}
	else
		return 0;
	}

void sciView::SetTransferMode(int mode)
	{
	transferMode = mode;
	}

int sciView::GetTransferMode()
	{
	return transferMode;
	}

RECT sciView::GetRectNoAlpha()
	{
	RECT r;
	int trimLeft, trimTop, trimRight, trimBottom;

	FigureTrim(trimLeft, trimTop, trimRight, trimBottom);
	r.left = trimLeft;
	r.top = trimTop;
	r.right = (right - left) - trimRight;
	r.bottom = (bottom - top) - trimBottom;
	return r;
	}

void sciView::FigureTrim(int &trimLeft, int &trimTop, int &trimRight, int &trimBottom)
	{
	int w, h;
	int x, y;
	BOOL clear;
	RGBA color;
	BYTE flags;

	w = GetWidth();
	h = GetHeight();

	// Find trimTop.
	trimTop = 0;
	for (y = 0; y < h; ++y)
		{
		clear = TRUE;
		for (x = 0; x < w; ++x)
			{
			sprite->GetPixel(loop, cel, x, y, color, flags);
			if (color.a == 255)
				{
				clear = FALSE;
				x = w;
				}
			}
		if (clear)
			trimTop = y + 1;
		else
			y = h;
		}

	// Find trimLeft.
	trimLeft = 0;
	for (x = 0; x < w; ++x)
		{
		clear = TRUE;
		for (y = 0; y < h; ++y)
			{
			sprite->GetPixel(loop, cel, x, y, color, flags);
			if (color.a == 255)
				{
				clear = FALSE;
				y = h;
				}
			}
		if (clear)
			trimLeft = x + 1;
		else
			x = w;
		}

	// Find trimBottom.
	trimBottom = 0;
	for (y = h-1; y >= 0; --y)
		{
		clear = TRUE;
		for (x = 0; x < w; ++x)
			{
			sprite->GetPixel(loop, cel, x, y, color, flags);
			if (color.a == 255)
				{
				clear = FALSE;
				x = w;
				}
			}
		if (clear)
			trimBottom = h - y;
		else
			y = 0;
		}

	// Find trimRight.
	trimRight = 0;
	for (x = w-1; x >= 0; --x)
		{
		clear = TRUE;
		for (y = 0; y < h; ++y)
			{
			sprite->GetPixel(loop, cel, x, y, color, flags);
			if (color.a == 255)
				{
				clear = FALSE;
				y = h;
				}
			}
		if (clear)
			trimRight = w - x;
		else
			x = 0;
		}
	}

void sciView::SetGamma(double g)
	{
	if (sprite)
		sprite->SetGamma(g);
	if (inited)
		{
		plane->MarkAsUpdated(rcWindow);
		}
	}

void sciView::SetHalfSize(bool onoff, SCALE_TYPE st /* = ST_BILINEAR*/)
{
	displayHalfSize = onoff;
	scaleType = st;

	assert(displayHalfSize == FALSE || displayHalfSize == TRUE);

	// we need to have the position updated using half-scaled tex offsets
	Posn(x, y);
}

void sciView::SetColorMap(BYTE rFrom, BYTE gFrom, BYTE bFrom, BYTE rTo, BYTE gTo, BYTE bTo)
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

COLORMAPPING *sciView::GetColorMapping()
{
	return colorMapping;
}

void sciView::CopyColorMapping(COLORMAPPING *sourceMap)
{
	int i;
	
	if (!sourceMap) {
		if (colorMapping) {
			delete colorMapping->mappings;
			delete colorMapping;
			colorMapping = NULL;
		}
		
		return;
	}
	
	if (!colorMapping) {
		colorMapping = new COLORMAPPING;
		colorMapping->numMaps = 0;
		colorMapping->mappings = NULL;
	}
	else
		delete [] colorMapping->mappings;
	
	colorMapping->numMaps = sourceMap->numMaps;
	colorMapping->mappings = new COLORMAP[colorMapping->numMaps];
	
	for (i = 0 ; i < colorMapping->numMaps ; i++)
		colorMapping->mappings[i] = sourceMap->mappings[i];
}
