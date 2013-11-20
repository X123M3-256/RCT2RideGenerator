#ifndef TYPES_H_INCLUDED
#define TYPES_H_INCLUDED
#define HEADER_SIZE 21
//Unlike Buggys Ridemaker, this doesn't handle paths
typedef struct
{
int StartAddress;
short Width;
short Height;
short Xoffset;
short Yoffset;
short Flags;
short Padding;
}TGraphicRecord;

typedef struct
{
short Width;
short Height;
short XOffset;
short YOffset;
short Flags;
char** Data;
}Image;

typedef struct
{
char Num;
char* Str;
}String;

typedef struct
{
char Header[HEADER_SIZE];
char* Unknown[0x1C2];
String** StringTables[3];
char* UnknownStructures;
int UnknownStructureSize;
int NumImages;
int GraphicBytes;
Image* Images;
}RideFile;


#endif // TYPES_H_INCLUDED
