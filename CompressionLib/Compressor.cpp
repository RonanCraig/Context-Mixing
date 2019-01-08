#include "Compressor.h"
#include "ArithmeticEncoder.h"
#include "Config.h"
#include "Model.h"

using namespace std;
using namespace compression;

void Compressor::compress()
{
	ofstream outputFileStream(directory + "\\code");
	ifstream inputFileStream(directory + "\\input");

	ArithmeticEncoder encoder(outputFileStream);

	// Main loop, compressing each character individually.
	char c;
	while (inputFileStream.get(c))
	{
		Model& model = contextMixer.getBestModel();
		model.encode(c, encoder);
	}
}

Compressor::Compressor(const string& directory) : directory(directory)
{
	compress();
}
