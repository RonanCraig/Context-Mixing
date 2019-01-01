#include "Decompressor.h"
#include "ArithmeticDecoder.h"
#include <deque>

using namespace std;
using namespace compression;

void Decompressor::decompress()
{
	ofstream outputFileStream(directory + "\\result");
	ifstream inputFileStream(directory + "\\code");
	ArithmeticDecoder decoder(inputFileStream);

	int order = ProbabilityModel::MaxOrderSize;
	bool endReached = false;
	while (!endReached)
	{

	}
}

void Decompressor::deserializeProbabilityModel()
{
	ifstream inputFileStream(directory + "\\model");
	boost::archive::binary_iarchive inputArchive(inputFileStream);
	inputArchive >> model;
}

Decompressor::Decompressor(const string& directory) : directory(directory)
{
	deserializeProbabilityModel();
	decompress();
}