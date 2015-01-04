#include<string.h>
#include <assert.h>
#include "serialization.h"

json_t* SerializeMatrix(Matrix matrix)
{
int i,j;
json_t* root=json_array();
    for(i=0;i<4;i++)
    {
    json_t* row=json_array();
            for(j=0;j<4;j++)
            {
            json_t* value=json_real(matrix.Data[j+(i<<2)]);
            json_array_append_new(row,value);
            }
    json_array_append_new(root,row);
    }
return root;
}
Matrix DeserializeMatrix(json_t* json)
{
int i,j;
Matrix matrix;
assert(json_array_size(json)==4);
    for(i=0;i<4;i++)
    {
    json_t* row=json_array_get(json,i);
    assert(json_array_size(row)==4);
            for(j=0;j<4;j++)
            {
            matrix.Data[j+(i<<2)]=json_real_value(json_array_get(row,j));
            }
    }
return matrix;
}

json_t* SerializeVector(Vector vector)
{
json_t* components=json_array();
json_array_append_new(components,json_real(vector.X));
json_array_append_new(components,json_real(vector.Y));
json_array_append_new(components,json_real(vector.Z));
return components;
}
Vector DeserializeVector(json_t* components)
{
Vector vector;
vector.X=json_real_value(json_array_get(components,0));
vector.Y=json_real_value(json_array_get(components,1));
vector.Z=json_real_value(json_array_get(components,2));
return vector;
}

json_t* SerializeModel(Model* model)
{
int i;

json_t* root=json_object();

json_object_set_new(root,"name",json_string(model->Name));

json_t* vertices=json_array();
    for(i=0;i<model->NumVertices;i++)
    {
    json_t* vertex=json_array();
    json_array_append_new(vertex,json_real(model->Vertices[i].X));
    json_array_append_new(vertex,json_real(model->Vertices[i].Y));
    json_array_append_new(vertex,json_real(model->Vertices[i].Z));
    json_array_append_new(vertices,vertex);
    }
json_object_set_new(root,"vertices",vertices);

json_t* normals=json_array();
    for(i=0;i<model->NumNormals;i++)
    {
    json_t* normal=json_array();
    json_array_append_new(normal,json_real(model->Normals[i].X));
    json_array_append_new(normal,json_real(model->Normals[i].Y));
    json_array_append_new(normal,json_real(model->Normals[i].Z));
    json_array_append_new(normals,normal);
    }
json_object_set_new(root,"normals",normals);
json_t* faces=json_array();
    for(i=0;i<model->NumFaces;i++)
    {
    json_t* face=json_array();
    json_array_append_new(face,json_integer(model->Faces[i].Color));
    json_array_append_new(face,json_integer(model->Faces[i].Flags));

    json_t* vert_indices=json_array();
    json_array_append_new(vert_indices,json_integer(model->Faces[i].Vertices[0]));
    json_array_append_new(vert_indices,json_integer(model->Faces[i].Vertices[1]));
    json_array_append_new(vert_indices,json_integer(model->Faces[i].Vertices[2]));
    json_array_append_new(face,vert_indices);

    json_t* norm_indices=json_array();
    json_array_append_new(norm_indices,json_integer(model->Faces[i].Normals[0]));
    json_array_append_new(norm_indices,json_integer(model->Faces[i].Normals[1]));
    json_array_append_new(norm_indices,json_integer(model->Faces[i].Normals[2]));
    json_array_append_new(face,norm_indices);

    json_array_append_new(faces,face);
    }
json_object_set_new(root,"faces",faces);

json_object_set_new(root,"transform",SerializeMatrix(model->transform));
return root;
}
Model* DeserializeModel(json_t* json)
{
int i,j;
json_t* name=json_object_get(json,"name");
json_t* vertices=json_object_get(json,"vertices");
json_t* normals=json_object_get(json,"normals");
json_t* faces=json_object_get(json,"faces");
json_t* transform=json_object_get(json,"transform");
assert(name!=NULL&&vertices!=NULL&&normals!=NULL&&faces!=NULL&&transform!=NULL);
//Allocate model
Model* model=malloc(sizeof(Model));
model->Name=strdup(json_string_value(name));
model->NumVertices=json_array_size(vertices);
model->NumNormals=json_array_size(normals);
model->NumFaces=json_array_size(faces);
model->Vertices=malloc(model->NumVertices*sizeof(Vector));
model->Normals=malloc(model->NumNormals*sizeof(Vector));
model->Faces=malloc(model->NumFaces*sizeof(Face));
model->transform=DeserializeMatrix(transform);
model->NumLines=0;
model->Lines=NULL;
    for(i=0;i<model->NumVertices;i++)
    {
    json_t* vertex=json_array_get(vertices,i);
    model->Vertices[i].X=json_real_value(json_array_get(vertex,0));
    model->Vertices[i].Y=json_real_value(json_array_get(vertex,1));
    model->Vertices[i].Z=json_real_value(json_array_get(vertex,2));
    }

    for(i=0;i<model->NumNormals;i++)
    {
    json_t* normal=json_array_get(normals,i);
    model->Normals[i].X=json_real_value(json_array_get(normal,0));
    model->Normals[i].Y=json_real_value(json_array_get(normal,1));
    model->Normals[i].Z=json_real_value(json_array_get(normal,2));
    }

    for(i=0;i<model->NumFaces;i++)
    {
    json_t* face=json_array_get(faces,i);
    model->Faces[i].Color=json_integer_value(json_array_get(face,0));
    model->Faces[i].Flags=json_integer_value(json_array_get(face,1));
    json_t* vertices=json_array_get(face,2);
    json_t* normals=json_array_get(face,3);
        for(j=0;j<3;j++)
        {
        model->Faces[i].Vertices[j]=json_integer_value(json_array_get(vertices,j));
        model->Faces[i].Normals[j]=json_integer_value(json_array_get(normals,j));
        }
    }
return model;
}

