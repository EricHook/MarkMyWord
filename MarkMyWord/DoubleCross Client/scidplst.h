#if !defined(_scidroplist)
#define _scidroplist

#ifndef NOSCIPOPUPS

#include "sciscrol.h"
#include "scipopup.h"

////////////////////CLASSES//////////////////////
class sciDropList : public sciPopUpMenu
{
	public:
		sciDropList(int newType = POPUP_DROPLIST, Boolean modal = TRUE);
		~sciDropList();
		
		int GetIDOfItemInListWithIndex(int index);
		sciPopUpMenuItem *GetSelectedItem();
		BOOL CanSelectThisItem(int itemID);
		char *GetSelectedItemText();
		char *GetTextOfItemWithID(int itemID);
		char *GetTextOfItemInListWithIndex(int index);
		virtual void RemoveItemByString(char *itemString);
		virtual void RemoveAllItems();

		void ScrollMenuItems(int direction, int numItemsToScroll);
		void RefreshList(int firstItem);

};
class ComboBoxScroll;
class sciScrollDropList : public sciDropList
{
	public:
		sciScrollDropList(int newType = POPUP_SCROLL_DROPLIST, Boolean modal = TRUE);
		~sciScrollDropList();

		void Init(PopUpMenuInfo *info, int newWidth, int newHeight);
		
		void SetMaxVisibleItems(int newMaxItems);
		BOOL IsSelectedItemVisible();
		void ScrollMenuItems(int direction, int numItemsToScroll);
		void RefreshList(int firstItem);

		void RemoveItemByString(char *itemString);
		void RemoveAllItems();

		void Show();
		void JustHideThisMenu();
		BOOL OnMe(int x, int y, sciPlane *lastPlane);

		ComboBoxScroll *scroll;

		int maxVisibleItems;
		int firstVisibleItem;
};


class ComboBoxScroll : public sciScrollBox
{
	public:
		ComboBoxScroll(sciScrollDropList *newOwner);
		~ComboBoxScroll();

		BOOL OnMe(int x, int y);

		int GetNumItems();
		int GetMaxVisibleItems();

		void DoScroll(int direction, int numItems);
		void DoRefresh(int firstItem);

	protected:
		sciScrollDropList *owner;
};

#endif

#endif //_scidroplist
