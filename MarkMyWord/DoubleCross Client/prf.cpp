#include "os.h"

#include "prf.h"
#include "unihand.h"
#include "symbol.h"
#include "util.h"
#include "unihand.h"

#include <string.h>

void fcopy(FILE *pDestFile, FILE *pSourceFile, DWORD dwLength) {
	BYTE *pBytes;
	DWORD dwBytesToCopy = dwLength;
	int iBufferSize = 16384;

	pBytes = new BYTE[iBufferSize];
	while (dwBytesToCopy) {
		fread(pBytes, (int)min((DWORD)iBufferSize, dwBytesToCopy), 1, pSourceFile);
		fwrite(pBytes, (int)min((DWORD)iBufferSize, dwBytesToCopy), 1, pDestFile);
		if (dwBytesToCopy > (DWORD)iBufferSize)
			dwBytesToCopy -= iBufferSize;
		else
			dwBytesToCopy = 0;
		}
	delete pBytes;
	}

PRF::PRF() {
	pFile = NULL;
	pPRFFileHeader = NULL;
	pPRFResourceDirectory = NULL;
	fSymbolTable = NULL;
	}

PRF::~PRF() {
	if (pFile)
		fclose(pFile);
	if (pPRFFileHeader)
		delete pPRFFileHeader;
	if (pPRFResourceDirectory)
		delete[] pPRFResourceDirectory;
	}

void PRF::New(char *szFilename) {
	pFile = fopen(szFilename, "wb+");
	pPRFFileHeader = new PRF_FILEHEADER;
	
	
	pPRFFileHeader->dwByteOrder = 1;
	pPRFFileHeader->c4ID = MAKEC4ID('P','R','F','\0');

	pPRFFileHeader->dwVersion = 2;
	pPRFFileHeader->dwSizeOfResourceDirectory = sizeof(PRF_RESOURCEDIRECTORY) + sizeof(PRF_RESOURCEMAP);
	pPRFFileHeader->dwFileSize = pPRFFileHeader->dwSizeOfResourceDirectory + sizeof(PRF_FILEHEADER);
	pPRFFileHeader->dwResourceDirectoryOffset = sizeof(PRF_FILEHEADER);

	pPRFResourceDirectory = (PRF_RESOURCEDIRECTORY*)new BYTE[pPRFFileHeader->dwSizeOfResourceDirectory];
	pPRFResourceDirectory->dwNumOfResources = 1;
	pPRFResourceDirectory->dwIncludeDescriptions = TRUE;
	
	// Dummy resource.
	pPRFResourceDirectory->prfResourceMap[0].c4Type = 0;
	pPRFResourceDirectory->prfResourceMap[0].dwID = 0;
	pPRFResourceDirectory->prfResourceMap[0].dwOffset = sizeof(PRF_FILEHEADER);
	pPRFResourceDirectory->prfResourceMap[0].dwLength = 0;
	
	WriteHeaders();
	}

void PRF::WriteHeaders() {
	fwrite(pPRFFileHeader, sizeof(PRF_FILEHEADER), 1, pFile);
	fwrite(pPRFResourceDirectory, (int)(pPRFFileHeader->dwSizeOfResourceDirectory), 1, pFile);
	}

