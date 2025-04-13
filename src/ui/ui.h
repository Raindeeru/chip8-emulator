#pragma once

#include <SDL3/SDL.h>

extern bool debug_mode;

extern SDL_FRect chip8_screen;
extern SDL_FRect instruction_container;
extern SDL_FRect register_container;

void RenderChip8Screen(SDL_Window* win, bool rom_loaded, SDL_Texture* tex = NULL);
void ToggleDebug(SDL_Window *win);