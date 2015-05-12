/*===============================================================
	
	wgdict.cpp - Class for access to dictionary File

    Copyright 1999 by Sierra On-Line Inc., All Rights Reserved

	Author: Eric Fleming

===============================================================*/

#include "ServerInclude.h"

/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
//
//		WGDict class functions
//
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
/*=================================================================
	Constructor/Destructor
===================================================================*/
WGDict::WGDict()
{
	short		ii;
	
	for ( ii = 0; ii < WGDICT_NUMDICTS; ii++ ) {
		m_flSize[ii] = 0;
		if ( ii < 10 )
			m_dictionary[ii] = NULL;
		if ( ii < 6 )
			m_memDict[ii] = NULL;
	}
		
	m_pageBuf = NULL;
	m_memLoad = 0;
	m_offensiveWords = NULL;
	definitions = NULL;
}

WGDict::~WGDict()
{
	CleanupDict();
	
	if (m_offensiveWords) { delete m_offensiveWords; m_offensiveWords = NULL; }
}

/*=================================================================
	Initialize Dictionary for Use in a game
===================================================================*/
bool WGDict::InitDict(short memLoad)
{
	char *ptr, path[256], defPath[256], offensivePath[256];
	int curSize, bytes, amount, ii, length;
	fpos_t pos;
	FILE *f;
	
	if ( m_memDict[0] )
		return true;
	
	if ( memLoad > 7 || memLoad < 5 )
		return false;
	
	// Open dictionary files; load into memory all the ones where the word length <= 7
	
	for ( ii = 0; ii < WGDICT_NUMDICTS; ii++ ) {
		// Check for the dictionary files at different spots
		strcpy(path, DICTIONARY_PATH);
		sprintf(&path[strlen(path)], "dict%d.dat", ii + 2);
		
		f = fopen(path, "rb");			
		
		if (!f)
			{ TRACE(_WARN_ "Can't open dictionary file!"); return false; }
		
		fseek(f, 0, SEEK_END);
		fgetpos(f, &pos);
		m_flSize[ii] = (int)pos;
		
		if ( ii <= memLoad - 2 ) {
			// Load these dictionaries into memory
			
			m_memDict[ii] = new char[m_flSize[ii]];
			if (m_memDict[ii]) {
				ptr = m_memDict[ii];

				// Read data from file
				fseek(f, 0, SEEK_SET);
				curSize = 0;
				while ( curSize < m_flSize[ii] ) {
					bytes = min(m_flSize[ii] - curSize, 30000);
					amount = fread(ptr, sizeof(char), bytes, f);
					curSize += amount;
					ptr += amount;
				}
			}
			else
				{ TRACE(_WARN_ "Not enough memory to load dictionary.\n"); return false; }
			
			fclose (f);
		}
		else {
			// Keep these ones on the disk
			m_dictionary[ii - (memLoad - 1)] = f;
		}
	}
	
	m_memLoad = memLoad;
	
	////////
	
	strcpy(path, DICTIONARY_PATH);
	strcat(path, "offensive.txt");
	f = fopen(path, "rb");
	
	if (f == NULL)
		{ ASSERT(_WARN_, FALSE); return false; }
	
	fseek(f, 0, SEEK_END);
	fgetpos(f, &pos);
	length = (int)pos;
	
	m_offensiveWords = new char[length + 1];
	fread(m_offensiveWords, sizeof(char), length, f);
	m_offensiveWords[length] = 0;
	fclose(f);
	
	// Read in definitions 
	definitions = new DictTree();
	
	strcpy(path, DICTIONARY_PATH);
	strcat(path, "dict.bin");
	
	strcpy(defPath, DICTIONARY_PATH);
	strcat(defPath, "dict.def");
	
	strcpy(offensivePath, DICTIONARY_PATH);
	strcat(offensivePath, "offensive.txt");
	
	definitions->ReadIn(path, defPath, offensivePath);

	return true;
}

/*=================================================================
	Cleanup Dictionary
===================================================================*/
void WGDict::CleanupDict()
{
	short		ii;
	
	if ( !m_memLoad )
		return;
	
	if ( m_pageBuf )
        { delete m_pageBuf; m_pageBuf = NULL; }
	
	for ( ii = 0; ii < WGDICT_NUMDICTS; ii++ ) {
		if ( ii <= m_memLoad - 2 ) {
			if ( m_memDict[ii] ) {
				delete [] m_memDict[ii];
				m_memDict[ii] = NULL;
			}
		}
		else {
			if ( m_dictionary[ii - (m_memLoad - 1)] ) {
				fclose(m_dictionary[ii - (m_memLoad - 1)]);
				m_dictionary[ii - (m_memLoad - 1)] = NULL;
			}
		}
	}

	m_memLoad = 0;

	if ( definitions ) 
		{ delete definitions; definitions = NULL; }
}

