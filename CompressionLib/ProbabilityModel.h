#pragma once
#include <memory>
#include <vector>
#include <map>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/map.hpp>

namespace compression
{

struct ProbRange
{
	// Represents lower bound and upper bound of the range as fractions, sharing a common demoninator.
	char character;
	int upper;
	int lower;
	int denom;
};

typedef std::vector<std::unique_ptr<ProbRange>> ProbRanges;

class ProbabilityModel
{

public:
class CharTable
{
//Attributes
private:
	int totalCount = 0;
	std::map<char, std::pair<int, int>> charMap;

//Methods
public:
	CharTable() { increaseSymbolCount(ProbabilityModel::EscapeCharacter); }
	CharTable(bool isNegativeOrder) {}
	const int& getTotalCount() const { return totalCount; }

	bool increaseSymbolCount(const char& c);
	std::unique_ptr<ProbRange> calculateRange(const char& c) const;
	std::unique_ptr<ProbRange> calculateRange(const int& count) const;

private:
	// Used for boost serialization
	friend class boost::serialization::access;
	friend class ProbabilityModel;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) { ar & totalCount; ar & charMap; }
};

//Attributes
public:
	static const char EscapeCharacter = '&';
	static const char EndCharacter = '*';
	static const unsigned int MaxOrderSize = 5; // Max size of order used by PPM.

private:
	typedef std::map<std::string, CharTable> ContextTable;
	typedef std::map<int, ContextTable> OrderTable;
	OrderTable orderTable;
	CharTable orderNegativeOneTable = CharTable(true);

//Methods
public:
	CharTable& getCharTable(const std::string* context = nullptr);
	#ifdef _DEBUG
		void outputTables(const std::string& directory);
	#endif

private:
	// Used for boost serialization
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version){ar & orderTable; ar & orderNegativeOneTable;}

};

}