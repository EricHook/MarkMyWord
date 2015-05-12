#include <memory.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#include "os.h"
#include "globals.h"
#include "resman.h"
#include "rooms.h"
#include "window.h"

#include "Client Include.h"

// Exports

WONplayDLL::WON *won;
WONParams wonParams;

Application *pApp = NULL;
Game *game = NULL;
int numBytesAllocated = 0;
char exeDir[MAX_PATH];
char exeFilename[MAX_PATH];
HINSTANCE hInstance = NULL;
HINSTANCE hResourceInstance = NULL;
HINSTANCE hGameResourceInstance = NULL;
BYTE btSoundBitSupport =0;
BYTE btColorBitSupport =0;
char cdDriveLetter = 0;
BOOL win32s = FALSE;
BOOL hdVersion = FALSE;
BOOL demoVersion = FALSE;
char helpFilename[MAX_PATH];
BOOL lowMemory = FALSE;
static DWORD dwRAM = 0;
BOOL runFullSpeed = FALSE;
BOOL showFrameRate = FALSE;
BOOL showMousePosition = FALSE;
BOOL skipPaletteChanges = FALSE;
RGBQUAD DEFAULT_GREY = {221, 221, 221, 0};
RGBQUAD DEFAULT_LTGREY = {251, 251, 251, 0};
RGBQUAD DEFAULT_DKGREY = {161, 161, 161, 0};
BOOL confirmQuit = TRUE;

#define MAX_RESOURCE_DIRECTORIES	16

int numResourceDirectories = 0;
char resourceDirectories[MAX_RESOURCE_DIRECTORIES][MAX_PATH];

#define MAX_RESOURCE_MAPPINGS		1000

int numResourceMappings = 0;
ResourceMapping resourceMappings[MAX_RESOURCE_MAPPINGS];

int maxMemory = 32 * 1024 * 1024;

char szAppName[256] = CLIENT_APP_NAME;
char szAppTitle[256] = CLIENT_APP_NAME;
char szIniFilename[] = "settings.ini";

int Main();

enum {
	ERROR_NONE,
	ERROR_WARNING,
	ERROR_SERIOUS,
	ERROR_FATAL
	};

