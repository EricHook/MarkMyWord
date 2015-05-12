#ifndef _unihand
#define _unihand

#include "object.h"

class UniHandle : public Object {
	public:
		UniHandle(DWORD dwNewSize);
		~UniHandle();
		void *Lock(void);
		void *LongLock(void);
		void Unlock(void);
		DWORD Size();
		HANDLE SetSize(DWORD dwNewSize);

	private:
		void *pData;
		int iLockCount;
		HANDLE hData;
		DWORD dwSize;
	};

#endif
