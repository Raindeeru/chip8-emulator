#ifndef MENU_H

#define MENU_H

#include <string>
#include <windows.h>

extern bool rom_loaded;
extern std::string current_rom;

void LoadRom();
void LoadRomFromPath(std::string rom_path);
void ParseSettings(char* argv[]);
#endif 