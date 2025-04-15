#include<iostream>
#include "../src/config/database.h"

int main(){
    std::string test = Sha1("test", 4);

    std::cout << test;
}