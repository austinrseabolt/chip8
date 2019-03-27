#include <iostream>
#include <fstream>
#include <string>
#include "chip8.hpp"

chip8 mychip8;

int main(int argc, char* argv[]){
    mychip8.initialize();
    if (argc > 1){
        mychip8.loadRom(argv[1]);
    }
    else {
        std::cout << "Please select a ROM." << std::endl; 
        exit(1);
    }
    
    
    

    for (;;){
        mychip8.emulateCycle();
        mychip8.gfxdump();
    }
   
}