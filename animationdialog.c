#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<assert.h>
#include "renderer.h"
#include "animationdialog.h"
#include "image.h"


void animation_viewer_update(animation_viewer_t* viewer)
{
renderer_clear_buffers();
    if(viewer->animation!=NULL)animation_render(viewer->animation,viewer->model_view,viewer->variables);
renderer_render_model(viewer->grid_model,viewer->model_view);
image_t* image=renderer_get_image();
image_viewer_set_image(viewer->image_viewer,image);
image_free(image);
}

animation_viewer_t* animation_viewer_new()
{
animation_viewer_t* viewer=malloc(sizeof(animation_viewer_t));
viewer->animation=NULL;
viewer->model_view=MatrixIdentity();
viewer->model_view.Data[0]=-4;
viewer->model_view.Data[5]=4;
viewer->model_view.Data[10]=-4;


viewer->grid_model=model_new_grid();
viewer->image_viewer=image_viewer_new();
viewer->container=gtk_vbox_new(FALSE,1);
gtk_box_pack_start(GTK_BOX(viewer->container),viewer->image_viewer->container,FALSE,FALSE,2);

animation_viewer_update(viewer);
return viewer;
}


void animation_viewer_set_animation(animation_viewer_t* viewer,animation_t* animation)
{
viewer->animation=animation;
animation_viewer_update(viewer);
}
/*
static void vector_editor_changed(GtkWidget* widget,gpointer data)
{
vector_editor_t* editor=(vector_editor_t*)data;
editor->vector.X=gtk_spin_button_get_value_as_float(GTK_SPIN_BUTTON(editor->x));
editor->vector.Y=gtk_spin_button_get_value_as_float(GTK_SPIN_BUTTON(editor->y));
editor->vector.Zhttps://duckduckgo.com/?q=CUDA&ia=about=gtk_spin_button_get_value_as_float(GTK_SPIN_BUTTON(editor->z));
}
void vector_editor_register_callback(vector_editor_t* editor,void (*callback)(GtkWidget* widget,gpointer data),gpointer data)
{https://duckduckgo.com/?q=CUDA&ia=about
g_signal_connect(editor->x,"value-changed",G_CALLBACK(callback),data);
g_signal_connect(editor->y,"value-changed",G_CALLBACK(callback),data);
g_signal_connect(editor->z,"value-changed",G_CALLBACK(callback),data);
}
vector_editor_t* vector_editor_new(const char* label,float min,float max,float step)
{
vector_editor_t* editor=malloc(sizeof(vector_editor_t));
editor->vector.X=0.0;
https://duckduckgo.com/?q=CUDA&ia=abouteditor->vector.Y=0.0;
editor->vector.Z=0.0;
editor->container=gtk_hbox_new(FALSE,1);
https://duckduckgo.com/?q=CUDA&ia=abouteditor->label=gtk_label_new(label);
editor->x=gtk_spin_button_new_with_range(min,max,step);
editor->y=gtk_spin_button_new_with_range(min,max,step);
editor->z=gtk_spin_button_new_with_range(min,max,step);
vector_editor_register_callback(editor,vector_editor_changed,editor);
gtk_box_pack_start(GTK_BOX(editor->container),editor->label,FALSE,FALSE,1);
gtk_box_pack_start(GTK_BOX(editor->container),editor->x,FALSE,FALSE,1);
gtk_box_pack_start(GTK_BOX(editor->container),editor->y,FALSE,FALSE,1);
gtk_box_pack_start(GTK_BOX(editor->container),editor->z,FALSE,FALSE,1);
return editor;
}
void vector_editor_set_value(vector_editor_t* editor,Vector vector)
{
editor->vector=vector;
gtk_spin_button_set_value(GTK_SPIN_BUTTON(editor->x),vector.X);
gtk_spin_button_set_value(GTK_SPIN_BUTTON(editor->y),vector.Y);
gtk_spin_button_set_value(GTK_SPIN_BUTTON(editor->z),vector.Z);
}

static void object_transform_editor_changed(GtkWidget* widget,gpointer data)
{
object_transform_editor_t* editor=(object_transform_editor_t*)data;
    if(editor->object_transform==NULL)return;
Vector radians;
radians.X=editor->rotation_editor->vector.X*3.141592654/180.0;
radians.Y=editor->rotation_editor->vector.Y*3.141592654/180.0;
radians.Z=editor->rotation_editor->vector.Z*3.141592654/180.0;
animation_update_transform(editor->object_transform,editor->position_editor->vector,radians);
}
void object_transform_editor_register_callback(object_transform_editor_t* editor,void (*callback)(GtkWidget* widget,gpointer data),gpointer data)
{
vector_editor_register_callback(editor->position_editor,callback,data);
vector_editor_register_callback(editor->rotation_editor,callback,data);
}
object_transform_editor_t* object_transform_editor_new()
{
object_transform_editor_t* editor=malloc(sizeof(object_transform_editor_t));
editor->container=gtk_vbox_new(TRUE,1);
editor->position_editor=vector_editor_new("Position",-10,10,0.1);
editor->rotation_editor=vector_editor_new("Rotation",0,360,1);
object_transform_editor_register_callback(editor,object_transform_editor_changed,editor);
gtk_box_pack_start(GTK_BOX(editor->container),editor->position_editor->container,FALSE,FALSE,2);
gtk_box_pack_start(GTK_BOX(editor->container),editor->rotation_editor->container,FALSE,FALSE,2);
return editor;
}
void object_transform_editor_set_object_transform(object_transform_editor_t* editor,object_transform_t* object_transform)
{
//Setting the spin button values triggers a cascade of update events. This is the simplest way of making sure they don't corrupt anything
editor->object_transform=NULL;

vector_editor_set_value(editor->position_editor,object_transform->position);
Vector degrees;
degrees.X=object_transform->rotation.X*180.0/3.141592654;
degrees.Y=object_transform->rotation.Y*180.0/3.141592654;
degrees.Z=object_transform->rotation.Z*180.0/3.141592654;
vector_editor_set_value(editor->rotation_editor,degrees);

editor->object_transform=object_transform;
}

*/
model_selector_t* model_selector_new(model_t** models,int num_models)
{
int i;
model_selector_t* selector=malloc(sizeof(model_selector_t));
selector->models=models;
selector->num_models=num_models;
selector->container=gtk_combo_box_text_new();
    for(i=0;i<num_models;i++)
    {
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(selector->container),models[i]->name);
    }
