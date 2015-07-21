#include <stdlib.h>
#include <string.h>
#include <jansson.h>
#include "mainwindow.h"
#include "interface.h"
#include "modeldialog.h"
#include "animationdialog.h"
#include "project.h"
#include "serialization.h"
#include "ridetypes.h"
#include "palette.h"



char* get_filename(char* message,int action)
{
GtkWidget* file_dialog=gtk_file_chooser_dialog_new(message,NULL,action,GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,GTK_STOCK_OK,GTK_RESPONSE_OK,NULL);
char* filename=NULL;
if(gtk_dialog_run(GTK_DIALOG(file_dialog))==GTK_RESPONSE_OK)
    {
    filename=gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(file_dialog));
    };
gtk_widget_destroy(file_dialog);
return filename;
}





static void track_type_editor_changed(GtkWidget* widget,gpointer data)
{
track_type_editor_t* editor=(track_type_editor_t*)data;
    if(editor->track_type==NULL)return;
//Get text from combo box
const char* text=gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(widget));
    if(text==NULL)return;
*(editor->track_type)=ride_type_by_name(text)->id;
}
track_type_editor_t* track_type_editor_new()
{
int i;
track_type_editor_t* editor=malloc(sizeof(track_type_editor_t));
editor->track_type=NULL;
editor->container=gtk_hbox_new(FALSE,1);
editor->label=gtk_label_new("Track type:");
editor->select=gtk_combo_box_text_new();
gtk_widget_set_sensitive(editor->select,FALSE);
gtk_box_pack_start(GTK_BOX(editor->container),editor->label,FALSE,FALSE,1);
gtk_box_pack_start(GTK_BOX(editor->container),editor->select,FALSE,FALSE,1);
    for(i=0;i<NUM_RIDE_TYPES;i++)
    {
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(editor->select),ride_type_by_index(i)->name);
    }
g_signal_connect(editor->select,"changed",G_CALLBACK(track_type_editor_changed),editor);
return editor;
}
void track_type_editor_set_track_type(track_type_editor_t* editor,uint8_t* track_type)
{
int i;
editor->track_type=track_type;
gtk_widget_set_sensitive(editor->select,TRUE);
    for(i=0;i<NUM_RIDE_TYPES;i++)
    {
        if(ride_type_by_index(i)->id==*track_type)
        {
        gtk_combo_box_set_active(GTK_COMBO_BOX(editor->select),i);
        break;
        }
    }
}



void flag_editor_checkbox_toggled(GtkWidget* widget,gpointer data)
{
flag_checkbox_t* flag_checkbox=(flag_checkbox_t*)data;
    if(flag_checkbox->flags==NULL)return;
int value=gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(flag_checkbox->checkbox));
    if(value) *(flag_checkbox->flags)|=flag_checkbox->flag;
    else *(flag_checkbox->flags)&=~flag_checkbox->flag;
}
flag_editor_t* flag_editor_new(const char* label)
{
flag_editor_t* editor=malloc(sizeof(flag_editor_t));
editor->flags=NULL;
editor->num_checkboxes=0;
editor->flag_checkboxes=NULL;
editor->container=gtk_frame_new(label);
editor->table=gtk_table_new(FALSE,1,0);
gtk_container_add(GTK_CONTAINER(editor->container),editor->table);
return editor;
}
void flag_editor_add_checkbox(flag_editor_t* editor,const char* label,uint32_t flag)
{
editor->flag_checkboxes=realloc(editor->flag_checkboxes,(editor->num_checkboxes+1)*sizeof(flag_checkbox_t*));
editor->flag_checkboxes[editor->num_checkboxes]=malloc(sizeof(flag_checkbox_t));
flag_checkbox_t* checkbox=editor->flag_checkboxes[editor->num_checkboxes++];
checkbox->flags=NULL;
checkbox->flag=flag;
checkbox->label=gtk_label_new(label);
checkbox->checkbox=gtk_check_button_new();
gtk_widget_set_sensitive(checkbox->checkbox,FALSE);
gtk_table_resize(GTK_TABLE(editor->table),editor->num_checkboxes,2);
gtk_table_attach_defaults(GTK_TABLE(editor->table),checkbox->label,0,1,editor->num_checkboxes-1,editor->num_checkboxes);
gtk_table_attach_defaults(GTK_TABLE(editor->table),checkbox->checkbox,1,2,editor->num_checkboxes-1,editor->num_checkboxes);
g_signal_connect(checkbox->checkbox,"toggled",G_CALLBACK(flag_editor_checkbox_toggled),checkbox);

}
void flag_editor_set_flags(flag_editor_t* editor,uint32_t* flags)
{
int i;
editor->flags=flags;
    for(i=0;i<editor->num_checkboxes;i++)
    {
    editor->flag_checkboxes[i]->flags=flags;
    gtk_widget_set_sensitive(editor->flag_checkboxes[i]->checkbox,TRUE);
        if((*flags)&editor->flag_checkboxes[i]->flag)gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(editor->flag_checkboxes[i]->checkbox),TRUE);
        else gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(editor->flag_checkboxes[i]->checkbox),FALSE);
    }
}


