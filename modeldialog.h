#ifndef MODELDIALOG_H_INCLUDED
#define MODELDIALOG_H_INCLUDED
#include <gtk/gtk.h>
#include "interface.h"
#include "model.h"
#include "image.h"

#define RECOLORABLE_1 -1
#define RECOLORABLE_2 -2
#define RECOLORABLE_3 -3
#define NON_RECOLORABLE -4

typedef struct
{
model_t* model;
model_t* grid;
Matrix model_view;

image_viewer_t* image_viewer;

GtkWidget* container;
GtkWidget* button_table;
GtkWidget* reset;
GtkWidget* rotate_left;
GtkWidget* rotate_right;
GtkWidget* rotate_up;
GtkWidget* rotate_down;
GtkWidget* translate_left;
GtkWidget* translate_right;
GtkWidget* translate_up;
GtkWidget* translate_down;
GtkWidget* zoom_in;
GtkWidget* zoom_out;
}model_viewer_t;

typedef struct
{
Matrix* matrix;
Matrix transform;

GtkWidget* container;
}matrix_transform_button_t;

typedef struct
{
uint8_t* color;
uint8_t value;
GdkPixbuf* pixbuf;
GtkWidget* image;
GtkToolItem* tool_item;
}color_select_tool_t;

typedef struct
{
uint8_t* color;
GtkWidget* container;
GtkWidget* remap_tools;
GtkWidget* peep_tools;
GtkWidget* color_tools;
color_select_tool_t** tools;
}color_selector_t;


typedef struct
{
model_t* model;
uint8_t color;
uint8_t painting;

string_editor_t* name_editor;
matrix_transform_button_t* flip_x;
matrix_transform_button_t* flip_y;
matrix_transform_button_t* flip_z;
matrix_transform_button_t* rotate_x;
matrix_transform_button_t* rotate_y;
matrix_transform_button_t* rotate_z;
model_viewer_t* model_viewer;
color_selector_t* color_selector;

GtkWidget* dialog;
GtkWidget* paint_all;
GtkWidget* is_rider;
}model_dialog_t;

model_dialog_t* model_dialog_new(model_t* model);
void model_dialog_run(model_dialog_t* dialog);
void model_dialog_free(model_dialog_t* dialog);
#endif // MODELDIALOG_H_INCLUDED
