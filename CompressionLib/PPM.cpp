#include "PPM.h"
#include "Config.h"
#include <fstream>
#include <set>
#include <new>

using namespace std;
using namespace compression;
using namespace types;

PPM::PPM() : maxDepth(5), base(&root), currentNode(&root)
{
	// TODO: Make more dynamic.
	orders[0] = Order(4); 
	orders[1] = Order(4);
	orders[2] = Order(8);
}

PPM::~PPM()
{
	deleteTree(root.child);
}

void PPM::deleteTree(Node* node)
{
	if (!node)
		return;
	if (node->child)
		deleteTree(node->child);
	if (node->sibling)
		deleteTree(node->sibling);
	delete node;
}

void PPM::setBestOrder()
{
	for (int i = 0; i < numberOfOrders; i++)
		if (orders[i].bw > bestOrder->bw)
			bestOrder = &orders[i];
}

void PPM::updateWeights()
{
	double betaProb = 0;

	for (int i = 0; i < numberOfOrders; i++)
		betaProb += orders[i].bw * orders[i].probability;

	double sum = 0;

	for (int i = 0; i < numberOfOrders; i++)
	{
		orders[i].bw = orders[i].bw*(orders[i].probability / betaProb);

		if (orders[i].bw < 0.01)
			orders[i].bw = 0.01;

		if (orders[i].bw > 0.99)
			orders[i].bw = 0.99;

		sum += orders[i].bw;
	}

	for (int i = 0; i < numberOfOrders; i++)
		orders[i].bw = orders[i].bw / sum;
}

// Update

bool PPM::update(const characterType& charToUpdate)
{
	setBestOrder();
	// Resets the exclusion list for each order and sets the next character to be encodedd.
	for (int i = 0; i < numberOfOrders; i++)
		orders[i].reset(charToUpdate);
	
	// The vine pointer is initialised.
	initialiseVine();
	int currentDepth = depthReached;

	try
	{
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

		for (int i = 0; i < numberOfOrders; i++)
			if (!orders[i].finished)
				orders[i].updateNegativeOrder();

		updateWeights();
	}
	catch (bad_alloc& badAlloc)
	{
		return true;
	}

	static int COUNT = 0;
	
	if (COUNT == config::countToReset)
	{
		COUNT = 0;
		return true;
	}
	else
		COUNT++;
		

	return false;
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
		ProbRange range;
		countType escapeCount = PPM::calculateEscapeCount(counts.uniqueCount, counts.denom);
		range.denom = counts.denom + escapeCount;

		if (!found)
		{
			// Escape will be encoded.
			range.lower = counts.denom;
			range.upper = range.lower + escapeCount;
			range.character = escapeCharacter;
		}
		else
		{
			// Found character will be encoded.
			finished = true;
			range.lower = counts.lower;
			range.upper = counts.upper;
			range.character = this->character;
		}

		ranges[depth] = range;
		probability = double(probability / range.denom) * (range.upper - range.lower);
		counts.reset();
	}
}

void PPM::Order::updateNegativeOrder()
{
	/*
	this->ranges[0].character = character;
	int denom = Arithmetic::TOTAL_UNIQUE_CHARS;
	int upper = character + 1;
	int lower = character;
	
	Exclusions::Node* start = exclusions.start;
	while (start != nullptr)
	{
		if (exclusions.exclusions[start->character])
		{
			denom--;
			if (start->character < character)
			{
				upper--;
				lower--;
			}
		}
		start = start->next;
	}
	
	probability = double(probability / denom) * (upper - lower);

	this->ranges[0].denom = denom;
	this->ranges[0].upper = upper;
	this->ranges[0].lower = lower;
	*/
	int denom = Arithmetic::TOTAL_UNIQUE_CHARS;
	int upper = character + 1;
	int lower = character;

	this->ranges[0].character = character;
	this->ranges[0].denom = denom;
	this->ranges[0].upper = upper;
	this->ranges[0].lower = lower;

	probability = double(probability / denom) * (upper - lower);
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

	for (int i = 0; i < this->order + 2; i++)
	{
		ranges[i].denom = 1;
		ranges[i].lower = 0;
		ranges[i].upper = 1;
		ranges[i].character = escapeCharacter;
	}
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

void PPM::encode(ArithmeticEncoder& encoder)
{
	vector<types::ProbRange>& ranges = bestOrder->getRanges();

	int maxRangeOrderPos = (depthReached <= bestOrder->order + 1) ? depthReached : bestOrder->order + 1; // Trie depth might not of reached the order depth yet. When it has just begun.

	bool finished = false;
	for (int i = maxRangeOrderPos; i > -1 && !finished; i--)
	{
		ProbRange& range = ranges[i];
		encoder.encode(range);
		if (range.character != escapeCharacter)
			finished = true;
	}
}

// Decode
characterType PPM::decode(ArithmeticDecoder& decoder)
{
	byte temp = depthReached;
	initialiseVine();
	int currentDepth = depthReached;
	depthReached = temp;

	setBestOrder();
	
	while (currentDepth > bestOrder->order + 1)
	{
		currentNode = currentNode->vine;
		currentDepth--;
	}

	for (int i = 0; i < numberOfOrders; i++)
		orders[i].reset(' ');

	bool found = false;
	characterType charToUpdate;
	while (!found)
	{
		types::ProbRange range = bestOrder->getCharacter(*currentNode, decoder);
		if (range.character != escapeCharacter)
		{
			found = true;
			charToUpdate = range.character;
		}
			
		decoder.decode(range);
		currentNode = currentNode->vine;

		if (currentNode == nullptr && !found)
		{
			/*
			range.denom = Arithmetic::TOTAL_UNIQUE_CHARS;
			Order::Exclusions::Node* start = order.exclusions.start;
			while (start != nullptr)
			{
				if (order.exclusions.exclusions[start->character])
					range.denom--;
				start = start->next;
			}
			Order::Exclusions::Node* start = order.exclusions.start;
			while (start != nullptr)
			{
				if (order.exclusions.exclusions[start->character])
					range.denom--;
				start = start->next;
			}
			
			range.lower = decoder.getCount(range.denom);
			range.upper = range.lower + 1;
			decoder.decode(range);
			found = true;
			*/
			range.denom = Arithmetic::TOTAL_UNIQUE_CHARS;
			range.lower = decoder.getCount(range.denom);
			range.upper = range.lower + 1;
			decoder.decode(range);
			found = true;
			charToUpdate = range.lower;
		}
	}

	return charToUpdate;
}

