#ifndef SERIALIZATION_H_INCLUDED
#define SERIALIZATION_H_INCLUDED
#include <jansson.h>
#include "renderer.h"

json_t* SerializeModel(Model* model);
Model* DeserializeModel(json_t* json);
#endif // SERIALIZATION_H_INCLUDED
