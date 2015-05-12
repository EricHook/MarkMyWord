#if !defined(TSPRITE_H)
#define TSPRITE_H

class TSprite : public Resource
	{
	public:
		TSprite(RESOURCE_TYPES newType, int newResourceID,
			ResourceManager *newResourceManager);
		~TSprite();
		LOOP *GetLoops() const;
		int GetNumLoops() const;
		int GetNumCels(int loop) const;
		int GetWidth(int loop, int cel) const;
		int GetHeight(int loop, int cel) const;
		int GetX(int loop, int cel) const;
		int GetY(int loop, int cel) const;
		TEX_PIXEL *GetColors() const;
		
		void CopyPixels(int loop, int cel, FrameBuffer *frameBuffer,
			RECT destRect, RECT sourceRect, COLORMAPPING *colorMap = NULL) const;
		
		void CopyScaledPixels(int loop, int cel, FrameBuffer *frameBuffer,
			RECT destRect, RECT sourceRect, SCALE_TYPE st, COLORMAPPING *colorMap = NULL) const;
		
		BYTE ColorOfPoint(int loop, int cel, int x, int y) const;
		TEX_PIXEL GetColor(int i);
		void GetPixel(int loop, int cel, int x, int y, RGBA &color, BYTE &flags);
		void SetGamma(double g);

	protected:
		
		short	numLoops;
		LOOP	*loops;
		BYTE	*stuff;
		TEX_PIXEL colors[256];
		TEX_PIXEL uncorrectedColors[256];
	};

#endif // !defined(TSPRITE_H)
