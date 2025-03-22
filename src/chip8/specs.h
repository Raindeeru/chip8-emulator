#ifndef SPECS_H
#define SPECS_H

#include <cstdint>
extern std::uint8_t ram[4096];
extern std::uint8_t pc;
extern std::uint16_t I;
extern std::uint16_t stack[2];
extern std::uint8_t delay;
extern std::uint8_t sound;

//Registers
extern std::uint8_t V0;
extern std::uint8_t V1;
extern std::uint8_t V2;
extern std::uint8_t V3;
extern std::uint8_t V4;
extern std::uint8_t V5;
extern std::uint8_t V6;
extern std::uint8_t V7;
extern std::uint8_t V8;
extern std::uint8_t V9;
extern std::uint8_t VA;
extern std::uint8_t VB;
extern std::uint8_t VC;
extern std::uint8_t VD;
extern std::uint8_t VE;
extern std::uint8_t VF;

void Chip8_Init();

#endif
