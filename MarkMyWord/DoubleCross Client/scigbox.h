#ifndef _scigbox
#define _scigbox

#include "scifeatr.h"

class sciRect;
class sciText;

class sciGroupBox : public sciFeature
	{
	public:
		sciGroupBox();
		~sciGroupBox();
		virtual void Posn(int x, int y);
		virtual void SetSize(int width, int height);
		virtual void Init();
		virtual void Show();
		virtual void Hide();
		virtual void SetPri(int newPri);
		void SetInnerColor(BYTE red, BYTE green, BYTE blue);
		void SetOutlineColor1(BYTE red, BYTE green, BYTE blue);
		void SetOutlineColor2(BYTE red, BYTE green, BYTE blue);
		void SetInnerColor(RGBQUAD color);
		void SetOutlineColor1(RGBQUAD color);
		void SetOutlineColor2(RGBQUAD color);
		void SetText(char *newText);
		void SetTextPri(int newPri);
		void SetFont(int fontNumber);

	protected:
		RGBQUAD innerColor;
		RGBQUAD outlineColor1;
		RGBQUAD outlineColor2;
		sciRect *outline1;
		sciRect *outline2;
		sciText *text;
		char *str;
		int fontNumber;
	};

#endif //_scigbox
