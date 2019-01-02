#include "Compressor.h"
#include "PPM.h"
#include "ArithmeticEncoder.h"
#include <set>

using namespace std;
using namespace compression;

void Compressor::compress()
{
	ofstream outputFileStream(directory + "\\code");
	ifstream inputFileStream(directory + "\\input");
	#ifdef _DEBUG
	ofstream debugOutputFileStream(directory + "\\debug");
	#endif

	ArithmeticEncoder encoder(outputFileStream);
	PPM ppm(model, alphabet);

	string buffer = "";
	char c;

	// Main loop, compressing each character individually.
	while (inputFileStream.get(c))
	{
		if (buffer.length() > ProbabilityModel::MaxOrderSize)
			buffer.erase(0,1);
		
		unique_ptr<ProbRanges> probRanges = ppm.getProbabilityRanges(c, buffer);
		for (auto it = probRanges->begin(); it < probRanges->end(); it++)
		{
			#ifdef _DEBUG
			debugOutputFileStream << (*it)->character;
			#endif
			// Arithmetic encoder encodes each probability range.
			encoder.encode(move(*it));
		}
		buffer.push_back(c);
	}
}

void Compressor::createAlphabet()
{
	// Finds all the unique characters in the message and adds them to the alphabet.
	ifstream inputFileStream(directory + "\\input");
	set<char> uniqueChars;
	char c;
	while (inputFileStream.get(c))
		uniqueChars.insert(c);
	for (char c : uniqueChars)
		alphabet += c;
}

void Compressor::serializeProbabilityModel()
{
	// Write ProbabilityModel to file.
	ofstream outputFileStream(directory + "\\model", ios::binary);
	boost::archive::binary_oarchive outputArchive(outputFileStream);
	outputArchive << model;
}

Compressor::Compressor(const string& directory) : directory(directory)
{
	createAlphabet();
	compress();
	serializeProbabilityModel();
	#ifdef _DEBUG
	model.outputTables(directory);
	#endif
}
