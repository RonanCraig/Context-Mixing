#pragma once
#include <vector>
#include "Model.h"
#include "PPM.h"

namespace compression
{

class ContextMixer
{
// Attributes
private:
	std::vector<Model*> models;
	PPM ppm;

// Methods
public:
	Model& getBestModel();
	void updateModels(const characterCode& charToUpdate);
	ContextMixer();
};

}

