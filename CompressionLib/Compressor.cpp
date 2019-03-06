#include "Compressor.h"
#include "ArithmeticEncoder.h"
#include "Config.h"
#include "PPM.h"
#include "CompressionTypes.h"

using namespace std;
using namespace compression;
using namespace types;

void Compressor::compress()
{
	basic_ifstream<byte> inputFileStream(directory + "\\" + config::inputfile);

	// Main loop, compressing each character individually.
	bool finished = false;
	while (!finished)
	{
		byte c;
		characterCode character;
		if (!inputFileStream.get(c))
		{
			finished = true;
			character = ModelMetrics::EndCharacter;
		}
		else
			character = (characterCode)c;

		Model& model = contextMixer->getBestModel();
		model.encode(character);
		contextMixer->updateModels(character);

	}
}

Compressor::Compressor(const string& directory) : directory(directory)
{
	basic_ofstream<byte> outputFileStream(directory + "\\code", ios::binary);

	ArithmeticEncoder* encoder = new ArithmeticEncoder(outputFileStream);

	vector<Model*>* models = new vector<Model*>;
	models->push_back(new PPM(*encoder));

	contextMixer = new ContextMixer(*models);

	compress();
}
