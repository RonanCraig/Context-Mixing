#pragma once
#include "ArithmeticEncoder.h"
#include "ArithmeticDecoder.h"

namespace compression
{

class Model
{
public:
	virtual void encode(const char& charToEncode, ArithmeticEncoder& encoder) = 0;
	virtual bool decode(ArithmeticDecoder& decoder) = 0;
};

}

