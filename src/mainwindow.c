#include "mainwindow.h"
#include "animationdialog.h"
#include "interface.h"
#include "modeldialog.h"
#include "palette.h"
#include "project.h"
#include "ridetypes.h"
#include "serialization.h"
#include <jansson.h>
#include <stdlib.h>
#include <string.h>

static char* get_filename(char* message, int action)
{
    GtkWidget* file_dialog = gtk_file_chooser_dialog_new(
        message, NULL, action, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
        GTK_STOCK_OK, GTK_RESPONSE_OK, NULL);
    char* filename = NULL;
    if (gtk_dialog_run(GTK_DIALOG(file_dialog)) == GTK_RESPONSE_OK) {
        filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(file_dialog));
    };
    gtk_widget_destroy(file_dialog);
    return filename;
}

static void track_type_editor_changed(GtkWidget* widget, gpointer data)
{
    track_type_editor_t* editor = (track_type_editor_t*)data;
    if (editor->track_type == NULL)
        return;
    // Get text from combo box
    const char* text = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(widget));
    if (text == NULL)
        return;
    *(editor->track_type) = ride_type_by_name(text)->id;
}

static track_type_editor_t* track_type_editor_new()
{
    int i;
    track_type_editor_t* editor = malloc(sizeof(track_type_editor_t));
    editor->track_type = NULL;
    editor->container = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 1);
    editor->label = gtk_label_new("Track type:");
    editor->select = gtk_combo_box_text_new();
    gtk_widget_set_sensitive(editor->select, FALSE);
    gtk_box_pack_start(GTK_BOX(editor->container), editor->label, FALSE, FALSE,
        1);
    gtk_box_pack_start(GTK_BOX(editor->container), editor->select, FALSE, FALSE,
        1);
    for (i = 0; i < NUM_RIDE_TYPES; i++) {
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(editor->select),
            ride_type_by_index(i)->name);
    }
    g_signal_connect(editor->select, "changed",
        G_CALLBACK(track_type_editor_changed), editor);
    return editor;
}
static void track_type_editor_set_track_type(track_type_editor_t* editor,
    uint8_t* track_type)
{
    int i;
    editor->track_type = track_type;
    gtk_widget_set_sensitive(editor->select, TRUE);
    for (i = 0; i < NUM_RIDE_TYPES; i++) {
        if (ride_type_by_index(i)->id == *track_type) {
            gtk_combo_box_set_active(GTK_COMBO_BOX(editor->select), i);
            break;
        }
    }
}

static void flag_editor_checkbox_toggled(GtkWidget* widget, gpointer data)
{
    flag_checkbox_t* flag_checkbox = (flag_checkbox_t*)data;
    if (flag_checkbox->flags == NULL)
        return;
    int value = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(flag_checkbox->checkbox));
    if (value)
        *(flag_checkbox->flags) |= flag_checkbox->flag;
    else
        *(flag_checkbox->flags) &= ~flag_checkbox->flag;
}

static flag_editor_t* flag_editor_new(const char* label)
{
    flag_editor_t* editor = malloc(sizeof(flag_editor_t));
    editor->flags = NULL;
    editor->num_checkboxes = 0;
    editor->flag_checkboxes = NULL;
    editor->container = gtk_frame_new(label);
    editor->table = gtk_table_new(FALSE, 1, 0);
    gtk_container_add(GTK_CONTAINER(editor->container), editor->table);
    return editor;
}

static void flag_editor_add_checkbox(flag_editor_t* editor,
    const char* label,
    uint32_t flag)
{
    editor->flag_checkboxes = realloc(editor->flag_checkboxes,
        (editor->num_checkboxes + 1) * sizeof(flag_checkbox_t*));
    editor->flag_checkboxes[editor->num_checkboxes] = malloc(sizeof(flag_checkbox_t));
    flag_checkbox_t* checkbox = editor->flag_checkboxes[editor->num_checkboxes++];
    checkbox->flags = NULL;
    checkbox->flag = flag;
    checkbox->label = gtk_label_new(label);
    checkbox->checkbox = gtk_check_button_new();
    gtk_widget_set_sensitive(checkbox->checkbox, FALSE);
    gtk_table_resize(GTK_TABLE(editor->table), editor->num_checkboxes, 2);
    gtk_table_attach_defaults(GTK_TABLE(editor->table), checkbox->label, 0, 1,
        editor->num_checkboxes - 1, editor->num_checkboxes);
    gtk_table_attach_defaults(GTK_TABLE(editor->table), checkbox->checkbox, 1, 2,
        editor->num_checkboxes - 1, editor->num_checkboxes);
    g_signal_connect(checkbox->checkbox, "toggled",
        G_CALLBACK(flag_editor_checkbox_toggled), checkbox);
}

static void flag_editor_set_flags(flag_editor_t* editor, uint32_t* flags)
{
    int i;
    editor->flags = flags;
    for (i = 0; i < editor->num_checkboxes; i++) {
        editor->flag_checkboxes[i]->flags = flags;
        gtk_widget_set_sensitive(editor->flag_checkboxes[i]->checkbox, TRUE);
        if ((*flags) & editor->flag_checkboxes[i]->flag)
            gtk_toggle_button_set_active(
                GTK_TOGGLE_BUTTON(editor->flag_checkboxes[i]->checkbox), TRUE);
        else
            gtk_toggle_button_set_active(
                GTK_TOGGLE_BUTTON(editor->flag_checkboxes[i]->checkbox), FALSE);
    }
}

