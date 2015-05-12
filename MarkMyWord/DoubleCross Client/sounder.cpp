#include "os.h"
#include "sounder.h"
#include "sound.h"
#include "inifile.h"
#include "game.h"
#include "resman.h"
#include "app.h"
#include "window.h"
#include "unihand.h"
#include "sciplane.h"

#if defined (HOYLE_BOARD_GAMES)
#include "prgame.h"
#endif

extern Game *game;
extern Application *pApp;
extern BOOL win32s;
extern BYTE btSoundBitSupport;
extern int sosezSampleSize;
extern int sosezChannels;

DWORD sosError;
BOOL useWaveOut;

#define WAVE_HEADER_SIZE 0

/*
void sosTestError(DWORD e)
	{
	if (e != _ERR_NO_ERROR)
		{
		DebugString("SOS Error: ");
		DebugString(sosGetErrorString(e));
		DebugString("\n");
		//MessageBox(NULL, sosGetErrorString(e), "SOS Error", MB_OK);
		}
	}
*/

void Message(char *str)
	{
	MessageBoxA(NULL, str, "SOS", MB_OK);
	}

Sounder::Sounder()
	{
	// BOOL useDirectSound;

	hWaveOut = NULL;
	TurnSoundOn();

	/*
	if ((btSoundBitSupport == 8) || win32s)
		{
		sosezSampleSize = _SOS_8_BIT;
		sosezChannels = _SOS_MONO;
		}
	if (win32s)
		useWaveOut = TRUE;
	else
		useWaveOut = pApp->pIniFile->ReadValue("UseWaveOut", FALSE);
#if defined(HIP2) || defined(SIGS_DEMO)
	useWaveOut = TRUE;
#endif
	*/
	useWaveOut = TRUE;
	
	if (!useWaveOut)
		{
		/*
		sosError = sosInitDLL();
		if (sosError == _ERR_NO_ERROR)
			{
			useDirectSound = pApp->pIniFile->ReadValue("UseDirectSound", FALSE);
			if (useDirectSound)
				sosError = sosEZInitSystem( _SOS_AUTO, ( WORD )-1, MAIN_HWND, MAIN_HWND);
			else
				sosError = sosEZInitSystem( _SOS_MME, ( WORD )-1, MAIN_HWND, MAIN_HWND);
			sosTestError(sosError);
			hsos = sosEZGetDIGIHandle();
			sosError = sosDIGIReleaseDriver(hsos, 0);
			sosTestError(sosError);
			driverAcquired = FALSE;
			}
		else
			{
			useWaveOut = TRUE;
			}
		*/
		}
	prevEndTime = 0;
	playingSong = NULL;
	curSongSpeed = 1.0;
	}

Sounder::~Sounder()
	{
	TurnSoundOff();
	if (!useWaveOut)
		{
		/*
		if (!driverAcquired)
			{
			sosError = sosDIGIAcquireDriver(hsos);
			sosTestError(sosError);
			}
		sosError = sosEZUnInitSystem();
		sosTestError(sosError);
		sosError = sosUnInitDLL();
		sosTestError(sosError);
		*/
		}
	}