return selector;
}
model_t* model_selector_get_model(model_selector_t* model_selector)
{
int index=gtk_combo_box_get_active(GTK_COMBO_BOX(model_selector->container));
    if(index<0)return NULL;
return model_selector->models[index];
}

static void animation_object_editor_x_entry_changed(GtkWidget *widget, gpointer user_data)
{
animation_object_editor_t* editor=(animation_object_editor_t*)user_data;
    if(editor->object==NULL)return;

const char* error;
const char* str=gtk_entry_get_text(GTK_ENTRY(editor->x_entry));
animation_expression_parse(editor->object->position[0],str,&error);

    if(error!=NULL)
    {
    gtk_label_set_text(GTK_LABEL(editor->error_label),error);
    GdkColor red;
    red.blue=20000;
    red.green=20000;
    red.red=65535;
    gtk_widget_modify_base(editor->x_entry,GTK_STATE_NORMAL,&red);
    }
    else
    {
    gtk_label_set_text(GTK_LABEL(editor->error_label),"No error");
    gtk_widget_modify_base(editor->x_entry,GTK_STATE_NORMAL,NULL);
    }
}
static void animation_object_editor_y_entry_changed(GtkWidget *widget, gpointer user_data)
{
animation_object_editor_t* editor=(animation_object_editor_t*)user_data;
    if(editor->object==NULL)return;

const char* error;
const char* str=gtk_entry_get_text(GTK_ENTRY(editor->y_entry));
animation_expression_parse(editor->object->position[1],str,&error);

    if(error!=NULL)
    {
    gtk_label_set_text(GTK_LABEL(editor->error_label),error);
    GdkColor red;
    red.blue=20000;
    red.green=20000;
    red.red=65535;
    gtk_widget_modify_base(editor->y_entry,GTK_STATE_NORMAL,&red);
    }
    else
    {
    gtk_label_set_text(GTK_LABEL(editor->error_label),"No error");
    gtk_widget_modify_base(editor->y_entry,GTK_STATE_NORMAL,NULL);
    }
}
static void animation_object_editor_z_entry_changed(GtkWidget *widget, gpointer user_data)
{
animation_object_editor_t* editor=(animation_object_editor_t*)user_data;
    if(editor->object==NULL)return;

const char* error;
const char* str=gtk_entry_get_text(GTK_ENTRY(editor->z_entry));
animation_expression_parse(editor->object->position[2],str,&error);

    if(error!=NULL)
    {
    gtk_label_set_text(GTK_LABEL(editor->error_label),error);
    GdkColor red;
    red.blue=20000;
    red.green=20000;
    red.red=65535;
    gtk_widget_modify_base(editor->z_entry,GTK_STATE_NORMAL,&red);
    }
    else
    {
    gtk_label_set_text(GTK_LABEL(editor->error_label),"No error");
    gtk_widget_modify_base(editor->z_entry,GTK_STATE_NORMAL,NULL);
    }
}
static void animation_object_editor_pitch_entry_changed(GtkWidget *widget, gpointer user_data)
{
animation_object_editor_t* editor=(animation_object_editor_t*)user_data;
    if(editor->object==NULL)return;

const char* error;
const char* str=gtk_entry_get_text(GTK_ENTRY(editor->pitch_entry));
animation_expression_parse(editor->object->rotation[0],str,&error);

    if(error!=NULL)
    {
    gtk_label_set_text(GTK_LABEL(editor->error_label),error);
    GdkColor red;
    red.blue=20000;
    red.green=20000;
    red.red=65535;
    gtk_widget_modify_base(editor->pitch_entry,GTK_STATE_NORMAL,&red);
    }
    else
    {
    gtk_label_set_text(GTK_LABEL(editor->error_label),"No error");
    gtk_widget_modify_base(editor->pitch_entry,GTK_STATE_NORMAL,NULL);
    }
}
static void animation_object_editor_yaw_entry_changed(GtkWidget *widget, gpointer user_data)
{
animation_object_editor_t* editor=(animation_object_editor_t*)user_data;
    if(editor->object==NULL)return;

const char* error;
const char* str=gtk_entry_get_text(GTK_ENTRY(editor->yaw_entry));
animation_expression_parse(editor->object->rotation[1],str,&error);

    if(error!=NULL)
    {
    gtk_label_set_text(GTK_LABEL(editor->error_label),error);
    GdkColor red;
    red.blue=20000;
    red.green=20000;
    red.red=65535;
    gtk_widget_modify_base(editor->yaw_entry,GTK_STATE_NORMAL,&red);
    }
    else
    {
    gtk_label_set_text(GTK_LABEL(editor->error_label),"No error");
    gtk_widget_modify_base(editor->yaw_entry,GTK_STATE_NORMAL,NULL);
    }
}
static void animation_object_editor_roll_entry_changed(GtkWidget *widget, gpointer user_data)
{
animation_object_editor_t* editor=(animation_object_editor_t*)user_data;
    if(editor->object==NULL)return;

const char* error;
const char* str=gtk_entry_get_text(GTK_ENTRY(editor->roll_entry));
animation_expression_parse(editor->object->rotation[2],str,&error);

    if(error!=NULL)
    {
    gtk_label_set_text(GTK_LABEL(editor->error_label),error);
    GdkColor red;
    red.blue=20000;
    red.green=20000;
    red.red=65535;
    gtk_widget_modify_base(editor->roll_entry,GTK_STATE_NORMAL,&red);
    }
    else
    {
    gtk_label_set_text(GTK_LABEL(editor->error_label),"No error");
    gtk_widget_modify_base(editor->roll_entry,GTK_STATE_NORMAL,NULL);
    }
}
void animation_object_editor_update_parent_select(animation_object_editor_t* editor)
{
//Easiest way to prevent this change from altering the object parent is to null the object temporarily
animation_object_t* object=editor->object;
editor->object=NULL;

    if(object->parent==NULL)gtk_combo_box_set_active(GTK_COMBO_BOX(editor->parent_select),0);
    {
        for(int i=0;i<editor->num_objects;i++)
        {
            if(editor->objects[i]==object->parent)
            {
                gtk_combo_box_set_active(GTK_COMBO_BOX(editor->parent_select),i+1);
                break;
            }
        }
    }
editor->object=object;
}
static void animation_object_editor_parent_select_changed(GtkWidget* widget, gpointer user_data)
{
animation_object_editor_t* editor=(animation_object_editor_t*)user_data;
    if(editor->object==NULL)return;

int active=gtk_combo_box_get_active(GTK_COMBO_BOX(editor->parent_select));
    if(active==0)animation_object_set_parent(editor->object,NULL);
    else if(active>0)
    {
        if(animation_object_set_parent(editor->object,editor->objects[active-1])==0)
        {
        show_error("Selected object is a descendant of the current object");
        animation_object_editor_update_parent_select(editor);
        }
    }
}

