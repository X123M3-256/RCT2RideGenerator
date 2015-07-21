#ifndef INTERFACE_H_INCLUDED
#define INTERFACE_H_INCLUDED
#include "image.h"
#include <gtk/gtk.h>

typedef enum
{
VALUE_SIZE_BYTE,
VALUE_SIZE_WORD,
VALUE_SIZE_DWORD
}value_size_t;

typedef struct
{
void* value;
value_size_t value_size;

GtkWidget* container;
GtkWidget* label;
GtkWidget* spin_button;
}value_editor_t;

typedef struct
{
uint8_t* value;
uint8_t values[8];
int num_values;

GtkWidget* container;
GtkWidget* label;
GtkWidget* select;
}value_selector_t;


typedef struct
{
image_t* image;

GtkWidget* container;
GtkWidget* preview;
GdkPixbuf* pixbuf;
}image_viewer_t;

typedef struct
{
char** string;
GtkWidget* container;
GtkWidget* label;
GtkWidget* entry;
}string_editor_t;

typedef struct
{
float* value;
GtkWidget* container;
GtkWidget* label;
GtkWidget* spin_button;
}float_editor_t;

void show_error(char* message);

value_editor_t* value_editor_new(value_size_t size,const char* label);
void value_editor_set_value(value_editor_t* editor,void* value_ptr);

value_selector_t* value_selector_new(const char* label);
void value_selector_set_value(value_selector_t* selector,uint8_t* value);
void value_selector_add_selection(value_selector_t* selector,const char* name,uint8_t value);

float_editor_t* float_editor_new(const char* label,float min,float max);
void float_editor_set_float(float_editor_t* editor,float* float_ptr);

image_viewer_t* image_viewer_new();
void image_viewer_set_image(image_viewer_t* viewer,image_t* image);
void image_viewer_free(image_viewer_t* viewer);

string_editor_t* string_editor_new(const char* label);
void string_editor_set_string(string_editor_t* editor,char** string);

#endif // INTERFACE_H_INCLUDED
