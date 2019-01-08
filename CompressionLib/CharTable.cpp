#include "CharTable.h"

using namespace std;
using namespace compression;

void CharTable::increaseSymbolCount(const char& c)
{
	auto it = charMap.insert(make_pair(c, make_pair(0, 0))).first;

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
}

/*
pair<ProbabilityModel::CharTable::CharMap::iterator, bool> ProbabilityModel::CharTable::getCharMapIteratorAtChar(const char& c)
{
	auto result = charMap.insert(make_pair(c, make_pair(0, 0)));
	return result;
}

void ProbabilityModel::CharTable::increaseSymbolCount(const char& c)
{
	increaseSymbolCount(getCharMapIteratorAtChar(c).first);
}

void ProbabilityModel::CharTable::increaseSymbolCount(ProbabilityModel::CharTable::CharMap::iterator it)
{
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

unique_ptr<ProbRange> ProbabilityModel::CharTable::calculateRange(const int& count) const
{
	// First finds which character the count belongs to.
	char c;
	unique_ptr<ProbRange> probRange = make_unique<ProbRange>(ProbRange());
	for (auto it = charMap.begin(); it != charMap.end(); it++)
	{
		if (count < it->second.second)
		{
			c = it->first;
			probRange = move(calculateRange(c));
			return probRange;
		}
	}
	return probRange;
}*/