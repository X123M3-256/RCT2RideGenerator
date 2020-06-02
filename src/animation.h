#ifndef ANIMATION_H_INCLUDED
#define ANIMATION_H_INCLUDED
#include "model.h"
#include "renderer.h"

#define ANIMATION_NUM_VARIABLES 8
typedef enum {
    VAR_PITCH = 0,
    VAR_YAW = 1,
    VAR_ROLL = 2,
    VAR_SPIN = 3,
    VAR_SWING = 4,
    VAR_FLIP = 5,
    VAR_RESTRAINT = 6,
    VAR_ANIMATION = 7
} animation_variable_t;

static char* ANIMATION_VAR_IDENTIFIERS[] = {
    "pitch",
    "yaw",
    "roll",
    "spin",
    "swing",
    "flip",
    "restraint",
    "animation"
};

typedef enum {
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_MINUS,
    OP_EXP = 5,
    OP_LN = 6,
    OP_SIN = 7,
    OP_COS = 8,
    OP_CLAMP = 9,
    OP_ABS = 10,
    OP_UNIT = 11,
    OP_SQRT = 12,
    OP_SQUARE = 13,
    OP_FLOOR = 14,
    OP_CEIL = 15,
    OP_LOD_IMM,//16
    OP_LOD_VAR,//17
    OP_OPEN_PAREN,//18 // Not opcodes, but it saves having a seperate datatype for
    // tokens
    OP_CLOSE_PAREN//19
} animation_opcode_t;

#define ANIMATION_NUM_FUNCTIONS 11
static animation_opcode_t ANIMATION_FUNCTIONS[] = {
    OP_EXP,
    OP_LN,
    OP_SIN,
    OP_COS,
    OP_CLAMP,
    OP_ABS,
    OP_UNIT,
    OP_SQRT,
    OP_SQUARE,
    OP_FLOOR,
    OP_CEIL
};
static char* OP_FUNC_IDENTIFIERS[] = {
    "exp",
    "ln",
    "sin",
    "cos",
    "clamp",
    "abs",
    "unit",
    "sqrt",
    "square",
    "floor",
    "ceil"
};

#define ANIMATION_MAX_OBJECTS 64
typedef struct {
    animation_opcode_t opcode;
    union {
        float immediate;
        animation_variable_t variable;
    } operand;
} animation_instruction_t;

typedef struct {
    char* str;
    animation_instruction_t* instructions;
    int num_instructions;
} animation_expression_t;

typedef struct animation_object_s {
    model_t* model;
    struct animation_object_s* parent;
    animation_expression_t* position[3];
    animation_expression_t* rotation[3];
    Matrix transform;
} animation_object_t;

typedef struct {
    Matrix model_view;
    float variables[ANIMATION_NUM_VARIABLES];
    int cur_object;
} render_data_t;

typedef struct {
    int num_objects;
    animation_object_t* objects[ANIMATION_MAX_OBJECTS];
} animation_t;

animation_expression_t* animation_expression_new();
void animation_expression_free(animation_expression_t* expr);
void animation_expression_parse(animation_expression_t* expr,
    const char* input,
    const char** error);
float animation_expression_evaluate(animation_expression_t* expr,
    float variables[ANIMATION_NUM_VARIABLES]);

int animation_object_set_parent(animation_object_t* object,
    animation_object_t* parent);

animation_t* animation_new();
void animation_calculate_object_transforms(
    animation_t* animation,
    float variables[ANIMATION_NUM_VARIABLES]);
void animation_add_new_object(animation_t* animation, model_t* model);
int animation_count_riders(animation_t* animation);
void animation_render(animation_t* animation,
    Matrix model_view,
    float variables[ANIMATION_NUM_VARIABLES]);
render_data_t animation_split_render_begin(
    animation_t* animation,
    Matrix model_view,
    float variables[ANIMATION_NUM_VARIABLES]);
void animation_split_render_next_image(animation_t* animation,
    render_data_t* data);
void animation_free(animation_t* animation);
#endif // ANIMATION_H_INCLUDED
