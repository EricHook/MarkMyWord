#include "os.h"
#include "core.h"
#include "sci.h"
#include <ddraw.h>

#include "Client Include.h"

int numJoys;
JOYINFO *joys = NULL;
JOYINFO *prevJoys = NULL;
BOOL *validJoys = NULL;
int joyPixels = 1;
int useJoystick = FALSE;

#define MAX_JOY_PIXELS 80
#define JOY_DIVIDER 8

extern BOOL changeDisplay;
extern BYTE btColorBitSupport;

void SetDisplay()
	{
	// Enum display settings.
	DEVMODE dm;
	DEVMODE bestDM;
	BOOL gotSetting;
	int i;
	BOOL foundBest;
	long err;
	HDC hDC;

	gotSetting = TRUE;
	i = 0;
	foundBest = FALSE;
	while (gotSetting)
		{
		gotSetting = EnumDisplaySettings(NULL, i, &dm);
		++i;
		if ((dm.dmBitsPerPel == 16) &&
			(dm.dmPelsWidth == 640) &&
			(dm.dmPelsHeight == 480))
			{
			bestDM = dm;
			foundBest = TRUE;
			}
		}
	if (foundBest)
		{
		bestDM.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;
		err = ChangeDisplaySettings(&bestDM, 0);
		bestDM.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
		err = ChangeDisplaySettings(&bestDM, 0);
		}

	switch (err)
		{
		case DISP_CHANGE_SUCCESSFUL:
			DebugString("DISP_CHANGE_SUCCESSFUL\n");
			break;
		case DISP_CHANGE_RESTART:
			DebugString("DISP_CHANGE_RESTART\n");
			break;
		case DISP_CHANGE_BADFLAGS:
			DebugString("DISP_CHANGE_BADFLAGS\n");
			break;
		case DISP_CHANGE_FAILED:
			DebugString("DISP_CHANGE_FAILED\n");
			break;
		case DISP_CHANGE_BADMODE:
			DebugString("DISP_CHANGE_BADMODE\n");
			break;
		}

	// Update the globals if the display was changed.
	hDC = GetDC(GetDesktopWindow());
	btColorBitSupport = GetDeviceCaps(hDC, BITSPIXEL);
	ReleaseDC(GetDesktopWindow(), hDC);
	}

void UnsetDisplay()
	{
	ChangeDisplaySettings(NULL, 0);
	}

extern void InitAlphaTable();

Application::Application()
	{
	int i;

	InitAlphaTable();
	HRESULT hResult = CoInitialize(NULL);

	if (changeDisplay)
		{
		SetDisplay();
		}

	// Setup joysticks.
	MMRESULT result;

	numJoys = joyGetNumDevs();
	joys = new JOYINFO[numJoys];
	prevJoys = new JOYINFO[numJoys];
	validJoys = new BOOL[numJoys];
	for (i = 0; i < numJoys; ++i)
		{
		result = joyGetPos(i, &joys[i]);
		result = joyGetPos(i, &prevJoys[i]);
		if (result == JOYERR_NOERROR)
			{
			validJoys[i] = TRUE;
			}
		else
			{
			validJoys[i] = FALSE;
			}
		}
	}

Application::~Application()
	{
	if (changeDisplay)
		{
		UnsetDisplay();
		}

	CoUninitialize();

	// Free joystick stuff
	delete[] joys;
	delete[] prevJoys;
	delete[] validJoys;
	}

DWORD oldMilli = 0;
DWORD newMilli = 0;

