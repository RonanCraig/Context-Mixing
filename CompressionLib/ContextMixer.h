#pragma once
#include <vector>
#include "Model.h"
#include "CompressionTypes.h"

namespace compression
{

class ContextMixer
{
// Attributes
private:
	std::vector<Model*>& models;

// Methods
public:
	Model* getBestModel();
	ContextMixer(std::vector<Model*>& models) : models(models) {}
	void updateModels(const types::characterType charToUpdate);
};

}