void PRF::GetFileInfo() {
	DWORD bNeedToSwap = 0;
	PRF_FILEHEADER_V1 *pPRFFileHeader_v1;
	PRF_RESOURCEDIRECTORY_V1 *pPRFResourceDirectory_v1;
	DWORD i;
	
	pPRFFileHeader = new PRF_FILEHEADER;
	fread(pPRFFileHeader, sizeof(PRF_FILEHEADER), 1, pFile);

	// Convert version1 headers to current.
	if (pPRFFileHeader->dwVersion == 1)
		{
		pPRFFileHeader_v1 = (PRF_FILEHEADER_V1*)pPRFFileHeader;
		pPRFFileHeader->dwByteOrder = pPRFFileHeader_v1->wByteOrder;
		}

	bNeedToSwap = pPRFFileHeader->dwByteOrder != 1;
	if(bNeedToSwap)
		SwapPrfFileHeader();
	fseek(pFile, pPRFFileHeader->dwResourceDirectoryOffset, SEEK_SET);
	pPRFResourceDirectory = (PRF_RESOURCEDIRECTORY*)new BYTE[pPRFFileHeader->dwSizeOfResourceDirectory];
	fread(pPRFResourceDirectory, (int)(pPRFFileHeader->dwSizeOfResourceDirectory), 1, pFile);

	// Convert version1 headers to current.
	if (pPRFFileHeader->dwVersion == 1)
		{
		pPRFResourceDirectory_v1 = (PRF_RESOURCEDIRECTORY_V1*)pPRFResourceDirectory;
		// Order of assignment is important to not lose data.
		pPRFResourceDirectory->dwIncludeDescriptions = pPRFResourceDirectory_v1->bIncludeDescriptions;
		pPRFResourceDirectory->dwNumOfResources = pPRFResourceDirectory_v1->wNumOfResources;

		for (i=pPRFResourceDirectory->dwNumOfResources-1; i!=0xFFFFFFFF; --i)
			{
			pPRFResourceDirectory->prfResourceMap[i].dwLength =
				pPRFResourceDirectory_v1->prfResourceMap[i].dwLength;
			pPRFResourceDirectory->prfResourceMap[i].dwOffset =
				pPRFResourceDirectory_v1->prfResourceMap[i].dwOffset;
			pPRFResourceDirectory->prfResourceMap[i].dwID =
				pPRFResourceDirectory_v1->prfResourceMap[i].wID;
			pPRFResourceDirectory->prfResourceMap[i].c4Type =
				pPRFResourceDirectory_v1->prfResourceMap[i].c4Type;
			}
		}

	if(bNeedToSwap) {
		SwapPrfResourceDirectory();

		fSymbolTable = new SymbolTable(1);
		
		//fseek(pFile, beginning of symbol table resource);
		//еее Make sure the symbol table exists
		CHAR4 c4ResourceType = MAKEC4ID('S','Y','M','B');
		DWORD dwResourceNumber = GetResourceIndex(c4ResourceType, 1);
		if( dwResourceNumber == -1 )
			DebugString("Symbol Table resource not found.\n");
		fseek(pFile, pPRFResourceDirectory->prfResourceMap[dwResourceNumber].dwOffset, SEEK_SET);
		DWORD dwLengthOfResource = pPRFResourceDirectory->prfResourceMap[dwResourceNumber].dwLength;
		//еее Might want to store length of description???
		DWORD i;
		if (pPRFResourceDirectory->dwIncludeDescriptions) {
			i = fgetc(pFile);
			while (i)
				{
				--dwLengthOfResource;
				i = fgetc(pFile);
				}
			}		
		fSymbolTable->Read(pFile);
		}
	}

void PRF::Open(char *szFilename, char *szMode) {
	strcpy(filename, szFilename);
	pFile = fopen(szFilename, szMode);
	if (pFile)
		GetFileInfo();
	}

DWORD PRF::GetResourceIndex(CHAR4 c4ResourceType, DWORD dwID)
{
	DWORD i = 0;
	while ((i < pPRFResourceDirectory->dwNumOfResources) &&
			((pPRFResourceDirectory->prfResourceMap[i].c4Type != c4ResourceType) ||
			(pPRFResourceDirectory->prfResourceMap[i].dwID != dwID)))
		++i;
	return i;
}

void PRF::Add(char *szResourceFilename, CHAR4 c4ResourceType, DWORD dwID, char *szDescription) {
	DWORD dwResourceNumber;
	PRF_RESOURCEDIRECTORY *pNewPRFResourceDirectory;
	FILE *pFileToBeAdded;
	char buff[128];

	fflush(pFile);
	pFileToBeAdded = fopen(szResourceFilename, "rb");
	if( !pFileToBeAdded ) {
		sprintf(buff, "Error opening file: %s.\n", szResourceFilename);
		DebugString(buff);
		return;
		}

	Remove(c4ResourceType, dwID);
	dwResourceNumber = pPRFResourceDirectory->dwNumOfResources;

	pNewPRFResourceDirectory = (PRF_RESOURCEDIRECTORY*)new BYTE[pPRFFileHeader->dwSizeOfResourceDirectory + sizeof(PRF_RESOURCEMAP)];
	memcpy(pNewPRFResourceDirectory, pPRFResourceDirectory, (int)(pPRFFileHeader->dwSizeOfResourceDirectory));
	delete[] pPRFResourceDirectory;
	pPRFResourceDirectory = pNewPRFResourceDirectory;
	pPRFFileHeader->dwSizeOfResourceDirectory += sizeof(PRF_RESOURCEMAP);
	++pPRFResourceDirectory->dwNumOfResources;
	pPRFResourceDirectory->prfResourceMap[dwResourceNumber].c4Type = c4ResourceType;
	pPRFResourceDirectory->prfResourceMap[dwResourceNumber].dwID = dwID;
	pPRFResourceDirectory->prfResourceMap[dwResourceNumber].dwOffset =
			pPRFResourceDirectory->prfResourceMap[dwResourceNumber-1].dwOffset + 
			pPRFResourceDirectory->prfResourceMap[dwResourceNumber-1].dwLength;
	pPRFResourceDirectory->prfResourceMap[dwResourceNumber].dwLength = 
			OSFileLength(pFileToBeAdded) + 
			((pPRFResourceDirectory->dwIncludeDescriptions) ? 
					(szDescription ? (strlen(szDescription)+1) : 1) : 0);
	pPRFFileHeader->dwResourceDirectoryOffset += pPRFResourceDirectory->prfResourceMap[dwResourceNumber].dwLength;

	// Add resource to file
	fseek(pFile, pPRFResourceDirectory->prfResourceMap[dwResourceNumber].dwOffset, SEEK_SET);
	if (pPRFResourceDirectory->dwIncludeDescriptions) {
		if (szDescription)
			fwrite(szDescription, strlen(szDescription)+1, 1, pFile);
		else
			putc(0, pFile);
		}
	fcopy(pFile, pFileToBeAdded, OSFileLength(pFileToBeAdded));

	// Write resource directory at end of file.
	fwrite(pPRFResourceDirectory, (int)(pPRFFileHeader->dwSizeOfResourceDirectory), 1, pFile);

	// Update file header.
	fseek(pFile, 0, SEEK_SET);
	fwrite(pPRFFileHeader, sizeof(PRF_FILEHEADER), 1, pFile);

	fclose(pFileToBeAdded);
	}

