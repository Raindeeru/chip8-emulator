
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
#include <vector>
using json = nlohmann::json;
namespace fs = std::filesystem;

std::string current_rom = "";
bool rom_loaded = false;
json current_settings = nullptr;

std::vector<std::string> recent_files = {};

void LoadRom(){
    std::string rom_path;
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED); 
    if (FAILED(hr)) return;

    IFileOpenDialog* pFileOpen = nullptr;
    hr = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_ALL, IID_PPV_ARGS(&pFileOpen));
    if (SUCCEEDED(hr)) {
        // Set file filters
        COMDLG_FILTERSPEC fileTypes[] = {
            { L"All Files", L"*.*" }
        };
        pFileOpen->SetFileTypes(1, fileTypes);

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


        std::cout << size;
        memcpy(ram + 0x200, memblock, size);
        std::cout << "the entire file content is in memory\n";
        rom_loaded = true;


        delete[] memblock;
        
        for(int i = 0; i < recent_files.size(); i++){
            if (recent_files[i] == rom_path)
            {
                recent_files.erase(recent_files.begin() + i);
            }
            
        }
        recent_files.insert(recent_files.begin(), rom_path);
        if(recent_files.size() > 10){
            recent_files.pop_back();
        }

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

        for(int i = 0; i < recent_files.size(); i++){
            if (recent_files[i] == rom_path)
            {
                recent_files.erase(recent_files.begin() + i);
            }
            
        }
        recent_files.insert(recent_files.begin(), rom_path);
        if(recent_files.size() > 10){
            recent_files.pop_back();
        }
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
    recent_files = settings["RecentFiles"];
    mode = settings["Mode"];

    current_settings = settings;
}

void UpdateQuirk(int quirk, fs::path settings_path, bool state){
    std::ofstream f(settings_path);
    if (!f) {
        std::cerr << "Could not open settings.json\n";
        return;
    }
    switch (quirk)
    {
    case VF_RESET:
        current_settings["Quirks"]["vf_reset"] = state;
        break;
    case MEMORY:
        current_settings["Quirks"]["memory"] = state;
        break;
    case DISPLAY_WAIT: 
        current_settings["Quirks"]["display_wait"] = state;
        break;
    case CLIPPING:
        current_settings["Quirks"]["clipping"] = state;
        break;
    case SHIFTING:
        current_settings["Quirks"]["shifting"] = state; 
        break;
    case JUMPING:
        current_settings["Quirks"]["jumping"] = state; 
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

void AddRecentFiles(HMENU menubar){
    if(recent_files.size() == 0){
        return;
    }
    HMENU file = GetSubMenu(menubar, 0);
    HMENU recent_files_menu = GetSubMenu(file, 1);

    if (!recent_files_menu)
    {
        MessageBoxW(NULL, L"Recent Roms submenu not found!", L"Error", MB_OK);
        return;
    }

    for (int i = 0; i < recent_files.size(); i++)
    {
        RemoveMenu(recent_files_menu, 0, MF_BYPOSITION);
    }

    for (int i = 0; i < recent_files.size(); i++)
    {
        std::string narrow = recent_files[i];
        std::wstring wide(narrow.begin(), narrow.end());
        BOOL success = AppendMenuW(recent_files_menu, MF_STRING, ID_RECENT_START+i, wide.c_str());
        if (!success)
        {
            MessageBoxW(NULL, L"AppendMenu failed", L"Error", MB_OK | MB_ICONERROR);
        }
    }
    
}

void SaveConfig(fs::path settings_path){
    std::cout << "Saving to: " << settings_path << '\n';
    std::ofstream f(settings_path);
    if (!f) {
        std::cerr << "Could not open settings.json\n";
        return;
    }
    if(recent_files.size() != 0){
        current_settings["RecentFiles"] = recent_files;
    }

    current_settings["Mode"] = mode;

    f << current_settings.dump(4);
    f.close();
}

void LoadFlags(fs::path flags_path){
    std::ifstream f(flags_path);
    if (!f) {
        std::cerr << "Could not open flags.json\n";
        return;
    }
    json json_flags = json::parse(f);

    if (!json_flags.is_array()){
        std::cerr << "Wrong Format in Settings, Cannot Load";
        f.close();
        return;
    }

    for (size_t i = 0; i <8; ++i) {
        int val = json_flags[i].get<int>();
        if (val < 0 || val > 255) {
            std::cerr << "Value out of range for uint8_t at index " << i << ": " << val << "\n";
            f.close();
            return;
        }
        flags[i] = static_cast<uint8_t>(val);
    }
    f.close();
}

void SaveFlags(fs::path flags_path){
    std::ofstream f(flags_path);
    if (!f) {
        std::cerr << "Could not open flags.json\n";
        return;
    }
    json json_flags = flags;

    f << json_flags.dump(4);
    f.close();
}