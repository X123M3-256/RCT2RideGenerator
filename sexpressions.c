#include <stdio.h>
#include <stdlib.h>
#include "renderer.h"
#include "sexpressions.h"


List* AddSymbol(List* list,char* symbol)
{
//Create new node containing symbol
List* exp=malloc(sizeof(List));
exp->type=SYMBOL;
char* str=malloc(strlen(symbol)+1);
strcpy(str,symbol);
exp->first=str;
exp->rest=list;
return exp;
}

List* AddInteger(List* list,int integer)
{
//Create new node containing integer
List* exp=malloc(sizeof(List));
exp->type=INTEGER;
exp->first=malloc(sizeof(int));
(*(int*)(exp->first))=integer;
exp->rest=list;
return exp;
}

List* AddFloat(List* list,float value)
{
//Create new node containing integer
List* exp=malloc(sizeof(List));
exp->type=FLOAT;
exp->first=malloc(sizeof(float));
(*(float*)(exp->first))=value;
exp->rest=list;
return exp;
}

List* AddList(List* list,List* newlist)
{
List* exp=malloc(sizeof(List));
exp->type=LIST;
exp->first=newlist;
exp->rest=list;
return exp;
}

#define NONE 0
#define START 1
#define SYMBOL 2
#define INTEGER 3
#define FLOAT 4
#define FLOAT_WITH_EXPONENT 5
#define OPEN_BRACKET 6
#define CLOSE_BRACKET 7
#define QUOTATION_PROCESSED 8
#define BACKSLASH_SYMBOL 9
#define BACKSLASH_STRING 10

typedef struct FileReader
{
char buffer[256];
};
/*
void Lexer()
{
static int last_accepting_state=NONE;
static int state=START;
    switch(state)
    {
    case START:
        if(chr>='0'&&chr<='9')state=INTEGER;
        else if(chr=='"')state=QUOTATION_PROCESSED;
        else if(chr=='(')state=OPEN_BRACKET;
        else if(chr==')')state=CLOSE_BRACKET;
        else if(chr!=\)
    break;
    }
}
*/
List* ReadList(FILE* stream)
{
char buffer[256];

    while(!feof(stream))
    {
    int read=fread(buffer,1,256,stream);
    int i;
        for(i=0;i<read;i++)Lexer(buffer[i]);
    }
return NULL;
}

void WriteList(FILE* stream,List* list,int indent)
{
fputc('\n',stream);
int i;
    for(i=0;i<indent;i++)fputc('\t',stream);
fputc('(',stream);
    do
    {
        switch(list->type)
        {
        case LIST:
        WriteList(stream,list->first,indent+1);
        break;
        case SYMBOL:
        fputs((char*)list->first,stream);
        break;
        case FLOAT:
        fprintf(stream,"%f",(*(float*)list->first));
        break;
        case INTEGER:
        fprintf(stream,"%d",(*(int*)list->first));
        break;
        }
    }
    while((list=list->rest)!=NULL&&fputc(' ',stream));
fputc(')',stream);
}



List* SerializeModel(Model* model)
{
int i;
List* modelList=NULL;

//Serialize faces
List* faceList=NULL;
    for(i=model->NumFaces-1;i>=0;i--)
    {
    Face* face=model->Faces+i;
    faceList=AddList(faceList,AddInteger(AddInteger(AddList(AddList(NULL,AddInteger(AddInteger(AddInteger(NULL,face->Normals[2]),face->Normals[1]),face->Normals[0])),AddInteger(AddInteger(AddInteger(NULL,face->Vertices[2]),face->Vertices[1]),face->Vertices[0])),face->Color),face->Flags));
    }

faceList=AddSymbol(faceList,"faces");
modelList=AddList(modelList,faceList);

//Serialize normals
List* normalList=NULL;
    for(i=model->NumNormals-1;i>=0;i--)
    {
    normalList=AddList(normalList,AddFloat(AddFloat(AddFloat(NULL,model->Normals[i].Z),model->Normals[i].Y),model->Normals[i].X));
    }
normalList=AddSymbol(normalList,"normals");
modelList=AddList(modelList,normalList);

//Serialize vertices
List* vertexList=NULL;
    for(i=model->NumVertices-1;i>=0;i--)
    {
    vertexList=AddList(vertexList,AddFloat(AddFloat(AddFloat(NULL,model->Vertices[i].Z),model->Vertices[i].Y),model->Vertices[i].X));
    }
vertexList=AddSymbol(vertexList,"vertices");
modelList=AddList(modelList,vertexList);



modelList=AddSymbol(modelList,"model");
return modelList;
}

void TestSerialization()
{

FILE* file=fopen("testmodel.txt","r");
List* test=ReadList(file);
fclose(file);
}

