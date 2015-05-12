#include "os.h"
#include "app.h"
#include "inifile.h"

#include "object.h"

extern Application *pApp;

int Object::numObjects = 0;
int Object::nextObjectNum = 0;

#ifdef _XDEBUG

typedef struct {
	int myNum;
	int total;
	Object *object;
} OARec;

static Boolean monitorObjectAllocations = FALSE;
static OARec *objectsOut = NULL;

#endif

Object::Object()
	{
	++numObjects;
	++nextObjectNum;
	objectNum = nextObjectNum;
#ifdef _XDEBUG
	int i;
	
	if (monitorObjectAllocations && objectsOut) {
		for (i = 0 ; i < 10000 ; i++)
			if (objectsOut[i].object == NULL) {
				objectsOut[i].myNum = objectNum;
				objectsOut[i].total = numObjects;
				objectsOut[i].object = this;
				break;
			}
		if (i == 10000)
			DebugString("OUT OF OBJECTSOUT SPACE\n");
	}
#endif
	}

Object::~Object()
	{
#ifdef _XDEBUG
	int i;
	
	if (objectsOut) {
		for (i = 0 ; i < 10000 ; i++)
			if (objectsOut[i].object == this) {
				objectsOut[i].myNum = -1;
				objectsOut[i].total = -1;
				objectsOut[i].object = NULL;
				break;
			}
		if (i == 10000)
			; // DebugString("OBJECT BEING DELETED NOT FOUND IN OBJECTSOUT\n");
	}
#endif
	--numObjects;
	objectNum = 0;
	}

#ifdef _XDEBUG

void ResetObjectsMonitor()
{
	short i;
	
	if (objectsOut)
		{ free(objectsOut); objectsOut = NULL; }
	
	if (pApp->pIniFile->ReadValue("MonitorAllocations", FALSE)) {
		objectsOut = (OARec *)malloc(10000 * sizeof(OARec));
		if (!objectsOut)
			DebugString("OBJECTSOUT COULD NOT BE ALLOCATED\n");
		else
			for (i = 0 ; i < 10000 ; i++) {
				objectsOut[i].myNum = -1;
				objectsOut[i].total = -1;
				objectsOut[i].object = NULL;
			}
	}
	
	monitorObjectAllocations = FALSE;
}

void MonitorObjectAllocations(Boolean monitor)
{
	monitorObjectAllocations = monitor;
}

void PrintUndisposedObjectNumbers()
{
	char s[128];
	short i;
	
	if (objectsOut) {
		for (i = 0 ; i < 10000 ; i++)
			if (objectsOut[i].object != NULL) {
				wsprintf(s, "~~~Object not deleted by room: #%ld total: %ld (0x%d)\n",
							(int)objectsOut[i].myNum,
							(int)objectsOut[i].total,
							(int)objectsOut[i].object);
				DebugString(s);
			}
		free(objectsOut);
		objectsOut = NULL;
	}
	monitorObjectAllocations = FALSE;
}

#else

void ResetObjectsMonitor()
{
}

void MonitorObjectAllocations(Boolean monitor)
{
}

void PrintUndisposedObjectNumbers()
{
}

#endif