static void car_type_editor_changed(GtkWidget* widget, gpointer data)
{
    car_type_editor_t* editor = (car_type_editor_t*)data;
    if (editor->car_type == NULL)
        return;
    int active = gtk_combo_box_get_active(GTK_COMBO_BOX(editor->car_select));
    if (editor->include_default)
    {
        switch (active) {
        case 0:
            *(editor->car_type) = 0xFF;
            break;
        case 1:
        case 2:
        case 3:
        case 4:
            *(editor->car_type) = active - 1;
            break;
        }
    }
    else
    {
        *(editor->car_type) = active;
    }
}
static car_type_editor_t* car_type_editor_new(const char* label, int include_default)
{
    int i;
    car_type_editor_t* editor = malloc(sizeof(car_type_editor_t));
    editor->car_type = NULL;
    editor->container = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 1);
    editor->label = gtk_label_new(label);
    editor->car_select = gtk_combo_box_text_new();
    gtk_widget_set_sensitive(editor->car_select, FALSE);
    editor->include_default = 0;
    if (include_default)
    {
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(editor->car_select),
            "Default");
        editor->include_default = 1;
    }
    for (i = 0; i < NUM_CARS; i++) {
        char option_text[256];
        sprintf(option_text, "Car %d", i);
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(editor->car_select),
            option_text);
    }
    gtk_combo_box_set_active(GTK_COMBO_BOX(editor->car_select), 0);
    g_signal_connect(editor->car_select, "changed",
        G_CALLBACK(car_type_editor_changed), editor);
    gtk_box_pack_start(GTK_BOX(editor->container), editor->label, FALSE, FALSE,
        2);
    gtk_box_pack_start(GTK_BOX(editor->container), editor->car_select, FALSE,
        FALSE, 2);
    return editor;
}

static void car_type_editor_set_car_type(car_type_editor_t* editor,
    uint8_t* car_type)
{
    editor->car_type = car_type;
    gtk_widget_set_sensitive(editor->car_select, TRUE);
    if (editor->include_default)
    {
        switch (*car_type) {
        case 0xFF:
            gtk_combo_box_set_active(GTK_COMBO_BOX(editor->car_select), 0);
            break;
        case 0:
        case 1:
        case 2:
        case 3:
            gtk_combo_box_set_active(GTK_COMBO_BOX(editor->car_select),
                (*car_type) + 1);
            break;
        }
    }
    else
    {
        gtk_combo_box_set_active(GTK_COMBO_BOX(editor->car_select),
            (*car_type));
    }
}

static void animation_type_editor_changed(GtkWidget* widget, gpointer data)
{
    animation_type_editor_t* editor = (animation_type_editor_t*)data;
    if (editor->animation_type == NULL)
        return;
    int active = gtk_combo_box_get_active(GTK_COMBO_BOX(editor->animation_select));
    *(editor->animation_type) = active;
}
static animation_type_editor_t* animation_type_editor_new(const char* label)
{
    int i;
    animation_type_editor_t* editor = malloc(sizeof(animation_type_editor_t));
    editor->animation_type = NULL;
    editor->container = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 1);
    editor->label = gtk_label_new(label);
    editor->animation_select = gtk_combo_box_text_new();
    gtk_widget_set_sensitive(editor->animation_select, FALSE);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(editor->animation_select),
        "None");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(editor->animation_select),
        "Steam Locomotive");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(editor->animation_select),
        "Swan Boats");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(editor->animation_select),
        "Canoes");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(editor->animation_select),
        "Rowboats");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(editor->animation_select),
        "Water Tricycles");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(editor->animation_select),
        "Observation Tower");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(editor->animation_select),
        "Helicars");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(editor->animation_select),
        "Monorail Cycles");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(editor->animation_select),
        "Multidimension Coaster");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(editor->animation_select),
        "Flying Animal");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(editor->animation_select),
        "Walking Animal");

    gtk_combo_box_set_active(GTK_COMBO_BOX(editor->animation_select), 0);
    g_signal_connect(editor->animation_select, "changed",
        G_CALLBACK(animation_type_editor_changed), editor);
    gtk_box_pack_start(GTK_BOX(editor->container), editor->label, FALSE, FALSE,
        2);
    gtk_box_pack_start(GTK_BOX(editor->container), editor->animation_select, FALSE,
        FALSE, 2);
    return editor;
}

static void animation_type_editor_set_animation_type(animation_type_editor_t* editor,
    uint8_t* animation_type)
{
    editor->animation_type = animation_type;
    gtk_widget_set_sensitive(editor->animation_select, TRUE);
    gtk_combo_box_set_active(GTK_COMBO_BOX(editor->animation_select), (*animation_type));
}

static void car_editor_edit_animation(GtkWidget* widget, gpointer data)
{
    car_editor_t* editor = (car_editor_t*)data;
    if (editor->project == NULL || editor->car_settings == NULL)
        return;
    int variable_flags = 0;
    if (editor->car_settings->flags & CAR_IS_SPINNING)
        variable_flags |= ANIMATION_DIALOG_SPIN;
    if (editor->car_settings->flags & CAR_IS_SWINGING)
        variable_flags |= ANIMATION_DIALOG_SWING;
    if (editor->car_settings->flags & CAR_IS_ANIMATED)
        variable_flags |= ANIMATION_DIALOG_ANIMATION;
    if (editor->car_settings->sprites & SPRITE_RESTRAINT_ANIMATION)
        variable_flags |= ANIMATION_DIALOG_RESTRAINT;
    animation_dialog_t* dialog = animation_dialog_new(
        editor->car_settings->animation, editor->project->models,
        editor->project->num_models, variable_flags);
    animation_dialog_run(dialog);
    animation_dialog_free(dialog);
}

