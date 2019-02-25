#include "Decompressor.h"
#include "ArithmeticDecoder.h"
#include "Config.h"
#include "Model.h"

using namespace std;
using namespace compression;

void Decompressor::decompress()
{
	basic_ofstream<byte> outputFileStream(directory + "\\result");
	basic_ifstream<byte> inputFileStream(directory + "\\code", ios::binary);
	ArithmeticDecoder decoder(inputFileStream, outputFileStream);

	characterCode decodedChar = (characterCode)' ';
	
	while (decodedChar != config::EndCharacter)
	{
		Model& model = contextMixer.getBestModel();
		decodedChar = model.decode(decoder);
		contextMixer.updateModels(decodedChar);
	}
}

Decompressor::Decompressor(const string& directory) : directory(directory)
{
	decompress();
}