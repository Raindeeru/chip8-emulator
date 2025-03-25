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
#include <fstream>
#include <ios>
#include <iosfwd>
#include <iostream>
#include <string>
#include "chip8/fonts.h"
#include "chip8/instructions.h"
#include "chip8/specs.h"

#define PIXEL_SCALE 10
#define FPS 700

void LoadRom(){
    char * memblock;
    std::streampos size;

    std::ifstream file ("ibm.ch8", std::ios::in|std::ios::binary|std::ios::ate);
    if (file.is_open())
    {
        size = file.tellg();
        memblock = new char[size];
        file.seekg (0, std::ios::beg);
        file.read (memblock, size);
        file.close();

        std::cout << "the entire file content is in memory\n";

        memcpy(ram + 0x200, memblock, size);

        delete[] memblock;
    }
    else std::cout << "Unable to open file";
}

int main(int argc, char *argv[]){
    Chip8_Init(); 
    loadFont(std_font, ram, 80);
    LoadRom();

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
    
    int32_t tickInterval = 1000/FPS;
    uint32_t lastUpdateTime = 0;
    int32_t deltaTime = 0;
    
    DecodeOpcode((uint16_t)0xABCD);

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
        
        lastUpdateTime = currentTime;

        //fetch decode execute logic here
        //fetch
       
        uint16_t opcode = 0x0;
        opcode += ram[pc] << 8;
        opcode += ram[pc+1];
        std::cout << std::hex << opcode << "\n";

        pc += 2;
        if(pc > 0x1000) pc = 0x200;
        //decode execute
        DecodeOpcode(opcode);

        //render stufff here
        void* rawPixels = NULL;
        int pitch = 0;

        for(int i = 0; i < 64*32; i++){
            pixel_display[i] = display[i] > 0 ? 0xFFFFFFFF : 0x00000000;
        }
        if(SDL_LockTexture(tex, NULL, &rawPixels, &pitch) == false){
            std::cout << "Could not lock texture! " << SDL_GetError() << "\n";
            return 1;
        }

        memcpy((uint8_t*)rawPixels, pixel_display, sizeof(uint32_t)*64*32);
        SDL_UnlockTexture(tex);
         
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_RenderTexture(renderer, tex, NULL, NULL);
        
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        
        //Debug stuff here
        std::string debug = std::to_string(pc);
        if( !(SDL_RenderDebugText(renderer, 10, 10, debug.c_str())) ){
            std::cout << "Could not render debug test " << SDL_GetError() << "\n"; 
        }

        SDL_RenderPresent(renderer);

    }

    SDL_DestroyWindow(win);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyTexture(tex);
    win = NULL;

    SDL_Quit();
    return 0;
}
