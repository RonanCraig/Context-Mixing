#include "PPM.h"
#include <fstream>
#include <set>

using namespace std;
using namespace compression;
using namespace types;

void PPM::encode(const characterType& charToEncode)
{
	encodingTraverser->traverse(charToEncode);
}

characterType PPM::decode()
{
	return decodingTraverser->traverse();
}

void PPM::update(const characterType& charToUpdate)
{
	traverser->traverse(charToUpdate);
}

PPM::PPM() : root(Node(0)), base(root)
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

void PPM::NodeTraverser::traverse(const characterType& character)
{
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
	if (depth <= MaxOrderSize)
	{
		vine = base;
		depth++;
	}
	else
		vine = base->vine;
}

void PPM::NodeTraverser::reduceCounts()
{
	for (auto it = vine->children.begin(); it != vine->children.end(); it++)
	{
		it->count = it->count / 2;
		if (it->count == 0)
			it->count++;
	}
		
}

const PPM::Node* PPM::NodeTraverser::updateNode()
{
	// TODO: Maybe make function take iterator as parameter.
	
	auto it = vine->children.insert(Node(character));
	const Node* updatedNode = &(*(it.first));
	if (updatedNode->count == Arithmetic::MAX_COUNT)
		reduceCounts();
	updatedNode->increaseCount(vine);
	return updatedNode;
}

// EncodingTraverser

void PPM::EncodingTraverser::traverse(const characterType& character)
{
	contextFound = false;
	NodeTraverser::traverse(character);
}

const PPM::Node* PPM::EncodingTraverser::updateNode()
{
	if (!contextFound)
	{
		countType cumulativeCount = 0;
		countType escapeCount = calculateEscapeCount();
		countType totalCount = vine->getTotalCount();
		countType denom = totalCount + escapeCount;

		auto it = vine->children.begin();
		for (it; it != vine->children.end() && it->character != character; it++)
			cumulativeCount += it->count;

		if (it == vine->children.end())
		{
			// Encode escape.
			encode(denom, totalCount, denom);

			// Check vine == null then encode -1.
			if (vine->vine == nullptr)
				encode((character + 1), character, Arithmetic::TOTAL_UNIQUE_CHARS);
		}
		else
		{
			contextFound = true;
			// Encode character.
			encode(cumulativeCount + it->count, cumulativeCount, denom);
		}
	}
	return NodeTraverser::updateNode();
}

countType PPM::NodeTraverser::calculateEscapeCount()
{
	countType uniqueCount = vine->children.size();
	countType escapeCount = vine->getTotalCount() / ((uniqueCount < 1) ? 1 : uniqueCount);
	return 1;
	return ((escapeCount < 1) ? 1 : escapeCount);
}

void PPM::EncodingTraverser::encode(countType upper, countType lower, countType denom)
{
	ProbRange range;
	range.lower = lower;
	range.upper = upper;
	range.denom = denom;
	encoder.encode(range);
}

// DecodingTraverser

characterType PPM::DecodingTraverser::traverse()
{
	// TODO: make it better.
	byte temp = depth;
	initialiseVine();
	depth = temp;

	bool found = false;
	characterType charToUpdate;
	while (vine && !found)
	{
		countType escapeCount = calculateEscapeCount();
		countType totalCount = vine->getTotalCount();
		countType denom = totalCount + escapeCount;
		countType charCount = decoder.getCount(denom);

		if (charCount >= totalCount)
		{
			decode(denom, totalCount, denom);
			if (vine->vine == nullptr)
			{
				charCount = decoder.getCount(Arithmetic::TOTAL_UNIQUE_CHARS);
				charToUpdate = charCount;
				decode(charCount + 1, charCount, Arithmetic::TOTAL_UNIQUE_CHARS);
			}
		}
		else
		{
			found = true;
			countType cumulativeCount = 0;
			auto it = vine->children.begin();
			for (it; it != vine->children.end() && cumulativeCount + it->count <= charCount; it++)
				cumulativeCount += it->count;

			charToUpdate = it->character;
			decode(cumulativeCount + it->count, cumulativeCount, denom);
		}
		vine = vine->vine;
	}

	NodeTraverser::traverse(charToUpdate);
	return charToUpdate;
}

void PPM::DecodingTraverser::decode(countType upper, countType lower, countType denom)
{
	ProbRange range;
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

countType PPM::Node::getTotalCount() const
{
	if (totalCountEqualsCount)
		return count;
	else
		return count - 1;
}