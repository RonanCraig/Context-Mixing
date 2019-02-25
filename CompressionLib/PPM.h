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
	int count = 0;
	Node* child = nullptr;
	Node* sibling = nullptr;
	Node* vine = nullptr;
	Node(const characterCode& character, int count = 0) : character(character), count(count) {}
};

class NodeTraverser
{
public:
	// Methods
	virtual Node* traverse();
	Node* addNode(const characterCode& character);
	Node* findNode(const characterCode& charToFind);
	void iterateToEnd();
	NodeTraverser(Node* parent);
	Node** next;
};

class CountingNodeTraverser : public NodeTraverser
{
public:
	// Attributes
	int count = 0;

	// Methods
	CountingNodeTraverser(Node* parent);
	Node* traverse();
	int getEscapeCount();
private:
	int escapeCount = 0;
};

// Attributes
private:
	Node* basePtr;
	Node* rootPtr;

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

#ifdef _DEBUG
	void outputDebug(std::ofstream& outputFileStream);
#endif // DEBUG

private:
	std::pair<Node*, ProbRange> getNodeAndRange(Node* parentNode, const characterCode& charToEncode);

};

}