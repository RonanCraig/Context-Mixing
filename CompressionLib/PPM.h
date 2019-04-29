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
class Exclusions
{
	bool exclusions[256];
	struct Node
	{
		Node* next;
		types::node::characterType character;
		Node(Node* next, types::node::characterType character) : next(next), character(character) {}
	};
	Node* start = nullptr;
public:
	bool add(types::node::characterType characterToExclude)
	{
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
		while (start != nullptr)
		{
			exclusions[start->character] = false;
			Node* next = start->next;
			delete start;
			start = next;
		}
	}
};

struct Counts
{
	types::countType lower = 0;
	types::countType upper = 0;
	types::countType denom = 0;
	types::countType uniqueCount = 0;
};

// Attributes
private:
	Exclusions exclusions;
	double probability = 1;
	types::ProbRange* ranges;
	int order;
	Counts counts;
	bool found = false;
	types::characterType character;

public:
	bool finished = false;

// Methods
public:
	Order() {};
	Order(int order) : order(order) { ranges = new types::ProbRange[5]; }
	void reset(types::characterType character);
	void update(const Node& node, const int order);
	double getProbability() { return probability; }
	types::ProbRange* getRanges() { return ranges; }
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
	static const int numberOfOrders = 1;

	Node root;
	Node* base;
	Node* currentNode;

	int depthReached = 0;
	int maxDepth;
	int maxOrderSize;
	Order orders[numberOfOrders];

// Methods
public:
	void update(const types::characterType& charToUpdate);
	void encode(ArithmeticEncoder& encoder, int order);
	PPM(int order) : maxDepth(order), base(&root), currentNode(&root) { orders[0] = Order(4);}
	double getEstimatedProb(int order = 0);
	static types::countType calculateEscapeCount(const types::countType& uniqueCount, const types::countType& totalCount);

private:
	Node* updateNode(Node& currentNode, int currentDepth, types::characterType character);
	void initialiseVine();
};

}