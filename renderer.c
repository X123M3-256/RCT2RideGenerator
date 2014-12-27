#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include "renderer.h"
#include "linearalgebra.h"
#define FRAME_BUFFER_SIZE 256


//3 metres per tile
#define S (64.0/(3.0*sqrt(3.0)))
//Dimetric projection
const Matrix projection={
      M_SQRT1_2*S  ,       0.0      ,   -M_SQRT1_2*S  , FRAME_BUFFER_SIZE/2.0,
    0.5*M_SQRT1_2*S, sqrt(3.0)/2.0*S,  0.5*M_SQRT1_2*S, FRAME_BUFFER_SIZE/2.0,
    sqrt(6.0)/4.0*S,   -1.0/2.0*S   ,  sqrt(6.0)/4.0*S,          0.0         ,
           0.0     ,       0.0      ,        0.0      ,          1.0
    };

unsigned char FrameBuffer[FRAME_BUFFER_SIZE][FRAME_BUFFER_SIZE];
float DepthBuffer[FRAME_BUFFER_SIZE][FRAME_BUFFER_SIZE];




Image ImageFromFrameBuffer()
{
Image image;
image.Width=FRAME_BUFFER_SIZE;
image.Height=FRAME_BUFFER_SIZE;
image.XOffset=-FRAME_BUFFER_SIZE/2;
image.YOffset=-FRAME_BUFFER_SIZE/2;
image.Data=malloc(FRAME_BUFFER_SIZE*sizeof(char*));
image.Flags=5;
int x,y;
    for(y=0;y<FRAME_BUFFER_SIZE;y++)
    {
    image.Data[y]=malloc(FRAME_BUFFER_SIZE);
        for(x=0;x<FRAME_BUFFER_SIZE;x++)image.Data[y][x]=FrameBuffer[x][y];
    }
return image;
}


void ClearBuffers()
{
int x,y;
    for(x=0;x<FRAME_BUFFER_SIZE;x++)
    for(y=0;y<FRAME_BUFFER_SIZE;y++)
    {
    FrameBuffer[x][y]=0;
    DepthBuffer[x][y]=-INFINITY;
    }
}

//Fragment shader
char ShadeFragment(Vector normal)
{
//printf("%f %f %f\n",normal.X,normal.Y,normal.Z);
const Vector lightDirection={sqrt(10.0)/5.0,-sqrt(10.0)/5.0,-sqrt(10.0)/5.0};
float lambert=VectorDotProduct(normal,lightDirection);
if(lambert<0.0)lambert=0.0;
return (int)(lambert*8.0)-4;
}


//There's lots of linear interpolation of vectors to do, and this helps limit the amount of code
typedef struct
{
Vector current;
Vector step;
}LinearInterp;
inline LinearInterp LinearInterpInit(Vector x1,Vector x2,float u_start,float u_step)
{
LinearInterp interp;
Vector diff=VectorSubtract(x2,x1);
interp.current=VectorAdd(x1,VectorMultiply(diff,u_start));
interp.step=VectorMultiply(diff,u_step);
return interp;
}
inline Vector LinearInterpStep(LinearInterp* interp)
{
interp->current=VectorAdd(interp->current,interp->step);
return interp->current;
}

/*Returns the indices of the first and last pixel enclosed int the range start-end.
The value of skip is distance from the start of the range to the centre of the first pixel*/
void GetEnclosedPixels(float start,float end,int* first,int* last,float* skip)
{
*first=(int)floor(start+0.5);
*last=(int)floor(end-0.5);
*skip=*first-start+0.5;
//Ignore pixels lying outside the frame buffer
    if(*first<0)
    {
    (*skip)+=-(*first);
    *first=0;
    }
    if(*last>=FRAME_BUFFER_SIZE)*last=FRAME_BUFFER_SIZE-1;
}

inline float lerp(float x1,float x2,float u)
{
return x1+u*(x2-x1);
}


