#include<stdlib.h>
#include<string.h>
#include<math.h>
#include "renderer.h"
#include "animationdialog.h"
#include "image.h"

/*

static void UpdateObject(GtkWidget* widget,gpointer user_data)
{
ObjectEditor* editor=(ObjectEditor*)user_data;
Vector position,rotation;
position.X=gtk_spin_button_get_value(GTK_SPIN_BUTTON(editor->positionXSpin));
position.Y=gtk_spin_button_get_value(GTK_SPIN_BUTTON(editor->positionYSpin));
position.Z=gtk_spin_button_get_value(GTK_SPIN_BUTTON(editor->positionZSpin));
rotation.X=M_PI*gtk_spin_button_get_value(GTK_SPIN_BUTTON(editor->rotationXSpin))/180.0;
rotation.Y=M_PI*gtk_spin_button_get_value(GTK_SPIN_BUTTON(editor->rotationYSpin))/180.0;
rotation.Z=M_PI*gtk_spin_button_get_value(GTK_SPIN_BUTTON(editor->rotationZSpin))/180.0;
UpdateTransform(editor->dialog->animation,editor->dialog->frame,editor->index,position,rotation);
RenderFramePreview(editor->dialog);
}

static void UpdateObjectParent(GtkWidget* widget,gpointer user_data)
{
ObjectEditor* editor=(ObjectEditor*)user_data;
int parent=gtk_combo_box_get_active(GTK_COMBO_BOX(widget));
UpdateParent(editor->dialog->animation,editor->index,parent==0?-1:parent-1);
}



void AddObjectEditor(AnimationDialog* dialog,int index)
{
int i;
//Create ObjectEditor to handle editing of this object
ObjectEditor* editor=malloc(sizeof(ObjectEditor));
editor->dialog=dialog;
editor->index=index;
//Create name label
editor->nameLabel=gtk_label_new(dialog->animation->Objects[editor->index].model->Name);
//Create position spin buttons
Vector position=dialog->animation->Frames[dialog->frame][index].Position;
editor->positionXSpin=gtk_spin_button_new_with_range(-20,20,0.1);
editor->positionYSpin=gtk_spin_button_new_with_range(-20,20,0.1);
editor->positionZSpin=gtk_spin_button_new_with_range(-20,20,0.1);
gtk_spin_button_set_value(GTK_SPIN_BUTTON(editor->positionXSpin),position.X);
gtk_spin_button_set_value(GTK_SPIN_BUTTON(editor->positionYSpin),position.Y);
gtk_spin_button_set_value(GTK_SPIN_BUTTON(editor->positionZSpin),position.Z);
g_signal_connect(editor->positionXSpin,"value-changed",G_CALLBACK(UpdateObject),editor);
g_signal_connect(editor->positionYSpin,"value-changed",G_CALLBACK(UpdateObject),editor);
g_signal_connect(editor->positionZSpin,"value-changed",G_CALLBACK(UpdateObject),editor);
//Create rotation spin buttons
Vector rotation=dialog->animation->Frames[dialog->frame][index].Rotation;
editor->rotationXSpin=gtk_spin_button_new_with_range(-180,180,10.0);
editor->rotationYSpin=gtk_spin_button_new_with_range(-180,180,10.0);
editor->rotationZSpin=gtk_spin_button_new_with_range(-180,180,10.0);
gtk_spin_button_set_value(GTK_SPIN_BUTTON(editor->rotationXSpin),rotation.X*180/M_PI);
gtk_spin_button_set_value(GTK_SPIN_BUTTON(editor->rotationYSpin),rotation.Y*180/M_PI);
gtk_spin_button_set_value(GTK_SPIN_BUTTON(editor->rotationZSpin),rotation.Z*180/M_PI);
g_signal_connect(editor->rotationXSpin,"value-changed",G_CALLBACK(UpdateObject),editor);
g_signal_connect(editor->rotationYSpin,"value-changed",G_CALLBACK(UpdateObject),editor);
g_signal_connect(editor->rotationZSpin,"value-changed",G_CALLBACK(UpdateObject),editor);
//Create parent selector
editor->parentSelect=gtk_combo_box_text_new();
gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(editor->parentSelect),"None");
    for(i=0;i<dialog->animation->NumObjects;i++)
    {
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(editor->parentSelect),dialog->animation->Objects[i].model->Name);
    }
int parent=dialog->animation->Objects[editor->index].parentIndex;
gtk_combo_box_set_active(GTK_COMBO_BOX(editor->parentSelect),parent+1);
g_signal_connect(editor->parentSelect,"changed",G_CALLBACK(UpdateObjectParent),editor);


dialog->ObjectEditors[dialog->numObjects++]=editor;





//Resize table to provide space for extra row
gtk_table_resize(GTK_TABLE(dialog->objectTable),3+dialog->numObjects,17);
//Add object editor widgets to table
int top=2+dialog->numObjects,bottom=3+dialog->numObjects;
//Add name label
gtk_table_attach_defaults(GTK_TABLE(dialog->objectTable),editor->nameLabel,0,1,top,bottom);
//Add position spin buttons
gtk_table_attach_defaults(GTK_TABLE(dialog->objectTable),editor->positionXSpin,2,3,top,bottom);
gtk_table_attach_defaults(GTK_TABLE(dialog->objectTable),editor->positionYSpin,4,5,top,bottom);
gtk_table_attach_defaults(GTK_TABLE(dialog->objectTable),editor->positionZSpin,6,7,top,bottom);
//Add rotation spin buttons
gtk_table_attach_defaults(GTK_TABLE(dialog->objectTable),editor->rotationXSpin,8,9,top,bottom);
gtk_table_attach_defaults(GTK_TABLE(dialog->objectTable),editor->rotationYSpin,10,11,top,bottom);
gtk_table_attach_defaults(GTK_TABLE(dialog->objectTable),editor->rotationZSpin,12,13,top,bottom);
//Add parent selector
gtk_table_attach_defaults(GTK_TABLE(dialog->objectTable),editor->parentSelect,14,15,top,bottom);
}

static void FramesChanged(GtkWidget* widget,gpointer user_data){
AnimationDialog* dialog=(AnimationDialog*)user_data;
int frames=(int)gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget));
    if(frames>dialog->animation->NumFrames)gtk_widget_set_sensitive(dialog->nextFrame,TRUE);
SetNumFrames(dialog->animation,frames);
gtk_spin_button_set_range(GTK_SPIN_BUTTON(dialog->curFrameSpin),0,frames-1);
    if(dialog->frame>=frames)SetCurrentFrame(dialog,frames-1);
}
static void UpdateName(GtkWidget* widget,gpointer user_data){
Animation* animation=(Animation*)user_data;
const char* name=gtk_entry_get_text(GTK_ENTRY(widget));
animation->Name=realloc(animation->Name,strlen(name)+1);
strcpy(animation->Name,name);
}


static void AddFrameModel(GtkWidget* widget,gpointer user_data)
{
AnimationDialog* dialog=(AnimationDialog*)user_data;
int modelIndex=gtk_combo_box_get_active(GTK_COMBO_BOX(dialog->modelSelect));
int objectIndex=AddObject(dialog->animation,GetModelByIndex(modelIndex));
RenderFramePreview(dialog);
AddObjectEditor(dialog,objectIndex);
gtk_widget_show_all(dialog->objectTable);
}
*/
void animation_viewer_update(animation_viewer_t* viewer)
{
renderer_clear_buffers();
    if(viewer->animation!=NULL)animation_render_frame(viewer->animation,viewer->frame,MatrixIdentity());
renderer_render_model(viewer->grid_model,MatrixIdentity());
image_t* image=renderer_get_image();
image_viewer_set_image(viewer->image_viewer,image);
image_free(image);
}
void animation_viewer_set_frame(animation_viewer_t* viewer,int frame)
{
//int i;
    if(viewer->animation==NULL)return;
viewer->frame=frame;
    if(viewer->frame==0)gtk_widget_set_sensitive(viewer->prev_frame,FALSE);
    else gtk_widget_set_sensitive(viewer->prev_frame,TRUE);

    if(viewer->frame==viewer->animation->num_frames-1)gtk_widget_set_sensitive(viewer->next_frame,FALSE);
    else gtk_widget_set_sensitive(viewer->next_frame,TRUE);
animation_viewer_update(viewer);
/*
//Update object editors with new values
    for(i=0;i<dialog->numObjects;i++)
    {
    ObjectEditor* editor=dialog->ObjectEditors[i];
    Vector position=dialog->animation->Frames[frame][editor->index].Position;
    Vector rotation=dialog->animation->Frames[frame][editor->index].Rotation;
    //Disconnect signal handlers so it won't try to update the object
    g_signal_handlers_disconnect_by_func(editor->positionXSpin,UpdateObject,editor);
    g_signal_handlers_disconnect_by_func(editor->positionYSpin,UpdateObject,editor);
    g_signal_handlers_disconnect_by_func(editor->positionZSpin,UpdateObject,editor);
    g_signal_handlers_disconnect_by_func(editor->rotationXSpin,UpdateObject,editor);
    g_signal_handlers_disconnect_by_func(editor->rotationYSpin,UpdateObject,editor);
    g_signal_handlers_disconnect_by_func(editor->rotationZSpin,UpdateObject,editor);
    //Update buttons with new values
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(editor->positionXSpin),position.X);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(editor->positionYSpin),position.Y);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(editor->positionZSpin),position.Z);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(editor->rotationXSpin),rotation.X*180/M_PI);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(editor->rotationYSpin),rotation.Y*180/M_PI);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(editor->rotationZSpin),rotation.Z*180/M_PI);
    //Reconnect signal handlers
    g_signal_connect(editor->positionXSpin,"value-changed",G_CALLBACK(UpdateObject),editor);
    g_signal_connect(editor->positionYSpin,"value-changed",G_CALLBACK(UpdateObject),editor);
    g_signal_connect(editor->positionZSpin,"value-changed",G_CALLBACK(UpdateObject),editor);
    g_signal_connect(editor->rotationXSpin,"value-changed",G_CALLBACK(UpdateObject),editor);
    g_signal_connect(editor->rotationYSpin,"value-changed",G_CALLBACK(UpdateObject),editor);
    g_signal_connect(editor->rotationZSpin,"value-changed",G_CALLBACK(UpdateObject),editor);
    }
*/
}
static void animation_viewer_frame_changed(GtkWidget* widget,gpointer user_data)
{
animation_viewer_t* viewer=(animation_viewer_t*)user_data;
int frame=gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(viewer->frame_spin));
animation_viewer_set_frame(viewer,frame);
}
static void animation_viewer_prev_frame(GtkWidget* widget,gpointer user_data)
{
animation_viewer_t* viewer=(animation_viewer_t*)user_data;
    if(viewer->frame>0)gtk_spin_button_set_value(GTK_SPIN_BUTTON(viewer->frame_spin),viewer->frame-1);
}
static void animation_viewer_next_frame(GtkWidget* widget,gpointer user_data)
{
animation_viewer_t* viewer=(animation_viewer_t*)user_data;
    if(viewer->frame<viewer->animation->num_frames-1)gtk_spin_button_set_value(GTK_SPIN_BUTTON(viewer->frame_spin),viewer->frame+1);
}
animation_viewer_t* animation_viewer_new()
{
animation_viewer_t* viewer=malloc(sizeof(animation_viewer_t));
viewer->animation=NULL;
viewer->frame=0;
viewer->grid_model=model_new_grid();
viewer->image_viewer=image_viewer_new();
viewer->container=gtk_vbox_new(FALSE,1);
gtk_box_pack_start(GTK_BOX(viewer->container),viewer->image_viewer->container,FALSE,FALSE,2);


//Frame selection
viewer->next_frame=gtk_button_new();
viewer->prev_frame=gtk_button_new();
GtkWidget* image=gtk_image_new_from_stock(GTK_STOCK_GO_FORWARD,GTK_ICON_SIZE_BUTTON);
gtk_container_add(GTK_CONTAINER(viewer->next_frame),image);
image=gtk_image_new_from_stock(GTK_STOCK_GO_BACK,GTK_ICON_SIZE_BUTTON);
gtk_container_add(GTK_CONTAINER(viewer->prev_frame),image);
gtk_widget_set_sensitive(viewer->next_frame,FALSE);
gtk_widget_set_sensitive(viewer->prev_frame,FALSE);
g_signal_connect(viewer->next_frame,"clicked",G_CALLBACK(animation_viewer_next_frame),viewer);
g_signal_connect(viewer->prev_frame,"clicked",G_CALLBACK(animation_viewer_prev_frame),viewer);

viewer->frame_spin=gtk_spin_button_new_with_range(0,0,1);
g_signal_connect(viewer->frame_spin,"value-changed",G_CALLBACK(animation_viewer_frame_changed),viewer);

viewer->lower_hbox=gtk_hbox_new(FALSE,1);
gtk_box_pack_start(GTK_BOX(viewer->lower_hbox),viewer->prev_frame,FALSE,FALSE,2);
gtk_box_pack_start(GTK_BOX(viewer->lower_hbox),viewer->frame_spin,FALSE,FALSE,2);
gtk_box_pack_start(GTK_BOX(viewer->lower_hbox),viewer->next_frame,FALSE,FALSE,2);
gtk_box_pack_start(GTK_BOX(viewer->container),viewer->lower_hbox,FALSE,FALSE,2);

animation_viewer_update(viewer);
return viewer;
}
void animation_viewer_set_animation(animation_viewer_t* viewer,animation_t* animation)
{
viewer->animation=animation;
    if(viewer->frame>=animation->num_frames)animation_viewer_set_frame(viewer,animation->num_frames-1);
    else if(viewer->frame==animation->num_frames-1)gtk_widget_set_sensitive(viewer->next_frame,FALSE);
    else gtk_widget_set_sensitive(viewer->next_frame,TRUE);
gtk_spin_button_set_range(GTK_SPIN_BUTTON(viewer->frame_spin),0,animation->num_frames-1);
animation_viewer_update(viewer);
}

