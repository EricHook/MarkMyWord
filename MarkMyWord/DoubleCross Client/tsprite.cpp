#include "os.h"
#include "core.h"
#include "unihand.h"
#include "sciplane.h"

extern Application *pApp;

void ClipSomeRects(RECT &rcDest, RECT &rcDestClippingRect,
				RECT &rcSource, RECT &rcSourceClippingRect);

TSprite::TSprite(RESOURCE_TYPES newType, int newResourceID,
		ResourceManager *newResourceManager)
	: Resource(newType, newResourceID, newResourceManager)
	{
	int i;
	RGB *viewColors;
	RGBQUAD aColor;
	BYTE *p;

	loops = NULL;
	numLoops	= 0;

	if (handle)
		p = (BYTE *)handle->Lock();
	else
		p = NULL;

	if (p)
		{
		stuff = p;
		TextureUnFlatten(stuff, &numLoops, &loops, &viewColors);

		for (i = 0; i < 256; ++i)
			{
			aColor.rgbRed = viewColors[i].red;
			aColor.rgbGreen = viewColors[i].green;
			aColor.rgbBlue = viewColors[i].blue;
			uncorrectedColors[i] = RGBQUADtoTEX_PIXEL(aColor);
			MAIN_WINDOW->pCurrentPalette->DoGammaCorrection(&aColor);
			colors[i] = RGBQUADtoTEX_PIXEL(aColor);
			}
		}
	}

TSprite::~TSprite()
	{
	int i, j;
	int numCels;

	if (loops)
		{
		for (i = 0; i < numLoops; ++i)
			{
			numCels = loops[i].numCels;
			for (j = 0; j < numCels; ++j)
				{
				delete[] loops[i].cels[j].rows;
				}
			delete[] loops[i].cels;
			}
		delete[] loops;
		}
	}

LOOP *TSprite::GetLoops() const
	{
	return loops;
	}

int TSprite::GetNumLoops() const
	{
	return numLoops;
	}

int TSprite::GetNumCels(int loop) const
	{
	if (loops)
		return loops[loop].numCels;
	else
		return 0;
	}

int TSprite::GetWidth(int loop, int cel) const
	{
	if (loops)
		return loops[loop].cels[cel].width;
	else
		return 0;
	}

int TSprite::GetHeight(int loop, int cel) const
	{
	if (loops)
		return loops[loop].cels[cel].height;
	else
		return 0;
	}

int TSprite::GetX(int loop, int cel) const
	{
	if (loops)
		return loops[loop].cels[cel].x;
	else
		return 0;
	}

int TSprite::GetY(int loop, int cel) const
	{
	if (loops)
		return loops[loop].cels[cel].y;
	else
		return 0;
	}

TEX_PIXEL *TSprite::GetColors() const
	{
		return (TEX_PIXEL *)&colors[0];
	}

//======================================================================
//
//======================================================================
void TSprite::CopyPixels(int loop, int cel, FrameBuffer *frameBuffer,
		RECT destRect, RECT sourceRect, COLORMAPPING *colorMap /*= NULL*/) const
{
	BYTE **curRow;
	BYTE *pDestPixels;
	BYTE *pSourcePixels;
	BYTE **rows;
	int bytesPerPixel, rowsToCopy, pixelsToCopy; // pixelsToCopy used to be bytesToCopy
	RECT rcClipSource, rcClipDest;
	
	if (!loops)
		return;
	
	SetRect(&rcClipSource, 0, 0, GetWidth(loop, cel), GetHeight(loop, cel));
	SetRect(&rcClipDest, 0, 0, frameBuffer->GetWidth(), frameBuffer->GetHeight());
	
	ClipSomeRects(destRect, rcClipDest, sourceRect, rcClipSource);
	
	if (!IsRectEmpty(&sourceRect))
		{
		assert(destRect.top < frameBuffer->GetHeight());
		assert(destRect.left < frameBuffer->GetWidth());
		assert(destRect.bottom <= frameBuffer->GetHeight());
		assert(destRect.right <= frameBuffer->GetWidth());
		assert(sourceRect.top < GetHeight(loop, cel));
		assert(sourceRect.left < GetWidth(loop, cel));
		assert(sourceRect.bottom <= GetHeight(loop, cel));
		assert(sourceRect.right <= GetWidth(loop, cel));
		
		bytesPerPixel = frameBuffer->GetBytesPerPixel();
		
		pDestPixels = frameBuffer->GetRow(destRect.top) + (destRect.left * bytesPerPixel);
		rows = loops[loop].cels[cel].rows;
		curRow = &(rows[sourceRect.top]);
		rowsToCopy = destRect.bottom - destRect.top;
		pixelsToCopy = destRect.right - destRect.left;

		while (rowsToCopy--)
			{
			pSourcePixels = *curRow;
			TextureRenderRow(pDestPixels, pSourcePixels,
							 sourceRect.left, pixelsToCopy, colors, colorMap);
			++curRow;
			if (rowsToCopy)
				pDestPixels += frameBuffer->GetStride();
			}
		}
}

