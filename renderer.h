#ifndef RENDERER_H_INCLUDED
#define RENDERER_H_INCLUDED
#include<GL/gl.h>
#include<GL/osmesa.h>
#include "types.h"

typedef struct
{
OSMesaContext Context;
int Shader;
GLubyte* Data;
int Width;
int Height;
}RenderInfo;

typedef struct
{
float x;
float y;
float z;
}Vector;

typedef struct obj
{
struct obj* Parent;
char* Name;
int VBO;
int IBO;
int NumVertices;
int NumIndices;
}Object;

typedef struct
{
GLfloat location[3];
GLfloat normal[3];
GLbyte color[3];
GLbyte padding[5];
}Vertex;

Object* CreateObject(char* filename,char* Name);
void StartRender();
Image* GetRenderedImage();
void FinishRender();
void DrawObject(Object* Obj);
void FreeObject(Object* Obj);
#endif // RENDERER_H_INCLUDED