void car_type_editor_changed(GtkWidget* widget,gpointer data)
{
car_type_editor_t* editor=(car_type_editor_t*)data;
    if(editor->car_type==NULL)return;
int active=gtk_combo_box_get_active(GTK_COMBO_BOX(editor->car_select));
    switch(active)
    {
    case 0:
    *(editor->car_type)=0xFF;
    break;
    case 1:
    case 2:
    case 3:
    case 4:
    *(editor->car_type)=active-1;
    break;
    }
}
car_type_editor_t* car_type_editor_new(const char* label)
{
int i;
car_type_editor_t* editor=malloc(sizeof(car_type_editor_t));
editor->car_type=NULL;
editor->container=gtk_hbox_new(FALSE,1);
editor->label=gtk_label_new(label);
editor->car_select=gtk_combo_box_text_new();
gtk_widget_set_sensitive(editor->car_select,FALSE);
gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(editor->car_select),"Default");
    for(i=0;i<NUM_CARS;i++)
    {
    char option_text[256];
    sprintf(option_text,"Car %d",i);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(editor->car_select),option_text);
    }
gtk_combo_box_set_active(GTK_COMBO_BOX(editor->car_select),0);
g_signal_connect(editor->car_select,"changed",G_CALLBACK(car_type_editor_changed),editor);
gtk_box_pack_start(GTK_BOX(editor->container),editor->label,FALSE,FALSE,2);
gtk_box_pack_start(GTK_BOX(editor->container),editor->car_select,FALSE,FALSE,2);
return editor;
}
void car_type_editor_set_car_type(car_type_editor_t* editor,uint8_t* car_type)
{
editor->car_type=car_type;
gtk_widget_set_sensitive(editor->car_select,TRUE);
    switch(*car_type)
    {
    case 0xFF:
    gtk_combo_box_set_active(GTK_COMBO_BOX(editor->car_select),0);
    break;
    case 0:
    case 1:
    case 2:
    case 3:
    gtk_combo_box_set_active(GTK_COMBO_BOX(editor->car_select),(*car_type)+1);
    break;
    }
}

void car_editor_edit_animation(GtkWidget* widget,gpointer data)
{
car_editor_t* editor=(car_editor_t*)data;
    if(editor->project==NULL||editor->car_settings==NULL)return;
int variable_flags=0;
    if(editor->car_settings->flags&CAR_IS_SPINNING)variable_flags|=ANIMATION_DIALOG_SPIN;
    if(editor->car_settings->flags&CAR_IS_SWINGING)variable_flags|=ANIMATION_DIALOG_SWING;
    if(editor->car_settings->sprites&SPRITE_RESTRAINT_ANIMATION)variable_flags|=ANIMATION_DIALOG_RESTRAINT;
animation_dialog_t* dialog=animation_dialog_new(editor->car_settings->animation,editor->project->models,editor->project->num_models,variable_flags);
animation_dialog_run(dialog);
animation_dialog_free(dialog);
}

