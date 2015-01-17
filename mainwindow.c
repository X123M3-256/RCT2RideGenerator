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

/*
static void EditModel(GtkWidget* widget,gpointer* data)
{
Model* model=(Model*)data;
CreateModelDialog(model);
gtk_menu_item_set_label(GTK_MENU_ITEM(widget),model->Name);
}
static void AddNewModel(GtkWidget* widget,gpointer* data)
{
//MainWindow* interface=(MainWindow*)data;
char* filename=GetFilenameFromUser("Select model to load:",GTK_FILE_CHOOSER_ACTION_OPEN);
    if(filename)
    {
    //Load model
    Model* model=LoadObj(filename);
    //Create model dialog
    CreateModelDialog(model);
    //Add model to menu
    GtkWidget* modelMenuItem=gtk_menu_item_new_with_label(model->Name);
    gtk_menu_shell_append(GTK_MENU_SHELL(MainInterface->ModelMenu),modelMenuItem);
    g_signal_connect(modelMenuItem,"activate",G_CALLBACK(EditModel),model);
    gtk_widget_show(modelMenuItem);
    }
}

static void EditAnimation(GtkWidget* widget,gpointer* data)
{
Animation* animation=(Animation*)data;
CreateAnimationDialog(animation);
gtk_menu_item_set_label(GTK_MENU_ITEM(widget),animation->Name);
}
static void AddNewAnimation(GtkWidget* widget,gpointer* data)
{
Animation* animation=CreateAnimation();
AddAnimation(animation);
CreateAnimationDialog(animation);

//Add animation to menu
GtkWidget* animationMenuItem=gtk_menu_item_new_with_label(animation->Name);
gtk_menu_shell_append(GTK_MENU_SHELL(MainInterface->AnimationMenu),animationMenuItem);
g_signal_connect(animationMenuItem,"activate",G_CALLBACK(EditAnimation),animation);
gtk_widget_show(animationMenuItem);
}


void ImageViewerUpdate(){
    if(MainInterface->Dat==NULL)return;
int NumImages=MainInterface->Dat->NumImages;
    if(MainInterface->ImageDisplayCurrentImage>=NumImages)MainInterface->ImageDisplayCurrentImage=0;

Image* RideImage=MainInterface->Dat->Images[MainInterface->ImageDisplayCurrentImage];

int rowstride=gdk_pixbuf_get_rowstride(MainInterface->ImageDisplayPixbuf);
guchar* pixels=gdk_pixbuf_get_pixels(MainInterface->ImageDisplayPixbuf);
//Clear pixbuf
int i,j;
    for(i=0;i<256;i++)
    {
        for(j=0;j<256*3;j++)
        {
        pixels[j]=0;
        }
    pixels+=rowstride;
    }
//Draw new image to pixbuf
int XOffset=128+RideImage->XOffset;//Draw image in centre of screen
int YOffset=128+RideImage->YOffset;
pixels=gdk_pixbuf_get_pixels(MainInterface->ImageDisplayPixbuf);
pixels+=YOffset*rowstride;
Color curcol;
    for(i=0;i<RideImage->Height;i++)
    {
        for(j=0;j<RideImage->Width;j++)
        {
        //int index=RideImage->Data[i][j];
        //if(index!=0)printf("Index %d\n",(unsigned char)index);
        curcol=GetColorFromPalette(RideImage->Data[i][j]);
        int pixelindex=(j+XOffset)*3;
        pixels[pixelindex]=curcol.Red;
        pixelindex++;
        pixels[pixelindex]=curcol.Green;
        pixelindex++;
        pixels[pixelindex]=curcol.Blue;
        pixelindex++;
        }
    pixels+=rowstride;
    }

//Update GUI
char str[16];
gtk_image_set_from_pixbuf(MainInterface->ImageDisplayImage,MainInterface->ImageDisplayPixbuf);
sprintf(str,"%6d of %6d",MainInterface->ImageDisplayCurrentImage+1,NumImages);
gtk_label_set_text(MainInterface->ImageDisplayPositionLabel,str);
}
void ImageViewerNextImage(){
    if(MainInterface->Dat==NULL)return;
//AnimationUpdateData();
if(MainInterface->ImageDisplayCurrentImage+1<MainInterface->Dat->NumImages)MainInterface->ImageDisplayCurrentImage++;
ImageViewerUpdate(MainInterface);
//AnimationUpdateText();
}
void ImageViewerPrevImage(){
    if(MainInterface->Dat==NULL)return;
//AnimationUpdateData();
if(MainInterface->ImageDisplayCurrentImage>0)MainInterface->ImageDisplayCurrentImage--;
ImageViewerUpdate();
//AnimationUpdateText();
}
void StringHandlerLoad(){
    if(MainInterface->Dat==NULL)return;
char* Str=GetString(MainInterface->Dat,STRING_TABLE_NAME,MainInterface->LanguageNum);
if(Str!=NULL)gtk_entry_set_text(MainInterface->RideNameEntry,Str);
else gtk_entry_set_text(MainInterface->RideNameEntry,"No text in this language");
Str=GetString(MainInterface->Dat,STRING_TABLE_DESCRIPTION,MainInterface->LanguageNum);
if(Str!=NULL)gtk_entry_set_text(MainInterface->RideDescriptionEntry,Str);
else gtk_entry_set_text(MainInterface->RideDescriptionEntry,"No text in this language");
Str=GetString(MainInterface->Dat,STRING_TABLE_CAPACITY,MainInterface->LanguageNum);
if(Str!=NULL)gtk_entry_set_text(MainInterface->RideCapacityEntry,Str);
else gtk_entry_set_text(MainInterface->RideCapacityEntry,"No text in this language");
}

static void OpenFile(GtkWidget* widget,gpointer data){
int i;
char* filename=GetFilenameFromUser("Select file to open",GTK_FILE_CHOOSER_ACTION_OPEN);
    if(filename==NULL)return;
DeserializeFile(filename);
int numModels=NumModels();
    for(i=0;i<numModels;i++)
    {
    //Add model to menu
    Model* model=GetModelByIndex(i);
    GtkWidget* modelMenuItem=gtk_menu_item_new_with_label(model->Name);
    gtk_menu_shell_append(GTK_MENU_SHELL(MainInterface->ModelMenu),modelMenuItem);
    g_signal_connect(modelMenuItem,"activate",G_CALLBACK(EditModel),model);
    gtk_widget_show(modelMenuItem);
    }
int numAnimations=NumAnimations();
    for(i=0;i<numAnimations;i++)
    {
    //Add animation to menu
    Animation* animation=GetAnimationByIndex(i);
    GtkWidget* animationMenuItem=gtk_menu_item_new_with_label(animation->Name);
    gtk_menu_shell_append(GTK_MENU_SHELL(MainInterface->AnimationMenu),animationMenuItem);
    g_signal_connect(animationMenuItem,"activate",G_CALLBACK(EditAnimation),animation);
    gtk_widget_show(animationMenuItem);
    }
}
static void SaveFile(GtkWidget* widget,gpointer data){
int i;
MainWindow* interface=(MainWindow*)data;

char* filename=GetFilenameFromUser("Select filename",GTK_FILE_CHOOSER_ACTION_SAVE);
    if(filename==NULL)return;

json_t* json=json_object();
json_t* models=json_array();
int numModels=NumModels();
    for(i=0;i<numModels;i++)
    {
    json_t* model=SerializeModel(GetModelByIndex(i));
    json_array_append_new(models,model);
    }
json_object_set_new(json,"models",models);

json_t* animations=json_array();
int numAnimations=NumAnimations();
    for(i=0;i<numAnimations;i++)
    {
    json_t* animation=SerializeAnimation(GetAnimationByIndex(i));
    json_array_append_new(animations,animation);
    }
json_object_set_new(json,"animations",animations);

json_dump_file(json,filename,0);
putchar('\n');
}
static void OpenDatFile(GtkWidget* widget,gpointer data){
char* Filename=GetFilenameFromUser("Select file to open",GTK_FILE_CHOOSER_ACTION_OPEN);
if(Filename!=NULL)MainInterface->Dat=LoadDat(Filename);
ImageViewerUpdate();
StringHandlerLoad();
}
static void SaveDatFile(GtkWidget* widget,gpointer data){

GtkFileChooserDialog* FileDialog=gtk_file_chooser_dialog_new("Select file to open",NULL,GTK_FILE_CHOOSER_ACTION_SAVE,GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,GTK_STOCK_OK,GTK_RESPONSE_OK,NULL);
char* Filename=NULL;
if(gtk_dialog_run(GTK_DIALOG(FileDialog))==GTK_RESPONSE_OK)
    {
    Filename=gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(FileDialog));
    SaveDat(MainInterface->Dat,Filename);
    };
gtk_widget_destroy(FileDialog);
}
static void OpenTemplateFile(GtkWidget* widget,gpointer data){
char* Filename=GetFilenameFromUser("Select template file",GTK_FILE_CHOOSER_ACTION_OPEN);
//if(Filename!=NULL)LoadTemplate(Filename);
}

static void NextImage(GtkWidget* widget,gpointer data){
ImageViewerNextImage();
}
static void PrevImage(GtkWidget* widget,gpointer data){
ImageViewerPrevImage();
}
static void MouseImage(GtkWidget *widget,GdkEvent *event,gpointer data){
    if(((GdkEventScroll*)event)->direction==GDK_SCROLL_UP)
    {
    ImageViewerNextImage();
    }
    else if(((GdkEventScroll*)event)->direction==GDK_SCROLL_DOWN)
    {
    ImageViewerPrevImage();
    }
}

static void SetLanguage(GtkWidget* widget,gpointer data){
char* Language=gtk_combo_box_get_active_text(GTK_COMBO_BOX(widget));
if(strcmp(Language,"English (UK)")==0)MainInterface->LanguageNum=0;
else if(strcmp(Language,"English (US)")==0)MainInterface->LanguageNum=1;
else if(strcmp(Language,"French")==0)MainInterface->LanguageNum=2;
else if(strcmp(Language,"German")==0)MainInterface->LanguageNum=3;
else if(strcmp(Language,"Spanish")==0)MainInterface->LanguageNum=4;
else if(strcmp(Language,"Italian")==0)MainInterface->LanguageNum=5;
else if(strcmp(Language,"Dutch")==0)MainInterface->LanguageNum=6;
else if(strcmp(Language,"Swedish")==0)MainInterface->LanguageNum=7;
else if(strcmp(Language,"Korean")==0)MainInterface->LanguageNum=9;
else if(strcmp(Language,"Chinese")==0)MainInterface->LanguageNum=11;
else if(strcmp(Language,"Portugese")==0)MainInterface->LanguageNum=13;
StringHandlerLoad();
}
static int UpdateNameString(GtkWidget* widget,gpointer data){
    if(MainInterface->Dat!=NULL)SetString(MainInterface->Dat,STRING_TABLE_NAME,MainInterface->LanguageNum,gtk_entry_get_text(MainInterface->RideNameEntry));
return FALSE;
}
static int UpdateDescriptionString(GtkWidget* widget,gpointer data){
    if(MainInterface->Dat!=NULL)SetString(MainInterface->Dat,STRING_TABLE_DESCRIPTION,MainInterface->LanguageNum,gtk_entry_get_text(MainInterface->RideDescriptionEntry));
return FALSE;
}
static int UpdateCapacityString(GtkWidget* widget,gpointer data){
    if(MainInterface->Dat!=NULL)SetString(MainInterface->Dat,STRING_TABLE_CAPACITY,MainInterface->LanguageNum,gtk_entry_get_text(MainInterface->RideCapacityEntry));
return FALSE;
}




void BuildImageDisplay(MainWindow* MainInterface)
{
//Set up the image display interface
MainInterface->ImageDisplayCurrentImage=0;
MainInterface->ImageDisplayImage=gtk_image_new();
MainInterface->ImageDisplayPixbuf=CreateBlankPixbuf();
gtk_image_set_from_pixbuf(MainInterface->ImageDisplayImage,MainInterface->ImageDisplayPixbuf);
MainInterface->ImageDisplayPositionLabel=gtk_label_new("0 of 0");
MainInterface->ImageDisplayNextButton=gtk_button_new_with_label("->");
MainInterface->ImageDisplayPrevButton=gtk_button_new_with_label("<-");

MainInterface->ImageDisplayHbox=gtk_hbox_new(TRUE,0);
gtk_box_pack_start(GTK_BOX(MainInterface->ImageDisplayHbox),MainInterface->ImageDisplayPrevButton,TRUE,TRUE,5);
gtk_box_pack_start(GTK_BOX(MainInterface->ImageDisplayHbox),MainInterface->ImageDisplayPositionLabel,FALSE,FALSE,5);
gtk_box_pack_start(GTK_BOX(MainInterface->ImageDisplayHbox),MainInterface->ImageDisplayNextButton,TRUE,TRUE,5);

MainInterface->ImageDisplayVBox=gtk_vbox_new(FALSE,5);
gtk_box_pack_start(GTK_BOX(MainInterface->ImageDisplayVBox),MainInterface->ImageDisplayImage,FALSE,FALSE,0);
gtk_box_pack_start(GTK_BOX(MainInterface->ImageDisplayVBox),MainInterface->ImageDisplayHbox,FALSE,FALSE,0);

MainInterface->ImageDisplayEventBox=gtk_event_box_new();
gtk_container_add(GTK_CONTAINER(MainInterface->ImageDisplayEventBox),MainInterface->ImageDisplayVBox);
gtk_box_pack_start(GTK_BOX(MainInterface->LowerHBox),MainInterface->ImageDisplayEventBox,FALSE,FALSE,0);
//Set up callbacks for image viewer
g_signal_connect(MainInterface->ImageDisplayNextButton,"clicked",G_CALLBACK(NextImage),NULL);
g_signal_connect(MainInterface->ImageDisplayPrevButton,"clicked",G_CALLBACK(PrevImage),NULL);
g_signal_connect(MainInterface->ImageDisplayEventBox,"scroll-event",G_CALLBACK(MouseImage),NULL);
}
void BuildStringHandler(MainWindow* MainInterface)
{
//Set up the string editing interface
MainInterface->LanguageNum=0;
MainInterface->RideNameLabel=gtk_label_new("Name:");
MainInterface->RideNameEntry=gtk_entry_new();
MainInterface->RideDescriptionLabel=gtk_label_new("Description:");
MainInterface->RideDescriptionEntry=gtk_entry_new();
MainInterface->RideCapacityEntry=gtk_entry_new();

MainInterface->LanguageComboBox=gtk_combo_box_new_text();
gtk_combo_box_append_text(MainInterface->LanguageComboBox,"English (UK)");
gtk_combo_box_append_text(MainInterface->LanguageComboBox,"English (US)");
gtk_combo_box_append_text(MainInterface->LanguageComboBox,"French");
gtk_combo_box_append_text(MainInterface->LanguageComboBox,"German");
gtk_combo_box_append_text(MainInterface->LanguageComboBox,"Spanish");
gtk_combo_box_append_text(MainInterface->LanguageComboBox,"Italian");
gtk_combo_box_append_text(MainInterface->LanguageComboBox,"Dutch");
gtk_combo_box_append_text(MainInterface->LanguageComboBox,"Swedish");
gtk_combo_box_append_text(MainInterface->LanguageComboBox,"Korean");
gtk_combo_box_append_text(MainInterface->LanguageComboBox,"Chinese");
gtk_combo_box_append_text(MainInterface->LanguageComboBox,"Portugese");

MainInterface->StringEditingTable=gtk_table_new(4,2,FALSE);
gtk_table_attach_defaults(MainInterface->StringEditingTable,MainInterface->RideNameLabel,0,1,0,1);
gtk_table_attach_defaults(MainInterface->StringEditingTable,MainInterface->RideNameEntry,1,2,0,1);
gtk_table_attach_defaults(MainInterface->StringEditingTable,MainInterface->RideDescriptionLabel,0,1,1,2);
gtk_table_attach_defaults(MainInterface->StringEditingTable,MainInterface->RideDescriptionEntry,1,2,1,2);
gtk_table_attach_defaults(MainInterface->StringEditingTable,MainInterface->RideCapacityEntry,1,2,2,3);
gtk_table_attach_defaults(MainInterface->StringEditingTable,MainInterface->LanguageComboBox,0,2,3,4);


gtk_box_pack_start(GTK_BOX(MainInterface->LeftVBox),MainInterface->StringEditingTable,FALSE,FALSE,0);


//Set up callbacks for string handler
g_signal_connect(MainInterface->LanguageComboBox,"changed",G_CALLBACK(UpdateNameString),NULL);
g_signal_connect(MainInterface->LanguageComboBox,"changed",G_CALLBACK(UpdateDescriptionString),NULL);
g_signal_connect(MainInterface->LanguageComboBox,"changed",G_CALLBACK(UpdateCapacityString),NULL);
g_signal_connect(MainInterface->LanguageComboBox,"changed",G_CALLBACK(SetLanguage),NULL);
g_signal_connect(MainInterface->RideNameEntry,"activate",G_CALLBACK(UpdateNameString),NULL);
g_signal_connect(MainInterface->RideNameEntry,"focus-out-event",G_CALLBACK(UpdateNameString),NULL);
g_signal_connect(MainInterface->RideDescriptionEntry,"activate",G_CALLBACK(UpdateDescriptionString),NULL);
g_signal_connect(MainInterface->RideDescriptionEntry,"focus-out-event",G_CALLBACK(UpdateDescriptionString),NULL);
g_signal_connect(MainInterface->RideCapacityEntry,"activate",G_CALLBACK(UpdateCapacityString),NULL);
g_signal_connect(MainInterface->RideCapacityEntry,"focus-out-event",G_CALLBACK(UpdateCapacityString),NULL);
}
*/




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
        if((*flags)&editor->flag_checkboxes[i]->flag)gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(editor->flag_checkboxes[i]->checkbox),TRUE);
        else gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(editor->flag_checkboxes[i]->checkbox),FALSE);
    }
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
gtk_box_pack_start(GTK_BOX(editor->container),editor->flag_editor->container,FALSE,FALSE,1);