void Sounder::PlayItNow(int index, int *outHandle)
	{
	Sound *playingSound;
	// HSOS hsosPlayingSample = 0;
	// _SOS_SAMPLE sosSample;
	// int nBlockAlign;
	// int wBitsPerSample;
	// int nChannels;
	int i;
	Sound *prevSound;

	playingSound = playingSounds[index];

	if (!soundOn)
		{
		playingSounds[index] = NULL;
 		i = FindSound(playingSound);
		if (i == -1)
			playingSound->Free();
		return;
		}

	if (useWaveOut)
		{
		// Only one at a time, end all others first.
		// Don't end important sounds for non-important ones.
		prevSound = NULL;
		for (i = 0; i < MAX_SOUNDS; ++i)
			if (playingSounds[i] && playingSounds[i]->important)
				prevSound = playingSounds[i];
		if (!playingSound->important && prevSound && prevSound->important)
			{
			// Skip it.
			playingSounds[index] = NULL;
 			i = FindSound(playingSound);
			if (i == -1)
				playingSound->Free();
			return;
			}
		else
			{
			EndSound(-1, TRUE);
			PlayWave(playingSound);
			playingSounds[index] = playingSound;
			}
		}
	else
		{
		/*
		// Acquire the sound card.
		if (!driverAcquired)
			{
			sosError = sosDIGIAcquireDriver(hsos);
			sosTestError(sosError);
			if (sosError == _ERR_NO_ERROR)
				{
				driverAcquired = TRUE;
				}
			else
				{
				playingSounds[index] = NULL;
 				i = FindSound(playingSound);
				if (i == -1)
					playingSound->Free();
				return;
				}
			}

		if (playingSound->pSamples)
			{
			lpWaveData = LPSTR(playingSound->pSamples->Lock());
			}
		else
			{
			DebugString("No Samples.\n");
			playingSounds[index] = NULL;
 			i = FindSound(playingSound);
			if (i == -1)
				playingSound->Free();
			return;
			}

		switch (playingSound->wfFormat)
			{
			case WF_8BITMONO:
				wBitsPerSample = 8;
				nChannels = 1;
				nBlockAlign = 1;
				break;
			case WF_8BITSTEREO:
				wBitsPerSample = 8;
				nChannels = 2;
				nBlockAlign = 2;
				break;
			case WF_16BITMONO:
				wBitsPerSample = 16;
				nChannels = 1;
				nBlockAlign = 2;
				break;
			case WF_16BITSTEREO:
				wBitsPerSample = 16;
				nChannels = 2;
				nBlockAlign = 4;
				break;
			}
                                                            
		memset(&sosSample, 0, sizeof(sosSample));
		sosSample.pSample = lpWaveData;
		sosSample.lp32Sample = NULL;
		sosSample.dwLength = nBlockAlign * playingSound->dwNumOfSamples;
		sosSample.dwVolume = _VOLUME_MAX;
		sosSample.dwLoopCount = playingSound->loop;
		sosSample.dwRate = playingSound->wSamplesPerSec;
		if (wBitsPerSample == 8)
			{
			sosSample.dwBitsPerSample = _PCM_8_BIT;
			sosSample.dwFormat = _PCM_UNSIGNED;
			}
		else
			{
			sosSample.dwBitsPerSample = _PCM_16_BIT;
			sosSample.dwFormat = 0;
			}
		sosSample.dwChannels = nChannels;

		if (playingSound->pan == -1)
			sosSample.dwPanPosition = _PAN_CENTER;
		else
			sosSample.dwPanPosition = playingSound->pan;

		sosError = sosPrepare32Memory( sosSample.pSample, sosSample.dwLength, &(sosSample.lp32Sample) );
		if (sosSample.lp32Sample == NULL)
			sosTestError(!_ERR_NO_ERROR);
		sosTestError(sosError);

		// Start the sound.
		sosError =sosDIGIStartSample(hsos, &sosSample, &hsosPlayingSample);
		sosTestError(sosError);
		//Message("Wait for sound to finish");
		*/
		}

	playingSoundNums[index] = 0; // hsosPlayingSample
	playingSoundHandles[index] = nextHandle;
	if (outHandle)
		*outHandle = nextHandle;
	++nextHandle;

	// Update endtime.
	if (playingSound->loop == -1)
		{
		// forever
		playingSound->itsEndTime = -1;
		}
	else
		{
		playingSound->itsEndTime = game->GetTrueTime();
		playingSound->itsEndTime += (playingSound->loop+1) * (int)((((double)playingSound->dwNumOfSamples) / playingSound->wSamplesPerSec) * 60 + 15);
		}
	}

void Sounder::EndSound(int index, BOOL endNow)
	{
	int i;
	Sound *playingSound;
	// char str[64];
	// PSOSSAMPLE ps;

	//Message("EndSound");
	// sprintf(str, "EndSound %d\n", index);
	// DebugString(str);

 	if (index == -1)
		{
		// end all sounds
		for (i = 0; i < MAX_SOUNDS; ++i)
			EndSound(i);
		return;
		}

	if (playingSounds[index] == NULL)
		return;

	playingSound = playingSounds[index];
	if (useWaveOut)
		{
		EndWave(playingSound);
		}
	else
		{
		/*
		ps = sosDIGIGetSamplePointer(hsos, playingSoundNums[index]);
		if (ps)
			{
			sosError = sosUnPrepare32Memory(ps->lp32Sample);
			sosTestError(sosError);
			}
		if (endNow)
			{
			sosError = sosDIGIStopSample(hsos, playingSoundNums[index]);
			sosTestError(sosError);
			}
		*/
		}
	playingSounds[index] = NULL;
	playingSoundNums[index] = 0;
	playingSoundHandles[index] = 0;
 	i = FindSound(playingSound);
	if (i == -1)
		playingSound->Free();
	prevEndTime = game->GetTime();
	//Message("End EndSound");
	}

