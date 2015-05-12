#include "os.h"
#include "scievent.h"

sciEvent::sciEvent()
	{
	claimed = FALSE;
	x = 0;
	y = 0;
	c = 0;
	plane = NULL;
	modifiers = 0;
	type = EVENT_TYPE_NONE;
	message = 0;
	lparam = 0;
	wparam = 0;
	}

sciEvent::~sciEvent()
	{
	}
