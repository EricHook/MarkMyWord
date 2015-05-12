#include "os.h"
#include "framebuf.h"
#include "bitmap.h"

FrameBuffer::FrameBuffer(int newWidth, int newHeight,
		int newBytesPerPixel, BOOL topDown)
	{
	width = newWidth;
	height = newHeight;
	bytesPerPixel = newBytesPerPixel;
	widthInBytes = (width*bytesPerPixel) + 3 - (((width*bytesPerPixel)-1) % 4);
	if (topDown)
		stride = widthInBytes;
	else
		stride = -widthInBytes;

	pixels = new BYTE[height * widthInBytes];
	memset(pixels, 0, height * widthInBytes);
#if defined(FLIP_DIB)
	row0 = pixels + (height - 1) * widthInBytes;
#else
	row0 = pixels;
#endif // defined(FLIP_DIB)
	}

FrameBuffer::~FrameBuffer()
	{
	delete[] pixels;
	}


int FrameBuffer::GetStride()
	{
	return stride;
	}

BYTE *FrameBuffer::GetRow(int row)
	{
	return row0 + (row * GetStride());
	}

int FrameBuffer::GetWidth()
	{
	return width;
	}

int FrameBuffer::GetHeight()
	{
	return height;
	}

int FrameBuffer::GetBytesPerPixel()
	{
	return bytesPerPixel;
	}

void FrameBuffer::DrawPixel(int x, int y, RGBA color, int transferMode)
	{
	TEX_PIXEL *dest;
	TEX_PIXEL source;

	if ((x >= GetWidth()) || (y >= GetHeight()))
		return;

	dest = PTEX_PIXEL(GetRow(y) + x*GetBytesPerPixel());
	source = RGBAtoTEX_PIXEL(color);

	switch (transferMode)
		{
		case ALPHA_FLAG:
			switch (color.a)
				{
				case 0:
					break;
				case 255:
					*dest = source;
					break;
				default:
					AlphaBlend(*dest, source, color.a);
					break;
				}
			break;
		case ADD_FLAG:
			AddBlend(*dest, source, color.a);
			break;
		case MULTIPLY_FLAG:
			MultiplyBlend(*dest, source, color.a);
			break;
		}
	}

void FrameBuffer::DrawHLine(int x1, int x2, int y, RGBA color, int transferMode)
	{
	while (x1 <= x2)
		{
		DrawPixel(x1, y, color, transferMode);
		++x1;
		}
	}

void FrameBuffer::DrawVLine(int x, int y1, int y2, RGBA color, int transferMode)
	{
	while (y1 <= y2)
		{
		DrawPixel(x, y1, color, transferMode);
		++y1;
		}
	}

void FrameBuffer::DrawClosedRect(int x1, int y1, int x2, int y2, RGBA color, int transferMode)
	{
	while (y1 <= y2)
		{
		DrawHLine(x1, x2, y1, color, transferMode);
		++y1;
		}
	}

void FrameBuffer::CopyPixelsWithScaling(Bitmap *destBitmap, double xScale, double yScale)
	{
	double x, y, Y1, Y2;
	int a1;
	PTEX_PIXEL p1, p2;

	if (destBitmap->GetPixels() == 0)
		return;
	for (y = 0; y < (destBitmap->Height()*yScale); ++y)
		{
		Y1 = y;
		Y2 = y/yScale;
		for (x = 0; x < (destBitmap->Width()*xScale); ++x)
			{
			a1 = int(Y1 * destBitmap->GetWidthInBytes() + x*bytesPerPixel);
			p1 = PTEX_PIXEL(destBitmap->GetPixels() + a1);
			p2 = PTEX_PIXEL(GetRow(int(Y2)) + int(x*GetBytesPerPixel()/xScale));
			*p1 = *p2;
			}
		}
	}
