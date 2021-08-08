#include "project.h"
#include "dat.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

project_t* project_new()
{
    int i;
    project_t* project = malloc(sizeof(project_t));
    project->name = malloc(13);
    project->description = malloc(15);
    strcpy(project->name, "Unnamed ride");
    strcpy(project->description, "No description");
	project->ride_categories[0]=CATEGORY_ROLLERCOASTER;
	project->ride_categories[1]=CATEGORY_NONE;
    project->num_color_schemes = 0;
    for (i = 0; i < MAX_COLOR_SCHEMES; i++) {
        project->color_schemes[i].colors[0] = 0;
        project->color_schemes[i].colors[1] = 0;
        project->color_schemes[i].colors[2] = 0;
    }
    project->track_type = 0x33; // Default to B&M track
    project->flags = RIDE_SEPARATE_RIDE_DEPRECATED | RIDE_SEPARATE_RIDE;
    project->minimum_cars = 3;
    project->maximum_cars = 8;
    project->zero_cars = 0;
    project->car_icon_index = 0;
    project->excitement = 0;
    project->intensity = 0;
    project->nausea = 0;
    project->max_height = 0;
    memset(project->car_types, 0xFF, 5);
    project->car_types[CAR_INDEX_DEFAULT] = 0;
    project->models = NULL;
    project->num_models = 0;
    project->preview_image = image_new(112, 112, 0);
    project->id = rand();
	project->track_sections = 0xFFFFFFFFFFFFFFFFl;

    for (i = 0; i < NUM_CARS; i++) {
        project->cars[i].animation = animation_new();
        project->cars[i].flags = 0;
        project->cars[i].sprites = SPRITE_GENTLE_SLOPE | SPRITE_STEEP_SLOPE | SPRITE_DIAGONAL_SLOPE;
        project->cars[i].spacing = 250000;
        project->cars[i].running_sound = RUNNING_SOUND_NONE;
        project->cars[i].secondary_sound = SECONDARY_SOUND_NONE;
        project->cars[i].z_value = 5;
        project->cars[i].friction = 0x2A8;
		project->cars[i].car_visual = 0;

		// stuff related to the .DAT
		project->cars[i].effect_visual = 1;
		project->cars[i].rider_pairs = 0;
		project->cars[i].riders = 0;
		project->cars[i].rider_sprites = 0;
		project->cars[i].spin_inertia = 0;
		project->cars[i].spin_friction = 0;
		project->cars[i].powered_acceleration = 0;
		project->cars[i].powered_velocity = 0;
		project->cars[i].logflume_reverser_vehicle = 0;
		project->cars[i].double_sound_frequency = 0;
        project->cars[i].override_vertical_frames = 0;

        project->cars[i].sprite_width = 0;
        project->cars[i].sprite_height_negative = 0;
        project->cars[i].sprite_height_positive = 0;

		memset(project->cars[i].unknown, 0, 9);
    }
    return project;
}
void project_add_model(project_t* project, model_t* model)
{
    project->models = realloc(project->models, (project->num_models + 1) * sizeof(model_t*));
    project->models[project->num_models++] = model;
}
void project_set_preview(project_t* project, image_t* image)
{
    image_free(project->preview_image);
    project->preview_image = image;
}

void project_free(project_t* project)
{
    int i;
    for (i = 0; i < project->num_models; i++)
        model_free(project->models[i]);
    for (i = 0; i < NUM_CARS; i++)
        animation_free(project->cars[i].animation);
    free(project->models);
    free(project->name);
    free(project->description);
    free(project);
}


int count_sprites_per_view(uint32_t flags, uint8_t animation_type)
{
    int sprites_per_view = 1;
    if (flags & CAR_IS_SWINGING) {
        /*
        Swinging flags karnaugh map
        FLAG_21 | FLAG_25 | FLAG_27 | number of sprites
        -         -         -        5

        -         1         -        3 <-- wood wild mouse

        1         1         1       13
        1         -         1       13 <-- bobsled

        -         1         1        7
        1         -         -        7 <-- arrow suspended
        1         1         -        7
        -         -         1        7 <-- dinghy slide
        */
        int f21 = flags & CAR_FLAG_21;
        int f25 = flags & CAR_FLAG_25;
        int f27 = flags & CAR_FLAG_27;

        if (!(f21) && !(f27)) {
            if (f25) {
                sprites_per_view = 3;
            }
            else {
                sprites_per_view = 5;
            }
        }
        else if (!(f21) || !(f27)) {
            sprites_per_view = 7;
        }
        else {
            sprites_per_view = 13;
        }

    }
    if (flags & CAR_IS_ANIMATED) {
        switch (animation_type)
        {
        case CAR_ANIMATION_NONE: return 1;
        case CAR_ANIMATION_LOCOMOTIVE: return 4;
        case CAR_ANIMATION_SWAN: return 2; // SORT-OF
        case CAR_ANIMATION_CANOES: return 6;
        case CAR_ANIMATION_ROW_BOATS: return 7;
        case CAR_ANIMATION_WATER_TRICYCLES: return 2;
        case CAR_ANIMATION_OBSERVATION: return 8;
        case CAR_ANIMATION_HELICARS: return 4;
        case CAR_ANIMATION_MONORAIL_CYCLE: return 4;
        case CAR_ANIMATION_4D: return 8;
        }
    }
    if (flags & CAR_IS_SPINNING) { sprites_per_view = 16; }
    if (flags & CAR_EXTRA_SPINNING_FRAMES) {sprites_per_view = 32; } // assumes car is spinning
    return sprites_per_view;
}


