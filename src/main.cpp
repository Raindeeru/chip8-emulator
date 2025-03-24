#include "SDL3/SDL_error.h"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_oldnames.h"
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_scancode.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_surface.h"
#include "SDL3/SDL_timer.h"
#include "SDL3/SDL_video.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include "chip8/fonts.h"
#include "chip8/specs.h"

#define PIXEL_SCALE 10
#define FPS 60

int main(int argc, char *argv[]){
    Chip8_Init(); 
    loadFont(std_font, ram, 80);

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Init(SDL_INIT_EVENTS);
    
    SDL_Window* win = SDL_CreateWindow("Chip-8", 64*PIXEL_SCALE, 32*PIXEL_SCALE, SDL_WINDOW_OPENGL);
    SDL_Renderer* renderer = SDL_CreateRenderer(win, NULL);

    SDL_Texture* tex = SDL_CreateTexture(
            renderer, 
            SDL_PIXELFORMAT_ARGB8888, 
            SDL_TEXTUREACCESS_STREAMING, 
            64, 
            32);
   SDL_SetTextureScaleMode(tex, SDL_SCALEMODE_NEAREST); 

    if (!win){
        std::cout<<"Failed to create window! Error: " << SDL_GetError() << "\n";
    }
    bool running = true;

    uint32_t pixel_display[64*32] = {0};
    
    int white_point = 0;

    int32_t tickInterval = 1000/FPS;
    uint32_t lastUpdateTime = 0;
    int32_t deltaTime = 0;

    while(running){
        //check if exiting
        SDL_Event event;
        if(SDL_PollEvent(&event)){
            switch (event.type) {
                case SDL_EVENT_QUIT:
                    running = false;
                    break;
                default:
                    break;
            } 
        }
        //time control
        uint32_t currentTime = SDL_GetTicks();
        deltaTime = currentTime - lastUpdateTime;

        int32_t timeToSleep = tickInterval - deltaTime;

        if(timeToSleep > 0){
            SDL_Delay(timeToSleep);
        }
        
        //render stufff here
       
        void* rawPixels = NULL;
        int pitch = 0;

        if(SDL_LockTexture(tex, NULL, &rawPixels, &pitch) == false){
            std::cout << "Could not lock texture! " << SDL_GetError() << "\n";
            return 1;
        }

        for(int i = 0; i < 64*32; i++){
            if(i == white_point){
                pixel_display[i] = 0xFFFFFFFF;
            }else{
                pixel_display[i] = 0x0;
            }
        }


        memcpy((uint8_t*)rawPixels, pixel_display, sizeof(uint32_t)*64*32);
        SDL_UnlockTexture(tex);
        white_point ++;
        white_point %= 64*32;
         
        //putting pixels to the renderere

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_RenderTexture(renderer, tex, NULL, NULL);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        
        //Debug stuff here
        std::string debug = "Test Text";
        if(!(SDL_RenderDebugText(renderer, 10, 10, debug.c_str()))){
            std::cout << "Could not render debug test " << SDL_GetError() << "\n"; 
        }
        SDL_RenderPresent(renderer);

        lastUpdateTime = currentTime;
    }

    SDL_DestroyWindow(win);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyTexture(tex);
    win = NULL;

    SDL_Quit();
    return 0;
}
