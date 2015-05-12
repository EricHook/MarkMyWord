#include "os.h"
#include "core.h"
#include "tex.h"
#include "quantize.h"
#include "neuquant.h"

static void SwapBytes(WORD &wData)
	{
	wData = WORD((wData << 8) | (wData >> 8));
	}

static void SwapBytes(DWORD &dwData)
	{
	WORD hiWORD = (WORD)(dwData >> 16);
	WORD loWORD = (WORD)(dwData);
	SwapBytes(hiWORD);
	SwapBytes(loWORD);
	dwData =  loWORD<<16 | hiWORD;
	}

static void EndianSwapBytes(WORD &wData)
	{
#if defined(OS_LITTLE_ENDIAN)
#else
	SwapBytes(wData);
#endif // OS_LITTLE_ENDIAN
	}

static void EndianSwapBytes(DWORD &dwData)
	{
#if defined(OS_LITTLE_ENDIAN)
#else
	SwapBytes(dwData);
#endif // OS_LITTLE_ENDIAN
	}

static void PutDWORD(PBYTE &stuff, DWORD i)
	{
	// Write out a DWORD, and adjust the stuff pointer accordingly.
	EndianSwapBytes(i);
	*PDWORD(stuff) = i;
	stuff += sizeof(DWORD);
	}

static void PutWORD(PBYTE &stuff, WORD i)
	{
	// Write out a WORD, and adjust the stuff pointer accordingly.
	EndianSwapBytes(i);
	*PWORD(stuff) = i;
	stuff += sizeof(WORD);
	}

static void PutBYTE(PBYTE &stuff, BYTE i)
	{
	// Write out a BYTE, and adjust the stuff pointer accordingly.
	*PBYTE(stuff) = i;
	stuff += sizeof(BYTE);
	}

static void PutBYTEs(PBYTE &stuff, BYTE *p, int count)
	{
	int i;

	// Write out a BYTE, and adjust the stuff pointer accordingly.
	for (i = 0; i < count; ++i)
		{
		*stuff++ = *p++;
		}
	}

TEX::TEX()
	{
	numLoops = 0;
	loops = NULL;
	numFrames = 0;
	frames = NULL;
	calcerror = TRUE;

	// A new TEX has one loop, with no cels.
	AddLoop();

	// For now, generate a generic palette.
	FigurePalette(NULL, 0);
	}

TEX::~TEX()
	{
	int i, j, k;

	for (i = 0; i < numLoops; ++i)
		{
		for (j = 0; j < loops[i].numCels; ++j)
			{
			delete[] loops[i].cels[j].lengthOfRows;
			for (k = 0; k < loops[i].cels[j].height; ++k)
				{
				delete[] loops[i].cels[j].rows[k];
				}
			delete[] loops[i].cels[j].rows;
			}
		delete[] loops[i].cels;
		}
	delete[] loops;
	}

