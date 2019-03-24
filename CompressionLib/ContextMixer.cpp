#include "ContextMixer.h"

using namespace std;
using namespace compression;
using namespace types;

 Model* ContextMixer::getBestModel(characterType charToUpdate)
{
	 updateModels(charToUpdate);
	 return models[0];
}

 void ContextMixer::updateModels(const characterType charToUpdate)
 {
	models[0]->update(charToUpdate);
 }


