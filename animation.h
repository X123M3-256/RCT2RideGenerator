#ifndef ANIMATION_H_INCLUDED
#define ANIMATION_H_INCLUDED
#include "renderer.h"

#define NONE 1
#define RENDER 2
#define MASK 3

typedef struct
{
Vector Position;
Vector Rotation;
int RenderType;
}Keyframe;

typedef struct ao
{
struct ao* Parent;
Object* Object;
Keyframe* Keyframes;
int NumKeyframes;
}AnimObject;

typedef struct
{
AnimObject* Objects;
int NumObjects;
}Animation;

void LoadTemplate(char* filename);
void RenderAnimation();
void AddObject(Object* Obj);
char* GetTextFromFrame(int Frame);

#endif // ANIMATION_H_INCLUDED
