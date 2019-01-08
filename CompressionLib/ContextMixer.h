#pragma once
#include "Model.h"
#include "PPM.h"

namespace compression
{

class ContextMixer
{
private:
	PPM ppm;
public:
	Model& getBestModel();
};

}