void PRF::Add1(void *pData, size_t dataSize, CHAR4 c4ResourceType, DWORD dwID, char *szDescription) {
	DWORD dwResourceNumber;
	PRF_RESOURCEDIRECTORY *pNewPRFResourceDirectory;

	if (pFile == NULL)
		return;

	fflush(pFile);

	Remove(c4ResourceType, dwID);
	dwResourceNumber = pPRFResourceDirectory->dwNumOfResources;

	pNewPRFResourceDirectory = (PRF_RESOURCEDIRECTORY*)new BYTE[pPRFFileHeader->dwSizeOfResourceDirectory + sizeof(PRF_RESOURCEMAP)];
	memcpy(pNewPRFResourceDirectory, pPRFResourceDirectory, (int)(pPRFFileHeader->dwSizeOfResourceDirectory));
	delete[] pPRFResourceDirectory;
	pPRFResourceDirectory = pNewPRFResourceDirectory;
	pPRFFileHeader->dwSizeOfResourceDirectory += sizeof(PRF_RESOURCEMAP);
	++pPRFResourceDirectory->dwNumOfResources;
	pPRFResourceDirectory->prfResourceMap[dwResourceNumber].c4Type = c4ResourceType;
	pPRFResourceDirectory->prfResourceMap[dwResourceNumber].dwID = dwID;
	pPRFResourceDirectory->prfResourceMap[dwResourceNumber].dwOffset =
			pPRFResourceDirectory->prfResourceMap[dwResourceNumber-1].dwOffset + 
			pPRFResourceDirectory->prfResourceMap[dwResourceNumber-1].dwLength;
	pPRFResourceDirectory->prfResourceMap[dwResourceNumber].dwLength = 
			dataSize + 
			((pPRFResourceDirectory->dwIncludeDescriptions) ? 
					(szDescription ? (strlen(szDescription)+1) : 1) : 0);
	pPRFFileHeader->dwResourceDirectoryOffset += pPRFResourceDirectory->prfResourceMap[dwResourceNumber].dwLength;

	// Add resource to file
	fseek(pFile, pPRFResourceDirectory->prfResourceMap[dwResourceNumber].dwOffset, SEEK_SET);
	if (pPRFResourceDirectory->dwIncludeDescriptions) {
		if (szDescription)
			fwrite(szDescription, strlen(szDescription)+1, 1, pFile);
		else
			putc(0, pFile);
		}
	fwrite(pData, dataSize, 1, pFile);

	// Write resource directory at end of file.
	fwrite(pPRFResourceDirectory, (int)(pPRFFileHeader->dwSizeOfResourceDirectory), 1, pFile);

	// Update file header.
	fseek(pFile, 0, SEEK_SET);
	fwrite(pPRFFileHeader, sizeof(PRF_FILEHEADER), 1, pFile);

	}

