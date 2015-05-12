#include "os.h"
#include "core.h"

void SwapBytes(WORD *wData);
void SwapBytes(DWORD *dwData);
void FileReadBytes(FILE *file, BYTE *buffer, int count = 1);
void FileReadWords(FILE *file, WORD *buffer, int count = 1);
void FileReadDWords(FILE *file, DWORD *buffer, int count = 1);
void FileWriteBytes(FILE *file, BYTE *buffer, int count = 1);
void FileWriteWords(FILE *file, WORD *buffer, int count = 1);
void FileWriteDWords(FILE *file, DWORD *buffer, int count = 1);
void MacSwapBytes(WORD *wData);
void MacSwapBytes(DWORD *dwData);
static void MapColor(COLORMAPPING *colorMap, PTEX_PIXEL pixel);

int IsTransparent(BYTE *sourceRow, BYTE *alphaRow, int transferMode = 0);
int IsOpaque(BYTE *sourceRow, BYTE *alphaRow, int transferMode = 0);
int IsAlpha(BYTE *sourceRow, BYTE *alphaRow);
int IsRLE(BYTE *sourceRow, BYTE *alphaRow, BYTE *endOfRow, int transferMode = 0);
int fsize(char* filename);
void TextureReadLoop(BYTE **stuff, LOOP **loops, int theLoop);
void TextureReadCel(BYTE **stuff, LOOP **loops, int theLoop, int theCel);
void InitAlphaTable();

#if defined(OS_LITTLE_ENDIAN)
#define TEX_PIXEL_INCREMENT(x) ++x
#else
#define TEX_PIXEL_INCREMENT(x) --x
#endif

RGB TEX_PIXELtoRGB(TEX_PIXEL texPixel)
	{
	RGB rgb;

#if defined(COLOR_32BIT)
	TEX_PIXEL_STRUCT *p;

	p = PTEX_PIXEL_STRUCT(&texPixel);
	rgb.red = p->r;
	rgb.green = p->g;
	rgb.blue = p->b;
#else
	rgb.red = BYTE((texPixel & TEX_R_MASK) >> TEX_R_SHIFT);
	rgb.green = BYTE((texPixel & TEX_G_MASK) >> TEX_G_SHIFT);
	rgb.blue = BYTE((texPixel & TEX_B_MASK) << -TEX_B_SHIFT);
#endif

	return rgb;
	}

RGBQUAD TEX_PIXELtoRGBQUAD(TEX_PIXEL texPixel)
	{
	RGBQUAD rgb;

#if defined(COLOR_32BIT)
	TEX_PIXEL_STRUCT *p;

	p = PTEX_PIXEL_STRUCT(&texPixel);
	rgb.rgbRed = p->r;
	rgb.rgbGreen = p->g;
	rgb.rgbBlue = p->b;
#else
	rgb.rgbRed = BYTE((texPixel & TEX_R_MASK) >> TEX_R_SHIFT);
	rgb.rgbGreen = BYTE((texPixel & TEX_G_MASK) >> TEX_G_SHIFT);
	rgb.rgbBlue = BYTE((texPixel & TEX_B_MASK) << -TEX_B_SHIFT);
#endif

	return rgb;
	}

RGBA TEX_PIXELtoRGBA(TEX_PIXEL texPixel)
	{
	RGBA rgba;

#if defined(COLOR_32BIT)
	TEX_PIXEL_STRUCT *p;

	p = PTEX_PIXEL_STRUCT(&texPixel);
	rgba.r = p->r;
	rgba.g = p->g;
	rgba.b = p->b;
	rgba.a = 255;
#else
	rgba.r = BYTE((texPixel & TEX_R_MASK) >> TEX_R_SHIFT);
	rgba.g = BYTE((texPixel & TEX_G_MASK) >> TEX_G_SHIFT);
	rgba.b = BYTE((texPixel & TEX_B_MASK) << -TEX_B_SHIFT);
	rgba.a = 255;
#endif

	return rgba;
	}

TEX_PIXEL RGBtoTEX_PIXEL(RGB rgb)
	{
	TEX_PIXEL texPixel;

#if defined(COLOR_32BIT)
	TEX_PIXEL_STRUCT *p;

	p = PTEX_PIXEL_STRUCT(&texPixel);
	p->r = rgb.red;
	p->g = rgb.green;
	p->b = rgb.blue;
#else
	int r, g, b;

	r = (rgb.red << TEX_R_SHIFT) & TEX_R_MASK;
	g = (rgb.green << TEX_G_SHIFT) & TEX_G_MASK;
	b = (rgb.blue >> -TEX_B_SHIFT) & TEX_B_MASK;
	texPixel = TEX_PIXEL(r | g | b);
#endif

	return texPixel;
	}

TEX_PIXEL RGBQUADtoTEX_PIXEL(RGBQUAD rgb)
	{
	TEX_PIXEL texPixel;

#if defined(COLOR_32BIT)
	TEX_PIXEL_STRUCT *p;

	p = PTEX_PIXEL_STRUCT(&texPixel);
	p->r = rgb.rgbRed;
	p->g = rgb.rgbGreen;
	p->b = rgb.rgbBlue;
#else
	int r, g, b;

	r = (rgb.rgbRed << TEX_R_SHIFT) & TEX_R_MASK;
	g = (rgb.rgbGreen << TEX_G_SHIFT) & TEX_G_MASK;
	b = (rgb.rgbBlue >> -TEX_B_SHIFT) & TEX_B_MASK;
	texPixel = TEX_PIXEL(r | g | b);
#endif
	return texPixel;
	}

TEX_PIXEL RGBAtoTEX_PIXEL(RGBA rgba)
	{
	TEX_PIXEL texPixel;

#if defined(COLOR_32BIT)
	TEX_PIXEL_STRUCT *p;

	p = PTEX_PIXEL_STRUCT(&texPixel);
	p->r = rgba.r;
	p->g = rgba.g;
	p->b = rgba.b;
#else
	int r, g, b;

	r = (rgba.r << TEX_R_SHIFT) & TEX_R_MASK;
	g = (rgba.g << TEX_G_SHIFT) & TEX_G_MASK;
	b = (rgba.b >> -TEX_B_SHIFT) & TEX_B_MASK;
	texPixel = TEX_PIXEL(r | g | b);
#endif

	return texPixel;
	}

int IsTransparent(BYTE *sourceRow, BYTE *alphaRow, int transferMode /* = 0 */)
{
	if (transferMode != NOPALETTE_FLAG)
	{
		if (
				((alphaRow == NULL) && (*sourceRow == 0)) ||
				((alphaRow != NULL) && (*alphaRow == 0))
			)
			return TRUE;
		else
			return FALSE;
	}
	else
	{
		if (
				((alphaRow == NULL) && (*sourceRow == 0) && (*(sourceRow+1) == 0) && (*(sourceRow+2) == 0)) ||
				((alphaRow != NULL) && (*alphaRow == 0))
			)
			return TRUE;
		else
			return FALSE;
	}
}

int IsOpaque(BYTE *sourceRow, BYTE *alphaRow, int transferMode /* = 0 */)
{
	if (transferMode != NOPALETTE_FLAG)
	{
		if (
				((alphaRow == NULL) && (*sourceRow != 0)) ||
				((alphaRow != NULL) && (*alphaRow == 255))
			)
			return TRUE;
		else
			return FALSE;
	}
	else
	{
		if (
				(alphaRow == NULL) ||
				((alphaRow != NULL) && (*alphaRow == 255))
			)
			return TRUE;
		else
			return FALSE;
	}
}

int IsAlpha(BYTE *, BYTE *alphaRow)
{
		if ((alphaRow != NULL) && (*alphaRow != 0) && (*alphaRow != 255))
			return TRUE;
		else
			return FALSE;
}

int IsRLE(BYTE *sourceRow, BYTE *alphaRow, BYTE *endOfRow, int transferMode /* = 0 */)
	{
	if (
			(
				(transferMode != NOPALETTE_FLAG) &&
				((sourceRow + 9) <= endOfRow) && 
				(*sourceRow == (*(sourceRow+1))) && 
				(*sourceRow == (*(sourceRow+2))) && 
				(*sourceRow == (*(sourceRow+3))) && 
				(*sourceRow == (*(sourceRow+4))) && 
				(*sourceRow == (*(sourceRow+5))) && 
				(*sourceRow == (*(sourceRow+6))) && 
				(*sourceRow == (*(sourceRow+7))) && 
				(*sourceRow == (*(sourceRow+8))) && 
				(*sourceRow == (*(sourceRow+9)))
			) 
			
			||

			(
				(transferMode == NOPALETTE_FLAG) &&
				((sourceRow + 36) <= endOfRow) && 
				(*sourceRow == (*(sourceRow+4)) && (*(sourceRow+1)) == (*(sourceRow+5)) && (*(sourceRow+2)) == (*(sourceRow+6))) &&
				(*sourceRow == (*(sourceRow+8)) && (*(sourceRow+1)) == (*(sourceRow+9)) && (*(sourceRow+2)) == (*(sourceRow+10))) &&
				(*sourceRow == (*(sourceRow+12)) && (*(sourceRow+1)) == (*(sourceRow+13)) && (*(sourceRow+2)) == (*(sourceRow+14))) &&
				(*sourceRow == (*(sourceRow+16)) && (*(sourceRow+1)) == (*(sourceRow+17)) && (*(sourceRow+2)) == (*(sourceRow+18))) && 
				(*sourceRow == (*(sourceRow+20)) && (*(sourceRow+1)) == (*(sourceRow+21)) && (*(sourceRow+2)) == (*(sourceRow+22))) &&
				(*sourceRow == (*(sourceRow+24)) && (*(sourceRow+1)) == (*(sourceRow+25)) && (*(sourceRow+2)) == (*(sourceRow+26))) &&
				(*sourceRow == (*(sourceRow+28)) && (*(sourceRow+1)) == (*(sourceRow+29)) && (*(sourceRow+2)) == (*(sourceRow+30))) &&
				(*sourceRow == (*(sourceRow+32)) && (*(sourceRow+1)) == (*(sourceRow+33)) && (*(sourceRow+2)) == (*(sourceRow+34))) &&
				(*sourceRow == (*(sourceRow+36)) && (*(sourceRow+1)) == (*(sourceRow+37)) && (*(sourceRow+2)) == (*(sourceRow+38)))
			) 
		)
		{
		if (
				(alphaRow != NULL) && (transferMode != NOPALETTE_FLAG) &&
				IsOpaque(sourceRow, alphaRow, transferMode) &&
				IsOpaque(sourceRow+1, alphaRow+1, transferMode) &&
				IsOpaque(sourceRow+2, alphaRow+2, transferMode) &&
				IsOpaque(sourceRow+3, alphaRow+3, transferMode) &&
				IsOpaque(sourceRow+4, alphaRow+4, transferMode) &&
				IsOpaque(sourceRow+5, alphaRow+5, transferMode) &&
				IsOpaque(sourceRow+6, alphaRow+6, transferMode) &&
				IsOpaque(sourceRow+7, alphaRow+7, transferMode) &&
				IsOpaque(sourceRow+8, alphaRow+8, transferMode) &&
				IsOpaque(sourceRow+9, alphaRow+9, transferMode)
			)
			return TRUE;
		else if (
				(alphaRow == NULL) && (transferMode != NOPALETTE_FLAG) &&
				IsOpaque(sourceRow, NULL, transferMode) &&
				IsOpaque(sourceRow+1, NULL, transferMode) &&
				IsOpaque(sourceRow+2, NULL, transferMode) &&
				IsOpaque(sourceRow+3, NULL, transferMode) &&
				IsOpaque(sourceRow+4, NULL, transferMode) &&
				IsOpaque(sourceRow+5, NULL, transferMode) &&
				IsOpaque(sourceRow+6, NULL, transferMode) &&
				IsOpaque(sourceRow+7, NULL, transferMode) &&
				IsOpaque(sourceRow+8, NULL, transferMode) &&
				IsOpaque(sourceRow+9, NULL, transferMode)
				)
			return TRUE;
		else if (
				(alphaRow == NULL) && (transferMode == NOPALETTE_FLAG) &&
				IsOpaque(sourceRow, NULL, transferMode) &&
				IsOpaque(sourceRow+4, NULL, transferMode) &&
				IsOpaque(sourceRow+8, NULL, transferMode) &&
				IsOpaque(sourceRow+12, NULL, transferMode) &&
				IsOpaque(sourceRow+16, NULL, transferMode) &&
				IsOpaque(sourceRow+20, NULL, transferMode) &&
				IsOpaque(sourceRow+24, NULL, transferMode) &&
				IsOpaque(sourceRow+28, NULL, transferMode) &&
				IsOpaque(sourceRow+32, NULL, transferMode) &&
				IsOpaque(sourceRow+36, NULL, transferMode)
				)
			return TRUE;
		else if (
				(alphaRow != NULL) && (transferMode == NOPALETTE_FLAG) &&
				IsOpaque(sourceRow, NULL, transferMode) &&
				IsOpaque(sourceRow+4, NULL, transferMode) &&
				IsOpaque(sourceRow+8, NULL, transferMode) &&
				IsOpaque(sourceRow+12, NULL, transferMode) &&
				IsOpaque(sourceRow+16, NULL, transferMode) &&
				IsOpaque(sourceRow+20, NULL, transferMode) &&
				IsOpaque(sourceRow+24, NULL, transferMode) &&
				IsOpaque(sourceRow+28, NULL, transferMode) &&
				IsOpaque(sourceRow+32, NULL, transferMode) &&
				IsOpaque(sourceRow+36, NULL, transferMode)
				)
			return TRUE;
		else
			return FALSE;
		}
	else
		return FALSE;
	}

