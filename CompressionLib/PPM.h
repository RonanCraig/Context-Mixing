#pragma once
#include <string>
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
public:
	types::countType cumulativeCount = 0;
	types::countType totalCount = 0;
	types::countType uniqueCount = 0;
	// Methods
public:
	CountingSiblingTraverser(Node** startNode, const types::characterType& character) { iterator = startNode; countToEnd(character); }
	CountingSiblingTraverser(Node** startNode) { iterator = startNode; countToEnd(); }
	void countToCount(Node** startNode, const types::countType& count);
private:
	void countToEnd(const types::characterType& character);
	void countToEnd();
	void next();
};

// Attributes
protected:
	types::characterType character;
	Node* vine;
	Node* root;
	Node* base;
	types::byte depth = 0;

// Methods
public:
	NodeTraverser(Node& root, Node& base) : root(&root), base(&base) {}
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
	EncodingTraverser(Node& root, Node& base, ArithmeticEncoder& encoder) : NodeTraverser(root, base), encoder(encoder) {}
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
	DecodingTraverser(Node& root, Node& base, ArithmeticDecoder& decoder) : NodeTraverser(root, base), decoder(decoder) {}
	types::characterType traverse();
private:
	void decode(types::countType upper, types::countType lower, types::countType denom);
};

// Attributes
private:
	static const int MaxOrderSize = 6; // Max size of order used by PPM.
	Node& base;
	Node root;
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
	PPM();
	PPM(ArithmeticEncoder& encoder);
	PPM(ArithmeticDecoder& decoder);

};

}