void PRF::CompactResourceMap(DWORD &dwResourceNumber, DWORD &dwDeltaOffset) {
	DWORD i;
	for ( i = dwResourceNumber; i < pPRFResourceDirectory->dwNumOfResources; i++ ) {
		pPRFResourceDirectory->prfResourceMap[i] = pPRFResourceDirectory->prfResourceMap[i+1];
		pPRFResourceDirectory->prfResourceMap[i].dwOffset -= dwDeltaOffset;
		}
	}

void PRF::CompactResourceData(DWORD &dwResourceNumber) {
	size_t dataSize=0;
	DWORD i;
	
	for ( i = dwResourceNumber; i < pPRFResourceDirectory->dwNumOfResources-1; i++ ) 
		dataSize += pPRFResourceDirectory->prfResourceMap[i+1].dwLength;
	
	fseek(pFile, pPRFResourceDirectory->prfResourceMap[dwResourceNumber+1].dwOffset, SEEK_SET);
	UniHandle *pHandle = new UniHandle(dataSize);
	void *pData = pHandle->Lock();
	fread((BYTE *)(pData), dataSize, 1, pFile);
	fseek(pFile, pPRFResourceDirectory->prfResourceMap[dwResourceNumber].dwOffset, SEEK_SET);
	fwrite(pData, dataSize, 1, pFile);
	pHandle->Unlock();
	delete pHandle;
	}

void PRF::Remove(CHAR4 c4ResourceType, DWORD dwID) {
	DWORD dwResourceNumber;
	PRF_RESOURCEDIRECTORY *pNewPRFResourceDirectory;
	char buff[128];
	char *c;

	fflush(pFile);
	dwResourceNumber = GetResourceIndex(c4ResourceType, dwID);

	// Update ResourceMap.
	if (dwResourceNumber <= (DWORD)pPRFResourceDirectory->dwNumOfResources-1)	{
		c = (char*)&c4ResourceType;
		sprintf(buff, "Deleteing resource %d type '%c%c%c%c'.\n", dwID, c[0], c[1], c[2], c[3]);
		DebugString(buff);
		CompactResourceData(dwResourceNumber);
	
		DWORD dwDeltaOffset = pPRFResourceDirectory->prfResourceMap[dwResourceNumber].dwLength;
		CompactResourceMap(dwResourceNumber, dwDeltaOffset);

		pPRFFileHeader->dwSizeOfResourceDirectory -= sizeof(PRF_RESOURCEMAP);
		--pPRFResourceDirectory->dwNumOfResources;
		pPRFFileHeader->dwResourceDirectoryOffset -= dwDeltaOffset;

		pNewPRFResourceDirectory = (PRF_RESOURCEDIRECTORY*)new BYTE[pPRFFileHeader->dwSizeOfResourceDirectory];
		memcpy(pNewPRFResourceDirectory, pPRFResourceDirectory, (int)(pPRFFileHeader->dwSizeOfResourceDirectory));
		delete[] pPRFResourceDirectory;
		pPRFResourceDirectory = pNewPRFResourceDirectory;
		
		// Write resource directory at end of file.
		fwrite(pPRFResourceDirectory, (int)(pPRFFileHeader->dwSizeOfResourceDirectory), 1, pFile);

		// Update file header.
		fseek(pFile, 0, SEEK_SET);
		fwrite(pPRFFileHeader, sizeof(PRF_FILEHEADER), 1, pFile);
		}
	}

void PRF::Load(CHAR4 c4ResourceType, DWORD dwID, UniHandle **handle, int *numOfBytes)
	{
	FILE *tmpFile;
	int start;
	int length;
	BYTE *data;

	tmpFile = Load(c4ResourceType, dwID, &start, &length);
	if (tmpFile)
		{
		// Resource found, load it.
		*numOfBytes = length;
		*handle = new UniHandle(length);
		data = (BYTE *)(*handle)->Lock();
		fseek(tmpFile, start, SEEK_SET);
		fread(data, length, 1, pFile);
		(*handle)->Unlock();
		//if(fSymbolTable)
		//	fSymbolTable->SwapResource((Ptr)data, c4ResourceType);
		}
	else
		{
		// Resource not found.
		*handle = NULL;
		}

	}

