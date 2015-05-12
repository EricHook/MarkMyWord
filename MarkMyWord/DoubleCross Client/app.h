#ifndef _app
#define _app

#include "vapp.h"
	
class Application : public VApplication
	{
	public:
		Application();
		~Application();
		void Idle();
		void WaitTillNextTick();
		void HandsOff();
		void HandsOn();
		int NumJoysticks();
	private:
	};	
	
#endif //_app
