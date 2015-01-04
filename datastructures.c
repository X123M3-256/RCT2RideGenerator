#include <stdlib.h>
#include "datastructures.h"
LList* ListAdd(LList* List,void* Item)
{
if(List==NULL)
{
List=malloc(sizeof(LList));
List->Data=Item;
List->Next=NULL;
}
else List->Next=ListAdd(List->Next,Item);
return List;
}
void* GetIndex(LList* List,int i)
{
if(List==NULL)return NULL;
else if(i==0)return List->Data;
else return GetIndex(List->Next,i-1);
}
void FreeList(LList* List)
{
free(List->Data);
if(List->Next!=NULL)FreeList(List->Next);
free(List);
}


DynamicBuffer* CreateBuffer(int Step)
{
DynamicBuffer* Buffer=malloc(sizeof(DynamicBuffer));
Buffer->Step=Step;
Buffer->Size=Step;
Buffer->Pos=0;
Buffer->Buffer=malloc(Step);
return Buffer;
};
//Writes bytes into buffer
void WriteBuffer(unsigned char* Source,int Num,DynamicBuffer* Buffer)
{
while(Buffer->Pos+Num>=Buffer->Size)
    {
    Buffer->Size+=Buffer->Step;
    Buffer->Buffer=realloc(Buffer->Buffer,Buffer->Size);
    }
memcpy(Buffer->Buffer+Buffer->Pos,Source,Num);
Buffer->Pos+=Num;
}
//Extends
void ExpandBuffer(int Size,DynamicBuffer* Buffer)
{
while(Buffer->Pos+Size>=Buffer->Size)
    {
    Buffer->Size+=Buffer->Step;
    Buffer->Buffer=realloc(Buffer->Buffer,Buffer->Size);
    }
memset(Buffer->Buffer+Buffer->Pos,0,Size);
Buffer->Pos+=Size;
}
//Frees the buffer and returns the underlying char array, resized to the right size
unsigned char* FreeBuffer(DynamicBuffer* Buffer)
{
unsigned char* Bytes=realloc(Buffer->Buffer,Buffer->Pos);
free(Buffer);
return Bytes;
}
;;
