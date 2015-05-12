/*
* FILTERFUNC.h
*
* Purpose:  error code enum for filter functions in UNGZIP.dll and MAKETEX.dll.
*
* Copyright (c) 1999 by Sierra On-Line, Inc.
* All Rights Reserved.
*
*/

#ifndef __FILTERFUNC_H__
#define __FILTERFUNC_H__

enum FilterFuncErrorCode {
	FF_SUCCESS = 0,
	FF_NOGZAFILE,
	FF_NOOUTDIR,
	FF_CREATEOUTPUT,
	FF_ERRDELETE,
	FF_BADGZNAME,
	FF_OPENGZA,
	FF_BADGZA,
	FF_NOGZFILE,
	FF_NOOUTFILE,
	FF_OPENGZ,
	FF_OPENOUT,
	FF_DECOMPRESS,
	FF_NOJPGFILE,
	FF_OPENJPG,
	FF_WRITETEX,
	FF_NOTEXBASE,
	FF_OPENFLG,
	FF_READPALETTE,
	FF_READTIME,
	FF_OPENTEX,
	FF_DELETEFLAG,
	FF_DELETEJPG,
	FF_STARTDECOMP,
	FF_READSCANLINE,

	FF_NOGIF,
	FF_NOOUT,
	FF_OPENGIF,
	FF_BADTYPE,
	FF_GIFEND,
	FF_WRITEOUT,
	FF_GIFERROR,
	FF_BADGIF,

	FF_LAST
};

typedef FilterFuncErrorCode (__cdecl *lpfnFilterFunc)(const char *inFile, const char *outFile);

#endif // __FILTERFUNC_H__
