#ifndef BACKEND_H_INCLUDED
#define BACKEND_H_INCLUDED
#include "image.h"
#include "math_defines.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_STRING_TABLES 3
#define NUM_LANGUAGES 11
#define NUM_CARS 4

#define TILE_SLOPE (1 / sqrt(6))

#define FLAT 0

#define GENTLE (atan(TILE_SLOPE))
#define STEEP (atan(4 * TILE_SLOPE))
#define VERTICAL M_PI_2
#define FG_TRANSITION ((FLAT + GENTLE) / 2)
#define GS_TRANSITION ((GENTLE + STEEP) / 2)
#define SV_TRANSITION ((STEEP + VERTICAL) / 2)

#define GENTLE_DIAGONAL (atan(TILE_SLOPE * M_SQRT1_2))
#define STEEP_DIAGONAL (atan(4 * TILE_SLOPE * M_SQRT1_2))
#define FG_TRANSITION_DIAGONAL ((FLAT + GENTLE_DIAGONAL) / 2)

#define BANK M_PI_4
#define BANK_TRANSITION (M_PI_4 / 2)

#define CORKSCREW_RIGHT_YAW(angle) \
    (atan2(0.5 * (1 - cos(angle)), 1 - 0.5 * (1 - cos(angle))))
#define CORKSCREW_RIGHT_PITCH(angle) (-asin(-sin(angle) / sqrt(2.0)))
#define CORKSCREW_RIGHT_ROLL(angle) (-atan2(sin(angle) / sqrt(2.0), cos(angle)))

#define CORKSCREW_LEFT_YAW(angle) (-CORKSCREW_RIGHT_YAW(angle))
#define CORKSCREW_LEFT_PITCH(angle) (-CORKSCREW_RIGHT_PITCH(-angle))
#define CORKSCREW_LEFT_ROLL(angle) (-CORKSCREW_RIGHT_ROLL(angle))

typedef enum {
    STRING_TABLE_NAME = 0,
    STRING_TABLE_DESCRIPTION = 1,
    STRING_TABLE_CAPACITY = 2
} string_table_index_t;

typedef enum {
    LANGUAGE_ENGLISH_UK = 0,
    LANGUAGE_ENGLISH_US = 1
} language_t;

enum {
    RIDE_VEHICLE_TAB_SCALE_HALF =                0x1u,
    RIDE_NO_INVERSIONS =                         0x2u,
    RIDE_NO_BANKED_TRACK =                       0x4u,
    RIDE_CHUFFING_ON_DEPART =                    0x8u,
    RIDE_SWING_MODE_1 =                         0x10u,
    RIDE_ROTATION_MODE_1 =                      0x20u, //twist
    RIDE_ROTATION_MODE_2 =                      0x40u, //enterprise
    RIDE_DISABLE_WANDERING =                    0x80u,
    RIDE_PLAY_SPLASH_SOUND =                   0x100u,
    RIDE_PLAY_SPLASH_SOUND_SLIDE =             0x200u,
    RIDE_COVERED =                             0x400u,
    RIDE_LIMIT_AIRTIME_BONUS =                 0x800u,
    RIDE_SEPARATE_RIDE_DEPRECATED =           0x1000u,
    RIDE_SEPARATE_RIDE =                      0x2000u,//this enables track designs
    RIDE_CANNOT_BREAK_DOWN =                  0x4000u,
    RIDE_DISABLE_LAST_OPERATING_MODE =        0x8000u,
    RIDE_FLAG_DISABLE_COLLISION_CRASHES =    0x10000u,
    RIDE_DISABLE_FIRST_TWO_OPERATING_MODES = 0x20000u,
    RIDE_DISABLE_DOOR_CONSTRUCTION =         0x40000u,
    RIDE_DISABLE_COLOR_TAB =                 0x80000u,
    RIDE_ALTERNATIVE_SWING_MODE_2 =         0x100000u,

    RIDE_WET =                      0x100u,
    RIDE_SLOW_IN_WATER =            0x200u,
    RIDE_SEPERATE =                 0x1000u
    // from showing in the window
};

