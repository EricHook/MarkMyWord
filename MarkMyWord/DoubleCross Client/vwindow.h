#ifndef _vwindow
#define _vwindow

#include "object.h"
#include "scilist.h"

class sciPlane;
class FrameBuffer;
class Palette;

class VWindow : public Object
	{
	public:
		VWindow();
		virtual ~VWindow();
		virtual void MakeWindow();
		virtual void Show();
		virtual void Hide();
		
		virtual void Erase(RECT &rcPaint) = 0;
		void Paint(RECT &rcPaint);
		void TruePaintDirty();
		void TruePaint(RECT &rcPaint);
		virtual void CopyToScreen(RECT &rcPaint) = 0;
		void PaintAll();
		void UnPaintAll();
		void ForceUpdate();
		virtual void SetTitle(char *newTitle);
		Boolean EditCursorIsFlashingInWindow();
	
		WORD Width();
		WORD Height();

		BOOL IsHidden();
		BOOL IsNotHidden();
		void SetSize(int width, int height);

		RECT rcOuter, rcInner;
		int x, y;
		FrameBuffer *frameBuffer;
		Palette *pCurrentPalette;

	protected:
		void OffscreenUpdate(RECT &rect);
		void OffscreenErase(RECT &rect);

		sciList	updateRects;
		RECT	dirtyRect;
		int		isNotHidden;
		char	*title;
	};

#endif