BYTE *TextureCompressRow(BYTE *sourceRow, int length, int *lengthOfRow, BYTE *alphaRow, int transferMode)
	{
	static BYTE tempRow[4096];
	BYTE *endOfSourceRow;
	BYTE *outRow;
	BYTE *tempOut;
	BYTE *compressedRow;
	int i, j;

	*lengthOfRow = 0;
	endOfSourceRow = sourceRow + length;

	if (transferMode == NOPALETTE_FLAG)
		endOfSourceRow += length * 3;

	outRow = &(tempRow[0]);

	while (sourceRow < endOfSourceRow)
		{
		i = 1;
		if (IsTransparent(sourceRow, alphaRow, transferMode))
			{
			++sourceRow;
			if (transferMode == NOPALETTE_FLAG)
				sourceRow += 3;
			if (alphaRow)
				++alphaRow;
			while (IsTransparent(sourceRow, alphaRow, transferMode) && (sourceRow < endOfSourceRow))
				{
				++sourceRow;
				if (transferMode == NOPALETTE_FLAG)
					sourceRow += 3;
				if (alphaRow)
					++alphaRow;
				++i;
				}
			*outRow = TRANSPARENT_FLAG;
			++outRow;
			*(short*)outRow = i;
			MacSwapBytes((WORD*)outRow);
			outRow += sizeof(short);
			}
		else if (IsAlpha(sourceRow, alphaRow))
			{
			*outRow = ALPHA_FLAG;
			if (transferMode == NOPALETTE_FLAG)
				*outRow |= NOPALETTE_FLAG;
			++outRow;
			tempOut = outRow;
			outRow += sizeof(short);
			for (j = 0; j < ((transferMode == NOPALETTE_FLAG) ? 3 : 1); j++)
			{
				*outRow = *sourceRow;
				++outRow;
				++sourceRow;
			}
			if (transferMode == NOPALETTE_FLAG)
				++sourceRow;
			*outRow = *alphaRow;
			++outRow;
			++alphaRow;
			while (IsAlpha(sourceRow, alphaRow) && (sourceRow < endOfSourceRow))
				{
				for (j = 0; j < ((transferMode == NOPALETTE_FLAG) ? 3 : 1); j++)
				{
					*outRow = *sourceRow;
					++outRow;
					++sourceRow;
				}
				if (transferMode == NOPALETTE_FLAG)
					++sourceRow;
				*outRow = *alphaRow;
				++outRow;
				++alphaRow;
				++i;
				}
			*tempOut = i;
			*(short*)tempOut = i;
			MacSwapBytes((WORD*)tempOut);
			tempOut += sizeof(short);
			}
		else if (IsOpaque(sourceRow, alphaRow, transferMode))
			{
			switch (transferMode)
				{
				case NOPALETTE_FLAG:
				case ALPHA_FLAG:
					if (!IsRLE(sourceRow, alphaRow, endOfSourceRow, transferMode))
						{
						*outRow = OPAQUE_FLAG;
						if (transferMode == NOPALETTE_FLAG)
							*outRow |= NOPALETTE_FLAG;
						++outRow;
						tempOut = outRow;
						outRow += sizeof(short);
						for (j = 0; j < ((transferMode == NOPALETTE_FLAG) ? 3 : 1); j++)
						{
							*outRow = *sourceRow;
							++outRow;
							++sourceRow;
						}
						if (transferMode == NOPALETTE_FLAG)
							++sourceRow;
						if (alphaRow)
							++alphaRow;
						while (IsOpaque(sourceRow, alphaRow, transferMode) && (sourceRow < endOfSourceRow)
								&& !IsRLE(sourceRow, alphaRow, endOfSourceRow, transferMode))
							{
							for (j = 0; j < ((transferMode == NOPALETTE_FLAG) ? 3 : 1); j++)
							{
								*outRow = *sourceRow;
								++outRow;
								++sourceRow;
							}
							if (transferMode == NOPALETTE_FLAG)
								++sourceRow;
							if (alphaRow)
								++alphaRow;
							++i;
							}
						*tempOut = i;
						*(short*)tempOut = i;
						MacSwapBytes((WORD*)tempOut);
						tempOut += sizeof(short);
						}
					else
						{
						// RLE
						*outRow = RLE_FLAG;
						if (transferMode == NOPALETTE_FLAG)
							*outRow |= NOPALETTE_FLAG;
						++outRow;
						tempOut = sourceRow;
						++sourceRow;
						if (transferMode == NOPALETTE_FLAG)
							sourceRow += 3;
						if (alphaRow)
							++alphaRow;
						while ((*sourceRow == *tempOut) && ((transferMode != NOPALETTE_FLAG) || ((*(sourceRow+1) == *(tempOut+1)) && (*(sourceRow+2) == *(tempOut+2))))
								&& (sourceRow < endOfSourceRow) && IsOpaque(sourceRow, alphaRow, transferMode))
							{
							++sourceRow;
							if (transferMode == NOPALETTE_FLAG)
								sourceRow += 3;
							if (alphaRow)
								++alphaRow;
							++i;
							}
						*(short*)outRow = i;
						MacSwapBytes((WORD*)outRow);
						outRow += sizeof(short);
						for (j = 0; j < ((transferMode == NOPALETTE_FLAG) ? 3 : 1); j++)
							{
							*outRow = *tempOut;
							++outRow;
							++tempOut;
							}
						}
					break;
				case ADD_FLAG:
				case MULTIPLY_FLAG:
				case OVERLAY_FLAG:
					*outRow = transferMode;
					++outRow;
					tempOut = outRow;
					outRow += sizeof(short);
					*outRow = *sourceRow;
					++outRow;
					++sourceRow;
					if (alphaRow)
						++alphaRow;
					while (IsOpaque(sourceRow, alphaRow) && (sourceRow < endOfSourceRow))
						{
						*outRow = *sourceRow;
						++outRow;
						++sourceRow;
						if (alphaRow)
							++alphaRow;
						++i;
						}
					*tempOut = i;
					*(short*)tempOut = i;
					MacSwapBytes((WORD*)tempOut);
					tempOut += sizeof(short);
					break;
				}
			}
		else
			{
			// Error!
			}
		}
	*lengthOfRow = outRow - &(tempRow[0]);
	compressedRow = new BYTE[*lengthOfRow];
	memcpy(compressedRow, tempRow, *lengthOfRow);
	return compressedRow;
	}

BYTE maxAlpha = 255;
#define AM_ALPHA_BITS 5
#define AM_COLOR_BITS 5
#define AM_COLOR_MAX	(1 << AM_COLOR_BITS)
#define AM_ALPHA_MAX	(1 << AM_ALPHA_BITS)
#define AM_ALPHA_DIVISOR 32.0

#define AT_PACK(jFore, jBack, jAlpha)	((jFore << (AM_ALPHA_BITS + AM_COLOR_BITS)) \
										| (jBack << AM_ALPHA_BITS) | jAlpha)
typedef BYTE	COLOR5;
COLOR5	majAlphaTable[AT_SIZE];

//#if defined(OS_WINDOWS)
//#define USE_ALPHA_TABLE
//#endif

void InitAlphaTable()
{
	int i,
	 j,
	 k;
	for (i = 0; i < AM_COLOR_MAX; ++i) {
		for (j = 0; j < AM_COLOR_MAX; ++j) {
			for (k = 0; k < AM_ALPHA_MAX; ++k) {
				// we'll do the alpha calculation with lots of excess
				// precision
				// just to be sure -- time is not of the essence here
				double fFore = i;
				double fBack = j;
				double fAlpha = k / AM_ALPHA_DIVISOR;
				double fResult = (fFore * fAlpha) + (fBack * (1.0 - fAlpha));

				// round and truncate
				signed char jColor = (signed char) (fResult + 0.5);
				if (jColor >= AM_COLOR_MAX) {
					jColor = AM_COLOR_MAX - 1;
				} else if (jColor < 0) {
					jColor = 0;
				}
				//vSetAlphaTable(i, j, k, jColor);
				majAlphaTable[AT_PACK(i, j, k)] = jColor << 3;
			}
		}
	}
}

#define TEX_ALPHA_R_SHIFT	10
#define TEX_ALPHA_G_SHIFT	5
#define TEX_ALPHA_B_SHIFT	0

#define FAST_ALPHA_BLEND_ASM

#define ASM_ALPHA() __asm	\
{							\
	__asm mov esi, f		\
							\
	__asm xor ecx, ecx		\
	__asm mov edi, b		\
							\
	__asm mov cl, [esi]		\
	__asm xor ebx, ebx		\
							\
	__asm mov bl, [edi]		\
	__asm inc f				\
							\
	__asm sub ebx, ecx		\
	__asm inc b				\
							\
	__asm imul ebx, eax		\
							\
	__asm mov cl, [edi]		\
	__asm sar ebx, 8		\
							\
	__asm sub ecx, ebx		\
							\
	__asm mov [edi], cl		\
}