static void vector_editor_changed(GtkWidget* widget,gpointer data)
{
vector_editor_t* editor=(vector_editor_t*)data;
editor->vector.X=gtk_spin_button_get_value_as_float(GTK_SPIN_BUTTON(editor->x));
editor->vector.Y=gtk_spin_button_get_value_as_float(GTK_SPIN_BUTTON(editor->y));
editor->vector.Z=gtk_spin_button_get_value_as_float(GTK_SPIN_BUTTON(editor->z));
}
void vector_editor_register_callback(vector_editor_t* editor,void (*callback)(GtkWidget* widget,gpointer data),gpointer data)
{
g_signal_connect(editor->x,"value-changed",G_CALLBACK(callback),data);
g_signal_connect(editor->y,"value-changed",G_CALLBACK(callback),data);
g_signal_connect(editor->z,"value-changed",G_CALLBACK(callback),data);
}
vector_editor_t* vector_editor_new(const char* label,float min,float max,float step)
{
vector_editor_t* editor=malloc(sizeof(vector_editor_t));
editor->vector.X=0.0;
editor->vector.Y=0.0;
editor->vector.Z=0.0;
editor->container=gtk_hbox_new(FALSE,1);
editor->label=gtk_label_new(label);
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

static void animation_dialog_add_model(GtkWidget *widget, gpointer user_data)
{
animation_dialog_t* dialog=(animation_dialog_t*)user_data;
model_t* model=model_selector_get_model(dialog->model_selector);
    if(model!=NULL)
    {
    animation_add_object(dialog->animation,model);
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
int selected_object=-1;
int frame=dialog->animation_viewer->frame;
    for(i=0;i<dialog->animation->num_objects;i++)
    {
    model_t* model=dialog->animation->objects[i].model;
    object_transform_t* object_data=&(dialog->animation->frames[frame][i]);
        if(renderer_get_face_by_point(model,object_data->transform,coords)!=NULL)selected_object=i;
    }
    if(selected_object!=-1)
    {
    dialog->selected_object=selected_object;
    object_transform_editor_set_object_transform(dialog->transform_editor,&(dialog->animation->frames[dialog->animation_viewer->frame][selected_object]));
    }
}
animation_dialog_t* animation_dialog_new(animation_t* animation,model_t** models,int num_models)
{
animation_dialog_t* dialog=malloc(sizeof(animation_dialog_t));

dialog->animation=animation;
//animationDialog.numObjects=0;

dialog->dialog=gtk_dialog_new_with_buttons("Animation Settings",NULL,0,"OK",GTK_RESPONSE_OK,NULL);
GtkWidget* content_area=gtk_dialog_get_content_area(GTK_DIALOG(dialog->dialog));


GtkWidget* add_model_hbox=gtk_hbox_new(FALSE,1);
dialog->model_selector=model_selector_new(models,num_models);
gtk_box_pack_start(GTK_BOX(add_model_hbox),dialog->model_selector->container,TRUE,TRUE,2);
dialog->add_model=gtk_button_new_from_stock(GTK_STOCK_ADD);
gtk_box_pack_start(GTK_BOX(add_model_hbox),dialog->add_model,FALSE,FALSE,2);
gtk_box_pack_start(GTK_BOX(content_area),add_model_hbox,FALSE,FALSE,2);
g_signal_connect(dialog->add_model,"clicked",G_CALLBACK(animation_dialog_add_model),dialog);



dialog->animation_viewer=animation_viewer_new();
animation_viewer_set_animation(dialog->animation_viewer,animation);
g_signal_connect(dialog->animation_viewer->image_viewer->container,"button_press_event",G_CALLBACK(animation_dialog_preview_pressed),dialog);
gtk_box_pack_start(GTK_BOX(content_area),dialog->animation_viewer->container,FALSE,FALSE,2);

dialog->transform_editor=object_transform_editor_new();
object_transform_editor_register_callback(dialog->transform_editor,animation_dialog_update_preview,dialog);
gtk_box_pack_start(GTK_BOX(content_area),dialog->transform_editor->container,FALSE,FALSE,2);

gtk_widget_show_all(content_area);
return dialog;

/*
//Name
GtkWidget* nameBox=gtk_hbox_new(FALSE,1);
GtkWidget* nameLabel=gtk_label_new("Name:");
GtkWidget* name=gtk_entry_new();
g_signal_connect(name,"changed",G_CALLBACK(UpdateName),animation);
gtk_entry_set_text(GTK_ENTRY(name),animation->Name);
gtk_box_pack_start(GTK_BOX(nameBox),nameLabel,FALSE,FALSE,2);
gtk_box_pack_start(GTK_BOX(nameBox),name,TRUE,TRUE,2);
gtk_box_pack_start(GTK_BOX(contentArea),nameBox,TRUE,TRUE,2);

//Top bar
GtkWidget* animationHBox=gtk_hbox_new(FALSE,1);
GtkWidget* framesLabel=gtk_label_new("Frames:");
GtkWidget* framesSpin=gtk_spin_button_new_with_range(1,MAX_FRAMES,1);
gtk_spin_button_set_value(GTK_SPIN_BUTTON(framesSpin),animation->NumFrames);
g_signal_connect(framesSpin,"value-changed",G_CALLBACK(FramesChanged),&animationDialog);
gtk_box_pack_start(GTK_BOX(animationHBox),framesLabel,FALSE,FALSE,2);
gtk_box_pack_start(GTK_BOX(animationHBox),framesSpin,TRUE,TRUE,2);


//Model addition bar
GtkWidget* modelHBox=gtk_hbox_new(FALSE,1);

GtkWidget* addModelLabel=gtk_label_new("Add model:");

GtkWidget* addModel=gtk_button_new();
image=gtk_image_new_from_stock(GTK_STOCK_ADD,GTK_ICON_SIZE_BUTTON);
gtk_container_add(GTK_CONTAINER(addModel),image);
g_signal_connect(addModel,"clicked",G_CALLBACK(AddFrameModel),&animationDialog);

animationDialog.modelSelect=gtk_combo_box_text_new();
    for(i=0;i<NumModels();i++)
    {
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(animationDialog.modelSelect),GetModelByIndex(i)->Name);
    }
    if(NumModels()>0)gtk_combo_box_set_active(GTK_COMBO_BOX(animationDialog.modelSelect),0);
    else gtk_widget_set_sensitive(addModel,FALSE);
gtk_box_pack_start(GTK_BOX(modelHBox),addModelLabel,FALSE,FALSE,2);
gtk_box_pack_start(GTK_BOX(modelHBox),animationDialog.modelSelect,TRUE,TRUE,2);
gtk_box_pack_start(GTK_BOX(modelHBox),addModel,TRUE,TRUE,2);
gtk_box_pack_start(GTK_BOX(contentArea),modelHBox,FALSE,FALSE,2);



//Object editing
animationDialog.objectTable=gtk_table_new(3,17,FALSE);
//Create headings
GtkWidget* objectLabel=gtk_label_new("Objects:");
gtk_table_attach_defaults(GTK_TABLE(animationDialog.objectTable),objectLabel,0,17,0,1);
GtkWidget* objectNameLabel=gtk_label_new("Name");
gtk_table_attach_defaults(GTK_TABLE(animationDialog.objectTable),objectNameLabel,0,1,1,2);
GtkWidget* positionLabel=gtk_label_new("Position");
gtk_table_attach_defaults(GTK_TABLE(animationDialog.objectTable),positionLabel,2,7,1,2);
GtkWidget* rotationLabel=gtk_label_new("Rotation");
gtk_table_attach_defaults(GTK_TABLE(animationDialog.objectTable),rotationLabel,8,13,1,2);
GtkWidget* parentLabel=gtk_label_new("Parent");
gtk_table_attach_defaults(GTK_TABLE(animationDialog.objectTable),parentLabel,14,15,1,2);
//GtkWidget* removeLabel=gtk_label_new("Remove");
//gtk_table_attach_defaults(GTK_TABLE(animationDialog.objectTable),removeLabel,16,17,1,2);
GtkWidget* positionXLabel=gtk_label_new("X");
gtk_table_attach_defaults(GTK_TABLE(animationDialog.objectTable),positionXLabel,2,3,2,3);
GtkWidget* positionYLabel=gtk_label_new("Y");
gtk_table_attach_defaults(GTK_TABLE(animationDialog.objectTable),positionYLabel,4,5,2,3);
GtkWidget* positionZLabel=gtk_label_new("Z");
gtk_table_attach_defaults(GTK_TABLE(animationDialog.objectTable),positionZLabel,6,7,2,3);
GtkWidget* rotationXLabel=gtk_label_new("X");
gtk_table_attach_defaults(GTK_TABLE(animationDialog.objectTable),rotationXLabel,8,9,2,3);
GtkWidget* rotationYLabel=gtk_label_new("Y");
gtk_table_attach_defaults(GTK_TABLE(animationDialog.objectTable),rotationYLabel,10,11,2,3);
GtkWidget* rotationZLabel=gtk_label_new("Z");
gtk_table_attach_defaults(GTK_TABLE(animationDialog.objectTable),rotationZLabel,12,13,2,3);
//Add seperators
GtkWidget* seperator=gtk_vseparator_new();
gtk_table_attach_defaults(GTK_TABLE(animationDialog.objectTable),seperator,1,2,1,3);
seperator=gtk_vseparator_new();
gtk_table_attach_defaults(GTK_TABLE(animationDialog.objectTable),seperator,7,8,1,3);
seperator=gtk_vseparator_new();
gtk_table_attach_defaults(GTK_TABLE(animationDialog.objectTable),seperator,13,14,1,3);
seperator=gtk_vseparator_new();
gtk_table_attach_defaults(GTK_TABLE(animationDialog.objectTable),seperator,15,16,1,3);
seperator=gtk_vseparator_new();
gtk_table_attach_defaults(GTK_TABLE(animationDialog.objectTable),seperator,3,4,2,3);
seperator=gtk_vseparator_new();
gtk_table_attach_defaults(GTK_TABLE(animationDialog.objectTable),seperator,5,6,2,3);
seperator=gtk_vseparator_new();
gtk_table_attach_defaults(GTK_TABLE(animationDialog.objectTable),seperator,9,10,2,3);
seperator=gtk_vseparator_new();
gtk_table_attach_defaults(GTK_TABLE(animationDialog.objectTable),seperator,11,12,2,3);
gtk_box_pack_start(GTK_BOX(contentArea),animationDialog.objectTable,TRUE,TRUE,2);
    for(i=0;i<animation->NumObjects;i++)AddObjectEditor(&animationDialog,i);
*/
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
