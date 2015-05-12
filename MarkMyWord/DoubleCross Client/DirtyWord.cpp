/******************************************************************************

   !  N O T I C E  !  N O T I C E  !  N O T I C E  !  N O T I C E  !

           ©1999 Sierra On-Line, Inc.  All Rights Reserved.
                   U.S. and foreign patents pending.

                        THIS SOFTWARE BELONGS TO
                          Sierra On-Line, Inc.
   IT IS CONSIDERED A TRADE SECRET AND IS NOT TO BE DIVULGED OR USED
      BY PARTIES WHO HAVE NOT RECEIVED WRITTEN AUTHORIZATION FROM
                          Sierra On-Line, Inc.
                     3380 146th Place SE, Suite 300
                        Bellevue, WA  98007-6472
                              425 649-9800

            Federal law provides severe civil penalties for
                 misuse or violation of trade secrets.

******************************************************************************/

/*****************************************************************************

File Name:	DirtyWord.cpp
Purpose:	This object will take a string via member function UpdateString()
			and filter out any profanities (according to an external resource),
			replacing them with inoffensive phrases.  Some features of this 
			facility are:

			- Searches are case insensitive.
			- Whitespace in the string is ignored.
			- The string is searched recursively.
			- Case of the original word replaced is preserved.

			This code was adapted from classes used in Dr. Brain 4.

 *****************************************************************************/
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <windows.h>
#include "DirtyWord.h"

#define DW_GROW_VALUE	20

// Static member classes.
int CDirtyWordFilter::s_nTotalInstances = 0;
CWordArray CDirtyWordFilter::s_DirtyWordArray;
CWordArray CDirtyWordFilter::s_ReplacementWordArray;
int CDirtyWordFilter::s_nDirtyWordTotal = 0;

CDirtyWordFilter::CDirtyWordFilter(char *pFile)
{
	if (!pFile)
		return;

	s_nTotalInstances++;

	// Initialize arrays if this is first instance of this class.
	if (s_nTotalInstances == 1)
	{
		// Read dirty & replacement words into arrays from message table resource.
		DWORD nCount = 0;
		LPVOID pTableWord = NULL;
		char strTemp[64], *p;
		FILE *fptr;
		BOOL readword = FALSE, more = TRUE;

		fptr = fopen(pFile, "rb");
		if (!fptr)
		{
			s_nTotalInstances = 0;
			return;
		}

		while (more)
		{
			p = strTemp;
			while(!readword && (more = (fread(p, 1, 1, fptr) == 1)))
			{
				if (!*p)
					readword = TRUE;
				else
					*p++ += 5;
			}
			s_DirtyWordArray.SetAtGrow(nCount, strTemp);
				
			p = strTemp;
			readword = FALSE;
			while(!readword && fread(p, 1, 1, fptr))
			{
				if (!*p)
					readword = TRUE;
				else
					*p++ += 5;
			}
			readword = FALSE;
			s_ReplacementWordArray.SetAtGrow(nCount, strTemp);
			nCount++;
		}
	}
}

CDirtyWordFilter::~CDirtyWordFilter()
{
	s_nTotalInstances--;

	// Flush the arrays if this was the last instance of this class.
	if (s_nTotalInstances == 0)
	{
		s_DirtyWordArray.RemoveAll();
		s_ReplacementWordArray.RemoveAll();
	}
}

void CDirtyWordFilter::UpdateString(char *strInput, int nMax)
{
	CWord strFinalString;
	CWord strUppercaseString;
	CWord strDirtyWord;
	CWord strReplacementWord;
	long lStartLocation, lEndLocation;
	BOOL bDone = FALSE;
	CWord strInputText;
	strInputText = strInput;
	strFinalString = strInput;

	while (bDone == FALSE) 
	{
		bDone = TRUE;
		strUppercaseString = strFinalString;
		strUppercaseString.MakeUpper();
		for (int i = 0; i < s_DirtyWordArray.GetSize(); i++) 
		{
			strDirtyWord = s_DirtyWordArray.GetAt(i);
			CheckDirtyWord(	strUppercaseString,
							strDirtyWord,
							lStartLocation,
							lEndLocation);
			if (lStartLocation >= 0) 
			{
				bDone = FALSE;
				strReplacementWord = s_ReplacementWordArray.GetAt(i);
				ReplaceWord(strFinalString,
							strReplacementWord,
							lStartLocation,
							lEndLocation);
				break;
			}
		}
	}

	strncpy(strInput, strFinalString.GetBuffer(strFinalString.GetLength()), nMax);
	strInput[nMax-1] = 0;
}

