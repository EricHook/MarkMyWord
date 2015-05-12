#include "os.h"
#include "scislidr.h"


sciSlider::sciSlider(Boolean newHorz, Boolean newEffect)
{
	horizontal		= newHorz;
	directEffect	= newEffect;
	mouseDown		= FALSE;
	gaugeValue		= 0;
	gauge			= NULL;
	text			= NULL;
	enabled			= TRUE;
}
sciSlider::~sciSlider()
{
	if(gauge)		{delete gauge; gauge = NULL;}
	if(text)		{delete text, text = NULL;}
}

void sciSlider::CreateGauge(int newViewNum, int newX, int newY, int newPri)
{
	if(gauge)
		delete gauge;
	gauge = new sciSliderGauge(this);
	gauge->plane = plane;
	gauge->Init();
	gauge->SetView(newViewNum);
	gauge->Posn(newX, newY);
	gauge->SetPri(newPri);
}

void sciSlider::SetMinMax(int newMin, int newMax)
{
	min = newMin;
	max = newMax;
}

int sciSlider::GetMin()
{
	return(min);
}

int sciSlider::GetMax()
{
	return(max);
}

void sciSlider::SetText(char *newText)
{
	if(text)
		delete text;

	text = new sciText();
	if(text)
	{
		text->SetText(newText);
		text->plane = plane;
		text->Init();
	}
}

void sciSlider::Show()
{
	sciActor::Show();
	if (text) {
		text->Show();
	}
	if (gauge) {
		if (horizontal) {
			Posn(min + gaugeValue, GetY());
		} else {
			Posn(GetX(), min + gaugeValue);
		}
		gauge->Show();
	}
}

void sciSlider::Hide()
{
	sciActor::Hide();
	if (text) {
		text->Hide();
	}
	if (gauge) {
		gauge->Hide();
	}
}

void sciSlider::Enable()
{
	enabled = TRUE;
}
void sciSlider::Disable()
{
	enabled = FALSE;
}
BOOL sciSlider::IsEnabled()
{
	return enabled;
}

void sciSlider::SetGaugeValue(int newValue)
{
	if (newValue < 0) {
		gaugeValue = 0;
	} else if (newValue > abs(max - min)) {
		gaugeValue = abs(max - min);
	} else {
		gaugeValue = newValue;
	}
}
int sciSlider::GetGaugeValue(void)
{
	return gaugeValue;
}

void sciSlider::DoEffect(int newValue, int otherVal)
{
}

Boolean sciSlider::HandleEvent(sciEvent &event)
{
	if ((!sciFeature::OnMe(event) && !mouseDown) || event.claimed || !IsEnabled())
		return FALSE;
	
	switch (event.type)
	{
		case MOUSE_DOWN:
			if (OnMe(event.x, event.y) && event.modifiers & RIGHT_MOUSE)
				event.claimed = TRUE;
		
			if (OnMe(event.x, event.y) && (event.modifiers & LEFT_MOUSE) && !mouseDown)
			{
				mouseDown = TRUE;
				event.claimed = TRUE;
				if(horizontal)
					xOffset = event.x - GetX();
				else
					yOffset = event.y - GetY();
			}
			break;
		case MOUSE_MOVE:
			if (mouseDown)
			{
				if(horizontal)
				{
					if(event.x - xOffset < min)
						Posn(min, GetY());
					else
					{
						if(event.x - xOffset > max)
							Posn(max, GetY());
						else
							Posn(event.x - xOffset, GetY());
					}
					DoEffect(GetX() - min, GetX());
				}
				else
				{
					if(event.y - yOffset < min)
						Posn(GetX(), min);
					else
					{
						if(event.y - yOffset > max)
							Posn(GetX(), max);
						else
							Posn(GetX(), event.y - yOffset);
					}
					DoEffect(GetY() - min, GetY());
				}
				
				event.claimed = TRUE;
			}
			break;
		case MOUSE_UP:
			if ((event.modifiers & LEFT_MOUSE) && mouseDown)
			{
				if(horizontal)
					SetGaugeValue(GetX() - min);
				else
					SetGaugeValue(GetY() - min);
				mouseDown = FALSE;
				event.claimed = TRUE;
			}
			break;
	}
	return event.claimed;
}

sciSliderGauge::sciSliderGauge(sciSlider *newOwner)
{
	mouseDown = FALSE;
	owner = newOwner;
}
sciSliderGauge::~sciSliderGauge()
{
}
Boolean sciSliderGauge::HandleEvent(sciEvent &event)
{
	if(!sciView::OnMe(event.x,event.y) || event.claimed)
		return event.claimed;

	switch (event.type)
	{
		case MOUSE_DOWN:
			if(!owner->OnMe(event.x,event.y) && owner->IsEnabled())
			{
				if(owner->horizontal)
				{
					owner->SetGaugeValue(event.x - owner->min - (owner->GetWidth() / 2));
					owner->DoEffect(event.x - owner->min, event.x);
				}
				else
				{
					owner->SetGaugeValue(event.y - owner->min - (owner->GetHeight() / 2));
					owner->DoEffect(event.y - owner->min, event.y);
				}
				owner->mouseDown = TRUE;
				owner->Show();
				if(owner->horizontal)
					owner->xOffset = event.x - owner->GetX();
				else
					owner->yOffset = event.y - owner->GetY();

			}
			break;
	}

	return event.claimed;
}