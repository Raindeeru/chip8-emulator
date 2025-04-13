#include "specs.h"

#include <cstdint>
#include <cstring>
std::uint8_t ram[4096] = {0};

std::uint16_t pc;
std::uint16_t I;

std::uint16_t stack[16] = {0};
std::int8_t sp;

std::uint8_t display[64 * 32] = {0};
std::uint8_t display_hires[128 * 64] = {0}; // For SuperChip
std::uint8_t keymap[16] = {0};
std::uint8_t keystate[16] = {0};

std::uint8_t delay;
std::uint8_t sound;

std::uint8_t display_flag;

// Registers
std::uint8_t V[16] = {0};

//Superchip stuff
bool superchip_mode = true;
bool hires = false;
uint8_t flags[8] = {0};


//Quirks
bool vf_reset = false;
bool memory = true;
bool display_wait = true;
bool clipping = true;
bool shifting = false;
bool jumping = false;

bool sound_on_press = false;

//Debug Stuff
bool debug_mode = false;
bool paused = false;
bool stepped = false;

void Chip8_Init() {
    memset(ram, 0, sizeof(ram));
    memset(stack, 0, sizeof(stack));
    memset(display, 0, sizeof(display));
    memset(display_hires, 0, sizeof(display_hires));
    memset(keymap, 0, sizeof(keymap));
    memset(keystate, 0, sizeof(keystate));
    pc = 0x200;
    display_flag = 0;
    I = 0x0;
    delay = 0;
    sound = 0;
    sp = -1;
    hires = false;
}
