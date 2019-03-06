#pragma once
#include <fstream>
#include "Arithmetic.h"
#include "CompressionTypes.h"

namespace compression
{

class ArithmeticDecoder : public Arithmetic
{
private:
	ModelMetrics::CODE_VALUE upperBound = ModelMetrics::MAX_CODE;
	ModelMetrics::CODE_VALUE lowerBound = 0;
	ModelMetrics::CODE_VALUE value = 0;

	std::basic_ifstream <types::byte> & inputFileStream;
	std::basic_ofstream<types::byte>& outputFileStream;

	types::byte buffer = 0;
	int current_bit = 8;
public:
	ArithmeticDecoder(std::basic_ifstream<types::byte>& inputFileStream, std::basic_ofstream<types::byte>& outputFileStream) : inputFileStream(inputFileStream), outputFileStream(outputFileStream)
	{
		for (int i = 0; i < ModelMetrics::CODE_VALUE_BITS; i++)
		{
			value <<= 1;
			value += getNextBit();
		}
	}

	void decode(ModelMetrics::ProbRange& probRange)
	{
		//if (probRange.denom >= ModelMetrics::MAX_FREQ)
			//probRange = getEvenlyDistributedProbRange(probRange.character);

		types::characterCode& c = probRange.character;
		if (c < ModelMetrics::TotalUniqueChars - 1)
			outputFileStream << (types::byte)probRange.character;

		ModelMetrics::CODE_VALUE range = upperBound - lowerBound + 1;
		upperBound = lowerBound + (range*probRange.upper) / probRange.denom - 1;
		lowerBound = lowerBound + (range*probRange.lower) / probRange.denom;

		renormalizeCode();
	}

	ModelMetrics::CODE_VALUE getCount(const ModelMetrics::CODE_VALUE& totalCount)
	{
		ModelMetrics::CODE_VALUE range = upperBound - lowerBound + 1;
		ModelMetrics::CODE_VALUE count = ((value - lowerBound + 1) * totalCount - 1) / range;
		return count;
	}
private:
	void renormalizeCode()
	{
		while (true)
		{
			if (upperBound < ONE_HALF) {} //do nothing, bit is a zero
			else if (lowerBound >= ONE_HALF) 
			{
				value -= ONE_HALF;  //subtract one half from all three code values
				lowerBound -= ONE_HALF;
				upperBound -= ONE_HALF;
			}
			else if (lowerBound >= ONE_FOURTH && upperBound < THREE_FOURTHS) 
			{
				value -= ONE_FOURTH;
				lowerBound -= ONE_FOURTH;
				upperBound -= ONE_FOURTH;
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