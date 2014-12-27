#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "palette.h"
#include "interface.h"
#include "dat.h"

/*
void dump_header(RideFile* file)
{
#define DST_DIR "/home/edward/RCT2 Reversing/Header Dumps/"
char dest_path[256]=DST_DIR;
strcat(dest_path,GetString(file,STRING_TABLE_NAME,0));
FILE* dump=fopen(dest_path,"w");
fwrite(file->Unknown,1,0x1C2,dump);
fclose(dump);
}

unsigned char header[0x1C2];

void or_headers(RideFile* file)
{
int i;
    for(i=0;i<0x1C2;i++)header[i]|=file->Unknown[i];
}


void list_track_type(RideFile* file)
{
#define LIST_FILE "/home/edward/RCT2 Reversing/track_types.txt"
FILE* track_types=fopen(LIST_FILE,"a+");
fprintf(track_types,"%s:  %x %x\n",GetString(file,STRING_TABLE_NAME,0),HEADER_BYTE(file,0xC),HEADER_BYTE(file,0xD));
fclose(track_types);
}

void show_nonzero(RideFile* file)
{
    if(file->Unknown[39]!=0)printf("%s\n",GetString(file,STRING_TABLE_NAME,0));
}


void dump_structures(RideFile* file)
{
#define DST_DIR "/home/edward/RCT2 Reversing/Other Dumps/"
char dest_path[256]=DST_DIR;
strcat(dest_path,GetString(file,STRING_TABLE_NAME,0));
FILE* dump=fopen(dest_path,"w");
fwrite(file->UnknownStructures,file->UnknownStructureSize,1,dump);
fclose(dump);
}

void iterate_files(void (subroutine)(RideFile* file))
{
#define SRC_DIR "/home/edward/.wine/drive_c/Program Files/Infogrames/RollerCoaster Tycoon 2 Clean/ObjData/"
DIR           *dir;
struct dirent *entity;
dir=opendir(SRC_DIR);
    while ((entity = readdir(dir)) != NULL)
    {
    char path[256]=SRC_DIR;
    strcat(path,entity->d_name);
    RideFile* file=LoadDat(path);
        if(file)
        {
        subroutine(file);
        FreeDat(file);
        }
    }
closedir(dir);
}

void isolate_relevant()
{
#define POS_DIR "/home/edward/RCT2\ Reversing/Object DATs/Positive/"
#define NEG_DIR "/home/edward/RCT2\ Reversing/Object DATs/Negative/"

int i;
DIR* dir;
struct dirent *entity;

memset(header,0xFF,0x1C2);

dir=opendir(POS_DIR);
    while ((entity = readdir(dir)) != NULL)
    {
    char path[256]=SRC_DIR;
    strcat(path,entity->d_name);
    RideFile* file=LoadDat(path);
        if(file)
        {
            for(i=0;i<0x1C2;i++)header[i]&=file->Unknown[i];
        FreeDat(file);
        }
    }
closedir(dir);


dir=opendir(NEG_DIR);
    while ((entity = readdir(dir)) != NULL)
    {
    char path[256]=NEG_DIR;
    strcat(path,entity->d_name);
    RideFile* file=LoadDat(path);
        if(file)
        {
            for(i=0;i<0x1C2;i++)header[i]&=~file->Unknown[i];
        FreeDat(file);
        }
    }
closedir(dir);
show_header();
}

void show_header()
{
int i;
    for(i=0;i<0x1C2;i++)
    {
    printf("%x ",header[i]);
        if(i%32==31)putchar('\n');
    }
}
*/
void DisplayRideInfo(ObjectFile* object)
{
RideHeader* header=object->ObjectHeader;
//Print categories
const char* categories[5]={"Transport ride","Gentle ride","Rollercoaster","Thrill ride","Water ride"};
printf("%s\n",categories[header->Categories[0]]);
    if(header->Categories[1]!=0xFF)printf("%s\n",categories[header->Categories[1]]);
//Print flags
    if(header->Flags&RIDE_COVERED)printf("Covered\n");
    if(header->Flags&RIDE_WET)printf("Wet\n");
//Print car types
    if(header->CarTypes[CAR_INDEX_FRONT]!=0xFF)printf("Front car %d\n",header->CarTypes[CAR_INDEX_FRONT]);
    if(header->CarTypes[CAR_INDEX_SECOND]!=0xFF)printf("Second car %d\n",header->CarTypes[CAR_INDEX_SECOND]);
    if(header->CarTypes[CAR_INDEX_THIRD]!=0xFF)printf("Third car %d\n",header->CarTypes[CAR_INDEX_THIRD]);
    if(header->CarTypes[CAR_INDEX_REAR]!=0xFF)printf("Rear car %d\n",header->CarTypes[CAR_INDEX_REAR]);
printf("Default car %d\n",header->CarTypes[CAR_INDEX_DEFAULT]);

printf("Car types\n");
int i;
    for(i=0;i<4;i++)
    {
            if(header->Cars[i].HighestRotationIndex==0)continue;
    printf("%d\n",i);
        if(header->Cars[i].Flags&CAR_ENABLE_REMAP2)printf("Has remap 2\n");
        if(header->Cars[i].Flags&CAR_ENABLE_REMAP3)printf("Has remap 3\n");
        if(header->Cars[i].Flags&CAR_IS_POWERED)printf("Powered- velocity %d\n",header->Cars[i].PoweredVelocity);
        if(header->Cars[i].Flags&CAR_IS_SPINNING)printf("Spinning- inertia %d, friction %d\n",header->Cars[i].SpinInertia,header->Cars[i].SpinFriction);
        if(header->Cars[i].Flags&CAR_IS_SWINGING)printf("Swinging\n");
        if(header->Cars[i].Flags&CAR_NO_UPSTOPS)printf("No upstops\n");
        if(header->Cars[i].RiderPairs)printf("Riders ride in pairs\n");
        printf("Riders: %d\n",header->Cars[i].Riders);
    }


}

