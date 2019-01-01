#pragma once
#include <memory>
#include "ProbabilityModel.h"

namespace compression
{

class PPM
{
	
private:
	const std::string& alphabet; // All characters in message without duplicates.
	ProbabilityModel& model;

public:
	// Iterates through each context until the character has been seen before at that context or -1 context.
	// Updates the count for the character at each context reached.
	// Creates a probablilty range for the escape character if the character has not been seen before at that context.
	// Creates a probability range for the character to encode if the character has been seen before.
	std::unique_ptr<ProbRanges> getProbabilityRanges(const char& charToEncode, std::string context);
	PPM(ProbabilityModel& model, const std::string& alphabet);
};

}