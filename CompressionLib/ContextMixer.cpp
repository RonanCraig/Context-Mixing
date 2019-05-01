#include "ContextMixer.h"

using namespace std;
using namespace compression;
using namespace types;

 Model* ContextMixer::getBestModel()
{
	 Model* model = models[0];
	 return models[0];
}

 void ContextMixer::updateModels(const characterType charToUpdate)
 {
	models[0]->update(charToUpdate);
	//double prob = model->getEstimatedProb(4);
 }


