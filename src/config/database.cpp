#include "database.h"
#include <iostream>
#include <fstream>
#include <ios>
#include <iosfwd>
#include <iostream>
#include <string>
#include <cstdint>
#include <sstream>
#include "../../vendored/nlohmann/json.hpp"
#include "menu.h"
using json = nlohmann::json;

namespace fs = std::filesystem;
#define LENGTH_BYTE_SIZE 8

json hashes;
json programs;
json quirks;
json platforms;

bool json_loaded = false;

uint32_t to_uint32_be(const uint8_t* bytes) {
    return (uint32_t)bytes[0] << 24 |
           (uint32_t)bytes[1] << 16 |
           (uint32_t)bytes[2] << 8  |
           (uint32_t)bytes[3];
}
char* to_char(uint32_t bytes) {
    char* byte_array = (char*)malloc(4);
    byte_array[0] = (char)(0x000F & (bytes >> 24));
    byte_array[1] = (char)(0x000F & (bytes >> 16));
    byte_array[2] = (char)(0x000F & (bytes >> 8));
    byte_array[3] = (char)(0x000F & bytes);

    return byte_array;
}

//Word is 32 bits (4 chars long)
uint32_t CircularLeftShift(uint32_t word, int shift_amount){
    return (word << shift_amount) | (word >> (32 - shift_amount));
}

//Sha 1 with data being a character string and l being the size in bytes, and it is a multiple of 8
std::string Sha1(const char *message, uint64_t l){
    uint32_t h0 = 0x67452301;
    uint32_t h1 = 0xEFCDAB89;
    uint32_t h2 = 0x98BADCFE;
    uint32_t h3 = 0x10325476;
    uint32_t h4 = 0xC3D2E1F0;

    //Preprocessing
    uint64_t padding = 64 - (l + 1 + LENGTH_BYTE_SIZE) % 64;
    uint64_t total = l + 1 + padding + LENGTH_BYTE_SIZE;
    uint8_t* full_message = (uint8_t*)malloc(total); 

    if(!full_message) return "";

    memcpy(full_message, message, l);
    full_message[l] = 0x80;
    memset(full_message + l + 1, 0, padding);
    for (int i = 0; i < 8; i++) {
        full_message[total - 1 - i] = (uint8_t)(l*8 >> (i * 8));
    }

    for (int i = 0; i < total; i++)
    {
        printf("%02x ", (unsigned char)full_message[i]);
    }
    std::cout << "\n"; 

    for (size_t i = 0; i < total; i+=64)
    {
        uint8_t* chunk = full_message + i;
        uint32_t words[80] = {};
        for (size_t j = 0; j < 16; j++)
        {
            words[j] = to_uint32_be(chunk+j*4);
            printf("words[%2d] = %08x\n", j, words[j]);
        }
        for (int j = 16; j < 80; j++){
            words[j] = CircularLeftShift(words[j-3] ^ words[j-8] ^ words[j-14] ^ words[j-16], 1);
            printf("words[%2d] = %08x\n", j, words[j]);
        }

        uint32_t a = h0;
        uint32_t b = h1;
        uint32_t c = h2;
        uint32_t d = h3;
        uint32_t e = h4;

        for (size_t j = 0; j < 80; j++)
        {
            std::cout << "i: " << j << std::hex << " a: " << a << " b: " << b << " c: " << c << " d: " << d << " e: " << e << "\n";
            uint32_t f = 0;
            uint32_t k = 0;
            if(j < 20){
                f = (b & c) | ((~b) & d);
                k = 0x5A827999;
            }else if(j < 40){
                f = b ^ c ^ d;
                k = 0x6ED9EBA1;
            }else if (j < 60){
                f = (b & c) ^ (b & d) ^ (c & d);
                k = 0x8F1BBCDC;
            }else if (j < 80){
                f = b ^ c ^ d;
                k = 0xCA62C1D6;
            }
            uint32_t temp = CircularLeftShift(a, 5) + f + e + k + words[j];
            e = d;
            d = c;
            c = CircularLeftShift(b, 30);
            b = a;
            a = temp;
        }

        h0 += a;
        h1 += b;
        h2 += c;
        h3 += d;
        h4 += e;
    }

    std::ostringstream out;
    out << std::hex << h0 << h1 << h2 << h3 << h4;

    std::string hh = out.str();


    free(full_message);

    return hh;
}

void LoadDatabaseJson(fs::path database_folder_path){
    fs::path hashes_path = database_folder_path/"sha1-hashes.json";
    std::ifstream hashes_file(hashes_path);
    if (!hashes_file) {
        std::cerr << "Could not open sha1-hashes.json\n";
        return;
    }
    hashes = json::parse(hashes_file);

    fs::path platforms_path = database_folder_path/"platforms.json";
    std::ifstream platforms_file(platforms_path);
    if (!platforms_file) {
        std::cerr << "Could not open platforms.json\n";
        return;
    }
    platforms = json::parse(platforms_file);

    fs::path quirks_path = database_folder_path/"quirks.json";
    std::ifstream quirks_file(quirks_path);
    if (!quirks_file) {
        std::cerr << "Could not open quirks.json\n";
        return;
    }
    quirks = json::parse(quirks_file);

    fs::path programs_path = database_folder_path/"programs.json";
    std::ifstream programs_file(programs_path);
    if (!programs_file) {
        std::cerr << "Could not open programs.json\n";
        return;
    }
    programs = json::parse(programs_file);

    json_loaded = true;
}

int GetProgramIndexFromHash(std::string hash){
    if (!json_loaded)
    {
        std::cerr << "Database Json Not Loaded";
        return -1;
    }
    
    if(hashes.contains(hash)){
        int program_index = hashes[hash];
        return hashes[hash];
    }else{
        return -1;
    }
}

std::string GetProgramTitle(int program_index){
    if (!json_loaded)
    {
        std::cerr << "Database Json Not Loaded";
        return "";
    }
    if (programs[program_index].contains("title"))
    {
        return programs[program_index]["title"];
    }else{
        return "";
    }
}

std::string GetProgramPlatform(int program_index, std::string hash){
    if (!json_loaded)
    {
        std::cerr << "Database Json Not Loaded";
        return "";
    }
    if (programs[program_index]["roms"].contains(hash))
    {
        uint8_t preferred_system = 0;

        for (const std::string &platform : programs[program_index]["roms"][hash]["platforms"])
        {
            // put unsupported systems here
            // will i ever suppor xo chip? probably not
            if (platform == "xochip" ||
                platform == "megachip8" ||
                platform == "chip8x" ||
                platform == "hybridVIP" ||
                platform == "superchip1")
            {
                return "Platform Not Supported";
            }
            return platform;

        }
    }else{
        return "";
    }

}

void UpdatePlatformSpecificQuirks(std::string platform){

}