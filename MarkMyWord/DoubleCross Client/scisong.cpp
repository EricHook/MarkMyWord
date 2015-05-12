#include "os.h"

#include "scisong.h"
#include "app.h"
#include "resman.h"
#include "sounder.h"

extern Application *pApp;
extern BYTE btSoundBitSupport;

sciSong::sciSong(int newResourceID, ResourceManager *newResourceManager)
	: Resource(RES_SOUND, newResourceID, newResourceManager, FALSE)
	{
	length -= 2;
	offset += 0x2c;
	length -= 0x2c;
	}

sciSong::~sciSong()
	{
	}

void sciSong::Play()
	{
	// pApp->pSounder->PlaySong(this);
	}
