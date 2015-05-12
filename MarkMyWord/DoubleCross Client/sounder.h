#ifndef _sounder
#define _sounder

#include "vsounder.h"
#include "scisong.h"

// #include "sos.h"

class Sounder : public VSounder
	{
	public:
		Sounder();
		~Sounder();
		void EndSound(int index = -1, BOOL endNow=TRUE);	// index = index into playingSounds[];
		void TurnSoundOff();
		void TurnSoundOn();
		void Doit();
		int GetPositionInTicks(int handle);
		/*
		void PlaySong(sciSong *newSong);
		void PlaySong(int songID);
		void PumpSong(PSOSSTREAMELEMENT psStream);
		void EndSong();
		void SetSongSpeed(double timesRate);
		double GetSongSpeed();
		void IncreaseSongSpeed(double timesRate);
		sciSong *GetSong() {return playingSong;}
		*/

	protected:
		void PlayItNow(int index, int *outHandle);	// index = index into playingSounds[];
		void PlayWave(Sound *playingSound);
		void EndWave(Sound *playingSound);

		WAVEHDR		whWaveHdr;
		LPSTR		lpWaveData;
		HWAVEOUT	hWaveOut;
		BOOL		headerPrepared;
		BOOL		soundOn;
		// HSOS		hsos;
		BOOL		driverAcquired;
		int			prevEndTime;
		sciSong		*playingSong;
		int			curSongPosition;
		double		curSongSpeed;
	};

#endif // _sounder
