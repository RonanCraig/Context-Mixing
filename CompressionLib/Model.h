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
	virtual bool update(const types::characterType& charToUpdate) { return false; }
	virtual void encode(ArithmeticEncoder& encoder) {}
	virtual double getEstimatedProb(int order = 0) { return 1; }
	virtual ~Model() = default;
};

}