int CDirtyWordFilter::CountDirtyWordsInString(CWord & strInputText)
{
	CWord strUppercaseString = strInputText;
	CWord strDirtyWord;
	int nInputLength = strInputText.GetLength();
	long lStartLocation, lEndLocation;
	int nDirtyWordCount = 0;

	strUppercaseString.MakeUpper();

	for (int i = 0; i < s_DirtyWordArray.GetSize(); i++) 
	{
		lStartLocation = 0;
		strDirtyWord = s_DirtyWordArray.GetAt(i);

		do //while (lStartLocation < nInputLength)
		{
			CheckDirtyWordAtPosition(	strUppercaseString, 
										strDirtyWord,
										lStartLocation,
										lEndLocation);
			if (lEndLocation >= 0) 
			{
				nDirtyWordCount++;
				lStartLocation = lEndLocation + 1;
			}
			else
			{
				lStartLocation++;
			}
		} while (lStartLocation < nInputLength);
	}

	s_nDirtyWordTotal += nDirtyWordCount;
	return nDirtyWordCount;
}

int CDirtyWordFilter::GetDirtyWordTotal(void)
{
	return s_nDirtyWordTotal;
}

void CDirtyWordFilter::ClearDirtyWordTotal(void)
{
	s_nDirtyWordTotal = 0;
}

void CDirtyWordFilter::CheckDirtyWord(	CWord & strInputText,
										CWord & strDirtyWord,
										long & lStartLocation,
										long & lEndLocation)
{
	long lInputLength;

	lInputLength = strInputText.GetLength();
	for (lStartLocation = 0; lStartLocation < lInputLength; lStartLocation++) 
	{
		CheckDirtyWordAtPosition(	strInputText, 
									strDirtyWord,
									lStartLocation,
									lEndLocation);
		if (lEndLocation >= 0)
		{
			return;
		}
	}

	lStartLocation = -1;
	lEndLocation = -1;
}

void CDirtyWordFilter::CheckDirtyWordAtPosition(CWord & strInputText,
												CWord & strDirtyWord,
												long lStartLocation,
												long & lEndLocation)
{
	long lInputLength;
	long lDirtyWordLength;
	char cInput;
	char cDirtyWord;
	int nCurrentInputChar = lStartLocation;
	int nCurrentDirtyWordChar = 0;
	
	cInput = strInputText.GetAt(nCurrentInputChar);
	cDirtyWord = strDirtyWord.GetAt(nCurrentDirtyWordChar);

	if (cInput == VK_SPACE) 
	{
		lEndLocation = -1;
		return;
	}

	lInputLength = strInputText.GetLength();
	lDirtyWordLength = strDirtyWord.GetLength();

	lStartLocation--;
	nCurrentInputChar--;
	nCurrentDirtyWordChar--;
	for (int i = 0; i < lDirtyWordLength; i++) 
	{
		lStartLocation++;
		if (lStartLocation == lInputLength) 
		{
			lEndLocation = -1;
			return;
		}

		nCurrentInputChar++;
		nCurrentDirtyWordChar++;
		cInput = strInputText.GetAt(nCurrentInputChar);
		cDirtyWord = strDirtyWord.GetAt(nCurrentDirtyWordChar);

		while (cInput == VK_SPACE) 
		{
			lStartLocation++;
			if (lStartLocation == lInputLength) 
			{
				lEndLocation = -1;
				return;
			}

			nCurrentInputChar++;
			cInput = strInputText.GetAt(nCurrentInputChar);
		}

		if (cInput != cDirtyWord) 
		{
			lEndLocation = -1;
			return;
		}
	}

	lEndLocation = lStartLocation;
}

void CDirtyWordFilter::ReplaceWord(	CWord & strInputText,
									CWord & strReplacementWord,
									long lStartLocation,
									long lEndLocation)
{
	BOOL bFirstLetterCap;
	BOOL bSecondLetterCap;
	CWord strTemp;
	BOOL bFirstLetter;
	char cChar;

	bFirstLetterCap = ::isupper(strInputText.GetAt(lStartLocation));
	bSecondLetterCap = ::isupper(strInputText.GetAt(lStartLocation + 1));

	strTemp = strReplacementWord;

	bFirstLetter = TRUE;
	for (int i = 0; i < strTemp.GetLength(); i++) 
	{
		cChar = strTemp.GetAt(i);
		if(cChar == VK_SPACE)
		{
			bFirstLetter = TRUE;
		}
		else 
		{
			CWord strConvertChar(strTemp.GetAt(i));
			if(bFirstLetter) 
			{
				if(bFirstLetterCap)
				{
					strConvertChar.MakeUpper();
				}
				else
				{
					strConvertChar.MakeLower();
				}

				bFirstLetter = FALSE;
			}
			else if(bSecondLetterCap)
			{
				strConvertChar.MakeUpper();
			}
			else
			{
				strConvertChar.MakeLower();
			}

			strTemp.SetAt(i, strConvertChar.GetAt(0));
		}
	}

	DeleteChars(strInputText, lStartLocation, lEndLocation);
	InsertChars(strInputText, strTemp, lStartLocation);
}

