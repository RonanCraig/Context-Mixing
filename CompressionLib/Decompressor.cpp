#include "Decompressor.h"
#include "ArithmeticDecoder.h"
#include "Config.h"
#include "Model.h"

using namespace std;
using namespace compression;

void Decompressor::decompress()
{
	characterCode decodedChar = (characterCode)' ';
	
	while (decodedChar != config::EndCharacter)
	{
		Model& model = contextMixer->getBestModel();
		decodedChar = model.decode();
		contextMixer->updateModels(decodedChar);
	}
}

Decompressor::Decompressor(const string& directory) : directory(directory)
{
	basic_ofstream<byte> outputFileStream(directory + "\\result");
	basic_ifstream<byte> inputFileStream(directory + "\\code", ios::binary);

	ArithmeticDecoder* decoder = new ArithmeticDecoder(inputFileStream, outputFileStream);

	vector<Model*>* models = new vector<Model*>;
	models->push_back(new PPM(*decoder));

	contextMixer = new ContextMixer(*models);
	decompress();
}