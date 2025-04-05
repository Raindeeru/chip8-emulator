#include "input.h"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_keyboard.h"
#include "SDL3/SDL_scancode.h"
#include "specs.h"
#include <cstdint>
#include <cstdlib>
#include <iostream>

void UpdateKeymap(){
   const bool* currentKeyStates = SDL_GetKeyboardState(NULL);

   if(currentKeyStates[SDL_SCANCODE_1]){
       keymap[0x1] = 1;  
   }else{
       keymap[0x1] = 0;
   }
   if(currentKeyStates[SDL_SCANCODE_2]){
       keymap[0x2] = 1;  
   }else{
       keymap[0x2] = 0;
   }
   if(currentKeyStates[SDL_SCANCODE_3]){
       keymap[0x3] = 1;  
   }else{
       keymap[0x3] = 0;
   }
   if(currentKeyStates[SDL_SCANCODE_4]){
       keymap[0xC] = 1;  
   }else{
       keymap[0xC] = 0;
   }
   if(currentKeyStates[SDL_SCANCODE_Q]){
       keymap[0x4] = 1;  
   }else{
       keymap[0x4] = 0;
   }
   if(currentKeyStates[SDL_SCANCODE_W]){
       keymap[0x5] = 1;  
   }else{
       keymap[0x5] = 0;
   }
   if(currentKeyStates[SDL_SCANCODE_E]){
       keymap[0x6] = 1;  
   }else{
       keymap[0x6] = 0;
   }
   if(currentKeyStates[SDL_SCANCODE_R]){
       keymap[0xD] = 1;  
   }else{
       keymap[0xD] = 0;
   }
   if(currentKeyStates[SDL_SCANCODE_A]){
       keymap[0x7] = 1;  
   }else{
       keymap[0x7] = 0;
   }
   if(currentKeyStates[SDL_SCANCODE_S]){
       keymap[0x8] = 1;  
   }else{
       keymap[0x8] = 0;
   }
   if(currentKeyStates[SDL_SCANCODE_D]){
       keymap[0x9] = 1;  
   }else{
       keymap[0x9] = 0;
   }
   if(currentKeyStates[SDL_SCANCODE_F]){
       keymap[0xE] = 1;  
   }else{
       keymap[0xE] = 0;
   }
   if(currentKeyStates[SDL_SCANCODE_Z]){
       keymap[0xA] = 1;  
   }else{
       keymap[0xA] = 0;
   }
   if(currentKeyStates[SDL_SCANCODE_X]){
       keymap[0x0] = 1;  
   }else{
       keymap[0x0] = 0;
   }
   if(currentKeyStates[SDL_SCANCODE_C]){
       keymap[0xB] = 1;  
   }else{
       keymap[0xB] = 0;
   }
   if(currentKeyStates[SDL_SCANCODE_V]){
       keymap[0xF] = 1;  
   }else{
       keymap[0xF] = 0;
   }

   /*std::cout << "|-|-|-|-|\n"
       << "|"
       << (int)keymap[0x1] << "|"
       << (int)keymap[0x2] << "|"
       << (int)keymap[0x3] << "|"
       << (int)keymap[0xC] << "|"
       << "\n|"
       << (int)keymap[0x4] << "|"
       << (int)keymap[0x5] << "|"
       << (int)keymap[0x6] << "|"
       << (int)keymap[0xD] << "|"
       << "\n|"
       << (int)keymap[0x7] << "|"
       << (int)keymap[0x8] << "|"
       << (int)keymap[0x9] << "|"
       << (int)keymap[0xE] << "|"
       << "\n|"
       << (int)keymap[0xA] << "|"
       << (int)keymap[0x0] << "|"
       << (int)keymap[0xB] << "|"
       << (int)keymap[0xF] << "|\n";
   std::cout << "\033[5F";*/

}
