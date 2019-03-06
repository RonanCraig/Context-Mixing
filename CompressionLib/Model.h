#pragma once
#include "CompressionTypes.h"

namespace compression
{

class Model
{

// Methods
public:
	virtual void encode(const types::characterCode& charToEncode) {}
	virtual types::characterCode decode() { return ' '; }
	virtual void update(const types::characterCode& charToUpdate) {}

};

}