static car_editor_t* car_editor_new()
{
    car_editor_t* editor = malloc(sizeof(car_editor_t));
    editor->car_settings = NULL;
	editor->container = gtk_hbox_new(FALSE, 1);
    editor->right_vbox = gtk_vbox_new(FALSE, 1);
	editor->left_vbox = gtk_vbox_new(FALSE, 1);

	gtk_box_pack_start(GTK_BOX(editor->container), editor->left_vbox, FALSE, FALSE, 1);
	gtk_box_pack_start(GTK_BOX(editor->container), editor->right_vbox, FALSE, FALSE, 1);

    editor->flag_editor = flag_editor_new("Car Flags");
    flag_editor_add_checkbox(editor->flag_editor, "Riders scream", CAR_ENABLE_ROLLING_SOUND);
    flag_editor_add_checkbox(editor->flag_editor, "Enable remap color 2", CAR_ENABLE_REMAP2);
    flag_editor_add_checkbox(editor->flag_editor, "Enable remap color 3", CAR_ENABLE_REMAP3);
    flag_editor_add_checkbox(editor->flag_editor, "No upstops -0.45 G", CAR_NO_UPSTOPS);
	flag_editor_add_checkbox(editor->flag_editor, "No upstops -0.4 G", CAR_NO_UPSTOPS_TIGHT_TOLERANCE);
    flag_editor_add_checkbox(editor->flag_editor, "Opens and closes doors", CAR_OPENS_DOORS);
    flag_editor_add_checkbox(editor->flag_editor, "Swinging", CAR_IS_SWINGING);
    flag_editor_add_checkbox(editor->flag_editor, "Spinning", CAR_IS_SPINNING);
    flag_editor_add_checkbox(editor->flag_editor, "Powered", CAR_IS_POWERED);
    flag_editor_add_checkbox(editor->flag_editor, "Car animated", CAR_IS_ANIMATED);
    flag_editor_add_checkbox(editor->flag_editor, "Rider animated", CAR_FLAG_RIDER_ANIMATION);
    flag_editor_add_checkbox(editor->flag_editor, "Can invert for long periods", CAR_CAN_INVERT);
    flag_editor_add_checkbox(editor->flag_editor, "Boat wandering and collision", CAR_WANDERS);
    flag_editor_add_checkbox(editor->flag_editor, "Powered cars freewheel downhill", CAR_COASTS_DOWNHILL);
	flag_editor_add_checkbox(editor->flag_editor, "Powered car powered by water", CAR_FLAG_WATER_PROPULSION);
    flag_editor_add_checkbox(editor->flag_editor, "Extra power on ascent", CAR_EXTRA_POWER_ON_ASCENT);
	flag_editor_add_checkbox(editor->flag_editor, "Chairlift car", CAR_IS_CHAIRLIFT);
	flag_editor_add_checkbox(editor->flag_editor, "Go-kart car", CAR_IS_GO_KART);
	flag_editor_add_checkbox(editor->flag_editor, "Swing Flag 21", CAR_FLAG_21);
	flag_editor_add_checkbox(editor->flag_editor, "Swing Flag 25", CAR_FLAG_25);
	flag_editor_add_checkbox(editor->flag_editor, "Swing Flag 27 (SLIDE_SWING)", CAR_FLAG_27);
	flag_editor_add_checkbox(editor->flag_editor, "Reverser bogie", CAR_FLAG_4);
	flag_editor_add_checkbox(editor->flag_editor, "Reverser car", CAR_FLAG_5);
	flag_editor_add_checkbox(editor->flag_editor, "Recalculate sprite bounds", CAR_RECALCULATE_SPRITE_BOUNDS);
	flag_editor_add_checkbox(editor->flag_editor, "Spinning uses 16 frames", CAR_USE_16_ROTATION_FRAMES);
    flag_editor_add_checkbox(editor->flag_editor, "Dodgem car", CAR_FLAG_DODGEM_CAR_PLACEMENT);
    flag_editor_add_checkbox(editor->flag_editor, "Dodgem car lights", CAR_DODGEM_USE_LIGHTS);
    flag_editor_add_checkbox(editor->flag_editor, "2D loading", CAR_FLAG_2D_LOADING_WAYPOINTS);
    flag_editor_add_checkbox(editor->flag_editor, "Sprite bound recalc. include inverted", SPRITE_BOUNDS_INCLUDE_INVERTED_SET);
    flag_editor_add_checkbox(editor->flag_editor, "Override vertical frames", CAR_OVERRIDE_VERTICAL_FRAMES);
    gtk_box_pack_start(GTK_BOX(editor->left_vbox), editor->flag_editor->container, FALSE, FALSE, 1);

    editor->sprite_editor = flag_editor_new("Sprites");
    flag_editor_add_checkbox(editor->sprite_editor, "Gentle slopes",
        SPRITE_GENTLE_SLOPE);
    flag_editor_add_checkbox(editor->sprite_editor, "Steep slopes",
        SPRITE_STEEP_SLOPE);
    flag_editor_add_checkbox(editor->sprite_editor, "Vertical slopes/Loops",
        SPRITE_VERTICAL_SLOPE);
    flag_editor_add_checkbox(editor->sprite_editor, "Diagonal slopes",
        SPRITE_DIAGONAL_SLOPE);
    flag_editor_add_checkbox(editor->sprite_editor, "Banked turns",
        SPRITE_BANKING);
    flag_editor_add_checkbox(editor->sprite_editor, "Sloped banked turns",
        SPRITE_SLOPED_BANKED_TURN);
    flag_editor_add_checkbox(editor->sprite_editor, "Inline twists",
        SPRITE_INLINE_TWIST);
    flag_editor_add_checkbox(editor->sprite_editor, "Corkscrews",
        SPRITE_CORKSCREW);
    flag_editor_add_checkbox(editor->sprite_editor, "Animated restraints",
        SPRITE_RESTRAINT_ANIMATION);
    flag_editor_add_checkbox(editor->sprite_editor, "Spiral lift (unused)",
        SPRITE_SPIRAL_LIFT);
    gtk_box_pack_start(GTK_BOX(editor->left_vbox),
        editor->sprite_editor->container, FALSE, FALSE, 1);

    editor->running_sound_editor = value_selector_new("Running sound:");
    value_selector_add_selection(editor->running_sound_editor, "None",
        RUNNING_SOUND_NONE);
    value_selector_add_selection(editor->running_sound_editor, "Wooden coaster 1",
        RUNNING_SOUND_WOODEN_MODERN);
    value_selector_add_selection(editor->running_sound_editor, "Wooden coaster 2",
        RUNNING_SOUND_WOODEN_OLD);
    value_selector_add_selection(editor->running_sound_editor, "Steel coaster 1",
        RUNNING_SOUND_STEEL);
    value_selector_add_selection(editor->running_sound_editor, "Steel coaster 2",
        RUNNING_SOUND_STEEL_SMOOTH);
    value_selector_add_selection(editor->running_sound_editor, "Waterslide",
        RUNNING_SOUND_WATERSLIDE);
    value_selector_add_selection(editor->running_sound_editor, "Train",
        RUNNING_SOUND_TRAIN);
    value_selector_add_selection(editor->running_sound_editor, "Go-Kart",
        RUNNING_SOUND_ENGINE);

    editor->secondary_sound_editor = value_selector_new("Secondary sound:");
    value_selector_add_selection(editor->secondary_sound_editor, "None",
        SECONDARY_SOUND_NONE);
    value_selector_add_selection(editor->secondary_sound_editor, "Screams 1",
        SECONDARY_SOUND_SCREAMS_1);
    value_selector_add_selection(editor->secondary_sound_editor, "Screams 2",
        SECONDARY_SOUND_SCREAMS_2);
    value_selector_add_selection(editor->secondary_sound_editor, "Screams 3",
        SECONDARY_SOUND_SCREAMS_3);
    value_selector_add_selection(editor->secondary_sound_editor, "Train whistle",
        SECONDARY_SOUND_WHISTLE);
    value_selector_add_selection(editor->secondary_sound_editor, "Bell",
        SECONDARY_SOUND_BELL);

    editor->spacing_editor = value_editor_new(VALUE_SIZE_DWORD, "Spacing:");
    editor->friction_editor = value_editor_new(VALUE_SIZE_WORD, "Mass:");
    editor->z_value_editor = value_editor_new(VALUE_SIZE_BYTE, "Z Value:");
    editor->vehicle_tab_vertical_offset_editor = value_editor_new(VALUE_SIZE_BYTE_SIGNED, "Vertical\nTab Offset:");
    editor->powered_acceleration_editor = value_editor_new(VALUE_SIZE_BYTE, "Powered\nAcceleration:");
    editor->powered_velocity_editor = value_editor_new(VALUE_SIZE_BYTE, "Powered\nVelocity:");

	editor->spin_inertia_editor = value_editor_new(VALUE_SIZE_BYTE, "Spin Inertia:");
	editor->spin_friction_editor = value_editor_new(VALUE_SIZE_BYTE, "Spin Friction:");

    editor->logflume_reverser_vehicle_editor = value_editor_new(VALUE_SIZE_BYTE, "Logflume\nReverser\n Vehicle:");
    editor->animation_type_selector = animation_type_editor_new("Animation Type:");

	editor->car_visual_editor = value_editor_new(VALUE_SIZE_BYTE, "Car Visual:");
	editor->effect_visual_editor = value_editor_new(VALUE_SIZE_BYTE, "Effect Visual:");
    editor->override_vertical_frames_editor = value_editor_new(VALUE_SIZE_BYTE, "Override\nVertical Frames:");

    editor->animation_button = gtk_button_new_with_label("Edit Animation");
    g_signal_connect(editor->animation_button, "clicked",
        G_CALLBACK(car_editor_edit_animation), editor);

    gtk_box_pack_start(GTK_BOX(editor->right_vbox), editor->animation_button, FALSE, FALSE, 1);
    gtk_box_pack_start(GTK_BOX(editor->right_vbox), editor->running_sound_editor->container, FALSE, FALSE, 1);
    gtk_box_pack_start(GTK_BOX(editor->right_vbox), editor->secondary_sound_editor->container, FALSE, FALSE, 1);
    gtk_box_pack_start(GTK_BOX(editor->right_vbox), editor->spacing_editor->container, FALSE, FALSE, 1);
    gtk_box_pack_start(GTK_BOX(editor->right_vbox), editor->friction_editor->container, FALSE, FALSE, 1);
    gtk_box_pack_start(GTK_BOX(editor->right_vbox), editor->z_value_editor->container, FALSE, FALSE, 1);
    gtk_box_pack_start(GTK_BOX(editor->right_vbox), editor->vehicle_tab_vertical_offset_editor->container, FALSE, FALSE, 1);
    gtk_box_pack_start(GTK_BOX(editor->right_vbox), editor->animation_type_selector->container, FALSE, FALSE, 1);

	gtk_box_pack_start(GTK_BOX(editor->right_vbox), editor->powered_velocity_editor->container, FALSE, FALSE, 1);
	gtk_box_pack_start(GTK_BOX(editor->right_vbox), editor->powered_acceleration_editor->container, FALSE, FALSE, 1);
	gtk_box_pack_start(GTK_BOX(editor->right_vbox), editor->spin_inertia_editor->container, FALSE, FALSE, 1);
	gtk_box_pack_start(GTK_BOX(editor->right_vbox), editor->spin_friction_editor->container, FALSE, FALSE, 1);
	gtk_box_pack_start(GTK_BOX(editor->right_vbox), editor->car_visual_editor->container, FALSE, FALSE, 1);
	gtk_box_pack_start(GTK_BOX(editor->right_vbox), editor->effect_visual_editor->container, FALSE, FALSE, 1);
    gtk_box_pack_start(GTK_BOX(editor->right_vbox), editor->override_vertical_frames_editor->container, FALSE, FALSE, 1);

    return editor;
}
static void car_editor_set_project(car_editor_t* editor, project_t* project)
{
    editor->project = project;
}

