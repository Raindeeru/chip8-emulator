#include <cstdint>
#include <windows.h>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <ios>
#include <iosfwd>
#include <iostream>
#include <string>
#include "src/chip8/fonts.h"
#include "src/chip8/input.h"
#include "src/chip8/instructions.h"
#include "src/chip8/specs.h"
#include "res/resource.h"
#include "src/config/menu.h"
#include "src/ui/ui.h"
#include <shobjidl.h> 
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>
namespace fs = std::filesystem;

#define PIXEL_SCALE 10
#define FPS 60
#define IPF 11

#define LIGHT 0xFFd7bcad
#define DARK 0xFF452f47

bool running = false;
WNDPROC SDLWndProc = nullptr;
fs::path settings_path;
HMENU menu = NULL;

LRESULT APIENTRY MainWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case ID_LOAD_ROM:
            std::cout << "Load Rom\n";
            LoadRom();
            break;
        case ID_VF_RESET:
            UpdateQuirk(VF_RESET, settings_path);
            if (current_settings["Quirks"]["vf_reset"])
                CheckMenuItem(menu, ID_VF_RESET, MF_CHECKED);
            else
                CheckMenuItem(menu, ID_VF_RESET, MF_UNCHECKED);
            if (current_rom != "")
                LoadRomFromPath(current_rom);
            break;
        case ID_MEMORY:
            UpdateQuirk(MEMORY, settings_path);
            if (current_settings["Quirks"]["memory"])
                CheckMenuItem(menu, ID_MEMORY, MF_CHECKED);
            else
                CheckMenuItem(menu, ID_MEMORY, MF_UNCHECKED);
            if (current_rom != "")
                LoadRomFromPath(current_rom);
            break;
        case ID_DISPLAY_WAIT:
            UpdateQuirk(DISPLAY_WAIT, settings_path);
            if (current_settings["Quirks"]["display_wait"])
                CheckMenuItem(menu, ID_DISPLAY_WAIT, MF_CHECKED);
            else
                CheckMenuItem(menu, ID_DISPLAY_WAIT, MF_UNCHECKED);
            if (current_rom != "")
                LoadRomFromPath(current_rom);
            break;
        case ID_CLIPPING:
            UpdateQuirk(CLIPPING, settings_path);
            if (current_settings["Quirks"]["clipping"])
                CheckMenuItem(menu, ID_CLIPPING, MF_CHECKED);
            else
                CheckMenuItem(menu, ID_CLIPPING, MF_UNCHECKED);
            if (current_rom != "")
                LoadRomFromPath(current_rom);
            break;
        case ID_SHIFTING:
            UpdateQuirk(SHIFTING, settings_path);
            if (current_settings["Quirks"]["shifting"])
                CheckMenuItem(menu, ID_SHIFTING, MF_CHECKED);
            else
                CheckMenuItem(menu, ID_SHIFTING, MF_UNCHECKED);
            if (current_rom != "")
                LoadRomFromPath(current_rom);
            break;
        case ID_JUMPING:
            std::cout << "Hey";
            UpdateQuirk(JUMPING, settings_path);
            if (current_settings["Quirks"]["jumping"])
                CheckMenuItem(menu, ID_JUMPING, MF_CHECKED);
            else
                CheckMenuItem(menu, ID_JUMPING, MF_UNCHECKED);
            if (current_rom != "")
                LoadRomFromPath(current_rom);
            break;

        default:
            break;
        }
        return 0;
    default:
        return CallWindowProc(SDLWndProc, hwnd, uMsg, wParam, lParam);
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    fs::path exe_path = fs::path(argv[0]).parent_path();
    settings_path = exe_path / "settings.json";
    ParseSettings(settings_path);
    MSG msg = {};


    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    TTF_Init();

    //testing the audio
    SDL_AudioSpec spec = {SDL_AUDIO_S16, 22050};

    spec.channels = 1;
    spec.format = SDL_AUDIO_F32;
    spec.freq = 8000;
    SDL_AudioStream *stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, NULL, NULL);
    if (!stream) {
        SDL_Log("Couldn't create audio stream: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    /* SDL_OpenAudioDeviceStream starts the device paused. You have to tell it to start! */
    SDL_ResumeAudioStreamDevice(stream);

    int menu_height = GetSystemMetrics(SM_CYMENU);

    SDL_Window *win = SDL_CreateWindow("Chip-8", 64 * PIXEL_SCALE, menu_height + (32 * PIXEL_SCALE), SDL_WINDOW_OPENGL);
    SDL_SetWindowResizable(win, true);
    SDL_SetWindowMinimumSize(win, 64*PIXEL_SCALE, 32*PIXEL_SCALE);
    
    //For Menu Stuff
    HWND hwnd = (HWND)SDL_GetPointerProperty(SDL_GetWindowProperties(win), SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);
    SDLWndProc = (WNDPROC) GetWindowLongPtr(hwnd, GWLP_WNDPROC);  

    SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)MainWndProc);
    if (hwnd)
    {
        menu = LoadMenu(NULL, MAKEINTRESOURCE(IDR_MENU));
        if(current_settings["Quirks"]["vf_reset"])
            CheckMenuItem(menu, ID_VF_RESET, MF_CHECKED);
        else
            CheckMenuItem(menu, ID_VF_RESET, MF_UNCHECKED);

        if(current_settings["Quirks"]["memory"])
            CheckMenuItem(menu, ID_MEMORY, MF_CHECKED);
        else
            CheckMenuItem(menu, ID_MEMORY, MF_UNCHECKED);

        if(current_settings["Quirks"]["display_wait"])
            CheckMenuItem(menu, ID_DISPLAY_WAIT, MF_CHECKED);
        else
            CheckMenuItem(menu, ID_DISPLAY_WAIT, MF_UNCHECKED);

        if(current_settings["Quirks"]["clipping"])
            CheckMenuItem(menu, ID_CLIPPING, MF_CHECKED);
        else
            CheckMenuItem(menu, ID_CLIPPING, MF_UNCHECKED);

        if(current_settings["Quirks"]["shifting"])
            CheckMenuItem(menu, ID_SHIFTING, MF_CHECKED);
        else
            CheckMenuItem(menu, ID_SHIFTING, MF_UNCHECKED);

        if(current_settings["Quirks"]["jumping"])
            CheckMenuItem(menu, ID_JUMPING, MF_CHECKED);
        else
            CheckMenuItem(menu, ID_JUMPING, MF_UNCHECKED);

        SetMenu(hwnd, menu);
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(win, NULL);

    SDL_Texture *tex = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        64,
        32);
    
    SDL_SetTextureScaleMode(tex, SDL_SCALEMODE_NEAREST);

    SDL_Texture *tex_hires = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        128,
        64);
    
    SDL_SetTextureScaleMode(tex_hires, SDL_SCALEMODE_NEAREST);

    if (!win)
    {
        std::cout << "Failed to create window! Error: " << SDL_GetError() << "\n";
    }
    running = true;

    uint32_t pixel_display[64 * 32] = {0};
    uint32_t pixel_display_hires[128*64] = {0};

    int32_t tickInterval = 1000 / FPS;
    uint32_t lastUpdateTime = 0;
    int32_t deltaTime = 0;

    DecodeOpcode((uint16_t)0xABCD);

    int count = 0;
    int current_sine_sample = 0;


    while (running)
    {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

        }
        // check if exiting
        SDL_Event event;
        if (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_EVENT_QUIT:
                running = false;
                break;
            case SDL_EVENT_WINDOW_RESIZED:
                ChangeScreenSize(win);
                break;
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                ToggleDebug(win);
                break;
            default:
                break;
            }
        }


        if(!rom_loaded) {

            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderPresent(renderer);
            continue;
        }
        // time control
        uint32_t currentTime = SDL_GetTicks();
        deltaTime = currentTime - lastUpdateTime;

        int32_t timeToSleep = tickInterval - deltaTime;

        if (timeToSleep > 0)
        {
            SDL_Delay(timeToSleep);
        }

        lastUpdateTime = currentTime;

        // Input
        UpdateKeymap();

        // Delay and Sound
        if (delay > 0)
            delay--;
        if (sound > 0 || (key_held && sound_on_press)){
            const int samples_per_frame = 8000/60; 
            float samples[samples_per_frame];
            //I just got this code straight from the example that SDL gave but replaced the sin wave with a square wave
            for (int i = 0; i < SDL_arraysize(samples); i++)
            {
                const int freq = 440;
                const float phase = current_sine_sample * freq / 8000.0f;
                samples[i] = 4* floorf(phase) - 2 * floorf(2*phase) + 1;
                current_sine_sample++;
            }
            current_sine_sample %= 8000;

            SDL_PutAudioStreamData(stream, samples, sizeof(samples));
            if(sound > 0)
                sound--;
        }else{
            SDL_ClearAudioStream(stream);
        }

        // fetch decode execute logic here
        // fetch

        for (int i = 0; i < IPF; i++)
        {
            if (display_flag && display_wait)
                break;
            uint16_t opcode = 0x0;
            uint16_t program_counter = (uint16_t)pc; //debug
            opcode += ram[pc] << 8;
            opcode += ram[pc + 1];

            pc += 2;
            if(superchip_mode)
            {
                DecodeOpcodeSuperChip(opcode);
            }
            else
                DecodeOpcode(opcode);
        }

        memcpy(keystate, keymap, sizeof(keymap));

        // render stufff here
        display_flag = 0;
        void *rawPixels = NULL;
        int pitch = 0;

        if (superchip_mode)
        {
            for (int i = 0; i < 128 * 64; i++)
            {
                pixel_display_hires[i] = display_hires[i] > 0 ? LIGHT : DARK;
            }
            if (SDL_LockTexture(tex_hires, NULL, &rawPixels, &pitch) == false)
            {
                std::cout << "Could not lock texture! " << SDL_GetError() << "\n";
                return 1;
            }

            memcpy((uint8_t *)rawPixels, pixel_display_hires, sizeof(uint32_t) * 128 * 64);
            SDL_UnlockTexture(tex_hires);

            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);
            SDL_RenderTexture(renderer, tex_hires, NULL, &chip8_screen);
        }
        else
        {
            for (int i = 0; i < 64 * 32; i++)
            {
                pixel_display[i] = display[i] > 0 ? LIGHT : DARK;
            }
            if (SDL_LockTexture(tex, NULL, &rawPixels, &pitch) == false)
            {
                std::cout << "Could not lock texture! " << SDL_GetError() << "\n";
                return 1;
            }

            memcpy((uint8_t *)rawPixels, pixel_display, sizeof(uint32_t) * 64 * 32);
            SDL_UnlockTexture(tex);

            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);
            SDL_RenderTexture(renderer, tex, NULL, &chip8_screen);
        }

        SDL_FRect rect_test = {
            .x = 0,
            .y =0,
            .w = 50,
            .h = 50
        };
        
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(renderer, &rect_test);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

        if (debug_mode)
        {
            RenderInstructionContainer(renderer);
            RenderRegisterContainer(renderer);
        }
        

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

        // Debug stuff here
        std::string debug = std::to_string(ram[pc]);
        if (!(SDL_RenderDebugText(renderer, 10, 10, debug.c_str())))
        {
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
