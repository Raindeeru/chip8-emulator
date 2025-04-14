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
  for (int i = 0; i < 128 * 64; i++) {
    display_hires[i] = 0x0;
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
    if(shifting){
        uint8_t original_vx = V[X];
        V[X] = V[X] >> 1;
        if (0b00000001 & original_vx)
            V[0xF] = 1;
        else
            V[0xF] = 0;
    }
    else{
        // original cosmac vip
        uint8_t original_vy = V[Y];
        V[X] = V[Y] >> 1;
        if (0b00000001 & original_vy)
            V[0xF] = 1;
        else
            V[0xF] = 0;
    }
}

// 8XY7
void SubVYVX(uint16_t X, uint16_t Y) {
    uint8_t original_vx = V[X];
    V[X] = V[Y] - V[X];
    if (V[Y] >= original_vx) V[0xF] = 1;
    else V[0xF] = 0;
}

// 8XYE
void ShiftLeft(uint16_t X, uint16_t Y) {
    if(shifting){
        uint8_t original_vx = V[X];
        V[X] = V[X] << 1;
        if (0b10000000 & original_vx)
            V[0xF] = 1;
        else
            V[0xF] = 0;

    }else{
        // original cosmac vip
        uint8_t original_vy = V[Y];
        V[X] = V[Y] << 1;
        if (0b10000000 & original_vy)
            V[0xF] = 1;
        else
            V[0xF] = 0;
    }
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
void JumpWithOffset(uint16_t NNN, uint16_t X) {
    if(jumping)
        pc = NNN + V[X];
    else
        pc = NNN + V[0x0];
}

// CXNN
void Random(uint16_t X, uint16_t NN) {
    V[X] = rand() & (uint8_t)NN;
}

// DXYN
void DrawSprite(uint16_t X, uint16_t Y, uint16_t N) {
    display_flag = 1;
    uint8_t X_coord = V[X] % 64;
    uint8_t Y_coord = V[Y] % 32;
    V[0xF] = 0;

    for (int i = 0; i < N; i++) {
        if(clipping && (i + Y_coord) >= 32)
            break;

        uint8_t sprite_row = ram[I + i];
        for (int b = 0; b < 8; b++) {
            if(clipping && (b + X_coord) >= 64)
                break;
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
    if(keymap[V[X] & N_MASK] == 1){
        pc += 2;
    }
}

// EXA1
void JumpIfNotPress(uint16_t X) {
    if(keymap[V[X] & N_MASK] == 0){
        pc += 2;
    }
}

// FX07
void GetDelay(uint16_t X) {
    V[X] = delay;
}

// FX0A
void GetKey(uint16_t X) {
    for (int i = 0; i < 0xF; i++)
    {
        if(!keymap[i] && keystate[i]){
            V[X] = i;
            return;
        }
    }
    pc -= 2;
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
    I += V[X];
    I %= 0x1000;
}

// FX29
void SetIndexLoc(uint16_t X) {
  uint8_t character = V[X] & 0x0F;
  I = 0x50 + character * 5;
  I %= 0x1000;
}

// FX33
void GetBCD(uint16_t X) {
  uint8_t hundreds = V[X] / 100;
  uint8_t tens = (V[X] % 100) / 10;
  uint8_t ones = V[X] % 10;
  ram[I] = hundreds;
  ram[I + 1] = tens;
  ram[I + 2] = ones;
}

// FX55
void StoreRegisters(uint16_t X) {
  for (int i = 0; i <= X && I + i < 4096; i++) {
    ram[I + i] = V[i];
  }
  //Quirk ?
  if (memory)
  {
      I = I + X + 1;
      I %= 0x1000;
  }
}

// FX65
void LoadRegisters(uint16_t X) {
  for (int i = 0; i <= X && I + i < 4096; i++) {
    V[i] = ram[I + i];
  }
  //Quirk ?
  if(memory){
      I = I + X + 1;
      I %= 0x1000;
  }
}

//SuperChip Specific Instruction

//Change to use hi res display
void DrawSpriteSuperChip(uint16_t X, uint16_t Y, uint16_t N) {
    display_flag = 1;
    uint8_t X_coord = hires ? V[X] % 128 : V[X] % 64;
    uint8_t Y_coord = hires ? V[Y] % 64 : V[Y] % 32;
    V[0xF] = 0;

    if(hires){
        for (int i = 0; i < N; i++)
        {
            if (clipping && (i + Y_coord) >= 64)
                break;

            uint8_t sprite_row = ram[I + i];
            for (int b = 0; b < 8; b++)
            {
                if (clipping && (b + X_coord) >= 128)
                    break;
                uint8_t current_bit = (sprite_row >> (7 - b)) & 1;
                uint16_t index = ((Y_coord + i) % 64) * 128 + ((X_coord + b) % 128);
                if (current_bit && display_hires[index])
                    V[0xF] = 0x1;
                display_hires[index] ^= current_bit;
            }
        }
    }else{
        for (int i = 0; i < N; i++)
        {
            if (clipping && (i + Y_coord) >= 32)
                break;

            uint8_t sprite_row = ram[I + i];
            for (int b = 0; b < 8; b++)
            {
                if (clipping && (b + X_coord) >= 64)
                    break;

                //For the large pixels in lores mode we got 4 pixels in 1
                uint8_t current_bit = (sprite_row >> (7 - b)) & 1;

                uint16_t indices[4] = {0};
                indices[0] = (2*(Y_coord + i) % 64) * 128 + (2*(X_coord + b) % 128);
                indices[1] = (2*(Y_coord + i) % 64) * 128 + (2*(X_coord + b) % 128) + 1;
                indices[2] = (2*(Y_coord + i) % 64) * 128 + (2*(X_coord + b) % 128) + 128;
                indices[3] = (2*(Y_coord + i) % 64) * 128 + (2*(X_coord + b) % 128) + 129;

                for (int j = 0; j < 4; j++ )
                {
                    if (current_bit && display_hires[indices[j]])
                        V[0xF] = 0x1;
                    display_hires[indices[j]] ^= current_bit;
                }                
            }
        }
    }
    
}

//00FF
void HiResOn(){
    hires = true;
    ClearDisplay();
}

//00FE
void HiResOff(){
    hires = false;
    ClearDisplay();
}

//00CN
void ScrollDown(uint16_t N){
    display_flag = 1;
    for (int i = 63-N; i >= 0; i--)
    {
        for (int j = 0; j < 128; j++)
        {
            display_hires[128 * (i + N) + j] = display_hires[128 * i + j];
        }
        
    }
    //Set Rows at top to 0
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < 128; j++)
        {
            display_hires[128*i + j] = 0;
        }
        
    }
    
}

//00FB
void ScrollRight(){
    display_flag = 1;
    for (int i = 123; i >= 0; i--)
    {
        for (int j = 0; j < 64; j++)
        {
            display_hires[128*j + i+4] = display_hires[128*j + i];
        }
        
    }
    //Set left rows 0
    for (int i = 0; i < 64; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            display_hires[128*i + j] = 0;
        }
        
    }
}

