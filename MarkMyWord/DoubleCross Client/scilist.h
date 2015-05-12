#ifndef _scilist
#define _scilist

#include "sciobj.h"

class sciList : public sciObject
	{
	public:
		sciList();
		~sciList();
		void AddToFront(sciObject *obj);
		void AddToBack(sciObject *obj);
		void AddAfter(sciObject *seekObject, sciObject *anObject);
		void Set(int i, sciObject *obj);
		void Delete(sciObject *obj);
		int Size();
		Boolean IsEmpty();
		Boolean Contains(sciObject *obj);
		void Release();
		sciObject *At(int i);
		int IndexOf(sciObject *obj);
		void Sort(int (*compare)(sciObject *, sciObject *));
	
	protected:
		void Grow();

		int numItems;
		int numAllocated;
		sciObject **theList;
	};

#endif // _scilist
