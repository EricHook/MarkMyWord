#include "os.h"
#include "scicycle.h"
#include "sciprop.h"
#include "game.h"
#include "sciscrip.h"

extern Game *game;
extern BOOL runFullSpeed;

sciCycle::sciCycle(sciScript *newCaller)
	{
	client = NULL;
	caller = newCaller;
	clientLastCel = 0;
	cycleDir = 0;
	cycleCnt = game->GetTime();
	inMotion = FALSE;
	}

sciCycle::~sciCycle()
	{
	}

void sciCycle::Init(sciProp *newClient)
	{
	client = newClient;
	clientLastCel = client->LastCel();
	inMotion = TRUE;
	}

int sciCycle::NextCel()
	{
#ifdef _DEBUG
	if (runFullSpeed)
		client->cycleSpeed = 0;
#endif

	if (abs(game->GetTime() - cycleCnt) < client->cycleSpeed)
		return client->GetCel();
	else
		{
		cycleCnt = game->GetTime();
		return client->GetCel() + cycleDir;
		}
	}

void sciCycle::Doit()
	{
	}

void sciCycle::CycleDone()
	{
	if (caller)
		caller->SetTicks(0);
	inMotion = FALSE;
	client->SetCycle(NULL);	// Detach and dispose of "this".
	}

sciForward::sciForward(sciScript *newCaller)
	: sciCycle(newCaller)
	{
	cycleDir = CYCLEDIR_FORWARD;
	}

sciForward::~sciForward()
	{
	}

void sciForward::Doit()
	{
	int newCel;

	if (!inMotion)
		return;

	newCel = NextCel();
	if (client->GetCel() != newCel)
		{
		if (newCel > clientLastCel)
			{
			CycleDone();
			}
		else
			{
			client->SetCel(newCel);
			}
		}
		
	}

void sciForward::CycleDone()
	{
	client->SetCel(0);
	}

sciForwardWithCue::sciForwardWithCue(sciScript *newCaller)
	: sciForward(newCaller)
	{
	}

sciForwardWithCue::~sciForwardWithCue()
	{
	}

void sciForwardWithCue::CycleDone()
	{
	if (caller) {
		caller->SetTicks(0);
	}

	sciForward::CycleDone();
	}

sciReverse::sciReverse(sciScript *newCaller)
	: sciCycle(newCaller)
	{
	cycleDir = CYCLEDIR_REVERSE;
	}

sciReverse::~sciReverse()
	{
	}

void sciReverse::Doit()
	{
	int newCel;

	if (!inMotion)
		return;

	newCel = NextCel();
	if (client->GetCel() != newCel)
		{
		if (newCel < 0)
			{
			CycleDone();
			}
		else
			{
			client->SetCel(newCel);
			}
		}
		
	}

void sciReverse::CycleDone()
	{
	client->SetCel(clientLastCel);
	}

sciReverseWithCue::sciReverseWithCue(sciScript *newCaller)
	: sciReverse(newCaller)
	{
	}

sciReverseWithCue::~sciReverseWithCue()
	{
	}

void sciReverseWithCue::CycleDone()
	{
	if (caller) {
		caller->SetTicks(0);
	}

	sciReverse::CycleDone();
	}

sciCycleTo::sciCycleTo(sciScript *newCaller)
	: sciCycle(newCaller)
	{
	endCel = 0;
	}

sciCycleTo::~sciCycleTo()
	{
	}

void sciCycleTo::Doit()
	{
	int newCel;

	if (!inMotion)
		return;

	newCel = NextCel();
	if (client->GetCel() != newCel)
		{
		if (newCel == endCel)
			{
			client->SetCel(endCel);
			CycleDone();
			}
		else
			{
			client->SetCel(newCel);
			}
		}
		
	}

void sciCycleTo::SetEndCel(int iNewEnd)
	{
	endCel = iNewEnd;
	}

sciCycleForwardTo::sciCycleForwardTo(sciScript *newCaller)
	: sciCycleTo(newCaller)
	{
	cycleDir = CYCLEDIR_FORWARD;
	endCel = 0;
	}

sciCycleForwardTo::sciCycleForwardTo(int newEndCel, sciScript *newCaller)
	: sciCycleTo(newCaller)
	{
	cycleDir = CYCLEDIR_FORWARD;
	endCel = newEndCel;
	}

sciCycleForwardTo::~sciCycleForwardTo()
	{
	}

sciCycleReverseTo::sciCycleReverseTo(sciScript *newCaller)
	: sciCycleTo(newCaller)
	{
	cycleDir = CYCLEDIR_REVERSE;
	endCel = 0;
	}

