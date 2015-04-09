#include<stdlib.h>
#include<math.h>
#include<string.h>
#include "modeldialog.h"
#include "image.h"
#include "palette.h"
#include "renderer.h"

#define M_PI 3.1415926535
#define M_PI_2 (3.1415926535/2.0)
#define M_SQRT2 sqrt(2.0)
#define M_SQRT1_2 (1.0/sqrt(2.0))

/*
static void ScaleChanged(GtkSpinButton *spinbutton,gpointer user_data)
{
ModelDialog* modelDialog=(ModelDialog*)user_data;

float scale=(float)gtk_spin_button_get_value(spinbutton);
#define SETMAGNITUDE(X,Y) ((X)=(X)>0?(Y):(-Y))
SETMAGNITUDE(modelDialog->model->transform.Data[0],scale);
SETMAGNITUDE(modelDialog->model->transform.Data[5],scale);
SETMAGNITUDE(modelDialog->model->transform.Data[10],scale);
RenderPreview(modelDialog);
}
static void PivotXChanged(GtkSpinButton *spinbutton,gpointer user_data)
{
ModelDialog* modelDialog=(ModelDialog*)user_data;
float x=(float)gtk_spin_button_get_value(spinbutton);
modelDialog->model->transform.Data[3]=x;
RenderPreview(modelDialog);
}
static void PivotYChanged(GtkSpinButton *spinbutton,gpointer user_data)
{
ModelDialog* modelDialog=(ModelDialog*)user_data;
float y=(float)gtk_spin_button_get_value(spinbutton);
modelDialog->model->transform.Data[7]=y;
RenderPreview(modelDialog);
}
static void PivotZChanged(GtkSpinButton *spinbutton,gpointer user_data)
{
ModelDialog* modelDialog=(ModelDialog*)user_data;
float z=(float)gtk_spin_button_get_value(spinbutton);
modelDialog->model->transform.Data[11]=z;
RenderPreview(modelDialog);
}


static void UpdateName(GtkWidget* widget,gpointer user_data)
{
Model* model=(Model*)user_data;
const char* name=gtk_entry_get_text(GTK_ENTRY(widget));
model->Name=realloc(model->Name,strlen(name)+1);
strcpy(model->Name,name);
}
*/


void pixbuf_set_color(GdkPixbuf* pixbuf,uint8_t color_index)
{
int i,j;
int rowstride=gdk_pixbuf_get_rowstride(pixbuf);
guchar* pixels=gdk_pixbuf_get_pixels(pixbuf);

color_t color=palette_color_from_index(color_index);
    for(i=0;i<gdk_pixbuf_get_height(pixbuf);i++)
    {
        for(j=0;j<gdk_pixbuf_get_width(pixbuf);j++)
        {
        int pixelindex=j*3;
        pixels[pixelindex]=color.red;
        pixels[pixelindex+1]=color.green;
        pixels[pixelindex+2]=color.blue;
        }
    pixels+=rowstride;
    }
}


