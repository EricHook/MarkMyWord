#ifndef _scilstbx
#define _scilstbx

#include "sciactor.h"
#include "scitext.h"
#include "scibuttn.h"
#include "scirect.h"
#include "scigbox.h"
#include "sciscrol.h"
#include "gdefs.h"

class sciListBox;

class sciListText : public sciText
{
	public:
		sciListText();
		~sciListText();
		void SetText(char *newText);
		Boolean HandleEvent(sciEvent &event);

		void Select();
		void Unselect();
		Boolean IsSelected();
		void SetID(int newID);
		int GetID();
		void SetOwner(sciListBox *newOwner);
		void SetFonts(int newNorm, int newHighlight = -1);

	protected:
		int normalFont;
		int highlightedFont;

		sciListBox *owner;
		Boolean mouseDown;
		Boolean selected;
		int textID;
};

class ListBoxScroll : public sciScrollBox
{
	public:
		ListBoxScroll(sciListBox *newOwner);
		~ListBoxScroll();

		int GetNumItems();
		int GetMaxVisibleItems();

		void DoScroll(int direction, int numItems);
		void DoRefresh(int firstItem);

	protected:
		sciListBox *owner;
};

class sciListBox : public sciFeature
{
	public:
		sciListBox();
		~sciListBox();
		
		void Init(int newPrevView = 0, int newNextView = 0, int newScrollerView = 0);
		Boolean HandleEvent(sciEvent &event);
		void Show();
		void Hide();
		void RefreshList(int firstItem);
		void ScrollList(int direction, int numItemsToScroll);
		void SetFonts(int newNorm, int newHighlight = -1);
		void SetFont(int newFont);
		void SetTopTextPosn(int newX, int newY);
		int Size();

		void SetBoxRect(int left, int top, int right, int bottom);
		void SetBoxView(int newView);
		void SetupScroller();
		sciView *GetBoxView();
		void SetBoxPosn(int x, int y);
		void SetScrollPosn(int x, int y);
		void SetScrollHeight(int newHeight);
		void SetTrueBoxWidth(int newWidth);
		void SetTextJustX(int newJustX);
		int GetMaxVisibleItems();
	
		int AddItem(sciListText *item);
		
		void RemoveItemByString(char *itemString, Boolean caseInsensitive = TRUE);
		void RemoveItemByID(int itemID);
		void RemoveAllItems();
		
		Boolean SelectItem(int itemID);
		Boolean SelectItemByText(char *text, Boolean caseInsensitive = TRUE);
		
		void UnselectAll();
		
		sciListText *GetSelectedItem();
		sciListText *GetItemByID(int itemID);
		sciListText *GetItemByText(char *text, Boolean caseInsensitive = TRUE);
		sciListText *GetFirstVisibleItem();

		int GetSelectedItemID();
		int GetFirstVisibleItemID();
		int GetIDByText(char *text, Boolean caseInsensitive = TRUE);
		
		char *GetSelectedText();
		char *GetTextByID(int itemID);

		Boolean IsSelectedItemVisible();
		Boolean IsTextInList(char *newText, Boolean caseInsensitive = TRUE);

		virtual void DoItemSelected(int itemID);	
		void Sort(int (*compare)(sciObject *, sciObject *));
	
	protected:
		sciView			*boxView;
		ListBoxScroll	*newScroll;
		sciList			*listBoxItems;

		int firstVisibleItem;
		int maxVisibleItems;
		int numHiddenItems;

		int trueBoxWidth;

		// for when you don't have a separate view for the box
		
		RECT boxRect;
		
		// scroll information
		int scrollX;
		int scrollY;

		// text information
		int font;
		int highlightFont;
		int topTextX;
		int topTextY;
		int xOffset;
		int xJustOffset;
};

#endif // _scilstbx