void Sounder::TurnSoundOn()
	{
	soundOn = TRUE;
	}

void Sounder::TurnSoundOff()
	{
	soundOn = FALSE;
	EndSound(-1);
	}

void Sounder::Doit()
	{
	int i;
	// int soundIsPlaying;
	// MSG msg;

	if (useWaveOut)
		{
		// Clean up any finished sounds.
		for (i = 0; i < MAX_SOUNDS; ++i)
			if (playingSounds[i])
				{
				if (GetPositionInTicks(playingSoundHandles[i]) == SOUND_ENDED)
					EndSound(i, TRUE);
				}
		return;
		}

	/*
	sosDIGIUserService( hsos );
	if ( (PeekMessage(&msg, NULL, _SOS_COMMAND, _SOS_COMMAND, PM_REMOVE))
		 )
		{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		}

	// If there are no sounds playing, release the sound card.
	soundIsPlaying = FALSE;
	for (i = 0; i < MAX_SOUNDS; ++i)
		if (playingSounds[i])
			soundIsPlaying = TRUE;
	if (soundIsPlaying)
		return;

	//Message("Start real Doit");
	if (driverAcquired)
		{
		if ((game->GetTime() > (prevEndTime + 300)) && !playingSong && !win32s)
			{
			sosError = sosDIGIReleaseDriver(hsos, 0);
			sosTestError(sosError);
			driverAcquired = FALSE;
			}
		}
	//Message("End real Doit");
	*/
	}

int Sounder::GetPositionInTicks(int h)
	{
	int i;
	int b;
	int rate;
	int ticksPerSecond;
	int bytesPerSample;
	MMTIME mmTime;

	//Message("Start GetPositionInTicks");

	i = 0;
	while ((playingSoundHandles[i] != h) && (i < MAX_SOUNDS))
		{
		++i;
		}
	if ((i < MAX_SOUNDS) && (playingSoundHandles[i] == h))
		{
		//rate = sosDIGIGetSampleRate(hsos, playingSoundNums[i]);
		rate = 22050;
		if (useWaveOut)
			{
			mmTime.wType = TIME_BYTES;
			waveOutGetPosition(hWaveOut, &mmTime, sizeof(mmTime));
			b = mmTime.u.cb;
			if (game->GetTrueTime() > playingSounds[i]->itsEndTime)
				return SOUND_ENDED;
			}
		else
			{
			// b = sosDIGIGetBytesProcessed(hsos, playingSoundNums[i]);
			}

		if ((b <= 0) || (rate <= 0))
			return SOUND_NOT_STARTED_YET;
		ticksPerSecond = 60;
		bytesPerSample = 4;
		//Message("End GetPositionInTicks");
		return (int)(((double)b)*ticksPerSecond/rate/bytesPerSample);
		}
	else
		{
		//Message("End GetPositionInTicks");
		return SOUND_ENDED;
		}
	}

// STREAMING SYSTEM
//
// maximum number of streams that can be active
#define _MAX_STREAMS           0x20

// number of stream buffers to use
#define _MAX_STREAM_BUFFERS    0x08

// size of each stream buffer
#define _STREAM_BUFFER_SIZE    0x10000L

// handles for the stream memory
static HGLOBAL hMemory[ _MAX_STREAMS ][ _MAX_STREAM_BUFFERS ];

// handle for the stream files
static HFILE hFile[ _MAX_STREAMS ]; 

// stream initialization structure
// static _SOS_STREAM_INIT sStreamInit;
 
// stream element structures for all of the stream buffers
// static _SOS_STREAM_ELEMENT sStreamBuffers[ _MAX_STREAMS ][ _MAX_STREAM_BUFFERS ];

// static HSOS hStream = 0;

