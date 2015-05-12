#if !defined (TEX_H)
#define TEX_H

typedef struct
	{
	int width;
	int height;
	int x;
	int y;
	int *lengthOfRows;
	BYTE **rows;
	} TEX_CEL;

typedef struct
	{
	int numCels;
	TEX_CEL *cels;
	} TEX_LOOP;

typedef struct
	{
	int x;
	int y;
	int width;
	int height;
	RGBA *pixels;
	} TEX_FRAME;

class TEX
	{
	public:
		TEX();
		~TEX();
		void AddCel(RGBA *bitmap, int w, int h, int transferMode, BOOL trim = TRUE);
		void AddLoop();
		int GetSize();
		int Write(PBYTE out);
		void SetPalette(RGB *colors);
		void SetPaletteN(RGB *colors, int n);
		void FigurePalette(RGB *pixels, int numPixels);
		void FigureBestPalette1(RGBA *bitmap, int w, int h);
		void FigureBestPalette1a(RGB *bitmap, int l);
		void FigureBestPalette2(RGB *bitmap, int l);
		void Dither24to15(RGBA *bitmap, int width, int height);
		void AddFrame(RGBA *pixels, int width, int height, int transferMode, BOOL trim = TRUE);
		void CompressFrames(int transferMode, int ditherMode);
		void SetLastCelOffsets(int x, int y);

	protected:
		void FigureTrim(RGBA *bitmap, int w, int h,
			int &trimLeft, int &trimTop, int &trimRight, int &trimBottom);
		void ReduceColors(	RGBA *sourceRow, RGBA *nextSourceRow, 
							BYTE *colorRow, BYTE *alphaRow, int width, int transferMode);
		BYTE FindNearestColor(BYTE red, BYTE green, BYTE blue);

		RGB palette[256];
		BOOL calcerror;
		int numLoops;
		TEX_LOOP *loops;
		int numFrames;
		TEX_FRAME *frames;
	};

#endif // TEX_H