void AlphaBlend(TEX_PIXEL &back, TEX_PIXEL fore, int alpha)
	{
#ifdef USE_ALPHA_TABLE
	RGBQUAD backColor;
	COLOR5 jFore, jBack, jAlpha;

	jFore = (fore & TEX_B_MASK) >> TEX_ALPHA_B_SHIFT;
	jBack = (back & TEX_B_MASK) >> TEX_ALPHA_B_SHIFT;
	jAlpha = alpha >> 3;
	backColor.rgbBlue = majAlphaTable[AT_PACK(jFore, jBack, jAlpha)];
	jFore = (fore & TEX_G_MASK) >> TEX_ALPHA_G_SHIFT;
	jBack = (back & TEX_G_MASK) >> TEX_ALPHA_G_SHIFT;
	backColor.rgbGreen = majAlphaTable[AT_PACK(jFore, jBack, jAlpha)];
	jFore = (fore & TEX_R_MASK) >> TEX_ALPHA_R_SHIFT;
	jBack = (back & TEX_R_MASK) >> TEX_ALPHA_R_SHIFT;
	backColor.rgbRed = majAlphaTable[AT_PACK(jFore, jBack, jAlpha)];
	back = RGBQUADtoTEX_PIXEL(backColor);
#else
#if defined(COLOR_32BIT)
	BYTE *f, *b;

	f = &PTEX_PIXEL_STRUCT(&fore)->b;
	b = &PTEX_PIXEL_STRUCT(&back)->b;

//	assert(alpha != 0);
//	assert(alpha != 255);

#if defined(FAST_ALPHA_BLEND_ASM) && defined(_WIN32)
	__asm mov eax, alpha;
	ASM_ALPHA();
	ASM_ALPHA();
	ASM_ALPHA();

#else
	*b = BYTE((*b - (((*b - *f) * alpha) >> 8)));
	TEX_PIXEL_INCREMENT(f);
	TEX_PIXEL_INCREMENT(b);
	*b = BYTE((*b - (((*b - *f) * alpha) >> 8)));
	TEX_PIXEL_INCREMENT(f);
	TEX_PIXEL_INCREMENT(b);
	*b = BYTE((*b - (((*b - *f) * alpha) >> 8)));
#endif

#else
	RGBQUAD foreColor;
	RGBQUAD backColor;

	foreColor = TEX_PIXELtoRGBQUAD(fore);
	backColor = TEX_PIXELtoRGBQUAD(back);
	backColor.rgbBlue = (BYTE)((backColor.rgbBlue - (((backColor.rgbBlue - foreColor.rgbBlue) * alpha) >> 8)));
	backColor.rgbGreen = (BYTE)((backColor.rgbGreen - (((backColor.rgbGreen - foreColor.rgbGreen) * alpha) >> 8)));
	backColor.rgbRed = (BYTE)((backColor.rgbRed - (((backColor.rgbRed - foreColor.rgbRed) * alpha) >> 8)));
	back = RGBQUADtoTEX_PIXEL(backColor);
#endif
#endif
	}

void AddBlend(TEX_PIXEL &back, TEX_PIXEL fore, int alpha)
	{
#if defined(COLOR_32BIT)
	BYTE *f, *b;
	int c;

	f = &PTEX_PIXEL_STRUCT(&fore)->b;
	b = &PTEX_PIXEL_STRUCT(&back)->b;
	c = *b + ((*f * alpha) >> 8);
	if (c > 255)
		c = 255;
	*b = c;
	TEX_PIXEL_INCREMENT(f);
	TEX_PIXEL_INCREMENT(b);
	c = *b + ((*f * alpha) >> 8);
	if (c > 255)
		c = 255;
	*b = c;
	TEX_PIXEL_INCREMENT(f);
	TEX_PIXEL_INCREMENT(b);
	c = *b + ((*f * alpha) >> 8);
	if (c > 255)
		c = 255;
	*b = c;
#else
	RGBQUAD foreColor;
	RGBQUAD backColor;
	int c;

	foreColor = TEX_PIXELtoRGBQUAD(fore);
	backColor = TEX_PIXELtoRGBQUAD(back);

	c = backColor.rgbBlue + ((foreColor.rgbBlue * alpha) >> 8);
	if (c > 255)
		c = 255;
	backColor.rgbBlue = c;

	c = backColor.rgbGreen + ((foreColor.rgbGreen * alpha) >> 8);
	if (c > 255)
		c = 255;
	backColor.rgbGreen = c;

	c = backColor.rgbRed + ((foreColor.rgbRed * alpha) >> 8);
	if (c > 255)
		c = 255;
	backColor.rgbRed = c;

	back = RGBQUADtoTEX_PIXEL(backColor);
#endif
	}

void MultiplyBlend(TEX_PIXEL &back, TEX_PIXEL fore, int alpha)
	{
#if defined(COLOR_32BIT)
	BYTE *f, *b;
	int c;

	f = &PTEX_PIXEL_STRUCT(&fore)->b;
	b = &PTEX_PIXEL_STRUCT(&back)->b;
	c = (*b * (255 - ((alpha * (255 - *f)) >> 8))) >> 8;
	if (c < 0)
		c = 0;
	*b = BYTE(c);
	TEX_PIXEL_INCREMENT(f);
	TEX_PIXEL_INCREMENT(b);
	c = (*b * (255 - ((alpha * (255 - *f)) >> 8))) >> 8;
	if (c < 0)
		c = 0;
	*b = BYTE(c);
	TEX_PIXEL_INCREMENT(f);
	TEX_PIXEL_INCREMENT(b);
	c = (*b * (255 - ((alpha * (255 - *f)) >> 8))) >> 8;
	if (c < 0)
		c = 0;
	*b = BYTE(c);
#else
	RGBQUAD foreColor;
	RGBQUAD backColor;
	int c;

	foreColor = TEX_PIXELtoRGBQUAD(fore);
	backColor = TEX_PIXELtoRGBQUAD(back);

	c = (backColor.rgbBlue * (255 - ((alpha * (255 - foreColor.rgbBlue)) >> 8))) >> 8;
	if (c < 0)
		c = 0;
	backColor.rgbBlue = c;

	c = (backColor.rgbGreen * (255 - ((alpha * (255 - foreColor.rgbGreen)) >> 8))) >> 8;
	if (c < 0)
		c = 0;
	backColor.rgbGreen = c;

	c = (backColor.rgbRed * (255 - ((alpha * (255 - foreColor.rgbRed)) >> 8))) >> 8;
	if (c < 0)
		c = 0;
	backColor.rgbRed = c;

	back = RGBQUADtoTEX_PIXEL(backColor);
#endif
	}

void OverlayBlend(TEX_PIXEL &back, TEX_PIXEL fore, int alpha)
	{
	TEX_PIXEL temp;

	// Overlay is fore * back + back;
	temp = fore;
	MultiplyBlend(temp, back, alpha);
	AddBlend(back, temp, alpha);
	}

