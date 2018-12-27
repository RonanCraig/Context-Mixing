#pragma once
#include <map>
#include <memory>
#include "ProbRange.h"

namespace compression
{

class PPM
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

public:
	// Iterates through each context until the character has been seen before at that context or -1 context.
	// Updates the count for the character at each context reached.
	// Creates a probablilty range for the escape character if the character has not been seen before at that context.
	// Creates a probability range for the character to encode if the character has been seen before.
	std::unique_ptr<ProbRanges> getProbabilityRanges(std::string input);
	PPM(const std::string& input);

private:
	OrderTable orderTable;
	CharTable orderNegativeOneTable = CharTable(true);
	const CharTable& getCharTable(const int& order, const std::string& context);
};

}