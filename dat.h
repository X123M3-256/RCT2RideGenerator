#ifndef BACKEND_H_INCLUDED
#define BACKEND_H_INCLUDED
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "animation.h"
#define NUM_STRING_TABLES 3
#define NUM_LANGUAGES 11
#define NUM_CARS 4

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

enum StringTables
{
STRING_TABLE_NAME=0,
STRING_TABLE_DESCRIPTION=1,
STRING_TABLE_CAPACITY=2
};
enum Languages
{
LANGUAGE_ENGLISH_UK=0,
LANGUAGE_ENGLISH_US=1
};
enum RideFlags
{
RIDE_WET=0x00000100u,
RIDE_COVERED=0x00000400u,
RIDE_SEPERATE=0x00001000u
};
enum CarFlags
{
CAR_ENABLE_REMAP2=0x01000000u,
CAR_ENABLE_REMAP3=0x00800000u,
CAR_IS_SWINGING=0x02000000u,
CAR_IS_SPINNING=0x04000000u,
CAR_IS_POWERED=0x08000000u,
CAR_NO_UPSTOPS=0x00000400u
};
enum SpriteFlags
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
};

enum CarIndices
{
CAR_INDEX_DEFAULT=0,
CAR_INDEX_FRONT=1,
CAR_INDEX_SECOND=2,
CAR_INDEX_THIRD=4,
CAR_INDEX_REAR=3
};
enum Categories
{
CATEGORY_TRANSPORT_RIDE,
CATEGORY_GENTLE_RIDE,
CATEGORY_THRILL_RIDE,
CATEGORY_ROLLERCOASTER=2,
CATEGORY_WATER_RIDE
};



typedef struct
{
uint32_t StartAddress;
uint16_t Width;
uint16_t Height;
int16_t Xoffset;
int16_t Yoffset;
uint16_t Flags;
}GraphicRecord;

typedef struct
{
short Width;
short Height;
short XOffset;
short YOffset;
short Flags;
unsigned char** Data;
}Image;


typedef struct
{
char Num;
char* Str;
}String;


typedef struct
{
uint32_t Flags;
uint16_t Unknown[7];
uint16_t Sprites;
uint8_t HighestRotationIndex;
uint8_t Spacing;
uint8_t RiderPairs;
uint8_t RiderSprites;
uint8_t Riders;
uint8_t SpinInertia;
uint8_t SpinFriction;
uint8_t PoweredVelocity;
uint8_t ZValue;
}Car;

typedef struct
{
uint64_t TrackSections;
uint32_t Flags;
uint16_t Unknown;
uint8_t PreviewIndex;
uint8_t TrackStyle;
uint8_t Excitement;
uint8_t Intensity;
uint8_t Nausea;
uint8_t MaxHeight;
uint8_t Categories[2];
uint8_t CarTypes[5];
uint8_t MinimumCars;
uint8_t MaximumCars;
Car Cars[NUM_CARS];
}RideHeader;

typedef struct
{
uint8_t a,b,c;
}struct3byte;

typedef struct
{
char* Positions;
uint16_t Num;
}PeepPositionData;

typedef struct
{
struct3byte* Structures;
uint8_t NumStructures;
PeepPositionData PeepPositions[4];
}RideStructures;

typedef struct
{
void* ObjectHeader;
String** StringTables[3];
void* Optional;

Image** Images;
int NumImages;
}ObjectFile;



ObjectFile* LoadDat(char* filename);
void SaveDat(ObjectFile* dat,char* filename);
char* GetString(ObjectFile* RideData,int TableNum,int Language);
void SetString(ObjectFile* RideData,int TableNum,int Language,char* NewStr);
void RenderSprites(ObjectFile* file,Animation* animations[5]);
void FreeImage(Image* image);
void FreeDat();

#endif // BACKEND_H_INCLUDED
