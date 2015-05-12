#ifndef _sciroom
#define _sciroom

#include "scievent.h"
#include "scilist.h"

class sciPlane;
class sciScript;

class sciRoom : public sciObject
	{
	public:
		sciRoom();
		virtual ~sciRoom();
		virtual void Init();
		virtual Boolean HandleEvent(sciEvent &event);
		void SetScript(sciScript *newScript);
		virtual void Doit();
		void SetPic(int picNumber);

		sciScript *script;

	protected:
		int picNumber;

	private:
	};


#endif //_sciroom
