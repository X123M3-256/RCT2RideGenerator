#ifndef INTERFACE_H_INCLUDED
#define INTERFACE_H_INCLUDED
#include <gtk/gtk.h>
#include "dat.h"
#include "renderer.h"
#include "types.h"

typedef struct
{
GtkWidget* Window;
GtkWidget* ModelMenu;
ObjectFile* Dat;

//Image display widgets
int ImageDisplayCurrentImage;
GtkHBox* ImageDisplayHbox;
GtkButton* ImageDisplayPrevButton;
GtkButton* ImageDisplayNextButton;
GtkLabel* ImageDisplayPositionLabel;
GtkImage* ImageDisplayImage;
GdkPixbuf* ImageDisplayPixbuf;
GtkVBox* ImageDisplayVBox;
GtkEventBox* ImageDisplayEventBox;

//String editing widgets
int LanguageNum;
GtkComboBox* LanguageComboBox;
GtkLabel* RideNameLabel;
GtkEntry* RideNameEntry;
GtkLabel* RideDescriptionLabel;
GtkEntry* RideDescriptionEntry;
GtkLabel* RideCapacityLabel;
GtkEntry* RideCapacityEntry;
GtkTable* StringEditingTable;

GtkTextView* AnimationTextView;
GtkButton* AnimationRenderButton;

}MainWindow;

MainWindow* CreateInterface();
char* GetFilenameFromUser(char* Message,int action);
#endif // INTERFACE_H_INCLUDED
