#include "serialization.h"
#include "dat.h"
#include <assert.h>
#include <string.h>

int try_catch_int(json_t* json, char* key, uint64_t def)
{
	json_t* deserialize = json_object_get(json, key);
	if (deserialize != NULL)
	{
		return json_integer_value(deserialize);
	}
	else
	{
		return def;
	}
}

void serialize_int_dict(json_t* json, char* key, uint64_t input)
{
	json_t* serialize = json_integer(input);
	json_object_set_new(json, key, serialize);
}


char* strdup(const char* str)
{
    char* new_str = malloc(strlen(str) + 1);
    strcpy(new_str, str);
    return new_str;
}

json_t* matrix_serialize(Matrix matrix)
{
    int i, j;
    json_t* root = json_array();
    for (i = 0; i < 4; i++) {
        json_t* row = json_array();
        for (j = 0; j < 4; j++) {
            json_t* value = json_real(matrix.Data[j + (i << 2)]);
            json_array_append_new(row, value);
        }
        json_array_append_new(root, row);
    }
    return root;
}
static Matrix matrix_deserialize(json_t* json)
{
    int i, j;
    Matrix matrix;
    assert(json_array_size(json) == 4);
    for (i = 0; i < 4; i++) {
        json_t* row = json_array_get(json, i);
        assert(json_array_size(row) == 4);
        for (j = 0; j < 4; j++) {
            matrix.Data[j + (i << 2)] = json_real_value(json_array_get(row, j));
        }
    }
    return matrix;
}

static Vector vector_deserialize(json_t* components)
{
    Vector vector;
    vector.X = json_real_value(json_array_get(components, 0));
    vector.Y = json_real_value(json_array_get(components, 1));
    vector.Z = json_real_value(json_array_get(components, 2));
    return vector;
}

json_t* model_serialize(model_t* model)
{
    int i;

    json_t* root = json_object();

    json_object_set_new(root, "name", json_string(model->name));

    json_object_set_new(root, "is_rider", json_integer(model->is_rider));

    json_t* vertices = json_array();
    for (i = 0; i < model->num_vertices; i++) {
        json_t* vertex = json_array();
        json_array_append_new(vertex, json_real(model->vertices[i].X));
        json_array_append_new(vertex, json_real(model->vertices[i].Y));
        json_array_append_new(vertex, json_real(model->vertices[i].Z));
        json_array_append_new(vertices, vertex);
    }
    json_object_set_new(root, "vertices", vertices);

    json_t* normals = json_array();
    for (i = 0; i < model->num_normals; i++) {
        json_t* normal = json_array();
        json_array_append_new(normal, json_real(model->normals[i].X));
        json_array_append_new(normal, json_real(model->normals[i].Y));
        json_array_append_new(normal, json_real(model->normals[i].Z));
        json_array_append_new(normals, normal);
    }
    json_object_set_new(root, "normals", normals);
    json_t* faces = json_array();
    for (i = 0; i < model->num_faces; i++) {
        json_t* face = json_array();
        json_array_append_new(face, json_integer(model->faces[i].color));

        json_t* vert_indices = json_array();
        json_array_append_new(vert_indices,
            json_integer(model->faces[i].vertices[0]));
        json_array_append_new(vert_indices,
            json_integer(model->faces[i].vertices[1]));
        json_array_append_new(vert_indices,
            json_integer(model->faces[i].vertices[2]));
        json_array_append_new(face, vert_indices);

        json_t* norm_indices = json_array();
        json_array_append_new(norm_indices,
            json_integer(model->faces[i].normals[0]));
        json_array_append_new(norm_indices,
            json_integer(model->faces[i].normals[1]));
        json_array_append_new(norm_indices,
            json_integer(model->faces[i].normals[2]));
        json_array_append_new(face, norm_indices);

        json_array_append_new(faces, face);
    }
    json_object_set_new(root, "faces", faces);

    json_object_set_new(root, "transform", matrix_serialize(model->transform));
    return root;
}
model_t* model_deserialize(json_t* json)
{
    int i, j;
    json_t* name = json_object_get(json, "name");
    json_t* vertices = json_object_get(json, "vertices");
    json_t* normals = json_object_get(json, "normals");
    json_t* faces = json_object_get(json, "faces");
    json_t* transform = json_object_get(json, "transform");
    json_t* is_rider = json_object_get(json, "is_rider");
    if (vertices == NULL || normals == NULL || faces == NULL) {
        fprintf(stderr,
            "Failed loading model because one or more of the "
            "vertex,normal,and face arrays are missing\n");
        return NULL;
    }
    // Allocate model
    model_t* model = malloc(sizeof(model_t));
    if (name)
        model->name = strdup(json_string_value(name));
    else
        model->name = strdup("Unnamed Model");
    model->num_vertices = json_array_size(vertices);
    model->num_normals = json_array_size(normals);
    model->num_faces = json_array_size(faces);
    model->vertices = malloc(model->num_vertices * sizeof(Vector));
    model->normals = malloc(model->num_normals * sizeof(Vector));
    model->faces = malloc(model->num_faces * sizeof(face_t));
    if (transform)
        model->transform = matrix_deserialize(transform);
    else
        model->transform = MatrixIdentity();
    if (is_rider)
        model->is_rider = json_integer_value(is_rider);
    else
        model->is_rider = 0;
    model->num_lines = 0;
    model->lines = NULL;
    for (i = 0; i < model->num_vertices; i++) {
        json_t* vertex = json_array_get(vertices, i);
        model->vertices[i] = vector_deserialize(vertex);
    }

    for (i = 0; i < model->num_normals; i++) {
        json_t* normal = json_array_get(normals, i);
        model->normals[i] = vector_deserialize(normal);
    }

    for (i = 0; i < model->num_faces; i++) {
        json_t* face = json_array_get(faces, i);
        model->faces[i].color = json_integer_value(json_array_get(face, 0));

        json_t* face_vertices = json_array_get(face, 1);
        json_t* face_normals = json_array_get(face, 2);
        for (j = 0; j < 3; j++) {
            model->faces[i].vertices[j] = json_integer_value(json_array_get(face_vertices, j));
            model->faces[i].normals[j] = json_integer_value(json_array_get(face_normals, j));
        }
    }
    return model;
}

