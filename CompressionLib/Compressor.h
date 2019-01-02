#pragma once
#include <fstream>
#include <string>
#include "ProbabilityModel.h"

namespace compression
{

class Compressor
{
private:
	std::string directory;
	std::string alphabet;
	ProbabilityModel model;

public:
	Compressor(const std::string& directory);

private:
	void compress();
	void createAlphabet();
	void serializeProbabilityModel();
};

}