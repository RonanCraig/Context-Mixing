#pragma once
#include <string>
#include <vector>
#include <list>
#include "Model.h"
#include "ArithmeticEncoder.h"
#include "ArithmeticDecoder.h"
#include "CompressionTypes.h"

namespace compression
{

class PPM : public Model
{

class Node
{
	// Attributes
public:
	types::node::countType count = 0;
	const types::node::characterType character;
	Node* child = nullptr;
	Node* sibling = nullptr;
	Node* vine = nullptr;

	// Methods
public:
	Node() : character(0) {}
	Node(const types::byte& character) : character(character) {}
	//bool operator=(Node& rhs) { return rhs.character == character; }
};

class Order
{

struct Counts
{
	types::countType lower = 0;
	types::countType upper = 0;
	types::countType denom = 0;
	types::countType uniqueCount = 0;

	void reset()
	{
		denom = 0;
		lower = 0;
		upper = 0;
		uniqueCount = 0;
	}
};

public:
class Exclusions
{
public:
	// A bool for every character, true meaning it has been excluded.
	bool exclusions[256] = { false };
	struct Node
	{
		// A list of nodes of excluded characters is used to quickly reset the exclusions array through iterating 
		// through the list and accessing the array with the character, this is faster than iterating through the array.
		Node* next;
		types::node::characterType character;
		Node(Node* next, types::node::characterType character) : next(next), character(character) {}
	};
	Node* start = nullptr;

	bool add(types::node::characterType characterToExclude)
	{
		// Returns if it has been excluded and sets it as excluded.
		bool* exclusionPointer = &exclusions[characterToExclude];
		bool exclusion = *exclusionPointer;
		if (!exclusion)
		{
			*exclusionPointer = true;
			Node* newNode = new Node(start, characterToExclude);
			start = newNode;
		}
		return exclusion;
	}
	void resetExclusions()
	{
		// Loops through the list of excluded nodes to access the array and reset it to false.
		while (start != nullptr)
		{
			exclusions[start->character] = false;
			Node* next = start->next;
			delete start;
			start = next;
		}
	}

	void resetLastExclusions(int toReset)
	{
		for (toReset; toReset > 0; toReset--)
		{
			exclusions[start->character] = false;
			Node* next = start->next;
			delete start;
			start = next;
		}
	}
};

// Attributes
public:
	int order;
	Exclusions exclusions;
	double bw = 0.33;
	double probability = 1;
private:
	std::vector<types::ProbRange> ranges;
	Counts counts;
	bool found = false;
	types::characterType character;

public:
	bool finished = false;

// Methods
public:
	Order() {};
	Order(int order) : order(order) { for (int a = 0; a < order + 2; a++) { ranges.push_back(types::ProbRange()); } } // For an order n there are n + 2 ranges, 1 extra for -1, 1 extra for 0.
	void reset(types::characterType character);
	void update(const Node& node, const int order);
	void updateNegativeOrder();
	types::ProbRange getCharacter(Node& node, ArithmeticDecoder decoder);
	double getProbability() { return probability; }
	std::vector<types::ProbRange>& getRanges() { return ranges; }
	~Order()
	{
		exclusions.resetExclusions();
	}
};

class ChildrenIterator
{
// Attributes
private:
	Node** iterator;
	Node& parentNode;

// Methods
public:
	ChildrenIterator(Node& parentNode) : iterator(&parentNode.child), parentNode(parentNode) {}
	bool increment() { return *(iterator = &node()->sibling); }
	types::characterType character() { return node()->character; }
	types::countType count() { return node()->count; }
	Node* node() { return *iterator; }
	Node* insertNode(types::characterType character);
};

// Attributes
public:
	static const types::characterType escapeCharacter = 256;

private:
	static const int numberOfOrders = 3; // TODO: make better.

	Node root;
	Node* base;
	Node* currentNode;

	int depthReached = 0;
	int maxDepth;
	int maxOrderSize;
	Order orders[numberOfOrders];
	Order* bestOrder = &orders[0];

// Methods
public:
	bool update(const types::characterType& charToUpdate);
	void encode(ArithmeticEncoder& encoder);
	types::characterType decode(ArithmeticDecoder& decoder);
	PPM();
	~PPM();
	static types::countType calculateEscapeCount(const types::countType& uniqueCount, const types::countType& totalCount);

private:
	Node* updateNode(Node& currentNode, int currentDepth, types::characterType character);
	void initialiseVine();
	void setBestOrder();
	void updateWeights();
	void deleteTree(Node* node);
};

}