editor->spacing_editor=value_editor_new(VALUE_SIZE_WORD,"Spacing:");
editor->friction_editor=value_editor_new(VALUE_SIZE_WORD,"Friction:");
editor->z_value_editor=value_editor_new(VALUE_SIZE_BYTE,"Z Value:");
gtk_box_pack_start(GTK_BOX(editor->container),editor->spacing_editor->container,FALSE,FALSE,1);
gtk_box_pack_start(GTK_BOX(editor->container),editor->friction_editor->container,FALSE,FALSE,1);
gtk_box_pack_start(GTK_BOX(editor->container),editor->z_value_editor->container,FALSE,FALSE,1);
return editor;
}
void car_editor_set_car(car_editor_t* editor,car_settings_t* car_settings)
{
editor->car_settings=car_settings;
flag_editor_set_flags(editor->flag_editor,&(car_settings->flags));
value_editor_set_value(editor->spacing_editor,&(car_settings->spacing));
value_editor_set_value(editor->friction_editor,&(car_settings->friction));
value_editor_set_value(editor->z_value_editor,&(car_settings->z_value));
}

header_editor_t* header_editor_new()
{
header_editor_t* editor=malloc(sizeof(header_editor_t));
editor->container=gtk_vbox_new(FALSE,2);


editor->track_type_editor=track_type_editor_new();
gtk_box_pack_start(GTK_BOX(editor->container),editor->track_type_editor->container,FALSE,FALSE,2);

editor->flag_editor=flag_editor_new("Flags");
flag_editor_add_checkbox(editor->flag_editor,"Show as seperate ride",RIDE_SEPERATE);
flag_editor_add_checkbox(editor->flag_editor,"Ride is covered",RIDE_COVERED);
flag_editor_add_checkbox(editor->flag_editor,"Riders get wet",RIDE_WET);
gtk_box_pack_start(GTK_BOX(editor->container),editor->flag_editor->container,FALSE,FALSE,2);

editor->min_cars_editor=value_editor_new(VALUE_SIZE_BYTE,"Minimum cars per train:");
editor->max_cars_editor=value_editor_new(VALUE_SIZE_BYTE,"Maximum cars per train:");
gtk_box_pack_start(GTK_BOX(editor->container),editor->min_cars_editor->container,FALSE,FALSE,2);
gtk_box_pack_start(GTK_BOX(editor->container),editor->max_cars_editor->container,FALSE,FALSE,2);

GtkWidget* cars=gtk_notebook_new();
int i;
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

/*
//Create flags editor
GtkWidget* flagsFrame=gtk_frame_new("Flags");
GtkWidget* flagsTable=gtk_table_new(3,2,FALSE);
GtkWidget* seperateLabel=gtk_label_new("Show as seperate ride");
GtkWidget* coveredLabel=gtk_label_new("Ride is covered");
GtkWidget* wetLabel=gtk_label_new("Guests get wet");
GtkWidget* seperateCheckbox=gtk_check_button_new();
GtkWidget* coveredCheckbox=gtk_check_button_new();
GtkWidget* wetCheckbox=gtk_check_button_new();
gtk_table_attach_defaults(GTK_TABLE(flagsTable),seperateLabel,0,1,0,1);
gtk_table_attach_defaults(GTK_TABLE(flagsTable),coveredLabel,0,1,1,2);
gtk_table_attach_defaults(GTK_TABLE(flagsTable),wetLabel,0,1,2,3);
gtk_table_attach_defaults(GTK_TABLE(flagsTable),seperateCheckbox,1,2,0,1);
gtk_table_attach_defaults(GTK_TABLE(flagsTable),coveredCheckbox,1,2,1,2);
gtk_table_attach_defaults(GTK_TABLE(flagsTable),wetCheckbox,1,2,2,3);
gtk_container_add(GTK_CONTAINER(flagsFrame),flagsTable);
gtk_box_pack_start(GTK_BOX(interface->LeftVBox),flagsFrame,FALSE,FALSE,2);

//Create flags editor
GtkWidget* carsFrame=gtk_frame_new("Cars");
GtkWidget* carsTable=gtk_table_new(5,2,FALSE);
GtkWidget* defaultLabel=gtk_label_new("Default car");
GtkWidget* frontLabel=gtk_label_new("Front car");
GtkWidget* secondLabel=gtk_label_new("Second car");
GtkWidget* thirdLabel=gtk_label_new("Third car");
GtkWidget* rearLabel=gtk_label_new("Rear car");
GtkWidget* defaultSelect=gtk_combo_box_text_new();
GtkWidget* frontSelect=gtk_combo_box_text_new();
GtkWidget* secondSelect=gtk_combo_box_text_new();
GtkWidget* thirdSelect=gtk_combo_box_text_new();
GtkWidget* rearSelect=gtk_combo_box_text_new();
    for(i=0;i<NUM_CARS;i++)
    {
    char optionText[256];
    sprintf(optionText,"Car %d",i);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(defaultSelect),optionText);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(frontSelect),optionText);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(secondSelect),optionText);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(thirdSelect),optionText);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(rearSelect),optionText);
    }
gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(frontSelect),"Default");
gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(secondSelect),"Default");
gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(thirdSelect),"Default");
gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(rearSelect),"Default");
gtk_table_attach_defaults(GTK_TABLE(carsTable),defaultLabel,0,1,0,1);
gtk_table_attach_defaults(GTK_TABLE(carsTable),frontLabel,0,1,1,2);
gtk_table_attach_defaults(GTK_TABLE(carsTable),secondLabel,0,1,2,3);
gtk_table_attach_defaults(GTK_TABLE(carsTable),thirdLabel,0,1,3,4);
gtk_table_attach_defaults(GTK_TABLE(carsTable),rearLabel,0,1,4,5);
gtk_table_attach_defaults(GTK_TABLE(carsTable),defaultSelect,1,2,0,1);
gtk_table_attach_defaults(GTK_TABLE(carsTable),frontSelect,1,2,1,2);
gtk_table_attach_defaults(GTK_TABLE(carsTable),secondSelect,1,2,2,3);
gtk_table_attach_defaults(GTK_TABLE(carsTable),thirdSelect,1,2,3,4);
gtk_table_attach_defaults(GTK_TABLE(carsTable),rearSelect,1,2,4,5);
gtk_container_add(GTK_CONTAINER(carsFrame),carsTable);
gtk_box_pack_start(GTK_BOX(interface->LeftVBox),carsFrame,FALSE,FALSE,2);
*/
}
void header_editor_set_project(header_editor_t* editor,project_t* project)
{
int i;
editor->project=project;
track_type_editor_set_track_type(editor->track_type_editor,&(project->track_type));
flag_editor_set_flags(editor->flag_editor,&(project->flags));
value_editor_set_value(editor->min_cars_editor,&(project->minimum_cars));
value_editor_set_value(editor->max_cars_editor,&(project->maximum_cars));
    for(i=0;i<NUM_CARS;i++)
    {
    car_editor_set_car(editor->car_editors[i],&(project->cars[i]));
    }
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

static void main_window_add_animation(GtkWidget* widget,gpointer data)
{
main_window_t* main_window=(main_window_t*)data;
    if(main_window->project==NULL)return;
animation_t* animation=animation_new();
project_add_animation(main_window->project,animation);
animation_dialog_t* dialog=animation_dialog_new(animation);
animation_dialog_run(dialog);
animation_dialog_free(dialog);
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
    if(filename!=NULL)main_window_set_project(main_window,project_load(filename));
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
    object_save_dat(object,filename);
    object_free(object);
    }
}


