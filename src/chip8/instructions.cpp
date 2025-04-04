// Here I need to decode the opcodes given in bytes
#include "instructions.h"
#include "specs.h"
#include <charconv>
#include <cstdint>
#include <cstdlib>
#include <iostream>

#define NIBBLE 4
#define LEAST_SIGNIFICANT_BIT_MASK 0b00000001
#define MOST_SIGNIFICANT_BIT_MASK 0b10000000
#define TYPE_MASK 0xF000
#define X_MASK 0x0F00
#define Y_MASK 0x00F0
#define N_MASK 0x000F
#define NN_MASK 0x00FF
#define NNN_MASK 0x0FFF

// 00E0
void ClearDisplay() {
  for (int i = 0; i < 64 * 32; i++) {
    display[i] = 0x0;
  }
}

// 00EE
void ReturnSubroutine() {
  pc = stack[sp];
  sp--;
}

// 1NNN
void Jump(uint16_t NNN) { pc = NNN; }

// 2NNN
void CallSubroutine(uint16_t NNN) {
  sp++;
  stack[sp] = pc;
  pc = NNN;
}

// 3XNN
void JumpEqVXNN(uint16_t X, uint16_t NN) {
  if (V[X] == NN) {
    pc += 2;
  }
}

// 4XNN
void JumpNqVXNN(uint16_t X, uint16_t NN) {
  if (V[X] != NN) {
    pc += 2;
  }
}

// 5XY0
void JumpEqVXVY(uint16_t X, uint16_t Y) {
  if (V[X] == V[Y]) {
    pc += 2;
  }
}

// 6XNN
void SetVXNN(uint16_t X, uint16_t NN) { V[X] = (uint8_t)NN; }

// 7XNN
void AddVXNN(uint16_t X, uint16_t NN) { V[X] += (uint8_t)NN; }

// 8XY0
void SetVXVY(uint16_t X, uint16_t Y) { V[X] = V[Y]; }

// 8XY1
void Or(uint16_t X, uint16_t Y) { 
    V[X] |= V[Y]; 
    //Quirk ?
    if(vf_reset)
        V[0xF] = 0;
}

// 8XY2
void And(uint16_t X, uint16_t Y) { 
    V[X] &= V[Y];
    //Quirk ?
    if(vf_reset)
        V[0xF] = 0;
}

// 8XY3
void Xor(uint16_t X, uint16_t Y) { 
    V[X] ^= V[Y]; 
    //Quirk ?
    if(vf_reset)
        V[0xF] = 0;
}
// 8XY4
void AddVXVY(uint16_t X, uint16_t Y) {
    uint16_t original_vx = V[X];
    V[X] += V[Y];
    if( (original_vx + (uint16_t)V[Y]) > 255 ){
        V[0xF] = 1;
    }else{
        V[0xF] = 0;
    }
}

// 8XY5
void SubVXVY(uint16_t X, uint16_t Y) {
    uint8_t original_vx = V[X];
    V[X] -= V[Y];
    if (original_vx >= V[Y]) V[0xF] = 1;
    else V[0xF] = 0;
}

// 8XY6
void ShiftRight(uint16_t X, uint16_t Y) {
    //original cosmac vip
    uint8_t original_vy = V[Y];
    V[X] = V[Y] >> 1;
    if(0b00000001 & original_vy) V[0xF] = 1;
    else V[0xF] = 0;
}

// 8XY7
void SubVYVX(uint16_t X, uint16_t Y) {
    V[X] = V[Y] - V[X];
    if (V[Y] >= V[X]) V[0xF] = 1;
    else V[0xF] = 0;
}

// 8XYE
void ShiftLeft(uint16_t X, uint16_t Y) {
    //original cosmac vip
    uint8_t original_vy = V[Y];
    V[X] = V[Y] << 1;
    if(0b10000000 & original_vy) V[0xF] = 1;
    else V[0xF] = 0;
}

// 9XY0
void JumpNqVXVY(uint16_t X, uint16_t Y) {
  if (V[X] != V[Y]) {
    pc += 2;
  }
}

// ANNN
void SetIndexRegister(uint16_t NNN) { I = NNN; }

// BNNN
void JumpWithOffset(uint16_t NNN) {
    pc = NNN + V[0x0];
}

// CXNN
void Random(uint16_t X, uint16_t NN) {
    uint8_t random = rand() % 255;
    V[X] = random & (uint8_t)NN;
}

// DXYN
void DrawSprite(uint16_t X, uint16_t Y, uint16_t N) {
    display_flag = 1;
    uint8_t X_coord = V[X] % 64;
    uint8_t Y_coord = V[Y] % 32;
    V[0xF] = 0;

    for (int i = 0; i < N; i++) {

        uint8_t sprite_row = ram[I + i];
        for (int b = 0; b < 8; b++) {
            uint8_t current_bit = (sprite_row >> (7 - b)) & 1;
            uint16_t index = ((Y_coord + i)%32) * 64 + ((X_coord + b)%64);
            if (current_bit && display[index])
                V[0xF] = 0x1;
            display[index] ^= current_bit;
        }
    }
}

// EX9E
void JumpIfPress(uint16_t X) {
    if (V[X] > 0xF) return;
    if(keymap[V[X]] == 1){
        pc += 2;
    }
}

// EXA1
void JumpIfNotPress(uint16_t X) {
    if (V[X] > 0xF) return;
    if(keymap[V[X]] == 0){
        pc += 2;
    }
}

// FX07
void GetDelay(uint16_t X) {
    V[X] = delay;
}