json_t* SerializeAnimation(Animation* animation)
{
int i,j;
json_t* root=json_object();
//Serialize name
json_t* name=json_string(animation->Name);
json_object_set(root,"name",name);


//Serialize model list
json_t* objects=json_array();
    for(i=0;i<animation->NumObjects;i++)
    {
    json_t* objectData=json_array();
    int index=GetModelIndexFromPointer(animation->Objects[i].model);
    json_array_append_new(objectData,json_integer(index));
    json_array_append_new(objectData,json_integer(animation->Objects[i].parentIndex));
    json_array_append_new(objects,objectData);
    }
json_object_set(root,"objects",objects);

//Serialize frames
json_t* frames=json_array();
    for(i=0;i<animation->NumFrames;i++)
    {
    json_t* frame=json_array();
        for(j=0;j<animation->NumObjects;j++)
        {
        json_t* transform=json_array();
        json_array_append_new(transform,SerializeVector(animation->Frames[i][j].Position));
        json_array_append_new(transform,SerializeVector(animation->Frames[i][j].Rotation));
        json_array_append_new(frame,transform);
        }
    json_array_append(frames,frame);
    }
json_object_set(root,"frames",frames);
return root;
}

Animation* DeserializeAnimation(json_t* json)
{
int i,j;
Animation* animation=CreateAnimation();
//Deserialize name
SetName(animation,json_string_value(json_object_get(json,"name")));


//Deserialize model list
json_t* objects=json_object_get(json,"objects");
    for(i=0;i<json_array_size(objects);i++)
    {
    json_t* objectData=json_array_get(objects,i);
    int index=json_integer_value(json_array_get(objectData,0));
    int object=AddObject(animation,GetModelByIndex(index));
    int parent=json_integer_value(json_array_get(objectData,1));
    animation->Objects[object].parentIndex=parent;
    }

//Deserialize frames
json_t* frames=json_object_get(json,"frames");
SetNumFrames(animation,json_array_size(frames));
    for(i=0;i<animation->NumFrames;i++)
    {
    json_t* frame=json_array_get(frames,i);
        for(j=0;j<animation->NumObjects;j++)
        {
        json_t* transform=json_array_get(frame,j);
        Vector position=DeserializeVector(json_array_get(transform,0));
        Vector rotation=DeserializeVector(json_array_get(transform,1));
        UpdateTransform(animation,i,j,position,rotation);
        }
    }
return animation;
}

Animation* SerializeFile()
{

}

void DeserializeFile(const char* filename)
{
int i;
json_t* file=json_load_file(filename,0,NULL);
//Load models
json_t* models=json_object_get(file,"models");
    for(i=0;i<json_array_size(models);i++)
    {
    Model* model=DeserializeModel(json_array_get(models,i));
    AddModel(model);
    }
//Load animations
json_t* animations=json_object_get(file,"animations");
    for(i=0;i<json_array_size(animations);i++)
    {
    Animation* animation=DeserializeAnimation(json_array_get(animations,i));
    AddAnimation(animation);
    }
}
