#include <memory.h>

#include "os.h"
#include "globals.h"

#include "resource.h"
#include "resman.h"
#include "util.h"
#include "app.h"
#include "unihand.h"
#include "prf.h"
#include "window.h"

#include "Client Include.h"

extern Application *pApp;
extern char cdDriveLetter;
extern BOOL outputResources;
extern BOOL listResources;
extern PRF *outputPRF;
extern BOOL hdVersion;

#if defined(OS_WINDOWS)
char pathChar = '\\';
#define PATH_SEPARATOR '\\'
#endif
#if defined(OS_MAC)
char pathChar = ':';
#define PATH_SEPARATOR ':'
#endif
#if defined(OS_BE)
char pathChar = '/';
#define PATH_SEPARATOR '/'
#endif

CHAR4 c4ResourceTypes[] =
	{
	MAKEC4ID('\0', '\0', '\0', '\0'),
	MAKEC4ID('W', 'A', 'V', 'E'),
	MAKEC4ID('P', 'A', 'L', ' '),
	MAKEC4ID('S', 'Y', 'N', 'C'),
	MAKEC4ID('F', 'O', 'N', 'T'),
	MAKEC4ID('P', 'L', 'Y', 'R'),
	MAKEC4ID('C', 'M', 'P', ' '),
	MAKEC4ID('S', 'O', 'N', 'G'),
	MAKEC4ID('T', 'E', 'X', ' '),
	MAKEC4ID('C', 'P', 'L', 'Y'),
	};

char *resourceExtensions[] =
	{
	0,
	"WAV",
	"PAL",
	"RLS",
	"FNT",
	"PLY",
	"CMP",
	"SNG",
	"TEX",
	"CPL",
	};

Boolean ResourceManager::ResourceIsAvailable(RESOURCE_TYPES type, int resourceID)
	{
	char fileName[MAX_PATH];
	int i;
	FILE *tmpFile;
	PRF *pPRF;
	static int foundResources[1000], numFoundResources = 0;
	static RESOURCE_TYPES foundResourceTypes[1000];
	
	// optimization: since in fact we are only checking and rechecking a very small number
	// of resource numbers, check if it's in the cache of found resources already
	
	for (i = 0 ; i < numFoundResources ; i++)
		if (foundResources[i] == resourceID && foundResourceTypes[i] == type)
			return TRUE;
	
	// find the resouce in the list of resources
	
	for (i = 0 ; i < numResourceMappings ; i++)
		if (resourceMappings[i].resourceID == resourceID)
			if (!stricmp(resourceMappings[i].type, resourceExtensions[type]))
				break; // found the file
	
	if (i == numResourceMappings) {
		// it wasn't in a free file; maybe it's in a PRF
		
		// but first, let's make sure the PRF list is up to date (background PRF downloads
		// may have finished)
		
		UpdatePRFList();
		
		for (pPRF = (PRF *)listOfPRFs.GetFirst() ; pPRF ; pPRF = (PRF *)listOfPRFs.GetNext())
			if (pPRF->HasResource(c4ResourceTypes[type], resourceID)) {
				if (numFoundResources < 1000) {
					foundResources[numFoundResources] = resourceID;
					foundResourceTypes[numFoundResources] = type;
					numFoundResources++;
				}
				
				return TRUE;
			}
		
		return FALSE;
	}
	
	sprintf(fileName, "%s%s%c%d.%s",
					  wonParams.wonPath,
					  resourceDirectories[resourceMappings[i].directory],
					  pathChar,
					  resourceMappings[i].resourceIDToUse,
					  resourceMappings[i].type);
	tmpFile = fopen(fileName, "rb");
	
	if (tmpFile) {
		fclose(tmpFile);
		
		if (numFoundResources < 1000) {
			foundResources[numFoundResources] = resourceID;
			foundResourceTypes[numFoundResources] = type;
			numFoundResources++;
		}
		
		return TRUE;
	}
	
	return FALSE;
}