void TEX::AddCel(RGBA *bitmap, int w, int h, int transferMode, BOOL trim)
	{
	TEX_LOOP *loop;
	TEX_CEL *cel;
	TEX_CEL *newCels;
	int i;
	int trimLeft, trimTop, trimRight, trimBottom;
	BYTE *colorRow;
	BYTE *alphaRow;
	int y;
	BOOL emptyAlpha;
	BOOL foundColor;
	BOOL foundAlpha;
	RGBA *nextRow;
	//int r, c;

	loop = &(loops[numLoops-1]);

	// Create room for another cel;
	newCels = new TEX_CEL[loop->numCels+1];
	for (i = 0; i < loop->numCels; ++i)
		{
		newCels[i] = loop->cels[i];
		}
	delete[] loop->cels;
	loop->cels = newCels;

	foundColor = FALSE;
	foundAlpha = FALSE;
	// Check for an empty alpha channel
	for (i = 0; i < w * h; ++i)
		{
		if ((bitmap[i].r != 0) ||
			(bitmap[i].g != 0) ||
			(bitmap[i].b != 0))
			{
			foundColor = TRUE;
			}
		if (bitmap[i].a != 0)
			{
			foundAlpha = TRUE;

			// Terminate loop. emptyAlpha can't be TRUE.
			break;
			}
		}

	switch (transferMode)
		{
		case NOPALETTE_FLAG:
		case ALPHA_FLAG:
			emptyAlpha = foundColor && !foundAlpha;
			break;
		case ADD_FLAG:
		case MULTIPLY_FLAG:
		case OVERLAY_FLAG:
		default:
			// ADD and MULTIPLY don't use alpha.
			emptyAlpha = TRUE;
			break;
		}

	// If its an empty alpha, fix it.
	if (emptyAlpha)
		{
		switch (transferMode)
			{
			default:
			case NOPALETTE_FLAG:
			case ALPHA_FLAG:
				for (i = 0; i < w * h; ++i)
					bitmap[i].a = 255;
				break;
			case ADD_FLAG:
			case OVERLAY_FLAG:
				for (i = 0; i < w * h; ++i)
					{
					if ((bitmap[i].r == 0) && (bitmap[i].g == 0) && (bitmap[i].b == 0))
						bitmap[i].a = 0;
					else
						bitmap[i].a = 255;
					}
				break;
			case MULTIPLY_FLAG:
				for (i = 0; i < w * h; ++i)
					{
					if ((bitmap[i].r == 255) && (bitmap[i].g == 255) && (bitmap[i].b == 255))
						bitmap[i].a = 0;
					else
						bitmap[i].a = 255;
					}
				break;
			}
		}

	int found;
	//int right;
	int bar;
	//RGBA pixel;

	// Check for frame separator.
	found = FALSE;
	bar = 0;
/*
	for (r = 0; r < h; ++r)
		{
		for (c = 0; c < w; ++c)
			{
			right = TRUE;
			pixel = bitmap[r*w + c];
			if (c % 2)
				{
				if ((pixel.r > 0) ||
					(pixel.g > 0) ||
					(pixel.b > 0))
					{
					right = FALSE;
					break;
					}
				}
			else
				{
				if ((pixel.r < 255) ||
					(pixel.g < 255) ||
					(pixel.b < 255))
					{
					right = FALSE;
					break;
					}
				}
			}
		if (right)
			{
			found = TRUE;
			bar = r;
			break;
			}
		}

	// Separator bar found, call AddCel with the sub image.
	if (found)
		{
		for (r = 0; r < h; r += (bar + 1))
			{
			AddCel(bitmap + (r*w), w, bar, transferMode, trim);
			}
		return;
		}
*/
	// No separator bar.
	if (trim)
		{
		FigureTrim(bitmap, w, h, trimLeft, trimTop, trimRight, trimBottom);
		}
	else
		{
		trimLeft = 0;
		trimTop = 0;
		trimRight = 0;
		trimBottom = 0;
		}

	cel = &(loop->cels[loop->numCels]);
	if ((trimLeft == w) ||
		(trimTop == h) ||
		(trimRight == w) ||
		(trimBottom == h))
		{
		cel->x = 0;
		cel->y = 0;
		cel->width = 0;
		cel->height = 0;
		cel->rows = NULL;
		cel->lengthOfRows = NULL;
		}
	else
		{
		cel->x = trimLeft;
		cel->y = trimTop;
		cel->width = w - trimLeft - trimRight;
		cel->height = h - trimTop - trimBottom;
		cel->rows = new PBYTE[cel->height];
		cel->lengthOfRows = new int[cel->height];

		colorRow = new BYTE[cel->width];
		alphaRow = new BYTE[cel->width];
		for (y = 0; y < cel->height; ++y)
			{
			if (y < (cel->height - 1))
				nextRow = &(bitmap[((y+1) + trimTop) * w + trimLeft]);
			else
				nextRow = NULL;
			ReduceColors(&(bitmap[(y + trimTop) * w + trimLeft]), nextRow,
				colorRow, alphaRow, cel->width, transferMode);
			cel->rows[y] = TextureCompressRow((transferMode == NOPALETTE_FLAG) ? (unsigned char*)&(bitmap[(y + trimTop) * w + trimLeft]) : colorRow, 
				cel->width,	&cel->lengthOfRows[y], alphaRow, transferMode);
			}

		delete[] colorRow;
		delete[] alphaRow;
		}

	++loop->numCels;
	}

