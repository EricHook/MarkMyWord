#ifndef _prf
#define _prf

#include <stdio.h>
#include "object.h"
class SymbolTable;

#ifdef OS_LITTLE_ENDIAN
	#define MAKEC4ID(c1, c2, c3, c4) \
		(long)((c4<<24) + (c3<<16) + (c2<<8) + c1)
#else
	#define MAKEC4ID(c1, c2, c3, c4) \
		(long)((c1<<24) + (c2<<16) + (c3<<8) + c4)
#endif // OS_LITTLE_ENDIAN


// Portable Resource File info.

/*
File Format:

PRF_FILEHEADER
RESOURCES
PRF_RESOURCEDIRECTORY
*/

// All offsets from beginning of file.

typedef long CHAR4;

typedef struct {
	WORD 	wByteOrder;	
	CHAR4	c4ID;		// 'P', 'R', 'F', 0x00
	DWORD	dwVersion;
	DWORD	dwFileSize;
	DWORD	dwSizeOfResourceDirectory;
	DWORD	dwResourceDirectoryOffset;
	} PRF_FILEHEADER_V1;

typedef struct {
	CHAR4	c4Type;
	WORD	wID;
	DWORD	dwOffset;
	DWORD	dwLength;		// Including optional description.
	} PRF_RESOURCEMAP_V1;

typedef struct {
	WORD				wNumOfResources;
	WORD				bIncludeDescriptions;
	PRF_RESOURCEMAP_V1	prfResourceMap[1];
	} PRF_RESOURCEDIRECTORY_V1;

typedef struct
	{
	DWORD 	dwByteOrder;	
	CHAR4	c4ID;		// 'P', 'R', 'F', 0x00
	DWORD	dwVersion;
	DWORD	dwFileSize;
	DWORD	dwSizeOfResourceDirectory;
	DWORD	dwResourceDirectoryOffset;
	} PRF_FILEHEADER;

typedef struct
	{
	CHAR4	c4Type;
	DWORD	dwID;
	DWORD	dwOffset;
	DWORD	dwLength;		// Including optional description.
	} PRF_RESOURCEMAP;

typedef struct
	{
	DWORD			dwNumOfResources;
	DWORD			dwIncludeDescriptions;
	PRF_RESOURCEMAP	prfResourceMap[1];
	} PRF_RESOURCEDIRECTORY;

class UniHandle;

class PRF : public Object
	{
	public:
		PRF();
		~PRF();
		void New(char *szFilename);
		void Open(char *szFilename, char *szMode);
		void Add(char *szResourceFilename, CHAR4 c4ResourceType, DWORD dwId, char *szDescription);
		void Add1(void *pData, size_t dataSize, CHAR4 c4ResourceType, DWORD dwId, char *szDescription);
		void Remove(CHAR4 c4ResourceType, DWORD dwID);
		void Load(CHAR4 c4ResourceType, DWORD dwId, UniHandle **data, int *numOfBytes);
		FILE *Load(CHAR4 c4ResourceType, DWORD dwID, int *offset, int *numOfBytes);
		WORD NeedToSwapBytes();
		BOOL HasResource(CHAR4 c4ResourceType, DWORD dwID);
		void OutputResources();
		char *GetFileName();

	private:
		void GetFileInfo();
		void WriteHeaders();
		DWORD GetResourceIndex(CHAR4 c4ResourceType, DWORD dwID);
		void CompactResourceData(DWORD &dwResourceNumber);
		void CompactResourceMap(DWORD &dwResourceNumber, DWORD &dwDeltaOffset);
		void SwapPrfFileHeader();
		void SwapPrfResourceDirectory();

		FILE *pFile;
		PRF_FILEHEADER *pPRFFileHeader;
		PRF_RESOURCEDIRECTORY *pPRFResourceDirectory;

  		WORD fNeedToSwapBytes;
  		SymbolTable *fSymbolTable;
		char filename[256];
		};

inline WORD PRF::NeedToSwapBytes()
{
	return fNeedToSwapBytes;
}

DWORD filelength(FILE *pFile);


#endif // _prf