/*
//======================================================================
//
//======================================================================
#pragma pack(push, 1)

typedef struct
{
	unsigned char	idlen,
					colorMapType,
					imageType;

	short	firstColorEntry,
			colorMapLength;

	unsigned char	colorMapBits;
	short	xorigin,
			yorigin,
			width,
			height;
	unsigned char pixelBits;
	unsigned char descriptor;
} TargaHeader;

typedef struct
{
  unsigned char b,g,r;
} TargaPixel;

#pragma pack(pop)

//
int WriteTarga(char *filename, TEX_PIXEL *buffer, int w, int h)
{
	int i,j;
	TargaHeader hdr;
	TargaPixel raw;
	FILE *f;
	RGBA rgbaPixel;

	f = fopen(filename,"wb");

	memset(&hdr, 0, sizeof(hdr));

	hdr.imageType = 2;
	hdr.colorMapBits = 24;
	hdr.width = w;
	hdr.height = h;
	hdr.pixelBits = 24;
	hdr.descriptor = 32;

	fwrite(&hdr, sizeof(hdr), 1, f);

	for (j = 0; j < h; j++)
	{
		for (i = 0; i < w; i++)
		{
			rgbaPixel = TEX_PIXELtoRGBA(*buffer);

			switch(rgbaPixel.a)
			{
			case 0:
				raw.r = 0;
				raw.g = 0;
				raw.b = 0;
				break;

			case 255:
				raw.r = rgbaPixel.r;
				raw.g = rgbaPixel.g;
				raw.b = rgbaPixel.b;
				break;

			default:
				raw.r = rgbaPixel.a * rgbaPixel.r >> 8;
				raw.g = rgbaPixel.a * rgbaPixel.g >> 8;
				raw.b = rgbaPixel.a * rgbaPixel.b >> 8;
			}

			buffer++;

			// write a pixel
			fwrite(&raw, sizeof(raw), 1, f);
		}
	}

	fclose(f);
	return 0;
}

//
int WriteBin(char *filename, TEX_PIXEL *buffer, int w, int h)
{
	int i,j;
	FILE *f;
	RGBA rgbaPixel;

	f = fopen(filename,"wt");

	for (j = 0; j < h; j++)
	{
		for (i = 0; i < w; i++)
		{
			rgbaPixel = TEX_PIXELtoRGBA(*buffer);
			buffer++;

			// write a pixel
			fprintf(f, "%02x%02x%02x%02x ", rgbaPixel.r, rgbaPixel.g, rgbaPixel.b, rgbaPixel.a);
		}
		fprintf(f, "\n");
	}

	fclose(f);
	return 0;
}
*/

