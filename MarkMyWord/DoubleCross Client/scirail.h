#ifndef _scirail
#define _scirail

#include "scicycle.h"

class sciSound;

typedef struct
	{
	WORD wFrameNumber;
	WORD wDuration;
	} RAIL_SYNC_FRAME;

typedef struct
	{
	WORD wSoundID;
	WORD wNewRestImage;
	WORD wNumSyncFrames;
	RAIL_SYNC_FRAME rsFrames[1];
	} RAIL_SYNC;

#define kSyncQueueLength 10
#define kRestImageNumber 0xFFFF

class ResourceManager;

class sciRailCycle : public sciCycle
	{
	public:
		sciRailCycle(sciScript *newCaller = NULL);
		~sciRailCycle();
		void Doit();
		void AppendSync(int iResourceID);
		void AppendRandomSync(int iResourceID);
		void AppendSync(int iSyncID, int iSubSyncNumber);
		void AppendSync(RAIL_SYNC *pTheSyncData);
		Boolean IsSyncing();
		void StopSyncing();
		void SetSound(sciSound *newSound);
		int GetNumSyncs(int iResourceID, ResourceManager *newResman = NULL);
		void SetRestImage(int theRestImage);

	private:
		int					iCurrentFrame;
		int					itsStartTime;
		int					itsPreviousSyncTime;
		RAIL_SYNC			*itsDirectSyncData[kSyncQueueLength];
		RAIL_SYNC			*pCurrentSyncData;
		int					itsHead;
		int					itsTail;
		int					itsNumQueuedSyncs;
		Boolean				bIStartedASound;
		int					itsRestImageNumber;
		int					itsCurrentSyncNumber;
		Boolean				bStartSyncImmediately;		// Start sync when appended,
														// or when next tick occurs.		
		sciSound			*sound;
		int					soundEndTime;

		void BeginSync();
		void EndSync();
		Boolean IsSyncQueueFull();
		RAIL_SYNC *GetSync(WORD *pData, int iIndex);
	};

#endif // _scirail
