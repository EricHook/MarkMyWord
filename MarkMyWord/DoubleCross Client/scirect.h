#ifndef _scirect
#define _scirect

#include "scifeatr.h"

class sciRect : public sciFeature
	{
	public:
		sciRect();
		~sciRect();
		virtual void Render(RECT &updateRect);
		virtual void Posn(int x, int y);
		virtual void SetSize(int width, int height);
		virtual void Posn(const RECT &newpos);
		virtual void Hide();
		virtual void Show();
		void SetInnerColor(BYTE red, BYTE green, BYTE blue);
		void SetOutlineColor(BYTE red, BYTE green, BYTE blue);
		void ClearOutlineColor();
		void SetInnerColor(RGBQUAD color);
		void SetOutlineColor(RGBQUAD color);
		void SetAlpha(BYTE newAlpha);
		BYTE GetAlpha();
		void SetTransferMode(int mode);
		int GetTransferMode();

	protected:
		BYTE innerColor;
		BYTE outlineColor;
		RGBQUAD rgbInnerColor;
		RGBQUAD rgbOutlineColor;
		BYTE alpha;
		int transferMode;
	};

class FourSidedRect : public sciFeature
	{
	public:
		FourSidedRect();
		~FourSidedRect();
		void Init();
		void SetSize(int width, int height);
		void Posn(int x, int y);
		void SetPri(int newPri);
		void Show();
		void Hide();
		void SetSideColors(RGBQUAD leftC, RGBQUAD topC, RGBQUAD rightC, RGBQUAD bottomC);
	
	protected:
		sciRect *sides[4];
	};

class EmbossedRect : public sciFeature
{
	public :
		EmbossedRect();
		~EmbossedRect();
		void Init();
		void Posn(int x, int y);
		void SetSize(int width, int height);
		void SetPri(int newPri);
		void Show(Boolean showFiller = TRUE);
		void Hide();

		void SetFrame1Color(BYTE red, BYTE green, BYTE blue);
		void SetFrame2Color(BYTE red, BYTE green, BYTE blue);
		void SetFillerColor(BYTE red, BYTE green, BYTE blue);
		void SetFrame1Color(RGBQUAD color);
		void SetFrame2Color(RGBQUAD color);
		void SetFillerColor(RGBQUAD color);

	protected:
		sciRect *frame1;
		sciRect *frame2;
		sciRect *filler;

		RGBQUAD frame1Color;
		RGBQUAD frame2Color;
		RGBQUAD fillerColor;
};


#endif //_scirect
