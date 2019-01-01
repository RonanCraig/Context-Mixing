#include "ProbabilityModel.h"

using namespace std;
using namespace compression;

ProbabilityModel::CharTable& ProbabilityModel::getCharTable(const string* context)
{
	if (!context)
		return orderNegativeOneTable; // order -1 table has no context and is therefore stored seperately.

	unsigned int order = context->size();
	ContextTable& contextTable = orderTable[order]; // Retrieves the contextTable containing all contexts of the order length.
	return contextTable[*context]; // Retrieves the CharTable for the given context.
}
		
bool ProbabilityModel::CharTable::increaseSymbolCount(const char& c)
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

unique_ptr<ProbRange> ProbabilityModel::CharTable::calculateRange(const char& c) const
{
	// Calculates the probablilty range for the given character.
	unique_ptr<ProbRange> probRange = make_unique<ProbRange>(ProbRange());
	pair<int, int> CountandCumCount = charMap.at(c);
	probRange->character = c;
	probRange->denom = totalCount;
	probRange->upper = CountandCumCount.second;
	probRange->lower = CountandCumCount.second - CountandCumCount.first;
	return probRange;
}