void CDirtyWordFilter::DeleteChars(	CWord & strInputText,
									long lStartLocation, 
									long lEndLocation)
{
	if (lEndLocation < lStartLocation)
	{
		return;
	}

	CWord strTemp = strInputText;
	int nStartLength = strInputText.GetLength();
	char * szTemp = strTemp.GetBuffer(strTemp.GetLength());
	memmove(	szTemp + lStartLocation, 
				szTemp + lEndLocation + 1, 
				nStartLength - lEndLocation);
	strTemp.ReleaseBuffer(nStartLength - (lEndLocation - lStartLocation + 1));

	strInputText = strTemp;
}

void CDirtyWordFilter::InsertChars(	CWord & strInputText,
									CWord strInsert,
									long lStartLocation)
{
	int nInputLength = strInputText.GetLength();
	int nInsertLength = strInsert.GetLength();
	char * szInput = strInputText.GetBuffer(nInputLength + nInsertLength);
	char * szInsert = strInsert.GetBuffer(nInsertLength);
	strInsert.ReleaseBuffer();
	memmove(	szInput + lStartLocation + nInsertLength,
				szInput + lStartLocation,
				nInputLength - lStartLocation + 1);	//add one for the terminator!
	memmove(	szInput + lStartLocation,
				szInsert,
				nInsertLength);
	strInputText.ReleaseBuffer();
	strInsert.ReleaseBuffer();
}

CWord::CWord()
{
	m_szWord = NULL;
	m_nSize = 0;
}

CWord::CWord(CWord& word)
{
	m_szWord = NULL;
	m_nSize = 0;
	*this = word;
}

CWord::CWord(char c)
{
	m_szWord = new char[2];
	m_szWord[0] = c;
	m_szWord[1] = 0;
	m_nSize = 1;
}

CWord::~CWord()
{
	if (m_szWord)
		delete [] m_szWord;
}

char CWord::GetAt(int i)
{
	return i <= m_nSize ? m_szWord[i] : 0;
}

void CWord::SetAt(int i, char c)
{
	if (i <= m_nSize)
	{
		m_szWord[i] = c;
	}
	else
	{
		char *p = new char[i+1];
		strcpy(p, m_szWord);
		m_nSize = i;
		p[i] = c;
		delete [] m_szWord;
		m_szWord = p;
	}
}

int CWord::GetLength()
{
	return m_nSize;
}

void CWord::MakeUpper()
{
	for (int i = 0; i < m_nSize; i++)
	{
		m_szWord[i] = toupper(m_szWord[i]);
	}
}

void CWord::MakeLower()
{
	for (int i = 0; i < m_nSize; i++)
	{
		m_szWord[i] = tolower(m_szWord[i]);
	}
}

char *CWord::GetBuffer(int nMin)
{
	if (nMin > m_nSize)
	{
		char *pNew = new char[nMin+1];
		memset(pNew, 0, nMin+1);
		m_nSize = nMin;
		if (m_szWord)
		{
			strcpy(pNew, m_szWord);
			delete [] m_szWord;
		}
		m_szWord = pNew;
	}
	
	return m_szWord ? m_szWord : "";
}

void CWord::ReleaseBuffer(int nNew)
{
	if (nNew == -1)
	{
		m_nSize = strlen(m_szWord);
		nNew = m_nSize;
	}
	else
	{
		m_nSize = nNew;
	}
	m_szWord[nNew] = 0;
}

CWord& CWord::operator=(CWord& word)
{
	if (m_szWord)
		delete [] m_szWord;
	m_nSize = word.GetLength();

	if (word.GetLength())
	{
		m_szWord = new char[word.GetLength()+1];

		strcpy(m_szWord, word.m_szWord);
	}
	else
	{
		m_szWord = NULL;		
	}
	return *this;
}

CWord& CWord::operator=(char *lpsz)
{
	if (lpsz && strlen(lpsz))
	{
		if (m_szWord)
			delete [] m_szWord;

		m_szWord = new char[strlen(lpsz)+1];

		strcpy(m_szWord, lpsz);
		m_nSize = strlen(lpsz);
	}
	return *this;
}

CWordArray::CWordArray()
{
	m_pArray = NULL;
	m_nSize = 0;
}

CWordArray::~CWordArray()
{
	RemoveAll();
}

int CWordArray::GetSize()
{
	return m_nSize;
}

void CWordArray::SetAtGrow(int nIndex, char *newElement)
{
	CWord *pNew;
	int i;

	if (nIndex < m_nSize)
	{
		m_pArray[nIndex] = newElement;
	}
	else
	{
		pNew = new CWord[nIndex + DW_GROW_VALUE];

		for (i = 0; i < nIndex + DW_GROW_VALUE; i++)
		{
			if (i < m_nSize)
				pNew[i] = m_pArray[i];
		}

		pNew[nIndex] = newElement;
		delete [] m_pArray;
		
		m_pArray = pNew;
		m_nSize = nIndex + DW_GROW_VALUE;
	}
	return;
}

void CWordArray::RemoveAll()
{
	delete [] m_pArray;
	m_pArray = NULL;
	m_nSize = 0;
}

char *CWordArray::GetAt(int i)
{
	if (i < m_nSize)
		return m_pArray[i].m_szWord;
	else
		return NULL;
}
