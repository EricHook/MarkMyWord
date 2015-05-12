#ifndef _scibtngp
#define _scibtngp

#include "scilist.h"
#include "scichkbx.h"

#define TABGROUP_MAXROWS		5
#define TABGROUP_MAXTABSPERROW	7
#define TABGROUP_SELECTEDPRI	100

class sciRadioButton : public sciCheckBox
{
	public:
		sciRadioButton(int newID, Boolean newSelected = FALSE);
		~sciRadioButton();
		Boolean HandleEvent(sciEvent &event);	
		int GetID();
		void SetID(int newID);
		void SetButtonGroupList(sciList *newButtonList);
		virtual void GotSelected() { }
	
	protected:
		int buttonID;
		sciList *buttonGroupList;		
};

class sciButtonGroup : public sciView
{
	public:
		sciButtonGroup();
		~sciButtonGroup();
		
		Boolean AddToGroup(sciRadioButton *newButton);
		sciRadioButton *GetSelectedButton();
		int GetSelectedButtonID();
		void SelectButtonByID(int newID);
		void EnableAll();
		void DisableAll();
		void UnselectAll();
		
		void Show();
		void Hide();
	
	protected:
		sciList *buttonList;

};

class sciTabGroup;

class sciTab : public sciRadioButton
{
	public:
		sciTab(int newID, int newLocation, int newSubGroupID = 0, Boolean newSelected = FALSE);
		~sciTab();
		void Init();
		void SetView(int newView);
		void AddToTab(sciFeature *newFeature);
		void RemoveFromTab(sciFeature *feature);
		void Select();
		void Unselect();
		void SetXPri(short newXPri);
		short GetXPri();
		int GetSubGroupID();
		int GetLocation();
		void SetOwner(sciTabGroup *newOwner);
		void UpdateCel();
		void SetText(char *newText);
		void SetFont(int newFont, int newHiFont = -1, int newDisabledFont = -1);
		Boolean HandleEvent(sciEvent &event);
	
	protected:
		sciList *featureList;
		sciTabGroup *owner;
		short xPri;
		int subGroupID; // unused
		int location; // unused
};

class sciTabGroup : public sciButtonGroup
{
	public:
		sciTabGroup();
		~sciTabGroup();
		Boolean AddToGroup(sciTab *newTab);
		void SetTabFonts(int newNormal, int newNormalHighlighted, int newBold, int newBoldHighlighted);
		// void SetNumRows(int newNumRows, tabINFO newTabInfoArray[][TABGROUP_MAXTABSPERROW]);
		// void AdvanceRow(int whichSubGroup, Boolean bForceUpdate = FALSE);
		void BringRowToBottom(sciTab *tab);
		
		//font types
		int normal;
		int normalHighlighted;
		int bold;
		int boldHighlighted;
};

#endif //_scibtngp
