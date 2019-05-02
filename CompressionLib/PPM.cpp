#include "PPM.h"
#include <fstream>
#include <set>

using namespace std;
using namespace compression;
using namespace types;

// Update

void PPM::update(const characterType& charToUpdate)
{
	// Resets the exclusion list for each order and sets the next character to be encodedd.
	for (int i = 0; i < numberOfOrders; i++)
		orders[i].reset(charToUpdate);
	
	// The vine pointer is initialised.
	initialiseVine();
	int currentDepth = depthReached;

	ProbRange* rn1 = &(orders[0].getRanges())[0]; //order -1
	ProbRange* r0 = &(orders[0].getRanges())[1]; //order  0
	ProbRange* r1 = &(orders[0].getRanges())[2]; //order  1
	ProbRange* r2 = &(orders[0].getRanges())[3]; //order  2
	ProbRange* r3 = &(orders[0].getRanges())[3]; //order  3
	ProbRange* r4 = &(orders[0].getRanges())[3]; //order  4

	base = updateNode(*currentNode, currentDepth, charToUpdate);
	Node* previousAdded = base;
	// Traverses the Vine pointers till all contexts have been visited.
	while (currentNode = currentNode->vine)
	{
		currentDepth--;
		previousAdded->vine = updateNode(*currentNode, currentDepth, charToUpdate);
		previousAdded = previousAdded->vine;
	}
	previousAdded->vine = &root;

	// Root is not visited so the count must be updated now.
	root.count++;
}

PPM::Node* PPM::updateNode(Node& currentNode, int currentDepth, types::characterType character)
{
	bool shouldCount = false;

	// Checks if all the orders have found the character, if so we only need to update the counts not calculate sub ranges.
	for (int i = 0; i < numberOfOrders; i++)
		if (!orders[i].finished)
			shouldCount = true;
	
	ChildrenIterator it(currentNode);
	Node* updatedNode = nullptr;

	if (!shouldCount)
	{
		// Loops through the siblings till the end is reached or the character node is found.
		while (it.node() && it.character() != character)
			it.increment();

		// Updates the character node. Or inserts it if it didn't exist.
		updatedNode = it.insertNode(character);
	}
	else
	{
		// Loops through the siblings till the end is reached or the character node is found.
		while (it.node() && it.character() != character)
		{
			for (int i = 0; i < numberOfOrders; i++)
				orders[i].update(*it.node(), currentDepth);
			it.increment();
		}

		if (it.node())
			for (int i = 0; i < numberOfOrders; i++)
				orders[i].update(*it.node(), currentDepth);

		// Updates the character node. Or inserts it if it didn't exist.
		updatedNode = it.insertNode(character);

		// Counts to the end to get the total count.
		while (it.increment())
			for (int i = 0; i < numberOfOrders; i++)
				orders[i].update(*it.node(), currentDepth);
	}
	return updatedNode;
}

PPM::Node* PPM::ChildrenIterator::insertNode(types::characterType character)
{
	if (*iterator == nullptr)
		*iterator = new Node(character);

	node()->count++;

	if (node()->count == Arithmetic::MAX_COUNT)
	{
		Node* currentNode = parentNode.child;
		while (currentNode)
		{
			currentNode->count = currentNode->count / 2;
			if (currentNode->count == 0)
				currentNode->count++;
			currentNode = currentNode->sibling;
		}
	}

	return *iterator;
}

void PPM::Order::update(const Node& node, const int depth)
{
	// A character seen at an order n will be at depth n + 1 in the tree.
	if (depth > this->order + 1|| finished)
		return;

	if (node.character == this->character)
	{
		counts.lower = counts.denom;
		counts.upper = counts.denom + node.count;
		found = true;
	}
		
	if (!exclusions.add(node.character))
	{
		counts.denom += node.count;
		counts.uniqueCount++;
	}

	if (node.sibling == nullptr)
	{
		countType escapeCount = PPM::calculateEscapeCount(counts.uniqueCount, counts.denom);
		if (!found)
		{
			counts.lower = counts.denom;
			counts.upper = counts.denom + escapeCount;
			character = escapeCharacter;
		}
		else
			finished = true;

		counts.denom += escapeCount;
		ranges[depth] = ProbRange(counts.upper, counts.lower, counts.denom, character);
		probability = double(probability / counts.denom) * (counts.upper - counts.lower);
		counts.reset();
	}
}

