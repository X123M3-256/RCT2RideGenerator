#ifndef BACKEND_H_INCLUDED
#define BACKEND_H_INCLUDED
#include "types.h"
#include <stdio.h>
#include <stdlib.h>
void LoadFile(char* filename);
void SaveFile(char* filename);
int GetNumImages();
Image* GetImage(int Index);
void SetImage(int Index,Image* Img);
void FreeRideFile();
#endif // BACKEND_H_INCLUDED
