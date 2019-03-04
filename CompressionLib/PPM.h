#pragma once
#include <string>
#include <set>
#include "Config.h"
#include "Model.h"
#include "ProbRange.h"
#include "ArithmeticEncoder.h"

namespace compression
{

class PPM : public Model
{

class Node
{
// Attributes
private:
	mutable bool totalCountEqualsCount = true;

public:
	mutable int count = 0;
	const characterCode character;
	mutable std::set<Node> children;
	const mutable Node* vine = nullptr;

// Methods
public:
	Node(const characterCode& character) : character(character) {}
	int getTotalCount() const;
	void increaseCount(const Node* parent) const;
	bool operator=(Node& rhs) { return rhs.character == character; }
	bool operator<(const Node& rhs) const { return character < rhs.character; }
};

class NodeTraverser
{
// Attributes
protected:
	characterCode character;
	const Node* vine;
	const Node* root;
	const Node* base;
	byte depth = 0;

// Methods
public:
	NodeTraverser(Node& root, Node& base) : root(&root), base(&base) {}
	virtual void traverse(const characterCode& character);
	
protected:
	void initialiseVine();
	virtual const Node* updateNode();
	int calculateEscapeCount();
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
	void traverse(const characterCode& character);
private:
	const Node* updateNode();
	void encode(int upper, int lower, int denom, characterCode character);
};

class DecodingTraverser : public NodeTraverser
{
// Attributes
private:
	ArithmeticDecoder& decoder;

// Methods
public:
	DecodingTraverser(Node& root, Node& base, ArithmeticDecoder& decoder) : NodeTraverser(root, base), decoder(decoder) {}
	characterCode traverse();
private:
	void decode(int upper, int lower, int denom, characterCode character);
};

// Attributes
private:
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
	void encode(const characterCode& charToEncode);
	// Similar to encode except uses code instead of character to find ranges.
	characterCode decode();
	// Updates the counts for contexts.
	void update(const characterCode& charToUpdate);
	PPM();
	PPM(ArithmeticEncoder& encoder);
	PPM(ArithmeticDecoder& decoder);

};

}