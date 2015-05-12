#include "os.h"
#include "app.h"
#include "inifile.h"
#include "bitmap.h"
#include "resman.h"
#include "timer.h"
#include "sci.h"
#include "rooms.h"
#include "sounder.h"
#include "framebuf.h"
#include "tsprite.h"

extern Application *pApp;
extern sciList unInitedFeatures;
extern BOOL runFullSpeed;
extern BOOL showFrameRate;
extern BOOL showMousePosition;
extern BOOL skipPaletteChanges;
extern sciText *frameRate;
extern sciText *mousePosition;

BOOL gameStartup;

sciGame::sciGame()
	{
	HandsOff();
	gameStartup = TRUE;
	quit = FALSE;
	time = GetTrueTime();
	nextRoomNum = ROOM_NONE;
	prevRoomNum = ROOM_NONE;
	curRoomNum = ROOM_NONE;
	curRoom = NULL;
	script = NULL;
	sound1 = new sciSound();
	sound2 = new sciSound();
	sound2->talkingSound = TRUE;
	runFullSpeed = pApp->pIniFile->ReadValue("RunFullSpeed", FALSE);
	showFrameRate = pApp->pIniFile->ReadValue("ShowFrameRate", FALSE);
	showMousePosition = pApp->pIniFile->ReadValue("ShowMousePosition", FALSE);
	skipPaletteChanges = pApp->pIniFile->ReadValue("SkipPaletteChanges", FALSE);
	
	prevDoit = 0;
	currDoit = 0;
	}

sciGame::~sciGame()
	{
	LoadTheRoom(-1);
	SetScript(NULL);
	delete sound1;
	delete sound2;
	}

Boolean sciGame::HandleEvent(sciEvent &event)
	{
	if (curRoom)
		curRoom->HandleEvent(event);
	return event.claimed;
	}

int sciGame::GetTime()
	{
	return time;
	}

int sciGame::GetTrueTime()
	{
	return pApp->pTimer->Get60ths();
	}

void sciGame::Play()
	{
	Init();

	while (!quit)
		{
		Doit();
		}
	}

Boolean sciGame::IsDone()
	{
	return quit;
	}

BOOL sciGame::QuitGame(Boolean iReallyMeanIt)
{
	// TODO - this should really be i18nStr getS, getS1
	char getS[256], getS1[256];
	char s[128];
	
	if (quit || iReallyMeanIt)
		quit = TRUE;
	else
		{
		/*
		if (curRoom->GetModalPlane() != NULL)
			return; // can't quit game when a dialog is up
		*/
		// "Are you sure you want to quit\n%s?"
		sprintf(s, GetString(359, getS), (char *)szAppName);
		// "Quitting"
		if (IDYES == MessageBox(NULL, s, GetString(358, getS1), MB_YESNO | MB_ICONQUESTION | MB_TASKMODAL))
			{
			if (curRoomNum != ROOM_END)
				SetNextRoom(ROOM_END);
			else
				quit = TRUE;
			}
		}
	return(quit || nextRoomNum == ROOM_END);
	}

void sciGame::Init()
	{
	}

#ifdef _DEBUG
#define COUNT_DOITS
#endif // _DEBUG

#ifdef COUNT_DOITS
int prevDoitTime = 0;
int curDoitTime = 0;
int numDoits = 0;
#endif // COUNT_DOITS

extern DWORD oldMilli;

void sciGame::Doit()
	{
	int i;
	sciPlane *plane;

	prevDoit = currDoit;
	oldMilli = timeGetTime();

	currDoit = GetTrueTime();

#ifdef _DEBUG
	if (!runFullSpeed)
		if (currDoit == prevDoit)
			return;
#else
	if (currDoit == prevDoit)
		return;
#endif // _DEBUG

#ifdef COUNT_DOITS
	char buff[64];
	POINT p;
	
	curDoitTime = GetTrueTime();
	++numDoits;
	if (curDoitTime >= prevDoitTime + 60)
		{
		prevDoitTime = curDoitTime;
		wsprintf(buff, "%d doits.", numDoits);
		if (frameRate)
			{
			frameRate->SetJust(JUST_VERTICAL | JUST_HORIZONTAL);
			frameRate->SetBackgroundColor(192, 192, 192);
			frameRate->SetWidth(100);
			frameRate->SetText(buff);
			frameRate->SetWidth(frameRate->GetMinWidth() + 8);
			frameRate->SetHeight(frameRate->GetMinHeight() + 2);
			}
		numDoits = 0;
		if (mousePosition)
			{
			GetCursorPos(&p);
			MapWindowPoints(NULL, MAIN_HWND, &p, 1);
			sprintf(buff, "(%ld, %ld)", p.x, p.y);
			mousePosition->SetJust(JUST_VERTICAL | JUST_HORIZONTAL);
			mousePosition->SetBackgroundColor(192, 192, 192);
			mousePosition->SetText(buff);
			}
		}
#endif // COUNT_DOITS
	
	time = currDoit;
	if (nextRoomNum != ROOM_NONE)
		LoadRoom(nextRoomNum);
	if (script)
		script->Doit();
	if (pApp->GetModalPlane())
		pApp->GetModalPlane()->Doit();
	else
		if (curRoom)
			curRoom->Doit();
	pApp->Idle();

	for (i = 0 ; i < pApp->planes.Size() ; i++) {
		plane = ((sciPlane *)pApp->planes.At(i));
		if (plane->GetParentPlane() == NULL && plane->IsNotHidden())
			plane->GetWindow()->TruePaintDirty();
	}

#ifdef _DEBUG
	if (!runFullSpeed)
		pApp->WaitTillNextTick();
#else
	pApp->WaitTillNextTick();
#endif // _DEBUG
	}

