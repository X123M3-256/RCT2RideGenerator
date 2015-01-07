#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include "renderer.h"
#include "linearalgebra.h"
#define FRAME_BUFFER_SIZE 255


//3 metres per tile
#define SQRT1_2 0.707106781
#define SQRT_3 1.73205080757
#define SQRT_6 2.44948974278
#define S (64.0/(3.0*SQRT_3))
//Dimetric projection
const Matrix projection={{
        SQRT1_2*S  ,       0.0     ,   -SQRT1_2*S  , FRAME_BUFFER_SIZE/2.0,
      0.5*SQRT1_2*S,  SQRT_3/2.0*S , 0.5*SQRT1_2*S , FRAME_BUFFER_SIZE/2.0,
       SQRT_6/4.0*S,   -1.0/2.0*S  ,  SQRT_6/4.0*S ,         0.0          ,
           0.0     ,       0.0     ,       0.0     ,         1.0
    }};

unsigned char frame_buffer[FRAME_BUFFER_SIZE][FRAME_BUFFER_SIZE];
float depth_buffer[FRAME_BUFFER_SIZE][FRAME_BUFFER_SIZE];

image_t* renderer_get_image()
{
image_t* image=malloc(sizeof(image_t));
image->width=FRAME_BUFFER_SIZE;
image->height=FRAME_BUFFER_SIZE;
image->x_offset=-FRAME_BUFFER_SIZE/2;
image->y_offset=-FRAME_BUFFER_SIZE/2;
image->data=malloc(FRAME_BUFFER_SIZE*sizeof(char*));
image->flags=5;
int x,y;
    for(y=0;y<FRAME_BUFFER_SIZE;y++)
    {
    image->data[y]=malloc(FRAME_BUFFER_SIZE);
        for(x=0;x<FRAME_BUFFER_SIZE;x++)image->data[y][x]=frame_buffer[x][y];
    }
return image;
}

/*
Image* ImageFromFrameBuffer()
{
Image* image=malloc(sizeof(Image));
image->Width=FRAME_BUFFER_SIZE;
image->Height=FRAME_BUFFER_SIZE;
image->XOffset=-FRAME_BUFFER_SIZE/2;
image->YOffset=-FRAME_BUFFER_SIZE/2;
image->Data=malloc(FRAME_BUFFER_SIZE*sizeof(char*));
image->Flags=5;
int x,y;
    for(y=0;y<FRAME_BUFFER_SIZE;y++)
    {
    image->Data[y]=malloc(FRAME_BUFFER_SIZE);
        for(x=0;x<FRAME_BUFFER_SIZE;x++)image->Data[y][x]=FrameBuffer[x][y];
    }
return image;
}
*/


void renderer_clear_buffers()
{
int x,y;
    for(x=0;x<FRAME_BUFFER_SIZE;x++)
    for(y=0;y<FRAME_BUFFER_SIZE;y++)
    {
    frame_buffer[x][y]=0;
    depth_buffer[x][y]=-INFINITY;
    }
}


//Fragment shader
char shade_fragment(Vector normal)
{
//printf("%f %f %f\n",normal.X,normal.Y,normal.Z);
const Vector light_direction={sqrt(10.0)/5.0,-sqrt(10.0)/5.0,-sqrt(10.0)/5.0};
float lambert=VectorDotProduct(normal,light_direction);
if(lambert<0.0)lambert=0.0;
return (int)(lambert*8.0)-4;
}


//There's lots of linear interpolation of vectors to do, and this helps limit the amount of code
typedef struct
{
Vector current;
Vector step;
}linear_interp_t;
linear_interp_t linear_interp_init(Vector x1,Vector x2,float u_start,float u_step)
{
linear_interp_t interp;
Vector diff=VectorSubtract(x2,x1);
interp.current=VectorAdd(x1,VectorMultiply(diff,u_start));
interp.step=VectorMultiply(diff,u_step);
return interp;
}
Vector linear_interp_step(linear_interp_t* interp)
{
interp->current=VectorAdd(interp->current,interp->step);
return interp->current;
}

