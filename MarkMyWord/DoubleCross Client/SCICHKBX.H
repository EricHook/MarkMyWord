#ifndef _scichkbx
#define _scichkbx

#include "scibuttn.h"

class sciCheckBox : public sciButton
{
	public:
		sciCheckBox(Boolean newSelected = FALSE);
		~sciCheckBox();
		void Init(int newWidth = 0, int newHeight = 0);
		void SetView(int newView);
		Boolean HandleEvent(sciEvent &event);
		virtual void Select();
		virtual void Unselect();
		Boolean IsSelected();
		void UpdateCel();
		void SetCheckBoxCels(int newNormal, int newDepressed, int newDisabled,
		 int newHilighted, int newNormalSelected, int newDepressedSelected,
		 int newDisabledSelected, int newHilightedSelected);
		void SetCheckBoxDefaultCels();
		void SetText(char *newText);
		void SetFont(int newFont, int newHiFont = -1, int newDisabledFont = -1);
		void SetTextOffset(int newXOffset, int newYOffset = 0);
		void Posn(int x, int y);
		Boolean OnMe(sciEvent &event);
		Boolean OnMe(int x, int y);
	
	protected:
		Boolean selected;

		int normalNonSelected;
		int depressedNonSelected;
		int disabledNonSelected;
		int hilightedNonSelected;
		int normalSelected;
		int depressedSelected;
		int disabledSelected;
		int hilightedSelected;

		int textXOffset;
		int textYOffset;
};

enum
{
	DEFAULT_BUTTON_NORMAL_SELECTED = 4,
	DEFAULT_BUTTON_DEPRESSED_SELECTED,
	DEFAULT_BUTTON_DISABLED_SELECTED,
	DEFAULT_BUTTON_HILIGHTED_SELECTED,
};


#endif // _scichkbx
