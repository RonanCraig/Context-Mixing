#pragma once
#include <memory>
#include <string>
#include "Config.h"
#include "CharTable.h"
#include "Model.h"
#include "ArithmeticEncoder.h"

namespace compression
{

class PPM : public Model
{

class PPMCharTable : public CharTable
{
	//Methods
public:
	PPMCharTable() { increaseSymbolCount(config::EscapeCharacter); }
	PPMCharTable(bool isNegativeOrder) {}
	std::unique_ptr<ProbRange> getRange(const char& c);
	std::unique_ptr<ProbRange> getRange(const int& count);
private:
	std::unique_ptr<ProbRange> calculateRange(CharMap::iterator it);
};
	
// Attributes
private:
	std::string context = "";
	typedef std::map<std::string, PPMCharTable> ContextTable;
	typedef std::map<int, ContextTable> OrderTable;
	OrderTable orderTable;
	PPMCharTable orderNegativeOneTable = PPMCharTable(true);

// Methods
public:
	// Iterates through each context until the character has been seen before at that context or -1 context.
	// Creates a probablilty range for the escape character if the character has not been seen before at that context.
	// Creates a probability range for the character to encode if the character has been seen before.
	void encode(const char& charToEncode, ArithmeticEncoder& encoder);
	// Similar to encode except uses code instead of character to find ranges.
	char decode(ArithmeticDecoder& decoder);
	// Updates the counts for contexts.
	void update(const char& charToUpdate);
	PPM();

#ifdef _DEBUG
	void outputDebug(std::ofstream& outputFileStream);
#endif // DEBUG

private:
	PPMCharTable& getCharTable(const std::string& context, const int& order);

};

}