#ifndef _Random
#define _Random

#include "object.h"

class Random : public Object
	{
	public:
		Random();
		~Random();
		void Seed(int s);
		unsigned int GetRandom();
		unsigned int GetRandom(unsigned int limit);
		void Reset();

	private:
		void LCSeed(unsigned int unSeed);
		unsigned int LCRand();

		int seed;
		unsigned int r250_buffer[250];
		int r250_index;
		unsigned int sunSeed;
	};

#endif // _Random
