#ifndef _scigame
#define _scigame

#include "sciobj.h"
#include "scilist.h"

class sciEvent;
class sciScript;
class sciRoom;
class sciSound;
class sciFeature;

class sciGame : public sciObject
	{
	public:
		sciGame();
		~sciGame();
		Boolean HandleEvent(sciEvent &event);
		int GetTime();	// GetTime won't change during an animation cycle.
		int GetTrueTime();
		void Play();
		BOOL QuitGame(Boolean iReallyMeanIt = FALSE);
		void Init();
		virtual void Doit();
		void UpdateTime();
		void SetScript(sciScript *newScript);
		virtual void SetNextRoom(int newRoomNum);
		int GetNextRoom();
		int GetPreviousRoomNum();
		int GetCurrentRoomNum();
		void SetCurrentRoomNum(int newRoomNum);
		Boolean IsDone();
		void SetGameName(char *);
		void HandsOff();
		void HandsOn();
		
		sciRoom *curRoom;
		sciSound *sound1, *sound2;
		sciList incomingEvents;

	protected:
		void LoadRoom(int roomNum);
		virtual void LoadTheRoom(int roomNum);
		
		int prevDoit;
		int currDoit;
		int time;
		Boolean quit;
		sciScript *script;
		int nextRoomNum;
		int prevRoomNum;
		int curRoomNum;
		BOOL handsOn;
	};

#endif // _scigame