/*=================================================================
	Find a Word in the specified dictionary file with the specified
	qualities
===================================================================*/
bool WGDict::FindWordInTree(char *word, short whichDict, bool startsWord, bool isWord,
					bool allowDirty /* = false */, char level /* = 2 */)
{
	unsigned short curNode = 0, pgnum = 0, numWords, nextpg;
	short	lower, upper, slotno, cmp, slotlen;
	char	*page, *ptr;
	bool	result;	

	slotlen = (whichDict + 2) + 1 /* null term */ + WGDICT_SLOTHDRSIZE;
	//len = strlen(word);
	for ( ; ; curNode++ ) {
		page = GetNode(pgnum, whichDict);

        memcpy(&numWords, page, sizeof(unsigned short));
		// MacSwapBytes(&numWords);
        
		if ( !numWords ) {
			return false;
		}
        
        lower = 0;
        upper = numWords - 1;
        do {
            slotno = (lower + upper) / 2;
            ptr = page + 2 + slotno*slotlen + WGDICT_SLOTHDRSIZE /* Get past header */;
			cmp = strcmp(word, ptr);
            
            if ( cmp == 0 )
                break;
            if (cmp < 0)
                upper = slotno - 1;
            else
                lower = slotno + 1;
        } while (upper >= lower);

        if ( cmp == 0 ) {
			// Found it! Check if it starts word or is word
			ptr--;
			result = true;
			if ( !allowDirty && (*ptr & WGDICT_ISDIRTY) )
				result = false;
			else if ( level == 0 /* WGDICT_ISBASIC */ ) {
				// For basic, the computer will only try to create partial
				// words that start with the the letters played.
				// It's not 'smart' enough to play middles of words
				if ( (!isWord && !(*ptr & WGDICT_BASSTART)) || 
						(isWord && !(*ptr & WGDICT_ISBASIC)) )
					result = false;
			}
			else if ( level == 1 /* WGDICT_ISMEDIUM */ ) {
				if ( (startsWord && !(*ptr & WGDICT_MEDSTART)) || 
						(isWord && !(*ptr & WGDICT_ISMEDIUM)) ||
						!(*ptr & WGDICT_MEDPART) )
					result = false;
			}
			else {
				if ( (startsWord && !(*ptr & WGDICT_STARTSWORD)) || 
						(isWord && !(*ptr & WGDICT_ISWORD)) )
					result = false;
			}
			
			return result;
		}
		else {
            // go to next node
			if ( cmp > 0 ) {
                if ( slotno == numWords - 1 ) {
				    // Value is higher than all values on page
                    slotno++;
                    ptr = page + WGDICT_MAXSLOTS*slotlen + 2;
				    memcpy(&nextpg, ptr, sizeof(unsigned short));
					// MacSwapBytes(&nextpg);
                }
                else {
                    // Get the prevptr for the next highest slot; we are higher
                    // than the current slot but lower than the next one
                    slotno++;
                    ptr = page + 2 + slotlen*slotno;
					memcpy(&nextpg, ptr, sizeof(unsigned short));
					// MacSwapBytes(&nextpg);
                }
			}
			else {
				// value is lower than current slot
                ptr -= WGDICT_SLOTHDRSIZE;
				memcpy(&nextpg, ptr, sizeof(unsigned short));
				// MacSwapBytes(&nextpg);
			}
				
			if ( nextpg == 0 ) {
				return false;
			}
			else {
				pgnum = nextpg;				
			}			
		}
	}

	return false;
}

/*=================================================================
	Initialize Dictionary for Use in a game
===================================================================*/
char *WGDict::GetNode(unsigned short pgnum, short whichDict)
{
	int     ret, which;
    char   *ptr;
	unsigned short pgsize = (whichDict + 2 + WGDICT_SLOTHDRSIZE + 1)*WGDICT_MAXSLOTS + 4;

	if ( !m_pageBuf )
        m_pageBuf = new char[WGDICT_MAXPGSIZE];

    if ( whichDict >= m_memLoad - 1 ) {
		which = whichDict - (m_memLoad - 1);
		ret = fseek(m_dictionary[which], pgnum*pgsize, SEEK_SET);
		if ( ret >= 0 ) {
			ret = fread(m_pageBuf, sizeof(char), pgsize, m_dictionary[which]);
			if ( !ret )
				memset(m_pageBuf, '\0', pgsize);
		}
		else {
			memset(m_pageBuf, '\0', pgsize);
		}
		ptr = m_pageBuf;
	}
	else {
		ptr = m_memDict[whichDict] + pgnum*pgsize;
	}
    
	return ptr;
}

