#ifndef _DBMESSAGES_H
#define _DBMESSAGES_H

#include "hwmessage.h"	 // in Connection manager, for MESSAGE_HEADER def
#include "SNMachineID.h" // for machine ID type
#include "DBGen.h"		 // for DB status codes

#define DBPORT  6666

enum {
		MFDB_STATUS = MB_DATABASE,

		MTDB_RIS_LOGIN,
		MFDB_RIS_LOGIN,
		MTDB_RIS_CREATE_ACCOUNT,
		MFDB_RIS_CREATE_ACCOUNT,
		MTDB_RIS_CLEAR_ROOM_ID,
		MFDB_RIS_CLEAR_ROOM_ID,
		MTDB_RIS_MACHINE_INVENTORY,
		MFDB_RIS_MACHINE_INVENTORY,
		
		MTDB_RS_CONNECT,
		MFDB_RS_CONNECT,
		MTDB_RS_SET_SHADOW,
		MFDB_RS_SET_SHADOW,
		MTDB_RS_SET_PROFILE,
		MFDB_RS_SET_PROFILE,
		MTDB_RS_SET_BANK,
		MFDB_RS_SET_BANK,
		MTDB_RS_SET_GAME_RECORD,
		MFDB_RS_SET_GAME_RECORD,
		MTDB_RS_SET_BANNED,
		MFDB_RS_SET_BANNED,
		MTDB_RS_SET_UNBANNED,
		MFDB_RS_SET_UNBANNED,
		MTDB_RS_DISCONNECT,
		MFDB_RS_DISCONNECT,
	
		MTDB_LAST_MESSAGE
};

//======================================================================
// General-purpose status message
struct MFDB_STATUS_MESSAGE : MESSAGE_HEADER
{
	DBSTATUS	dbStat; // DBS_SUCCESS, DBS_NOT_FOUND, DBS_BANNED, ...
};

//======================================================================
// MESSAGES BETWEEN ROOM INFORMATION SERVER (RIS) AND DATABASE

//////////////////////////////////////////////////////////////

// RIS -> DB
struct MTDB_RIS_LOGIN_MESSAGE : MESSAGE_HEADER
{
	char		login[MAX_PLAYER_NAME];
	
	magic_t		magicIndex;

	SNMachineID playerMachineID;
	MTDB_RIS_LOGIN_MESSAGE() : playerMachineID(dwPC1) {}
};

// DB -> RIS
struct MFDB_RIS_LOGIN_MESSAGE : MESSAGE_HEADER
{
	DBSTATUS	dbStat;
	char		login[MAX_PLAYER_NAME];
	
	magic_t		magicIndex;

	// the following fields are ignored unless dbStat is DBS_SUCCESS
	
	playerid_t	playerID; // database-assigned id
	char		password[MAX_PASSWORD];
	int			type;
	DWORD		roomIDs[ROOM_ID_COUNT];
};

//////////////////////////////////////////////////////////////

// RIS -> DB
struct MTDB_RIS_CREATE_ACCOUNT_MESSAGE : MESSAGE_HEADER
{
	char		login[MAX_PLAYER_NAME];
	char		password[MAX_PASSWORD];
	int			type;
	int			shadow;
	
	magic_t		magicIndex;

	SNMachineID playerMachineID;
	
	char		realName[MAX_REAL_NAME];
	char		email[MAX_EMAIL];
	
	int			profileLength; // includes null-terminator
	char		profile[1]; // variable length null-terminated (profileLength)

	MTDB_RIS_CREATE_ACCOUNT_MESSAGE() : playerMachineID(dwPC1) {}
};

// DB -> RIS
struct MFDB_RIS_CREATE_ACCOUNT_MESSAGE : MESSAGE_HEADER
{
	DBSTATUS	dbStat;
	char		login[MAX_PLAYER_NAME];
	
	magic_t		magicIndex;

	// the following fields are ignored unless dbStat is DBS_SUCCESS
	
	playerid_t	playerID; // database-assigned id
};

//////////////////////////////////////////////////////////////

// RIS -> DB
struct MTDB_RIS_CLEAR_ROOM_ID_MESSAGE : MESSAGE_HEADER
{
	playerid_t	playerID; // database-assigned id
	int			roomIDIndex;
};

// DB -> RIS
struct MFDB_RIS_CLEAR_ROOM_ID_MESSAGE : MESSAGE_HEADER
{
	DBSTATUS	dbStat;
};

//////////////////////////////////////////////////////////////

// RIS -> DB
struct MTDB_RIS_MACHINE_INVENTORY_MESSAGE : MESSAGE_HEADER // 928 bytes
{
	char		login[MAX_PLAYER_NAME];
	
	magic_t		magicIndex;

	// OS info
	long os;
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

// DB -> RIS
struct MFDB_RIS_MACHINE_INVENTORY_MESSAGE : MESSAGE_HEADER // 928 bytes
{
	DBSTATUS	dbStat;

	magic_t		magicIndex;
};

//======================================================================
// MESSAGES BETWEEN ROOM SERVER (RS) AND DATABASE

//////////////////////////////////////////////////////////////

// RS -> DB
struct MTDB_RS_CONNECT_MESSAGE : MESSAGE_HEADER
{
	playerid_t	playerID;
	magic_t		magicIndex;
	int			type;		// can only be FREEBIE or CUSTOMER; does not override higher levels
	DWORD		roomID; // put into first empty (zero) roomIDs entry
	
