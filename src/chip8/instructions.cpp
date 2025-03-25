//Here I need to decode the opcodes given in bytes
#include <cstdint>
#include <iostream>
#include "instructions.h"
#include "specs.h"

#define NIBBLE 4
#define LEAST_SIGNIFICANT_BIT_MASK 0b00000001
#define MOST_SIGNIFICANT_BIT_MASK  0b10000000
#define TYPE_MASK 0xF000
#define X_MASK 0x0F00
#define Y_MASK 0x00F0
#define N_MASK 0x000F
#define NN_MASK 0x00FF
#define NNN_MASK 0x0FFF

//00E0
void ClearDisplay(){
    for (int i = 0; i < 64*32; i++) {
        display[i] = 0x0;
    }
}



//00EE
void ReturnSubroutine(){

}

//1NNN
void Jump(uint16_t NNN){
    pc =  NNN;
}

//2NNN
void CallSubroutine(){

}

//3XNN
void JumpEqVXNN(){
    
}

//4XNN
void JumpNqVXNN(){

}

//5XY0
void JumpEqVXVY(){

}

//6XNN
void SetVXNN(uint16_t X, uint16_t NN){
    V[X] = (uint8_t)NN; 
}

//7XNN
void AddVXNN(uint16_t X, uint16_t NN){ 
    V[X] += (uint8_t)NN; 
}

//8XY0
void SetVXVY(){
    
}

//8XY1
void Or(){

}

//8XY2
void And(){
    
}

//8XY3
void Xor(){

}
//8XY4
void AddVXVY(){

}

//8XY5
void SubVXVY(){
    
}


//8XY6
void ShiftRight(){
    
}

//8XY7
void SubVYVX(){

}


//8XYE
void ShiftLeft(){

}

//9XY0
void JumpNqVXVY(){

}

//ANNN
void SetIndexRegister(uint16_t NNN){
    I = NNN;
}

//BNNN
void JumpWithOffset(){

}

//CXNN
void Random(){

}

//DXYN
void DrawSprite(uint16_t X, uint16_t Y, uint16_t N){
    uint8_t X_coord = V[X] % 64;
    uint8_t Y_coord = V[Y] % 32;
    V[0xF] = 0;

    for (int i = 0; i < N; i++) {
        if(Y_coord + N > 31) break; 
        uint8_t sprite_row = ram[I + i];
        for (int b = 0; b < 8; b++){
            if(X_coord + b > 63) break;
            uint8_t current_bit = (sprite_row >> (7-b)) & 1;
            uint16_t index = (Y_coord + i) * 64 + (X_coord + b);
            if (current_bit && display[index])
                V[0xF] = 0x1;
            display[index] = current_bit; 
        }
    }
}

//EX9E
void JumpIfPress(){

}

//EXA1
void JumpIfNotPress(){

}

//FX07
void GetDelay(){

}

//FX0A
void GetKey(){
}

//FX15
void SetDelay(){

}

//FX18
void SetSound(){

}

//FX1E
void AddIndex(){

}

//FX29
void SetIndexLoc(){

}

//FX33 
void GetBCD(){

}

//FX55
void StoreRegisters(){

}

//FX65
void LoadRegisters(){

}


void DecodeOpcode(uint16_t opcode){
    uint16_t op_type = opcode >> 3*NIBBLE;
    uint16_t X = (opcode & X_MASK) >> 2*NIBBLE;
    uint16_t Y = (opcode & Y_MASK) >> NIBBLE;
    uint16_t N = opcode & N_MASK;
    uint16_t NN = opcode & NN_MASK;
    uint16_t NNN = opcode & NNN_MASK;

    switch (op_type) {
        case 0x0:
            if(NNN == 0x0E0)
                ClearDisplay();
            break;
        case 0x1:
            Jump(NNN);
            break;
        case 0x2:
            break;
        case 0x3:
            break;
        case 0x4:
            break;
        case 0x5:
            break;
        case 0x6:
            SetVXNN(X, NN);
            break;
        case 0x7:
            AddVXNN(X, NN);
            break;
        case 0x8:
            break;
        case 0x9:
            break;
        case 0xa:
            SetIndexRegister(NNN);
            break;
        case 0xb:
            break;
        case 0xc:
            break;
        case 0xd:
            DrawSprite(X, Y, N);
           break;
        case 0xe:
            break;
        case 0xf:
            break;
    }

}