/*
void Sounder::PlaySong(sciSong *newSong)
	{
	// FILE *file;
	// int offset;
	// int length;
	// HSOS hDIGIDriver;

	if (useWaveOut)
		return;

	if (playingSong)
		EndSong();
	playingSong = newSong;
	if (newSong == NULL)
		playingSong = NULL;
	if (newSong && !newSong->file)
		playingSong = NULL;
	if (playingSong)
		{
		file = playingSong->file;
		offset = playingSong->offset + WAVE_HEADER_SIZE;
		length = playingSong->length;
		WORD  wIndex;

		// Acquire the sound card.
		if (!driverAcquired)
			{
			sosError = sosDIGIAcquireDriver(hsos);
			if (sosError == _ERR_NO_ERROR)
				{
				sosTestError(sosError);
				driverAcquired = TRUE;
				}
			else
				{
				// Error
				playingSong = NULL;
				return;
				}
			}

		hDIGIDriver = sosEZGetDIGIHandle();

		// setup the start sample structure that is to be used for 
		// the stream sample template
		sStreamInit.wFlags = 0x00;
		sStreamInit.hWnd = MAIN_HWND;
		sStreamInit.sSample.dwFlags = 0x00;
		sStreamInit.sSample.dwPanPosition = _PAN_CENTER;
		sStreamInit.sSample.dwBitsPerSample = 8;
		sStreamInit.sSample.dwChannels = 0x01;
		sStreamInit.sSample.dwFormat = _PCM_UNSIGNED;
		sStreamInit.sSample.dwRate = 22050;
		if (game->IsSpeech() && (game->GetCurrentRoomNum() != ROOM_CREDITS))
			sStreamInit.sSample.dwVolume = MK_VOLUME( 0x2fff, 0x2fff );
		else
			sStreamInit.sSample.dwVolume = MK_VOLUME( 0x6fff, 0x6fff );

		// attempt to acquire a stream to use
		sosError = sosDIGIStreamAcquire( hDIGIDriver, &sStreamInit, &hStream );
		sosTestError(sosError);

		curSongPosition = offset;
		fseek(file, curSongPosition, SEEK_SET);

		// initialize all of the stream buffers
		for( wIndex = 0x00; wIndex < _MAX_STREAM_BUFFERS; wIndex++ )
			{
			// allocate the memory for the stream element
			hMemory[ hStream ][ wIndex ] = GlobalAlloc( GPTR,
					_STREAM_BUFFER_SIZE);
			if( ! ( hMemory[ hStream ][ wIndex ]) )
				return;

			// lock the sample memory
			( PSTR )sStreamBuffers[ hStream ][ wIndex ].pData = ( PSTR )GlobalLock( 
				hMemory[ hStream ][ wIndex ] );

			// load the sample into memory
			sStreamBuffers[ hStream ][ wIndex ].dwSize = 0;
			sStreamBuffers[ hStream ][ wIndex ].dwSize = 
				fread(sStreamBuffers[ hStream ][ wIndex ].pData,
				1, _STREAM_BUFFER_SIZE, file);

			// Check for end of song.
			if ((curSongPosition + _STREAM_BUFFER_SIZE) >
				(offset + length))
				{
				sStreamBuffers[ hStream ][ wIndex ].dwSize =
					offset + length - curSongPosition;
				}
			curSongPosition += sStreamBuffers[ hStream ][ wIndex ].dwSize;
			}

		// loop through all of the samples, sending them to the streaming system
		for( wIndex = 0x00; wIndex < _MAX_STREAM_BUFFERS; wIndex++ )
		{
		  // verify that there is data to send to the streaming system
		  if( sStreamBuffers[ hStream ][ wIndex ].dwSize )
		  {
			// prepare the memory for use with the 16 bit dll
			sosError = sosPrepare32Memory( sStreamBuffers[ hStream ][ wIndex ].pData, 
				sStreamBuffers[ hStream ][ wIndex ].dwSize,
				&sStreamBuffers[ hStream ][ wIndex ].lp32Data );
			if (sStreamBuffers[ hStream ][ wIndex ].lp32Data == NULL)
				sosTestError(!_ERR_NO_ERROR);
			sosTestError(sosError);

			// prepare the memory for use with the 16 bit dll
			sosError = sosPrepare32Memory( ( PSTR )&sStreamBuffers[ hStream ][ wIndex ], 
				sizeof( _SOS_STREAM_ELEMENT ),
				&sStreamBuffers[ hStream ][ wIndex ].lp32Element );
			if (sStreamBuffers[ hStream ][ wIndex ].lp32Element == NULL)
				sosTestError(!_ERR_NO_ERROR);
			sosTestError(sosError);

			// prepare the buffer for the streaming system
			sosError = sosDIGIStreamPrepareBuffer( hDIGIDriver, hStream, 
				&sStreamBuffers[ hStream ][ wIndex ] );
			sosTestError(sosError);

			// add the buffer to the stream
			sosError = sosDIGIStreamAddBuffer( hDIGIDriver, hStream, 
				&sStreamBuffers[ hStream ][ wIndex ] );
			sosTestError(sosError);
		  }
		}

		// start the stream playing
		sosError = sosDIGIStreamStart( hDIGIDriver, hStream );
		sosTestError(sosError);
		}
	}
*/