//00FC
void ScrollLeft(){
    display_flag = 1;
    for (int i = 4; i < 128; i++)
    {
        for (int j = 0; j < 64; j++)
        {
            display_hires[128*j + i - 4] = display_hires[128*j + i];
        }
        
    }
    //set right rows 0
    for (int i = 0; i < 64; i++)
    {
        for (int j = 127; j >= 123; j--)
        {
            display_hires[128*i + j] = 0;
        }
        
    }

}

//DXY0 16x16 sprite in Hires 8x16 in lores
//Tho there in the database its a quirk that they always draw 16x16 sprites
void DrawBigSprite(uint16_t X, uint16_t Y){
    display_flag = 1;
    uint8_t X_coord = hires ? V[X] % 128 : V[X] % 64;
    uint8_t Y_coord = hires ? V[Y] % 64 : V[Y] % 32;
    V[0xF] = 0;
    if(hires){
        //Draw 16x16 sprite
        for (int i = 0; i < 16; i++)
        {
            if (clipping && (i + Y_coord) >= 64)
                break;

            uint16_t sprite_row = ram[I + i*2 + 1] + (ram[I + i*2] << 8);
            for (int b = 0; b < 16; b++)
            {
                if (clipping && (b + X_coord) >= 128)
                    break;
                uint8_t current_bit = (sprite_row >> (15 - b)) & 1;
                uint16_t index = ((Y_coord + i) % 64) * 128 + ((X_coord + b) % 128);
                if (current_bit && display_hires[index])
                    V[0xF] = 0x1;
                display_hires[index] ^= current_bit;
            }
        }
    }else{
        //Draw 8x16 sprite
        for (int i = 0; i < 16; i++)
        {
            if (clipping && (i + Y_coord) >= 32)
                break;

            uint8_t sprite_row = ram[I + i];
            for (int b = 0; b < 8; b++)
            {
                if (clipping && (b + X_coord) >= 64)
                    break;

                //For the large pixels in lores mode we got 4 pixels in 1
                uint8_t current_bit = (sprite_row >> (7 - b)) & 1;

                uint16_t indices[4] = {0};
                indices[0] = (2*(Y_coord + i) % 64) * 128 + (2*(X_coord + b) % 128);
                indices[1] = (2*(Y_coord + i) % 64) * 128 + (2*(X_coord + b) % 128) + 1;
                indices[2] = (2*(Y_coord + i) % 64) * 128 + (2*(X_coord + b) % 128) + 128;
                indices[3] = (2*(Y_coord + i) % 64) * 128 + (2*(X_coord + b) % 128) + 129;

                for (int j = 0; j < 4; j++ )
                {
                    if (current_bit && display_hires[indices[j]])
                        V[0xF] = 0x1;
                    display[indices[j]] ^= current_bit;
                }                
            }
        }

    }
}

