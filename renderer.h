#ifndef RENDERER_H_INCLUDED
#define RENDERER_H_INCLUDED

#include "dat.h"
#include "linearalgebra.h"
#include "model.h"

typedef struct
{
unsigned char Color;
Vector Vertices[3];
Vector Normals[3];
}Primitive;


void ClearBuffers();
void RenderModel(Model* model,Matrix modelview);
Face* GetFaceEnclosingPoint(Model* model,Matrix modelView,Vector coords);
Image ImageFromFrameBuffer();

#endif // RENDERER_H_INCLUDED
