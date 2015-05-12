#include <math.h>

#include "os.h"
#include "scimotn.h"
#include "sciactor.h"
#include "game.h"
#include "sciscrip.h"

extern Game *game;

sciMotion::sciMotion(sciScript *newCaller)
	{
	client = NULL;
	caller = newCaller;
	inMotion = FALSE;
	SetTarget(0, 0);
	}

sciMotion::~sciMotion()
	{
	}

void sciMotion::Doit()
	{
	POINT p;

	if (!inMotion)
		return;

	if (abs(game->GetTime() - moveCnt) >= client->moveSpeed)
		{
		moveCnt = game->GetTime();
		if (inMotion)
			{
			fX += fXDelta;
			fY += fYDelta;
			p.x = (int)fX;
			p.y = (int)fY;
			client->Posn(p.x, p.y);
			if(abs(p.x - targetX) < 5 && abs(p.y - targetY) < 5)
				{
				MotionDone();
				}
			}
		}
	}

void sciMotion::MotionDone()
	{
	if (caller)
		caller->SetTicks(0);
	inMotion = FALSE;
	client->Posn(targetX, targetY);
	client->SetMotion(NULL);
	}

void sciMotion::SetTarget(int newTargetX, int newTargetY)
	{
	targetX = newTargetX;
	targetY = newTargetY;
	}

void sciMotion::Init(sciActor *newClient)
	{
	int numSteps;
	int pixSpeed;

	client = newClient;
	inMotion = TRUE;
	fX = (float)client->GetX();
	fY = (float)client->GetY();
	fXDelta = (targetX - fX);
	fYDelta = (targetY - fY);
	
	pixSpeed = client->pixelSpeed;
	pixSpeed = (pixSpeed == 0) ? 25 : abs(pixSpeed);

	numSteps = (int)((int)((abs((int)fXDelta)<abs((int)fYDelta)) ? abs((int)fYDelta) : abs((int)fXDelta)) / pixSpeed);
	if (numSteps == 0)
		numSteps = 1;
	if (client->moveSpeed == 0)
		numSteps = 1; // "snap" motion when speed is 0
	fXDelta /= numSteps;
	fYDelta /= numSteps;
	moveCnt = game->GetTime();
	}

sciMoveTo::sciMoveTo(sciScript *newCaller, int newTargetX, int newTargetY)
	:sciMotion(newCaller)
	{
	SetTarget(newTargetX, newTargetY);
	}

sciMoveTo::~sciMoveTo()
	{
	}

//
void sciOrbitalMotion::Doit()
{
	POINT p;
	float r;

	if (!inMotion)
		return;

	if (abs(game->GetTime() - moveCnt) < client->moveSpeed)
		return;

	moveCnt = game->GetTime();

	if (inMotion)
	{
		if(client->moveSpeed == 0)
		{
			t = 1.0f - deltat;
		}

		t += deltat;

		// user overrideable orbital decay function.  usually f(t)
		r = DecayFunction();

		fX = ecc *	r * (float)cos(loops * 6.28 * t) + targetX;
		fY =		r * (float)sin(loops * 6.28 * t) + targetY;

		p.x = (int)fX;
		p.y = (int)fY;

//		DebugString("%f: %d %d\n", t, p.x, p.y);

		client->Posn(p.x, p.y);

		if(abs(p.x - targetX) < 5 && abs(p.y - targetY) < 5)
		{
			MotionDone();
		}
	}
}

//
void sciHermiteMotion::Doit()
{
	POINT p;

	if (!inMotion)
		return;

	if (abs(game->GetTime() - moveCnt) < client->moveSpeed)
		return;

	moveCnt = game->GetTime();

	if (inMotion)
	{
		if(client->moveSpeed == 0)
		{
			ptCurrent = (ptCount - 1) * 4;
			t = 1.0f - delta;
		}

		t += delta;

		// start next spline segment
		if(t > 1.0f + delta)
		{
			ptCurrent += 4;
			t = delta;
		}

		t2 = t * t;
		t3 = t2 * t;

// parametric cubic eqn
//		fX = cx.a * t3 + cx.b * t2 + cx.c * t + cx.d;
//		fY = cy.a * t3 + cy.b * t2 + cy.c * t + cy.d;

// Hermite spline equation
		fX =	(2 * t3 - 3 * t2 + 1)	* pts[ptCurrent].x + 
				(-2 * t3 + 3 * t2)		* pts[ptCurrent + 1].x + 
				(t3 - 2 * t2 + t)		* pts[ptCurrent + 2].x + 
				(t3 - t2)				* pts[ptCurrent + 3].x;

		fY =	(2 * t3 - 3 * t2 + 1)	* pts[ptCurrent].y + 
				(-2 * t3 + 3 * t2)		* pts[ptCurrent + 1].y + 
				(t3 - 2 * t2 + t)		* pts[ptCurrent + 2].y + 
				(t3 - t2)				* pts[ptCurrent + 3].y;

		p.x = (int)fX;
		p.y = (int)fY;

//		DebugString("%f: %d %d\n", t, p.x, p.y);

		client->Posn(p.x, p.y);

		if(abs(p.x - targetX) < 5 && abs(p.y - targetY) < 5)
		{
			MotionDone();
		}
	}
}
