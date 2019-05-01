#pragma once
#include <string>
#include "ContextMixer.h"
#include "ArithmeticEncoder.h"

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
	void encode(types::characterType c);
};

}