static void car_editor_set_car(car_editor_t* editor, car_settings_t* car_settings)
{
    editor->car_settings = car_settings;
    flag_editor_set_flags(editor->flag_editor, &(car_settings->flags));
    flag_editor_set_flags(editor->sprite_editor, &(car_settings->sprites));
    value_selector_set_value(editor->running_sound_editor,
        &(car_settings->running_sound));
    value_selector_set_value(editor->secondary_sound_editor,
        &(car_settings->secondary_sound));
    value_editor_set_value(editor->spacing_editor, &(car_settings->spacing));
    value_editor_set_value(editor->friction_editor, &(car_settings->friction));
    value_editor_set_value(editor->z_value_editor, &(car_settings->z_value));

	value_editor_set_value(editor->spin_inertia_editor, &(car_settings->spin_inertia));
	value_editor_set_value(editor->spin_friction_editor, &(car_settings->spin_friction));
	value_editor_set_value(editor->powered_acceleration_editor, &(car_settings->powered_acceleration));
	value_editor_set_value(editor->powered_velocity_editor, &(car_settings->powered_velocity));
	value_editor_set_value(editor->car_visual_editor, &(car_settings->car_visual));
	value_editor_set_value(editor->effect_visual_editor, &(car_settings->effect_visual));
    animation_type_editor_set_animation_type(editor->animation_type_selector, &(car_settings->animation_type));
    value_editor_set_value(editor->logflume_reverser_vehicle_editor, &(car_settings->logflume_reverser_vehicle));
    value_editor_set_value(editor->vehicle_tab_vertical_offset_editor, &(car_settings->vehicle_tab_vertical_offset));
    value_editor_set_value(editor->override_vertical_frames_editor, &(car_settings->override_vertical_frames));
}

