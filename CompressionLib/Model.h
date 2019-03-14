#pragma once
#include "CompressionTypes.h"
#include "Arithmetic.h"

namespace compression
{

class Model
{

// Methods
public:
	virtual void encode(const types::characterType& charToEncode) {}
	virtual types::characterType decode() { return ' '; }
	virtual void update(const types::characterType& charToUpdate) {}

};

}


