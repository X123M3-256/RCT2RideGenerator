#ifndef DATASTRUCTURES_H_INCLUDED
#define DATASTRUCTURES_H_INCLUDED
//Linked list
typedef struct LL
{
void* Data;
struct LL* Next;
}LList;

//A buffer that dynamically expands when data is written past the end
typedef struct
{
int Step;//Bytes by which buffer increases
int Size;//Current size of buffer
int Pos;//Current position in buffer
unsigned char* Buffer;//The buffer
}DynamicBuffer;


LList* ListAdd(LList* List,void* Item);
void* GetIndex(LList* List,int i);
void FreeList(LList* List);

DynamicBuffer* CreateBuffer(int Step);
void WriteBuffer(unsigned char* Source,int Num,DynamicBuffer* Buffer);
void ExpandBuffer(int Size,DynamicBuffer* Buffer);
unsigned char* FreeBuffer(DynamicBuffer* Buffer);

#endif // DATASTRUCTURES_H_INCLUDED
