#include "PPM.h"
#include <string>

using namespace std;
using namespace compression;

unique_ptr<ProbRanges> PPM::getProbabilityRanges(const char& charToEncode, string context)
{
	unique_ptr<ProbRanges> ranges = make_unique<ProbRanges>(ProbRanges());
	bool contextFound = false;

	// Runs from order of context size to the smallest order -1. Unless the context is found at an order.
	for (int order = context.length(); order > -2 && !contextFound; order--)
	{
		ProbabilityModel::CharTable& charTable = model.getCharTable(&context);
		if (order == -1)
			ranges->push_back(move(charTable.calculateRange(charToEncode)));
		else
		{
			contextFound = charTable.increaseSymbolCount(charToEncode);
			if (contextFound)
				ranges->push_back(move(charTable.calculateRange(charToEncode)));
			else
			{
				ranges->push_back(move(charTable.calculateRange(ProbabilityModel::EscapeCharacter)));
				context.erase(0, 1);
			}			
		}
	}

	return ranges;
}

PPM::PPM(ProbabilityModel& model, const string& alphabet) : model(model), alphabet(alphabet)
{
	// Add them to the negativeOrderTable
	for (char c : alphabet)
		model.getCharTable().increaseSymbolCount(c);
}

