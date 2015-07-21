#include <stdlib.h>
#include <string.h>
#include "interface.h"
#include "palette.h"

void show_error(char* message)
{
GtkWidget* dialog=gtk_message_dialog_new(NULL,0,GTK_MESSAGE_ERROR,GTK_BUTTONS_CLOSE,message);
gtk_dialog_run (GTK_DIALOG (dialog));
gtk_widget_destroy (dialog);
}


static void value_editor_changed(GtkWidget* widget,gpointer data)
{
value_editor_t* editor=(value_editor_t*)data;
    if(editor->value==NULL)return;
uint32_t value=gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(editor->spin_button));
    switch(editor->value_size)
    {
    case VALUE_SIZE_BYTE:
    *((uint8_t*)(editor->value))=value;
    break;
    case VALUE_SIZE_WORD:
    *((uint16_t*)(editor->value))=value;
    break;
    case VALUE_SIZE_DWORD:
    *((uint32_t*)(editor->value))=value;
    break;
    }
}
value_editor_t* value_editor_new(value_size_t size,const char* label)
{
value_editor_t* editor=malloc(sizeof(value_editor_t));
editor->value=NULL;
editor->value_size=size;

editor->container=gtk_hbox_new(FALSE,1);
editor->label=gtk_label_new(label);
    switch(editor->value_size)
    {
    case VALUE_SIZE_BYTE:
    editor->spin_button=gtk_spin_button_new_with_range(0,255,1);
    break;
    case VALUE_SIZE_WORD:
    editor->spin_button=gtk_spin_button_new_with_range(0,65535,1);
    break;
    case VALUE_SIZE_DWORD:
    editor->spin_button=gtk_spin_button_new_with_range(0,4294967296,1);
    break;
    }
gtk_widget_set_sensitive(editor->spin_button,FALSE);
gtk_box_pack_start(GTK_BOX(editor->container),editor->label,FALSE,FALSE,1);
gtk_box_pack_start(GTK_BOX(editor->container),editor->spin_button,FALSE,FALSE,1);
g_signal_connect(editor->spin_button,"value-changed",G_CALLBACK(value_editor_changed),editor);
return editor;
}
void value_editor_set_value(value_editor_t* editor,void* value_ptr)
{
editor->value=value_ptr;
uint32_t value=0;
    switch(editor->value_size)
    {
    case VALUE_SIZE_BYTE:
    value=*((uint8_t*)value_ptr);
    break;
    case VALUE_SIZE_WORD:
    value=*((uint16_t*)value_ptr);
    break;
    case VALUE_SIZE_DWORD:
    value=*((uint32_t*)value_ptr);
    break;
    }
gtk_widget_set_sensitive(editor->spin_button,TRUE);
gtk_spin_button_set_value(GTK_SPIN_BUTTON(editor->spin_button),value);
}

static void float_editor_changed(GtkWidget* widget,gpointer data)
{
float_editor_t* editor=(float_editor_t*)data;
    if(editor->value==NULL)return;
*(editor->value)=gtk_spin_button_get_value(GTK_SPIN_BUTTON(editor->spin_button));
}
float_editor_t* float_editor_new(const char* label,float min,float max)
{
float_editor_t* editor=malloc(sizeof(float_editor_t));
editor->value=NULL;

editor->container=gtk_hbox_new(FALSE,1);
editor->label=gtk_label_new(label);

editor->spin_button=gtk_spin_button_new_with_range(min,max,0.01);
gtk_spin_button_set_value(GTK_SPIN_BUTTON(editor->spin_button),0);

gtk_widget_set_sensitive(editor->spin_button,FALSE);
gtk_box_pack_start(GTK_BOX(editor->container),editor->label,FALSE,FALSE,1);
gtk_box_pack_start(GTK_BOX(editor->container),editor->spin_button,FALSE,FALSE,1);
g_signal_connect(editor->spin_button,"value-changed",G_CALLBACK(float_editor_changed),editor);
return editor;
}
void float_editor_set_float(float_editor_t* editor,float* value_ptr)
{
editor->value=value_ptr;
gtk_widget_set_sensitive(editor->spin_button,TRUE);
gtk_spin_button_set_value(GTK_SPIN_BUTTON(editor->spin_button),*value_ptr);
}

