#ifndef _scimotn
#define _scimotn

#include "sciobj.h"

class sciActor;
class sciScript;

class sciMotion : public sciObject
	{
	public:
		sciMotion(sciScript *newCaller = NULL);
		~sciMotion();
		virtual void Doit();
		virtual void MotionDone();
		void SetTarget(int newTargetX, int newTargetY);
		virtual void Init(sciActor *newClient);

	protected:
		sciActor *client;
		sciScript *caller;
		Boolean inMotion;
		int targetX, targetY;
		float fX, fY;
		float fXDelta, fYDelta;
		int moveCnt;
	};

class sciMoveTo : public sciMotion
	{
	public:
		sciMoveTo(sciScript *newCaller = NULL, int newTargetX = 0, int newTargetY = 0);
		~sciMoveTo();
	};

class sciOrbitalMotion : public sciMoveTo
{
protected:
	int radius;
	float t;
	float loops;
	int steps;
	float deltat;
	float ecc;

public:
	sciOrbitalMotion(sciScript *newCaller, int newTargetX, int newTargetY, int aradius = 100)
		: sciMoveTo(newCaller, newTargetX, newTargetY)
	{
		t = 0.0f;
		loops = 1.0f;
		radius = aradius;
		steps = 10;
		deltat = 1 / (float)steps;
		ecc = 1.0f;
	}
	~sciOrbitalMotion()			{}
	virtual float DecayFunction()	{ return radius * (1-t*t); }
	void SetLoops(float nloops)	{ loops = nloops; }
	void SetRadius(int aradius)	{ radius = aradius; }
	void SetSteps(int nsteps)	{ steps = nsteps; deltat = 1 / (float)steps; }
	void Doit();
	void SetEcc(float anecc)	{ ecc = anecc; }
};

class sciHermiteMotion : public sciMoveTo
{
protected:
	POINT *pts;
	int ptCount;
	int ptCurrent;
	float t, t2, t3;
	float delta;
	int steps;

public:
	// arrayOfPoints is splineSegmentCount * 4
	// splineSegmentCount
	sciHermiteMotion(sciScript *newCaller, POINT *arrayOfPoints, int splineSegmentCount) 
		: sciMoveTo(newCaller, arrayOfPoints[4*(splineSegmentCount-1)+1].x, arrayOfPoints[4*(splineSegmentCount-1)+1].y)
	{ 
		pts = arrayOfPoints;
		ptCount = splineSegmentCount;
		t = t2 = t3 = 0; 
		steps = 10; 
		delta = 1 / (float)steps;
		ptCurrent = 0;
	}
	~sciHermiteMotion() {}
	void SetSteps(int n)
	{
		steps = n;
		delta = 1 / (float)n;
	}
	void Doit();
	void Init(sciActor *newClient)
	{
		sciMoveTo::Init(newClient);
	}
};


#endif // _scimotn
