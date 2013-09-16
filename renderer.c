#include <stdio.h>
#include "palette.h"
#include "datastructures.h"
#include "renderer.h"
static RenderInfo* Renderer;

typedef struct
{
char* Name;
float r;
float g;
float b;
}Material;
typedef struct
{
int PointIndices[3];
int NormalIndices[3];
int MaterialIndex;
}Face;
typedef struct
{
LList* Materials;
LList* Points;
LList* Normals;
LList* Faces;
}ObjData;

Material* CreateMaterial(char* name,float r,float g,float b)
{
Material* Mat=malloc(sizeof(Material));
int NameLen=strlen(name);
Mat->Name=malloc(NameLen+1);
strcpy(Mat->Name,name);
Mat->r=r;
Mat->g=g;
Mat->b=b;
return Mat;
}
void FreeMaterial(Material* Mat)
{
free(Mat->Name);
free(Mat);
}
void FreeMaterials(LList* Materials)
{
FreeMaterial(Materials->Data);
if(Materials->Next!=NULL)FreeMaterials(Materials->Next);
free(Materials);
}
Vector* CreateVector(float x,float y,float z)
{
Vector* Vec=malloc(sizeof(Vector));
Vec->x=x;
Vec->y=y;
Vec->z=z;
return Vec;
}
Face* CreateFace(int MaterialIndex,int* Points,int* Normals)
{
Face* Poly=malloc(sizeof(Face));
Poly->MaterialIndex=MaterialIndex;
int i;
    for(i=0;i<3;i++)
    {
    Poly->PointIndices[i]=Points[i];
    Poly->NormalIndices[i]=Normals[i];
    }
return Poly;
}
LList* LoadMaterials(char* filename)
{
char* Data=ReadFileText(filename);
LList* Materials=NULL;
Material* CurMaterial=NULL;
char* CurPtr=Data;
char Str[8];
    while(*CurPtr!=0)
    {
        if(sscanf(CurPtr,"%7s",Str)>0)
        {
            if(strcmp(Str,"newmtl")==0)
            {
            char name[256];
                if(sscanf(CurPtr,"newmtl %255s",name)==1)
                {
                CurMaterial=CreateMaterial(name,0,0,0);
                Materials=ListAdd(Materials,CurMaterial);
                }
            }
            //Diffuse color is the only property actually read
            else if(strcmp(Str,"Kd")==0)
            {
            float r,g,b;
                if(sscanf(CurPtr,"Kd %f %f %f",&r,&g,&b)==3)
                {
                CurMaterial->r=r;
                CurMaterial->g=g;
                CurMaterial->b=b;
                }
            }
        }
    while(*CurPtr!='\n'&&*CurPtr!=0)CurPtr++;//Go to next line
    if(*CurPtr!=0)CurPtr++;
    }
return Materials;
}
ObjData* LoadObj(char* filename)
{
char* Data=ReadFileText(filename);

LList* Points=NULL;
LList* Normals=NULL;
LList* Materials=NULL;
LList* Faces=NULL;
int CurMaterialIndex=-1;
//Read file and extract data
char* CurPtr=Data;
char Str[8];
    while(*CurPtr!=0)
    {
        if(sscanf(CurPtr,"%8s",Str)>0)
        {
            if(strcmp(Str,"mtllib")==0)
            {
            char* name[256];
                if(sscanf(CurPtr,"mtllib %255s",name)==1)
                {
                Materials=LoadMaterials(name);
                }
            }
            //Change material
            else if(strcmp(Str,"usemtl")==0)
            {
            char* name[256];
                if(sscanf(CurPtr,"usemtl %255s",name)==1)
                {
                LList* CurList=Materials;
                CurMaterialIndex=-1;
                while(CurList!=NULL)
                {
                CurMaterialIndex++;
                if(strcmp(name,((Material*)(CurList->Data))->Name)==0)break;
                CurList=CurList->Next;
                }
                }
            }
            //Load vertex
            else if(strcmp(Str,"v")==0)
            {
            float x,y,z;
                if(sscanf(CurPtr,"v %f %f %f",&x,&y,&z)==3)
                {
                Points=ListAdd(Points,CreateVector(x,y,z));
                }
            }
            //Load normal
            else if(strcmp(Str,"vn")==0)
            {
            float x,y,z;
                if(sscanf(CurPtr,"vn %f %f %f",&x,&y,&z)==3)
                {
                Normals=ListAdd(Normals,CreateVector(x,y,z));
                }
            }
            //Load face
            else if(strcmp(Str,"f")==0)
            {
                int PointIndices[3];
                int NormalIndices[3];
                if(sscanf(CurPtr,"f %d//%d %d//%d %d//%d",PointIndices,NormalIndices,PointIndices+1,NormalIndices+1,PointIndices+2,NormalIndices+2)==6)
                {
                Faces=ListAdd(Faces,CreateFace(CurMaterialIndex,PointIndices,NormalIndices));
                }
            }
        }
    while(*CurPtr!='\n'&&*CurPtr!=0)CurPtr++;//Go to next line
    if(*CurPtr!=0)CurPtr++;
    }
ObjData* Result=malloc(sizeof(ObjData));
Result->Materials=Materials;
Result->Points=Points;
Result->Normals=Normals;
Result->Faces=Faces;
return Result;
}
struct VertexSignature
{
int MaterialIndex;
int PointIndex;
int NormalIndex;
};
Object* CreateObject(char* filename,char* name)
{
ObjData* Data=LoadObj(filename);

//Generate the opengl vertices
DynamicBuffer* VertexBuffer=CreateBuffer(512);
DynamicBuffer* IndexBuffer=CreateBuffer(512);
GLuint NumVertices=0;
GLuint NumIndices=0;
LList* VertexSignatures=NULL;
LList* CurList=Data->Faces;
    while(CurList!=NULL)
    {
    Face* CurFace=CurList->Data;
    Material* CurMaterial=(Material*)GetIndex(Data->Materials,CurFace->MaterialIndex);
    int i;
        for(i=2;i>=0;i--)
        {
        Vertex CurVertex;
        Vector* CurPoint=(Vector*)GetIndex(Data->Points,CurFace->PointIndices[i]-1);
        Vector* CurNormal=(Vector*)GetIndex(Data->Normals,CurFace->NormalIndices[i]-1);
        CurVertex.location[0]=CurPoint->x;
        CurVertex.location[1]=CurPoint->y;
        CurVertex.location[2]=CurPoint->z;

        CurVertex.normal[0]=CurNormal->x;
        CurVertex.normal[1]=CurNormal->y;
        CurVertex.normal[2]=CurNormal->z;
        if(CurMaterial!=NULL)
        {
        CurVertex.color[0]=(GLubyte)((CurMaterial->r*255)+0.5);
        CurVertex.color[1]=(GLubyte)((CurMaterial->g*255)+0.5);
        CurVertex.color[2]=(GLubyte)((CurMaterial->b*255)+0.5);
        }
        else
        {
        CurVertex.color[0]=255;
        CurVertex.color[1]=255;
        CurVertex.color[2]=255;
        }
        WriteBuffer(&CurVertex,sizeof(Vertex),VertexBuffer);
        WriteBuffer(&NumVertices,sizeof(GLuint),IndexBuffer);
        NumVertices++;
        NumIndices++;
        }
    CurList=CurList->Next;
    }
FreeMaterials(Data->Materials);
FreeList(Data->Points);
FreeList(Data->Normals);
FreeList(Data->Faces);


Vertex* Vertices=(Vertex*)FreeBuffer(VertexBuffer);
GLuint* Indices=(Vertex*)FreeBuffer(IndexBuffer);

Object* Obj=malloc(sizeof(Object));
Obj->Name=malloc(strlen(name)+1);
strcpy(Obj->Name,name);
Obj->NumVertices=NumVertices;
Obj->NumIndices=NumIndices;
Obj->Parent=NULL;
glGenBuffers(1,&Obj->VBO);
glGenBuffers(1,&Obj->IBO);
glBindBuffer(GL_ARRAY_BUFFER,Obj->VBO);
glBufferData(GL_ARRAY_BUFFER,Obj->NumVertices*sizeof(Vertex),Vertices,GL_STATIC_DRAW);

glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,Obj->IBO);
glBufferData(GL_ELEMENT_ARRAY_BUFFER,Obj->NumIndices*sizeof(GLuint),Indices,GL_STATIC_DRAW);
return Obj;
}

