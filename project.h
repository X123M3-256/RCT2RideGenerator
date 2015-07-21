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
uint32_t sprites;
uint32_t spacing;
uint16_t friction;
uint8_t z_value;
}car_settings_t;

typedef struct
{
char* name;
char* description;
uint32_t id;
uint32_t flags;
uint8_t track_type;
uint8_t minimum_cars;
uint8_t maximum_cars;
uint8_t zero_cars;
uint8_t car_icon_index;
uint8_t excitement;
uint8_t intensity;
uint8_t nausea;
uint8_t max_height;
uint8_t car_types[5];
car_settings_t cars[4];
image_t* preview_image;
model_t** models;
int num_models;
}project_t;

project_t* project_new();
void project_add_model(project_t* project,model_t* model);
void project_set_preview(project_t* project,image_t* image);
object_t* project_export_dat(project_t* project);
int car_settings_count_required_animation_frames(car_settings_t* car);
void project_free(project_t* project);

#endif // PROJECT_H_INCLUDED