void main_window_build_menus(main_window_t* main_window)
{
//Set up the menus
main_window->main_menu=gtk_menu_bar_new();
GtkWidget* file_menu_item=gtk_menu_item_new_with_label("File");
GtkWidget* model_menu_item=gtk_menu_item_new_with_label("Model");
GtkWidget* animation_menu_item=gtk_menu_item_new_with_label("Animation");
gtk_menu_shell_append(GTK_MENU_SHELL(main_window->main_menu),file_menu_item);
gtk_menu_shell_append(GTK_MENU_SHELL(main_window->main_menu),model_menu_item);
gtk_menu_shell_append(GTK_MENU_SHELL(main_window->main_menu),animation_menu_item);

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




main_window->animation_menu=gtk_menu_new();
GtkWidget* add_animation_menu_item=gtk_menu_item_new_with_label("Add Animation");
g_signal_connect(add_animation_menu_item,"activate",G_CALLBACK(main_window_add_animation),main_window);
gtk_menu_shell_append(GTK_MENU_SHELL(main_window->animation_menu),add_animation_menu_item);
gtk_menu_item_set_submenu(GTK_MENU_ITEM(animation_menu_item),main_window->animation_menu);
//g_signal_connect(addAnimationMenuItem,"activate",G_CALLBACK(AddNewAnimation),MainInterface);


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

main_window->header_editor=header_editor_new();
gtk_box_pack_start(GTK_BOX(main_window->main_vbox),main_window->header_editor->container,FALSE,FALSE,2);

gtk_widget_show_all(main_window->window);

return main_window;

/*
MainInterface->LeftVBox=gtk_vbox_new(FALSE,5);
MainInterface->LowerHBox=gtk_hbox_new(FALSE,5);

BuildMenus(MainInterface);
BuildStringHandler(MainInterface);
BuildHeaderEditor(MainInterface);

GtkWidget* renderButton=gtk_button_new_with_label("Render Sprites");

gtk_box_pack_start(GTK_BOX(MainInterface->LowerHBox),MainInterface->LeftVBox,TRUE,TRUE,0);
gtk_box_pack_start(GTK_BOX(MainInterface->MainVBox),MainInterface->LowerHBox,TRUE,TRUE,0);

BuildImageDisplay(MainInterface);
*/
}

void main_window_free(main_window_t* main_window)
{
//gtk_widget_destroy(main_window->window);
free(main_window);
}
