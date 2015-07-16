#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include "mainwindow.h"
//#include "dat.h"
/*
//These methods are used to dump various bits of information for reversing purposes
uint32_t flags;

void or_flags(ObjectFile* file)
{
flags|=((RideHeader*)file->ObjectHeader)->Flags;
}

void dump_track_types(ObjectFile* object)
{
FILE* dump=fopen("ridetypes.txt","a");
fprintf(dump,"%x %s\n",((RideHeader*)object->ObjectHeader)->TrackStyle,GetString(object,STRING_TABLE_NAME,0));
fclose(dump);
}

void test_flags(ObjectFile* object)
{
RideHeader* header=(RideHeader*)object->ObjectHeader;
    if(header->Cars[0].Flags&flags)printf("%s\n",GetString(object,STRING_TABLE_NAME,0));
}
#define SRC_DIR "/home/edward/RCT2\ Reversing/Object DATs/"
void iterate_files(void (subroutine)(ObjectFile* file))
{
DIR           *dir;
struct dirent *entity;
dir=opendir(SRC_DIR);
    while ((entity = readdir(dir)) != NULL)
    {
    char path[256]=SRC_DIR;
    strcat(path,entity->d_name);
    ObjectFile* file=LoadDat(path);
        if(file)
        {
        subroutine(file);
        FreeDat(file);
        }
    }
closedir(dir);
}
void run_rename()
{
#define RAW_DIR "/home/edward/RCT2\ Reversing/Object DATs/Raw/"
#define DEST_DIR "/home/edward/RCT2\ Reversing/Object DATs/"

int i;
DIR* dir;
struct dirent *entity;

dir=opendir(RAW_DIR);
    while ((entity = readdir(dir)) != NULL)
    {
    char path[512]=RAW_DIR;
    strcat(path,entity->d_name);
    printf("%s\n",path);

    ObjectFile* file=LoadDat(path);
        if(file)
        {
        char destpath[512]=DEST_DIR;
        strcat(destpath,GetString(file,STRING_TABLE_NAME,0));
        char cmd[512];
        sprintf(cmd,"cp \'%s\' \'%s\'",path,destpath);
        system(cmd);
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

flags=0xFFFFFFFF;

dir=opendir(POS_DIR);
    while ((entity = readdir(dir)) != NULL)
    {
    char path[256]=SRC_DIR;
    strcat(path,entity->d_name);

    ObjectFile* file=LoadDat(path);
        if(file)
        {
        flags&=((RideHeader*)file->ObjectHeader)->Unknown;
        printf("%s %x\n",entity->d_name,((RideHeader*)file->ObjectHeader)->Unknown);
        FreeDat(file);
        }
    }
closedir(dir);
dir=opendir(NEG_DIR);
    while ((entity = readdir(dir)) != NULL)
    {
    char path[256]=NEG_DIR;
    strcat(path,entity->d_name);
    ObjectFile* file=LoadDat(path);
        if(file)
        {
        flags&=~((RideHeader*)file->ObjectHeader)->Flags;
        FreeDat(file);
        }
    }
closedir(dir);
printf("%x\n",flags);
}
/*
void show_header()
{
int i;
    for(i=0;i<0x1C2;i++)
    {
    printf("%x ",header[i]);
        if(i%32==31)putchar('\n');
    }
}

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


*/


int main(int argc,char**argv)
{
srand(time(0));
gtk_init(&argc,&argv);
//Create main interface
main_window_t* main_window=main_window_new();
gtk_main();
main_window_free(main_window);
return 0;
}

