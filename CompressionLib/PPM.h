#pragma once
#include <string>
#include <set>
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
private:
	mutable bool totalCountEqualsCount = true;

public:
	mutable ModelMetrics::CODE_VALUE count = 0;
	const types::characterCode character;
	mutable std::set<Node> children;
	const mutable Node* vine = nullptr;

// Methods
public:
	Node(const types::characterCode& character) : character(character) {}
	ModelMetrics::CODE_VALUE getTotalCount() const;
	void increaseCount(const Node* parent) const;
	bool operator=(Node& rhs) { return rhs.character == character; }
	bool operator<(const Node& rhs) const { return character < rhs.character; }
};

class NodeTraverser
{
// Attributes
protected:
	types::characterCode character;
	const Node* vine;
	const Node* root;
	const Node* base;
	types::byte depth = 0;

// Methods
public:
	NodeTraverser(Node& root, Node& base) : root(&root), base(&base) {}
	virtual void traverse(const types::characterCode& character);
	
protected:
	void initialiseVine();
	virtual const Node* updateNode();
	ModelMetrics::CODE_VALUE calculateEscapeCount();
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
	void traverse(const types::characterCode& character);
private:
	const Node* updateNode();
	void encode(ModelMetrics::CODE_VALUE upper, ModelMetrics::CODE_VALUE lower, ModelMetrics::CODE_VALUE denom, types::characterCode character);
};

class DecodingTraverser : public NodeTraverser
{
// Attributes
private:
	ArithmeticDecoder& decoder;

// Methods
public:
	DecodingTraverser(Node& root, Node& base, ArithmeticDecoder& decoder) : NodeTraverser(root, base), decoder(decoder) {}
	types::characterCode traverse();
private:
	void decode(ModelMetrics::CODE_VALUE upper, ModelMetrics::CODE_VALUE lower, ModelMetrics::CODE_VALUE denom, types::characterCode character);
};

// Attributes
private:
	static const types::characterCode EscapeCharacter = 257;
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
	void encode(const types::characterCode& charToEncode);
	// Similar to encode except uses code instead of character to find ranges.
	types::characterCode decode();
	// Updates the counts for contexts.
	void update(const types::characterCode& charToUpdate);
	PPM();
	PPM(ArithmeticEncoder& encoder);
	PPM(ArithmeticDecoder& decoder);

};

}