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

PPM::PPM(int order) : base(root)
{
	traverser = new NodeTraverser(root, base, order);
}

PPM::PPM(ArithmeticEncoder& encoder, int order) : PPM(order)
{
	encodingTraverser = new EncodingTraverser(root, base, encoder, order);
}

PPM::PPM(ArithmeticDecoder& decoder, int order) : PPM(order)
{
	decodingTraverser = new DecodingTraverser(root, base, decoder, order);
}

// NodeTraverser

void PPM::NodeTraverser::traverse(const characterType& character)
{
	this->character = character;
	initialiseVine();
	
	base = updateNode();
	Node* previousAdded = base;
	while (vine = vine->vine)
	{
		previousAdded->vine = updateNode();
		previousAdded = previousAdded->vine;
	}
	previousAdded->vine = root;
	root->count++;
}

void PPM::NodeTraverser::initialiseVine()
{
	if (depth <= maxOrderSize)
	{
		vine = base;
		depth++;
	}
	else
		vine = base->vine;
}

PPM::Node* PPM::NodeTraverser::updateNode()
{
	SiblingTraverser traverser(vine, &(vine->child), character);
	return *traverser.iterator;
}

// SiblingTraverser

void PPM::NodeTraverser::SiblingTraverser::insertNode(Node* parent, Node** startNode, const types::characterType& character)
{
	iterator = startNode;
	while (*iterator != nullptr && (*iterator)->character != character)
		iterator = &(*iterator)->sibling;
	if (*iterator == nullptr)
		*iterator = new Node(character);

	(*iterator)->count++;

	if ((*iterator)->count == Arithmetic::MAX_COUNT)
	{
		Node* currentNode = parent->child;
		while (currentNode)
		{
			currentNode->count = currentNode->count / 2;
			if (currentNode->count == 0)
				currentNode->count++;
			currentNode = currentNode->sibling;
		}
	}
}

// EncodingTraverser

void PPM::EncodingTraverser::traverse(const characterType& character)
{
	contextFound = false;
	exclusions.resetExclusions();
	NodeTraverser::traverse(character);
}

PPM::Node* PPM::EncodingTraverser::updateNode()
{
	if (!contextFound)
	{
		CountingSiblingTraverser traverser(&(vine->child), exclusions, character);
		countType denom = traverser.totalCount + calculateEscapeCount(traverser.uniqueCount, traverser.totalCount);
		Node* characterNode = *(traverser.iterator);

		if (!characterNode)
		{
			// Encode escape.
			encode(denom, traverser.totalCount, denom);

			// Check vine == null then encode -1.
			if (vine->vine == nullptr)
				encode((character + 1), character, Arithmetic::TOTAL_UNIQUE_CHARS);
		}
		else
		{
			contextFound = true;
			// Encode character.
			encode(traverser.cumulativeCount + characterNode->count, traverser.cumulativeCount, denom);
		}
		traverser.insertNode(vine, traverser.iterator, character);
		return *traverser.iterator;
	}
	return NodeTraverser::updateNode();
}

countType PPM::NodeTraverser::calculateEscapeCount(const countType& uniqueCount, const countType& totalCount)
{
	countType escapeCount = totalCount / ((uniqueCount < 1) ? 1 : uniqueCount);
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

// CountingSiblingTraverser

void PPM::EncodingTraverser::CountingSiblingTraverser::countToEnd(const types::characterType& character)
{
	while (*iterator != nullptr && (*iterator)->character != character)
		next();

	cumulativeCount = totalCount;
	Node** temp = iterator;

	while (*iterator != nullptr)
		next();

	iterator = temp;
}

void PPM::EncodingTraverser::CountingSiblingTraverser::countToEnd()
{
	while (*iterator != nullptr)
		next();
}

void PPM::EncodingTraverser::CountingSiblingTraverser::countToCount(Node** startNode, const countType& count)
{
	totalCount = 0;
	iterator = startNode;
	bool found = false;
	while (*iterator != nullptr && !found)
	{
		if (!exclusions.add((*iterator)->character))
		{
			if ((totalCount + (*iterator)->count) > count)
				found = true;
			else
			{
				totalCount += (*iterator)->count;
				uniqueCount++;
			}
			
		}
		if(!found)
			iterator = &(*iterator)->sibling;
	}

	cumulativeCount = totalCount;
}

void PPM::EncodingTraverser::CountingSiblingTraverser::next()
{
	if (!exclusions.add((*iterator)->character))
	{
		totalCount += (*iterator)->count;
		uniqueCount++;
	}
	iterator = &(*iterator)->sibling;
}

// DecodingTraverser

characterType PPM::DecodingTraverser::traverse()
{
	// TODO: make it better: exclusion temp.
	byte temp = depth;
	initialiseVine();
	depth = temp;

	exclusions.resetExclusions();

	bool found = false;
	characterType charToUpdate;
	while (vine && !found)
	{
		Exclusions exclusionsTemp = exclusions;
		CountingSiblingTraverser traverser(&(vine->child), exclusions);
		countType denom = traverser.totalCount + calculateEscapeCount(traverser.uniqueCount, traverser.totalCount);
		countType charCount = decoder.getCount(denom);

		if (charCount >= traverser.totalCount)
		{
			decode(denom, traverser.totalCount, denom);
			if (vine->vine == nullptr)
			{
				charCount = decoder.getCount(Arithmetic::TOTAL_UNIQUE_CHARS);
				charToUpdate = charCount;
				decode(charCount + 1, charCount, Arithmetic::TOTAL_UNIQUE_CHARS);
			}
		}
		else
		{
			exclusions = exclusionsTemp;
			found = true;
			traverser.countToCount(&(vine->child), charCount);

			charToUpdate = (*traverser.iterator)->character;
			decode(traverser.cumulativeCount + (*traverser.iterator)->count, traverser.cumulativeCount, denom);
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