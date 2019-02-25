#pragma once
#include <string>

typedef unsigned int characterCode;
typedef unsigned char byte;

namespace config
{
const int TotalUniqueChars = 257;
const std::string inputfile = "input";
const characterCode EndCharacter = 256;
const characterCode EscapeCharacter = 257;
const int MaxOrderSize = 2; // Max size of order used by PPM.

}

