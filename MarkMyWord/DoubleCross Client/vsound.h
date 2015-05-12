#ifndef _vsound
#define _vsound

#include "resource.h"

// Similar to SOS's defines
#define  PAN_LEFT         0x00000000
#define  PAN_CENTER       0x00008000
#define  PAN_RIGHT        0x0000ffff

typedef enum
	{
	WF_NONE	= 0,
	WF_8BITMONO,
	WF_8BITSTEREO,
	WF_16BITMONO,
	WF_16BITSTEREO
	} WAVE_FORMATS;

class UniHandle;

class VSound : public Resource
	{
	public:
		VSound(RESOURCE_TYPES newType, int newResourceID, ResourceManager *newResourceManager);
		virtual ~VSound();
		void Free();
		BYTE *GetData();
		void Convert16to8();

		WAVE_FORMATS	wfFormat;
		WORD			wSamplesPerSec;
		DWORD			dwNumOfSamples;
		UniHandle		*pSamples;
		int				loop;
		int				volume;
		int				pan;
		BOOL			important;
	};

#endif // _vsound