types::ProbRange PPM::Order::getCharacter(Node& node, ArithmeticDecoder decoder)
{
	ChildrenIterator it(node);
	while (it.node())
	{
		if (!exclusions.add(it.node()->character))
		{
			counts.denom += it.node()->count;
			counts.uniqueCount++;
		}
		it.increment();
	}

	types::ProbRange range;
	countType escapeLower = counts.denom;
	range.denom = escapeLower + PPM::calculateEscapeCount(counts.uniqueCount, counts.denom);
	countType charCount = decoder.getCount(range.denom);

	if (charCount >= escapeLower)
	{
		range.character = escapeCharacter;
		range.upper = range.denom;
		range.lower = escapeLower;
	}
	else
	{
		exclusions.resetLastExclusions(counts.uniqueCount);
		counts.reset();
		ChildrenIterator it(node);
		bool found = false;
		while (!found)
		{
			if (!exclusions.add(it.node()->character))
			{
				if ((counts.denom + it.node()->count) > charCount)
				{
					found = true;
					range.character = it.node()->character;
					range.lower = counts.denom;
					range.upper = range.lower + it.node()->count;
				}
				else
					counts.denom += it.node()->count;
			}
			if(!found)
				it.increment();
		}
	}
	counts.reset();
	return range;
}

void PPM::Order::reset(types::characterType character)
{
	// TODO: make better.
	exclusions.resetExclusions();
	finished = false;
	found = false;
	probability = 1;
	this->character = character;
	this->ranges[0].denom = Arithmetic::TOTAL_UNIQUE_CHARS;
	this->ranges[0].upper = character + 1;
	this->ranges[0].lower = character;
	this->ranges[0].character = character;
}

void PPM::initialiseVine()
{
	if (depthReached < maxDepth)
	{
		currentNode = base;
		depthReached++;
	}
	else
		currentNode = base->vine;
}

countType PPM::calculateEscapeCount(const countType& uniqueCount, const countType& totalCount)
{
	countType escapeCount = totalCount / ((uniqueCount < 1) ? 1 : uniqueCount);
	return 1;
	return ((escapeCount < 1) ? 1 : escapeCount);
}

double PPM::getEstimatedProb(int order)
{
	return this->orders[0].getProbability();
}

void PPM::encode(ArithmeticEncoder& encoder)
{
	ProbRange* ranges = this->orders[0].getRanges();

	bool finished = false;
	for (int i = depthReached; i > -1 && !finished; i--)
	{
		ProbRange& range = ranges[i];
		encoder.encode(range);
		if (range.character != escapeCharacter)
			finished = true;
		range.denom = 1;
		range.lower = 0;
		range.upper = 1;
		range.character = escapeCharacter;
	}
}

// Decode
characterType PPM::decode(ArithmeticDecoder& decoder)
{
	byte temp = depthReached;
	initialiseVine();
	depthReached = temp;

	for (int i = 0; i < numberOfOrders; i++)
		orders[i].reset(' ');

	bool found = false;
	characterType charToUpdate;
	while (!found)
	{
		types::ProbRange range = orders[0].getCharacter(*currentNode, decoder);
		if (range.character != escapeCharacter)
		{
			found = true;
			charToUpdate = range.character;
		}
			
		decoder.decode(range);
		currentNode = currentNode->vine;

		if (currentNode == nullptr && !found)
		{
			range.denom = Arithmetic::TOTAL_UNIQUE_CHARS;
			range.lower = decoder.getCount(Arithmetic::TOTAL_UNIQUE_CHARS);
			range.upper = range.lower + 1;
			decoder.decode(range);
			found = true;
			charToUpdate = range.lower;
		}
	}

	return charToUpdate;
}

