#include <SDL3/SDL.h>
#include <iostream>

bool debug_mode = false;

//This rect's position will always be 0,0 and grow depending on the window size
SDL_FRect chip8_screen = {
    .x = 0,
    .y = 0,
    .w = 640,
    .h = 320,
};

//one of their 2 dimensions will stay fixed and the other will grow and shrink
SDL_FRect instruction_container = {
    .x = 0,
    .y = 0,
    .w = 200,
    .h = 320,
};
SDL_FRect register_container = {
    .x = 0,
    .y = 0,
    .w = 640,
    .h = 150,
};

void ChangeScreenSize(SDL_Window *win)
{
    int height;
    int width;
    SDL_GetWindowSize(win, &width, &height);
    if(debug_mode){
        height -= register_container.h;
        width -= instruction_container.w;

        //instruction change size
        instruction_container.x  = width;
        instruction_container.h = height;
        
        //register change size
        register_container.y = height;
        register_container.w = width + instruction_container.w;

    }
    if ((float)width / height > 2)
    {
        // screen is wide
        chip8_screen.h = height;
        chip8_screen.w = chip8_screen.h * 2;

        chip8_screen.y = 0;
        chip8_screen.x = (width - chip8_screen.w) / 2;
    }
    else
    {
        // screen is tall
        chip8_screen.w = width;
        chip8_screen.h = chip8_screen.w / 2;

        chip8_screen.x = 0;
        chip8_screen.y = (height - chip8_screen.h) / 2;
    }
}

void ToggleDebug(SDL_Window* win){
    debug_mode = !debug_mode;
    if(debug_mode){
        int width = 0;
        int height = 0;

        SDL_GetWindowSize(win, &width, &height);

        SDL_SetWindowMinimumSize(win, 640 + instruction_container.w, 320 + register_container.h);
        SDL_SetWindowSize(win, width + instruction_container.w, height + register_container.h);
    }else{
        int width = 0;
        int height = 0;

        SDL_GetWindowSize(win, &width, &height);

        SDL_SetWindowMinimumSize(win, 640, 320);
        SDL_SetWindowSize(win, width - instruction_container.w, height - register_container.h);
    }
}

void RenderInstructionContainer(SDL_Renderer* renderer){
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &instruction_container);
    SDL_SetRenderDrawColor(renderer, 0,0,0,255);
}

void RenderRegisterContainer(SDL_Renderer* renderer){
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderFillRect(renderer, &register_container);
    SDL_SetRenderDrawColor(renderer, 0,0,0,255);
}