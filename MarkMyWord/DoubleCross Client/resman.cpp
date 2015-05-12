#include "os.h"
#include "core.h"
#include "sci.h"

#include "Client Include.h"

extern Application *pApp;
extern CHAR4 c4ResourceTypes[];
extern char cdDriveLetter;
extern int numBytesAllocated;

void MacSwapBytes(WORD *wData);
void MacSwapBytes(DWORD *dwData);

void SetCurrentDirectory()
	{
	}

#if defined(OS_WINDOWS)
#define PATH_SEPARATOR '\\'
#endif

#if defined(OS_MAC)
#define PATH_SEPARATOR ':'
#endif

#if defined(OS_BE)
#define PATH_SEPARATOR '/'
#endif

ResourceManager::ResourceManager(int prfID)
	{
	name = NULL;
	if (prfID != 0)
		{
		name = new char[20];
		sprintf(name, "%d", prfID);
		}
	
	UpdatePRFList();
	
	pApp->listOfResourceManagers->AddToEnd(this);
	}    

ResourceManager::~ResourceManager()
	{
	ResourceNode *pNode;
	PRF *pPRF;

	if (name)
		delete[] name;
	pApp->listOfResourceManagers->Remove(this);
	pNode = (ResourceNode*)(listOfResources.GetFirst());
	while (pNode)
		{
		listOfResources.Remove(pNode);
		delete pNode->resource;
		delete pNode;
		pNode = (ResourceNode*)(listOfResources.GetFirst());
		}
	pPRF = (PRF*)(listOfPRFs.GetFirst());
	while (pPRF)
		{
		listOfPRFs.Remove(pPRF);
		delete pPRF;
		pPRF = (PRF*)(listOfPRFs.GetFirst());
		}
	}

Resource *ResourceManager::LoadResource(RESOURCE_TYPES newType, int newResourceID)
	{
	ResourceNode *pNode = NULL;
#ifdef SHOW_RESOURCE_LOADS
	char prompt[64];
#endif // SHOW_RESOURCE_LOADS
	DWORD *dwp;
	WORD *wp;
	int i;

	// Keep the sound alive.
	if (pApp && pApp->pSounder)
		pApp->pSounder->Doit();

	// Try to locate previously loaded resource.
	pNode = FindNode(newType, newResourceID);

	if (pNode == NULL)
		{
		// Load a new resource.
		pNode = new ResourceNode;
		switch (newType)
			{
			default:
#ifdef SHOW_RESOURCE_LOADS
				wsprintf(prompt, "Loading Resource %d.\n", newResourceID);
				DebugString(prompt);
#endif // SHOW_RESOURCE_LOADS
				pNode->resource = new Resource(newType, newResourceID, this);

				// Byte swapping broken, do it manually.
				switch (newType)
					{
					case RES_PLAYER:
						dwp = (DWORD *)pNode->resource->GetData();
						if (dwp)
							{
							for (i = 0; i < (sizeof(BOARD_PLAYER)-2) / 4; ++i)
								{
								MacSwapBytes(dwp);
								++dwp;
								}
							}
						break;
					case RES_CASINO_PLAYER:
						dwp = (DWORD *)pNode->resource->GetData();
						if (dwp)
							{
							for (i = 0; i < (sizeof(CASINO_PLAYER)-2) / 4; ++i)
								{
								MacSwapBytes(dwp);
								++dwp;
								}
							}
						break;
					case RES_SYNC:
					case RES_FONT:
						wp = (WORD *)pNode->resource->GetData();
						if (wp)
							{
							for (i = 0; i < pNode->resource->GetSize()/2; ++i)
								{
								MacSwapBytes(wp);
								++wp;
								}
							}
						break;
					default:
						break;
					}
				break;
			case RES_SOUND:
#ifdef SHOW_RESOURCE_LOADS
				wsprintf(prompt, "Loading Sound %d.\n", newResourceID);
				DebugString(prompt);
#endif // SHOW_RESOURCE_LOADS
				// Load a compressed sound first.
				// pNode->resource = new SoundC(newResourceID, this);
				// if (!pNode->resource->GetData())
					{
					// No compressed sound, load an uncompressed one.
					// delete pNode->resource;
					pNode->resource = new Sound(newResourceID, this);
					}
				break;
			case RES_SONG:
#ifdef SHOW_RESOURCE_LOADS
				wsprintf(prompt, "Loading Sound %d.\n", newResourceID);
				DebugString(prompt);
#endif // SHOW_RESOURCE_LOADS
				// Load a compressed sound first.
				pNode->resource = new sciSong(newResourceID, this);
				break;
			case RES_TSPRITE:
#ifdef SHOW_RESOURCE_LOADS
				wsprintf(prompt, "Loading TEX %d.\n", newResourceID);
				DebugString(prompt);
#endif // SHOW_RESOURCE_LOADS
				pNode->resource = new TSprite(RES_TSPRITE, newResourceID, this);
				break;
			}
		if (pNode->resource)
			{
			pNode->usageCount = 1;
			listOfResources.AddToEnd(pNode);
			return pNode->resource;
			}
		else {
#ifdef SHOW_RESOURCE_LOADS
			wsprintf(prompt, "Cannot find %d of resource type %d.", newResourceID, newType);
			MessageBox(NULL, prompt, "Error", MB_OK);
#endif // SHOW_RESOURCE_LOADS
			if (pNode->resource)
				delete pNode->resource;
			delete pNode;
			return NULL;
			}
		}
	else {
		// Increment usage count.
		++pNode->usageCount;
		return pNode->resource;
		}
	}