void model_viewer_render_preview(model_viewer_t* viewer)
{
renderer_clear_buffers();
renderer_render_model(viewer->grid,viewer->model_view);
renderer_render_model(viewer->model,viewer->model_view);
image_t* image=renderer_get_image();
image_viewer_set_image(viewer->image_viewer,image);
image_free(image);
}
static void model_viewer_reset(GtkWidget* button,gpointer user_data)
{
model_viewer_t* model_viewer=(model_viewer_t*)user_data;
model_viewer->model_view=MatrixIdentity();
    if(model_viewer->model!=NULL)model_viewer_render_preview(model_viewer);
}
static void model_viewer_rotate_left(GtkWidget* button,gpointer user_data)
{
model_viewer_t* model_viewer=(model_viewer_t*)user_data;

Matrix rotation=
    {{
     cos(M_PI/12.0),      0.0,      sin(M_PI/12.0), 0.0,
          0.0      ,      1.0,          0.0       , 0.0,
    -sin(M_PI/12.0),      0.0,      cos(M_PI/12.0), 0.0,
          0.0      ,      0.0,          0.0       , 1.0
    }};

model_viewer->model_view=MatrixMultiply(model_viewer->model_view,rotation);
    if(model_viewer->model!=NULL)model_viewer_render_preview(model_viewer);
}
static void model_viewer_rotate_right(GtkWidget* button,gpointer user_data)
{
model_viewer_t* model_viewer=(model_viewer_t*)user_data;

Matrix rotation=
    {{
    cos(M_PI/12.0),      0.0,      -sin(M_PI/12.0), 0.0,
          0.0     ,      1.0,          0.0        , 0.0,
    sin(M_PI/12.0),      0.0,       cos(M_PI/12.0), 0.0,
          0.0     ,      0.0,          0.0        , 1.0
    }};

model_viewer->model_view=MatrixMultiply(model_viewer->model_view,rotation);
    if(model_viewer->model!=NULL)model_viewer_render_preview(model_viewer);
}
static void model_viewer_rotate_up(GtkWidget* button,gpointer user_data)
{
model_viewer_t* model_viewer=(model_viewer_t*)user_data;

Matrix rotation=
    {{
       0.5*(cos(M_PI/12.0)+1), -sin(M_PI/12.0)/M_SQRT2,  0.5*(cos(M_PI/12.0)-1)  , 0.0,
       sin(M_PI/12.0)/M_SQRT2,      cos(M_PI/12.0)    ,    sin(M_PI/12.0)/M_SQRT2, 0.0,
       0.5*(cos(M_PI/12.0)-1), -sin(M_PI/12.0)/M_SQRT2,  0.5*(cos(M_PI/12.0)+1)  , 0.0,
                0.0          ,            0.0         ,           0.0            , 1.0
    }};

model_viewer->model_view=MatrixMultiply(rotation,model_viewer->model_view);
    if(model_viewer->model!=NULL)model_viewer_render_preview(model_viewer);
}
static void model_viewer_rotate_down(GtkWidget* button,gpointer user_data)
{
model_viewer_t* model_viewer=(model_viewer_t*)user_data;

Matrix rotation=
    {{
       0.5*(cos(M_PI/12.0)+1), sin(M_PI/12.0)/M_SQRT2,  0.5*(cos(M_PI/12.0)-1), 0.0,
      -sin(M_PI/12.0)/M_SQRT2,     cos(M_PI/12.0)    , -sin(M_PI/12.0)/M_SQRT2, 0.0,
       0.5*(cos(M_PI/12.0)-1), sin(M_PI/12.0)/M_SQRT2,  0.5*(cos(M_PI/12.0)+1), 0.0,
                0.0          ,           0.0         ,           0.0          , 1.0
    }};

model_viewer->model_view=MatrixMultiply(rotation,model_viewer->model_view);
    if(model_viewer->model!=NULL)model_viewer_render_preview(model_viewer);
}
static void model_viewer_translate_left(GtkWidget* button,gpointer user_data)
{
model_viewer_t* model_viewer=(model_viewer_t*)user_data;

Matrix translation=
    {{
    1.0, 0.0, 0.0, M_SQRT1_2,
    0.0, 1.0, 0.0,    0.0,
    0.0, 0.0, 1.0, -M_SQRT1_2,
    0.0, 0.0, 0.0,    1.0
    }};

model_viewer->model_view=MatrixMultiply(translation,model_viewer->model_view);
    if(model_viewer->model!=NULL)model_viewer_render_preview(model_viewer);
}
static void model_viewer_translate_right(GtkWidget* button,gpointer user_data)
{
model_viewer_t* model_viewer=(model_viewer_t*)user_data;

Matrix translation=
    {{
    1.0, 0.0, 0.0,-M_SQRT1_2,
    0.0, 1.0, 0.0,   0.0,
    0.0, 0.0, 1.0, M_SQRT1_2,
    0.0, 0.0, 0.0,   1.0
    }};

model_viewer->model_view=MatrixMultiply(translation,model_viewer->model_view);
    if(model_viewer->model!=NULL)model_viewer_render_preview(model_viewer);
}
static void model_viewer_translate_up(GtkWidget* button,gpointer user_data)
{
model_viewer_t* model_viewer=(model_viewer_t*)user_data;

Matrix translation=
    {{
    1.0, 0.0, 0.0,M_SQRT1_2,
    0.0, 1.0, 0.0,   0.0,
    0.0, 0.0, 1.0,M_SQRT1_2,
    0.0, 0.0, 0.0,   1.0
    }};

model_viewer->model_view=MatrixMultiply(translation,model_viewer->model_view);
    if(model_viewer->model!=NULL)model_viewer_render_preview(model_viewer);
}
static void model_viewer_translate_down(GtkWidget* button,gpointer user_data)
{
model_viewer_t* model_viewer=(model_viewer_t*)user_data;

Matrix translation=
    {{
    1.0, 0.0, 0.0, -M_SQRT1_2,
    0.0, 1.0, 0.0,    0.0,
    0.0, 0.0, 1.0, -M_SQRT1_2,
    0.0, 0.0, 0.0,    1.0
    }};

model_viewer->model_view=MatrixMultiply(translation,model_viewer->model_view);
    if(model_viewer->model!=NULL)model_viewer_render_preview(model_viewer);
}
static void model_viewer_zoom_in(GtkWidget* button,gpointer user_data)
{
model_viewer_t* model_viewer=(model_viewer_t*)user_data;

Matrix scale=
    {{
    1.1, 0.0, 0.0, 0.0,
    0.0, 1.1, 0.0, 0.0,
    0.0, 0.0, 1.1, 0.0,
    0.0, 0.0, 0.0, 1.0
    }};

model_viewer->model_view=MatrixMultiply(scale,model_viewer->model_view);
    if(model_viewer->model!=NULL)model_viewer_render_preview(model_viewer);
}
static void model_viewer_zoom_out(GtkWidget* button,gpointer user_data)
{
model_viewer_t* model_viewer=(model_viewer_t*)user_data;

Matrix scale=
    {{
    1.0/1.1,   0.0  ,   0.0  , 0.0,
      0.0  , 1.0/1.1,   0.0  , 0.0,
      0.0  ,   0.0  , 1.0/1.1, 0.0,
      0.0  ,   0.0  ,   0.0  , 1.0
    }};

model_viewer->model_view=MatrixMultiply(scale,model_viewer->model_view);
    if(model_viewer->model!=NULL)model_viewer_render_preview(model_viewer);
}

