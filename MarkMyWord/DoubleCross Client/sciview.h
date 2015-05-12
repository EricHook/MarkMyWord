#ifndef _sciview
#define _sciview

#include "scifeatr.h"
#include "texture.h"

class TSprite;
class ResourceManager;

class sciView : public sciFeature
	{
	public:
		sciView();
		~sciView();
		void Init();
		void ParamInit(int newView, int newX, int newY, int newPri);
		void SetView(int newView);
		void SetLoop(int newLoop);
		void SetCel(int newCel);
		int GetCel();
		int GetLoop();
		int GetView();
		int LastCel();
		int LastLoop();
		void Render(RECT &updateRect);
		void Posn(int posnX, int posnY);
		void Show();
		void Hide();
		void SetAlpha(BYTE newAlpha);
		BYTE GetAlpha();
		void SetTransferMode(int mode);
		int GetTransferMode();
		virtual Boolean OnMe(int x, int y);
		void UseColorOfPoint(BOOL b);
		virtual int GetX();
		virtual int GetY();
		int GetBitmapX();
		int GetBitmapY();
		RECT GetRectNoAlpha();
		void SetGamma(double g);

		void SetHalfSize(bool onoff, SCALE_TYPE st = ST_BILINEAR);

		void SetColorMap(BYTE rFrom, BYTE gFrom, BYTE bFrom, BYTE rTo, BYTE gTo, BYTE bTo);
		COLORMAPPING *GetColorMapping();
		void CopyColorMapping(COLORMAPPING *sourceMap);
		
		// Someone else has to delete resourceManager;
		ResourceManager *resourceManager;

	protected:
		void FigureTrim(int &trimLeft, int &trimTop, int &trimRight, int &trimBottom);

		int view;
		int loop;
		int cel;
		TSprite *sprite;
		RECT rcWindow;
		int lastCel;
		int lastLoop;
		BYTE alpha;
		BOOL useColorOfPoint;
		int x, y;
		int transferMode;

		bool displayHalfSize;
		SCALE_TYPE scaleType;
		
		COLORMAPPING *colorMapping;
	};

#endif //_sciview
