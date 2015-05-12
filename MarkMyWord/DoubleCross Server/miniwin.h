#include "version.h"

#ifdef _WIN32

// Turn off header parts that aren't used.
#define WIN32_LEAN_AND_MEAN

/*
#define OEMRESOURCE
#define NOATOM
#define NOCLIPBOARD
//#define NOCOLOR
//#define NOCTLMGR
#define NODRAWTEXT
#define NONLS
#define NOMETAFILE
#define NOSCROLL
#define NOSERVICE
#define NOTEXTMETRIC
#define NOWH
#define NOCOMM
#define NOKANJI
#define NODEFERWINDOWPOS
#define NOMCX
#define NOCRYPT
#define NOIME
#define NOKEYSTATES
#define NOICONS
#define NOGDICAPMASKS

#define MMNOMIDI
#define MMNODRV
#define MMNOSOUND
#define MMNOAUX
#define MMNOMIXER
//#define MMNOJOY
#define MMNOMMSYSTEM

// Turn off other headers
#define _WINNETWK_
#define _WINREG_
#define _WINCON_
*/

#define STRICT

#include <windows.h>
#include <mmsystem.h>
#include <windowsx.h>

// stupid MS didn't make these C++ callable
extern "C" {
#include "ipexport.h"
//#include "icmpapi.h"
}

#endif

#include "cmBase.h"
#include <assert.h>
#include <list>
#include <string.h>

//
#define GUARDS
#define TRAY_ICON

// we are going to use STL classes
using namespace std;

#ifdef _WIN32

//
#define BEGIN_MSG_MAP switch(msg) {

#define END_MSG_MAP default: return DefWindowProc(hwnd, msg, wParam, lParam); \
	} return 0

#undef HANDLE_MSG
#define HANDLE_MSG(msg, handler) case msg: handler(hwnd, wParam, lParam); break

#include "resource.h"
#include <shellapi.h>
#include <prsht.h>

extern HINSTANCE g_hInstance;

#endif

typedef char Boolean;
