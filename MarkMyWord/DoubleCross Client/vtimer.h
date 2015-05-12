#ifndef _VTimer
#define _VTimer

#include "object.h"

class VTimer : public Object
	{
	public:
		VTimer();
		~VTimer();
		virtual int Get60ths() = 0;
	};

#endif // _VTimer