car_editor_t* car_editor_new()
{
car_editor_t* editor=malloc(sizeof(car_editor_t));
editor->car_settings=NULL;
editor->container=gtk_vbox_new(FALSE,1);

editor->flag_editor=flag_editor_new("Flags");
flag_editor_add_checkbox(editor->flag_editor,"Enable remap color 2",CAR_ENABLE_REMAP2);
flag_editor_add_checkbox(editor->flag_editor,"Enable remap color 3",CAR_ENABLE_REMAP3);
flag_editor_add_checkbox(editor->flag_editor,"No upstops",CAR_NO_UPSTOPS);
flag_editor_add_checkbox(editor->flag_editor,"Fake spinning",CAR_FAKE_SPINNING);
flag_editor_add_checkbox(editor->flag_editor,"Swinging",CAR_IS_SWINGING);
gtk_box_pack_start(GTK_BOX(editor->container),editor->flag_editor->container,FALSE,FALSE,1);

editor->sprite_editor=flag_editor_new("Sprites");
flag_editor_add_checkbox(editor->sprite_editor,"Gentle slopes",SPRITE_GENTLE_SLOPE);
flag_editor_add_checkbox(editor->sprite_editor,"Steep slopes",SPRITE_STEEP_SLOPE);
flag_editor_add_checkbox(editor->sprite_editor,"Vertical slopes/Loops",SPRITE_VERTICAL_SLOPE);
flag_editor_add_checkbox(editor->sprite_editor,"Diagonal slopes",SPRITE_DIAGONAL_SLOPE);
flag_editor_add_checkbox(editor->sprite_editor,"Banked turns",SPRITE_BANKING);
flag_editor_add_checkbox(editor->sprite_editor,"Sloped banked turns",SPRITE_SLOPED_BANKED_TURN);
flag_editor_add_checkbox(editor->sprite_editor,"Inline twists",SPRITE_INLINE_TWIST);
flag_editor_add_checkbox(editor->sprite_editor,"Corkscrews",SPRITE_CORKSCREW);
flag_editor_add_checkbox(editor->sprite_editor,"Animated restraints",SPRITE_RESTRAINT_ANIMATION);
gtk_box_pack_start(GTK_BOX(editor->container),editor->sprite_editor->container,FALSE,FALSE,1);

editor->spacing_editor=value_editor_new(VALUE_SIZE_DWORD,"Spacing:");
editor->friction_editor=value_editor_new(VALUE_SIZE_WORD,"Friction:");
editor->z_value_editor=value_editor_new(VALUE_SIZE_BYTE,"Z Value:");

editor->animation_button=gtk_button_new_with_label("Edit Animation");
g_signal_connect(editor->animation_button,"clicked",G_CALLBACK(car_editor_edit_animation),editor);

gtk_box_pack_start(GTK_BOX(editor->container),editor->animation_button,FALSE,FALSE,1);
gtk_box_pack_start(GTK_BOX(editor->container),editor->spacing_editor->container,FALSE,FALSE,1);
gtk_box_pack_start(GTK_BOX(editor->container),editor->friction_editor->container,FALSE,FALSE,1);
gtk_box_pack_start(GTK_BOX(editor->container),editor->z_value_editor->container,FALSE,FALSE,1);


return editor;
}
void car_editor_set_project(car_editor_t* editor,project_t* project)
{
editor->project=project;
}

void car_editor_set_car(car_editor_t* editor,car_settings_t* car_settings)
{
editor->car_settings=car_settings;
flag_editor_set_flags(editor->flag_editor,&(car_settings->flags));
flag_editor_set_flags(editor->sprite_editor,&(car_settings->sprites));
value_editor_set_value(editor->spacing_editor,&(car_settings->spacing));
value_editor_set_value(editor->friction_editor,&(car_settings->friction));
value_editor_set_value(editor->z_value_editor,&(car_settings->z_value));
}


