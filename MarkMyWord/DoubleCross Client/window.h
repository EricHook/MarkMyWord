#ifndef _window
#define _window

#include "vwindow.h"

class Window : public VWindow
	{
	public:
		Window();
		~Window();
		void MakeWindow(int width = 640, int height = 480, Boolean closeBox = FALSE);
		void Show();
		void Hide();
		virtual void Erase(RECT &rcPaint);
		void EraseOuter();
		void SetNormalCursor(HCURSOR hNewNormal);
		void SetSize(int width, int height);
		virtual void SetTitle(char *newTitle);
		void Move(int x, int y);
		void CopyToScreen(RECT &rcPaint);
		void GoFullScreen();
		void GoWindowed(Boolean changeINIFile = TRUE);
		BOOL InFullScreen();
		void ShowTitleInMenuBar();
		
		HWND hWnd;
		HDC hDC;
		HCURSOR hNormalCursor;
		HCURSOR hWaitCursor;
		HCURSOR hHandCursor;
		BOOL canMaximize;
		HCURSOR hLeftCursor;
		HCURSOR hRightCursor;
		HCURSOR hUpCursor;
		HCURSOR hDownCursor;
		HCURSOR hExitCursor;
		HCURSOR hOldNormal;
		HCURSOR hNullCursor;
		
		JOYINFO *newJoys;
		JOYINFO *oldJoys;

	protected:
		void CreateWindowedWindow(Boolean changeINIFile = TRUE, int width = 640, int height = 480, Boolean closeBox = FALSE);
		void CreateFullscreenWindow();
		void DestroyWindow();
		
		HWND urlWindow;
		BOOL inFullScreen;
		WNDCLASS wc;
	};

#endif // _window
