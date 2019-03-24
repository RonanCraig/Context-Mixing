#pragma once
#include "CompressionTypes.h"
#include "ArithmeticDecoder.h"

namespace compression
{

class Model
{

// Methods
public:
	virtual types::characterType decode(ArithmeticDecoder& decoder) { return ' '; }
	virtual void update(const types::characterType& charToUpdate) {}
	virtual types::ProbRange getProbability(int order) { types::ProbRange a; return a; }
};

}