// FX0A
void GetKey(uint16_t X) {
    if(keymap[0x0]){
        V[X] = 0x0;
    }
    else if(keymap[0x1]){
        V[X] = 0x1;
    }
    else if(keymap[0x2]){
        V[X] = 0x2;
    }
    else if(keymap[0x3]){
        V[X] = 0x3;
    }
    else if(keymap[0x4]){
        V[X] = 0x4;
    }
    else if(keymap[0x5]){
        V[X] = 0x5;
    }
    else if(keymap[0x6]){
        V[X] = 0x6;
    }
    else if(keymap[0x7]){
        V[X] = 0x7;
    }
    else if(keymap[0x8]){
        V[X] = 0x8;
    }
    else if(keymap[0x9]){
        V[X] = 0x9;
    }
    else if(keymap[0xa]){
        V[X] = 0xa;
    }
    else if(keymap[0xb]){
        V[X] = 0xb;
    }
    else if(keymap[0xc]){
        V[X] = 0xc;
    }
    else if(keymap[0xd]){
        V[X] = 0xd;
    }
    else if(keymap[0xe]){
        V[X] = 0xe;
    }
    else if(keymap[0xf]){
        V[X] = 0xf;
    }else{
        pc -= 2;
    }
}

// FX15
void SetDelay(uint16_t X) {
    delay = V[X];
}

// FX18
void SetSound(uint16_t X) {
    sound = V[X];
}

// FX1E
void AddIndex(uint16_t X) {
    if(I + V[X] >=  4096) V[0xf] = 1;
    I += V[X];
}

// FX29
void SetIndexLoc(uint16_t X) {
  uint8_t character = V[X] & 0x0F;
  I = 0x50 + character * 5;
}

// FX33
void GetBCD(uint16_t X) {
  uint8_t hundreds = V[X] / 100;
  uint8_t tens = (V[X] % 100) / 10;
  uint8_t ones = V[X] % 10;
  V[I] = hundreds;
  V[I + 1] = tens;
  V[I + 2] = ones;
}

// FX55
void StoreRegisters(uint16_t X) {
  for (int i = 0; i <= X && I + i < 4096; i++) {
    ram[I + i] = V[i];
  }
  //Quirk ?
  I = I + X + 1;
}

// FX65
void LoadRegisters(uint16_t X) {
  for (int i = 0; i <= X && I + i < 4096; i++) {
    V[i] = ram[I + i];
  }
  //Quirk ?
  I = I + X + 1;
}

void DecodeOpcode(uint16_t opcode) {
    uint16_t op_type = opcode >> (3 * NIBBLE);
    uint16_t X = (opcode & X_MASK) >> 2 * NIBBLE;
    uint16_t Y = (opcode & Y_MASK) >> NIBBLE;
    uint16_t N = opcode & N_MASK;
    uint16_t NN = opcode & NN_MASK;
    uint16_t NNN = opcode & NNN_MASK;

    switch (op_type) {
        case 0x0:
            if (NNN == 0x0E0)
                ClearDisplay();
            else if (NNN == 0x0EE)
                ReturnSubroutine();
            break;
        case 0x1:
            Jump(NNN);
            break;
        case 0x2:
            CallSubroutine(NNN);
            break;
        case 0x3:
            JumpEqVXNN(X, NN);
            break;
        case 0x4:
            JumpNqVXNN(X, NN);
            break;
        case 0x5:
            JumpEqVXVY(X, Y);
            break;
        case 0x6:
            SetVXNN(X, NN);
            break;
        case 0x7:
            AddVXNN(X, NN);
            break;
        case 0x8:
            switch (N) {
                case 0x0:
                    SetVXVY(X, Y);
                    break;
                case 0x1:
                    Or(X, Y);
                    break;
                case 0x2:
                    And(X, Y);
                    break;
                case 0x3:
                    Xor(X, Y);
                    break;
                case 0x4:
                    AddVXVY(X, Y);
                    break;
                case 0x5:
                    SubVXVY(X, Y);
                    break;
                case 0x6:
                    ShiftRight(X, Y);
                    break;
                case 0x7:
                    SubVYVX(X, Y);
                    break;
                case 0xE:
                    ShiftLeft(X, Y);
                    break;
                default:
                    break;
            }
            break;
        case 0x9:
            if(N == 0) JumpNqVXVY(X, Y);
            break;
        case 0xa:
            SetIndexRegister(NNN);
            break;
        case 0xb:
            JumpWithOffset(NNN);
            break;
        case 0xc:
            Random(X, NN);
            break;
        case 0xd:
            DrawSprite(X, Y, N);
            break;
        case 0xe:
            switch (NN) {
                case 0x9E:
                    JumpIfPress(X);
                    break;
                case 0xA1:
                    JumpIfNotPress(X);
                    break;
            }
            break;
        case 0xf:
            switch (NN) {
                case 0x07:
                    GetDelay(X);
                    break;
                case 0x0A:
                    GetKey(X);
                    break;
                case 0x15:
                    SetDelay(X);
                    break;
                case 0x18:
                    SetSound(X);
                    break;
                case 0x1E:
                    AddIndex(X);
                    break;
                case 0x29:
                    SetIndexLoc(X);
                    break;
                case 0x33:
                    GetBCD(X);
                    break;
                case 0x55:
                    StoreRegisters(X);
                    break;
                case 0x65:
                    LoadRegisters(X);
                    break;
            }
            break;
        default:
            std::cout << "Unknown opcode encountered " << std::hex << opcode << "\n"; 
            break;
    }
}
