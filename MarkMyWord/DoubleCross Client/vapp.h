#ifndef _vapp
#define _vapp

#include "object.h"
#include "sciFeatr.h"
#include "sciList.h"

#define MAIN_WINDOW pApp->pMainPlane->GetWindow()
#define MAIN_HWND pApp->pMainPlane->GetWindow()->hWnd

class Sounder;
class Timer;
class Window;
class PRF;
class ResourceManager;
class sciPlane;
class IniFile;
class Random;
class List;

class VApplication : public Object
	{
	public:
		VApplication();
		virtual ~VApplication();
		virtual int Run();
		virtual void Idle();
		void PurgeResources(int bytesCanUse = -1);
		virtual void HandsOff() = NULL;
		virtual void HandsOn() = NULL;
		
		void AddPlane(sciPlane *newPlane);
		void MarkPlaneForDeletion(sciPlane *oldPlane);
		void RemovePlane(sciPlane *oldPlane);
		void SetModalPlane(sciPlane *thePlane);
		virtual void DoModalPlaneCreated();
		virtual void DoModalPlaneDestroyed();
		sciPlane *GetModalPlane();
		sciPlane *FindPlane(int x, int y, Window *window);
		sciPlane *FindTruePlane(int x, int y, Window *window);
		Boolean MouseIsOverFeatureInPlane(sciPlane *plane, sciFeature *feature);
		void SetCapturedPlane(sciPlane *plane);
		sciPlane *GetCapturedPlane();
		void SetViewGamma(double gamma);
		
		sciList planes;
		sciPlane *pMainPlane;
		
		Sounder *pSounder;
		Timer *pTimer;
		ResourceManager *pResourceManager;
		List *listOfResourceManagers;
		IniFile *pIniFile;
		Random *randomGenerator;
		
		Boolean handsOff;
		DWORD dwLastClick;
	
	protected:
		int iReturnResult;
		sciPlane *modalPlane;
		sciPlane *capturedPlane;
	};

#endif	// _vapp
