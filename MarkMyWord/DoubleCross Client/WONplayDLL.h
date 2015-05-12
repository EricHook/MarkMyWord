////////////////////////////////////////////////////////////////////////////////////
//
// WONplayDLL.h
//
// Copyright (c) World Opponent Network 1999, All rights reserved.
//
// Initial coding by Pete Isensee, 01/08/99
//
// Prototypes, structures and constants for WONplay DLL WONplay.dll.
// WON reserves the right to make continuing changes to the interface to improve
// its usability and features.
//
// All structs use an initial nSize value to support future enhancements
// (i.e. additional elements) to the structures.

#if !defined(Inc_WONplayDLL_)
	#define  Inc_WONplayDLL_

#ifndef  _WINDOWS_
#include <windows.h>
#endif

#ifdef __cplusplus
extern "C" {
namespace WONplayDLL {
#endif

////////////////////////////////////////////////////////////////////////////////////
//
// Simplify the differences between C and C++ for enums and default parameters

#ifdef __cplusplus
	#define WONERR				WONplayDLL::WonErr
	#define WONPLAYERTYPE		WONplayDLL::WonPlayerType
	#define WONDWNLOAD			WONplayDLL::WonDownload
	#define WONMESSAGES			WONplayDLL::WonMessages
	#define WONDEFAULT(value)	=(value)					//lint !e????
#else
	#define WONERR				enum WonErr
	#define WONPLAYERTYPE		enum WonPlayerType
	#define WONDWNLOAD			enum WonDownload
	#define WONMESSAGES			enum WonMessages
	#define WONDEFAULT(value)
#endif

#define WONCALL __stdcall
#define WONAPI(T) __declspec(dllexport) T WONCALL
#define WONCAPI(T) T WONCALL

////////////////////////////////////////////////////////////////////////////////////
//
// Handles

DECLARE_HANDLE(HWON);		// handle to room information; unique for life of room

////////////////////////////////////////////////////////////////////////////////////
//
// Error handling

enum WonErr // Error and warning codes
{
	 SUCCESS				=  0	// function completed successfully

	// warnings (>0)
	,WARN_MSG_NOT_PROCESSED =  1	// win message sent but not processed by WONplay

	// errors (<0)
	,ERR_INVALID_PARAM		= -1	// invalid param (e.g. NULL ptr, bad HWON,
									//					bad struct nSize)
	,ERR_MEMORY				= -2	// bad memory access or insufficient memory
	,ERR_INVALID_ID			= -3	// the room ID specified was invalid
	,ERR_LAUNCH				= -4	// browser launch failed
	,ERR_REGISTRY			= -5	// registry access failed
	,ERR_LIST_FULL			= -6	// registry friend/foe list is filled
	,ERR_EXISTS				= -7	// adding friend/foe that already exists
	,ERR_DOESNT_EXIST		= -8	// deleting friend/foe that doesn't exist
};

// 0 or greater indicates success; less than zero indicates failure
#define WONSUCCESS(status)	((WONERR)(status) >= 0)
#define WONFAILED(status)	((WONERR)(status) <  0)

////////////////////////////////////////////////////////////////////////////////////
//
// Download status

enum WonDownload
{
	 DWNLOAD_OFF			// not OK to download, or not currently downloading
	,DWNLOAD_ON				// OK to download, or currently downloading
	,DWNLOAD_DONE			// download complete
};

////////////////////////////////////////////////////////////////////////////////////
//
// Maximum string lengths, including trailing null
//
// These are purposefully larger than the current internal lengths to allow
// internal changes without affecting the external interface.

enum
{
	 nWON_ROOM_ID	    = 40
	,nWON_PLAYER_NAME	= 64
	,nWON_PLAYER_EMAIL	= 128
	,nWON_ROOM_NAME		= 64
	,nWON_PATH		    = 256
	,nWON_AD_TAG		= 256
	,nWON_URL		    = 512
	,nWON_TOOLTIP		= 128
	,nWON_SERVER_LIST	= 512
	,nWON_MAX_FRIENDS	= 128
	,nWON_MAX_FOES		= 128
	,nWON_MACHINE_ID	= 64
};

////////////////////////////////////////////////////////////////////////////////////
//
// Used by WONgetPlayerInfo()

typedef struct WONplayerInfo
{
	DWORD		nSize;							// for versioning
	TCHAR		sPlayerName[nWON_PLAYER_NAME]; 	// player account name
	int			nPlayerId;						// ID returned on successful login
	BYTE		machineId[nWON_MACHINE_ID];		// unique player machine ID

#ifdef __cplusplus
	WONplayerInfo()
	{
		nSize = sizeof(*this);
		*sPlayerName = 0;
		nPlayerId = 0;
		::ZeroMemory(machineId, nWON_MACHINE_ID);
	}
#endif
} WONplayerInfo;

////////////////////////////////////////////////////////////////////////////////////
//
// Used by WONinit()

typedef struct WONinitialize
{
	DWORD	nSize;					// sizeof(WONInitialize); for versioning
	TCHAR	sRoomID[nWON_ROOM_ID];	// Unique room ID.	Available from command-line.
	HWND	hRoomWnd; 				// Main window of room client.  Cannot be NULL.

#ifdef __cplusplus
	WONinitialize()
	{
		nSize = sizeof(*this);
		*sRoomID = 0;
		hRoomWnd = NULL;
	}
#endif
} WONinitialize;

////////////////////////////////////////////////////////////////////////////////////
//
// Used by WONgetRoomInfo()

typedef struct WONroomInfo
{
	DWORD	nSize;						// sizeof(WONRoomInfo); for versioning
	TCHAR	sRoomName[nWON_ROOM_NAME];	// Name of room
	DWORD	nRoomId;					// Unique room ID
	TCHAR	sRoomExe[nWON_PATH];		// Relative room exe file
										//		(e.g. "\Poker\poker.exe")
	DWORD	nRoomServers;				// Number of potential room servers

	// Room server list in host:port format.  Each server is null terminated.
	// Final server is terminated with dual nulls.
	TCHAR	sRoomServers[nWON_SERVER_LIST];

#ifdef __cplusplus
	WONroomInfo()
	{
		nSize = sizeof(*this);
		*sRoomName = 0;
		nRoomId = 0;
		*sRoomExe = 0;
		nRoomServers = 0;
		sRoomServers[0] = 0;
		sRoomServers[1] = 0;
	}

#endif
} WONroomInfo;

////////////////////////////////////////////////////////////////////////////////////
//
// Used by WONgetRoomFileDownload(), 
//         WONsetRoomFileDownload(),

typedef struct WONroomFileDownload
{
	DWORD		nSize;			// sizeof(WONRoomFileDownload); for versioning
	WONDWNLOAD	nStatus; 		// Indicates current state of download

#ifdef __cplusplus
	WONroomFileDownload()
	{
		nSize = sizeof(*this);
		nStatus = DWNLOAD_ON;
	}
#endif
} WONroomFileDownload;

////////////////////////////////////////////////////////////////////////////////////
//
// Used by WONgetAdFileDownload(), and
//		   WONsetAdFileDownload()

typedef struct WONadFileDownload
{
	DWORD		nSize;					// sizeof(WONAdFileDownload); for versioning
	WONDWNLOAD	nStatus; 				// Indicates current state of download
	TCHAR		sAdTags[nWON_AD_TAG];	// Tags passed to ad server
										//		(e.g. "area=Hoyle&type=banner")
	DWORD		nAdFrequencySeconds;	// Time between ad requests (0 = no ads,
										//		0xFFFFFFFF = 1 ad)
	TCHAR		sAdFolder[nWON_PATH];	// Destination folder for ads
										// If empty, destination folder is
										//		standard ad cache folder
										// If specified, ads are copied from cache
										//		to this folder
										// May be fully qualified path or relative
										//		path (relative from WONplay folder)

#ifdef __cplusplus
	WONadFileDownload()
	{
		nSize = sizeof(*this);
		nStatus = DWNLOAD_OFF;
		*sAdTags = 0;
		nAdFrequencySeconds = 0;
		*sAdFolder = 0;
	}
#endif
} WONadFileDownload;

////////////////////////////////////////////////////////////////////////////////////
//
// Used by WONgetAdInfo()

typedef struct WONadInfo
{
	DWORD	nSize;					// sizeof(WONAdInfo); for versioning
	TCHAR	sAdPath[nWON_PATH];		// location of ad file on disk (full path)
	TCHAR	sClickURL[nWON_URL];	// browser target when ad is clicked
	TCHAR	sToolTip[nWON_TOOLTIP];	// tool tip (descriptive string) for ad display

#ifdef __cplusplus
	WONadInfo()
	{
		nSize = sizeof(*this);
		*sAdPath = 0;
		*sClickURL = 0;
		*sToolTip = 0;
	}
#endif
} WONadInfo;

////////////////////////////////////////////////////////////////////////////////////
//
// Function ordinals for LoadLibrary/GetProcAddress

enum
{
	 nWONinit = 1
	,nWONexit
	,nWONgetRoomFileDownload
	,nWONsetRoomFileDownload
	,nWONgetAdFileDownload
	,nWONsetAdFileDownload
	,nWONsetAdClicked
	,nWONgetPlayerInfo
	,nWONgetRoomInfo
	,nWONgetFriendList
	,nWONaddFriend
	,nWONdeleteFriend
	,nWONgetFoeList
	,nWONaddFoe
	,nWONdeleteFoe
	,nWONgetHwnd
	,nWONshowHelp
	,nWONgetWONplayFolder
	,nWONlaunchBrowser
	,nWONmsgGetRoomFileInfo
	,nWONmsgGetAdInfo
	,nWONmsgGetAspect
	,nWONmsgGetFriendList
	,nWONmsgGetFoeList
	,nWONkill

	,nWON_MAX_FUNCTION
};

////////////////////////////////////////////////////////////////////////////////////
//
// WM_COPYDATA messages sent from WONplay to room client when:
//
//		1) a new room file has been successfully downloaded/expanded
//		2) a new ad file has been successfully downloaded/expanded
//		3) WONplay has been minimized or maximized
//		4) the room should shut down (cheat app detected, patching WONplay, etc.)
//		5) the friend list changed (via WONplay or another room/game)
//		6) the foe list changed (via WONplay or another room/game)

enum WonMessages
{								// Data
								// --------------------------------------
	 WM_WON_ROOM_FILE_AVAILABLE	// room file name (full path)
	,WM_WON_AD_FILE_AVAILABLE	// ad file name (full path) and click URL
	,WM_WON_ASPECT_CHANGED		// WONplay new nCmdShow flag
	,WM_WON_SHUTTING_DOWN		// [none]
	,WM_WON_FRIENDS_CHANGED		// new friend list
	,WM_WON_FOES_CHANGED		// new foe list
};

////////////////////////////////////////////////////////////////////////////////////
//
// Define function prototypes and function pointers for GetProcAddress

#define WONFUNC1(fnName, param1)												\
	WONAPI (WONERR) fnName(param1);												\
	typedef WONERR (WONCALL * pfn##fnName)(param1)

#define WONFUNC2(fnName, param1, param2)										\
	WONAPI (WONERR) fnName(param1, param2);										\
	typedef WONERR (WONCALL * pfn##fnName)(param1, param2)

#define WONFUNC3(fnName, param1, param2, param3)								\
	WONAPI (WONERR) fnName(param1, param2, param3);								\
	typedef WONERR (WONCALL * pfn##fnName)(param1, param2, param3)

////////////////////////////////////////////////////////////////////////////////////
//
// Function prototypes
//
// Functions return SUCCESS upon successful completion.
// Functions return ERR_INVALID_PARAM if any incoming parameters are invalid.
//		Typical parameter errors include: NULL pointers, and structs that don't
//		have the size value set correctly.
//
//			Function Name			Function parameters
// ---------------------------------------------------------------------------------

// Call immediately after the room has created its main window.
// If successful, returns a valid HWON handle that is used by the remaining funcs.
//
// Returns ERR_INVALID_ID if the room ID specified in the initialize structure
// does not match an existing room.
//
// Returns ERR_MEMORY in the rare cases where the Windows paging file
// is corrupt or there is insufficient memory to initialize.  The initialize call
// requires approximately a single page of memory (4K).

WONFUNC2(	WONinit,				const WONinitialize*, HWON*);

// ---------------------------------------------------------------------------------
// Call prior to room termination.
// HWON handle is invalid after this call.
// Returns WARN_MSG_NOT_PROCESSED if WONplay could not be notified that the room
// has terminated.  WONplay will eventually detect shutdown by noting that the
// room HWND is gone.

WONFUNC1(	WONexit,				HWON);

// ---------------------------------------------------------------------------------
// Get and set info on the current background file download status.
// The "set" function returns WARN_MSG_NOT_PROCESSED if WONplay could not be
// notified of the new status.

WONFUNC2(	WONgetRoomFileDownload, HWON, WONroomFileDownload*);
WONFUNC2(	WONsetRoomFileDownload, HWON, const WONroomFileDownload*);

// ---------------------------------------------------------------------------------
// Get and set info on the current ad file download status, frequency and tags.
// The "set" function returns WARN_MSG_NOT_PROCESSED if WONplay could not be
// notified of the new status.

WONFUNC2(	WONgetAdFileDownload,	HWON, WONadFileDownload*);
WONFUNC2(	WONsetAdFileDownload,	HWON, const WONadFileDownload*);

// ---------------------------------------------------------------------------------
// Call when the given ad has been clicked.  Set bBackground true to record
// click without bringing browser to foreground.
// Returns ERR_LAUNCH if the default web browser could not be found or launched.

WONFUNC3(	WONsetAdClicked,		HWON, LPCTSTR sURL, 
									BOOL bBackground WONDEFAULT(FALSE)); //lint !e????

// ---------------------------------------------------------------------------------
// Get player and room info

WONFUNC2(	WONgetPlayerInfo,		HWON, WONplayerInfo*);
WONFUNC2(	WONgetRoomInfo,			HWON, WONroomInfo*);

// ---------------------------------------------------------------------------------
// Friends; list formatted with null-terminated strings; dual null marks end.
// Incoming string must have at least (nWON_PLAYER_NAME * nFriends + 1) characters.
// sFriendList or pnFriends may be NULL.
// addFriend returns ERR_LIST_FULL if the friend list is full.
// addFriend returns ERR_EXISTS if the friend already exists in the list.
// deleteFriend returns ERR_DOESNT_EXIST if the friend doesn't exist in the list.
// Functions return ERR_REGISTRY if the friend list could not be accessed.
// add/deleteFriend return WARN_MSG_NOT_PROCESSED if WONplay could not be
// notified of the new status.

WONFUNC3(	WONgetFriendList,		HWON, LPTSTR sFriendList, int* pnFriends);
WONFUNC2(	WONaddFriend,			HWON, LPCTSTR sFriend);
WONFUNC2(	WONdeleteFriend,		HWON, LPCTSTR sFriend);

// ---------------------------------------------------------------------------------
// Foes; list formatted with null-terminated strings; dual null marks end.
// Incoming string must have at least (nWON_PLAYER_NAME * nFoes + 1) characters.
// sFoeList or pnFoes may be NULL.
// addFoe returns ERR_LIST_FULL if the foe list is full.
// addFoe returns ERR_EXISTS if the foe already exists in the list.
// deleteFoe returns ERR_DOESNT_EXIST if the foe doesn't exist in the list.
// Functions return ERR_REGISTRY if the foe list could not be accessed.
// add/deleteFoe return WARN_MSG_NOT_PROCESSED if WONplay could not be
// notified of the new status.

WONFUNC3(	WONgetFoeList,			HWON, LPTSTR sFoeList, int* pnFoes);
WONFUNC2(	WONaddFoe,				HWON, LPCTSTR sFoe);
WONFUNC2(	WONdeleteFoe,			HWON, LPCTSTR sFoe);

// ---------------------------------------------------------------------------------
// Returns the main WONplay HWND

WONFUNC2(	WONgetHwnd,				HWON, HWND*);

// ---------------------------------------------------------------------------------
// Invokes the default browser to display WON online help
// Returns ERR_LAUNCH if the web browser could not be found or launched.

WONFUNC1(	WONshowHelp,			HWON);

// ---------------------------------------------------------------------------------
// Returns the folder containing WONplay.  Doesn't include trailing backslash.
// Returns ERR_REGISTRY if the folder could not be accessed from the registry.

WONFUNC2(	WONgetWONplayFolder,	HWON, LPTSTR sFolder);

// ---------------------------------------------------------------------------------
// Launch the default browser in a new window with the given URL.
// The state of the window can be specified using the nCmdShow flag (e.g.
// SW_SHOW, SW_HIDE, SWMINNOACTIVE, etc.)
// Returns ERR_LAUNCH if the web browser could not be found or launched.

WONFUNC3(	WONlaunchBrowser,		HWON, LPCTSTR sURL, int nCmdShow);

// ---------------------------------------------------------------------------------
// Tells WONplay and any other WON rooms or games to immediately terminate.
// This function is typically used when a player has been banned from the system.
// Caller will receive a WM_WON_SHUTTING_DOWN on its next message loop.

WONFUNC1(	WONkill,				HWON);

// ---------------------------------------------------------------------------------
// Extract message data

WONFUNC3(	WONmsgGetRoomFileInfo,	const PCOPYDATASTRUCT, LPTSTR sRoomFile, 
									int* pnChars);
WONFUNC2(	WONmsgGetAdInfo,		const PCOPYDATASTRUCT, WONadInfo*);
WONFUNC2(	WONmsgGetAspect,		const PCOPYDATASTRUCT, int* pnCmdShow);
WONFUNC3(	WONmsgGetFriendList,	const PCOPYDATASTRUCT, LPTSTR sFriendList,
									int* pnChars);
WONFUNC3(	WONmsgGetFoeList,		const PCOPYDATASTRUCT, LPTSTR sFoeList,
									int* pnChars);

////////////////////////////////////////////////////////////////////////////////////
//
/* Example use of message extraction functions in room client message loop:

	using namespace WONplayDLL;
	case (WM_COPYDATA) :
	{
		PCOPYDATASTRUCT pcds = reinterpret_cast<PCOPYDATASTRUCT>(lParam);
		switch (pcds->dwData)
		{
		case (WM_WON_ROOM_FILE_AVAILABLE) :
		{
			const int nChars = nWON_PATH;
			TCHAR sRoomFile[nChars];
			WONERR err = WONMsgGetRoomFileInfo(pcds, sRoomFile, &nChars);
			break;
		}
		case (WM_WON_AD_FILE_AVAILABLE) :
		{
			WONadInfo wonAdInfo;
			WONERR err = WONMsgGetAdInfo(pcds, &wonAdInfo);
			break;
		}
		// etc.
		}
	}
*/

#ifdef __cplusplus

////////////////////////////////////////////////////////////////////////////////////
//
// Helper class

class WON
{
private:
	HMODULE						m_hLib;
	HWON						m_hWON;
	pfnWONinit					m_pfnWONinit;
	pfnWONexit					m_pfnWONexit;
	pfnWONgetRoomFileDownload	m_pfnWONgetRoomFileDownload;
	pfnWONsetRoomFileDownload	m_pfnWONsetRoomFileDownload;
	pfnWONgetAdFileDownload		m_pfnWONgetAdFileDownload;
	pfnWONsetAdFileDownload		m_pfnWONsetAdFileDownload;
	pfnWONsetAdClicked			m_pfnWONsetAdClicked;
	pfnWONgetPlayerInfo			m_pfnWONgetPlayerInfo;
	pfnWONgetRoomInfo			m_pfnWONgetRoomInfo;
	pfnWONgetFriendList			m_pfnWONgetFriendList;
	pfnWONaddFriend				m_pfnWONaddFriend;
	pfnWONdeleteFriend			m_pfnWONdeleteFriend;
	pfnWONgetFoeList			m_pfnWONgetFoeList;
	pfnWONaddFoe				m_pfnWONaddFoe;
	pfnWONdeleteFoe				m_pfnWONdeleteFoe;
	pfnWONgetHwnd				m_pfnWONgetHwnd;
	pfnWONshowHelp				m_pfnWONshowHelp;
	pfnWONgetWONplayFolder		m_pfnWONgetWONplayFolder;
	pfnWONlaunchBrowser			m_pfnWONlaunchBrowser;
	pfnWONmsgGetRoomFileInfo	m_pfnWONmsgGetRoomFileInfo;
	pfnWONmsgGetAdInfo			m_pfnWONmsgGetAdInfo;
	pfnWONmsgGetAspect			m_pfnWONmsgGetAspect;
	pfnWONmsgGetFriendList		m_pfnWONmsgGetFriendList;
	pfnWONmsgGetFoeList			m_pfnWONmsgGetFoeList;
	pfnWONkill					m_pfnWONkill;

	void Clear()
	{
		m_hLib = NULL;
		m_hWON = NULL;
		m_pfnWONinit = NULL;
		m_pfnWONexit = NULL;
		m_pfnWONgetRoomFileDownload = NULL;
		m_pfnWONsetRoomFileDownload = NULL;
		m_pfnWONgetAdFileDownload = NULL;
		m_pfnWONsetAdFileDownload = NULL;
		m_pfnWONsetAdClicked = NULL;
		m_pfnWONgetPlayerInfo = NULL;
		m_pfnWONgetRoomInfo = NULL;
		m_pfnWONgetFriendList = NULL;
		m_pfnWONaddFriend = NULL;
		m_pfnWONdeleteFriend = NULL;
		m_pfnWONgetFoeList = NULL;
		m_pfnWONaddFoe = NULL;
		m_pfnWONdeleteFoe = NULL;
		m_pfnWONgetHwnd = NULL;
		m_pfnWONshowHelp = NULL;
		m_pfnWONgetWONplayFolder = NULL;
		m_pfnWONlaunchBrowser = NULL;
		m_pfnWONmsgGetRoomFileInfo = NULL;
		m_pfnWONmsgGetAdInfo = NULL;
		m_pfnWONmsgGetAspect = NULL;
		m_pfnWONmsgGetFriendList = NULL;
		m_pfnWONmsgGetFoeList = NULL;
		m_pfnWONkill = NULL;
	}
	friend class WONprivate;
public:
	WONCALL WON()
	{
		Clear();
	}
	WONCALL ~WON()
	{
		if (m_hLib != NULL)
			::FreeLibrary(m_hLib);
	}
	WONCAPI (WONERR) Init(const WONinitialize* pWONInit, LPCTSTR sDllPath)
	{
		m_hLib = ::LoadLibrary(sDllPath);
		if (m_hLib == NULL)
			return (ERR_DOESNT_EXIST);

		// Load by ordinal
		FARPROC* ppfn = (reinterpret_cast<FARPROC*>(&m_pfnWONinit));
		for (int i = nWONinit; i < nWON_MAX_FUNCTION; ++i, ++ppfn)
			*ppfn = ::GetProcAddress(m_hLib, MAKEINTRESOURCE(i));

		return (m_pfnWONinit(pWONInit, &m_hWON));
	}
	WONCAPI (WONERR) Exit()
	{
		WONERR err = m_pfnWONexit(m_hWON);
		::FreeLibrary(m_hLib);
		Clear();
		return (err);
	}
	WONCAPI (WONERR) GetRoomFileDownload(WONroomFileDownload* pWONroom) const
	{
		return (m_pfnWONgetRoomFileDownload == NULL ? ERR_MEMORY :
				m_pfnWONgetRoomFileDownload(m_hWON, pWONroom));
	}
	WONCAPI (WONERR) SetRoomFileDownload(const WONroomFileDownload* pWONroom) const
	{
		return (m_pfnWONsetRoomFileDownload == NULL ? ERR_MEMORY :
				m_pfnWONsetRoomFileDownload(m_hWON, pWONroom));
	}
	WONCAPI (WONERR) GetAdFileDownload(WONadFileDownload* pWONad) const
	{
		return (m_pfnWONgetAdFileDownload == NULL ? ERR_MEMORY :
				m_pfnWONgetAdFileDownload(m_hWON, pWONad));
	}
	WONCAPI (WONERR) SetAdFileDownload(const WONadFileDownload* pWONad) const
	{
		return (m_pfnWONsetAdFileDownload == NULL ? ERR_MEMORY :
				m_pfnWONsetAdFileDownload(m_hWON, pWONad));
	}
	WONCAPI (WONERR) SetAdClicked(LPCTSTR sURL, BOOL bBackground = FALSE) const
	{
		return (m_pfnWONsetAdClicked == NULL ? ERR_MEMORY :
				m_pfnWONsetAdClicked(m_hWON, sURL, bBackground));
	}
	WONCAPI (WONERR) GetPlayerInfo(WONplayerInfo* pWONplayer) const
	{
		return (m_pfnWONgetPlayerInfo == NULL ? ERR_MEMORY :
				m_pfnWONgetPlayerInfo(m_hWON, pWONplayer));
	}
	WONCAPI (WONERR) GetRoomInfo(WONroomInfo* pWONroom) const
	{
		return (m_pfnWONgetRoomInfo == NULL ? ERR_MEMORY :
				m_pfnWONgetRoomInfo(m_hWON, pWONroom));
	}
	WONCAPI (WONERR) GetFriendList(LPTSTR sFriendList, int* pnFriends = NULL) const
	{
		return (m_pfnWONgetFriendList == NULL ? ERR_MEMORY :
				m_pfnWONgetFriendList(m_hWON, sFriendList, pnFriends));
	}
	WONCAPI (WONERR) AddFriend(LPCTSTR sFriend) const
	{
		return (m_pfnWONaddFriend == NULL ? ERR_MEMORY :
				m_pfnWONaddFriend(m_hWON, sFriend));
	}
	WONCAPI (WONERR) DeleteFriend(LPCTSTR sFriend) const
	{
		return (m_pfnWONdeleteFriend == NULL ? ERR_MEMORY :
				m_pfnWONdeleteFriend(m_hWON, sFriend));
	}
	WONCAPI (WONERR) GetFoeList(LPTSTR sFoeList, int* pnFoes = NULL) const
	{
		return (m_pfnWONgetFoeList == NULL ? ERR_MEMORY :
				m_pfnWONgetFoeList(m_hWON, sFoeList, pnFoes));
	}
	WONCAPI (WONERR) AddFoe(LPCTSTR sFoe) const
	{
		return (m_pfnWONaddFoe == NULL ? ERR_MEMORY :
				m_pfnWONaddFoe(m_hWON, sFoe));
	}
	WONCAPI (WONERR) DeleteFoe(LPCTSTR sFoe) const
	{
		return (m_pfnWONdeleteFoe == NULL ? ERR_MEMORY :
				m_pfnWONdeleteFoe(m_hWON, sFoe));
	}
	WONCAPI (WONERR) GetHwnd(HWND* phWnd) const
	{
		return (m_pfnWONgetHwnd == NULL ? ERR_MEMORY :
				m_pfnWONgetHwnd(m_hWON, phWnd));
	}
	WONCAPI (WONERR) ShowHelp() const
	{
		return (m_pfnWONshowHelp == NULL ? ERR_MEMORY :
				m_pfnWONshowHelp(m_hWON));
	}
	WONCAPI (WONERR) GetWONplayFolder(LPTSTR sFolder) const
	{
		return (m_pfnWONgetWONplayFolder == NULL ? ERR_MEMORY :
				m_pfnWONgetWONplayFolder(m_hWON, sFolder));
	}
	WONCAPI (WONERR) LaunchBrowser(LPCTSTR sURL, int nShowCmd = SW_SHOWNORMAL) const
	{
		return (m_pfnWONlaunchBrowser == NULL ? ERR_MEMORY :
				m_pfnWONlaunchBrowser(m_hWON, sURL, nShowCmd));
	}
	WONCAPI (WONERR) Kill() const
	{
		return (m_pfnWONkill == NULL ? ERR_MEMORY :
				m_pfnWONkill(m_hWON));
	}
	WONCAPI (WONERR) MsgGetRoomFileInfo(const PCOPYDATASTRUCT pcds, LPTSTR sRoomFile,
										int* pnChars) const
	{
		return (m_pfnWONmsgGetRoomFileInfo == NULL ? ERR_MEMORY :
				m_pfnWONmsgGetRoomFileInfo(pcds, sRoomFile, pnChars));
	}
	WONCAPI (WONERR) MsgGetAdInfo(const PCOPYDATASTRUCT pcds, 
									WONadInfo* pAdInfo) const
	{
		return (m_pfnWONmsgGetAdInfo == NULL ? ERR_MEMORY :
				m_pfnWONmsgGetAdInfo(pcds, pAdInfo));
	}
	WONCAPI (WONERR) MsgGetAspect(const PCOPYDATASTRUCT pcds, int* pnCmdShow) const
	{
		return (m_pfnWONmsgGetAspect == NULL ? ERR_MEMORY :
				m_pfnWONmsgGetAspect(pcds, pnCmdShow));
	}
	WONCAPI (WONERR) MsgGetFriendList(const PCOPYDATASTRUCT pcds, LPTSTR sFriendList,
									int* pnChars) const
	{
		return (m_pfnWONmsgGetFriendList == NULL ? ERR_MEMORY :
				m_pfnWONmsgGetFriendList(pcds, sFriendList, pnChars));
	}
	WONCAPI (WONERR) MsgGetFoeList(const PCOPYDATASTRUCT pcds, LPTSTR sFoeList,
									int* pnChars) const
	{
		return (m_pfnWONmsgGetFoeList == NULL ? ERR_MEMORY :
				m_pfnWONmsgGetFoeList(pcds, sFoeList, pnChars));
	}
};

} // end of WONplayDLL namespace
} // end of extern "C"

#endif // __cplusplus

#endif // Inc_WONplayDLL_

////////////////////////////////////////////////////////////////////////////////////
