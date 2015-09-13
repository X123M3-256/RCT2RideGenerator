#include <stdio.h>
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

image_t* image_copy(image_t* source)
{
image_t* image=malloc(sizeof(image_t));
image->x_offset=source->x_offset;
image->y_offset=source->y_offset;
image->flags=source->flags;
image->width=source->width;
image->height=source->height;
image->data=malloc(source->height*sizeof(uint8_t*));

    for(int y=0;y<source->height;y++)
    {
    image->data[y]=malloc(source->width*sizeof(uint8_t));
        for(int x=0;x<source->height;x++)image->data[y][x]=source->data[y][x];
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