void TEX::SetLastCelOffsets(int x, int y)
{
	loops[numLoops-1].cels[loops[numLoops-1].numCels-1].x = x;
	loops[numLoops-1].cels[loops[numLoops-1].numCels-1].y = y;
}

void TEX::AddLoop()
	{
	TEX_LOOP *newLoops;
	int i;

	// Create another loop;
	newLoops = new TEX_LOOP[numLoops+1];
	newLoops[numLoops].numCels = 0;
	newLoops[numLoops].cels = NULL;
	for (i = 0; i < numLoops; ++i)
		{
		newLoops[i] = loops[i];
		}
	delete[] loops;
	loops = newLoops;
	++numLoops;
	}

int TEX::Write(PBYTE out)
	{
	PBYTE tempOut;
	int tempInt;
	short tempShort;
	int i, j, k;

	tempOut = out;

	// 'tex '
	tempInt = (' '<<24) | ('x'<<16) | ('e'<<8) | 't';
	PutDWORD(tempOut, DWORD(tempInt));

	// Version
	tempInt = ('1'<<24) | ('0'<<16) | ('0'<<8) | '1';
	PutDWORD(tempOut, DWORD(tempInt));

	// palette
	for (i = 0; i < 256; ++i)
		{
		PutBYTEs(tempOut, PBYTE(&palette[i].red), 1);
		PutBYTEs(tempOut, PBYTE(&palette[i].green), 1);
		PutBYTEs(tempOut, PBYTE(&palette[i].blue), 1);
		}

	// numLoops
	tempShort = numLoops;
	PutWORD(tempOut, WORD(tempShort));

	for (i = 0; i < numLoops; ++i)
		{
		// numCels
		tempShort = loops[i].numCels;
		PutWORD(tempOut, WORD(tempShort));

		// cel data
		for (j = 0; j < loops[i].numCels; ++j)
			{
			// width, height, xOrigin, yOrigin, x, y
			tempShort = loops[i].cels[j].width;
			PutWORD(tempOut, WORD(tempShort));
			tempShort = loops[i].cels[j].height;
			PutWORD(tempOut, WORD(tempShort));
			tempShort = 0;
			PutWORD(tempOut, WORD(tempShort));
			tempShort = 0;
			PutWORD(tempOut, WORD(tempShort));
			tempShort = loops[i].cels[j].x;
			PutWORD(tempOut, WORD(tempShort));
			tempShort = loops[i].cels[j].y;
			PutWORD(tempOut, WORD(tempShort));

			for (k = 0; k < loops[i].cels[j].height; ++k)
				{
				tempShort = loops[i].cels[j].lengthOfRows[k];
				PutWORD(tempOut, WORD(tempShort));
				PutBYTEs(tempOut, loops[i].cels[j].rows[k], tempShort);
				}
			}
		}

	return tempOut - out;
	}

int TEX::GetSize()
	{
	int i, j, k, nSize = 0;

	// 'tex '
	nSize += 4;

	// Version
	nSize += 4;

	// palette
	nSize += 768;

	// numLoops
	nSize += 2;

	for (i = 0; i < numLoops; ++i)
		{
		// numCels
		nSize += 2;

		// cel data
		for (j = 0; j < loops[i].numCels; ++j)
			{
			// width, height, xOrigin, yOrigin, x, y
			nSize += 12;

			for (k = 0; k < loops[i].cels[j].height; ++k)
				{
				nSize += 2;
				nSize += loops[i].cels[j].lengthOfRows[k];
				}
			}
		}

	return nSize;
	}

