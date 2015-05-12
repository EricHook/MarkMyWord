#ifndef _H_palette_
#define _H_palette_

#include "vpalette.h"

class Palette : public VPalette
	{
	public:
		Palette();
		virtual ~Palette();
		virtual void ActivatePalette();
		HPALETTE ActivatePalette(HDC hDC);

	private:
	};

#endif // _H_palette_
