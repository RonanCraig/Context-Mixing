#pragma once
namespace compression
{
	struct ProbRange
	{
		ProbRange() {}
		ProbRange(char c, int up, int low, int denom) : character(c), upper(up), lower(low), denom(denom) {}
		// Represents lower bound and upper bound of the range as fractions, sharing a common demoninator.
		char character;
		int upper;
		int lower;
		int denom;
	};
}