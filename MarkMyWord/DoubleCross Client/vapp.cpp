#include "os.h"

#include "app.h"
#include "sounder.h"
#include "timer.h"
#include "resman.h"
#include "prf.h"
#include "inifile.h"
#include "sciplane.h"
#include "game.h"
#include "random.h"

#include <stdlib.h>
#include <string.h>
#include <time.h>

#if defined(OS_WINDOWS)
#define _SIZE_T_DEFINED
#include <direct.h>
#endif

extern char szIniFilename[];
extern int numBytesAllocated;
extern VApplication *pApp;
extern Game *game;
extern int maxMemory;
extern char cdDriveLetter;
extern char exeDir[];
BOOL outputResources;
BOOL listResources;
BOOL changeDisplay;
BOOL useDD = FALSE;
// PRF *outputPRF;

VApplication::VApplication()
	{
	// IniFile *sierraINF;

	iReturnResult = 0;
	randomGenerator = new Random();
	randomGenerator->Seed(time(NULL));
	pMainPlane = NULL;
	game = NULL;
	pSounder = NULL;
	pResourceManager = NULL;
	listOfResourceManagers = new List();
	handsOff = FALSE;

	char szIniPath[MAX_PATH];
	strcpy(szIniPath, exeDir);
	strcat(szIniPath, szIniFilename);
	pIniFile = new IniFile(szIniPath, "Settings");

	if (cdDriveLetter)
		pIniFile->WriteValue("CDDrive", cdDriveLetter);

	/*
	// Get PatchVersion string.
	strcpy(szIniPath, exeDir);
	strcat(szIniPath, "sierra.inf");
	sierraINF = new IniFile(szIniPath, "Ident");
	sierraINF->ReadString("PatchVersion", "0.0.0.0", versionText, sizeof(versionText));
	delete sierraINF;
	*/

	outputResources = pIniFile->ReadValue("OutputResources", FALSE);
	listResources = pIniFile->ReadValue("ListResources", FALSE);
	changeDisplay = pIniFile->ReadValue("ChangeDisplay", FALSE);
#if defined(_DEBUG) || defined(SIGS_DEMO) || !defined(HOYLE_CASINO2)
	useDD = pIniFile->ReadValue("UseDirectDraw", FALSE);
#else
	useDD = pIniFile->ReadValue("UseDirectDraw", TRUE);
#endif // defined(_DEBUG)

	if (outputResources)
		{
		_mkdir("prfout");
		/*
		FILE *file;

		file = fopen("prf.out", "rb");
		outputPRF = new PRF();
		if (file)
			{
			// File exists, just add to it.
			fclose(file);
			outputPRF->Open("prf.out", "rb+");
			}
		else
			{
			// File doesn't exist. Create a new one.
			outputPRF->New("prf.out");
			}
		*/
		}
	
	SetModalPlane(NULL);
	capturedPlane = NULL;
	}

VApplication::~VApplication()
	{
	delete pIniFile;
	delete randomGenerator;
	delete listOfResourceManagers;
	/*
	if (outputResources)
		delete outputPRF;
	*/
	}

int VApplication::Run()
	{
	pTimer = new Timer();
	pResourceManager = new ResourceManager();
	pMainPlane = new sciPlane(FALSE, NULL);
	pSounder = new Sounder();
	game = new Game();
	
	game->Play();
	
	if (pSounder)
		delete pSounder;
	pSounder = NULL;
	delete game;
	delete pMainPlane;
	delete pResourceManager;
	delete pTimer;

	return iReturnResult;
	}

void VApplication::Idle()
	{
	if (pSounder)
		pSounder->Doit();
	}

void VApplication::PurgeResources(int bytesCanUse)
	{
	ResourceManager *aResMan;

	if (bytesCanUse == -1)
		bytesCanUse = maxMemory;
	aResMan = (ResourceManager*)listOfResourceManagers->GetFirst();
	while (aResMan && (numBytesAllocated > bytesCanUse))
		{
		aResMan->PurgeResources(bytesCanUse);
		aResMan = (ResourceManager*)listOfResourceManagers->GetNext();
		}
	}

