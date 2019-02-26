#include "PPM.h"
#include <fstream>
#include <set>
#include <vector>

using namespace std;
using namespace compression;

void PPM::encode(const characterCode& charToEncode, ArithmeticEncoder& encoder)
{
	EncodingTraverser traverser(charToEncode, base, root, encoder);
}

characterCode PPM::decode(ArithmeticDecoder& decoder)
{
	return 0;
	/*
	// First node to be added will be updated as base node.
	bool contextFound = false;
	int count = 0;
	int totalCount = 0;

	Node* vinePtr = basePtr->vine;
	
	// Decode escape characters till the count is less than the escape char count.
	while (!contextFound && vinePtr != nullptr)
	{
		CountingNodeTraverser traverser(vinePtr);
		traverser.iterateToEnd();

		int escapeCount = traverser.getEscapeCount();
		totalCount = traverser.count + escapeCount;
		count = decoder.getCount(totalCount);

		if (count >= totalCount - escapeCount)
		{
			ProbRange range(config::EscapeCharacter, totalCount, totalCount - escapeCount, totalCount);
			decoder.decode(range);
			vinePtr = vinePtr->vine;
		}
		else
			contextFound = true;
	}

	ProbRange charRange;

	// If the char to be decoded is one of the children of the current vinePtr.
	if (contextFound)
	{
		CountingNodeTraverser traverser(vinePtr);
		Node* node = vinePtr->child;
		bool found = false;
		while (!found)
		{
			if (traverser.count <= count && (traverser.count + node->count) > count)
			{
				charRange.character = node->character;
				charRange.denom = totalCount;
				charRange.lower = traverser.count;
				charRange.upper = traverser.count + node->count;
				found = true;
			}
			node = traverser.traverse();
		}
	}
	// If this character had not been seen yet and was therefore encoded with order -1.
	else
	{
		count = decoder.getCount(config::TotalUniqueChars);
		charRange.character = count;
		charRange.denom = config::TotalUniqueChars;
		charRange.lower = count;
		charRange.upper = count + 1;
	}

	decoder.decode(charRange);
	
	// Update all counts with decoded character.
	update(charRange.character);
	
	return charRange.character;*/
}

void PPM::update(const characterCode& charToUpdate)
{
	/*
	// First node to be added will be updated as base node.
	Node* vinePtr = basePtr->vine;
	NodeTraverser traverser(vinePtr);
	Node* lastNodeAdded = traverser.findNode(charToUpdate);
	if (lastNodeAdded == nullptr)
		lastNodeAdded = traverser.addNode(charToUpdate);
	lastNodeAdded->count++;
	basePtr = lastNodeAdded;

	while (vinePtr = vinePtr->vine)
	{
		NodeTraverser traverser(vinePtr);
		Node* nodeAdded = traverser.findNode(charToUpdate);
		if (nodeAdded == nullptr)
			nodeAdded = traverser.addNode(charToUpdate);
		nodeAdded->count++;
		lastNodeAdded->vine = nodeAdded;
		lastNodeAdded = nodeAdded;
	}
	lastNodeAdded->vine = rootPtr;*/
}

PPM::PPM()
{
	Node* firstNode = new Node(config::EscapeCharacter);
	root = firstNode;
	for (int i = 0; i < config::MaxOrderSize + 1; i++)
	{
		Node* secondNode = new Node(config::EscapeCharacter);
		firstNode->child = secondNode;
		secondNode->vine = firstNode;
		firstNode = secondNode;
	}
	base = firstNode;
}

// NodeTraverser

PPM::NodeTraverser::NodeTraverser(const characterCode& character, Node* base, Node* root) : character(character)
{
	vine = base->vine;
	base = run();
	Node* previousAdded = base;
	while (vine = vine->vine)
	{
		previousAdded->vine = run();
		previousAdded = previousAdded->vine;
	}
	previousAdded->vine = root;
}

PPM::Node* PPM::NodeTraverser::run()
{
	next = &(vine->child);
	while (*next != nullptr && (*next)->character != character)
		traverse();
	addNode();
	return *next;
}

void PPM::NodeTraverser::traverse()
{
	next = &((*next)->sibling);
}

void PPM::NodeTraverser::addNode()
{
	*next = new Node(character);
}

// CountingNodeTraverser

PPM::Node* PPM::CountingNodeTraverser::run()
{
	totalCount = vine->count - 1;
	count = 0;
	uniqueCount = 0;
	return NodeTraverser::run();
}

void PPM::CountingNodeTraverser::traverse()
{
	uniqueCount++;
	count += (*next)->count;
	NodeTraverser::traverse();
}

int  PPM::CountingNodeTraverser::calculateEscapeCount()
{
	int escapeCount = uniqueCount;

	if (escapeCount == 0)
		escapeCount++;

	// Escape count equals the total count divided by the number of different characters seen at the context.
	escapeCount = totalCount / escapeCount;

	if (escapeCount == 0)
		escapeCount++;
	return 1;
	//return escapeCount;
}

// EncodingTraverser

void PPM::EncodingTraverser::addNode()
{
	if (!contextFound)
	{
		int denom = totalCount + calculateEscapeCount();
		if (*next == nullptr)
		{
			// Encode escape.
			encode(denom, totalCount, denom, config::EscapeCharacter);

			// Check vine == null then encode -1.
			if (vine->vine == nullptr)
				encode((character + 1), character, config::TotalUniqueChars, character);
		}
		else
		{
			// Encode character.
			encode((count + (*next)->count), count, denom, character);
		}
	}
	NodeTraverser::addNode();
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


