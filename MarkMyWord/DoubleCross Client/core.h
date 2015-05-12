#if !defined(core_h)
#define core_h

#define LOG

#ifdef LOG
extern FILE * Log;
# define DUMPLOG(MSG) \
  if (Log) \
  { \
    MSG; \
    fflush (Log); \
  }
#else
# define DUMPLOG(MSG) ;
#endif

#include "app.h"
#include "bitmap.h"
#include "inifile.h"
#include "palette.h"
#include "sound.h"
#include "sounder.h"
#include "timer.h"
#include "window.h"
#include "texture.h"
#include "tex.h"
#include "prf.h"
#include "list.h"
#include "resource.h"
#include "resman.h"
#include "framebuf.h"
#include "tsprite.h"

#endif // core_h
