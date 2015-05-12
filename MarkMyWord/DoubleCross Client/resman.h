#ifndef _resourcemanager
#define _resourcemanager

#include "resource.h"
#include "list.h"

class List;
class PRF;

class ResourceNode : public Object
	{
	public:
		Resource *resource;
		int usageCount;
	};

class ResourceManager : public Object
	{
	public:
		ResourceManager(int prfID = 0);
		~ResourceManager();
		// void AddPrfs(char *prfName = NULL);
		void UpdatePRFList();
		Boolean ResourceIsAvailable(RESOURCE_TYPES type, int resourceID);
		Resource *LoadResource(RESOURCE_TYPES newType, int newResourceID);
		void FreeResource(Resource *resource);
		void FreeResource(RESOURCE_TYPES newType, int newResourceID);
		void PurgeResource(Resource *resource);
		void PurgeResource(RESOURCE_TYPES newType, int newResourceID);
		void PurgeResources(int bytesCanUse);
		void PreloadResource(RESOURCE_TYPES newType, int newResourceID);
		char *GetName();

		List listOfPRFs;

	private:
		ResourceNode *FindNode(RESOURCE_TYPES newType, int newResourceID);
		ResourceNode *FindNode(Resource *resource);

		List listOfResources;
		char *name;
	};

#endif // _resourcemanager