enum {
    CAR_ANIMATION_NONE = 0x00u, //1 vehicle sprite, 1 peep sprite
    CAR_ANIMATION_LOCOMOTIVE = 0x01u, //4 vehicle sprites, uknown peep sprites, slow animation
    CAR_ANIMATION_SWAN = 0x02u, //1 vehicle sprite, 2 peep sprites for the first pair, 1 peep sprite for the second pair (maybe more?)
    CAR_ANIMATION_CANOES = 0x03u, // 1 vehicle sprite, 6 peep sprites
    CAR_ANIMATION_ROW_BOATS = 0x04u, // 1 vehicle sprite, 7 peep sprites
    CAR_ANIMATION_WATER_TRICYCLES = 0x05u, //1 vehicle sprite, 2 peep sprites
    CAR_ANIMATION_OBSERVATION = 0x06u, //8 vehicle sprites, unknown peep sprites, we don't really know how to get this to work
    CAR_ANIMATION_HELICARS = 0x07u, //4 vehicle sprites, 4 peep sprites, fast animation speed
    CAR_ANIMATION_MONORAIL_CYCLE = 0x08u, //4 vehicle sprites, 4 peep sprites, medium animation speed, only animates with riders present
    CAR_ANIMATION_4D = 0x09u, // 8 vehicle sprites, 8 peep sprites=
    CAR_ANIMATION_ANIMAL_FLYING = 10u,
    CAR_ANIMATION_ANIMAL_WALKING = 11u
};

// clang-format off
typedef enum {
    CAR_COASTS_DOWNHILL =             1 << 0,//these are all bit-shifted 8 because the flags field is offset one byte
    CAR_NO_UPSTOPS_TIGHT_TOLERANCE =  1 << 1,
    CAR_NO_UPSTOPS =                  1 << 2,//bobsled-style with 0.05 extra G's
    CAR_IS_MINIGOLFER =               1 << 3,
    CAR_FLAG_4 =                      1 << 4,
    CAR_FLAG_5 =                      1 << 5,
    CAR_CAN_INVERT =                  1 << 6,//I assume this is set on the flying and lay-down so they can spawn properly on inverted station track
    CAR_DODGEM_USE_LIGHTS =           1 << 7,
    CAR_OPENS_DOORS =                 1 << 8,//only relevant for making things backwards-compatible with RCT2 and doors
    CAR_ENABLE_REMAP3 =               1 << 9,
    CAR_RECALCULATE_SPRITE_BOUNDS =   1 << 10,
    CAR_USE_16_ROTATION_FRAMES =      1 << 11,
    CAR_OVERRIDE_VERTICAL_FRAMES =    1 << 12,
    SPRITE_BOUNDS_INCLUDE_INVERTED_SET = 1 << 13,
    CAR_EXTRA_SPINNING_FRAMES =       1 << 14,
    CAR_EXTRA_POWER_ON_ASCENT =       1 << 15,
    CAR_ENABLE_REMAP2 =               1 << 16,
    CAR_IS_SWINGING =                 1 << 17,
    CAR_IS_SPINNING =                 1 << 18,
    CAR_IS_POWERED =                  1 << 19,
    CAR_ENABLE_ROLLING_SOUND =        1 << 20,//riders scream
    CAR_FLAG_21 =                     1 << 21,//related to swinging sprites - I believe this is set automatically
    CAR_WANDERS =                     1 << 22,
    CAR_IS_ANIMATED =                 1 << 23,

    CAR_FLAG_RIDER_ANIMATION = 1 << 24,
    CAR_FLAG_25 = 1 << 25,// related to swinging sprites?
    CAR_FLAG_2D_LOADING_WAYPOINTS = 1 << 26,
    CAR_FLAG_27 = 1 << 27,// related to swinging sprites
    CAR_IS_CHAIRLIFT = 1 << 28,
    CAR_FLAG_WATER_PROPULSION = 1 << 29,//accelerates downhill, does not stop when ride breaks down b/c water continues to flow
    CAR_IS_GO_KART = 1 << 30,
    CAR_FLAG_DODGEM_CAR_PLACEMENT = 1u << 31,
} car_flags_t;

typedef enum {
    SPRITE_FLAT_SLOPE =               0x0001,
    SPRITE_GENTLE_SLOPE =             0x0002,
    SPRITE_STEEP_SLOPE =              0x0004,
    SPRITE_VERTICAL_SLOPE =           0x0008,
    SPRITE_DIAGONAL_SLOPE =           0x0010,
    SPRITE_BANKING =                  0x0020,
    SPRITE_INLINE_TWIST =             0x0040,
    SPRITE_SLOPE_BANK_TRANSITION =    0x0080,
    SPRITE_DIAGONAL_BANK_TRANSITION = 0x0100,
    SPRITE_SLOPED_BANK_TRANSITION =   0x0200,
    SPRITE_SLOPED_BANKED_TURN =       0x0400,
    SPRITE_BANKED_SLOPE_TRANSITION =  0x0800,
    SPRITE_CORKSCREW =                0x1000,
    SPRITE_RESTRAINT_ANIMATION =      0x2000,
    SPRITE_SPIRAL_LIFT =              0x4000
} sprite_flags_t;