void render_rotation(image_list_t* image_list,
    animation_t* animation,
    uint32_t flags,
    uint8_t animation_type,
    int base_frame,
    int sprites_per_image,
    int images,
    int num_frames,
    double pitch,
    double roll,
    double yaw)
{
    Matrix transform_matrix = MatrixFromEulerAngles(VectorFromComponents(-pitch, -yaw, -roll));
    float variables[ANIMATION_NUM_VARIABLES] = { 0, 0, 0, 0, 0, 0, 0, 0 };
    variables[VAR_PITCH] = -pitch;
    variables[VAR_YAW] = -yaw;
    variables[VAR_ROLL] = -roll;

    int sprites_per_view = count_sprites_per_view(flags,animation_type);

    double rotation = 0;
    double step = 2 * 3.141592654 / num_frames;
    double swingstep = (sprites_per_view > 1) ? 3.141592654 / (sprites_per_view - 1) : 0;
    for (int view = 0; view < num_frames; view++) {
        Matrix rotation_matrix = MatrixIdentity();
        rotation_matrix.Data[0] = cos(rotation);
        rotation_matrix.Data[2] = -sin(rotation);
        rotation_matrix.Data[8] = sin(rotation);
        rotation_matrix.Data[10] = cos(rotation);

        variables[VAR_ANIMATION] = 0.0;
        variables[VAR_SWING] = 0.0;

        for (int frame = 0; frame < sprites_per_view; frame++) { //this assumes swinging and animation is mutually exclusive
            renderer_clear_buffers();
            render_data_t render_data = animation_split_render_begin(animation, MatrixMultiply(rotation_matrix, transform_matrix), variables);
            for (int image = 0; image < images; image++) {
                image_list_set_image(image_list, base_frame + image * sprites_per_image + view * sprites_per_view + frame, renderer_get_image());
                animation_split_render_next_image(animation, &render_data);
            }
        if(flags & CAR_IS_SWINGING)variables[VAR_SWING]+= (frame%2==0)?swingstep*(frame):-swingstep*(frame);
        if(flags & CAR_IS_ANIMATED)variables[VAR_ANIMATION]+=1.0/sprites_per_view;
        if(flags & CAR_IS_SPINNING)variables[VAR_SPIN]+=3.141592654*2/sprites_per_view;
        }

        rotation += step;
        variables[VAR_YAW] += step;
    }
}
void render_loading(image_list_t* image_list,
    animation_t* animation,
    uint32_t flags,
    uint8_t animation_type,
    int base_frame,
    int sprites_per_image,
    int images)
{
    Matrix rotation_matrix = MatrixIdentity();
    Matrix yaw_matrix = MatrixIdentity();
    yaw_matrix.Data[0] = 0;
    yaw_matrix.Data[2] = -1;
    yaw_matrix.Data[8] = 1;
    yaw_matrix.Data[10] = 0;

    float variables[ANIMATION_NUM_VARIABLES] = { 0, 0, 0, 0, 0, 0, 0, 0 };

    for (int anim_frame = 0; anim_frame < 3; anim_frame++) {
        variables[VAR_RESTRAINT] += 0.25;
        variables[VAR_YAW] = 0;
		printf("var_restraint is %i\n", variables[VAR_RESTRAINT]);
        for (int i = 0; i < 4; i++) {
            renderer_clear_buffers();
            render_data_t render_data = animation_split_render_begin(animation, rotation_matrix, variables);
            for (int j = 0; j < images; j++) {
                image_list_set_image(image_list, base_frame + j * sprites_per_image + i,
                    renderer_get_image());
                animation_split_render_next_image(animation, &render_data);
            }
            rotation_matrix = MatrixMultiply(rotation_matrix, yaw_matrix);
            variables[VAR_YAW] += M_PI_2;
        }
        base_frame += 4;
    }
}


