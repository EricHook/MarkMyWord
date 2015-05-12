#include "os.h"

#include "scirail.h"
#include "app.h"
#include "resman.h"
#include "game.h"
#include "sciprop.h"
#include "scisound.h"
#include "sounder.h"

extern Application *pApp;
extern Game *game;

#include "string.h"

//#define SHOW_SYNC_DETAILS

sciRailCycle::sciRailCycle(sciScript *newCaller)
	: sciCycle(newCaller)
	{
	itsStartTime = 0;
	itsNumQueuedSyncs = 0;
	itsRestImageNumber = 0;
	itsCurrentSyncNumber = 0;
	iCurrentFrame = 0;
	itsHead = 0;
	itsTail = 0;
	itsPreviousSyncTime = 0;
	pCurrentSyncData = NULL;
	bStartSyncImmediately = TRUE;
	bIStartedASound = FALSE;
	sound = NULL;
	}

sciRailCycle::~sciRailCycle()
	{
	//StopSyncing();
	while (itsNumQueuedSyncs)
		{
		--itsNumQueuedSyncs;
		delete[] itsDirectSyncData[itsHead];
		itsDirectSyncData[itsHead] = NULL;
		itsHead = (itsHead+1) % (int)kSyncQueueLength;
		}
	}

void sciRailCycle::AppendSync(int iResourceID)
	{
	Resource *pResource;

	pResource = client->resourceManager->LoadResource(RES_SYNC, iResourceID);
	AppendSync((RAIL_SYNC *)(pResource->GetData()));
	client->resourceManager->FreeResource(pResource);
	}

void sciRailCycle::AppendSync(int iSyncID, int iSubSyncNumber)
	{
	Resource *pResource;
	char str[64];

	pResource = client->resourceManager->LoadResource(RES_SYNC, iSyncID);
	sprintf(str, "Loading sync %d\n", iSyncID + iSubSyncNumber);
	DebugString(str);
	AppendSync(GetSync((WORD *)(pResource->GetData()), iSubSyncNumber));
	client->resourceManager->FreeResource(pResource);
	}

void sciRailCycle::AppendRandomSync(int iSyncID)
	{
	Resource *pResource;
	WORD *pRailSyncGroup;
#ifdef SHOW_SYNC_DETAILS
	char str[64];
#endif // SHOW_SYNC_DETAILS
	int iSubSync;

	pResource = client->resourceManager->LoadResource(RES_SYNC, iSyncID);
	if (pResource)
		{
		pRailSyncGroup = (WORD *)(pResource->GetData());
		if (pRailSyncGroup)
			{
			iSubSync = random(*pRailSyncGroup);
#ifdef SHOW_SYNC_DETAILS
			sprintf(str, "Doing sync group #%d, sub-sync %d.\n", iSyncID, iSubSync);
			DebugString(str);
#endif // SHOW_SYNC_DETAILS
			AppendSync(GetSync(pRailSyncGroup, iSubSync));
			}
		//else
		//	DebugString("NULL sync.\n");
		}
	//else
	//	DebugString("Sync not found.\n");
	client->resourceManager->FreeResource(pResource);
	}

RAIL_SYNC *sciRailCycle::GetSync(WORD *pData, int iIndex)
	{
	WORD	far *theRailSyncPtr;
	WORD	theNumSyncFrames;

	if (pData == NULL || iIndex >= *pData || iIndex < 0)
		return NULL;

	#define kSyncFrameSize 2		// 2 WORD
	#define kRailSyncHeader 2		// 2 WORD

	theRailSyncPtr = ((WORD far *)pData)+1;

	while ((iIndex--) > 0) {
		theRailSyncPtr += kRailSyncHeader;
		theNumSyncFrames = *(theRailSyncPtr++);
		theRailSyncPtr += (theNumSyncFrames*kSyncFrameSize);
		}
	return ((RAIL_SYNC *)theRailSyncPtr);
	}

