#include "os.h"
#include "core.h"
#include "globals.h"

void ClipSomeRects(RECT &rcDest, RECT &rcDestClippingRect,
				RECT &rcSource, RECT &rcSourceClippingRect);

BTMP_RES btmpResGlobalHeader;

Bitmap::Bitmap(int newWidth, int newHeight)
	{
	int size;

	pPixels = NULL;

	if ((newWidth == 0) || (newHeight == 0))
		{
		width = 0;
		height = 0;
		widthInBytes = 0;
		bytesPerPixel = 0;
		}
	else
		{
		width = newWidth;
		height = newHeight;
		bytesPerPixel = sizeof(TEX_PIXEL);
		widthInBytes = (width*bytesPerPixel) + 3 - (((width*bytesPerPixel)-1) % 4);

		// Allocation memory for the pixels.
		size = (DWORD)(widthInBytes) * (DWORD)(height);
		pPixels = new BYTE[size];
		memset(pPixels, 0, size);
		}
	}

Bitmap::~Bitmap()
	{
	delete[] pPixels;
	}

int Bitmap::Width() const
	{
	return width;
	}

int Bitmap::Height() const
	{
	return height;
	}

void ClipSomeRects(RECT &rcDest, RECT &rcDestClippingRect,
				RECT &rcSource, RECT &rcSourceClippingRect)
	{
    RECT rcNewSource, rcNewSourceDelta;
    RECT rcNewDest, rcNewDestDelta;

	// Widths and Heights must match - Stretching not supported.
	if ((rcDest.right - rcDest.left != rcSource.right - rcSource.left) ||
			(rcDest.right - rcDest.left != rcSource.right - rcSource.left) ||
			(rcDest.right - rcDest.left != rcSource.right - rcSource.left) ||
			(rcDest.right - rcDest.left != rcSource.right - rcSource.left)) {
		SetRect(&rcDest, 0, 0, 0, 0);
		SetRect(&rcSource, 0, 0, 0, 0);
		return;
		}

	// Clip the source rect to its valid bounds.
	IntersectRect(&rcNewSource, &rcSourceClippingRect, &rcSource);

	// Get the amounts the source rect changed.
	rcNewSourceDelta.left   = rcNewSource.left - rcSource.left;
	rcNewSourceDelta.top    = rcNewSource.top - rcSource.top;
	// ??? Are right and bottom negative or positive?
	rcNewSourceDelta.right  = rcNewSource.right - rcSource.right;
	rcNewSourceDelta.bottom = rcNewSource.bottom - rcSource.bottom;

	// Change the dest rect by the same amounts.
	rcDest.left   += rcNewSourceDelta.left;
	rcDest.top    += rcNewSourceDelta.top;
	rcDest.right  += rcNewSourceDelta.right;
	rcDest.bottom += rcNewSourceDelta.bottom;
	rcSource.left   += rcNewSourceDelta.left;
	rcSource.top    += rcNewSourceDelta.top;
	rcSource.right  += rcNewSourceDelta.right;
	rcSource.bottom += rcNewSourceDelta.bottom;

	// Make sure the dest rect didn't collapse.
	if ((rcDest.right - rcDest.left <= 0) ||
			(rcDest.bottom - rcDest.top <= 0)) {
		SetRect(&rcDest, 0, 0, 0, 0);
		SetRect(&rcSource, 0, 0, 0, 0);
		return;
		}

	// Clip the dest rect to its valid bounds.
	IntersectRect(&rcNewDest, &rcDestClippingRect, &rcDest);

	// Get the amounts the dest rect changed.
	rcNewDestDelta.left   = rcNewDest.left - rcDest.left;
	rcNewDestDelta.top    = rcNewDest.top - rcDest.top;
	// ??? Are right and bottom negative or positive?
	rcNewDestDelta.right  = rcNewDest.right - rcDest.right;
	rcNewDestDelta.bottom = rcNewDest.bottom - rcDest.bottom;

	// Change the source rect by the same amounts.
	rcSource.left   += rcNewDestDelta.left;
	rcSource.top    += rcNewDestDelta.top;
	rcSource.right  += rcNewDestDelta.right;
	rcSource.bottom += rcNewDestDelta.bottom;
	rcDest.left   += rcNewDestDelta.left;
	rcDest.top    += rcNewDestDelta.top;
	rcDest.right  += rcNewDestDelta.right;
	rcDest.bottom += rcNewDestDelta.bottom;

	// Make sure the source rect didn't collapse.
	if ((rcSource.right - rcSource.left <= 0) ||
			(rcSource.bottom - rcSource.top <= 0)) {
		SetRect(&rcDest, 0, 0, 0, 0);
		SetRect(&rcSource, 0, 0, 0, 0);
		return;
		}
	}

void Bitmap::CopyRow(BYTE far *hpDest, BYTE far *hpSource, WORD wWidth) {
	do {
		*PTEX_PIXEL(hpDest) = *PTEX_PIXEL(hpSource);
		hpDest += sizeof(TEX_PIXEL);
		hpSource += sizeof(TEX_PIXEL);
		--wWidth;
		}
	while (wWidth);
	}

