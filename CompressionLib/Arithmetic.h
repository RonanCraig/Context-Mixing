#pragma once
#include "ModelMetrics.h"

namespace compression
{

class Arithmetic
{

protected:
	static const ModelMetrics::CODE_VALUE ONE_FOURTH = ModelMetrics::CODE_VALUE(1) << (ModelMetrics::CODE_VALUE_BITS - 2);
	static const ModelMetrics::CODE_VALUE ONE_HALF = 2 * ONE_FOURTH;
	static const ModelMetrics::CODE_VALUE THREE_FOURTHS = 3 * ONE_FOURTH;

public:
	void fixDistribution(ModelMetrics::ProbRange& range)
	{
		/*
		ModelMetrics::ProbRange range;
		range.character = character;
		range.lower = character;
		range.upper = character + 1;
		range.denom = ModelMetrics::TotalUniqueChars;
		return range;*/
	}

};

}
