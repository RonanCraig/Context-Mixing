#pragma once
#include <memory>
#include <string>
#include "Config.h"
#include "Model.h"
#include "ProbRange.h"
#include "ArithmeticEncoder.h"

namespace compression
{

class PPM : public Model
{

struct Node
{
	characterCode character;
	int count = 1;
	Node* child = nullptr;
	Node* sibling = nullptr;
	Node* vine = nullptr;
	Node(const characterCode& character) : character(character) {}
};

class NodeTraverser
{
// Attributes
protected:
	characterCode character;
	Node** next;
	Node* vine;

// Methods
public:
	NodeTraverser(const characterCode& character, Node* base, Node* root);
	
protected:
	virtual Node* run();
	virtual void traverse();
	virtual void addNode();
};

class CountingNodeTraverser : public NodeTraverser
{
// Attributes
protected:
	int uniqueCount = 0;
	int count = 0;
	int totalCount = 0;
	bool contextFound = false;

// Methods
public:
	CountingNodeTraverser(const characterCode& character, Node* base, Node* root) : NodeTraverser(character, base, root) {}
	
protected:
	virtual Node* run();
	virtual void traverse();
	int calculateEscapeCount();

};

class EncodingTraverser : CountingNodeTraverser
{
// Attributes
private:
	ArithmeticEncoder& encoder;

// Methods
public:
	EncodingTraverser(const characterCode& character, Node* base, Node* root, ArithmeticEncoder& encoder) : CountingNodeTraverser(character, base, root), encoder(encoder) {}

private:
	void addNode();
	void encode(int upper, int lower, int denom, characterCode character);
};

// Attributes
private:
	Node* base;
	Node* root;

// Methods
public:
	// Iterates through each context until the character has been seen before at that context or -1 context.
	// Creates a probablilty range for the escape character if the character has not been seen before at that context.
	// Creates a probability range for the character to encode if the character has been seen before.
	void encode(const characterCode& charToEncode, ArithmeticEncoder& encoder);
	// Similar to encode except uses code instead of character to find ranges.
	characterCode decode(ArithmeticDecoder& decoder);
	// Updates the counts for contexts.
	void update(const characterCode& charToUpdate);
	PPM();

private:
	std::pair<Node*, ProbRange> getNodeAndRange(Node* parentNode, const characterCode& charToEncode);

};

}