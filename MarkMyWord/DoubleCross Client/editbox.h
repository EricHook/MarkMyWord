#ifndef _editbox
#define _editbox

#include "scitext.h"

#define EB_NORMAL	0
#define EB_DELETING	1

class sciProp;
class sciFourSidedRect;

class EditBox : public sciText
	{
	public:
		EditBox();
		~EditBox();
		Boolean HandleEvent(sciEvent &event);

		void DoClick(BOOL netMessage = FALSE);
		
		void Posn(int posnX, int posnY);
		void Init();
		void SetSize(int width, int height);
		void SetPri(int newPri);
		void Show();
		void Hide();
		
		void Enable();
		void Disable();
		Boolean IsEnabled();
		void SetFocus();
		void LoseFocus();
		int GetTabID();
		void SetTabID(int newTabID);
		Boolean IsLastTab();
		void WantReturn(Boolean want);
		void SetLastTab(Boolean newIsLastTab);
		
		void SetMaxLength(int l);
		void SetText(char *newText);
		int GetLastRenderableChar();
		void PositionCaret();
		void CalcWidth(int *width, int mouseY);
		void MouseMove(sciEvent &event);
		void SetCaretView(int newView);
		virtual void Render(RECT &updateRect);
		void FindNewCaretIndex(char isUp);
		void HomeOrEnd(char isEnd);
		void HighLight();
		void SetHighLightColor(BYTE red, BYTE green, BYTE blue);
		void SetHighLightColor(RGBQUAD color);
		void Set3DOutLine(Boolean);

		HWND hwnd;
		FourSidedRect *tdOutLine;
		int status;

	protected:
		Boolean isUp;
		Boolean enabled;
		Boolean mouseDown;
		Boolean wantReturn;
		int maxLength;
		Boolean tdShown;
		BOOL capturing;
		int tabID;
		Boolean isLastTab;
		unsigned int startSel;
		unsigned int endSel;
		int realStart;
		sciRect *highLights;
		RGBQUAD rgbHighLightColor;
		DWORD timer;
	};

#endif // _editbox
