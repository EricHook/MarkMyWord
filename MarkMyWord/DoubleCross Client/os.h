#ifndef _os
#define _os

#define OS_WINDOWS
#define OS_LITTLE_ENDIAN
#define FLIP_DIB

#include "miniwin.h"

typedef char *	Ptr;

#define _booleandefined
typedef BOOL Boolean;
// #define bool BOOL
// #define true TRUE
// #define false FALSE

// #undef MessageBox
// #define MessageBox SCIMessageBox

// Turn on possible warnings
#pragma warning (3 : 4551)	//function call missing argument list
//#pragma warning (3 : 4244)	//conversion from ..., possible loss of data
#pragma warning (3 : 4706)	//assignment within conditional expression
#pragma warning (3 : 4019)	//empty statement at global scope
#pragma warning (3 : 4130)	//logical operation on string constant
#pragma warning (3 : 4699)	//precompiled header notice
//#pragma warning (3 : 4701)	//local may be used before initialized
#pragma warning (3 : 4705)	//statement has no effect
#pragma warning (3 : 4709)	//common in array index expression
#pragma warning (3 : 4127)	//conditional expression is constant
#pragma warning (3 : 4727)	//conditional expression is constant

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include "util.h"
#include "version.h"

void OSOutputString(char *outputString); // pointer to string to be displayed
int OSFileLength(FILE *file);

#endif // _os