FILE *PRF::Load(CHAR4 c4ResourceType, DWORD dwID, int *offset, int *numOfBytes)
	{
	DWORD dwResourceNumber;
	DWORD i;
	DWORD dwLengthOfResource;
	
	if (pFile == NULL)
		return NULL;
	
	dwResourceNumber = GetResourceIndex(c4ResourceType, dwID);
	if (dwResourceNumber < pPRFResourceDirectory->dwNumOfResources)
		{
		// Resource found
		*offset = pPRFResourceDirectory->prfResourceMap[dwResourceNumber].dwOffset;
		dwLengthOfResource = pPRFResourceDirectory->prfResourceMap[dwResourceNumber].dwLength;
		fseek(pFile, *offset, SEEK_SET);
		if (pPRFResourceDirectory->dwIncludeDescriptions)
			{
			i = fgetc(pFile);
			if (i == -1)
				return NULL;
			while (i)
				{
				--dwLengthOfResource;
				*offset = *offset + 1;
				i = fgetc(pFile);
				if (i == -1)
					return NULL;
				}
			*offset = *offset + 1;
			}
		*numOfBytes = dwLengthOfResource;
		
		return pFile;
		}
	else
		return NULL;
	}

void PRF::SwapPrfFileHeader() {
	SwapBytes(&pPRFFileHeader->dwByteOrder);
	SwapBytes(&pPRFFileHeader->dwVersion);
	SwapBytes(&pPRFFileHeader->dwFileSize);
	SwapBytes(&pPRFFileHeader->dwSizeOfResourceDirectory);
	SwapBytes(&pPRFFileHeader->dwResourceDirectoryOffset);
	}

void PRF::SwapPrfResourceDirectory() {
	SwapBytes(&pPRFResourceDirectory->dwNumOfResources);
	SwapBytes(&pPRFResourceDirectory->dwIncludeDescriptions);
	
	DWORD i;
	
	for( i = 0; i<pPRFResourceDirectory->dwNumOfResources; i++ )	{
		SwapBytes(&pPRFResourceDirectory->prfResourceMap[i].dwID);
		SwapBytes(&pPRFResourceDirectory->prfResourceMap[i].dwOffset);
		SwapBytes(&pPRFResourceDirectory->prfResourceMap[i].dwLength);
		}
	}

BOOL PRF::HasResource(CHAR4 c4ResourceType, DWORD dwID)
	{
	DWORD dwResourceNumber;

	dwResourceNumber = GetResourceIndex(c4ResourceType, dwID);
	if (dwResourceNumber < pPRFResourceDirectory->dwNumOfResources)
		{
		// Resource found
		return TRUE;
		}
	else
		{
		return FALSE;
		}
	}

CHAR4 prfTypes[] =
	{
	MAKEC4ID('W', 'A', 'V', 'E'),
	MAKEC4ID('P', 'A', 'L', ' '),
	MAKEC4ID('S', 'Y', 'N', 'C'),
	MAKEC4ID('F', 'O', 'N', 'T'),
	MAKEC4ID('P', 'L', 'Y', 'R'),
	MAKEC4ID('C', 'M', 'P', ' '),
	MAKEC4ID('S', 'O', 'N', 'G'),
	MAKEC4ID('T', 'E', 'X', ' '),
	MAKEC4ID('C', 'P', 'L', 'Y'),
	MAKEC4ID('\0', '\0', '\0', '\0'),
	};

char *prfExtensions[] =
	{
	"WAV",
	"PAL",
	"RLS",
	"FNT",
	"PLY",
	"CMP",
	"SNG",
	"TEX",
	"CPL",
	0,
	};

void PRF::OutputResources()
	{
	DWORD i;
	PRF_RESOURCEMAP *resMap;
	int j, k;
	char outName[128];
	UniHandle *handle;
	int numBytes;
	FILE *outFile;
	WORD *pw;

	for (i=0; i<pPRFResourceDirectory->dwNumOfResources; ++i)
		{
		resMap = &pPRFResourceDirectory->prfResourceMap[i];
		j = 0;
		while (prfExtensions[j] && (resMap->c4Type != prfTypes[j]))
			++j;
		if (prfExtensions[j])
			{
			// Type extension found, create file.
			sprintf(outName, "%d.%s", resMap->dwID, prfExtensions[j]);
			outFile = fopen(outName, "wb");
			if (outFile)
				{
				DebugString(outName);
				DebugString("\n");
				Load(resMap->c4Type, resMap->dwID, &handle, &numBytes);

				// If its a sync, swap the bytes back to BigEndian format.
				if (resMap->c4Type == MAKEC4ID('S', 'Y', 'N', 'C'))
					{
					pw = (WORD *)handle->Lock();
					for (k=0; k<numBytes; k+=2)
						{
						SwapBytes(pw);
						++pw;
						}
					handle->Unlock();
					}

				fwrite(handle->Lock(), 1, numBytes, outFile);
				delete handle;
				fclose(outFile);
				}
			}
		}
	}

char *PRF::GetFileName()
{
	return filename;
}
