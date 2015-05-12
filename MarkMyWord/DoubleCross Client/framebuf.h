#if !defined(FRAMEBUF_H)
#define FRAMEBUF_H

#include "texture.h"
class Bitmap;

class FrameBuffer
	{
	public:
		FrameBuffer(int width, int height, int bytesPerPixel, BOOL topDown = FALSE);
		~FrameBuffer();

		int GetStride();	// Number of bytes to next row.
		BYTE *GetRow(int row);
		int GetBytesPerPixel();
		int GetWidth();
		int GetHeight();
		void DrawPixel(int x, int y, RGBA color, int transferMode);
		void DrawHLine(int x1, int x2, int y, RGBA color, int transferMode);
		void DrawVLine(int x, int y1, int y2, RGBA color, int transferMode);
		void DrawClosedRect(int x1, int y1, int x2, int y2, RGBA color, int transferMode);
		void CopyPixelsWithScaling(Bitmap *destBitmap, double xScale, double yScale);

	protected:
		int width;
		int height;
		int widthInBytes;
		int stride;
		int bytesPerPixel;
		BYTE *pixels;
		BYTE *row0;
	};

#endif // !defined(FRAMEBUF_H)
