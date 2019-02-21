#pragma once

typedef int characterCode;

namespace config
{

enum encodingValues
{
	UTF8
};

const encodingValues encodingType = UTF8;
const characterCode EscapeCharacter = 256;
const characterCode EndCharacter = 257;
const int MaxOrderSize = 2; // Max size of order used by PPM.

}

