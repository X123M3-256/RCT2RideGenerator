#include <stdio.h>
#include <stdlib.h>
#include "interface.h"
#include "backend.h"
#include "palette.h"


int main(int argc,char**argv)
{
    InitializePalette();
    InitOpenGL(256,256);

    gtk_init(&argc,&argv);
    //Create main interface
    MainWindow* MainInterface=CreateInterface();

    QuitOpenGL();
    return 0;
}

