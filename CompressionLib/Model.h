#pragma once
#include "ArithmeticEncoder.h"
#include "ArithmeticDecoder.h"

namespace compression
{

class Model
{
public:
	virtual void encode(const characterCode& charToEncode, ArithmeticEncoder& encoder) {}
	virtual int decode(ArithmeticDecoder& decoder) { return ' '; }
	virtual void update(const characterCode& charToUpdate) {}
#ifdef _DEBUG
	virtual void outputDebug(std::ofstream& outputFileStream) {}
#endif // _DEBUG

};

}