void TextureRenderRow(BYTE *destRow, BYTE *compressedRow, int start, int length, const TEX_PIXEL *colors, COLORMAPPING *colorMap)
	{
	int size;
	int flags;
	int i;
	BYTE a;
	BYTE f;
	int BYTES_PER_PIXEL = sizeof(TEX_PIXEL);
	TEX_PIXEL tempTexColor, tempColor;
	WORD w;
	
	// skip as many blocks in the row as necessary to catch up to start
	
	flags = *compressedRow;
	++compressedRow;
	w = *(short*)compressedRow;
	MacSwapBytes(&w);
	size = w;
	compressedRow += sizeof(short);
	while ((start >= size) && (size > 0))
		{
		start -= size;
		switch (flags)
			{
			case TRANSPARENT_FLAG:
				break;
			case OPAQUE_FLAG  | NOPALETTE_FLAG:
				compressedRow += size * 3;
				break;
			case OPAQUE_FLAG:
				compressedRow += size;
				break;
			case ALPHA_FLAG | NOPALETTE_FLAG:
				compressedRow += size * 4;
				break;
			case ALPHA_FLAG:
				compressedRow += size * 2;
				break;
			case RLE_FLAG | NOPALETTE_FLAG:
				compressedRow += 3;
				break;
			case RLE_FLAG:
				++compressedRow;
				break;
			case ADD_FLAG:
			case MULTIPLY_FLAG:
			case OVERLAY_FLAG:
				compressedRow += size;
				break;
			default:
			#ifdef OS_WINDOWS
				MessageBox(NULL, "Texture decompression failed!", "Error", MB_OK | MB_TASKMODAL);
			#endif // OS_WINDOWS
				break;
			}
		flags = *compressedRow;
		++compressedRow;
//		size = *(short*)compressedRow;
		w = *(short*)compressedRow;
		MacSwapBytes(&w);
		size = w;
		compressedRow += sizeof(short);
		}
	
	// skip as many blocks in the row as necessary to catch up to start
	
	while ((length > 0) && (size > 0))
		{
		size -= start;
		switch (flags)
			{
			case TRANSPARENT_FLAG:
				if (length > size)
					{
					// This run of pixels isn't enough.
					start = 0;
					destRow += size * BYTES_PER_PIXEL;
					}
				else
					{
					// This run of pixels is enough.
					length = 0;
					}
				break;
			case OPAQUE_FLAG:
				compressedRow += start;
				if (length > size)
					{
					// This run of pixels isn't enough.
					start = 0;
					if (maxAlpha != 255)
						{
						// do alpha
						a = maxAlpha;
						for (i = 0; i < size; ++i)
							{
							tempColor = colors[*compressedRow];
							if (colorMap)
								MapColor(colorMap, &tempColor);
							AlphaBlend(*PTEX_PIXEL(destRow), tempColor, a);
							destRow += BYTES_PER_PIXEL;
							++compressedRow;
							}
						}
					else
						{
						for (i = 0; i < size; ++i)
							{
							tempColor = colors[*compressedRow];
							if (colorMap)
								MapColor(colorMap, &tempColor);
							*PTEX_PIXEL(destRow) = tempColor;
							destRow += BYTES_PER_PIXEL;
							++compressedRow;
							}
						}
					}
				else
					{
					// This run of pixels is enough.
					if (maxAlpha != 255)
						{
						// do alpha
						a = maxAlpha;
						for (i = 0; i < length; ++i)
							{
							tempColor = colors[*compressedRow];
							if (colorMap)
								MapColor(colorMap, &tempColor);
							AlphaBlend(*PTEX_PIXEL(destRow), tempColor, a);
							destRow += BYTES_PER_PIXEL;
							++compressedRow;
							}
						}
					else
						{
						for (i = 0; i < length; ++i)
							{
							tempColor = colors[*compressedRow];
							if (colorMap)
								MapColor(colorMap, &tempColor);
							*PTEX_PIXEL(destRow) = tempColor;
							destRow += BYTES_PER_PIXEL;
							++compressedRow;
							}
						}
					length = 0;
					}
				break;
			case OPAQUE_FLAG | NOPALETTE_FLAG:
				compressedRow += start*3;
				if (length > size)
					{
					// This run of pixels isn't enough.
					start = 0;
					if (maxAlpha != 255)
						{
						// do alpha
						a = maxAlpha;
						for (i = 0; i < size; ++i)
							{
							((BYTE*)&tempTexColor)[0] = *compressedRow++;
							((BYTE*)&tempTexColor)[1] = *compressedRow++;
							((BYTE*)&tempTexColor)[2] = *compressedRow++;
							((BYTE*)&tempTexColor)[3] = 254;
							if (colorMap)
								MapColor(colorMap, &tempTexColor);
							AlphaBlend(*PTEX_PIXEL(destRow), tempTexColor, a);
							destRow += BYTES_PER_PIXEL;
							}
						}
					else
						{
						for (i = 0; i < size; ++i)
							{
							((BYTE*)&tempTexColor)[0] = *compressedRow++;
							((BYTE*)&tempTexColor)[1] = *compressedRow++;
							((BYTE*)&tempTexColor)[2] = *compressedRow++;
							((BYTE*)&tempTexColor)[3] = 254;
							if (colorMap)
								MapColor(colorMap, &tempTexColor);
							*PTEX_PIXEL(destRow) = tempTexColor;
							destRow += BYTES_PER_PIXEL;
							}
						}
					}
				else
					{
					// This run of pixels is enough.
					if (maxAlpha != 255)
						{
						// do alpha
						a = maxAlpha;
						for (i = 0; i < length; ++i)
							{
							((BYTE*)&tempTexColor)[0] = *compressedRow++;
							((BYTE*)&tempTexColor)[1] = *compressedRow++;
							((BYTE*)&tempTexColor)[2] = *compressedRow++;
							((BYTE*)&tempTexColor)[3] = 254;
							if (colorMap)
								MapColor(colorMap, &tempTexColor);
							AlphaBlend(*PTEX_PIXEL(destRow), tempTexColor, a);
							destRow += BYTES_PER_PIXEL;
							}
						}
					else
						{
						for (i = 0; i < length; ++i)
							{
							((BYTE*)&tempTexColor)[0] = *compressedRow++; //b
							((BYTE*)&tempTexColor)[1] = *compressedRow++; //r
							((BYTE*)&tempTexColor)[2] = *compressedRow++; //g
							((BYTE*)&tempTexColor)[3] = 254;
							if (colorMap)
								MapColor(colorMap, &tempTexColor);
							*PTEX_PIXEL(destRow) = tempTexColor;
							destRow += BYTES_PER_PIXEL;
							}
						}
					length = 0;
					}
				break;
			case ALPHA_FLAG:
				compressedRow += start * 2;
				if (length > size)
					{
					// This run of pixels isn't enough.
					start = 0;
					//memcpy(destRow, compressedRow, size);
					for (i = 0; i < size; ++i)
						{
						f = *compressedRow;
						++compressedRow;
						a = *compressedRow;
						++compressedRow;
						if (maxAlpha != 255)
							{
							// do alpha
							a = (a * maxAlpha) >> 8; // / 255;
							}
						tempColor = colors[f];
						if (colorMap)
							MapColor(colorMap, &tempColor);
						AlphaBlend(*PTEX_PIXEL(destRow), tempColor, a);
						destRow += BYTES_PER_PIXEL;
						}
					}
				else
					{
					// This run of pixels is enough.
					//memcpy(destRow, compressedRow, length);
					for (i = 0; i < length; ++i)
						{
						f = *compressedRow;
						++compressedRow;
						a = *compressedRow;
						++compressedRow;
						if (maxAlpha != 255)
							{
							// do alpha
							a = (a * maxAlpha) >> 8; // / 255;
							}
						tempColor = colors[f];
						if (colorMap)
							MapColor(colorMap, &tempColor);
						AlphaBlend(*PTEX_PIXEL(destRow), tempColor, a);
						destRow += BYTES_PER_PIXEL;
						}
					length = 0;
					}
				break;
			case ALPHA_FLAG | NOPALETTE_FLAG:
				compressedRow += start * 4;
				if (length > size)
					{
					// This run of pixels isn't enough.
					start = 0;
					//memcpy(destRow, compressedRow, size);
					for (i = 0; i < size; ++i)
						{
						((BYTE*)&tempTexColor)[0] = *compressedRow++;
						((BYTE*)&tempTexColor)[1] = *compressedRow++;
						((BYTE*)&tempTexColor)[2] = *compressedRow++;
						((BYTE*)&tempTexColor)[3] = 254;
						a = *compressedRow;
						++compressedRow;
						if (maxAlpha != 255)
							{
							// do alpha
							a = (a * maxAlpha) >> 8; // / 255;
							}
						if (colorMap)
							MapColor(colorMap, &tempTexColor);
						AlphaBlend(*PTEX_PIXEL(destRow), tempTexColor, a);
						destRow += BYTES_PER_PIXEL;
						}
					}
				else
					{
					// This run of pixels is enough.
					//memcpy(destRow, compressedRow, length);
					for (i = 0; i < length; ++i)
						{
						((BYTE*)&tempTexColor)[0] = *compressedRow++;
						((BYTE*)&tempTexColor)[1] = *compressedRow++;
						((BYTE*)&tempTexColor)[2] = *compressedRow++;
						((BYTE*)&tempTexColor)[3] = 254;
						a = *compressedRow;
						++compressedRow;
						if (maxAlpha != 255)
							{
							// do alpha
							a = (a * maxAlpha) >> 8; // / 255;
							}
						if (colorMap)
							MapColor(colorMap, &tempTexColor);
						AlphaBlend(*PTEX_PIXEL(destRow), tempTexColor, a);
						destRow += BYTES_PER_PIXEL;
						}
					length = 0;
					}
				break;
			case RLE_FLAG:
				if (length > size)
					{
					// This run of pixels isn't enough.
					start = 0;
					//memset(destRow, *compressedRow, size);
					tempTexColor = colors[*compressedRow];
					if (colorMap)
						MapColor(colorMap, &tempTexColor);
					if (maxAlpha != 255)
						{
						// do alpha
						a = maxAlpha;
						for (i = 0; i < size; ++i)
							{
							AlphaBlend(*PTEX_PIXEL(destRow), tempTexColor, a);
							destRow += BYTES_PER_PIXEL;
							}
						}
					else
						{
						for (i = 0; i < size; ++i)
							{
							*PTEX_PIXEL(destRow) = tempTexColor;
							destRow += BYTES_PER_PIXEL;
							}
						}
					++compressedRow;
					}
				else
					{
					// This run of pixels is enough.
					//memset(destRow, *compressedRow, length);
					tempTexColor = colors[*compressedRow];
					if (colorMap)
						MapColor(colorMap, &tempTexColor);
					if (maxAlpha != 255)
						{
						// do alpha
						a = maxAlpha;
						for (i = 0; i < length; ++i)
							{
							AlphaBlend(*PTEX_PIXEL(destRow), tempTexColor, a);
							destRow += BYTES_PER_PIXEL;
							}
						}
					else
						{
						for (i = 0; i < length; ++i)
							{
							*PTEX_PIXEL(destRow) = tempTexColor;
							destRow += BYTES_PER_PIXEL;
							}
						}
					length = 0;
					}
				break;
			case RLE_FLAG | NOPALETTE_FLAG:
				if (length > size)
					{
					// This run of pixels isn't enough.
					start = 0;
					//memset(destRow, *compressedRow, size);
					((BYTE*)&tempTexColor)[0] = *compressedRow++;
					((BYTE*)&tempTexColor)[1] = *compressedRow++;
					((BYTE*)&tempTexColor)[2] = *compressedRow++;
					((BYTE*)&tempTexColor)[3] = 254;
					if (colorMap)
						MapColor(colorMap, &tempTexColor);
					if (maxAlpha != 255)
						{
						// do alpha
						a = maxAlpha;
						for (i = 0; i < size; ++i)
							{
							AlphaBlend(*PTEX_PIXEL(destRow), tempTexColor, a);
							destRow += BYTES_PER_PIXEL;
							}
						}
					else
						{
						for (i = 0; i < size; ++i)
							{
							*PTEX_PIXEL(destRow) = tempTexColor;
							destRow += BYTES_PER_PIXEL;
							}
						}
					}
				else
					{
					// This run of pixels is enough.
					//memset(destRow, *compressedRow, length);
					((BYTE*)&tempTexColor)[0] = *compressedRow++;
					((BYTE*)&tempTexColor)[1] = *compressedRow++;
					((BYTE*)&tempTexColor)[2] = *compressedRow++;
					((BYTE*)&tempTexColor)[3] = 254;
					if (colorMap)
						MapColor(colorMap, &tempTexColor);
					if (maxAlpha != 255)
						{
						// do alpha
						a = maxAlpha;
						for (i = 0; i < length; ++i)
							{
							AlphaBlend(*PTEX_PIXEL(destRow), tempTexColor, a);
							destRow += BYTES_PER_PIXEL;
							}
						}
					else
						{
						for (i = 0; i < length; ++i)
							{
							*PTEX_PIXEL(destRow) = tempTexColor;
							destRow += BYTES_PER_PIXEL;
							}
						}
					length = 0;
					}
				break;
			case ADD_FLAG:
				compressedRow += start;
				a = maxAlpha;
				if (length > size)
					{
					// This run of pixels isn't enough.
					start = 0;
					for (i = 0; i < size; ++i)
						{
						AddBlend(*PTEX_PIXEL(destRow), colors[*compressedRow], a);
						destRow += BYTES_PER_PIXEL;
						++compressedRow;
						}
					}
				else
					{
					// This run of pixels is enough.
					for (i = 0; i < length; ++i)
						{
						AddBlend(*PTEX_PIXEL(destRow), colors[*compressedRow], a);
						destRow += BYTES_PER_PIXEL;
						++compressedRow;
						}
					length = 0;
					}
				break;
			case MULTIPLY_FLAG:
				compressedRow += start;
				a = maxAlpha;
				if (length > size)
					{
					// This run of pixels isn't enough.
					start = 0;
					for (i = 0; i < size; ++i)
						{
						MultiplyBlend(*PTEX_PIXEL(destRow), colors[*compressedRow], a);
						destRow += BYTES_PER_PIXEL;
						++compressedRow;
						}
					}
				else
					{
					// This run of pixels is enough.
					for (i = 0; i < length; ++i)
						{
						MultiplyBlend(*PTEX_PIXEL(destRow), colors[*compressedRow], a);
						destRow += BYTES_PER_PIXEL;
						++compressedRow;
						}
					length = 0;
					}
				break;
			case OVERLAY_FLAG:
				compressedRow += start;
				a = maxAlpha;
				if (length > size)
					{
					// This run of pixels isn't enough.
					start = 0;
					for (i = 0; i < size; ++i)
						{
						OverlayBlend(*PTEX_PIXEL(destRow), colors[*compressedRow], a);
						destRow += BYTES_PER_PIXEL;
						++compressedRow;
						}
					}
				else
					{
					// This run of pixels is enough.
					for (i = 0; i < length; ++i)
						{
						OverlayBlend(*PTEX_PIXEL(destRow), colors[*compressedRow], a);
						destRow += BYTES_PER_PIXEL;
						++compressedRow;
						}
					length = 0;
					}
				break;
			default:
			#ifdef OS_WINDOWS
				MessageBox(NULL, "Texture decompression failed!", "Error", MB_OK | MB_TASKMODAL);
			#endif // OS_WINDOWS
				break;
			}

		if (length)
			{
			length -= size;
			flags = *compressedRow;
			++compressedRow;
//			size = *(short*)compressedRow;
			w = *(short*)compressedRow;
			MacSwapBytes(&w);
			size = w;
			compressedRow += sizeof(short);
			}
		}
	}