void Bitmap::CopyPixels(FrameBuffer *frameBuffer, RECT rcDest, RECT rcSource) {
	RECT rcClipSource, rcClipDest;

	if (pPixels == 0)
		return;

	SetRect(&rcClipSource, 0, 0, Width(), Height());
	SetRect(&rcClipDest, 0, 0, frameBuffer->GetWidth(), frameBuffer->GetHeight());
	ClipSomeRects(rcDest, rcClipDest, rcSource, rcClipSource);

	if (!IsRectEmpty(&rcSource))
		{
		CopyRectangle(frameBuffer,
				(WORD)rcDest.left, (WORD)rcDest.top, (WORD)rcSource.left, (WORD)rcSource.top,
				(WORD)(rcSource.right - rcSource.left), (WORD)(rcSource.bottom - rcSource.top),
				frameBuffer->GetStride(), widthInBytes);
		}
	}

void Bitmap::CopyRectangle(FrameBuffer *frameBuffer,
		WORD wDestX, WORD wDestY, WORD wSourceX, WORD wSourceY,
		WORD wBytesToCopy, WORD wRowsToCopy, WORD wDestWidthInBytes, WORD wSourceWidthInBytes)
	{
	BYTE *pSourcePixels;
	BYTE *pDestPixels;

	pSourcePixels = pPixels;
	pSourcePixels += (DWORD)wSourceY * wSourceWidthInBytes + (wSourceX * bytesPerPixel);
	while (wRowsToCopy--) {
		pDestPixels = frameBuffer->GetRow(wDestY) + (wDestX * frameBuffer->GetBytesPerPixel());
		CopyRow(pDestPixels, pSourcePixels, wBytesToCopy);
		pSourcePixels += wSourceWidthInBytes;
		++wDestY;
		}
	}

void Bitmap::Erase(RECT rcErase, RGBA color)
	{
    RECT rcTemp;
	BYTE *pixels;
	int x, y;
	int left, right;

	if (pPixels == 0)
		return;

	// Clip the rect to its valid bounds.
	SetRect(&rcTemp, 0, 0, Width(), Height());
	IntersectRect(&rcErase, &rcTemp, &rcErase);

	// Don't erase if empty rect.
	if (IsRectEmpty(&rcErase))
		return;

	pixels = pPixels;
	if (pixels)
		{
		for (y = rcErase.top; y < rcErase.bottom; ++y)
			{
			left = y * widthInBytes + (rcErase.left*bytesPerPixel);
			right = y * widthInBytes + (rcErase.left*bytesPerPixel) + ((rcErase.right - rcErase.left)*bytesPerPixel);
			for (x = left; x < right; x += bytesPerPixel)
				{
				*PTEX_PIXEL(&(pixels[x])) = RGBAtoTEX_PIXEL(color);
				}
			}
		}
	}

BYTE Bitmap::ColorOfPoint(int x, int y)
	{
	BYTE btOutput;
	BYTE *pByte;

	pByte = pPixels;

	btOutput = pByte[x + (DWORD)y * widthInBytes];

	return btOutput;
	}

BYTE Bitmap::ColorOfPoint(POINT point)
	{
	return ColorOfPoint(point.x, point.y);
	}

int Bitmap::GetWidthInBytes() const
	{
	return widthInBytes;
	}

BYTE *Bitmap::GetPixels() const
	{
	return pPixels;
	}

void Bitmap::CopyPixelsWithScaling(Bitmap *destBitmap, double xScale, double yScale)
	{
	double x, y, Y1, Y2;
	int a1, a2;

	if (pPixels == 0 || destBitmap->pPixels == 0)
		return;
	for (y = 0; y < (destBitmap->Height()*yScale); ++y)
		{
		Y1 = y;
		Y2 = y/yScale;
		for (x = 0; x < (destBitmap->Width()*xScale); ++x)
			{
			a1 = (int)(Y1 * destBitmap->GetWidthInBytes() + x*bytesPerPixel);
			a2 = (int)(Y2 * GetWidthInBytes() + (x*bytesPerPixel/xScale));
			*PTEX_PIXEL(destBitmap->pPixels + a1) =
				*PTEX_PIXEL(pPixels + a2);
			}
		}
	}

int Bitmap::GetBytesPerPixel() const
	{
	return bytesPerPixel;
	}

int Bitmap::GetX()
	{
	return 0;
	}

int Bitmap::GetY()
	{
	return 0;
	}

DWORD fsize(FILE *file);
void VBitmapCopyRLEPixels(WORD widthBytes, BYTE *pb, BYTE *prle);
void VBitmapCopyPixels(WORD w, WORD h, WORD wb, TEX_PIXEL *pb, RGB *prle);