model_viewer_t* model_viewer_new()
{
model_viewer_t* model_viewer=malloc(sizeof(model_viewer_t));
model_viewer->model=NULL;
model_viewer->grid=model_new_grid();
model_viewer->model_view=MatrixIdentity();

model_viewer->container=gtk_vbox_new(FALSE,1);

model_viewer->image_viewer=image_viewer_new();
gtk_box_pack_start(GTK_BOX(model_viewer->container),model_viewer->image_viewer->container,TRUE,TRUE,2);

//Create buttons
model_viewer->reset=gtk_button_new_with_label("Reset");
model_viewer->rotate_left=gtk_button_new();
model_viewer->rotate_right=gtk_button_new();
model_viewer->rotate_up=gtk_button_new();
model_viewer->rotate_down=gtk_button_new();
model_viewer->translate_left=gtk_button_new();
model_viewer->translate_right=gtk_button_new();
model_viewer->translate_up=gtk_button_new();
model_viewer->translate_down=gtk_button_new();
model_viewer->zoom_in=gtk_button_new();
model_viewer->zoom_out=gtk_button_new();
//Add images to buttons
GtkWidget* image=gtk_image_new_from_stock(GTK_STOCK_GO_BACK,GTK_ICON_SIZE_BUTTON);
gtk_container_add(GTK_CONTAINER(model_viewer->rotate_left),image);
image=gtk_image_new_from_stock(GTK_STOCK_GO_FORWARD,GTK_ICON_SIZE_BUTTON);
gtk_container_add(GTK_CONTAINER(model_viewer->rotate_right),image);
image=gtk_image_new_from_stock(GTK_STOCK_GO_UP,GTK_ICON_SIZE_BUTTON);
gtk_container_add(GTK_CONTAINER(model_viewer->rotate_up),image);
image=gtk_image_new_from_stock(GTK_STOCK_GO_DOWN,GTK_ICON_SIZE_BUTTON);
gtk_container_add(GTK_CONTAINER(model_viewer->rotate_down),image);
image=gtk_image_new_from_stock(GTK_STOCK_GO_BACK,GTK_ICON_SIZE_BUTTON);
gtk_container_add(GTK_CONTAINER(model_viewer->translate_left),image);
image=gtk_image_new_from_stock(GTK_STOCK_GO_FORWARD,GTK_ICON_SIZE_BUTTON);
gtk_container_add(GTK_CONTAINER(model_viewer->translate_right),image);
image=gtk_image_new_from_stock(GTK_STOCK_GO_UP,GTK_ICON_SIZE_BUTTON);
gtk_container_add(GTK_CONTAINER(model_viewer->translate_up),image);
image=gtk_image_new_from_stock(GTK_STOCK_GO_DOWN,GTK_ICON_SIZE_BUTTON);
gtk_container_add(GTK_CONTAINER(model_viewer->translate_down),image);
image=gtk_image_new_from_stock(GTK_STOCK_ZOOM_IN,GTK_ICON_SIZE_BUTTON);
gtk_container_add(GTK_CONTAINER(model_viewer->zoom_in),image);
image=gtk_image_new_from_stock(GTK_STOCK_ZOOM_OUT,GTK_ICON_SIZE_BUTTON);
gtk_container_add(GTK_CONTAINER(model_viewer->zoom_out),image);
//Attach buttons to table
model_viewer->button_table=gtk_table_new(3,6,TRUE);
gtk_table_attach_defaults(GTK_TABLE(model_viewer->button_table),model_viewer->reset,2,4,2,3);
gtk_table_attach_defaults(GTK_TABLE(model_viewer->button_table),model_viewer->rotate_left,0,1,1,2);
gtk_table_attach_defaults(GTK_TABLE(model_viewer->button_table),model_viewer->rotate_right,2,3,1,2);
gtk_table_attach_defaults(GTK_TABLE(model_viewer->button_table),model_viewer->rotate_up,1,2,0,1);
gtk_table_attach_defaults(GTK_TABLE(model_viewer->button_table),model_viewer->rotate_down,1,2,2,3);
gtk_table_attach_defaults(GTK_TABLE(model_viewer->button_table),model_viewer->translate_left,3,4,1,2);
gtk_table_attach_defaults(GTK_TABLE(model_viewer->button_table),model_viewer->translate_right,5,6,1,2);
gtk_table_attach_defaults(GTK_TABLE(model_viewer->button_table),model_viewer->translate_up,4,5,0,1);
gtk_table_attach_defaults(GTK_TABLE(model_viewer->button_table),model_viewer->translate_down,4,5,2,3);
gtk_table_attach_defaults(GTK_TABLE(model_viewer->button_table),model_viewer->zoom_in,2,3,0,1);
gtk_table_attach_defaults(GTK_TABLE(model_viewer->button_table),model_viewer->zoom_out,3,4,0,1);
gtk_box_pack_start(GTK_BOX(model_viewer->container),model_viewer->button_table,TRUE,TRUE,2);
//Connect event handlers
g_signal_connect(model_viewer->reset,"clicked",G_CALLBACK(model_viewer_reset),model_viewer);
g_signal_connect(model_viewer->rotate_left,"clicked",G_CALLBACK(model_viewer_rotate_left),model_viewer);
g_signal_connect(model_viewer->rotate_right,"clicked",G_CALLBACK(model_viewer_rotate_right),model_viewer);
g_signal_connect(model_viewer->rotate_up,"clicked",G_CALLBACK(model_viewer_rotate_up),model_viewer);
g_signal_connect(model_viewer->rotate_down,"clicked",G_CALLBACK(model_viewer_rotate_down),model_viewer);
g_signal_connect(model_viewer->translate_left,"clicked",G_CALLBACK(model_viewer_translate_left),model_viewer);
g_signal_connect(model_viewer->translate_right,"clicked",G_CALLBACK(model_viewer_translate_right),model_viewer);
g_signal_connect(model_viewer->translate_up,"clicked",G_CALLBACK(model_viewer_translate_up),model_viewer);
g_signal_connect(model_viewer->translate_down,"clicked",G_CALLBACK(model_viewer_translate_down),model_viewer);
g_signal_connect(model_viewer->zoom_in,"clicked",G_CALLBACK(model_viewer_zoom_in),model_viewer);
g_signal_connect(model_viewer->zoom_out,"clicked",G_CALLBACK(model_viewer_zoom_out),model_viewer);

return model_viewer;
}
void model_viewer_set_model(model_viewer_t* viewer,model_t* model)
{
viewer->model=model;
model_viewer_render_preview(viewer);
}
void model_viewer_free(model_viewer_t* viewer)
{
image_viewer_free(viewer->image_viewer);
gtk_widget_destroy(viewer->reset);
gtk_widget_destroy(viewer->rotate_left);
gtk_widget_destroy(viewer->rotate_right);
gtk_widget_destroy(viewer->rotate_up);
gtk_widget_destroy(viewer->rotate_down);
gtk_widget_destroy(viewer->translate_left);
gtk_widget_destroy(viewer->translate_right);
gtk_widget_destroy(viewer->translate_up);
gtk_widget_destroy(viewer->translate_down);
gtk_widget_destroy(viewer->zoom_in);
gtk_widget_destroy(viewer->zoom_out);
gtk_widget_destroy(viewer->button_table);
gtk_widget_destroy(viewer->container);
model_free(viewer->grid);
free(viewer);
}


