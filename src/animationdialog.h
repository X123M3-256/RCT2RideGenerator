#ifndef ANIMATIONDIALOG_H_INCLUDED
#define ANIMATIONDIALOG_H_INCLUDED
#include "animation.h"
#include "interface.h"
#include <gtk/gtk.h>

enum animation_dialog_variable_flags {
    ANIMATION_DIALOG_RESTRAINT = 1,
    ANIMATION_DIALOG_SPIN = 2,
    ANIMATION_DIALOG_SWING = 4,
    ANIMATION_DIALOG_FLIP = 8
};

typedef struct {
    animation_t* animation;
    model_t* grid_model;
    Matrix model_view;
    float variables[ANIMATION_NUM_VARIABLES];

    image_viewer_t* image_viewer;

    GtkWidget* container;

} animation_viewer_t;

typedef struct {
    model_t** models;
    int num_models;
    GtkWidget* container;
} model_selector_t;

typedef struct {
    animation_object_t* object;
    animation_object_t** objects;
    int num_objects;

    GtkWidget* parent_select;
    GtkWidget* x_entry;
    GtkWidget* y_entry;
    GtkWidget* z_entry;
    GtkWidget* pitch_entry;
    GtkWidget* roll_entry;
    GtkWidget* yaw_entry;
    GtkWidget* error_label;
    GtkWidget* container;
} animation_object_editor_t;

typedef struct {
    animation_t* animation;
    // int selected_object;

    model_selector_t* model_selector;
    animation_viewer_t* animation_viewer;
    animation_object_editor_t* object_editor;

    GtkWidget* dialog;
    GtkWidget* add_model;
    GtkWidget* pitch_slider;
    GtkWidget* yaw_slider;
    GtkWidget* roll_slider;
    GtkWidget* spin_slider;
    GtkWidget* swing_slider;
    GtkWidget* flip_slider;
    GtkWidget* restraint_slider;
} animation_dialog_t;

animation_dialog_t* animation_dialog_new(animation_t* animation,
    model_t** models,
    int num_models,
    int variable_flags);
void animation_dialog_run(animation_dialog_t* dialog);
void animation_dialog_free(animation_dialog_t* dialog);

#endif // ANIMATIONDIALOG_H_INCLUDED
