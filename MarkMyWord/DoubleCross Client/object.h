#ifndef _object
#define _object

class Object {
	public:
		Object();
		virtual ~Object();

		static int numObjects;
		static int nextObjectNum;
		int objectNum;
	};

#endif