BOOL PreFlight() {
	HDC hDC;
	char szPrompt[255];
	char szTitle[255];
	i18nStr getS, getS2;
	WAVEOUTCAPS waveDevCaps;
	int iErrorLevel = ERROR_NONE;
	OSVERSIONINFO osvi;
	// FILE *file;
	
	/*
	strcpy(helpFilename, exeDir);
	strcat(helpFilename, shortHelpFilename);
	file = fopen(helpFilename, "rb");
	if (file)
		{
		// Found help file.
		fclose(file);
		}
	else
		{
		// Can't find help file.
		strcpy(helpFilename, shortHelpFilename);
		}
	*/

 	// check the Windows version.
	osvi.dwOSVersionInfoSize = sizeof(osvi);
	GetVersionEx(&osvi);
	if (osvi.dwPlatformId == VER_PLATFORM_WIN32s)
		win32s = TRUE;
	else
		win32s = FALSE;
	
	// Check for sound boards.
	waveOutGetDevCaps(0, &waveDevCaps, sizeof(waveDevCaps));
	if (waveDevCaps.dwFormats & WAVE_FORMAT_2M16) {
		btSoundBitSupport = 16;
		}
	else if (waveDevCaps.dwFormats & WAVE_FORMAT_2M08) {
		btSoundBitSupport = 8;
		}
	else
		btSoundBitSupport = 0;
	if (waveOutGetNumDevs()==0) {
		// No sound support.
		//wsprintf(szPrompt, "%s requires an installed sound driver.", szAppName);
		//iErrorLevel = ERROR_SERIOUS;
		}		
	else if (waveDevCaps.dwSupport & WAVECAPS_SYNC) {
		wsprintf(szPrompt, GetString(228, getS));
		iErrorLevel = ERROR_SERIOUS;
		}
	
	// Check for monitor support.
	hDC = GetDC(GetDesktopWindow());
	btColorBitSupport = GetDeviceCaps(hDC, BITSPIXEL);
	if ((GetDeviceCaps(hDC, HORZRES)<640) || (GetDeviceCaps(hDC, VERTRES)<480)) {
		wsprintf(szPrompt, GetString(229, getS), (char *)szAppName);
		iErrorLevel = ERROR_SERIOUS;
		}
	
	if ((GetDeviceCaps(hDC, BITSPIXEL) != 8) && win32s)
		{
		wsprintf(szPrompt, GetString(230, getS), (char *)szAppName);
		iErrorLevel = ERROR_FATAL;
		}
	ReleaseDC(GetDesktopWindow(), hDC);
	
	// Check for memory.
	MEMORYSTATUS memoryStatus;
	
	memoryStatus.dwLength = sizeof(memoryStatus);
	GlobalMemoryStatus(&memoryStatus);
	dwRAM = memoryStatus.dwTotalPhys;
	
	// Reserve 4MB ram for the OS, but take at least 8MB.
	maxMemory = dwRAM - 4*1024*1024;
	if (maxMemory < 8*1024*1024)
		maxMemory = 8*1024*1024;
	if (dwRAM < 15*1024*1024)
		{
		lowMemory = TRUE;
		//wsprintf(szPrompt, "%s may need more memory than you have free.  Consider closing some other application.", szAppName);
		//iErrorLevel = ERROR_SERIOUS;
		}
	else
		{
		lowMemory = FALSE;
		}
	
	switch (iErrorLevel)
		{
		case ERROR_NONE:
			break;
		case ERROR_WARNING:
			wsprintf(szTitle, GetString(231, getS), (char *)szAppName);
			MessageBoxA(NULL, szPrompt, szTitle, MB_OK | MB_ICONINFORMATION | MB_TASKMODAL);
			break;
		case ERROR_SERIOUS:
			wsprintf(szTitle, GetString(232, getS), (char *)szAppName, GetString(233, getS2));
			strcat(szPrompt, GetString(234, getS));
			strcat(szPrompt, (char *)szAppName);
			strcat(szPrompt, GetString(235, getS));
			if (IDYES == MessageBoxA(NULL, szPrompt, szTitle, MB_YESNO | MB_ICONQUESTION | MB_TASKMODAL))
				return TRUE;
			else
				return FALSE;
		case ERROR_FATAL:
			/*
			wsprintf(szTitle, "%s - %s ERROR", szAppName, "FATAL");
			strcat(szPrompt, "  Fatal errors prevent ");
			strcat(szPrompt, szAppName);
			strcat(szPrompt, " from running at all.");
			MessageBoxA(NULL, szPrompt, szTitle, MB_OK | MB_ICONSTOP | MB_TASKMODAL);
			*/
			int result;
			
			result = MessageBoxA(NULL, szPrompt, (char *)szAppName, MB_OK | MB_ICONSTOP | MB_TASKMODAL);
			
			/*
			if (result == IDOK)
				return PreFlight();
			*/
			
			return FALSE;
		}
	
	return TRUE;
	}

//======================================================================
// TODO - this could be a call to a RoomClient factory if/when we
// TODO - choose to implement that particular feature
//======================================================================
bool InitClientObject()
{
	assert(!roomClient);

	roomClient = new ROOMCLIENT();
	if (!roomClient)
	{
		TRACE(_ROUTINE_ "Failed to create Client object.\n");
		return false;
	}

	if (roomClient->Init())
	{
		TRACE(_ROUTINE_ "Client Init failed.\n");

		delete roomClient;
		roomClient = NULL;
		return false;
	}

	return true;
}

int AddDirectory(char *path)
{
	i18nStr getS;
	int i;
	
#ifdef _DEBUG
	// char m[256];
	// sprintf(m, "AddDirectory(path = %s)", path);
	// MessageBoxA(NULL, m, szAppName, MB_OK | MB_ICONSTOP | MB_TASKMODAL);
#endif
	
	for (i = 0 ; i < numResourceDirectories ; i++)
		if (!stricmp(path, resourceDirectories[i]))
			return i;
	
	if (numResourceDirectories == MAX_RESOURCE_DIRECTORIES) {
		MessageBoxA(NULL, GetString(236, getS), (char *)szAppName, MB_OK | MB_ICONSTOP | MB_TASKMODAL);
		
		return -1;
	}
	
	strcpy(resourceDirectories[numResourceDirectories], path);
	numResourceDirectories++;
	
	return numResourceDirectories - 1;
}

