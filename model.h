#ifndef MODEL_H_INCLUDED
#define MODEL_H_INCLUDED

#define RECOLOR_GREEN 1
#define RECOLOR_MAGENTA 2
#define RECOLOR_YELLOW 3

#include "linearalgebra.h"

typedef struct
{
unsigned short Color;
unsigned short Vertices[2];
}Line;

typedef struct
{
//Flags such as recolorability etc.
unsigned short Flags;
unsigned short Color;
//Each point has a position, normal
unsigned short Vertices[3];
unsigned short Normals[3];
}Face;

typedef struct
{
char* Name;
Matrix transform;
unsigned short NumVertices;
unsigned short NumNormals;
unsigned short NumLines;
unsigned short NumFaces;
Vector* Vertices;
Vector* Normals;
Line* Lines;
Face* Faces;
}Model;


#define MAX_MODELS 32

struct
    {
    Model* Models[32];
    int NumModels;
    }ModelList;

int NumModels();
Model* GetModelByIndex(int index);
Model* GetModelByName(const char* name);

#endif // MODEL_H_INCLUDED
