#include "animation.h"
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "datastructures.h"
#include "interface.h"
#include "backend.h"
static Animation* Anim=NULL;


char* ReadFileText(char* filename)
{
FILE* file=fopen(filename,"r");
if(file==NULL)return NULL;
fseek(file,0,SEEK_END);
int length=ftell(file);
fseek(file,0,SEEK_SET);
char* Data=malloc(length+1);
fread(Data,length,1,file);
Data[length]=0;
fclose(file);
return Data;
}


Object* LoadObject(char* name)
{
char text[64];
sprintf(text,"Select object file for %.40s:",name);
char* Filename=GetFilenameFromUser(text);
if(Filename==NULL)return NULL;
Object* Obj=CreateObject(Filename,name);
return Obj;
}

AnimObject* GetObjectByName(char* Name)
{
int i;
    for(i=0;i<Anim->NumObjects;i++)
    {
        if(strcmp(Anim->Objects[i].Object->Name,Name)==0)
        {
        return Anim->Objects+i;
        }
    }
return NULL;
}

void ParseFrames(char* Str)
{
int FrameIndex=0;
char* Token;
    while((Token=strtok(NULL," \n"))!=NULL)
    {
    int Command=0;
        if(strcmp(Token,"keyframe")==0)
        {
        Token=strtok(NULL," \n");
        if(Token==NULL)continue;
        continue;
        }
        else if(strcmp(Token,"ignore")==0)
        {
        continue;
        }
        else if(strcmp(Token,"position")==0)
        {
//        Command=POSITION;
        }
        else if(strcmp(Token,"render")==0)
        {
  //      Command=RENDER;
        }
    Token=strtok(NULL," \n");
    if(Token==NULL)continue;
    AnimObject* Obj=GetObjectByName(Token);
//    KeyFrame* Frame=GetKeyFrameByIndex(Obj,FrameIndex);
    }
}


/*
char* GetTextFromFrame(AnimFrame* Frame)
{
DynamicBuffer* Text=CreateBuffer(256);

if(Frame->Flags&IGNORED)
{
WriteBuffer("ignore\n",8,Text);
}
int i;
    for(i=0;i<Frame->NumCommands;i++)
    {
    char Line[512];
    switch(Frame->Commands[i].Command)
    {
    case SET_POSITION_ROTATION:
        {
        Vector* data=Frame->Commands[i].data;
       sprintf(Line,"%.255s %.3f,%.3f,%.3f,%.3f,%.3f,%.3f\n",Frame->Commands[i].Object->Name,data[0].x,data[0].y,data[0].z,data[1].x,data[1].y,data[1].z);
        }
    break;
    case SET_ROTATION:
        {
        Vector* data=Frame->Commands[i].data;
        sprintf(Line,"%.255s %.3f,%.3f,%.3f\n",Frame->Commands[i].Object->Name,data[0].x,data[0].y,data[0].z);
        }
    break;
    case RENDER:
       sprintf(Line,"render %.255s\n",Frame->Commands[i].Object->Name);
    break;
    }
     WriteBuffer(Line,strlen(Line),Text);
    }
char zero=0;
WriteBuffer(&zero,1,Text);
char* Str=FreeBuffer(Text);

if(Str==NULL)
{
Str=malloc(1);
*Str=0;
}
return Str;
}
*/
AnimObject CreateAnimObject(Object* Obj)
{
AnimObject AnimObj;
AnimObj.KeyFrames=NULL;
AnimObj.NumKeyframes=0;
AnimObj.Parent=NULL;
AnimObj.Object=Obj;
return AnimObj;
}

void AddObject(Object* Obj)
{
Anim->Objects=realloc(Anim->Objects,(Anim->NumObjects+1)*sizeof(AnimObject));
Anim->Objects[Anim->NumObjects]=CreateAnimObject(Obj);
Anim->NumObjects++;
}