static void MapColor(COLORMAPPING *colorMap, PTEX_PIXEL pixel)
{
#if defined(COLOR_32BIT)
	int i;
	
	for (i = 0; i < colorMap->numMaps; i++)
	{
		if (((PTEX_PIXEL_STRUCT)pixel)->r == colorMap->mappings[i].fromColor.r &&
			((PTEX_PIXEL_STRUCT)pixel)->g == colorMap->mappings[i].fromColor.g &&
			((PTEX_PIXEL_STRUCT)pixel)->b == colorMap->mappings[i].fromColor.b)
		{
			((PTEX_PIXEL_STRUCT)pixel)->r = colorMap->mappings[i].toColor.r;
			((PTEX_PIXEL_STRUCT)pixel)->g = colorMap->mappings[i].toColor.g;
			((PTEX_PIXEL_STRUCT)pixel)->b = colorMap->mappings[i].toColor.b;
		}
	}	
#endif
}

#if defined(OS_WINDOWS) || defined(OS_BE)
int fsize(char* filename)
	{
	FILE *file;
	int old, r;

	file = fopen(filename, "rb");
	old = ftell(file);
	fseek(file, 0, SEEK_END);
	r = ftell(file);
	fseek(file, old, SEEK_SET);
	fclose(file);
	return r;
	}
#else
#include <unix.h>
#include <unistd.h>
int fsize(char* filename)
	{
	long 		dataLength;
	OSErr		myErr;
	FSSpec		mySpec;
	short		myVolRefNum;
	
	myErr = FSMakeFSSpec (0, 0, c2pstr(filename), &mySpec);
	if (myErr == noErr){
		myErr = FSpOpenDF(&mySpec, 0, &myVolRefNum);
		myErr = GetEOF(myVolRefNum, &dataLength);
		myErr = FSClose (myVolRefNum);
		}
	
	return (int)dataLength;
	}
#endif

void Convert2BMPsToTEX(char *inColorFilename, char *inAlphaFilename, char *outFilename, int transferMode)
	{
	int i, j, l;
	FILE *inColorFile;
	FILE *inAlphaFile;
	FILE *outFile;
	BYTE *inColorData;
	BYTE *inAlphaData;
	int inColorFileSize;
	int inAlphaFileSize;
	int inWidth, inHeight, inWidthInBytes;
	BYTE *inColorPixels;
	BYTE *inAlphaPixels;
	int colorsUsed;
	RGBQUAD inColorPalette[256];
	int inColorBytesPerPixel;
	RGB *pic;
	int inAlphaWidthInBytes;

	j = 0;
	inColorFile	= fopen(inColorFilename, "rb");
	inAlphaFile	= fopen(inAlphaFilename, "rb");
	outFile		= fopen(outFilename, "wb");
	if ((inColorFile == NULL) ||
		(outFile == NULL))
		{
		if (inColorFile)
			fclose(inColorFile);
		if (inAlphaFile)
			fclose(inAlphaFile);
		if (outFile)
			fclose(outFile);
		}
		
	inColorFileSize = fsize(inColorFilename);

	if (inColorFileSize)
		{
		inColorData = new BYTE[inColorFileSize];
		if (inColorData)
			{
			FileReadWords(inColorFile, (WORD*)(inColorData + 0));
			FileReadDWords(inColorFile, (DWORD*)(inColorData + 2));
			FileReadWords(inColorFile, (WORD*)(inColorData + 6), 2);
			FileReadDWords(inColorFile, (DWORD*)(inColorData + 10), 4);
			FileReadWords(inColorFile, (WORD*)(inColorData + 26), 2);
			FileReadDWords(inColorFile, (DWORD*)(inColorData + 30), 6);
			FileReadBytes(inColorFile, inColorData + 54, inColorFileSize - 54);
			}
		else
			{
			if (inColorFile)
				fclose(inColorFile);
			if (inAlphaFile)
				fclose(inAlphaFile);
			if (outFile)
				fclose(outFile);
			return;
			}
		}

	if (inAlphaFile)
		{
		inAlphaFileSize = fsize(inAlphaFilename);
		if (inAlphaFileSize)
			{
			inAlphaData = new BYTE[inAlphaFileSize];
			if (inAlphaData)
				{
				FileReadWords(inAlphaFile, (WORD*)(inAlphaData + 0));
				FileReadDWords(inAlphaFile, (DWORD*)(inAlphaData + 2));
				FileReadWords(inAlphaFile, (WORD*)(inAlphaData + 6), 2);
				FileReadDWords(inAlphaFile, (DWORD*)(inAlphaData + 10), 4);
				FileReadWords(inAlphaFile, (WORD*)(inAlphaData + 26), 2);
				FileReadDWords(inAlphaFile, (DWORD*)(inAlphaData + 30), 6);
				FileReadBytes(inAlphaFile, inAlphaData + 54, inAlphaFileSize - 54);
				}
			else
				{
				if (inColorFile)
					fclose(inColorFile);
				if (inAlphaFile)
					fclose(inAlphaFile);
				if (outFile)
					fclose(outFile);
				return;
				}
			}
		else
			{
			if (inColorFile)
				fclose(inColorFile);
			if (inAlphaFile)
				fclose(inAlphaFile);
			if (outFile)
				fclose(outFile);
			return;
			}
		}


	inWidth = *(DWORD*)(inColorData + 18);
	inHeight = *(DWORD*)(inColorData + 22);
	inColorBytesPerPixel = *(WORD*)(inColorData + 28) / 8;
	inWidthInBytes = (inWidth*inColorBytesPerPixel) + 3 -
		(((inWidth*inColorBytesPerPixel)-1) % 4);
	inAlphaWidthInBytes = inWidth + 3 - ((inWidth-1) % 4);
	inColorPixels = new BYTE[inWidthInBytes * inHeight];
	memset(inColorPixels, 0, inWidthInBytes * inHeight);
	
	if (inAlphaFile)
		{
		inAlphaPixels = new BYTE[inWidthInBytes * inHeight];
		memset(inAlphaPixels, 0, inWidthInBytes * inHeight);
		}


	// Get the palette from the color file.
	colorsUsed = *(DWORD*)(inColorData + 46);

	if (colorsUsed)
		{
		for (i = 0; i < colorsUsed; ++i)
			{
			inColorPalette[i].rgbBlue = *(inColorData + 54 + i*4);
			inColorPalette[i].rgbGreen = *(inColorData + 54 + i*4 + 1);
			inColorPalette[i].rgbRed = *(inColorData + 54 + i*4 + 2);
			inColorPalette[i].rgbReserved = *(inColorData + 54 + i*4 + 3);
			}
		for (i = (int)colorsUsed; i < 256; ++i)
			{
			inColorPalette[i].rgbBlue = *(inColorData + 54);
			inColorPalette[i].rgbGreen = *(inColorData + 54 + 1);
			inColorPalette[i].rgbRed = *(inColorData + 54 + 2);
			inColorPalette[i].rgbReserved = *(inColorData + 54 + 3);
			}
		}
	else
		{
		for (i = 0; i < 256; ++i)
			{
			inColorPalette[i].rgbBlue = *(inColorData + 54 + i*4);
			inColorPalette[i].rgbGreen = *(inColorData + 54 + i*4 + 1);
			inColorPalette[i].rgbRed = *(inColorData + 54 + i*4 + 2);
			inColorPalette[i].rgbReserved = *(inColorData + 54 + i*4 + 3);
			}
		}

	// Get the palette from the alpha file.
	// Do later if the palettes aren't in the right order.

	// Check for RLE compression
	switch (*(DWORD*)(inColorData + 30))
		{
		case BI_RLE8:
			// Compressed
			BitmapCopyRLEPixels(inWidthInBytes, inColorPixels, inColorData + *(DWORD*)(inColorData + 10));
			break;
		case BI_RGB:
			// Uncompressed
			BitmapCopyPixels(inWidthInBytes, inColorPixels, inColorData + *(DWORD*)(inColorData + 10), inWidthInBytes, inHeight);
			break;
		default:
			// Unknown format, don't use it.
			break;
		}

	if (inAlphaFile)
		{
		switch (*(DWORD*)(inAlphaData + 30))
			{
			case BI_RLE8:
				// Compressed
				BitmapCopyRLEPixels(inWidthInBytes, inAlphaPixels, inAlphaData + *(DWORD*)(inAlphaData + 10));
				break;
			case BI_RGB:
				// Uncompressed
				BitmapCopyPixels(inWidthInBytes, inAlphaPixels, inAlphaData + *(DWORD*)(inAlphaData + 10), inWidthInBytes, inHeight);
				break;
			default:
				// Unknown format, don't use it.
				break;
			}
		}

	BYTE *out;
	TEX *tex;
	int size;
	RGBA *bitmap;
	int x, y;

	out = new BYTE[10000000];
	tex = new TEX();
	bitmap = new RGBA[inWidth * inHeight];

	// Copy pixels to bitmap.
	switch (inColorBytesPerPixel)
		{
		case 1:
			for (y = 0; y < inHeight; ++y)
				for (x = 0; x < inWidth; ++x)
					{
					bitmap[y * inWidth + x].r = inColorPalette[inColorPixels[(inHeight - y - 1) * inWidthInBytes + x]].rgbRed;
					bitmap[y * inWidth + x].g = inColorPalette[inColorPixels[(inHeight - y - 1) * inWidthInBytes + x]].rgbGreen;
					bitmap[y * inWidth + x].b = inColorPalette[inColorPixels[(inHeight - y - 1) * inWidthInBytes + x]].rgbBlue;
					if (inAlphaFile)
						bitmap[y * inWidth + x].a = inAlphaPixels[(inHeight - y - 1) * inWidthInBytes + x];
					else
						bitmap[y * inWidth + x].a = 255;
					}
			break;
		case 3:
			for (y = 0; y < inHeight; ++y)
				for (x = 0; x < inWidth; ++x)
					{
					bitmap[y * inWidth + x].b = inColorPixels[(inHeight - y - 1) * inWidthInBytes + (x * inColorBytesPerPixel)];
					bitmap[y * inWidth + x].g = inColorPixels[(inHeight - y - 1) * inWidthInBytes + (x * inColorBytesPerPixel) + 1];
					bitmap[y * inWidth + x].r = inColorPixels[(inHeight - y - 1) * inWidthInBytes + (x * inColorBytesPerPixel) + 2];
					if (inAlphaFile)
						bitmap[y * inWidth + x].a = inAlphaPixels[(inHeight - y - 1) * inAlphaWidthInBytes + x];
					else
						bitmap[y * inWidth + x].a = 255;
					}
			break;
		}

	l = inWidth * inHeight;
	pic = new RGB[l];

	for (y = 0; y < inHeight; ++y)
		{
		for (x = 0; x < inWidth; ++x)
			{
			// Don't look at transparent pixels.
			if (bitmap[y * inWidth + x].a != 0)
				{
				pic[j].red = bitmap[y * inWidth + x].r & 0xF8;
				pic[j].green = bitmap[y * inWidth + x].g & 0xF8;
				pic[j].blue = bitmap[y * inWidth + x].b & 0xF8;
				++j;
				}
			}
		}

	tex->FigureBestPalette2(pic, j);
	tex->AddCel(bitmap, inWidth, inHeight, transferMode, TRUE);
	size = tex->Write(out);
	fwrite(out, 1, size, outFile);

	delete[] pic;
	delete[] bitmap;
	delete tex;
	delete[] out;

	// Cleanup
	delete[] inColorPixels;
	delete[] inColorData;
	fclose(inColorFile);
	if (inAlphaFile)
		{
		delete[] inAlphaPixels;
		delete[] inAlphaData;
		fclose(inAlphaFile);
		}
	fclose(outFile);
	}


