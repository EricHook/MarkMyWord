#ifndef _sciplane
#define _sciplane

#include "sciobj.h"
#include "scilist.h"
#include "window.h"
#include "texture.h"

class sciFeature;
class sciEvent;
class sciProp;
class sciView;
class sciRect;
class EditBox;

class sciPlane : public sciObject
	{
	public:
		sciPlane(Boolean newModal = FALSE, sciPlane *newParentPlane = NULL, int width = 640, int height = 480, Boolean closeBox = FALSE);
		~sciPlane();
		void AddToCast(sciFeature *feature);
		void DeleteFromCast(sciFeature *feature);
		void AddToProps(sciProp *prop);
		void DeleteFromProps(sciProp *prop);
		virtual Boolean HandleEvent(sciEvent &event);
		virtual void Doit();
		void Render(RECT &updateRect);
		void Show();
		void Hide();
		void SetPic(int newPicNumber, int newCelNumber = 0);
		int GetPicNumber();
		void Posn(int x, int y, BOOL updateWindow = TRUE);
		int GetX();
		int GetY();
		int GetWidth();
		int GetHeight();
		void SetSize(int w, int h);
		Boolean IsModal();
		void SetDraggableArea(int newDraggableArea);
		void CaptureEvents(sciFeature *feature);
		BOOL IsHidden();
		BOOL IsNotHidden();
		int GetCursorX();
		int GetCursorY();
		void SetBackgroundColor(RGBA color);
		void UnsetBackgroundColor();
		virtual void DrawBackground(RECT rect);
		void DrawPixel(int x, int y, RGBA color, int transferMode);
		void DrawHLine(int x1, int x2, int y, RGBA color, int transferMode);
		void DrawVLine(int x, int y1, int y2, RGBA color, int transferMode);
		void DrawClosedRect(int x1, int y1, int x2, int y2, RGBA color, int transferMode);
		void MarkAsUpdated(const RECT &rect);
		void Delete();
		BOOL IsToBeDeleted();
		void SetViewGamma(double gamma);
		Window *GetWindow() { return window; }
		sciPlane *GetParentPlane() { return parentPlane; }
		virtual void AttemptClosePlane();
		EditBox *GetEditFocus();
		EditBox *SetEditFocus(EditBox *editBox);
		BOOL IsEditFocusPlane() { return isEditFocusPlane; }
		void SetEditFocusPlane(BOOL isFocus) { isEditFocusPlane = isFocus; }
	
	protected:
		void LoadPic();
		void MarkAsUpdated();

		int		top;
		int		left;
		int		bottom;
		int		right;
		sciList	cast;	// Used to pass HandleEvents()s along.
		sciList props;	// Used to pass Doit()s along.
		int		isNotHidden;
		int		picNumber;
		int		celNumber;
		BOOL	itsMyWindow;
		BOOL	modal;
		sciFeature *capturingEventsFeature;
		int cursorX, cursorY;
		sciPlane *prevModalPlane;
		RGBA	backgroundColor;
		BOOL	useBackgroundColor;
		sciView	*background;
		int		draggableArea;
		BOOL	mouseDown;
		int		xOffset;
		int		yOffset;
		sciRect	*dimmingRect;
		BOOL	toBeDeleted;
		sciPlane *parentPlane;
		Window	*window;
		EditBox *editFocus;
		BOOL isEditFocusPlane;
	};

#endif // _sciplane
