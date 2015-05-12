#include "os.h"
#include "rc.h"
#include "sci.h"
#include "globals.h"
#include "inifile.h"
#include "tsprite.h"
#include "framebuf.h"

#include <io.h>

#include "Client Include.h"

#define FM_VIEW_BASE 1000000
#define MAX_FM_FILES 1000
#define MAX_FM_FILENAMES 64000

void RoomClient::GetMyFace(FaceInfo *faceInfo, char *path)
{
	char fileName[MAX_PATH];
	
	memset(faceInfo, 0, sizeof(FaceInfo));
	path[0] = 0;
	
	if (globalIniFile->ReadValue("UseFaceFile", 0) == 0)
		return;
	
	globalIniFile->ReadString("FaceFileName", "", fileName, MAX_PATH - 1);
	
	if (fileName[0] == 0)
		return;
	
	sprintf(path, "c:\\program files\\won\\facemaker\\%s", fileName);
	
	ReadFaceFile(path, faceInfo);
}

static char *fmNames;
static int *fmIndex;

int __cdecl CompareFMFileNames(const void *i1, const void *i2)
{
	int index1 = *(int *)i1, index2 = *(int *)i2;
	
	return lstrcmpi(&fmNames[index1], &fmNames[index2]);
}

int RoomClient::ReadAndSortFaceFiles()
{
	int count, total;
	long handle;
	struct _finddata_t fileInfo;
	
	handle = _findfirst("c:\\program files\\won\\facemaker\\*.fac", &fileInfo);
	if (handle == -1)
		return -1;
	
	fmIndex = new int[MAX_FM_FILES];
	fmNames = new char[MAX_FM_FILENAMES];
	total = 0;
	count = 0;
	
	for ( ; ; ) {
		if (fileInfo.size == sizeof(FaceFile))
			if ((total + strlen(fileInfo.name) + 1) < MAX_FM_FILENAMES && count < MAX_FM_FILES) {
				strcpy(&fmNames[total], fileInfo.name);
				fmIndex[count] = total;
				total += strlen(fileInfo.name) + 1;
				count++;
			}
		
		if (_findnext(handle, &fileInfo))
			break;
	}
	
	_findclose(handle);
	
	qsort(fmIndex, count, sizeof(int), CompareFMFileNames);
	
	return count;
}

int RoomClient::CountFaceFiles(char *fileNameToFind)
{
	char p[MAX_PATH];
	int i, n, result;
	
	if (!pApp->pResourceManager->ResourceIsAvailable(RES_TSPRITE, 1001001))
		return 0;
	
	strcpy(p, "c:\\program files\\won\\facemaker\\*.fac");
	
	n = ReadAndSortFaceFiles();
	
	if (fileNameToFind == NULL) {
		delete [] fmIndex;
		delete [] fmNames;
		
		return n;
	}
	
	result = -1;
	
	for (i = 0 ; i < n ; i++)
		if (!lstrcmpi(&fmNames[fmIndex[i]], fileNameToFind))
			{ result = i; break; }
	
	delete [] fmIndex;
	delete [] fmNames;
	
	return result;
}

bool RoomClient::LoadNthFaceFile(int n, FaceInfo *faceInfo, char *fileName)
{
	char p[MAX_PATH];
	int count;
	
	memset(faceInfo, 0, sizeof(FaceInfo));
	fileName[0] = 0;
	
	if (!pApp->pResourceManager->ResourceIsAvailable(RES_TSPRITE, 1001001))
		return false;
	
	strcpy(p, "c:\\program files\\won\\facemaker\\*.fac");
	
	count = ReadAndSortFaceFiles();
	
	if (count < n) {
		delete [] fmIndex;
		delete [] fmNames;
		
		return false;
	}
	
	strcpy(fileName, &fmNames[fmIndex[n]]);
	sprintf(p, "c:\\program files\\won\\facemaker\\%s", fileName);
	
	delete [] fmIndex;
	delete [] fmNames;
	
	return ReadFaceFile(p, faceInfo);
}

