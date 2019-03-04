#pragma once
#include <fstream>
#include <map>
#include <string>
#include "ProbRange.h"

namespace compression
{

class ArithmeticDecoder
{
private:
	unsigned int upperBound = 0xFFFFFFFFU;
	unsigned int lowerBound = 0;
	unsigned int value = 0;
	std::basic_ifstream<byte>& inputFileStream;
	std::basic_ofstream<byte>& outputFileStream;

	byte buffer = 0;
	int current_bit = 8;
public:
	ArithmeticDecoder(std::basic_ifstream<byte>& inputFileStream, std::basic_ofstream<byte>& outputFileStream) : inputFileStream(inputFileStream), outputFileStream(outputFileStream)
	{
		for (int i = 0; i < 32; i++)
		{
			value <<= 1;
			value += getNextBit();
		}
	}

	void decode(ProbRange& probRange)
	{
		unsigned int range = upperBound - lowerBound;
		unsigned int countRange = (range / probRange.denom);
		upperBound = lowerBound + (probRange.upper * countRange);
		lowerBound += probRange.lower * countRange;

		renormalizeCode();
		characterCode& c = probRange.character;
		if(c != config::EscapeCharacter && c != config::EndCharacter)
			outputFileStream << (byte)probRange.character;
	}

	int getCount(const int& totalCount)
	{
		unsigned int range = upperBound - lowerBound;
		unsigned int count = (value - lowerBound) / (range / totalCount);
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
		if (current_bit == 8)
		{
			current_bit = 0;
			inputFileStream.get(buffer);
		}
			
		bool bit = buffer & (128 >> current_bit);
		current_bit++;

		return bit;
	}
};

}