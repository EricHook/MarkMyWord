#include "os.h"
#include "sci.h"
#include "core.h"

#include "Client Include.h"

// #include <dbt.h>
// #define INITGUID
// #include <ddraw.h>

using namespace WONplayDLL;

#ifndef WM_MOUSEWHEEL
#	define WM_MOUSEWHEEL 0x020A
#endif

// #include "cashelp.h"

extern "C" long FAR PASCAL WndProc(HWND hWnd, unsigned message, unsigned wParam, LONG lParam);
// void ProcessNotificationMessage(HWND ,WPARAM);
BYTE InverseColorLookup(BYTE r, BYTE g, BYTE b);

extern char cdDriveLetter;
extern double defaultGamma;
extern char helpFilename[];
extern int numBytesAllocated;
extern int maxMemoryAllocated;
extern BYTE btColorBitSupport;
extern HINSTANCE hInstance;
extern BTMP_RES btmpResGlobalHeader;
extern BOOL useDD;
extern BOOL confirmQuit;
extern BOOL win32s;

void ClipSomeRects(RECT &rcDest, RECT &rcDestClippingRect,
		RECT &rcSource, RECT &rcSourceClippingRect);

BOOL returnToFullScreen = FALSE;

// BOOL DDInit(HWND);
// BOOL DDSetMode(int width, int height, int bpp);
// void DDTerm(HWND);

// LPDIRECTDRAW dd;

#define JOYSTICK_SENSITIVITY 512

/**************************************************************************
  DDInit

  Description:
    initialize all the DirectDraw specific stuff
 **************************************************************************/

/*
HRESULT DirectDrawCreate2( GUID FAR *lpGUID, LPDIRECTDRAW FAR *lplpDD, IUnknown FAR *pUnkOuter  ) 
{
	HRESULT hResult;

	hResult = CoCreateInstance(CLSID_DirectDraw,
                                       NULL,
                                       CLSCTX_INPROC_SERVER,
                                       IID_IDirectDraw,
                                       (void**)lplpDD);

	if( !FAILED(hResult) )
	{
		hResult = (*lplpDD)->Initialize(lpGUID);
	}

	return hResult;
}

BOOL DDInit(HWND hwndApp)
{
    HRESULT err;

    err = DirectDrawCreate2(NULL, &dd, NULL);

    if (err != DD_OK)
        return FALSE;

    err = dd->SetCooperativeLevel(hwndApp,
        DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN);

    if (err != DD_OK)
        return FALSE;

    return TRUE;
}
*/

/**************************************************************************
  DDSetMode
 **************************************************************************/

/*
BOOL DDSetMode(int width, int height, int bpp)
{
    HRESULT err;

    err = dd->SetDisplayMode(width, height, bpp);

    if (err != DD_OK)
        return FALSE;

    return TRUE;
}
*/

/**************************************************************************
  DDTerm
 **************************************************************************/

/*
void DDTerm(HWND hwnd)
{
	if (dd)
		{
		dd->RestoreDisplayMode();
		dd->SetCooperativeLevel(hwnd, DDSCL_NORMAL);
		dd->Release();
		dd = NULL;
		}
}
*/

void ShowHelpTopic(int id)
	{
	if (MAIN_WINDOW->InFullScreen())
		{
		MAIN_WINDOW->GoWindowed(FALSE);
		game->CheckFillScreenMenuItem(FALSE);
		// returnToFullScreen = TRUE;
		}
	WinHelp(MAIN_HWND, helpFilename, HELP_CONTEXT, id);
	}

Window::Window()
	{
	hDC = NULL;
	hWaitCursor = NULL;
	hNormalCursor = NULL;
	hLeftCursor = NULL;
	hRightCursor = NULL;
	hUpCursor = NULL;
	hDownCursor = NULL;
	hExitCursor = NULL;
	hNullCursor = NULL;
	hOldNormal = NULL;
	newJoys = NULL;
	oldJoys = NULL;
	canMaximize = TRUE;
	hWnd = 0;
	inFullScreen = FALSE;
	wc.lpszClassName = NULL;
	VWindow::SetTitle((char *)szAppTitle);
	}

Window::~Window()
	{
	// Cleanup joystick.
	if (canMaximize)
		{
		joyReleaseCapture(JOYSTICKID1);
		joyReleaseCapture(JOYSTICKID2);
		delete[] newJoys;
		delete[] oldJoys;
		}

	// Remove the reference to this from the window class.
	// Otherwise, member functions might be called.
	SetWindowLong(hWnd, 0, 0L);

	// Shut down help system.
	WinHelp(hWnd, helpFilename, HELP_QUIT, 0L);

	DestroyWindow();

	delete[] (char *)wc.lpszClassName;
	}

void Window::MakeWindow(int width, int height, Boolean closeBox)
	{
	int screenWidth, screenHeight;
	BOOL goFullScreen;
	char szEntry[32];
	char *className;
	
	VWindow::MakeWindow();
	pApp->pIniFile->ReadString("Gamma", "1.5", szEntry, 5);
	defaultGamma = atof(szEntry);
	pCurrentPalette->SetGamma(defaultGamma);
	pApp->pIniFile->ReadString("FullScreen", "No", szEntry, 5);
	
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);
	
	if (stricmp(szEntry, "No") || useDD || (screenWidth <= 640 && width >= 640))
		goFullScreen = TRUE;
	else
		goFullScreen = FALSE;
	
#if defined(SIGS_DEMO) || !defined(HOYLE_CASINO2)
	goFullScreen = FALSE;
