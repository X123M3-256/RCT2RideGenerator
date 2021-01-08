#include "model.h"
#include "objLoader/obj_parser.h" //TODO make this a library
#include "palette.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
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
    //If the next char is not also a slash, there is a tex coord, which we
ignore
        if(*indices!='/')strtol(indices,&indices,10);
    indices++;
    face.Normals[i]=strtol(indices,&indices,10)-1;
    //face.Flags=RECOLOR_GREEN;
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
//In the first pass over the file, count the numbers of elements that will need
to be allocated
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
        else
if(strcmp("vn",type)==0)model->Normals[curNormal++]=ParseObjVertex();
        else
if(strcmp("f",type)==0)model->Faces[curFace++]=ParseObjFace(curVertex,curNormal);
        else
if(strcmp("l",type)==0)model->Lines[curLine++]=ParseObjLine(curVertex);
    }

//Add model to list
ModelList.Models[ModelList.NumModels++]=model;
return model;
}
*/

model_t* model_new()
{
    model_t* model = malloc(sizeof(model_t));
    model->name = NULL;
    model->is_rider = 0;
    model->num_faces = 0;
    model->num_normals = 0;
    model->num_vertices = 0;
    model->num_lines = 0;
    model->faces = NULL;
    model->vertices = NULL;
    model->normals = NULL;
    model->lines = NULL;
    model->transform = MatrixIdentity();
    return model;
}
model_t* model_load_obj(char* filename)
{
    int i, j;
    obj_scene_data obj_data;
    if (!parse_obj_scene(&obj_data, filename))
        return NULL;
    model_t* model = model_new();

    model->name = malloc(14);
    strcpy(model->name, "Unnamed Model");
    // Count vertices
    model->num_vertices = obj_data.vertex_count;
	// Count normals
    model->num_normals = obj_data.vertex_normal_count;
    // Count faces
    for (i = 0; i < obj_data.face_count; i++)
        model->num_faces += obj_data.face_list[i]->vertex_count - 2;

    // Allocate arrays
    model->vertices = malloc(model->num_vertices * sizeof(Vector));
    model->normals = malloc(model->num_normals * sizeof(Vector));
    model->faces = malloc(model->num_faces * sizeof(face_t));

    // Load vertices
    for (i = 0; i < model->num_vertices; i++) {
        model->vertices[i].X = -obj_data.vertex_list[i]->e[0];
        model->vertices[i].Y = -obj_data.vertex_list[i]->e[1];
        model->vertices[i].Z = -obj_data.vertex_list[i]->e[2];
    }
    // Load vertices
    for (i = 0; i < model->num_normals; i++) {
        model->normals[i].X = -obj_data.vertex_normal_list[i]->e[0];
        model->normals[i].Y = -obj_data.vertex_normal_list[i]->e[1];
        model->normals[i].Z = -obj_data.vertex_normal_list[i]->e[2];
    }
    // Load faces
    int face_index = 0;
    for (i = 0; i < obj_data.face_count; i++) {
        model->faces[face_index].color = 0; // Default to grey
        if (obj_data.material_list != NULL && obj_data.face_list[i]->material_index >= 0) {
            obj_material* material = obj_data.material_list[obj_data.face_list[i]->material_index];
            if (material->diff[0] < 0.3 && material->diff[1] > 0.7 && material->diff[2] < 0.3)
                model->faces[face_index].color = COLOR_REMAP_1;
            else if (material->diff[0] > 0.7 && material->diff[1] < 0.3 && material->diff[2] > 0.7)
                model->faces[face_index].color = COLOR_REMAP_2;
            else if (material->diff[0] > 0.7 && material->diff[1] > 0.7 && material->diff[2] < 0.3)
                model->faces[face_index].color = COLOR_REMAP_3;
            else if (material->diff[0] > 0.7 && material->diff[1] > 0.7 && material->diff[2] > 0.7)
                model->faces[face_index].color = 2;
            else if (material->diff[0] > 0.3 && material->diff[0] < 0.7 && material->diff[1] > 0.3 && material->diff[1] < 0.7 && material->diff[2] > 0.3 && material->diff[2] < 0.7)
                model->faces[face_index].color = 1;
            else if (material->diff[0] < 0.3 && material->diff[1] < 0.3 && material->diff[2] < 0.3)
                model->faces[face_index].color = 0;
        }
        for (j = 0; j < 3; j++) {
            model->faces[face_index].vertices[j] = obj_data.face_list[i]->vertex_index[j];
            model->faces[face_index].normals[j] = obj_data.face_list[i]->normal_index[j];
        }
        face_index++;
        // If polygon is a quad, output another triangle
        if (obj_data.face_list[i]->vertex_count > 3) {
            model->faces[face_index].color = 0;
            model->faces[face_index].vertices[0] = obj_data.face_list[i]->vertex_index[2];
            model->faces[face_index].vertices[1] = obj_data.face_list[i]->vertex_index[3];
            model->faces[face_index].vertices[2] = obj_data.face_list[i]->vertex_index[0];
            model->faces[face_index].normals[0] = obj_data.face_list[i]->normal_index[2];
            model->faces[face_index].normals[1] = obj_data.face_list[i]->normal_index[3];
            model->faces[face_index].normals[2] = obj_data.face_list[i]->normal_index[0];
            face_index++;
        }
    }

    delete_obj_data(&obj_data);
    return model; 
}
void model_free(model_t* model)
{
    free(model->vertices);
    free(model->normals);
    free(model->faces);
    free(model->lines);
    free(model);
}

#define GRID_TILES 20
#define GRID_SIZE (3.3 * (GRID_TILES - 1))
#define HALF_GRID_SIZE (GRID_SIZE / 2.0)
model_t* model_new_grid()
{
    int i;
    model_t* preview_grid = model_new();
    preview_grid->num_vertices = 4 * GRID_TILES;
    preview_grid->num_lines = 2 * GRID_TILES;
    preview_grid->vertices = malloc(4 * GRID_TILES * sizeof(Vector));
    preview_grid->lines = malloc(2 * GRID_TILES * sizeof(line_t));

    Vector* top = preview_grid->vertices;
    Vector* left = top + GRID_TILES;
    Vector* bottom = left + GRID_TILES;
    Vector* right = bottom + GRID_TILES;
    for (i = 0; i < GRID_TILES; i++) {
        top[i].X = 3.3 * i - HALF_GRID_SIZE;
        top[i].Y = 0.0;
        top[i].Z = HALF_GRID_SIZE;
        bottom[i] = top[i];
        bottom[i].Z -= GRID_SIZE;

        left[i].X = -HALF_GRID_SIZE;
        left[i].Y = 0.0;
        left[i].Z = 3.3 * i - HALF_GRID_SIZE;
        right[i] = left[i];
        right[i].X += GRID_SIZE;
        preview_grid->lines[i].color = 6;
        preview_grid->lines[i].vertices[0] = i;
        preview_grid->lines[i].vertices[1] = i + 2 * GRID_TILES;
        preview_grid->lines[i + GRID_TILES].color = 6;
        preview_grid->lines[i + GRID_TILES].vertices[0] = i + GRID_TILES;
        preview_grid->lines[i + GRID_TILES].vertices[1] = i + 3 * GRID_TILES;
    }
    return preview_grid;
}