void FreeObject(Object* Obj)
{
free(Obj->Name);
free(Obj);
}
void InitOpenGL(int Width,int Height)
{
//Create render context
Renderer=malloc(sizeof(RenderInfo));
Renderer->Data=malloc(Width*Height*4*sizeof(GLubyte));
Renderer->Width=Width;
Renderer->Height=Height;
OSMesaContext context=OSMesaCreateContext(OSMESA_RGB,NULL);
OSMesaMakeCurrent(context,Renderer->Data,GL_UNSIGNED_BYTE,Width,Height);
Renderer->Context=context;
//Create shaders
int FragmentShader=glCreateShader(GL_FRAGMENT_SHADER);
int VertexShader=glCreateShader(GL_VERTEX_SHADER);


char* FragmentShaderSource=ReadFileText("rct2shader.glsl");
int Error;
glShaderSource(FragmentShader,1,&FragmentShaderSource,NULL);
glCompileShader(FragmentShader);
glGetShaderiv(FragmentShader, GL_COMPILE_STATUS, &Error);
if(!Error)printf("Failed to compile fragment shader\n");
free(FragmentShaderSource);

char* VertexShaderSource=ReadFileText("vertexshader.glsl");
glShaderSource(VertexShader,1,&VertexShaderSource,NULL);
glCompileShader(VertexShader);
glGetShaderiv(VertexShader, GL_COMPILE_STATUS, &Error);
if(!Error)printf("Failed to compile vertex shader\n");
free(VertexShaderSource);


int ShaderProgram=glCreateProgram();
glAttachShader(ShaderProgram,FragmentShader);
glAttachShader(ShaderProgram,VertexShader);
glLinkProgram(ShaderProgram);
glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Error);
if(!Error)printf("Failed to link shader program\n");
Renderer->Shader=ShaderProgram;
glUseProgram(ShaderProgram);
//Create palette for render
SetDrawablePalette(Renderer);
//Set up matrices
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glClearColor(0.0,0.0,0.0,0.0);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  float ViewportWidth=((float)Renderer->Width)/12.3168;
  float ViewportHeight=((float)Renderer->Height)/12.3168;
  glOrtho(ViewportWidth/-2,ViewportWidth/2,ViewportHeight/-2,ViewportWidth/2,1,50);
  glScalef(1.0,-1.0,1.0);
  glTranslatef(0.0, 0.0, -25.0);
  glRotatef(30, 1.0, 0.0, 0.0);
  glRotatef(-45, 0.0, 1.0, 0.0);
  return Renderer;
}

