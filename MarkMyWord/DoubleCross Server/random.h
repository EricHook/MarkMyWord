#ifndef _Random
#define _Random

class Random
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

char *MoneyToString(int n, char *s);


//======================================================================
// Calculate the Chi-squared value of N random numbers in the range
// [0..r]
//=================d=====================================================
class ChiSquared
{
protected:
	int *freqs;

	int range;
	int N;
	
public:
	ChiSquared(int arange);
	~ChiSquared();

	void Add(int number);
	float Result();
	bool IsSampleBigEnough();
	bool IsRandomEnough();
	int SampleSize() { return N; }
	float GetDiff();
};

#endif // _Random