#endif
	
	SetSize(width, height);
	
	hNormalCursor = LoadCursor(NULL, IDC_ARROW);
	hWaitCursor = LoadCursor(NULL, IDC_WAIT);
	hHandCursor = LoadCursor(hInstance, MAKEINTRESOURCE(1000));
	hLeftCursor = LoadCursor(hInstance, MAKEINTRESOURCE(1000));
	hRightCursor = LoadCursor(hInstance, MAKEINTRESOURCE(1000));
	hUpCursor = LoadCursor(hInstance, MAKEINTRESOURCE(1000));
	hDownCursor = LoadCursor(hInstance, MAKEINTRESOURCE(1000));
	hExitCursor = LoadCursor(hInstance, MAKEINTRESOURCE(1000));
	hNullCursor = LoadCursor(hInstance, MAKEINTRESOURCE(1000));
	hOldNormal = hNormalCursor;
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	className = new char[strlen((char *)szAppTitle) + strlen(" Class") + 1];
	strcpy(className, (char *)szAppTitle);
	strcat(className, " Class");
	wc.lpszClassName = className;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = sizeof(Window *);
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wc.hCursor = hNormalCursor;
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL; // MAKEINTRESOURCE(ROOMINT_MENU)
	
	RegisterClass(&wc);

	if (goFullScreen == TRUE)
		{
		CreateFullscreenWindow();
		}
	else
		{
		CreateWindowedWindow(FALSE, width, height, closeBox);
		}

	Erase(rcInner);
	EraseOuter();

	SetTimer( hWnd, 1, 100, NULL );

	int i;

	if (btmpResGlobalHeader.biXPelsPerMeter != 2835)
		{
		for(i=0; i<256; ++i)
			{
			btmpResGlobalHeader.bmiColors[i].rgbRed = 0;
			btmpResGlobalHeader.bmiColors[i].rgbGreen = 0;
			btmpResGlobalHeader.bmiColors[i].rgbBlue = 0;
			btmpResGlobalHeader.bmiColors[i].rgbReserved = 0;
			}
		btmpResGlobalHeader.biSize = sizeof(BITMAPINFOHEADER);
		btmpResGlobalHeader.biWidth = Width();
		btmpResGlobalHeader.biHeight = Height();
		btmpResGlobalHeader.biPlanes = 1;
		btmpResGlobalHeader.biBitCount = 16;
		btmpResGlobalHeader.biCompression = BI_RGB;
		btmpResGlobalHeader.biSizeImage = 0;
		btmpResGlobalHeader.biXPelsPerMeter = 2835;
		btmpResGlobalHeader.biYPelsPerMeter = 2835;
		btmpResGlobalHeader.biClrUsed = 0;
		btmpResGlobalHeader.biClrImportant = 0;
		}

	// Setup joystick.
	joySetCapture(hWnd, JOYSTICKID1, 16, TRUE);
	joySetThreshold(JOYSTICKID1, JOYSTICK_SENSITIVITY);
	joySetCapture(hWnd, JOYSTICKID2, 16, TRUE);
	joySetThreshold(JOYSTICKID2, JOYSTICK_SENSITIVITY);
	newJoys = new JOYINFO[joyGetNumDevs()];
	oldJoys = new JOYINFO[joyGetNumDevs()];
	for (i = 0; i < int(joyGetNumDevs()); ++i)
		{
		oldJoys[i].wXpos = 32768;
		oldJoys[i].wYpos = 32768;
		oldJoys[i].wButtons = 0;
		newJoys[i].wXpos = 32768;
		newJoys[i].wYpos = 32768;
		newJoys[i].wButtons = 0;
		}
	}

void Window::Show()
	{
	long styles;
	
	// BOOL maximize;
	
	// If window is minimized, restore it.
	WINDOWPLACEMENT wp;
	wp.length = sizeof(wp);
	wp.showCmd = SW_SHOWMINIMIZED;
	GetWindowPlacement(hWnd, &wp);
	if (wp.showCmd == SW_SHOWMINIMIZED)
		ShowWindow(hWnd, SW_RESTORE);

	/*
	if (canMaximize)
		maximize = pApp->pIniFile->ReadValue("MaximizeWindow", FALSE);
	else
		maximize = FALSE;
	if (maximize || (canMaximize && GetSystemMetrics(SM_CXSCREEN) <= 640))
		ShowWindow(hWnd, SW_SHOWMAXIMIZED);
	else
	*/
		ShowWindow(hWnd, SW_SHOW);
	VWindow::Show();
	
	if (GetForegroundWindow() && GetForegroundWindow() == GetActiveWindow())
		{
		styles = GetWindowLong(GetForegroundWindow(), GWL_EXSTYLE);
		if (styles & WS_EX_TOPMOST)
			return; // don't steal the focus from our own topmost (chat) window
		}
	
	SetForegroundWindow(hWnd);
	}

void Window::Hide()
	{
	ShowWindow(hWnd, SW_HIDE);
	VWindow::Hide();
	}

void Window::Erase(RECT &rcPaint)
	{
	if (hDC == NULL)
		return;
	PatBlt(hDC, rcInner.left, rcInner.top, rcInner.right, rcInner.bottom, BLACKNESS);
	}

void Window::EraseOuter()
	{
	RECT rcTemp;

	if (hDC == NULL)
		return;
	// Blacken areas outside the game screen.

	// Top
	SetRect(&rcTemp, 0, 0, rcOuter.right, rcInner.top);
	PatBlt(hDC, rcTemp.left, rcTemp.top, rcTemp.right, rcTemp.bottom, BLACKNESS);

	// Left
	SetRect(&rcTemp, 0, 0, rcInner.left, rcOuter.bottom);
	PatBlt(hDC, rcTemp.left, rcTemp.top, rcTemp.right, rcTemp.bottom, BLACKNESS);

	// Bottom
	SetRect(&rcTemp, 0, rcInner.bottom, rcOuter.right, rcOuter.bottom);
	PatBlt(hDC, rcTemp.left, rcTemp.top, rcTemp.right, rcTemp.bottom, BLACKNESS);

	// Right
	SetRect(&rcTemp, rcInner.right, 0, rcOuter.right, rcOuter.bottom);
	PatBlt(hDC, rcTemp.left, rcTemp.top, rcTemp.right, rcTemp.bottom, BLACKNESS);
	}

BOOL bProcessingMouseMove = FALSE;

