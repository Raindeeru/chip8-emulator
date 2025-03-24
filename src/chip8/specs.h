#ifndef SPECS_H
#define SPECS_H

#include <cstdint>
extern std::uint8_t ram[4096];

extern std::uint16_t pc;
extern std::uint16_t I;

extern std::uint16_t stack[2];

extern std::uint8_t display[64 * 32];

extern std::uint8_t delay;
extern std::uint8_t sound;


//Registers
extern std::uint8_t V[16];

void Chip8_Init();

#endif
