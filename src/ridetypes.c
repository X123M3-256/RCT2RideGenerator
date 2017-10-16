#include "ridetypes.h"
#include <string.h>

static ride_type_t ride_types[NUM_RIDE_TYPES] = {
    { 0x0, "Spiral Coaster" },
    { 0x1, "Stand-up Coaster" },
    { 0x2, "Suspended Swinging Coaster" },
    { 0x3, "Inverted Coaster" },
    { 0x4, "Junior Coaster" },
    { 0x5, "Trains" },
    { 0x6, "Monorail" },
    { 0x7, "Mini Suspended Coaster" },
    { 0x8, "Boats" },
    { 0x9, "Wooden Wild Mouse" },
    { 0xa, "Single-Rail Track" },
    { 0xb, "Car Ride" },
    { 0xd, "Bobsleigh Coaster" },
    { 0xf, "Roller Coaster Trains" },
    { 0x10, "Dinghy Slide" },
    { 0x11, "Mine Train Coaster" },
    { 0x12, "Chairlift" },
    { 0x13, "Corkscrew Coaster" },
    { 0x16, "Go Karts" },
    { 0x17, "Log Flume" },
    { 0x18, "River Rapids" },
    { 0x2a, "Reverse Freefall Coaster" },
    { 0x2c, "Vertical Drop Coaster" },
    { 0x32, "Ghost Train" },
    { 0x33, "Twister Coaster" },
    { 0x34, "Wooden Coaster" },
    { 0x35, "Side-Friction Coaster" },
    { 0x36, "Wild Mouse" },
    { 0x37, "Multi-Dimension Coaster" },
    { 0x39, "Flying Coaster" },
    { 0x3b, "Virginia Reel" },
    { 0x3c, "Splash Boats" },
    { 0x3d, "Mini Helicopters" },
    { 0x3e, "Lay-down Coaster" },
    { 0x3f, "Suspended Monorail" },
    { 0x41, "Reverser Roller Coaster" },
    { 0x42, "Twister Cars" },
    { 0x44, "Giga Coaster" },
    { 0x48, "Monorail Cycles" },
    { 0x49, "Compact Inverted Coaster" },
    { 0x4a, "Coaster Boats" },
    { 0x4b, "Air Powered Vertical Coaster" },
    { 0x4c, "Inverted Hairpin Coaster" },
    { 0x4e, "Submarine Ride" },
    { 0x4f, "River Rafts" },
    { 0x56, "Inverted Impulse Coaster" },
    { 0x57, "Mini Coaster" },
    { 0x58, "Mine Ride" },
    { 0x5a, "LIM Launched Roller Coaster" }
};

ride_type_t* ride_type_by_index(int index)
{
    return ride_types + index;
}

ride_type_t* ride_type_by_name(const char* name)
{
    int i;
    for (i = 0; i < NUM_RIDE_TYPES; i++)
        if (strcmp(ride_types[i].name, name) == 0)
            return ride_types + i;
    return NULL;
}
