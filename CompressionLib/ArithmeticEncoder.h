#pragma once
#include <fstream>
#include "Config.h"
#include "ProbRange.h"

namespace compression
{

class ArithmeticEncoder
{
private:
	// Represents numberline of 1s
	unsigned int upperBound = 0xFFFFFFFFU;
	unsigned int lowerBound = 0;
	int pending_bits = 0;
	std::ofstream& outputFileStream;
public:
	ArithmeticEncoder(std::ofstream& outputFileStream) : outputFileStream(outputFileStream) {}

	void encode(const ProbRange& probRange)
	{
		unsigned int range = upperBound - lowerBound;
		upperBound = lowerBound + (probRange.upper * (range / probRange.denom));
		lowerBound += probRange.lower * (range / probRange.denom);

		renormalizeBounds();

		// Character representing end of encoding.
		if (probRange.character == config::EndCharacter)
		{
			pending_bits++;
			if (lowerBound < 0x40000000)
				output_bit_plus_pending(0);
			else
				output_bit_plus_pending(1);
		}
	}

private:
	void renormalizeBounds()
	{
		while (true)
		{
			if (upperBound < 0x80000000U)
				output_bit_plus_pending(0);
			else if (lowerBound >= 0x80000000U)
				output_bit_plus_pending(1);
			else if (lowerBound >= 0x40000000 && upperBound < 0xC0000000U)
			{
				pending_bits++;
				lowerBound -= 0x40000000;
				upperBound -= 0x40000000;
			}
			else
				break;
			upperBound <<= 1;
			upperBound++;
			lowerBound <<= 1;
		}
	}

	void output_bit_plus_pending(bool bit)
	{
		outputFileStream << bit;
		for (int i = 0; i < pending_bits; i++)
			outputFileStream << !bit;
		pending_bits = 0;
	}
};

}