extern "C" LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// char filename[32];
	BOOL doBoth;
	int i, x, y; // driveBit
	POINT point;
	LRESULT retValue;
	// FILE *file;
	PAINTSTRUCT ps;
	// DEV_BROADCAST_HDR *devBroadcastHdr;
	// DEV_BROADCAST_VOLUME *devBroadcastVolume;
	Window *pWindow;
	sciPlane *plane;
	sciEvent event;
	HWND findHWND;
	PCOPYDATASTRUCT pcds;
	WONadInfo wonAdInfo;	
	WONERR wonErr;
	
	pWindow = (Window *) GetWindowLong(hWnd, 0);
	event.message = message;
	event.lparam = lParam;
	event.wparam = wParam;
	
	if (message != WM_CREATE && pWindow == NULL)
		return DefWindowProc(hWnd, message, wParam, lParam);
	
	switch (message)
		{
		/*
		case _SOS_COMMAND:
			switch( LOWORD( wParam ) )
				{
				case _SOS_STREAM_DONE:
				   // release all of the buffers associated to the stream
					//pApp->pSounder->EndSong();
				   //sosEZStopStream( ( HSOS )LOWORD( lParam ) );

				   break;

				case _SOS_STREAM_BUFFER_DONE:
				   // call the stream system to fill and send the next buffer
					// pApp->pSounder->PumpSong(( PSOSSTREAMELEMENT )lParam);
				   //sosEZFillStreamBuffer( ( ( PSOSSTREAMELEMENT )lParam )
					//  ->hStream, ( PSOSSTREAMELEMENT )lParam );

				   break;

				case _SOS_SAMPLE_CALLBACK:
					switch (LOWORD(lParam))
						{
						case 0x2000:
							break;
						case 0x4000:
							DebugString("Received END_SAMPLE.\n");
							pApp->pSounder->EndSoundNum(HIWORD(lParam), FALSE);
							break;
						}
					break;
				}
			break;
		*/

		case WM_TIMER:
			// keep sending WM_MOUSEMOVE messages to all of our real windows
			// even when the mouse is outside them
			
			GetCursorPos(&point);
			findHWND = WindowFromPoint(point);
			
			for (i = 0 ; i < pApp->planes.Size() ; i++)
				{
				plane = (sciPlane *)pApp->planes.At(i);
				if (plane->GetParentPlane() == NULL && plane->GetWindow()->hWnd != findHWND)
					{
					GetCursorPos(&point);
					MapWindowPoints(GetDesktopWindow(), plane->GetWindow()->hWnd, &point, 1);
					SendMessage(plane->GetWindow()->hWnd, WM_MOUSEMOVE, 0, MAKELPARAM(point.x, point.y));
					}
				}
			
			// Keep the sound alive.
			if (win32s)
				if (pApp && pApp->pSounder)
					pApp->pSounder->Doit();
			break;

		case WM_CREATE:
			SetWindowLong(hWnd, 0, *(long *)lParam);
			pWindow = (Window *) GetWindowLong(hWnd, 0);
			pWindow->hWnd = hWnd;
			break;
		case WM_PAINT:
			BeginPaint(hWnd, &ps);
				pWindow->ShowTitleInMenuBar();
				OffsetRect(&(ps.rcPaint), -pWindow->rcInner.left, -pWindow->rcInner.top);
				pWindow->TruePaint(ps.rcPaint);
			EndPaint(hWnd, &ps);
			break;
		case WM_ERASEBKGND:
			// Only erase the outer area.
			pWindow->EraseOuter();
			break;
		case WM_COMMAND:
			// ProcessNotificationMessage(hWnd, wParam);
			break;
		case WM_ENTERMENULOOP:
			break;
		case WM_EXITMENULOOP:
			break;
		case WM_CLOSE:
			if (hWnd != MAIN_HWND)
				{
				for (i = 0 ; i < pApp->planes.Size() ; i++)
					{
					plane = (sciPlane *)pApp->planes.At(i);
					if (plane->GetParentPlane() == NULL && plane->GetWindow() == pWindow)
						{
						plane->AttemptClosePlane();
						break;
						}
					}
				break;
				}
		// FALL THROUGH
		case WM_QUIT:
			if (!pWindow->InFullScreen())
				pWindow->Show();
			event.plane = pApp->GetModalPlane();
			// If there's a modal plane up, don't exit.
			if (event.plane)
				break;

			if (game->GetCurrentRoomNum() == ROOM_END)
				game->QuitGame(TRUE);
			else
				game->QuitGame(!confirmQuit);
			break;
		case WM_LBUTTONDOWN:
			SetCapture(hWnd);
			point.x = short(LOWORD(lParam))-pWindow->rcInner.left;
			point.y = short(HIWORD(lParam))-pWindow->rcInner.top;

			// Now do an SCI event.
			event.type = MOUSE_DOWN;
			event.modifiers = LEFT_MOUSE;
			if (GetKeyState(VK_CONTROL)&0xf000)
				event.modifiers |= CTRL_DOWN;
			if (GetKeyState(VK_SHIFT)&0xf000)
				event.modifiers |= SHIFT_DOWN;
			if (GetKeyState(VK_MENU)&0xf000)
				event.modifiers |= ALT_DOWN;
			event.claimed = FALSE;
			event.x = point.x;
			event.y = point.y;
			event.plane = pApp->FindPlane(event.x, event.y, pWindow);
			if (event.plane && event.plane->GetWindow() == pWindow)
				{
				event.x -= event.plane->GetX();
				event.y -= event.plane->GetY();
				game->HandleEvent(event);
				}
			break;
		/*case WM_MOUSEACTIVATE:
			point.x = 0;
			point.y = 0;

			// Now do an SCI event.
			event.type = MOUSE_DOWN;
			event.modifiers = LEFT_MOUSE;
			event.claimed = FALSE;
			event.x = point.x;
			event.y = point.y;
			event.plane = game->curRoom->FindPlane(event.x, event.y, pWindow);
			game->HandleEvent(event);
			break;
		*/
		case WM_LBUTTONUP:
			ReleaseCapture();
			point.x = short(LOWORD(lParam))-pWindow->rcInner.left;
			point.y = short(HIWORD(lParam))-pWindow->rcInner.top;

			// Now do an SCI event.
			event.type = MOUSE_UP;
			event.modifiers = LEFT_MOUSE;
			if (GetKeyState(VK_CONTROL)&0xf000)
				event.modifiers |= CTRL_DOWN;
			if (GetKeyState(VK_SHIFT)&0xf000)
				event.modifiers |= SHIFT_DOWN;
			if (GetKeyState(VK_MENU)&0xf000)
				event.modifiers |= ALT_DOWN;
			event.claimed = FALSE;
			event.x = point.x;
			event.y = point.y;
			event.plane = pApp->FindPlane(event.x, event.y, pWindow);
			if (event.plane && event.plane->GetWindow() == pWindow)
				{
				event.x -= event.plane->GetX();
				event.y -= event.plane->GetY();
				game->HandleEvent(event);
				}
			break;
		case WM_RBUTTONDOWN:
			point.x = short(LOWORD(lParam))-pWindow->rcInner.left;
			point.y = short(HIWORD(lParam))-pWindow->rcInner.top;

			// Now do an SCI event.
			event.type = MOUSE_DOWN;
			event.modifiers = RIGHT_MOUSE;
			if (GetKeyState(VK_CONTROL)&0xf000)
				event.modifiers |= CTRL_DOWN;
			if (GetKeyState(VK_SHIFT)&0xf000)
				event.modifiers |= SHIFT_DOWN;
			if (GetKeyState(VK_MENU)&0xf000)
				event.modifiers |= ALT_DOWN;
			event.claimed = FALSE;
			event.x = point.x;
			event.y = point.y;
			event.plane = pApp->FindPlane(event.x, event.y, pWindow);
			if (event.plane && event.plane->GetWindow() == pWindow)
				{
				event.x -= event.plane->GetX();
				event.y -= event.plane->GetY();
				game->HandleEvent(event);
				}
			break;
		case WM_RBUTTONUP:
			point.x = short(LOWORD(lParam))-pWindow->rcInner.left;
			point.y = short(HIWORD(lParam))-pWindow->rcInner.top;

			// Now do an SCI event.
			event.type = MOUSE_UP;
			event.modifiers = RIGHT_MOUSE;
			if (GetKeyState(VK_CONTROL)&0xf000)
				event.modifiers |= CTRL_DOWN;
			if (GetKeyState(VK_SHIFT)&0xf000)
				event.modifiers |= SHIFT_DOWN;
			if (GetKeyState(VK_MENU)&0xf000)
				event.modifiers |= ALT_DOWN;
			event.claimed = FALSE;
			event.x = point.x;
			event.y = point.y;
			event.plane = pApp->FindPlane(event.x, event.y, pWindow);
			if (event.plane && event.plane->GetWindow() == pWindow)
				{
				event.x -= event.plane->GetX();
				event.y -= event.plane->GetY();
				game->HandleEvent(event);
				}
			break;
		case WM_MOUSEMOVE:
			if (bProcessingMouseMove || !game || !game->curRoom)
				break;
			bProcessingMouseMove = TRUE;
			point.x = short(LOWORD(lParam))-pWindow->rcInner.left;
			point.y = short(HIWORD(lParam))-pWindow->rcInner.top;

			// Now do an SCI event.
			event.type = MOUSE_MOVE;
			event.modifiers = (EVENT_MODIFIER)0;
			if (GetKeyState(VK_CONTROL)&0xf000)
				event.modifiers |= CTRL_DOWN;
			if (GetKeyState(VK_SHIFT)&0xf000)
				event.modifiers |= SHIFT_DOWN;
			if (GetKeyState(VK_MENU)&0xf000)
				event.modifiers |= ALT_DOWN;
			event.claimed = FALSE;
			event.x = point.x;
			event.y = point.y;
			event.plane = pApp->FindPlane(event.x, event.y, pWindow);
			if (event.plane && event.plane->GetWindow() == pWindow)
				{
				event.x -= event.plane->GetX();
				event.y -= event.plane->GetY();
				game->HandleEvent(event);
				}
			
			bProcessingMouseMove = FALSE;
			break;

		case WM_MOUSEWHEEL:
			if (bProcessingMouseMove || !game || !game->curRoom)
				break;
//			fwKeys = LOWORD(wParam);    // key flags

			// horizontal/vertical position of pointer
			point.x = short(LOWORD(lParam))-pWindow->rcInner.left;
			point.y = short(HIWORD(lParam))-pWindow->rcInner.top;

			// Now do an SCI event.
			event.type = MOUSE_WHEEL;
			event.z = (short) HIWORD(wParam);    // wheel rotation
			event.modifiers = (EVENT_MODIFIER)0;
			if (GetKeyState(VK_CONTROL)&0xf000)
				event.modifiers |= CTRL_DOWN;
			if (GetKeyState(VK_SHIFT)&0xf000)
				event.modifiers |= SHIFT_DOWN;
			if (GetKeyState(VK_MENU)&0xf000)
				event.modifiers |= ALT_DOWN;
			event.claimed = FALSE;
			event.x = point.x;
			event.y = point.y;
			event.plane = pApp->FindPlane(event.x, event.y, pWindow);
			if (event.plane && event.plane->GetWindow() == pWindow)
				{
					// calculate window relative event position
					event.x -= pWindow->x;
					event.x -= event.plane->GetX();

					event.y -= pWindow->y;
					event.y -= event.plane->GetY();
					game->HandleEvent(event);
				}
			break;

		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_CHAR:
			// Translate vk key to ascii.
			if (wParam <= 26)
				{
				point.x = wParam + 0x60;
				event.modifiers = (EVENT_MODIFIER)CTRL_DOWN;
				}
			else
				{
				point.x = wParam;
				event.modifiers = (EVENT_MODIFIER)0;
				}
#ifdef _DEBUG
if (wParam == 1)
	{
	char str[64];

	wsprintf(str, "Max Memory used = %d.\n", maxMemoryAllocated);
	MessageBox(NULL, str, "Memory", MB_OK | MB_TASKMODAL);
	wsprintf(str, "Current Memory used = %d.\n", numBytesAllocated);
	MessageBox(NULL, str, "Memory", MB_OK | MB_TASKMODAL);
	}
#endif
			// Now do an SCI event.
			doBoth = FALSE;
			switch(message)
				{
				case WM_KEYDOWN:
					if ((wParam > 27) && (wParam < 41) && (wParam != 32))
						{
						// These keys aren't converted to WM_CHAR.
						event.type = KEY_PRESS;
						doBoth = TRUE;
						}
					else
						{
						event.type = KEY_DOWN;
						}
					break;
				case WM_KEYUP:
					event.type = KEY_UP;
					break;
				case WM_CHAR:
					event.type = KEY_PRESS;
					break;
				}
			event.claimed = FALSE;
			event.x = 0;
			event.y = 0;
			event.c = (char)wParam;
/*			event.plane = pApp->FindPlane(event.x, event.y, pWindow);
			if (event.plane)
				{
				game->HandleEvent(event);
				if (doBoth)
					{
					event.type = KEY_DOWN;
					game->HandleEvent(event);
					}
				}*/
			for (i = pApp->planes.Size() - 1 ; i >= 0 ; i--)
			{
				event.plane = (sciPlane*)(pApp->planes.At(i));
				if (event.plane->GetWindow() != pWindow)
					continue;
				game->HandleEvent(event);
				if (doBoth)
				{
					event.type = KEY_DOWN;
					game->HandleEvent(event);
				}
			}
		
			break;
		case WM_SYSKEYDOWN:
			if ((wParam == 13) &&
					(game->GetCurrentRoomNum() != ROOM_INTRO) &&
					(game->GetCurrentRoomNum() != ROOM_CREDITS))
				{
				/*
				// don't do full screen for internet play
				if (pWindow->InFullScreen())
					{
					pWindow->GoWindowed();
					game->CheckFillScreenMenuItem(FALSE);
					}
				else
					{
					pWindow->GoFullScreen();
					game->CheckFillScreenMenuItem(TRUE);
					}
				returnToFullScreen = FALSE;
				*/
				}
			else if ((wParam >= '0') && (wParam <= '9'))
				{
				/*
				// chat sound keys handled elsewhere to avoid system beep
				if (roomClient)
					roomClient->ChatSoundKeyPressed((char)wParam);
				*/
				}
			else
				if (pApp->GetModalPlane())
					return 0;
			return DefWindowProc(hWnd, message, wParam, lParam);

		case WM_SYSKEYUP:
			return 0; // don't allow alt key to freeze game since we don't have menus now
		
		case WM_ACTIVATEAPP:
			// wParam = TRUE if being activated.
			// wParam = FALSE if being de-activated
			DebugString("Window WM_ACTIVATEAPP: ");
			DebugString(wParam ? "ACTIVATED\n" : "DEACTIVATED\n");
			retValue = DefWindowProc(hWnd, message, wParam, lParam);
			if (wParam && game && game->curRoom)
				{
				if (returnToFullScreen)
					{
					pWindow->GoFullScreen();
					game->CheckFillScreenMenuItem(TRUE);
					returnToFullScreen = FALSE;
					}
				pWindow->pCurrentPalette->ActivatePalette(pWindow->hDC);
				pWindow->ForceUpdate();
				}
			
			return retValue;
		
		case WM_SETFOCUS:
			// TODO - see how often this occurs - mds
			pWindow->ForceUpdate();
			if (roomClient && roomClient->rsvpPlane && roomClient->rsvpPlane->IsNotHidden())
				BringWindowToTop(roomClient->rsvpPlane->GetWindow()->hWnd);
			break;
		
		case WM_DISPLAYCHANGE:
			DebugString("Window WM_DISPLAYCHANGE\n");
			btColorBitSupport = (BYTE)wParam;
			if (btColorBitSupport == 8)
				{
				pWindow->pCurrentPalette->tableCalculated = FALSE;
				pWindow->pCurrentPalette->CalculateInverseTable();
				}
			SendMessage(hWnd, WM_PALETTECHANGED, (WPARAM)GetDesktopWindow(), 0);
			break;
		
		case WM_QUERYNEWPALETTE:
			DebugString("Window QueryNewPalette\n");
			pWindow->pCurrentPalette->ActivatePalette(pWindow->hDC);
			pWindow->PaintAll();
			return TRUE;

		case WM_PALETTECHANGED:
			if ((HWND) wParam != hWnd)
				{
				if (game && game->curRoom)
					{
					DebugString("Window PaletteChanged\n");
					pWindow->pCurrentPalette->ActivatePalette(pWindow->hDC);
					pWindow->PaintAll();
					}
				}
			break;

		/*
		case WM_DEVICECHANGE:
			switch (wParam)
				{
				case DBT_DEVICEREMOVECOMPLETE:
					devBroadcastHdr = (DEV_BROADCAST_HDR*) lParam;
					if (devBroadcastHdr->dbch_devicetype == DBT_DEVTYP_VOLUME)
						{
						devBroadcastVolume = (DEV_BROADCAST_VOLUME*) lParam;
						driveBit = devBroadcastVolume->dbcv_unitmask;
						while (cdDriveLetter && 
								(driveBit == (1 << (cdDriveLetter - 'a'))))
							{
							// If window is minimized, restore it.
							WINDOWPLACEMENT wp;
							wp.length = sizeof(wp);
							wp.showCmd = SW_SHOWMINIMIZED;
							GetWindowPlacement(hWnd, &wp);
							if (wp.showCmd == SW_SHOWMINIMIZED)
								ShowWindow(hWnd, SW_RESTORE);
							SetForegroundWindow(hWnd);

							if (IDYES == MessageBoxA(NULL,
								"Do you want to quit?\n\nIf not, re-insert the CD and click <No>", "CD ejected!", MB_YESNO  | MB_TASKMODAL))
								{
								game->QuitGame(TRUE);
								driveBit = 0;
								}
							else
								{
								Sleep(5000);
								wsprintf(filename, "%c:\\%s", cdDriveLetter, szResourceFilename);
								file = fopen(filename, "rb");
								if (file != NULL)
									{
									fclose(file);
									driveBit = 0;
									}
								}
							}
						}
					break;
				case DBT_DEVICEQUERYREMOVE:
					// Deny removal of our CD.
					// Note: Windows will still ask the user "are you sure",
					// and allow them to eject it.
					devBroadcastHdr = (DEV_BROADCAST_HDR*) lParam;
					if (devBroadcastHdr->dbch_devicetype == DBT_DEVTYP_VOLUME)
						{
						devBroadcastVolume = (DEV_BROADCAST_VOLUME*) lParam;
						driveBit = devBroadcastVolume->dbcv_unitmask;
						if (cdDriveLetter && 
								(driveBit == (1 << (cdDriveLetter - 'a'))))
							{
							return BROADCAST_QUERY_DENY;
							}
						}
					break;
				}
			break;
		*/
		/*
		case WM_HELP:	// <F1> on a dialog.
			ShowHelpTopic(IDH_CONTENTS);
			break;
		*/
		case WM_MOVE:
			pWindow->x = (int) LOWORD(lParam);
			pWindow->y = (int) HIWORD(lParam);
			break;
		
		case WM_NCHITTEST:
			// allow everything as normal except menus when a modal plane is up
			retValue = DefWindowProc(hWnd, message, wParam, lParam);
#if 0
			if ((retValue == HTMENU) &&
				game && game->curRoom && game->curRoom->GetModalPlane())
				return HTERROR;
			else
				if (retValue == HTCAPTION)
					if (pWindow->InFullScreen() || GetSystemMetrics(SM_CXSCREEN) == 640)
						return HTNOWHERE;
#endif
			return retValue;
			
/*			point.x = short(LOWORD(lParam))-pWindow->rcInner.left;
			point.y = short(HIWORD(lParam))-pWindow->rcInner.top;
			ScreenToClient(pWindow->hWnd, &point);
			if (game && game->curRoom && game->curRoom->GetModalPlane() &&
				(game->curRoom->FindPlane(point.x, point.y) != game->curRoom->GetModalPlane()))
				return HTERROR;
			else {
				retValue = DefWindowProc(hWnd, message, wParam, lParam);
				
				if (pWindow->canMaximize && GetSystemMetrics(SM_CXSCREEN) <= 640)
					if (retValue == HTCAPTION)
						return HTNOWHERE;
				
				return retValue;
			}
*/		
		case WM_SYSCOMMAND:
			//if (game && game->curRoom && game->curRoom->GetModalPlane() && !IsIconic(hWnd))
			//	break;
#if 0
			if (pWindow->canMaximize && GetSystemMetrics(SM_CXSCREEN) <= 640)
				if (wParam == SC_SIZE || wParam == SC_MOVE)
					return 0;
#endif		
			return DefWindowProc(hWnd, message, wParam, lParam);
		
		case WM_SIZE:
			switch (wParam)
				{
				case SIZE_MINIMIZED:
					break;
				case SIZE_MAXIMIZED:
					if (pWindow && pWindow->IsNotHidden() && pWindow->canMaximize)
						pApp->pIniFile->WriteValue("MaximizeWindow", 1);
					break;
				case SIZE_RESTORED:
					if (pWindow && pWindow->IsNotHidden() && pWindow->canMaximize)
						pApp->pIniFile->WriteValue("MaximizeWindow", 0);
					break;
				}

			if (wParam == SIZE_MINIMIZED)
				{
				}
			else
				{
				x = (int)LOWORD(lParam);
				y = (int)HIWORD(lParam);
				x = (x - 640) / 2;
				y = (y - 480) / 2;
				if (x < 0)
					x = 0;
				if (y < 0)
					y = 0;
				SetRect(&(pWindow->rcInner), x, y, x + 640, y + 480);
				GetClientRect(pWindow->hWnd, &(pWindow->rcOuter));

				// Blacken the outer area.
				pWindow->EraseOuter();
				}

			// Repaint game screen.
			pWindow->PaintAll();

			return DefWindowProc(hWnd, message, wParam, lParam);
		
		case WM_SETCURSOR:
			if (LOWORD(lParam) == HTCLIENT) {
				if (IsWindowEnabled(hWnd) && roomClient && roomClient->MouseIsOverAd())
					SetCursor(pWindow->hHandCursor);
				else {
					if (IsWindowEnabled(hWnd) && roomClient && roomClient->MouseIsOverChatSizeBar())
						SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZENS)));
					else
						SetCursor(pWindow->hNormalCursor);
				}
				
				return TRUE;
			}
			
			return DefWindowProc(hWnd, message, wParam, lParam);

		case MM_JOY1BUTTONDOWN:
		case MM_JOY2BUTTONDOWN:
			/*
			event.type = (EVENT_TYPE)0;
			if (wParam & JOY_BUTTON1CHG)
				{
				DebugString("JOYSTICK_BUTTON_1_DOWN\n");
				event.type = JOYSTICK_BUTTON_1_DOWN;
				}
			if (wParam & JOY_BUTTON2CHG)
				{
				DebugString("JOYSTICK_BUTTON_2_DOWN\n");
				event.type = JOYSTICK_BUTTON_2_DOWN;
				}
			if (wParam & JOY_BUTTON3CHG)
				{
				DebugString("JOYSTICK_BUTTON_3_DOWN\n");
				event.type = JOYSTICK_BUTTON_3_DOWN;
				}
			if (wParam & JOY_BUTTON4CHG)
				{
				DebugString("JOYSTICK_BUTTON_4_DOWN\n");
				event.type = JOYSTICK_BUTTON_4_DOWN;
				}
			if (event.type)
				{
				if (message == MM_JOY1BUTTONDOWN)
					event.modifiers = JOYSTICK_1;
				else
					event.modifiers = JOYSTICK_2;
				event.claimed = FALSE;
				event.x = 0;
				event.y = 0;
				event.c = 0;
				event.plane = pApp->FindPlane(event.x, event.y, pWindow);
				if (event.plane)
					{
					game->HandleEvent(event);
					}
				}
			*/
			break;
		
		case MM_JOY1BUTTONUP:
		case MM_JOY2BUTTONUP:
			/*
			event.type = (EVENT_TYPE)0;
			if (wParam & JOY_BUTTON1CHG)
				{
				DebugString("JOYSTICK_BUTTON_1_UP\n");
				event.type = JOYSTICK_BUTTON_1_UP;
				}
			if (wParam & JOY_BUTTON2CHG)
				{
				DebugString("JOYSTICK_BUTTON_2_UP\n");
				event.type = JOYSTICK_BUTTON_2_UP;
				}
			if (wParam & JOY_BUTTON3CHG)
				{
				DebugString("JOYSTICK_BUTTON_3_UP\n");
				event.type = JOYSTICK_BUTTON_3_UP;
				}
			if (wParam & JOY_BUTTON4CHG)
				{
				DebugString("JOYSTICK_BUTTON_4_UP\n");
				event.type = JOYSTICK_BUTTON_4_UP;
				}
			if (event.type)
				{
				if (message == MM_JOY1BUTTONUP)
					event.modifiers = JOYSTICK_1;
				else
					event.modifiers = JOYSTICK_2;
				event.claimed = FALSE;
				event.x = 0;
				event.y = 0;
				event.c = 0;
				event.plane = pApp->FindPlane(event.x, event.y, pWindow);
				if (event.plane)
					{
					game->HandleEvent(event);
					}
				}
			*/
			break;
		
		case MM_JOY1MOVE:
		case MM_JOY2MOVE:
			/*
			if (message == MM_JOY1MOVE)
				{
				i = 0;
				event.modifiers = JOYSTICK_1;
				}
			else
				{
				i = 1;
				event.modifiers = JOYSTICK_2;
				}
			event.claimed = FALSE;
			event.x = 0;
			event.y = 0;
			event.c = 0;
			event.plane = pApp->FindPlane(event.x, event.y, pWindow);
			pWindow->newJoys[i].wXpos = LOWORD(lParam);
			pWindow->newJoys[i].wYpos = HIWORD(lParam);

			// Send GAME_PAD_* message.
			if ((pWindow->newJoys[i].wYpos < 16384) && (pWindow->oldJoys[i].wYpos > 16384))
				{
				DebugString("GAMEPAD_NORTH_DOWN\n");
				event.type = GAMEPAD_NORTH_DOWN;
				game->HandleEvent(event);
				}
			if ((pWindow->newJoys[i].wYpos > 16384) && (pWindow->oldJoys[i].wYpos < 16384))
				{
				DebugString("GAMEPAD_NORTH_UP\n");
				event.type = GAMEPAD_NORTH_UP;
				game->HandleEvent(event);
				}
			if ((pWindow->newJoys[i].wYpos > 49152) && (pWindow->oldJoys[i].wYpos < 49152))
				{
				DebugString("GAMEPAD_SOUTH_DOWN\n");
				event.type = GAMEPAD_SOUTH_DOWN;
				game->HandleEvent(event);
				}
			if ((pWindow->newJoys[i].wYpos < 49152) && (pWindow->oldJoys[i].wYpos > 49152))
				{
				DebugString("GAMEPAD_SOUTH_UP\n");
				event.type = GAMEPAD_SOUTH_UP;
				game->HandleEvent(event);
				}
			if ((pWindow->newJoys[i].wXpos < 16384) && (pWindow->oldJoys[i].wXpos > 16384))
				{
				DebugString("GAMEPAD_WEST_DOWN\n");
				event.type = GAMEPAD_WEST_DOWN;
				game->HandleEvent(event);
				}
			if ((pWindow->newJoys[i].wXpos > 16384) && (pWindow->oldJoys[i].wXpos < 16384))
				{
				DebugString("GAMEPAD_WEST_UP\n");
				event.type = GAMEPAD_WEST_UP;
				game->HandleEvent(event);
				}
			if ((pWindow->newJoys[i].wXpos > 49152) && (pWindow->oldJoys[i].wXpos < 49152))
				{
				DebugString("GAMEPAD_EAST_DOWN\n");
				event.type = GAMEPAD_EAST_DOWN;
				game->HandleEvent(event);
				}
			if ((pWindow->newJoys[i].wXpos < 49152) && (pWindow->oldJoys[i].wXpos > 49152))
				{
				DebugString("GAMEPAD_EAST_UP\n");
				event.type = GAMEPAD_EAST_UP;
				game->HandleEvent(event);
				}

			// Send JOYSTICK_MOVE message.
			if ((pWindow->newJoys[i].wYpos != pWindow->oldJoys[i].wYpos) ||
				(pWindow->newJoys[i].wXpos != pWindow->oldJoys[i].wXpos))
				{
//				DebugString("JOYSTICK_MOVE\n");
				event.type = JOYSTICK_MOVE;
				event.x = pWindow->newJoys[i].wXpos << 16;
				event.y = pWindow->newJoys[i].wYpos << 16;
				game->HandleEvent(event);
				}

			pWindow->oldJoys[i].wXpos = pWindow->newJoys[i].wXpos;
			pWindow->oldJoys[i].wYpos = pWindow->newJoys[i].wYpos;
			*/
			break;
		
		case WM_COPYDATA: // message sent from WONplay DLL
			pcds = reinterpret_cast<PCOPYDATASTRUCT>(lParam);
			
			if (!won)
				{ assert(FALSE); break; }
			
			switch (pcds->dwData) {
				case WM_WON_ROOM_FILE_AVAILABLE:
					// const int nChars = nWON_PATH;
					// TCHAR sRoomFile[nChars];
					// WONERR err = WONMsgGetRoomFileInfo(pcds, sRoomFile, &nChars);
					break;
				case WM_WON_AD_FILE_AVAILABLE:
					wonErr = won->MsgGetAdInfo(pcds, &wonAdInfo);
					if (WONSUCCESS(wonErr))
						if (roomClient)
							roomClient->SetCurrentAd(&wonAdInfo);
					break;
				case WM_WON_SHUTTING_DOWN:
					game->QuitGame(TRUE);
					break;
			}
			break;
		
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
    
	return NULL;
}

