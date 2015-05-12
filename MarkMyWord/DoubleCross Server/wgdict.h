/*===============================================================
	
	wgdict.h - Class for access to dictionary File

    Copyright 1999 by Sierra On-Line Inc., All Rights Reserved

	Author: Eric Fleming

===============================================================*/

#include "random.h"

#define	WGDICT_NUMDICTS		14
#define WGDICT_SLOTHDRSIZE	3
#define WGDICT_MAXSLOTSIZE	19
#define WGDICT_MAXSLOTS		50
#define WGDICT_MAXPGSIZE   (WGDICT_MAXSLOTSIZE * WGDICT_MAXSLOTS + 4)

//define DCDICT_NUMMEMDICTS	6
//define DCDICT_NUMDISKDICTS 8
#define WGDICT_BASSTART			1	// Good start for a basic word
#define WGDICT_MEDSTART			2	// Good start for a medium word
#define WGDICT_ISBASIC			4	// Easiest word list
#define WGDICT_ISMEDIUM			8	// Middle word list
#define WGDICT_STARTSWORD		16
#define WGDICT_ISWORD			32
#define WGDICT_ALLOWPARTIAL		32
#define WGDICT_ISDIRTY			64
#define WGDICT_ALLOWDIRTY		64
#define WGDICT_MEDPART			128	// Legal partial word for medium list
#define WGDICT_MAXWORDLENGTH	15
#define WGDICT_MINWORDLENGTH	2

/*=================================================================
	Classes for Word Game Dictionary Access
===================================================================*/
class Node26
{
public:	
	Node26() :
		mBitSet(0),
		mNodeArray(NULL),
		mDefPos(0)
	{ }
	
	bool IsWord() const { return (mBitSet & 0x80000000) != 0; }
	
	unsigned long mBitSet;
	Node26 *mNodeArray;
	DWORD mDefPos;
};

class DictTree
{
public:
	DictTree();
	~DictTree();
	
	int FileSize(FILE *fptr);
	bool ReadIn(char *theFileName, char *theDefinitionFileName, char *theOffensiveFileName);
	void ReadChunk(FILE *aFileP, Node26 *aNode);
	bool Contains(char *theWord, Node26 **theEndNode = NULL);
	bool IsOffensive(char *theWord);
	void FindDefinition(char *theWord, char *theDefinition);
	
	Node26 mRoot;
	FILE *mDefFile;
	char *offensiveWords;
	
	Node26 *gNodePool;
	int gPoolAllocIdx;
	int gCount;
};

class WGDict
{
public:
	WGDict();
	~WGDict();
	
	bool InitDict(short memLoad = 7);
	void CleanupDict();
	bool FindWordInTree(char *word, short whichDict, bool startsWord, bool isWord, bool allowDirty, char level);
	char *GetNode(unsigned short pgnum, short whichDict);
	bool IsLegalPartialWord(char *str, short len, bool allowDirty = false, char level = 2);
	bool IsLegalWord(char *str, short minWordSize, bool allowDirty = false, char level = 2);
	bool IsLegalWordStart(char *str, short len, char level = 2);
	char *GetRandomWord(short lenStart, short lenEnd = 0, short flags = 0);
	unsigned short NumPages(short lenStart);
	
	// data
	char *m_memDict[6]; // memory based dictionaries - 2 to 7 letter words
	FILE *m_dictionary[10]; // Disk based dictionaries - 8 letter words and over
	char *m_pageBuf;
	short m_memLoad; // all dicts with this count and below in memory
	int m_flSize[WGDICT_NUMDICTS]; // size of each dictionary
	char *m_offensiveWords;

	DictTree *definitions;
};
