#ifndef _scidlgbx
#define _scidlgbx

#include "sciplane.h"
#include "scitext.h"

class sciDialogBox : public sciPlane
{
	public:
		sciDialogBox(Boolean modal, sciPlane *parentPlane);
		~sciDialogBox();
		void Posn(int posnX, int posnY, Boolean updateWindow = FALSE);
		void Center(short dX = 0, short dY = 0);
		void SetTitleText(char *newText, int newFont = 2, int xOffset = 0, int yOffset = 0);
		virtual void DrawBackground(RECT rect);

	protected:
		sciText *titleBarText;
};

#endif //_scidlgbx