#pragma once
#include <string>
#include <vector>
#include "ContextMixer.h"
#include "ArithmeticEncoder.h"
#include "Config.h"

namespace compression
{

class Compressor
{
private:
	ContextMixer* contextMixer;
	std::string directory;
	ArithmeticEncoder* encoder;

public:
	Compressor(const std::string& directory, const std::string& resultFileName, std::vector<int> ordersToRun);
	~Compressor() { delete encoder; delete contextMixer; }

private:
	void compress();
	bool encode(types::characterType c);
};

}