//======================================================================
//
//======================================================================
void TSprite::CopyScaledPixels(int loop, int cel, FrameBuffer *frameBuffer,
		RECT destRect, RECT sourceRect, SCALE_TYPE scaleType, COLORMAPPING *colorMap /*= NULL*/) const
{
	BYTE *pSourcePixels, *pDestPixels, **curRow;
	int viewWidth, viewHeight, bytesPerPixel, rowsToCopy, pixelsToCopy; // pixelsToCopy used to be bytesToCopy
	DWORD *pixelData;
	TEX_PIXEL *rows;
	RECT viewRect, frameRect, drawableRect;
	
	if (!loops)
		return;
	
	if (IsRectEmpty(&sourceRect))
		return;
	
	viewWidth = GetWidth(loop, cel);
	viewHeight = GetHeight(loop, cel);
	
	SetRect(&viewRect, 0, 0, viewWidth, viewHeight);
	
	// special case: since we can't call ClipSomeRects() here as we do in CopyPixels(),
	// we'll just apply the special rule that if the destRect lies outside of the
	// frameBuffer at all, we just won't draw the scaled view; this means that you can
	// never have a scaled view only partially on the screen, but that's OK for the
	// mini character views for HOGS, which is currently the only system that uses
	// these shunken views
	
	SetRect(&frameRect, 0, 0, frameBuffer->GetWidth(), frameBuffer->GetHeight());
	IntersectRect(&drawableRect, &destRect, &frameRect);
	if (!EqualRect(&drawableRect, &destRect))
		return;
	
	pixelData = new TEX_PIXEL[viewWidth * viewHeight];
	if (!pixelData)
		{ assert(FALSE); return; }
	
	memset(pixelData, 0, viewWidth * viewHeight * sizeof(TEX_PIXEL));
	
	// expand the compressed TEX file into our RGBA buffer
	
	curRow = &(loops[loop].cels[cel].rows[viewRect.top]);
	for (rowsToCopy = 0 ; rowsToCopy < viewHeight ; rowsToCopy++) {
		pSourcePixels = *curRow;
		TextureUncompressRow((BYTE *)(pixelData + (rowsToCopy * viewWidth)), pSourcePixels, viewWidth, colors, colorMap);
		curRow++;
	}
	
	// copy the expanded buffer into the destination rect, with scaling
	
	assert(destRect.top			< frameBuffer->GetHeight());
	assert(destRect.left		< frameBuffer->GetWidth());
	assert(destRect.bottom		<= frameBuffer->GetHeight());
	assert(destRect.right		<= frameBuffer->GetWidth());
	assert(sourceRect.top		< GetHeight(loop, cel));
	assert(sourceRect.left		< viewWidth);
	assert(sourceRect.bottom	<= GetHeight(loop, cel));
	assert(sourceRect.right		<= viewWidth);
	
	bytesPerPixel	= frameBuffer->GetBytesPerPixel();
	pDestPixels		= frameBuffer->GetRow(destRect.top) + (destRect.left * bytesPerPixel);
	
	// get a ptr to the correct row of the original TEX image
	rows = pixelData + (sourceRect.top * viewWidth);
	
	rowsToCopy		= destRect.bottom - destRect.top;
	pixelsToCopy	= destRect.right  - destRect.left;
	
	while (rowsToCopy--) {
		TextureRenderScaledRow(pDestPixels, rows, sourceRect.left, pixelsToCopy, viewWidth, rowsToCopy, colors, scaleType, colorMap);
		
		rows += (2 * viewWidth); // skip every other row of the original TEX image
		pDestPixels += frameBuffer->GetStride();
	}
	
	delete [] pixelData;
	pixelData = NULL;
}

BYTE TSprite::ColorOfPoint(int loop, int cel, int x, int y) const
	{
	if (loops)
		return TextureIsPointOpaque(loop, cel, loops, x, y);
	else
		return 0;
	}

TEX_PIXEL TSprite::GetColor(int i)
	{
	return colors[i];
	}

void TSprite::GetPixel(int loop, int cel, int x, int y, RGBA &color, BYTE &flags)
	{
	BYTE c, a, f;

	TextureGetPixel(loop, cel, loops, x, y,
					 c, a, f);
	color = TEX_PIXELtoRGBA(colors[c]);
	color.a = a;
	flags = f;
	switch (flags)
		{
		case TRANSPARENT_FLAG:
			color.a = 0;
			break;
		case OPAQUE_FLAG:
		case ADD_FLAG:
		case MULTIPLY_FLAG:
			color.a = 255;
			break;
		case ALPHA_FLAG:
			break;
		case RLE_FLAG:
			color.a = 255;
			break;
		}
	}

void TSprite::SetGamma(double g)
	{
	int i;
	RGBQUAD aColor;
	double oldGamma;

	oldGamma = MAIN_WINDOW->pCurrentPalette->GetGamma();
	MAIN_WINDOW->pCurrentPalette->SetGamma(g);
	if (loops)
		{
		for (i = 0; i < 256; ++i)
			{
			aColor = TEX_PIXELtoRGBQUAD(uncorrectedColors[i]);
			MAIN_WINDOW->pCurrentPalette->DoGammaCorrection(&aColor);
			colors[i] = RGBQUADtoTEX_PIXEL(aColor);
			}
		}
	MAIN_WINDOW->pCurrentPalette->SetGamma(oldGamma);
	}