//FX30
void SetIndexLocBig(uint16_t X){
  uint8_t character = V[X] & 0x0F;
  I = 0xA0 + character * 10;
  I %= 0x1000;
}

//FX75
void SaveFlags(uint16_t X)
{
    X %= 8;
    for (int i = 0; i <= X; i++)
    {
        flags[i] = V[i];
    }
}

//FX85
void LoadFlags(uint16_t X){
    X %= 8;
    for (int i = 0; i <= X; i++)
    {
        V[i] = flags[i];
    }

}

//00FD
void Exit(){

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
            else{
                std::cout << "Invalid Instruction: " << std::hex << opcode << "\n";
            }
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
            if(N==0) JumpEqVXVY(X, Y);
            else std::cout << "Invalid Instruction: " << std::hex << opcode;
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
                    std::cout << "Invalid Instruction: " << std::hex << opcode << "\n";
                    break;
            }
            break;
        case 0x9:
            if(N == 0) JumpNqVXVY(X, Y);
            else std::cout << "Invalid Instruction: " << std::hex << opcode << "\n";
            break;
        case 0xa:
            SetIndexRegister(NNN);
            break;
        case 0xb:
            JumpWithOffset(NNN, X);
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
                default:
                    std::cout << "Invalid Instruction: " << std::hex << opcode << "\n";
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
                default:
                    std::cout << "Invalid Instruction: " << std::hex << opcode << "\n";
            }
            break;
        default:
            break;
    }
}

void DecodeOpcodeSuperChip(uint16_t opcode) {
    uint16_t op_type = opcode >> (3 * NIBBLE);
    uint16_t X = (opcode & X_MASK) >> 2 * NIBBLE;
    uint16_t Y = (opcode & Y_MASK) >> NIBBLE;
    uint16_t N = opcode & N_MASK;
    uint16_t NN = opcode & NN_MASK;
    uint16_t NNN = opcode & NNN_MASK;

    switch (op_type) {
        case 0x0:
            switch (NNN)
            {
            case 0x0E0:
                ClearDisplay();
                break;
            case 0x0EE:
                ReturnSubroutine();
                break;
            case 0x0FF:
                HiResOn();
                break;
            case 0x0FE:
                HiResOff();
                break;
            case 0x0FB:
                ScrollRight();
                break;
            case 0x0FC:
                ScrollLeft();
                break;
            case 0x0FD:
                Exit();
                break;
            default:
                if((NNN & 0xFF0) == 0x0C0){
                    ScrollDown(N);
                }else{
                    std::cout << "Invalid Instruction 0: " << std::hex << opcode << "\n" << "address: " << pc - 0x200 << "\n";
                }
                break;
            }
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
            if(N==0) JumpEqVXVY(X, Y);
            else std::cout << "Invalid Instruction: 5" << std::hex << opcode;
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
                    std::cout << "Invalid Instruction: 8" << std::hex << opcode << "\n";
                    break;
            }
            break;
        case 0x9:
            if(N == 0) JumpNqVXVY(X, Y);
            else std::cout << "Invalid Instruction: 9" << std::hex << opcode << "\n";
            break;
        case 0xa:
            SetIndexRegister(NNN);
            break;
        case 0xb:
            JumpWithOffset(NNN, X);
            break;
        case 0xc:
            Random(X, NN);
            break;
        case 0xd:
            if(N == 0)
                DrawBigSprite(X, Y);
            else
                DrawSpriteSuperChip(X, Y, N);
            break;
        case 0xe:
            switch (NN) {
                case 0x9E:
                    JumpIfPress(X);
                    break;
                case 0xA1:
                    JumpIfNotPress(X);
                    break;
                default:
                    std::cout << "Invalid Instruction E: " << std::hex << opcode << "\n";
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
                case 0x30:
                    SetIndexLocBig(X);
                    break;
                case 0x75:
                    SaveFlags(X);
                    break;
                case 0x85:
                    LoadFlags(X);
                    break;
                default:
                    std::cout << "Invalid Instruction F: " << std::hex << opcode << "\n" ;
            }
            break;
        default:
            break;
    }
}