header_editor_t* header_editor_new()
{
header_editor_t* editor=malloc(sizeof(header_editor_t));
editor->project=NULL;
editor->container=gtk_vbox_new(FALSE,2);


editor->track_type_editor=track_type_editor_new();
gtk_box_pack_start(GTK_BOX(editor->container),editor->track_type_editor->container,FALSE,FALSE,2);

editor->flag_editor=flag_editor_new("Flags");
flag_editor_add_checkbox(editor->flag_editor,"Show as seperate ride",RIDE_SEPERATE);
flag_editor_add_checkbox(editor->flag_editor,"Ride is covered",RIDE_COVERED);
flag_editor_add_checkbox(editor->flag_editor,"Riders get wet",RIDE_WET);
flag_editor_add_checkbox(editor->flag_editor,"Ride slows down in water",RIDE_SLOW_IN_WATER);
gtk_box_pack_start(GTK_BOX(editor->container),editor->flag_editor->container,FALSE,FALSE,2);

editor->excitement_editor=value_editor_new(VALUE_SIZE_BYTE,"Excitement:");
gtk_box_pack_start(GTK_BOX(editor->container),editor->excitement_editor->container,FALSE,FALSE,2);
editor->intensity_editor=value_editor_new(VALUE_SIZE_BYTE,"Intensity:");
gtk_box_pack_start(GTK_BOX(editor->container),editor->intensity_editor->container,FALSE,FALSE,2);
editor->nausea_editor=value_editor_new(VALUE_SIZE_BYTE,"Nausea:");
gtk_box_pack_start(GTK_BOX(editor->container),editor->nausea_editor->container,FALSE,FALSE,2);
editor->max_height_editor=value_editor_new(VALUE_SIZE_BYTE,"Max height increase:");
gtk_box_pack_start(GTK_BOX(editor->container),editor->max_height_editor->container,FALSE,FALSE,2);

//Edit car related information
GtkWidget* cars_frame=gtk_frame_new("Cars");
GtkWidget* cars_vbox=gtk_vbox_new(FALSE,1);
gtk_container_add(GTK_CONTAINER(cars_frame),cars_vbox);
gtk_box_pack_start(GTK_BOX(editor->container),cars_frame,FALSE,FALSE,2);


editor->min_cars_editor=value_editor_new(VALUE_SIZE_BYTE,"Minimum cars per train:");
editor->max_cars_editor=value_editor_new(VALUE_SIZE_BYTE,"Maximum cars per train:");
gtk_box_pack_start(GTK_BOX(cars_vbox),editor->min_cars_editor->container,FALSE,FALSE,2);
gtk_box_pack_start(GTK_BOX(cars_vbox),editor->max_cars_editor->container,FALSE,FALSE,2);
editor->zero_cars_editor=value_editor_new(VALUE_SIZE_BYTE,"Number of zero cars:");
gtk_box_pack_start(GTK_BOX(cars_vbox),editor->zero_cars_editor->container,FALSE,FALSE,2);
editor->car_icon_index_editor=value_editor_new(VALUE_SIZE_BYTE,"Car icon index:");
gtk_box_pack_start(GTK_BOX(cars_vbox),editor->car_icon_index_editor->container,FALSE,FALSE,2);

editor->default_car_editor=car_type_editor_new("Default car");
editor->front_car_editor=car_type_editor_new("First car");
editor->second_car_editor=car_type_editor_new("Second car");
editor->third_car_editor=car_type_editor_new("Third car");
editor->rear_car_editor=car_type_editor_new("Rear car");
gtk_box_pack_start(GTK_BOX(cars_vbox),editor->default_car_editor->container,FALSE,FALSE,2);
gtk_box_pack_start(GTK_BOX(cars_vbox),editor->front_car_editor->container,FALSE,FALSE,2);
gtk_box_pack_start(GTK_BOX(cars_vbox),editor->second_car_editor->container,FALSE,FALSE,2);
gtk_box_pack_start(GTK_BOX(cars_vbox),editor->third_car_editor->container,FALSE,FALSE,2);
gtk_box_pack_start(GTK_BOX(cars_vbox),editor->rear_car_editor->container,FALSE,FALSE,2);


int i;
GtkWidget* cars=gtk_notebook_new();
    for(i=0;i<NUM_CARS;i++)
    {
    char label_text[256];
    sprintf(label_text,"Car %d",i);
    GtkWidget* page_label=gtk_label_new(label_text);
    editor->car_editors[i]=car_editor_new();
    gtk_notebook_append_page(GTK_NOTEBOOK(cars),editor->car_editors[i]->container,page_label);
    }
gtk_box_pack_start(GTK_BOX(editor->container),cars,FALSE,FALSE,2);
return editor;
}
void header_editor_set_project(header_editor_t* editor,project_t* project)
{
int i;
editor->project=project;
track_type_editor_set_track_type(editor->track_type_editor,&(project->track_type));
flag_editor_set_flags(editor->flag_editor,&(project->flags));
value_editor_set_value(editor->excitement_editor,&(project->excitement));
value_editor_set_value(editor->intensity_editor,&(project->intensity));
value_editor_set_value(editor->nausea_editor,&(project->nausea));
value_editor_set_value(editor->max_height_editor,&(project->max_height));
value_editor_set_value(editor->min_cars_editor,&(project->minimum_cars));
value_editor_set_value(editor->max_cars_editor,&(project->maximum_cars));
value_editor_set_value(editor->zero_cars_editor,&(project->zero_cars));
value_editor_set_value(editor->car_icon_index_editor,&(project->car_icon_index));
car_type_editor_set_car_type(editor->default_car_editor,&(project->car_types[CAR_INDEX_DEFAULT]));
car_type_editor_set_car_type(editor->front_car_editor,&(project->car_types[CAR_INDEX_FRONT]));
car_type_editor_set_car_type(editor->second_car_editor,&(project->car_types[CAR_INDEX_SECOND]));
car_type_editor_set_car_type(editor->third_car_editor,&(project->car_types[CAR_INDEX_THIRD]));
car_type_editor_set_car_type(editor->rear_car_editor,&(project->car_types[CAR_INDEX_REAR]));
    for(i=0;i<NUM_CARS;i++)
    {
    car_editor_set_project(editor->car_editors[i],project);
    car_editor_set_car(editor->car_editors[i],&(project->cars[i]));
    }
}

