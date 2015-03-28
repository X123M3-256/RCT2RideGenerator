#ifndef ANIMATION_H_INCLUDED
#define ANIMATION_H_INCLUDED
#include "model.h"
#include "linearalgebra.h"

#define NONE 1
#define RENDER 2
#define MASK 3

#define MAX_FRAMES 64
#define MAX_OBJECTS_PER_FRAME 16

typedef struct
{
Vector position;
Vector rotation;
Matrix transform;
}object_transform_t;

typedef struct
{
model_t* model;
int parent_index;
}object_data_t;

typedef struct
{
object_transform_t frames[MAX_FRAMES][MAX_OBJECTS_PER_FRAME];
object_data_t objects[MAX_OBJECTS_PER_FRAME];
int num_objects;
int num_frames;
}animation_t;




animation_t* animation_new();
void animation_set_num_frames(animation_t* animation,int frames);
int animation_add_object(animation_t* animation,model_t* model);
void animation_update_transform(object_transform_t* object_transform,Vector position,Vector rotation);
void animation_update_parent(animation_t* animation,int object,int parent);
void animation_render_frame(animation_t* animation,int frame,Matrix model_view);
void animation_free(animation_t* animation);




#endif // ANIMATION_H_INCLUDED
