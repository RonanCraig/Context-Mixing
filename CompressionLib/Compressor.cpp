#include "Compressor.h"
#include "Config.h"
#include "PPM.h"

using namespace std;
using namespace compression;
using namespace types;

void Compressor::compress()
{
	basic_ifstream<byte> inputFileStream(directory + "\\" + config::inputfile);

	// Main loop, compressing each character individually.
	byte c;
	Model* model;
	while (inputFileStream.get(c))
	{
		static int a = 0;
		a++;
		characterType character = (characterType)c;
		model = contextMixer->getBestModel();
		model->encode(character);
		contextMixer->updateModels(character);
	}
	model = contextMixer->getBestModel();
	model->encode(Arithmetic::END_CHARACTER);
	encoder->end();
}

Compressor::Compressor(const string& directory) : directory(directory)
{
	basic_ofstream<byte> outputFileStream(directory + "\\code", ios::binary);
	encoder = new ArithmeticEncoder(outputFileStream);

	vector<Model*>* models = new vector<Model*>;
	models->push_back(new PPM(*encoder));

	contextMixer = new ContextMixer(*models);

	compress();
}
