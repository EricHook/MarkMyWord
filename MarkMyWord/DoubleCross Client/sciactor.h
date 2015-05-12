#ifndef _sciactor
#define _sciactor

#include "sciprop.h"

class sciMotion;

class sciActor : public sciProp
	{
	public:
		sciActor();
		~sciActor();
		void SetMotion(sciMotion *newMotion);
		sciMotion *GetMotion();
		virtual void Doit();

		int moveSpeed;
		int pixelSpeed;

	protected:
		sciMotion *mover;
	};

#endif //_sciactor
