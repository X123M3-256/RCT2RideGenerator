#ifndef BACKEND_H_INCLUDED
#define BACKEND_H_INCLUDED
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include "image.h"

#define NUM_STRING_TABLES 3
#define NUM_LANGUAGES 11
#define NUM_CARS 4


#define M_PI 3.1415926535
#define M_PI_2 (3.1415926535/2.0)
#define M_PI_4 (3.1415926535/4.0)
#define M_SQRT1_2 (1/sqrt(2.0))

#define TILE_SLOPE (1/sqrt(6))

#define FLAT 0

#define GENTLE (atan(TILE_SLOPE))
#define STEEP (atan(4*TILE_SLOPE))
#define VERTICAL M_PI_2
#define FG_TRANSITION ((FLAT+GENTLE)/2)
#define GS_TRANSITION ((GENTLE+STEEP)/2)
#define SV_TRANSITION ((STEEP+VERTICAL)/2)

#define GENTLE_DIAGONAL (atan(TILE_SLOPE*M_SQRT1_2))
#define STEEP_DIAGONAL (atan(4*TILE_SLOPE*M_SQRT1_2))
#define FG_TRANSITION_DIAGONAL ((FLAT+GENTLE_DIAGONAL)/2)

#define BANK M_PI_4
#define BANK_TRANSITION (M_PI_4/2)

#define PI_12 (M_PI/12.0)

typedef enum
{
STRING_TABLE_NAME=0,
STRING_TABLE_DESCRIPTION=1,
STRING_TABLE_CAPACITY=2
}string_table_index_t;

typedef enum
{
LANGUAGE_ENGLISH_UK=0,
LANGUAGE_ENGLISH_US=1
}language_t;

enum
{
RIDE_WET=0x00000100u,
RIDE_COVERED=0x00000400u,
RIDE_SEPERATE=0x00001000u
}ride_flags_t;

enum
{
CAR_ENABLE_REMAP2=0x01000000u,
CAR_ENABLE_REMAP3=0x00800000u,
CAR_IS_SWINGING=0x02000000u,
CAR_IS_SPINNING=0x04000000u,
CAR_IS_POWERED=0x08000000u,
CAR_NO_UPSTOPS=0x00000400u
}car_flags_t;

enum
{
SPRITE_FLAT_SLOPE=0x0001,
SPRITE_GENTLE_SLOPE=0x0002,
SPRITE_STEEP_SLOPE=0x0004,
SPRITE_VERTICAL_SLOPE=0x0008,
SPRITE_DIAGONAL_SLOPE=0x0010,
SPRITE_BANKING=0x0020,
SPRITE_UNKNOWN1=0x0040,
SPRITE_SLOPE_BANK_TRANSITION=0x0080,
SPRITE_DIAGONAL_BANK_TRANSITION=0x0100,
SPRITE_SLOPED_BANK_TRANSITION=0x0200,
SPRITE_SLOPED_BANKED_TURN=0x0400,
SPRITE_BANKED_SLOPE_TRANSITION=0x0800,
SPRITE_UNKNOWN2=0x1000,
SPRITE_RESTRAINT_ANIMATION=0x2000
}sprite_flags_t;

enum
{
CAR_INDEX_DEFAULT=0,
CAR_INDEX_FRONT=1,
CAR_INDEX_SECOND=2,
CAR_INDEX_THIRD=4,
CAR_INDEX_REAR=3
}car_index_t;

enum
{
CATEGORY_TRANSPORT_RIDE,
CATEGORY_GENTLE_RIDE,
CATEGORY_THRILL_RIDE,
CATEGORY_ROLLERCOASTER=2,
CATEGORY_WATER_RIDE
}category_t;


#define TRACK_STATION 0x0000000000000006l
#define TRACK_S_BEND 0x0000000000002000l
#define TRACK_LARGE_TURN 0x0000000000010000l
#define TRACK_LARGE_TURN_AS_SMALL_TURN 0x0000000000008000l
#define TRACK_HALF_CORKSCREW 0x0000000000080000l
#define TRACK_BRAKES 0x0000000001000000l
#define TRACK_ON_RIDE_PHOTO_SECTION 0x0000000004000000l
#define TRACK_BARREL_ROLL 0x0000000020000000l
#define TRACK_BLOCK_BRAKES 0x0000004000000000l

typedef uint64_t track_section_t;

typedef struct
{
uint32_t start_address;
uint16_t width;
uint16_t height;
int16_t x_offset;
int16_t y_offset;
uint16_t flags;
}graphic_record_t;

typedef struct
{
image_t** images;
uint32_t num_images;
uint32_t allocated;
}image_list_t;


typedef struct
{
language_t language;
char* str;
}string_table_entry_t;

typedef struct
{
string_table_entry_t* strings;
uint8_t num_strings;
}string_table_t;


typedef struct
{
uint32_t flags;
uint16_t unknown[6];
uint16_t sprites;
uint16_t friction;
uint8_t highest_rotation_index;
uint8_t spacing;
uint8_t rider_pairs;
uint8_t riders;
uint8_t rider_sprites;
uint8_t spin_inertia;
uint8_t spin_friction;
uint8_t powered_velocity;
uint8_t z_value;
}car_t;

typedef struct
{
uint64_t track_sections;
uint32_t flags;
uint8_t unknown;
uint8_t zero_cars;
uint8_t preview_index;
uint8_t track_style;
uint8_t excitement;
uint8_t intensity;
uint8_t nausea;
uint8_t max_height;
uint8_t categories[2];
uint8_t car_types[5];
uint8_t minimum_cars;
uint8_t maximum_cars;
car_t cars[NUM_CARS];
}ride_header_t;

typedef struct
{
uint8_t a,b,c;
}struct3byte_t;

typedef struct
{
uint8_t* positions;
uint16_t num;
}peep_position_data_t;

typedef struct
{
struct3byte_t* structures;
uint8_t num_structures;
peep_position_data_t peep_positions[4];
}ride_structures_t;

typedef struct
{
ride_header_t* ride_header;
string_table_t* string_tables[3];
ride_structures_t* optional;
image_list_t* images;
uint32_t num_images;
}object_t;

string_table_t* string_table_new();
char* string_table_get_string_by_language(string_table_t* table,language_t language);
void string_table_set_string_by_language(string_table_t* table,language_t language,char* str);

void image_list_set_image(image_list_t* list,int index,image_t* image);
void image_list_set_num_images(image_list_t* list,int num_images);

ride_header_t* ride_header_new();

ride_structures_t* ride_structures_new();

object_t* object_new_ride();
object_t* object_load_dat(const char* filename);
void object_save_dat(object_t* object,const char* filename);
char* object_get_string(object_t* object,string_table_index_t table_num,language_t language);
void object_set_string(object_t* object,string_table_index_t table_num,language_t language,char* new_string);
/*void RenderSprites(object_t* file,Animation* animations[5]);*/
void object_free(object_t* object);

#endif /*BACKEND_H_INCLUDED*/
