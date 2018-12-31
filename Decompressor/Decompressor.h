#pragma once
#include <fstream>
#include <string>

class Decompressor
{
private:
	std::ifstream inputFileStream; // Used to input the compressed message.
	std::ofstream outputFileStream; // Used to output the decompression result.
	const unsigned int order = 5; // Size of order used by PPM.

public:
	Decompressor(const std::string& inputDirectory, const std::string& outputDirectory) : inputFileStream(inputDirectory + "\\code"), outputFileStream(outputDirectory + "\\result")
	{	Decompress();	}
		
private:
	void Decompress();

};