#ifndef SERIALIZATION_H_INCLUDED
#define SERIALIZATION_H_INCLUDED
#include <jansson.h>
#include "animation.h"
#include "model.h"
#include "project.h"

json_t* model_serialize(model_t* model);
model_t* model_deserialize(json_t* json);
json_t* animation_serialize(animation_t* animation,model_t** model_list,int num_models);
animation_t* animation_deserialize(json_t* json,model_t** model_list,int num_models);
json_t* project_serialize(project_t* project);
project_t* project_deserialize(json_t* json);
void project_save(const char* filename,project_t* project);
project_t* project_load(const char* filename);
#endif // SERIALIZATION_H_INCLUDED
