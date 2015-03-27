#ifndef PROJECT_H_INCLUDED
#define PROJECT_H_INCLUDED
#include <stdint.h>
#include "dat.h"
#include "model.h"
#include "renderer.h"
#include "animation.h"

typedef struct
{
animation_t* animation;
uint32_t flags;
uint16_t friction;
uint16_t spacing;
uint8_t z_value;
}car_settings_t;

typedef struct
{
uint32_t flags;
uint8_t track_type;
uint8_t minimum_cars;
uint8_t maximum_cars;
uint8_t zero_cars;
uint8_t car_types[5];
car_settings_t cars[4];
model_t** models;
int num_models;
}project_t;

project_t* project_new();
void project_add_model(project_t* project,model_t* model);
object_t* project_export_dat(project_t* project);
void project_free(project_t* project);

#endif // PROJECT_H_INCLUDED