/*
void Sounder::PumpSong(PSOSSTREAMELEMENT psStream)
	{
	if (!playingSong)
		return;

	// load the sample memory into the stream element memory
	fseek(playingSong->file, curSongPosition, SEEK_SET);
	psStream->dwSize = fread(psStream->pData, 1, _STREAM_BUFFER_SIZE, playingSong->file);

	// add the buffer to the streaming system if there is any data for 
	// the element
	if( psStream->dwSize == _STREAM_BUFFER_SIZE)
		{
		// A full buffer was read.
		// Not end of file. Maybe end of sound, though.
		if ((curSongPosition + _STREAM_BUFFER_SIZE) >
			(playingSong->offset + playingSong->length))
			{
			psStream->dwSize = playingSong->offset + playingSong->length - curSongPosition;
			curSongPosition = playingSong->offset + WAVE_HEADER_SIZE;
			DebugString("Starting next song loop.\n");
			}
		else
			{
			curSongPosition += _STREAM_BUFFER_SIZE;
			}
		}
	else
		{
		// A partial buffer was read.
		// End of file - loop.
		psStream->dwSize = playingSong->offset + playingSong->length - curSongPosition;
		curSongPosition = playingSong->offset + WAVE_HEADER_SIZE;
		DebugString("Starting next song loop.\n");
		}
	sosError = sosDIGIStreamAddBuffer( hsos, hStream, psStream );
	sosTestError(sosError);
	}
*/

/*
void Sounder::EndSong()
	{
	// WORD  wIndex;

	if (!playingSong)
		return;

	sosDIGIStreamStop( hsos, hStream );
	sosDIGIStreamRelease( hsos, hStream );
	// loop through all of the samples
	for( wIndex = 0x00; wIndex < _MAX_STREAM_BUFFERS; wIndex++ )
		{
		// unprepare the sample
		sosDIGIStreamUnPrepareBuffer( hsos, hStream, 
		 &sStreamBuffers[ hStream ][ wIndex ] );

		// unprepare the memory for use with the 16 bit dll
		sosUnPrepare32Memory( sStreamBuffers[ hStream ][ wIndex ].lp32Element ); 
		sosUnPrepare32Memory( sStreamBuffers[ hStream ][ wIndex ].lp32Data ); 

		// unlock the memory
		GlobalUnlock( hMemory[ hStream ][ wIndex ] );

		// free the memory
		GlobalFree( hMemory[ hStream ][ wIndex ] );
		}
	delete playingSong;
	playingSong = NULL;
	}
*/

/*
void Sounder::PlaySong(int songID)
	{
	sciSong *song;
	int i, numSongs = 0;
#ifdef HOYLE_BOARD_GAMES
	int musicNums[] = { 3503, 3504, 3505, 
						3508, 3509, 3510, 3506,
						3513, 3512, 3511, 3500, 0 };
#else
	int musicNums[] = { 3503, 3504, 3505, 
						3508, 3509, 3510, 3506,
						3513, 3512, 3511, 3500, 0 };
#endif
	if(songID == -1)//Random Song
	{
		for(i = 0; musicNums[i] != 0; i++)
			numSongs++;
#ifdef HOYLE_BOARD_GAMES
		songID = musicNums[random(numSongs)];
#else
		songID = musicNums[random(numSongs)];
#endif
	}

#if defined (HOYLE_BOARD_GAMES)
	if (game->GetCurrentRoomNum() == ROOM_PLACER && songID)
	{
		if (prGame && prGame->board[0]->bPause == FALSE)//prGame->bPreGame == FALSE)
			songID = PR_SONG;
		else
			songID = NULL;	//dont play song unless we're in the game
	}
#endif

	song = NULL;
	if (songID)
		{
		song = new sciSong(songID);
		}
	PlaySong(song);
	}
*/

