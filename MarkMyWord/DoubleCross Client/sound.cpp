#include "os.h"

#include "sound.h"
#include "app.h"
#include "resman.h"
#include "unihand.h"
// #include "sos.h"

extern Application *pApp;
extern BYTE btSoundBitSupport;

Sound::Sound(int iResourceID, ResourceManager *resourceManager)
	: VSound(RES_SOUND, iResourceID, resourceManager)
	{
	HMMIO			hmmio;
	MMCKINFO		mmckinfoParent;
	MMCKINFO		mmckinfoSubchunk;
	MMIOINFO *		pmmioInfo;
	HPSTR			hpWaveData;
	DWORD			dwWaveDataSize;
	PCMWAVEFORMAT	wfWaveFormat;
	BYTE *p;

	if (handle)
		p = (BYTE *)handle->Lock();
	else
		p = NULL;
	if (!p)
		{
		return;
		}

	/* Open the resource for reading using buffered I/O.
	 */
	pmmioInfo = new MMIOINFO;
	pmmioInfo->pIOProc = NULL;
	pmmioInfo->fccIOProc = FOURCC_MEM;
	pmmioInfo->pchBuffer = (char *)p;
	pmmioInfo->cchBuffer = numOfBytes;
	pmmioInfo->adwInfo[0] = NULL;
	pmmioInfo->dwFlags = NULL;
	pmmioInfo->wErrorRet = NULL;
	pmmioInfo->htask = NULL;
	pmmioInfo->pchNext = NULL;
	pmmioInfo->pchEndRead = NULL;
	pmmioInfo->pchEndWrite = NULL;
	pmmioInfo->lBufOffset = NULL;
	pmmioInfo->lDiskOffset = NULL;
	pmmioInfo->dwReserved1 = NULL;
	pmmioInfo->dwReserved2 = NULL;
	pmmioInfo->hmmio = NULL;

	hmmio = mmioOpen(NULL, pmmioInfo, MMIO_READWRITE);

	// Locate a 'RIFF' chunk with a 'WAVE' form type 
	// to make sure it's a WAVE file.
	mmckinfoParent.fccType = mmioFOURCC('W', 'A', 'V', 'E');
	mmioDescend(hmmio, (LPMMCKINFO) &mmckinfoParent, NULL, MMIO_FINDRIFF);

	// Now, find the format chunk (form type 'fmt '). It should be
	// a subchunk of the 'RIFF' parent chunk.
	mmckinfoSubchunk.ckid = mmioFOURCC('f', 'm', 't', ' ');
	mmioDescend(hmmio, &mmckinfoSubchunk, &mmckinfoParent, MMIO_FINDCHUNK);

	// Read the format chunk.
	mmioRead(hmmio, (HPSTR) &wfWaveFormat, sizeof(wfWaveFormat));

	switch (wfWaveFormat.wf.nChannels) {
		case 1:
			switch (wfWaveFormat.wf.nBlockAlign) {
				case 1:
					wfFormat = WF_8BITMONO;
					break;
				case 2:
					wfFormat = WF_16BITMONO;
					break;
				}
			break;
		case 2:
			switch (wfWaveFormat.wf.nBlockAlign) {
				case 2:
					wfFormat = WF_8BITSTEREO;
					break;
				case 4:
					wfFormat = WF_16BITSTEREO;
					break;
				}
			break;
		}
	wSamplesPerSec = (WORD)(wfWaveFormat.wf.nSamplesPerSec);

	if ((wSamplesPerSec != 11025) &&
		(wSamplesPerSec != 22050) &&
		(wSamplesPerSec != 44100))
		{
#ifdef _DEBUG
		//MessageBox(NULL, "Sound at Mac rate!", "Error", MB_OK | MB_TASKMODAL);
		DebugString("Sound at Mac rate!\n");
#endif // _DEBUG
		if (wSamplesPerSec < 22050)
			wSamplesPerSec = 11025;
		else if (wSamplesPerSec < 44100)
			wSamplesPerSec = 22050;
		else
			wSamplesPerSec = 44100;
		}

	// Ascend out of the format subchunk.
	mmioAscend(hmmio, &mmckinfoSubchunk, 0);

	// Find the data subchunk.
	mmckinfoSubchunk.ckid = mmioFOURCC('d', 'a', 't', 'a');
	mmioDescend(hmmio, &mmckinfoSubchunk, &mmckinfoParent, MMIO_FINDCHUNK);

	// Get the size of the data subchunk.
	dwWaveDataSize = mmckinfoSubchunk.cksize;

	dwNumOfSamples = dwWaveDataSize;

	// Allocate and lock memory for the waveform data.
	pSamples = new UniHandle(dwWaveDataSize);
	hpWaveData = PCHAR(pSamples->Lock());

	// Read the waveform data subchunk.
	mmioRead(hmmio, hpWaveData, dwWaveDataSize);

	// We're done with the file, close it.
	mmioClose(hmmio, 0);

	delete pmmioInfo;

	if (wfWaveFormat.wBitsPerSample == 8) {
		if (wfWaveFormat.wf.nChannels == 1) {
			wfFormat = WF_8BITMONO;
			dwNumOfSamples = dwWaveDataSize;
			}
		else if (wfWaveFormat.wf.nChannels == 2) {
			wfFormat = WF_8BITSTEREO;
			dwNumOfSamples = dwWaveDataSize / 2;
			}
		}
	else if (wfWaveFormat.wBitsPerSample == 16) {
		if (wfWaveFormat.wf.nChannels == 1) {
			wfFormat = WF_16BITMONO;
			dwNumOfSamples = dwWaveDataSize / 2;
			}
		else if (wfWaveFormat.wf.nChannels == 2) {
			wfFormat = WF_16BITSTEREO;
			dwNumOfSamples = dwWaveDataSize / 4;
			}
		}

	// Convert 16bit to 8bit if necessary.
	if ((btSoundBitSupport == 8) && ((wfFormat == WF_16BITMONO) || (wfFormat == WF_16BITSTEREO)))
		{
		Convert16to8();
		}

	if (handle)
		{
		delete handle;
		handle = NULL;
		}
	}

