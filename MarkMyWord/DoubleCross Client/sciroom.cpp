#include "os.h"
#include "app.h"
#include "palette.h"
#include "bitmap.h"
#include "sciroom.h"
#include "sciplane.h"
#include "sciscrip.h"
#include "scitext.h"

extern Application *pApp;
extern int maxMemoryAllocated;
extern BOOL showFrameRate;
extern BOOL showMousePosition;
extern double curGamma;

sciText *frameRate = NULL;
sciText *mousePosition = NULL;

sciRoom::sciRoom()
	{
	script = NULL;
	maxMemoryAllocated = 0;
	}

sciRoom::~sciRoom()
	{
	char str[128];
	sciPlane *plane;
	
	wsprintf(str, "Max Memory used = %d.\n", maxMemoryAllocated);
	DebugString(str);
	
	// the room deletes all planes but the main plane, which belongs to the application
	
	assert((sciPlane *)pApp->planes.At(0) == pApp->pMainPlane);
	
	while (pApp->planes.Size() > 1)
		{
		plane = (sciPlane *)pApp->planes.At(1);
		delete plane;
		}
	SetScript(NULL);
	}

void sciRoom::Init()
	{
	// the room always uses the app's main plane as its main plane
	
	pApp->pMainPlane->SetDraggableArea(0);
	pApp->pMainPlane->Show();
	
	SetPic(picNumber);

	if (showFrameRate)
		{
		frameRate = new sciText();
		frameRate->Init();
		frameRate->SetPri(9999);
		frameRate->Show();
		}
	else
		{
		frameRate = NULL;
		}
	
	if (showMousePosition)
		{
		mousePosition = new sciText();
		mousePosition->Init();
		mousePosition->SetPri(9999);
		mousePosition->Posn(0, 15);
		mousePosition->SetWidth(60);
		mousePosition->SetHeight(15);
		mousePosition->Show();
		}
	else
		{
		mousePosition = NULL;
		}
	}

Boolean sciRoom::HandleEvent(sciEvent &event)
	{
	int i, windowX, windowY;
	sciPlane *plane;
	Window *eventWindow;
	
	if (event.plane) {
		eventWindow = event.plane->GetWindow();
		
		// Make x/y global to the OS window.
		event.x += event.plane->GetX();
		event.y += event.plane->GetY();
		
		windowX = event.x;
		windowY = event.y;
	}
	
	if (!event.claimed && event.plane)
		{
		// Pass events to the modal plane, if there is one.
		if (pApp->GetModalPlane())
			{
			event.plane = pApp->GetModalPlane();
			// Make x/y local to the SCI plane.
			event.x -= event.plane->GetX();
			event.y -= event.plane->GetY();
			event.plane->HandleEvent(event);
			
			return event.claimed;
			}
		if (pApp->GetCapturedPlane())
			{
			event.plane = pApp->GetCapturedPlane();
			// Make x/y local to the SCI plane.
			event.x -= event.plane->GetX();
			event.y -= event.plane->GetY();
			event.plane->HandleEvent(event);
			if (event.claimed)
				return event.claimed;
			
			event.x = windowX;
			event.y = windowY;
			event.plane = pApp->FindTruePlane(event.x, event.y, eventWindow);
			if (!event.plane)
				return event.claimed;
			}
		
		switch (event.type)
			{
			case MOUSE_DOWN:
				// Send to the right plane.
				// Make x/y local to the SCI plane.
				event.x -= event.plane->GetX();
				event.y -= event.plane->GetY();
				event.plane->HandleEvent(event);
				break;
			default:
				// Pass the event to all planes that share event's window.
				
				for (i = pApp->planes.Size() - 1; i >= 0; --i)
					{
					plane = (sciPlane *)pApp->planes.At(i);
					if (plane->GetWindow() != eventWindow)
						continue;
					event.x = windowX;
					event.y = windowY;
					// Make x/y local to the SCI plane.
					event.plane = plane;
					event.x -= event.plane->GetX();
					event.y -= event.plane->GetY();
					event.plane->HandleEvent(event);
					}
				break;
			}
		}
	
	return event.claimed;
	}

void sciRoom::SetScript(sciScript *newScript)
	{
	if (script)
		delete script;
	script = newScript;
	if (script)
		script->Init(this);
	}

void sciRoom::Doit()
	{
	int i;
	sciPlane *plane;

	if (script)
		script->Doit();

	i = 0;
	plane = (sciPlane *)pApp->planes.At(i);
	while (i < pApp->planes.Size())
		{
		plane->Doit();
		++i;
		plane = (sciPlane *)pApp->planes.At(i);
		}

	// Iterate through and delete planes to be deleted.
	i = 0;
	plane = (sciPlane *)pApp->planes.At(i);
	while (i < pApp->planes.Size())
		{
		if (plane->IsToBeDeleted())
			{
			delete plane;

			// Start the loop over.
			i = 0;
			}
		else
			{
			++i;
			}
		plane = (sciPlane *)pApp->planes.At(i);
		}
	}

void sciRoom::SetPic(int newPicNumber)
	{
	picNumber = newPicNumber;
	if (pApp->pMainPlane)
		pApp->pMainPlane->SetPic(newPicNumber);
	}
