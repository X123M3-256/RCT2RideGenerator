#ifndef BACKEND_H_INCLUDED
#define BACKEND_H_INCLUDED
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#define NUM_STRING_TABLES 3
#define NUM_LANGUAGES 11
#define NUM_CARS 4



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
RIDE_WET=0x00010000u,
RIDE_COVERED=0x00020000u
};
enum CarFlags
{
CAR_ENABLE_REMAP2=0x01000000u,
CAR_ENABLE_REMAP3=0x00800000u,
CAR_IS_SPINNING=0x02000000u,
CAR_IS_SWINGING=0x04000000u,
CAR_IS_POWERED=0x08000000u,
CAR_NO_UPSTOPS=0x00000400u
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

int NumImages;
int GraphicBytes;
Image* Images;
}ObjectFile;



ObjectFile* LoadDat(char* filename);
void SaveDat(ObjectFile* dat,char* filename);
char* GetString(ObjectFile* RideData,int TableNum,int Language);
void SetString(ObjectFile* RideData,int TableNum,int Language,char* NewStr);
void FreeImage(Image* image);
void FreeDat();

#endif // BACKEND_H_INCLUDED
