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

	string buffer = "";
	int order = 0;
	bool endReached = false;
	
	while (!endReached)
	{
		ProbabilityModel::CharTable charTable;
		if (order == -1)
			charTable = model.getCharTable();
		else
		{
			int end = buffer.length();
			int start = end - order;
			string context = buffer.substr(start,end);
			charTable = model.getCharTable(&context);
		}

		char output = decoder.decode(charTable);

		if (output == ProbabilityModel::EscapeCharacter)
			order--;
		else if (output == ProbabilityModel::EndCharacter)
			endReached = true;
		else
		{
			buffer.push_back(output);
			outputFileStream << output;
			int bufferSize = buffer.length();
			if (bufferSize > ProbabilityModel::MaxOrderSize)
				buffer.erase(0, 1);
			order = bufferSize;
		}
		
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