#include<stdlib.h>
#include<string.h>
#include<math.h>
#include "project.h"

project_t* project_new()
{
int i;
project_t* project=malloc(sizeof(project_t));
project->name=malloc(13);
project->description=malloc(15);
strcpy(project->name,"Unnamed ride");
strcpy(project->description,"No description");
project->num_color_schemes=0;
    for(i=0;i<MAX_COLOR_SCHEMES;i++)
    {
    project->color_schemes[i].colors[0]=0;
    project->color_schemes[i].colors[1]=0;
    project->color_schemes[i].colors[2]=0;
    }
project->track_type=0x33;//Default to B&M track
project->flags=RIDE_SEPERATE;
project->minimum_cars=3;
project->maximum_cars=8;
project->zero_cars=0;
project->car_icon_index=0;
project->excitement=0;
project->intensity=0;
project->nausea=0;
project->max_height=0;
memset(project->car_types,0xFF,5);
project->car_types[CAR_INDEX_DEFAULT]=0;
project->models=NULL;
project->num_models=0;
project->preview_image=image_new(112,112,0);
project->id=rand();

    for(i=0;i<NUM_CARS;i++)
    {
    project->cars[i].animation=animation_new();
    project->cars[i].flags=0;
    project->cars[i].sprites=SPRITE_GENTLE_SLOPE|SPRITE_STEEP_SLOPE|SPRITE_BANKING|SPRITE_SLOPED_BANKED_TURN|SPRITE_DIAGONAL_SLOPE;
    project->cars[i].spacing=(32*8192)/3;
    project->cars[i].running_sound=RUNNING_SOUND_NONE;
    project->cars[i].secondary_sound=SECONDARY_SOUND_NONE;
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
void project_set_preview(project_t* project,image_t* image)
{
image_free(project->preview_image);
project->preview_image=image;
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


void render_rotation(image_list_t* image_list,animation_t* animation,uint32_t flags,int base_frame,int sprites_per_image,int images,int num_frames,double pitch,double roll,double yaw)
{
Matrix transform_matrix=MatrixFromEulerAngles(VectorFromComponents(-pitch,-yaw,-roll));
float variables[ANIMATION_NUM_VARIABLES]={0,0,0,0,0,0,0};
variables[VAR_PITCH]=-pitch;
variables[VAR_YAW]=-yaw;
variables[VAR_ROLL]=-roll;



int sprites_per_view=(flags&CAR_IS_SWINGING)?13:1;

double rotation=0;
double step=2*3.141592654/num_frames;
    for(int view=0;view<num_frames;view++)
    {
    Matrix rotation_matrix=MatrixIdentity();
    rotation_matrix.Data[0]=cos(rotation);
    rotation_matrix.Data[2]=-sin(rotation);
    rotation_matrix.Data[8]=sin(rotation);
    rotation_matrix.Data[10]=cos(rotation);

        for(int frame=0;frame<sprites_per_view;frame++)
        {
        renderer_clear_buffers();
        render_data_t render_data=animation_split_render_begin(animation,MatrixMultiply(rotation_matrix,transform_matrix),variables);
            for(int image=0;image<images;image++)
            {
            image_list_set_image(image_list,base_frame+image*sprites_per_image+view*sprites_per_view+frame,renderer_get_image());
            animation_split_render_next_image(animation,&render_data);
            }
        }

    rotation+=step;
    variables[VAR_YAW]+=step;
    }
}
void render_loading(image_list_t* image_list,animation_t* animation,uint32_t flags,int base_frame,int sprites_per_image,int images)
{
Matrix rotation_matrix=MatrixIdentity();
Matrix yaw_matrix=MatrixIdentity();
yaw_matrix.Data[0]=0;
yaw_matrix.Data[2]=-1;
yaw_matrix.Data[8]=1;
yaw_matrix.Data[10]=0;

float variables[ANIMATION_NUM_VARIABLES]={0,0,0,0,0,0,0};

    for(int anim_frame=0;anim_frame<3;anim_frame++)
    {
    variables[VAR_RESTRAINT]+=0.25;
    variables[VAR_YAW]=0;
        for(int i=0;i<4;i++)
        {
        renderer_clear_buffers();
        render_data_t render_data=animation_split_render_begin(animation,rotation_matrix,variables);
            for(int j=0;j<images;j++)
            {
            image_list_set_image(image_list,base_frame+j*sprites_per_image+i,renderer_get_image());
            animation_split_render_next_image(animation,&render_data);
            }
        rotation_matrix=MatrixMultiply(rotation_matrix,yaw_matrix);
        variables[VAR_YAW]+=M_PI_2;
        }
    base_frame+=4;
    }
}


int car_settings_count_required_animation_frames(car_settings_t* car)
{
int frames=1;
    if(car->flags&CAR_FAKE_SPINNING)frames+=31;
    if(car->sprites&SPRITE_RESTRAINT_ANIMATION)frames+=3;
return frames;
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
    if(sprites&SPRITE_INLINE_TWIST)count+=40;
    if(sprites&SPRITE_SLOPE_BANK_TRANSITION)count+=128;
    if(sprites&SPRITE_DIAGONAL_BANK_TRANSITION)count+=16;
    if(sprites&SPRITE_SLOPED_BANK_TRANSITION)count+=16;
    if(sprites&SPRITE_SLOPED_BANKED_TURN)count+=128;
    if(sprites&SPRITE_BANKED_SLOPE_TRANSITION)count+=16;
    if(sprites&SPRITE_CORKSCREW)count+=80;
    if(sprites&SPRITE_RESTRAINT_ANIMATION)count+=12;
return count;
}
void project_render_sprites(project_t* project,object_t* object)
{
int i;
ride_header_t* header=object->ride_header;

//Render preview images
image_list_set_num_images(object->images,3);
image_list_set_image(object->images,0,project->preview_image);
image_list_set_image(object->images,1,image_new(1,1,0));
image_list_set_image(object->images,2,image_new(1,1,0));

//Compute number of sprites
    for(i=0;i<NUM_CARS;i++)
    {
    //Set flags
    uint32_t flags=project->cars[i].flags;
    uint16_t sprite_flags=header->cars[i].sprites;
    //Number of images needed for this car (car + riders)
    int car_num_riders=animation_count_riders(project->cars[i].animation);
    int car_images=(car_num_riders!=1?car_num_riders/2:1)+1;
    //Number of angles that need to be rendered for each image
    int views_per_image=count_sprites_from_flags(sprite_flags);
    //Number of sprites that must be rendered for each angle of an image
    int sprites_per_view=(flags&CAR_IS_SWINGING)?13:1;
    //Total sprites for each image
    int sprites_per_image=sprites_per_view*views_per_image;
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
        render_rotation(images,animation,flags,base_frame,sprites_per_image,car_images,32,FLAT,0,0);
        base_frame+=32*sprites_per_view;
        }
        if(sprite_flags&SPRITE_GENTLE_SLOPE)
        {
        render_rotation(images,animation,flags,base_frame,sprites_per_image,car_images,4,FG_TRANSITION,0,0);
        base_frame+=4*sprites_per_view;
        render_rotation(images,animation,flags,base_frame,sprites_per_image,car_images,4,-FG_TRANSITION,0,0);
        base_frame+=4*sprites_per_view;
        render_rotation(images,animation,flags,base_frame,sprites_per_image,car_images,32,GENTLE,0,0);//Why the fuck does this become a 4 frame rotation with spinning enabled?
        base_frame+=32*sprites_per_view;
        render_rotation(images,animation,flags,base_frame,sprites_per_image,car_images,32,-GENTLE,0,0);
        base_frame+=32*sprites_per_view;
        }
        if(sprite_flags&SPRITE_STEEP_SLOPE)
        {
        render_rotation(images,animation,flags,base_frame,sprites_per_image,car_images,8,GS_TRANSITION,0,0);
        base_frame+=8*sprites_per_view;
        render_rotation(images,animation,flags,base_frame,sprites_per_image,car_images,8,-GS_TRANSITION,0,0);
        base_frame+=8*sprites_per_view;
        render_rotation(images,animation,flags,base_frame,sprites_per_image,car_images,32,STEEP,0,0);
        base_frame+=32*sprites_per_view;
        render_rotation(images,animation,flags,base_frame,sprites_per_image,car_images,32,-STEEP,0,0);
        base_frame+=32*sprites_per_view;
        }
        if(sprite_flags&SPRITE_VERTICAL_SLOPE)
        {
        render_rotation(images,animation,flags,base_frame,sprites_per_image,car_images,4,SV_TRANSITION,0,0);
        base_frame+=4*sprites_per_view;
        render_rotation(images,animation,flags,base_frame,sprites_per_image,car_images,4,-SV_TRANSITION,0,0);
        base_frame+=4*sprites_per_view;
        render_rotation(images,animation,flags,base_frame,sprites_per_image,car_images,32,VERTICAL,0,0);
        base_frame+=32*sprites_per_view;
        render_rotation(images,animation,flags,base_frame,sprites_per_image,car_images,32,-VERTICAL,0,0);
        base_frame+=32*sprites_per_view;
        //Loop sprites
        render_rotation(images,animation,flags,base_frame,sprites_per_image,car_images,4,VERTICAL+M_PI_12,0,0);
        base_frame+=4*sprites_per_view;
        render_rotation(images,animation,flags,base_frame,sprites_per_image,car_images,4,-VERTICAL-M_PI_12,0,0);
        base_frame+=4*sprites_per_view;
        render_rotation(images,animation,flags,base_frame,sprites_per_image,car_images,4,VERTICAL+2*M_PI_12,0,0);
        base_frame+=4*sprites_per_view;
        render_rotation(images,animation,flags,base_frame,sprites_per_image,car_images,4,-VERTICAL-2*M_PI_12,0,0);
        base_frame+=4*sprites_per_view;
        render_rotation(images,animation,flags,base_frame,sprites_per_image,car_images,4,VERTICAL+3*M_PI_12,0,0);
        base_frame+=4*sprites_per_view;
        render_rotation(images,animation,flags,base_frame,sprites_per_image,car_images,4,-VERTICAL-3*M_PI_12,0,0);
        base_frame+=4*sprites_per_view;
        render_rotation(images,animation,flags,base_frame,sprites_per_image,car_images,4,VERTICAL+4*M_PI_12,0,0);
        base_frame+=4*sprites_per_view;
        render_rotation(images,animation,flags,base_frame,sprites_per_image,car_images,4,-VERTICAL-4*M_PI_12,0,0);
        base_frame+=4*sprites_per_view;
        render_rotation(images,animation,flags,base_frame,sprites_per_image,car_images,4,VERTICAL+5*M_PI_12,0,0);
        base_frame+=4*sprites_per_view;
        render_rotation(images,animation,flags,base_frame,sprites_per_image,car_images,4,-VERTICAL-5*M_PI_12,0,0);
        base_frame+=4*sprites_per_view;
        render_rotation(images,animation,flags,base_frame,sprites_per_image,car_images,4,M_PI,0,0);
        base_frame+=4*sprites_per_view;
        }
        if(sprite_flags&SPRITE_DIAGONAL_SLOPE)
        {
        render_rotation(images,animation,flags,base_frame,sprites_per_image,car_images,4,FG_TRANSITION_DIAGONAL,0,M_PI_4);
        base_frame+=4*sprites_per_view;
        render_rotation(images,animation,flags,base_frame,sprites_per_image,car_images,4,-FG_TRANSITION_DIAGONAL,0,M_PI_4);
        base_frame+=4*sprites_per_view;
        render_rotation(images,animation,flags,base_frame,sprites_per_image,car_images,4,GENTLE_DIAGONAL,0,M_PI_4);
        base_frame+=4*sprites_per_view;
        render_rotation(images,animation,flags,base_frame,sprites_per_image,car_images,4,-GENTLE_DIAGONAL,0,M_PI_4);
        base_frame+=4*sprites_per_view;
        render_rotation(images,animation,flags,base_frame,sprites_per_image,car_images,4,STEEP_DIAGONAL,0,M_PI_4);
        base_frame+=4*sprites_per_view;
        render_rotation(images,animation,flags,base_frame,sprites_per_image,car_images,4,-STEEP_DIAGONAL,0,M_PI_4);
        base_frame+=4*sprites_per_view;
        }
        if(sprite_flags&SPRITE_BANKING)
        {
        render_rotation(images,animation,flags,base_frame,sprites_per_image,car_images,8,FLAT,BANK_TRANSITION,0);
        base_frame+=8*sprites_per_view;
        render_rotation(images,animation,flags,base_frame,sprites_per_image,car_images,8,FLAT,-BANK_TRANSITION,0);
        base_frame+=8*sprites_per_view;
        render_rotation(images,animation,flags,base_frame,sprites_per_image,car_images,32,FLAT,BANK,0);
        base_frame+=32*sprites_per_view;
        render_rotation(images,animation,flags,base_frame,sprites_per_image,car_images,32,FLAT,-BANK,0);
        base_frame+=32*sprites_per_view;
        }
        if(sprite_flags&SPRITE_INLINE_TWIST)
        {
        render_rotation(images,animation,flags,base_frame,sprites_per_image,car_images,4,FLAT,3.0*M_PI_8,0);
        base_frame+=4*sprites_per_view;
        render_rotation(images,animation,flags,base_frame,sprites_per_image,car_images,4,FLAT,-3.0*M_PI_8,0);
        base_frame+=4*sprites_per_view;
        render_rotation(images,animation,flags,base_frame,sprites_per_image,car_images,4,FLAT,M_PI_2,0);
        base_frame+=4*sprites_per_view;
        render_rotation(images,animation,flags,base_frame,sprites_per_image,car_images,4,FLAT,-M_PI_2,0);
        base_frame+=4*sprites_per_view;
        render_rotation(images,animation,flags,base_frame,sprites_per_image,car_images,4,FLAT,5.0*M_PI_8,0);
        base_frame+=4*sprites_per_view;
        render_rotation(images,animation,flags,base_frame,sprites_per_image,car_images,4,FLAT,-5.0*M_PI_8,0);
        base_frame+=4*sprites_per_view;
        render_rotation(images,animation,flags,base_frame,sprites_per_image,car_images,4,FLAT,3.0*M_PI_4,0);
        base_frame+=4*sprites_per_view;
        render_rotation(images,animation,flags,base_frame,sprites_per_image,car_images,4,FLAT,-3.0*M_PI_4,0);
        base_frame+=4*sprites_per_view;
        render_rotation(images,animation,flags,base_frame,sprites_per_image,car_images,4,FLAT,7.0*M_PI_8,0);
        base_frame+=4*sprites_per_view;
        render_rotation(images,animation,flags,base_frame,sprites_per_image,car_images,4,FLAT,-7.0*M_PI_8,0);
        base_frame+=4*sprites_per_view;
        }
        if(sprite_flags&SPRITE_SLOPE_BANK_TRANSITION)
        {
        render_rotation(images,animation,flags,base_frame,sprites_per_image,car_images,32,FG_TRANSITION,BANK_TRANSITION,0);
        base_frame+=32*sprites_per_view;
        render_rotation(images,animation,flags,base_frame,sprites_per_image,car_images,32,FG_TRANSITION,-BANK_TRANSITION,0);
        base_frame+=32*sprites_per_view;
        render_rotation(images,animation,flags,base_frame,sprites_per_image,car_images,32,-FG_TRANSITION,BANK_TRANSITION,0);
        base_frame+=32*sprites_per_view;
        render_rotation(images,animation,flags,base_frame,sprites_per_image,car_images,32,-FG_TRANSITION,-BANK_TRANSITION,0);
        base_frame+=32*sprites_per_view;
        }
        if(sprite_flags&SPRITE_DIAGONAL_BANK_TRANSITION)
        {
        render_rotation(images,animation,flags,base_frame,sprites_per_image,car_images,4,GENTLE_DIAGONAL,BANK_TRANSITION,M_PI_4);
        base_frame+=4*sprites_per_view;
        render_rotation(images,animation,flags,base_frame,sprites_per_image,car_images,4,GENTLE_DIAGONAL,-BANK_TRANSITION,M_PI_4);
        base_frame+=4*sprites_per_view;
        render_rotation(images,animation,flags,base_frame,sprites_per_image,car_images,4,-GENTLE_DIAGONAL,BANK_TRANSITION,M_PI_4);
        base_frame+=4*sprites_per_view;
        render_rotation(images,animation,flags,base_frame,sprites_per_image,car_images,4,-GENTLE_DIAGONAL,-BANK_TRANSITION,M_PI_4);
        base_frame+=4*sprites_per_view;
        }
        if(sprite_flags&SPRITE_SLOPED_BANK_TRANSITION)
        {
        render_rotation(images,animation,flags,base_frame,sprites_per_image,car_images,4,GENTLE,BANK_TRANSITION,0);
        base_frame+=4*sprites_per_view;
        render_rotation(images,animation,flags,base_frame,sprites_per_image,car_images,4,GENTLE,-BANK_TRANSITION,0);
        base_frame+=4*sprites_per_view;
        render_rotation(images,animation,flags,base_frame,sprites_per_image,car_images,4,-GENTLE,BANK_TRANSITION,0);
        base_frame+=4*sprites_per_view;
        render_rotation(images,animation,flags,base_frame,sprites_per_image,car_images,4,-GENTLE,-BANK_TRANSITION,0);
        base_frame+=4*sprites_per_view;
        }
        if(sprite_flags&SPRITE_SLOPED_BANKED_TURN)
        {
        render_rotation(images,animation,flags,base_frame,sprites_per_image,car_images,32,GENTLE,BANK,0);
        base_frame+=32*sprites_per_view;
        render_rotation(images,animation,flags,base_frame,sprites_per_image,car_images,32,GENTLE,-BANK,0);
        base_frame+=32*sprites_per_view;
        render_rotation(images,animation,flags,base_frame,sprites_per_image,car_images,32,-GENTLE,BANK,0);
        base_frame+=32*sprites_per_view;
        render_rotation(images,animation,flags,base_frame,sprites_per_image,car_images,32,-GENTLE,-BANK,0);
        base_frame+=32*sprites_per_view;
        }
        if(sprite_flags&SPRITE_BANKED_SLOPE_TRANSITION)
        {
        render_rotation(images,animation,flags,base_frame,sprites_per_image,car_images,4,FG_TRANSITION,BANK,0);
        base_frame+=4*sprites_per_view;
        render_rotation(images,animation,flags,base_frame,sprites_per_image,car_images,4,FG_TRANSITION,-BANK,0);
        base_frame+=4*sprites_per_view;
        render_rotation(images,animation,flags,base_frame,sprites_per_image,car_images,4,-FG_TRANSITION,BANK,0);
        base_frame+=4*sprites_per_view;
        render_rotation(images,animation,flags,base_frame,sprites_per_image,car_images,4,-FG_TRANSITION,-BANK,0);
        base_frame+=4*sprites_per_view;
        }
        if(sprite_flags&SPRITE_CORKSCREW)
        {
        #define CORKSCREW_ANGLE_1 2.0*M_PI_12
        #define CORKSCREW_ANGLE_2 4.0*M_PI_12
        #define CORKSCREW_ANGLE_3 M_PI_2
        #define CORKSCREW_ANGLE_4 8.0*M_PI_12
        #define CORKSCREW_ANGLE_5 10.0*M_PI_12

        //Corkscrew right
        render_rotation(images,animation,flags,base_frame,sprites_per_image,car_images,4,CORKSCREW_RIGHT_PITCH(CORKSCREW_ANGLE_1),CORKSCREW_RIGHT_ROLL(CORKSCREW_ANGLE_1),CORKSCREW_RIGHT_YAW(CORKSCREW_ANGLE_1));
        base_frame+=4*sprites_per_view;
        render_rotation(images,animation,flags,base_frame,sprites_per_image,car_images,4,CORKSCREW_RIGHT_PITCH(CORKSCREW_ANGLE_2),CORKSCREW_RIGHT_ROLL(CORKSCREW_ANGLE_2),CORKSCREW_RIGHT_YAW(CORKSCREW_ANGLE_2));
        base_frame+=4*sprites_per_view;
        render_rotation(images,animation,flags,base_frame,sprites_per_image,car_images,4,CORKSCREW_RIGHT_PITCH(CORKSCREW_ANGLE_3),CORKSCREW_RIGHT_ROLL(CORKSCREW_ANGLE_3),CORKSCREW_RIGHT_YAW(CORKSCREW_ANGLE_3));
        base_frame+=4*sprites_per_view;
        render_rotation(images,animation,flags,base_frame,sprites_per_image,car_images,4,CORKSCREW_RIGHT_PITCH(CORKSCREW_ANGLE_4),CORKSCREW_RIGHT_ROLL(CORKSCREW_ANGLE_4),CORKSCREW_RIGHT_YAW(CORKSCREW_ANGLE_4));
        base_frame+=4*sprites_per_view;
        render_rotation(images,animation,flags,base_frame,sprites_per_image,car_images,4,CORKSCREW_RIGHT_PITCH(CORKSCREW_ANGLE_5),CORKSCREW_RIGHT_ROLL(CORKSCREW_ANGLE_5),CORKSCREW_RIGHT_YAW(CORKSCREW_ANGLE_5));
        base_frame+=4*sprites_per_view;

        render_rotation(images,animation,flags,base_frame,sprites_per_image,car_images,4,CORKSCREW_RIGHT_PITCH(-CORKSCREW_ANGLE_1),CORKSCREW_RIGHT_ROLL(-CORKSCREW_ANGLE_1),CORKSCREW_RIGHT_YAW(-CORKSCREW_ANGLE_1));
        base_frame+=4*sprites_per_view;
        render_rotation(images,animation,flags,base_frame,sprites_per_image,car_images,4,CORKSCREW_RIGHT_PITCH(-CORKSCREW_ANGLE_2),CORKSCREW_RIGHT_ROLL(-CORKSCREW_ANGLE_2),CORKSCREW_RIGHT_YAW(-CORKSCREW_ANGLE_2));
        base_frame+=4*sprites_per_view;
        render_rotation(images,animation,flags,base_frame,sprites_per_image,car_images,4,CORKSCREW_RIGHT_PITCH(-CORKSCREW_ANGLE_3),CORKSCREW_RIGHT_ROLL(-CORKSCREW_ANGLE_3),CORKSCREW_RIGHT_YAW(-CORKSCREW_ANGLE_3));
        base_frame+=4*sprites_per_view;
        render_rotation(images,animation,flags,base_frame,sprites_per_image,car_images,4,CORKSCREW_RIGHT_PITCH(-CORKSCREW_ANGLE_4),CORKSCREW_RIGHT_ROLL(-CORKSCREW_ANGLE_4),CORKSCREW_RIGHT_YAW(-CORKSCREW_ANGLE_4));
        base_frame+=4*sprites_per_view;
        render_rotation(images,animation,flags,base_frame,sprites_per_image,car_images,4,CORKSCREW_RIGHT_PITCH(-CORKSCREW_ANGLE_5),CORKSCREW_RIGHT_ROLL(-CORKSCREW_ANGLE_5),CORKSCREW_RIGHT_YAW(-CORKSCREW_ANGLE_5));
        base_frame+=4*sprites_per_view;

        //Half corkscrew left
        render_rotation(images,animation,flags,base_frame,sprites_per_image,car_images,4,CORKSCREW_LEFT_PITCH(CORKSCREW_ANGLE_1),CORKSCREW_LEFT_ROLL(CORKSCREW_ANGLE_1),CORKSCREW_LEFT_YAW(CORKSCREW_ANGLE_1));
        base_frame+=4*sprites_per_view;
        render_rotation(images,animation,flags,base_frame,sprites_per_image,car_images,4,CORKSCREW_LEFT_PITCH(CORKSCREW_ANGLE_2),CORKSCREW_LEFT_ROLL(CORKSCREW_ANGLE_2),CORKSCREW_LEFT_YAW(CORKSCREW_ANGLE_2));
        base_frame+=4*sprites_per_view;
        render_rotation(images,animation,flags,base_frame,sprites_per_image,car_images,4,CORKSCREW_LEFT_PITCH(CORKSCREW_ANGLE_3),CORKSCREW_LEFT_ROLL(CORKSCREW_ANGLE_3),CORKSCREW_LEFT_YAW(CORKSCREW_ANGLE_3));
        base_frame+=4*sprites_per_view;
        render_rotation(images,animation,flags,base_frame,sprites_per_image,car_images,4,CORKSCREW_LEFT_PITCH(CORKSCREW_ANGLE_4),CORKSCREW_LEFT_ROLL(CORKSCREW_ANGLE_4),CORKSCREW_LEFT_YAW(CORKSCREW_ANGLE_4));
        base_frame+=4*sprites_per_view;
        render_rotation(images,animation,flags,base_frame,sprites_per_image,car_images,4,CORKSCREW_LEFT_PITCH(CORKSCREW_ANGLE_5),CORKSCREW_LEFT_ROLL(CORKSCREW_ANGLE_5),CORKSCREW_LEFT_YAW(CORKSCREW_ANGLE_5));
        base_frame+=4*sprites_per_view;

        render_rotation(images,animation,flags,base_frame,sprites_per_image,car_images,4,CORKSCREW_LEFT_PITCH(-CORKSCREW_ANGLE_1),CORKSCREW_LEFT_ROLL(-CORKSCREW_ANGLE_1),CORKSCREW_LEFT_YAW(-CORKSCREW_ANGLE_1));
        base_frame+=4*sprites_per_view;
        render_rotation(images,animation,flags,base_frame,sprites_per_image,car_images,4,CORKSCREW_LEFT_PITCH(-CORKSCREW_ANGLE_2),CORKSCREW_LEFT_ROLL(-CORKSCREW_ANGLE_2),CORKSCREW_LEFT_YAW(-CORKSCREW_ANGLE_2));
        base_frame+=4*sprites_per_view;
        render_rotation(images,animation,flags,base_frame,sprites_per_image,car_images,4,CORKSCREW_LEFT_PITCH(-CORKSCREW_ANGLE_3),CORKSCREW_LEFT_ROLL(-CORKSCREW_ANGLE_3),CORKSCREW_LEFT_YAW(-CORKSCREW_ANGLE_3));
        base_frame+=4*sprites_per_view;
        render_rotation(images,animation,flags,base_frame,sprites_per_image,car_images,4,CORKSCREW_LEFT_PITCH(-CORKSCREW_ANGLE_4),CORKSCREW_LEFT_ROLL(-CORKSCREW_ANGLE_4),CORKSCREW_LEFT_YAW(-CORKSCREW_ANGLE_4));
        base_frame+=4*sprites_per_view;
        render_rotation(images,animation,flags,base_frame,sprites_per_image,car_images,4,CORKSCREW_LEFT_PITCH(-CORKSCREW_ANGLE_5),CORKSCREW_LEFT_ROLL(-CORKSCREW_ANGLE_5),CORKSCREW_LEFT_YAW(-CORKSCREW_ANGLE_5));
        base_frame+=4*sprites_per_view;
        }
        if(sprite_flags&SPRITE_RESTRAINT_ANIMATION)
        {
        render_loading(images,animation,flags,base_frame,sprites_per_image,car_images);
        base_frame+=12*sprites_per_view;
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
sprintf(capacity,"%d passengers per car",animation_count_riders(project->cars[project->car_types[CAR_INDEX_DEFAULT]].animation));
string_table_set_string_by_language(object->string_tables[STRING_TABLE_NAME],LANGUAGE_ENGLISH_UK,project->name);
string_table_set_string_by_language(object->string_tables[STRING_TABLE_NAME],LANGUAGE_ENGLISH_US,project->name);
string_table_set_string_by_language(object->string_tables[STRING_TABLE_DESCRIPTION],LANGUAGE_ENGLISH_UK,project->description);
string_table_set_string_by_language(object->string_tables[STRING_TABLE_DESCRIPTION],LANGUAGE_ENGLISH_US,project->description);
string_table_set_string_by_language(object->string_tables[STRING_TABLE_CAPACITY],LANGUAGE_ENGLISH_UK,capacity);
string_table_set_string_by_language(object->string_tables[STRING_TABLE_CAPACITY],LANGUAGE_ENGLISH_US,capacity);

//Set color schemes
ride_structures_set_num_default_colors(object->optional,project->num_color_schemes);
printf("Num color schemes: %d\n",object->optional->num_default_colors);
    for(i=0;i<project->num_color_schemes;i++)
    {
    object->optional->default_colors[i]=project->color_schemes[i];
    }

//Set car types
object->ride_header->car_types[CAR_INDEX_DEFAULT]=project->car_types[CAR_INDEX_DEFAULT];
object->ride_header->car_types[CAR_INDEX_FRONT]=project->car_types[CAR_INDEX_FRONT];
object->ride_header->car_types[CAR_INDEX_SECOND]=project->car_types[CAR_INDEX_SECOND];
object->ride_header->car_types[CAR_INDEX_THIRD]=project->car_types[CAR_INDEX_THIRD];
object->ride_header->car_types[CAR_INDEX_REAR]=project->car_types[CAR_INDEX_REAR];

object->ride_header->flags=project->flags|RIDE_ENABLE_OR_ELSE;

//Set categories
object->ride_header->categories[0]=CATEGORY_ROLLERCOASTER;
object->ride_header->categories[1]=(project->flags&RIDE_WET)?CATEGORY_WATER_RIDE:0xFF;
object->ride_header->track_sections=0xFFFFFFFFFFFFFFFFl;

object->ride_header->minimum_cars=project->minimum_cars;
object->ride_header->maximum_cars=project->maximum_cars;

object->ride_header->zero_cars=project->zero_cars;

object->ride_header->car_icon_index=project->car_icon_index;

object->ride_header->excitement=project->excitement;
object->ride_header->intensity=project->intensity;
object->ride_header->nausea=project->nausea;
object->ride_header->max_height=project->max_height;

unsigned char cars_used[NUM_CARS];
memset(cars_used,0,NUM_CARS);
    for(i=0;i<5;i++)if(project->car_types[i]!=0xFF)cars_used[project->car_types[i]]=1;
    for(i=0;i<NUM_CARS;i++)
    {
        if(cars_used[i])
        {
        //printf("%d %d %d %d %d\n",object->ride_header->cars[i].unknown[0],object->ride_header->cars[i].unknown[1],object->ride_header->cars[i].unknown[2],object->ride_header->cars[i].unknown[3],object->ride_header->cars[i].unknown[4]);
        object->ride_header->cars[i].highest_rotation_index=31;
        object->ride_header->cars[i].flags=CAR_ENABLE_ROLLING_SOUND|project->cars[i].flags;
        //Enable all extra swinging frames

            if(project->cars[i].flags&CAR_IS_SWINGING)
            {
            object->ride_header->cars[i].flags|=0x20000000;
            object->ride_header->cars[i].extra_swing_frames=0x08;
            }
        object->ride_header->cars[i].friction=project->cars[i].friction;
        object->ride_header->cars[i].spacing=project->cars[i].spacing;
        object->ride_header->cars[i].running_sound=project->cars[i].running_sound;
        object->ride_header->cars[i].secondary_sound=project->cars[i].secondary_sound;
        object->ride_header->cars[i].z_value=project->cars[i].z_value;
        //Some sprites ought be included if others are. Here we compute the minimum set of sprite flags needed that includes all the selected flags
        object->ride_header->cars[i].sprites=project->cars[i].sprites|SPRITE_FLAT_SLOPE;
            if(project->cars[i].sprites&SPRITE_VERTICAL_SLOPE)object->ride_header->cars[i].sprites|=SPRITE_STEEP_SLOPE;
            if(project->cars[i].sprites&SPRITE_STEEP_SLOPE)object->ride_header->cars[i].sprites|=SPRITE_GENTLE_SLOPE;
                if(project->cars[i].sprites&SPRITE_BANKING)
                {
                    if(project->cars[i].sprites&SPRITE_DIAGONAL_SLOPE)object->ride_header->cars[i].sprites|=SPRITE_DIAGONAL_BANK_TRANSITION|SPRITE_SLOPED_BANK_TRANSITION;
                    if(project->cars[i].sprites&SPRITE_GENTLE_SLOPE)object->ride_header->cars[i].sprites|=SPRITE_BANKED_SLOPE_TRANSITION|SPRITE_SLOPE_BANK_TRANSITION|SPRITE_SLOPED_BANK_TRANSITION|SPRITE_SLOPED_BANKED_TURN;
                }
                if(project->cars[i].sprites&SPRITE_INLINE_TWIST)
                {
                object->ride_header->cars[i].sprites|=SPRITE_BANKING;
                object->ride_header->cars[i].sprites|=SPRITE_VERTICAL_SLOPE;
                }


        animation_t* animation=project->cars[i].animation;
        int num_riders=animation_count_riders(animation);
        object->ride_header->cars[i].riders=num_riders;
        object->ride_header->cars[i].rider_pairs=num_riders!=1?0x80:0;
        object->ride_header->cars[i].rider_sprites=num_riders!=1?(num_riders/2):1;

        ride_structures_set_num_peep_positions(object->optional,i,num_riders);
        int rider=0;
        int model=0;
        float variables[ANIMATION_NUM_VARIABLES]={0,0,0,0,0,0,0};
        variables[VAR_RESTRAINT]=1;

            while(rider<num_riders&&model<animation->num_objects)
            {
                if(animation->objects[model]->model->is_rider)
                {
                int8_t peep_position=(int8_t)(-animation_expression_evaluate(animation->objects[model]->position[2],variables)*(32.0/3.0));
                object->optional->peep_positions[i].positions[rider]=*((uint8_t*)(&peep_position));
                rider++;
                }
            model++;
            }

        }
    }
project_render_sprites(project,object);
return object;
}

