#ifndef FONTS_H
#define FONTS_H

#include <cstdint>

extern uint8_t std_font[80];

void loadFont(uint8_t font[], uint8_t ram[], int size);

#endif
