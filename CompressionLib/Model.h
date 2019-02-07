#pragma once
#include "ArithmeticEncoder.h"
#include "ArithmeticDecoder.h"

namespace compression
{

class Model
{
protected:
	CharTable nextCharProbabilityDistribution;
public:
	virtual void encode(const char& charToEncode, ArithmeticEncoder& encoder) {}
	virtual char decode(ArithmeticDecoder& decoder) { return ' '; }
	virtual void update(const char& charToUpdate) {}
	const CharTable& getNextDistribution() { return nextCharProbabilityDistribution; }
#ifdef _DEBUG
	virtual void outputDebug(std::ofstream& outputFileStream) {}
#endif // _DEBUG

};

}

