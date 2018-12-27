#pragma once
#include <memory>
#include <vector>

namespace compression
{

struct ProbRange
{
	// Represents lower bound and upper bound of the range as fractions, sharing a common demoninator.
	int upper;
	int lower;
	int denom;
};

typedef std::vector<std::unique_ptr<ProbRange>> ProbRanges;

}