Model* model;

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

void DrawFrames(ObjectFile* object,int frame,int numFrames,double pitch,double roll,double yaw)
{
int i,j;

Matrix pitchMatrix=MatrixIdentity();
pitchMatrix.Data[5]=cos(pitch);
pitchMatrix.Data[6]=sin(pitch);
pitchMatrix.Data[9]=-sin(pitch);
pitchMatrix.Data[10]=cos(pitch);

Matrix rollMatrix=MatrixIdentity();
rollMatrix.Data[0]=cos(roll);
rollMatrix.Data[1]=sin(roll);
rollMatrix.Data[4]=-sin(roll);
rollMatrix.Data[5]=cos(roll);

Matrix yawMatrix=MatrixIdentity();
yawMatrix.Data[0]=cos(yaw);
yawMatrix.Data[2]=-sin(yaw);
yawMatrix.Data[8]=sin(yaw);
yawMatrix.Data[10]=cos(yaw);

Matrix transformMatrix=MatrixMultiply(yawMatrix,MatrixMultiply(pitchMatrix,rollMatrix));

double rotation=0;
double step=2*M_PI/numFrames;
    for(j=0;j<numFrames;j++)
    {
    //FreeImage(&object->Images[i]);
    ClearBuffers();
    Matrix rotationMatrix=MatrixIdentity();
    rotationMatrix.Data[0]=cos(rotation);
    rotationMatrix.Data[2]=-sin(rotation);
    rotationMatrix.Data[8]=sin(rotation);
    rotationMatrix.Data[10]=cos(rotation);
    RenderModel(model,MatrixMultiply(rotationMatrix,transformMatrix));
    object->Images[frame]=ImageFromFrameBuffer();
    rotation+=step;
    frame++;
    }
}

