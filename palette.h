#ifndef PALETTE_H_INCLUDED
#define PALETTE_H_INCLUDED
#include<stdint.h>
#define PALETTE_SIZE 255

#define COLOR_REMAP_1 32
#define COLOR_REMAP_2 33
#define COLOR_REMAP_3 34

typedef struct
{
uint8_t blue;
uint8_t green;
uint8_t red;
uint8_t reserved;
}color_t;
/*
typedef struct
{
unsigned char Hue;
unsigned char Saturation;
unsigned char Value;
unsigned char Reserved;
}ColorHSV;
*/
uint8_t palette_remap_section_index(uint8_t color,uint8_t section_index);
color_t palette_color_from_index(uint8_t index);
#endif // PALETTE_H_INCLUDED
