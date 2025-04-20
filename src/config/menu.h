#ifndef MENU_H

#define MENU_H

#include <string>
#include <windows.h>
#include <vector>
#include "../../vendored/nlohmann/json.hpp"
using json = nlohmann::json;
namespace fs = std::filesystem;

#define VF_RESET 0
#define MEMORY 1
#define DISPLAY_WAIT 2
#define CLIPPING 3
#define SHIFTING 4
#define JUMPING 5

extern bool rom_loaded;
extern std::string current_rom;
extern json current_settings;
extern std::vector<std::string> recent_files;

void LoadRom();
void LoadRomFromPath(std::string rom_path);
void ParseSettings(fs::path settings_path);
void UpdateQuirk(int quirk, fs::path settings_path, bool state);
void AddRecentFiles(HMENU menubar);
void SaveConfig(fs::path settings_path);
void SaveFlags(fs::path flags_path);
void LoadFlags(fs::path flags_path);
#endif 