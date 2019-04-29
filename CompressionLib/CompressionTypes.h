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
	countType upper = 1;
	countType lower = 0;
	countType denom = 1;
	characterType character = 256;

	ProbRange() {}
	ProbRange(countType up, countType low, countType denom, characterType character) : upper(up), lower(low), denom(denom), character(character) {}
};

}

}