int AddResourceMapping(char *type, int resourceID, int directory, int resourceIDToUse)
{
	i18nStr getS;
	int i;
	
#ifdef _DEBUG
	// char m[256];
	// sprintf(m, "AddDirectory(type = %s, resourceID = %d, directory = %d)", type, resourceID, directory);
	// MessageBoxA(NULL, m, szAppName, MB_OK | MB_ICONSTOP | MB_TASKMODAL);
#endif
	
	// override an existing resouce with the same ID, if one is present
	
	for (i = 0 ; i < numResourceMappings ; i++)
		if (resourceID == resourceMappings[i].resourceID && !stricmp(type, resourceMappings[i].type)) {
			resourceMappings[i].directory = directory;
			resourceMappings[i].resourceIDToUse = resourceIDToUse;
			
			return i;
		}
	
	if (numResourceMappings == MAX_RESOURCE_MAPPINGS) {
		MessageBoxA(NULL, GetString(237, getS), (char *)szAppName, MB_OK | MB_ICONSTOP | MB_TASKMODAL);
		
		return -1;
	}
	
	strcpy(resourceMappings[numResourceMappings].type, type);
	resourceMappings[numResourceMappings].resourceID = resourceID;
	resourceMappings[numResourceMappings].directory = directory;
	resourceMappings[numResourceMappings].resourceIDToUse = resourceIDToUse;
	numResourceMappings++;
	
	return numResourceMappings - 1;
}

int ParseResourcePaths(char *iniFilePath)
{
	char key[256], s[256], p[MAX_PATH], getS[256], types[4][4] = { "PRF", "PAL", "TEX", "WAV" };
	int i, n, type, resID, localDir, resIDToUse, localDirs[16], numLocalDirs = 0;
	
	// process included files
	
#ifdef _DEBUG
	// char m[256];
	// sprintf(m, "ParseResourcePaths(iniFilePath = %s)", iniFilePath);
	// MessageBoxA(NULL, m, szAppName, MB_OK | MB_ICONSTOP | MB_TASKMODAL);
#endif
	
	for (i = 0 ; ; i++) {
		itoa(i, key, 10);
		GetPrivateProfileString("INCLUDE", key, "", s, 255, iniFilePath);
		if (s[0]) {
#ifdef _DEBUG
			// sprintf(m, "found include file: %s", s);
			// MessageBoxA(NULL, m, szAppName, MB_OK | MB_ICONSTOP | MB_TASKMODAL);
#endif
			sprintf(p, "%s%s", wonParams.wonPath, s);
			if (ParseResourcePaths(p) != 0)
				return -1;
		}
		else
			break;
	}
	
	// process directories
	
	for (i = 0 ; ; i++) {
		itoa(i, key, 10);
		GetPrivateProfileString("DIRECTORY", key, "", s, 255, iniFilePath);
		if (s[0]) {
#ifdef _DEBUG
			// sprintf(m, "found directory: %s", s);
			// MessageBoxA(NULL, m, szAppName, MB_OK | MB_ICONSTOP | MB_TASKMODAL);
#endif
			localDirs[numLocalDirs] = AddDirectory(s);
			if (localDirs[numLocalDirs] == -1)
				return -1;
			numLocalDirs++;
		}
		else
			break;
	}
#ifdef _DEBUG
	// sprintf(m, "found %d directories in %s", i, iniFilePath);
	// MessageBoxA(NULL, m, szAppName, MB_OK | MB_ICONSTOP | MB_TASKMODAL);
#endif
	
	// process files of known types
	
	for (type = 0 ; type < 4 ; type++) {
		for (i = 0 ; ; i++) {
			itoa(i, key, 10);
			GetPrivateProfileString(types[type], key, "", s, 255, iniFilePath);
			if (s[0]) {
#ifdef _DEBUG
				// sprintf(m, "found file: %s", s);
				// MessageBoxA(NULL, m, szAppName, MB_OK | MB_ICONSTOP | MB_TASKMODAL);
#endif
				n = sscanf(s, "%d %d %d", &resID, &localDir, &resIDToUse);
				if (n != 3 || localDir < 0 || localDir >= numLocalDirs) {
					MessageBoxA(NULL, GetString(238, getS), (char *)szAppName, MB_OK | MB_ICONSTOP | MB_TASKMODAL);
					
					return -1;
				}
				n = AddResourceMapping(types[type], resID, localDirs[localDir], resIDToUse);
				if (n == -1)
					return -1;
			}
			else
				break;
		}
#ifdef _DEBUG
		// sprintf(m, "found %d files of type %s in %s", i, types[type], iniFilePath);
		// MessageBoxA(NULL, m, szAppName, MB_OK | MB_ICONSTOP | MB_TASKMODAL);
#endif
	}

	return 0;
}