static void preview_editor_set_preview_pressed(GtkWidget* widget,
	gpointer data)
{
	preview_editor_t* editor = (preview_editor_t*)data;

	char* filename = get_filename("Select preview image", GTK_FILE_CHOOSER_ACTION_OPEN);

	if (filename == NULL)
		return;

	GError* error = NULL;
	GdkPixbuf* pixbuf = gdk_pixbuf_new_from_file(filename, &error);
	if (!pixbuf) {
		show_error(error->message);
		return;
	}

	if (gdk_pixbuf_get_width(pixbuf) != 112 || gdk_pixbuf_get_height(pixbuf) != 112) {
		show_error("Preview images must be 112 by 112 pixels");
		return;
	}

	image_free(*(editor->image));
	image_t* image = image_new(112, 112, 0);
	// Set image from pixbuf
	int rowstride = gdk_pixbuf_get_rowstride(pixbuf);
	guchar* pixels = gdk_pixbuf_get_pixels(pixbuf);
	color_t color;
	int i, j;
	for (i = 0; i < 112; i++) {
		for (j = 0; j < 112; j++) {
			color.red = pixels[j * 3];
			color.green = pixels[j * 3 + 1];
			color.blue = pixels[j * 3 + 2];
			image->data[i][j] = palette_index_from_color(color);
		}
		pixels += rowstride;
	}
	*(editor->image) = image;
	image_viewer_set_image(editor->preview_viewer, image);
}
preview_editor_t* preview_editor_new()
{
	preview_editor_t* editor = malloc(sizeof(preview_editor_t));
	editor->image = NULL;
	editor->container = gtk_vbox_new(FALSE, 2);

	editor->preview_viewer = image_viewer_new();
	gtk_box_pack_start(GTK_BOX(editor->container),
		editor->preview_viewer->container, FALSE, FALSE, 2);

	editor->set_preview = gtk_button_new_with_label("Set preview image");
	g_signal_connect(editor->set_preview, "clicked",
		G_CALLBACK(preview_editor_set_preview_pressed), editor);
	gtk_box_pack_start(GTK_BOX(editor->container), editor->set_preview, FALSE,
		FALSE, 2);

	return editor;
}
void preview_editor_set_image(preview_editor_t* editor, image_t** image)
{
	editor->image = image;
	image_viewer_set_image(editor->preview_viewer, *image);
}

static void ride_category_editor_changed(GtkWidget* widget, gpointer data)
{
	ride_category_editor_t* editor = (ride_category_editor_t*)data;
	if (editor->category == NULL)
		return;
	// Get text from combo box
	const char* text = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(widget));
	if (text == NULL)
		return;
	*(editor->category) = ride_category_by_name(text)->id;
}
ride_category_editor_t* ride_category_editor_new(const char* label)
{
	int i;
	ride_category_editor_t* editor = malloc(sizeof(ride_category_editor_t));
	editor->category = NULL;
	editor->container = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 1);
	editor->label = gtk_label_new(label);
	editor->select = gtk_combo_box_text_new();
	gtk_widget_set_sensitive(editor->select, FALSE);
	gtk_box_pack_start(GTK_BOX(editor->container), editor->label, FALSE, FALSE,
		1);
	gtk_box_pack_start(GTK_BOX(editor->container), editor->select, FALSE, FALSE,
		1);
	for (i = 0; i < NUM_RIDE_CATEGORIES; i++) {
		gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(editor->select),
			ride_category_by_index(i)->name);
	}
	g_signal_connect(editor->select, "changed",
		G_CALLBACK(ride_category_editor_changed), editor);
	return editor;
}
void ride_category_editor_set_category(ride_category_editor_t* editor,
	uint8_t* category)
{
	int i;
	editor->category = category;
	gtk_widget_set_sensitive(editor->select, TRUE);
	for (i = 0; i < NUM_RIDE_CATEGORIES; i++) {
		if (ride_category_by_index(i)->id == *category) {
			gtk_combo_box_set_active(GTK_COMBO_BOX(editor->select), i);
			break;
		}
	}
}

categories_editor_t* categories_editor_new()
{
	int i;
    categories_editor_t* editor = malloc(sizeof(categories_editor_t));
	editor->category_editors = NULL;
	GtkWidget* pretty_frame = gtk_vbox_new(FALSE, 1);
	editor->container = gtk_frame_new("Ride Categories");
	gtk_container_add(GTK_CONTAINER(editor->container), pretty_frame);
	for (i=0; i < 2; i++)
	{
		char input_text[13];
		sprintf(input_text, "Category %d:", i + 1);
		editor->category_editors = realloc(editor->category_editors, (i + 1) * sizeof(ride_category_editor_t));
		editor->category_editors[i] = ride_category_editor_new(input_text);
		gtk_box_pack_start(GTK_BOX(pretty_frame), editor->category_editors[i]->container, FALSE, FALSE, 2);
	}
	return editor;
}

void categories_editor_set_project(categories_editor_t* editor, project_t* project)
{
	int i;
	for (i=0; i < 2; i++)
	{
		ride_category_editor_set_category(editor->category_editors[i],&project->ride_categories[i]);
	}
}


