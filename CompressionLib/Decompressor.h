#pragma once
#include <fstream>
#include <string>
#include <boost/archive/binary_iarchive.hpp>
#include "ProbabilityModel.h"

namespace compression
{

class Decompressor
{
private:
	std::string directory;
	ProbabilityModel model;

public:
	Decompressor(const std::string& directory);

private:
	void decompress();
	void deserializeProbabilityModel();

};

}