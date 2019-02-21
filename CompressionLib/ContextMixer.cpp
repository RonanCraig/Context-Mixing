#include "ContextMixer.h"

using namespace std;
using namespace compression;

 Model& ContextMixer::getBestModel()
{
	 return ppm;
}

 void ContextMixer::updateModels(const characterCode& charToUpdate)
 {
	 for (Model* m : models)
	 {
		 //m->update(charToUpdate);
	 }
 }

 ContextMixer::ContextMixer()
 {
	 models.push_back(&ppm);
 }

#ifdef _DEBUG
 void ContextMixer::outputDebug(ofstream& outputFileStream)
 {
	 for (Model* m : models)
	 {
		 m->outputDebug(outputFileStream);
	 }
 }
#endif // _DEBUG
