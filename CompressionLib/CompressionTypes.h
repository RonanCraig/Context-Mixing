#pragma once

namespace compression
{

namespace types
{

namespace node
{
	typedef unsigned char characterType;
	typedef unsigned short countType;
}

typedef unsigned char byte;
typedef unsigned short characterType;
typedef unsigned int countType;

struct ProbRange
{
	// Represents lower bound and upper bound of the range as fractions, sharing a common demoninator.
	countType upper;
	countType lower;
	countType denom;

	ProbRange() {}
	ProbRange(countType up, countType low, countType denom) : upper(up), lower(low), denom(denom) {}
	
	void reduce(ProbRange& range2)
	{
		countType difference = upper - lower;

		countType differenceUpper = range2.upper * difference;
		if (differenceUpper == 0)
		{
			int a = 5;
		}
		countType differenceLower = range2.lower * difference;
		countType differenceDenom = range2.denom * denom;

		denom = differenceDenom * denom;
		lower = (differenceDenom * lower) + (denom * differenceLower);
		upper = (differenceDenom * lower) + (denom * differenceUpper);

	}
};

}

}