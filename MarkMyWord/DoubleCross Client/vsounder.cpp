#include "os.h"

#include "vsounder.h"
#include "sound.h"
#include "app.h"
#include "inifile.h"
#include "game.h"
#include "resman.h"

#include <string.h>

extern Game *game;
extern Application *pApp;

VSounder::VSounder()
	{
	char buffer[16];
	int i;

	pApp->pIniFile->ReadString("Sound", "On", buffer, sizeof(buffer));
	if (stricmp(buffer, "On"))
		playSounds = FALSE;
	else
		playSounds = TRUE;
	for (i = 0; i < MAX_SOUNDS; ++i)
		{
		playingSounds[i] = NULL;
		playingSoundNums[i] = 0;
		playingSoundHandles[i] = 0;
		}
	nextHandle = 1;
	}

VSounder::~VSounder()
	{
	}

int VSounder::Play(int resourceID, ResourceManager *resourceManager,
				   int newLoop, int *handle, int volume, int pan, int important)
	{
	Sound *newSound;
	int i;

	if (!playSounds)
		return 0;

	if (resourceID == 0)
		return 0;

	if (resourceManager == NULL)
		resourceManager = pApp->pResourceManager;
	if (!resourceManager->ResourceIsAvailable(RES_SOUND, resourceID))
		return 0;
	newSound = (Sound*)resourceManager->LoadResource(RES_SOUND, resourceID);
	if (newSound && (newSound->wfFormat != WF_NONE))
		{
		newSound->important = important;
		i = FindSound(NULL);
		if (i == -1)
			{
			// Error, too many sounds playing.
			newSound->Free();
			return 0;
			}
		else
			{
			newSound->volume = volume;
			newSound->pan = pan;
			playingSounds[i] = newSound;
			playingSounds[i]->loop = newLoop;
			PlayItNow(i, handle);
			if (playingSounds[i])
				return playingSounds[i]->itsEndTime - game->GetTrueTime();
			else
				return 0;
			}
		}
	else
		{
		if (newSound)
			newSound->Free();
		return 0;
		}
    }

BOOL VSounder::IsSoundPlaying()
	{
	Sound *playingSound;

	// What sound to check???
	playingSound = playingSounds[0];
	if (playingSound != NULL)
		{
		if ((playingSound->itsEndTime == -1) || (playingSound->itsEndTime >= game->GetTrueTime()))
			return TRUE;
		}
	return FALSE;
	}

int VSounder::FindSound(Sound *s)
	{
	int i;

	i = 0;
	while ((playingSounds[i] != s) && (i < MAX_SOUNDS))
		{
		++i;
		}
	if ((i < MAX_SOUNDS) && (playingSounds[i] == s))
		return i;
	else
		return -1;
	}

void VSounder::EndSoundNum(int s, BOOL endNow)
	{
	int i;

	i = 0;
	while ((playingSoundNums[i] != s) && (i < MAX_SOUNDS))
		{
		++i;
		}
	if ((i < MAX_SOUNDS) && (playingSoundNums[i] == s))
		EndSound(i, endNow);
	}

void VSounder::EndSoundHandle(int h, BOOL endNow)
	{
	int i;

	i = 0;
	while ((playingSoundHandles[i] != h) && (i < MAX_SOUNDS))
		{
		++i;
		}
	if ((i < MAX_SOUNDS) && (playingSoundHandles[i] == h))
		EndSound(i, endNow);
	}
