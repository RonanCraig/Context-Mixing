#pragma once
#include <fstream>
#include <string>

class Compressor
{
private:
	const std::string message;
	std::ofstream outputFileStream; // Used to output the compression result.

public:
	Compressor(const std::string& outputDirectory, const std::string& message); 

private:
	void compress();
	
};