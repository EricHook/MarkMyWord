#ifndef _RISMSG_H
#define _RISMSG_H

#include "hwmessage.h"
#include "DBMessages.h"
#include "SNMachineID.h"

// Room Info Server Messages
enum {
	MFS_RIS_ERROR = MB_ROOM_INFO_SERVER,

	// ISAPI web page query
	MTS_RIS_WWW_QUERY,
	MFS_RIS_WWW_QUERY,

	// ISAPI find player
	MTS_RIS_FIND_PLAYER,
	MFS_RIS_FIND_PLAYER,

	// WON Client login request
	MTS_RIS_LOGIN,
	MFS_RIS_LOGIN,

	// WON Account creation
	MTS_RIS_CREATE_ACCOUNT,
	
	// WON Client room resource query
	MTS_RIS_ROOM_QUERY,
	MFS_RIS_ROOM_QUERY,
	MFS_RIS_FILTER,
	MFS_RIS_FILE,

	// RIS looks for player in a RS
	MTS_RS_FIND_PLAYER,
	MFS_RS_FIND_PLAYER,

	// RIS asks RS how many players are there
	MTS_RS_NUMPLAYERS,
	MFS_RS_NUMPLAYERS,

	// allow clients to tell us to update the RS information
	MTS_RIS_UPDATE_RS_INFO,
	MFS_RIS_UPDATE_RS_INFO,

	// WON Client sends machine inventory
	MTS_RIS_MACHINE_INVENTORY,
	MFS_RIS_MACHINE_INVENTORY,

	// ISAPI reply one per room/node
	MFS_RIS_WWW_QUERY_ROOM,

	// WON Client asks for known cheat programs
	MTS_RIS_CHEAT_APP_QUERY,
	MFS_RIS_CHEAT_APP_QUERY,
	MFS_RIS_CHEAT_APP,

	MFS_RIS_FIND_PLAYER_HEADER,

	// System player counts and other perf. data from the RIS
	MTS_RIS_MRTG_DATA,
	MFS_RIS_MRTG_DATA,

	MTS_RIS_LAST_MESSAGE
};

// RIS error codes for MFS_RIS_ERROR_MESSAGE
enum RIS_RESULTS {
	E_RIS_NO_ERROR,
	E_RIS_BAD_REQUEST,
	E_RIS_NO_FREE,
	E_RIS_NO_MEM,

	E_RIS_CANT_PLAY_IN_ROOM,
	E_RIS_BANNED,
	E_RIS_BAD_PASSWORD,
	E_RIS_ACCOUNT_NOT_CREATED,
	E_RIS_ACCOUNT_NOT_FOUND,
	E_RIS_ACCOUNT_ALREADY_EXISTS,

	// nudge failed
	E_RIS_BAD_CONFIG,

	E_RIS_LAST_ERROR
};

typedef RIS_RESULTS RisResult_t;

//
#define RIS_KEY_SEPARATOR	"\\"
#define RIS_ROOM_MARKER		'~'
#define RIS_ROOM_MARKER_STR "~"

// bit flags for RIS -> ISAPI messages
#define RF_TABLE_VIEW		0x01
#define RF_FANCY_DIGITS		0x02
#define RF_CENTER			0x04
#define RF_RESERVED			0xF8

//======================================================================
// A generic error message response.
//======================================================================
struct MFS_RIS_ERROR_MESSAGE : MESSAGE_HEADER
{
	MsgType_t originalMessageType;
	
	RisResult_t errCode;
	char errText[MAX_ERROR_TEXT];
};

//======================================================================
// ISAPI DLL web query messages
//======================================================================
struct MTS_RIS_WWW_QUERY_MESSAGE : MESSAGE_HEADER
{
	// format of key is: \\Hoyle\\Card+Games\\Spades\\Social+Room+1
	char key[MAX_RIS_KEY];
};

// ISAPI gets one of these 
struct MFS_RIS_WWW_QUERY_MESSAGE : MESSAGE_HEADER
{
	// format of key is: \\Hoyle\\Card+Games\\Spades\\Social+Room+1
	char key[MAX_RIS_KEY];

	// web page title
	char title[MAX_TITLE_TEXT];

	int systemTotalPlayers;
        
	// this tells the ISAPI how many MFS_RIS_WWW_QUERY_ROOM_MESSAGE messages
	// follow this message
	short numRooms;

	// properties needed for JavaScript on web pages
	DWORD wonPlaySize;
	char launchProperties[1];
};

//
struct TRisRoomInfo
{
	short	sPort;

	// number of players in system below this node
	short playerCount;

