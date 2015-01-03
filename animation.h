#ifndef ANIMATION_H_INCLUDED
#define ANIMATION_H_INCLUDED
#include "model.h"
#include "linearalgebra.h"

#define NONE 1
#define RENDER 2
#define MASK 3
/*
typedef struct
{
int FrameNum;
int RenderType;
//Vector Position;
//Vector Rotation;
}KeyFrame;


typedef struct ao
{
struct ao* Parent;
Object* Object;
KeyFrame* KeyFrames;
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
*/
#define MAX_ANIMATIONS 8
#define MAX_FRAMES 64
#define MAX_OBJECTS_PER_FRAME 16

typedef struct
{
Vector Position;
Vector Rotation;
Matrix Transform;
}ObjectTransform;

typedef struct
{
Model* model;
int parentIndex;
}ObjectData;

typedef struct
{
char* Name;
ObjectTransform Frames[MAX_FRAMES][MAX_OBJECTS_PER_FRAME];
ObjectData Objects[MAX_OBJECTS_PER_FRAME];
int NumObjects;
int NumFrames;
}Animation;

struct
{
Animation* Animations[MAX_ANIMATIONS];
int NumAnimations;
}AnimationList;



Animation* CreateAnimation();
void SetName(Animation* animation,const char* name);
void SetNumFrames(Animation* animation,int frames);
int AddObject(Animation* animation,Model* model);
void UpdateTransform(Animation* animation,int frame,int object,Vector position,Vector rotation);
void UpdateParent(Animation* animation,int object,int parent);
void RenderFrame(Animation* animation,int frame,Matrix modelView);

void AddAnimation(Animation* animation);
int NumAnimations();
Animation* GetAnimationByIndex(int index);



#endif // ANIMATION_H_INCLUDED
