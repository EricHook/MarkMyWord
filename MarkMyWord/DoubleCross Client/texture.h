#if !defined (texture_h)
#define texture_h

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define TRANSPARENT_FLAG	0x01
#define OPAQUE_FLAG			0x02
#define ALPHA_FLAG			0x04
#define RLE_FLAG			0x08
#define ADD_FLAG			0x10
#define MULTIPLY_FLAG		0x20
#define OVERLAY_FLAG		0x40
#define NOPALETTE_FLAG		0x80

enum SCALE_TYPE {
	ST_NONE,
	ST_DISCARD,
	ST_BILINEAR
};

//#define COLOR_16BIT
//#define COLOR_24BIT
#define COLOR_32BIT

#if defined(COLOR_16BIT)
typedef WORD TEX_PIXEL, *PTEX_PIXEL;

// TEX_PIXEL is implemented as 0rrrrrgggggbbbbb with
// TEX_PIXEL is implemented as 0rrr rrgg gggb bbbb with
// the following shifts and masks.
#define TEX_R_SHIFT	7
#define TEX_G_SHIFT	2
#define TEX_B_SHIFT	-3
#define TEX_R_RES	5
#define TEX_G_RES	5
#define TEX_B_RES	5
#define TEX_R_MASK	0x7C00
#define TEX_G_MASK	0x03E0
#define TEX_B_MASK	0x001F
#endif

#if defined(COLOR_24BIT)
typedef struct
	{
	BYTE b1;
	BYTE b2;
	BYTE b3;
	} B3;

typedef B3 TEX_PIXEL, *PTEX_PIXEL;

// TEX_PIXEL is implemented ARGB with
// the following shifts and masks.
#define TEX_R_SHIFT	16
#define TEX_G_SHIFT	8
#define TEX_B_SHIFT	0
#define TEX_R_RES	8
#define TEX_G_RES	8
#define TEX_B_RES	8
#define TEX_R_MASK	0x0000FF00
#define TEX_G_MASK	0x00FF0000
#define TEX_B_MASK	0xFF000000
#endif

#if defined(COLOR_32BIT)
typedef DWORD TEX_PIXEL, *PTEX_PIXEL;

#if defined(OS_LITTLE_ENDIAN)
typedef struct
	{
	BYTE b;
	BYTE g;
	BYTE r;
	BYTE a;
	} TEX_PIXEL_STRUCT, *PTEX_PIXEL_STRUCT;
#else // defined(OS_BIG_ENDIAN)
typedef struct
	{
	BYTE a;
	BYTE r;
	BYTE g;
	BYTE b;
	} TEX_PIXEL_STRUCT, *PTEX_PIXEL_STRUCT;
#endif // OS_LITTLE_ENDIAN

// TEX_PIXEL is implemented ARGB with
// the following shifts and masks.
#define TEX_R_SHIFT	16
#define TEX_G_SHIFT	8
#define TEX_B_SHIFT	0
#define TEX_R_RES	8
#define TEX_G_RES	8
#define TEX_B_RES	8
#define TEX_R_MASK	0x00FF0000
#define TEX_G_MASK	0x0000FF00
#define TEX_B_MASK	0x000000FF
#endif

typedef struct
	{
	short	numPixels;
	} TRANSPARENT_BLOCK;

typedef struct
	{
	short	numPixels;
	BYTE	*pixels;
	} OPAQUE_BLOCK;

typedef struct
	{
	BYTE	alpha;
	BYTE	pixel;
	} ALPHA_PIXEL;

typedef struct
	{
	short	numPixels;
	ALPHA_PIXEL	*pixels;
	} ALPHA_BLOCK;

typedef struct
	{
	short	numPixels;
	BYTE	pixel;
	} RLE_BLOCK;

typedef struct
	{
	BYTE	flags;
	union
		{
		TRANSPARENT_BLOCK	tb;
		OPAQUE_BLOCK		ob;
		ALPHA_BLOCK			ab;
		RLE_BLOCK			rb;
		} SOME_BLOCK;
	} BLOCK;

typedef struct
	{
	short length;
	BLOCK *blocks;	// variable length blocks
	} ROW;

