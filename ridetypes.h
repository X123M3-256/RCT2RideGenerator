#ifndef RIDETYPES_H_INCLUDED
#define RIDETYPES_H_INCLUDED
#include<stdint.h>
#define NUM_RIDE_TYPES 48

typedef struct
{
uint8_t id;
const char* name;
}ride_type_t;

ride_type_t* ride_type_by_index(int index);
ride_type_t* ride_type_by_name(const char* name);

#endif // RIDETYPES_H_INCLUDED