/*Returns the indices of the first and last pixel enclosed int the range start-end.
The value of skip is distance from the start of the range to the centre of the first pixel*/
void get_enclosed_pixels(float start,float end,int* first,int* last,float* skip)
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
float lerp(float x1,float x2,float u)
{
return x1+u*(x2-x1);
}

/*
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
*/
//DWISOTT
void rasterize_primitive(primitive_t* primitive)
{
Vector top_vertex,middle_vertex,bottom_vertex,top_normal,middle_normal,bottom_normal;

//Sort the vertices
//Table of possible permutations. 0 values are not used
const unsigned char permutations[8]={36,0,24,18,33,9,0,6};
unsigned char bits=0;
    //Three comparisons are sufficient to enumerate all 6 permutations
    if(primitive->vertices[0].Y<primitive->vertices[1].Y)bits|=4;
    if(primitive->vertices[1].Y<primitive->vertices[2].Y)bits|=2;
    if(primitive->vertices[0].Y<primitive->vertices[2].Y)bits|=1;
unsigned char permutation=permutations[bits];
//printf("Permutation %d bits %d\n",permutation,bits);
top_vertex=primitive->vertices[(permutation>>4)&3];
top_normal=primitive->normals[(permutation>>4)&3];
middle_vertex=primitive->vertices[(permutation>>2)&3];
middle_normal= primitive->normals[(permutation>>2)&3];
bottom_vertex=primitive->vertices[permutation&3];
bottom_normal= primitive->normals[permutation&3];
//printf("%f %f %f\n",topVertex.Y,middleVertex.Y,bottomVertex.Y);

//Is the longest side on the right?
int longest_side_right=middle_vertex.X<lerp(top_vertex.X,bottom_vertex.X,(middle_vertex.Y-top_vertex.Y)/(bottom_vertex.Y-top_vertex.Y));

//Actually rasterize triangle
int x_start=0,x_end=0;
int y_start,y_end;
float x_skip,y_skip;

//Determine the vertical range of pixels covered by this triangle
get_enclosed_pixels(top_vertex.Y,middle_vertex.Y,&y_start,&y_end,&y_skip);


//Draw top triangle
//Assume longest side is on the left
float u_step=1/(middle_vertex.Y-top_vertex.Y);
linear_interp_t right_position_interp=linear_interp_init(top_vertex,middle_vertex,y_skip*u_step,u_step);
linear_interp_t right_normal_interp=linear_interp_init(top_normal,middle_normal,y_skip*u_step,u_step);

u_step=1/(bottom_vertex.Y-top_vertex.Y);
linear_interp_t left_position_interp=linear_interp_init(top_vertex,bottom_vertex,y_skip*u_step,u_step);
linear_interp_t left_normal_interp=linear_interp_init(top_normal,bottom_normal,y_skip*u_step,u_step);

//Swap if that isn't the case
    if(longest_side_right)
    {
    linear_interp_t temp=left_position_interp;
    left_position_interp=right_position_interp;
    right_position_interp=temp;
    temp=left_normal_interp;
    left_normal_interp=right_normal_interp;
    right_normal_interp=temp;
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
        Vector left_position=left_position_interp.current;
        Vector right_position=right_position_interp.current;
        Vector left_normal=left_normal_interp.current;
        Vector right_normal=right_normal_interp.current;
        //printf("%f %f\n",leftPosition.X,rightPosition);
        //Get pixel range to use
        get_enclosed_pixels(left_position.X,right_position.X,&x_start,&x_end,&x_skip);
        u_step=1/(right_position.X-left_position.X);
        linear_interp_t cur_position_interp=linear_interp_init(left_position,right_position,x_skip*u_step,u_step);
        linear_interp_t cur_normal_interp=linear_interp_init(left_normal,right_normal,x_skip*u_step,u_step);
            for(x=x_start;x<=x_end;x++)
            {
            Vector cur_position=cur_position_interp.current;
            Vector cur_normal=VectorNormalize(cur_normal_interp.current);
                if(cur_position.Z>depth_buffer[x][y])
                {
                frame_buffer[x][y]=primitive->color+shade_fragment(cur_normal);
                depth_buffer[x][y]=cur_position.Z;
                }
            linear_interp_step(&cur_position_interp);
            linear_interp_step(&cur_normal_interp);
            }
        linear_interp_step(&left_position_interp);
        linear_interp_step(&right_position_interp);
        linear_interp_step(&left_normal_interp);
        linear_interp_step(&right_normal_interp);
        }
    //Compute new values of y_start and y_end
    get_enclosed_pixels(middle_vertex.Y,bottom_vertex.Y,&y_start,&y_end,&y_skip);
    //Compute new interpolation for the side that has changed
    u_step=1/(bottom_vertex.Y-middle_vertex.Y);
        if(longest_side_right)
        {
        left_position_interp=linear_interp_init(middle_vertex,bottom_vertex,y_skip*u_step,u_step);
        left_normal_interp=linear_interp_init(middle_normal,bottom_normal,y_skip*u_step,u_step);
        }
        else
        {
        right_position_interp=linear_interp_init(middle_vertex,bottom_vertex,y_skip*u_step,u_step);
        right_normal_interp=linear_interp_init(middle_normal,bottom_normal,y_skip*u_step,u_step);
        }
    }
}
void transform_vectors(Matrix transform,Vector* source,Vector* dest,unsigned int num,float w)
{
int i;
    for(i=0;i<num;i++)
    {
    dest[i].X=source[i].X*transform.Data[0]+source[i].Y*transform.Data[1]+source[i].Z*transform.Data[2]+w*transform.Data[3];
    dest[i].Y=source[i].X*transform.Data[4]+source[i].Y*transform.Data[5]+source[i].Z*transform.Data[6]+w*transform.Data[7];
    dest[i].Z=source[i].X*transform.Data[8]+source[i].Y*transform.Data[9]+source[i].Z*transform.Data[10]+w*transform.Data[11];
    //printf("%f %f %f\n",dest[i].X,dest[i].Y,dest[i].Z);
    }
}

