#include "Decompressor.h"
#include "Config.h"
#include "PPM.h"

using namespace std;
using namespace compression;
using namespace types;

void Decompressor::decompress()
{
	characterType decodedChar = (characterType)' ';
	byte cBehind;
	basic_ofstream<byte> outputFileStream(directory + "\\result");
	int charactersIn = 0;
	
	while (decodedChar != Arithmetic::END_CHARACTER)
	{
		Model& model = contextMixer->getBestModel();
		decodedChar = model.decode(*decoder);
		if (decodedChar != Arithmetic::END_CHARACTER)
		{
			outputFileStream << (byte)decodedChar;
			if(contextMixer->updateModels(decodedChar))
			{
				// Get fielstream to position where it will start rebuilding trie.
				outputFileStream.close();
				basic_ifstream<byte> inputFileStreamTailing(directory + "\\result");
				for (int a = 0; a <= charactersIn - config::charactersToTrail; a++)
					inputFileStreamTailing.get(cBehind);

				// Rebuild trie.
				contextMixer->resetModels();
				int charactersBehind = config::charactersToTrail;
				while (charactersBehind > 0)
				{
					model = contextMixer->getBestModel();
					contextMixer->updateModels(cBehind);
					inputFileStreamTailing.get(cBehind);
					charactersBehind--;
				}
				Model& model = contextMixer->getBestModel();
				contextMixer->updateModels(cBehind);

				// Get writing filestream back to it's previous location.
				inputFileStreamTailing.close();
				outputFileStream.open(directory + "\\result", ios_base::app);
			}
			charactersIn++;
		}	
	}
}

Decompressor::Decompressor(const string& directory) : directory(directory)
{
	basic_ifstream<byte> inputFileStream(directory + "\\code", ios::binary);
	decoder = new ArithmeticDecoder(inputFileStream);

	contextMixer = new ContextMixer();
	decompress();
}