void VApplication::AddPlane(sciPlane *newPlane)
	{
	planes.AddToBack(newPlane);
	}

void VApplication::MarkPlaneForDeletion(sciPlane *oldPlane)
	{
	int i;
	sciPlane *plane;
	
	// also mark for deletion any plane that has this plane as its parent
	
	for (i = 0 ; i < planes.Size() ; i++)
		{
		plane = (sciPlane *)planes.At(i);
		if (plane->GetParentPlane() == oldPlane)
			plane->Delete();
		}
	
	oldPlane->Delete();
	}

void VApplication::RemovePlane(sciPlane *newPlane)
	{
	planes.Delete(newPlane);
	}

void VApplication::SetModalPlane(sciPlane *thePlane)
	{
	modalPlane = thePlane;
	if (modalPlane)
		{
		SetCapturedPlane(NULL);
		DoModalPlaneCreated();
		}
	else
		{
		DoModalPlaneDestroyed();
		}
	}

void VApplication::DoModalPlaneCreated()
	{
	}

void VApplication::DoModalPlaneDestroyed()
	{
	}

sciPlane *VApplication::GetModalPlane()
	{
	return modalPlane;
	}

sciPlane *VApplication::FindPlane(int x, int y, Window *window)
	{
	if (GetCapturedPlane())
		return GetCapturedPlane();
	if (GetModalPlane())
		return GetModalPlane();
	
	return FindTruePlane(x, y, window);
	}

sciPlane *VApplication::FindTruePlane(int x, int y, Window *window)
	{
	int i;
	sciPlane *plane = NULL;
	
	// x and y are in local coordinates with respect to window
	
	for (i = planes.Size() - 1 ; i >= 0 ; i--)
		{
		plane = (sciPlane*)planes.At(i);
		if (plane->GetWindow() != window)
			continue;
		if ((x >= plane->GetX()) && (x <= (plane->GetX() + plane->GetWidth())) &&
			(y >= plane->GetY()) && (y <= (plane->GetY() + plane->GetHeight())) &&
			plane->IsNotHidden())
			return plane;
		}
	
	//No planes found yet, find the plane without a parent plane whose Window is window...
	
	for (i = planes.Size() - 1 ; i >= 0 ; i--)
		{
		plane = (sciPlane*)planes.At(i);
		if (plane->GetParentPlane() == NULL && plane->GetWindow() == window)
			return plane;
		}

	// No planes found. Return the default plane (may be NULL).

	return plane;
	}

Boolean VApplication::MouseIsOverFeatureInPlane(sciPlane *plane, sciFeature *feature)
{
	Boolean mouseOnPlane;
	POINT p;
	Window *w;
	sciEvent event;
	
	w = plane->GetWindow();
	if (w->hWnd != GetActiveWindow())
		return FALSE;
	
	GetCursorPos(&p);
	MapWindowPoints(NULL, w->hWnd, &p, 1);
	p.x -= w->rcInner.left;
	p.y -= w->rcInner.top;
	event.x = p.x;
	event.y = p.y;
	mouseOnPlane = FindTruePlane(p.x, p.y, w) == plane;
	
	event.x -= plane->GetX();
	event.y -= plane->GetY();
	
	return mouseOnPlane && feature->OnMe(event);
}

void VApplication::SetCapturedPlane(sciPlane *plane)
	{
	capturedPlane = plane;
	}

sciPlane *VApplication::GetCapturedPlane()
	{
	return capturedPlane;
	}

void VApplication::SetViewGamma(double gamma)
	{
	int i;
	sciPlane *plane;
	
	for (i = 0; i <planes.Size(); ++i)
		{
		plane = (sciPlane*)planes.At(i);
		plane->SetViewGamma(gamma);
		}
	}
