#ifndef _list
#define _list

#include "object.h"

class List : public Object {
	public:
		List();
		~List();
		void AddToEnd(Object *pObject);
		void AddToFront(Object *pObject);
		void AddAfter(Object *pSeekObject, Object *pObject);
		void Insert(Object *pObject, int iIndex);
		void Remove(Object *pObject);
		void RemoveIndex(int iIndex);
		Object *GetFirst();
		Object *GetNext();
		Object *GetLast();
		Object *GetPrevious();
		Object *GetItem(int iIndex);
		void DisposeAll();
		void MoveToFront(Object *pObject);
	protected:
		void Grow();		// Allocate more space for Objects.

		int	iListCount;		// The number of items in the list
		BYTE *puhTheList;	// Pointers to the objects in the list
		int iSpaceAllocated;	// How much space allocated for the list
		int iNextIndex;		// Where GetNext looks
		int iPreviousIndex;		// Where GetPrevious looks
		Object **apReturnObject;

	};

#endif