json_t* animation_serialize(animation_t* animation,
    model_t** models,
    int num_models)
{
    json_t* root = json_array();

    for (int i = 0; i < animation->num_objects; i++) {
        int model_index = -1;
        int parent_index = -1;
        for (int j = 0; j < num_models; j++)
            if (models[j] == animation->objects[i]->model)
                model_index = j;
        for (int j = 0; j < animation->num_objects; j++)
            if (animation->objects[j] == animation->objects[i]->parent)
                parent_index = j;

        if (model_index == -1)
            fprintf(stderr,
                "Object missing model or model index out of range. This "
                "should not happen.\n");

        json_t* anim_object = json_object();
        json_object_set_new(anim_object, "model", json_integer(model_index));
        json_object_set_new(anim_object, "parent", json_integer(parent_index));

        json_t* position = json_array();
        json_t* rotation = json_array();
        for (int j = 0; j < 3; j++) {
            json_array_append_new(
                position, json_string(animation->objects[i]->position[j]->str));
            json_array_append_new(
                rotation, json_string(animation->objects[i]->rotation[j]->str));
        }
        json_object_set_new(anim_object, "position", position);
        json_object_set_new(anim_object, "rotation", rotation);
        json_array_append_new(root, anim_object);
    }
    return root;
}

animation_t* animation_deserialize(json_t* json,
    model_t** models,
    int num_models)
{
    animation_t* animation = animation_new();

    for (unsigned int i = 0; i < json_array_size(json); i++) {
        json_t* object = json_array_get(json, i);
        int model_index = json_integer_value(json_object_get(object, "model"));
        if (model_index < 0 || model_index >= num_models) {
            fprintf(stderr,
                "Failed loading animation because model index is out of range\n");
            animation_free(animation);
            return NULL;
        }
        animation_add_new_object(animation, models[model_index]);
    }

    for (int i = 0; i < animation->num_objects; i++) {
        json_t* object = json_array_get(json, i);

        json_t* parent = json_object_get(object, "parent");
        int parent_index = json_integer_value(parent);

        if (parent != NULL && parent_index >= 0 && parent_index < animation->num_objects) {
            if (animation_object_set_parent(animation->objects[i],
                    animation->objects[parent_index])
                == 0) {
                fprintf(
                    stderr,
                    "Failed loading animation because object parents form a cycle\n");
                animation_free(animation);
                return NULL;
            }
        }

        json_t* position = json_object_get(object, "position");
        json_t* rotation = json_object_get(object, "rotation");
        for (int j = 0; j < 3; j++) {
            const char* error;
            animation_expression_parse(animation->objects[i]->position[j],
                json_string_value(json_array_get(position, j)),
                &error);
            if (error == NULL)
                animation_expression_parse(
                    animation->objects[i]->rotation[j],
                    json_string_value(json_array_get(rotation, j)), &error);
            if (error != NULL) {
                fprintf(stderr,
                    "Failed loading animation because of error parsing "
                    "expression: %s\n",
                    error);
                animation_free(animation);
                return NULL;
            }
        }
    }
    return animation;
}

