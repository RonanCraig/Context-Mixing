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
	std::basic_ofstream<byte>& outputFileStream;

	// Used for outputting bits in blocks of bytes.
	byte buffer = 0;
	int current_bit = 0;

public:
	ArithmeticEncoder(std::basic_ofstream<byte>& outputFileStream) : outputFileStream(outputFileStream) {}

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
			flushBuffer();
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
		writeBit(bit);
		for (int i = 0; i < pending_bits; i++)
			writeBit(!bit);
		pending_bits = 0;
	}

	void writeBit(bool bit)
	{
		if (bit)
			buffer |= (128 >> current_bit);

		current_bit++;
		if (current_bit == 8)
		{
			outputFileStream << buffer;
			current_bit = 0;
			buffer = 0;
		}
	}

	void flushBuffer()
	{
		while (current_bit)
			writeBit(0);
	}
};

}