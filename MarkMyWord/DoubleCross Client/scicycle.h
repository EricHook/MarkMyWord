#ifndef _scicycle
#define _scicycle

#include "sciobj.h"

class sciProp;
class sciScript;

#define CYCLEDIR_FORWARD 1
#define CYCLEDIR_REVERSE -1

class sciCycle : public sciObject
	{
	public:
		sciCycle(sciScript *newCaller = NULL);
		~sciCycle();
		virtual void Init(sciProp *newClient);
		virtual int NextCel();
		virtual void CycleDone();
		virtual void Doit();
		sciProp *client;
		sciScript *caller;

	protected:
		int cycleDir;
		int cycleCnt;
		int clientLastCel;
		Boolean inMotion;
	};

class sciForward : public sciCycle
	{
	public:
		sciForward(sciScript *newCaller = NULL);
		~sciForward();
		void Doit();
		void CycleDone();
	};

class sciForwardWithCue : public sciForward
	{
	public:
		sciForwardWithCue(sciScript *newCaller = NULL);
		~sciForwardWithCue();
		void CycleDone();
	};

class sciReverse : public sciCycle
	{
	public:
		sciReverse(sciScript *newCaller = NULL);
		~sciReverse();
		void Doit();
		void CycleDone();
	};

class sciReverseWithCue : public sciReverse
	{
	public:
		sciReverseWithCue(sciScript *newCaller = NULL);
		~sciReverseWithCue();
		void CycleDone();
	};

class sciCycleTo : public sciCycle
	{
	public:
		sciCycleTo(sciScript *newCaller = NULL);
		~sciCycleTo();
		void Doit();
		void SetEndCel(int iNewEnd);
	protected:
		int endCel;
	};

class sciCycleForwardTo : public sciCycleTo
	{
	public:
		sciCycleForwardTo(sciScript *newCaller = NULL);
		sciCycleForwardTo(int newEndCel, sciScript *newCaller = NULL);
		~sciCycleForwardTo();
	};

class sciCycleReverseTo : public sciCycleTo
	{
	public:
		sciCycleReverseTo(sciScript *newCaller = NULL);
		sciCycleReverseTo(int newEndCel, sciScript *newCaller = NULL);
		~sciCycleReverseTo();
	};

class sciBegLoop : public sciCycleTo
	{
	public:
		sciBegLoop(sciScript *newCaller = NULL);
		virtual void Init(sciProp *newClient);
	};

class sciEndLoop : public sciCycleTo
	{
	public:
		sciEndLoop(sciScript *newCaller = NULL);
		virtual void Init(sciProp *newClient);
	};

class sciOscillate : public sciCycle
	{
	public:
		sciOscillate();
		~sciOscillate();
		void Doit();
		void CycleDone();
	};

class sciFlash : public sciCycle
	{
	public:
		sciFlash(int newNumFlashes = 0, BOOL newEndShown = TRUE, Boolean constntFlash = FALSE);
		~sciFlash();
		BOOL FlashNow();
		void Doit();
		void CycleDone();

		int numFlashes;
		BOOL endShown;
		Boolean constantFlash;
	};


class sciRandomCycle : public sciCycle
{
public:
	sciRandomCycle();
	~sciRandomCycle();
	void Doit();
	int GetACel();
};

class sciToggle : public sciCycle
{
public:
	sciToggle();
	sciToggle(int theCelOne, int theCelTwo);
	~sciToggle();
	void Doit();
	void SetCelOne(int c);
	void SetCelTwo(int c);
protected:
	int celOne,celTwo;
};

#endif // _scicycle