void animation_object_editor_update_object_list(animation_object_editor_t* editor,animation_object_t** objects,int num_objects)
{
editor->objects=objects;
editor->num_objects=num_objects;

gtk_combo_box_text_remove_all(GTK_COMBO_BOX_TEXT(editor->parent_select));
gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(editor->parent_select),NULL,"None");
    for(int i=0;i<num_objects;i++)gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(editor->parent_select),NULL,objects[i]->model->name);
}

animation_object_editor_t* animation_object_editor_new(animation_object_t** objects,int num_objects)
{
animation_object_editor_t* editor=malloc(sizeof(animation_object_editor_t));
editor->object=NULL;

GtkWidget* parent_label=gtk_label_new("Parent");
editor->parent_select=gtk_combo_box_text_new();
animation_object_editor_update_object_list(editor,objects,num_objects);
gtk_combo_box_set_active(GTK_COMBO_BOX(editor->parent_select),0);
g_signal_connect(editor->parent_select,"changed",G_CALLBACK(animation_object_editor_parent_select_changed),editor);

editor->error_label=gtk_label_new("No error");

editor->x_entry=gtk_entry_new();
editor->y_entry=gtk_entry_new();
editor->z_entry=gtk_entry_new();
editor->pitch_entry=gtk_entry_new();
editor->yaw_entry=gtk_entry_new();
editor->roll_entry=gtk_entry_new();

GtkWidget* x_label=gtk_label_new("X");
GtkWidget* y_label=gtk_label_new("Y");
GtkWidget* z_label=gtk_label_new("Z");
GtkWidget* pitch_label=gtk_label_new("Pitch");
GtkWidget* yaw_label=gtk_label_new("Yaw");
GtkWidget* roll_label=gtk_label_new("Roll");

g_signal_connect(editor->x_entry,"changed",G_CALLBACK(animation_object_editor_x_entry_changed),editor);
g_signal_connect(editor->y_entry,"changed",G_CALLBACK(animation_object_editor_y_entry_changed),editor);
g_signal_connect(editor->z_entry,"changed",G_CALLBACK(animation_object_editor_z_entry_changed),editor);
g_signal_connect(editor->pitch_entry,"changed",G_CALLBACK(animation_object_editor_pitch_entry_changed),editor);
g_signal_connect(editor->yaw_entry,"changed",G_CALLBACK(animation_object_editor_yaw_entry_changed),editor);
g_signal_connect(editor->roll_entry,"changed",G_CALLBACK(animation_object_editor_roll_entry_changed),editor);


editor->container=gtk_table_new(9,2,FALSE);

gtk_table_attach_defaults(GTK_TABLE(editor->container),parent_label,0,1,1,2);
gtk_table_attach_defaults(GTK_TABLE(editor->container),editor->parent_select,1,2,1,2);

gtk_table_attach_defaults(GTK_TABLE(editor->container),x_label,0,1,2,3);
gtk_table_attach_defaults(GTK_TABLE(editor->container),editor->x_entry,1,2,2,3);
gtk_table_attach_defaults(GTK_TABLE(editor->container),y_label,0,1,3,4);
gtk_table_attach_defaults(GTK_TABLE(editor->container),editor->y_entry,1,2,3,4);
gtk_table_attach_defaults(GTK_TABLE(editor->container),z_label,0,1,4,5);
gtk_table_attach_defaults(GTK_TABLE(editor->container),editor->z_entry,1,2,4,5);
gtk_table_attach_defaults(GTK_TABLE(editor->container),pitch_label,0,1,5,6);
gtk_table_attach_defaults(GTK_TABLE(editor->container),editor->pitch_entry,1,2,5,6);
gtk_table_attach_defaults(GTK_TABLE(editor->container),yaw_label,0,1,6,7);
gtk_table_attach_defaults(GTK_TABLE(editor->container),editor->yaw_entry,1,2,6,7);
gtk_table_attach_defaults(GTK_TABLE(editor->container),roll_label,0,1,7,8);
gtk_table_attach_defaults(GTK_TABLE(editor->container),editor->roll_entry,1,2,7,8);

gtk_table_attach_defaults(GTK_TABLE(editor->container),editor->error_label,0,2,8,9);

return editor;
}
void animation_object_editor_set_object(animation_object_editor_t* editor,animation_object_t* object)
{
editor->object=NULL;
gtk_entry_set_text(GTK_ENTRY(editor->x_entry),object->position[0]->str);
gtk_entry_set_text(GTK_ENTRY(editor->y_entry),object->position[1]->str);
gtk_entry_set_text(GTK_ENTRY(editor->z_entry),object->position[2]->str);
gtk_entry_set_text(GTK_ENTRY(editor->pitch_entry),object->rotation[0]->str);
gtk_entry_set_text(GTK_ENTRY(editor->yaw_entry),object->rotation[1]->str);
gtk_entry_set_text(GTK_ENTRY(editor->roll_entry),object->rotation[2]->str);
editor->object=object;
animation_object_editor_update_parent_select(editor);
}