void ResourceManager::UpdatePRFList()
	{
	char path[MAX_PATH];
	int i, j;
	FILE *file;
	PRF *pPRF;
	
	for (i = 0 ; i < numResourceMappings ; i++)
		{
		if (stricmp(resourceMappings[i].type, "PRF"))
			continue;
		
		if (strchr(resourceDirectories[resourceMappings[i].directory], ':'))
			sprintf(path, "%s%c%d.%s",
						  resourceDirectories[resourceMappings[i].directory],
						  PATH_SEPARATOR,
						  resourceMappings[i].resourceIDToUse,
						  resourceMappings[i].type);
		else
			sprintf(path, "%s%s%c%d.%s",
						  wonParams.wonPath,
						  resourceDirectories[resourceMappings[i].directory],
						  PATH_SEPARATOR,
						  resourceMappings[i].resourceIDToUse,
						  resourceMappings[i].type);
		
		// check if this PRF file is already loaded
		
		for (j = 0 ; ; j++) {
			pPRF = (PRF *)listOfPRFs.GetItem(j);
			if (pPRF == NULL)
				break;
			if (!stricmp(path, pPRF->GetFileName()))
				break;
		}
		
		if (pPRF != NULL)
			continue; // this PRF is already loaded
		
		// check that the file exists
		
		file = fopen(path, "rb");
		if (file)
			{
			// it exists, so open it and add it to the list
			fclose(file);
			pPRF = new PRF();
			pPRF->Open(path, "rb");
			listOfPRFs.AddToFront(pPRF);
			}
		else
			; // assert(FALSE); // allow dynamic PRFs
		/*
		// Try to find the .PRFs on a CD if a CD was located.
		if (cdDriveLetter)
			{
			i = 0;
			while (prfFilenames[i][0])
				{
				str[0] = cdDriveLetter;
				str[1] = 0;
				strcat(str, ":\\");
				strcat(str, prfFilenames[i]);

				// Check that the file exists
				file = fopen(str, "rb");
				if (file)
					{
					// It exists, so open it and add it to the list.
					fclose(file);
					pPRF = new PRF();
					pPRF->Open(str, "rb");
					listOfPRFs.AddToFront(pPRF);
					}
				++i;
				}
			}
		*/
		}
	}    

///////////////////////////////////////////////////////////////////////////////////////////

Resource::Resource(RESOURCE_TYPES newType, int newResourceID, ResourceManager *newResourceManager, BOOL loadNow)
	{
	type = newType;
	resourceID = newResourceID;
	numOfBytes = 0;
	data = NULL;
	handle = NULL;
	file = NULL;
	offset = 0;
	length = 0;
	canCloseFile = FALSE;
	if (newResourceManager)
		resourceManager = newResourceManager;
	else
		resourceManager = pApp->pResourceManager;

	if (resourceID == 0)	// 0 is an invalid resource number.
		return;

	if (loadNow)
		{
//#ifdef _DEBUG
		ReadFile();
		if (data == NULL)
//#endif // _DEBUG
			ReadPRF();
		}
	else
		{
//#ifdef _DEBUG
		ReadFileNoLoad();
		if (file == NULL)
//#endif // _DEBUG
			ReadPRFNoLoad();
		}

	if (handle)
		{
		data = PBYTE(handle->Lock());
		}
	}

Resource::~Resource()
	{
	delete handle;
	if (file && canCloseFile)
		fclose(file);
	}

