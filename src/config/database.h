#pragma once
#include <cstdint>
#include <string>
#include <fstream>
namespace fs = std::filesystem;

std::string Sha1(const char *message, uint64_t l);
int GetProgramIndexFromHash(std::string hash, fs::path hashes_path);