/*=================================================================
	Is this a legal partial word?
===================================================================*/
bool WGDict::IsLegalPartialWord(char *str, short len, bool allowDirty, char level)
{
	bool	 result;
	char	 buf[16];
	short	 wordLen;
	
	if ( len ) {
		strncpy(buf, str, len);
		buf[len] = '\0';
	}
	else
		strcpy(buf, str);
	strlwr(buf);
	wordLen = strlen(buf);
	
	result = FindWordInTree(buf, wordLen - 2, false, false, true, level);
	
	// since some words are missing from the DC dictionary, allow partial dirty words here
	
	if (!result && allowDirty && strstr(m_offensiveWords, buf))
		return true;
	
	return result;
}

/*=================================================================
	Is this a legal Word?
===================================================================*/
bool WGDict::IsLegalWord(char *str, short minWordSize, bool allowDirty, char level)
{
	char	word[20], s[32];
	bool	result;
	short	len;
	
	if ( (signed short)strlen(str) < minWordSize )
		return false;

	strcpy(word, str);
	strlwr(word);
	len = strlen(word);

	result = FindWordInTree(word, len - 2, false, true, true /* allowDirty */, level);
	
	// allow words that are not included in the DC dictionary at all
	
	if (!result && allowDirty) {
		sprintf(s, "-%s-", word);
		if (strstr(m_offensiveWords, s))
			return true;
	}
	
	// since we allowed all words above, filter dirty words here
	
	if (result && !allowDirty) {
		sprintf(s, "-%s-", word);
		if (strstr(m_offensiveWords, s))
			return false;
	}
	
	return result;
}

/*=================================================================
	Does this fragment start a legal word?
===================================================================*/
bool WGDict::IsLegalWordStart(char *str, short len, char level)
{
	char	buf[20];

	strncpy(buf, str, len);
	buf[len] = '\0';
	strlwr(buf);

	return FindWordInTree(buf, len - 2, true, false, false, level);	
}

/*=================================================================
	Return a Random word of specified length(s) and Quality
===================================================================*/
char *WGDict::GetRandomWord(short lenStart, short lenEnd, short flags)
{
	short	wordLen, numWordLens;
	unsigned short numPages, curPage, numWords, curWord;
	unsigned short slotlen;
	bool	found = false;
	char   *ptr = NULL;

	assert(!lenEnd || (lenEnd >= lenStart));

	// Get random length

	numWordLens = (!lenEnd) ? 1 : lenEnd - lenStart + 1;

	while ( !found ) {
		wordLen = roomServer->randomGenerator->GetRandom(numWordLens) + lenStart;
		
		slotlen = WGDICT_SLOTHDRSIZE + 1 + wordLen;

		// Get Random Page from file
		numPages = NumPages(wordLen);
		curPage = roomServer->randomGenerator->GetRandom(numPages);
		ptr = GetNode(curPage, wordLen - 2);

		memcpy(&numWords, ptr, sizeof(unsigned short));
		// MacSwapBytes(&numWords);
        if (!numWords)
            continue;
		curWord = roomServer->randomGenerator->GetRandom(numWords);
		ptr += (2 + curWord*slotlen + 2);

		// Does this word match the flag criteria?
		if ( !(flags & WGDICT_ALLOWDIRTY) && (*ptr & WGDICT_ISDIRTY) ) 
			continue;
		else if ( flags & WGDICT_ISBASIC ) {
			if ( (!(flags & WGDICT_ALLOWPARTIAL) && !(*ptr & WGDICT_ISBASIC)) ||
					((flags & WGDICT_ALLOWPARTIAL) && !(*ptr & WGDICT_BASSTART)) )
				continue;
		}
		else if ( flags & WGDICT_ISMEDIUM ) {
			if ( (!(flags & WGDICT_ALLOWPARTIAL) && !(*ptr & WGDICT_ISMEDIUM)) ||
					((flags & WGDICT_ALLOWPARTIAL) && !(*ptr & WGDICT_MEDPART)) )
				continue;
		}
		else if	(!(flags & WGDICT_ALLOWPARTIAL) && !(*ptr & WGDICT_ISWORD)) 
			continue;

		// Found one
		found = true;
	}

	ptr++;
	return ptr;
}

/*=================================================================
	Return number of pages in specified dictionary
===================================================================*/
unsigned short WGDict::NumPages(short wordLen)
{
	unsigned short slotlen = wordLen + 1 + WGDICT_SLOTHDRSIZE;
	long	pgsize = slotlen*WGDICT_MAXSLOTS + 4;
	
	return (m_flSize[wordLen-2]/pgsize);
}

////////////////////////////////////////////////////////////////////////////////////////////

DictTree::DictTree()
{
	mDefFile = NULL;
	offensiveWords = NULL;
	
	gNodePool = NULL;
	gPoolAllocIdx = 0;
	gCount = 0;
}

