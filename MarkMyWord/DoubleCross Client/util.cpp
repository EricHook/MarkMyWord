#include "os.h"
#include "rc.h"
#include "..\all room clients\rc.h"

#include "globals.h"

#ifdef OS_WINDOWS
#include <time.h>
#include <afxres.h>
#endif // OS_WINDOWS

#include "app.h"
#include "timer.h"
#include "game.h"
#include "inifile.h"
#include "random.h"
#include "window.h"
#include "rooms.h"

extern BOOL win32s;
extern BOOL useDD;
extern BOOL hdVersion;

unsigned int random(unsigned int limit)
	{
	if (limit)
		return pApp->randomGenerator->GetRandom(limit);
	else
		return 0;
	}

int random(int limit)
	{
	return random((unsigned int) limit);
	}

int random(int lowerBound, int upperBound)
	{
	unsigned int limit;

	limit = (unsigned int)(upperBound - lowerBound);
	return lowerBound + random(limit + 1); //tw - added the +1 as the upperbound was never a possibility
	}

static void BlandDebugString(LPCTSTR lpOutputString) // pointer to string to be displayed
{
#ifdef _DEBUG
	static Boolean gotFlag = FALSE, writeToFile = FALSE;
	static FILE *f = NULL;
	
	OutputDebugString(lpOutputString);
	
	if (!gotFlag && pApp && pApp->pIniFile) {
		writeToFile = pApp->pIniFile->ReadValue("DebugStringToFile", 0);
		gotFlag = TRUE;
	}
	
	if (writeToFile) {
		OutputDebugString(lpOutputString);
		if (!f)
			f = fopen("DEBUGLOG.TXT", "w+");
		if (f) {
			fputs(lpOutputString, f);
			fflush(f);
		}
		// f will be closed when the application quits or crashes
	}
#endif // _DEBUG
}

#if defined(OS_MAC)
extern int currentMenu;
#endif

void SetMainMenu(WORD menu)
{
}

Boolean URectInRect(RECT *r1, RECT *r2)
{
	RECT dest;
	
	return IntersectRect(&dest, r1, r2);
}

int IntersectArea(RECT *r1, RECT *r2)
{
	int interSect;
	RECT dest;

	interSect = IntersectRect(&dest, r1, r2);

	if(interSect)
		return int((dest.right - dest.left) * (dest.bottom - dest.top));

	return 0;
}

Boolean ControlKeyDown()
{
	return (GetAsyncKeyState(VK_CONTROL) & 0x8000) ? TRUE : FALSE;
}

Boolean ShiftKeyDown()
{
	return (GetAsyncKeyState(VK_SHIFT) & 0x8000) ? TRUE : FALSE;
}

Boolean EscapeKeyDown()
{
	return (GetAsyncKeyState(VK_ESCAPE)) ? TRUE : FALSE;
}

char *MoneyToString(int n, char *s)
{
	if (n > 999999999)
		sprintf(s, "$%d,%.3d,%.3d,%.3d", n / 1000000000, (n % 1000000000) / 1000000, (n % 1000000) / 1000, n % 1000);
	else {
		if (n > 999999)
			sprintf(s, "$%d,%.3d,%.3d", n / 1000000, (n % 1000000) / 1000, n % 1000);
		else {
			if (n > 999)
				sprintf(s, "$%d,%.3d", n / 1000, n % 1000);
			else
				sprintf(s, "$%d", n);
		}
	}
	
	return s;
}

void DebugString(char *lpszFormat,...) 
	{
#if defined(OS_WINDOWS)
    va_list args;
    va_start(args, lpszFormat);

    int nBuf;
    char szBuffer[512];

    nBuf = vsprintf(szBuffer, lpszFormat, args);
    assert(nBuf < 512);

    BlandDebugString(szBuffer);

    va_end(args);
#else
    BlandDebugString(lpszFormat);
#endif
	}

void ReportError(const char *fmt, ...)
{
	char s[1024];
	va_list argptr;
	
	va_start(argptr, fmt);
	vsprintf(s, fmt, argptr);
	
	MessageBox(NULL, s, "Room Server Error", MB_OK);
	
	va_end(argptr);
}

static char *gsdPrompt;
static char *gsdResult;
static int gsdMaxLength;