static void preview_editor_set_preview_pressed(GtkWidget* widget,gpointer data)
{
preview_editor_t* editor=(preview_editor_t*)data;

char* filename=get_filename("Select preview image",GTK_FILE_CHOOSER_ACTION_OPEN);

    if(filename==NULL)return;

GError* error=NULL;
GdkPixbuf* pixbuf=gdk_pixbuf_new_from_file(filename,&error);
        if(!pixbuf)
        {
        show_error(error->message);
        return;
        }

        if(gdk_pixbuf_get_width(pixbuf)!=112||gdk_pixbuf_get_height(pixbuf)!=112)
        {
        show_error("Preview images must be 112 by 112 pixels");
        return;
        }

image_free(*(editor->image));
image_t* image=image_new(112,112,0);
//Set image from pixbuf
int rowstride=gdk_pixbuf_get_rowstride(pixbuf);
guchar* pixels=gdk_pixbuf_get_pixels(pixbuf);
color_t color;
int i,j;
    for(i=0;i<112;i++)
    {
        for(j=0;j<112;j++)
        {
        color.red=pixels[j*3];
        color.green=pixels[j*3+1];
        color.blue=pixels[j*3+2];
        image->data[i][j]=palette_index_from_color(color);
        }
    pixels+=rowstride;
    }
*(editor->image)=image;
image_viewer_set_image(editor->preview_viewer,image);
}
preview_editor_t* preview_editor_new()
{
preview_editor_t* editor=malloc(sizeof(preview_editor_t));
editor->image=NULL;
editor->container=gtk_vbox_new(FALSE,2);

editor->preview_viewer=image_viewer_new();
gtk_box_pack_start(GTK_BOX(editor->container),editor->preview_viewer->container,FALSE,FALSE,2);

editor->set_preview=gtk_button_new_with_label("Set preview image");
g_signal_connect(editor->set_preview,"clicked",G_CALLBACK(preview_editor_set_preview_pressed),editor);
gtk_box_pack_start(GTK_BOX(editor->container),editor->set_preview,FALSE,FALSE,2);

return editor;
}
void preview_editor_set_image(preview_editor_t* editor,image_t** image)
{
editor->image=image;
image_viewer_set_image(editor->preview_viewer,*image);
}


static gboolean main_window_delete_event(GtkWidget* widget,GdkEvent* event,gpointer data){
return FALSE;
}
static void main_window_exit(GtkWidget* widget,gpointer data)
{
gtk_main_quit();
}


