#include<stdlib.h>
#include<string.h>
#include<math.h>
#include "project.h"

project_t* project_new()
{
project_t* project=malloc(sizeof(project_t));
project->track_type=0x33;//Default to B&M track
project->flags=RIDE_SEPERATE;
project->minimum_cars=3;
project->maximum_cars=8;
project->zero_cars=0;
memset(project->car_types,0xFF,5);
project->car_types[CAR_INDEX_DEFAULT]=0;
project->models=NULL;
project->num_models=0;
int i;
    for(i=0;i<NUM_CARS;i++)
    {
    project->cars[i].animation=animation_new();
    project->cars[i].flags=0;
    project->cars[i].spacing=0x300;
    project->cars[i].z_value=8;
    project->cars[i].friction=0x2A8;
    }
return project;
}
void project_add_model(project_t* project,model_t* model)
{
project->models=realloc(project->models,(project->num_models+1)*sizeof(model_t*));
project->models[project->num_models++]=model;
}
void project_free(project_t* project)
{
int i;
    for(i=0;i<project->num_models;i++)model_free(project->models[i]);
    for(i=0;i<NUM_CARS;i++)animation_free(project->cars[i].animation);
free(project->models);
free(project);
}


void render_rotation(image_list_t* image_list,animation_t* animation,int* image_index_ptr,int num_frames,double pitch,double roll,double yaw)
{
int i,j;
int image_index=*image_index_ptr;

Matrix pitch_matrix=MatrixIdentity();
pitch_matrix.Data[5]=cos(pitch);
pitch_matrix.Data[6]=sin(pitch);
pitch_matrix.Data[9]=-sin(pitch);
pitch_matrix.Data[10]=cos(pitch);

Matrix roll_matrix=MatrixIdentity();
roll_matrix.Data[0]=cos(roll);
roll_matrix.Data[1]=sin(roll);
roll_matrix.Data[4]=-sin(roll);
roll_matrix.Data[5]=cos(roll);

Matrix yaw_matrix=MatrixIdentity();
yaw_matrix.Data[0]=cos(yaw);
yaw_matrix.Data[2]=-sin(yaw);
yaw_matrix.Data[8]=sin(yaw);
yaw_matrix.Data[10]=cos(yaw);

Matrix transform_matrix=MatrixMultiply(yaw_matrix,MatrixMultiply(pitch_matrix,roll_matrix));

double rotation=0;
double step=2*3.141592654/num_frames;
    for(j=0;j<num_frames;j++)
    {
    Matrix rotation_matrix=MatrixIdentity();
    rotation_matrix.Data[0]=cos(rotation);
    rotation_matrix.Data[2]=-sin(rotation);
    rotation_matrix.Data[8]=sin(rotation);
    rotation_matrix.Data[10]=cos(rotation);
    int animindex=0;/*(int)((2*(yaw+rotation)+pitch)*16.0/M_PI)%32;*/
    renderer_clear_buffers();
    animation_render_frame(animation,animindex,MatrixMultiply(rotation_matrix,transform_matrix));
    image_list_set_image(image_list,image_index,renderer_get_image());
    image_index++;
    rotation+=step;
    }
*image_index_ptr=image_index;
}
void render_loading(image_list_t* image_list,animation_t* animation,int* frame_ptr)
{
int i,j;
int frame=*frame_ptr;
Matrix rotation_matrix=MatrixIdentity();
Matrix yaw_matrix=MatrixIdentity();
yaw_matrix.Data[0]=0;
yaw_matrix.Data[2]=-1;
yaw_matrix.Data[8]=1;
yaw_matrix.Data[10]=0;
    for(i=1;i<4;i++)
    {
        for(j=0;j<4;j++)
        {
        animation_render_frame(animation,i,rotation_matrix);
        image_list_set_image(image_list,frame,renderer_get_image());
        frame++;
        rotation_matrix=MatrixMultiply(rotation_matrix,yaw_matrix);
        }
    }
*frame_ptr=frame;
}
void project_render_sprites(project_t* project,object_t* object)
{
int i;
ride_header_t* header=object->ride_header;
int num_sprites=3;
//Compute number of sprites
    for(i=0;i<NUM_CARS;i++)
    {
    uint16_t sprites=header->cars[i].sprites;
        if(sprites&SPRITE_FLAT_SLOPE)num_sprites+=32;
        if(sprites&SPRITE_GENTLE_SLOPE)num_sprites+=72;
        if(sprites&SPRITE_STEEP_SLOPE)num_sprites+=80;
        if(sprites&SPRITE_VERTICAL_SLOPE)num_sprites+=116;//These also include loop sprites by default
        if(sprites&SPRITE_DIAGONAL_SLOPE)num_sprites+=24;
        if(sprites&SPRITE_BANKING)num_sprites+=80;
        if(sprites&SPRITE_SLOPE_BANK_TRANSITION)num_sprites+=128;
        if(sprites&SPRITE_DIAGONAL_BANK_TRANSITION)num_sprites+=16;
        if(sprites&SPRITE_SLOPED_BANK_TRANSITION)num_sprites+=16;
        if(sprites&SPRITE_SLOPED_BANKED_TURN)num_sprites+=128;
        if(sprites&SPRITE_BANKED_SLOPE_TRANSITION)num_sprites+=16;
        if(sprites&SPRITE_RESTRAINT_ANIMATION)num_sprites+=12;
    }

//Reallocate images
image_list_set_num_images(object->images,num_sprites);

//Render sprites
int frame=3;
image_list_set_image(object->images,0,image_new(112,112,20));
image_list_set_image(object->images,1,image_new(1,1,0));
image_list_set_image(object->images,2,image_new(1,1,0));
    for(i=0;i<NUM_CARS;i++)//Test render only one car
    {
    uint16_t sprites=header->cars[i].sprites;
    animation_t* animation=project->cars[i].animation;
    image_list_t* images=object->images;
        if(sprites&SPRITE_FLAT_SLOPE)
        {
        render_rotation(images,animation,&frame,32,FLAT,0,0);
        }
        if(sprites&SPRITE_GENTLE_SLOPE)
        {
        render_rotation(images,animation,&frame,4,FG_TRANSITION,0,0);
        render_rotation(images,animation,&frame,4,-FG_TRANSITION,0,0);
        render_rotation(images,animation,&frame,32,GENTLE,0,0);//Why the fuck does this become a 4 frame rotation with spinning enabled?
        render_rotation(images,animation,&frame,32,-GENTLE,0,0);
        }
        if(sprites&SPRITE_STEEP_SLOPE)
        {
        render_rotation(images,animation,&frame,8,GS_TRANSITION,0,0);
        render_rotation(images,animation,&frame,8,-GS_TRANSITION,0,0);
        render_rotation(images,animation,&frame,32,STEEP,0,0);
        render_rotation(images,animation,&frame,32,-STEEP,0,0);
        }
        if(sprites&SPRITE_VERTICAL_SLOPE)
        {
        render_rotation(images,animation,&frame,4,SV_TRANSITION,0,0);
        render_rotation(images,animation,&frame,4,-SV_TRANSITION,0,0);
        render_rotation(images,animation,&frame,32,VERTICAL,0,0);
        render_rotation(images,animation,&frame,32,-VERTICAL,0,0);
        //Loop sprites
        render_rotation(images,animation,&frame,4,VERTICAL+PI_12,0,0);
        render_rotation(images,animation,&frame,4,-VERTICAL-PI_12,0,0);
        render_rotation(images,animation,&frame,4,VERTICAL+2*PI_12,0,0);
        render_rotation(images,animation,&frame,4,-VERTICAL-2*PI_12,0,0);
        render_rotation(images,animation,&frame,4,VERTICAL+3*PI_12,0,0);
        render_rotation(images,animation,&frame,4,-VERTICAL-3*PI_12,0,0);
        render_rotation(images,animation,&frame,4,VERTICAL+4*PI_12,0,0);
        render_rotation(images,animation,&frame,4,-VERTICAL-4*PI_12,0,0);
        render_rotation(images,animation,&frame,4,VERTICAL+5*PI_12,0,0);
        render_rotation(images,animation,&frame,4,-VERTICAL-5*PI_12,0,0);
        render_rotation(images,animation,&frame,4,M_PI,0,0);
        }
        if(sprites&SPRITE_DIAGONAL_SLOPE)
        {
        render_rotation(images,animation,&frame,4,FG_TRANSITION_DIAGONAL,0,M_PI_4);
        render_rotation(images,animation,&frame,4,-FG_TRANSITION_DIAGONAL,0,M_PI_4);
        render_rotation(images,animation,&frame,4,GENTLE_DIAGONAL,0,M_PI_4);
        render_rotation(images,animation,&frame,4,-GENTLE_DIAGONAL,0,M_PI_4);
        render_rotation(images,animation,&frame,4,STEEP_DIAGONAL,0,M_PI_4);
        render_rotation(images,animation,&frame,4,-STEEP_DIAGONAL,0,M_PI_4);
        }
        if(sprites&SPRITE_BANKING)
        {
        render_rotation(images,animation,&frame,8,FLAT,BANK_TRANSITION,0);
        render_rotation(images,animation,&frame,8,FLAT,-BANK_TRANSITION,0);
        render_rotation(images,animation,&frame,32,FLAT,BANK,0);
        render_rotation(images,animation,&frame,32,FLAT,-BANK,0);
        }
        if(sprites&SPRITE_SLOPE_BANK_TRANSITION)
        {
        render_rotation(images,animation,&frame,32,FG_TRANSITION,BANK_TRANSITION,0);
        render_rotation(images,animation,&frame,32,FG_TRANSITION,-BANK_TRANSITION,0);
        render_rotation(images,animation,&frame,32,-FG_TRANSITION,BANK_TRANSITION,0);
        render_rotation(images,animation,&frame,32,-FG_TRANSITION,-BANK_TRANSITION,0);
        }
        if(sprites&SPRITE_DIAGONAL_BANK_TRANSITION)
        {
        render_rotation(images,animation,&frame,4,GENTLE_DIAGONAL,BANK_TRANSITION,M_PI_4);
        render_rotation(images,animation,&frame,4,GENTLE_DIAGONAL,-BANK_TRANSITION,M_PI_4);
        render_rotation(images,animation,&frame,4,-GENTLE_DIAGONAL,BANK_TRANSITION,M_PI_4);
        render_rotation(images,animation,&frame,4,-GENTLE_DIAGONAL,-BANK_TRANSITION,M_PI_4);
        }
        if(sprites&SPRITE_SLOPED_BANK_TRANSITION)
        {
        render_rotation(images,animation,&frame,4,GENTLE,BANK_TRANSITION,0);
        render_rotation(images,animation,&frame,4,GENTLE,-BANK_TRANSITION,0);
        render_rotation(images,animation,&frame,4,-GENTLE,BANK_TRANSITION,0);
        render_rotation(images,animation,&frame,4,-GENTLE,-BANK_TRANSITION,0);
        }
        if(sprites&SPRITE_SLOPED_BANKED_TURN)
        {
        render_rotation(images,animation,&frame,32,GENTLE,BANK,0);
        render_rotation(images,animation,&frame,32,GENTLE,-BANK,0);
        render_rotation(images,animation,&frame,32,-GENTLE,BANK,0);
        render_rotation(images,animation,&frame,32,-GENTLE,-BANK,0);
        }
        if(sprites&SPRITE_BANKED_SLOPE_TRANSITION)
        {
        render_rotation(images,animation,&frame,4,FG_TRANSITION,BANK,0);
        render_rotation(images,animation,&frame,4,FG_TRANSITION,-BANK,0);
        render_rotation(images,animation,&frame,4,-FG_TRANSITION,BANK,0);
        render_rotation(images,animation,&frame,4,-FG_TRANSITION,-BANK,0);
        }
        if(sprites&SPRITE_RESTRAINT_ANIMATION)
        {
        render_loading(images,project->cars[i].animation,&frame);
        }
    }

}