static void color_select_tool_clicked(GtkWidget* widget,gpointer user_data)
{
color_select_tool_t* tool=(color_select_tool_t*)user_data;
        if(tool->color!=NULL) *(tool->color)=tool->value;
}
color_select_tool_t* color_select_tool_new(uint8_t color)
{
color_select_tool_t* tool=malloc(sizeof(color_select_tool_t));
tool->color=NULL;
tool->value=color;
tool->pixbuf=gdk_pixbuf_new(GDK_COLORSPACE_RGB,FALSE,8,20,20);
pixbuf_set_color(tool->pixbuf,palette_remap_section_index(color,6));
tool->image=gtk_image_new_from_pixbuf(tool->pixbuf);
tool->tool_item=gtk_tool_button_new(tool->image,"");
g_signal_connect(tool->tool_item,"clicked",G_CALLBACK(color_select_tool_clicked),tool);
return tool;
}
void color_select_tool_set_color(color_select_tool_t* tool,uint8_t* color)
{
tool->color=color;
}
void color_select_tool_free(color_select_tool_t* tool)
{
gtk_widget_destroy(tool->image);
g_object_unref(G_OBJECT(tool->pixbuf));
gtk_widget_destroy(GTK_WIDGET(tool->tool_item));
free(tool);
}
color_selector_t* color_selector_new()
{
color_selector_t* selector=malloc(sizeof(color_selector_t));
selector->color=NULL;
selector->container=gtk_tool_palette_new();
selector->remap_tools=gtk_tool_item_group_new("Remap colors");
selector->peep_tools=gtk_tool_item_group_new("Peep colors");
selector->color_tools=gtk_tool_item_group_new("Other colors");
gtk_container_add(GTK_CONTAINER(selector->container),selector->remap_tools);
gtk_container_add(GTK_CONTAINER(selector->container),selector->peep_tools);
gtk_container_add(GTK_CONTAINER(selector->container),selector->color_tools);

selector->tools=malloc(39*sizeof(color_select_tool_t*));

selector->tools[0]=color_select_tool_new(COLOR_REMAP_1);
selector->tools[1]=color_select_tool_new(COLOR_REMAP_2);
selector->tools[2]=color_select_tool_new(COLOR_REMAP_3);
gtk_tool_item_group_insert(GTK_TOOL_ITEM_GROUP(selector->remap_tools),selector->tools[0]->tool_item,0);
gtk_tool_item_group_insert(GTK_TOOL_ITEM_GROUP(selector->remap_tools),selector->tools[1]->tool_item,1);
gtk_tool_item_group_insert(GTK_TOOL_ITEM_GROUP(selector->remap_tools),selector->tools[2]->tool_item,2);

selector->tools[3]=color_select_tool_new(COLOR_PEEP_LEG);
selector->tools[4]=color_select_tool_new(COLOR_PEEP_SKIN);
selector->tools[5]=color_select_tool_new(COLOR_PEEP_REMAP_1);
selector->tools[6]=color_select_tool_new(COLOR_PEEP_REMAP_2);
gtk_tool_item_group_insert(GTK_TOOL_ITEM_GROUP(selector->peep_tools),selector->tools[3]->tool_item,0);
gtk_tool_item_group_insert(GTK_TOOL_ITEM_GROUP(selector->peep_tools),selector->tools[4]->tool_item,1);
gtk_tool_item_group_insert(GTK_TOOL_ITEM_GROUP(selector->peep_tools),selector->tools[5]->tool_item,3);
gtk_tool_item_group_insert(GTK_TOOL_ITEM_GROUP(selector->peep_tools),selector->tools[6]->tool_item,4);
int i;
    for(i=0;i<32;i++)
    {
    selector->tools[7+i]=color_select_tool_new(i);
    gtk_tool_item_group_insert(GTK_TOOL_ITEM_GROUP(selector->color_tools),selector->tools[7+i]->tool_item,i);
    }
return selector;
}
void color_selector_set_color(color_selector_t* selector,uint8_t* color)
{
int i;
selector->color=color;
    for(i=0;i<39;i++)
    {
    color_select_tool_set_color(selector->tools[i],color);
    }
}
void color_selector_free(color_selector_t* selector)
{
int i;
    for(i=0;i<39;i++)
    {
    color_select_tool_free(selector->tools[i]);
    }
gtk_widget_destroy(selector->remap_tools);
gtk_widget_destroy(selector->peep_tools);
gtk_widget_destroy(selector->color_tools);
gtk_widget_destroy(selector->container);
free(selector->tools);
free(selector);
}

