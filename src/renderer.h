#ifndef RENDERER_H_INCLUDED
#define RENDERER_H_INCLUDED
#include "image.h"
#include "linearalgebra.h"
#include "model.h"

typedef struct {
    unsigned char color;
    Vector vertices[3];
    Vector normals[3];
} primitive_t;

void renderer_clear_buffers();
image_t* renderer_get_image();
void renderer_remap_color(uint8_t source, uint8_t dest);
void renderer_render_model(model_t* model, Matrix modelview);
face_t* renderer_get_face_by_point(model_t* model,
    Matrix model_view,
    Vector coords,
    float* depth);
void renderer_clear_color_buffer();
void renderer_cut(Vector point, Vector normal);
#endif // RENDERER_H_INCLUDED
