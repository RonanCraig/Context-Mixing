#include "PPM.h"
#include <string>

using namespace std;
using namespace compression;

unique_ptr<ProbRanges> PPM::getProbabilityRanges(string input)
{
	// Split input into the character being encoded and it's context.
	char charToEncode = input.back();
	string context = input.substr(0, input.size() - 1);

	unique_ptr<ProbRanges> ranges = make_unique<ProbRanges>(ProbRanges());
	bool contextFound = false;

	// Runs from order of context size to the smallest order -1. Unless the context is found at an order.
	for (int order = context.size(); order > -2 && !contextFound; order--)
	{
		CharTable charTable = getCharTable(order, context);
		if (order == -1)
			ranges->push_back(move(charTable.calculateRange(charToEncode)));
		else
		{
			contextFound = charTable.increaseSymbolCount(charToEncode);
			if (contextFound)
				ranges->push_back(move(charTable.calculateRange(charToEncode)));
			else
				ranges->push_back(move(charTable.calculateRange('&')));
		}
	}

	return ranges;
}

const PPM::CharTable& PPM::getCharTable(const int& order, const string& context)
{
	if (order > -1)
	{
		string tail = context.substr(context.size() - order);
		ContextTable& contextTable = orderTable[order]; // Retrieves the contextTable containing all contexts of the order length.
		return contextTable[tail]; // Retrieves the CharTable for the given context.
	}
	else
		return orderNegativeOneTable; // order -1 table has no context and is therefore stored seperately.
}

PPM::PPM(const string& input)
{
	for (char c : input)
		orderNegativeOneTable.increaseSymbolCount(c);
}

bool PPM::CharTable::increaseSymbolCount(const char& c)
{
	// If the char has not been seen it will be added with a count of 1.
	auto result = charMap.insert(make_pair(c, make_pair(0, 0)));
	auto it = result.first;

	it->second.first++;
	totalCount++;

	// Update cumulative counts.
	auto current = it;
	auto itNext = next(current, 1);
	if (itNext != charMap.end())
		it->second.second = itNext->second.second - itNext->second.first + 1;
	else
		it->second.second = totalCount;

	for (itNext; itNext != charMap.end(); itNext++)
		itNext->second.second++;

	return !result.second;
}

unique_ptr<ProbRange> PPM::CharTable::calculateRange(const char& c)
{
	// Calculates the probablilty range for the given character.
	unique_ptr<ProbRange> probRange = make_unique<ProbRange>(ProbRange());
	pair<int, int> CountandCumCount = charMap.at(c);
	probRange->denom = totalCount;
	probRange->upper = CountandCumCount.second;
	probRange->lower = CountandCumCount.second - CountandCumCount.first;
	return probRange;
}

PPM::CharTable::CharTable(bool isNegativeOrder)
{
	// All CharTables should have a escape character count of 1, except the negativeOrder table.
	if (!isNegativeOrder)
		increaseSymbolCount('&');
};

