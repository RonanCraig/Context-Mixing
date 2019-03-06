#pragma once
#include "CompressionTypes.h"

namespace compression
{

namespace ModelMetrics
{

typedef unsigned long long CODE_VALUE;
typedef unsigned int count;

static const types::characterCode EndCharacter = 256;

static const int CODE_VALUE_BITS = std::numeric_limits<count>::digits;
static const int FREQUENCY_BITS = std::numeric_limits<CODE_VALUE>::digits - CODE_VALUE_BITS;

//static const int CODE_VALUE_BITS = (std::numeric_limits<CODE_VALUE>::digits + 3) / 2;
//static const int FREQUENCY_BITS = std::numeric_limits<CODE_VALUE>::digits - CODE_VALUE_BITS;
static const int PRECISION = std::numeric_limits<CODE_VALUE>::digits;

static const CODE_VALUE MAX_CODE = (CODE_VALUE(1) << CODE_VALUE_BITS) - 1;
static const CODE_VALUE MAX_FREQ = (CODE_VALUE(1) << FREQUENCY_BITS) - 1;

static const CODE_VALUE TotalUniqueChars = 257;


static_assert(std::numeric_limits<CODE_VALUE>::digits >= CODE_VALUE_BITS,
	"CODE_VALUE_BITS is too large to fit in a CODE_VALUE type");
static_assert(FREQUENCY_BITS <= (CODE_VALUE_BITS + 2),
	"FREQUENCY_BITS can be no greater than CODE_VALUE_BITS - 2");
static_assert((CODE_VALUE_BITS + FREQUENCY_BITS) <= PRECISION,
	"CODE_VALUE_BITS + FREQUENCY_BITS cannot exceed precision of CODE_VALUE");
static_assert(TotalUniqueChars < MAX_FREQ,
	"TotalUniqueChars must be less than MAX_FREQ");

struct ProbRange
{
	ProbRange() {}
	ProbRange(types::characterCode c, CODE_VALUE up, CODE_VALUE low, CODE_VALUE denom) : character(c), upper(up), lower(low), denom(denom) {}
	// Represents lower bound and upper bound of the range as fractions, sharing a common demoninator.
	types::characterCode character;
	CODE_VALUE upper;
	CODE_VALUE lower;
	CODE_VALUE denom;
};

}

}