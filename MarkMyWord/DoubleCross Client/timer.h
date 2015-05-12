#ifndef _Timer
#define _Timer

#include "vtimer.h"

class Timer : public VTimer
	{
	public:
		Timer();
		~Timer();
		int Get60ths();
	};

#endif // _Timer

