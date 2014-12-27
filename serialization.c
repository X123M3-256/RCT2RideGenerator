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
    json_array_append_new(face,vert_indices);

    json_array_append_new(faces,face);
    }
json_object_set_new(root,"faces",faces);

json_object_set_new(root,"transform",SerializeMatrix(model->transform));
return root;
}
