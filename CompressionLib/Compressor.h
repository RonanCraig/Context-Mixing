#pragma once
#include <fstream>
#include <string>
#include "ContextMixer.h"

namespace compression
{

class Compressor
{
private:
	compression::ContextMixer contextMixer;
	std::string directory;

public:
	Compressor(const std::string& directory);

private:
	void compress();
};

}