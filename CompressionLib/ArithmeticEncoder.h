#pragma once
#include <fstream>
#include "Arithmetic.h"
#include "CompressionTypes.h"

namespace compression
{

class ArithmeticEncoder : public Arithmetic
{
private:
	// Represents numberline of 1s
	ModelMetrics::CODE_VALUE lowerBound = 0;
	ModelMetrics::CODE_VALUE upperBound = ModelMetrics::MAX_CODE;

	int pending_bits = 0;
	std::basic_ofstream<types::byte>& outputFileStream;

	// Used for outputting bits in blocks of bytes.
	types::byte buffer = 0;
	int current_bit = 0;

public:
	ArithmeticEncoder(std::basic_ofstream<types::byte>& outputFileStream) : outputFileStream(outputFileStream) {}

	void encode(ModelMetrics::ProbRange& probRange)
	{
		if (probRange.denom >= ModelMetrics::MAX_FREQ)
			int a = 5;
		ModelMetrics::CODE_VALUE range = upperBound - lowerBound + 1;
		upperBound = lowerBound + (range * probRange.upper / probRange.denom) - 1;
		lowerBound = lowerBound + (range * probRange.lower / probRange.denom);

		renormalizeBounds();

		// Character representing end of encoding.
		if (probRange.character == ModelMetrics::EndCharacter)
		{
			pending_bits++;
			if (lowerBound < ONE_FOURTH)
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
			if (upperBound < ONE_HALF)
				output_bit_plus_pending(0);
			else if (lowerBound >= ONE_HALF)
				output_bit_plus_pending(1);
			else if (lowerBound >= ONE_FOURTH && upperBound < THREE_FOURTHS) 
			{
				pending_bits++;
				lowerBound -= ONE_FOURTH;
				upperBound -= ONE_FOURTH;
			}
			else
				break;
			upperBound <<= 1;
			upperBound++;
			lowerBound <<= 1;
			upperBound &= ModelMetrics::MAX_CODE;
			lowerBound &= ModelMetrics::MAX_CODE;
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