void Window::SetNormalCursor(HCURSOR hNewNormal)
	{
	if (hNewNormal)
		hNormalCursor = hNewNormal;
	else
		hNormalCursor = hOldNormal;
	::SetCursor(pApp->handsOff ? hWaitCursor : hNormalCursor);
	}

void Window::SetSize(int width, int height)
	{
	RECT rect;

	if (hWnd)
		{
		GetWindowRect(hWnd, &rect);
		MoveWindow(hWnd, rect.left, rect.top, width, height, TRUE);
		}

	VWindow::SetSize(width, height);
	}

void Window::SetTitle(char *newTitle)
	{
	VWindow::SetTitle(newTitle);
	SetWindowText(hWnd, title);
	}

void Window::Move(int x, int y)
	{
	RECT rect;

	GetWindowRect(hWnd, &rect);
	MoveWindow(hWnd, x, y, rect.right - rect.left, rect.bottom - rect.top, TRUE);
	}

void Window::CopyToScreen(RECT &rcSource)
	{
	RECT rcDest;
	RECT rcClipSource, rcClipDest;
	int i;
	BYTE *pSourcePixels;

	if (hDC == NULL)
		return;

	// Offset rcDest to match rcInner.
	rcDest = rcSource;
	SetRect(&rcClipSource, 0, 0, frameBuffer->GetWidth(), frameBuffer->GetHeight());
	SetRect(&rcClipDest, 0, 0, Width(), Height());
	OffsetRect(&rcDest, rcInner.left, rcInner.top);
	OffsetRect(&rcClipDest, rcInner.left, rcInner.top);
	ClipSomeRects(rcDest, rcClipDest, rcSource, rcClipSource);

	if (rcSource.bottom == rcSource.top)
		return;

	if (frameBuffer->GetStride() < 0)
		{
		pSourcePixels = frameBuffer->GetRow(frameBuffer->GetHeight() - 1);
		pSourcePixels += (frameBuffer->GetHeight() - rcSource.bottom) * -frameBuffer->GetStride();
		}
	else
		{
		pSourcePixels = frameBuffer->GetRow(0);
		}

	btmpResGlobalHeader.biHeight = frameBuffer->GetHeight();
	btmpResGlobalHeader.biWidth = frameBuffer->GetWidth();
	btmpResGlobalHeader.biBitCount = frameBuffer->GetBytesPerPixel() * 8;

	// Win32s does a crummy 16/32bit to 8bit conversion.
	// So does NT 4.
	// Do it manually.
	if ((btColorBitSupport == 8) || win32s)
		{
		// Convert the 16/32bit bitmap to 8bit, then blit.
		BYTE *lowColorPixels;
		BYTE *tmpLowColorPixels;
		BYTE *highColorPixels;
		int w, h, wb;
		int x, y;

		w = rcDest.right - rcDest.left;
		h = rcDest.bottom - rcDest.top;
		wb = w + 3 - ((w-1) % 4);
		lowColorPixels = new BYTE[wb * h];
		for (y=0; y < h; ++y)
			{
			highColorPixels = frameBuffer->GetRow(rcSource.top + y) + (rcSource.left*sizeof(TEX_PIXEL));
			if (frameBuffer->GetStride() < 0)
				{
				tmpLowColorPixels = lowColorPixels + ((h-y-1)*wb);
				}
			else
				{
				tmpLowColorPixels = lowColorPixels + (y*wb);
				}
			for (x=0; x<w; ++x)
				{
				*tmpLowColorPixels = 
					InverseColorLookup(*(highColorPixels+2), *(highColorPixels+1), *(highColorPixels));
				highColorPixels += sizeof(TEX_PIXEL);
				++tmpLowColorPixels;
				}
			}
		btmpResGlobalHeader.biHeight = h;
		btmpResGlobalHeader.biWidth = w;
		btmpResGlobalHeader.biBitCount = 8;
		for (i=0; i<256; ++i)
			{
			btmpResGlobalHeader.bmiColors[i] = pCurrentPalette->colors[i];
			}
		i = SetDIBitsToDevice(hDC, rcDest.left, rcDest.top,
			w, h,
			0, 0,
			0, h,
			lowColorPixels,
			LPBITMAPINFO(PBYTE(&btmpResGlobalHeader)+16), DIB_RGB_COLORS);

		delete[] lowColorPixels;
		}
	else
		{
		i = SetDIBitsToDevice(hDC, rcDest.left, rcDest.top,
			rcDest.right - rcDest.left, (rcDest.bottom - rcDest.top),
			rcSource.left, 0, 0, frameBuffer->GetHeight(), pSourcePixels,
			LPBITMAPINFO(PBYTE(&btmpResGlobalHeader)+16), DIB_RGB_COLORS);
		}

	if (i == 0)
		{
		i = GetLastError();
		DebugString("Error returned by SetDIBitsToDevice.\n");
		}
	}