void QuitOpenGL()
{
OSMesaDestroyContext(Renderer->Context);
free(Renderer->Data);
free(Renderer);
}

int Maximum(int a,int b,int c)
{
int max1=a>b?a:b;
return max1>c?max1:c;
}
int Minimum(int a,int b,int c)
{
int max1=a<b?a:b;
return max1<c?max1:c;
}
typedef struct
{
float Hue;
float Saturation;
float Value;
}HSV;
HSV GetHSV(Color RGB)
{
HSV ColorHSV;
float Max=(float)(Maximum(RGB.Red,RGB.Blue,RGB.Green))/255.0;
float Min=(float)(Minimum(RGB.Red,RGB.Blue,RGB.Green))/255.0;
float Delta=Max-Min;
//V
ColorHSV.Value=Max;
//S
if(Max>0.002)
{
ColorHSV.Saturation=Delta/Max;
    //H
    if(RGB.Red==Max)
		ColorHSV.Hue=((float)(RGB.Green-RGB.Blue)/255.0)/Delta;
	else if(RGB.Blue==Max)
		ColorHSV.Hue=2.0+((float)(RGB.Blue-RGB.Red)/255.0)/Delta;
	else
		ColorHSV.Hue=4.0+((float)(RGB.Red-RGB.Green)/255.0)/Delta;

ColorHSV.Hue=ColorHSV.Hue/6.0;
if(ColorHSV.Hue<0)ColorHSV.Hue+=1;
}
else
{
ColorHSV.Saturation=0.0;
ColorHSV.Hue=0.0;//Technically, it's undefined, but I have to give it *some* value
}
return ColorHSV;
}
void SetDrawablePalette(RenderInfo* Renderer)
{
//Get locations of shader variables
int Palette=glGetUniformLocation(Renderer->Shader,"Palette");
int PaletteHSV=glGetUniformLocation(Renderer->Shader,"PaletteHSV");
int Remap1Index=glGetUniformLocation(Renderer->Shader,"Remap1Index");
int Remap2Index=glGetUniformLocation(Renderer->Shader,"Remap2Index");

GLfloat* DrawablePalette=malloc(255*3*sizeof(GLfloat));
GLfloat* DrawablePaletteHSV=malloc(255*3*sizeof(GLfloat));

//Fill pallete with data: for now, this is everything minus the two default remappable colors. Later, this will depend on what's selected
int i;
Color Col;
HSV ColHSV;
for(i=0;i<255;i++)
    {
    Col=GetColorFromPalette(i);
    ColHSV=GetHSV(Col);
    DrawablePalette[i*3]=((float)Col.Red)/255.0;
    DrawablePalette[(i*3)+1]=((float)Col.Green)/255.0;
    DrawablePalette[(i*3)+2]=((float)Col.Blue)/255.0;
    DrawablePaletteHSV[i*3]=ColHSV.Hue;
    DrawablePaletteHSV[(i*3)+1]=ColHSV.Saturation;
    DrawablePaletteHSV[(i*3)+2]=ColHSV.Value;
    }

glUniform3fv(Palette,255,DrawablePalette);
glUniform3fv(PaletteHSV,255,DrawablePaletteHSV);
glUniform1i(Remap1Index,243);
glUniform1i(Remap2Index,202);

free(DrawablePalette);
free(DrawablePaletteHSV);
}

