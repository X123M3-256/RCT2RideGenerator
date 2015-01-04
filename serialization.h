#ifndef SERIALIZATION_H_INCLUDED
#define SERIALIZATION_H_INCLUDED
#include <jansson.h>
#include "animation.h"
#include "model.h"

json_t* SerializeModel(Model* model);
Model* DeserializeModel(json_t* json);
json_t* SerializeAnimation(Animation* animation);
Animation* DeserializeAnimation(json_t* json);
void DeserializeFile(const char* filename);
#endif // SERIALIZATION_H_INCLUDED
