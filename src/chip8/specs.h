#ifndef SPECS_H
#define SPECS_H

#include <cstdint>
extern std::uint8_t ram[4096];

extern std::uint16_t pc;
extern std::uint16_t I;

extern std::uint16_t stack[16];
extern std::int8_t sp;

extern std::uint8_t display[64 * 32];
extern std::uint8_t display_hires[128 * 64]; //For SuperChip
extern std::uint8_t keymap[16];
extern std::uint8_t keystate[16];

extern std::uint8_t delay;
extern std::uint8_t sound;

extern std::uint8_t display_flag;


//Registers
extern std::uint8_t V[16];

//Superchip stuff
extern bool superchip_mode;
extern bool hires;
extern uint8_t flags[8];


//Quirks
extern bool vf_reset;
extern bool memory;
extern bool display_wait;
extern bool clipping;
extern bool shifting;
extern bool jumping;

//Other Options
extern bool sound_on_press;

//Debug Options
extern bool debug_mode;
extern bool paused;
extern bool stepped;

void Chip8_Init();

#endif
