#pragma once
#include <fstream>
#include <map>
#include <string>
#include "CharTable.h"

namespace compression
{

class ArithmeticDecoder
{
private:
	unsigned int upperBound = 0xFFFFFFFFU;
	unsigned int lowerBound = 0;
	unsigned int value = 0;
	std::ifstream& inputFileStream;
	std::ofstream& outputFileStream;
public:
	ArithmeticDecoder(std::ifstream& inputFileStream, std::ofstream& outputFileStream) : inputFileStream(inputFileStream), outputFileStream(outputFileStream)
	{
		for (int i = 0; i < 32; i++)
		{
			value <<= 1;
			value += getNextBit();
		}
	}

	char decode(std::unique_ptr<ProbRange> probRange)
	{
		unsigned int range = upperBound - lowerBound;
		upperBound = lowerBound + (probRange->upper * (range / probRange->denom));
		lowerBound += probRange->lower * (range / probRange->denom);

		renormalizeCode();
		char& c = probRange->character;
		if(c != config::EscapeCharacter && c != config::EndCharacter)
			outputFileStream << probRange->character;
		return c;
	}

	int getCount(const int& totalCount)
	{
		unsigned int range = upperBound - lowerBound;
		int count = (value - lowerBound) / (range / totalCount);
		return count;
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