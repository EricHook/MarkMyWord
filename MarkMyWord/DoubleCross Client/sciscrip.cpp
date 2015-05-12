#include "os.h"
#include "app.h"
#include "sciscrip.h"
#include "scievent.h"
#include "game.h"

extern Game *game;
extern BOOL runFullSpeed;

sciScript::sciScript(sciScript *newCaller)
	{
	caller = newCaller;
	if (caller)
		caller->callee = this;
	callee = NULL;
	client = NULL;
	state = -1;
	nextTicks = 0;
	script = NULL;
	}

sciScript::~sciScript()
	{
	SetScript(NULL);
	if (caller)
		{
//		caller->Cue();
		caller->SetTicks(0);
		caller->callee = NULL;
		}
	if (callee)
		callee->caller = NULL;
	caller = NULL;
	callee = NULL;
	}

void sciScript::Init(sciObject *newClient)
	{
	client = newClient;
	SetTicks(0);		// Get a cue started for next Doit cycle.
	}

void sciScript::ChangeState(int newState)
	{
	state = newState;
	}

void sciScript::Cue()
	{
	// Don't cue if game is done.
	if (game->IsDone())
		return;
	if (script)
		script->Cue();
	else
		ChangeState(state+1);
	}

void sciScript::SetScript(sciScript *newScript)
	{
	if (script)
		delete script;
	script = newScript;
	if (script)
		script->Init(this);
	}

void sciScript::SetSeconds(int seconds)
	{
	SetTicks(seconds * 60); // nextTicks = game->GetTime() + seconds*60;
	}

void sciScript::SetTicks(int ticks)
	{
#ifdef _DEBUG
	/*
	char debugBuff[64];

	if (nextTicks)
		{
		wsprintf(debugBuff, "Ticks before: %d\n", nextTicks);
		DebugString(debugBuff);
		}
	else
		debugBuff[0] = 0;
	*/
#endif

#ifdef _DEBUG
	if (runFullSpeed)
		nextTicks = game->GetTime();
	else
		nextTicks = game->GetTime() + ticks;
#else
	nextTicks = game->GetTime() + ticks;
#endif

#ifdef _DEBUG
	/*
	char debugBuff1[64];

	if (debugBuff[0])
		{
		wsprintf(debugBuff1, "Ticks after: %d\n", nextTicks);
		DebugString(debugBuff1);
		}
	*/
#endif
	}

void sciScript::UnSetTicks()
	{
	nextTicks = 0;
	}

void sciScript::SetState(int newState)
	{
	state = newState;
	}

void sciScript::SetNextState(int newState)
	{
	state = newState - 1;
	}

int sciScript::GetState()
	{
	return state;
	}

void sciScript::Doit()
	{
	if (script)
		script->Doit();
	else
		{
		if (nextTicks && (game->GetTime() >= nextTicks))
			{
			nextTicks = 0;
			Cue();
			}
		}
	}

/* Example script code.
class Room0Script : public sciScript
	{
	void ChangeState(int newState)
		{
		sciScript::ChangeState(newState);
		switch (state)
			{
			case 0:
				MessageBox(NULL, "Testing scripts.", "Testing", MB_OK);
				SetState(GetState() - 1);
				SetSeconds(10);
				break;
			}
		}
	};

SetScript(new Room0Script);
*/
