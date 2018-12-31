#pragma once
#include <fstream>
#include <map>
#include <string>

namespace compression
{

class ArithmeticDecoder
{
private:
	unsigned int upperBound = 0xFFFFFFFFU;
	unsigned int lowerBound = 0;
	unsigned int value = 0;
	std::string fullCode;
	std::ofstream& outputFileStream;
public:
	ArithmeticDecoder(std::string fullCode, std::ofstream& outputFileStream) : fullCode(fullCode), outputFileStream(outputFileStream)
	{
		for (int i = 0; i < 32; i++)
		{
			value <<= 1;
			value += getNextBit();
		}
	}

	bool decode(const std::map<char, float>& probabilities)
	{
		unsigned int range = upperBound - lowerBound;
		upperBound = lowerBound;
		char currentChar;
		for (auto it = probabilities.begin(); it != probabilities.end() && !(lowerBound <= value && upperBound > value); it++)
		{
			currentChar = it->first;
			lowerBound = upperBound;
			upperBound += (probabilities.at(currentChar) * range);
		}

		if (currentChar == '*')
			return true;

		outputFileStream << currentChar;
		renormalizeCode();
		return false;
	}

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

	int getNextBit()
	{
		int nextBit = 0;
		if (fullCode.length() > 0)
		{
			nextBit = fullCode.at(0) - '0';
			fullCode.erase(0, 1);
		}
		return nextBit;
	}
};

}