sciCycleReverseTo::sciCycleReverseTo(int newEndCel, sciScript *newCaller)
	: sciCycleTo(newCaller)
	{
	cycleDir = CYCLEDIR_REVERSE;
	endCel = newEndCel;
	}

sciCycleReverseTo::~sciCycleReverseTo()
	{
	}

sciBegLoop::sciBegLoop(sciScript *newCaller)
	: sciCycleTo(newCaller)
	{
	}

void sciBegLoop::Init(sciProp *newClient)
	{
	sciCycle::Init(newClient);
	endCel = 0;
	cycleDir = CYCLEDIR_REVERSE;
	}

sciEndLoop::sciEndLoop(sciScript *newCaller)
	: sciCycleTo(newCaller)
	{
	}

void sciEndLoop::Init(sciProp *newClient)
	{
	sciCycle::Init(newClient);
	endCel = clientLastCel;
	cycleDir = CYCLEDIR_FORWARD;
	}

sciOscillate::sciOscillate()
	: sciCycle(NULL)
	{
	cycleDir = CYCLEDIR_FORWARD;
	}

sciOscillate::~sciOscillate()
	{
	}

void sciOscillate::Doit()
	{
	int newCel;

	if (!inMotion)
		return;

	newCel = NextCel();
	if (client->GetCel() != newCel)
		{
		if ((newCel > clientLastCel) || (newCel < 0))
			{
			CycleDone();
			}
		else
			{
			client->SetCel(newCel);
			}
		}
		
	}

void sciOscillate::CycleDone()
	{
	if (cycleDir == CYCLEDIR_FORWARD)
		cycleDir = CYCLEDIR_REVERSE;
	else
		cycleDir = CYCLEDIR_FORWARD;
	}

sciFlash::sciFlash(int newNumFlashes, BOOL newEndShown, Boolean constntFlash)
	: sciCycle(NULL)
	{
		numFlashes	= newNumFlashes;
		endShown	= newEndShown;
		constantFlash = constntFlash;
	}

sciFlash::~sciFlash()
	{
	}

BOOL sciFlash::FlashNow()
	{
#ifdef _DEBUG
	if (runFullSpeed)
		client->cycleSpeed = 0;
#endif

	if (abs(game->GetTime() - cycleCnt) < client->cycleSpeed)
		return FALSE;
	else
		{
		cycleCnt = game->GetTime();
		return TRUE;
		}
	}
void sciFlash::Doit()
	{
	if (!inMotion)
		return;

	if(FlashNow())
		{
		if(client->IsHidden())
			{
			client->Show();
			if (constantFlash == FALSE)
				numFlashes--;
			}
		else
			client->Hide();
		}
	
	if(constantFlash == FALSE && numFlashes <= 0)
		CycleDone();

		
	}

void sciFlash::CycleDone()
	{
	if(!endShown)
		client->Hide();
	sciCycle::CycleDone();
	}


sciToggle::sciToggle() : sciCycle(NULL)
{
}

sciToggle::sciToggle(int theCelOne, int theCelTwo)
	: sciCycle(NULL)
{
	celOne = theCelOne;
	celTwo = theCelTwo;
	cycleCnt = 0;
}

sciToggle::~sciToggle()
{
}

void sciToggle::Doit()
{
	if (!inMotion)
		return;
	
	if (abs(game->GetTime() - cycleCnt) < client->cycleSpeed)
		return;
	
	cycleCnt = game->GetTime();
	
	client->SetCel((client->GetCel()==celOne) ? celTwo : celOne);
}

void sciToggle::SetCelOne(int c)
{
	celOne = c;
}

void sciToggle::SetCelTwo(int c)
{
	celTwo = c;
}

sciRandomCycle::sciRandomCycle()
	: sciCycle(NULL)
{
//	clientLastCel = client->LastCel();
}

sciRandomCycle::~sciRandomCycle()
{
}

void sciRandomCycle::Doit()
{
	int newCel;

	if (!inMotion)
		return;

	newCel = GetACel();

	client->SetCel(newCel);
}

int sciRandomCycle::GetACel()
{
#ifdef _DEBUG
	if (runFullSpeed)
		client->cycleSpeed = 0;
#endif

	if (abs(game->GetTime() - cycleCnt) < client->cycleSpeed)
	{
		return client->GetCel();
	} else {
		cycleCnt = game->GetTime();
		return random(clientLastCel + 1);
	}
}
