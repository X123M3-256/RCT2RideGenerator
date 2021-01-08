#include "animation.h"
#include "palette.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "floatparse.h"

typedef struct {
    const char* str;
    int position;
    const char* error;
} parser_state_t;

typedef struct {
    animation_instruction_t* instructions;
    int allocated_instructions;
    int num_instructions;
} instruction_list_t;

static animation_instruction_t parse_literal(parser_state_t* state)
{
    animation_instruction_t instruction;
    char* end_ptr;

    instruction.opcode = OP_LOD_IMM;
    instruction.operand.immediate = hack_strtod(state->str + state->position, &end_ptr);
    if (end_ptr == (state->str + state->position)) {
        state->error = "Invalid numeric literal";
        return instruction;
    }
    state->position += end_ptr - (state->str + state->position);

    return instruction;
}

static animation_instruction_t parse_identifier(parser_state_t* state)
{
    animation_instruction_t instruction = {};

    char identifier[32];
    int identifier_length = 0;
    while (identifier_length < 31 && state->str[state->position + identifier_length] >= 'a' && state->str[state->position + identifier_length] <= 'z') {
        identifier[identifier_length] = state->str[state->position + identifier_length];
        identifier_length++;
    }
    identifier[identifier_length] = 0;

    // List of valid identifiers and their lengths

    // Find the index of the input identifier in aformentioned list
    int index;
    for (index = 0; index < ANIMATION_NUM_VARIABLES+ANIMATION_NUM_FUNCTIONS; index++) {
        if (index < ANIMATION_NUM_VARIABLES) {
            if (strcmp(identifier, ANIMATION_VAR_IDENTIFIERS[index]) == 0)
                break;
        } else {
            if (strcmp(identifier, OP_FUNC_IDENTIFIERS[index-ANIMATION_NUM_VARIABLES]) == 0)
                break;
        }
    }
    // If valid identifier not found, error
    if (index >= ANIMATION_NUM_VARIABLES+ANIMATION_NUM_FUNCTIONS) {
        state->error = "Unrecognized identifier";
        return instruction;
    }

    if (index < ANIMATION_NUM_VARIABLES) { // less than 8
        instruction.opcode = OP_LOD_VAR;
        instruction.operand.variable = index;
    } else
        instruction.opcode = ANIMATION_FUNCTIONS[index-ANIMATION_NUM_VARIABLES];

    state->position += identifier_length;
    return instruction;
}

static animation_instruction_t parse_operator(parser_state_t* state)
{
    animation_instruction_t instruction = {};
    switch (state->str[state->position]) {
    case '+':
        instruction.opcode = OP_ADD;
        break;
    case '-':
        if (state->position == 0 || state->str[state->position - 1] == '(' || state->str[state->position - 1] == '+' || state->str[state->position - 1] == '-' || state->str[state->position - 1] == '*' || state->str[state->position - 1] == '/')
            instruction.opcode = OP_MINUS;
        else
            instruction.opcode = OP_SUB;
        break;
    case '*':
        instruction.opcode = OP_MUL;
        break;
    case '/':
        instruction.opcode = OP_DIV;
        break;
    default:
        state->error = "Unrecognized operator";
        return instruction;
    }
    state->position++;
    return instruction;
}

static animation_instruction_t parse_instruction(parser_state_t* state)
{
    animation_instruction_t instruction = {};
    if (state->str[state->position] == '+' || state->str[state->position] == '-' || state->str[state->position] == '*' || state->str[state->position] == '/')
        instruction = parse_operator(state);
    else if ((state->str[state->position] >= '0' && state->str[state->position] <= '9') || state->str[state->position] == '-')
        instruction = parse_literal(state);
    else if (state->str[state->position] >= 'a' && state->str[state->position] <= 'z')
        instruction = parse_identifier(state);
    else if (state->str[state->position] == '(') {
        instruction.opcode = OP_OPEN_PAREN;
        state->position++;
    } else if (state->str[state->position] == ')') {
        instruction.opcode = OP_CLOSE_PAREN;
        state->position++;
    } else
        state->error = "Invalid character";
    return instruction;
}

animation_expression_t* animation_expression_new()
{
    animation_expression_t* expr = malloc(sizeof(animation_expression_t));
    expr->num_instructions = 1;
    expr->instructions = malloc(sizeof(animation_instruction_t));
    expr->str = malloc(2);
    strcpy(expr->str, "0");
    expr->instructions[0].opcode = OP_LOD_IMM;
    expr->instructions[0].operand.immediate = 0.0;
    return expr;
}

void animation_expression_free(animation_expression_t* expr)
{
    free(expr->instructions);
    free(expr);
}

