#include <math.h>

#include "os.h"
#include "sci.h"

sciScrollButton::sciScrollButton(sciScrollBox *newOwner, int newDirection)
{
	owner = newOwner;
	direction = newDirection;
}

sciScrollButton::~sciScrollButton()
{
}

void sciScrollButton::DoClick(BOOL netMessage)
{
	owner->DoScroll(direction, 1);
}

sciScroller::sciScroller(sciScrollBox *newOwner)
{
	owner = newOwner;
	mouseDown = FALSE;
	xOffset = 0;
	yOffset = 0;
	min = 0;
	max = 0;
}

sciScroller::~sciScroller()
{
}

void sciScroller::SetMinMax(int newMin, int newMax)
{
	min = newMin;
	max = newMax;
}

int sciScroller::GetMin()
{
	return(min);
}

int sciScroller::GetMax()
{
	return(max);
}

Boolean sciScroller::HandleEvent(sciEvent &event)
{
	int oldX, oldY;
	
	if ((!sciFeature::OnMe(event) && !mouseDown) || event.claimed)
		return FALSE;
	
	switch (event.type)
	{
		case MOUSE_DOWN:
			if (OnMe(event.x, event.y) && event.modifiers & RIGHT_MOUSE)
				event.claimed = TRUE;
		
			if (OnMe(event.x, event.y) && (event.modifiers & LEFT_MOUSE) && !mouseDown)
			{
				mouseDown = TRUE;
				event.claimed = TRUE;
				if(owner->GetDirection() == GDefs::VERTICAL)
					yOffset = event.y - GetY();
				else
					xOffset = event.x - GetX();
			}
			break;
		case MOUSE_MOVE:
			if (mouseDown)
			{
				oldX = GetX();
				oldY = GetX();
				
				if(owner->GetDirection() == GDefs::VERTICAL)
				{
					if (event.y - yOffset < min)
						Posn(GetX(), min);
					else
					{
						if (event.y - yOffset > max)
							Posn(GetX(), max);
						else
							Posn(GetX(), event.y - yOffset);
					}
//					DoEffect(GetY() - min, GetY());
				}
				else
				{
					if (event.x - xOffset < min)
						Posn(min, GetY());
					else
					{
						if (event.x - xOffset > max)
							Posn(max, GetY());
						else
							Posn(event.x - xOffset, GetY());
					}
//					DoEffect(GetX() - min, GetX());
				}
				
				if (GetX() != oldX || GetY() != oldY)
					owner->ScrollerRelease(&event);
				
				event.claimed = TRUE;
			}
			break;
		case MOUSE_UP:
			if ((event.modifiers & LEFT_MOUSE) && mouseDown)
			{
				owner->ScrollerRelease(&event);
				mouseDown = FALSE;
				event.claimed = TRUE;
			}
			break;
	}
	return event.claimed;
}

sciScrollBox::sciScrollBox(int newDirection)
{
	direction = newDirection;
	enabled = TRUE;
	range = 0;
	selectedID = 0;
	prevButtonView	= 0;
	nextButtonView	= 0;
	scrollerView	= 0;
	
	SetScrollerOffsets(0, 0, 0);
	
	previousButton = new sciScrollButton(this, GDefs::REVERSE);
	nextButton = new sciScrollButton(this, GDefs::FORWARD);
	scroller = new sciScroller(this);
}

sciScrollBox::~sciScrollBox()
{
	if (previousButton)		{delete previousButton; previousButton = NULL;}
	if (nextButton)			{delete nextButton; nextButton = NULL;}
	if (scroller)			{delete scroller; scroller = NULL;}
}

void sciScrollBox::Init()
{
	SetOutlineColor(0, 0, 0);
	sciRect::Init();

	previousButton->plane = plane;
	if(prevButtonView)
		previousButton->SetView(prevButtonView);
	else
		previousButton->SetView(direction == GDefs::VERTICAL ? 25022 : 25098);
	previousButton->SetCel(0);
	previousButton->Init();

	nextButton->plane = plane;
	if(nextButtonView)
		nextButton->SetView(nextButtonView);
	else
		nextButton->SetView(direction == GDefs::VERTICAL ? 25019 : 25099);
	nextButton->SetCel(0);
	nextButton->Init();

	scroller->plane = plane;
	if(scrollerView)
		scroller->SetView(scrollerView);
	else
		scroller->SetView(direction == GDefs::VERTICAL ? 25021 : 25100);
	scroller->SetCel(0);
	scroller->Init();

	Disable();
}

