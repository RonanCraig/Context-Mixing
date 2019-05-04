#include "Compressor.h"

using namespace std;
using namespace compression;
using namespace types;

void Compressor::compress()
{
	basic_ifstream<byte> inputFileStream(directory + "\\" + config::inputfile);
	basic_ifstream<byte> inputFileStreamTailing(directory + "\\" + config::inputfile);
	int charactersBehind = 0;
	// Main loop, compressing each character individually.
	byte c;
	byte cBehind;
	while (inputFileStream.get(c))
	{
		if (charactersBehind < config::charactersToTrail) // Trails 10000000 characters behind.
			charactersBehind++;
		else
			inputFileStreamTailing.get(cBehind);

		if (encode(c)) // True if models need to be reset.
		{
			contextMixer->resetModels();
			while (charactersBehind > 0)
			{
				contextMixer->updateModels(cBehind);
				inputFileStreamTailing.get(cBehind);
				charactersBehind--;
			}
			contextMixer->updateModels(cBehind);
			charactersBehind = 0;
		}
	}
	encode(Arithmetic::END_CHARACTER);
	encoder->end();
}

bool Compressor::encode(characterType c)
{
	Model& model = contextMixer->getBestModel();
	if (contextMixer->updateModels(c))
	{
		model.encode(*encoder);
		return true;
	}
	model.encode(*encoder);
	return false;
}

Compressor::Compressor(const string& directory) : directory(directory)
{
	basic_ofstream<byte> outputFileStream(directory + "\\code", ios::binary);
	encoder = new ArithmeticEncoder(outputFileStream);

	contextMixer = new ContextMixer();
	compress();
}
