#include "ContextMixer.h"

using namespace std;
using namespace compression;
using namespace types;

 Model& ContextMixer::getBestModel()
{
	 return *(models[0]);
}

 bool ContextMixer::updateModels(const characterType charToUpdate)
 {
	return models[0]->update(charToUpdate);
 }

 void ContextMixer::resetModels()
 {
	 delete models[0];
	 models[0] = new PPM(ordersToRun);
 }


