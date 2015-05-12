/*
* MAKETEX.h
*
* Purpose:  Interface declarations for MAKETEX.dll.
*
* Copyright (c) 1999 by Sierra On-Line, Inc.
* All Rights Reserved.
*
********* PRELIMINARY **************
*
*	Prototypes and defines for MAKETEX.dll
*
*/

#ifndef __MAKETEX_H__
#define __MAKETEX_H__

#include "filterfunc.h"

/************* Gza2Tex(char *gzaFile, char *outFile) **************
* 
*	Unarchives a .gza archive of .gz files, and unzips each .gz file.  Then
*	for each collection of *.flg and *.jpg files, it produces a *.tex file.
*	
*	
*
*	Input:
*		gzaFile -	A pathname of a .gza files
*		outFile  -	An output pathname
*
*	Output:
*		Status	- JT_SUCCESS if successful, 
*				- See above list of statuses for failures
*
*/
typedef FilterFuncErrorCode (__cdecl *lpfnGza2TexFunc)(const char *gzaFile, const char *outFile);
extern "C" FilterFuncErrorCode __declspec( dllexport ) Gza2Tex(const char *infile, const char *outfile);

/************* Jpeg2Tex(char *inFilename, char *outFilename) **************
* 
*	Converts a .jpg file to a .tex file with one cell
*
*	Input:
*		inFilename -	The pathname of a .jpg file
*		outFilename  -	The pathname of an output *.tex file
*
*	Output:
*		Status	- JT_SUCCESS if successful, 
*				- See above list of statuses for failures
*
*/
typedef FilterFuncErrorCode (__cdecl *lpfnJpeg2TexFunc)(const char *inFilename, const char *outFilename);
extern "C" FilterFuncErrorCode __declspec( dllexport ) Jpeg2Tex(const char *inFilename, const char *outFilename);

/************* GIF2Tex(char *inFilename, char *outFilename) **************
* 
*	Converts a .gif file to a .tex file, appending timing info to the end
*
*	Input:
*		inFilename -	The pathname of a .gif file
*		outFilename  -	The pathname of an output *.tex file
*
*	Output:
*		Status	- JT_SUCCESS if successful, 
*				- See above list of statuses for failures
*
*/
typedef FilterFuncErrorCode (__cdecl *lpfnGIF2TexFunc)(const char *inFilename, const char *outFilename);
extern "C" FilterFuncErrorCode __declspec( dllexport ) GIF2Tex(const char *inFilename, const char *outFilename);

#endif // __MAKETEX_H__