int main(int argc,char**argv)
{
//Test code to create a ride based on the mini coaster
/*
int i;
model=LoadObj("model.obj");
model->transform.Data[5]=-1;
ObjectFile* file=LoadDat("/path/to/WCATC.DAT");
RideHeader* header=file->ObjectHeader;
header->MinimumCars=1;
header->MaximumCars=1;
header->CarTypes[CAR_INDEX_DEFAULT]=0;
header->CarTypes[CAR_INDEX_REAR]=0xFF;
header->CarTypes[CAR_INDEX_FRONT]=0xFF;
header->Cars[0].Spacing=6;
SetString(file,STRING_TABLE_NAME,1,"Name_Of_Ride");

int frame=3;
DrawFrames(file,frame,32,FLAT,0,0);
frame+=32;
DrawFrames(file,frame,4,FG_TRANSITION,0,0);
frame+=4;
DrawFrames(file,frame,4,-FG_TRANSITION,0,0);
frame+=4;
DrawFrames(file,frame,32,GENTLE,0,0);
frame+=32;
DrawFrames(file,frame,32,-GENTLE,0,0);
frame+=32;
DrawFrames(file,frame,8,GS_TRANSITION,0,0);
frame+=8;
DrawFrames(file,frame,8,-GS_TRANSITION,0,0);
frame+=8;
DrawFrames(file,frame,32,STEEP,0,0);
frame+=32;
DrawFrames(file,frame,32,-STEEP,0,0);
frame+=32;


DrawFrames(file,frame,4,FG_TRANSITION_DIAGONAL,0,M_PI_4);
frame+=4;
DrawFrames(file,frame,4,-FG_TRANSITION_DIAGONAL,0,M_PI_4);
frame+=4;
DrawFrames(file,frame,4,GENTLE_DIAGONAL,0,M_PI_4);
frame+=4;
DrawFrames(file,frame,4,-GENTLE_DIAGONAL,0,M_PI_4);
frame+=4;
DrawFrames(file,frame,4,STEEP_DIAGONAL,0,M_PI_4);
frame+=4;
DrawFrames(file,frame,4,-STEEP_DIAGONAL,0,M_PI_4);
frame+=4;

DrawFrames(file,frame,8,FLAT,BANK_TRANSITION,0);
frame+=8;
DrawFrames(file,frame,8,FLAT,-BANK_TRANSITION,0);
frame+=8;
DrawFrames(file,frame,32,FLAT,BANK,0);
frame+=32;
DrawFrames(file,frame,32,FLAT,-BANK,0);
frame+=32;

DrawFrames(file,frame,32,FG_TRANSITION,BANK_TRANSITION,0);
frame+=32;
DrawFrames(file,frame,32,FG_TRANSITION,-BANK_TRANSITION,0);
frame+=32;
DrawFrames(file,frame,32,-FG_TRANSITION,BANK_TRANSITION,0);
frame+=32;
DrawFrames(file,frame,32,-FG_TRANSITION,-BANK_TRANSITION,0);
frame+=32;
DrawFrames(file,frame,4,GENTLE_DIAGONAL,BANK_TRANSITION,M_PI_4);
frame+=4;
DrawFrames(file,frame,4,GENTLE_DIAGONAL,-BANK_TRANSITION,M_PI_4);
frame+=4;
DrawFrames(file,frame,4,-GENTLE_DIAGONAL,BANK_TRANSITION,M_PI_4);
frame+=4;
DrawFrames(file,frame,4,-GENTLE_DIAGONAL,-BANK_TRANSITION,M_PI_4);
frame+=4;
DrawFrames(file,frame,4,GENTLE,BANK_TRANSITION,0);
frame+=4;
DrawFrames(file,frame,4,GENTLE,-BANK_TRANSITION,0);
frame+=4;
DrawFrames(file,frame,4,-GENTLE,BANK_TRANSITION,0);
frame+=4;
DrawFrames(file,frame,4,-GENTLE,-BANK_TRANSITION,0);
frame+=4;
DrawFrames(file,frame,32,GENTLE,BANK,0);
frame+=32;
DrawFrames(file,frame,32,GENTLE,-BANK,0);
frame+=32;
DrawFrames(file,frame,32,-GENTLE,BANK,0);
frame+=32;
DrawFrames(file,frame,32,-GENTLE,-BANK,0);
frame+=32;
DrawFrames(file,frame,4,FG_TRANSITION,BANK,0);
frame+=4;
DrawFrames(file,frame,4,FG_TRANSITION,-BANK,0);
frame+=4;
DrawFrames(file,frame,4,-FG_TRANSITION,BANK,0);
frame+=4;
DrawFrames(file,frame,4,-FG_TRANSITION,-BANK,0);
frame+=4;
SaveDat(file,"/home/edward/.wine/drive_c/Program Files/Infogrames/RollerCoaster Tycoon 2/ObjData/TEST.DAT");
*/


InitializePalette();

gtk_init(&argc,&argv);
//Create main interface
MainWindow* MainInterface=CreateInterface();

return 0;
}

