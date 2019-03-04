#include "PPM.h"
#include <fstream>
#include <set>
#include <vector>

using namespace std;
using namespace compression;

void PPM::encode(const characterCode& charToEncode)
{
	encodingTraverser->traverse(charToEncode);
}

characterCode PPM::decode()
{
	unsigned int totalCount = 15266;
	unsigned int range = 1704352003;
	unsigned int upper = 2700723523;
	unsigned int lower = 996371520;
	unsigned int value = 2663536439;
	unsigned int x1 = value - lower;
	unsigned int x2 = range / totalCount + (range % totalCount != 0);
	unsigned int x3 = x1 / x2;
	return decodingTraverser->traverse();
}

void PPM::update(const characterCode& charToUpdate)
{
	traverser->traverse(charToUpdate);
}

PPM::PPM() : root(Node(config::EscapeCharacter)), base(root)
{
	traverser = new NodeTraverser(root, base);
}

PPM::PPM(ArithmeticEncoder& encoder) : PPM()
{
	encodingTraverser = new EncodingTraverser(root, base, encoder);
}

PPM::PPM(ArithmeticDecoder& decoder) : PPM()
{
	decodingTraverser = new DecodingTraverser(root, base, decoder);
}

// NodeTraverser

void PPM::NodeTraverser::traverse(const characterCode& character)
{
	if (root->count == 3087217)
	{
		int a = 5;
	}
	this->character = character;
	initialiseVine();
	
	base = updateNode();
	const Node* previousAdded = base;
	while (vine = vine->vine)
	{
		previousAdded->vine = updateNode();
		previousAdded = previousAdded->vine;
	}
	previousAdded->vine = root;
	root->increaseCount(root);
}

void PPM::NodeTraverser::initialiseVine()
{
	if (depth <= config::MaxOrderSize)
	{
		vine = base;
		depth++;
	}
	else
		vine = base->vine;
}

const PPM::Node* PPM::NodeTraverser::updateNode()
{
	// TODO: Maybe make function take iterator as parameter.
	auto it = vine->children.insert(Node(character));
	const Node* updatedNode = &(*(it.first));
	updatedNode->increaseCount(vine);
	return updatedNode;
}

// EncodingTraverser

void PPM::EncodingTraverser::traverse(const characterCode& character)
{
	contextFound = false;
	NodeTraverser::traverse(character);
}

const PPM::Node* PPM::EncodingTraverser::updateNode()
{
	if (!contextFound)
	{
		int cumulativeCount = 0;
		int escapeCount = calculateEscapeCount();
		int totalCount = vine->getTotalCount();
		int denom = totalCount + escapeCount;

		auto it = vine->children.begin();
		for (it; it != vine->children.end() && it->character != character; it++)
			cumulativeCount += it->count;

		if (it == vine->children.end())
		{
			// Encode escape.
			encode(denom, totalCount, denom, config::EscapeCharacter);

			// Check vine == null then encode -1.
			if (vine->vine == nullptr)
				encode((character + 1), character, config::TotalUniqueChars, character);
		}
		else
		{
			contextFound = true;
			// Encode character.
			encode(cumulativeCount + it->count, cumulativeCount, denom, character);
		}
	}
	return NodeTraverser::updateNode();
}

int PPM::NodeTraverser::calculateEscapeCount()
{
	int uniqueCount = vine->children.size();
	int escapeCount = vine->getTotalCount() / ((uniqueCount < 1) ? 1 : uniqueCount);
	return ((escapeCount < 1) ? 1 : escapeCount);
}

void PPM::EncodingTraverser::encode(int upper, int lower, int denom, characterCode character)
{
	ProbRange range;
	range.character = character;
	range.lower = lower;
	range.upper = upper;
	range.denom = denom;
	encoder.encode(range);
}

// DecodingTraverser

characterCode PPM::DecodingTraverser::traverse()
{
	// TODO: make it better.
	byte temp = depth;
	initialiseVine();
	depth = temp;

	bool found = false;
	characterCode charToUpdate;
	while (vine && !found)
	{
		int escapeCount = calculateEscapeCount();
		int totalCount = vine->getTotalCount();
		int denom = totalCount + escapeCount;
		unsigned int charCount = decoder.getCount(denom);

		if (charCount >= totalCount)
		{
			decode(denom, totalCount, denom, config::EscapeCharacter);
			if (vine->vine == nullptr)
			{
				charCount = decoder.getCount(config::TotalUniqueChars);
				decode(charCount + 1, charCount, config::TotalUniqueChars, charCount);
				charToUpdate = charCount;
			}
		}
		else
		{
			found = true;
			int cumulativeCount = 0;
			auto it = vine->children.begin();
			for (it; it != vine->children.end() && cumulativeCount + it->count <= charCount; it++)
				cumulativeCount += it->count;

			charToUpdate = it->character;
			decode(cumulativeCount + it->count, cumulativeCount, denom, charToUpdate);
		}
		vine = vine->vine;
	}
	NodeTraverser::traverse(charToUpdate);
	return charToUpdate;
}

void PPM::DecodingTraverser::decode(int upper, int lower, int denom, characterCode character)
{
	ProbRange range;
	range.character = character;
	range.lower = lower;
	range.upper = upper;
	range.denom = denom;
	decoder.decode(range);
}

// Node

void PPM::Node::increaseCount(const Node* parent) const
{
	count++;
	totalCountEqualsCount = false;
	parent->totalCountEqualsCount = true;
}

int PPM::Node::getTotalCount() const
{
	if (totalCountEqualsCount)
		return count;
	else
		return count - 1;
}