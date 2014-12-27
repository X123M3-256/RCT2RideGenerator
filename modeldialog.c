#include<math.h>
#include "modeldialog.h"
#include "image.h"
#include "renderer.h"
#include "types.h"

#define GRID_TILES 20
#define GRID_SIZE (3.3*(GRID_TILES-1))
#define HALF_GRID_SIZE (GRID_SIZE/2.0)

Model* GetGridModel()
{
static Vector vertices[4*GRID_TILES];
static Line lines[2*GRID_TILES];
static Model previewGrid;
static int initialised=0;
    if(!initialised)
    {
    previewGrid.transform=MatrixIdentity();
    previewGrid.NumFaces=0;
    previewGrid.NumNormals=0;
    previewGrid.NumVertices=4*GRID_TILES;
    previewGrid.NumLines=2*GRID_TILES;
    previewGrid.Vertices=vertices;
    previewGrid.Lines=lines;

    int i;
    Vector* top=vertices;
    Vector* left=top+GRID_TILES;
    Vector* bottom=left+GRID_TILES;
    Vector* right=bottom+GRID_TILES;
        for(i=0;i<GRID_TILES;i++)
        {
        top[i].X=3.3*i-HALF_GRID_SIZE;
        top[i].Y=0.0;
        top[i].Z=HALF_GRID_SIZE;
        bottom[i]=top[i];
        bottom[i].Z-=GRID_SIZE;

        left[i].X=-HALF_GRID_SIZE;
        left[i].Y=0.0;
        left[i].Z=3.3*i-HALF_GRID_SIZE;
        right[i]=left[i];
        right[i].X+=GRID_SIZE;
        lines[i].Color=134;
        lines[i].Vertices[0]=i;
        lines[i].Vertices[1]=i+2*GRID_TILES;
        lines[i+GRID_TILES].Color=134;
        lines[i+GRID_TILES].Vertices[0]=i+GRID_TILES;
        lines[i+GRID_TILES].Vertices[1]=i+3*GRID_TILES;
        }
    }
return &previewGrid;
}

void RenderPreview(ModelDialog* dialog)
{
ClearBuffers();
RenderModel(dialog->model,dialog->modelView);
RenderModel(GetGridModel(),dialog->modelView);
Image image=ImageFromFrameBuffer();
ShowImageInPixbuf(dialog->pixbuf,&image);
gtk_image_set_from_pixbuf(GTK_IMAGE(dialog->preview),dialog->pixbuf);
//FreeImage(image);
}

static void FlipX(GtkCheckButton* checkbox,gpointer user_data)
{
ModelDialog* modelDialog=(ModelDialog*)user_data;
modelDialog->model->transform.Data[0]*=-1;
RenderPreview(modelDialog);
}
static void FlipY(GtkCheckButton* checkbox,gpointer user_data)
{
ModelDialog* modelDialog=(ModelDialog*)user_data;
modelDialog->model->transform.Data[5]*=-1;
RenderPreview(modelDialog);
}
static void FlipZ(GtkCheckButton* checkbox,gpointer user_data)
{
ModelDialog* modelDialog=(ModelDialog*)user_data;
modelDialog->model->transform.Data[10]*=-1;
RenderPreview(modelDialog);
}

