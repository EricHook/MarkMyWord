#ifndef _scibuttn
#define _scibuttn

#include "sciprop.h"
#include "scitext.h"

class sciButton : public sciProp
	{
	public:
		sciButton();
		~sciButton();
		virtual void Init(int newWidth = 0, int newHeight = 0);
		void ParamInit2(int newFont, char *newText, int newView, int newX, int newY, int newPri);
		Boolean HandleEvent(sciEvent &event);
		virtual void Enable();
		virtual void Disable();
		Boolean IsEnabled();
		virtual void DoClick(BOOL netMessage = FALSE);
		void SetHotkey(char c, bool deferToEditing = FALSE);
		void SetHotkey2(char c, bool deferToEditing = FALSE);
		void SetRightHotkey(char c, bool deferToEditing = FALSE);
		void SetButtonCels(int newNormal, int newDepressed, int newDisabled, int newHilighted);
		void SetButtonDefaultCels();
		void SetRepeat(int newStartDelay = -1, int newRepeatDelay = -1);
		virtual void DoRightClick();
		friend class sciButtonRepeatScript;
		virtual void SetText(char *newText);
		char *GetText();
		void SetLineSpacing(int newLineSpacing);
		virtual void SetFont(int newFont, int newHiFont = -1, int newDisabledFont = -1);
		void ToggleFont(Boolean bHilighted);
		virtual void SetView(int newView);
		void SetPri(int newPri);
		void Posn(int x, int y);
		void TextPosn(int newTextXOffset, int newTextYOffset);
		int GetTextFontNumber();
		void SetTextJust(int newJust);
		void SetJustOffsets(int x, int y);
		void SetAlpha(BYTE newAlpha);
		void Show();
		void Hide();
		void SetState(int newState);
		int  GetState(void);
	
	protected:
		virtual void UpdateCel();

		sciText *text;
		Boolean mouseDown;
		Boolean enabled;
		char hotkey, hotkey2;
		bool hkDeferToEditing, hk2DeferToEditing;
		char rightHotkey;
		bool rhkDeferToEditing;
		int justification;
		int normal;
		int depressed;
		int disabled;
		int hilighted;
		int state;
		int startDelay;
		int repeatDelay;
		int normalFont;
		int hilightedFont;
		int disabledFont;
		RECT textArea;
		int justX, justY;
		int textXOffset, textYOffset;
	};

enum
	{
	DEFAULT_BUTTON_NORMAL = 0,
	DEFAULT_BUTTON_DEPRESSED,
	DEFAULT_BUTTON_DISABLED,
	DEFAULT_BUTTON_HILIGHTED,
	};

#endif // _scibuttn