void TEX::FigureTrim(RGBA *bitmap, int w, int h,
	int &trimLeft, int &trimTop, int &trimRight, int &trimBottom)
	{
	int x, y;
	BOOL clear;

	// Find trimTop.
	trimTop = 0;
	for (y = 0; y < h; ++y)
		{
		clear = TRUE;
		for (x = 0; x < w; ++x)
			{
			if (bitmap[y * w + x].a > 0)
				{
				clear = FALSE;
				x = w;
				}
			}
		if (clear)
			trimTop = y + 1;
		else
			y = h;
		}

	// Find trimLeft.
	trimLeft = 0;
	for (x = 0; x < w; ++x)
		{
		clear = TRUE;
		for (y = 0; y < h; ++y)
			{
			if (bitmap[y * w + x].a > 0)
				{
				clear = FALSE;
				y = h;
				}
			}
		if (clear)
			trimLeft = x + 1;
		else
			x = w;
		}

	// Find trimBottom.
	trimBottom = 0;
	for (y = h-1; y >= 0; --y)
		{
		clear = TRUE;
		for (x = 0; x < w; ++x)
			{
			if (bitmap[y * w + x].a > 0)
				{
				clear = FALSE;
				x = w;
				}
			}
		if (clear)
			trimBottom = h - y;
		else
			y = 0;
		}

	// Find trimRight.
	trimRight = 0;
	for (x = w-1; x >= 0; --x)
		{
		clear = TRUE;
		for (y = 0; y < h; ++y)
			{
			if (bitmap[y * w + x].a > 0)
				{
				clear = FALSE;
				y = h;
				}
			}
		if (clear)
			trimRight = w - x;
		else
			x = 0;
		}
	}

void TEX::SetPalette(RGB *colors)
	{
	memcpy(palette, colors, 768);
	calcerror = FALSE;
	}

void TEX::SetPaletteN(RGB *colors, int n)
	{
	memcpy(palette, colors, 3 * n);
	calcerror = FALSE;
	}

void TEX::FigurePalette(RGB *pixels, int numPixels)
	{
	int r, g, b;
	int i;

	// Generate a uniform palette for now.
	i = 0;
	for (r = 0; r < 8; ++r)
		for (g = 0; g < 8; ++g)
			for (b = 0; b < 4; ++b)
				{
				palette[i].red = BYTE(255.0 / 7 * r);
				palette[i].green = BYTE(255.0 / 7 * g);
				palette[i].blue = BYTE(255.0 / 3 * b);
				++i;
				}

	InitInverseColorTable(palette);
	}

void TEX::FigureBestPalette1(RGBA *bitmap, int w, int h)
	{
	CQuantizer *quantizer;
	RGBQUAD pal[256];
	RGB *pixels;
	int i;

	pixels = new RGB[w*h];
	for (i = 0; i < w*h; ++i)
		{
		pixels[i].red = bitmap[i].r;
		pixels[i].green = bitmap[i].g;
		pixels[i].blue = bitmap[i].b;
		}
	quantizer = new CQuantizer(256, 5);
	quantizer->ProcessImage (pixels, w, h);
	i = quantizer->GetColorCount();
	quantizer->GetColorTable(pal);
	for (i = 0; i < 256; ++i)
		{
		palette[i].red = pal[i].rgbRed;
		palette[i].green = pal[i].rgbGreen;
		palette[i].blue = pal[i].rgbBlue;
		}

	delete quantizer;

	InitInverseColorTable(palette);
	}

void TEX::FigureBestPalette1a(RGB *bitmap, int l)
	{
	CQuantizer *quantizer;
	RGBQUAD pal[256];
	int i;

	quantizer = new CQuantizer(256, 5);
	quantizer->ProcessImage (bitmap, l, 1);
	i = quantizer->GetColorCount();
	quantizer->GetColorTable(pal);
	for (i = 0; i < 256; ++i)
		{
		palette[i].red = pal[i].rgbRed;
		palette[i].green = pal[i].rgbGreen;
		palette[i].blue = pal[i].rgbBlue;
		}

	delete quantizer;

	InitInverseColorTable(palette);
	}

#define MIN_COLORS_TO_DO 10000
#define MAX_COLORS_TO_DO 1000000

int CountColors(RGB *bitmap, int l)
	{
	int i, j;
	RGB colorCountArray[256];
	int colorCount;
	BOOL colorFound;

	// Count colors.
	i = 0;	// loop through colorCountArray[];
	j = 0;	// loop through bitmap[];
	colorCount = 0;
	while ((j < l) && (colorCount < 256))
		{
		colorFound = FALSE;
		for (i=0; i<colorCount; ++i)
			{
			if ((colorCountArray[i].red == bitmap[j].red) &&
				(colorCountArray[i].green == bitmap[j].green) &&
				(colorCountArray[i].blue == bitmap[j].blue))
				colorFound = TRUE;
			}
		if (colorFound)
			{
			}
		else
			{
			colorCountArray[colorCount].red = bitmap[j].red;
			colorCountArray[colorCount].green = bitmap[j].green;
			colorCountArray[colorCount].blue = bitmap[j].blue;
			++colorCount;
			}
		++j;
		}
	return colorCount;
	}

