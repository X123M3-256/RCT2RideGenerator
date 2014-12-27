#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "model.h"


Vector ParseObjVertex()
{
Vector result;
char* token;
token=strtok(NULL," ");
result.X=strtof(token,NULL);
token=strtok(NULL," ");
result.Y=strtof(token,NULL);
token=strtok(NULL," ");
result.Z=strtof(token,NULL);
return result;
}
Line ParseObjLine(unsigned int curVertex)
{
Line line;
line.Color=69;
int i;
    for(i=0;i<2;i++)
    {
    char* indices=strtok(NULL," ");
    line.Vertices[i]=strtol(indices,NULL,10)-1;
    }
return line;
}
Face ParseObjFace(unsigned int curVertex,unsigned int curNormal)
{
Face face;
//Initialize these with fixed values for now
face.Flags=0;
face.Color=16;

int i;
    for(i=0;i<3;i++)
    {
    char* indices=strtok(NULL," ");
    face.Vertices[i]=strtol(indices,&indices,10)-1;
    //Skip the slash
    indices++;
    //If the next char is not also a slash, there is a tex coord, which we ignore
        if(*indices!='/')strtol(indices,&indices,10);
    indices++;
    face.Normals[i]=strtol(indices,&indices,10)-1;
    face.Flags=RECOLOR_GREEN;
    }
return face;
}
Model* LoadObj(const char* filename)
{
//Check there is space to store a new model
    if(ModelList.NumModels>=MAX_MODELS)return NULL;

//Buffer to read lines into
char line[256];

FILE* file=fopen(filename,"r");
    if(file==NULL)return NULL;
//In the first pass over the file, count the numbers of elements that will need to be allocated
unsigned int numVertices=0;
unsigned int numNormals=0;
unsigned int numFaces=0;
unsigned int numLines=0;
    while(!feof(file))
    {
    //Read a line from the file
    if(fgets(line,256,file)==NULL)break;

    //Get the first token in the line
    const char* type=strtok(line," ");

        if(strcmp("v",type)==0)numVertices++;
        else if(strcmp("vn",type)==0)numNormals++;
        else if(strcmp("f",type)==0)numFaces++;
        else if(strcmp("l",type)==0)numLines++;
    }
//Allocate model
Model* model=malloc(sizeof(Model));
model->Name=malloc(8);
snprintf(model->Name,7,"model%d",ModelList.NumModels);
model->transform=MatrixIdentity();
model->NumVertices=numVertices;
model->NumNormals=numNormals;
model->NumFaces=numFaces;
model->NumLines=numLines;
model->Vertices=malloc(numVertices*sizeof(Vector));
model->Normals=malloc(numNormals*sizeof(Vector));
model->Faces=malloc(numFaces*sizeof(Face));
model->Lines=malloc(numLines*sizeof(Line));
//Need to keep track of indices
int curVertex=0;
int curNormal=0;
int curFace=0;
int curLine=0;

//Seek back to start of file
fseek(file,SEEK_SET,0);
//Read file into model
    while(!feof(file))
    {
    //Read a line from the file
    if(fgets(line,256,file)==NULL)break;
    //Get the first token in the line
    const char* type=strtok(line," ");
        if(strcmp("v",type)==0)model->Vertices[curVertex++]=ParseObjVertex();
        else if(strcmp("vn",type)==0)model->Normals[curNormal++]=ParseObjVertex();
        else if(strcmp("f",type)==0)model->Faces[curFace++]=ParseObjFace(curVertex,curNormal);
        else if(strcmp("l",type)==0)model->Lines[curLine++]=ParseObjLine(curVertex);
    }

//Add model to list
ModelList.Models[ModelList.NumModels++]=model;
return model;
}


int NumModels()
{
return ModelList.NumModels;
}
Model* GetModelByIndex(int index)
{
    if(index>=ModelList.NumModels)return NULL;
return ModelList.Models[index];
}