#define RLE_ESCAPE	0
#define RLE_EOL		0
#define RLE_EOF		1
#define RLE_JMP		2
#define RLE_RUN		3

void BitmapCopyRLEPixels(WORD wWidthBytes, BYTE *pb, BYTE *prle)
	{
	BYTE	cnt;
	BYTE	b;
	WORD	x;
	WORD	dx,dy;
								
	x = 0;
								
	for(;;)
		{
		cnt	= *prle++;
		b	= *prle++;
									
		if (cnt == RLE_ESCAPE)
			{
			switch (b)
				{
				case RLE_EOF:
					return;
				case RLE_EOL:
					pb += wWidthBytes - x;
					x = 0;
					break;
				case RLE_JMP:
					dx = (WORD)*prle++;
					dy = (WORD)*prle++;
					pb += ((DWORD)wWidthBytes * dy) + dx;
					x  += dx;
					break;
				default:
					cnt = b;
					x  += (WORD)cnt;
					while (cnt-- > 0)
						*pb++ = (*prle++);
					if (b & 1)
						prle++;
					break;
				}
			}
		else
			{
			x += (WORD)cnt;
							
			while (cnt-- > 0)
				*pb++ = (b);
			}
		}
	}

void BitmapCopyPixels(WORD , BYTE *pb, BYTE *prle, int wWidthInBytes, int wHeight)
	{
	int i, j;

	j = wWidthInBytes * wHeight;
	for (i = 0; i < j; ++i)
		{
		*pb++ = (*prle++);
		}
	}

#define SLACK 100
#define CLOSE_ENOUGH 1

BYTE PaletteFindNearest(RGBQUAD colors[256], BYTE red, BYTE green, BYTE blue)
	{
	int i;
	int best;
	int bestDelta;
	int delta;
	int dr, dg, db;
	RGBQUAD *theColor;

	best = 0;
	bestDelta = 256*256*256;
	theColor = &(colors[0]);
	for (i = 0; i < 256; ++i)
		{
		dr = theColor->rgbRed - red;
		dg = theColor->rgbGreen - green;
		db = theColor->rgbBlue - blue;
		++theColor;
		if ((dr < -SLACK) || (dr > SLACK))
			continue;
		if ((dg < -SLACK) || (dg > SLACK))
			continue;
		if ((db < -SLACK) || (db > SLACK))
			continue;
		delta = dr*dr + dg*dg + db*db;
		if (delta < 0)
			delta = -delta;
		if (delta < bestDelta)
			{
			best = i;
			bestDelta = delta;
			}
		if (delta < CLOSE_ENOUGH)
			{
			i = 256;
			}
		}
	return best;
	}

void FileReadBytes(FILE *file, BYTE *buffer, int count)
	{
	fread(buffer, 1, count, file);
	}

void FileReadWords(FILE *file, WORD *buffer, int count)
	{
	fread(buffer, 2, count, file);
	int i;

	for (i = 0; i < count; ++i)
		MacSwapBytes(&(buffer[i]));
	}

void FileReadDWords(FILE *file, DWORD *buffer, int count)
	{
	fread(buffer, 4, count, file);
	int i;

	for (i = 0; i < count; ++i)
		MacSwapBytes(&(buffer[i]));
	}

void FileWriteBytes(FILE *file, BYTE *buffer, int count)
	{
	fwrite(buffer, 1, count, file);
	}

void FileWriteWords(FILE *file, WORD *buffer, int count)
	{
	int i;

	for (i = 0; i < count; ++i)
		MacSwapBytes(&(buffer[i]));
	fwrite(buffer, 2, count, file);
	}

void FileWriteDWords(FILE *file, DWORD *buffer, int count)
	{
	int i;

	for (i = 0; i < count; ++i)
		MacSwapBytes(&(buffer[i]));
	fwrite(buffer, 4, count, file);
	}

void MacSwapBytes(WORD *wData)
	{
#ifdef OS_LITTLE_ENDIAN
#else
	SwapBytes(wData);
#endif // OS_LITTLE_ENDIAN
	}

void MacSwapBytes(DWORD *dwData)
	{
#ifdef OS_LITTLE_ENDIAN
#else
	SwapBytes(dwData);
#endif // OS_LITTLE_ENDIAN
	}

void SwapBytes(WORD *wData)
	{
	*wData = (*wData << 8) | (*wData >> 8);
	}

void SwapBytes(DWORD *dwData)
	{
	WORD hiWORD = (WORD)(*dwData >> 16);
	WORD loWORD = (WORD)(*dwData);
	SwapBytes(&hiWORD);
	SwapBytes(&loWORD);
	*dwData =  loWORD<<16 | hiWORD;
	}

void TextureUnFlatten(BYTE *stuff,									// input params
		short *numLoops, LOOP **loops, RGB **colors)	// output params
	{
	int		i;
	WORD w;

	i = *(int*)stuff;	// tag = 'tex '
	stuff += sizeof(int);
	i = *(int*)stuff;	// version
	stuff += sizeof(int);
	*colors = (RGB*)stuff;	// palette
	stuff += 768;
	w = *(short*)stuff;
	stuff += sizeof(short);
	MacSwapBytes(&w);
	*numLoops = w;

	*loops = new LOOP[*numLoops];
	for (i = 0; i < *numLoops; ++i)
		{
		TextureReadLoop(&stuff, loops, i);
		}
	}

void TextureFlatten(BYTE **fileData, int *lengthOfFileData,			// output params
		BYTE **rows, short numLoops, LOOP *loops, RGB *colors)		// input params
	{
	}

void TextureReadLoop(BYTE **stuff, LOOP **loops, int theLoop)
	{
	int i;
	int numCels;
	WORD w;

	w = *(short*)*stuff;
	*stuff += sizeof(short);
	MacSwapBytes(&w);	
	numCels = w;
	loops[theLoop]->numCels = numCels;
	loops[theLoop]->cels = new CEL[numCels];

	for (i = 0; i < numCels; ++i)
		{
		TextureReadCel(stuff, loops, theLoop, i);
		}
	}

void TextureReadCel(BYTE **stuff, LOOP **loops, int theLoop, int theCel)
	{
	int i, j;
	int height;
	WORD w;

	w = *(short*)*stuff;
	*stuff += sizeof(short);
	MacSwapBytes(&w);
	loops[theLoop]->cels[theCel].width = w;

	w = *(short*)*stuff;
	*stuff += sizeof(short);
	MacSwapBytes(&w);
	height = w;
	loops[theLoop]->cels[theCel].height = height;

	w = *(short*)*stuff;
	*stuff += sizeof(short);
	MacSwapBytes(&w);
	loops[theLoop]->cels[theCel].xOrigin = w;

	w = *(short*)*stuff;
	*stuff += sizeof(short);
	MacSwapBytes(&w);
	loops[theLoop]->cels[theCel].yOrigin = w;

	w = *(short*)*stuff;
	*stuff += sizeof(short);
	MacSwapBytes(&w);
	loops[theLoop]->cels[theCel].x = w;

	w = *(short*)*stuff;
	*stuff += sizeof(short);
	MacSwapBytes(&w);
	loops[theLoop]->cels[theCel].y = w;

	loops[theLoop]->cels[theCel].rows = new PBYTE[height];

	for (i = 0; i < height; ++i)
		{
		w = *(short*)*stuff;
		*stuff += sizeof(short);
		MacSwapBytes(&w);
		j = w;
		loops[theLoop]->cels[theCel].rows[i] = *stuff;
		*stuff += j;
		}
	}

BYTE TextureColorOfPoint(int loop, int cel, LOOP *loops, int x, int y)
	{
	BYTE *compressedRow;
	int size;
	int flags;
	int start;
	WORD w;

	if (x >= loops[loop].cels[cel].width)
		return 0;
	if (y >= loops[loop].cels[cel].height)
		return 0;
	start = x;
	compressedRow = loops[loop].cels[cel].rows[y];
	flags = *compressedRow;
	++compressedRow;
	w = *(short*)compressedRow;
	compressedRow += sizeof(short);
	MacSwapBytes(&w);
	size = w;
	while ((start >= size) && (size > 0))
		{
		start -= size;
		switch (flags)
			{
			case TRANSPARENT_FLAG:
				break;
			case OPAQUE_FLAG:
				compressedRow += size;
				break;
			case ALPHA_FLAG:
				compressedRow += size * 2;
				break;
			case RLE_FLAG:
				++compressedRow;
				break;
			default:
			#ifdef OS_WINDOWS
				MessageBox(NULL, "Texture decompression failed!", "Error", MB_OK | MB_TASKMODAL);
			#endif // OS_WINDOWS
				break;
			}
		flags = *compressedRow;
		++compressedRow;
		w = *(short*)compressedRow;
		compressedRow += sizeof(short);
		MacSwapBytes(&w);
		size = w;
		}
	if (size > 0)
		{
		size -= start;
		switch (flags)
			{
			case TRANSPARENT_FLAG:
				return 0;
			case OPAQUE_FLAG:
				compressedRow += start;
				return *compressedRow;
			case ALPHA_FLAG:
				return 0;
			case RLE_FLAG:
				return *compressedRow;
			default:
			#ifdef OS_WINDOWS
				MessageBox(NULL, "Texture decompression failed!", "Error", MB_OK | MB_TASKMODAL);
			#endif // OS_WINDOWS
				return 0;
			}
		}
	return 0;
	}

BOOL TextureIsPointOpaque(int loop, int cel, LOOP *loops, int x, int y)
	{
	BYTE *compressedRow;
	int size;
	int flags;
	int start;
	WORD w;

	if (x >= loops[loop].cels[cel].width)
		return FALSE;
	if (y >= loops[loop].cels[cel].height)
		return FALSE;
	start = x;
	compressedRow = loops[loop].cels[cel].rows[y];
	flags = *compressedRow;
	++compressedRow;
	w = *(short*)compressedRow;
	compressedRow += sizeof(short);
	MacSwapBytes(&w);
	size = w;
	while ((start >= size) && (size > 0))
		{
		start -= size;
		switch (flags)
			{
			case TRANSPARENT_FLAG:
				break;
			case OPAQUE_FLAG:
				compressedRow += size;
				break;
			case ALPHA_FLAG:
				compressedRow += size * 2;
				break;
			case RLE_FLAG:
				++compressedRow;
				break;
			default:
			#ifdef OS_WINDOWS
				MessageBox(NULL, "Texture decompression failed!", "Error", MB_OK | MB_TASKMODAL);
			#endif // OS_WINDOWS
				break;
			}
		flags = *compressedRow;
		++compressedRow;
		w = *(short*)compressedRow;
		compressedRow += sizeof(short);
		MacSwapBytes(&w);
		size = w;
		}
	if (size > 0)
		{
		size -= start;
		switch (flags)
			{
			case TRANSPARENT_FLAG:
				return FALSE;
			case OPAQUE_FLAG:
				compressedRow += start;
				return TRUE;
			case ALPHA_FLAG:
				return FALSE;
			case RLE_FLAG:
				return TRUE;
			default:
			#ifdef OS_WINDOWS
				MessageBox(NULL, "Texture decompression failed!", "Error", MB_OK | MB_TASKMODAL);
			#endif // OS_WINDOWS
				return FALSE;
			}
		}
	return FALSE;
	}