void TEX::FigureBestPalette2(RGB *bitmap, int l)
	{
	RGB *pixels;
	int i, j;
	RGB colorCountArray[256];
	int colorCount;
	BOOL colorFound;

	// Count colors.
	i = 0;	// loop through colorCountArray[];
	j = 0;	// loop through bitmap[];
	colorCount = 0;
	while ((j < l) && (colorCount < 256))
		{
		colorFound = FALSE;
		for (i=0; i<colorCount; ++i)
			{
			if ((colorCountArray[i].red == bitmap[j].red) &&
				(colorCountArray[i].green == bitmap[j].green) &&
				(colorCountArray[i].blue == bitmap[j].blue))
				colorFound = TRUE;
			}
		if (colorFound)
			{
			}
		else
			{
			colorCountArray[colorCount].red = bitmap[j].red;
			colorCountArray[colorCount].green = bitmap[j].green;
			colorCountArray[colorCount].blue = bitmap[j].blue;
			++colorCount;
			}
		++j;
		}

	if (colorCount < 256)
		{
		for (i=0; i<colorCount; ++i)
			{
			palette[i] = colorCountArray[i];
			}
		}
	else
		{
		if (l < MIN_COLORS_TO_DO)
			{
			pixels = new RGB[MIN_COLORS_TO_DO];
			for (i = 0; i < MIN_COLORS_TO_DO; ++i)
				{
				pixels[i] = bitmap[i%l];
				}
			initnet((BYTE*)pixels, 3*MIN_COLORS_TO_DO, 1);
			}
		else
			{
			i = l/MIN_COLORS_TO_DO/10;
			if (i < 1)
				i = 1;
			initnet((BYTE*)bitmap, 3*l, i);
			}
		learn();
		unbiasnet();
		getcolourmap(palette);
		}
	InitInverseColorTable(palette);
	}

void FS(RGBA *sourceRow, RGBA *sourceRow1,
		double rError, double gError, double bError, int part)
	{
	double r, g, b;

	r = (sourceRow1)->r + part * rError / 16 + .5;
	g = (sourceRow1)->g + part * gError / 16 + .5;
	b = (sourceRow1)->b + part * bError / 16 + .5;

	if (r > 255)
		r = 255;
	if (r < 0)
		r = 0;

	if (g > 255)
		g = 255;
	if (g < 0)
		g = 0;

	if (b > 255)
		b = 255;
	if (b < 0)
		b = 0;

	sourceRow1->r = BYTE(r);
	sourceRow1->g = BYTE(g);
	sourceRow1->b = BYTE(b);
	}

void TEX::ReduceColors(	RGBA *sourceRow, RGBA *nextSourceRow, 
						BYTE *colorRow, BYTE *alphaRow, int width, int transferMode)
	{
	int x;
	double rError, gError, bError;

	rError = 0;
	gError = 0;
	bError = 0;
	for (x = 0; x < width; ++x)
		{
		if (sourceRow->a == 0)
			{
			*colorRow = 0;
			*alphaRow = 0;
			}
		else
			{
			if (transferMode != NOPALETTE_FLAG)
				*colorRow = FindNearestColor(sourceRow->r, sourceRow->g, sourceRow->b);
			else
				*colorRow = 8;  // This should make it obvious that there has been an error
								// if the colorRow is even used.

			*alphaRow = sourceRow->a;

			// Calculate new error.
			if (calcerror && (transferMode != NOPALETTE_FLAG))
				{
				rError = sourceRow->r - double(palette[*colorRow].red);
				gError = sourceRow->g - double(palette[*colorRow].green);
				bError = sourceRow->b - double(palette[*colorRow].blue);

				while ((rError > 64) || (gError > 64) || (bError > 64))
					{
					rError *= 0.5;
					gError *= 0.5;
					bError *= 0.5;
					}

				// Pass some error to the right, if there's room.
				if (x < (width-1))
					{
					FS(sourceRow, sourceRow+1,
						rError, gError, bError, 7);
					}
				// Pass some error down, if there's room.
				if (nextSourceRow)
					{
					// Pass some error down and to the right, if there's room.
					if (x < (width-1))
						{
						FS(sourceRow, nextSourceRow+1,
							rError, gError, bError, 1);
						}
					// Pass some error down.
					FS(sourceRow, nextSourceRow,
						rError, gError, bError, 5);
					// Pass some error down and to the left, if there's room.
					if (x > 0)
						{
						FS(sourceRow, nextSourceRow-1,
							rError, gError, bError, 3);
						}
					}
				}
			}

		++sourceRow;
		if (nextSourceRow)
			++nextSourceRow;
		++colorRow;
		++alphaRow;
		}
	}

