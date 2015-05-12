// DirtyWord.h : header file
//

#ifndef DIRTYWORD_H
#define DIRTYWORD_H

class CWord
{
	friend class CWordArray;
// Construction
public:
	CWord();
	CWord(char c);
	CWord(CWord& word);
	~CWord();
	
//Attributes
private:
	char *m_szWord;
	int m_nSize;
		
// Implementation
public:
	CWord& operator=(CWord& stringSrc);
	CWord& operator=(char *psz);
	int GetLength();
	char GetAt(int i);
	void SetAt(int i, char c);
	void MakeUpper();
	void MakeLower();
	char *GetBuffer(int nMin);
	void ReleaseBuffer(int nNew = -1);

protected:
};

class CWordArray
{
// Construction
public:
	CWordArray();
	~CWordArray();
	
//Attributes
private:
	CWord *m_pArray;
	int m_nSize;

// Implementation
public:
	int GetSize();
	void SetAtGrow(int nIndex, char *newElement);
	void RemoveAll();
	char *GetAt(int i);

protected:
};

class CDirtyWordFilter
{
// Construction
public:
	CDirtyWordFilter(char *pFile);
	~CDirtyWordFilter();
	
//Attributes
private:
	static int s_nTotalInstances;
	static CWordArray s_DirtyWordArray;
	static CWordArray s_ReplacementWordArray;
	// Keeps a running count of all dirty words sent through CountDirtyWordsInString().
	static int s_nDirtyWordTotal;
	
// Implementation
public:
	void UpdateString(char *strInputText, int nMax);	//used to find & replace dirty words
	int CountDirtyWordsInString(CWord & strInputText);	//used to count dirty words without replacing
	int GetDirtyWordTotal(void);
	void ClearDirtyWordTotal(void);

protected:
	void CheckDirtyWord(CWord & strInputText,
						CWord & strDirtyWord,
						long & lStartLocation,
						long & lEndLocation);

	void CheckDirtyWordAtPosition(	CWord & strInputText,
									CWord & strDirtyWord,
									long lStartLocation,
									long & lEndLocation);

	void ReplaceWord(CWord & strInputText,
					 CWord & strReplacementWord,
					 long lStartLocation,
					 long lEndLocation);

	void DeleteChars(	CWord & strInputText,
						long lStartLocation, 
						long lEndLocation);

	void InsertChars(	CWord & strInputText,
						CWord strInsert,
						long lStartLocation);
};

#endif //DIRTYWORD_H