BYTE inverseColorTable[AT_SIZE];

void InitInverseColorTable(RGB colors[256])
	{
	int i, j, k;
	RGBQUAD rgbColors[256];

	for (i = 0; i < 256; ++i)
		{
		rgbColors[i].rgbRed = colors[i].red;
		rgbColors[i].rgbGreen = colors[i].green;
		rgbColors[i].rgbBlue = colors[i].blue;
		}

	for (i = 0; i < 32; ++i)
		{
		for (j = 0; j < 32; ++j)
			{
			for (k = 0; k < 32; ++k)
				{
				inverseColorTable[i*32*32 + j*32 + k] = PaletteFindNearest(rgbColors, i<<3, j<<3, k<<3);
				}
			}
		}
	}

BYTE InverseColorLookup(BYTE r, BYTE g, BYTE b)
	{
	return inverseColorTable[(r>>3)*32*32 + (g>>3)*32 + (b>>3)];
	}

void TextureGetPixel(int loop, int cel, LOOP *loops, int x, int y,
					 BYTE &color, BYTE &alpha, BYTE &flags)
	{
	BYTE *compressedRow;
	int size;
	int start;
	WORD w;

	color = 0;
	alpha = 0;
	flags = TRANSPARENT_FLAG;
	if (x >= loops[loop].cels[cel].width)
		return;
	if (y >= loops[loop].cels[cel].height)
		return;
	start = x;
	compressedRow = loops[loop].cels[cel].rows[y];
	flags = *compressedRow;
	++compressedRow;
	w = *(short*)compressedRow;
	compressedRow += sizeof(short);
	MacSwapBytes(&w);
	size = w;
	while ((start >= size) && (size > 0))
		{
		start -= size;
		switch (flags)
			{
			case TRANSPARENT_FLAG:
				break;
			case OPAQUE_FLAG:
			case ADD_FLAG:
			case MULTIPLY_FLAG:
			case OVERLAY_FLAG:
				compressedRow += size;
				break;
			case ALPHA_FLAG:
				compressedRow += size * 2;
				break;
			case RLE_FLAG:
				++compressedRow;
				break;
			default:
			#ifdef OS_WINDOWS
				MessageBox(NULL, "Texture decompression failed!", "Error", MB_OK | MB_TASKMODAL);
			#endif // OS_WINDOWS
				break;
			}
		flags = *compressedRow;
		++compressedRow;
		w = *(short*)compressedRow;
		compressedRow += sizeof(short);
		MacSwapBytes(&w);
		size = w;
		}
	if (size > 0)
		{
		size -= start;
		switch (flags)
			{
			case TRANSPARENT_FLAG:
				color = 0;
				return;
			case OPAQUE_FLAG:
			case ADD_FLAG:
			case MULTIPLY_FLAG:
			case OVERLAY_FLAG:
				compressedRow += start;
				color = *compressedRow;
				return;
			case ALPHA_FLAG:
				compressedRow += start * 2;
				color = *compressedRow;
				++compressedRow;
				alpha = *compressedRow;
				return;
			case RLE_FLAG:
				color = *compressedRow;
				return;
			default:
			#ifdef OS_WINDOWS
				MessageBox(NULL, "Texture decompression failed!", "Error", MB_OK | MB_TASKMODAL);
			#endif // OS_WINDOWS
				return;
			}
		}
	color = 0;
	flags = TRANSPARENT_FLAG;
	}

void TURApplyAlpha(BYTE **destRow, TEX_PIXEL texColor, BYTE a)
{
	RGBA rgbaSrcPixel;
	
	// if the destination pixel alpha is 0, it is transparent and
	// we can copy the source pixel opaquely over it
	// THIS SCHEME DOES NOT WORK FOR ALPHA IS NEVER 0 FOR NOW
	
	if ((*destRow)[3] == 0) {
		rgbaSrcPixel = TEX_PIXELtoRGBA(texColor);
		*((*destRow)++) = rgbaSrcPixel.b;
		*((*destRow)++) = rgbaSrcPixel.g;
		*((*destRow)++) = rgbaSrcPixel.r;
		*((*destRow)++) = a;
	}
	else { // otherwise we do an alpha blend of the source and destination
		AlphaBlend(*PTEX_PIXEL(*destRow), texColor, a);
		(*destRow) += 4; // RGBA
	}
}