typedef struct
	{
	short	width;
	short	height;
	short	xOrigin;
	short	yOrigin;
	short	x;
	short	y;
//	ROW		*rows;
	BYTE	**rows;
	} CEL;

typedef struct
	{
	short	numCels;
	CEL		*cels;
	} LOOP;

typedef struct
	{
	BYTE	red;
	BYTE	green;
	BYTE	blue;
	} RGB;

typedef struct
	{
	BYTE r;
	BYTE g;
	BYTE b;
	BYTE a;
	} RGBA;

typedef struct
	{
	int		sig;		// 'tex ', i.e. low byte = 't'
	int		version;	// High Byte = major version, etc.
	RGB		colors[256];
	short	numLoops;
	LOOP	*loops;
	} TEXTURE;

typedef struct {
	RGBA fromColor;
	RGBA toColor;
	} COLORMAP;

typedef struct {
	int numMaps;
	COLORMAP *mappings;
	} COLORMAPPING;

///// MDS - TODO update!
void TextureUncompressRow(BYTE *destRow, BYTE *compressedRow, int length, const TEX_PIXEL *colors, COLORMAPPING *colorMap);
void TextureRenderScaledRow(BYTE *destRow, TEX_PIXEL *uncompressedRow, int start, int length, int viewWidth, int moreRows, const TEX_PIXEL *colors, SCALE_TYPE scaleType, COLORMAPPING *colorMap);

void TextureRenderRow(BYTE *destRow, BYTE *compressedRow, int start, int length, const TEX_PIXEL *colors, COLORMAPPING *colorMap);
BYTE *TextureCompressRow(BYTE *sourceRow, int length, int *lengthOfRow, BYTE *alphaRow=NULL, int transferMode=ALPHA_FLAG);
void Convert2BMPsToTEX(char *inColorFilename, char *inAlphaFilename, char *outFilename, int transferMode=ALPHA_FLAG);
void BitmapCopyRLEPixels(WORD wWidthBytes, BYTE *pb, BYTE *prle);
void BitmapCopyPixels(WORD wWidthBytes, BYTE *pb, BYTE *prle, int wWidthInBytes, int wHeight);
BYTE PaletteFindNearest(RGBQUAD colors[256], BYTE red, BYTE green, BYTE blue);
BYTE TextureColorOfPoint(int loop, int cel, LOOP *loops, int x, int y);
BOOL TextureIsPointOpaque(int loop, int cel, LOOP *loops, int x, int y);
RGB TEX_PIXELtoRGB(TEX_PIXEL texPixel);
RGBQUAD TEX_PIXELtoRGBQUAD(TEX_PIXEL texPixel);
RGBA TEX_PIXELtoRGBA(TEX_PIXEL texPixel);
TEX_PIXEL RGBtoTEX_PIXEL(RGB rgb);
TEX_PIXEL RGBQUADtoTEX_PIXEL(RGBQUAD rgb);
TEX_PIXEL RGBAtoTEX_PIXEL(RGBA rgba);
void InitInverseColorTable(RGB colors[256]);
BYTE InverseColorLookup(BYTE r, BYTE g, BYTE b);
void AlphaBlend(TEX_PIXEL &back, TEX_PIXEL fore, int alpha);
void AddBlend(TEX_PIXEL &back, TEX_PIXEL fore, int alpha);
void MultiplyBlend(TEX_PIXEL &back, TEX_PIXEL fore, int alpha);
void TextureGetPixel(int loop, int cel, LOOP *loops, int x, int y,
					 BYTE &color, BYTE &alpha, BYTE &flags);

// TextureUnFlatten: Convert a byte array into a tex in memory.
// This is the last step after reading a tex from disk.
void TextureUnFlatten(BYTE *stuff,									// input params
		short *numLoops, LOOP **loops, RGB **colors);				// output params

// TEXFlatten: Convert a tex in memory into a byte array.
// This is the first step before writing a text to disk.
void TextureFlatten(BYTE **fileData, int *lengthOfFileData,			// output params
		BYTE **rows, short numLoops, LOOP *loops, RGB *colors);		// input params

#endif // texture_h