left_panel_t* left_panel_new()
{
	left_panel_t* editor = malloc(sizeof(left_panel_t));
	editor->project = NULL;
	editor->container = gtk_vbox_new(FALSE, 2);


	editor->preview_editor = preview_editor_new();
	gtk_box_pack_start(GTK_BOX(editor->container), editor->preview_editor->container, FALSE, FALSE, 2);

	editor->name_editor = string_editor_new("Name:");
	gtk_box_pack_start(GTK_BOX(editor->container), editor->name_editor->container, FALSE, FALSE, 2);

	editor->description_editor = string_editor_new("Description:");
	gtk_box_pack_start(GTK_BOX(editor->container), editor->description_editor->container, FALSE, FALSE, 2);

	editor->track_type_editor = track_type_editor_new(); // MAKING THE TRACK TYPE EDITOR
	gtk_box_pack_start(GTK_BOX(editor->container), editor->track_type_editor->container, FALSE, FALSE, 2);

	editor->categories_editor = categories_editor_new(); // MAKING THE CATEGORIES EDITOR
	gtk_box_pack_start(GTK_BOX(editor->container), editor->categories_editor->container, FALSE, FALSE, 2);

	editor->flag_editor = flag_editor_new("Ride Flags"); // MAKING THE RIDE FLAG EDITOR
	flag_editor_add_checkbox(editor->flag_editor, "Half-scale ride tab",
		RIDE_VEHICLE_TAB_SCALE_HALF);
	flag_editor_add_checkbox(editor->flag_editor, "Disallow inversions",
		RIDE_NO_INVERSIONS);
	flag_editor_add_checkbox(editor->flag_editor, "Disallow banked track",
		RIDE_NO_BANKED_TRACK);
	flag_editor_add_checkbox(editor->flag_editor, "Chuffing on depart",
		RIDE_CHUFFING_ON_DEPART);
	flag_editor_add_checkbox(editor->flag_editor, "Disable wandering",
		RIDE_DISABLE_WANDERING);
	flag_editor_add_checkbox(editor->flag_editor, "Water ride propulsion",
		RIDE_PLAY_SPLASH_SOUND);
	flag_editor_add_checkbox(editor->flag_editor, "Coaster boats propulsion",
		RIDE_PLAY_SPLASH_SOUND_SLIDE);
	flag_editor_add_checkbox(editor->flag_editor, "Riders protected from rain",
		RIDE_COVERED);
	flag_editor_add_checkbox(editor->flag_editor, "Limit airtime bonus",
		RIDE_LIMIT_AIRTIME_BONUS);
	flag_editor_add_checkbox(editor->flag_editor, "Disable breakdowns",
		RIDE_CANNOT_BREAK_DOWN);
	flag_editor_add_checkbox(editor->flag_editor, "Disable collision crashes",
		RIDE_FLAG_DISABLE_COLLISION_CRASHES);
	flag_editor_add_checkbox(editor->flag_editor, "Disable paint tab",
		RIDE_DISABLE_COLOR_TAB);
	gtk_box_pack_start(GTK_BOX(editor->container), editor->flag_editor->container,
		FALSE, FALSE, 2);
	/**/
	return editor;
}
void left_panel_set_project(left_panel_t* editor, project_t* project)
{
	editor->project = project;

	preview_editor_set_image(editor->preview_editor, &(project->preview_image));

	string_editor_set_string(editor->name_editor, &(project->name));
	string_editor_set_string(editor->description_editor, &(project->description));

	track_type_editor_set_track_type(editor->track_type_editor, &(project->track_type));

	flag_editor_set_flags(editor->flag_editor, &(project->flags));

	categories_editor_set_project(editor->categories_editor,project);


}

center_panel_t* center_panel_new()
{
	center_panel_t* editor = malloc(sizeof(center_panel_t));
	editor->project = NULL;
	editor->container = gtk_vbox_new(FALSE, 2);

	editor->excitement_editor = value_editor_new(VALUE_SIZE_BYTE, "Excitement:");
	gtk_box_pack_start(GTK_BOX(editor->container),
		editor->excitement_editor->container, FALSE, FALSE, 2);
	editor->intensity_editor = value_editor_new(VALUE_SIZE_BYTE, "Intensity:");
	gtk_box_pack_start(GTK_BOX(editor->container),
		editor->intensity_editor->container, FALSE, FALSE, 2);
	editor->nausea_editor = value_editor_new(VALUE_SIZE_BYTE, "Nausea:");
	gtk_box_pack_start(GTK_BOX(editor->container),
		editor->nausea_editor->container, FALSE, FALSE, 2);
	editor->max_height_editor = value_editor_new(VALUE_SIZE_BYTE, "Max height increase:");
	gtk_box_pack_start(GTK_BOX(editor->container),
		editor->max_height_editor->container, FALSE, FALSE, 2);

	// Edit car related information
	GtkWidget* cars_frame = gtk_frame_new("Cars");
	GtkWidget* cars_vbox = gtk_vbox_new(FALSE, 1);
	gtk_container_add(GTK_CONTAINER(cars_frame), cars_vbox);
	gtk_box_pack_start(GTK_BOX(editor->container), cars_frame, FALSE, FALSE, 2);

	editor->min_cars_editor = value_editor_new(VALUE_SIZE_BYTE, "Minimum cars per train:");
	editor->max_cars_editor = value_editor_new(VALUE_SIZE_BYTE, "Maximum cars per train:");
	gtk_box_pack_start(GTK_BOX(cars_vbox), editor->min_cars_editor->container,
		FALSE, FALSE, 2);
	gtk_box_pack_start(GTK_BOX(cars_vbox), editor->max_cars_editor->container,
		FALSE, FALSE, 2);
    editor->cars_per_flat_ride_editor = value_editor_new(VALUE_SIZE_BYTE, "Cars per flat ride:");
    gtk_box_pack_start(GTK_BOX(cars_vbox), editor->cars_per_flat_ride_editor->container,
        FALSE, FALSE, 2);
	editor->zero_cars_editor = value_editor_new(VALUE_SIZE_BYTE, "Number of zero cars:");
	gtk_box_pack_start(GTK_BOX(cars_vbox), editor->zero_cars_editor->container,
		FALSE, FALSE, 2);
	editor->car_icon_index_editor = value_editor_new(VALUE_SIZE_BYTE, "Car icon index:");
	gtk_box_pack_start(GTK_BOX(cars_vbox),
		editor->car_icon_index_editor->container, FALSE, FALSE, 2);

	editor->default_car_editor = car_type_editor_new("Default car",0);
	editor->front_car_editor = car_type_editor_new("First car",1);
	editor->second_car_editor = car_type_editor_new("Second car", 1);
	editor->third_car_editor = car_type_editor_new("Third car", 1);
	editor->rear_car_editor = car_type_editor_new("Rear car", 1);
	gtk_box_pack_start(GTK_BOX(cars_vbox), editor->default_car_editor->container,
		FALSE, FALSE, 2);
	gtk_box_pack_start(GTK_BOX(cars_vbox), editor->front_car_editor->container,
		FALSE, FALSE, 2);
	gtk_box_pack_start(GTK_BOX(cars_vbox), editor->second_car_editor->container,
		FALSE, FALSE, 2);
	gtk_box_pack_start(GTK_BOX(cars_vbox), editor->third_car_editor->container,
		FALSE, FALSE, 2);
	gtk_box_pack_start(GTK_BOX(cars_vbox), editor->rear_car_editor->container,
		FALSE, FALSE, 2);
	return editor;
}
void center_panel_set_project(center_panel_t* editor, project_t* project)
{
	int i;
	editor->project = project;
	value_editor_set_value(editor->excitement_editor, &(project->excitement));
	value_editor_set_value(editor->intensity_editor, &(project->intensity));
	value_editor_set_value(editor->nausea_editor, &(project->nausea));
	value_editor_set_value(editor->max_height_editor, &(project->max_height));
	value_editor_set_value(editor->min_cars_editor, &(project->minimum_cars));
	value_editor_set_value(editor->max_cars_editor, &(project->maximum_cars));
	value_editor_set_value(editor->zero_cars_editor, &(project->zero_cars));
	value_editor_set_value(editor->car_icon_index_editor,
		&(project->car_icon_index));
	car_type_editor_set_car_type(editor->default_car_editor,
		&(project->car_types[CAR_INDEX_DEFAULT]));
	car_type_editor_set_car_type(editor->front_car_editor,
		&(project->car_types[CAR_INDEX_FRONT]));
	car_type_editor_set_car_type(editor->second_car_editor,
		&(project->car_types[CAR_INDEX_SECOND]));
	car_type_editor_set_car_type(editor->third_car_editor,
		&(project->car_types[CAR_INDEX_THIRD]));
	car_type_editor_set_car_type(editor->rear_car_editor,
		&(project->car_types[CAR_INDEX_REAR]));
    value_editor_set_value(editor->cars_per_flat_ride_editor,
        &(project->cars_per_flat_ride));
}