static void ResetView(GtkButton* button,gpointer user_data)
{
ModelDialog* dialog=(ModelDialog*)user_data;
dialog->modelView=MatrixIdentity();
RenderPreview(dialog);
}
static void RotateViewLeft(GtkButton* button,gpointer user_data)
{
ModelDialog* dialog=(ModelDialog*)user_data;

const Matrix rotation=
    {
     cos(M_PI/12.0),      0.0,      sin(M_PI/12.0), 0.0,
          0.0      ,      1.0,          0.0       , 0.0,
    -sin(M_PI/12.0),      0.0,      cos(M_PI/12.0), 0.0,
          0.0      ,      0.0,          0.0       , 1.0
    };

dialog->modelView=MatrixMultiply(dialog->modelView,rotation);
RenderPreview(dialog);
}
static void RotateViewRight(GtkButton* button,gpointer user_data)
{
ModelDialog* dialog=(ModelDialog*)user_data;

const Matrix rotation=
    {
    cos(M_PI/12.0),      0.0,      -sin(M_PI/12.0), 0.0,
          0.0     ,      1.0,          0.0        , 0.0,
    sin(M_PI/12.0),      0.0,       cos(M_PI/12.0), 0.0,
          0.0     ,      0.0,          0.0        , 1.0
    };

dialog->modelView=MatrixMultiply(dialog->modelView,rotation);
RenderPreview(dialog);
}
static void RotateViewUp(GtkButton* button,gpointer user_data)
{
ModelDialog* dialog=(ModelDialog*)user_data;

const Matrix rotation=
    {
       0.5*(cos(M_PI/12.0)+1), -sin(M_PI/12.0)/M_SQRT2,  0.5*(cos(M_PI/12.0)-1)  , 0.0,
       sin(M_PI/12.0)/M_SQRT2,      cos(M_PI/12.0)    ,    sin(M_PI/12.0)/M_SQRT2, 0.0,
       0.5*(cos(M_PI/12.0)-1), -sin(M_PI/12.0)/M_SQRT2,  0.5*(cos(M_PI/12.0)+1)  , 0.0,
                0.0          ,            0.0         ,           0.0            , 1.0
    };

dialog->modelView=MatrixMultiply(rotation,dialog->modelView);
RenderPreview(dialog);
}
static void RotateViewDown(GtkButton* button,gpointer user_data)
{
ModelDialog* dialog=(ModelDialog*)user_data;

const Matrix rotation=
    {
       0.5*(cos(M_PI/12.0)+1), sin(M_PI/12.0)/M_SQRT2,  0.5*(cos(M_PI/12.0)-1), 0.0,
      -sin(M_PI/12.0)/M_SQRT2,     cos(M_PI/12.0)    , -sin(M_PI/12.0)/M_SQRT2, 0.0,
       0.5*(cos(M_PI/12.0)-1), sin(M_PI/12.0)/M_SQRT2,  0.5*(cos(M_PI/12.0)+1), 0.0,
                0.0          ,           0.0         ,           0.0          , 1.0
    };

dialog->modelView=MatrixMultiply(rotation,dialog->modelView);
RenderPreview(dialog);
}
static void ZoomViewIn(GtkButton* button,gpointer user_data)
{
ModelDialog* dialog=(ModelDialog*)user_data;

const Matrix rotation=
    {
    1.1, 0.0, 0.0, 0.0,
    0.0, 1.1, 0.0, 0.0,
    0.0, 0.0, 1.1, 0.0,
    0.0, 0.0, 0.0, 1.0
    };
dialog->modelView=MatrixMultiply(rotation,dialog->modelView);
RenderPreview(dialog);
}
static void ZoomViewOut(GtkButton* button,gpointer user_data)
{
ModelDialog* dialog=(ModelDialog*)user_data;

const Matrix rotation=
    {
    1.0/1.1,   0.0  ,   0.0  , 0.0,
      0.0  , 1.0/1.1,   0.0  , 0.0,
      0.0  ,   0.0  , 1.0/1.1, 0.0,
      0.0  ,   0.0  ,   0.0  , 1.0
    };
dialog->modelView=MatrixMultiply(rotation,dialog->modelView);
RenderPreview(dialog);
}

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

static void PreviewPressed(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
ModelDialog* dialog=(ModelDialog*)user_data;
dialog->painting=1;
}
static void PreviewReleased(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
ModelDialog* dialog=(ModelDialog*)user_data;
dialog->painting=0;
}
static void PaintModel(GtkWidget *widget, GdkEventMotion *event, gpointer user_data)
{
ModelDialog* dialog=(ModelDialog*)user_data;
    if(!dialog->painting)return;
Vector coords;
coords.X=event->x;
coords.Y=event->y;
Face* face=GetFaceEnclosingPoint(dialog->model,dialog->modelView,coords);
    if(face!=NULL)
    {
    int nextFlags;
        switch(dialog->paintState)
        {
        case RECOLORABLE_1:
        nextFlags=RECOLOR_GREEN;
        break;
        case RECOLORABLE_2:
        nextFlags=RECOLOR_MAGENTA;
        break;
        case RECOLORABLE_3:
        nextFlags=RECOLOR_YELLOW;
        break;
        case NON_RECOLORABLE:
        nextFlags=0;
        break;
        }
        if(face->Flags!=nextFlags)
        {
        face->Flags=nextFlags;
        RenderPreview(dialog);
        }
    }

}

static void Recolor1Clicked(GtkButton* button,gpointer user_data)
{
ModelDialog* dialog=(ModelDialog*)user_data;
dialog->paintState=RECOLORABLE_1;
}
static void Recolor2Clicked(GtkButton* button,gpointer user_data)
{
ModelDialog* dialog=(ModelDialog*)user_data;
dialog->paintState=RECOLORABLE_2;
}
static void Recolor3Clicked(GtkButton* button,gpointer user_data)
{
ModelDialog* dialog=(ModelDialog*)user_data;
dialog->paintState=RECOLORABLE_3;
}
static void NonColorClicked(GtkButton* button,gpointer user_data)
{
ModelDialog* dialog=(ModelDialog*)user_data;
dialog->paintState=NON_RECOLORABLE;
}

