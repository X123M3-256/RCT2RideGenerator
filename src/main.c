#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include "mainwindow.h"

//TODO: Make labels use actual units
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