//Rasterizes a line from the first two vertices in the supplied primitive; others are ignored
#define ABS(X) ((X)>0?(X):-(X))
void RasterizeLine(Primitive* primitive)
{
Vector firstVertex,lastVertex;


float dx=ABS(primitive->Vertices[0].X-primitive->Vertices[1].X);
float dy=ABS(primitive->Vertices[0].Y-primitive->Vertices[1].Y);

int steep=dy>dx;
//If not steep, we step over x coordinate, otherwise, step over y. Either way, the first vertex must have the smaller coord

    if((steep&&primitive->Vertices[0].Y<primitive->Vertices[1].Y)||(!steep&&primitive->Vertices[0].X<primitive->Vertices[1].X))
    {
    firstVertex=primitive->Vertices[0];
    lastVertex=primitive->Vertices[1];
    }
    else
    {
    firstVertex=primitive->Vertices[1];
    lastVertex=primitive->Vertices[0];
    }

int start,end;
float skip;
float u_step;
//Step over y if steep, else x
    if(steep)
    {
    GetEnclosedPixels(firstVertex.Y,lastVertex.Y,&start,&end,&skip);
    u_step=1.0/dy;
    }
    else
    {
    GetEnclosedPixels(firstVertex.X,lastVertex.X,&start,&end,&skip);
    u_step=1.0/dx;
    }

LinearInterp positionInterp=LinearInterpInit(firstVertex,lastVertex,skip*u_step,u_step);

int i;
    for(i=start;i<=end;i++)
    {
    Vector position=positionInterp.current;
    int x,y;
        if(steep)
        {
        x=(int)position.X,
        y=i;
        }
        else
        {
        x=i;
        y=(int)position.Y;
        }
        if(x>=0&&x<FRAME_BUFFER_SIZE&&y>=0&&y<FRAME_BUFFER_SIZE&&position.Z>DepthBuffer[x][y])
        {
        FrameBuffer[x][y]=primitive->Color;
        DepthBuffer[x][y]=position.Z;
        }
    LinearInterpStep(&positionInterp);
    }
}
//DWISOTT
void RasterizePrimitive(Primitive* primitive)
{
Vector topVertex,middleVertex,bottomVertex,topNormal,middleNormal,bottomNormal;

//Sort the vertices
//Table of possible permutations. 0 values are not used
const unsigned char permutations[8]={36,0,24,18,33,9,0,6};
unsigned char bits=0;
    //Three comparisons are sufficient to enumerate all 6 permutations
    if(primitive->Vertices[0].Y<primitive->Vertices[1].Y)bits|=4;
    if(primitive->Vertices[1].Y<primitive->Vertices[2].Y)bits|=2;
    if(primitive->Vertices[0].Y<primitive->Vertices[2].Y)bits|=1;
unsigned char permutation=permutations[bits];
//printf("Permutation %d bits %d\n",permutation,bits);
topVertex=primitive->Vertices[(permutation>>4)&3];
topNormal=primitive->Normals[(permutation>>4)&3];
middleVertex=primitive->Vertices[(permutation>>2)&3];
middleNormal= primitive->Normals[(permutation>>2)&3];
bottomVertex=primitive->Vertices[permutation&3];
bottomNormal= primitive->Normals[permutation&3];
//printf("%f %f %f\n",topVertex.Y,middleVertex.Y,bottomVertex.Y);


int longest_side_right=middleVertex.X<lerp(topVertex.X,bottomVertex.X,(middleVertex.Y-topVertex.Y)/(bottomVertex.Y-topVertex.Y));
//Actually rasterize triangle

int x_start=0,x_end=0;
int y_start,y_end;
float x_skip,y_skip;

GetEnclosedPixels(topVertex.Y,middleVertex.Y,&y_start,&y_end,&y_skip);


//Draw top triangle
//Assume longest side is on the left
float u_step=1/(middleVertex.Y-topVertex.Y);
LinearInterp rightPositionInterp=LinearInterpInit(topVertex,middleVertex,y_skip*u_step,u_step);
LinearInterp rightNormalInterp=LinearInterpInit(topNormal,middleNormal,y_skip*u_step,u_step);

u_step=1/(bottomVertex.Y-topVertex.Y);
LinearInterp leftPositionInterp=LinearInterpInit(topVertex,bottomVertex,y_skip*u_step,u_step);
LinearInterp leftNormalInterp=LinearInterpInit(topNormal,bottomNormal,y_skip*u_step,u_step);



//Swap if that isn't the case
    if(longest_side_right)
    {
    LinearInterp temp=leftPositionInterp;
    leftPositionInterp=rightPositionInterp;
    rightPositionInterp=temp;
    temp=leftNormalInterp;
    leftNormalInterp=rightNormalInterp;
    rightNormalInterp=temp;
    }
//Rasterize the primitive
int i;
//Loop twice: One to draw the top half and then again for the bottom half
for(i=0;i<2;i++)
{
int x,y;
    for(y=y_start;y<=y_end;y++)
    {
    //Interpolate values
    Vector leftPosition=leftPositionInterp.current;
    Vector rightPosition=rightPositionInterp.current;
    Vector leftNormal=leftNormalInterp.current;
    Vector rightNormal=rightNormalInterp.current;
    //printf("%f %f\n",leftPosition.X,rightPosition);
    //Get pixel range to use
    GetEnclosedPixels(leftPosition.X,rightPosition.X,&x_start,&x_end,&x_skip);
    u_step=1/(rightPosition.X-leftPosition.X);
    LinearInterp curPositionInterp=LinearInterpInit(leftPosition,rightPosition,x_skip*u_step,u_step);
    LinearInterp curNormalInterp=LinearInterpInit(leftNormal,rightNormal,x_skip*u_step,u_step);
        for(x=x_start;x<=x_end;x++)
        {
        Vector curPosition=curPositionInterp.current;
        Vector curNormal=VectorNormalize(curNormalInterp.current);
            if(curPosition.Z>DepthBuffer[x][y])
            {
            FrameBuffer[x][y]=primitive->Color+ShadeFragment(curNormal);
            DepthBuffer[x][y]=curPosition.Z;
            }
        LinearInterpStep(&curPositionInterp);
        LinearInterpStep(&curNormalInterp);
        }
    LinearInterpStep(&leftPositionInterp);
    LinearInterpStep(&rightPositionInterp);
    LinearInterpStep(&leftNormalInterp);
    LinearInterpStep(&rightNormalInterp);
    }
//Compute new values of y_start and y_end
GetEnclosedPixels(middleVertex.Y,bottomVertex.Y,&y_start,&y_end,&y_skip);
//Compute new interpolation for the side that has changed
u_step=1/(bottomVertex.Y-middleVertex.Y);
    if(longest_side_right)
    {
    leftPositionInterp=LinearInterpInit(middleVertex,bottomVertex,y_skip*u_step,u_step);
    leftNormalInterp=LinearInterpInit(middleNormal,bottomNormal,y_skip*u_step,u_step);
    }
    else
    {
    rightPositionInterp=LinearInterpInit(middleVertex,bottomVertex,y_skip*u_step,u_step);
    rightNormalInterp=LinearInterpInit(middleNormal,bottomNormal,y_skip*u_step,u_step);
    }
}

}
void TransformVectors(Matrix transform,Vector* source,Vector* dest,unsigned int num,float W)
{
int i;
    for(i=0;i<num;i++)
    {
    dest[i].X=source[i].X*transform.Data[0]+source[i].Y*transform.Data[1]+source[i].Z*transform.Data[2]+W*transform.Data[3];
    dest[i].Y=source[i].X*transform.Data[4]+source[i].Y*transform.Data[5]+source[i].Z*transform.Data[6]+W*transform.Data[7];
    dest[i].Z=source[i].X*transform.Data[8]+source[i].Y*transform.Data[9]+source[i].Z*transform.Data[10]+W*transform.Data[11];
    //printf("%f %f %f\n",dest[i].X,dest[i].Y,dest[i].Z);
    }
}
void RenderModel(Model* model,Matrix modelView)
{
modelView=MatrixMultiply(modelView,model->transform);
Matrix modelViewProjection=MatrixMultiply(projection,modelView);

Vector* transformedVertices=malloc(model->NumVertices*sizeof(Vector));
Vector* transformedNormals=malloc(model->NumNormals*sizeof(Vector));
//Transform model into screen space
TransformVectors(modelViewProjection,model->Vertices,transformedVertices,model->NumVertices,1.0);
//Transform normals into world space (inverse transpose is broken but not actually needed)
//Matrix normalTransform=MatrixTranspose(MatrixInverse(modelView));
TransformVectors(modelView,model->Normals,transformedNormals,model->NumNormals,0.0);
//Rasterize primitives TODO: Backface culling
Primitive primitive;
int i,j;
    for(i=0;i<model->NumFaces;i++)
    {
    primitive.Color=model->Faces[i].Color;
        for(j=0;j<3;j++)
        {
                switch(model->Faces[i].Flags)
            {
            case RECOLOR_GREEN:
            primitive.Color=250;
            break;
            case RECOLOR_MAGENTA:
            primitive.Color=209;
            break;
            default:
            primitive.Color=model->Faces[i].Color;
            break;
            }
        primitive.Vertices[j]=transformedVertices[model->Faces[i].Vertices[j]];
        primitive.Normals[j]=transformedNormals[model->Faces[i].Normals[j]];
        }
    RasterizePrimitive(&primitive);
    }
    for(i=0;i<model->NumLines;i++)
    {

    primitive.Color=model->Lines[i].Color;
    primitive.Vertices[0]=transformedVertices[model->Lines[i].Vertices[0]];
    primitive.Vertices[1]=transformedVertices[model->Lines[i].Vertices[1]];
    RasterizeLine(&primitive);
    }
free(transformedVertices);
free(transformedNormals);
}


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
//Buffer to read lines into
char line[256];