void ResourceManager::FreeResource(Resource *resource)
	{
	ResourceNode *pNode;

	// Try to locate previously loaded resource.
	pNode = FindNode(resource);
	if (pNode)
		{
		if (pNode->usageCount > 0)
			--pNode->usageCount;
		}
	else
		{
		DebugString("Freeing an invalid resource!\n");
		}
	}

void ResourceManager::FreeResource(RESOURCE_TYPES newType, int newResourceID)
	{
	ResourceNode *pNode;

	// Try to locate previously loaded resource.
	pNode = FindNode(newType, newResourceID);
	if (pNode)
		{
		if (pNode->usageCount > 0)
			--pNode->usageCount;
		}
	else
		{
		DebugString("Freeing an invalid resource!\n");
		}
	}

void ResourceManager::PurgeResource(Resource *resource)
	{
	ResourceNode *pNode;

	pNode = FindNode(resource);
	if (pNode)
		{
		listOfResources.Remove(pNode);
		delete pNode->resource;
		delete pNode;
		}
	}

void ResourceManager::PurgeResource(RESOURCE_TYPES newType, int newResourceID)
	{
	ResourceNode *pNode;

	pNode = FindNode(newType, newResourceID);
	if (pNode)
		{
		listOfResources.Remove(pNode);
		delete pNode->resource;
		delete pNode;
		}
	}

void ResourceManager::PurgeResources(int bytesCanUse)
	{
	ResourceNode *pNode;

	pNode = (ResourceNode*)(listOfResources.GetFirst());
	while ((numBytesAllocated > bytesCanUse) && pNode)
		{
		if (pNode->usageCount == 0)
			{
			// Don't put out a message when purging *all* resources - too many messages.
			if (bytesCanUse)
				DebugString("Purging a resource.\n");
			listOfResources.Remove(pNode);
			delete pNode->resource;
			delete pNode;
			pNode = (ResourceNode*)(listOfResources.GetFirst());
			}
		else
			{
			pNode = (ResourceNode*)(listOfResources.GetNext());
			}
		}
	}

ResourceNode *ResourceManager::FindNode(RESOURCE_TYPES newType, int newResourceID)
	{
	ResourceNode *pNode;

	pNode = (ResourceNode*)(listOfResources.GetFirst());
	while (pNode && (pNode->resource->type != newType || pNode->resource->resourceID != newResourceID))
		pNode = (ResourceNode*)(listOfResources.GetNext());
	return pNode;
	}

ResourceNode *ResourceManager::FindNode(Resource *resource)
	{
	ResourceNode *pNode;

	pNode = (ResourceNode*)(listOfResources.GetFirst());
	while (pNode && (pNode->resource != resource))
		pNode = (ResourceNode*)(listOfResources.GetNext());
	return pNode;
	}

void ResourceManager::PreloadResource(RESOURCE_TYPES newType, int newResourceID)
	{
	FreeResource(LoadResource(newType, newResourceID));
	}

char *ResourceManager::GetName()
	{
	return name;
	}

/*
void ResourceManager::AddPrfs(char *prfName)
	{
	PRF *pPRF;
	char str[64];
	int i;
	FILE *file;

	i = 0;
	while (resourcePaths[i][0])
		{
		if (strcmp(resourcePaths[i], "."))
			{
			strcpy(str, resourcePaths[i]);
			strcat(str, PATH_SEPARATOR);
			}
		else
			{
			strcpy(str, "");
			}
		strcat(str, prfName);

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
	// Try to find the .PRFs on a CD if a CD was located.
	if (cdDriveLetter)
		{
		str[0] = cdDriveLetter;
		str[1] = 0;
		strcat(str, ":\\");
		strcat(str, prfName);

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
		}
	}
*/
