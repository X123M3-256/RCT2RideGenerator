#include<stdlib.h>
#include<string.h>
#include<math.h>
#include "project.h"

project_t* project_new()
{
project_t* project=malloc(sizeof(project_t));
project->name=malloc(13);
project->description=malloc(14);
strcpy(project->name,"Unnamed ride");
strcpy(project->description,"No description");
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
free(project->name);
free(project->description);
free(project);
}


void render_rotation(image_list_t* image_list,animation_t* animation,int base_frame,int sprites_per_image,int images,int num_frames,double pitch,double roll,double yaw)
{
int i,j;
Matrix transform_matrix=MatrixFromEulerAngles(VectorFromComponents(-pitch,-yaw,-roll));

double rotation=0;
double step=2*3.141592654/num_frames;
    for(i=0;i<num_frames;i++)
    {
    Matrix rotation_matrix=MatrixIdentity();
    rotation_matrix.Data[0]=cos(rotation);
    rotation_matrix.Data[2]=-sin(rotation);
    rotation_matrix.Data[8]=sin(rotation);
    rotation_matrix.Data[10]=cos(rotation);
    rotation+=step;

    //int animindex=0;/*(int)((2*(yaw+rotation)+pitch)*16.0/M_PI)%32;*/
    renderer_clear_buffers();
    render_data_t render_data=animation_split_render_begin(animation,0,MatrixMultiply(rotation_matrix,transform_matrix));
        for(j=0;j<images;j++)
        {
        image_list_set_image(image_list,base_frame+j*sprites_per_image+i,renderer_get_image());
        animation_split_render_next_image(animation,&render_data);
        }

    }
}

void render_loading(image_list_t* image_list,animation_t* animation,int base_frame,int sprites_per_image,int images)
{
int i,j;
Matrix rotation_matrix=MatrixIdentity();
Matrix yaw_matrix=MatrixIdentity();
yaw_matrix.Data[0]=0;
yaw_matrix.Data[2]=-1;
yaw_matrix.Data[8]=1;
yaw_matrix.Data[10]=0;
int anim_frame;
    for(anim_frame=1;anim_frame<4;anim_frame++)
    {
        for(i=0;i<4;i++)
        {
        renderer_clear_buffers();
        render_data_t render_data=animation_split_render_begin(animation,anim_frame,rotation_matrix);
            for(j=0;j<images;j++)
            {
            image_list_set_image(image_list,base_frame+j*sprites_per_image+i,renderer_get_image());
            animation_split_render_next_image(animation,&render_data);
            }
        rotation_matrix=MatrixMultiply(rotation_matrix,yaw_matrix);
        }
    base_frame+=4;
    }
}


int count_sprites_from_flags(uint16_t sprites)
{
int count=0;
    if(sprites&SPRITE_FLAT_SLOPE)count+=32;
    if(sprites&SPRITE_GENTLE_SLOPE)count+=72;
    if(sprites&SPRITE_STEEP_SLOPE)count+=80;
    if(sprites&SPRITE_VERTICAL_SLOPE)count+=116;//These also include loop sprites by default
    if(sprites&SPRITE_DIAGONAL_SLOPE)count+=24;
    if(sprites&SPRITE_BANKING)count+=80;
    if(sprites&SPRITE_SLOPE_BANK_TRANSITION)count+=128;
    if(sprites&SPRITE_DIAGONAL_BANK_TRANSITION)count+=16;
    if(sprites&SPRITE_SLOPED_BANK_TRANSITION)count+=16;
    if(sprites&SPRITE_SLOPED_BANKED_TURN)count+=128;
    if(sprites&SPRITE_BANKED_SLOPE_TRANSITION)count+=16;
    if(sprites&SPRITE_RESTRAINT_ANIMATION)count+=12;
return count;
}