static void UpdateName(GtkWidget* widget,gpointer user_data)
{
Model* model=(Model*)user_data;
const char* name=gtk_entry_get_text(GTK_ENTRY(widget));
model->Name=realloc(model->Name,strlen(name)+1);
strcpy(model->Name,name);
}

void CreateModelDialog(Model* model)
{
ModelDialog modelDialog;
modelDialog.model=model;
modelDialog.modelView=MatrixIdentity();
modelDialog.painting=0;

GtkWidget* dialog=gtk_dialog_new_with_buttons("Model Settings",NULL,0,"OK",GTK_RESPONSE_OK,NULL);
GtkWidget* contentArea=gtk_dialog_get_content_area(GTK_DIALOG(dialog));
modelDialog.dialog=dialog;

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


//Flip options
GtkWidget* flipOptions=gtk_hbox_new(TRUE,1);
GtkWidget* flipX=gtk_check_button_new_with_label("Flip X");
GtkWidget* flipY=gtk_check_button_new_with_label("Flip Y");
GtkWidget* flipZ=gtk_check_button_new_with_label("Flip Z");
g_signal_connect(flipX,"toggled",G_CALLBACK(FlipX),&modelDialog);
g_signal_connect(flipY,"toggled",G_CALLBACK(FlipY),&modelDialog);
g_signal_connect(flipZ,"toggled",G_CALLBACK(FlipZ),&modelDialog);
gtk_box_pack_start(GTK_BOX(flipOptions),flipX,TRUE,TRUE,5);
gtk_box_pack_start(GTK_BOX(flipOptions),flipY,TRUE,TRUE,5);
gtk_box_pack_start(GTK_BOX(flipOptions),flipZ,TRUE,TRUE,5);
gtk_box_pack_start(GTK_BOX(contentArea),flipOptions,TRUE,TRUE,2);

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

//Preview
modelDialog.preview=gtk_image_new();
GtkWidget* eventBox=gtk_event_box_new();
gtk_widget_set_events(eventBox,GDK_POINTER_MOTION_MASK);
gtk_container_add(GTK_CONTAINER(eventBox),modelDialog.preview);
modelDialog.pixbuf=CreateBlankPixbuf();
gtk_image_set_from_pixbuf(GTK_IMAGE(modelDialog.preview),modelDialog.pixbuf);
g_signal_connect(eventBox,"motion_notify_event",G_CALLBACK(PaintModel),&modelDialog);
g_signal_connect(eventBox,"button_press_event",G_CALLBACK(PreviewPressed),&modelDialog);
g_signal_connect(eventBox,"button_release_event",G_CALLBACK(PreviewReleased),&modelDialog);
gtk_box_pack_start(GTK_BOX(previewHBox),eventBox,TRUE,TRUE,2);


//Painting options
GtkWidget* paintLabel=gtk_label_new("Paint mode:");
GtkWidget* recolorable1=gtk_radio_button_new_with_label(NULL,"Recolorable area 1");
GtkWidget* recolorable2=gtk_radio_button_new_with_label(gtk_radio_button_get_group(GTK_RADIO_BUTTON(recolorable1)),"Recolorable area 2");
GtkWidget* recolorable3=gtk_radio_button_new_with_label(gtk_radio_button_get_group(GTK_RADIO_BUTTON(recolorable2)),"Recolorable area 3");
GtkWidget* nonColorable=gtk_radio_button_new_with_label(gtk_radio_button_get_group(GTK_RADIO_BUTTON(recolorable3)),"Clear recolorability");
GtkWidget* selectColor=gtk_radio_button_new_with_label(gtk_radio_button_get_group(GTK_RADIO_BUTTON(nonColorable)),"Select color:");
g_signal_connect(recolorable1,"clicked",G_CALLBACK(Recolor1Clicked),&modelDialog);
g_signal_connect(recolorable2,"clicked",G_CALLBACK(Recolor2Clicked),&modelDialog);
g_signal_connect(recolorable3,"clicked",G_CALLBACK(Recolor3Clicked),&modelDialog);
g_signal_connect(nonColorable,"clicked",G_CALLBACK(NonColorClicked),&modelDialog);
gtk_box_pack_start(GTK_BOX(previewVBox),paintLabel,FALSE,FALSE,2);
gtk_box_pack_start(GTK_BOX(previewVBox),recolorable1,FALSE,FALSE,2);
gtk_box_pack_start(GTK_BOX(previewVBox),recolorable2,FALSE,FALSE,2);
gtk_box_pack_start(GTK_BOX(previewVBox),recolorable3,FALSE,FALSE,2);
gtk_box_pack_start(GTK_BOX(previewVBox),nonColorable,FALSE,FALSE,2);
gtk_box_pack_start(GTK_BOX(previewVBox),selectColor,FALSE,FALSE,2);
gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(recolorable1),1);
modelDialog.paintState=RECOLORABLE_1;


