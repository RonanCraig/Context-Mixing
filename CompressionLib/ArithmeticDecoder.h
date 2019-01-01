#pragma once
#include <fstream>
#include <map>
#include <string>
#include "ProbabilityModel.h"

namespace compression
{

class ArithmeticDecoder
{
private:
	unsigned int upperBound = 0xFFFFFFFFU;
	unsigned int lowerBound = 0;
	unsigned int value = 0;
	std::ifstream& inputFileStream;
public:
	ArithmeticDecoder(std::ifstream& inputFileStream) : inputFileStream(inputFileStream)
	{
		for (int i = 0; i < 32; i++)
		{
			value <<= 1;
			value += getNextBit();
		}
	}

	char decode(const ProbabilityModel::CharTable& charTable)
	{
		unsigned int range = upperBound - lowerBound;
		int count = ((value - lowerBound)/range)*(charTable.getTotalCount());
		std::unique_ptr<ProbRange> probabilityRange = move(charTable.calculateRange(count));
		lowerBound += (probabilityRange->lower / probabilityRange->denom) * range;
		upperBound += (probabilityRange->upper / probabilityRange->denom) * range;

		renormalizeCode();
		return probabilityRange->character;
	}
private:
	void renormalizeCode()
	{
		while (true)
		{
			if (upperBound < 0x80000000U) {} //do nothing, bit is a zero
			else if (lowerBound >= 0x80000000U) {
				value -= 0x80000000U;  //subtract one half from all three code values
				lowerBound -= 0x80000000U;
				upperBound -= 0x80000000U;
			}
			else if (lowerBound >= 0x40000000 && upperBound < 0xC0000000U) {
				value -= 0x40000000;
				lowerBound -= 0x40000000;
				upperBound -= 0x40000000;
			}
			else
				break;
			lowerBound <<= 1;
			upperBound <<= 1;
			upperBound++;
			value <<= 1;
			value += getNextBit();
		}
	}

	bool getNextBit()
	{
		char nextBit;
		inputFileStream.get(nextBit);
		if (!inputFileStream.eof())
		{
			if (nextBit == '1')
				return true;
		}
		return false;
	}
};

}