static void matrix_transform_button_clicked(GtkWidget* widget,gpointer user_data)
{
matrix_transform_button_t* button=(matrix_transform_button_t*)user_data;
    if(button->matrix==NULL)return;
*(button->matrix)=MatrixMultiply(button->transform,*(button->matrix));
}
matrix_transform_button_t* matrix_transform_button_new(const char* label,Matrix matrix)
{
matrix_transform_button_t* button=malloc(sizeof(matrix_transform_button_t));
button->matrix=NULL;
button->transform=matrix;
button->container=gtk_button_new_with_label(label);
g_signal_connect(button->container,"clicked",G_CALLBACK(matrix_transform_button_clicked),button);
return button;
}
void matrix_transform_button_set_matrix(matrix_transform_button_t* button,Matrix* matrix)
{
button->matrix=matrix;
}
void matrix_transform_button_free(matrix_transform_button_t* button)
{
gtk_widget_destroy(button->container);
free(button);
}

static void model_dialog_update_model_viewer(GtkWidget *widget, gpointer user_data)
{
model_dialog_t* dialog=(model_dialog_t*)user_data;
model_viewer_render_preview(dialog->model_viewer);
}
static void model_dialog_preview_pressed(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
model_dialog_t* dialog=(model_dialog_t*)user_data;
dialog->painting=1;
}
static void model_dialog_preview_released(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
model_dialog_t* dialog=(model_dialog_t*)user_data;
dialog->painting=0;
}

