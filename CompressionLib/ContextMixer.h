#pragma once
#include <vector>
#include "Model.h"
#include "CompressionTypes.h"
#include "PPM.h"

namespace compression
{

class ContextMixer
{
// Attributes
private:
	std::vector<int> ordersToRun;
	Model** models;

// Methods
public:
	Model& getBestModel();
	ContextMixer(std::vector<int> ordersToRun) : ordersToRun(ordersToRun) { models = new Model*[1]; models[0] = new PPM(ordersToRun); }
	bool updateModels(const types::characterType charToUpdate);
	void resetModels();
	~ContextMixer() { delete models[0]; delete[] models; }
};

}

