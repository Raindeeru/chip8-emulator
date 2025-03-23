#include "specs.h"

#include <cstdint>
std::uint8_t ram[4096] = {0};

std::uint8_t pc;
std::uint16_t I;

std::uint16_t stack[2] = {0};

std::uint8_t display[64 * 32] = {0};

std::uint8_t delay;
std::uint8_t sound;

// Registers
std::uint8_t V0;
std::uint8_t V1;
std::uint8_t V2;
std::uint8_t V3;
std::uint8_t V4;
std::uint8_t V5;
std::uint8_t V6;
std::uint8_t V7;
std::uint8_t V8;
std::uint8_t V9;
std::uint8_t VA;
std::uint8_t VB;
std::uint8_t VC;
std::uint8_t VD;
std::uint8_t VE;
std::uint8_t VF;

void Chip8_Init() {
  pc = 0;
  delay = 0;
  sound = 0;

  V0 = 0;
  V1 = 0;
  V2 = 0;
  V3 = 0;
  V4 = 0;
  V5 = 0;
  V6 = 0;
  V7 = 0;
  V8 = 0;
  V9 = 0;
  VA = 0;
  VB = 0;
  VC = 0;
  VD = 0;
  VE = 0;
  VF = 0;
}
