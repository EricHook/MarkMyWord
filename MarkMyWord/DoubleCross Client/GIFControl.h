#ifndef _gifcontrol
#define _gifcontrol

typedef struct
{
	BYTE disposalMethod;
	BYTE userInput;
	BYTE useTrans;
	WORD delay;
	BYTE transIndex;
}
GIFControls;

#endif // _gifcontrol
