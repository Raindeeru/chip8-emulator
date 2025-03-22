#include "SDL3/SDL_error.h"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_surface.h"
#include "SDL3/SDL_video.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <iostream>

#define PIXEL_SCALE 10

int main(int argc, char *argv[]){
   SDL_Init(SDL_INIT_VIDEO);
   SDL_Init(SDL_INIT_EVENTS);

   SDL_Window* win = SDL_CreateWindow("Chip-8", 64*PIXEL_SCALE, 32*PIXEL_SCALE, SDL_WINDOW_OPENGL);
   if (!win){
       std::cout<<"Failed to create window! Error: " << SDL_GetError() << "\n";
   }
   SDL_Surface* winSurface = SDL_GetWindowSurface(win);
   const SDL_PixelFormatDetails* winFormatDetails = SDL_GetPixelFormatDetails(winSurface->format);
   SDL_FillSurfaceRect(winSurface, NULL, SDL_MapRGB(winFormatDetails, NULL, 255, 255, 255));
   
   bool running = true;

   while(running){
       SDL_Event event;
       if(SDL_PollEvent(&event)>0){
              switch (event.type) {
                  case SDL_EVENT_KEY_DOWN:
                      SDL_log(event.key.key);
                      std::cout << event.key.key << "\n" << event.key.scancode << "\n";
                      SDL_log(event.key.scancode);
                      running = false;
                      break;
               } 
    }
   }
   
   SDL_DestroyWindow(win);
   win = NULL;
   winSurface = NULL;

   SDL_Quit();
   return 0;
}
