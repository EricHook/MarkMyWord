#ifndef _sciprop
#define _sciprop

#include "sciview.h"

class sciScript;
class sciCycle;

class sciProp : public sciView
	{
	public:
		sciProp();
		~sciProp();
		void Init();
		void SetCycle(sciCycle *newCycle);
		void SetScript(sciScript *newScript);
		Boolean HandleEvent(sciEvent &event);
		virtual void Doit();

		int cycleSpeed;
		sciScript *script;
		sciCycle *cycler;

	protected:
		
	private:
	};

#endif //_sciprop