void project_render_sprites(project_t* project,object_t* object)
{
int i;
ride_header_t* header=object->ride_header;

//Render preview images
image_list_set_num_images(object->images,3);
image_list_set_image(object->images,0,image_new(112,112,20));
image_list_set_image(object->images,1,image_new(1,1,0));
image_list_set_image(object->images,2,image_new(1,1,0));

//Compute number of sprites
    for(i=0;i<NUM_CARS;i++)
    {
    uint16_t sprite_flags=header->cars[i].sprites;
    //Number of images needed for this car (car + riders)
    int car_images=((project->cars[i].animation->num_riders/2)+1);
    //Number of sprites that need to be rendered for each image
    int sprites_per_image=count_sprites_from_flags(sprite_flags);
    //Total sprites related to this car
    int total_car_sprites=sprites_per_image*car_images;
    //Compute first frame of this car's sprites
    int base_frame=object->images->num_images;
    //Allocate images for car
    image_list_set_num_images(object->images,base_frame+total_car_sprites);
    //Render images for car
    image_list_t* images=object->images;
    animation_t* animation=project->cars[i].animation;
        if(sprite_flags&SPRITE_FLAT_SLOPE)
        {
        render_rotation(images,animation,base_frame,sprites_per_image,car_images,32,FLAT,0,0);
        base_frame+=32;
        }
        if(sprite_flags&SPRITE_GENTLE_SLOPE)
        {
        render_rotation(images,animation,base_frame,sprites_per_image,car_images,4,FG_TRANSITION,0,0);
        render_rotation(images,animation,base_frame+4,sprites_per_image,car_images,4,-FG_TRANSITION,0,0);
        render_rotation(images,animation,base_frame+8,sprites_per_image,car_images,32,GENTLE,0,0);//Why the fuck does this become a 4 frame rotation with spinning enabled?
        render_rotation(images,animation,base_frame+40,sprites_per_image,car_images,32,-GENTLE,0,0);
        base_frame+=72;
        }
        if(sprite_flags&SPRITE_STEEP_SLOPE)
        {
        render_rotation(images,animation,base_frame,sprites_per_image,car_images,8,GS_TRANSITION,0,0);
        render_rotation(images,animation,base_frame+8,sprites_per_image,car_images,8,-GS_TRANSITION,0,0);
        render_rotation(images,animation,base_frame+16,sprites_per_image,car_images,32,STEEP,0,0);
        render_rotation(images,animation,base_frame+48,sprites_per_image,car_images,32,-STEEP,0,0);
        base_frame+=80;
        }
        if(sprite_flags&SPRITE_VERTICAL_SLOPE)
        {
        render_rotation(images,animation,base_frame,sprites_per_image,car_images,4,SV_TRANSITION,0,0);
        render_rotation(images,animation,base_frame+4,sprites_per_image,car_images,4,-SV_TRANSITION,0,0);
        render_rotation(images,animation,base_frame+8,sprites_per_image,car_images,32,VERTICAL,0,0);
        render_rotation(images,animation,base_frame+40,sprites_per_image,car_images,32,-VERTICAL,0,0);
        //Loop sprites
        render_rotation(images,animation,base_frame+72,sprites_per_image,car_images,4,VERTICAL+PI_12,0,0);
        render_rotation(images,animation,base_frame+76,sprites_per_image,car_images,4,-VERTICAL-PI_12,0,0);
        render_rotation(images,animation,base_frame+80,sprites_per_image,car_images,4,VERTICAL+2*PI_12,0,0);
        render_rotation(images,animation,base_frame+84,sprites_per_image,car_images,4,-VERTICAL-2*PI_12,0,0);
        render_rotation(images,animation,base_frame+88,sprites_per_image,car_images,4,VERTICAL+3*PI_12,0,0);
        render_rotation(images,animation,base_frame+92,sprites_per_image,car_images,4,-VERTICAL-3*PI_12,0,0);
        render_rotation(images,animation,base_frame+96,sprites_per_image,car_images,4,VERTICAL+4*PI_12,0,0);
        render_rotation(images,animation,base_frame+100,sprites_per_image,car_images,4,-VERTICAL-4*PI_12,0,0);
        render_rotation(images,animation,base_frame+104,sprites_per_image,car_images,4,VERTICAL+5*PI_12,0,0);
        render_rotation(images,animation,base_frame+108,sprites_per_image,car_images,4,-VERTICAL-5*PI_12,0,0);
        render_rotation(images,animation,base_frame+112,sprites_per_image,car_images,4,M_PI,0,0);
        base_frame+=116;
        }
        if(sprite_flags&SPRITE_DIAGONAL_SLOPE)
        {
        render_rotation(images,animation,base_frame,sprites_per_image,car_images,4,FG_TRANSITION_DIAGONAL,0,M_PI_4);
        render_rotation(images,animation,base_frame+4,sprites_per_image,car_images,4,-FG_TRANSITION_DIAGONAL,0,M_PI_4);
        render_rotation(images,animation,base_frame+8,sprites_per_image,car_images,4,GENTLE_DIAGONAL,0,M_PI_4);
        render_rotation(images,animation,base_frame+12,sprites_per_image,car_images,4,-GENTLE_DIAGONAL,0,M_PI_4);
        render_rotation(images,animation,base_frame+16,sprites_per_image,car_images,4,STEEP_DIAGONAL,0,M_PI_4);
        render_rotation(images,animation,base_frame+20,sprites_per_image,car_images,4,-STEEP_DIAGONAL,0,M_PI_4);
        base_frame+=24;
        }
        if(sprite_flags&SPRITE_BANKING)
        {
        render_rotation(images,animation,base_frame,sprites_per_image,car_images,8,FLAT,BANK_TRANSITION,0);
        render_rotation(images,animation,base_frame+8,sprites_per_image,car_images,8,FLAT,-BANK_TRANSITION,0);
        render_rotation(images,animation,base_frame+16,sprites_per_image,car_images,32,FLAT,BANK,0);
        render_rotation(images,animation,base_frame+48,sprites_per_image,car_images,32,FLAT,-BANK,0);
        base_frame+=80;
        }
        if(sprite_flags&SPRITE_SLOPE_BANK_TRANSITION)
        {
        render_rotation(images,animation,base_frame,sprites_per_image,car_images,32,FG_TRANSITION,BANK_TRANSITION,0);
        render_rotation(images,animation,base_frame+32,sprites_per_image,car_images,32,FG_TRANSITION,-BANK_TRANSITION,0);
        render_rotation(images,animation,base_frame+64,sprites_per_image,car_images,32,-FG_TRANSITION,BANK_TRANSITION,0);
        render_rotation(images,animation,base_frame+96,sprites_per_image,car_images,32,-FG_TRANSITION,-BANK_TRANSITION,0);
        base_frame+=128;
        }
        if(sprite_flags&SPRITE_DIAGONAL_BANK_TRANSITION)
        {
        render_rotation(images,animation,base_frame,sprites_per_image,car_images,4,GENTLE_DIAGONAL,BANK_TRANSITION,M_PI_4);
        render_rotation(images,animation,base_frame+4,sprites_per_image,car_images,4,GENTLE_DIAGONAL,-BANK_TRANSITION,M_PI_4);
        render_rotation(images,animation,base_frame+8,sprites_per_image,car_images,4,-GENTLE_DIAGONAL,BANK_TRANSITION,M_PI_4);
        render_rotation(images,animation,base_frame+12,sprites_per_image,car_images,4,-GENTLE_DIAGONAL,-BANK_TRANSITION,M_PI_4);
        base_frame+=16;
        }
        if(sprite_flags&SPRITE_SLOPED_BANK_TRANSITION)
        {
        render_rotation(images,animation,base_frame,sprites_per_image,car_images,4,GENTLE,BANK_TRANSITION,0);
        render_rotation(images,animation,base_frame+4,sprites_per_image,car_images,4,GENTLE,-BANK_TRANSITION,0);
        render_rotation(images,animation,base_frame+8,sprites_per_image,car_images,4,-GENTLE,BANK_TRANSITION,0);
        render_rotation(images,animation,base_frame+12,sprites_per_image,car_images,4,-GENTLE,-BANK_TRANSITION,0);
        base_frame+=16;
        }
        if(sprite_flags&SPRITE_SLOPED_BANKED_TURN)
        {
        render_rotation(images,animation,base_frame,sprites_per_image,car_images,32,GENTLE,BANK,0);
        render_rotation(images,animation,base_frame+32,sprites_per_image,car_images,32,GENTLE,-BANK,0);
        render_rotation(images,animation,base_frame+64,sprites_per_image,car_images,32,-GENTLE,BANK,0);
        render_rotation(images,animation,base_frame+96,sprites_per_image,car_images,32,-GENTLE,-BANK,0);
        base_frame+=128;
        }
        if(sprite_flags&SPRITE_BANKED_SLOPE_TRANSITION)
        {
        render_rotation(images,animation,base_frame,sprites_per_image,car_images,4,FG_TRANSITION,BANK,0);
        render_rotation(images,animation,base_frame+4,sprites_per_image,car_images,4,FG_TRANSITION,-BANK,0);
        render_rotation(images,animation,base_frame+8,sprites_per_image,car_images,4,-FG_TRANSITION,BANK,0);
        render_rotation(images,animation,base_frame+12,sprites_per_image,car_images,4,-FG_TRANSITION,-BANK,0);
        base_frame+=16;
        }
        if(sprite_flags&SPRITE_RESTRAINT_ANIMATION)
        {
        render_loading(images,animation,base_frame,sprites_per_image,car_images);
        base_frame+=12;
        }
    }
}

