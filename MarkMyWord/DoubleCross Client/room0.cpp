#include "os.h"
#include "inifile.h"
#include "sounder.h"

#include "Client Include.h"

#include "room0.h"

using namespace WONplayDLL;

Room0 *room0;

Room0::Room0()
{
	room0 = this;
	
	SetPic(ROOM_BACKGROUND_PIC);
}

Room0::~Room0()
{
	// MessageBoxA(NULL, "Goodbye 1", "DEBUG", MB_OK);
	if (roomClient)
		{ delete roomClient; roomClient = NULL; }
	// MessageBoxA(NULL, "Goodbye 2", "DEBUG", MB_OK);
	if (script)
		script->SetScript(NULL);
}

void Room0::Init()
{
	char *p, s[256], getS[256];
	int i;
	WonErr wonErr;
	WONinitialize wonInitialize;
	WONplayerInfo wonPlayerInfo;
	WONroomInfo wonRoomInfo;
	
	sciRoom::Init();
	
	//////////////////////////////////////////////////////////////////
	
	// now that we have our window, use it to initialze the DLL and retrieve our other params
	
	strcpy(wonInitialize.sRoomID, wonParams.wonPlayRoomGUID);
	wonInitialize.hRoomWnd = pApp->pMainPlane->GetWindow()->hWnd;
	wonErr = won->Init(&wonInitialize, wonParams.wonPlayDLLPath);
	if (!WONSUCCESS(wonErr)) {
		delete won;
		won = NULL;
		
#ifdef _DEBUG
		wonParams.wonPlayHWnd = NULL;
		pApp->pIniFile->ReadString("UserName", "???", wonParams.userName, MAX_PLAYER_NAME - 1);
		wonParams.databaseID = pApp->pIniFile->ReadValue("DatabaseID", 0);
		// fill in myMachineID ???
		SNMachineID myMachineID('PC 1');
		wonParams.myMachineID = myMachineID;

		pApp->pIniFile->ReadString("RoomName", "???", wonParams.roomName, MAX_PLAYER_NAME - 1);
		pApp->pIniFile->ReadString("ExePath", "", wonParams.exePath, MAX_PATH - 1);
		pApp->pIniFile->ReadString("ServerLocation", "???", wonParams.serverLocation, MAX_PLAYER_NAME - 1);
		wonParams.serverPort = pApp->pIniFile->ReadValue("ServerPort", 0);
		pApp->pIniFile->ReadString("RoomID", "0x00000000", s, 255);
		sscanf(s, "%x", &wonParams.roomID);
#else
		sprintf(s, GetString(224, getS), (int)wonErr);
		MessageBoxA(NULL, s, (char *)szAppName, MB_OK | MB_ICONSTOP | MB_TASKMODAL);
		
		return;
#endif
	}
	else {
		wonErr = won->GetHwnd(&wonParams.wonPlayHWnd);
		if (!WONSUCCESS(wonErr))
			{ MessageBoxA(NULL, GetString(225, getS), (char *)szAppName, MB_OK | MB_ICONSTOP | MB_TASKMODAL); game->QuitGame(TRUE); return; }
		
		wonErr = won->GetPlayerInfo(&wonPlayerInfo);
		if (!WONSUCCESS(wonErr))
			{ MessageBoxA(NULL, GetString(226, getS), (char *)szAppName, MB_OK | MB_ICONSTOP | MB_TASKMODAL); game->QuitGame(TRUE); return; }
		strnzcpy(wonParams.userName, wonPlayerInfo.sPlayerName, MAX_PLAYER_NAME - 1);
		wonParams.databaseID = wonPlayerInfo.nPlayerId;
		wonParams.myMachineID = wonPlayerInfo.machineId;
		
		wonErr = won->GetRoomInfo(&wonRoomInfo);
		if (!WONSUCCESS(wonErr))
			{ MessageBoxA(NULL, GetString(227, getS), (char *)szAppName, MB_OK | MB_ICONSTOP | MB_TASKMODAL); game->QuitGame(TRUE); return; }
		strnzcpy(wonParams.roomName, wonRoomInfo.sRoomName, MAX_LONG_ROOM_NAME - 1);
		strnzcpy(wonParams.exePath, wonRoomInfo.sRoomExe, MAX_PATH - 1);
		p = wonRoomInfo.sRoomServers;
		for (i = 0 ; p[0] && p[0] != ':' && i < (64 - 1) ; p++, i++)
			wonParams.serverLocation[i] = p[0];
		wonParams.serverLocation[i] = 0;
		sscanf(&p[1], "%d", &wonParams.serverPort);
		wonParams.roomID = wonRoomInfo.nRoomId;
		if (wonParams.roomID == 0)
			{ MessageBoxA(NULL, GetString(293, getS), (char *)szAppName, MB_OK | MB_ICONSTOP | MB_TASKMODAL); game->QuitGame(TRUE); return; }
		wonParams.sponsorURL[0] = 0;
	}
	
	SetWindowText(MAIN_HWND, wonParams.roomName);
	// SetWindowText(MAIN_HWND, "DEBUG SPADES");
	
	//////////////////////////////////////////////////////////////////
	
	// create and set the roomClient global object pointer
	if (!InitClientObject())
		{ game->QuitGame(TRUE); return; }
	
	SetScript(new Room0Script());
}

void Room0::Doit()
{
	sciRoom::Doit();
	
	if (roomClient)
		roomClient->Tick();
}

Boolean Room0::HandleEvent(sciEvent &event)
{
	if (roomClient)
		if (roomClient->popup)
			if (event.plane == roomClient->popup->plane)
				if (roomClient->popup->HandleEvent(event))
					return TRUE;
	
	return sciRoom::HandleEvent(event);
}

void Room0Script::ChangeState(int newState)
{
	sciScript::ChangeState(newState);
	
	switch (state) {
		case 0:
			break;
	}
}