object_t* project_export_dat(project_t* project)
{
int i;
object_t* object=object_new_ride();
object->ride_header->track_style=project->track_type;

//Set car types
object->ride_header->car_types[CAR_INDEX_DEFAULT]=project->car_types[CAR_INDEX_DEFAULT];
object->ride_header->car_types[CAR_INDEX_FRONT]=project->car_types[CAR_INDEX_FRONT];
object->ride_header->car_types[CAR_INDEX_SECOND]=project->car_types[CAR_INDEX_SECOND];
object->ride_header->car_types[CAR_INDEX_THIRD]=project->car_types[CAR_INDEX_THIRD];
object->ride_header->car_types[CAR_INDEX_REAR]=project->car_types[CAR_INDEX_REAR];


object->ride_header->flags=project->flags;
//Set categories
object->ride_header->categories[0]=2;
object->ride_header->categories[1]=project->flags&RIDE_WET?0x3:0xFF;
object->ride_header->track_sections=0xFFFFFFFFFFFFFFFFl;
object->ride_header->minimum_cars=project->minimum_cars;
object->ride_header->maximum_cars=project->maximum_cars;
object->ride_header->zero_cars=project->zero_cars;

unsigned char cars_used[NUM_CARS];
memset(cars_used,0,NUM_CARS);
    for(i=0;i<5;i++)if(project->car_types[i]!=0xFF)cars_used[project->car_types[i]]=1;
    for(i=0;i<NUM_CARS;i++)
    {
        if(cars_used[i])
        {
        object->ride_header->cars[i].highest_rotation_index=31;
        object->ride_header->cars[i].flags=project->cars[i].flags;
        object->ride_header->cars[i].friction=project->cars[i].friction;
        object->ride_header->cars[i].spacing=project->cars[i].spacing;
        object->ride_header->cars[i].z_value=project->cars[i].z_value;
        object->ride_header->cars[i].sprites=SPRITE_FLAT_SLOPE|SPRITE_GENTLE_SLOPE|SPRITE_STEEP_SLOPE|SPRITE_BANKED_SLOPE_TRANSITION|SPRITE_BANKING|SPRITE_DIAGONAL_BANK_TRANSITION|SPRITE_DIAGONAL_SLOPE|SPRITE_SLOPE_BANK_TRANSITION|SPRITE_SLOPED_BANK_TRANSITION|SPRITE_SLOPED_BANKED_TURN|SPRITE_VERTICAL_SLOPE;
            if(project->cars[i].animation->num_frames>=4)object->ride_header->cars[i].sprites|=SPRITE_RESTRAINT_ANIMATION;
        object->ride_header->cars[i].riders=0;
        object->ride_header->cars[i].rider_pairs=0;
        object->ride_header->cars[i].rider_sprites=0;
        }
    }
project_render_sprites(project,object);
return object;
}
