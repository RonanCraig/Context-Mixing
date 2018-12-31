#include "PPM.h"
#include <string>
#include <set>

using namespace std;
using namespace compression;

unique_ptr<ProbRanges> PPM::getProbabilityRanges(const unsigned int& position)
{
	char charToEncode = message[position];

	unique_ptr<ProbRanges> ranges = make_unique<ProbRanges>(ProbRanges());
	bool contextFound = false;

	// Runs from order of context size to the smallest order -1. Unless the context is found at an order.
	for (int order = MaxOrderSize; order > -2 && !contextFound; order--)
	{
		string context = getMessageContext(position, order);
		if (order == -1)
			ranges->push_back(move(model.getRange(charToEncode)));
		else
		{
			contextFound = model.increaseSymbolCount(charToEncode, &context);
			if (contextFound)
				ranges->push_back(move(model.getRange(charToEncode, &context)));
			else
				ranges->push_back(move(model.getRange('&', &context)));
		}
	}

	return ranges;
}

std::string PPM::getMessageContext(const unsigned int& position, const unsigned int& order)
{
	int start = position - order;
	if (start < 0)
		start = 0;
	return message.substr(start, position + 1);
}

PPM::PPM(const string& message) : message(message)
{
	// Finds all the unique characters in the message and adds them to the alphabet.
	set<char> uniqueChars(message.begin(), message.end());
	for (char c : uniqueChars)
		alphabet += c;

	// Add them to the negativeOrderTable
	for (char c : alphabet)
		model.increaseSymbolCount(c);
}

