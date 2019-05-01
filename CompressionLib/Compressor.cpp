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
	while (inputFileStream.get(c))
		encode(c);
	encode(Arithmetic::END_CHARACTER);
	encoder->end();
}

void Compressor::encode(characterType c)
{
	Model* model = contextMixer->getBestModel();
	contextMixer->updateModels(c);
	model->encode(*encoder, 4);
}

Compressor::Compressor(const string& directory) : directory(directory)
{
	basic_ofstream<byte> outputFileStream(directory + "\\code", ios::binary);
	encoder = new ArithmeticEncoder(outputFileStream);

	vector<Model*>* models = new vector<Model*>;
	models->push_back(new PPM(2));

	contextMixer = new ContextMixer(*models);

	compress();
}
