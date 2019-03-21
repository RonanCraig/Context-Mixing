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
	bool operator=(Node& rhs) { return rhs.character == character; }
};

class NodeTraverser
{

protected:
class SiblingTraverser
{
// Attributes
public:
	Node** iterator;
// Methods
protected:
	SiblingTraverser() {};
public:
	SiblingTraverser(Node* parent, Node** startNode, const types::characterType& character) { insertNode(parent, startNode, character); }
	void insertNode(Node* parent, Node** startNode, const types::characterType& character);
};

class CountingSiblingTraverser : public SiblingTraverser
{
	// Attributes
private:
	Exclusions& exclusions;
public:
	types::countType cumulativeCount = 0;
	types::countType totalCount = 0;
	types::countType uniqueCount = 0;
	// Methods
public:
	CountingSiblingTraverser(Node** startNode, Exclusions& exclusions, const types::characterType& character) : exclusions(exclusions) { iterator = startNode; countToEnd(character); }
	CountingSiblingTraverser(Node** startNode, Exclusions& exclusions) : exclusions(exclusions) { iterator = startNode; countToEnd(); }
	void countToCount(Node** startNode, const types::countType& count);
private:
	void countToEnd(const types::characterType& character);
	void countToEnd();
	void next();
};

// Attributes
protected:
	Exclusions exclusions;
	//std::list<int> exclusionsToReset;
	types::characterType character;
	Node* vine;
	Node* root;
	Node* base;
	int maxOrderSize;
	types::byte depth = 0;

// Methods
public:
	NodeTraverser(Node& root, Node& base, int order) : root(&root), base(&base), maxOrderSize(order) {}
	virtual void traverse(const types::characterType& character);
	
protected:
	void initialiseVine();
	virtual Node* updateNode();
	types::countType calculateEscapeCount(const types::countType& uniqueCount, const types::countType& totalCount);
};

class EncodingTraverser : public NodeTraverser
{

// Attributes
private:
	ArithmeticEncoder& encoder;
	bool contextFound = false;

// Methods
public:
	EncodingTraverser(Node& root, Node& base, ArithmeticEncoder& encoder, int order) : NodeTraverser(root, base, order), encoder(encoder) { /*exclusions = std::vector<bool>(256, false);*/ }
	void traverse(const types::characterType& character);
private:
	Node* updateNode();
	void encode(types::countType upper, types::countType lower, types::countType denom);
};

class DecodingTraverser : public NodeTraverser
{
// Attributes
private:
	ArithmeticDecoder& decoder;

// Methods
public:
	DecodingTraverser(Node& root, Node& base, ArithmeticDecoder& decoder, int order) : NodeTraverser(root, base, order), decoder(decoder) { /*exclusions = std::vector<bool>(256, false);*/ }
	types::characterType traverse();
private:
	void decode(types::countType upper, types::countType lower, types::countType denom);
};

// Attributes
private:
	Node& base;
	static Node root;
	NodeTraverser* traverser;
	EncodingTraverser* encodingTraverser;
	DecodingTraverser* decodingTraverser;

// Methods
public:
	// Iterates through each context until the character has been seen before at that context or -1 context.
	// Creates a probablilty range for the escape character if the character has not been seen before at that context.
	// Creates a probability range for the character to encode if the character has been seen before.
	void encode(const types::characterType& charToEncode);
	// Similar to encode except uses code instead of character to find ranges.
	types::characterType decode();
	// Updates the counts for contexts.
	void update(const types::characterType& charToUpdate);
	PPM(int order);
	PPM(ArithmeticEncoder& encoder, int order);
	PPM(ArithmeticDecoder& decoder, int order);

};

}