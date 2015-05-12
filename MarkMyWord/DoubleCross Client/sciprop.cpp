#include "os.h"
#include "sciprop.h"
#include "scicycle.h"
#include "sciscrip.h"
#include "sciplane.h"

sciProp::sciProp()
	{
	script = NULL;
	cycler = NULL;
	cycleSpeed = 4;
	}

sciProp::~sciProp()
	{
	if (plane)
		plane->DeleteFromProps(this);
	SetScript(NULL);
	SetCycle(NULL);
	}

void sciProp::Init()
	{
	sciView::Init();
	if (plane)
		{
		plane->AddToProps(this);
		}
	}

void sciProp::SetCycle(sciCycle *newCycle)
	{
	sciCycle *oldCycle;

	if (cycler)
		{
		oldCycle = cycler;
		cycler = NULL;
		delete oldCycle;
		}
	cycler = newCycle;
	if (cycler)
		cycler->Init(this);
	}

void sciProp::SetScript(sciScript *newScript)
	{
	if (script)
		delete script;
	script = newScript;
	if (script)
		script->Init(this);
	}

Boolean sciProp::HandleEvent(sciEvent &event)
	{
	return FALSE;
	}

void sciProp::Doit()
	{
	if (script)
		script->Doit();
	if (cycler)
		cycler->Doit();
	}
