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
	Model** models;

// Methods
public:
	Model& getBestModel();
	ContextMixer() { models = new Model*[1]; models[0] = new PPM(); }
	bool updateModels(const types::characterType charToUpdate);
	void resetModels();
};

}