static void main_window_add_model(GtkWidget* widget,gpointer data)
{
main_window_t* main_window=(main_window_t*)data;
    if(main_window->project==NULL)return;
char* filename=get_filename("Select file to open",GTK_FILE_CHOOSER_ACTION_OPEN);
    if(filename!=NULL)
    {
    model_t* model=model_load_obj(filename);
        if(model!=NULL)
        {
        project_add_model(main_window->project,model);
        model_dialog_t* dialog=model_dialog_new(model);
        model_dialog_run(dialog);
        model_dialog_free(dialog);
        }
    }
}
static void main_window_edit_model(GtkWidget* widget,gpointer data)
{
model_t* model=(model_t*)data;
        if(model!=NULL)
        {
        model_dialog_t* dialog=model_dialog_new(model);
        model_dialog_run(dialog);
        model_dialog_free(dialog);
        gtk_menu_item_set_label(GTK_MENU_ITEM(widget),model->name);
        }
}


void main_window_populate_model_menu(main_window_t* main_window)
{
int i;
    for(i=0;i<main_window->project->num_models;i++)
    {
    //Add model to menu
    GtkWidget* model_menu_item=gtk_menu_item_new_with_label(main_window->project->models[i]->name);
    gtk_menu_shell_append(GTK_MENU_SHELL(main_window->model_menu),model_menu_item);
    g_signal_connect(model_menu_item,"activate",G_CALLBACK(main_window_edit_model),main_window->project->models[i]);
    gtk_widget_show(model_menu_item);
    }
}

void main_window_set_project(main_window_t* main_window,project_t* project)
{
main_window->project=project;
preview_editor_set_image(main_window->preview_editor,&(project->preview_image));
string_editor_set_string(main_window->name_editor,&(project->name));
string_editor_set_string(main_window->description_editor,&(project->description));
header_editor_set_project(main_window->header_editor,project);
main_window_populate_model_menu(main_window);
}
static void main_window_new_project(GtkWidget* widget,gpointer data)
{
main_window_t* main_window=(main_window_t*)data;
main_window_set_project(main_window,project_new());
}
static void main_window_open_project(GtkWidget* widget,gpointer data)
{
main_window_t* main_window=(main_window_t*)data;
char* filename=get_filename("Select file to open",GTK_FILE_CHOOSER_ACTION_OPEN);
    if(filename!=NULL)
    {
    project_t* project=project_load(filename);
        if(project!=NULL)main_window_set_project(main_window,project);
        else show_error("Failed to load project file\n");
    }
}
static void main_window_save_project(GtkWidget* widget,gpointer data)
{
main_window_t* main_window=(main_window_t*)data;
    if(main_window->project==NULL)
    {
    //TODO: show error
    return;
    }
char* filename=get_filename("Enter filename",GTK_FILE_CHOOSER_ACTION_SAVE);
    if(filename!=NULL)project_save(filename,main_window->project);
}
static void main_window_export_project(GtkWidget* widget,gpointer data)
{
main_window_t* main_window=(main_window_t*)data;
char* filename=get_filename("Enter filename",GTK_FILE_CHOOSER_ACTION_SAVE);
    if(filename!=NULL)
    {
    object_t* object=project_export_dat(main_window->project);
    object_save_dat(object,filename,main_window->project->id);
    object_free(object);
    }
}


