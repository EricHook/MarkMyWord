#ifndef _scifeature
#define _scifeature

#include "scievent.h"

class sciPlane;
class sciList;

class sciFeature : public sciObject
	{
	public:
		sciFeature();
		~sciFeature();
		virtual void Init();
		void ParamInit(int newX, int newY, int newPri);
		virtual Boolean HandleEvent(sciEvent &event);
		Boolean OnMe(sciFeature &obj);
		Boolean OnMe(sciEvent &event);
		virtual Boolean OnMe(int x, int y);
		virtual void SetPri(int newPri);
		int GetPri();
		virtual void Hide();
		virtual void Show();
		virtual void HideOffScreen();
		virtual void ShowOnScreen();
		virtual void Render(RECT &updateRect);
		virtual void Posn(int x, int y);
		virtual void CenterInPlane();
		virtual void SetSize(int width, int height);
		virtual int GetX();
		virtual int GetY();
		int GetWidth();
		int GetHeight();
		void SetWidth(int width);
		void SetHeight(int height);
		Boolean IsNotHidden();
		Boolean IsHidden();

		sciPlane *plane;
		sciList *cast;

	protected:
		void MarkAsUpdated();
		void MarkAsUpdated(const RECT &rect);

		int left;
		int top;
		int right;
		int bottom;
		int priority;
		Boolean hidden;
		int inited;
	};

#endif //_scifeature
