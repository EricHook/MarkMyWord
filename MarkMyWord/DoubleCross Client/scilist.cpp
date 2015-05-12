#include "os.h"
#include "scilist.h"

#define GROW_DELTA 10

typedef sciObject *PsciObject;

sciList::sciList()
	{
	numItems = 0;
	numAllocated = GROW_DELTA;
	theList = new PsciObject[GROW_DELTA];
	}

sciList::~sciList()
	{
	Release();
	if(numAllocated)
		delete[] theList;
	}

void sciList::Grow()
	{
	sciObject **newList;
	int i;

	numAllocated += GROW_DELTA;
	newList = new PsciObject[numAllocated];
	for (i = 0; i < numItems; ++i)
		{
		newList[i] = theList[i];
		theList[i] = 0;
		}
	delete[] theList;
	theList = newList;
	}

void sciList::AddToFront(sciObject *obj)
	{
	int i;

	if(numItems == numAllocated)
		{		// Need more room
		Grow();
		}
	for (i = numItems - 1; i >= 0; --i)
		theList[i+1] = theList[i];
	theList[0] = obj;
	++numItems;
	}

void sciList::AddToBack(sciObject *obj)
	{
	if(numItems == numAllocated)
		{		// Need more room
		Grow();
		}
	theList[numItems] = obj;
	++numItems;
	}

void sciList::Delete(sciObject *delObject)
	{
	int i;

	i = IndexOf(delObject);
	if (i > -1)
		{
		for (; i < numItems-1; ++i)
			{
			theList[i] = theList[i+1];
			}
		--numItems;
		}
	}

int sciList::Size()
	{
	return numItems;
	}

Boolean sciList::IsEmpty()
	{
	if (Size() == 0)
		return TRUE;
	else
		return FALSE;
	}

Boolean sciList::Contains(sciObject *obj)
	{
	if (IndexOf(obj) == -1)
		return FALSE;
	else
		return TRUE;
	}

void sciList::Release()
	{
	while (Size())
		{
		Delete(At(Size()-1));
		}
	}

sciObject *sciList::At(int i)
	{
	if ((i < 0) || (i >= numItems))
		return NULL;
	else
		return theList[i];
	}

void sciList::Set(int i, sciObject *obj)
	{
	// If the list is too small, make it bigger.
	while (i >= Size())
		{
		AddToBack(NULL);
		}

	theList[i] = obj;
	if (i > numItems)
		numItems = i;
	}

int sciList::IndexOf(sciObject *obj)
	{
	sciObject *p;
	int i;

	i = 0;
	p = At(i);
	while (p && (obj != p))
		{
		++i;
		p = At(i);
		}
	if (p && (obj == p))
		return i;
	else
		return -1;
	}

void sciList::AddAfter(sciObject *seekObject, sciObject *anObject)
	{
	int i, j;

	i = IndexOf(seekObject);
	if (i > -1)
		{
		if (numItems == numAllocated)
			Grow();
		for (j = numItems - 1; j > i; --j)
			{
			theList[j+1] = theList[j];
			}
		theList[i+1] = anObject;
		++numItems;
		}
	}


void sciList::Sort(int (*compare)(sciObject *, sciObject *))
{
	int i, j;
	sciObject *objectA, *objectB;
	
	for (i = 0 ; i < Size() ; i++)
		for (j = 0 ; j < (Size() - 1) ; j++) {
			objectA = (sciObject *)At(j);
			objectB = (sciObject *)At(j + 1);
			if (compare(objectA, objectB) > 0) {
				Set(j, objectB);
				Set(j + 1, objectA);
			}
		}
}
