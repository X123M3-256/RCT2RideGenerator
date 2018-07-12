#ifndef MAINWINDOW_H_INCLUDED
#define MAINWINDOW_H_INCLUDED
#include "interface.h"
#include "project.h"
#include <stdint.h>
/*
typedef struct
{
int ImageDisplayCurrentImage;
GtkHBox* ImageDisplayHbox;
GtkButton* ImageDisplayPrevButton;
GtkButton* ImageDisplayNextButton;
GtkLabel* ImageDisplayPositionLabel;
GtkImage* ImageDisplayImage;
GdkPixbuf* ImageDisplayPixbuf;
GtkVBox* ImageDisplayVBox;
GtkEventBox* ImageDisplayEventBox;
}image_viewer_t;

typedef struct
{
int LanguageNum;
GtkComboBox* LanguageComboBox;
GtkLabel* RideNameLabel;
GtkEntry* RideNameEntry;
GtkLabel* RideDescriptionLabel;
GtkEntry* RideDescriptionEntry;
GtkLabel* RideCapacityLabel;
GtkEntry* RideCapacityEntry;
GtkTable* StringEditingTable;
}string_editor_t;
*/

typedef struct { // changes the track type of the ride
    uint8_t* track_type;
    GtkWidget* container;
    GtkWidget* label;
    GtkWidget* select;
} track_type_editor_t;

typedef struct { // changes a single flag
    uint32_t* flags;
    uint32_t flag;
    GtkWidget* label;
    GtkWidget* checkbox;
} flag_checkbox_t;

typedef struct { // changes various flags
    uint32_t* flags;
    flag_checkbox_t** flag_checkboxes;
    int num_checkboxes;

    GtkWidget* container;
    GtkWidget* table;
} flag_editor_t;

typedef struct { // chooses car slot
    uint8_t* car_type;
    GtkWidget* container;
    GtkWidget* label;
    GtkWidget* car_select;
} car_type_editor_t;


typedef struct { // changes one of the two ride categories
    uint8_t* category;
    GtkWidget* container;
    GtkWidget* label;
    GtkWidget* select;
} ride_category_editor_t;

typedef struct { // holds the 2 ride category editors
	ride_category_editor_t** category_editors;
    GtkWidget* container;
} categories_editor_t;

typedef struct { // edits a single car at a time
    car_settings_t* car_settings;
    project_t* project;

    flag_editor_t* flag_editor;
    flag_editor_t* sprite_editor;
    value_selector_t* running_sound_editor;
    value_selector_t* secondary_sound_editor;
    value_editor_t* spacing_editor;
    value_editor_t* friction_editor;
    value_editor_t* z_value_editor;

	value_editor_t* spin_inertia_editor;
	value_editor_t* spin_friction_editor;
	value_editor_t* powered_acceleration_editor;
	value_editor_t* powered_velocity_editor;
	value_editor_t* car_visual_editor;
	value_editor_t* effect_visual_editor;

    GtkWidget* animation_button;
    GtkWidget* container;
	GtkWidget* left_vbox;
	GtkWidget* right_vbox;
} car_editor_t;


typedef struct {
	image_t** image;

	image_viewer_t* preview_viewer;
	GtkWidget* set_preview;
	GtkWidget* container;
} preview_editor_t;


typedef struct { // edits major ride properties
	project_t* project;

	preview_editor_t* preview_editor;
	string_editor_t* name_editor;
	string_editor_t* description_editor;
	track_type_editor_t* track_type_editor;
	categories_editor_t* categories_editor;
	flag_editor_t* flag_editor;
	GtkWidget* container;
} left_panel_t;

typedef struct { // edits train configuration
	project_t* project;

	value_editor_t* excitement_editor;
	value_editor_t* intensity_editor;
	value_editor_t* nausea_editor;
	value_editor_t* max_height_editor;
	value_editor_t* min_cars_editor;
	value_editor_t* max_cars_editor;
	value_editor_t* zero_cars_editor;
	value_editor_t* car_icon_index_editor;
	car_type_editor_t* default_car_editor;
	car_type_editor_t* front_car_editor;
	car_type_editor_t* second_car_editor;
	car_type_editor_t* third_car_editor;
	car_type_editor_t* rear_car_editor;
	GtkWidget* container;
} center_panel_t;

typedef struct { // edits individual cars
	project_t* project;

	car_editor_t* car_editors[NUM_CARS];
	GtkWidget* container;
} right_panel_t;

typedef struct {
    project_t* project;

	left_panel_t* left_panel;
	center_panel_t* center_panel;
	right_panel_t* right_panel;

    GtkWidget* window;
    GtkWidget* main_menu;
    GtkWidget* model_menu;
    GtkWidget* animation_menu;
    GtkWidget* main_vbox;
    GtkWidget* main_hbox;
    GtkWidget* left_vbox;
	GtkWidget* center_vbox;
	GtkWidget* right_vbox;

} main_window_t;

main_window_t* main_window_new();
void main_window_free(main_window_t* main_window);

#endif // MAINWINDOW_H_INCLUDED