void Sounder::PlayWave(Sound *playingSound)
	{
	WAVEFORMATEX wfWaveFormatEx;
	int iError;

	if (!soundOn)
		return;
	// Setup wfWaveFormat.
	wfWaveFormatEx.wFormatTag = WAVE_FORMAT_PCM;
	wfWaveFormatEx.nSamplesPerSec = playingSound->wSamplesPerSec;
	wfWaveFormatEx.cbSize = 0;
	switch (playingSound->wfFormat)
		{
		case WF_8BITMONO:
			wfWaveFormatEx.wBitsPerSample = 8;
			wfWaveFormatEx.nChannels = 1;
			wfWaveFormatEx.nBlockAlign = 1;
			break;
		case WF_8BITSTEREO:
			wfWaveFormatEx.wBitsPerSample = 8;
			wfWaveFormatEx.nChannels = 2;
			wfWaveFormatEx.nBlockAlign = 2;
			break;
		case WF_16BITMONO:
			wfWaveFormatEx.wBitsPerSample = 16;
			wfWaveFormatEx.nChannels = 1;
			wfWaveFormatEx.nBlockAlign = 2;
			break;
		case WF_16BITSTEREO:
			wfWaveFormatEx.wBitsPerSample = 16;
			wfWaveFormatEx.nChannels = 2;
			wfWaveFormatEx.nBlockAlign = 4;
			break;
		}
	wfWaveFormatEx.nAvgBytesPerSec =
			wfWaveFormatEx.nBlockAlign * wfWaveFormatEx.nSamplesPerSec;
                                                            
	if (playingSound->pSamples)
		{
		lpWaveData = LPSTR(playingSound->pSamples->Lock());
		}
	else
		{
		DebugString("No Samples.\n");
		//if (deleteSound)
		//	playingSound->Free();
		playingSound = NULL;
		return;
		}

	iError = waveOutOpen((LPHWAVEOUT)&hWaveOut, WAVE_MAPPER,
			&wfWaveFormatEx, 0L, 0L, CALLBACK_NULL);

	if (iError)
		{
		DebugString("waveOutOpen error.\n");
		EndWave(playingSound);
		return;    
		}

	whWaveHdr.lpData = (char *)lpWaveData;
	whWaveHdr.dwBufferLength = wfWaveFormatEx.nBlockAlign * playingSound->dwNumOfSamples;
	whWaveHdr.dwBytesRecorded = 0;
	whWaveHdr.dwUser = 0;
	whWaveHdr.lpNext = 0;
	whWaveHdr.reserved = 0;

	if (playingSound->loop)
		{
		whWaveHdr.dwFlags = WHDR_BEGINLOOP | WHDR_ENDLOOP;
		whWaveHdr.dwLoops = playingSound->loop;
		}
	else
		{
		whWaveHdr.dwFlags = 0L;
		whWaveHdr.dwLoops = 0L;
		}

	iError = waveOutPrepareHeader(hWaveOut, &whWaveHdr, sizeof(WAVEHDR));
	if (iError || !hWaveOut)
		{
		DebugString("waveOutPrepareHeader error.\n");
		headerPrepared = FALSE;
		EndWave(playingSound);
		return;    
		}
	headerPrepared = TRUE;

	iError = waveOutWrite(hWaveOut, &whWaveHdr, sizeof(WAVEHDR));
	if (iError || !hWaveOut)
		{
		DebugString("waveOutWrite error.\n");
		EndWave(playingSound);
		return;    
		}

	// Update endtime.
	if (playingSound->loop == -1)
		{
		// forever
		playingSound->itsEndTime = -1;
		}
	else
		{
		playingSound->itsEndTime = game->GetTrueTime();
		playingSound->itsEndTime += (playingSound->loop+1) * (int)((((double)playingSound->dwNumOfSamples) / playingSound->wSamplesPerSec) * 60 + 15);
		}
	}

void Sounder::EndWave(Sound *playingSound)
	{
	if (playingSound == NULL)
		return;

	if (hWaveOut)
		{
		waveOutReset(hWaveOut);
		if (headerPrepared)
			waveOutUnprepareHeader(hWaveOut, &whWaveHdr, sizeof(WAVEHDR));
		}
	playingSound->pSamples->Unlock();
	if (hWaveOut)
		{
		waveOutClose(hWaveOut);
		hWaveOut = NULL;
		}

	playingSound = NULL;
	}

/*
void Sounder::SetSongSpeed(double timesRate)
	{
	curSongSpeed = timesRate;
	// sosError = sosDIGISetSampleRate( hsos, hStream, WORD(curSongSpeed*22050));
	}

double Sounder::GetSongSpeed()
	{
	return curSongSpeed;
	}

void Sounder::IncreaseSongSpeed(double timesRate)
	{
	SetSongSpeed(curSongSpeed * timesRate);
	}
*/
