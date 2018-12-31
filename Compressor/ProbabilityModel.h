#pragma once
#include <memory>
#include <vector>
#include <map>

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

class ProbabilityModel
{
	class CharTable
	{
	private:
		int totalCount = 0;
		std::map<char, std::pair<int, int>> charMap;

	public:
		CharTable(bool isNegativeOrder = false);
		bool increaseSymbolCount(const char& c);
		std::unique_ptr<ProbRange> calculateRange(const char& c);
	};

	typedef std::map<std::string, CharTable> ContextTable;
	typedef std::map<int, ContextTable> OrderTable;

private:
	OrderTable orderTable;
	CharTable orderNegativeOneTable = CharTable(true);

public:
	bool increaseSymbolCount(const char& c, const std::string* context = nullptr);
	std::unique_ptr<ProbRange> getRange(const char& c, const std::string* context = nullptr);

private:
	CharTable& getCharTable(const std::string* context = nullptr);

};

}