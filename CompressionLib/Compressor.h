#pragma once
#include <string>
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
	Compressor(const std::string& directory);

private:
	void compress();
	bool encode(types::characterType c);
};

}