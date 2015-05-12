// Turn off header parts that aren't used.
#define WIN32_LEAN_AND_MEAN

#define OEMRESOURCE
#define NOATOM
// #define NOCLIPBOARD
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
//#define _WINREG_
#define _WINCON_

#define STRICT

#include <windows.h>
#include <mmsystem.h>
#include <windowsx.h>

#define CLIENT
#define NOSCIPOPUPS

#include "os.h"