void Bitmap::ReadFromFile(char *path)
	{
	FILE *file;
	int size;
	BYTE *fileData;
	RGBQUAD palettecolors[256];
	DWORD colorsUsed;
	int i;

	file = fopen(path, "rb");
	if (file)
		{
		size=fsize(file);
		fileData = new BYTE[size];
		fread(fileData, 1, size, file);

		width = *(WORD*)(fileData + 18);
		height = *(WORD*)(fileData + 22);
		bytesPerPixel = sizeof(TEX_PIXEL);
		widthInBytes = (width*bytesPerPixel) + 3 - (((width*bytesPerPixel)-1) % 4);

		// Allocation memory for the pixels.
		size = (DWORD)(widthInBytes) * (DWORD)(height);
		delete[] pPixels;
		pPixels = new BYTE[size];
		memset(pPixels, 0, size);

		// Get the palette from the resource.
		colorsUsed = *(DWORD*)(fileData + 46);
		if (colorsUsed)
			{
			for (i = 0; i < (int)colorsUsed; ++i)
				{
				palettecolors[i] = *(RGBQUAD*)(fileData + 54 + i*4);
				}
			for (i = (int)colorsUsed; i < 256; ++i)
				{
				palettecolors[i] = *(RGBQUAD*)(fileData + 54);
				}
			}
		else
			{
			for (i = 0; i < 256; ++i)
				{
				palettecolors[i] = *(RGBQUAD*)(fileData + 54 + i*4);
				}
			}
		for (i = 0; i < 256; ++i)
			{
			colors[i] = RGBQUADtoTEX_PIXEL(palettecolors[i]);
			}

		// Check for RLE compression
		switch (*(WORD*)(fileData + 30))
			{
			case BI_RLE8:
				// Compressed
				VBitmapCopyRLEPixels(widthInBytes, pPixels + (DWORD)(widthInBytes) * (DWORD)(height-1), fileData + *(DWORD*)(fileData + 10));
				break;
			case BI_RGB:
				// Uncompressed
				VBitmapCopyPixels(width, height, widthInBytes, (TEX_PIXEL*)(pPixels + (DWORD)(widthInBytes) * (DWORD)(height-1)), (RGB*)(fileData + *(DWORD*)(fileData + 10)));
				break;
			default:
				// Unknown format, don't use it.
				break;
			}
		fclose(file);
		delete[] fileData;
		}
	}

void Bitmap::Resize(int newWidth, int newHeight)
	{
	int x, y;
	BYTE *newPixels;
	int newWidthInBytes;
	double dx, dy;

	newWidthInBytes = (newWidth*bytesPerPixel) + 3 - (((newWidth*bytesPerPixel)-1) % 4);
	newPixels = new BYTE[newWidthInBytes * newHeight];
	dx = double(width) / newWidth;
	dy = double(height) / newHeight;
	for (y=0; y<newHeight; ++y)
		{
		for (x=0; x<newWidth; ++x)
			{
			*(TEX_PIXEL*)(newPixels + y*newWidthInBytes + x*bytesPerPixel) =
				*(TEX_PIXEL*)(pPixels + int(int(dy*y)*widthInBytes + int(dx*x)*bytesPerPixel));
			}
		}
	width = newWidth;
	height = newHeight;
	widthInBytes = newWidthInBytes;
	delete[] pPixels;
	pPixels = newPixels;
	}

#define RLE_ESCAPE  0
#define RLE_EOL     0
#define RLE_EOF     1
#define RLE_JMP     2
#define RLE_RUN     3

void VBitmapCopyRLEPixels(WORD wWidthBytes, BYTE *pb, BYTE *prle) {
	BYTE    cnt;
	BYTE    b;
	WORD    x;
	WORD    dx,dy;
	BYTE	rb;
								
	x = 0;
								
	for(;;) {
		cnt = *prle++;
		b   = *prle++;
									
		if (cnt == RLE_ESCAPE) {
			switch (b) {
				case RLE_EOF:
					return;
				case RLE_EOL:
					//pb += wWidthBytes - x;	// bottom-up
					pb += -wWidthBytes - x;		// top-down
					x = 0;
					break;
				case RLE_JMP:
					dx = (WORD)*prle++;
					dy = (WORD)*prle++;
					//pb += ((DWORD)wWidthBytes * dy) + dx;	// bottom-up
					pb += -((int)wWidthBytes * dy) + dx;	// top-down
					x  += dx;
					break;
				default:
					cnt = b;
					x  += (WORD)cnt;
					while (cnt-- > 0)
					    *pb++ = *prle++;
					if (b & 1)
					    prle++;
					break;
				}
			}
		else {
			x += (WORD)cnt;
			rb = b;
			while (cnt-- > 0)
				*pb++ = rb;
			}
		}
	}

void VBitmapCopyPixels(WORD w, WORD h, WORD wb, TEX_PIXEL *pb, RGB *prle) {
	int i, j;
	RGB rgb;
	BYTE temp;

	j = wb * h;
	for (i = 0; i < h; ++i)
		{
		for (j = 0; j < w; ++j)
			{
			rgb = *prle++;
			temp = rgb.red;
			rgb.red = rgb.blue;
			rgb.blue = temp;
			*pb++ = RGBtoTEX_PIXEL(rgb);
			}
		pb -= wb;
		}
	}
