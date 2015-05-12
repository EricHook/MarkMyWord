#ifndef _gifview
#define _gifview

#include "sciactor.h"
#include "gifcontrol.h"

class GIFView : public sciActor
{
	public:
		GIFView();
		~GIFView();
		
		void SetView(int newView, char *fullPath, bool theUpdateOnlyInForeground);
		void Posn(int posnX, int posnY);
		void SetPri(int newPri);
		void Doit();
	
	protected:
		GIFControls *celControls;
		sciView *celViews;
		int lastUpdate;
		int curCel;
		bool updateOnlyInForeground;
};

#endif //_gifview
