#include "Compressor.h"
#include "ArithmeticEncoder.h"
#include "Config.h"
#include "Model.h"

using namespace std;
using namespace compression;

void Compressor::compress()
{
	basic_ofstream<byte> outputFileStream(directory + "\\code", ios::binary);
	basic_ifstream<byte> inputFileStream(directory + "\\" + config::inputfile);

	ArithmeticEncoder encoder(outputFileStream);

	// Main loop, compressing each character individually.
	bool finished = false;
	while (!finished)
	{
		byte c;
		characterCode character;
		if (!inputFileStream.get(c))
		{
			finished = true;
			character = config::EndCharacter;
		}
		else
			character = (characterCode)c;

		Model& model = contextMixer.getBestModel();
		model.encode(character, encoder);
		contextMixer.updateModels(character);

	}
}

Compressor::Compressor(const string& directory) : directory(directory)
{
	compress();
}
