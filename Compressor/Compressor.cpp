#include "Compressor.h"
#include "PPM.h"
#include "ArithmeticEncoder.h"

using namespace std;
using namespace compression;

void Compressor::compress()
{
	ArithmeticEncoder encoder(outputFileStream);
	PPM ppm(message);

	// Main loop, compressing each character individually.
	for (unsigned int u = 0; u < message.length(); u++)
	{
		// PPM returns probability ranges for encoding the given character, there can be multiple ranges because of escape characters.
		unique_ptr<ProbRanges> probRanges = ppm.getProbabilityRanges(u);
		for (auto it = probRanges->begin(); it < probRanges->end(); it++)
		{
			// Arithmetic encoder encodes each probability range.
			encoder.encode(message.at(u), move(*it));
		}
	}
	
	// Compression finished.
	outputFileStream.close();
}

Compressor::Compressor(const string& outputDirectory, const string& message) : outputFileStream(outputDirectory + "\\code"), message(message + '*')
{
	compress();
}
