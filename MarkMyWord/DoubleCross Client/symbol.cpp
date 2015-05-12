#include "os.h"
#include "symbol.h"

SymbolTable::SymbolTable(short bNeedToSwapBytes)
{
	fIdx = 0;
	fError = 0;
	fNeedToSwapBytes = bNeedToSwapBytes;
}

SymbolTable::~SymbolTable()
{
}

void SymbolTable::AddConstant(char *pName, long value)
{
	long *pTable = (long *)new BYTE[sizeof(long)];
	if(pTable == NULL)
	{
		fError = kNotEnoughMemory;
		return;
	}
	*pTable = value;
	AddEntry(pName, pTable, number, TRUE);
}

short SymbolTable::LookupIndex(char *pName)
{	
	short i;
	LPSymbolTableElement pTableEntry = fList;
	
	for (i=0; i<fIdx; i++, pTableEntry++)
	{
		if(!strcmp(pName, pTableEntry->pName))
			return i;
	}
	return -1;
}

short SymbolTable::LookupIndex(char *pName, Kind kind)
{
	short i = LookupIndex(pName);
	if(fList[i].kind != kind)
		i = -1;
	return i;	
}

LPSymbolTableElement SymbolTable::Lookup(char *pName)
{	
	short i = LookupIndex(pName);
	if( i!=-1 )
		return &fList[i];
	else
		return NULL;
}

LPSymbolTableElement SymbolTable::Lookup(char *pName, Kind kind)
{	
	LPSymbolTableElement pTableEntry = Lookup(pName);
	if(!pTableEntry)
	{
		fError = kIdentifierNotFound;
		return NULL;
	}
	if(pTableEntry->kind != kind)
	{
		fError = kDefinedAsAnotherType;
		return NULL;
	}
	return pTableEntry;
}

int SymbolTable::LookupNumber(char *pName, long *pValue)
{
	LPSymbolTableElement pTableEntry = Lookup(pName, number);
	if(!pTableEntry)
		return fError;
	*pValue = *(long*)pTableEntry->pTable;
	return 0;
}

int SymbolTable::LookupTable(char *pName, void **ppTable, Kind kind)
{
	LPSymbolTableElement pTableEntry = Lookup(pName, kind);
	if(!pTableEntry)
		return fError;
	*ppTable = pTableEntry->pTable;
	return 0;
}



void SymbolTable::AddEntry(char *pName, void *pTable, Kind kind, Boolean bRedefine)
{
	LPSymbolTableElement pTableEntry = Lookup(pName);
	if(pTableEntry)	// name already defined
	{
		if(!bRedefine)
			fError = kTypeAlreadyDefined;
		else
			delete (pTableEntry->pTable);
	}
	else
	{
		pTableEntry = &fList[fIdx];
			
		pTableEntry->pName = (Ptr)new BYTE[strlen(pName)+1];
		if(!pTableEntry->pName) 
			fError = kNotEnoughMemory;
		else
		{
			strcpy(pTableEntry->pName, pName);
			fIdx++;
		}
	}

	if(fError)
		delete (pTable);
	else
	{
		pTableEntry->pTable = pTable;
		pTableEntry->kind = kind;
	}
}

LPTypeDefTable SymbolTable::TypeAt(short index)
{
	LPSymbolTableElement pTableEntry = &fList[index];
	if(!(pTableEntry->kind & kindType))
		return NULL;
	else
		return (LPTypeDefTable)pTableEntry->pTable;
}

void SymbolTable::Write(FILE *pFile)
{
	LPSymbolTableElement pSymbolTableElement = fList;
	short i;
	short numOfEntries = 0;
	
	//fwrite (&fIdx, sizeof_short, 1, pFile);	// number of symbol table entries/records
	fwrite (&numOfEntries, sizeof_short, 1, pFile);	// reserve 2 bytes for the number of table entries - write out 0 initially

	for(i=0; i < fIdx; i++, pSymbolTableElement++)
	{
		short nameLength = strlen(pSymbolTableElement->pName);
		fwrite (&nameLength, sizeof_short, 1, pFile);	
		fwrite (pSymbolTableElement->pName, nameLength+1, 1, pFile);

		fwrite (&pSymbolTableElement->kind, sizeof_short, 1, pFile);

		short dataSize;
		switch(pSymbolTableElement->kind)
		{
			case number:
				dataSize = 4;
				break;
			case singleQuotedString:
			case doubleQuotedString:
				dataSize = strlen((char*)pSymbolTableElement->pTable)+1;
				break;
			case declarationType:
			case resourceType:
				dataSize = (((TypeDefTable *)pSymbolTableElement->pTable)->count+1)<<1;
				break;
		}
		fwrite (&dataSize, sizeof_short, 1, pFile);
		fwrite (pSymbolTableElement->pTable, dataSize, 1, pFile);
		numOfEntries++;
	}
	
	fseek(pFile, 0, SEEK_SET);
	fwrite (&numOfEntries, sizeof_short, 1, pFile);	// number of symbol table entries/records
}
 