int count_sprites_from_flags(uint16_t sprites, uint32_t flags)
{
    int count = 0;
    if (sprites & SPRITE_FLAT_SLOPE)
        count += 32;
    if (sprites & SPRITE_GENTLE_SLOPE){
        if (flags & CAR_IS_SPINNING) {
            count += 16;
        }
        else {
            count += 72;
        }
    }
    if (sprites & SPRITE_RESTRAINT_ANIMATION)
        count += 12;
    if (flags & CAR_IS_SPINNING) {
        return count;
    }
    if (sprites & SPRITE_STEEP_SLOPE)
        count += 80;
    if (sprites & SPRITE_VERTICAL_SLOPE)
        count += 116; // These also include loop sprites by default
    if (sprites & SPRITE_DIAGONAL_SLOPE)
        count += 24;
    if (sprites & SPRITE_BANKING)
        count += 80;
    if (sprites & SPRITE_INLINE_TWIST)
        count += 40;
    if (sprites & SPRITE_SLOPE_BANK_TRANSITION)
        count += 128;
    if (sprites & SPRITE_DIAGONAL_BANK_TRANSITION)
        count += 16;
    if (sprites & SPRITE_SLOPED_BANK_TRANSITION)
        count += 16;
    if (sprites & SPRITE_SLOPED_BANKED_TURN)
        count += 128;
    if (sprites & SPRITE_BANKED_SLOPE_TRANSITION)
        count += 16;
    if (sprites & SPRITE_CORKSCREW)
        count += 80;
    return count;
}