BYTE TEX::FindNearestColor(BYTE red, BYTE green, BYTE blue)
	{
	//return InverseColorLookup(red, green, blue);

	int i;
	int best;
	int bestDelta;
	int delta;
	int dr, dg, db;
	RGB *theColor;

	best = 0;
	bestDelta = 256*256*256;
	theColor = &(palette[0]);

	for (i = 0; i < 256; ++i)
		{
		dr = int(theColor->red) - red;
		dg = int(theColor->green) - green;
		db = int(theColor->blue) - blue;
		++theColor;
		delta = dr*dr + dg*dg + db*db;
		if (delta < 0)
			delta = -delta;
		if (delta < bestDelta)
			{
			best = i;
			bestDelta = delta;
			}
		if (delta == 0)
			{
			i = 256;
			}
		}
	return best;
	}

void TEX::Dither24to15(RGBA *bitmap, int width, int height)
	{
	int x, y;
	double rError, gError, bError;
	RGBA *sourceRow, *nextSourceRow;
	int r, g, b;

	for (y=0; y<height; ++y)
		{
		sourceRow = bitmap + (y * width);
		if (y == (height-1))
			nextSourceRow = NULL;
		else
			nextSourceRow = bitmap + ((y+1) * width);
		for (x=0; x<width; ++x)
			{
			r = sourceRow->r;
			g = sourceRow->g;
			b = sourceRow->b;
			
			if (r & 0x4)
				r += 4;
			if (g & 0x4)
				g += 4;
			if (b & 0x4)
				b += 4;
			if (r > 255)
				r = 255;
			if (g > 255)
				g = 255;
			if (b > 255)
				b = 255;
			r &= 0xF8;
			g &= 0xF8;
			b &= 0xF8;
			
			// Calculate new error.
			rError = sourceRow->r - r;
			gError = sourceRow->g - g;
			bError = sourceRow->b - b;

			//rError *= 0.5;
			//gError *= 0.5;
			//bError *= 0.5;
			
			sourceRow->r = r;
			sourceRow->g = g;
			sourceRow->b = b;

			// Pass some error to the right, if there's room.
			if (x < (width-1))
				{
				FS(sourceRow, sourceRow+1,
					rError, gError, bError, 7);
				}
			// Pass some error down, if there's room.
			if (nextSourceRow)
				{
				// Pass some error down and to the right, if there's room.
				if (x < (width-1))
					{
					FS(sourceRow, nextSourceRow+1,
						rError, gError, bError, 1);
					}
				// Pass some error down.
				FS(sourceRow, nextSourceRow,
					rError, gError, bError, 5);
				// Pass some error down and to the left, if there's room.
				if (x > 0)
					{
					FS(sourceRow, nextSourceRow-1,
						rError, gError, bError, 3);
					}
				}
			++sourceRow;
			if (nextSourceRow)
				++nextSourceRow;
			}
		}
	}

#define BAR_SLOP 25

