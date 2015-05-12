#include "os.h"
#include "timer.h"

Timer::Timer()
	{
	}

Timer::~Timer()
	{
	}

int Timer::Get60ths()
	{
	return (3 * timeGetTime() / 50);
	}
