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

json_t* SerializeDat(ObjectFile* dat)
{
json_t* root=json_object();
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
