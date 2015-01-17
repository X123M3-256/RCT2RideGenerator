#ifndef ANIMATIONDIALOG_H_INCLUDED
#define ANIMATIONDIALOG_H_INCLUDED
#include <gtk/gtk.h>
#include "interface.h"
#include "animation.h"

typedef struct
{
animation_t* animation;
model_t* grid_model;
int frame;

image_viewer_t* image_viewer;

GtkWidget* container;
GtkWidget* lower_hbox;
GtkWidget* next_frame;
GtkWidget* prev_frame;
GtkWidget* frame_spin;
}animation_viewer_t;

typedef struct
{
Vector* vector;

GtkWidget* container;
GtkWidget* label;
GtkWidget* x;
GtkWidget* y;
GtkWidget* z;
}vector_editor_t;

typedef struct
{
vector_editor_t* position_editor;
vector_editor_t* rotation_editor;
GtkWidget* container;
}object_transform_editor_t;

typedef struct
{
model_t* models;
int num_models;
GtkWidget* container;
}model_selector_t;

typedef struct
{
animation_t* animation;

string_editor_t* name_editor;
animation_viewer_t* animation_viewer;
object_transform_editor_t* transform_editor;

GtkWidget* dialog;
}animation_dialog_t;
#endif // ANIMATIONDIALOG_H_INCLUDED