static void animation_dialog_add_model(GtkWidget *widget, gpointer user_data)
{
animation_dialog_t* dialog=(animation_dialog_t*)user_data;
model_t* model=model_selector_get_model(dialog->model_selector);
    if(model!=NULL)
    {
    animation_add_new_object(dialog->animation,model);
    animation_object_editor_set_object(dialog->object_editor,dialog->animation->objects[dialog->animation->num_objects-1]);
    animation_object_editor_update_object_list(dialog->object_editor,dialog->animation->objects,dialog->animation->num_objects);
    animation_viewer_update(dialog->animation_viewer);
    }
}

static void animation_dialog_update_preview(GtkWidget* widget,gpointer data)
{
animation_dialog_t* dialog=(animation_dialog_t*)data;
animation_viewer_update(dialog->animation_viewer);
}

static void animation_dialog_preview_pressed(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
int i;
animation_dialog_t* dialog=(animation_dialog_t*)user_data;

GtkAllocation container_alloc;
GdkPixbuf* pixbuf=dialog->animation_viewer->image_viewer->pixbuf;
gtk_widget_get_allocation(widget,&container_alloc);

Vector coords;
coords.X=event->x-(container_alloc.width-gdk_pixbuf_get_width(pixbuf))/2;
coords.Y=event->y-(container_alloc.height-gdk_pixbuf_get_height(pixbuf))/2;


animation_calculate_object_transforms(dialog->animation,dialog->animation_viewer->variables);


animation_object_t* selected_object=NULL;

float largest_depth=-INFINITY;
    for(i=0;i<dialog->animation->num_objects;i++)
    {
    animation_object_t* object=dialog->animation->objects[i];

    float depth;
        if(renderer_get_face_by_point(object->model,MatrixMultiply(dialog->animation_viewer->model_view,object->transform),coords,&depth)!=NULL&&depth>largest_depth)
        {
        selected_object=object;
        largest_depth=depth;
        }
    }
    if(selected_object!=NULL)
    {
    animation_object_editor_set_object(dialog->object_editor,selected_object);
    }
}


