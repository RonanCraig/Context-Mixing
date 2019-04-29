#include "PPM.h"
#include <fstream>
#include <set>

using namespace std;
using namespace compression;
using namespace types;

// Update

void PPM::update(const characterType& charToUpdate)
{
	for (int i = 0; i < numberOfOrders; i++)
		orders[i].reset(charToUpdate);
	
	initialiseVine();
	int currentDepth = depthReached;

	base = updateNode(*currentNode, currentDepth, charToUpdate);
	Node* previousAdded = base;
	while (currentNode = currentNode->vine)
	{
		currentDepth--;
		previousAdded->vine = updateNode(*currentNode, currentDepth, charToUpdate);
		previousAdded = previousAdded->vine;
	}
	previousAdded->vine = &root;
	root.count++;
}

PPM::Node* PPM::updateNode(Node& currentNode, int currentDepth, types::characterType character)
{
	bool shouldCount = false;

	for (int i = 0; i < numberOfOrders; i++)
		if (!orders[i].finished)
			shouldCount = true;
	
	ChildrenIterator it(currentNode);
	Node* updatedNode = nullptr;

	while (it.node() && it.character() != character)
	{
		if (shouldCount)
		{
			for (int i = 0; i < numberOfOrders; i++)
				orders[i].update(*it.node(), currentDepth);
		}
		it.increment();
	}
	updatedNode = it.insertNode(character);

	if (shouldCount)
	{
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

void PPM::Order::update(const Node& node, const int order)
{
	if (order > this->order || finished)
		return;

	if (node.character == this->character)
	{
		counts.lower = counts.denom;
		counts.upper = counts.denom + node.count;
		found = true;
	}
		
	if (exclusions.add(node.character))
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
		}
		else
			finished = true;

		counts.denom += escapeCount;
		ranges[order + 1] = ProbRange(counts.upper, counts.lower, counts.denom, character);
		probability = double(probability / counts.denom) * (counts.upper - counts.lower);
	}
}

void PPM::Order::reset(types::characterType character)
{
	exclusions.resetExclusions();
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

void PPM::encode(ArithmeticEncoder& encoder, int order)
{
	ProbRange* ranges = this->orders[0].getRanges();

	bool finished = false;
	for (int i = depthReached; i > -1 && !finished; i--)
	{
		ProbRange& range = ranges[i];
		encoder.encode(range);
		if (range.character != escapeCharacter)
			finished = true;
	}
}

// Decode
/*
characterType PPM::decode(ArithmeticDecoder& decoder)
{
	// TODO: make it better: exclusion temp.
	byte temp = depthReached;
	initialiseVine();
	depthReached = temp;

	exclusions.resetExclusions();

	bool found = false;
	characterType charToUpdate;
	while (!found)
	{
		Exclusions exclusionsTemp = exclusions;
		SiblingTraverser traverser(currentNode, exclusions);
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

	return charToUpdate;
}
*/
/*
void PPM::SiblingTraverser::countToCount(Node** startNode, const countType& count)
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
}*/

