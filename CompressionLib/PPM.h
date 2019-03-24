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

class SiblingTraverser
{
	// Attributes
private:
	Exclusions& exclusions;
	const types::characterType character;
	Node& parentNode;
public:
	Node** iterator;
	bool found = false;
	types::countType cumulativeCount = 0;
	types::countType totalCount = 0;
	types::countType uniqueCount = 0;
	// Methods
public:
	SiblingTraverser(Node& parentNode, Exclusions& exclusions, const types::characterType character) : exclusions(exclusions), parentNode(parentNode), character(character), iterator(&parentNode.child) { traverse(); }
	//SiblingTraverser(Node& parentNode, Exclusions& exclusions) : exclusions(exclusions), parentNode(parentNode), iterator(&parentNode.child) { countToEnd(); }
	void countToCount(Node** startNode, const types::countType& count);
private:
	void traverse();
	void next();
	void insertNode();
};

// Attributes
private:
	static const types::characterType escapeCharacter = 256;

	Node* base;
	Node root;
	Node* currentNode;

	int depthReached = 0;
	int maxDepth;
	int maxOrderSize;
	Exclusions exclusions;
	std::pair<types::ProbRange, types::characterType>* rangesForOrders;

// Methods
public:
	//types::characterType decode(ArithmeticDecoder& decoder);
	void update(const types::characterType& charToUpdate);
	PPM(int order) : maxDepth(order), base(&root), currentNode(&root) { rangesForOrders = new std::pair<types::ProbRange, types::characterType>[order + 1]; }
	types::ProbRange getProbability(int order);

private:
	void setProbabilityRange(types::countType upper, types::countType lower, types::countType denom, int currentDepth, types::characterType character);
	Node* updateNode(Node& currentNode, int currentDepth, types::characterType character);
	void initialiseVine();
	types::countType calculateEscapeCount(const types::countType& uniqueCount, const types::countType& totalCount);
};

}