static instruction_list_t* instruction_list_new()
{
    instruction_list_t* list = malloc(sizeof(instruction_list_t));
    list->allocated_instructions = 4;
    list->instructions = malloc(4 * sizeof(animation_instruction_t));
    list->num_instructions = 0;
    return list;
}
static void instruction_list_add(instruction_list_t* list,
    animation_instruction_t instruction)
{
    if (list->allocated_instructions == list->num_instructions) {
        list->allocated_instructions *= 2;
        list->instructions = realloc(list->instructions,
            list->allocated_instructions * sizeof(animation_instruction_t));
    }
    list->instructions[list->num_instructions] = instruction;
    list->num_instructions++;
}
static animation_instruction_t* instruction_list_get_instructions(
    instruction_list_t* list,
    int* num_instructions)
{
    animation_instruction_t* instructions = realloc(list->instructions,
        list->num_instructions * sizeof(animation_instruction_t));
    *num_instructions = list->num_instructions;
    free(list);
    return instructions;
}
static void instruction_list_free(instruction_list_t* list)
{
    free(list->instructions);
    free(list);
}

void animation_expression_parse(animation_expression_t* expr,
    const char* str,
    const char** error)
{
    parser_state_t state;
    state.str = str;
    state.position = 0;
    state.error = NULL;
    *error = NULL;
    
    instruction_list_t* instruction_list = instruction_list_new();

    animation_instruction_t stack[256];
    int stack_top = -1;

    while (state.str[state.position] != 0) {
        animation_instruction_t instruction = parse_instruction(&state);
        if (state.error != NULL) {
            *error = state.error;
            instruction_list_free(instruction_list);
            return;
        }
        switch (instruction.opcode) {
        case OP_LOD_IMM:
        case OP_LOD_VAR:
            instruction_list_add(instruction_list, instruction);
            break;
        case OP_ADD:
        case OP_SUB:
            while (stack_top >= 0 && stack[stack_top].opcode != OP_OPEN_PAREN) {
                instruction_list_add(instruction_list, stack[stack_top]);
                stack_top--;
            }
            stack_top++;
            stack[stack_top] = instruction;
            break;
        case OP_MUL:
        case OP_DIV:
        case OP_MINUS:
        case OP_EXP:
        case OP_LN:
        case OP_SIN:
        case OP_COS:
        case OP_CLAMP:
        case OP_ABS:
        case OP_UNIT:
        case OP_SQRT:
        case OP_SQUARE:
        case OP_FLOOR:
        case OP_CEIL:
        case OP_OPEN_PAREN:
            stack_top++;
            stack[stack_top] = instruction;
            break;
        case OP_CLOSE_PAREN:
            while (stack_top >= 0 && stack[stack_top].opcode != OP_OPEN_PAREN) {
                instruction_list_add(instruction_list, stack[stack_top]);
                stack_top--;
            }
            if (stack_top < 0) {
                *error = "Mismatched parentheses";
                instruction_list_free(instruction_list);
                return;
            }
            stack_top--;
            
            if (stack_top >= 0){
                int index;
                for (index = 0; index < ANIMATION_NUM_FUNCTIONS; index++) {
                    if (stack[stack_top].opcode == ANIMATION_FUNCTIONS[index]) {
                        instruction_list_add(instruction_list, stack[stack_top]);
                        stack_top--;
                        break;
                    }
                }
            }
            break;
        }
        //*/
    }
    while (stack_top >= 0) {
        if (stack[stack_top].opcode == OP_OPEN_PAREN) {
            *error = "Mismatched parentheses";
            instruction_list_free(instruction_list);
            return;
        }
        instruction_list_add(instruction_list, stack[stack_top]);
        stack_top--;
    }
    // Check validity of expression
    int stack_size = 0;
    for (int i = 0; i < instruction_list->num_instructions; i++) {
        switch (instruction_list->instructions[i].opcode) {
        case OP_LOD_IMM:
        case OP_LOD_VAR:
            stack_size++;
            break;
        case OP_ADD:
        case OP_SUB:
        case OP_MUL:
        case OP_DIV:
            stack_size--;
            break;
        default:
            break;
        }
    }

    if (stack_size != 1) {
        *error = "Incorrect number of operands";
        instruction_list_free(instruction_list);
        return;
    }

    // Update expression
    expr->str = realloc(expr->str, strlen(str) + 1);
    strcpy(expr->str, str);
    free(expr->instructions);
    expr->instructions = instruction_list_get_instructions(
        instruction_list, &(expr->num_instructions));
}

