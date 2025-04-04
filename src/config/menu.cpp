
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <ios>
#include <iosfwd>
#include <iostream>
#include <string>
#include <windows.h>
#include "../chip8/specs.h"
#include "../chip8/fonts.h"
#include "../../res/resource.h"
#include "menu.h"
#include <shobjidl.h> 
#include "../../vendored/nlohmann/json.hpp"
using json = nlohmann::json;
namespace fs = std::filesystem;

std::string current_rom = "";
bool rom_loaded = false;
json current_settings = nullptr;

void LoadRom(){
    std::string rom_path;
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED); 
    if (FAILED(hr)) return;

    IFileOpenDialog* pFileOpen = nullptr;
    hr = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_ALL, IID_PPV_ARGS(&pFileOpen));
    if (SUCCEEDED(hr)) {
        // Set file filters
        COMDLG_FILTERSPEC fileTypes[] = {
            { L"CHIP-8 ROM Files", L"*.ch8;*.rom" },
            { L"All Files", L"*.*" }
        };
        pFileOpen->SetFileTypes(2, fileTypes);

        // Show dialog
        hr = pFileOpen->Show(nullptr);
        if (SUCCEEDED(hr)) {
            // Get the selected file
            IShellItem* pItem;
            hr = pFileOpen->GetResult(&pItem);
            if (SUCCEEDED(hr)) {
                PWSTR pszFilePath;
                pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

                char narrowPath[MAX_PATH];
                WideCharToMultiByte(CP_UTF8, 0, pszFilePath, -1, narrowPath, MAX_PATH, nullptr, nullptr);
                rom_path = narrowPath;

                CoTaskMemFree(pszFilePath);
                pItem->Release();
            }
        }
        pFileOpen->Release();
    }

    CoUninitialize(); 
    current_rom = rom_path;

    char * memblock;
    std::streampos size;

    std::cout<< "Loaded: "<<rom_path <<"\n";
    std::ifstream file (rom_path.c_str(), std::ios::in|std::ios::binary|std::ios::ate);
    if (file.is_open())
    {

        Chip8_Init();
        loadFont(std_font, ram, 80);

        size = file.tellg();
        memblock = new char[size];
        file.seekg (0, std::ios::beg);
        file.read (memblock, size);
        file.close();

        memcpy(ram + 0x200, memblock, size);
        std::cout << "the entire file content is in memory\n";
        rom_loaded = true;


        delete[] memblock;
    }
    else std::cout << "Unable to open file";
}

void LoadRomFromPath(std::string rom_path){
    current_rom = rom_path;
    char * memblock;
    std::streampos size;

    std::cout<< "Loaded: "<<rom_path <<"\n";
    std::ifstream file (rom_path.c_str(), std::ios::in|std::ios::binary|std::ios::ate);
    if (file.is_open())
    {

        Chip8_Init();
        loadFont(std_font, ram, 80);

        size = file.tellg();
        memblock = new char[size];
        file.seekg (0, std::ios::beg);
        file.read (memblock, size);
        file.close();

        memcpy(ram + 0x200, memblock, size);
        std::cout << "the entire file content is in memory\n";
        rom_loaded = true;


        delete[] memblock;
    }
    else std::cout << "Unable to open file";
}

void ParseSettings(fs::path settings_path){
    std::ifstream f(settings_path);
    if (!f) {
        std::cerr << "Could not open settings.json\n";
        return;
    }
    json settings = json::parse(f);
    vf_reset = settings["Quirks"]["vf_reset"];
    memory = settings["Quirks"]["memory"];
    display_wait = settings["Quirks"]["display_wait"];
    clipping = settings["Quirks"]["clipping"];
    shifting = settings["Quirks"]["shifting"];
    jumping = settings["Quirks"]["jumping"];
    current_settings = settings;
}

void UpdateQuirk(int quirk, fs::path settings_path){
    std::ofstream f(settings_path);
    if (!f) {
        std::cerr << "Could not open settings.json\n";
        return;
    }
    switch (quirk)
    {
    case VF_RESET:
        current_settings["Quirks"]["vf_reset"] = !current_settings["Quirks"]["vf_reset"];
        break;
    case MEMORY:
        current_settings["Quirks"]["memory"] = !current_settings["Quirks"]["memory"];
        break;
    case DISPLAY_WAIT: 
        current_settings["Quirks"]["display_wait"] = !current_settings["Quirks"]["display_wait"];
        break;
    case CLIPPING:
        current_settings["Quirks"]["clipping"] = !current_settings["Quirks"]["clipping"];
        break;
    case SHIFTING:
        current_settings["Quirks"]["shifting"] = !current_settings["Quirks"]["shifting"];
        break;
    case JUMPING:
        current_settings["Quirks"]["jumping"] = !current_settings["Quirks"]["jumping"];
        break;
    
    default:
        break;
    }
    f << current_settings.dump(4);
    f.close();
    vf_reset = current_settings["Quirks"]["vf_reset"];
    memory = current_settings["Quirks"]["memory"];
    display_wait = current_settings["Quirks"]["display_wait"];
    clipping = current_settings["Quirks"]["clipping"];
    shifting = current_settings["Quirks"]["shifting"];
    jumping = current_settings["Quirks"]["jumping"];
}
