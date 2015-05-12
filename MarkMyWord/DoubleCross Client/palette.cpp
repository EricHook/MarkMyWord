#include "os.h"

#include "app.h"
#include "window.h"
#include "palette.h"
#include "resman.h"
#include "inifile.h"
#include "sciplane.h"

#include <string.h>

extern Application *pApp;
extern char szIniFilename[];
extern BOOL skipPaletteChanges;
extern BYTE btColorBitSupport;

#include <math.h>

Palette::Palette()
	{
	}

Palette::~Palette()
	{
	}

void Palette::ActivatePalette()
	{
	if (!enabled)
		return;
	if (pApp->pMainPlane && pApp->pMainPlane->GetWindow())
		{
		ActivatePalette(MAIN_WINDOW->hDC);
		}
	}

HPALETTE Palette::ActivatePalette(HDC hDC)
	{
	int i;
	HPALETTE hOldPalette = 0;
	LPLOGPALETTE lpNoCollapsePalette;
	HPALETTE hpalTheOneTruePalette;
	
	if (!enabled)
		return 0;

#ifdef _DEBUG
	if (skipPaletteChanges)
		return 0;
#endif
	
	VPalette::ActivatePalette();

	if (btColorBitSupport > 8)
		{
		// True-Color doesn't need realizing of the palette.
		SetColors(preColors);
		return 0;
		}

	lpNoCollapsePalette = (LOGPALETTE*) new BYTE[UINT(sizeof(LOGPALETTE) + 256 * sizeof(PALETTEENTRY))];
	lpNoCollapsePalette->palVersion = 0x0300;
	lpNoCollapsePalette->palNumEntries = 256;

	for (i=0; i<256; ++i)
		{
		lpNoCollapsePalette->palPalEntry[i].peRed	= preColors[i].rgbRed;
		lpNoCollapsePalette->palPalEntry[i].peGreen	= preColors[i].rgbGreen;
		lpNoCollapsePalette->palPalEntry[i].peBlue	= preColors[i].rgbBlue;
		if ((i > 9) && (i < 246))
			{
			lpNoCollapsePalette->palPalEntry[i].peFlags = PC_NOCOLLAPSE;
			}
		else
			{
			lpNoCollapsePalette->palPalEntry[i].peFlags = 0;
			}
		}

	hpalTheOneTruePalette = CreatePalette(lpNoCollapsePalette);
	hOldPalette = SelectPalette(hDC, hpalTheOneTruePalette, FALSE);
	DeleteObject(hOldPalette);
	RealizePalette(hDC);

	SetColors(preColors, TRUE);

	delete[] lpNoCollapsePalette;

	return 0;
	}
