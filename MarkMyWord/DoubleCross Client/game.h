#if !defined(_game)
#define _game

#include "scigame.h"
#include "scisound.h"
#include "rooms.h"
#include "game.h"

/////////////////////////////////////////////////
/////										/////
/////				DEFINES					/////
/////										/////
/////////////////////////////////////////////////

#define MAX_NUM_MULTIPLAYERS 8

enum { PLAYERSIZE_TINY, PLAYERSIZE_SMALL, PLAYERSIZE_LARGE };
enum { CASINO_HUMAN, CASINO_AI, CASINO_NONE };

/////////////////////////////////////////////////
/////										/////
/////				STRUCTS					/////
/////										/////
/////////////////////////////////////////////////

typedef struct {
	char commonSettings[4096];		//make sure this is BIGGER than the sizeof CommonSettings or ELSE dammit!!!
	char gameSpecificSettings[ROOM_LASTGAME][256];
} GameSettings;

typedef struct {
	int dollars;
	char cents;
} CasinoMoney;

class BoardCharacter;

typedef struct {
	//player info
	char type;
	char index;
	char loginName[16];
	char fileName[13];
	BoardCharacter *character;
	int charNum;
	char gender;
	CasinoMoney bank;
	char cardNumber[24];
	char tookLoan;
	char creditLevel;
	
	//global info
	char animations;
	char backgroundMusic;
	char soundEffects;
	char speech;
	char speed;
	double gamma;
	char attitude;

	//casino specific stuff
	char tutorialMode[8];
	char practiceMode[8];
	char numOpponents;
	//end casino specific stuff
} CommonSettings;

typedef struct {
	int minBet;
	int maxBet;
} CasinoTableLimits;


/////////////////////////////////////////////////
/////										/////
/////				CLASSES					/////
/////										/////
/////////////////////////////////////////////////

class BoardCharacter;

class Game : public sciGame
	{
	public:
		Game();
		~Game();
		
		void Doit();
		void SetNextRoom(int newRoomNum);
		void GetGameName(int roomNumber, char *name);
		BOOL InAGameRoom();
		BOOL IsCurRoom(int aRoomNum);
		BOOL LeftAGameRoom();
		BOOL QuitGame(Boolean iReallyMeanIt = FALSE);

		void DemoMessages(int msgNum);	//for DEMO only
		
		//Game settings functions
		void InitSettings(int index = 0, GameSettings *gSettings = NULL);
		void InitCommonSettings(int index = 0, GameSettings *gSettings = NULL);
		void InitGameSpecificSettings(int index = 0, GameSettings *gSettings = NULL);
		void ClearGameSpecificStatistics(int roomNum, char *gsSettings);// int index = 0);
		void ClearSettingsForRoomChange();
		
		BOOL IsMale(int gender);
		BOOL IsFemale(int gender);

		CommonSettings *GetPlayerSettings(int index = 0);
		CommonSettings *GetPlayerSettings(char *name, BOOL human = FALSE);
		CommonSettings *GetPlayerSettings(BoardCharacter *c);
		char *GetPlayerGameSettings(int roomNum, int index = 0);
		char *GetPlayerGameSettings(int roomNum, char *name, BOOL human = TRUE);
		char *GetPlayerGameSettings(int roomNum, BoardCharacter *c);

		int GetPlayerIndex(char *name, BOOL human = FALSE);
		int GetPlayerIndex(BoardCharacter *c);

		BOOL SaveSettings(GameSettings *gSettings, char *fileName);
		BOOL GetSettings(GameSettings *gSettings, char *fileName);

		void CopySettingsIfSamePlayer(int index1, int index2, GameSettings *tempPlayers);
		BOOL SavePlayerSettings(int index = 0);
		BOOL SavePlayerSettings(char *name);
		BOOL SavePlayerSettings(BoardCharacter *c);

		BOOL IsSignedIn(char *name, BOOL human = TRUE);
		void GetFileName(char *name, char *fileName);

		//returns the hosts' preferences for these options
		BOOL IsAnimations();
		BOOL IsBackgroundMusic();
		BOOL IsSoundEffects();
		BOOL IsChatSoundEffects();
		BOOL IsChatFiltered();
		BOOL IsTutorialMode();
		BOOL IsPracticeMode();
		int GetChatDisplay();
		int GetMoveSpeed();
		int GetAttitude();
		int GetSpeed(int option);
		double GetGamma();
		int GetNumOpponents();
		int GetGender();
		
		void SaveBank(char *name, BOOL human, CasinoMoney &aBank, 
						BOOL forKeno = FALSE, BOOL forTourn = FALSE, BOOL forLoan = FALSE);
		void GetBank(char *name, BOOL human, CasinoMoney &aBank);
		void SaveStats(int index, CasinoMoney &oldBank, CasinoMoney &newBank, int gameRoom);
		BOOL ResetCasinoPlayer(char *name, BOOL forceReset = FALSE);
		
		CasinoMoney GetInitialBank();
		BOOL OverCreditLimit(int index);
		void GiveLoan(int index);
		BOOL RepayLoan(int index, BOOL forceRepay = FALSE);
		BOOL AwardNextCreditCard(int index);


		int GetLoanAmount(char *name);
		void SetMinTableBet(int newMin);
		short GetMinTableBet();
		void GetTableLimits(int *min, int *max);
		
		//Character information
		int GetPlayerViewNumber(int playerNumber, int playerSize, Boolean facingWest = FALSE);
		int GetPlayerViewCel(int playerNumber, int playerSize, Boolean facingWest = FALSE);
		void GetCharacterNameByNumber(int characterNumber, char *name);
		int GetCharacterNumberByName(char *name);
		BoardCharacter *FindCharacter(int n);
		
		//Multiplayer functions

		void PlayerCounts(int whichGame, int *max, int *min, int *maxHumans);
		int GetNumSeatsPlayerIsIn(char *name);
		int GetNumPlayers(BOOL humans = TRUE);
		int GetNumDifferentHumansPlaying();
		void ClearAllButHost();
		BOOL TooManyLogins(int newRoomNum);
		BOOL HumansOnlyGame(int roomNum);
		
		GameSettings players[MAX_NUM_MULTIPLAYERS];
		
		// a funciton to keep menu information out of window.cpp
		void CheckFillScreenMenuItem(Boolean checked);
		
		int tableMin;
		int slotMachineID;
		int slotMachineValue; 
		BOOL bGrandPrizeMachine;	//only TRUE if the machine is playing for a vehicle(grand prize slots)
		int vpMachineID;
		int vpMachineValue;
		BOOL inATournament;
		
	protected:
		virtual void LoadTheRoom(int roomNum);
	};

#endif // _game
