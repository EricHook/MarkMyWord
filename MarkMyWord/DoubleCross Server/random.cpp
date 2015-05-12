#include "miniwin.h"
#include <math.h>
#include "random.h"

Random::Random()
	{
	Seed(0);
	}

Random::~Random()
	{
	}

void Random::Seed(int s)
	{
	int j,
	 k;
	unsigned int mask;
	unsigned int msb;
	unsigned int unSeed;

	seed = s;
	unSeed = (unsigned int)s;
	LCSeed(unSeed);
	r250_index = 0;
	for (j = 0; j < 250; j++)
		{	/* Fill the r250 buffer with 32-bit values */
		// the LC generator doesn't make full 32-bit values, so we'll get two
		// and XOR them together, shifted slightly
		r250_buffer[j] = LCRand() ^ (LCRand() << 8);
		}

	msb = 0x80000000;				/* To turn on the diagonal bit   */
	mask = 0xffffffff;				/* To turn off the leftmost bits */
	for (j = 0; j < 32; j++)
		{
		k = 5 * j + 3;			/* Select a word to operate on        */
		r250_buffer[k] &= mask;	/* Turn off bits left of the diagonal */
		r250_buffer[k] |= msb;	/* Turn on the diagonal bit           */
		mask >>= 1;
		msb >>= 1;
		}
	}

unsigned int Random::GetRandom(unsigned int limit)
	{
	unsigned int mask;
	unsigned int r;
	unsigned int smallR;

	// Grab a random number.
	// Adjust it for the limit.
	// Get another random number if the first one (after being cut down by
	// the next higher power of 2) is still above the limit.
	// This will give a uniform random number between 0 and limit.
	// Simply modding gives lower numbers a greater frequency.

	smallR = limit;
	while (smallR >= limit)
		{
		r = GetRandom();
		mask = 1;
		while (limit > mask)
			{
			mask = (mask << 1) | 1;
			}
		smallR = r & mask;
		}

	ASSERT(_WARN_, smallR < limit);

	return smallR;
	}

unsigned int Random::GetRandom()
	{
	int j;
	unsigned int new_rand;

	if (r250_index >= 147)
		j = r250_index - 147;	/* Wrap pointer around */
	else
		j = r250_index + 103;

	new_rand = r250_buffer[r250_index] ^ r250_buffer[j];
	r250_buffer[r250_index] = new_rand;
	if (r250_index >= 249)		/* Increment pointer for next time */
		r250_index = 0;
	else
		r250_index++;

	return new_rand;
	}

void Random::Reset()
	{
	Seed(seed);
	}

#define a      16807			/* 7^5 */
#define m      2147483647		/* 2^31 - 1 */
#define q      127773			/* m/a */
#define r      2836				/* m%a */

void Random::LCSeed(unsigned int unSeed)
	{
	sunSeed = unSeed;
	}

unsigned int Random::LCRand(void)
	{
	int tmp_seed;

	tmp_seed = a * (sunSeed % q) - r * (sunSeed / q);
	if (tmp_seed >= 0)
		sunSeed = tmp_seed;
	else
		sunSeed = tmp_seed + m;

	return sunSeed;
	}

char *MoneyToString(int n, char *s)
{
	if (n > 999999999)
		sprintf(s, "$%d,%.3d,%.3d,%.3d", n / 1000000000, (n % 1000000000) / 1000000, (n % 1000000) / 1000, n % 1000);
	else {
		if (n > 999999)
			sprintf(s, "$%d,%.3d,%.3d", n / 1000000, (n % 1000000) / 1000, n % 1000);
		else {
			if (n > 999)
				sprintf(s, "$%d,%.3d", n / 1000, n % 1000);
			else
				sprintf(s, "$%d", n);
		}
	}
	
	return s;
}

//======================================================================
// ctor
//======================================================================
ChiSquared::ChiSquared(int arange)
{
	freqs = NULL;
	N = 0;

	range = arange;
	freqs = new int[range];
	
	memset(freqs, 0, sizeof(DWORD) * range);
}

// dtor
ChiSquared::~ChiSquared()
{
	delete freqs;
	freqs = NULL;
}

//
void ChiSquared::Add(int number)
{
	assert(number >= 0 && number < range);

	N++;
	freqs[number]++;
}

//
float ChiSquared::Result()
{
	int i, t;

	// calculated the sum of the squares
	for (i = 0, t = 0; i < range; i++)
		t += freqs[i] * freqs[i];
	
	return (float) ((range * t / N) - N);
}

//
bool ChiSquared::IsSampleBigEnough()
{
	if (N > 10 * range)
		return true;
	
	return false;
}

//
bool ChiSquared::IsRandomEnough()
{
	float result, diff;

	result = Result();
	diff = fabs(range - result);

	if (diff <= (2 * sqrt(range)))
		return true;
		
	return false;
}

float ChiSquared::GetDiff()
{
	float result, diff;

	result = Result();
	diff = fabs(range - result);
	return diff;
}

