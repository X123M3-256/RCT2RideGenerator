#ifndef PROJECT_H_INCLUDED
#define PROJECT_H_INCLUDED
#include "animation.h"
#include "dat.h"
#include "model.h"
#include "renderer.h"
#include <stdint.h>

#define MAX_COLOR_SCHEMES 16

typedef struct { // moving the car_t properties here for better WYSIWYG (do-able since the flags has been straightened out better)
    animation_t* animation;
    uint32_t flags;
    uint32_t sprites;
	uint32_t render_sprites;
    uint32_t spacing;
    uint16_t friction;
    uint8_t running_sound;
    uint8_t secondary_sound;
	uint8_t animation_type;
	uint8_t car_visual;// set to 0 by default
	uint8_t effect_visual;// set to 1 by default
    uint8_t z_value;
	uint8_t spin_inertia;
	uint8_t spin_friction;
	uint8_t powered_acceleration;
	uint8_t powered_velocity;
	uint8_t logflume_reverser_vehicle;
	uint8_t double_sound_frequency;
	uint8_t override_vertical_frames;
	int8_t vehicle_tab_vertical_offset;

	uint8_t sprite_width;
	uint8_t sprite_height_negative;
	uint8_t sprite_height_positive;

	uint8_t highest_rotation_index; // not for editing by mere mortals
	uint8_t rider_pairs;
	uint8_t riders;
	uint8_t rider_sprites;
	uint16_t unknown[9];
} car_settings_t;

typedef struct {
    char* name;
    char* description;
    color_scheme_t color_schemes[16];
	int color_scheme_override; // set to 255 to indicate the ride has different colors for each train
	uint32_t id;
    uint32_t num_color_schemes;
	uint64_t track_sections;
    uint32_t flags;
    uint8_t car_icon_index;
	uint8_t zero_cars;
	uint8_t track_type;
	uint8_t excitement;
	uint8_t intensity;
	uint8_t nausea;
	uint8_t max_height;
	uint8_t ride_categories[2];
	uint8_t car_types[5];
	uint8_t minimum_cars;
	uint8_t maximum_cars;
    car_settings_t cars[4];
    image_t* preview_image;
    model_t** models;
    int num_models;
} project_t;

project_t* project_new();
void project_add_model(project_t* project, model_t* model);
void project_set_preview(project_t* project, image_t* image);
object_t* project_export_dat(project_t* project);
void project_free(project_t* project);

#endif // PROJECT_H_INCLUDED
