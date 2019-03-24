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

void Compressor::encode(byte c)
{
	characterType character = (characterType)c;

	Model* model = contextMixer->getBestModel(character);
	types::ProbRange range = model->getProbability(4);
	encoder->encode(range);
}

Compressor::Compressor(const string& directory) : directory(directory)
{
	basic_ofstream<byte> outputFileStream(directory + "\\code", ios::binary);
	encoder = new ArithmeticEncoder(outputFileStream);

	vector<Model*>* models = new vector<Model*>;
	models->push_back(new PPM(4));

	contextMixer = new ContextMixer(*models);

	compress();
}
