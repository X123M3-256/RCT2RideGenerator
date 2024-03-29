#ifndef RIDETYPES_H_INCLUDED
#define RIDETYPES_H_INCLUDED
#include <stdint.h>
#define NUM_RIDE_TYPES 53
#define NUM_RIDE_CATEGORIES 6

typedef struct {
    uint8_t id;
    const char* name;
} ride_type_t;

typedef struct {
    uint8_t id;
    const char* name;
} ride_category_t;

ride_type_t* ride_type_by_index(int index);
ride_type_t* ride_type_by_name(const char* name);

ride_category_t* ride_category_by_index(int index);
ride_category_t* ride_category_by_name(const char* name);

#endif // RIDETYPES_H_INCLUDED
