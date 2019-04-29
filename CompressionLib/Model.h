#pragma once
#include "CompressionTypes.h"
#include "ArithmeticDecoder.h"
#include "ArithmeticEncoder.h"

namespace compression
{

class Model
{
// Methods
public:
	virtual types::characterType decode(ArithmeticDecoder& decoder) { return ' '; }
	virtual void update(const types::characterType& charToUpdate) {}
	virtual void encode(ArithmeticEncoder& encoder, int order = 0) {}
	virtual double getEstimatedProb(int order = 0) { return 1; }
};

}


