#include <iostream>
#include <fstream>
#include <string>
#include "chip8.hpp"

chip8 mychip8;

int main(){
    mychip8.initialize();
    mychip8.loadRom();
    
    

    for (;;){
        mychip8.emulateCycle();
    }
   
}