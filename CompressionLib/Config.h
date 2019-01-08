#pragma once

namespace config
{

enum encodingValues
{
	UTF8
};

const encodingValues encodingType = UTF8;
const char EscapeCharacter = '&';
const char EndCharacter = '*';
const int MaxOrderSize = 5; // Max size of order used by PPM.

}

