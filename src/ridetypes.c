#include "ridetypes.h"
#include "dat.h"
#include <string.h>

ride_type_t ride_types[NUM_RIDE_TYPES] = {
    { 0x4b, "Air Powered Vertical Coaster" },
    { 0x8, "Boat hire" },
    { 0xd, "Bobsleigh Coaster" },
    { 0xb, "Car Ride" },
    { 0x12, "Chairlift" },
    { 0x49, "Compact Inverted Coaster" },
    { 0x13, "Corkscrew Coaster" },
    { 0x10, "Dinghy Slide" },
    { 25, "Dodgems" },
    { 0x39, "Flying Coaster" },
    { 0x32, "Ghost Train" },
    { 0x44, "Giga Coaster" },
    { 0x16, "Go Karts" },
    { 0x42, "Heartline Twister Coaster" },
    { 0x4c, "Inverted Hairpin Coaster" },
    { 0x56, "Inverted Impulse Coaster" },
    { 0x3, "Inverted Roller Coaster" },
    { 0x4, "Junior Roller Coaster" },
    { 0x5a, "LIM Launched Roller Coaster" },
    { 0x3e, "Lay-down Roller Coaster" },
    { 0x17, "Log Flume" },
    { 0xf, "Looping Roller Coaster" },
    { 0x58, "Mine Ride" },
    { 0x11, "Mine Train Coaster" },
    { 0x3d, "Mini Helicopters" },
    { 0x57, "Mini Roller Coaster" },
    { 0x7, "Mini Suspended Coaster" },
    { 0x5, "Miniature Railway" },
    { 0x6, "Monorail" },
    { 0x48, "Monorail Cycles" },
    { 0x37, "Multi-Dimension Coaster" },
    { 0x2a, "Reverse Freefall Coaster" },
    { 0x41, "Reverser Roller Coaster" },
    { 0x4f, "River Rafts" },
    { 0x18, "River Rapids" },
    { 96, "RMC Hybrid Coaster" },
    { 97, "RMC Raptor Coaster" },
    { 0x35, "Side-Friction Coaster" },
    { 0x0, "Spiral Roller Coaster" },
    { 0x3c, "Splash Boats" },
    { 0x1, "Stand-up Coaster" },
    { 0x36, "Steel Wild Mouse" },
    { 0xa, "Steeplechase" },
    { 0x4e, "Submarine Ride" },
    { 0x3f, "Suspended Monorail" },
    { 0x2, "Suspended Swinging Coaster" },
    { 0x33, "Twister Roller Coaster" },
    { 0x2c, "Vertical Drop Coaster" },
    { 0x3b, "Virginia Reel" },
    { 0x4a, "Water Coaster" },
    { 0x34, "Wooden Roller Coaster" },
    { 0x9, "Wooden Wild Mouse" },
	
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



ride_category_t ride_categories[NUM_RIDE_CATEGORIES] = {
    { CATEGORY_TRANSPORT_RIDE, "Transport Ride" },
    { CATEGORY_GENTLE_RIDE, "Gentle Ride" },
    { CATEGORY_ROLLERCOASTER, "Roller Coaster" },
    { CATEGORY_THRILL_RIDE, "Thrill Ride" },
    { CATEGORY_WATER_RIDE, "Water Ride" },
    { CATEGORY_NONE, "N/A" },
};

ride_category_t* ride_category_by_index(int index)
{
    return ride_categories + index;
}

ride_category_t* ride_category_by_name(const char* name)
{
    int i;
    for (i = 0; i < NUM_RIDE_CATEGORIES; i++)
        if (strcmp(ride_categories[i].name, name) == 0)
            return ride_categories + i;
    return NULL;
}