right_panel_t* right_panel_new()
{
	int i;
    right_panel_t* editor = malloc(sizeof(right_panel_t));
    editor->project = NULL;
    editor->container = gtk_vbox_new(FALSE, 2);

    GtkWidget* cars = gtk_notebook_new();
    for (i = 0; i < NUM_CARS; i++) {
        char label_text[256];
        sprintf(label_text, "Car %d", i);
        GtkWidget* page_label = gtk_label_new(label_text);
        editor->car_editors[i] = car_editor_new();
        gtk_notebook_append_page(GTK_NOTEBOOK(cars),
            editor->car_editors[i]->container, page_label);
    }
    gtk_box_pack_start(GTK_BOX(editor->container), cars, FALSE, FALSE, 2);
    return editor;
}
void right_panel_set_project(right_panel_t* editor, project_t* project)
{
    int i;
    editor->project = project;
    for (i = 0; i < NUM_CARS; i++) {
        car_editor_set_project(editor->car_editors[i], project);
        car_editor_set_car(editor->car_editors[i], &(project->cars[i]));
    }
}

static gboolean main_window_delete_event(GtkWidget* widget,
    GdkEvent* event,
    gpointer data)
{
    return FALSE;
}
static void main_window_exit(GtkWidget* widget, gpointer data)
{
    gtk_main_quit();
}

static void main_window_edit_model(GtkWidget* widget, gpointer data)
{
    model_t* model = (model_t*)data;
    if (model != NULL) {
        model_dialog_t* dialog = model_dialog_new(model);
        model_dialog_run(dialog);
        model_dialog_free(dialog);
        gtk_menu_item_set_label(GTK_MENU_ITEM(widget), model->name);
    }
}

static void main_window_add_model_to_menu(main_window_t* main_window, model_t* model)
{
    GtkWidget* model_menu_item = gtk_menu_item_new_with_label(model->name);
    gtk_menu_shell_append(GTK_MENU_SHELL(main_window->model_menu),
        model_menu_item);
    g_signal_connect(model_menu_item, "activate",
        G_CALLBACK(main_window_edit_model), model);
    gtk_widget_show(model_menu_item);
}

static void main_window_add_model(GtkWidget* widget, gpointer data)
{
    main_window_t* main_window = (main_window_t*)data;
    if (main_window->project == NULL)
        return;
    char* filename = get_filename("Select file to open", GTK_FILE_CHOOSER_ACTION_OPEN);
    if (filename != NULL) {
        model_t* model = model_load_obj(filename);
        if (model != NULL) {
            project_add_model(main_window->project, model);
            model_dialog_t* dialog = model_dialog_new(model);
            model_dialog_run(dialog);
            model_dialog_free(dialog);
            main_window_add_model_to_menu(main_window, model);
        }
    }
}

static void main_window_populate_model_menu(main_window_t* main_window)
{
    if (main_window->project == NULL)
        return;

    for (int i = 0; i < main_window->project->num_models; i++) {
        // Add model to menu
        main_window_add_model_to_menu(main_window, main_window->project->models[i]);
    }
}

