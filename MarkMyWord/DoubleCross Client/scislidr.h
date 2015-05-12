#ifndef _scislidr
#define _scislidr

#include "sciactor.h"
#include "scitext.h"

class sciSliderGauge;

class sciSlider : public sciActor
{
	public:
		sciSlider(Boolean newHorz = TRUE, Boolean newEffect = FALSE);
		~sciSlider();
		Boolean HandleEvent(sciEvent &event);
		void CreateGauge(int newViewNum, int newX, int newY, int newPri);
		void SetMinMax(int newMin, int newMax);
		int GetMin();
		int GetMax();
		virtual void DoEffect(int newValue, int otherVal);
		void SetNumDivisions(int newNumDiv);
		void SetGaugeValue(int newValue);
		int	 GetGaugeValue(void);
		void SetText(char *newText);
		void Show();
		void Hide();
		void Enable();
		void Disable();
		BOOL IsEnabled();
		
		sciText *text;
		sciSliderGauge *gauge;

		Boolean mouseDown;
		Boolean horizontal;
		int min, max; //currently assumes minimum will be at left or top of guage
		int xOffset, yOffset;

	protected:
		
		Boolean directEffect;
		int gaugeValue;
		int divisions;
		BOOL enabled;
};

class sciSliderGauge : public sciView
{
	public:
		sciSliderGauge(sciSlider *newOwner);
		~sciSliderGauge();
		Boolean HandleEvent(sciEvent &event);

		sciSlider *owner;
		BOOL mouseDown;
};
#endif //_scislidr