static void animation_dialog_update_variables(GtkWidget *widget, gpointer user_data)
{
animation_dialog_t* dialog=(animation_dialog_t*)user_data;
dialog->animation_viewer->variables[VAR_PITCH]=gtk_range_get_value(GTK_RANGE(dialog->pitch_slider));
dialog->animation_viewer->variables[VAR_YAW]=gtk_range_get_value(GTK_RANGE(dialog->yaw_slider));
dialog->animation_viewer->variables[VAR_ROLL]=gtk_range_get_value(GTK_RANGE(dialog->roll_slider));
    if(dialog->spin_slider!=NULL)dialog->animation_viewer->variables[VAR_SPIN]=gtk_range_get_value(GTK_RANGE(dialog->spin_slider));
    if(dialog->swing_slider!=NULL)dialog->animation_viewer->variables[VAR_SWING]=gtk_range_get_value(GTK_RANGE(dialog->swing_slider));
    if(dialog->flip_slider!=NULL)dialog->animation_viewer->variables[VAR_FLIP]=gtk_range_get_value(GTK_RANGE(dialog->flip_slider));
    if(dialog->restraint_slider!=NULL)dialog->animation_viewer->variables[VAR_RESTRAINT]=gtk_range_get_value(GTK_RANGE(dialog->restraint_slider));
animation_viewer_update(dialog->animation_viewer);
}

