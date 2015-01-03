#include<stdlib.h>
#include<string.h>
#include<math.h>
#include "animationdialog.h"
#include "image.h"


void RenderFramePreview(AnimationDialog* dialog)
{
RenderFrame(dialog->animation,dialog->frame,MatrixIdentity());
RenderModel(dialog->gridModel,MatrixIdentity());
Image* image=ImageFromFrameBuffer();
ShowImageInPixbuf(dialog->pixbuf,image);
FreeImage(image);
gtk_image_set_from_pixbuf(GTK_IMAGE(dialog->preview),dialog->pixbuf);
}

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

void SetCurrentFrame(AnimationDialog* dialog,int frame)
{
int i;
dialog->frame=frame;

    if(dialog->frame==0)gtk_widget_set_sensitive(dialog->prevFrame,FALSE);
    else gtk_widget_set_sensitive(dialog->prevFrame,TRUE);

    if(dialog->frame==dialog->animation->NumFrames-1)gtk_widget_set_sensitive(dialog->nextFrame,FALSE);
    else gtk_widget_set_sensitive(dialog->nextFrame,TRUE);

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
RenderFramePreview(dialog);
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
static void CurFrameChanged(GtkWidget* widget,gpointer user_data){
AnimationDialog* dialog=(AnimationDialog*)user_data;
int frame=(int)gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget));
SetCurrentFrame(dialog,frame);
}
static void PrevFrame(GtkWidget* widget,gpointer user_data){
AnimationDialog* dialog=(AnimationDialog*)user_data;
    if(dialog->frame<dialog->animation->NumFrames-1)gtk_spin_button_set_value(GTK_SPIN_BUTTON(dialog->curFrameSpin),dialog->frame-1);
}
static void NextFrame(GtkWidget* widget,gpointer user_data)
{
AnimationDialog* dialog=(AnimationDialog*)user_data;
    if(dialog->frame<dialog->animation->NumFrames-1)gtk_spin_button_set_value(GTK_SPIN_BUTTON(dialog->curFrameSpin),dialog->frame+1);
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

void CreateAnimationDialog(Animation* animation)
{
int i;
AnimationDialog animationDialog;
animationDialog.animation=animation;
animationDialog.numObjects=0;
animationDialog.frame=0;
animationDialog.gridModel=GetGridModel();

GtkWidget* dialog=gtk_dialog_new_with_buttons("Animation Settings",NULL,0,"OK",GTK_RESPONSE_OK,NULL);
GtkWidget* contentArea=gtk_dialog_get_content_area(GTK_DIALOG(dialog));
animationDialog.dialog=dialog;

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
//Frame selection
GtkWidget* image=gtk_image_new_from_stock(GTK_STOCK_GO_BACK,GTK_ICON_SIZE_BUTTON);
animationDialog.prevFrame=gtk_button_new();
gtk_container_add(GTK_CONTAINER(animationDialog.prevFrame),image);
gtk_widget_set_sensitive(animationDialog.prevFrame,FALSE);
g_signal_connect(animationDialog.prevFrame,"clicked",G_CALLBACK(PrevFrame),&animationDialog);
image=gtk_image_new_from_stock(GTK_STOCK_GO_FORWARD,GTK_ICON_SIZE_BUTTON);
animationDialog.nextFrame=gtk_button_new();
gtk_container_add(GTK_CONTAINER(animationDialog.nextFrame),image);
    if(animation->NumFrames<=1)gtk_widget_set_sensitive(animationDialog.nextFrame,FALSE);
g_signal_connect(animationDialog.nextFrame,"clicked",G_CALLBACK(NextFrame),&animationDialog);
animationDialog.curFrameSpin=gtk_spin_button_new_with_range(0,animation->NumFrames-1,1);
gtk_spin_button_set_value(GTK_SPIN_BUTTON(animationDialog.curFrameSpin),0);
g_signal_connect(animationDialog.curFrameSpin,"value-changed",G_CALLBACK(CurFrameChanged),&animationDialog);
gtk_box_pack_start(GTK_BOX(animationHBox),animationDialog.prevFrame,FALSE,FALSE,2);
gtk_box_pack_start(GTK_BOX(animationHBox),animationDialog.curFrameSpin,FALSE,FALSE,2);
gtk_box_pack_start(GTK_BOX(animationHBox),animationDialog.nextFrame,FALSE,FALSE,2);
gtk_box_pack_start(GTK_BOX(contentArea),animationHBox,FALSE,FALSE,2);

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


//Preview
animationDialog.preview=gtk_image_new();
GtkWidget* eventBox=gtk_event_box_new();
gtk_widget_set_events(eventBox,GDK_POINTER_MOTION_MASK);
gtk_container_add(GTK_CONTAINER(eventBox),animationDialog.preview);
animationDialog.pixbuf=CreateBlankPixbuf();
gtk_image_set_from_pixbuf(GTK_IMAGE(animationDialog.preview),animationDialog.pixbuf);
gtk_box_pack_start(GTK_BOX(contentArea),eventBox,TRUE,TRUE,2);
RenderFramePreview(&animationDialog);


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

gtk_widget_show_all(contentArea);

gtk_dialog_run(GTK_DIALOG(animationDialog.dialog));
gtk_widget_destroy(animationDialog.dialog);
}