static json_t* image_serialize(image_t* image)
{
    json_t* json = json_object();

    json_t* width = json_integer(image->width);
    json_object_set_new(json, "width", width);

    json_t* height = json_integer(image->height);
    json_object_set_new(json, "height", height);

    json_t* x_offset = json_integer(image->x_offset);
    json_object_set_new(json, "x_offset", x_offset);

    json_t* y_offset = json_integer(image->y_offset);
    json_object_set_new(json, "y_offset", y_offset);

    json_t* flags = json_integer(image->flags);
    json_object_set_new(json, "flags", flags);

    json_t* data = json_array();
    int x, y;
    for (y = 0; y < image->height; y++) {
        json_t* row = json_array();
        for (x = 0; x < image->width; x++) {
            json_t* pixel = json_integer(image->data[x][y]);
            json_array_append_new(row, pixel);
        }
        json_array_append_new(data, row);
    }
    json_object_set_new(json, "data", data);

    return json;
}
static image_t* image_deserialize(json_t* json)
{
    json_t* width = json_object_get(json, "width");
    json_t* height = json_object_get(json, "height");
    json_t* x_offset = json_object_get(json, "x_offset");
    json_t* y_offset = json_object_get(json, "y_offset");
    json_t* flags = json_object_get(json, "flags");

    if (width == NULL || height == NULL) {
        fprintf(stderr, "Failed loading image because width or height missing\n");
        return NULL;
    }

    image_t* image = image_new(json_integer_value(width), json_integer_value(height), 0);

    image->x_offset = json_integer_value(x_offset);
    image->y_offset = json_integer_value(y_offset);
    image->flags = json_integer_value(flags);

    json_t* data = json_object_get(json, "data");
    int x, y;
    for (y = 0; y < image->height; y++) {
        json_t* row = json_array_get(data, y);
        for (x = 0; x < image->width; x++) {
            image->data[x][y] = json_integer_value(json_array_get(row, x));
        }
    }

    return image;
}