animation_dialog_t* animation_dialog_new(animation_t* animation,model_t** models,int num_models,int variable_flags)
{
animation_dialog_t* dialog=malloc(sizeof(animation_dialog_t));

dialog->animation=animation;

dialog->dialog=gtk_dialog_new_with_buttons("Animation Settings",NULL,0,"OK",GTK_RESPONSE_OK,NULL);
GtkWidget* content_area=gtk_dialog_get_content_area(GTK_DIALOG(dialog->dialog));

GtkWidget* add_model_hbox=gtk_hbox_new(FALSE,1);

dialog->model_selector=model_selector_new(models,num_models);
gtk_box_pack_start(GTK_BOX(add_model_hbox),dialog->model_selector->container,TRUE,TRUE,2);
dialog->add_model=gtk_button_new_from_stock(GTK_STOCK_ADD);
gtk_box_pack_start(GTK_BOX(add_model_hbox),dialog->add_model,FALSE,FALSE,2);
gtk_box_pack_start(GTK_BOX(content_area),add_model_hbox,FALSE,FALSE,2);
g_signal_connect(dialog->add_model,"clicked",G_CALLBACK(animation_dialog_add_model),dialog);

GtkWidget* main_hbox=gtk_hbox_new(FALSE,1);

dialog->animation_viewer=animation_viewer_new();
g_signal_connect(dialog->animation_viewer->image_viewer->container,"button_press_event",G_CALLBACK(animation_dialog_preview_pressed),dialog);
animation_viewer_set_animation(dialog->animation_viewer,animation);
gtk_box_pack_start(GTK_BOX(main_hbox),dialog->animation_viewer->container,FALSE,FALSE,2);

dialog->object_editor=animation_object_editor_new(animation->objects,animation->num_objects);
gtk_box_pack_start(GTK_BOX(main_hbox),dialog->object_editor->container,FALSE,FALSE,2);

gtk_box_pack_start(GTK_BOX(content_area),main_hbox,FALSE,FALSE,2);


GtkWidget* var_table=gtk_table_new(8,2,FALSE);

GtkWidget* pitch_label=gtk_label_new("Pitch:");
GtkWidget* yaw_label=gtk_label_new("Yaw:");
GtkWidget* roll_label=gtk_label_new("Roll:");
dialog->pitch_slider=gtk_hscale_new_with_range(-3.14,3.14,0.1);
dialog->yaw_slider=gtk_hscale_new_with_range(-3.14,3.14,0.1);
dialog->roll_slider=gtk_hscale_new_with_range(-3.14,3.14,0.1);
gtk_table_attach(GTK_TABLE(var_table),pitch_label,0,1,1,2,GTK_SHRINK,GTK_FILL,1,1);
gtk_table_attach_defaults(GTK_TABLE(var_table),dialog->pitch_slider,1,2,1,2);
gtk_table_attach(GTK_TABLE(var_table),yaw_label,0,1,2,3,GTK_SHRINK,GTK_FILL,1,1);
gtk_table_attach_defaults(GTK_TABLE(var_table),dialog->yaw_slider,1,2,2,3);
gtk_table_attach(GTK_TABLE(var_table),roll_label,0,1,3,4,GTK_SHRINK,GTK_FILL,1,1);
gtk_table_attach_defaults(GTK_TABLE(var_table),dialog->roll_slider,1,2,3,4);
g_signal_connect(dialog->pitch_slider,"value-changed",G_CALLBACK(animation_dialog_update_variables),dialog);
g_signal_connect(dialog->yaw_slider,"value-changed",G_CALLBACK(animation_dialog_update_variables),dialog);
g_signal_connect(dialog->roll_slider,"value-changed",G_CALLBACK(animation_dialog_update_variables),dialog);

    if(variable_flags&ANIMATION_DIALOG_RESTRAINT)
    {
    GtkWidget* restraint_label=gtk_label_new("Restraint:");
    dialog->restraint_slider=gtk_hscale_new_with_range(0,1,0.1);
    gtk_table_attach(GTK_TABLE(var_table),restraint_label,0,1,4,5,GTK_SHRINK,GTK_FILL,1,1);
    gtk_table_attach_defaults(GTK_TABLE(var_table),dialog->restraint_slider,1,2,4,5);
    g_signal_connect(dialog->restraint_slider,"value-changed",G_CALLBACK(animation_dialog_update_variables),dialog);
    }else dialog->restraint_slider=NULL;

    if(variable_flags&ANIMATION_DIALOG_SPIN)
    {
    GtkWidget* spin_label=gtk_label_new("Spin:");
    dialog->spin_slider=gtk_hscale_new_with_range(-3.14,3.14,0.1);
    gtk_table_attach(GTK_TABLE(var_table),spin_label,0,1,5,6,GTK_SHRINK,GTK_FILL,1,1);
    gtk_table_attach_defaults(GTK_TABLE(var_table),dialog->spin_slider,1,2,5,6);
    g_signal_connect(dialog->spin_slider,"value-changed",G_CALLBACK(animation_dialog_update_variables),dialog);
    }else dialog->spin_slider=NULL;

    if(variable_flags&ANIMATION_DIALOG_SWING)
    {
    GtkWidget* swing_label=gtk_label_new("Swing:");
    dialog->swing_slider=gtk_hscale_new_with_range(-3.14,3.14,0.1);
    gtk_table_attach(GTK_TABLE(var_table),swing_label,0,1,6,7,GTK_SHRINK,GTK_FILL,1,1);
    gtk_table_attach_defaults(GTK_TABLE(var_table),dialog->swing_slider,1,2,6,7);
    g_signal_connect(dialog->swing_slider,"value-changed",G_CALLBACK(animation_dialog_update_variables),dialog);
    }else dialog->swing_slider=NULL;

    if(variable_flags&ANIMATION_DIALOG_FLIP)
    {
    GtkWidget* flip_label=gtk_label_new("Flip:");
    dialog->flip_slider=gtk_hscale_new_with_range(-3.14,3.14,0.1);
    gtk_table_attach(GTK_TABLE(var_table),flip_label,0,1,7,8,GTK_SHRINK,GTK_FILL,1,1);
    gtk_table_attach_defaults(GTK_TABLE(var_table),dialog->flip_slider,1,2,7,8);
    g_signal_connect(dialog->flip_slider,"value-changed",G_CALLBACK(animation_dialog_update_variables),dialog);
    }else dialog->flip_slider=NULL;

gtk_box_pack_start(GTK_BOX(content_area),var_table,TRUE,TRUE,2);

gtk_widget_show_all(content_area);
return dialog;
}
void animation_dialog_run(animation_dialog_t* dialog)
{
gtk_dialog_run(GTK_DIALOG(dialog->dialog));
}
void animation_dialog_free(animation_dialog_t* dialog)
{
gtk_widget_destroy(dialog->dialog);
free(dialog);
}
