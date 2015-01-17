#ifndef IMAGE_H_INCLUDED
#define IMAGE_H_INCLUDED
#include<stdint.h>

typedef struct
{
uint16_t width;
uint16_t height;
int16_t x_offset;
int16_t y_offset;
uint16_t flags;
uint8_t** data;
}image_t;

image_t* image_new(uint8_t width,uint8_t height,uint8_t color);
void image_free(image_t* image);
//GdkPixbuf* CreateBlankPixbuf();
//void ShowImageInPixbuf(GdkPixbuf* pixbuf,Image* image);
#endif // IMAGE_H_INCLUDED