void Window::GoFullScreen()
	{
	if (!InFullScreen())
		{
		DestroyWindow();
		CreateFullscreenWindow();
		}
	}

void Window::GoWindowed(Boolean changeINIFile)
	{
	if (InFullScreen())
		{
		DestroyWindow();
		CreateWindowedWindow(changeINIFile);
		}
	}

BOOL Window::InFullScreen()
	{
	return inFullScreen;
	}

void Window::ShowTitleInMenuBar()
{
#if 0
	char *p, title[256], s[256];
	int menuHeight, oldBackMode;
	long style;
	UINT oldTextAlignMode;
	DWORD menuTextColor;
	COLORREF oldTextColor;
	HDC hDC;
	HFONT hFont, oldFont;
	RECT r;
	LOGFONT logFont;
	
	style = GetWindowLong(hWnd, GWL_STYLE);
	if ((style & WS_CAPTION) != 0)
		return;
	
	hDC = GetWindowDC(hWnd);
	if (!hDC)
		return;
	
	menuHeight = GetSystemMetrics(SM_CYMENU);
	menuTextColor = GetSysColor(COLOR_MENUTEXT);
	
	oldTextAlignMode = GetTextAlign(hDC);
	oldTextColor = GetTextColor(hDC);
	oldBackMode = GetBkMode(hDC);
	
	SetTextAlign(hDC, TA_RIGHT | TA_TOP);
	SetTextColor(hDC, menuTextColor);
	SetBkMode(hDC, TRANSPARENT);
	
	memset((char *)&logFont, 0, sizeof(LOGFONT));
	logFont.lfHeight = 10;
	logFont.lfWeight = FW_BOLD;
	logFont.lfCharSet = DEFAULT_CHARSET;
	strcpy(logFont.lfFaceName, "MS Sans Serif");
	
	hFont = CreateFontIndirect(&logFont);
	oldFont = (HFONT)SelectObject(hDC, hFont);
	
	GetWindowText(hWnd, title, 255);
	p = strchr(title, '-');
	if (p)
		sprintf(s, "Hoyle® %s", (char *)(p + 2));
	else
		strcpy(s, title);
	
	GetWindowRect(hWnd, &r);
	
	TextOut(hDC, r.right - 10, 4, s, strlen(s));
	
	SelectObject(hDC, oldFont);
	DeleteObject(hFont);
	
	SetTextAlign(hDC, oldTextAlignMode);
	SetTextColor(hDC, oldTextColor);
	SetBkMode(hDC, oldBackMode);
	
	ReleaseDC(hWnd, hDC);
#endif
}

