#pragma once
#include <fstream>
#include "ProbabilityModel.h"

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

	void encode(char charToEncode, std::unique_ptr<ProbRange> probRange)
	{
		unsigned int range = upperBound - lowerBound;
		lowerBound = (probRange->lower*range) / probRange->denom;
		upperBound = (probRange->upper*range) / probRange->denom;

		renormalizeBounds();

		// Character representing end of encoding.
		if (charToEncode == '*')
		{
			pending_bits++;
			if (lowerBound < 0x40000000)
				output_bit_plus_pending(0);
			else
				output_bit_plus_pending(1);
		}
	}

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