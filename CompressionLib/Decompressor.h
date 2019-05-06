#pragma once
#include <string>
#include "ContextMixer.h"
#include "ArithmeticDecoder.h"

namespace compression
{

class Decompressor
{
// Attributes
private:
	ContextMixer* contextMixer;
	std::string directory;
	ArithmeticDecoder* decoder;

// Methods
public:
	Decompressor(const std::string& directory, const std::string& inputFileName, std::vector<int> ordersToRun);

private:
	void decompress();

};

}