object_t* project_export_dat(project_t* project)
{
int i;
object_t* object=object_new_ride();

object->ride_header->track_style=project->track_type;

//Set strings
char capacity[256];
sprintf(capacity,"%d passengers per car",project->cars[project->car_types[CAR_INDEX_DEFAULT]].animation->num_riders);
string_table_set_string_by_language(object->string_tables[STRING_TABLE_NAME],LANGUAGE_ENGLISH_UK,project->name);
string_table_set_string_by_language(object->string_tables[STRING_TABLE_DESCRIPTION],LANGUAGE_ENGLISH_UK,project->description);
string_table_set_string_by_language(object->string_tables[STRING_TABLE_CAPACITY],LANGUAGE_ENGLISH_UK,capacity);
//Set car types

object->ride_header->car_types[CAR_INDEX_DEFAULT]=project->car_types[CAR_INDEX_DEFAULT];
object->ride_header->car_types[CAR_INDEX_FRONT]=project->car_types[CAR_INDEX_FRONT];
object->ride_header->car_types[CAR_INDEX_SECOND]=project->car_types[CAR_INDEX_SECOND];
object->ride_header->car_types[CAR_INDEX_THIRD]=project->car_types[CAR_INDEX_THIRD];
object->ride_header->car_types[CAR_INDEX_REAR]=project->car_types[CAR_INDEX_REAR];

object->ride_header->flags=project->flags|RIDE_ENABLE_OR_ELSE;

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
        object->ride_header->cars[i].riders=project->cars[i].animation->num_riders;
        object->ride_header->cars[i].rider_pairs=0;
        object->ride_header->cars[i].rider_sprites=project->cars[i].animation->num_riders/2;
        }
    }
project_render_sprites(project,object);
return object;
}