DictTree::~DictTree()
{
	if (offensiveWords) { delete offensiveWords; offensiveWords = NULL; }
	if (mDefFile) { fclose(mDefFile); mDefFile = NULL; }
	if (gNodePool) { delete [] gNodePool; gNodePool = NULL; }
}

//
// Get the size of the file, leaving the file ptr where it was...
//
int DictTree::FileSize(FILE *fptr)
{
	long oldPos;
	int fileSize;

	oldPos = ftell(fptr);

	fseek(fptr, 0, SEEK_END);
	fileSize = ftell(fptr);
	fseek(fptr, oldPos, SEEK_SET);

	return fileSize;
}

bool DictTree::ReadIn(char *theFileName, char *theDefinitionFileName, char *theOffensiveFileName)
{
	int length;
	FILE *aFileP;
	
	aFileP = fopen(theFileName, "rb");
	
	if (aFileP == NULL)
		{ ASSERT(_WARN_, FALSE); return false; }
	
	gNodePool = new Node26[FileSize(aFileP) / 4];
	ReadChunk(aFileP, &mRoot);
	fclose(aFileP);
	
	/////
	
	aFileP = fopen(theOffensiveFileName, "rb");
	
	if (aFileP == NULL)
		{ ASSERT(_WARN_, FALSE); return false; }
	
	length = FileSize(aFileP);
	offensiveWords = new char[length + 1];
	fread(offensiveWords, sizeof(char), length, aFileP);
	offensiveWords[length] = 0;
	fclose(aFileP);
	
	/////
	
	mDefFile = fopen(theDefinitionFileName, "rb");
	if (mDefFile == NULL)
		{ ASSERT(_WARN_, FALSE); return false; }
	
	return true;
}

void DictTree::ReadChunk(FILE *aFileP, Node26 *aNode)
{	
	int i, anElementCount = 0;
	
	fread(&aNode->mBitSet, sizeof(long), 1, aFileP);
	
	for (i = 0 ; i < 26 ; i++)
		if ((aNode->mBitSet & (1 << i)) != 0)
			anElementCount++;
	
	aNode->mNodeArray = &gNodePool[gPoolAllocIdx];
	gPoolAllocIdx += anElementCount;
	
	gCount += anElementCount;
	
	anElementCount = 0;
	
	for (i = 0 ; i < 26 ; i++)	
		if ((aNode->mBitSet & (1 << i)) != 0) {		
			ReadChunk(aFileP, &aNode->mNodeArray[anElementCount]);		
			aNode->mNodeArray[anElementCount].mBitSet |= (i << 26);
			anElementCount++;
		}	
	
	if (aNode->IsWord())
		fread(&aNode->mDefPos, sizeof(DWORD), 1, aFileP);
}

bool DictTree::Contains(char *theWord, Node26 **theEndNode)
{
	int i, length, anIdx;
	unsigned int aMask;
	register Node26 *aNode = &mRoot;
	
	length = strlen(theWord);
	
	if (!aNode->mNodeArray)
		return false;
	
	for (i = 0 ; i < length ; i++) {
		anIdx = toupper(theWord[i]) - 'A';
		
		if ((anIdx < 0) || (anIdx >= 26))
			return false;
		
		if ((aNode->mBitSet & (1 << anIdx)) == 0)
			return false;
		
		aMask = (anIdx << 26);
		
		for (aNode = aNode->mNodeArray ; ; aNode++)
			if ((aNode->mBitSet & aMask) == (unsigned int)aMask) 
				break;
	}
	
	if (theEndNode)
		*theEndNode = aNode;
	
	return (aNode->mBitSet & 0x80000000) != 0;
}

bool DictTree::IsOffensive(char *theWord)
{
	char lowerWord[256];
	
	sprintf(lowerWord, "-%s-", theWord);
	strlwr(lowerWord);
	
	if (offensiveWords && strstr(offensiveWords, lowerWord))
		return true;
	
	return false;
}

void DictTree::FindDefinition(char *theWord, char *theDefinition)
{
	DWORD aDefSize;
	Node26 *anEndNode;
	
	theDefinition[0] = 0;
	
	if (!Contains(theWord, &anEndNode))
		{ strcpy(theDefinition, "<not defined>"); return; }
	
	if (anEndNode->mDefPos == (DWORD)-1)
		{ strcpy(theDefinition, "<definition not available>"); return; }
	
	fseek(mDefFile, anEndNode->mDefPos, SEEK_SET);
	fread(&aDefSize, sizeof(DWORD), 1, mDefFile);
	
	if (aDefSize > 511)
		aDefSize = 511;
	fread(theDefinition, sizeof(char), aDefSize, mDefFile);
	theDefinition[aDefSize] = 0;
}