void Window::CreateWindowedWindow(Boolean changeINIFile, int width, int height, Boolean closeBox)
	{
	int extraHeight, extraWidth;
	DWORD dwStyle;
	RECT rect;
	
	// Make the window a windowed window.
	if (GetSystemMetrics(SM_CXSCREEN) <= 640 && width >= 640)
		dwStyle = WS_POPUP;
	else {
		if (width != 640) {
			if (closeBox)
				dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_CLIPCHILDREN | WS_SYSMENU;
			else
				dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_CLIPCHILDREN;
		}
		else
			dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU |
				WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME | WS_CLIPCHILDREN;
	}
	SetRect(&rect, 0, 0, width, height);
	
	hWnd = CreateWindow(wc.lpszClassName, title, dwStyle,
		rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
		NULL, 0, hInstance, this);
	
	hDC = GetDC(hWnd);
	GetWindowRect(hWnd, &rect);
	GetClientRect(hWnd, &rcInner);
	GetClientRect(hWnd, &rcOuter);
	extraWidth = rect.right - rect.left - (rcInner.right - rcInner.left);
	extraHeight = rect.bottom - rect.top - (rcInner.bottom - rcInner.top);
	x = rcOuter.right - rcOuter.left;
	y = rcOuter.bottom - rcOuter.top;
	x = (x - width)/2;
	y = (y - height)/2;
	if (x < 0)
		x = 0;
	if (y < 0)
		y = 0;
	SetRect(&rcInner, x, y, x + width, y + height);
	
	CenterHWND(hWnd, extraWidth, extraHeight);
	
	inFullScreen = FALSE;
	
	/*
	if (pApp->pMainPlane == NULL)
		Show(); // let the caller move subsequent windows before showing them
	*/
	
	if (changeINIFile)
		pApp->pIniFile->WriteValue("UseDirectDraw", FALSE);
	
	urlWindow = CreateWindow("EDIT", "URL",
							 WS_CHILD | WS_DISABLED | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_LEFT | ES_NOHIDESEL,
							 0, 0, 300, 20, hWnd, (HMENU)0, hInstance, NULL);
	SetWindowText(urlWindow, "http://www.won.net/channels/hoyle/");
	// sosUpdateHWND(hWnd);
	}