void Application::Idle()
	{
	MSG msg;
	static char funnyKeys[256] = "";
	// int i;
	// MMRESULT result;
	// POINT p;
	// static int oldX = 0;
	// static int oldY = 0;
	// char buff[64];
	
//	oldMilli = timeGetTime();
	VApplication::Idle();
	
	if (funnyKeys[0] == 0)
		GetString(221, funnyKeys);
	
	if (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
		{
		/*
		if (roomClient && msg.hwnd == roomClient->roomChatEditWindow)
			{
			if (msg.message == WM_CHAR && msg.wParam == 27)
				msg.hwnd = MAIN_HWND;
			// if (msg.message == WM_SYSKEYDOWN && msg.wParam == 13)
			// 	msg.hwnd = MAIN_HWND;
			}
		*/
		if (roomClient && msg.message == WM_SYSKEYDOWN && strchr(funnyKeys, (char)msg.wParam))
			roomClient->ChatSoundKeyPressed((char)msg.wParam);
		else
			{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			}
		}
	
	// Get the mouse position and update the cursor.
/*	GetCursorPos(&p);
	ScreenToClient(pWindow->hWnd, &p);
	if (((p.x != oldX) || (p.y != oldY)) &&
		(((p.x < 0) || (p.x > 639)) ||
		((p.y < 0) || (p.y > 479))))

		{
		msg.hwnd = pWindow->hWnd;
		msg.message = WM_MOUSEMOVE;
		msg.wParam = 0;
		msg.lParam = MAKELONG(p.x, p.y);
		DispatchMessage(&msg);
		}
*/

	/*
	// Check for joystick movement.
	sciEvent event;

	if (useJoystick)
		{
		for (i = 0; i < 1; ++i)
			{
			result = joyGetPos(i, &joys[i]);
			if (result == JOYERR_NOERROR)
				{
				POINT p, localPoint;
				HWND hwnd;

				GetCursorPos(&p);
				hwnd = WindowFromPoint(p);
				localPoint = p;
				ScreenToClient(hwnd, &localPoint);

				if ((joys[i].wButtons & JOY_BUTTON1) && !(prevJoys[i].wButtons & JOY_BUTTON1))
					{
					SendMessage(hwnd, WM_LBUTTONDOWN, 0, (localPoint.y << 16) + localPoint.x);
					prevJoys[i] = joys[i];
					}
				else if (!(joys[i].wButtons & JOY_BUTTON1) && (prevJoys[i].wButtons & JOY_BUTTON1))
					{
					SendMessage(hwnd, WM_LBUTTONUP, 0, (localPoint.y << 16) + localPoint.x);
					prevJoys[i] = joys[i];
					}
				if ((joys[i].wButtons & JOY_BUTTON2) && !(prevJoys[i].wButtons & JOY_BUTTON2))
					{
					SendMessage(hwnd, WM_RBUTTONDOWN, 0, (localPoint.y << 16) + localPoint.x);
					prevJoys[i] = joys[i];
					}
				else if (!(joys[i].wButtons & JOY_BUTTON2) && (prevJoys[i].wButtons & JOY_BUTTON2))
					{
					SendMessage(hwnd, WM_RBUTTONUP, 0, (localPoint.y << 16) + localPoint.x);
					prevJoys[i] = joys[i];
					}
				if ((joys[i].wYpos > 16384) && (joys[i].wYpos < 49152) && 
					(joys[i].wXpos > 16384) && (joys[i].wXpos < 49152))
					{
					joyPixels = 1;
					}
				else
					{
					++joyPixels;
					if (joyPixels > MAX_JOY_PIXELS)
						joyPixels = MAX_JOY_PIXELS;
					}
				if (joys[i].wXpos > 49152)
					{
					SetCursorPos(p.x + (joyPixels / JOY_DIVIDER), p.y);
					GetCursorPos(&p);
					}
				if (joys[i].wXpos < 16384)
					{
					SetCursorPos(p.x - (joyPixels / JOY_DIVIDER), p.y);
					GetCursorPos(&p);
					}
				if (joys[i].wYpos > 49152)
					{
					SetCursorPos(p.x, p.y + (joyPixels / JOY_DIVIDER));
					GetCursorPos(&p);
					}
				if (joys[i].wYpos < 16384)
					{
					SetCursorPos(p.x, p.y - (joyPixels / JOY_DIVIDER));
					GetCursorPos(&p);
					}
				prevJoys[i] = joys[i];
				}
			}
		}
	*/
	}

void Application::WaitTillNextTick()
	{
	int timeToSleep;
	//char buff[64];

	newMilli = timeGetTime();
	timeToSleep = 10 - (newMilli - oldMilli);
	if (timeToSleep < 2)
		{
		timeToSleep = 0;
		//DebugString("Not Sleeping.\n");
		}
	if (timeToSleep > 0)
		{
		//sprintf(buff, "Sleeping for %d milliseconds.\n", timeToSleep);
		//DebugString(buff);
		Sleep(timeToSleep);
		}
	}

void Application::HandsOn()
	{
	handsOff = FALSE;
	MAIN_WINDOW->SetNormalCursor(MAIN_WINDOW->hNormalCursor);
#ifdef _DEBUG
	// DebugString("HandsOn()\n");
#endif
	/*
	SetClassLong(pWindow->hWnd, GCL_HCURSOR,
		(LONG) pWindow->hNormalCursor);
	*/
	}

void Application::HandsOff()
	{
	handsOff = TRUE;
	MAIN_WINDOW->SetNormalCursor(MAIN_WINDOW->hNormalCursor);
#ifdef _DEBUG
	// DebugString("HandsOff()\n");
#endif
	/*
	SetClassLong(pWindow->hWnd, GCL_HCURSOR,
		(LONG) pWindow->hWaitCursor);
	*/
	}


int Application::NumJoysticks()
{
	int joyCount = 0;
	for (int i = 0; i < numJoys; ++i)
	{
		if 	(validJoys[i] == TRUE)
		{
			++joyCount;
		}
	}
	return joyCount;
}