json_t* project_serialize(project_t* project)
{
    json_t* json = json_object();
    // Serialize strings
    json_t* name = json_string(project->name);
    json_object_set_new(json, "name", name);
    json_t* description = json_string(project->description);
    json_object_set_new(json, "description", description);
    // Serialize id
    json_t* id = json_integer(project->id);
    json_object_set_new(json, "id", id);
    // Serialize flags
    json_t* flags = json_integer(project->flags);
    json_object_set_new(json, "flags", flags);
    // Serialize excitement
    json_t* excitement = json_integer(project->excitement);
    json_object_set_new(json, "excitement", excitement);
    // Serialize intensity
    json_t* intensity = json_integer(project->intensity);
    json_object_set_new(json, "intensity", intensity);
    // Serialize nausea
    json_t* nausea = json_integer(project->nausea);
    json_object_set_new(json, "nausea", nausea);
    // Serialize max height adjustment
    json_t* max_height = json_integer(project->max_height);
    json_object_set_new(json, "max_height", max_height);
    // Serialize track type
    json_t* track_type = json_integer(project->track_type);
    json_object_set_new(json, "track_type", track_type);
    // Serialize minimum/maximum cars
    json_t* min_cars = json_integer(project->minimum_cars);
    json_object_set_new(json, "minimum_cars", min_cars);
    json_t* max_cars = json_integer(project->maximum_cars);
    json_object_set_new(json, "maximum_cars", max_cars);
    // Serialize zero cars
    json_t* zero_cars = json_integer(project->zero_cars);
    json_object_set_new(json, "zero_cars", zero_cars);
    // Serialize car icon index
    json_t* car_icon_index = json_integer(project->car_icon_index);
    json_object_set_new(json, "car_icon_index", car_icon_index);
	//Serialize ride categories
	serialize_int_dict(json, "ride_category_0", project->ride_categories[0]);
	serialize_int_dict(json, "ride_category_1", project->ride_categories[1]);

	serialize_int_dict(json, "track_sections", project->track_sections);
    // Serialize preview image
    json_t* preview = image_serialize(project->preview_image);
    json_object_set_new(json, "preview", preview);

    // Serialize default color schemes
    json_t* default_color_schemes = json_array();
    for (uint32_t i = 0; i < project->num_color_schemes; i++) {
        json_t* color_scheme = json_array();
        json_array_append_new(color_scheme,
            json_integer(project->color_schemes[i].colors[0]));
        json_array_append_new(color_scheme,
            json_integer(project->color_schemes[i].colors[1]));
        json_array_append_new(color_scheme,
            json_integer(project->color_schemes[i].colors[2]));
        json_array_append_new(default_color_schemes, color_scheme);
    }
    json_object_set_new(json, "default_color_schemes", default_color_schemes);

    // Serialize car types
    json_t* car_types = json_object();
    if (project->car_types[CAR_INDEX_DEFAULT] != 0xFF)
        json_object_set_new(car_types, "default",
            json_integer(project->car_types[CAR_INDEX_DEFAULT]));
    if (project->car_types[CAR_INDEX_FRONT] != 0xFF)
        json_object_set_new(car_types, "front",
            json_integer(project->car_types[CAR_INDEX_FRONT]));
    if (project->car_types[CAR_INDEX_SECOND] != 0xFF)
        json_object_set_new(car_types, "second",
            json_integer(project->car_types[CAR_INDEX_SECOND]));
    if (project->car_types[CAR_INDEX_THIRD] != 0xFF)
        json_object_set_new(car_types, "third",
            json_integer(project->car_types[CAR_INDEX_THIRD]));
    if (project->car_types[CAR_INDEX_REAR] != 0xFF)
        json_object_set_new(car_types, "rear",
            json_integer(project->car_types[CAR_INDEX_REAR]));
    json_object_set_new(json, "car_types", car_types);
    // Serialize cars
    // Determine which cars are used
    unsigned char cars_used[NUM_CARS];
    memset(cars_used, 0, NUM_CARS);
    for (int i = 0; i < 5; i++) {
        if (project->car_types[i] != 0xFF) {
            cars_used[project->car_types[i]] = 1;
        }
    }
    json_t* cars = json_array();
    for (int i = 0; i < NUM_CARS; i++) {
        json_t* car;
        if (cars_used[i] || project->cars[i].flags & CAR_CAN_INVERT) {
            car = json_object();
            // Animation
            json_t* anim = animation_serialize(project->cars[i].animation,
                project->models, project->num_models);
            json_object_set_new(car, "animation", anim);
            // Flags
            json_t* flags = json_integer(project->cars[i].flags);
            json_object_set_new(car, "flags", flags);
            // Animation type
            json_t* animation_type = json_integer(project->cars[i].animation_type);
            json_object_set_new(car, "animation_type", animation_type);
            // Sprites
            json_t* sprites = json_integer(project->cars[i].sprites);
            json_object_set_new(car, "sprites", sprites);
            // Spacing
            json_t* spacing = json_integer(project->cars[i].spacing);
            json_object_set_new(car, "spacing", spacing);
            // Friction
            json_t* friction = json_integer(project->cars[i].friction);
            json_object_set_new(car, "friction", friction);
            // Running sound
            json_t* running_sound = json_integer(project->cars[i].running_sound);
            json_object_set_new(car, "running_sound", running_sound);
            // Secondary sound
            json_t* secondary_sound = json_integer(project->cars[i].secondary_sound);
            json_object_set_new(car, "secondary_sound", secondary_sound);
            // Z value
            json_t* z_value = json_integer(project->cars[i].z_value);
            json_object_set_new(car, "z_value", z_value);


			json_t* spin_inertia = json_integer(project->cars[i].spin_inertia);
			json_object_set_new(car, "spin_inertia", spin_inertia);
			json_t* spin_friction = json_integer(project->cars[i].spin_friction);
			json_object_set_new(car, "spin_friction", spin_friction);
			json_t* powered_velocity = json_integer(project->cars[i].powered_velocity);
			json_object_set_new(car, "powered_velocity", powered_velocity);
			json_t* powered_acceleration = json_integer(project->cars[i].powered_acceleration);
			json_object_set_new(car, "powered_acceleration", powered_acceleration);

			json_t* car_visual = json_integer(project->cars[i].car_visual);
			json_object_set_new(car, "car_visual", car_visual);
			json_t* effect_visual = json_integer(project->cars[i].effect_visual);
			json_object_set_new(car, "effect_visual", effect_visual);

			json_t* logflume_reverser_vehicle = json_integer(project->cars[i].logflume_reverser_vehicle);
			json_object_set_new(car, "logflume_reverser_vehicle", logflume_reverser_vehicle);

			json_t* double_sound_frequency = json_integer(project->cars[i].double_sound_frequency);
			json_object_set_new(car, "double_sound_frequency", double_sound_frequency);

        } else
            car = json_null();
        json_array_append_new(cars, car);
    }
    json_object_set_new(json, "cars", cars);
    // Load models
    json_t* models = json_array();
    for (int i = 0; i < project->num_models; i++) {
        json_t* model = model_serialize(project->models[i]);
        json_array_append_new(models, model);
    }
    json_object_set_new(json, "models", models);

    return json;
}

