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
	int count;
	Node* child = nullptr;
	Node* sibling = nullptr;
	Node* vine = nullptr;
	Node(const characterCode& character, int count = 1) : character(character), count(count) {}
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
	virtual void traverse(const characterCode& character, Node** base, Node* root);
	
protected:
	virtual Node* traverseChildren();
	virtual void nextNode();
	virtual void addNode();
};

class CountingNodeTraverser : public NodeTraverser
{
// Attributes
protected:
	int totalCount = 0;
	int escapeCount = 0;
	int uniqueCount = 0;

// Methods
protected:
	void calculateTotalAndEscapeCounts();
};

class EncodingTraverser : public CountingNodeTraverser
{
// Attributes
private:
	ArithmeticEncoder& encoder;
	int count = 0;
	bool contextFound = false;

// Methods
public:
	EncodingTraverser(ArithmeticEncoder& encoder) : encoder(encoder) {}
private:
	void addNode();
	void encode(int upper, int lower, int denom, characterCode character);
	virtual Node* traverseChildren();
	virtual void nextNode();
};

class DecodingTraverser : public CountingNodeTraverser
{
// Attributes
private:
	ArithmeticDecoder& decoder;

// Methods
public:
	DecodingTraverser(ArithmeticDecoder& decoder) : decoder(decoder) {}
	characterCode traverse(Node** base, Node* root);
private:
	void findCharacterForCount(int& charCount);
	void decode(int upper, int lower, int denom, characterCode character);
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

};

}