void renderer_render_model(model_t* model,Matrix model_view)
{
model_view=MatrixMultiply(model_view,model->transform);
Matrix model_view_projection=MatrixMultiply(projection,model_view);
//Allocate space for transformed vectors
Vector* transformed_vertices=malloc(model->num_vertices*sizeof(Vector));
Vector* transformed_normals=malloc(model->num_normals*sizeof(Vector));
//Transform model into screen space
transform_vectors(model_view_projection,model->vertices,transformed_vertices,model->num_vertices,1.0);
//Transform normals into world space (inverse transpose is broken but not actually needed)
//Matrix normalTransform=MatrixTranspose(MatrixInverse(modelView));
transform_vectors(model_view,model->normals,transformed_normals,model->num_normals,0.0);

//Rasterize primitives TODO: Backface culling
primitive_t primitive;
int i,j;
    for(i=0;i<model->num_faces;i++)
    {
    switch(model->faces[i].flags)
        {
        case RECOLOR_GREEN:
        primitive.color=250;
        break;
        case RECOLOR_MAGENTA:
        primitive.color=209;
        break;
        default:
        primitive.color=model->faces[i].color;
        break;
        }
        for(j=0;j<3;j++)
        {
        primitive.vertices[j]=transformed_vertices[model->faces[i].vertices[j]];
        primitive.normals[j]=transformed_normals[model->faces[i].normals[j]];
        }
    rasterize_primitive(&primitive);
    }
    /*
    for(i=0;i<model->NumLines;i++)
    {
    primitive.Color=model->Lines[i].Color;
    primitive.Vertices[0]=transformedVertices[model->Lines[i].Vertices[0]];
    primitive.Vertices[1]=transformedVertices[model->Lines[i].Vertices[1]];
    RasterizeLine(&primitive);
    }
    */
free(transformed_vertices);
free(transformed_normals);
}

/*
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

*/
