#ifndef _sound
#define _sound

#include "vsound.h"

class ResourceManager;

class Sound : public VSound
	{
	public:
		Sound(int iResourceID, ResourceManager *resourceManager = NULL);
		~Sound();
		int			itsEndTime;
	private:
	};

class SoundC : public VSound
	{
	public:
		SoundC(int iResourceID, ResourceManager *resourceManager = NULL);
		~SoundC();
		int			itsEndTime;
	private:
	};

#endif // _sound
