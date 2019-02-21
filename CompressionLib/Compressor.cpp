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
	bool finished = false;
	while (!finished)
	{
		char c;
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

#ifdef _DEBUG
	outputFileStream.close();
	outputFileStream.open(directory + "\\debug");
	contextMixer.outputDebug(outputFileStream);
#endif // _DEBUG


}

Compressor::Compressor(const string& directory) : directory(directory)
{
	compress();
}
