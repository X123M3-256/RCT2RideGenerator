#ifndef ANIMATIONDIALOG_H_INCLUDED
#define ANIMATIONDIALOG_H_INCLUDED
#include <gtk/gtk.h>
#include "renderer.h"
#include "linearalgebra.h"
#include "animation.h"
struct animDialogStruct;

typedef struct
{
struct animDialogStruct* dialog;
int index;
GtkWidget* nameLabel;
GtkWidget* positionXSpin;
GtkWidget* positionYSpin;
GtkWidget* positionZSpin;
GtkWidget* rotationXSpin;
GtkWidget* rotationYSpin;
GtkWidget* rotationZSpin;
GtkWidget* parentSelect;
}ObjectEditor;

typedef struct animDialogStruct
{
Animation* animation;
Model* gridModel;
GtkWidget* dialog;
GtkWidget* nextFrame;
GtkWidget* prevFrame;
GtkWidget* curFrameSpin;
GtkWidget* modelSelect;
GtkWidget* objectTable;
GtkWidget* preview;
GdkPixbuf* pixbuf;
ObjectEditor* ObjectEditors[MAX_OBJECTS_PER_FRAME];
unsigned int numObjects;
unsigned int frame;
}AnimationDialog;

void CreateAnimationDialog(Animation* animation);

#endif // ANIMATIONDIALOG_H_INCLUDED