project_t* project_deserialize(json_t* json)
{
    project_t* project = project_new();
    // Deserialize strings
    json_t* name = json_object_get(json, "name");
    json_t* description = json_object_get(json, "description");
    if (name != NULL) {
        project->name = realloc(project->name, json_string_length(name) + 1);
        strcpy(project->name, json_string_value(name));
    }
    if (description != NULL) {
        project->description = realloc(project->description, json_string_length(description) + 1);
        strcpy(project->description, json_string_value(description));
    }
    // Deserialize id
	project->id = try_catch_int(json, "flags", rand());
    // Deserialize flags
	project->flags = try_catch_int(json, "flags", RIDE_SEPARATE_RIDE_DEPRECATED | RIDE_SEPARATE_RIDE);
    // Deserialize modifiers
	project->excitement = try_catch_int(json, "excitement", 0);
	project->intensity = try_catch_int(json, "intensity", 0);
	project->nausea = try_catch_int(json, "nausea", 0);
	project->max_height = try_catch_int(json, "max_height", 0);
    // Deserialize track type
	project->track_type = try_catch_int(json, "track_type", 5);
    // Deserialize minimum/maximum cars
	project->minimum_cars = try_catch_int(json, "minimum_cars", 8);
	project->maximum_cars = try_catch_int(json, "maximum_cars", 8);
    // Deserialize zero cars
	project->zero_cars = try_catch_int(json, "zero_cars", 0);
    // Deserialize car icon index
	project->car_icon_index = try_catch_int(json, "car_icon_index", 0);
	//Deserialize ride categories
	project->ride_categories[0] = try_catch_int(json, "ride_category_0", 0);
	project->ride_categories[1] = try_catch_int(json, "ride_category_1", 255);
	project->track_sections = try_catch_int(json, "track_sections", 0xFFFFFFFFFFFFFFFFl);

    // Deserialize preview image
    json_t* preview = json_object_get(json, "preview");
    if (preview != NULL) {
        image_t* preview_image = image_deserialize(preview);
        if (preview_image != NULL)
            project_set_preview(project, preview_image);
    }

    // Deserialize default color schemes
    json_t* default_color_schemes = json_object_get(json, "default_color_schemes");
    project->num_color_schemes = json_array_size(default_color_schemes) > MAX_COLOR_SCHEMES
        ? MAX_COLOR_SCHEMES
        : json_array_size(default_color_schemes);
    for (uint32_t i = 0; i < project->num_color_schemes; i++) {
        json_t* color_scheme = json_array_get(default_color_schemes, i);
        if (json_array_size(color_scheme) > 0)
            project->color_schemes[i].colors[0] = json_integer_value(json_array_get(color_scheme, 0));
        if (json_array_size(color_scheme) > 1)
            project->color_schemes[i].colors[1] = json_integer_value(json_array_get(color_scheme, 1));
        if (json_array_size(color_scheme) > 2)
            project->color_schemes[i].colors[2] = json_integer_value(json_array_get(color_scheme, 2));
    }

    // Deserialize car types
    json_t* car_types = json_object_get(json, "car_types");
    if (car_types != NULL) {
        json_t* car_type = json_object_get(car_types, "default");
        project->car_types[CAR_INDEX_DEFAULT] = car_type != NULL ? json_integer_value(car_type) : 0xFF;
        car_type = json_object_get(car_types, "front");
        project->car_types[CAR_INDEX_FRONT] = car_type != NULL ? json_integer_value(car_type) : 0xFF;
        car_type = json_object_get(car_types, "second");
        project->car_types[CAR_INDEX_SECOND] = car_type != NULL ? json_integer_value(car_type) : 0xFF;
        car_type = json_object_get(car_types, "third");
        project->car_types[CAR_INDEX_THIRD] = car_type != NULL ? json_integer_value(car_type) : 0xFF;
        car_type = json_object_get(car_types, "rear");
        project->car_types[CAR_INDEX_REAR] = car_type != NULL ? json_integer_value(car_type) : 0xFF;
    }

    // Load models
    json_t* models = json_object_get(json, "models");
    if (models != NULL) {
        for (unsigned int i = 0; i < json_array_size(models); i++) {
            model_t* model = model_deserialize(json_array_get(models, i));
            if (model == NULL) {
                fprintf(
                    stderr,
                    "Failed loading project because one of its models is invalid\n");
                project_free(project);
                return NULL;
            }
            project_add_model(project, model);
        }
    }

    // Deserialize cars
    json_t* cars = json_object_get(json, "cars");
    for (unsigned int i = 0; i < NUM_CARS; i++) {
        json_t* car = json_array_get(cars, i);
        if (!json_is_null(car)) {
            // Animation
            json_t* anim = json_object_get(car, "animation");
            if (anim != NULL) {
                animation_t* animation = animation_deserialize(anim, project->models, project->num_models);
                if (animation == NULL) {
                    fprintf(stderr,
                        "Failed loading project because an animation is invalid\n");
                    project_free(project);
                    return NULL;
                }
                animation_free(project->cars[i].animation);
                project->cars[i].animation = animation;
            } else
                project->cars[i].animation = animation_new();
            // Flags
            json_t* flags = json_object_get(car, "flags");
            json_t* animation_type = json_object_get(car, "animation_type");

			if (animation_type != NULL && flags != NULL) {
				project->cars[i].flags = json_integer_value(flags);
				project->cars[i].animation_type = json_integer_value(animation_type);
			}
			else if (flags != NULL && animation_type == NULL) { // previous iterations of the tool grouped animation type into flags erroneously
				project->cars[i].flags = json_integer_value(flags) >> 8; // updating to the new logic requires additional steps
				project->cars[i].animation_type = json_integer_value(flags) & 0xFF; // aaaaaa
			}

            json_t* sprites = json_object_get(car, "sprites");
            if (sprites != NULL)
                project->cars[i].sprites = json_integer_value(sprites);
            // Spacing
			project->cars[i].spacing = try_catch_int(car, "spacing", 8675309);
            // Friction
			json_t* mass = json_object_get(car, "mass");
			json_t* friction = json_object_get(car, "friction");
			if (mass != NULL) {
				project->cars[i].friction = json_integer_value(mass);
			}
			else if (friction != NULL)
			{
				project->cars[i].friction = json_integer_value(friction);
			}
			else
			{
				project->cars[i].friction = 235;
			} /**/
            // Running sound
			project->cars[i].running_sound = try_catch_int(car, "running_sound", 255);
			project->cars[i].secondary_sound = try_catch_int(car, "secondary_sound", 255);
			project->cars[i].z_value = try_catch_int(car, "z_value", 5);
			project->cars[i].spin_inertia = try_catch_int(car, "spin_inertia", 0);
			project->cars[i].spin_friction = try_catch_int(car, "spin_friction", 0);
			project->cars[i].powered_acceleration = try_catch_int(car, "powered_acceleration", 0);
			project->cars[i].powered_velocity = try_catch_int(car, "powered_velocity", 0);

			project->cars[i].car_visual = try_catch_int(car, "car_visual", 0);
			project->cars[i].effect_visual = try_catch_int(car, "effect_visual", 1);
			project->cars[i].logflume_reverser_vehicle = try_catch_int(car, "logflume_reverser_vehicle", 0);
			project->cars[i].double_sound_frequency = try_catch_int(car, "double_sound_frequency", 0);
        }
    }
    return project;
}
project_t* project_load(const char* filename)
{
    json_t* file = json_load_file(filename, 0, NULL);
    if (file == NULL) {
        fprintf(stderr, "Failed loading project because file is not valid JSON\n");
        return NULL;
    }
    project_t* project = project_deserialize(file);
    json_delete(file);
    return project;
}
void project_save(const char* filename, project_t* project)
{
    json_t* json = project_serialize(project);
    json_dump_file(json, filename, 0);
    json_delete(json);
}
