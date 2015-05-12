#ifndef _sciscrip
#define _sciscrip

#include "sciobj.h"

class sciEvent;

class sciScript : public sciObject
	{
	public:
		sciScript(sciScript *newCaller = NULL);
		~sciScript();
		void Init(sciObject *newClient = NULL);
		virtual void ChangeState(int newState);
		void Cue();
		void SetScript(sciScript *newScript);
		void SetSeconds(int seconds);
		virtual void SetTicks(int ticks);
		virtual void UnSetTicks();
		void SetState(int newState);
		virtual void SetNextState(int newState);
		int GetState();
		void Doit();

		sciScript *script;
		sciScript *callee;	// Script doing the cueing when its done.
							// Counter part to caller.

	protected:
		sciObject *client;	// Object this script is attached to.
		sciScript *caller;	// Script to be cued when this
							// script is completed.
		int state;
		int nextTicks;
	private:
	};

#endif //_sciscrip