Boolean sciScrollBox::HandleEvent(sciEvent &event)
{
	if ((!sciFeature::OnMe(event)) || !enabled || event.claimed)
		return FALSE;
	
	switch (event.type)
	{
		case MOUSE_DOWN:
			if (OnMe(event.x, event.y) && event.modifiers & RIGHT_MOUSE)
				event.claimed = TRUE;
		
			if (OnMe(event.x, event.y) && (event.modifiers & LEFT_MOUSE))
			{
				event.claimed = TRUE;

				Page(&event);
			}
			break;
		case MOUSE_UP:
			if ((event.modifiers & LEFT_MOUSE))
			{
				event.claimed = TRUE;
			}
			break;
	}
	return event.claimed;
}

void sciScrollBox::SetScrollerOffsets(int newDPrev, int newDNext, int newDScroller)
{
	dPrev = newDPrev;
	dNext = newDNext;
	dScroller = newDScroller;
}

void sciScrollBox::Posn(int x, int y)
{
	sciRect::Posn(x, y);
	previousButton->Posn(x, y);

	if (direction == GDefs::VERTICAL) {
		nextButton->Posn(x, y + GetHeight() - (nextButton->GetHeight() - 1));
		scroller->Posn(x + dScroller, y + previousButton->GetHeight() + dPrev);
		scroller->SetMinMax(scroller->GetY(), (nextButton->GetY() + dNext) - scroller->GetHeight());
	} else {
		nextButton->Posn(x + GetWidth() - (nextButton->GetWidth() - 1), y);
		scroller->Posn(x + previousButton->GetWidth() + dPrev, y + dScroller);
		scroller->SetMinMax(scroller->GetX(), (nextButton->GetX() + dNext) - scroller->GetWidth());
	}
	
	range = scroller->GetMax() - scroller->GetMin();
}

void sciScrollBox::SetSize(int width, int height)
{
	int x, y;
	
	sciRect::SetSize(width, height);
	
	x = GetX();
	y = GetY();
	
	previousButton->Posn(x, y);
	if (direction == GDefs::VERTICAL) {
		nextButton->Posn(x, y + GetHeight() - (nextButton->GetHeight() - 1));
		scroller->Posn(x + dScroller, y + previousButton->GetHeight() + dPrev);
		scroller->SetMinMax(scroller->GetY(), (nextButton->GetY() + dNext) - scroller->GetHeight());
	}
	else {
		nextButton->Posn(x + GetWidth() - (nextButton->GetWidth() - 1), y);
		scroller->Posn(x + previousButton->GetWidth() + dPrev, y + dScroller);
		scroller->SetMinMax(scroller->GetX(), (nextButton->GetX() + dNext) - scroller->GetWidth());
	}
	
	range = scroller->GetMax() - scroller->GetMin();
}

void sciScrollBox::SetPri(int newPri)
{
	sciRect::SetPri(newPri);
	previousButton->SetPri(newPri + 1);
	nextButton->SetPri(newPri + 1);
	scroller->SetPri(newPri + 1);
}

void sciScrollBox::SetViews(int newPrevView, int newNextView, int newScrollerView)
{
	prevButtonView	= newPrevView;
	nextButtonView	= newNextView;
	scrollerView	= newScrollerView;

	previousButton->SetView(prevButtonView);
	nextButton->SetView(nextButtonView);
	scroller->SetView(scrollerView);
}

void sciScrollBox::Show()
{
	sciRect::Show();
	previousButton->Show();
	nextButton->Show();
	if (enabled)
		scroller->Show();
	else
		scroller->Hide();
}

void sciScrollBox::Hide()
{
	sciRect::Hide();
	previousButton->Hide();
	nextButton->Hide();
	scroller->Hide();
}

void sciScrollBox::Enable()
{
	enabled = TRUE;
	SetScrollable();
	scroller->Show();
	previousButton->Enable();
	nextButton->Enable();
}