static void project_render_sprites(project_t* project, object_t* object)
{
    int i;
    ride_header_t* header = object->ride_header;

    // Render preview images
    image_list_set_num_images(object->images, 3);
    image_list_set_image(object->images, 0, image_copy(project->preview_image));
    image_list_set_image(object->images, 1, image_new(1, 1, 0));
    image_list_set_image(object->images, 2, image_new(1, 1, 0));

    // Compute number of sprites
    for (i = 0; i < NUM_CARS; i++) {
        // Set flags
        uint32_t car_flags = project->cars[i].flags;
        printf("flags %i spinning %i\n", car_flags, car_flags & CAR_IS_SPINNING);
        uint8_t animation_type = project->cars[i].animation_type;
        uint16_t sprite_flags = header->cars[i].sprites;
        // Number of images needed for this car (car + riders)
        int car_num_riders = animation_count_riders(project->cars[i].animation);
        int car_images = (car_num_riders != 1 ? car_num_riders / 2 : 1) + 1;
        // Number of angles that need to be rendered for each image
        int views_per_image = count_sprites_from_flags(sprite_flags, car_flags);
        // Number of sprites that must be rendered for each angle of an image
        int sprites_per_view = count_sprites_per_view(car_flags, project->cars[i].animation_type);
        // Total sprites for each image
        int sprites_per_image = sprites_per_view * views_per_image;
        // Total sprites related to this car
        int total_car_sprites = sprites_per_image * car_images;
        // Compute first frame of this car's sprites
        int base_frame = object->images->num_images;
        // Allocate images for car
        image_list_set_num_images(object->images, base_frame + total_car_sprites);

        // Render images for car
        image_list_t* images = object->images;
        animation_t* animation = project->cars[i].animation;

        if (sprite_flags & SPRITE_FLAT_SLOPE) {
            render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                car_images, 32, FLAT, 0, 0);
            base_frame += 32 * sprites_per_view;
        }
        if (sprite_flags & SPRITE_GENTLE_SLOPE) {
            render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                car_images, 4, FG_TRANSITION, 0, 0);
            base_frame += 4 * sprites_per_view;
            render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                car_images, 4, -FG_TRANSITION, 0, 0);
            base_frame += 4 * sprites_per_view;

            if (car_flags & CAR_IS_SPINNING) {
                render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                    car_images, 4, GENTLE, 0, 0);
                base_frame += 4 * sprites_per_view;
                render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                    car_images, 4, -GENTLE, 0, 0);
                base_frame += 4 * sprites_per_view;
            }
            else {
                render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                    car_images, 32, GENTLE, 0, 0);
                base_frame += 32 * sprites_per_view;
                render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                    car_images, 32, -GENTLE, 0, 0);
                base_frame += 32 * sprites_per_view;
            }
        }
        if (sprite_flags & SPRITE_STEEP_SLOPE) {
            render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                car_images, 8, GS_TRANSITION, 0, 0);
            base_frame += 8 * sprites_per_view;
            render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                car_images, 8, -GS_TRANSITION, 0, 0);
            base_frame += 8 * sprites_per_view;
            render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                car_images, 32, STEEP, 0, 0);
            base_frame += 32 * sprites_per_view;
            render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                car_images, 32, -STEEP, 0, 0);
            base_frame += 32 * sprites_per_view;
        }
        if (sprite_flags & SPRITE_VERTICAL_SLOPE) {
            render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                car_images, 4, SV_TRANSITION, 0, 0);
            base_frame += 4 * sprites_per_view;
            render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                car_images, 4, -SV_TRANSITION, 0, 0);
            base_frame += 4 * sprites_per_view;
            render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                car_images, 32, VERTICAL, 0, 0);
            base_frame += 32 * sprites_per_view;
            render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                car_images, 32, -VERTICAL, 0, 0);
            base_frame += 32 * sprites_per_view;
            // Loop sprites
            render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                car_images, 4, VERTICAL + M_PI_12, 0, 0);
            base_frame += 4 * sprites_per_view;
            render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                car_images, 4, -VERTICAL - M_PI_12, 0, 0);
            base_frame += 4 * sprites_per_view;
            render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                car_images, 4, VERTICAL + 2 * M_PI_12, 0, 0);
            base_frame += 4 * sprites_per_view;
            render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                car_images, 4, -VERTICAL - 2 * M_PI_12, 0, 0);
            base_frame += 4 * sprites_per_view;
            render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                car_images, 4, VERTICAL + 3 * M_PI_12, 0, 0);
            base_frame += 4 * sprites_per_view;
            render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                car_images, 4, -VERTICAL - 3 * M_PI_12, 0, 0);
            base_frame += 4 * sprites_per_view;
            render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                car_images, 4, VERTICAL + 4 * M_PI_12, 0, 0);
            base_frame += 4 * sprites_per_view;
            render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                car_images, 4, -VERTICAL - 4 * M_PI_12, 0, 0);
            base_frame += 4 * sprites_per_view;
            render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                car_images, 4, VERTICAL + 5 * M_PI_12, 0, 0);
            base_frame += 4 * sprites_per_view;
            render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                car_images, 4, -VERTICAL - 5 * M_PI_12, 0, 0);
            base_frame += 4 * sprites_per_view;
            render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                car_images, 4, M_PI, 0, 0);
            base_frame += 4 * sprites_per_view;
        }
        if (sprite_flags & SPRITE_DIAGONAL_SLOPE) {
            render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                car_images, 4, FG_TRANSITION_DIAGONAL, 0, M_PI_4);
            base_frame += 4 * sprites_per_view;
            render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                car_images, 4, -FG_TRANSITION_DIAGONAL, 0, M_PI_4);
            base_frame += 4 * sprites_per_view;
            render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                car_images, 4, GENTLE_DIAGONAL, 0, M_PI_4);
            base_frame += 4 * sprites_per_view;
            render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                car_images, 4, -GENTLE_DIAGONAL, 0, M_PI_4);
            base_frame += 4 * sprites_per_view;
            render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                car_images, 4, STEEP_DIAGONAL, 0, M_PI_4);
            base_frame += 4 * sprites_per_view;
            render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                car_images, 4, -STEEP_DIAGONAL, 0, M_PI_4);
            base_frame += 4 * sprites_per_view;
        }
        if (sprite_flags & SPRITE_BANKING) {
            render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                car_images, 8, FLAT, BANK_TRANSITION, 0);
            base_frame += 8 * sprites_per_view;
            render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                car_images, 8, FLAT, -BANK_TRANSITION, 0);
            base_frame += 8 * sprites_per_view;
            render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                car_images, 32, FLAT, BANK, 0);
            base_frame += 32 * sprites_per_view;
            render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                car_images, 32, FLAT, -BANK, 0);
            base_frame += 32 * sprites_per_view;
        }
        if (sprite_flags & SPRITE_INLINE_TWIST) {
            render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                car_images, 4, FLAT, 3.0 * M_PI_8, 0);
            base_frame += 4 * sprites_per_view;
            render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                car_images, 4, FLAT, -3.0 * M_PI_8, 0);
            base_frame += 4 * sprites_per_view;
            render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                car_images, 4, FLAT, M_PI_2, 0);
            base_frame += 4 * sprites_per_view;
            render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                car_images, 4, FLAT, -M_PI_2, 0);
            base_frame += 4 * sprites_per_view;
            render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                car_images, 4, FLAT, 5.0 * M_PI_8, 0);
            base_frame += 4 * sprites_per_view;
            render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                car_images, 4, FLAT, -5.0 * M_PI_8, 0);
            base_frame += 4 * sprites_per_view;
            render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                car_images, 4, FLAT, 3.0 * M_PI_4, 0);
            base_frame += 4 * sprites_per_view;
            render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                car_images, 4, FLAT, -3.0 * M_PI_4, 0);
            base_frame += 4 * sprites_per_view;
            render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                car_images, 4, FLAT, 7.0 * M_PI_8, 0);
            base_frame += 4 * sprites_per_view;
            render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                car_images, 4, FLAT, -7.0 * M_PI_8, 0);
            base_frame += 4 * sprites_per_view;
        }
        if (sprite_flags & SPRITE_SLOPE_BANK_TRANSITION) {
            render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                car_images, 32, FG_TRANSITION, BANK_TRANSITION, 0);
            base_frame += 32 * sprites_per_view;
            render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                car_images, 32, FG_TRANSITION, -BANK_TRANSITION, 0);
            base_frame += 32 * sprites_per_view;
            render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                car_images, 32, -FG_TRANSITION, BANK_TRANSITION, 0);
            base_frame += 32 * sprites_per_view;
            render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                car_images, 32, -FG_TRANSITION, -BANK_TRANSITION, 0);
            base_frame += 32 * sprites_per_view;
        }
        if (sprite_flags & SPRITE_DIAGONAL_BANK_TRANSITION) {
            render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                car_images, 4, GENTLE_DIAGONAL, BANK_TRANSITION, M_PI_4);
            base_frame += 4 * sprites_per_view;
            render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                car_images, 4, GENTLE_DIAGONAL, -BANK_TRANSITION, M_PI_4);
            base_frame += 4 * sprites_per_view;
            render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                car_images, 4, -GENTLE_DIAGONAL, BANK_TRANSITION, M_PI_4);
            base_frame += 4 * sprites_per_view;
            render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                car_images, 4, -GENTLE_DIAGONAL, -BANK_TRANSITION,
                M_PI_4);
            base_frame += 4 * sprites_per_view;
        }
        if (sprite_flags & SPRITE_SLOPED_BANK_TRANSITION) {
            render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                car_images, 4, GENTLE, BANK_TRANSITION, 0);
            base_frame += 4 * sprites_per_view;
            render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                car_images, 4, GENTLE, -BANK_TRANSITION, 0);
            base_frame += 4 * sprites_per_view;
            render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                car_images, 4, -GENTLE, BANK_TRANSITION, 0);
            base_frame += 4 * sprites_per_view;
            render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                car_images, 4, -GENTLE, -BANK_TRANSITION, 0);
            base_frame += 4 * sprites_per_view;
        }
        if (sprite_flags & SPRITE_SLOPED_BANKED_TURN) {
            render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                car_images, 32, GENTLE, BANK, 0);
            base_frame += 32 * sprites_per_view;
            render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                car_images, 32, GENTLE, -BANK, 0);
            base_frame += 32 * sprites_per_view;
            render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                car_images, 32, -GENTLE, BANK, 0);
            base_frame += 32 * sprites_per_view;
            render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                car_images, 32, -GENTLE, -BANK, 0);
            base_frame += 32 * sprites_per_view;
        }
        if (sprite_flags & SPRITE_BANKED_SLOPE_TRANSITION) {
            render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                car_images, 4, FG_TRANSITION, BANK, 0);
            base_frame += 4 * sprites_per_view;
            render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                car_images, 4, FG_TRANSITION, -BANK, 0);
            base_frame += 4 * sprites_per_view;
            render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                car_images, 4, -FG_TRANSITION, BANK, 0);
            base_frame += 4 * sprites_per_view;
            render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                car_images, 4, -FG_TRANSITION, -BANK, 0);
            base_frame += 4 * sprites_per_view;
        }
        if (sprite_flags & SPRITE_CORKSCREW) {
#define CORKSCREW_ANGLE_1 2.0 * M_PI_12
#define CORKSCREW_ANGLE_2 4.0 * M_PI_12
#define CORKSCREW_ANGLE_3 M_PI_2
#define CORKSCREW_ANGLE_4 8.0 * M_PI_12
#define CORKSCREW_ANGLE_5 10.0 * M_PI_12

            // Corkscrew right
            render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                car_images, 4, CORKSCREW_RIGHT_PITCH(CORKSCREW_ANGLE_1),
                CORKSCREW_RIGHT_ROLL(CORKSCREW_ANGLE_1),
                CORKSCREW_RIGHT_YAW(CORKSCREW_ANGLE_1));
            base_frame += 4 * sprites_per_view;
            render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                car_images, 4, CORKSCREW_RIGHT_PITCH(CORKSCREW_ANGLE_2),
                CORKSCREW_RIGHT_ROLL(CORKSCREW_ANGLE_2),
                CORKSCREW_RIGHT_YAW(CORKSCREW_ANGLE_2));
            base_frame += 4 * sprites_per_view;
            render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                car_images, 4, CORKSCREW_RIGHT_PITCH(CORKSCREW_ANGLE_3),
                CORKSCREW_RIGHT_ROLL(CORKSCREW_ANGLE_3),
                CORKSCREW_RIGHT_YAW(CORKSCREW_ANGLE_3));
            base_frame += 4 * sprites_per_view;
            render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                car_images, 4, CORKSCREW_RIGHT_PITCH(CORKSCREW_ANGLE_4),
                CORKSCREW_RIGHT_ROLL(CORKSCREW_ANGLE_4),
                CORKSCREW_RIGHT_YAW(CORKSCREW_ANGLE_4));
            base_frame += 4 * sprites_per_view;
            render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                car_images, 4, CORKSCREW_RIGHT_PITCH(CORKSCREW_ANGLE_5),
                CORKSCREW_RIGHT_ROLL(CORKSCREW_ANGLE_5),
                CORKSCREW_RIGHT_YAW(CORKSCREW_ANGLE_5));
            base_frame += 4 * sprites_per_view;

            render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                car_images, 4, CORKSCREW_RIGHT_PITCH(-CORKSCREW_ANGLE_1),
                CORKSCREW_RIGHT_ROLL(-CORKSCREW_ANGLE_1),
                CORKSCREW_RIGHT_YAW(-CORKSCREW_ANGLE_1));
            base_frame += 4 * sprites_per_view;
            render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                car_images, 4, CORKSCREW_RIGHT_PITCH(-CORKSCREW_ANGLE_2),
                CORKSCREW_RIGHT_ROLL(-CORKSCREW_ANGLE_2),
                CORKSCREW_RIGHT_YAW(-CORKSCREW_ANGLE_2));
            base_frame += 4 * sprites_per_view;
            render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                car_images, 4, CORKSCREW_RIGHT_PITCH(-CORKSCREW_ANGLE_3),
                CORKSCREW_RIGHT_ROLL(-CORKSCREW_ANGLE_3),
                CORKSCREW_RIGHT_YAW(-CORKSCREW_ANGLE_3));
            base_frame += 4 * sprites_per_view;
            render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                car_images, 4, CORKSCREW_RIGHT_PITCH(-CORKSCREW_ANGLE_4),
                CORKSCREW_RIGHT_ROLL(-CORKSCREW_ANGLE_4),
                CORKSCREW_RIGHT_YAW(-CORKSCREW_ANGLE_4));
            base_frame += 4 * sprites_per_view;
            render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                car_images, 4, CORKSCREW_RIGHT_PITCH(-CORKSCREW_ANGLE_5),
                CORKSCREW_RIGHT_ROLL(-CORKSCREW_ANGLE_5),
                CORKSCREW_RIGHT_YAW(-CORKSCREW_ANGLE_5));
            base_frame += 4 * sprites_per_view;

            // Half corkscrew left
            render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                car_images, 4, CORKSCREW_LEFT_PITCH(CORKSCREW_ANGLE_1),
                CORKSCREW_LEFT_ROLL(CORKSCREW_ANGLE_1),
                CORKSCREW_LEFT_YAW(CORKSCREW_ANGLE_1));
            base_frame += 4 * sprites_per_view;
            render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                car_images, 4, CORKSCREW_LEFT_PITCH(CORKSCREW_ANGLE_2),
                CORKSCREW_LEFT_ROLL(CORKSCREW_ANGLE_2),
                CORKSCREW_LEFT_YAW(CORKSCREW_ANGLE_2));
            base_frame += 4 * sprites_per_view;
            render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                car_images, 4, CORKSCREW_LEFT_PITCH(CORKSCREW_ANGLE_3),
                CORKSCREW_LEFT_ROLL(CORKSCREW_ANGLE_3),
                CORKSCREW_LEFT_YAW(CORKSCREW_ANGLE_3));
            base_frame += 4 * sprites_per_view;
            render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                car_images, 4, CORKSCREW_LEFT_PITCH(CORKSCREW_ANGLE_4),
                CORKSCREW_LEFT_ROLL(CORKSCREW_ANGLE_4),
                CORKSCREW_LEFT_YAW(CORKSCREW_ANGLE_4));
            base_frame += 4 * sprites_per_view;
            render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                car_images, 4, CORKSCREW_LEFT_PITCH(CORKSCREW_ANGLE_5),
                CORKSCREW_LEFT_ROLL(CORKSCREW_ANGLE_5),
                CORKSCREW_LEFT_YAW(CORKSCREW_ANGLE_5));
            base_frame += 4 * sprites_per_view;

            render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                car_images, 4, CORKSCREW_LEFT_PITCH(-CORKSCREW_ANGLE_1),
                CORKSCREW_LEFT_ROLL(-CORKSCREW_ANGLE_1),
                CORKSCREW_LEFT_YAW(-CORKSCREW_ANGLE_1));
            base_frame += 4 * sprites_per_view;
            render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                car_images, 4, CORKSCREW_LEFT_PITCH(-CORKSCREW_ANGLE_2),
                CORKSCREW_LEFT_ROLL(-CORKSCREW_ANGLE_2),
                CORKSCREW_LEFT_YAW(-CORKSCREW_ANGLE_2));
            base_frame += 4 * sprites_per_view;
            render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                car_images, 4, CORKSCREW_LEFT_PITCH(-CORKSCREW_ANGLE_3),
                CORKSCREW_LEFT_ROLL(-CORKSCREW_ANGLE_3),
                CORKSCREW_LEFT_YAW(-CORKSCREW_ANGLE_3));
            base_frame += 4 * sprites_per_view;
            render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                car_images, 4, CORKSCREW_LEFT_PITCH(-CORKSCREW_ANGLE_4),
                CORKSCREW_LEFT_ROLL(-CORKSCREW_ANGLE_4),
                CORKSCREW_LEFT_YAW(-CORKSCREW_ANGLE_4));
            base_frame += 4 * sprites_per_view;
            render_rotation(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                car_images, 4, CORKSCREW_LEFT_PITCH(-CORKSCREW_ANGLE_5),
                CORKSCREW_LEFT_ROLL(-CORKSCREW_ANGLE_5),
                CORKSCREW_LEFT_YAW(-CORKSCREW_ANGLE_5));
            base_frame += 4 * sprites_per_view;
        }
        if (sprite_flags & SPRITE_RESTRAINT_ANIMATION) {
            render_loading(images, animation, car_flags, animation_type, base_frame, sprites_per_image,
                car_images);
            base_frame += 12 * 1;
        }
    }
}

