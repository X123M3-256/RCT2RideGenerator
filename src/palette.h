#ifndef PALETTE_H_INCLUDED
#define PALETTE_H_INCLUDED
#include <stdint.h>
#define PALETTE_SIZE 255

#define NUMBER_OF_COLORS 44

#define COLOR_PEEP_SKIN 32
#define COLOR_PEEP_LEG 33
#define COLOR_PEEP_REMAP_1 34
#define COLOR_PEEP_REMAP_2 35

#define COLOR_REMAP_1 36
#define COLOR_REMAP_2 37
#define COLOR_REMAP_3 38

#define TRANSPARENT 39
#define BLACKTILE 40

#define NEON_REMAP_1 41
#define NEON_REMAP_2 42
#define NEON_REMAP_3 43

typedef struct {
    uint8_t blue;
    uint8_t green;
    uint8_t red;
    uint8_t reserved;
} color_t;

uint8_t palette_remap_section_index(uint8_t color, uint8_t section_index);
color_t palette_color_from_index(uint8_t index);
uint8_t palette_index_from_color(color_t color);
#endif // PALETTE_H_INCLUDED