static void model_dialog_paint_model(GtkWidget *widget, GdkEventMotion *event, gpointer user_data)
{
model_dialog_t* dialog=(model_dialog_t*)user_data;
    if(!dialog->painting)return;
Vector coords;
coords.X=event->x;
coords.Y=event->y;
face_t* face=renderer_get_face_by_point(dialog->model,dialog->model_viewer->model_view,coords);
    if(face!=NULL&&face->color!=dialog->color)
    {
    face->color=dialog->color;
    model_viewer_render_preview(dialog->model_viewer);
    }
}

static void model_dialog_paint_all(GtkWidget *widget,gpointer user_data)
{
model_dialog_t* dialog=(model_dialog_t*)user_data;
int i;
    for(i=0;i<dialog->model->num_faces;i++)dialog->model->faces[i].color=dialog->color;
model_viewer_render_preview(dialog->model_viewer);
}

static void model_dialog_is_rider_changed(GtkWidget* widget, gpointer data)
{
model_dialog_t* dialog=(model_dialog_t*)data;
    if(dialog->model==NULL)return;
dialog->model->is_rider=gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(dialog->is_rider));
}

model_dialog_t* model_dialog_new(model_t* model)
{
model_dialog_t* dialog=malloc(sizeof(model_dialog_t));
dialog->model=model;
dialog->painting=0;
dialog->color=0;


dialog->dialog=gtk_dialog_new_with_buttons("Model Settings",NULL,0,"OK",GTK_RESPONSE_OK,NULL);
GtkWidget* content_area=gtk_dialog_get_content_area(GTK_DIALOG(dialog->dialog));

GtkWidget* name_hbox=gtk_hbox_new(FALSE,1);
//Create name editor
dialog->name_editor=string_editor_new("Name:");
string_editor_set_string(dialog->name_editor,&(model->name));
gtk_box_pack_start(GTK_BOX(name_hbox),dialog->name_editor->container,TRUE,TRUE,2);

//Create rider checkbox
dialog->is_rider=gtk_check_button_new_with_label("Rider");
gtk_box_pack_start(GTK_BOX(name_hbox),dialog->is_rider,FALSE,FALSE,2);
g_signal_connect(dialog->is_rider,"toggled",G_CALLBACK(model_dialog_is_rider_changed),dialog);

gtk_box_pack_start(GTK_BOX(content_area),name_hbox,FALSE,FALSE,2);

//Create transform buttons
GtkWidget* top_hbox=gtk_hbox_new(FALSE,1);
gtk_box_pack_start(GTK_BOX(content_area),top_hbox,FALSE,FALSE,2);
Matrix matrix=MatrixIdentity();
matrix.Data[0]=-1;
dialog->flip_x=matrix_transform_button_new("Flip X",matrix);
matrix=MatrixIdentity();
matrix.Data[5]=-1;
dialog->flip_y=matrix_transform_button_new("Flip Y",matrix);
matrix=MatrixIdentity();
matrix.Data[10]=-1;
dialog->flip_z=matrix_transform_button_new("Flip Z",matrix);
dialog->rotate_x=matrix_transform_button_new("Rotate X",MatrixFromEulerAngles(VectorFromComponents(M_PI_2,0,0)));
dialog->rotate_y=matrix_transform_button_new("Rotate Y",MatrixFromEulerAngles(VectorFromComponents(0,M_PI_2,0)));
dialog->rotate_z=matrix_transform_button_new("Rotate Z",MatrixFromEulerAngles(VectorFromComponents(0,0,M_PI_2)));
matrix_transform_button_set_matrix(dialog->flip_x,&(model->transform));
matrix_transform_button_set_matrix(dialog->flip_y,&(model->transform));
matrix_transform_button_set_matrix(dialog->flip_z,&(model->transform));
matrix_transform_button_set_matrix(dialog->rotate_x,&(model->transform));
matrix_transform_button_set_matrix(dialog->rotate_y,&(model->transform));
matrix_transform_button_set_matrix(dialog->rotate_z,&(model->transform));
g_signal_connect(dialog->flip_x->container,"clicked",G_CALLBACK(model_dialog_update_model_viewer),dialog);
g_signal_connect(dialog->flip_y->container,"clicked",G_CALLBACK(model_dialog_update_model_viewer),dialog);
g_signal_connect(dialog->flip_z->container,"clicked",G_CALLBACK(model_dialog_update_model_viewer),dialog);
g_signal_connect(dialog->rotate_x->container,"clicked",G_CALLBACK(model_dialog_update_model_viewer),dialog);
g_signal_connect(dialog->rotate_y->container,"clicked",G_CALLBACK(model_dialog_update_model_viewer),dialog);
g_signal_connect(dialog->rotate_z->container,"clicked",G_CALLBACK(model_dialog_update_model_viewer),dialog);
gtk_box_pack_start(GTK_BOX(top_hbox),dialog->flip_x->container,FALSE,FALSE,2);
gtk_box_pack_start(GTK_BOX(top_hbox),dialog->flip_y->container,FALSE,FALSE,2);
gtk_box_pack_start(GTK_BOX(top_hbox),dialog->flip_z->container,FALSE,FALSE,2);
gtk_box_pack_start(GTK_BOX(top_hbox),dialog->rotate_x->container,FALSE,FALSE,2);
gtk_box_pack_start(GTK_BOX(top_hbox),dialog->rotate_y->container,FALSE,FALSE,2);
gtk_box_pack_start(GTK_BOX(top_hbox),dialog->rotate_z->container,FALSE,FALSE,2);

GtkWidget* hbox=gtk_hbox_new(FALSE,1);
gtk_box_pack_start(GTK_BOX(content_area),hbox,FALSE,FALSE,2);

dialog->model_viewer=model_viewer_new();
model_viewer_set_model(dialog->model_viewer,model);
gtk_box_pack_start(GTK_BOX(hbox),dialog->model_viewer->container,FALSE,FALSE,2);

GtkWidget* paint_vbox=gtk_vbox_new(FALSE,2);

dialog->color_selector=color_selector_new();
color_selector_set_color(dialog->color_selector,&(dialog->color));
gtk_box_pack_start(GTK_BOX(paint_vbox),dialog->color_selector->container,TRUE,TRUE,2);

dialog->paint_all=gtk_button_new_with_label("Paint all");
g_signal_connect(dialog->paint_all,"clicked",G_CALLBACK(model_dialog_paint_all),dialog);
gtk_box_pack_start(GTK_BOX(paint_vbox),dialog->paint_all,FALSE,FALSE,2);

gtk_box_pack_start(GTK_BOX(hbox),paint_vbox,FALSE,FALSE,2);

g_signal_connect(dialog->model_viewer->image_viewer->container,"motion_notify_event",G_CALLBACK(model_dialog_paint_model),dialog);
g_signal_connect(dialog->model_viewer->image_viewer->container,"button_press_event",G_CALLBACK(model_dialog_preview_pressed),dialog);
g_signal_connect(dialog->model_viewer->image_viewer->container,"button_release_event",G_CALLBACK(model_dialog_preview_released),dialog);

gtk_widget_show_all(content_area);
return dialog;
/*
//Name
GtkWidget* nameBox=gtk_hbox_new(FALSE,1);
GtkWidget* nameLabel=gtk_label_new("Name:");
GtkWidget* name=gtk_entry_new();

g_signal_connect(name,"changed",G_CALLBACK(UpdateName),model);
gtk_entry_set_text(GTK_ENTRY(name),model->Name);
gtk_box_pack_start(GTK_BOX(nameBox),nameLabel,FALSE,FALSE,2);
gtk_box_pack_start(GTK_BOX(nameBox),name,TRUE,TRUE,2);
gtk_box_pack_start(GTK_BOX(contentArea),nameBox,TRUE,TRUE,2);

//Scale
GtkWidget* scaleBox=gtk_hbox_new(FALSE,1);
GtkWidget* scaleLabel=gtk_label_new("Scale");
GtkWidget* scaleSpin=gtk_spin_button_new_with_range(0.1,10.0,0.1);
gtk_spin_button_set_value(GTK_SPIN_BUTTON(scaleSpin),1.0);
g_signal_connect(scaleSpin,"value-changed",G_CALLBACK(ScaleChanged),&modelDialog);
gtk_box_pack_start(GTK_BOX(scaleBox),scaleLabel,TRUE,TRUE,5);
gtk_box_pack_start(GTK_BOX(scaleBox),scaleSpin,TRUE,TRUE,5);
gtk_box_pack_start(GTK_BOX(contentArea),scaleBox,TRUE,TRUE,2);

//Pivot
GtkWidget* pivotLabel=gtk_label_new("Pivot:");
GtkWidget* xLabel=gtk_label_new("X");
GtkWidget* yLabel=gtk_label_new("Y");
GtkWidget* zLabel=gtk_label_new("Z");
GtkWidget* xSpin=gtk_spin_button_new_with_range(-10.0,10.0,0.1);
GtkWidget* ySpin=gtk_spin_button_new_with_range(-10.0,10.0,0.1);
GtkWidget* zSpin=gtk_spin_button_new_with_range(-10.0,10.0,0.1);
gtk_spin_button_set_value(GTK_SPIN_BUTTON(xSpin),0.0);
gtk_spin_button_set_value(GTK_SPIN_BUTTON(ySpin),0.0);
gtk_spin_button_set_value(GTK_SPIN_BUTTON(zSpin),0.0);
g_signal_connect(xSpin,"value-changed",G_CALLBACK(PivotXChanged),&modelDialog);
g_signal_connect(ySpin,"value-changed",G_CALLBACK(PivotYChanged),&modelDialog);
g_signal_connect(zSpin,"value-changed",G_CALLBACK(PivotZChanged),&modelDialog);
gtk_box_pack_start(GTK_BOX(scaleBox),pivotLabel,FALSE,FALSE,5);
gtk_box_pack_start(GTK_BOX(scaleBox),xLabel,FALSE,FALSE,2);
gtk_box_pack_start(GTK_BOX(scaleBox),xSpin,FALSE,FALSE,2);
gtk_box_pack_start(GTK_BOX(scaleBox),yLabel,FALSE,FALSE,2);
gtk_box_pack_start(GTK_BOX(scaleBox),ySpin,FALSE,FALSE,2);
gtk_box_pack_start(GTK_BOX(scaleBox),zLabel,FALSE,FALSE,2);
gtk_box_pack_start(GTK_BOX(scaleBox),zSpin,FALSE,FALSE,2);
GtkWidget* previewHBox=gtk_hbox_new(FALSE,2);
GtkWidget* previewVBox=gtk_vbox_new(FALSE,2);

gtk_box_pack_start(GTK_BOX(previewVBox),viewTable,FALSE,FALSE,2);

gtk_box_pack_start(GTK_BOX(previewHBox),previewVBox,TRUE,TRUE,2);
gtk_box_pack_start(GTK_BOX(contentArea),previewHBox,TRUE,TRUE,2);
RenderPreview(&modelDialog);

gtk_widget_show_all(contentArea);
*/
}
void model_dialog_run(model_dialog_t* dialog)
{
gtk_dialog_run(GTK_DIALOG(dialog->dialog));
}
void model_dialog_free(model_dialog_t* dialog)
{
matrix_transform_button_free(dialog->flip_x);
matrix_transform_button_free(dialog->flip_y);
matrix_transform_button_free(dialog->flip_z);
matrix_transform_button_free(dialog->rotate_x);
matrix_transform_button_free(dialog->rotate_y);
matrix_transform_button_free(dialog->rotate_z);
model_viewer_free(dialog->model_viewer);
color_selector_free(dialog->color_selector);
gtk_widget_destroy(dialog->dialog);
free(dialog);
}

