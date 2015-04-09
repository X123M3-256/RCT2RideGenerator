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
Vector vector;
GtkWidget* container;
GtkWidget* label;
GtkWidget* x;
GtkWidget* y;
GtkWidget* z;
}vector_editor_t;

typedef struct
{
object_transform_t* object_transform;
vector_editor_t* position_editor;
vector_editor_t* rotation_editor;
GtkWidget* container;
}object_transform_editor_t;

typedef struct
{
model_t** models;
int num_models;
GtkWidget* container;
}model_selector_t;

typedef struct
{
animation_t* animation;
int selected_object;

string_editor_t* name_editor;
model_selector_t* model_selector;
animation_viewer_t* animation_viewer;
object_transform_editor_t* transform_editor;

GtkWidget* dialog;
GtkWidget* add_model;
}animation_dialog_t;


animation_dialog_t* animation_dialog_new(animation_t* animation,model_t** models,int num_models);
void animation_dialog_run(animation_dialog_t* dialog);
void animation_dialog_free(animation_dialog_t* dialog);

#endif // ANIMATIONDIALOG_H_INCLUDED