Sound::~Sound()
	{
	}

/*
SoundC::SoundC(int iResourceID, ResourceManager *resourceManager)
	: VSound(RES_CMP, iResourceID, resourceManager)
	{
	_SOS_COMPRESS_HEADER *header;
	_SOS_COMPRESS_INFO info;
	PSTR  pDestBuffer;
	int wBitSize;
	BYTE *p;

	if (handle)
		p = (BYTE *)handle->Lock();
	else
		p = NULL;
	if (!p)
		{
		return;
		}

	header = (_SOS_COMPRESS_HEADER *)p;
	wBitSize = 16;

	// allocate memory for source and dest buffers
	pSamples = new UniHandle(header->dwUnCompressedSize);
	pDestBuffer    =  PSTR(pSamples->Lock());

	// initialize compression structure
	info.dwCompSize   =  header->dwCompressedSize;
	info.dwUnCompSize =  header->dwUnCompressedSize;
	info.wBitSize     =  wBitSize;

	// decompress init stream
	sosCODECInitStream( &info );

	// set source and destination pointer
	info.pSource     =  ((PSTR)header) + sizeof(_SOS_COMPRESS_HEADER);
	info.pDest       =  (PSTR)pDestBuffer;

	// decompress buffer
	sosCODECDecompressData( &info, header->dwUnCompressedSize);

	wfFormat = WF_16BITMONO;
	wSamplesPerSec = 22050;
	dwNumOfSamples = header->dwUnCompressedSize/2;

	// Convert 16bit to 8bit if necessary.
	if ((btSoundBitSupport == 8) && ((wfFormat == WF_16BITMONO) || (wfFormat == WF_16BITSTEREO)))
		{
		Convert16to8();
		}

	if (handle)
		{
		delete handle;
		handle = NULL;
		}
	}

SoundC::~SoundC()
	{
	}
*/
