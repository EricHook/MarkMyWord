#ifndef _globals
#define _globals

#include "app.h"
#include "game.h"
#include "sci.h"
#include "chat.h"
#include "room0.h"

extern Application *pApp;
extern Game *game;
extern RGBQUAD DEFAULT_GREY;
extern RGBQUAD DEFAULT_LTGREY;
extern RGBQUAD DEFAULT_DKGREY;
extern char szAppName[];
extern char szAppTitle[];
extern HINSTANCE hInstance;
extern HINSTANCE hResourceInstance;
extern HINSTANCE hGameResourceInstance;
extern BOOL hdVersion;
extern BOOL demoVersion;
extern Room0 *room0;

typedef struct {
	char type[4];
	int resourceID;
	int directory;
	int resourceIDToUse;
} ResourceMapping;

extern int numResourceDirectories;
extern char resourceDirectories[16][MAX_PATH];

extern int numResourceMappings;
extern ResourceMapping resourceMappings[1000];

#endif
