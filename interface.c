#include <stdlib.h>
#include "interface.h"
#include "palette.h"
#include "renderer.h"
#include "animation.h"

static MainWindow* MainInterface;

GdkPixbuf* CreateBlankPixbuf(){
GdkPixbuf* BlankPixbuf=gdk_pixbuf_new(GDK_COLORSPACE_RGB,FALSE,8,256,256);
int rowstride=gdk_pixbuf_get_rowstride(BlankPixbuf);
guchar* pixels=gdk_pixbuf_get_pixels(BlankPixbuf);
int i,j;
    for(i=0;i<256;i++)
    {
        for(j=0;j<256*3;j++)
        {
        pixels[j]=0;
        }
    pixels+=rowstride;
    }
return BlankPixbuf;
}


char* GetFilenameFromUser(char* Message)
{
GtkFileChooserDialog* FileDialog=gtk_file_chooser_dialog_new(Message,NULL,GTK_FILE_CHOOSER_ACTION_OPEN,GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,GTK_STOCK_OK,GTK_RESPONSE_OK,NULL);
char* Filename=NULL;
if(gtk_dialog_run(GTK_DIALOG(FileDialog))==GTK_RESPONSE_OK)
    {
    Filename=gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(FileDialog));
    };
gtk_widget_destroy(FileDialog);
return Filename;
}

/*
void AnimationUpdateText()
{
char* Text=GetAnimationFrameAsText(MainInterface->ImageDisplayCurrentImage);
if(Text!=NULL)
{
GtkTextBuffer* Buffer=gtk_text_view_get_buffer(MainInterface->AnimationTextView);
gtk_text_buffer_set_text(Buffer,Text,strlen(Text));
free(Text);
}
}
void AnimationUpdateData()
{
GtkTextIter Start,End;
GtkTextBuffer* Buffer=gtk_text_view_get_buffer(MainInterface->AnimationTextView);
gtk_text_buffer_get_bounds(Buffer,&Start,&End);
char* Text=gtk_text_buffer_get_text(Buffer,&Start,&End,FALSE);
SetAnimationFrameFromText(MainInterface->ImageDisplayCurrentImage,Text);
}
*/

void ImageViewerUpdate(){
int NumImages=GetNumImages();
if(MainInterface->ImageDisplayCurrentImage>=NumImages)MainInterface->ImageDisplayCurrentImage=0;
Image* RideImage=GetImage(MainInterface->ImageDisplayCurrentImage);
if(RideImage==NULL)return;
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
//AnimationUpdateData();
if(MainInterface->ImageDisplayCurrentImage+1<GetNumImages())MainInterface->ImageDisplayCurrentImage++;
ImageViewerUpdate();
//AnimationUpdateText();
}
void ImageViewerPrevImage(){
//AnimationUpdateData();
if(MainInterface->ImageDisplayCurrentImage>0)MainInterface->ImageDisplayCurrentImage--;
ImageViewerUpdate();
//AnimationUpdateText();
}
void StringHandlerLoad(){
char* Str=GetNameString(MainInterface->LanguageNum);
if(Str!=NULL)gtk_entry_set_text(MainInterface->RideNameEntry,Str);
else gtk_entry_set_text(MainInterface->RideNameEntry,"No text in this language");
Str=GetDescriptionString(MainInterface->LanguageNum);
if(Str!=NULL)gtk_entry_set_text(MainInterface->RideDescriptionEntry,Str);
else gtk_entry_set_text(MainInterface->RideDescriptionEntry,"No text in this language");
Str=GetCapacityString(MainInterface->LanguageNum);
if(Str!=NULL)gtk_entry_set_text(MainInterface->RideCapacityEntry,Str);
else gtk_entry_set_text(MainInterface->RideCapacityEntry,"No text in this language");
}


static gboolean DeleteEvent(GtkWidget* widget,GdkEvent* event,gpointer data){
return FALSE;
}
static void Exit(GtkWidget* widget,gpointer* data){
DestroyInterface();
gtk_main_quit();
}

static void OpenFile(GtkWidget* widget,gpointer* data){
char* Filename=GetFilenameFromUser("Select file to open");
if(Filename!=NULL)LoadFile(Filename);
ImageViewerUpdate();
StringHandlerLoad();
}
static void SaveDatFile(GtkWidget* widget,gpointer* data){

GtkFileChooserDialog* FileDialog=gtk_file_chooser_dialog_new("Select file to open",NULL,GTK_FILE_CHOOSER_ACTION_SAVE,GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,GTK_STOCK_OK,GTK_RESPONSE_OK,NULL);
char* Filename=NULL;
if(gtk_dialog_run(GTK_DIALOG(FileDialog))==GTK_RESPONSE_OK)
    {
    Filename=gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(FileDialog));
    SaveFile(Filename);
    };