void sciGame::UpdateTime()
	{
	// this is like a Doit(), but without actually doing anything
	// it is used within a loop, itself called by Doit(), in order to keep the game
	// up to date so, for example, SetTicks() on scripts work out
	
	prevDoit = currDoit;
	oldMilli = timeGetTime();
	
	currDoit = GetTrueTime();
	
	time = currDoit;
	
	pApp->WaitTillNextTick();
	}

void sciGame::SetScript(sciScript *newScript)
	{
	if (script)
		delete script;
	script = newScript;
	if (script)
		script->Init(this);
	}

void sciGame::SetNextRoom(int newRoomNum)
	{
	nextRoomNum = newRoomNum;
	}

int sciGame::GetNextRoom()
{
	return nextRoomNum;
}

void sciGame::LoadRoom(int roomNum)
	{
	prevRoomNum = curRoomNum;
	curRoomNum = roomNum;
	nextRoomNum = ROOM_NONE;
	
	LoadTheRoom(curRoomNum);
	}

void sciGame::LoadTheRoom(int roomNum)
	{
	int i;
	sciObject *obj;
	sciPlane *plane;

#ifdef HOYLE_CASINO2

	RGBA black;
	TSprite *sprite;
	RECT source, dest;
	
	// Draw the "please wait" graphic.
	black.r = 0;
	black.g = 0;
	black.b = 0;
	black.a = 255;
	MAIN_WINDOW->frameBuffer->DrawClosedRect(0, 0, 640, 480, black, ALPHA_FLAG);
	sprite = (TSprite *)pApp->pResourceManager->LoadResource(RES_TSPRITE, 1256);
	SetRect(&source, 0, 0, sprite->GetWidth(0, 0), sprite->GetHeight(0, 0));
	SetRect(&dest, 320 - sprite->GetWidth(0, 0)/2, 240 - sprite->GetHeight(0, 0)/2,
		320 - sprite->GetWidth(0, 0)/2 + sprite->GetWidth(0, 0),
		240 - sprite->GetHeight(0, 0)/2 + sprite->GetHeight(0, 0));
	sprite->CopyPixels(0, 0, MAIN_WINDOW->frameBuffer, dest, source);
	pApp->pResourceManager->FreeResource(sprite);
	SetRect(&source, 0, 0, 640, 480);
	MAIN_WINDOW->CopyToScreen(source);

#endif //HOYLE_CASINO2

	if (curRoom)
		{
		if (pApp->pSounder)
			{
			pApp->pSounder->EndSound();
			// pApp->pSounder->EndSong();
			}
		// curRoom->plane->window->Erase(curRoom->plane->window->rcOuter); // MAK
		delete curRoom;
		curRoom = NULL;
		while (unInitedFeatures.Size())
			{
			obj = unInitedFeatures.At(0);
			unInitedFeatures.Delete(obj);
			delete obj;
			}
		}
	pApp->PurgeResources(0);

	// Paint the windows so that the dirty rects get disposed so memory usage
	// can be tracked more accurately.
	for (i = 0 ; i < pApp->planes.Size() ; i++) {
		plane = ((sciPlane *)pApp->planes.At(i));
		if (plane->GetParentPlane() == NULL)
			plane->GetWindow()->UnPaintAll();
	}

#ifdef _DEBUG
	char s[256];
	static int nbaWhenRoomCreated = 0, objectCountWhenRoomCreated = 0;
	
	extern int numBytesAllocated;
	extern int newDeleteBalance;
	extern void ResetObjectsMonitor();
	extern void PrintUndisposedObjectNumbers();
	
	PrintUndisposedObjectNumbers();
	
	wsprintf(s, "Object delta by room: %d\n", Object::numObjects - objectCountWhenRoomCreated);
	DebugString(s);
	
	wsprintf(s, "Memory delta by room: %d bytes.\n", numBytesAllocated - nbaWhenRoomCreated);
	DebugString(s);
	
	wsprintf(s, "NewDeleteBalance by room: %d\n", newDeleteBalance);
	DebugString(s);
	
	newDeleteBalance = 0;
	nbaWhenRoomCreated = numBytesAllocated;
	objectCountWhenRoomCreated = Object::numObjects;
	
	ResetObjectsMonitor();
#endif
	}
int sciGame::GetPreviousRoomNum()
	{
	return prevRoomNum;
	}
int sciGame::GetCurrentRoomNum()
	{
	return curRoomNum;
	}

void sciGame::SetCurrentRoomNum(int newRoomNum)
	{
	curRoomNum = newRoomNum;
	}

void sciGame::SetGameName(char *name)
	{
	char windowText[256];

	if (name)
		{
		wsprintf(windowText, "%s - %s", (char *)szAppTitle, name);
		MAIN_WINDOW->SetTitle(windowText);
		}
	else
		{
		MAIN_WINDOW->SetTitle((char *)szAppTitle);
		}
	}

void sciGame::HandsOn()
	{
	handsOn = TRUE;
	pApp->HandsOn();
	}

void sciGame::HandsOff()
	{
	handsOn = FALSE;
	pApp->HandsOff();
	}