typedef enum {
    RUNNING_SOUND_WOODEN_OLD = 1,
    RUNNING_SOUND_WOODEN_MODERN = 54,
    RUNNING_SOUND_STEEL = 2,
    RUNNING_SOUND_STEEL_SMOOTH = 57,
    RUNNING_SOUND_WATERSLIDE = 32,
    RUNNING_SOUND_TRAIN = 31,
    RUNNING_SOUND_ENGINE = 21,
    RUNNING_SOUND_NONE = 255
} primary_sound_t;

typedef enum {
    SECONDARY_SOUND_SCREAMS_1 = 0,
    SECONDARY_SOUND_SCREAMS_2 = 1,
    SECONDARY_SOUND_SCREAMS_3 = 2,
    SECONDARY_SOUND_WHISTLE = 3,
    SECONDARY_SOUND_BELL = 4,
    SECONDARY_SOUND_NONE = 255
} secondary_sound_t;

typedef enum {
    CAR_INDEX_DEFAULT = 0,
    CAR_INDEX_FRONT = 1,
    CAR_INDEX_SECOND = 2,
    CAR_INDEX_THIRD = 4,
    CAR_INDEX_REAR = 3
} car_index_t;

typedef enum {
    CATEGORY_TRANSPORT_RIDE = 0,
    CATEGORY_GENTLE_RIDE = 1,
    CATEGORY_ROLLERCOASTER = 2,
    CATEGORY_THRILL_RIDE = 3,
    CATEGORY_WATER_RIDE = 4,
    CATEGORY_NONE = 255
} category_t;

#define TRACK_STATION 0x0000000000000004l
#define TRACK_STRAIGHT 0x0000000000000002l
#define TRACK_CHAIN_LIFT 0x0000000000000008l
#define TRACK_STEEP_CHAIN_LIFT 0x0000000000000010l
#define TRACK_TIRE_DRIVE_LIFT 0x0000000000000020l // Junior only
#define TRACK_BANKING 0x0000000000000040l
#define TRACK_VERTICAL_LOOP 0x0000000000000080l
#define TRACK_GENTLE_SLOPE 0x0000000000000100l
#define TRACK_STEEP_SLOPE 0x0000000000000200l
#define TRACK_FLAT_TO_STEEP_SLOPE_SMALL 0x0000000000000400l
#define TRACK_SLOPED_TURNS 0x0000000000000800l //(Enables available turns)
#define TRACK_STEEP_SLOPED_TURNS 0x0000000000001000l
#define TRACK_S_BEND 0x0000000000002000l
#define TRACK_TINY_TURNS 0x0000000000004000l
#define TRACK_SMALL_TURN 0x0000000000008000l //(Flat enabled)
#define TRACK_LARGE_TURN 0x0000000000010000l
#define TRACK_INLINE_TWIST 0x0000000000020000l
#define TRACK_HALF_LOOP 0x0000000000040000l
#define TRACK_HALF_CORKSCREW 0x0000000000080000l
#define TRACK_VERTICAL_TOWER \
    0x0000000000100000l // Launched Freefall + Roto-Drop?)
#define TRACK_HELIX 0x0000000000200000l //(For upright track only)
#define TRACK_QUARTER_HELIX 0x0000000000400000l //(For inverted track only)
#define TRACK_UNBANKED_QUARTER_HELIX \
    0x0000000000800000l // Suspended Swinging only
#define TRACK_BRAKES 0x0000000001000000l
#define TRACK_ON_RIDE_PHOTO_SECTION 0x0000000004000000l
#define TRACK_WATER_SPLASH 0x0000000008000000l // Wooden only
#define TRACK_VERTICAL_SLOPE 0x0000000010000000l
#define TRACK_BARREL_ROLL 0x0000000020000000l
#define TRACK_LAUNCHED_LIFT 0x0000000040000000l
#define TRACK_LARGE_HALF_LOOP 0x0000000080000000l
#define TRACK_BANKED_FLAT_TO_UNBANKED_GENTLE_SLOPE_TURN 0x0000000100000000l
#define TRACK_HEARTLINE_ROLL 0x0000000400000000l // Heartline only
#define TRACK_REVERSER_TURNTABLE 0x0000000200000000l // Log Flume only
#define TRACK_REVERSER 0x0000000800000000l // Reverser only
#define TRACK_FLAT_TO_VERTICAL_SLOPE \
    0x0000001000000000l // Enables vertical track
#define TRACK_VERTICAL_TO_FLAT 0x0000002000000000l // Enables top section
#define TRACK_BLOCK_BRAKES 0x0000004000000000l
#define TRACK_GENTLE_SLOPE_BANKING \
    0x0000008000000000l //(Enables available turns)