void Resource::ReadPRF()
	{
	PRF *pPRF;

	numOfBytes = 0;
	pPRF = (PRF*)(resourceManager->listOfPRFs.GetFirst());
	while (pPRF)
		{
		pPRF->Load(c4ResourceTypes[type], resourceID, &handle, &numOfBytes);
		if (handle)
			{
			// Resource found. Terminate while() loop.
			pPRF = NULL;

#ifdef _DEBUG
			if (outputResources)
				{
				/*
				if (!outputPRF->HasResource(c4ResourceTypes[type], resourceID))
					outputPRF->Add1(data, numOfBytes,
						c4ResourceTypes[type], resourceID, "");
				*/
				}
			if (listResources)
				{
				char str[MAX_PATH];
				switch (type)
					{
					case RES_TSPRITE:
						sprintf(str, "file 'TEX ' (%d) \"..\\resource\\%d.TEX\"\n", resourceID, resourceID);
						break;
					case RES_SOUND:
						sprintf(str, "file 'WAVE' (%d) \"..\\resource\\%d.WAV\"\n", resourceID, resourceID);
						break;
					default:
						sprintf(str, "unknown resource type\n");
						break;
					}
				DebugString(str);
				}
#endif
			}
		else
			{
			// Resource not found. Try another PRF.
			pPRF = (PRF*)(resourceManager->listOfPRFs.GetNext());
			}
		}
	}

DWORD fsize(FILE *file)
	{
	long oldPosition;
	long newPosition;

	oldPosition = ftell(file);
	fseek(file, 0, SEEK_END);
	newPosition = ftell(file);
	fseek(file, oldPosition, SEEK_SET);
	return newPosition;
	}

void Resource::ReadFile()
	{
	char fileName[MAX_PATH];
	int i;
	FILE *tmpFile;
	
	// find the resouce in the list of resources
	
	for (i = 0 ; i < numResourceMappings ; i++)
		if (resourceMappings[i].resourceID == resourceID)
			if (!stricmp(resourceMappings[i].type, resourceExtensions[type]))
				break; // found the file
	
	if (i == numResourceMappings)
		{ /* DebugString("Resource not found.\n"); */ return; }
	
	sprintf(fileName, "%s%s%c%d.%s",
					  wonParams.wonPath,
					  resourceDirectories[resourceMappings[i].directory],
					  pathChar,
					  resourceMappings[i].resourceIDToUse,
					  resourceMappings[i].type);
	tmpFile = fopen(fileName, "rb");
	
	/*
	i = 0;
	tmpFile = NULL;
	while (resourcePaths[i][0] && !tmpFile)
		{
		if (resourceManager && resourceManager->GetName())
			sprintf(filename, "%s%c%s%c%d.%s", resourcePaths[i], pathChar, resourceManager->GetName(), 
				pathChar, resourceID, resourceExtensions[type]);
		else
			sprintf(filename, "%s%c%d.%s", resourcePaths[i], pathChar,
				resourceID, resourceExtensions[type]);
		tmpFile = fopen(filename, "rb");
		if (!hdVersion && !tmpFile)
			{
			// CD version, check optional files.
			if (resourceManager && resourceManager->GetName())
				sprintf(filename, "%s%c%s%coptional%c%d.%s", resourcePaths[i], pathChar, resourceManager->GetName(), 
					pathChar, pathChar, resourceID, resourceExtensions[type]);
			else
				sprintf(filename, "%s%coptional%c%d.%s", resourcePaths[i], pathChar,
					pathChar, resourceID, resourceExtensions[type]);
			tmpFile = fopen(filename, "rb");
			}
		++i;
		}
	*/
	// Don't search the CD for single files.
	/*
	i = 0;
	if (cdDriveLetter)
		{
		while (!file && resourcePaths[i][0])
			{
			sprintf(filename, "%c:\\%s\\%d.%s", cdDriveLetter, resourcePaths[i], 
				resourceID, resourceExtensions[type]);
			file = fopen(filename, "rb");
			++i;
			}
		}
	*/
	if (tmpFile)
		{
		numOfBytes = fsize(tmpFile);
		handle = new UniHandle(numOfBytes);
		data = (BYTE *)handle->Lock();
		fread(data, numOfBytes, 1, tmpFile);
		fclose(tmpFile);

		if (outputResources)
			{
			char path[MAX_PATH];
			
			strcpy(path, "prfout");
			strcat(path, strrchr(fileName, '\\'));
			CopyFile(fileName, path, TRUE);
			/*
			if (!outputPRF->HasResource(c4ResourceTypes[type], resourceID))
				outputPRF->Add1(data, numOfBytes,
					c4ResourceTypes[type], resourceID, "");
			*/
			}
		if (listResources)
			{
			char str[MAX_PATH];
			switch (type)
				{
				case RES_TSPRITE:
					sprintf(str, "file 'TEX ' (%d) \"..\\resource\\%d.TEX\"\n", resourceID, resourceID);
					break;
				case RES_SOUND:
					sprintf(str, "file 'WAVE' (%d) \"..\\resource\\%d.WAV\"\n", resourceID, resourceID);
					break;
				default:
					sprintf(str, "unknown resource type\n");
					break;
				}
			DebugString(str);
			}
		}
	}