bool RoomClient::ReadFaceFile(char *path, FaceInfo *faceInfo)
{
	int i, count = 0;
	FILE *f;
	FaceFile faceFile;
	
	f = fopen(path, "rb");
	if (f == NULL)
		return false;
	
	count = fread(&faceFile, sizeof(FaceFile), 1, f);
	fclose(f);
	
	if (count != 1)
		{ assert(FALSE); return FALSE; }
	
	for (i = 0 ; i < FM_MAX_FEATURES ; i++) {
		if (faceFile.featureViews[i] == 0)
			faceInfo->featureViews[i] = 0;
		else
			faceInfo->featureViews[i] = (short)(faceFile.featureViews[i] - FM_VIEW_BASE);
		faceInfo->xOffsets[i] = faceFile.xOffsets[i];
		faceInfo->yOffsets[i] = faceFile.yOffsets[i];
	}
	faceInfo->skinTone = faceFile.skinTone;
	
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////

FaceMakerFace::FaceMakerFace()
{
	int i;
	
	for (i = 0 ; i < FM_MAX_VIEWS ; i++)
		subViews[i] = NULL;
	
	memset(&faceInfo, 0, sizeof(FaceInfo));
	
	halfSize = false;
}

FaceMakerFace::~FaceMakerFace()
{
	int i;
	
	for (i = 0 ; i < FM_MAX_VIEWS ; i++)
		if (subViews[i]) { delete subViews[i]; subViews[i] = NULL; }
}

void FaceMakerFace::Init(sciPlane *thePlane, GameCharacter *newOwner)
{
	int i;
	
	plane = thePlane;
	
	sciFeature::Init();
	
	memset(&faceInfo, 0, sizeof(FaceInfo));
	
	for (i = 0 ; i < FM_MAX_VIEWS ; i++) {
		if (!subViews[i])
			subViews[i] = new FaceMakerSubView();
		subViews[i]->plane = thePlane;
		subViews[i]->Init();
	}
}

void FaceMakerFace::Posn(int newX, int newY)
{
	sciFeature::Posn(newX, newY);
	
	right = newX + HOYLE_CHARACTER_WIDTH;
	bottom = newY + HOYLE_CHARACTER_HEIGHT;
}

//
void FaceMakerFace::SetHalfSize(bool onoff, SCALE_TYPE st /*= ST_DISCARD */)
{
	halfSize = onoff;
	/*
	for (i = 0 ; i < FM_MAX_VIEWS ; i++)
		if (subViews[i])
			subViews[i]->SetHalfSize(onoff, st);
	*/
}

void FaceMakerFace::SetPri(int newPri)
{
	/*
		TORSO, TORSOSHADOW,
		CLOTHING,
		HEAD, HEADSHADOW,
		LEYE1, LEYE2, LEYE3,
		REYE1, REYE2, REYE3,
		HAIR1, HAIR2,
		NOSE1, NOSE2, NOSE3,
		MOUTH1, MOUTH2, MOUTH3,
		LEYEBROW,
		REYEBROW,
		FACEHAIR,
		GLASSES
	*/
	
	int i;
	static int priorities[FM_MAX_VIEWS] = {
			/*
			10, 1, 15, 20, 2, 62, 61, 60, 62, 61, 60, 110, 6, 32, 31, 30,
			52, 51, 50, 45, 44, 43, 42, 41, 40, 80, 120
			*/
			4, 1, 5, 6, 2, 23, 21, 19, 24, 22, 20, 26, 3, 9, 8, 7,
			18, 17, 16, 15, 14, 13, 12, 11, 10, 25, 27
		};
	
	sciFeature::SetPri(newPri);
	
	for (i = 0 ; i < FM_MAX_VIEWS ; i++)
		if (subViews[i])
			subViews[i]->SetPri(newPri + priorities[i]);
}

void FaceMakerFace::Show()
{
	int i;
	
	sciFeature::Show();
	
	for (i = 0 ; i < FM_MAX_FEATURES ; i++)
		ShowFeature(i);
}

void FaceMakerFace::ShowFeature(int feature)
{
	int i, numPieces, index, xOffset, yOffset, viewNum;
	
	numPieces = GetFeatureNumPieces(feature);
	index = GetFeatureFirstViewIndex(feature);
	xOffset = faceInfo.xOffsets[feature];
	yOffset = faceInfo.yOffsets[feature];
	viewNum = faceInfo.featureViews[feature];
	
	for (i = 0 ; i < numPieces ; i++) {
		if (viewNum == 0) {
			subViews[index + i]->Hide();
			subViews[index + i]->SetView(0);
			continue;
		}
		
		// we need this special case because
		// torsos, heads, and hair have two views but their
		// second view has the same viewNum as their first
		
		if (i == 1 && (feature == FM_TORSO || feature == FM_HEAD || feature == FM_HAIR)) {
			subViews[index + i]->SetView(FM_VIEW_BASE + viewNum);
			if (feature == FM_HAIR)
				subViews[index + i]->SetCel(1);
			else
				subViews[index + i]->SetCel(FM_MAX_SKIN_TONES);
		}
		else {
			subViews[index + i]->SetView(FM_VIEW_BASE + viewNum + i);
			if (feature == FM_TORSO || feature == FM_HEAD)
				subViews[index]->SetCel(faceInfo.skinTone);
		}
		subViews[index + i]->Posn(left + xOffset, top + yOffset);
		subViews[index + i]->Show();
		
		if ((index + i) != GetFeatureFirstViewIndex(FM_GLASSES))
			continue;
		
		if (subViews[index + i]->LastCel() != 0) {
			if (!subViews[index + i]->cycler) {
				subViews[index + i]->cycleSpeed = 10;
				subViews[index + i]->SetCycle(new sciForward());
			}
		}
		else {
			subViews[index + i]->cycleSpeed = 4;
			subViews[index + i]->SetCycle(NULL);
		}
	}
}

void FaceMakerFace::Hide()
{
	int i;
	
	sciFeature::Hide();
	
	for (i = 0 ; i < FM_MAX_VIEWS ; i++)
		subViews[i]->Hide();
}

/*
#ifdef _DEBUG

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

#endif
*/

void FaceMakerFace::Render(RECT &updateRect)
{
	/*
		TORSO, TORSOSHADOW,
		CLOTHING,
		HEAD, HEADSHADOW,
		LEYE1, LEYE2, LEYE3,
		REYE1, REYE2, REYE3,
		HAIR1, HAIR2,
		NOSE1, NOSE2, NOSE3,
		MOUTH1, MOUTH2, MOUTH3,
		LEYEBROW,
		REYEBROW,
		FACEHAIR,
		GLASSES
	*/
	
	BYTE *pDestPixels, *pDestPixelsBase, *p;
	int i, x, y, base, b, startX, startY, bytesPerPixel, rowsToCopy, pixelsToCopy, viewIndex, featureIndex;
	RGB rgb;
	DWORD *pixelData;
	TEX_PIXEL *rows;
	RECT myScreenRect, clippedUpdateRect;
	FrameBuffer *frameBuffer;
	static int order[FM_MAX_VIEWS] = {
			/*
			4, 1, 5, 6, 2, 23, 21, 19, 24, 22, 20, 26, 3, 9, 8, 7,
			18, 17, 16, 15, 14, 13, 12, 11, 10, 25, 27
			*/
			1, 4, 12, 0, 2, 3, 15, 14, 13, 24, 23, 22, 21, 20, 19,
			18, 17, 16, 7, 10, 6, 9, 5, 8, 25, 11, 26
		};
	
	if (!halfSize) {
		for (i = 0 ; i < FM_MAX_VIEWS ; i++) // FM_MAX_VIEWS
			if (subViews[order[i]]->IsNotHidden())
				subViews[order[i]]->sciProp::Render(updateRect);
		
		return;
	}
	
	SetRect(&myScreenRect, left, top, left + HOYLE_CHARACTER_WIDTH / 2, top + HOYLE_CHARACTER_HEIGHT / 2);
	IntersectRect(&clippedUpdateRect, &updateRect, &myScreenRect);
	if (IsRectEmpty(&clippedUpdateRect))
		return;
	
	if ((right - left) != HOYLE_CHARACTER_WIDTH || (bottom - top) != HOYLE_CHARACTER_HEIGHT)
		{ assert(FALSE); return; }
	
	// special case: don't draw a shunken face partially off the frame buffer
	
	frameBuffer = plane->GetWindow()->frameBuffer;
	bytesPerPixel = frameBuffer->GetBytesPerPixel();
	
	if (left < 0 || top < 0 || right >= frameBuffer->GetWidth() || bottom >= frameBuffer->GetHeight())
		return;
	
	pixelData = new TEX_PIXEL[HOYLE_CHARACTER_WIDTH * HOYLE_CHARACTER_HEIGHT];
	if (!pixelData)
		return;
	
	// OK, now we have our buffer, but the question is: what background should we
	// fill the buffer with to render onto?  Answer: the pixels from the screen that
	// we are about to draw onto.  But since the screen pixels are half the size
	// of the buffer in each dimension, we'll have to expand the screen bits into
	// the buffer by doubling each pixel in each dimension
	
	// n = HOYLE_CHARACTER_WIDTH * HOYLE_CHARACTER_HEIGHT * sizeof(TEX_PIXEL);
	// memset(pixelData, 0, n);
	
	pDestPixels = frameBuffer->GetRow(plane->GetY() + top) + ((plane->GetX() + left) * bytesPerPixel);
	p = (BYTE *)pixelData;
	base = 0;
	
	for (y = 0 ; y < HOYLE_CHARACTER_HEIGHT ; y += 2) {
		pDestPixelsBase = pDestPixels;
		
		for (x = 0 ; x < HOYLE_CHARACTER_WIDTH ; x += 2) {
			rgb = TEX_PIXELtoRGB(*PTEX_PIXEL(pDestPixelsBase));
			pDestPixelsBase += bytesPerPixel;
			
			b = base + x * sizeof(DWORD);
			
			p[b] = rgb.blue;
			p[b + 1] = rgb.green;
			p[b + 2] = rgb.red;
			p[b + 3] = 255;
			p[b + 4] = rgb.blue;
			p[b + 5] = rgb.green;
			p[b + 6] = rgb.red;
			p[b + 7] = 255;
			
			b += HOYLE_CHARACTER_WIDTH * sizeof(DWORD);
			p[b] = rgb.blue;
			p[b + 1] = rgb.green;
			p[b + 2] = rgb.red;
			p[b + 3] = 255;
			p[b + 4] = rgb.blue;
			p[b + 5] = rgb.green;
			p[b + 6] = rgb.red;
			p[b + 7] = 255;
		}
		
		base += 2 * HOYLE_CHARACTER_WIDTH * sizeof(DWORD);
		pDestPixels += frameBuffer->GetStride();
	}
	
#ifdef _DEBUG
	/*
	if (ControlKeyDown())
		WriteTarga("targa.tga", pixelData, HOYLE_CHARACTER_WIDTH, HOYLE_CHARACTER_HEIGHT);
	*/
#endif
	
	// draw each sub-view onto the temporary buffer
	
	for (i = 0 ; i < FM_MAX_VIEWS ; i++) { // FM_MAX_VIEWS 
		viewIndex = order[i];
		featureIndex = GetFeatureIndexFromViewIndex(viewIndex);
		if (subViews[viewIndex]->IsNotHidden())
			subViews[viewIndex]->FMRender(pixelData, faceInfo.xOffsets[featureIndex], faceInfo.yOffsets[featureIndex]);
	}
	
	// copy the temporary buffer, scaled, to the frame buffer, respecting the updateRect
	
	startX = clippedUpdateRect.left - left;
	pixelsToCopy = clippedUpdateRect.right - clippedUpdateRect.left;
	
	startY = clippedUpdateRect.top - top;
	rowsToCopy = clippedUpdateRect.bottom - clippedUpdateRect.top;
	
	rows = pixelData + startY * 2 * HOYLE_CHARACTER_WIDTH + startX;
	
	pDestPixels = frameBuffer->GetRow(plane->GetY() + top + startY) + ((plane->GetX() + left + startX) * bytesPerPixel);
	
	while (rowsToCopy--) {
		TextureRenderScaledRow(pDestPixels, rows, startX, pixelsToCopy, HOYLE_CHARACTER_WIDTH, rowsToCopy, NULL, ST_BILINEAR, NULL);
		rows += 2 * HOYLE_CHARACTER_WIDTH; // skip every other row of the full image
		pDestPixels += frameBuffer->GetStride();
	}
	
	delete [] pixelData;
	pixelData = NULL;
}

int FaceMakerFace::GetFeatureNumPieces(int feature)
{
	// torso (2), clothing, head (2), left eye (3), right eye (3), hair (2), nose (3),
	// mouth (3), left eyebrow (3), right eyebrow (3), facialHair, glasses
	
	static int numViews[FM_MAX_FEATURES] = { 2, 1, 2, 3, 3, 2, 3, 3, 3, 3, 1, 1 };
	
	return numViews[feature];
}

int FaceMakerFace::GetFeatureFirstViewIndex(int feature)
{
	// torso (2), clothing, head (2), left eye (3), right eye (3), hair (2), nose (3),
	// mouth (3), left eyebrow (3), right eyebrow (3), facialHair, glasses
	
	static int index[FM_MAX_FEATURES] = { 0, 2, 3, 5, 8, 11, 13, 16, 19, 22, 25, 26 };
	
	return index[feature];
}

int FaceMakerFace::GetFeatureIndexFromViewIndex(int viewIndex)
{
	// torso (2), clothing, head (2), left eye (3), right eye (3), hair (2), nose (3),
	// mouth (3), left eyebrow (3), right eyebrow (3), facialHair, glasses
	
	static int index[FM_MAX_VIEWS] = { 0, 0, 1, 2, 2, 3, 3, 3, 4, 4, 4, 5, 5, 6, 6, 6,
									   7, 7, 7, 8, 8, 8, 9, 9, 9, 10, 11 };
	
	return index[viewIndex];
}

////////////////////////////////////////////////////////////////////////////////////////////////

void FaceMakerSubView::Render(RECT &updateRect)
{
}

void FaceMakerSubView::FMRender(DWORD *pixelData, int xOffset, int yOffset)
{
	BYTE **curRow, **rows, *pSourcePixels;
	int numRows, w, renderX, renderY;
	LOOP *loops;
	
	loops = sprite->GetLoops();
	
	if (!loops)
		return;
	
	// expand the feature into the local coordinate system of the face
	// taking into account the feature's relative position to the face
	// and its internal offset
	
	renderX = xOffset + GetBitmapX();
	renderY = yOffset + GetBitmapY();
	
	// expand the compressed TEX cel into our RGBA buffer
	
	w = right - left;
	rows = loops[loop].cels[cel].rows;
	curRow = &rows[0];
	
	for (numRows = 0 ; numRows < (bottom - top) ; numRows++) {
		pSourcePixels = *curRow;
		TextureUncompressRow((BYTE *)(pixelData + ((renderY + numRows) * HOYLE_CHARACTER_WIDTH + renderX)), pSourcePixels, w, sprite->GetColors(), NULL);
		curRow++;
	}
}