void StartRender()
{
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
glEnableClientState(GL_VERTEX_ARRAY);
glEnableClientState(GL_NORMAL_ARRAY);
glEnableClientState(GL_COLOR_ARRAY);
}
void FinishRender()
{
glDisableClientState(GL_NORMAL_ARRAY);
glDisableClientState(GL_VERTEX_ARRAY);
glFlush();
}
Image* GetRenderedImage()
{
Image* Img=malloc(sizeof(Image));


int XStart=-1;
int YStart=-1;
int XEnd=-1;
int YEnd=-1;

int x,y;
    for(x=0;x<Renderer->Width;x++)
    {
    int Flag=0;
        for(y=0;y<Renderer->Height;y++)
        {
            if(Renderer->Data[(((y*Renderer->Width)+x)*3)]!=0)
            {
            Flag=1;
            if(YStart==-1||y<YStart)YStart=y;
            if(YEnd==-1||y+1>YEnd)YEnd=y+1;
            }
        }
    if(XStart==-1&&Flag)XStart=x;
    if(Flag)XEnd=x+1;
    }

if(XStart==-1||YStart==-1||XEnd==-1||YEnd==-1)
{
XStart=0;
XEnd=1;
YStart=0;
YEnd=1;
}

int Width=XEnd-XStart;
int Height=YEnd-YStart;

Img->Width=Width;
Img->Height=Height;
Img->Flags=1;
Img->Data=malloc(Height*sizeof(char*));
Img->XOffset=XStart-(Renderer->Width/2);
Img->YOffset=YStart-(Renderer->Height/2);
    for(y=0;y<Height;y++)
    {
    Img->Data[y]=malloc(Width);
        for(x=0;x<Width;x++)
        {
        Img->Data[y][x]=Renderer->Data[((((y+YStart)*Renderer->Width)+(x+XStart))*3)];
        }
    }
return Img;
}
/*
void SetModelView(Object* Obj)
{
if(Obj->Parent!=NULL)SetModelView(Obj->Parent);
glTranslatef(Obj->Position.x,Obj->Position.y,Obj->Position.z);
glRotatef(-Obj->Rotation.y,0.0,1.0,0.0);
glRotatef(Obj->Rotation.x,1.0,0.0,0.0);
glRotatef(Obj->Rotation.z,0.0,0.0,1.0);
}
*/
void DrawObject(Object* Obj)
{
glMatrixMode(GL_MODELVIEW);
glLoadIdentity();
//SetModelView(Obj);
glBindBuffer(GL_ARRAY_BUFFER,Obj->VBO);
glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,Obj->IBO);
glNormalPointer(GL_FLOAT,sizeof(Vertex),12);
glColorPointer(3,GL_UNSIGNED_BYTE,sizeof(Vertex),24);
glVertexPointer(3,GL_FLOAT,sizeof(Vertex),0);
glDrawElements(GL_TRIANGLES,Obj->NumIndices,GL_UNSIGNED_INT,0);
}
