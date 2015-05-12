#ifndef _H_vpalette_
#define _H_vpalette_

#include "object.h"

#define AT_SIZE 32768		// 15 bits: 5 fore, 5 back, 5 alpha
extern BYTE inverseColorTable[AT_SIZE];

class VPalette : public Object
	{
	public:
		VPalette();
		virtual ~VPalette();
		virtual void ActivatePalette();
		void CalculateInverseTable();
		void SetColors(RGBQUAD newColors[256], BOOL calcInverse = TRUE);
		void SetPreColors(RGBQUAD newColors[256]);
		void Enable();
		void Disable();
		void LoadResource(int resID);
		double GetGamma();
		void SetGamma(double newGamma);
		void DoGammaCorrection(RGBQUAD *quad);

		RGBQUAD colors[256];
		BOOL tableCalculated;
		RGBQUAD preColors[256];

	protected:
		BOOL enabled;
		double curGamma;
	};

#endif // _H_vpalette_
