#include "Compressor.h"

using namespace std;
using namespace compression;
using namespace types;

void Compressor::compress()
{
	basic_ifstream<byte> inputFileStream(directory + "\\" + config::inputfile, ios::binary);
	int charactersIn = 0;
	// Main loop, compressing each character individually.
	byte c;

	while (inputFileStream.get(c))
	{
		if (encode(c)) // True if models need to be reset.
		{
			contextMixer->resetModels();
			basic_ifstream<byte> inputFileStreamTailing(directory + "\\" + config::inputfile);
			byte cBehind;
			int charactersToSkip = charactersIn - config::charactersToTrail;//1 * (charactersIn / 3);
			int charactersToEncode = charactersIn - charactersToSkip;

			while (charactersToSkip >= 0)
			{
				inputFileStreamTailing.get(cBehind);
				charactersToSkip--;
			}
			while (charactersToEncode > 0)
			{
				contextMixer->updateModels(cBehind);
				inputFileStreamTailing.get(cBehind);
				charactersToEncode--;
			}
			contextMixer->updateModels(cBehind);
			Model& model = contextMixer->getBestModel();
			model.encode(*encoder);
		}
		charactersIn++;
	}
	encode(Arithmetic::END_CHARACTER);
	encoder->end();
}

bool Compressor::encode(characterType c)
{
	Model& model = contextMixer->getBestModel();
	if (contextMixer->updateModels(c))	
		return true;

	model.encode(*encoder);
	return false;
}

Compressor::Compressor(const string& directory, const std::string& resultFileName, vector<int> ordersToRun) : directory(directory)
{
	basic_ofstream<byte> outputFileStream(directory + "\\" + resultFileName, ios::binary);
	encoder = new ArithmeticEncoder(outputFileStream);

	contextMixer = new ContextMixer(ordersToRun);
	compress();
}
