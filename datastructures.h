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
char* Buffer;//The buffer
}DynamicBuffer;


LList* ListAdd(LList* List,void* Item);
void* GetIndex(LList* List,int i);
void FreeList(LList* List);

DynamicBuffer* CreateBuffer(int Step);
void WriteBuffer(char* Source,int Num,DynamicBuffer* Buffer);
char* FreeBuffer(DynamicBuffer* Buffer);

#endif // DATASTRUCTURES_H_INCLUDED