void SymbolTable::WriteTypeDefTables(FILE *pFile)
{
	LPSymbolTableElement pSymbolTableElement = fList;
	short i;
	short numOfEntries = 0;
	short *symbolMap = new short[fIdx];
	
	fwrite (&numOfEntries, sizeof_short, 1, pFile);	// reserve 2 bytes for the number of table entries - write out 0 initially

	for(i=0; i < fIdx; i++, pSymbolTableElement++)
	{
		if( pSymbolTableElement->kind & kindType )
		{
			short nameLength = strlen(pSymbolTableElement->pName);
			fwrite (&nameLength, sizeof_short, 1, pFile);	
			fwrite (pSymbolTableElement->pName, nameLength+1, 1, pFile);
	
			fwrite (&pSymbolTableElement->kind, sizeof_short, 1, pFile);
			TypeDefTable *pTDTable = (TypeDefTable *)pSymbolTableElement->pTable;
			short dataSize = (pTDTable->count+1)<<1;
			fwrite (&dataSize, sizeof_short, 1, pFile);
			symbolMap[i] = numOfEntries;
			MaxTypeDefTable table;
			table.count = pTDTable->count;
			for(i=0; i < pTDTable->count; i++)
			{
				short idx = pTDTable->data[i];
				table.data[i] = idx < 0 ? idx : symbolMap[idx];
			}
			fwrite (&table, dataSize, 1, pFile);
			numOfEntries++;
		}
	}
	
	fseek(pFile, 0, SEEK_SET);
	fwrite (&numOfEntries, sizeof_short, 1, pFile);	// number of symbol table entries/records
}
 
 
//еее The print statements in Read() should be alerts during runtime.
int SymbolTable::Read(FILE *pFile)
{
	if( !fread(&fIdx,sizeof_short,1,pFile)  )
	{
		DebugString("Error reading symbol table data.\n");
		return -1;
	}
	else if(NeedToSwapBytes())
		SwapBytes((WORD *)&fIdx);
	
	short i;
	for (i=0; i<fIdx; i++)
	{
		short nameLength;
		if( !fread(&nameLength,sizeof_short,1,pFile) )
		{
			DebugString("Error reading symbol table data.\n");
			return -1;
		}
		else if(NeedToSwapBytes())
			SwapBytes((WORD *)&nameLength);
		 
		fList[i].pName = (Ptr)new BYTE[nameLength+1];
		if(!fList[i].pName)
		{
			DebugString("Not enough memory\n");
			return -1;
		}
		
		short dataSize;
		if( !fread(fList[i].pName,nameLength+1,1,pFile) ||
			!fread (&fList[i].kind, sizeof_short, 1, pFile) ||
			!fread(&dataSize,sizeof_short,1,pFile) )
		{
			DebugString("Error reading symbol table data.\n");
			return -1;
		}
		else if(NeedToSwapBytes())
		{
			SwapBytes((WORD *)&fList[i].kind);
			SwapBytes((WORD *)&dataSize);
		}

		void *pTable = new BYTE[dataSize];
		fList[i].pTable = pTable;
		if(!pTable)
		{
			DebugString("Not enough memory\n");
			return -1;
		}
		
		if( !fread (pTable, dataSize, 1, pFile) )
		{
			DebugString("Error reading symbol table data.\n");
			return -1;
		}
		else if(NeedToSwapBytes())
		{
			switch(fList[i].kind)
			{
				case number:
					SwapBytes((DWORD *)pTable);
					break;
				case singleQuotedString:
				case doubleQuotedString:
					break;
				case declarationType:
				case resourceType:
					TypeDefTable *pDefTable = (TypeDefTable *)pTable;
					SwapBytes((WORD *)&pDefTable->count);
					short *pData = &pDefTable->data[0];
					short count = pDefTable->count;
					short j;
					for (j=0; j<count; j++, pData++)
						SwapBytes((WORD *)pData);
					break;
			}
		}
	}
	
	return 0;
}

