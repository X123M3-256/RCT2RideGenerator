#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "palette.h"
#include "renderer.h"
#include "animation.h"


animation_t* animation_new()
{
animation_t* animation=malloc(sizeof(animation_t));
animation->num_frames=1;
animation->num_riders=0;
animation->num_objects=0;
return animation;
}
void animation_set_num_frames(animation_t* animation,int frames)
{
int i,j;
    if(frames>animation->num_frames)
    {
        for(i=animation->num_frames;i<frames;i++)
        for(j=0;j<animation->num_objects;j++)
        {
        animation->frames[i][j]=animation->frames[i-1][j];
        }
    }
animation->num_frames=frames;
}
int animation_add_object(animation_t* animation,model_t* model)
{
int i;
animation->objects[animation->num_objects].model=model;
animation->objects[animation->num_objects].parent_index=-1;
    for(i=0;i<animation->num_frames;i++)
    {
    object_transform_t* transform=&(animation->frames[i][animation->num_objects]);
    transform->position.X=0;
    transform->position.Y=0;
    transform->position.Z=0;
    transform->rotation.X=0;
    transform->rotation.Y=0;
    transform->rotation.Z=0;
    transform->transform=MatrixIdentity();
    }
    if(model->is_rider)animation->num_riders++;
return animation->num_objects++;
}

void animation_update_transform(object_transform_t* transform,Vector position,Vector rotation)
{
transform->position=position;
transform->rotation=rotation;
transform->transform=MatrixFromEulerAngles(rotation);
transform->transform.Data[3]=position.X;
transform->transform.Data[7]=position.Y;
transform->transform.Data[11]=position.Z;
}
void animation_update_parent(animation_t* animation,int object,int parent)
{
    if(parent>=0&&parent<animation->num_objects)animation->objects[object].parent_index=parent;
    else animation->objects[object].parent_index=-1;
}

void animation_render_object(animation_t* animation,int frame,int object,Matrix model_view)
{
Matrix transform=animation->frames[frame][object].transform;
int cur_object_index=object;
    while((cur_object_index=animation->objects[cur_object_index].parent_index)!=-1)
    {
    transform=MatrixMultiply(animation->frames[frame][cur_object_index].transform,transform);
    }
renderer_render_model(animation->objects[object].model,MatrixMultiply(model_view,transform));
}

void animation_render_frame(animation_t* animation,int frame,Matrix model_view)
{
int i;
renderer_clear_buffers();
    for(i=0;i<animation->num_objects;i++)
    {
    animation_render_object(animation,frame,i,model_view);
    }
}

render_data_t animation_split_render_begin(animation_t* animation,int frame,Matrix model_view)
{
int i;
render_data_t data;
data.cur_object=0;
data.frame=frame;
data.model_view=model_view;

renderer_clear_buffers();
    for(i=0;i<animation->num_objects;i++)
    {
        if(!animation->objects[i].model->is_rider)
        {
        animation_render_object(animation,frame,i,model_view);
        }
    }
return data;
}

int animation_split_render_render_rider(animation_t* animation,int frame,Matrix model_view,int cur_object)
{
    while(cur_object<animation->num_objects)
    {
        if(animation->objects[cur_object].model->is_rider)
        {
        animation_render_object(animation,frame,cur_object,model_view);
        cur_object++;
        break;
        }
    cur_object++;
    }
return cur_object;
}

void animation_split_render_next_image(animation_t* animation,render_data_t* data)
{
renderer_clear_color_buffer();
//Render pair of riders
data->cur_object=animation_split_render_render_rider(animation,data->frame,data->model_view,data->cur_object);
    if(animation->num_riders>1)
    {
    renderer_remap_color(COLOR_PEEP_REMAP_1,COLOR_PEEP_REMAP_2);
    data->cur_object=animation_split_render_render_rider(animation,data->frame,data->model_view,data->cur_object);
    }
}



void animation_free(animation_t* animation)
{
free(animation);
}
