#include "os.h"
#include "core.h"
#include "sciplane.h"
#include "gifview.h"

GIFView::GIFView()
{
	celControls = NULL;
	celViews = NULL;
	lastUpdate = 0;
	curCel = 0;
	updateOnlyInForeground = FALSE;
}

GIFView::~GIFView()
{
	if (celControls) { delete[] celControls; celControls = NULL; }
	if (celViews) { delete[] celViews; celViews = NULL; }
}

void GIFView::SetView(int newView, char *fullPath, bool theUpdateOnlyInForeground)
{
	int i, offOff = sizeof(long);
	long offset;
	FILE *fptr;
	
	updateOnlyInForeground = theUpdateOnlyInForeground;
	
	if (celControls) { delete[] celControls; celControls = NULL; }
	if (celViews) { delete[] celViews; celViews = NULL; }
	
	sciView::SetView(newView);
	
	if (newView == 0)
		return;
	
	lastUpdate = 0;
	
	fptr = fopen(fullPath, "rb");
	assert(fptr);
	if (!fptr)
		return;
	
	fseek(fptr, offOff * -1, SEEK_END);
	fread(&offset, sizeof(long), 1, fptr);
	fseek(fptr, offset, SEEK_SET);
	
	celControls = new GIFControls[lastCel + 1];
	celViews = new sciView[lastCel + 1];
	curCel = 0;
	
	for (i = 0 ; i < lastCel + 1 ; i++) {
		fread(&celControls[i], sizeof(GIFControls), 1, fptr);
		celViews[i].plane = plane;
		celViews[i].Init();
		celViews[i].SetView(newView);
		celViews[i].SetCel(i);
		celViews[i].Posn(GetX(), GetY());
		celViews[i].SetPri(GetPri());
	}
	fclose(fptr);
	
	celViews[0].Show();
}

void GIFView::Posn(int posnX, int posnY)
{
	int i;
	
	sciView::Posn(posnX, posnY);
	
	if (celViews)
		for (i = 0 ; i < lastCel + 1 ; i++)
			celViews[i].Posn(posnX, posnY);
}

void GIFView::SetPri(int newPri)
{
	int i;
	
	sciFeature::SetPri(newPri);
	
	if (celViews) {
		for (i = 0 ; i < curCel + 1 ; i++)
			celViews[i].SetPri(newPri);
		for (i = curCel ; i < lastCel + 1; i++)
			celViews[i].SetPri(newPri - 1);
	}
}

void GIFView::Doit()
{
	int i;
	
	sciActor::Doit();	
	
	if (!celControls || !celViews)
		return;
	
	if (updateOnlyInForeground)
		if (plane->GetWindow()->hWnd != GetActiveWindow())
			return;
	
	lastUpdate++;
	
	if (((celControls[curCel].delay * 6) / 10) < lastUpdate) {
		switch (celControls[curCel].disposalMethod) {
			case 0:
			case 1:
				break;
			
			case 2:
			case 3:
				celViews[curCel].Hide();
				break;
			
			default:
				break;
		}
		lastUpdate = 0;
		
		curCel++;
		if (curCel > lastCel) {
			for (i = 0 ; i < lastCel + 1 ; i++) {
				celViews[i].SetPri(GetPri() - 1);
				celViews[i].Hide();
			}
			curCel = 0;
		}
		
		celViews[curCel].SetPri(GetPri());
		celViews[curCel].Show();
	}
}
