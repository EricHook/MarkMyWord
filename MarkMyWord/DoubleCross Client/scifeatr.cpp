#include "os.h"
#include "sci.h"

sciList unInitedFeatures;

sciFeature::sciFeature()
	{
	left = 0;
	top = 0;
	right = 0;
	bottom = 0;
	priority = 0;
	hidden = TRUE;
	plane = NULL;
	cast = NULL;
	inited = FALSE;
	unInitedFeatures.AddToFront(this);
	}

sciFeature::~sciFeature()
	{
	if (inited)
		{
		if (plane)
			plane->DeleteFromCast(this);
		}
	else
		{
		unInitedFeatures.Delete(this);
		}
	}

void sciFeature::Init()
	{
	int tmpPri;

	if (plane == NULL)
		plane = pApp->pMainPlane;

	if (plane)
		{
		plane->AddToCast(this);

		// Force an update to the priority.
		tmpPri = priority;
		SetPri(0);
		SetPri(tmpPri);

		unInitedFeatures.Delete(this);
		}
	inited = TRUE;
	}

void sciFeature::ParamInit(int newX, int newY, int newPri)
	{
	Init();
	Posn(newX,newY);
	SetPri(newPri);
	}

Boolean sciFeature::HandleEvent(sciEvent &event)
	{
	return FALSE;
	}

Boolean sciFeature::IsNotHidden()
	{
	return (!hidden);
	}

Boolean sciFeature::IsHidden()
	{
	return (hidden);
	}

Boolean sciFeature::OnMe(int onMeX, int onMeY)
	{
	if (IsNotHidden() &&
		((onMeX >= left) && (onMeX <= right)) &&
		((onMeY >= top) && (onMeY <= bottom))
		)
		return (TRUE);
	else
		return (FALSE);
	}

Boolean sciFeature::OnMe(sciFeature &obj)
	{
	return (OnMe(obj.GetX(), obj.GetY()));
	}

Boolean sciFeature::OnMe(sciEvent &event)
	{
	return (OnMe(event.x, event.y));
	}

void sciFeature::SetPri(int newPri)
	{
	sciFeature *tmpFeature;
	sciFeature *prevTmpFeature;
	int i;
	RECT rcWindow;

	if (newPri == priority)
		return;

	priority = newPri;
	if (cast == NULL)
		return;

	// Adjust the priority in the cast.
	cast->Delete(this);
	i = 0;
	prevTmpFeature = NULL;
	tmpFeature = (sciFeature*)cast->At(i);
	while (tmpFeature && (tmpFeature->GetPri() > GetPri()))
		{
		++i;
		prevTmpFeature = tmpFeature;
		tmpFeature = (sciFeature*)cast->At(i);
		}
	if (prevTmpFeature)
		cast->AddAfter(prevTmpFeature, this);
	else
		cast->AddToFront(this);

	if (plane && inited && IsNotHidden())
		{
		SetRect(&rcWindow, left, top, right, bottom);
		plane->GetWindow()->Paint(rcWindow);
		}
	}

int sciFeature::GetPri()
	{
	return priority;
	}

void sciFeature::Render(RECT &updateRect)
	{
	}

int sciFeature::GetX()
	{
	return left;
	}

int sciFeature::GetY()
	{
	return top;
	}

int sciFeature::GetWidth()
	{
	return (right - left);
	}

int sciFeature::GetHeight()
	{
	return (bottom - top);
	}

void sciFeature::SetSize(int width, int height)
	{
	SetWidth(width);
	SetHeight(height);
	}

void sciFeature::SetWidth(int width)
	{
	right = left + width;
	}

void sciFeature::SetHeight(int height)
	{
	bottom = top + height;
	}

void sciFeature::Posn(int posnX, int posnY)
	{
	int width, height;

	width = right - left;
	height = bottom - top;
	left = posnX;
	top = posnY;
	SetWidth(width);
	SetHeight(height);
	}

void sciFeature::CenterInPlane()
	{
		int x, y;
		
		x = plane->GetWidth() / 2 - GetWidth() / 2;
		y = plane->GetHeight() / 2 - GetHeight() / 2;
		
		Posn(x, y);
	}

void sciFeature::Hide()
	{
	hidden = TRUE;
	}

void sciFeature::Show()
	{
	hidden = FALSE;
	}

void sciFeature::HideOffScreen()
	{
	if (left < 10000)
		Posn(left + 10000, top);
	}

void sciFeature::ShowOnScreen()
	{
	if (left >= 10000)
		Posn(left - 10000, top);
	}

void sciFeature::MarkAsUpdated()
	{
	RECT rect;

	SetRect(&rect, left, top, right, bottom);
	MarkAsUpdated(rect);
	}

void sciFeature::MarkAsUpdated(const RECT &rect)
	{
	if (plane)
		{
		plane->MarkAsUpdated(rect);
		}
	}

