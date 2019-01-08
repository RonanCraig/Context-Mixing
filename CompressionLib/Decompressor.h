#pragma once
#include <fstream>
#include <string>
#include "ContextMixer.h"

namespace compression
{

class Decompressor
{
private:
	compression::ContextMixer contextMixer;
	std::string directory;

public:
	Decompressor(const std::string& directory);

private:
	void decompress();

};

}