//======================================================================
// Windows code entry-point
//======================================================================
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpszCmdParam, int )
	{
	char *p, szPrompt[64], path[MAX_PATH];
	int i, nIndex, successes = 0;
	
	// set up the global path variables
	
	nIndex = ::GetModuleFileName(NULL, path, MAX_PATH);
	if (nIndex > 0)
		{
		strcpy(exeFilename, path);
		while ((nIndex > 0) && (path[nIndex] != '\\'))
			--nIndex;
		path[nIndex] = '\\';
		path[nIndex+1] = '\0';
		}
	else
		{
		// Failure? How in the world can that happen?
		exeFilename[0] = 0;
		path[0] = 0;
		}
	strcpy(exeDir, path);
	
	// look for the string tables
	
	memset(&wonParams, 0, sizeof(WONParams));
	
	p = strstr(lpszCmdParam, "wonpath:\"");
	if (p != NULL)
		{
		p += strlen("wonpath:\"");
		for (i = 0 ; p[0] && p[0] != '"' && i < (MAX_PATH - 1) ; p++, i++)
			wonParams.wonPath[i] = (p[0] == '/') ? '\\' : p[0];
		if (i > 0 && wonParams.wonPath[i - 1] == '\\')
			i--; // take the final backslash off
		wonParams.wonPath[i] = 0;
		successes++;
		}
	
	if (successes == 0)
		{
		MessageBoxA(NULL, "Unable to locate string resources.  Visit www.won.net for help.", (char *)szAppName, MB_OK | MB_ICONSTOP | MB_TASKMODAL);
		
		return 0;
		}
	
	sprintf(path, "%s\\client.dll", wonParams.wonPath);
	hResourceInstance = LoadLibrary(path);
	if (!hResourceInstance)
		{
		MessageBoxA(NULL, "Unable to locate string resources.  Visit www.won.net for help.", (char *)szAppName, MB_OK | MB_ICONSTOP | MB_TASKMODAL);
		
		return 0;
		}
	
	strcpy(szPrompt, CLIENT_APP_NAME);
	p = strchr(szPrompt, ' ');
	if (p)
		p[0] = 0;
	sprintf(path, "%s%s.dll", exeDir, szPrompt);
	hGameResourceInstance = LoadLibrary(path);
	if (!hGameResourceInstance)
		{
			MessageBoxA(NULL, GetString(240, path), (char *)szAppName, MB_OK | MB_ICONSTOP | MB_TASKMODAL);
			
			return 0;
		}
	
	GetGameString(2, (char *)szAppName);
	GetGameString(2, (char *)szAppTitle);
	
	// check for multiple instances
	
	if (hPrevInst)
		{
		wsprintf(szPrompt, GetString(239, path), (char *)szAppName);
		MessageBoxA(NULL, szPrompt, (char *)szAppName, MB_OK | MB_ICONSTOP | MB_TASKMODAL);
		
		return 0;
		}
	
	// process the rest of the command line
	
	p = strstr(lpszCmdParam, "guid:{");
	if (p != NULL)
		{
		p += strlen("guid:");
		for (i = 0 ; p[0] && p[0] != '}' && i < (WONplayDLL::nWON_ROOM_ID - 1) ; p++, i++)
			wonParams.wonPlayRoomGUID[i] = p[0];
		wonParams.wonPlayRoomGUID[i] = '}';
		wonParams.wonPlayRoomGUID[i + 1] = 0;
		successes++;
		}
	
	p = strstr(lpszCmdParam, "wonplaydllpath:\"");
	if (p != NULL)
		{
		p += strlen("wonplaydllpath:\"");
		for (i = 0 ; p[0] && p[0] != '"' && i < (MAX_PATH - 1) ; p++, i++)
			wonParams.wonPlayDLLPath[i] = (p[0] == '/') ? '\\' : p[0];
		wonParams.wonPlayDLLPath[i] = 0;
		successes++;
		}
	
	p = strstr(lpszCmdParam, "mapinipath:\"");
	if (p != NULL)
		{
		p += strlen("mapinipath:\"");
		for (i = 0 ; p[0] && p[0] != '"' && i < (MAX_PATH - 1) ; p++, i++)
			wonParams.mapINIPath[i] = (p[0] == '/') ? '\\' : p[0];
		wonParams.mapINIPath[i] = 0;
		successes++;
		}
	
	if (successes != 4)
		{
		MessageBoxA(NULL, GetString(241, path), (char *)szAppName, MB_OK | MB_ICONSTOP | MB_TASKMODAL);
		
		return 0;
		}
	
	if (ParseResourcePaths(wonParams.mapINIPath) != 0)
	{
		return 0;
	}
	
	hInstance = hInst;
	
	return Main();
	}