void SymbolTable::Swap(Ptr *pData, TypeIterator &iterator)
{
	short size = 0;
	switch(iterator.SymType())
	{
		case SymbolTable::byte:
		case SymbolTable::barray:
			size = 1;
			break;
		case SymbolTable::word:
		case SymbolTable::warray:
			SwapBytes((WORD *)*pData);
			size = 2;
			break;
		case SymbolTable::dword:
		case SymbolTable::dwarray:
			SwapBytes((DWORD *)*pData);
			size = 4;
			break;
	};
	iterator.Next();
	*pData += size;
}

void SymbolTable::SwapComplex(Ptr *pData, TypeIterator &iterator)
{
	TypeIterator newIterator(TypeAt(iterator.SymType()));
	while(!newIterator.End())
	{
		SwapResourceData(pData, newIterator);
		newIterator.Next();
	}
	iterator.Next();
}

void SymbolTable::SwapResourceData(Ptr *pData, TypeIterator &iterator)
{
	DWORD i;
	Ptr pArraySize;
	short sIteratorType;

	while(!iterator.End())
	{
		sIteratorType = iterator.SymType();
		switch(sIteratorType)
		{
			case SymbolTable::byte:
			case SymbolTable::word:
			case SymbolTable::dword:
				Swap(pData, iterator);
				break;
				
			case SymbolTable::barray:
			case SymbolTable::warray:
			case SymbolTable::dwarray:
				pArraySize = *pData;
				Swap(pData, iterator);
					switch(sIteratorType) {
						case SymbolTable::barray:
							i = *(BYTE*) pArraySize;
							break;
						case SymbolTable::warray:
							i = *(WORD*) pArraySize;
								break;
						case SymbolTable::dwarray:
							i = *(DWORD*) pArraySize;
							break;
						}
					
					if(iterator.SymType() >= 0)	{	//  not a simple type
						while (i--) {
							TypeIterator newIterator(TypeAt(iterator.SymType()));
							SwapResourceData(pData, newIterator);
							}
					}
					else {
						short pos = iterator.Position();
						while (i--) {
							SwapResourceData(pData, iterator);
							iterator.SetPosition(pos);
						}
					}
					iterator.Next();
				break;
	
			default:
				SwapComplex(pData, iterator);
				break;
		}
	}	
}


void SymbolTable::SwapResource(Ptr pData, CHAR4 c4ResourceType)
{
	char pName[5];
	memcpy(pName, &c4ResourceType, 4);
	pName[4] = '\0';
	
	TypeIterator iterator(TypeAt(LookupIndex(pName)));
	SwapResourceData(&pData, iterator);
}


void SymbolTable::AddSymbolicName(char *pName, char *pQString, Kind kind)
{
	char *pTable = (Ptr)new BYTE[strlen(pQString)+1];
	if(pTable == NULL)
	{
		fError = kNotEnoughMemory;
		return;
	}
	
	strcpy(pTable, pQString);
	AddEntry(pName, pTable, kind, TRUE);
}


int SymbolTable::LookupQuotedString(char *pName, char *pString, Kind kind)
{
	LPSymbolTableElement pTableEntry = Lookup(pName, kind);
	if(!pTableEntry)
		return fError;
		
	if(strlen((char *)pTableEntry->pTable) > MAX_STRING_SIZE-3)	// allow space for quotes
	{
		fError = STRING_TOO_LONG;
		return fError;
	}
	
	char pTemp[MAX_STRING_SIZE];
	char quote[2];
	quote[0] =  (kind == singleQuotedString) ? '\'' : '\"';
	quote[1] = '\0';
	
	strcat(pTemp, (char *)pTableEntry->pTable);
	strcat(pTemp, quote);
	strcpy(pString, pTemp);

	return 0;
}

/*------------------------------------------------------------------------------------*/

TypeIterator::TypeIterator(LPTypeDefTable pTable)
{
	fTypeDefTable = pTable;
	fIndex = 0;
}

TypeIterator::~TypeIterator()
{
}