	// subkey node name
	char subKey[MAX_RIS_SUBKEY];

	// bit flags describing how to display web links
	char	roomFlags;
	char	isAlive;

	// info for WONClient room launch
	DWORD	ipaddr;
	DWORD	roomID;

	// text to display as web hyper-link
	char linkText	[MAX_LINK_TEXT];
	char description[MAX_ROOM_DESC];
	char header		[MAX_HEADER_TEXT];
	
	// web link graphics
	char bullet[MAX_ICON_FILE];
	char suffix[MAX_ICON_FILE];
};

// ISAPI gets one of these messages per room / node
struct MFS_RIS_WWW_QUERY_ROOM_MESSAGE : MESSAGE_HEADER
{
	TRisRoomInfo ri;
};

//======================================================================
// Find Player messages from the ISAPI WWW DLL
//======================================================================
struct MTS_RIS_FIND_PLAYER_MESSAGE : MESSAGE_HEADER
{
	char playerName[MAX_PLAYER_NAME];

	// FindPlayer, FindInterest, etc.
	char submitText[MAX_SUBMIT_TEXT];
};

// one per find player transaction
struct MFS_RIS_FIND_PLAYER_HEADER_MESSAGE : MESSAGE_HEADER
{
	// properties needed for JavaScript on web pages
	DWORD wonPlaySize;
	char launchProperties[1];
};

// one per room player was found in
struct MFS_RIS_FIND_PLAYER_MESSAGE : MESSAGE_HEADER
{
	char playerName[MAX_PLAYER_NAME];
	char wasFound;

	// info about the room the player was in
	TRisRoomInfo ri;
};

//======================================================================
// WON Client to RIS login messages
//======================================================================
struct MTS_RIS_LOGIN_MESSAGE : MESSAGE_HEADER
{
	char playerName[MAX_PLAYER_NAME];
	char password[MAX_PASSWORD];

	SNMachineID playerMachineID;
	MTS_RIS_LOGIN_MESSAGE() : playerMachineID(dwPC1) {} // add this line
};

struct MFS_RIS_LOGIN_MESSAGE : MESSAGE_HEADER
{
	RisResult_t result;	// see enum

	playerid_t	playerID;
	int			type;
};

//======================================================================
// WON Client to RIS new account creation.  Note this structure derives
// from the normal login message!  In return, the WON client receives
// a MFS_RIS_WON_LOGIN_MESSAGE or an MFS_RIS_ERROR_MESSAGE message.
//======================================================================
struct MTS_RIS_CREATE_ACCOUNT_MESSAGE : MTS_RIS_LOGIN_MESSAGE
{
	char email[MAX_EMAIL];
	char realName[MAX_REAL_NAME];
	char profile[1]; // 512 varchar blob
};

//======================================================================
// Won Client -> RIS resource query
//======================================================================
struct MTS_RIS_ROOM_QUERY_MESSAGE : MESSAGE_HEADER
{
	int			type;
	DWORD		roomID;
	DWORD		roomIDs[ROOM_ID_COUNT];
};

typedef struct		// currently 50 bytes
{
	char ip[MAX_IP_STRING];
	short port;
} IPAddress;

struct MFS_RIS_ROOM_QUERY_MESSAGE : MESSAGE_HEADER
{
	RisResult_t	result;

	short numberOfFiles;
	short numberOfFilters;
	
	IPAddress room;

	short numberOfFileServers;
	IPAddress fileServers[MAX_FILE_SERVERS];

	short numberOfAuthServers;
	IPAddress authServers[MAX_AUTH_SERVERS];

	short numberOfAdServers;
	IPAddress adServers[MAX_AD_SERVERS];

	char inipath		[MAX_FILE_NAME];
	char exepath		[MAX_FILE_NAME];
	char communityName	[MAX_COMMUNITY_NAME];
};

struct MFS_RIS_FILTER_MESSAGE : MESSAGE_HEADER
{
	char lastOne;

	char flag;
	char extension[5];
	char dll[20];
	char function[32];
	char filename[20];
};

struct MFS_RIS_FILE_MESSAGE : MESSAGE_HEADER
{
	char lastOne;

	// variable length string
	char filename[1];
};

//======================================================================
// WON Client -> RIS machine inventory.
//======================================================================
struct MTS_RIS_MACHINE_INVENTORY_MESSAGE : MESSAGE_HEADER // 928 bytes
{
	char		login[MAX_PLAYER_NAME];

	// OS info
	long os;							// 0 = 95, 1 = 98, 2 = NT, 3 = Win2000
	long osMajorVersion;
	long osMinorVersion;
	long osBuild;
	long languageID;

