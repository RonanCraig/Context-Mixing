#pragma once
#include <string>
#include "ContextMixer.h"

namespace compression
{

class Compressor
{
private:
	ContextMixer* contextMixer;
	std::string directory;

public:
	Compressor(const std::string& directory);

private:
	void compress();
};

}