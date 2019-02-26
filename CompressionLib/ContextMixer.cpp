#include "ContextMixer.h"

using namespace std;
using namespace compression;

 Model& ContextMixer::getBestModel()
{
	 return ppm;
}

 void ContextMixer::updateModels(const characterCode& charToUpdate)
 {

 }

 ContextMixer::ContextMixer()
 {
	 models.push_back(&ppm);
 }

