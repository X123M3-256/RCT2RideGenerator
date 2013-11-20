#ifndef PALETTE_H_INCLUDED
#define PALETTE_H_INCLUDED

#define PALETTE_SIZE 255
typedef struct
{
unsigned char Blue;
unsigned char Green;
unsigned char Red;
unsigned char Reserved;
}Color;
typedef struct
{
unsigned char Hue;
unsigned char Saturation;
unsigned char Value;
unsigned char Reserved;
}ColorHSV;
Color GetColorFromPalette(unsigned char index);
#endif // PALETTE_H_INCLUDED