void sciRailCycle::AppendSync(RAIL_SYNC *pTheSyncData)
	{
	int sLength;
	Boolean bStartSync;
	char str[64];

	if (!pTheSyncData)
		{
		//DebugString("Appending a NULL sync.\n");
		return;
		}
	
	// If no syncs are currently syncing, start this one after its been appended.
	bStartSync = !IsSyncing();

	itsNumQueuedSyncs++;
	if (pTheSyncData->wSoundID < 1000)
		{
		sprintf(str, "Bad Sound %d***\n", pTheSyncData->wSoundID);
		DebugString(str);
		pTheSyncData->wSoundID = 0xFFFF;
		}
	if (pTheSyncData->wNumSyncFrames)
		{
		sLength = (int)(sizeof(RAIL_SYNC) + (sizeof(RAIL_SYNC_FRAME) * (pTheSyncData->wNumSyncFrames-1)));
		itsDirectSyncData[itsTail] = (RAIL_SYNC *)new BYTE[sLength];
		memcpy(itsDirectSyncData[itsTail], pTheSyncData, sLength);
		}
	else
		{
		// Special case empty syncs.
		sLength = (int)(sizeof(RAIL_SYNC) - sizeof(RAIL_SYNC_FRAME));
		itsDirectSyncData[itsTail] = (RAIL_SYNC *)new BYTE[sizeof(RAIL_SYNC)];
		memcpy(itsDirectSyncData[itsTail], pTheSyncData, sLength);
		itsDirectSyncData[itsTail]->rsFrames[0].wFrameNumber = 0;
		itsDirectSyncData[itsTail]->rsFrames[0].wDuration = 0;
		//itsDirectSyncData[itsTail]->wSoundID = 0xFFFF;
		}

	itsTail = (itsTail+1) % (int)kSyncQueueLength;

	if (bStartSync && bStartSyncImmediately)
		BeginSync();
	}

void sciRailCycle::BeginSync()
	{
	soundEndTime = 0;
	pCurrentSyncData = itsDirectSyncData[itsHead];
	if (pCurrentSyncData->wNewRestImage != kRestImageNumber)
		SetRestImage(pCurrentSyncData->wNewRestImage);

	if ((pCurrentSyncData->wSoundID > 1) && (pCurrentSyncData->wSoundID < 65535))
		{
		if (sound)
			{
#if defined(HOYLE_BOARD_GAMES) || defined(HOYLE_CG2)
			if (pCurrentSyncData->wSoundID > 32767)
				soundEndTime = sound->Play(pCurrentSyncData->wSoundID + 65536);
			else
				soundEndTime = sound->Play(pCurrentSyncData->wSoundID);
#else
			soundEndTime = sound->Play(pCurrentSyncData->wSoundID);
#endif
			}
		else
			{
			soundEndTime = game->sound2->Play(pCurrentSyncData->wSoundID);
			}
		soundEndTime += game->GetTrueTime();
		bIStartedASound = TRUE;
		}
	else
		{
		//DebugString("Trying to sync to sound 0, 1, or -1.\n");
		}
	itsStartTime = game->GetTrueTime();
	itsPreviousSyncTime = itsStartTime - 1;
	itsCurrentSyncNumber = 0;
	iCurrentFrame = pCurrentSyncData->rsFrames[itsCurrentSyncNumber].wFrameNumber;
	if (iCurrentFrame == -1)
		iCurrentFrame = itsRestImageNumber;
	client->SetCel(iCurrentFrame);
	}

void sciRailCycle::EndSync()
	{
	soundEndTime = 0;

	if (!IsSyncing())
		return;
//	if (pCurrentSyncData && pCurrentSyncData->wSoundID)
//		pApp->pSounder->EndSound();
	iCurrentFrame = itsRestImageNumber;
	client->SetCel(iCurrentFrame);
	itsNumQueuedSyncs--;
	delete[] itsDirectSyncData[itsHead];
	itsDirectSyncData[itsHead] = NULL;
	itsHead = (itsHead+1) % (int)kSyncQueueLength;
	itsStartTime = 0;
	pCurrentSyncData = NULL;
	bIStartedASound = FALSE;

	if (itsNumQueuedSyncs == 0)
		{
		}
	}

