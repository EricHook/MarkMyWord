#include "os.h"
#include "list.h"
#include <string.h>

#define iListDelta 10

/* Make a new list */
List::List()
	{
	iListCount = 0;
    /* Make space for a group of pointers */
	iSpaceAllocated = iListDelta;
	puhTheList = new BYTE[iListDelta * sizeof(Object *)];
	apReturnObject = NULL;
	}

List::~List()
	{
	DisposeAll();
	if(iSpaceAllocated)
		delete[] puhTheList;
	}

void List::Grow()
	{
	int oldSize, newSize;
	BYTE *newList;

	oldSize = iSpaceAllocated * sizeof(Object *);
	newSize = (iSpaceAllocated + iListDelta) * sizeof(Object *);
	newList = new BYTE[newSize];
	memset(newList, 0, newSize);
	memcpy(newList, puhTheList, oldSize);
	delete[] puhTheList;
	puhTheList = newList;
	iSpaceAllocated += iListDelta;
	}

void List::AddToEnd(Object *pObject)
	{
	Object  * *p1;

	if(iListCount == iSpaceAllocated)
		{		// Need more room
		Grow();
		}
	p1 = (Object * *) puhTheList;
	p1[iListCount++] = (Object *) pObject;
	}

void List::AddToFront(Object *pObject)
	{
	Insert(pObject, 0);
	}

void List::AddAfter(Object *seekObject, Object *pObject)
	{
	Object *p;

	// Try to find the seekObject.
	p = GetFirst();
	while (p && (seekObject != p))
		p = GetNext();
	if (p == seekObject)
		{
		// seekObject found
		Insert(pObject, iNextIndex);
		}
	}

void List::Insert(Object *pObject, int iIndex)
	{
	Object * *p1;
	int i;

	if(iListCount == iSpaceAllocated)
		{		// Need more room
		Grow();
		}
	p1 = (Object * *) puhTheList;
	for(i = iListCount-1; i >= iIndex; --i)	// Open up a space
		p1[i+1] = p1[i];
	++iListCount;
	p1[iIndex] = (Object *) pObject;	// Put the pointer in the space
	}

void List::Remove(Object *pObject)
	{
	Object *p;

	p = GetFirst();
	while (p && (pObject != p))
		p = GetNext();
	if (p == pObject)
		RemoveIndex(iNextIndex - 1);
	}

void List::RemoveIndex(int iIndex)
	{
	int i;
	Object * *p;

	if(iListCount && (iIndex >= 0) && (iIndex < iListCount))
		{
		p = (Object * *) puhTheList;
		--iListCount;
		for(i=iIndex; i<iListCount; ++i)
			p[i] = p[i+1];
		}
	}

Object *List::GetFirst()
	{
	iNextIndex = 1;
	iPreviousIndex = 0;
	if(iNextIndex <= iListCount)
		{
		apReturnObject = (Object **)puhTheList;
		return *apReturnObject;
		}
	else
		{
		apReturnObject = NULL;
		return NULL;
		}
	}

Object *List::GetLast()
	{
	if (iListCount > 0)
		{
		iPreviousIndex = iListCount - 1;
		apReturnObject = &(((Object**)puhTheList)[iPreviousIndex]);
		return *apReturnObject;
		}
	else
		{
		apReturnObject = NULL;
		return NULL;
		}
	}

Object *List::GetNext()
	{
	if(iNextIndex < iListCount)
		{
		++iNextIndex;
		return *(++apReturnObject);
		}
	else
		{
		return NULL;
		}
	}

Object *List::GetPrevious()
	{
	if(iPreviousIndex > 0)
		{
		--iPreviousIndex;
		return *(--apReturnObject);
		}
	else
		{
		return NULL;
		}
	}

void List::DisposeAll()
	{
	Object * *p;

	p = (Object * *) puhTheList;
	while (iListCount)
		delete p[--iListCount];
	}

Object *List::GetItem(int iIndex)
	{
	Object *pReturnObject;
	Object * *p;

	p = (Object * *) puhTheList;
	if(iIndex < iListCount)
		pReturnObject = p[iIndex];
	else
		pReturnObject = NULL;
	return pReturnObject;
	}

void List::MoveToFront(Object *pObject)
	{
	Remove(pObject);
	Insert(pObject, 0);
	}