float animation_expression_evaluate(animation_expression_t* expr,
    float variables[ANIMATION_NUM_VARIABLES])
{
    int i;
    float stack[256];
    int stack_top = -1;
    for (i = 0; i < expr->num_instructions; i++) {
        switch (expr->instructions[i].opcode) {
        case OP_LOD_IMM:
            stack_top++;
            stack[stack_top] = expr->instructions[i].operand.immediate;
            break;
        case OP_LOD_VAR:
            stack_top++;
            stack[stack_top] = variables[expr->instructions[i].operand.variable];
            break;
        case OP_ADD:
            assert(stack_top >= 1);
            stack[stack_top - 1] += stack[stack_top];
            stack_top--;
            break;
        case OP_SUB:
            assert(stack_top >= 1);
            stack[stack_top - 1] -= stack[stack_top];
            stack_top--;
            break;
        case OP_MUL:
            assert(stack_top >= 1);
            stack[stack_top - 1] *= stack[stack_top];
            stack_top--;
            break;
        case OP_DIV:
            assert(stack_top >= 1);
            stack[stack_top - 1] /= stack[stack_top];
            stack_top--;
            break;
        case OP_MINUS:
            assert(stack_top >= 0);
            stack[stack_top] *= -1;
            break;
        case OP_EXP:
            assert(stack_top >= 0);
            stack[stack_top] = exp(stack[stack_top]);
            break;
        case OP_LN:
            assert(stack_top >= 0);
            stack[stack_top] = log(stack[stack_top]);
            break;
        case OP_SIN:
            assert(stack_top >= 0);
            stack[stack_top] = sin(stack[stack_top]);
            break;
        case OP_COS:
            assert(stack_top >= 0);
            stack[stack_top] = cos(stack[stack_top]);
            break;
        case OP_CLAMP:
            assert(stack_top >= 0);
            if (stack[stack_top] < 0)
                stack[stack_top] = 0;
            else if (stack[stack_top] > 1)
                stack[stack_top] = 1;
            break;
        case OP_ABS:
            assert(stack_top >= 0);
            if (stack[stack_top] < 0)
                stack[stack_top] *= -1.0;
            break;
        case OP_UNIT:
            assert(stack_top >= 0);
            if (stack[stack_top] < 0)
                stack[stack_top] = 0;
            else
                stack[stack_top] = 1;
            break;
        case OP_SQRT:
            assert(stack_top >= 0);
            stack[stack_top] = sqrt(stack[stack_top]);
            break;
        case OP_SQUARE:
            assert(stack_top >= 0);
            stack[stack_top] = stack[stack_top]*stack[stack_top];
            break;
        case OP_FLOOR:
            assert(stack_top >= 0);
            stack[stack_top] = floor(stack[stack_top]);
            break;
        case OP_CEIL:
            assert(stack_top >= 0);
            stack[stack_top] = ceil(stack[stack_top]);
            break;
        default:
            fprintf(
                stderr,
                "animation_expression_evaluate: Attempt to execute invalid opcode %i\n",expr->instructions[i].opcode);
            assert(0);
            break;
        }
    }
    assert(stack_top == 0);
    return stack[0];
}

static animation_object_t* animation_object_new(model_t* model)
{
    animation_object_t* object = malloc(sizeof(animation_object_t));
    object->model = model;
    object->parent = NULL;

    for (int i = 0; i < 3; i++) {
        object->position[i] = animation_expression_new();
        object->rotation[i] = animation_expression_new();
    }
    return object;
}

static void animation_object_free(animation_object_t* object)
{
    int i;
    for (i = 0; i < 3; i++) {
        animation_expression_free(object->position[i]);
        animation_expression_free(object->rotation[i]);
    }
    free(object);
}

int animation_object_set_parent(animation_object_t* object,
    animation_object_t* parent)
{
    if (parent != NULL) {
        animation_object_t* cur_object = parent;
        while (cur_object != object && cur_object->parent != NULL)
            cur_object = cur_object->parent;
        if (cur_object == object)
            return 0;
    }
    object->parent = parent;
    return 1;
}

animation_t* animation_new()
{
    animation_t* animation = malloc(sizeof(animation_t));
    animation->num_objects = 0;
    return animation;
}
void animation_free(animation_t* animation)
{
    int i;
    for (i = 0; i < animation->num_objects; i++) {
        animation_object_free(animation->objects[i]);
    }
    free(animation);
}

void animation_add_new_object(animation_t* animation, model_t* model)
{
    animation->objects[animation->num_objects] = animation_object_new(model);
    animation->num_objects++;
}

