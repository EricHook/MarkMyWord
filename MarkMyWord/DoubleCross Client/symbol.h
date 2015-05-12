#ifndef _symbol
#define	_symbol

#include <string.h>
#include "prfdef.h"
#include <stdio.h>
#include <stdlib.h>

const MAX_STRING_SIZE=256;

#define kMaxTypeDefTableSize		100
#define kNumberOfSymbolTableEntries	1000

struct TypeDefTable {
	short count;
	short data[1];
};
typedef struct TypeDefTable TypeDefTable, *LPTypeDefTable;

// еее Could make this a class with Write & read methods
struct MaxTypeDefTable {
	short count;
	short data[kMaxTypeDefTableSize];
};
typedef struct MaxTypeDefTable MaxTypeDefTable, *LPMaxTypeDefTable;


struct SymbolTableElement {
	char *pName;
	short kind;
	void *pTable;		
};
typedef struct SymbolTableElement SymbolTableElement, *LPSymbolTableElement;

class TypeIterator;
class SymbolTable {
  public:
    SymbolTable(short bNeedToSwapBytes = 0);
    ~SymbolTable();

	enum Type {
		byte = -16,
		word,
		dword,
		barray,
		warray,
		array = warray,
		dwarray
	};
	
	enum Kind {
		kindConstant = 0x0100,
		number = kindConstant,
		singleQuotedString,
		doubleQuotedString,

		kindType = 0x0200,
		declarationType = kindType,
		resourceType
	};
 
  	void AddConstant(char *pName, long value);
	void AddSymbolicName(char *pName, char *pQString, Kind kind);
	void AddEntry(char *pName, void *pTable, Kind kind, Boolean bRedefine);
	int LookupNumber(char *pName, long *pValue);
	
	LPSymbolTableElement Lookup(char *pName);
	LPSymbolTableElement Lookup(char *pName, Kind kind);
	short LookupIndex(char *pName);
	short LookupIndex(char *pName, Kind kind);
	int LookupTable(char *pName, void **ppTable, Kind kind);
	LPTypeDefTable TypeAt(short index);
	
	void Write(FILE *pFile);
	void WriteTypeDefTables(FILE *pFile);
	int Read(FILE *pFile);
	short NeedToSwapBytes();
	
	void SwapResource(Ptr pData, CHAR4 c4ResourceType);
	int LookupQuotedString(char *pName, char *pString, Kind kind);

  private:
//  Hash(char *pName);
	void Swap(Ptr *pData, TypeIterator &iterator);
	void SwapComplex(Ptr *pData, TypeIterator &iterator);
	void SwapResourceData(Ptr *pData, TypeIterator &iterator);


	SymbolTableElement fList[kNumberOfSymbolTableEntries];
	short fIdx;
	int fError;
	short fNeedToSwapBytes;
};

inline short SymbolTable::NeedToSwapBytes()
{
	return  fNeedToSwapBytes;
}

class TypeIterator
{
  public:
	TypeIterator(LPTypeDefTable pTable);
	~TypeIterator();
	short SymType()	{ return fTypeDefTable->data[fIndex]; }
	Boolean End() { return fIndex >= fTypeDefTable->count; } 
	void  Next() { fIndex++; }
	short Position() { return fIndex; }
	void SetPosition(short iPos) { fIndex = iPos; }
	
  protected:
	LPTypeDefTable fTypeDefTable;
	short fIndex;
};

#endif //_symbol


