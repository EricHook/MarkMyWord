#ifndef _vsounder
#define _vsounder

#include "object.h"
#include "sound.h"

class Sound;
class ResourceManager;

#define MAX_SOUNDS 16

// Defines for GetPositionInTicks
#define SOUND_ENDED -1
#define SOUND_NOT_STARTED_YET -2

class VSounder : public Object
	{
	public:
		VSounder();
		~VSounder();
		int Play(int resourceID, ResourceManager *resourceManager = NULL,
			int newLoop = 0, int *handle = NULL, int volume = 127, int pan = -1, int important = FALSE);
		BOOL IsSoundPlaying();
		virtual void EndSound(int index = -1, BOOL endNow=TRUE) = 0;	// index = index into playingSounds[];
		void EndSoundNum(int s, BOOL endNow=TRUE);				// s = playingSounds[].soundNum;
		void EndSoundHandle(int h, BOOL endNow=TRUE);
		virtual int GetPositionInTicks(int handle) = 0;

	protected:
		virtual void PlayItNow(int index, int *outHandle) = 0;	// index = index into playingSounds[];
		int FindSound(Sound *s);				// find index of sound s;

		Sound 		*playingSounds[MAX_SOUNDS];
		int			playingSoundNums[MAX_SOUNDS];
		int			playingSoundHandles[MAX_SOUNDS];
		Boolean		playSounds;
		int			nextHandle;
	};

#endif // _vsounder