	// CPU info
	long cpuType;						// 3 = 386, etc
	long numProcessors;
	unsigned long cpuSignature;
	unsigned long cpuFeatureFlags;
	long cpuFrequency;					// MHz
	char cpuManufacturer[MAX_CPU_MFG];

	// RAM
	unsigned long physicalRam;			// Mb

	// Video
	long numMonitors;
	long screenDepth;					// bits per pixel
	long screenHeight;					// pixels
	long screenWidth;					// pixels
	unsigned long videoRam;				// Mb
	char videoInfo[MAX_VIDEO_INFO];		// card mfg; not currently implemented

	// Sound
	unsigned long soundRam;				// Mb; not currently implemented
	char soundInfo[MAX_SOUND_INFO];		// not currently implemented

	// Local drives
	unsigned long hdSpace;				// Mb
	char cdInfo[MAX_CD_INFO];			// not currently implemented

	// Internet/network
	unsigned long pingTime;				// mS; not currently implemented
	char browserInfo[MAX_BROWSER_INFO];	// name, version, mfg
	char modemInfo[MAX_MODEM_INFO];		// not currently implemented
	char nicInfo[MAX_NIC_INFO];			// not currently implemented

	// Peripherals
	char mouseInfo[MAX_MOUSE_INFO];		// not currently implemented
};

// RIS -> WONClient
struct MFS_RIS_MACHINE_INVENTORY_MESSAGE : MESSAGE_HEADER
{
	// nothing to say!
};

//======================================================================
// RIS asks RS if it has a player
//======================================================================
struct MTS_RS_FIND_PLAYER_MESSAGE : MESSAGE_HEADER
{
	char playerName[MAX_PLAYER_NAME];
};

struct MFS_RS_FIND_PLAYER_MESSAGE : MESSAGE_HEADER
{
	char playerName[MAX_PLAYER_NAME];
	char wasFound;
	int playerCount;
};

//======================================================================
// RIS asks RS how many players
//======================================================================
struct MTS_RS_NUMPLAYERS_MESSAGE : MESSAGE_HEADER
{
	// round trip time
	DWORD rtt;
};

struct MFS_RS_NUMPLAYERS_MESSAGE : MESSAGE_HEADER
{
	int playerCount;
	DWORD rtt;
	DWORD gameStarts;

	DWORD heartbeatBoots;
	DWORD serverStatusCode;

	DWORD warnMessages;
	DWORD crashMessages;
	DWORD criticalMessages;
};

//======================================================================
// Someone asked us to re-read our config file
//======================================================================
struct MTS_RIS_UPDATE_RS_INFO_MESSAGE : MESSAGE_HEADER
{
	// nothing to say!
};

struct MFS_RIS_UPDATE_RS_INFO_MESSAGE : MESSAGE_HEADER
{
	RisResult_t result;
};

//======================================================================
// sent from WONplay to RoomInfo server
//======================================================================
struct MTS_RIS_CHEAT_APP_QUERY_MESSAGE : MESSAGE_HEADER
{
	// nothing to say!
};

// sent from RoomInfo server to WONplay
struct MFS_RIS_CHEAT_APP_QUERY_MESSAGE : MESSAGE_HEADER 
{
	RisResult_t result;
	short numberOfCheatPrograms;
};

// sent from RoomInfo server to WONplay; one message for each cheat program or variation
struct MFS_RIS_CHEAT_APP_MESSAGE : MESSAGE_HEADER
{
	char lastOne;

	// variable length
	char nameandclass[1];
};

// sent from MRTG to RoomInfo server
struct MTS_RIS_MRTG_DATA_MESSAGE : MESSAGE_HEADER
{
	char requestType;
};

// sent from RoomInfo server to MRTG
struct MFS_RIS_MRTG_DATA_MESSAGE : MESSAGE_HEADER
{
	int systemTotalPlayers;
	int minPlayersToday;
	int maxPlayersToday;
	int loginsToday;
	int accountCreations;
	int heartbeatBoots;

	DWORD warnMessages;
	DWORD crashMessages;
	DWORD criticalMessages;

	DWORD playersOnThisMachine;

	// TODO - wire these up
	DWORD filesInUse;
	DWORD filesMax;
	DWORD memoryInUse;
	DWORD swapSpaceUsed;
	DWORD diskSpaceUsed;

	int cpuUsage;

	DWORD dbTransactions;
	DWORD tickCount;
	DWORD msgSent;
	DWORD msgRecv;
	DWORD loginsOnThisRIS;

	char uptimeString[1];
};

#endif	// _RISMSG_H
