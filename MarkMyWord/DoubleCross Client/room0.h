#ifndef _room0
#define _room0

class Room0 : public sciRoom
	{
	public:
		Room0();
		~Room0();
		
		void Init();
		void Doit();
		Boolean HandleEvent(sciEvent &event);
	};

class Room0Script : public sciScript
	{
	public:
		void ChangeState(int newState);
	};

#endif
