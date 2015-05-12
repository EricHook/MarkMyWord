#ifndef _tickertext
#define _tickertext

#include "scitext.h"
#include "sciprop.h"

class TickerSegment;

class TickerText : public sciProp
{
public:
	TickerText();
	~TickerText();
	
	void Init(int theVOffset, int theLeftInset, int theRightInset);
	void SetPri(int newPri);
	void Show();
	void Hide();
	
	void SetSegments(char *segments);
	bool AddSegment(char *segment);
	bool DeleteSegment(int index);
	char *GetToken(char *s);
	
	void Cycle();
	
	int GetNumTexts() { return numTexts; }
	
	int speed;
	int currentSegment;
	
protected:
	int numTexts;
	TickerSegment **texts;
	int vOffset;
	int leftInset;
	int rightInset;
	
	/*
	bool capturing;
	int startPixel;
	int lastMove;
	int increment;
	int lastMousePos;
	*/
};

class TickerSegment : public sciText
{
public:
	TickerSegment();
	~TickerSegment();
	
	void SetMaxLoops(int newMax);
	void IncrNumLoops();
	int GetNumLoops();
	int GetMaxLoops();
	
protected:
	int maxLoops;
	int numLoops;
};

class TickerScript : public sciScript
{
public:
	TickerScript(TickerText *newT);
	void ChangeState(int newState);
	
private:
	TickerText *t;
};

#endif // _tickertext
