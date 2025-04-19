#include <SDL3/SDL.h>
#include <iostream>
#include "../../vendored/ImGui/imgui.h"
#include "../chip8/specs.h"
#include "../../res/resource.h"

//Left Side of Screen
#define RAM_WIDTH 200
//Right Side of Screen
#define REGISTER_WIDTH 100
#define FLAG_WIDTH 100

#define DEBUG_ADDED_WIDTH (RAM_WIDTH + REGISTER_WIDTH + FLAG_WIDTH)
#define DEBUG_ADDED_HEIGHT 200

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

void RenderChip8Screen(SDL_Window* win, bool rom_loaded, SDL_Texture* tex = NULL){
        int width = 0, height = 0;
        SDL_GetWindowSize(win, &width, &height);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
        
        ImGui::Begin("Main Screen", NULL, 
            ImGuiWindowFlags_NoDecoration |
            ImGuiWindowFlags_NoInputs |
            ImGuiWindowFlags_NoNav
        );

        ImGui::SetWindowSize(
            ImVec2(width, height)
        );
        ImGui::SetWindowPos(ImVec2(0,0));

        if(debug_mode){
            ImGui::BeginChild("Instructions", ImVec2(RAM_WIDTH,height),
                ImGuiChildFlags_Border
            );

            ImGui::Text("RAM");

            ImGui::Separator();

            if(rom_loaded){
                ImVec2 remaining = ImGui::GetContentRegionAvail();
                float row_height = ImGui::GetTextLineHeightWithSpacing() + 4;
                int row_count = (int)std::floor(remaining.y / row_height);

                ImGui::BeginChild("Table", remaining);

                if (ImGui::BeginTable("Ram", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
                {
                    for (int row = 0; row < row_count; ++row)
                    {
                        int current_address = pc + (row*2);
                        int current_opcode = (ram[current_address] << 8) + ram[current_address+1];
                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text(((row==0)?"> 0x%04x":"0x%04x"), current_address);
                        ImGui::TableSetColumnIndex(1);
                        ImGui::Text("0x%04x", current_opcode);
                    }

                    ImGui::EndTable();
                }
                ImGui::EndChild();
            }

            ImGui::EndChild();
            ImGui::SameLine();
        }
        {
            if (debug_mode)
            {
                ImGui::BeginChild("Chip8 Screen",
                                  ImVec2(width - DEBUG_ADDED_WIDTH, height),
                                  ImGuiChildFlags_Border);
            }else{
                ImGui::BeginChild("Chip8 Screen",
                                  ImVec2(width, height),
                                  ImGuiChildFlags_Border);
            }
            
            if (rom_loaded)
            {
                int image_x = 0;
                int image_y = 0;
                int image_w = 0;
                int image_h = 0;

                int available_width = width - (debug_mode ? DEBUG_ADDED_WIDTH : 0);
                // window is wide
                if ((float)width / (float)height > 2)
                {
                    image_w = height * 2;
                    image_h = height;
                    image_y = 0;
                    image_x = available_width/2 - image_h;
                }
                else
                {
                    image_w = available_width;
                    image_h = available_width / 2;
                    image_y = (height - image_h) / 2;
                    image_x = 0;
                }

                ImGui::SetCursorPos(ImVec2(image_x, image_y));
                ImGui::Image((ImTextureID)(intptr_t)tex, ImVec2(image_w, image_h));

                ImGui::SetCursorPos(ImVec2(0,0));
                ImGui::Text((paused?"Paused":""));
            }
            ImGui::EndChild();
        }

        if(debug_mode){
            //Registers
            ImGui::SameLine();
            ImGui::BeginChild("Registers", ImVec2(REGISTER_WIDTH,height),
                ImGuiChildFlags_Border
            );

            ImGui::Indent(5.0f);
            ImGui::Text("Registers");
            ImGui::Unindent(5.0f);

            ImGui::Separator();
            
            if (rom_loaded)
            {
                ImGui::Indent(5.0f);
                for (int i = 0; i < 16; i++)
                {
                    ImGui::Text("V%x: 0x%04x", i, V[i]);
                }
                ImGui::Unindent(5.0f);
            }

            ImGui::EndChild();
            ImGui::SameLine();

            //User Flags in SuperChip 1.1

            ImGui::BeginChild("Flags", ImVec2(FLAG_WIDTH,height),
                ImGuiChildFlags_Border
            );

            ImGui::Indent(5.0f);
            ImGui::Text("Flags");
            ImGui::Unindent(5.0f);

            ImGui::Separator();

            ImGui::Indent(5.0f);

            if(mode == ID_SUPERCHIP_LEGACY || mode == ID_SUPERCHIP_MODERN && rom_loaded){
                for (int i = 0; i < 8; i++)
                {
                    ImGui::Text("F%x: 0x%02x", i, flags[i]);
                }
            }else{
                ImGui::TextWrapped(
                    (rom_loaded ? "User Flags Only used in SuperChip" : "")
                );
            }

            ImGui::Unindent(5.0f);

            ImGui::EndChild();
            ImGui::SameLine();
        }

        ImGui::PopStyleVar(3);
        ImGui::End();
}

void ToggleDebug(SDL_Window* win){
    debug_mode = !debug_mode;
    if(debug_mode){
        int width = 0;
        int height = 0;

        SDL_GetWindowSize(win, &width, &height);

        SDL_SetWindowMinimumSize(win, 640 + DEBUG_ADDED_WIDTH, 320);
        SDL_SetWindowSize(win, width + DEBUG_ADDED_WIDTH, height);
    }else{
        int width = 0;
        int height = 0;

        SDL_GetWindowSize(win, &width, &height);

        SDL_SetWindowMinimumSize(win, 640, 320);
        SDL_SetWindowSize(win, width - DEBUG_ADDED_WIDTH, height);
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