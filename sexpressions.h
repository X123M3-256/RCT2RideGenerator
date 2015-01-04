#ifndef SEXPRESSIONS_H_INCLUDED
#define SEXPRESSIONS_H_INCLUDED


#define LIST 0
#define INTEGER 1
#define FLOAT 2
#define STRING 3
#define SYMBOL 4

typedef struct
{
void* first;
void* rest;
unsigned int type;
}List;


void TestSerialization();

List* Cons(List* first,List* rest);


void SExpressionFree();
#endif
