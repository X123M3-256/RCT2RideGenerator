#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include "mainwindow.h"


#include "dat.h"
#include <string.h>
//These methods are used to dump various bits of information for reversing purposes



void dump_unknown_data(object_t* object)
{

printf("%s\n",string_table_get_string_by_language(object->string_tables[STRING_TABLE_NAME],LANGUAGE_ENGLISH_UK));

int i=0;
    for(i=0;i<4;i++)
    {
    car_t* car=object->ride_header->cars+i;
    printf("Car %d: %d %d %d %d %d\n",i,car->unknown[0],car->unknown[1],car->unknown[2],car->unknown[3],car->unknown[3]);
    }

}

#define SRC_DIR "/home/edward/OpenRCT2/RCT2 Reversing/Object DATs/"
void iterate_files(void (subroutine)(object_t* file))
{
DIR           *dir;
struct dirent *entity;
dir=opendir(SRC_DIR);
    while ((entity = readdir(dir)) != NULL)
    {
    char path[256]=SRC_DIR;
    strcat(path,entity->d_name);
    object_t* object=object_load_dat(path);
        if(object)
        {
        subroutine(object);
        object_free(object);
        }
    }
closedir(dir);
}




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

