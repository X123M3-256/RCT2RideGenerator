#include <stdio.h>
#include <stdlib.h>
#include "mainwindow.h"
#include "dat.h"
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




void MakeRide()
{
int i;
DeserializeFile("msspinner9.rgen");

ObjectFile* file=LoadDat("RCT2 Reversing/Object DATs/Raw/BMFL.DAT");

RideHeader* header=file->ObjectHeader;
header->Flags|=RIDE_SEPERATE;
//header->TrackSections=0xFFFFFFFFFFFFFFFFl;
header->MinimumCars=1;
header->MaximumCars=8;
header->Cars[0].Sprites=0xF|SPRITE_BANKING;
header->CarTypes[CAR_INDEX_DEFAULT]=0;
header->CarTypes[CAR_INDEX_REAR]=0xFF;
header->CarTypes[CAR_INDEX_FRONT]=0xFF;
header->Cars[0].RiderPairs=0;
header->Cars[0].Riders=0;
header->Cars[0].RiderSprites=0;
header->Cars[0].Spacing=2;

header->Cars[1].Sprites=0;
header->Cars[2].Sprites=0;
header->Cars[3].Sprites=0;

SetString(file,STRING_TABLE_NAME,1,"AAAAAAA");

Animation* animations[5];
animations[0]=GetAnimationByIndex(0);
animations[1]=NULL;
animations[2]=NULL;
animations[3]=NULL;
animations[4]=GetAnimationByIndex(1);
RenderSprites(file,animations);

SaveDat(file,".wine/drive_c/Program Files/Infogrames/RollerCoaster Tycoon 2/ObjData/TEST.DAT");
}


*/

int main(int argc,char**argv)
{
/*
object_t* object=object_load_dat("/home/edward/RCT2 Reversing/Object DATs/Inverted Hairpin Coaster");
string_table_set_string_by_language(object->string_tables[0],LANGUAGE_ENGLISH_US,"AAAAAAA");
object->ride_header->track_style=2;
object->ride_header->maximum_cars=8;
object->ride_header->minimum_cars=3;
object->ride_header->track_sections=0xFFFFFFFFFFFFFFFFl;
object->ride_header->cars[0].flags|=CAR_IS_POWERED;
object->ride_header->cars[0].powered_velocity=80;
object->ride_header->cars[0].powered_acceleration=80;
object_save_dat(object,"ObjData/TEST.DAT");
*/

gtk_init(&argc,&argv);
//Create main interface
main_window_t* main_window=main_window_new();
gtk_main();
main_window_free(main_window);

return 0;
}