int Main()
{
	int iResult;
	
	if (!PreFlight())
		return 0;
	
	won = new WONplayDLL::WON();
	pApp = new Application();
	iResult = pApp->Run();
	// MessageBoxA(NULL, "Goodbye 3", "DEBUG", MB_OK);
	delete pApp;
	// MessageBoxA(NULL, "Goodbye 4", "DEBUG", MB_OK);
	if (won) {
		won->Exit();
		// MessageBoxA(NULL, "Goodbye 5", "DEBUG", MB_OK);
		delete won;
		// MessageBoxA(NULL, "Goodbye 6", "DEBUG", MB_OK);
	}
	
	if (hResourceInstance)
	{
		BOOL rv = FreeLibrary(hResourceInstance);
		assert(rv);
		hResourceInstance = NULL;
	}

	// MessageBoxA(NULL, "Goodbye 7", "DEBUG", MB_OK);
	
	if (hGameResourceInstance)
	{
		BOOL rv = FreeLibrary(hGameResourceInstance);
		assert(rv);
		hGameResourceInstance = NULL;
	}
	
	// MessageBoxA(NULL, "Goodbye 8", "DEBUG", MB_OK);
	
	return iResult;
}

int maxMemoryAllocated = 0;
int newDeleteBalance = 0;

static char newS[32];
static char newTitle[32];

void * __cdecl operator new(size_t s)
	{
	size_t *temp;

	// Free some memory if we're already using too much.
	// "Too much" is defined in VApp.
	if (pApp)
		pApp->PurgeResources();

	temp = (size_t *)malloc(s + 4);
	if ((temp == NULL) && pApp)
		{
		// Allocation failed, try to free some memory.
		pApp->PurgeResources(0);	// Purge all!

		// Try again.
		temp = (size_t *)malloc(s + 4);

		// Failure!
		if (temp == NULL)
			{
			MessageBoxA(NULL, GetString(1, newS, 32), GetString(2, newTitle, 32), MB_OK | MB_TASKMODAL);
			return NULL;
			}
		}
	*temp = s;
	numBytesAllocated += s;
	if (numBytesAllocated > maxMemoryAllocated)
		maxMemoryAllocated = numBytesAllocated;
	newDeleteBalance++;
	return temp+1;
	}

void  __cdecl operator delete(void *s)
	{
	size_t *temp;

	if (s)
		{
		temp = (size_t*)s;
		--temp;
		numBytesAllocated -= *temp;
		newDeleteBalance--;
		free(temp);
		}
	}