BOOL CALLBACK GSDCallback(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
		case WM_INITDIALOG:
			SetDlgItemText(hDlg, IDC_PROMPT, gsdPrompt);
			return TRUE;
		
		case WM_COMMAND:
			if (HIWORD(wParam) == BN_CLICKED) {
				GetDlgItemText(hDlg, IDC_TEXT, gsdResult, gsdMaxLength);
				EndDialog(hDlg, 0);
			}
			return TRUE;
		
		case WM_CLOSE:
			DestroyWindow(hDlg);
			return TRUE;
	}
	
	return FALSE;
}

void GetStringDialog(char *prompt, char *result, int maxLength)
{
	gsdPrompt = prompt;
	gsdResult = result;
	gsdMaxLength = maxLength;
	
	DialogBox(hResourceInstance, MAKEINTRESOURCE(IDD_GETSTRING), MAIN_HWND, GSDCallback);
}

/*
char *strnzcpy(char *s, char *t, int n)
{
	strncpy(s, t, n);
	s[n] = 0;
	
	return s;
}
*/

bool strizzmember(char *s, char *member)
{
	for ( ; s[0] ; ) {
		if (!stricmp(s, member))
			return true;
		for ( ; s[0] ; s++) ;
		s++;
	}
	
	return false;
}

Boolean IHaveAHoyleCDProductInstalled(char *s1, char *s2, char *s3)
{
	Boolean found = FALSE;
	char title[256];
	long result;
	DWORD i, size;
	HKEY key;
	FILETIME time;
	
	result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Sierra On-Line", 0, KEY_READ, &key);
	
	if (result != ERROR_SUCCESS)
		return FALSE;
	
	for (i = 0 ; ; i++) {
		size = 255;
		result = RegEnumKeyEx(key, i, title, &size, NULL, NULL, 0, &time);
		if (result != ERROR_SUCCESS)
			break;
		if (strstr(title, "Internet") || strstr(title, "internet"))
			continue;
		if (strstr(title, "Demo") || strstr(title, "demo"))
			continue;
		if ((s1 && strstr(title, s1)) ||
			(s2 && strstr(title, s2)) ||
			(s3 && strstr(title, s3)))
			{ found = TRUE; break; }
		
		// new deal: any non-internet, non-demo Hoyle product is OK
		
		if (strstr(title, "Hoyle"))
			{ found = TRUE; break; }
	}
	
	RegCloseKey(key);
	
	return found;
}

void CenterHWND(HWND w, int dX, int dY)
{
	int width, height, screenHeight, screenWidth;
	RECT rcWorkArea, r;
	
	GetWindowRect(w, &r);
	width = r.right - r.left;
	height = r.bottom - r.top;
	
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);
	
	if (!SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWorkArea, 0)) {
		rcWorkArea.top = 0;
		rcWorkArea.left = 0;
		rcWorkArea.bottom = screenHeight;
		rcWorkArea.right = screenWidth;
	}
	
	r.left = rcWorkArea.left + ((rcWorkArea.right - rcWorkArea.left - (width + dX)) / 2);
	if (r.left < rcWorkArea.left)
		r.left = rcWorkArea.left;
	r.top = rcWorkArea.top + ((rcWorkArea.bottom - rcWorkArea.top - (height + dY)) / 2);
	if (r.top < rcWorkArea.top)
		r.top = rcWorkArea.top;
	r.right = r.left + width + dX;
	r.bottom = r.top + height + dY;
	
	MoveWindow(w, r.left, r.top, r.right - r.left, r.bottom - r.top, FALSE);
}

char *GetString(int id, char *s, int maxLength /* = 256 */)
{
	int bytes;
	
	s[0] = 0;
	
	if (!hResourceInstance)
		{ assert(FALSE); return s; }
	
	bytes = LoadString(hResourceInstance, id, s, maxLength - 1);
	
	return s;
}

char *GetGameString(int id, char *s, int maxLength /* = 256 */)
{
	int bytes;
	
	s[0] = 0;
	
	if (!hGameResourceInstance)
		{ assert(FALSE); return s; }
	
	bytes = LoadString(hGameResourceInstance, id, s, maxLength - 1);
	
	return s;
}