void sciRailCycle::Doit()
	{
	Boolean bFinished;
	Boolean bNeedsUpdating;               
	int dwTickCount;
	int sCurrentDuration;
	// int currPos;
#ifdef SHOW_SYNC_DETAILS
	char str[64];
#endif // SHOW_SYNC_DETAILS
	bFinished = FALSE;
	bNeedsUpdating = FALSE;
	if ((itsStartTime == 0) && IsSyncing())
		BeginSync();

	// Adjust for sounds that aren't immediately audible.
/*	if (sound)
		currPos = sound->GetPositionInTicks();
	else
		currPos = SOUND_NOT_STARTED_YET;
	if ((currPos == SOUND_NOT_STARTED_YET) && itsStartTime)
		{
		itsStartTime = game->GetTrueTime();
		itsPreviousSyncTime = itsStartTime - 1;
		return;
		}
*/
	if (itsStartTime > 0)
		{
		// Advance sync number until it is current.
		dwTickCount = game->GetTrueTime();
		if (pCurrentSyncData->wNumSyncFrames)
			sCurrentDuration = pCurrentSyncData->rsFrames[itsCurrentSyncNumber].wDuration;
		else
			sCurrentDuration = 1;
		while (!bFinished &&
					dwTickCount > itsPreviousSyncTime + sCurrentDuration)
			{
			bNeedsUpdating = TRUE;
			++itsCurrentSyncNumber;
			if (itsCurrentSyncNumber >= pCurrentSyncData->wNumSyncFrames)
				{
				// This sync is finished. End it and start another if one is queued.
				// If the sound is still playing, hold off.
				if (soundEndTime > game->GetTrueTime())
					{
					dwTickCount = 0;
					bNeedsUpdating = FALSE;
					client->SetCel(itsRestImageNumber);
					bIStartedASound = FALSE;
					itsCurrentSyncNumber = pCurrentSyncData->wNumSyncFrames - 1;
					}
				else
					{
					bFinished = TRUE;
					EndSync();
					if (IsSyncing())
						BeginSync();
					}
				}
			else
				{
				itsPreviousSyncTime += sCurrentDuration;
				if (pCurrentSyncData->wNumSyncFrames)
					sCurrentDuration = pCurrentSyncData->rsFrames[itsCurrentSyncNumber].wDuration;
				else
					sCurrentDuration = 1;
				}
			}
		if (bNeedsUpdating)
			{
			if (pCurrentSyncData)
				{
				iCurrentFrame = pCurrentSyncData->rsFrames[itsCurrentSyncNumber].wFrameNumber;
				if (iCurrentFrame == -1)
					iCurrentFrame = itsRestImageNumber;
				client->SetCel(iCurrentFrame);
#ifdef SHOW_SYNC_DETAILS
				sprintf(str, "Doing sync frame #%d for %d ticks.\n", iCurrentFrame, sCurrentDuration);
				DebugString(str);
#endif // SHOW_SYNC_DETAILS
				}
			}
		}
	if (!IsSyncing())
		CycleDone();
	}

Boolean sciRailCycle::IsSyncing()
	{
 	return (itsNumQueuedSyncs > 0) || (bIStartedASound && pApp->pSounder->IsSoundPlaying());
	}

Boolean sciRailCycle::IsSyncQueueFull()
	{
 	return (itsNumQueuedSyncs == kSyncQueueLength);
	}

void sciRailCycle::SetRestImage(int theRestImage)
	{
 	itsRestImageNumber = theRestImage;
	}

void sciRailCycle::StopSyncing()
	{
	if (IsSyncing())
		{
		if (bIStartedASound)
			{
			if (sound)
				sound->Stop();
			}
		bIStartedASound = FALSE;
		while (IsSyncing())
			EndSync();
		CycleDone();
		}
	}

void sciRailCycle::SetSound(sciSound *newSound)
	{
	sound = newSound;
	}

int sciRailCycle::GetNumSyncs(int iResourceID, ResourceManager *newResman)
	{
	Resource *pResource;
	WORD *pRailSyncGroup;
	int i;
	ResourceManager *resman;

	if (newResman == NULL)
		{
		if (client && client->resourceManager)
			resman = client->resourceManager;
		else
			resman = pApp->pResourceManager;
		}
	else
		{
		resman = newResman;
		}
	pResource = resman->LoadResource(RES_SYNC, iResourceID);
	if (pResource)
		{
		pRailSyncGroup = (WORD *)(pResource->GetData());
		if (pRailSyncGroup)
			i = *pRailSyncGroup;
		else
			i = 0;
		resman->FreeResource(pResource);
		}
	else
		{
		i = 0;
		}

	return i;
	}