gtk_widget_destroy(FileDialog);
}
static void OpenTemplateFile(GtkWidget* widget,gpointer* data){
char* Filename=GetFilenameFromUser("Select template file");
//if(Filename!=NULL)LoadTemplate(Filename);
}

static void NextImage(GtkWidget* widget,gpointer* data){
ImageViewerNextImage();
}
static void PrevImage(GtkWidget* widget,gpointer* data){
ImageViewerPrevImage();
}
static void MouseImage(GtkWidget *widget,GdkEvent *event,gpointer* data){
    if(((GdkEventScroll*)event)->direction==GDK_SCROLL_UP)
    {
    ImageViewerNextImage();
    }
    else if(((GdkEventScroll*)event)->direction==GDK_SCROLL_DOWN)
    {
    ImageViewerPrevImage();
    }
}

static void SetLanguage(GtkWidget* widget,gpointer* data){
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
static int UpdateNameString(GtkWidget* widget,gpointer* data){
SetNameString(MainInterface->LanguageNum,gtk_entry_get_text(MainInterface->RideNameEntry));
return FALSE;
}
static int UpdateDescriptionString(GtkWidget* widget,gpointer* data){
SetDescriptionString(MainInterface->LanguageNum,gtk_entry_get_text(MainInterface->RideDescriptionEntry));
return FALSE;
}
static int UpdateCapacityString(GtkWidget* widget,gpointer* data){
SetCapacityString(MainInterface->LanguageNum,gtk_entry_get_text(MainInterface->RideCapacityEntry));
return FALSE;
}

static int UpdateAnimationData(GtkWidget* widget,gpointer* data){
//AnimationUpdateData();
return FALSE;
}
static void RenderFrame(GtkWidget* widget,gpointer* data){
//RenderAnimation();
ImageViewerUpdate();
}

static void GetObjectFilename(GtkWidget* widget,gpointer* data)
{
GtkEntry* Entry=(GtkEntry*)data;
char* Filename=GetFilenameFromUser("Select object model:");
if(Filename!=NULL)gtk_entry_set_text(Entry,Filename);
}
static void AddNewObject(GtkWidget* widget,gpointer* data)
{
GtkLabel* NameLabel=gtk_label_new("Enter name for the object:");
GtkEntry* NameEntry=gtk_entry_new();
GtkLabel* FileLabel=gtk_label_new("Enter filename of model:");
GtkEntry* FileEntry=gtk_entry_new();
GtkButton* FileButton=gtk_button_new_with_label("Browse");
GtkHBox* FileHBox=gtk_hbox_new(FALSE,2);
gtk_box_pack_start(GTK_BOX(FileHBox),FileEntry,FALSE,FALSE,2);
gtk_box_pack_start(GTK_BOX(FileHBox),FileButton,FALSE,FALSE,2);

g_signal_connect(FileButton,"clicked",G_CALLBACK(GetObjectFilename),FileEntry);

GtkDialog* dialog=gtk_dialog_new_with_buttons("Add object",NULL,GTK_DIALOG_DESTROY_WITH_PARENT,"Ok",GTK_RESPONSE_OK,"Cancel",GTK_RESPONSE_CANCEL,NULL);
GtkBox* ContentBox=GTK_BOX(gtk_dialog_get_content_area(dialog));
gtk_box_pack_start(ContentBox,NameLabel,FALSE,FALSE,2);
gtk_box_pack_start(ContentBox,NameEntry,FALSE,FALSE,2);
gtk_box_pack_start(ContentBox,FileLabel,FALSE,FALSE,2);
gtk_box_pack_start(ContentBox,FileHBox,FALSE,FALSE,2);
gtk_widget_show_all(dialog);
    if(gtk_dialog_run(dialog)==GTK_RESPONSE_OK)
    {
     char* Name=gtk_entry_get_text(NameEntry);
     char* Filename=gtk_entry_get_text(FileEntry);
        if(strlen(Name)>0&&strlen(Filename)>0)
        {
        Object* Obj=CreateObject(Filename,Name);
//        AddObject(Obj);
        }
    }
gtk_widget_destroy(GTK_WIDGET(dialog));
}

MainWindow* CreateInterface()
{
MainInterface=malloc(sizeof(MainWindow));
MainInterface->Window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
g_signal_connect(MainInterface->Window,"delete-event",G_CALLBACK(DeleteEvent),NULL);
g_signal_connect(MainInterface->Window,"destroy",G_CALLBACK(Exit),NULL);

//Set up the menus
{
MainInterface->MainMenu=gtk_menu_bar_new();
MainInterface->FileMenuItem=gtk_menu_item_new_with_label("File");
MainInterface->AnimationMenuItem=gtk_menu_item_new_with_label("Animation");
gtk_menu_shell_append(GTK_MENU_SHELL(MainInterface->MainMenu),MainInterface->FileMenuItem);
gtk_menu_shell_append(GTK_MENU_SHELL(MainInterface->MainMenu),MainInterface->AnimationMenuItem);

MainInterface->FileMenu=gtk_menu_new();
MainInterface->NewMenuItem=gtk_menu_item_new_with_label("New from template");
MainInterface->OpenMenuItem=gtk_menu_item_new_with_label("Open DAT");
MainInterface->SaveMenuItem=gtk_menu_item_new_with_label("Save DAT");
gtk_menu_shell_append(GTK_MENU_SHELL(MainInterface->FileMenu),MainInterface->NewMenuItem);
gtk_menu_shell_append(GTK_MENU_SHELL(MainInterface->FileMenu),MainInterface->OpenMenuItem);
gtk_menu_shell_append(GTK_MENU_SHELL(MainInterface->FileMenu),MainInterface->SaveMenuItem);

MainInterface->AnimationMenu=gtk_menu_new();
MainInterface->AddObjectMenuItem=gtk_menu_item_new_with_label("Add Object");
MainInterface->SaveTemplateMenuItem=gtk_menu_item_new_with_label("Save template");
gtk_menu_shell_append(GTK_MENU_SHELL(MainInterface->AnimationMenu),MainInterface->AddObjectMenuItem);
gtk_menu_shell_append(GTK_MENU_SHELL(MainInterface->AnimationMenu),MainInterface->SaveTemplateMenuItem);

gtk_menu_item_set_submenu(MainInterface->FileMenuItem,MainInterface->FileMenu);
gtk_menu_item_set_submenu(MainInterface->AnimationMenuItem,MainInterface->AnimationMenu);
}
//Set up the image display interface
{
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
}
//Set up the string editing interface
{
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
}

MainInterface->AnimationTextView=gtk_text_view_new();
MainInterface->AnimationRenderButton=gtk_button_new_with_label("Render");

MainInterface->LeftVBox=gtk_vbox_new(FALSE,5);
gtk_box_pack_start(GTK_BOX(MainInterface->LeftVBox),MainInterface->StringEditingTable,FALSE,FALSE,0);
gtk_box_pack_start(GTK_BOX(MainInterface->LeftVBox),MainInterface->AnimationTextView,TRUE,TRUE,0);
gtk_box_pack_start(GTK_BOX(MainInterface->LeftVBox),MainInterface->AnimationRenderButton,FALSE,FALSE,0);
MainInterface->LowerHBox=gtk_hbox_new(FALSE,5);
gtk_box_pack_start(GTK_BOX(MainInterface->LowerHBox),MainInterface->LeftVBox,TRUE,TRUE,0);
gtk_box_pack_start(GTK_BOX(MainInterface->LowerHBox),MainInterface->ImageDisplayEventBox,FALSE,FALSE,0);


MainInterface->MainVBox=gtk_vbox_new(FALSE,5);
gtk_box_pack_start(GTK_BOX(MainInterface->MainVBox),MainInterface->MainMenu,FALSE,FALSE,0);
gtk_box_pack_start(GTK_BOX(MainInterface->MainVBox),MainInterface->LowerHBox,TRUE,TRUE,0);

gtk_container_add(GTK_CONTAINER(MainInterface->Window),MainInterface->MainVBox);

gtk_widget_show_all(MainInterface->Window);

//Set up callbacks for menu
g_signal_connect(MainInterface->NewMenuItem,"activate",G_CALLBACK(OpenTemplateFile),NULL);
g_signal_connect(MainInterface->OpenMenuItem,"activate",G_CALLBACK(OpenFile),NULL);
g_signal_connect(MainInterface->SaveMenuItem,"activate",G_CALLBACK(SaveDatFile),NULL);
g_signal_connect(MainInterface->AddObjectMenuItem,"activate",G_CALLBACK(AddNewObject),NULL);

//Set up callbacks for image viewer
g_signal_connect(MainInterface->ImageDisplayNextButton,"clicked",G_CALLBACK(NextImage),NULL);
g_signal_connect(MainInterface->ImageDisplayPrevButton,"clicked",G_CALLBACK(PrevImage),NULL);
g_signal_connect(MainInterface->ImageDisplayEventBox,"scroll-event",G_CALLBACK(MouseImage),NULL);

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

g_signal_connect(MainInterface->AnimationTextView,"focus-out-event",G_CALLBACK(UpdateAnimationData),NULL);
g_signal_connect(MainInterface->AnimationRenderButton,"clicked",G_CALLBACK(RenderFrame),NULL);


LoadTemplate("Canoes");
//AnimationUpdateText();
ImageViewerUpdate();
gtk_main();
}


void DestroyInterface()
{
g_object_unref(MainInterface->ImageDisplayPixbuf);
free(MainInterface);
}