int Resource::GetSize()
	{
	return numOfBytes;
	}
	
BYTE *Resource::GetData()
	{
	// Don't allow null pointers.
	if (this == NULL)
		{
#ifdef _DEBUG
		DebugString("Locking a NULL resource!\n");
#endif // _DEBUG
		return NULL;
		}
	return data;
	}

void Resource::ReadPRFNoLoad()
	{
	PRF *pPRF;

	numOfBytes = 0;
	pPRF = (PRF*)(resourceManager->listOfPRFs.GetFirst());
	canCloseFile = FALSE;
	while (pPRF)
		{
		file = pPRF->Load(c4ResourceTypes[type], resourceID, &offset, &length);
		if (file)
			{
			// Resource found. Terminate while() loop.
			pPRF = NULL;
			}
		else
			{
			// Resource not found. Try another PRF.
			file = NULL;
			pPRF = (PRF*)(resourceManager->listOfPRFs.GetNext());
			}
		}
	}

void Resource::ReadFileNoLoad()
	{
	char fileName[MAX_PATH];
	int i;

	file = NULL;
	
	// find the resouce in the list of resources
	
	for (i = 0 ; i < numResourceMappings ; i++)
		if (resourceMappings[i].resourceID == resourceID)
			if (!stricmp(resourceMappings[i].type, resourceExtensions[type]))
				break; // found the file
	
	if (i == numResourceMappings)
		{ /* DebugString("Resource not found.\n"); */ return; }
	
	sprintf(fileName, "%s%s%c%d.%s",
					  wonParams.wonPath,
					  resourceDirectories[resourceMappings[i].directory],
					  pathChar,
					  resourceMappings[i].resourceIDToUse,
					  resourceMappings[i].type);
	file = fopen(fileName, "rb");
	
	/*
	i = 0;
	file = NULL;
	while (resourcePaths[i][0] && !file)
		{
		if (resourceManager && resourceManager->GetName())
			sprintf(filename, "%s\\%s\\%d.%s", resourcePaths[i], resourceManager->GetName(), 
				resourceID, resourceExtensions[type]);
		else
			sprintf(filename, "%s\\%d.%s", resourcePaths[i], 
				resourceID, resourceExtensions[type]);
		file = fopen(filename, "rb");
		if (!hdVersion && !file)
			{
			if (resourceManager && resourceManager->GetName())
				sprintf(filename, "%s\\%s\\optional\\%d.%s", resourcePaths[i], resourceManager->GetName(), 
					resourceID, resourceExtensions[type]);
			else
				sprintf(filename, "%s\\optional\\%d.%s", resourcePaths[i], 
					resourceID, resourceExtensions[type]);
			file = fopen(filename, "rb");
			}
		++i;
		}
	*/
	
	// Don't search the CD for single files.
	/*
	i = 0;
	if (cdDriveLetter)
		{
		while (!file && resourcePaths[i][0])
			{
			sprintf(filename, "%c:\\%s\\%d.%s", cdDriveLetter, resourcePaths[i], 
				resourceID, resourceExtensions[type]);
			file = fopen(filename, "rb");
			++i;
			}
		}
	*/
	if (file)
		{
		numOfBytes = fsize(file);
		offset = 0;
		length = numOfBytes;
		canCloseFile = TRUE;
		}
	}

void *Resource::Lock()
	{
	return handle->Lock();
	}

void Resource::Unlock()
	{
	handle->Unlock();
	}
