#ifndef _scisound
#define _scisound

#include "sciobj.h"
#include "sound.h"

class sciScript;
class ResourceManager;

class sciSound : public sciObject
	{
	public:
		sciSound(ResourceManager *newResourceManager = NULL);
		~sciSound();
		int Play(int newNumber = -1, int volume = -1, sciScript *whoToCue = NULL,
			int newPan = -1);	// Returns length of sound in ticks.
		void SetNumber(int newNumber);
		void Stop();
		void SetLoop(int newLoop);
		int GetPositionInTicks();
		void SetPan(int newPan);

		BOOL talkingSound;
		ResourceManager *resourceManager;

	protected:
		int number;
		int volume;
		int loop;
		int handle;
		int pan;
	};

#endif // _scisound
