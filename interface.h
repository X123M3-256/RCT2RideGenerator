#ifndef INTERFACE_H_INCLUDED
#define INTERFACE_H_INCLUDED
#include <gtk/gtk.h>
#include "types.h"

typedef struct
{
GtkWindow* Window;
GtkVBox* MainVBox;
GtkHBox* LowerHBox;
GtkVBox* LeftVBox;

//Menu bar
GtkMenuBar* MainMenu;
GtkMenuItem* FileMenuItem;
GtkMenuItem* AnimationMenuItem;
//File menu;
GtkMenu* FileMenu;
GtkMenuItem* NewMenuItem;
GtkMenuItem* OpenMenuItem;
GtkMenuItem* SaveMenuItem;
//Animation menu
GtkMenu* AnimationMenu;
GtkMenuItem* AddObjectMenuItem;
GtkMenuItem* SaveTemplateMenuItem;

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
GtkEntry* RideCapacityEntry;
GtkTable* StringEditingTable;

GtkTextView* AnimationTextView;
GtkButton* AnimationRenderButton;

}MainWindow;

MainWindow* CreateInterface();
char* GetFilenameFromUser(char* Message);
#endif // INTERFACE_H_INCLUDED
