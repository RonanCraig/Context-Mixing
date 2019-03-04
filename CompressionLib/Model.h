#pragma once
#include "ArithmeticEncoder.h"
#include "ArithmeticDecoder.h"

namespace compression
{

class Model
{
public:
	virtual void encode(const characterCode& charToEncode) {}
	virtual characterCode decode() { return ' '; }
	virtual void update(const characterCode& charToUpdate) {}
};

}

