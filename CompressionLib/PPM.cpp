#include "PPM.h"
#include <fstream>
#include <set>

using namespace std;
using namespace compression;
using namespace types;

// Update

void PPM::update(const characterType& charToUpdate)
{
	initialiseVine();
	int currentDepth = depthReached;
	exclusions.resetExclusions();

	setProbabilityRange((charToUpdate + 1), charToUpdate, Arithmetic::TOTAL_UNIQUE_CHARS, 0, charToUpdate);
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
	SiblingTraverser traverser(currentNode, exclusions, character);
	countType denom = traverser.totalCount + calculateEscapeCount(traverser.uniqueCount, traverser.totalCount);
	Node* updatedNode = (*traverser.iterator);

	if (traverser.found) // Encode character.
		setProbabilityRange(traverser.cumulativeCount + updatedNode->count, traverser.cumulativeCount, denom, currentDepth, character);
	else  // Encode escape.
		setProbabilityRange(denom, traverser.totalCount, denom, currentDepth, escapeCharacter);

	return updatedNode;
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

void PPM::setProbabilityRange(countType upper, countType lower, countType denom, int currentDepth, types::characterType character)
{
	rangesForOrders[currentDepth].second = character;
	ProbRange& range = rangesForOrders[currentDepth].first;
	range.lower = lower;
	range.upper = upper;
	range.denom = denom;
}

types::ProbRange PPM::getProbability(int order)
{
	if (order > depthReached)
		order = depthReached;

	bool found = (rangesForOrders[order].second != escapeCharacter);
	ProbRange range = rangesForOrders[order].first;

	for (int i = order - 1; i > -1 && !found; i--)
	{
		range.reduce(rangesForOrders[i].first);

		if (rangesForOrders[i].second != escapeCharacter)
			found = true;
			
	}

	return range;
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
// SiblingTraverser

void PPM::SiblingTraverser::insertNode()
{
	if (*iterator == nullptr)
		*iterator = new Node(character);

	(*iterator)->count++;

	if ((*iterator)->count == Arithmetic::MAX_COUNT)
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
}

void PPM::SiblingTraverser::traverse()
{
	while (*iterator != nullptr && (*iterator)->character != character)
		next();

	if (*iterator)
		found = true;

	cumulativeCount = totalCount;
	Node** nodeToBeUpdated = iterator;

	while (*iterator != nullptr)
		next();

	iterator = nodeToBeUpdated;
	insertNode();
}
/*
void PPM::SiblingTraverser::traverse()
{
	while (*iterator != nullptr)
		next();
}
*/
void PPM::SiblingTraverser::next()
{
	if (!exclusions.add((*iterator)->character))
	{
		totalCount += (*iterator)->count;
		uniqueCount++;
	}
	iterator = &(*iterator)->sibling;
}

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
}

