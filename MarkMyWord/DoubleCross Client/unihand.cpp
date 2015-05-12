#include "os.h"

#include "unihand.h"
#include "app.h"
#include "resman.h"
#include <assert.h>

extern Application *pApp;
extern int numBytesAllocated;

UniHandle::UniHandle(DWORD dwNewSize) {
	pData = NULL;
	hData = NULL;
	iLockCount = 0;
	dwSize = 0;
	SetSize(dwNewSize);
	}

UniHandle::~UniHandle() {
	while (iLockCount > 0)
		Unlock();
	if (hData)
		{
		#if defined(OS_WINDOWS)
		GlobalFree(hData);
		#endif // OS_WINDOWS

		#if defined(OS_BE)
		pData = (void *)hData;
		if (pData)
			delete[] pData;
		#endif // OS_BE

		#if defined(OS_MAC)
		DisposeHandle(Handle(hData));
		#endif // OS_MAC

		numBytesAllocated -= dwSize;
		}
	}

void *UniHandle::Lock(void) {
	if(hData == NULL)
		return NULL;
	if(!iLockCount++) {
		#if defined(OS_WINDOWS)
		pData = GlobalLock(hData);
		#endif // OS_WINDOWS
		
		#if defined(OS_BE)
		pData = (void *)hData;
		#endif // OS_BE

		#if defined(OS_MAC)
		//MoveHHi(Handle(hData));
		HLock(Handle(hData));
		pData = *(void**)hData;
		#endif // OS_MAC
		}
	return (pData);
	}

void *UniHandle::LongLock(void) {
	return Lock();
	}

void UniHandle::Unlock(void) {
	if(iLockCount == 1) {
		#if defined(OS_WINDOWS)
		GlobalUnlock(hData);
		#endif // OS_WINDOWS

		#if defined(OS_BE)
		#endif // OS_BE

		#if defined(OS_MAC)
		HUnlock(Handle(hData));
		#endif // OS_MAC

		pData = NULL;
		}
	if(iLockCount > 0)
		iLockCount--;
	}

DWORD UniHandle::Size() {
	return dwSize;
	}

HANDLE UniHandle::SetSize(DWORD dwNewSize) {
	HANDLE hRetVal;

	if (pApp && pApp->pResourceManager)
		pApp->PurgeResources();
	while (iLockCount)
		Unlock();
	if(dwNewSize == 0) {
		if (dwSize != 0) {
			#if defined(OS_WINDOWS)
			GlobalFree(hData);
			#endif // OS_WINDOWS

			#if defined(OS_BE)
			pData = (void *)hData;
			delete[] pData;
			#endif // OS_BE

			#if defined(OS_MAC)
			DisposeHandle(Handle(hData));
			#endif // OS_MAC

			hData = NULL;
			pData = NULL;
			numBytesAllocated -= dwSize;
			dwSize = 0;
			}
		}
	else {
		numBytesAllocated -= dwSize;
		if(dwSize == 0) {
			#if defined(OS_WINDOWS)
			// GMEM_SHARE may be needed to play sounds.
			hRetVal = GlobalAlloc(GHND /* | GMEM_SHARE*/ , dwNewSize);
			if (hRetVal == 0) {
//				DebugString("UniHandle failed.  Purging Resources.\n");
				pApp->PurgeResources(0);	// Purge all!
				hRetVal = GlobalAlloc(GHND /* | GMEM_SHARE*/ , dwNewSize);
				if (hRetVal == 0) {
//					DebugString("UniHandle failed!\n");
					}
				}
			#endif // OS_WINDOWS

			#if defined(OS_BE)
			hRetVal = (int)new BYTE[dwNewSize];
			#endif // OS_BE

			#if defined(OS_MAC)
			hRetVal = (int)NewHandleClear(dwNewSize);
			if (hRetVal)
				{
				//MoveHHi(Handle(hRetVal));
				}
			else
				{
				pApp->PurgeResources(0);	// Purge all!
		
				// Try again.
				PurgeMem(100000000);
				//CompactMem(100000000);

				hRetVal = (int)NewHandleClear(dwNewSize);
				if (hRetVal)
					{
					//MoveHHi(Handle(hRetVal));
					}
				else
					{
					// Out of memory.
					assert(0);
					}
				}
			#endif // OS_MAC
			}
		else {
			#if defined(OS_WINDOWS)
			// GMEM_SHARE may be needed to play sounds.
			//hRetVal = GlobalReAlloc(hData, dwNewSize, GMEM_MODIFY);
			hRetVal = GlobalReAlloc(hData, dwNewSize, GMEM_ZEROINIT);
			if(hRetVal)
				hData = hRetVal;
			#endif // OS_WINDOWS

			#if defined(OS_BE)
			assert(1);
			hRetVal = 0;
			#endif // OS_BE

			#if defined(OS_MAC)
			assert(1);
			hRetVal = 0;
			#endif // OS_MAC
			}
		if(hRetVal) {
			dwSize = dwNewSize;
			numBytesAllocated += dwSize;
			hData = hRetVal;
			}
		}
	return hRetVal;
	}
