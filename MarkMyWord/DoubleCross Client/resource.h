#ifndef _resource
#define _resource

#include "object.h"

class UniHandle;
class ResourceManager;

typedef enum
	{
	RES_NONE = 0,
	RES_SOUND,
	RES_PALETTE,
	RES_SYNC,
	RES_FONT,
	RES_PLAYER,
	RES_CMP,
	RES_SONG,
	RES_TSPRITE,
	RES_CASINO_PLAYER,
	RES_NUM_OF_TYPES,
	} RESOURCE_TYPES;

class Resource : public Object
	{
	public:
		Resource(RESOURCE_TYPES newType, int newResourceID,
			ResourceManager *newResourceManager, BOOL loadNow=TRUE);
		~Resource();
		int GetSize();
		virtual BYTE *GetData();
		void *Lock();
		void Unlock();

		RESOURCE_TYPES type;
		int resourceID;
		FILE *file;
		int offset;
		int length;

	protected:
		void ReadPRF();
		void ReadFile();
		void ReadPRFNoLoad();
		void ReadFileNoLoad();

		int numOfBytes;
		ResourceManager *resourceManager;
		BOOL canCloseFile;
		UniHandle *handle;

	private:
		BYTE *data;

	};

#endif	// _resource
