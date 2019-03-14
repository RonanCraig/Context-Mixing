#include "Decompressor.h"
#include "Config.h"
#include "PPM.h"

using namespace std;
using namespace compression;
using namespace types;

void Decompressor::decompress()
{
	characterType decodedChar = (characterType)' ';
	basic_ofstream<byte> outputFileStream(directory + "\\result");
	Model* model;
	
	while (decodedChar != Arithmetic::END_CHARACTER)
	{
		model = contextMixer->getBestModel();
		decodedChar = model->decode();
		if (decodedChar != Arithmetic::END_CHARACTER)
		{
			outputFileStream << (byte)decodedChar;
			contextMixer->updateModels(decodedChar);
		}
	}
}

Decompressor::Decompressor(const string& directory) : directory(directory)
{
	basic_ifstream<byte> inputFileStream(directory + "\\code", ios::binary);

	ArithmeticDecoder* decoder = new ArithmeticDecoder(inputFileStream);

	vector<Model*>* models = new vector<Model*>;
	models->push_back(new PPM(*decoder));

	contextMixer = new ContextMixer(*models);
	decompress();
}