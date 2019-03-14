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
	ProbRange() {}
	ProbRange(countType up, countType low, countType denom) : upper(up), lower(low), denom(denom) {}
	// Represents lower bound and upper bound of the range as fractions, sharing a common demoninator.
	countType upper;
	countType lower;
	countType denom;
};

}

}