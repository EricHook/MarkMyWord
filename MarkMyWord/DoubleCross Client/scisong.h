#ifndef _scisong
#define _scisong

#include "resource.h"

class sciSong : public Resource
	{
	public:
		sciSong(int iResourceID, ResourceManager *resourceManager = NULL);
		~sciSong();
		void Play();

	private:
	};

#endif // _scisong