void sciScrollBox::Disable()
{
	enabled = FALSE;
	SetNonScrollable();
	scroller->Hide();
	previousButton->Disable();
	nextButton->Disable();
}

void sciScrollBox::SetScrollable()
{
	// Change background color to dark grey
	SetInnerColor(DEFAULT_DKGREY);
}

void sciScrollBox::SetNonScrollable()
{
	// Change background color to light grey
	SetInnerColor(DEFAULT_LTGREY);
}

int sciScrollBox::GetDirection()
{
	return(direction);
}

void sciScrollBox::SnapThumbnail(int firstItem)
{
	int newPosn = scroller->GetMin();

	if (firstItem && firstItem >= GetNumItems() - GetMaxVisibleItems())
		newPosn += range;
	else
		if (GetNumHiddenItems())
			newPosn += (range * firstItem) / GetNumHiddenItems();
	
	if (direction == GDefs::VERTICAL)
		scroller->Posn(scroller->GetX(), newPosn);
	else
		scroller->Posn(newPosn, scroller->GetY());
}

void sciScrollBox::Page(sciEvent *event)
{
	int maxVisibleItems = GetMaxVisibleItems();
	int eventPosn, scrollerPosn;

	if (direction == GDefs::VERTICAL) {
		eventPosn = event->y;
		scrollerPosn = scroller->GetY();
	} else {
		eventPosn = event->x;
		scrollerPosn = scroller->GetX();
	}

	if (eventPosn < scrollerPosn) {
		DoScroll(GDefs::REVERSE, maxVisibleItems <= 1 ? 1 : maxVisibleItems - 1);
	} else {
		DoScroll(GDefs::FORWARD, maxVisibleItems <= 1 ? 1 : maxVisibleItems - 1);
	}
}

void sciScrollBox::ScrollerRelease(sciEvent *event)
{
	int scrollerPosn;
	double unit, firstItem = 0;
	
	if (direction == GDefs::VERTICAL)
		scrollerPosn = scroller->GetY();
	else
		scrollerPosn = scroller->GetX();
	scrollerPosn -= scroller->GetMin();
	
	if (GetNumHiddenItems()) {
		unit = (double)range / (double)GetNumHiddenItems();
		firstItem = scrollerPosn / unit;
		if (fmod(firstItem, 1) < 0.5)
			firstItem = floor(firstItem);
		else
			firstItem = ceil(firstItem);
	}
	
	assert(firstItem >= 0 && (!GetNumItems() || firstItem < GetNumItems()));
	
	DoRefresh((int)firstItem);
}

int sciScrollBox::GetFirstVisibleItem()
{
	return selectedID;
}

int sciScrollBox::GetNumHiddenItems()
{
	int returnVal = GetNumItems() - GetMaxVisibleItems();

	if (returnVal < 0)
		returnVal = 0;

	return(returnVal);
}

int sciScrollBox::GetNumItems()
{
	return(0);
}

int sciScrollBox::GetMaxVisibleItems()
{
	return(0);
}

void sciScrollBox::DoScroll(int direction, int numItems)
{
	if (direction == GDefs::REVERSE) {
		if (selectedID - numItems < 0)
			selectedID = 0;
		else
			selectedID -= numItems;
	}
	else {
		if ((selectedID + (GetMaxVisibleItems() - 1) + numItems) > (GetNumItems() - 1)) {
			selectedID = GetNumItems() - GetMaxVisibleItems();
			if (selectedID < 0)
				selectedID = 0;
		}
		else
			selectedID += numItems;
	}
	assert(selectedID >= 0 && selectedID <= (GetNumItems() - GetMaxVisibleItems()));
	DoRefresh(selectedID);
}

void sciScrollBox::DoRefresh(int firstItem)
{
	selectedID = firstItem;
	SnapThumbnail(selectedID);
}

Boolean sciScrollBox::AtTop()
{
	return GetFirstVisibleItem() == 0;
}

Boolean sciScrollBox::AtBottom(int delta)
{
	return (GetFirstVisibleItem() + delta) >= (GetNumItems() - GetMaxVisibleItems());
}
