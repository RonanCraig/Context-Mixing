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
	bool finished = false;
	while (!finished)
	{
		if (!inputFileStream.get(c))
		{
			finished = true;
			c = config::EndCharacter;
		}
		Model& model = contextMixer.getBestModel();
		model.encode(c, encoder);
		contextMixer.updateModels(c);

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
