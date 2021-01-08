#ifndef MODEL_H_INCLUDED
#define MODEL_H_INCLUDED
#include "linearalgebra.h"
#include <stdint.h>

typedef struct {
    uint8_t color;
    uint16_t vertices[2];
} line_t;

typedef struct {
    uint16_t color;
    // Each point has a position, normal
    uint32_t vertices[3];
    uint32_t normals[3];
} face_t;

typedef struct {
    char* name;
    Matrix transform;
    unsigned short is_rider;
    unsigned int num_vertices;
    unsigned int num_normals;
    unsigned int num_lines;
    unsigned int num_faces;
    Vector* vertices;
    Vector* normals;
    line_t* lines;
    face_t* faces;
} model_t;

model_t* model_new();
model_t* model_load_obj(char* filename);
void model_free(model_t* model);
model_t* model_new_grid();

#endif // MODEL_H_INCLUDED