#define TRACK_FLAT_TO_STEEP_SLOPE 0x0000010000000000l
#define TRACK_VERTICAL_SLOPED_TURNS 0x0000020000000000l
#define TRACK_CABLE_LIFT 0x0000080000000000 // Giga only
#define TRACK_SPIRAL_LIFT 0x0000100000000000l // Spiral Coaster only
#define TRACK_QUARTER_LOOP 0x0000200000000000l
#define TRACK_SPINNING_TUNNEL 0x0000400000000000l //(Ghost Train and Car Rides)
#define TRACK_SPINNING_TOGGLE 0x0000800000000000l // Spinning Wild Mouse only)
#define TRACK_INLINE_TWIST_UNINVERT 0x0001000000000000l
#define TRACK_INLINE_TWIST_INVERT 0x0002000000000000l
#define TRACK_QUARTER_LOOP_INVERT 0x0004000000000000l
#define TRACK_QUARTER_LOOP_UNINVERT 0x0008000000000000l
#define TRACK_RAPIDS \
    0x0010000000000000l // Rapids / Log Bumps, River Rapids / Monster Trucks only
#define TRACK_HALF_LOOP_UNINVERT 0x0020000000000000l
#define TRACK_HALF_LOOP_INVERT 0x0040000000000000l

typedef struct {
    uint8_t* data;
    uint32_t size;
    uint32_t allocated;
} buffer_t;

typedef uint64_t track_section_t;

typedef struct {
    uint32_t start_address;
    uint16_t width;
    uint16_t height;
    int16_t x_offset;
    int16_t y_offset;
    uint16_t flags;
} graphic_record_t;

typedef struct {
    image_t** images;
    uint32_t num_images;
    uint32_t allocated;
} image_list_t;

typedef struct {
    language_t language;
    char* str;
} string_table_entry_t;

typedef struct {
    string_table_entry_t* strings;
    uint8_t num_strings;
} string_table_t;

typedef struct {
    uint32_t flags;
	uint8_t animation_type;
    uint32_t spacing;
    uint16_t unknown[9];
    uint16_t sprites;
    uint16_t friction;
    uint8_t running_sound;
    uint8_t secondary_sound;
    uint8_t highest_rotation_index;
    uint8_t rider_pairs;
    uint8_t riders;
    uint8_t rider_sprites;
    uint8_t spin_inertia;
    uint8_t spin_friction;
    uint8_t powered_acceleration;
    uint8_t powered_velocity;
    uint8_t z_value;
	uint8_t car_visual;
	uint8_t effect_visual;
	uint8_t logflume_reverser_vehicle;
	uint8_t double_sound_frequency;
    uint8_t override_vertical_frames;
    int8_t vehicle_tab_vertical_offset;

    uint8_t sprite_width;
    uint8_t sprite_height_negative;
    uint8_t sprite_height_positive;
} car_t;

typedef struct {
    uint64_t track_sections;
    uint32_t flags;
    uint8_t car_icon_index;
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
} ride_header_t;

typedef struct {
    uint8_t colors[3];
} color_scheme_t;

typedef struct {
    uint8_t* positions;
    uint16_t num;
} peep_position_data_t;

typedef struct {
    color_scheme_t* default_colors;
    uint8_t num_default_colors;
	uint8_t num_colors_override; // set to 255 to indicate each train gets a different color OF THE 32 COLORS INCLUDED!!!!
    peep_position_data_t peep_positions[4];
} ride_structures_t;

typedef struct {
    ride_header_t* ride_header;
    string_table_t* string_tables[3];
    ride_structures_t* optional;
    image_list_t* images;
    uint32_t num_images;
} object_t;

string_table_t* string_table_new();
char* string_table_get_string_by_language(string_table_t* table,
    language_t language);
void string_table_set_string_by_language(string_table_t* table,
    language_t language,
    char* str);

image_list_t* image_list_new();
image_list_t* image_list_load(uint8_t* bytes, uint32_t* pos_ptr);
void image_list_write(image_list_t* list, buffer_t* buffer);
void image_list_set_image(image_list_t* list, int index, image_t* image);
void image_list_set_num_images(image_list_t* list, uint32_t num_images);
void image_list_free(image_list_t* list);

ride_header_t* ride_header_new();

ride_structures_t* ride_structures_new();
void ride_structures_set_num_peep_positions(ride_structures_t* structures,
    int car,
    int num);
void ride_structures_set_num_default_colors(ride_structures_t* structures,
    int num);

object_t* object_new_ride();
object_t* object_load_dat(const char* filename);
void object_save_dat(object_t* object, const char* filename, uint32_t checksum);
char* object_get_string(object_t* object,
    string_table_index_t table_num,
    language_t language);
void object_set_string(object_t* object,
    string_table_index_t table_num,
    language_t language,
    char* new_string);
/*void RenderSprites(object_t* file,Animation* animations[5]);*/
void object_free(object_t* object);
void track_encode(buffer_t* data, char* filename);
buffer_t* load_file(char* filename);
buffer_t* track_decode(char* file);
#endif /*BACKEND_H_INCLUDED*/
