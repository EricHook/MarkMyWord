#include "os.h"
#include "scisound.h"
#include "sciscrip.h"
#include "app.h"
#include "sounder.h"
#include "game.h"

extern Application *pApp;
extern Game *game;

sciSound::sciSound(ResourceManager *newResourceManager)
	{
	volume = 127;
	number = -1;
	talkingSound = FALSE;
	if (newResourceManager)
		resourceManager = newResourceManager;
	else
		resourceManager = pApp->pResourceManager;
	loop = 0;
	handle = 0;
	pan = -1;
	}

sciSound::~sciSound()
	{
	}

int sciSound::Play(int newNumber, int volume, sciScript *whoToCue, int newPan)
	{
	int t = 0;
	BOOL important;

	if (newNumber != -1)
		SetNumber(newNumber);
	if (volume == -1)
		volume = 127;
	if (newPan != -1)
		SetPan(newPan);
	if (game->IsSoundEffects() || talkingSound)
		{
		if (handle)
			{
			pApp->pSounder->EndSoundHandle(handle);
			}
#ifdef _DEBUG
		char buff[250];
		wsprintf(buff, "\nPlaying sound %d.\n", newNumber);
		DebugString(buff);
#endif
		if (talkingSound)
			important = TRUE;
		else
			important = FALSE;
		t = pApp->pSounder->Play(number, resourceManager, loop, &handle, -1, pan, important);
		}
	if (whoToCue)
		whoToCue->SetTicks(t + (game->GetTrueTime() - game->GetTime()));

	// Turn looping back off.
	loop = 0;

	return t;
	}

void sciSound::SetNumber(int newNumber)
	{
	number = newNumber;
	}

void sciSound::Stop()
	{
	if (handle && pApp && pApp->pSounder)
		{
		pApp->pSounder->EndSoundHandle(handle);
		handle = 0;
		}
	}

void sciSound::SetLoop(int newLoop)
	{
	loop = newLoop;
	}

int sciSound::GetPositionInTicks()
	{
	if (handle)
		return pApp->pSounder->GetPositionInTicks(handle);
	else
		return -1;
	}

void sciSound::SetPan(int newPan)
	{
	pan = newPan;
	}
