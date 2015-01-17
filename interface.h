#ifndef INTERFACE_H_INCLUDED
#define INTERFACE_H_INCLUDED
#include "image.h"
#include <gtk/gtk.h>

typedef enum
{
VALUE_SIZE_BYTE,
VALUE_SIZE_WORD
}value_size_t;

typedef struct
{
uint32_t value;
value_size_t value_size;

GtkWidget* container;
GtkWidget* label;
GtkWidget* spin_button;
}value_editor_t;

typedef struct
{
image_t* image;

GtkWidget* container;
GtkWidget* preview;
GtkWidget* pixbuf;
}image_viewer_t;

typedef struct
{
char** string;
GtkWidget* container;
GtkWidget* label;
GtkWidget* entry;
}string_editor_t;

static void value_editor_changed(GtkWidget* widget,gpointer data);
value_editor_t* value_editor_new(value_size_t size,const char* label);
void value_editor_set_value(value_editor_t* editor,void* value_ptr);

image_viewer_t* image_viewer_new();
void image_viewer_set_image(image_viewer_t* viewer,image_t* image);
void image_viewer_free(image_viewer_t* viewer);

static void string_editor_changed(GtkWidget* button,gpointer user_data);
string_editor_t* string_editor_new(const char* label);
void string_editor_set_string(string_editor_t* editor,char** string);

#endif // INTERFACE_H_INCLUDED
