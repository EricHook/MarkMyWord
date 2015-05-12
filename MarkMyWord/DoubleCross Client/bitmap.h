#ifndef _bitmap
#define _bitmap

typedef struct
	{
	WORD	bfType;
	DWORD	bfSize;
	WORD	bfReserved1;
	WORD	bfReserved2;
	DWORD	bfOffBits;

    DWORD	biSize;
    DWORD	biWidth;
    DWORD	biHeight;
    WORD	biPlanes;
    WORD	biBitCount;
    DWORD	biCompression;
    DWORD	biSizeImage;
    DWORD	biXPelsPerMeter;
    DWORD	biYPelsPerMeter;
    DWORD	biClrUsed;
    DWORD	biClrImportant;
	RGBQUAD	bmiColors[256];
	} BTMP_RES;

#include "texture.h"

class FrameBuffer;

class Bitmap
	{
	public:
		Bitmap(int newWidth, int newHeight);
		virtual ~Bitmap();
		void CopyPixels(FrameBuffer *frameBuffer, RECT rcDest, RECT rcSource);
		void CopyPixelsWithScaling(Bitmap *destBitmap, double xScale, double yScale);
		void Erase(RECT rcErase, RGBA color);
		int Width() const;
		int Height() const;
		BYTE ColorOfPoint(POINT point);
		virtual BYTE ColorOfPoint(int x, int y);
		int GetWidthInBytes() const;
		BYTE *GetPixels() const;
		int GetBytesPerPixel() const;
		virtual int GetX();
		virtual int GetY();
		void ReadFromFile(char *path);
		void Resize(int newWidth, int newHeight);

		// Data
		TEX_PIXEL colors[256];

	protected:
    	// Functions.
		void CopyRow(BYTE far *hpDest, BYTE far *hpSource, WORD wWidth);
		virtual void CopyRectangle(FrameBuffer *frameBuffer,
				WORD wDestX, WORD wDestY, WORD wSourceX, WORD wSourceY,
				WORD wBytesToCopy, WORD wRowsToCopy, WORD wDestWidthInBytes, WORD wSourceWidthInBytes);

		// Data.
		BYTE *pPixels;
		int width;
		int height;
		int widthInBytes;
		int bytesPerPixel;
	};
#endif