void LoadTemplate(char* filename)
{
//if(Anim!=NULL)FreeAnimation();

char* Text=ReadFileText(filename);
if(Text!=NULL)
{
    Anim=malloc(sizeof(Animation));
    Anim->Objects=NULL;
    Anim->NumObjects=0;

    char* Token=strtok(Text," \n");
        while(Token!=NULL)
        {
                if(strcmp(Token,"object")==0)
                {
                char* Name=strtok(NULL," \n");
                    if(Name!=NULL)
                    {
                    Object* Obj=LoadObject(Name);
                    if(Obj!=NULL)AddObject(Obj);
                    }
                }
                else if(strcmp(Token,"parent")==0)
                {
                    Token=strtok(NULL," \n");
                    if(Token!=NULL)
                    {
                        AnimObject* Child=GetObjectByName(Token);
                            if(Child!=NULL)
                            {
                            Token=strtok(NULL," \n");
                                if(Token!=NULL)
                                {
                                AnimObject* Parent=GetObjectByName(Token);
                                    if(Parent!=NULL)Child->Parent=Parent;
                                }
                            }
                    }
                }
                else if(strcmp(Token,"dat")==0)
                {
                char* Name=strtok(NULL," \n");
                    if(Name!=NULL)
                    {
                    LoadFile(Name);
                    }
                }
                else if(strcmp(Token,"end")==0)
                {
                ParseFrames(NULL);
                }

        Token=strtok(NULL," \n");
        }
}
}

/*
AnimFrame* GetAnimationFrame(int Index)
{
if(Anim==NULL)return NULL;
if(Index>=Anim->NumFrames)return NULL;
return Anim->Frames+Index;
}
AnimFrame* SetAnimationFrame(int Index,AnimFrame* Frame)
{
if(Anim==NULL)return NULL;
if(Index>=Anim->NumFrames)return NULL;
free(Anim->Frames[Index].Commands);
Anim->Frames[Index]=*Frame;
}
char* GetAnimationFrameAsText(int Index)
{
if(Anim==NULL)return NULL;
if(Index>=Anim->NumFrames)return NULL;
return GetTextFromFrame(Anim->Frames+Index);
}
void SetAnimationFrameFromText(int Index,char* Text)
{
if(Anim==NULL)return NULL;
if(Index>=Anim->NumFrames)return NULL;
free(Anim->Frames[Index].Commands);

char* TextCpy=malloc(strlen(Text)+1);
strcpy(TextCpy,Text);
AnimFrame Frame=ParseFrame(TextCpy);
free(TextCpy);
Anim->Frames[Index]=Frame;
}


Image* RenderAnimationFrame(AnimFrame* Frame)
{
if(Frame->Flags&IGNORED)return NULL;
StartRender();
int i;
for(i=0;i<Frame->NumCommands;i++)
{
RenderCommand* Command=Frame->Commands+i;

switch(Command->Command)
{
case SET_ROTATION:
{
Vector* data=Command->data;
Command->Object->Rotation.x+=data[0].x;
Command->Object->Rotation.y+=data[0].y;
Command->Object->Rotation.z+=data[0].z;
}
break;
case SET_POSITION_ROTATION:
{
Vector* data=Command->data;
Command->Object->Position=data[0];
Command->Object->Rotation=data[1];
}
break;
case RENDER:
DrawObject(Command->Object);
break;
}

}
FinishRender();
return GetRenderedImage();
}

void RenderAnimation()
{
int i;
for(i=0;i<Anim->NumFrames;i++)
{
AnimFrame* Frame=GetAnimationFrame(i);
if(Frame==NULL)return;//Either no animation loaded, or out of frames. Either way, abort the render
Image* Img=RenderAnimationFrame(Frame);
if(Img==NULL)continue;//This is an ignored frame
Img->Flags=5;
SetImage(i,Img);
}

}

void FreeAnimation()
{
if(Anim==NULL)return;
int CurObject=0;
while(Anim->Objects[CurObject]!=NULL)FreeObject(Anim->Objects[CurObject]);
int i,j;
for(i=0;i<Anim->NumFrames;i++)
{
for(j=0;j<Anim->Frames[i].NumCommands;j++)
free(Anim->Frames[i].Commands[j].data);
free(Anim->Frames[i].Commands);
}

free(Anim->Frames);
free(Anim);
}
*/
