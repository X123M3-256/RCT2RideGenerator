#include <stdlib.h>
#include <string.h>
#include "image.h"

image_t* image_new(uint8_t width,uint8_t height,uint8_t color)
{
image_t* image=malloc(sizeof(image_t));
image->x_offset=0;
image->y_offset=0;
image->flags=1;
image->width=width;
image->height=height;
image->data=malloc(height*sizeof(uint8_t*));
int i;
    for(i=0;i<height;i++)
    {
    image->data[i]=malloc(width*sizeof(uint8_t));
    memset(image->data[i],color,width);
    }
return image;
}

void image_free(image_t* image)
{
int i;
    for(i=0;i<image->height;i++)free(image->data[i]);
free(image->data);
free(image);
}