void TEX::AddFrame(RGBA *pixels, int width, int height, int transferMode, BOOL trim)
	{
	int trimLeft, trimRight, trimTop, trimBottom;
	TEX_FRAME *newFrames;
	TEX_FRAME *curFrame;
	int i, j;
	BOOL hasAlpha;
	BOOL hasColor;
	BOOL hasBar;
	RGBA *row;
	int subHeight;
	BOOL foundBar;
	RGBA *tempPixels;

	// Check for a separater bar. If it exists, recursively call AddFrame with each frame.
	hasBar = FALSE;
	subHeight = 0;
	foundBar = FALSE;
	tempPixels = pixels;
	for (i=0; i<height; ++i)
		{
		row = &pixels[i*width];
		hasBar = TRUE;
		for (j=0; j<width; ++j)
			{
			if (j%2)
				{
				if ((row->r > BAR_SLOP) || (row->g > BAR_SLOP) || (row->b > BAR_SLOP))
					{
					hasBar = FALSE;
					j = width;
					}
				}
			else
				{
				if ((row->r < (255-BAR_SLOP)) || (row->g < (255-BAR_SLOP)) || (row->b < (255-BAR_SLOP)))
					{
					hasBar = FALSE;
					j = width;
					}
				}
			++row;
			}
		if (hasBar)
			{
			foundBar = TRUE;
			AddFrame(tempPixels, width, subHeight, trim);
			tempPixels = tempPixels + (subHeight+1) * width;
			subHeight = 0;
			}
		else
			{
			++subHeight;
			}
		}

	if (foundBar)
		{
		AddFrame(tempPixels, width, subHeight, trim);
		return;
		}	

	// Adjust for possible missing alpha channel.
	switch (transferMode)
		{
		case ALPHA_FLAG:
			hasAlpha = FALSE;
			hasColor = FALSE;
			for (i=0; i<height; ++i)
				{
				for (j=0; j<width; ++j)
					{
					if ((pixels[i*width + j].r != 0) || 
						(pixels[i*width + j].g != 0) ||
						(pixels[i*width + j].b != 0))
						{
						hasColor = TRUE;
						}
					
					if (pixels[i*width + j].a != 0)
						{
						hasAlpha = TRUE;
						i = height;
						j = width;
						}
					}
				}
			if (!hasAlpha && hasColor)
				{
				for (i=0; i<height; ++i)
					{
					for (j=0; j<width; ++j)
						{
						pixels[i*width + j].a = 255;
						}
					}
				}
			break;
		case ADD_FLAG:
		case OVERLAY_FLAG:
			for (i=0; i<height; ++i)
				{
				for (j=0; j<width; ++j)
					{
					if ((pixels[i*width + j].r != 0) || (pixels[i*width + j].g != 0) || (pixels[i*width + j].b != 0))
						{
						pixels[i*width + j].a = 255;
						}
					else
						{
						pixels[i*width + j].a = 0;
						}
					}
				}
			break;
		case MULTIPLY_FLAG:
			for (i=0; i<height; ++i)
				{
				for (j=0; j<width; ++j)
					{
					if ((pixels[i*width + j].r != 255) || (pixels[i*width + j].g != 255) || (pixels[i*width + j].b != 255))
						{
						pixels[i*width + j].a = 255;
						}
					else
						{
						pixels[i*width + j].a = 0;
						}
					}
				}
			break;
		}

	trimLeft	= 0;
	trimRight	= 0;
	trimBottom	= 0;
	trimTop		= 0;
	if (trim)
		FigureTrim(pixels, width, height, trimLeft, trimTop, trimRight, trimBottom);
		
	newFrames = new TEX_FRAME[numFrames+1];
	for (i=0; i<numFrames; ++i)
		{
		newFrames[i] = frames[i];
		}
	curFrame = &newFrames[numFrames];
	++numFrames;
	delete[] frames;
	frames = newFrames;
	
	if ((trimLeft == width) ||
		(trimTop == height) ||
		(trimRight == width) ||
		(trimBottom == height))
		{
		curFrame->x = 0;
		curFrame->y = 0;
		curFrame->width = 0;
		curFrame->height = 0;
		curFrame->pixels = NULL;
		}
	else
		{
		curFrame->x = trimLeft;
		curFrame->y = trimTop;
		curFrame->width = width - trimLeft - trimRight;
		curFrame->height = height - trimTop - trimBottom;
		curFrame->pixels = new RGBA[curFrame->width * curFrame->height];
		}

	for (i=0; i<curFrame->height; ++i)
		{
		for (j=0; j<curFrame->width; ++j)
			{
			curFrame->pixels[i*curFrame->width + j] = pixels[(i+trimTop)*width + j + trimLeft];
			}
		}
	}

