#include "PPM.h"
#include <fstream>
#include <set>
#include <vector>

using namespace std;
using namespace compression;

void PPM::encode(const characterCode& charToEncode, ArithmeticEncoder& encoder)
{
	// First node to be added will be updated as base node.
	bool contextFound = false;
	Node* vinePtr = basePtr->vine;
	auto NodeAndRange = getNodeAndRange(vinePtr, charToEncode);
	Node* childNodePtr = NodeAndRange.first;
	basePtr = childNodePtr;
	if (childNodePtr->count > 1)
		contextFound = true;
	encoder.encode(NodeAndRange.second);

	// Reduce context size and add character as node. 
	while (vinePtr = vinePtr->vine)
	{
		NodeAndRange = getNodeAndRange(vinePtr, charToEncode);
		Node* childNodePtr2 = NodeAndRange.first;

		// Only encodes if the character has not been seen at higher order contexts.
		if (!contextFound)
			encoder.encode(NodeAndRange.second);

		if (childNodePtr2->count > 1)
			contextFound = true;
		childNodePtr->vine = childNodePtr2;
		childNodePtr = childNodePtr2;
	}
	childNodePtr->vine = rootPtr;

	// If character was not seen at any contexts the -1 orderTable is used.
	if (!contextFound)
	{
		ProbRange range;
		range.character = charToEncode;
		int cumCount = orderNegativeOneTableByChar.at(charToEncode);
		range.denom = orderNegativeOneTableByChar.size();
		range.lower = cumCount;
		range.upper = cumCount + 1;
		encoder.encode(range);
	}
}

pair<PPM::Node*, ProbRange> PPM::getNodeAndRange(PPM::Node* parentNode, const characterCode& charToEncode)
{
	ProbRange range;
	CountingNodeTraverser traverser(parentNode);
	Node* charNode = traverser.findNode(charToEncode);
	int lower = traverser.count;
	traverser.iterateToEnd();
	int escapeCount = traverser.getEscapeCount();

	if (charNode != nullptr)
	{
		range.lower = lower;
		range.upper = lower + charNode->count;
		range.character = charToEncode;
	}
	else
	{
		charNode = traverser.addNode(charToEncode);
		range.lower = traverser.count;
		range.upper = lower + escapeCount;
		range.character = config::EscapeCharacter;
	}

	range.denom = traverser.count + escapeCount;
	charNode->count++;
	pair<PPM::Node*, ProbRange> pair(charNode, range);
	return pair;
}

characterCode PPM::decode(ArithmeticDecoder& decoder)
{
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
		count = decoder.getCount(orderNegativeOneTableByCount.size());
		charRange.character = orderNegativeOneTableByCount[count];
		charRange.denom = orderNegativeOneTableByCount.size();
		charRange.lower = count;
		charRange.upper = count + 1;
	}

	decoder.decode(charRange);
	
	// Update all counts with decoded character.
	update(charRange.character);
	
	return charRange.character;
}

void PPM::update(const characterCode& charToUpdate)
{
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
	lastNodeAdded->vine = rootPtr;
}

PPM::PPM()
{
	for (characterCode i = 0; i <= config::EndCharacter; i++)
	{
		orderNegativeOneTableByChar.insert(pair<characterCode, int>(i, orderNegativeOneTableByChar.size()));
		orderNegativeOneTableByCount.insert(pair<int, characterCode>(orderNegativeOneTableByCount.size(), i));
	}

	Node* firstNode = new Node(config::EscapeCharacter);
	rootPtr = firstNode;
	for (int i = 0; i < config::MaxOrderSize + 1; i++)
	{
		Node* secondNode = new Node(config::EscapeCharacter);
		firstNode->child = secondNode;
		secondNode->vine = firstNode;
		firstNode = secondNode;
	}
	basePtr = firstNode;
}

PPM::Node* PPM::NodeTraverser::findNode(const characterCode& charToFind)
{
	Node* currentNode = *next;
	while (currentNode != nullptr && currentNode->character != charToFind)
		currentNode = traverse();
	return currentNode;
}

PPM::Node* PPM::NodeTraverser::traverse()
{
	next = &((*next)->sibling);
	return *next;
}

PPM::Node* PPM::CountingNodeTraverser::traverse()
{
	escapeCount++;
	count += (*next)->count;
	return NodeTraverser::traverse();
}

PPM::Node* PPM::NodeTraverser::addNode(const characterCode& character)
{
	*next = new Node(character);
	return *next;
}

void PPM::NodeTraverser::iterateToEnd()
{
	while (*next != nullptr)
		traverse();
}

int PPM::CountingNodeTraverser::getEscapeCount()
{
	if (escapeCount == 0)
		escapeCount++;

	// Escape count equals the total count divided by the number of different characters seen at the context.
	escapeCount = count / escapeCount;

	if (escapeCount == 0)
		escapeCount++;

	return escapeCount;
}

PPM::NodeTraverser::NodeTraverser(Node* parent)
{
	next = &parent->child;
}

PPM::CountingNodeTraverser::CountingNodeTraverser(Node* parent) : NodeTraverser(parent) {}

#ifdef _DEBUG
void PPM::outputDebug(ofstream& outputFileStream)
{

}
#endif

