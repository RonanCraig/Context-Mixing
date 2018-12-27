#include "Compressor.h"
#include "PPM.h"
#include "ArithmeticEncoder.h"
#include <set>

using namespace std;
using namespace compression;

void Compressor::compress()
{
	ArithmeticEncoder encoder(outputFileStream);
	PPM ppm(alphabet);

	// Main loop, compressing each character individually.
	for (unsigned int u = 0; u < message.length(); u++)
	{
		// PPM returns probability ranges for encoding the given character, there can be multiple ranges because of escape characters.
		unique_ptr<ProbRanges> probRanges = ppm.getProbabilityRanges(getMessageContextAt(u));
		for (auto it = probRanges->begin(); it < probRanges->end(); it++)
		{
			// Arithmetic encoder encodes each probability range.
			encoder.encode(message.at(u), move(*it));
		}
	}
	
	// Compression finished.
	outputFileStream.close();
}

std::string Compressor::getMessageContextAt(const unsigned int position)
{
	int start = position - order;
	if (start < 0)
		start = 0;
	return message.substr(start, position + 1);
}

Compressor::Compressor(const string outputDirectory, const string message) : outputFileStream(outputDirectory + "\\code"), message(message + '*')
{
	// Finds all the unique characters in the message and adds them to the alphabet.
	set<char> uniqueChars(message.begin(), message.end());
	for (char c : uniqueChars)
		alphabet += c;
	compress();
}