void animation_calculate_object_transforms(
    animation_t* animation,
    float variables[ANIMATION_NUM_VARIABLES])
{
    Matrix object_global_transforms[animation->num_objects];
    // Calculate transformations relative to parent object
    for (int i = 0; i < animation->num_objects; i++) {
        Vector position;
        Vector rotation;
        animation_object_t* object = animation->objects[i];
        position.X = animation_expression_evaluate(object->position[0], variables);
        position.Y = animation_expression_evaluate(object->position[1], variables);
        position.Z = animation_expression_evaluate(object->position[2], variables);
        rotation.X = animation_expression_evaluate(object->rotation[0], variables);
        rotation.Y = animation_expression_evaluate(object->rotation[1], variables);
        rotation.Z = animation_expression_evaluate(object->rotation[2], variables);
        object->transform = MatrixFromEulerAngles(rotation);
        object->transform.Data[3] = position.X;
        object->transform.Data[7] = position.Y;
        object->transform.Data[11] = position.Z;
        object_global_transforms[i] = animation->objects[i]->transform;
    }
    // Calculate global transformations
    for (int i = 0; i < animation->num_objects; i++) {
        animation_object_t* current_object = animation->objects[i];
        while (current_object->parent != NULL) {
            object_global_transforms[i] = MatrixMultiply(
                current_object->parent->transform, object_global_transforms[i]);
            current_object = current_object->parent;
        }
    }
    // Transfer global transforms to objects
    for (int i = 0; i < animation->num_objects; i++)
        animation->objects[i]->transform = object_global_transforms[i];
}

void animation_render(animation_t* animation,
    Matrix model_view,
    float variables[ANIMATION_NUM_VARIABLES])
{
    animation_calculate_object_transforms(animation, variables);
    for (int i = 0; i < animation->num_objects; i++)
        renderer_render_model(
            animation->objects[i]->model,
            MatrixMultiply(model_view, animation->objects[i]->transform));
}

static int animation_object_is_descendant_of_object(animation_object_t* object,
    animation_object_t* parent)
{
    if (parent == object) {
        return 1;
    }
    if (object->parent != NULL && object->parent != object) { //self-parent check is not necessary with the do/while loop structure but it saves on calculations
        animation_object_t* cur_object = object;
        do {
            cur_object = cur_object->parent;
            if (cur_object == parent) {
                return 1;
            }
        } while (cur_object != object && cur_object->parent != NULL);
    }
    return 0;
}

static int animation_object_is_descendant_of_rider(animation_object_t* object)
{
    if (object->model->is_rider) {
        return 1;
    }
    if (object->parent != NULL && object->parent != object) {
        animation_object_t* cur_object = object;
        do {
            cur_object = cur_object->parent;
            if (cur_object->model->is_rider) {
                return 1;
            }
        } while (cur_object != object && cur_object->parent != NULL);
    }
    return 0;
}

render_data_t animation_split_render_begin(
    animation_t* animation,
    Matrix model_view,
    float variables[ANIMATION_NUM_VARIABLES])
{
    render_data_t data;
    data.model_view = model_view;
    data.cur_object = 0;
    for (int i = 0; i < ANIMATION_NUM_VARIABLES; i++)
        data.variables[i] = variables[i];

    animation_calculate_object_transforms(animation, variables);

    for (int i = 0; i < animation->num_objects; i++) {
        if (animation_object_is_descendant_of_rider(animation->objects[i]) == 0) {
            renderer_render_model(
                animation->objects[i]->model,
                MatrixMultiply(model_view, animation->objects[i]->transform));
        }
    }
    return data;
}

void animation_split_render_next_image(animation_t* animation,
    render_data_t* data)
{
    renderer_clear_color_buffer();

    animation_object_t* first_rider = NULL;
    animation_object_t* second_rider = NULL;

    while (data->cur_object < animation->num_objects) {
        if (animation->objects[data->cur_object]->model->is_rider) {
            if (first_rider == NULL)
                first_rider = animation->objects[data->cur_object];
            else if (second_rider == NULL)
                second_rider = animation->objects[data->cur_object];
            else
                break;
        }
        data->cur_object++;
    }

    // Render pair of riders
    if (second_rider != NULL) {
        for (int i = 0; i < animation->num_objects; i++) {
            if (animation_object_is_descendant_of_object(animation->objects[i], second_rider) == 1) {
                renderer_render_model(
                    animation->objects[i]->model,
                    MatrixMultiply(data->model_view, animation->objects[i]->transform));
                renderer_remap_color(COLOR_PEEP_REMAP_1, COLOR_PEEP_REMAP_2);
                renderer_remap_color(COLOR_REMAP_1, COLOR_REMAP_2);
            }
        }
    }
    if (first_rider != NULL) {
        for (int i = 0; i < animation->num_objects; i++) {
            if (animation_object_is_descendant_of_object(animation->objects[i], first_rider) == 1) {
                renderer_render_model(
                    animation->objects[i]->model,
                    MatrixMultiply(data->model_view, animation->objects[i]->transform));
                //can't have renderer_remap_color down here because otherwise both models get remapped
            }
        }
    }
}

int animation_count_riders(animation_t* animation)
{
    int riders = 0;
    for (int i = 0; i < animation->num_objects; i++)
        if (animation->objects[i]->model->is_rider)
            riders++;
    return riders;
}