//View controls
GtkWidget* viewTable=gtk_table_new(3,3,TRUE);
GtkWidget* image;
GtkWidget* reset=gtk_button_new_with_label("Reset");
image=gtk_image_new_from_stock(GTK_STOCK_GO_BACK,GTK_ICON_SIZE_BUTTON);
GtkWidget* rotateLeft=gtk_button_new();
gtk_container_add(GTK_CONTAINER(rotateLeft),image);
image=gtk_image_new_from_stock(GTK_STOCK_GO_FORWARD,GTK_ICON_SIZE_BUTTON);
GtkWidget* rotateRight=gtk_button_new();
gtk_container_add(GTK_CONTAINER(rotateRight),image);
image=gtk_image_new_from_stock(GTK_STOCK_GO_UP,GTK_ICON_SIZE_BUTTON);
GtkWidget* rotateUp=gtk_button_new();
gtk_container_add(GTK_CONTAINER(rotateUp),image);
image=gtk_image_new_from_stock(GTK_STOCK_GO_DOWN,GTK_ICON_SIZE_BUTTON);
GtkWidget* rotateDown=gtk_button_new();
gtk_container_add(GTK_CONTAINER(rotateDown),image);
image=gtk_image_new_from_stock(GTK_STOCK_ZOOM_IN,GTK_ICON_SIZE_BUTTON);
GtkWidget* zoomIn=gtk_button_new();
gtk_container_add(GTK_CONTAINER(zoomIn),image);
image=gtk_image_new_from_stock(GTK_STOCK_ZOOM_OUT,GTK_ICON_SIZE_BUTTON);
GtkWidget* zoomOut=gtk_button_new();
gtk_container_add(GTK_CONTAINER(zoomOut),image);
g_signal_connect(reset,"clicked",G_CALLBACK(ResetView),&modelDialog);
g_signal_connect(rotateLeft,"clicked",G_CALLBACK(RotateViewLeft),&modelDialog);
g_signal_connect(rotateRight,"clicked",G_CALLBACK(RotateViewRight),&modelDialog);
g_signal_connect(rotateUp,"clicked",G_CALLBACK(RotateViewUp),&modelDialog);
g_signal_connect(rotateDown,"clicked",G_CALLBACK(RotateViewDown),&modelDialog);
g_signal_connect(zoomIn,"clicked",G_CALLBACK(ZoomViewIn),&modelDialog);
g_signal_connect(zoomOut,"clicked",G_CALLBACK(ZoomViewOut),&modelDialog);
gtk_table_attach_defaults(GTK_TABLE(viewTable),reset,1,2,1,2);
gtk_table_attach_defaults(GTK_TABLE(viewTable),rotateLeft,0,1,1,2);
gtk_table_attach_defaults(GTK_TABLE(viewTable),rotateRight,2,3,1,2);
gtk_table_attach_defaults(GTK_TABLE(viewTable),rotateUp,1,2,0,1);
gtk_table_attach_defaults(GTK_TABLE(viewTable),rotateDown,1,2,2,3);
gtk_table_attach_defaults(GTK_TABLE(viewTable),zoomIn,0,1,0,1);
gtk_table_attach_defaults(GTK_TABLE(viewTable),zoomOut,2,3,0,1);
gtk_box_pack_start(GTK_BOX(previewVBox),viewTable,FALSE,FALSE,2);

gtk_box_pack_start(GTK_BOX(previewHBox),previewVBox,TRUE,TRUE,2);
gtk_box_pack_start(GTK_BOX(contentArea),previewHBox,TRUE,TRUE,2);
RenderPreview(&modelDialog);

gtk_widget_show_all(contentArea);

gtk_dialog_run(GTK_DIALOG(modelDialog.dialog));
gtk_widget_destroy(modelDialog.dialog);
}