void TEX::CompressFrames(int transferMode, int ditherMode)
	{
	TEX_LOOP *loop;
	TEX_CEL *cel;
	int i, j;
	TEX_FRAME *curFrame;
	BYTE *colorRow;
	BYTE *alphaRow;
	int x, y;
	RGBA *nextRow;
	RGB *bitmap;
	BOOL hasColor;
	int colorCount;

	loop = &(loops[numLoops-1]);

	// Create room for more cels;
	delete[] loop->cels;
	loop->cels = new TEX_CEL[numFrames];
	loop->numCels = numFrames;

	// Setup the cels.
	for (i = 0; i < loop->numCels; ++i)
		{
		curFrame = &frames[i];
		cel = &(loop->cels[i]);
		cel->x = curFrame->x;
		cel->y = curFrame->y;
		cel->width = curFrame->width;
		cel->height = curFrame->height;
		cel->rows = new PBYTE[cel->height];
		cel->lengthOfRows = new int[cel->height];
		}

	// Grab the colored pixels, and count them.
	bitmap = new RGB[MAX_COLORS_TO_DO];
	j = 0;
	for (i = 0; i < loop->numCels; ++i)
		{
		curFrame = &frames[i];
		cel = &(loop->cels[i]);
		for (y=0; y < cel->height; ++y)
			{
			for (x=0; x<cel->width; ++x)
				{
				if (curFrame->pixels[y*cel->width + x].a != 0)
					hasColor = TRUE;
				else
					hasColor = FALSE;
				if (hasColor && (j < MAX_COLORS_TO_DO))
					{
					bitmap[j].red = curFrame->pixels[y*cel->width + x].r;
					bitmap[j].green = curFrame->pixels[y*cel->width + x].g;
					bitmap[j].blue = curFrame->pixels[y*cel->width + x].b;
					++j;
					}
				}
			}
		}
	colorCount = CountColors(bitmap, j);
	delete[] bitmap;

	if (ditherMode)
		{
		if (colorCount >= 256)
			{
			// First dither to 15bit color.
			for (i = 0; i < loop->numCels; ++i)
				{
				curFrame = &frames[i];
				cel = &(loop->cels[i]);
				Dither24to15(curFrame->pixels, cel->width, cel->height);
				}
			}
		}
		
	// Grap a collection of pixels, and figure out the best palette.
	bitmap = new RGB[MAX_COLORS_TO_DO];
	j = 0;
	for (i = 0; i < loop->numCels; ++i)
		{
		curFrame = &frames[i];
		cel = &(loop->cels[i]);
		for (y=0; y < cel->height; ++y)
			{
			for (x=0; x<cel->width; ++x)
				{
				if (curFrame->pixels[y*cel->width + x].a != 0)
					hasColor = TRUE;
				else
					hasColor = FALSE;
				if (hasColor && (j < MAX_COLORS_TO_DO))
					{
					bitmap[j].red = curFrame->pixels[y*cel->width + x].r;
					bitmap[j].green = curFrame->pixels[y*cel->width + x].g;
					bitmap[j].blue = curFrame->pixels[y*cel->width + x].b;
					++j;
					}
				}
			}
		}
	FigureBestPalette2(bitmap, j);
	delete[] bitmap;


	// Dither down to 8bit color.
	for (i = 0; i < loop->numCels; ++i)
		{
		curFrame = &frames[i];
		cel = &(loop->cels[i]);
		colorRow = new BYTE[cel->width];
		alphaRow = new BYTE[cel->width];
		for (y = 0; y < cel->height; ++y)
			{
			if (y < (cel->height - 1))
				nextRow = &(curFrame->pixels[(y+1) * curFrame->width]);
			else
				nextRow = NULL;
			ReduceColors(&(curFrame->pixels[y * curFrame->width]), nextRow,
				colorRow, alphaRow, cel->width, transferMode);
			cel->rows[y] = TextureCompressRow(colorRow, cel->width,
				&cel->lengthOfRows[y], alphaRow, transferMode);
			}
	
		delete[] colorRow;
		delete[] alphaRow;
		}

	// Frame compressed. Delete them.
	for (i=0; i<numFrames; ++i)
		{
		delete[] frames[i].pixels;
		}
	delete[] frames;
	numFrames = 0;
	}
