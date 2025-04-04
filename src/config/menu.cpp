
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

std::string current_rom;
bool rom_loaded = false;

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

void ParseSettings(char* argv[]){
    std::cout << "Hi";
    fs::path exe_path = fs::path(argv[0]).parent_path();
    fs::path settings_path = exe_path / "settings.json";
    std::ifstream f(settings_path);
    if (!f) {
        std::cerr << "Could not open settings.json\n";
        return;
    }
    std::cout << "Hello";
    json settings = json::parse(f);
    bool x = settings.contains("Quirks"); 
    std::cout << settings["Quirks"]["vf_reset"];
}