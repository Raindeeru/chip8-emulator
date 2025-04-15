#include<iostream>
#include <fstream>
#include <ios>
#include <iosfwd>
#include <iostream>
#include <cstdint>
#include<filesystem>
#include <string>
#include "../src/config/database.h"
namespace fs = std::filesystem;

int main(int argc, char *argv[]){
    std::string test = Sha1("test", 4);
    fs::path exe_path = fs::path(argv[0]).parent_path();
    fs::path hashes= exe_path / "database/sha1-hashes.json";
    std::string test_hash = "ea9af3c09b0d9e265fcd92bcc5d51a2939fdf27a";
    int t = GetProgramIndexFromHash(test_hash, hashes);

    std::cout << t;
}