	// for marketing tracking
	char		gameName[MAX_GAME_NAME];
	char		roomName[MAX_ROOM_NAME];
	int			userCount;
};

// DB -> RS
struct MFDB_RS_CONNECT_MESSAGE : MESSAGE_HEADER
{
	DBSTATUS	dbStat;
	magic_t		magicIndex;
	playerid_t	playerID; // database-assigned id
	
	// the following fields are ignored unless dbStat is DBS_SUCCESS or DBS_BANNED
	
	char		login[MAX_PLAYER_NAME];
	char		password[MAX_PASSWORD];
	char		email[MAX_EMAIL];
	int			type; 

	int			shadow;
	
	int			bank;
	char		gameRecord[MAX_PLAYER_GAME_RECORD]; // null-terminated string
	
	int			profileLength; // includes null-terminator
	char		profile[1]; // variable length null-terminated (profileLength)
};

//////////////////////////////////////////////////////////////

// RS -> DB
struct MTDB_RS_SET_SHADOW_MESSAGE : MESSAGE_HEADER
{
	playerid_t	playerID; // database-assigned id
	magic_t		magicIndex;
	int			shadow;
};

// DB -> RS
struct MFDB_RS_SET_SHADOW_MESSAGE : MESSAGE_HEADER
{
	DBSTATUS	dbStat;	
	magic_t		magicIndex;
	playerid_t	playerID; // database-assigned id
};

//////////////////////////////////////////////////////////////

// RS -> DB
struct MTDB_RS_SET_PROFILE_MESSAGE : MESSAGE_HEADER
{
	playerid_t	playerID; // database-assigned id
	magic_t		magicIndex;
	int			profileLength; // includes null-terminator
	char		profile[1]; // variable length null-terminated (profileLength)
};

// DB -> RS
struct MFDB_RS_SET_PROFILE_MESSAGE : MESSAGE_HEADER
{
	DBSTATUS	dbStat;	
	magic_t		magicIndex;
	playerid_t	playerID; // database-assigned id
};

//////////////////////////////////////////////////////////////

// RS -> DB
struct MTDB_RS_SET_BANK_MESSAGE : MESSAGE_HEADER
{
	playerid_t	playerID; // database-assigned id
	magic_t		magicIndex;
	int			bank;
};

// DB -> RS
struct MFDB_RS_SET_BANK_MESSAGE : MESSAGE_HEADER
{
	DBSTATUS	dbStat;	
	magic_t		magicIndex;
	playerid_t	playerID; // database-assigned id
};

//////////////////////////////////////////////////////////////

// RS -> DB
struct MTDB_RS_SET_GAME_RECORD_MESSAGE : MESSAGE_HEADER
{
	playerid_t	playerID; // database-assigned id
	magic_t		magicIndex;
	char		gameName[MAX_GAME_NAME];
	int			grLength; // includes null-terminator
	char		gameRecord[1]; // variable length null-terminated (grLength)
};

// DB -> RS
struct MFDB_RS_SET_GAME_RECORD_MESSAGE : MESSAGE_HEADER
{
	DBSTATUS	dbStat;	
	magic_t		magicIndex;
	playerid_t	playerID; // database-assigned id
};

//////////////////////////////////////////////////////////////

// RS -> DB
struct MTDB_RS_SET_BANNED_MESSAGE : MESSAGE_HEADER
{
	char		login[MAX_PLAYER_NAME];
	int			OfflineBanFlag; // 1 = banned offline therefore disregard machine ID field
								// 0 = banned online, store the machine ID field
	SNMachineID playerMachineID;
	char		moderatorName[MAX_PLAYER_NAME];
	char		gameName[MAX_GAME_NAME];
	char		roomName[MAX_ROOM_NAME];

	MTDB_RS_SET_BANNED_MESSAGE() : playerMachineID(dwPC1) {}
};

// DB -> RS
struct MFDB_RS_SET_BANNED_MESSAGE : MESSAGE_HEADER
{
	DBSTATUS	dbStat;	
};

//////////////////////////////////////////////////////////////

// RS -> DB
struct MTDB_RS_SET_UNBANNED_MESSAGE : MESSAGE_HEADER
{
	char		login[MAX_PLAYER_NAME];
};

// DB -> RS
struct MFDB_RS_SET_UNBANNED_MESSAGE : MESSAGE_HEADER
{
	DBSTATUS	dbStat;	
};

//////////////////////////////////////////////////////////////

// RS -> DB
struct MTDB_RS_DISCONNECT_MESSAGE : MESSAGE_HEADER
{
	playerid_t	playerID; // database-assigned id
	magic_t		magicIndex;
	DWORD		roomID; // remove from roomIDs array
	
	// for marketing tracking
	char		gameName[MAX_GAME_NAME];
	char		roomName[MAX_ROOM_NAME];
	int			userCount;
	int			active; // 0 = passive logout (client probably crashed), 1 = active
	
	// final state of player's data
	int			shadow;
	int			bank;
	int			grLength; // includes null-terminator
	char		gameRecord[1]; // variable length null-terminated (grLength)
};

// DB -> RS
struct MFDB_RS_DISCONNECT_MESSAGE : MESSAGE_HEADER
{
	DBSTATUS	dbStat;	
	magic_t		magicIndex;
	playerid_t	playerID; // database-assigned id
};

//////////////////////////////////////////////////////////////

#endif	// _DBMESSAGES_H