static void main_window_set_project(main_window_t* main_window, project_t* project)
{
    main_window->project = project;

	left_panel_set_project(main_window->left_panel, project);

	center_panel_set_project(main_window->center_panel, project);

	right_panel_set_project(main_window->right_panel, project);

    main_window_populate_model_menu(main_window);
}
static void main_window_new_project(GtkWidget* widget, gpointer data)
{
    main_window_t* main_window = (main_window_t*)data;
    main_window_set_project(main_window, project_new());
}
static void main_window_open_project(GtkWidget* widget, gpointer data)
{
    main_window_t* main_window = (main_window_t*)data;
    char* filename = get_filename("Select file to open", GTK_FILE_CHOOSER_ACTION_OPEN);
    if (filename != NULL) {
        project_t* project = project_load(filename);
        if (project != NULL)
            main_window_set_project(main_window, project);
        else
            show_error("Failed to load project file\n");
    }
}
static void main_window_save_project(GtkWidget* widget, gpointer data)
{
    main_window_t* main_window = (main_window_t*)data;
    if (main_window->project == NULL) {
        // TODO: show error
        return;
    }
    char* filename = get_filename("Enter filename", GTK_FILE_CHOOSER_ACTION_SAVE);
    if (filename != NULL)
        project_save(filename, main_window->project);
}
static void main_window_export_project(GtkWidget* widget, gpointer data)
{
    main_window_t* main_window = (main_window_t*)data;
    char* filename = get_filename("Enter filename", GTK_FILE_CHOOSER_ACTION_SAVE);
    if (filename != NULL) {
        object_t* object = project_export_dat(main_window->project);
        object_save_dat(object, filename, main_window->project->id);
        object_free(object);
    }
}

static void main_window_build_menus(main_window_t* main_window)
{
    // Set up the menus
    main_window->main_menu = gtk_menu_bar_new();
    GtkWidget* file_menu_item = gtk_menu_item_new_with_label("File");
    GtkWidget* model_menu_item = gtk_menu_item_new_with_label("Model");
    // GtkWidget* animation_menu_item=gtk_menu_item_new_with_label("Animation");
    gtk_menu_shell_append(GTK_MENU_SHELL(main_window->main_menu), file_menu_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(main_window->main_menu),
        model_menu_item);
    // gtk_menu_shell_append(GTK_MENU_SHELL(main_window->main_menu),animation_menu_item);

    GtkWidget* file_menu = gtk_menu_new();
    GtkWidget* new_menu_item = gtk_menu_item_new_with_label("New");
    GtkWidget* open_menu_item = gtk_menu_item_new_with_label("Open");
    GtkWidget* save_menu_item = gtk_menu_item_new_with_label("Save");
    GtkWidget* export_menu_item = gtk_menu_item_new_with_label("Export DAT");
    g_signal_connect(new_menu_item, "activate",
        G_CALLBACK(main_window_new_project), main_window);
    g_signal_connect(open_menu_item, "activate",
        G_CALLBACK(main_window_open_project), main_window);
    g_signal_connect(save_menu_item, "activate",
        G_CALLBACK(main_window_save_project), main_window);
    g_signal_connect(export_menu_item, "activate",
        G_CALLBACK(main_window_export_project), main_window);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), new_menu_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), open_menu_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), save_menu_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), export_menu_item);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(file_menu_item), file_menu);

    main_window->model_menu = gtk_menu_new();
    GtkWidget* add_model_menu_item = gtk_menu_item_new_with_label("Add Model");
    g_signal_connect(add_model_menu_item, "activate",
        G_CALLBACK(main_window_add_model), main_window);
    gtk_menu_shell_append(GTK_MENU_SHELL(main_window->model_menu),
        add_model_menu_item);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(model_menu_item),
        main_window->model_menu);

    /*
  main_window->animation_menu=gtk_menu_new();
  GtkWidget* add_animation_menu_item=gtk_menu_item_new_with_label("Add
  Animation");
  g_signal_connect(add_animation_menu_item,"activate",G_CALLBACK(main_window_add_animation),main_window);
  gtk_menu_shell_append(GTK_MENU_SHELL(main_window->animation_menu),add_animation_menu_item);
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(animation_menu_item),main_window->animation_menu);
  //g_signal_connect(addAnimationMenuItem,"activate",G_CALLBACK(AddNewAnimation),MainInterface);
  */

    gtk_box_pack_start(GTK_BOX(main_window->main_vbox), main_window->main_menu,
        FALSE, FALSE, 0);
}

main_window_t* main_window_new()
{
    main_window_t* main_window = malloc(sizeof(main_window_t));
    main_window->project = NULL;

    main_window->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    g_signal_connect(main_window->window, "delete-event",
        G_CALLBACK(main_window_delete_event), NULL);
    g_signal_connect(main_window->window, "destroy", G_CALLBACK(main_window_exit),
        NULL);

    main_window->main_vbox = gtk_vbox_new(FALSE, 5);
    gtk_container_add(GTK_CONTAINER(main_window->window), main_window->main_vbox);

    main_window_build_menus(main_window);

    main_window->main_hbox = gtk_hbox_new(FALSE, 2);
    gtk_box_pack_start(GTK_BOX(main_window->main_vbox), main_window->main_hbox,
        FALSE, FALSE, 2);

	// create the 3 columns
    main_window->left_vbox = gtk_vbox_new(FALSE, 2);
    gtk_box_pack_start(GTK_BOX(main_window->main_hbox), main_window->left_vbox, FALSE, FALSE, 2);
	main_window->center_vbox = gtk_vbox_new(FALSE, 2);
	gtk_box_pack_start(GTK_BOX(main_window->main_hbox), main_window->center_vbox, FALSE, FALSE, 2);
	main_window->right_vbox = gtk_vbox_new(FALSE, 2);
	gtk_box_pack_start(GTK_BOX(main_window->main_hbox), main_window->right_vbox, FALSE, FALSE, 2);

	main_window->left_panel = left_panel_new();
	gtk_box_pack_start(GTK_BOX(main_window->left_vbox), main_window->left_panel->container, FALSE, FALSE, 2);

	main_window->center_panel = center_panel_new();
	gtk_box_pack_start(GTK_BOX(main_window->center_vbox), main_window->center_panel->container, FALSE, FALSE, 2);

	main_window->right_panel = right_panel_new();
	gtk_box_pack_start(GTK_BOX(main_window->right_vbox), main_window->right_panel->container, FALSE, FALSE, 2);

    gtk_widget_show_all(main_window->window);

    return main_window;
}

void main_window_free(main_window_t* main_window)
{
    // gtk_widget_destroy(main_window->window);
    free(main_window);
}