object_t* project_export_dat(project_t* project)
{
    object_t* object = object_new_ride();

    object->ride_header->track_style = project->track_type;

    // Set strings
    char capacity[256];
    sprintf(capacity, "%d passengers per car",
        animation_count_riders(
            project->cars[project->car_types[CAR_INDEX_DEFAULT]].animation));
    string_table_set_string_by_language(object->string_tables[STRING_TABLE_NAME],
        LANGUAGE_ENGLISH_UK, project->name);
    string_table_set_string_by_language(object->string_tables[STRING_TABLE_NAME],
        LANGUAGE_ENGLISH_US, project->name);
    string_table_set_string_by_language(
        object->string_tables[STRING_TABLE_DESCRIPTION], LANGUAGE_ENGLISH_UK,
        project->description);
    string_table_set_string_by_language(
        object->string_tables[STRING_TABLE_DESCRIPTION], LANGUAGE_ENGLISH_US,
        project->description);
    string_table_set_string_by_language(
        object->string_tables[STRING_TABLE_CAPACITY], LANGUAGE_ENGLISH_UK,
        capacity);
    string_table_set_string_by_language(
        object->string_tables[STRING_TABLE_CAPACITY], LANGUAGE_ENGLISH_US,
        capacity);

    // Set color schemes
    ride_structures_set_num_default_colors(object->optional,
        project->num_color_schemes);
    for (uint32_t i = 0; i < project->num_color_schemes; i++) {
        object->optional->default_colors[i] = project->color_schemes[i];
    }

    // Set car types
    object->ride_header->car_types[CAR_INDEX_DEFAULT] = project->car_types[CAR_INDEX_DEFAULT];
    object->ride_header->car_types[CAR_INDEX_FRONT] = project->car_types[CAR_INDEX_FRONT];
    object->ride_header->car_types[CAR_INDEX_SECOND] = project->car_types[CAR_INDEX_SECOND];
    object->ride_header->car_types[CAR_INDEX_THIRD] = project->car_types[CAR_INDEX_THIRD];
    object->ride_header->car_types[CAR_INDEX_REAR] = project->car_types[CAR_INDEX_REAR];

    object->ride_header->flags = project->flags | RIDE_SEPARATE_RIDE; //this is kept for backwards compatability

    // Set categories
    object->ride_header->categories[0] = project->ride_categories[0];
    object->ride_header->categories[1] = project->ride_categories[1];
    object->ride_header->track_sections = 0xFFFFFFFFFFFFFFFFl;//refer to dat.h for what flags are what

    object->ride_header->minimum_cars = project->minimum_cars;
    object->ride_header->maximum_cars = project->maximum_cars;

    object->ride_header->zero_cars = project->zero_cars;

    object->ride_header->car_icon_index = project->car_icon_index;

    object->ride_header->excitement = project->excitement;
    object->ride_header->intensity = project->intensity;
    object->ride_header->nausea = project->nausea;
    object->ride_header->max_height = project->max_height;

    unsigned char cars_used[NUM_CARS];
    memset(cars_used, 0, NUM_CARS);
    for (int i = 0; i < 5; i++)
        if (project->car_types[i] != 0xFF)
            cars_used[project->car_types[i]] = 1;
    for (int i = 0; i < NUM_CARS; i++) {
        if (cars_used[i] || project->cars[i].flags & CAR_CAN_INVERT) {
            object->ride_header->cars[i].highest_rotation_index = 31; // TODO: change this based on flags
			object->ride_header->cars[i].car_visual = project->cars[i].car_visual;
			object->ride_header->cars[i].effect_visual = project->cars[i].effect_visual;
            object->ride_header->cars[i].flags = project->cars[i].flags;
            object->ride_header->cars[i].friction = project->cars[i].friction;
            object->ride_header->cars[i].spacing = project->cars[i].spacing;
            object->ride_header->cars[i].running_sound = project->cars[i].running_sound;
            object->ride_header->cars[i].secondary_sound = project->cars[i].secondary_sound;
            object->ride_header->cars[i].z_value = project->cars[i].z_value;
            object->ride_header->cars[i].override_vertical_frames = project->cars[i].override_vertical_frames;
            object->ride_header->cars[i].vehicle_tab_vertical_offset = project->cars[i].vehicle_tab_vertical_offset; // vertical offset in vehicle preview and paint preview

            if (project->cars[i].flags & CAR_IS_POWERED) {
				object->ride_header->cars[i].powered_velocity = project->cars[i].powered_velocity;
                object->ride_header->cars[i].powered_acceleration = project->cars[i].powered_acceleration;
            }
            if (project->cars[i].flags & CAR_IS_ANIMATED) {
                object->ride_header->cars[i].animation_type = project->cars[i].animation_type;
            }
            if (project->cars[i].flags & CAR_IS_SPINNING) {
                object->ride_header->cars[i].spin_inertia = project->cars[i].spin_inertia;
                object->ride_header->cars[i].spin_friction = project->cars[i].spin_friction;
            }

            // Some sprites ought be included if others are. Here we compute the
            // minimum set of sprite flags needed that includes all the selected flags
            object->ride_header->cars[i].sprites = project->cars[i].sprites | SPRITE_FLAT_SLOPE;
            if (project->cars[i].sprites & SPRITE_VERTICAL_SLOPE)
                object->ride_header->cars[i].sprites |= SPRITE_STEEP_SLOPE;
            if (project->cars[i].sprites & SPRITE_STEEP_SLOPE)
                object->ride_header->cars[i].sprites |= SPRITE_GENTLE_SLOPE;
            if (project->cars[i].sprites & SPRITE_BANKING) {
                if (project->cars[i].sprites & SPRITE_DIAGONAL_SLOPE)
                    object->ride_header->cars[i].sprites |= SPRITE_DIAGONAL_BANK_TRANSITION | SPRITE_SLOPED_BANK_TRANSITION;
                if (project->cars[i].sprites & SPRITE_GENTLE_SLOPE)
                    object->ride_header->cars[i].sprites |= SPRITE_BANKED_SLOPE_TRANSITION | SPRITE_SLOPE_BANK_TRANSITION | SPRITE_SLOPED_BANK_TRANSITION | SPRITE_SLOPED_BANKED_TURN;
            }
            if (project->cars[i].sprites & SPRITE_INLINE_TWIST) {
                object->ride_header->cars[i].sprites |= SPRITE_BANKING;
                object->ride_header->cars[i].sprites |= SPRITE_VERTICAL_SLOPE;
            }

            animation_t* animation = project->cars[i].animation;
            int num_riders = animation_count_riders(animation);
            object->ride_header->cars[i].riders = num_riders;
            object->ride_header->cars[i].rider_pairs = num_riders != 1 ? 0x80 : 0;
            object->ride_header->cars[i].rider_sprites = num_riders != 1 ? (num_riders / 2) : 1;

            ride_structures_set_num_peep_positions(object->optional, i, num_riders);
            int rider = 0;
            int model = 0;
            float variables[ANIMATION_NUM_VARIABLES] = { 0, 0, 0, 0, 0, 0, 0 };
            variables[VAR_RESTRAINT] = 1;
            animation_calculate_object_transforms(animation,variables);

            while (rider < num_riders && model < animation->num_objects) {
                if (animation->objects[model]->model->is_rider) {
                    int8_t peep_position = (int8_t)(-animation->objects[model]->transform.Data[11]
                        * (32.0 / 3.0));
                    object->optional->peep_positions[i].positions[rider] = *((uint8_t*)(&peep_position));
                    rider++;
                }
                model++;
            }
        }
    }
    project_render_sprites(project, object);
    return object;
}
