#ifndef _clientinclude
#define _clientinclude

#include "miniwin.h"
#include "version.h"
#include "wonplaydll.h"

#ifdef HOGS_BACKGAMMON
	#include "Backgammon Client.h"
#endif

#ifdef HOGS_BLACKJACK
	#include "Blackjack Client.h"
#endif

#ifdef HOGS_BRIDGE
	#include "Bridge Client.h"
#endif

#ifdef HOGS_CRAPS
	#include "Craps Client.h"
#endif

#ifdef HOGS_CHECKERS
	#include "Checkers Client.h"
#endif

#ifdef HOGS_CHESS
	#include "Chess Client.h"
#endif

#ifdef HOGS_CRIBBAGE
	#include "Cribbage Client.h"
#endif

#ifdef HOGS_CROSSWORDS
	#include "Crosswords Client.h"
#endif

#ifdef HOGS_DOMINOES
	#include "Dominoes Client.h"
#endif

#ifdef HOGS_DOUBLECROSS
	#include "DoubleCross Client.h"
#endif

#ifdef HOGS_EUCHRE
	#include "Euchre Client.h"
#endif

#ifdef HOGS_GIN
	#include "Gin Client.h"
#endif

#ifdef HOGS_HANGMAN
	#include "Hangman Client.h"
#endif

#ifdef HOGS_HEARTS
	#include "Hearts Client.h"
#endif

#ifdef HOGS_PACHISI
	#include "Pachisi Client.h"
#endif

#ifdef HOGS_PAIGOW
	#include "Paigow Client.h"
#endif

#ifdef HOGS_PINOCHLE
	#include "Pinochle Client.h"
#endif

#ifdef HOGS_POKER
	#include "Poker Client.h"
#endif

#ifdef HOGS_REVERSI
	#include "Reversi Client.h"
#endif

#ifdef HOGS_ROULETTE
	#include "Roulette Client.h"
#endif

#ifdef HOGS_SPADES
	#include "Spades Client.h"
#endif

#ifdef HOGS_WORDSEARCH
	#include "Wordsearch Client.h"
#endif

#ifdef HOGS_WORDOX
	#include "Wordox Client.h"
#endif

#ifdef HOGS_WORDYACHT
	#include "WordYacht Client.h"
#endif

#ifdef HOGS_YACHT
	#include "Yacht Client.h"
#endif

typedef struct WONParams {
	// from command line
	char wonPlayRoomGUID[WONplayDLL::nWON_ROOM_ID];
	char wonPlayDLLPath[MAX_PATH];
	char mapINIPath[MAX_PATH];
	
	// from WONplayDLL
	char wonPath[MAX_PATH];
	HWND wonPlayHWnd;
	char userName[MAX_PLAYER_NAME];
	int databaseID;
	char roomName[MAX_LONG_ROOM_NAME];
	DWORD roomID;
	char serverLocation[64];
	int serverPort;
	char exePath[MAX_PATH];
	SNMachineID myMachineID;
	WONParams() : myMachineID('PC 1') { }
	
	// from room server
	int adChangeRateInSeconds;
	char adTags[256];
	char sponsorURL[MAX_ROOM_NAME];
} WONParams;

extern WONplayDLL::WON *won;
extern WONParams wonParams;

bool InitClientObject();

#endif