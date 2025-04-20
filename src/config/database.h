#pragma once
#include <cstdint>
#include <string>
#include <fstream>
namespace fs = std::filesystem;

std::string Sha1(const char *message, uint64_t l);
int GetProgramIndexFromHash(std::string hash);
void LoadDatabaseJson(fs::path database_folder_path);
std::string GetProgramPlatform(int program_index, std::string hash);
std::string GetProgramTitle(int program_index);
void UpdatePlatformQuirks(std::string platform, fs::path settings_path);
void UpdatePlatform(std::string platform);