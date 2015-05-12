#include "os.h"
#include "vsound.h"
#include "app.h"
#include "resman.h"
#include "unihand.h"

extern Application *pApp;

VSound::VSound(RESOURCE_TYPES newType, int newResourceID, ResourceManager *newResourceManager)
	: Resource(newType, newResourceID, newResourceManager)
	{
	wfFormat = WF_NONE;
	wSamplesPerSec = 0;
	dwNumOfSamples = 0;
	pSamples = NULL;
	loop = 0;
	important = FALSE;
	}

VSound::~VSound()
	{
	delete pSamples;
	}

void VSound::Free()
	{
	resourceManager->FreeResource(this);
	}

BYTE *VSound::GetData()
	{
	if (pSamples)
		return PBYTE(pSamples->Lock());
	else
		return NULL;
	}

void VSound::Convert16to8()
	{
	UniHandle *newHandle;
	BYTE *bytePointer;
	WORD *wordPointer;
	DWORD i;

	DebugString("Converting 16bit sound to 8bit.\n");
	switch (wfFormat)
		{
		case WF_16BITMONO:
			newHandle = new UniHandle(dwNumOfSamples);
			wfFormat = WF_8BITMONO;
			break;
		case WF_16BITSTEREO:
			newHandle = new UniHandle(dwNumOfSamples * 2);
			wfFormat = WF_8BITSTEREO;
			break;
		}
	bytePointer = (BYTE *)newHandle->Lock();
	wordPointer = (WORD *)pSamples->Lock();
	for (i = 0; i < dwNumOfSamples; ++i)
		{
		*bytePointer = (*wordPointer >> 8) + 128;
		++bytePointer;
		++wordPointer;
		}
	delete pSamples;
	pSamples = newHandle;
	}
