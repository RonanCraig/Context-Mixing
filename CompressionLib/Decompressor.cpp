#include "Decompressor.h"
#include "ArithmeticDecoder.h"
#include "Config.h"
#include "Model.h"

using namespace std;
using namespace compression;

void Decompressor::decompress()
{
	ofstream outputFileStream(directory + "\\result");
	ifstream inputFileStream(directory + "\\code");
	ArithmeticDecoder decoder(inputFileStream, outputFileStream);

	bool finished = false;
	
	while (!finished)
	{
		Model& model = contextMixer.getBestModel();
		finished = model.decode(decoder);
	}
}

Decompressor::Decompressor(const string& directory) : directory(directory)
{
	decompress();
}