FILE* file=fopen(filename,"r");

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
return model;
}


#define MIN(X,Y) ((X)<(Y)?(X):(Y))
#define MAX(X,Y) ((X)>(Y)?(X):(Y))
int IsInTriangle(Vector point,Vector t1,Vector t2,Vector t3,float* depth)
{
float denominator=((t2.Y-t3.Y)*(t1.X-t3.X)+(t3.X-t2.X)*(t1.Y-t3.Y));
float a=((t2.Y-t3.Y)*(point.X-t3.X)+(t3.X-t2.X)*(point.Y-t3.Y))/denominator;
float b=((t3.Y-t1.Y)*(point.X-t3.X)+(t1.X-t3.X)*(point.Y-t3.Y))/denominator;
float c=1-a-b;
    if(a>=0&&a<=1&&b>=0&&b<=1&&c>=0&&c<=1)
    {
    *depth=a*t1.Z+b*t2.Z+c*t3.Z;
    return 1;
    }
return 0;
}
Face* GetFaceEnclosingPoint(Model* model,Matrix modelView,Vector coords)
{
//Transform model into screen space, reducing the test for intersection to a 2D problem
Vector* transformedVertices=malloc(model->NumVertices*sizeof(Vector));
modelView=MatrixMultiply(modelView,model->transform);
Matrix modelViewProjection=MatrixMultiply(projection,modelView);
TransformVectors(modelViewProjection,model->Vertices,transformedVertices,model->NumVertices,1.0);

//Abbreviate variable name because it's used a lot
#define TV transformedVertices

float largestDepth=-INFINITY;
Face* nearestFace=NULL;
int i;
    for(i=0;i<model->NumFaces;i++)
    {
    float depth;
        if(IsInTriangle(coords,TV[model->Faces[i].Vertices[0]],TV[model->Faces[i].Vertices[1]],TV[model->Faces[i].Vertices[2]],&depth)&&depth>largestDepth)
        {
        largestDepth=depth;
        nearestFace=model->Faces+i;
        }
    }
free(transformedVertices);
return nearestFace;
}


