#pragma once
#include <fstream>
#include <string>

class Compressor
{
private:
	const std::string message;
	std::string alphabet; // All characters in message without duplicates.
	std::ofstream outputFileStream; // Used to output the compression result.
	const unsigned int order = 5; // Size of order used by PPM.

public:
	Compressor(const std::string outputDirectory, const std::string message); 

private:
	void compress();
	std::string getMessageContextAt(const unsigned int position); // Returns a string containing the character in message at given position, with trailing characters of size order (the context).
};