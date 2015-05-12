#include "os.h"
#include "sciactor.h"
#include "scimotn.h"

sciActor::sciActor()
	{
	mover = NULL;
	moveSpeed = 4;
	pixelSpeed = 25;
	}

sciActor::~sciActor()
	{
	SetMotion(NULL);
	}

void sciActor::SetMotion(sciMotion *newMotion)
	{
	if (mover)
		{
		//mover->MotionDone();	// Do this??? // no: it will cause an infinite loop
		delete mover;
		}
	mover = newMotion;
	if (mover)
		mover->Init(this);
	}

sciMotion *sciActor::GetMotion()
	{
	return mover;
	}

void sciActor::Doit()
	{
	sciProp::Doit();
	if (mover)
		mover->Doit();
	}
