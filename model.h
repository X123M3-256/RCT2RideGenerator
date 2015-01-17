#ifndef MODEL_H_INCLUDED
#define MODEL_H_INCLUDED
#include <stdint.h>
#include "linearalgebra.h"

typedef struct
{
uint8_t color;
uint16_t vertices[2];
}line_t;

typedef struct
{
uint16_t color;
//Each point has a position, normal
uint16_t vertices[3];
uint16_t normals[3];
}face_t;

typedef struct
{
char* name;
Matrix transform;
unsigned short num_vertices;
unsigned short num_normals;
unsigned short num_lines;
unsigned short num_faces;
Vector* vertices;
Vector* normals;
line_t* lines;
face_t* faces;
}model_t;



model_t* model_new();
model_t* model_load_obj(char* filename);
void model_free(model_t* model);
model_t* model_new_grid();

#endif // MODEL_H_INCLUDED
