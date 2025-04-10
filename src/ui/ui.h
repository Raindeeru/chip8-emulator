#pragma once

#include <SDL3/SDL.h>

extern bool debug_mode;

extern SDL_FRect chip8_screen;
extern SDL_FRect instruction_container;
extern SDL_FRect register_container;

void ChangeScreenSize(SDL_Window *win);
void ToggleDebug(SDL_Window *win);
void RenderInstructionContainer(SDL_Renderer* renderer);
void RenderRegisterContainer(SDL_Renderer* renderer);