void Window::CreateFullscreenWindow()
	{
	/*
	DWORD dwStyle;
	RECT rect;
	int screenWidth, screenHeight;

	// Make the window fill the screen.
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	dwStyle = WS_POPUP;
	SetRect(&rect, 0, 0, screenWidth, screenHeight);

	hWnd = CreateWindow(wc.lpszClassName, title, dwStyle,
		rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
		NULL, 0, hInstance, this);
	Show();

	if (hWnd && DDInit(hWnd) &&
		(DDSetMode(640, 480, 24) || DDSetMode(640, 480, 16) || DDSetMode(640, 480, 8)))
		{
		pApp->pIniFile->WriteValue("UseDirectDraw", TRUE);
		useDD = TRUE;
		inFullScreen = TRUE;
		hDC = GetDC(hWnd);
		}
	else
		{
		pApp->pIniFile->WriteValue("UseDirectDraw", FALSE);
		DDTerm(hWnd);
		inFullScreen = FALSE;
		useDD = FALSE;
		DestroyWindow();
		CreateWindowedWindow();
		}
	// sosUpdateHWND(hWnd);
	*/
	}

void Window::DestroyWindow()
	{
	HPALETTE hOldPalette;
	HMENU oldMenu;

	if (InFullScreen())
		{
		// DDTerm(hWnd);
		inFullScreen = FALSE;
		}

	if (hDC)
		{
		hOldPalette = SelectPalette(hDC, (HPALETTE)GetStockObject(DEFAULT_PALETTE), FALSE);
		DeleteObject(hOldPalette);

		oldMenu = GetMenu(hWnd);
		SetMenu(hWnd, NULL);
		if (oldMenu)
			DestroyMenu(oldMenu);

		ReleaseDC(hWnd, hDC);
		hDC = NULL;
		}

	::DestroyWindow(hWnd);
	}