//======================================================================
// Expand a row of TEX pixels into an uncompressed RGBA buffer.  Use the
// buffer as the background to apply all effects.
//======================================================================
void TextureUncompressRow(BYTE *destRow, BYTE *compressedRow, int length, const TEX_PIXEL *colors, COLORMAPPING *colorMap)
{
	int size;
	int flags;
	int i;
	BYTE a;
	BYTE f;
	int BYTES_PER_PIXEL = sizeof(TEX_PIXEL);
	TEX_PIXEL tempTexColor;
	WORD w;
	RGBA rgbaSrcPixel;
	
	flags = *compressedRow;
	++compressedRow;
	w = *(short*)compressedRow;
	MacSwapBytes(&w);
	size = w;
	compressedRow += sizeof(short);
	
	while ((length > 0) && (size > 0)) {
		// see what this spans flag says
		switch (flags)
		{
		case TRANSPARENT_FLAG:
			if (length > size)
			{
				// This run of pixels isn't enough to complete the row
				destRow += size * 4; // RGBA
			}
			else
			{
				// This run of pixels completes the row
				length = 0;
			}
			break;
		
		case OPAQUE_FLAG:
			if (length > size)
			{
				// This run of pixels isn't enough.
				if (maxAlpha != 255)
				{
					// do alpha
					a = maxAlpha;
					for (i = 0 ; i < size ; ++i)
					{
						tempTexColor = colors[*compressedRow];
						if (colorMap)
							MapColor(colorMap, &tempTexColor);
						
						TURApplyAlpha(&destRow, tempTexColor, a);
						
						++compressedRow;
					}
				}
				else
				{
					// no alpha
					for (i = 0; i < size; ++i)
					{
						tempTexColor = colors[*compressedRow];
						if (colorMap)
							MapColor(colorMap, &tempTexColor);
						
						rgbaSrcPixel = TEX_PIXELtoRGBA(tempTexColor);
						*destRow++ = rgbaSrcPixel.b;
						*destRow++ = rgbaSrcPixel.g;
						*destRow++ = rgbaSrcPixel.r;
						*destRow++ = 255;

						++compressedRow;
					}
				}
			}
			else
			{
				// This run of pixels is enough.
				if (maxAlpha != 255)
				{
					// do alpha
					a = maxAlpha;
					for (i = 0; i < length; ++i)
					{
						tempTexColor = colors[*compressedRow];
						if (colorMap)
							MapColor(colorMap, &tempTexColor);

						TURApplyAlpha(&destRow, tempTexColor, a);
						
						++compressedRow;
					}
				}
				else
				{
					// no alpha
					for (i = 0; i < length; ++i)
					{
						tempTexColor = colors[*compressedRow];
						if (colorMap)
							MapColor(colorMap, &tempTexColor);

						rgbaSrcPixel = TEX_PIXELtoRGBA(tempTexColor);
						*destRow++ = rgbaSrcPixel.b;
						*destRow++ = rgbaSrcPixel.g;
						*destRow++ = rgbaSrcPixel.r;
						*destRow++ = 255;

						++compressedRow;
					}
				}
				length = 0;
			}
			break;

		case OPAQUE_FLAG | NOPALETTE_FLAG:
			// this flag is not yet implemented
			assert(false);

			/*
			if (length > size)
			{
				// This run of pixels isn't enough.
				if (maxAlpha != 255)
				{
					// do alpha
					a = maxAlpha;
					for (i = 0; i < size; ++i)
					{
						((BYTE*)&tempTexColor)[0] = *compressedRow++;
						((BYTE*)&tempTexColor)[1] = *compressedRow++;
						((BYTE*)&tempTexColor)[2] = *compressedRow++;
						((BYTE*)&tempTexColor)[3] = 254;
						if (colorMap)
							MapColor(colorMap, &tempTexColor);
						AlphaBlend(*PTEX_PIXEL(destRow), tempTexColor, a);
						destRow += BYTES_PER_PIXEL;
					}
				}
				else
				{
					for (i = 0; i < size; ++i)
					{
						((BYTE*)&tempTexColor)[0] = *compressedRow++;
						((BYTE*)&tempTexColor)[1] = *compressedRow++;
						((BYTE*)&tempTexColor)[2] = *compressedRow++;
						((BYTE*)&tempTexColor)[3] = 254;
						if (colorMap)
							MapColor(colorMap, &tempTexColor);
						*PTEX_PIXEL(destRow) = tempTexColor;
						destRow += BYTES_PER_PIXEL;
					}
				}
			}
			else
			{
				// This run of pixels is enough.
				if (maxAlpha != 255)
				{
					// do alpha
					a = maxAlpha;
					for (i = 0; i < length; ++i)
						{
						((BYTE*)&tempTexColor)[0] = *compressedRow++;
						((BYTE*)&tempTexColor)[1] = *compressedRow++;
						((BYTE*)&tempTexColor)[2] = *compressedRow++;
						((BYTE*)&tempTexColor)[3] = 254;
						if (colorMap)
							MapColor(colorMap, &tempTexColor);
						AlphaBlend(*PTEX_PIXEL(destRow), tempTexColor, a);
						destRow += BYTES_PER_PIXEL;
					}
				}
				else
				{
					for (i = 0; i < length; ++i)
					{
						((BYTE*)&tempTexColor)[0] = *compressedRow++; //b
						((BYTE*)&tempTexColor)[1] = *compressedRow++; //r
						((BYTE*)&tempTexColor)[2] = *compressedRow++; //g
						((BYTE*)&tempTexColor)[3] = 254;
						if (colorMap)
							MapColor(colorMap, &tempTexColor);
						*PTEX_PIXEL(destRow) = tempTexColor;
						destRow += BYTES_PER_PIXEL;
					}
				}
				length = 0;
			}
			*/
			break;

		case ALPHA_FLAG:
			if (length > size)
			{
				// This run of pixels isn't enough.
				for (i = 0; i < size; ++i)
				{
					f = *compressedRow;
					++compressedRow;
					a = *compressedRow;
					++compressedRow;
					if (maxAlpha != 255)
					{
						// do alpha
						a = (a * maxAlpha) >> 8; // / 255;
					}
					tempTexColor = colors[f];
					if (colorMap)
						MapColor(colorMap, &tempTexColor);

					TURApplyAlpha(&destRow, tempTexColor, a);
				}
			}
			else
			{
				// This run of pixels is enough.
				for (i = 0; i < length; ++i)
				{
					f = *compressedRow;
					++compressedRow;
					a = *compressedRow;
					++compressedRow;
					if (maxAlpha != 255)
					{
						// do alpha
						a = (a * maxAlpha) >> 8; // / 255;
					}
					tempTexColor = colors[f];
					if (colorMap)
						MapColor(colorMap, &tempTexColor);

					TURApplyAlpha(&destRow, tempTexColor, a);
				}
				length = 0;
			}
			break;

		case ALPHA_FLAG | NOPALETTE_FLAG:
			// this flag is not yet implemented
			assert(false);

			/*
			if (length > size)
			{
				// This run of pixels isn't enough.
				for (i = 0; i < size; ++i)
				{
					((BYTE*)&tempTexColor)[0] = *compressedRow++;
					((BYTE*)&tempTexColor)[1] = *compressedRow++;
					((BYTE*)&tempTexColor)[2] = *compressedRow++;
					((BYTE*)&tempTexColor)[3] = 254;
					a = *compressedRow;
					++compressedRow;
					if (maxAlpha != 255)
					{
						// do alpha
						a = (a * maxAlpha) >> 8; // / 255;
					}
					if (colorMap)
						MapColor(colorMap, &tempTexColor);
					AlphaBlend(*PTEX_PIXEL(destRow), tempTexColor, a);
					destRow += BYTES_PER_PIXEL;
				}
			}
			else
			{
				// This run of pixels is enough.
				for (i = 0; i < length; ++i)
				{
					((BYTE*)&tempTexColor)[0] = *compressedRow++;
					((BYTE*)&tempTexColor)[1] = *compressedRow++;
					((BYTE*)&tempTexColor)[2] = *compressedRow++;
					((BYTE*)&tempTexColor)[3] = 254;
					a = *compressedRow;
					++compressedRow;
					if (maxAlpha != 255)
					{
						// do alpha
						a = (a * maxAlpha) >> 8; // / 255;
					}
					if (colorMap)
						MapColor(colorMap, &tempTexColor);
					AlphaBlend(*PTEX_PIXEL(destRow), tempTexColor, a);
					destRow += BYTES_PER_PIXEL;
				}
				length = 0;
			}
			*/
			break;

		case RLE_FLAG:
			if (length > size)
			{
				// This run of pixels isn't enough.
				tempTexColor = colors[*compressedRow];
				if (colorMap)
					MapColor(colorMap, &tempTexColor);
				if (maxAlpha != 255)
				{
					// do alpha
					a = maxAlpha;
					for (i = 0; i < size; ++i)
					{
						TURApplyAlpha(&destRow, tempTexColor, a);
					}
				}
				else
				{
					// no alpha
					for (i = 0; i < size; ++i)
					{
						rgbaSrcPixel = TEX_PIXELtoRGBA(tempTexColor);
						*destRow++ = rgbaSrcPixel.b;
						*destRow++ = rgbaSrcPixel.g;
						*destRow++ = rgbaSrcPixel.r;
						*destRow++ = 255;
					}
				}
				++compressedRow;
			}
			else
			{
				// This run of pixels is enough.
				tempTexColor = colors[*compressedRow];
				if (colorMap)
					MapColor(colorMap, &tempTexColor);
				if (maxAlpha != 255)
				{
					// do alpha
					a = maxAlpha;
					for (i = 0; i < length; ++i)
					{
						TURApplyAlpha(&destRow, tempTexColor, a);
					}
				}
				else
				{
					// no alpha
					for (i = 0; i < length; ++i)
					{
						rgbaSrcPixel = TEX_PIXELtoRGBA(tempTexColor);
						*destRow++ = rgbaSrcPixel.b;
						*destRow++ = rgbaSrcPixel.g;
						*destRow++ = rgbaSrcPixel.r;
						*destRow++ = 255;
					}
				}
				length = 0;
			}
			break;

		case RLE_FLAG | NOPALETTE_FLAG:
			// this flag is not yet implemented
			assert(false);
			
			/*
			if (length > size)
			{
				// This run of pixels isn't enough.
				((BYTE*)&tempTexColor)[0] = *compressedRow++;
				((BYTE*)&tempTexColor)[1] = *compressedRow++;
				((BYTE*)&tempTexColor)[2] = *compressedRow++;
				((BYTE*)&tempTexColor)[3] = 254;
				if (colorMap)
					MapColor(colorMap, &tempTexColor);
				if (maxAlpha != 255)
				{
					// do alpha
					a = maxAlpha;
					for (i = 0; i < size; ++i)
					{
						AlphaBlend(*PTEX_PIXEL(destRow), tempTexColor, a);
						destRow += BYTES_PER_PIXEL;
					}
				}
				else
				{
					for (i = 0; i < size; ++i)
					{
						*PTEX_PIXEL(destRow) = tempTexColor;
						destRow += BYTES_PER_PIXEL;
					}
				}
			}
			else
			{
				// This run of pixels is enough.
				((BYTE*)&tempTexColor)[0] = *compressedRow++;
				((BYTE*)&tempTexColor)[1] = *compressedRow++;
				((BYTE*)&tempTexColor)[2] = *compressedRow++;
				((BYTE*)&tempTexColor)[3] = 254;
				if (colorMap)
					MapColor(colorMap, &tempTexColor);
				if (maxAlpha != 255)
				{
					// do alpha
					a = maxAlpha;
					for (i = 0; i < length; ++i)
					{
						AlphaBlend(*PTEX_PIXEL(destRow), tempTexColor, a);
						destRow += BYTES_PER_PIXEL;
					}
				}
				else
				{
					for (i = 0; i < length; ++i)
					{
						*PTEX_PIXEL(destRow) = tempTexColor;
						destRow += BYTES_PER_PIXEL;
					}
				}
				length = 0;
			}
			*/
			break;

		case ADD_FLAG:
			a = maxAlpha;
			
			// tell the TSprite object what flags were found in image
			// effectFlags = flags;
			
			if (length > size)
			{
				// This run of pixels isn't enough.
				for (i = 0; i < size; ++i)
				{
					AddBlend(*PTEX_PIXEL(destRow), colors[*compressedRow], a);
					destRow += 4; // RGBA
					++compressedRow;
				}
			}
			else
			{
				// This run of pixels is enough.
				for (i = 0; i < length; ++i)
				{
					AddBlend(*PTEX_PIXEL(destRow), colors[*compressedRow], a);
					destRow += 4; // RGBA
					++compressedRow;
				}
				length = 0;
			}
			break;

		case MULTIPLY_FLAG:
			a = maxAlpha;
			
			// tell the TSprite object what flags were found in image
			// effectFlags = flags;
			
			if (length > size)
			{
				// This run of pixels isn't enough.
				for (i = 0; i < size; ++i)
				{
					MultiplyBlend(*PTEX_PIXEL(destRow), colors[*compressedRow], a);
					destRow += 4; // RGBA
					++compressedRow;
				}
			}
			else
			{
				// This run of pixels is enough.
				for (i = 0; i < length; ++i)
				{
					MultiplyBlend(*PTEX_PIXEL(destRow), colors[*compressedRow], a);
					destRow += 4; // RGBA
					++compressedRow;
				}
				length = 0;
			}
			break;

		case OVERLAY_FLAG:
			a = maxAlpha;
			
			// tell the TSprite object what flags were found in image
			// effectFlags = flags;
			
			if (length > size)
			{
				// This run of pixels isn't enough.
				for (i = 0; i < size; ++i)
				{
					OverlayBlend(*PTEX_PIXEL(destRow), colors[*compressedRow], a);
					destRow += 4; // RGBA
					++compressedRow;
				}
			}
			else
			{
				// This run of pixels is enough.
				for (i = 0; i < length; ++i)
				{
					OverlayBlend(*PTEX_PIXEL(destRow), colors[*compressedRow], a);
					destRow += 4; // RGBA
					++compressedRow;
				}
				length = 0;
			}
			break;

		default:
			#if defined(OS_WINDOWS)
				MessageBox(NULL, "Texture decompression failed!", "Error", MB_OK | MB_TASKMODAL);
			#endif // OS_WINDOWS
			break;
		}

		if (length)
		{
			length -= size;
			flags = *compressedRow;
			++compressedRow;
			w = *(short*)compressedRow;
			MacSwapBytes(&w);
			size = w;
			compressedRow += sizeof(short);
		}
	} // while()
}

//======================================================================
// Draw from an unpcompressed RGBA buffer onto the screen (frame buffer).
// Note that no effects except for alpha are supported; any other effects
// must have already been performed in making the uncompressed RGBA.
//======================================================================
void TextureRenderScaledRow(BYTE *destRow, TEX_PIXEL *uncompressedRow, int start, int length, int viewWidth, int moreRows, const TEX_PIXEL *colors, SCALE_TYPE scaleType, COLORMAPPING *colorMap)
{
	int BYTES_PER_PIXEL = sizeof(TEX_PIXEL);
	RGBA rgbaSrcPixel;
	TEX_PIXEL tempColor;
	TEX_PIXEL_STRUCT *p;
	TEX_PIXEL_STRUCT q;
	TEX_PIXEL_STRUCT *p1, *p2, *p3, *p4;
	DWORD accum;
	
	assert(scaleType != ST_NONE);
	
	// render this row to the framebuffer
	
	uncompressedRow += start;
	
	while (length--) {
		if (scaleType == ST_DISCARD)
		{
			// discard scaling
			tempColor = *uncompressedRow;
			p = PTEX_PIXEL_STRUCT(uncompressedRow);
		}
		else
		{
			// quad-merge scaling
			// get the super-pixel (in this case four pixels)
			p1 = PTEX_PIXEL_STRUCT(uncompressedRow);
			p2 = PTEX_PIXEL_STRUCT(uncompressedRow + 1);
			
			if (moreRows)
			{
				p3 = PTEX_PIXEL_STRUCT(uncompressedRow + viewWidth);
				p4 = PTEX_PIXEL_STRUCT(uncompressedRow + viewWidth + 1);
			}
			else
			{
				p3 = p1;
				p4 = p2;
			}
			
			// interpolate down to one pixel
			p = &q;
			
			accum = (p1->r + p2->r + p3->r + p4->r);
			p->r = BYTE(accum >> 2);
			
			accum = (p1->g + p2->g + p3->g + p4->g);
			p->g = BYTE(accum >> 2);
			
			accum = (p1->b + p2->b + p3->b + p4->b);
			p->b = BYTE(accum >> 2);
			
			accum = (p1->a + p2->a + p3->a + p4->a);
			p->a = BYTE(accum >> 2);
			
			rgbaSrcPixel.r = p->r;
			rgbaSrcPixel.g = p->g;
			rgbaSrcPixel.b = p->b;
			tempColor = RGBAtoTEX_PIXEL(rgbaSrcPixel);
		}
		
		// skip to next pixel
		uncompressedRow += 2;
		
		// deal with alpha channel effects
		switch (p->a)
		{
		case 0:
			// transparent pixel, do nothing!
			break;
		
		case 255:
			// opaque pixel
			*PTEX_PIXEL(destRow) = tempColor;
			break;
		
		default:
			AlphaBlend(*PTEX_PIXEL(destRow), tempColor, p->a);
			break;
		}
		
		/*
		// deal with special effects flags
		switch(effectFlags)
		{
		case ADD_FLAG:
			AddBlend		(*PTEX_PIXEL(destRow), tempColor, p->a);
			break;

		case OVERLAY_FLAG:
			OverlayBlend	(*PTEX_PIXEL(destRow), tempColor, p->a);
			break;

		case MULTIPLY_FLAG:
			MultiplyBlend	(*PTEX_PIXEL(destRow), tempColor, p->a);
			break;

		default:
			// deal with alpha channel effects
			switch(p->a)
			{
			case 0:
				// transparent pixel, do nothing!
				break;

			case 255:
				// opaque pixel
				*PTEX_PIXEL(destRow) = tempColor;
				break;

			default:
				AlphaBlend(*PTEX_PIXEL(destRow), tempColor, p->a);
				break;
			}
			break;
		}
		*/
		
		destRow += BYTES_PER_PIXEL;
	}
}
