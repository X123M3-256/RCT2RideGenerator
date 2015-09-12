#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include "mainwindow.h"

//TODO: Make models appear in menu as soon as they're added
//TODO: Fix crashing when reexporting
//TODO: Make labels use actual units
//TODO: Make animation preview update immediately instead of waiting for slider
//TODO: Color selection window


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

