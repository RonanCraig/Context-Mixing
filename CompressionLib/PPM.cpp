#include "PPM.h"
#include <fstream>
#include <set>
#include <vector>

using namespace std;
using namespace compression;

void PPM::encode(const characterCode& charToEncode, ArithmeticEncoder& encoder)
{
	EncodingTraverser traverser(encoder);
	traverser.traverse(charToEncode, &base, root);
}

characterCode PPM::decode(ArithmeticDecoder& decoder)
{
	DecodingTraverser traverser(decoder);
	return traverser.traverse(&base, root);
}

void PPM::update(const characterCode& charToUpdate)
{
	NodeTraverser traverser;
	traverser.traverse(charToUpdate, &base, root);
}

PPM::PPM()
{
	Node* firstNode = new Node(config::EscapeCharacter);
	root = firstNode;
	for (int i = 0; i < config::MaxOrderSize + 1; i++)
	{
		Node* secondNode = new Node(config::EscapeCharacter, 0);
		firstNode->child = secondNode;
		secondNode->vine = firstNode;
		firstNode = secondNode;
	}
	base = firstNode;
}

// NodeTraverser

void PPM::NodeTraverser::traverse(const characterCode& character, Node** base, Node* root)
{
	this->character = character;
	vine = (*base)->vine;
	(*base) = traverseChildren();
	Node* previousAdded = (*base);
	while (vine = vine->vine)
	{
		previousAdded->vine = traverseChildren();
		previousAdded = previousAdded->vine;
	}
	previousAdded->vine = root;
	root->count++;
}

PPM::Node* PPM::NodeTraverser::traverseChildren()
{
	next = &(vine->child);
	while (*next != nullptr && (*next)->character != character)
		nextNode();
	addNode();
	return *next;
}

void PPM::NodeTraverser::nextNode()
{
	next = &((*next)->sibling);
}

void PPM::NodeTraverser::addNode()
{
	if (*next == nullptr)
		*next = new Node(character);
	else
		(*next)->count++;
}

// CountingNodeTraverser

void PPM::CountingNodeTraverser::calculateTotalAndEscapeCounts()
{
	Node* tempNext = *(next);
	while (tempNext != nullptr)
	{
		uniqueCount++;
		totalCount = tempNext->count;
		tempNext = tempNext->sibling;
	}
		
	escapeCount = uniqueCount;

	if (escapeCount == 0)
		escapeCount++;

	// Escape count equals the total count divided by the number of different characters seen at the context.
	escapeCount = totalCount / escapeCount;

	if (escapeCount == 0)
		escapeCount++;
}

// EncodingTraverser

void PPM::EncodingTraverser::addNode()
{
	if (!contextFound)
	{
		calculateTotalAndEscapeCounts();
		int denom = totalCount + escapeCount;
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
			contextFound = true;
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

PPM::Node* PPM::EncodingTraverser::traverseChildren()
{
	count = 0;
	uniqueCount = 0;
	return NodeTraverser::traverseChildren();
}

void PPM::EncodingTraverser::nextNode()
{
	uniqueCount++;
	count += (*next)->count;
	NodeTraverser::nextNode();
}

// DecodingTraverser

characterCode PPM::DecodingTraverser::traverse(PPM::Node** base, PPM::Node* root)
{
	vine = (*base);
	bool found = false;
	characterCode charToUpdate;
	while ((vine = vine->vine) && !found)
	{
		calculateTotalAndEscapeCounts();
		int denom = totalCount + escapeCount;
		int charCount = decoder.getCount(denom);
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
			findCharacterForCount(charCount);
			charToUpdate = (*next)->character;
			decode(charCount + (*next)->count, charCount, denom, charToUpdate);
		}
	}
	NodeTraverser::traverse(charToUpdate, base, root);
	return charToUpdate;
}

void PPM::DecodingTraverser::findCharacterForCount(int& charCount)
{
	next = &(vine->child);
	if ((*next)->count == 0)
		nextNode();

	int count = (*next)->count;
	while (count < charCount)
	{
		nextNode();
		count += (*next)->count;
	}
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


