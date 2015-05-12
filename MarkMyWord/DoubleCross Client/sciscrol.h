#ifndef _sciscrol
#define _sciscrol

#include "sciactor.h"
#include "scitext.h"
#include "scibuttn.h"
#include "scirect.h"
#include "scigbox.h"
#include "gdefs.h"

class sciScrollBox;

class sciScrollButton : public sciButton
{
	public:
		sciScrollButton(sciScrollBox *newOwner, int newDirection);
		~sciScrollButton();
		void DoClick(BOOL netMessage = FALSE);
	
	protected:
		sciScrollBox *owner;
		int direction;
};

class sciScroller : public sciActor
{
	public:
		sciScroller(sciScrollBox *newOwner);
		~sciScroller();
		Boolean HandleEvent(sciEvent &event);
		void SetMinMax(int newMin, int newMax);
		int GetMin();
		int GetMax();
		Boolean TrackingMouse() { return mouseDown; }
	
	protected:
		sciScrollBox *owner;
		Boolean mouseDown;
		int xOffset, yOffset;
		int min, max;
};

class sciScrollBox : public sciRect
{
	public:
		sciScrollBox(int newDirection);
		~sciScrollBox();
		void Init();
		Boolean HandleEvent(sciEvent &event);
		void SetScrollerOffsets(int newDPrev, int newDNext, int newDScroller);
		void Posn(int x, int y);
		void SetSize(int width, int height);
		void SetPri(int newPri);
		void SetViews(int newPrevView, int newNextView, int newScrollerView);
		void Show();
		void Hide();
		void Enable();
		void Disable();
		void SetScrollable();
		void SetNonScrollable();
		int GetDirection();
		void SnapThumbnail(int firstItem);
		void Page(sciEvent *event);
		void ScrollerRelease(sciEvent *event);
		int GetFirstVisibleItem();
		int GetNumHiddenItems();
		
		virtual int GetNumItems();
		virtual int GetMaxVisibleItems();
		
		virtual void DoScroll(int direction, int numItems);
		virtual void DoRefresh(int firstItem);
		
		Boolean AtTop();
		Boolean AtBottom(int delta = 0);
		
		sciScrollButton *previousButton;
		sciScrollButton *nextButton;
		sciScroller *scroller;
	
	protected:
		int direction;
		Boolean enabled;
		int range;
		int selectedID;
		
		int prevButtonView;
		int nextButtonView;
		int scrollerView;
		
		int dNext;
		int dPrev;
		int dScroller;
};

#endif // _sciscrol
