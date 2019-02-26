#pragma once
#include <fstream>
#include <string>
#include "ContextMixer.h"

namespace compression
{

class Decompressor
{
// Attributes
private:
	compression::ContextMixer contextMixer;
	std::string directory;

// Methods
public:
	Decompressor(const std::string& directory);

private:
	void decompress();

};

}