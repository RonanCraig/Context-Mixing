#pragma once
#include "CompressionTypes.h"
#include <cmath>

namespace compression
{

class Arithmetic
{
public:

	static const types::characterType END_CHARACTER = std::numeric_limits<types::node::characterType>::max() + 1;
	static const types::countType TOTAL_UNIQUE_CHARS = END_CHARACTER + 1;
	static const unsigned long MAX_COUNT = std::numeric_limits<types::node::countType >::max();

protected:
	typedef unsigned long long CODE_VALUE;
	
	static constexpr unsigned long MAX_DENOM = TOTAL_UNIQUE_CHARS * MAX_COUNT;
	static const int CODE_VALUE_BITS = (std::numeric_limits<CODE_VALUE>::digits + 3) / 2;
	static const int FREQUENCY_BITS = std::numeric_limits<CODE_VALUE>::digits - CODE_VALUE_BITS;
	static const int PRECISION = std::numeric_limits<CODE_VALUE>::digits;

	static const CODE_VALUE MAX_CODE = (CODE_VALUE(1) << CODE_VALUE_BITS) - 1;
	static const CODE_VALUE MAX_FREQ = (CODE_VALUE(1) << FREQUENCY_BITS) - 1;

	static_assert(std::numeric_limits<CODE_VALUE>::digits >= CODE_VALUE_BITS,
		"CODE_VALUE_BITS is too large to fit in a CODE_VALUE type");
	static_assert(FREQUENCY_BITS <= (CODE_VALUE_BITS + 2),
		"FREQUENCY_BITS can be no greater than CODE_VALUE_BITS - 2");
	static_assert((CODE_VALUE_BITS + FREQUENCY_BITS) <= PRECISION,
		"CODE_VALUE_BITS + FREQUENCY_BITS cannot exceed precision of CODE_VALUE");
	static_assert(MAX_DENOM <= MAX_FREQ,
		"MAX_DENOM cannot exceed MAX_FREQ");


	static const CODE_VALUE ONE_FOURTH = CODE_VALUE(1) << (CODE_VALUE_BITS - 2);
	static const CODE_VALUE ONE_HALF = 2 * ONE_FOURTH;
	static const CODE_VALUE THREE_FOURTHS = 3 * ONE_FOURTH;

};

}
