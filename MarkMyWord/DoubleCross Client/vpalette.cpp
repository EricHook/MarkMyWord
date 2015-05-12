#include "os.h"

#include <math.h>
#include "sci.h"
#include "core.h"

extern BYTE btColorBitSupport;
extern BTMP_RES btmpResGlobalHeader;

double defaultGamma = 1;

VPalette::VPalette()
	{
	int i;

#ifdef _DEBUG
	DebugString("VPalette::VPalette()\n");
#endif
	tableCalculated = TRUE;
	enabled = TRUE;
	curGamma = defaultGamma;

	int r, g, b;

	// Generate a uniform palette for now.
	i = 0;
	for (r = 0; r < 8; ++r)
		for (g = 0; g < 8; ++g)
			for (b = 0; b < 4; ++b)
				{
				colors[i].rgbRed = BYTE(255.0 / 7 * r);
				colors[i].rgbGreen = BYTE(255.0 / 7 * g);
				colors[i].rgbBlue = BYTE(255.0 / 3 * b);
				colors[i].rgbReserved	= 0;
				preColors[i].rgbRed = BYTE(255.0 / 7 * r);
				preColors[i].rgbGreen = BYTE(255.0 / 7 * g);
				preColors[i].rgbBlue = BYTE(255.0 / 3 * b);
				preColors[i].rgbReserved	= 0;
				++i;
				}
	}

VPalette::~VPalette()
	{
#ifdef _DEBUG
	DebugString("VPalette::~VPalette()\n");
#endif
	}

void VPalette::ActivatePalette()
	{
	if (!enabled)
		return;
	}

void VPalette::CalculateInverseTable()
	{
	RGB rgb[256];
	char str[64];
	int startTime;
	int endTime;
	int i;

	if (tableCalculated == FALSE)
		{
		startTime = game->GetTrueTime();
		tableCalculated = TRUE;
		for (i=0; i<256; ++i)
			{
			rgb[i].red = colors[i].rgbRed;
			rgb[i].green = colors[i].rgbGreen;
			rgb[i].blue = colors[i].rgbBlue;
			}
		InitInverseColorTable(rgb);
		endTime = game->GetTrueTime();
		sprintf(str, "Time to calculate inverse table: %d ticks.\n", endTime-startTime);
		DebugString(str);
		}
	for (i=0; i<256; ++i)
		btmpResGlobalHeader.bmiColors[i] = colors[i];
	}

void VPalette::SetColors(RGBQUAD newColors[256], BOOL calcInverse)
	{
	int i;

	for (i=0; i<256; ++i)
		{
		colors[i] = newColors[i];
		}
	if (btColorBitSupport == 8)
		CalculateInverseTable();
	}

void VPalette::SetPreColors(RGBQUAD newColors[256])
	{
	int i;

	for (i=0; i<256; ++i)
		{
		preColors[i] = newColors[i];
		}
	tableCalculated = FALSE;
	SetColors(preColors, FALSE);
	}

void VPalette::Enable()
	{
	enabled = TRUE;
	}

void VPalette::Disable()
	{
	enabled = FALSE;
	}

typedef struct
	{
	DWORD	riffTag;
	DWORD	sizeOfRiff;
	DWORD	palTag;
	DWORD	dataTag;
	DWORD	sizeOfData;
	WORD	version;
	WORD	numEntries;
	RGBQUAD	colors[256];
	} PAL_RES, *PPAL_RES;

void VPalette::LoadResource(int resID)
	{
	Resource *res;
	PAL_RES *palRes;

	res = pApp->pResourceManager->LoadResource(RES_PALETTE, resID);
	if (res)
		{
		palRes = PPAL_RES(res->GetData());

		if (palRes)
			{
			// Swap red and blue.
			int i;
			BYTE temp;

			for (i = 0; i < 256; ++i)
				{
				temp = palRes->colors[i].rgbRed;
				palRes->colors[i].rgbRed = palRes->colors[i].rgbBlue;
				palRes->colors[i].rgbBlue = temp;
				}

			if (palRes)
				{
				SetPreColors(palRes->colors);
				ActivatePalette();
				}
			pApp->pResourceManager->FreeResource(res);
			}
		}
	}

void VPalette::DoGammaCorrection(RGBQUAD *quad)
	{
	double fInvGamma;

	fInvGamma = double(1) / curGamma;
	quad->rgbRed	=
		BYTE(.5 + 256 * pow(double(quad->rgbRed)  / 256, fInvGamma));
	quad->rgbGreen	=
		BYTE(.5 + 256 * pow(double(quad->rgbGreen)/ 256, fInvGamma));
	quad->rgbBlue	=
		BYTE(.5 + 256 * pow(double(quad->rgbBlue) / 256, fInvGamma));
	}

double VPalette::GetGamma()
	{
	return curGamma;
	}

void VPalette::SetGamma(double newGamma)
	{
	curGamma = newGamma;
	}
