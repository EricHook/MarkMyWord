#ifndef _scitext
#define _scitext

#include "scirect.h"
#include "sciscrip.h"
#include "texture.h"

typedef struct {
	WORD top;
	WORD left;
	WORD bottom;
	WORD right;
	} characters;

typedef struct {
	WORD bitmapID;
	WORD shadowWidth;
	WORD shadowHeight;
	short characterSpacing;
	short lineSpacing;
	WORD numChars;
	characters theChars[256];
	} FONT;

class TSprite;

enum
	{
	JUST_LEFT		= 0x01,
	JUST_RIGHT		= 0x02,
	JUST_TOP		= 0x04,
	JUST_BOTTOM		= 0x08,
	JUST_VERTICAL	= 0x10,
	JUST_HORIZONTAL	= 0x20,
	};

class sciText : public sciRect
	{
	public:
		sciText();
		~sciText();
		void ParamInit(int newFont, char *newText, int newWidth, int newHeight, int newX, int newY, 
			int newPri, int newJust = JUST_VERTICAL | JUST_HORIZONTAL);
		void SetFont(int newFont);
		virtual void SetText(char *newText);
		void SetTextToWidth(char *newText);
		void SetWidth(int newWidth);
		void SetHeight(int newHeight);
		void SetBackgroundColor(BYTE red, BYTE green, BYTE blue);
		void SetBackgroundColor(RGBQUAD color);
		void SetJust(int newJust);
		void SetJustOffsets(int x, int y);
		void SetJustEndOffsets(int x, int y);
		void SetLineSpacing(int newLineSpacing);
		virtual char* GetText();
		void Init();
		int GetMinWidth();
		int GetMinHeight();
		int CalculateFlatWidth();
		int CalculateFlatHeight();
		void AutoSize();
		virtual void Render(RECT &updateRect);
		void SetAlpha(BYTE newAlpha);
		int GetFontNumber() { return fontNumber; }
		void MoveCaret(int mouseX, int mouseY);
		void ShowCaret();
		void HideCaret();
		void DrawCaret(BOOL on);
		int GetJust() { return just; }
		int GetLineSpacing() { return customLineSpacing; }
		int GetFirstLineBreak();
		int CountLines();
		BOOL HaveFocus();
		void SetColorMap(BYTE rFrom, BYTE gFrom, BYTE bFrom, BYTE rTo, BYTE gTo, BYTE bTo);
		COLORMAPPING *GetColorMapping();
		void CopyColorMapping(COLORMAPPING *sourceMap);
		void RenderCaret(int leftPosition, int topPosition, RECT *clippedUpdateRect, int lineWidth);
		void ResetCaretIndex() {caretIndex = 0;}

		sciProp *caret;

	protected:
		unsigned char GetCharIdx(unsigned char theChar);
		void GetNextLine(int i, int *start, int *end, int *lineWidth);
		void CalculateSize(unsigned char *theText, int *theWidth, int *theHeight);

		RGBQUAD backgroundColor;
		int width, height;
		unsigned char *text;
		FONT fontResource;
		int just;
		int fontNumber;
		int customLineSpacing;
		TSprite *sprite;
		BYTE alpha;
		int justX, justY;
		int justEndX, justEndY;
		BOOL caretShown;
		BOOL caretsOn;
		char endFlag;
		int caretIndex;
		COLORMAPPING *colorMapping;
	};

class CaretScript : public sciScript
	{
	public:
		CaretScript(sciText *newT);
		void ChangeState(int newState);

	private:
		sciText *t;
	};

#endif // _scitext
