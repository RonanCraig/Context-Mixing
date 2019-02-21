#pragma once
#include <vector>
#include "Model.h"
#include "PPM.h"

namespace compression
{

class ContextMixer
{
private:
	std::vector<Model*> models;
	PPM ppm;
public:
	Model& getBestModel();
	void updateModels(const characterCode& charToUpdate);
#ifdef _DEBUG
	void outputDebug(std::ofstream& outputFileStream);
#endif // _DEBUG
	ContextMixer();
};

}