image_viewer_t* image_viewer_new()
{
image_viewer_t* viewer=malloc(sizeof(image_viewer_t));
viewer->image=NULL;

viewer->container=gtk_event_box_new();
gtk_widget_set_events(viewer->container,GDK_POINTER_MOTION_MASK);
viewer->preview=gtk_image_new();
viewer->pixbuf=gdk_pixbuf_new(GDK_COLORSPACE_RGB,FALSE,8,256,256);
//Set pixbuf to black
int rowstride=gdk_pixbuf_get_rowstride(viewer->pixbuf);
guchar* pixels=gdk_pixbuf_get_pixels(viewer->pixbuf);
int i,j;
    for(i=0;i<256;i++)
    {
        for(j=0;j<256*3;j++)
        {
        pixels[j]=0;
        }
    pixels+=rowstride;
    }

gtk_image_set_from_pixbuf(GTK_IMAGE(viewer->preview),viewer->pixbuf);
gtk_container_add(GTK_CONTAINER(viewer->container),viewer->preview);
return viewer;
}
void image_viewer_set_image(image_viewer_t* viewer,image_t* image)
{
int i,j;
int rowstride=gdk_pixbuf_get_rowstride(viewer->pixbuf);
guchar* pixels=gdk_pixbuf_get_pixels(viewer->pixbuf);
//Clear pixbuf
    for(i=0;i<256;i++)
    {
        for(j=0;j<256*3;j++)
        {
        pixels[j]=0;
        }
    pixels+=rowstride;
    }
//Draw new image to pixbuf
int x_offset=128+image->x_offset;//Draw image in centre of screen
int y_offset=128+image->y_offset;
pixels=gdk_pixbuf_get_pixels(viewer->pixbuf);
pixels+=y_offset*rowstride;

color_t curcol;
    for(i=0;i<image->height;i++)
    {
        for(j=0;j<image->width;j++)
        {
        curcol=palette_color_from_index(image->data[i][j]);
        int pixelindex=(j+x_offset)*3;
        pixels[pixelindex]=curcol.red;
        pixelindex++;
        pixels[pixelindex]=curcol.green;
        pixelindex++;
        pixels[pixelindex]=curcol.blue;
        pixelindex++;
        }
    pixels+=rowstride;
    }
gtk_image_set_from_pixbuf(GTK_IMAGE(viewer->preview),viewer->pixbuf);
}
void image_viewer_free(image_viewer_t* viewer)
{
gtk_widget_destroy(viewer->preview);
g_object_unref(G_OBJECT(viewer->pixbuf));
gtk_widget_destroy(viewer->container);
free(viewer);
}


static void string_editor_changed(GtkWidget* button,gpointer user_data)
{
string_editor_t* editor=(string_editor_t*)user_data;
    if(editor->string==NULL)return;
const char* text=gtk_entry_get_text(GTK_ENTRY(editor->entry));
*(editor->string)=realloc(*(editor->string),strlen(text)+1);
strcpy(*(editor->string),text);
}
string_editor_t* string_editor_new(const char* label)
{
string_editor_t* editor=malloc(sizeof(string_editor_t));
editor->container=gtk_hbox_new(FALSE,1);
editor->label=gtk_label_new(label);
editor->entry=gtk_entry_new();
gtk_box_pack_start(GTK_BOX(editor->container),editor->label,FALSE,FALSE,1);
gtk_box_pack_start(GTK_BOX(editor->container),editor->entry,TRUE,TRUE,1);
g_signal_connect(editor->entry,"changed",G_CALLBACK(string_editor_changed),editor);
return editor;
}
void string_editor_set_string(string_editor_t* editor,char** string)
{
editor->string=string;
gtk_entry_set_text(GTK_ENTRY(editor->entry),*string);
}


