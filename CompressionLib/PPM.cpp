#include "PPM.h"
#include <fstream>
#include <set>

using namespace std;
using namespace compression;

// TODO: Reduce duplicated code between encode, decode and update.

void PPM::encode(const char& charToEncode, ArithmeticEncoder& encoder)
{
	bool contextFound = false;

	// Runs from order of context size to the smallest order -1. Unless the context is found at an order.
	for (int order = config::MaxOrderSize; !contextFound; order--)
	{
		unique_ptr<ProbRange> range;
		string currentContext = context.substr(context.length() - (order >= 0 ? order : 0), context.length());
		PPMCharTable& charTable = getCharTable(currentContext, order);
		range = charTable.getRange(charToEncode);
		if (range->character == charToEncode)
			contextFound = true;
		encoder.encode(move(range));
	}
}

char PPM::decode(ArithmeticDecoder& decoder)
{
	bool contextFound = false;
	char decodedChar;

	// Runs from order of context size to the smallest order -1. Unless the context is found at an order.
	for (int order = config::MaxOrderSize; !contextFound; order--)
	{
		int count;
		unique_ptr<ProbRange> range;
		string currentContext = context.substr(context.length() - (order >= 0 ? order : 0), context.length());
		PPMCharTable& charTable = getCharTable(currentContext, order);

		count = decoder.getCount(charTable.getTotalCount());
		range = charTable.getRange(count);
		decodedChar = range->character;
		if (decodedChar != config::EscapeCharacter)
			contextFound = true;
		decoder.decode(move(range));
	}

	return decodedChar;
}

void PPM::update(const char& charToUpdate)
{
	if (charToUpdate == config::EndCharacter)
		return;

	bool contextFound = false;

	// Runs from order of context size to the smallest order -1. Unless the context is found at an order.
	for (int order = config::MaxOrderSize; !contextFound; order--)
	{
		unique_ptr<ProbRange> range;
		string currentContext = context.substr(context.length() - (order >= 0 ? order : 0), context.length());
		PPMCharTable& charTable = getCharTable(currentContext, order);
		range = charTable.getRange(charToUpdate);
		if (range->character == charToUpdate)
			contextFound = true;

		if (order > -1)
			charTable.increaseSymbolCount(charToUpdate);
	}

	context.push_back(charToUpdate);
	context.erase(0, 1);
	nextCharProbabilityDistribution = getCharTable(context, config::MaxOrderSize);
}

PPM::PPMCharTable& PPM::getCharTable(const string& context, const int& order)
{
	if (order == -1)
		return orderNegativeOneTable; // order -1 table has no context and is therefore stored seperately.

	ContextTable& contextTable = orderTable[order]; // Retrieves the contextTable containing all contexts of the order length.
	return contextTable[context]; // Retrieves the CharTable for the given context.
}

PPM::PPM()
{
	// Finds all the unique characters in the message and adds them to the alphabet.
	ifstream inputFileStream("C:/Users/Ronan/source/repos/ContextMixing/Debug/input");
	set<char> uniqueChars;
	char c;
	while (inputFileStream.get(c))
		uniqueChars.insert(c);
	uniqueChars.insert(config::EndCharacter);
	for (char c : uniqueChars)
		orderNegativeOneTable.increaseSymbolCount(c);

	for (int i = 0; i < config::MaxOrderSize; i++)
		context += " ";
}

#ifdef _DEBUG
void PPM::outputDebug(ofstream& outputFileStream)
{
	outputFileStream << "\n" << "-1";
	orderNegativeOneTable.outputDebug(outputFileStream);
	for (auto orderIt = orderTable.begin(); orderIt != orderTable.end(); orderIt++)
	{
		outputFileStream << "\n" << orderIt->first;
		for (auto contextIt = orderIt->second.begin(); contextIt != orderIt->second.end(); contextIt++)
		{
			outputFileStream << "\n" << "\t" << "\"" << contextIt->first.c_str() << "\"";
			CharTable charTable = contextIt->second;
			charTable.outputDebug(outputFileStream);
		}
	}
}
#endif

unique_ptr<ProbRange> PPM::PPMCharTable::getRange(const char& c)
{
	unique_ptr<ProbRange> range;
	auto it = charMap.find(c);
	if (it == charMap.end()) // Did not exist.
	{
		it = charMap.find(config::EscapeCharacter);
		range = move(calculateRange(it));
	}
	else
		range = move(calculateRange(it));

	return range;
}

unique_ptr<ProbRange> PPM::PPMCharTable::getRange(const int& count)
{
	auto it = charMap.begin();
	while (!(count < it->second.second))
		it++;

	unique_ptr<ProbRange> range = move(calculateRange(it));
	return range;
}

std::unique_ptr<ProbRange>  PPM::PPMCharTable::calculateRange(CharMap::iterator it)
{
	// Calculates the probablilty range for the given character.
	unique_ptr<ProbRange> probRange = make_unique<ProbRange>(ProbRange());
	pair<int, int> CountandCumCount = it->second;
	probRange->character = it->first;
	probRange->denom = totalCount;
	probRange->upper = CountandCumCount.second;
	probRange->lower = CountandCumCount.second - CountandCumCount.first;
	return probRange;
}