void main_window_build_menus(main_window_t* main_window)
{
//Set up the menus
main_window->main_menu=gtk_menu_bar_new();
GtkWidget* file_menu_item=gtk_menu_item_new_with_label("File");
GtkWidget* model_menu_item=gtk_menu_item_new_with_label("Model");
//GtkWidget* animation_menu_item=gtk_menu_item_new_with_label("Animation");
gtk_menu_shell_append(GTK_MENU_SHELL(main_window->main_menu),file_menu_item);
gtk_menu_shell_append(GTK_MENU_SHELL(main_window->main_menu),model_menu_item);
//gtk_menu_shell_append(GTK_MENU_SHELL(main_window->main_menu),animation_menu_item);

GtkWidget* file_menu=gtk_menu_new();
GtkWidget* new_menu_item=gtk_menu_item_new_with_label("New");
GtkWidget* open_menu_item=gtk_menu_item_new_with_label("Open");
GtkWidget* save_menu_item=gtk_menu_item_new_with_label("Save");
GtkWidget* export_menu_item=gtk_menu_item_new_with_label("Export DAT");
g_signal_connect(new_menu_item,"activate",G_CALLBACK(main_window_new_project),main_window);
g_signal_connect(open_menu_item,"activate",G_CALLBACK(main_window_open_project),main_window);
g_signal_connect(save_menu_item,"activate",G_CALLBACK(main_window_save_project),main_window);
g_signal_connect(export_menu_item,"activate",G_CALLBACK(main_window_export_project),main_window);
gtk_menu_shell_append(GTK_MENU_SHELL(file_menu),new_menu_item);
gtk_menu_shell_append(GTK_MENU_SHELL(file_menu),open_menu_item);
gtk_menu_shell_append(GTK_MENU_SHELL(file_menu),save_menu_item);
gtk_menu_shell_append(GTK_MENU_SHELL(file_menu),export_menu_item);
gtk_menu_item_set_submenu(GTK_MENU_ITEM(file_menu_item),file_menu);

main_window->model_menu=gtk_menu_new();
GtkWidget* add_model_menu_item=gtk_menu_item_new_with_label("Add Model");
g_signal_connect(add_model_menu_item,"activate",G_CALLBACK(main_window_add_model),main_window);
gtk_menu_shell_append(GTK_MENU_SHELL(main_window->model_menu),add_model_menu_item);
gtk_menu_item_set_submenu(GTK_MENU_ITEM(model_menu_item),main_window->model_menu);

/*
main_window->animation_menu=gtk_menu_new();
GtkWidget* add_animation_menu_item=gtk_menu_item_new_with_label("Add Animation");
g_signal_connect(add_animation_menu_item,"activate",G_CALLBACK(main_window_add_animation),main_window);
gtk_menu_shell_append(GTK_MENU_SHELL(main_window->animation_menu),add_animation_menu_item);
gtk_menu_item_set_submenu(GTK_MENU_ITEM(animation_menu_item),main_window->animation_menu);
//g_signal_connect(addAnimationMenuItem,"activate",G_CALLBACK(AddNewAnimation),MainInterface);
*/

gtk_box_pack_start(GTK_BOX(main_window->main_vbox),main_window->main_menu,FALSE,FALSE,0);
}


main_window_t* main_window_new()
{
main_window_t* main_window=malloc(sizeof(main_window_t));
main_window->project=NULL;

main_window->window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
g_signal_connect(main_window->window,"delete-event",G_CALLBACK(main_window_delete_event),NULL);
g_signal_connect(main_window->window,"destroy",G_CALLBACK(main_window_exit),NULL);

main_window->main_vbox=gtk_vbox_new(FALSE,5);
gtk_container_add(GTK_CONTAINER(main_window->window),main_window->main_vbox);

main_window_build_menus(main_window);

main_window->main_hbox=gtk_hbox_new(FALSE,2);
gtk_box_pack_start(GTK_BOX(main_window->main_vbox),main_window->main_hbox,FALSE,FALSE,2);
main_window->left_vbox=gtk_vbox_new(FALSE,2);
gtk_box_pack_start(GTK_BOX(main_window->main_hbox),main_window->left_vbox,FALSE,FALSE,2);

main_window->preview_editor=preview_editor_new();
gtk_box_pack_start(GTK_BOX(main_window->left_vbox),main_window->preview_editor->container,FALSE,FALSE,2);

main_window->name_editor=string_editor_new("Name:");
gtk_box_pack_start(GTK_BOX(main_window->left_vbox),main_window->name_editor->container,FALSE,FALSE,2);

main_window->description_editor=string_editor_new("Description:");
gtk_box_pack_start(GTK_BOX(main_window->left_vbox),main_window->description_editor->container,FALSE,FALSE,2);

main_window->header_editor=header_editor_new();
gtk_box_pack_start(GTK_BOX(main_window->main_hbox),main_window->header_editor->container,FALSE,FALSE,2);

gtk_widget_show_all(main_window->window);

return main_window;
}

void main_window_free(main_window_t* main_window)
{
//gtk_widget_destroy(main_window->window);
free(main_window);
}
