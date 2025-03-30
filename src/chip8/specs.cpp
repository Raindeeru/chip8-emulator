#include "specs.h"

#include <cstdint>
std::uint8_t ram[4096] = {0};

std::uint16_t pc;
std::uint16_t I;

std::uint16_t stack[16] = {0};
std::uint8_t sp = 0;

std::uint8_t display[64 * 32] = {0};

std::uint8_t delay;
std::uint8_t sound;

// Registers
std::uint8_t V[16] = {0};

void Chip8_Init() {
  pc = 0x200;
  I = 0x0;
  delay = 0;
  sound = 0;

}
