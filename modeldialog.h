#ifndef MODELDIALOG_H_INCLUDED
#define MODELDIALOG_H_INCLUDED
#include <gtk/gtk.h>
#include "renderer.h"
#include "linearalgebra.h"

#define RECOLORABLE_1 -1
#define RECOLORABLE_2 -2
#define RECOLORABLE_3 -3
#define NON_RECOLORABLE -4

typedef struct
{
Model* model;
Matrix modelView;
GtkWidget* dialog;
GtkWidget* preview;
GdkPixbuf* pixbuf;
int painting;
int paintState;
}ModelDialog;

void CreateModelDialog(Model* model);

#endif // MODELDIALOG_H_INCLUDED
