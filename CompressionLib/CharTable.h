#pragma once
#include <memory>
#include <vector>
#include <map>

namespace compression
{

struct ProbRange
{
	// Represents lower bound and upper bound of the range as fractions, sharing a common demoninator.
	char character;
	int upper;
	int lower;
	int denom;
};

typedef std::vector<std::unique_ptr<ProbRange>> ProbRanges;

class CharTable
{
	//Attributes
protected:
	typedef std::map<char, std::pair<int, int>> CharMap;
	int totalCount = 0;
	CharMap charMap;

	//Methods
public:
	const int& getTotalCount() const { return totalCount; }
	void increaseSymbolCount(const char& c);
	std::unique_ptr<ProbRange> calculateRange(const char& c);
};

}