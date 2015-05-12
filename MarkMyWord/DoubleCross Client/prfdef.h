#ifndef _prfDef
#define _prfDef

#define sizeof_short	2
#define kSymbolsOn		1		// create symbol table with #defines included
#define kSymbolsOff		0		// create symbol table with only types included

// Lex
#define ID_TOO_LONG 			-1
#define STRING_TOO_LONG 		-2
#define INVALID_TOKEN			-3

// Parser & Symbol Table
#define kNotEnoughMemory		-10
#define kDefinedAsAnotherType	-11
#define kBadIdentifierType		-12
#define kInvalidTypeToken		-13
#define kTypeAlreadyDefined		-14
#define kTooManyTypeFields		-15
#define kIdentifierNotFound		-16
#define kInvalidDirective		-17
#define kInvalidResourceType	-18
#define kResourceTypeNotFound	-19
#define kBadSymbolicType		-20
#define kNumberTooBig			-21


//еее
typedef long CHAR4;

void SwapBytes(WORD *wData);
void SwapBytes(DWORD *dwData);

#endif // _prfDef
