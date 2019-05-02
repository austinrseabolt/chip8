#include <iostream>
#include <string>
#include <fstream>
#include <stdlib.h>

class chip8{
    public:

    unsigned short opcode;
    unsigned char memory[4096];
    unsigned char V[16];
    unsigned short I;
    unsigned short pc;
    unsigned short stack[16];
    unsigned short sp;
    unsigned char key[16];
    unsigned char gfx[64 * 32];
    unsigned char delay_timer;

    unsigned char chip8_fontset[80] = { 
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };



    void initialize()
    {
        pc = 0x200; // Program counter starts at 0x200
        opcode = 0; // Reset current opcdoe 
        I = 0; // Reset Index Register
        sp = 0; // Reset Stack Pointer

        //clear memory
        for (int i = 0; i < sizeof(memory); ++i){
            memory[i] = 0;
        }
        //clear display

        //clear stack
        for (int i = 0; i < sizeof(stack); ++i){
            stack[i] = 0;
        }
        //clear registers 
        for (int i = 0; i < sizeof(V); ++i){
            V[i] = 0;
            
        }
        //load font set
        for (int i = 0; i < 80; ++i){
            memory[i] = chip8_fontset[i];
        }
    }

    void clearscreen()
    {
        for(int i = 0; i < sizeof(gfx); ++i){
            gfx[i] = 0;
        }
    }

    void memdump()
    {
        
        for (int i = 0; i < sizeof(memory); ++i){
            std::cout << "Value at address " << i << " is: " << std::hex << +memory[i] << std::endl;
        }
        

    }

    void vdump()
    {
        for (int i = 0; i < sizeof(V); ++i){
            std::cout << "Value at V" << std::dec << i << " is: " << +V[i] << std::endl;
        }
    }

    void gfxdump()
    {
        for (int x = 0; x < sizeof(gfx); ++x){
            unsigned char currentchar;
            currentchar = gfx[x];
            if (currentchar == 0){
                std::cout << " ";
            }
            else {
                std::cout << "█";
            }
            if ((x) % 64 == 0){
                std::cout << std::endl;
            }
        }
    }



    void emulateCycle(){
        /* fetch opcode, each opcode is 2 bytes and each memory array index is 1 byte,
        shift opcode one byte to the left and use bitwise OR to merge */
        opcode = memory[pc] << 8 | memory[pc + 1];
        int nextpc = pc + 2;
        unsigned short nextopcode = memory[nextpc] << 8 | memory[nextpc + 1];

        //decode opcode
        /* since value is only 12 bits at the end of opcode,
        use bitwise AND with 0x0FFF (0000111111111111) to set first four bits to zero */
        std::cout << std::endl << "OPCODE: " << std::hex << opcode << " AT ADDRESS " << pc << std::endl;
        std::cout << std::endl << "Next Opcode in Memory: " << nextopcode << std::endl;
        switch(opcode & 0xf000) //bitwise and makes all 12 value bits equal to zero
        {
            //check for opcodes 0x00EE and 0x00E0
            case 0x0000:
                switch(opcode & 0x000F)
                    {
                        case 0x0000: //0x00E0, Clears screen
                            clearscreen();
                            pc += 2;
                            //drawflag = true 
                        break;

                        case 0x000E: // RET return from subroutine
                            --sp;
                            pc = stack[sp];
                            pc += 2;

                        break;
                    }
            break;
            
            case 0x1000: //1NNN: jump to address NNN
                pc = (opcode & 0x0FFF);
            break;
            
            case 0x2000:
                stack[sp] = pc;
                ++sp;
                pc = opcode & 0x0FFF;
            break;
            
            case 0x3000: //3XNN: if Vx == NN skip next instruction.
                if (V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF)){
                    pc += 4;
                }
                else {
                    pc +=2;
                }
            break;


            case 0x4000:  //4xNN: If Vx != NN skip next instruction.
                if (V[(opcode & 0x0F00)] != (opcode & 0x00FF)){
                    pc += 4;
                }
                else {
                    pc += 2;
                }

            break;
            
            case 0x5000: //5XY0: Skips next instruction if VX equals VY.
                if (V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4]){
                    pc += 4;
                }
                else {
                    pc += 2;
                }
            break;
            
            case 0x6000: // 6XNN: VX = NN
                V[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
                pc += 2;
            break;
            
            case 0x7000:  //7XNN: Adds NN to VX (Carry flag is not changed.)
                {
                    int x = (opcode & 0x0F00) >> 8;
                    std::cout << "int x = (opcode & 0x0F00) >> 8;" << std::endl;
                    
                    V[x] = V[x] + (opcode & 0x00FF);
                    std::cout << "V[x] = V[x] + (opcode & 0x00FF);" << std::endl;
                    
                  
                    
                    pc += 2;
                    
                }
            break;

            case 0x8000: 
                switch(opcode & 0x000F){
                    case 0x0000: //8XY0, Sets Vx = Vy
                    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;

                    case 0x0001: //8XY1 sets VX to (Vx OR Vy), bitwise OR
                        V[(opcode & 0x0F00)] = (V[(opcode & 0x0F00) >> 8]) | (V[(opcode & 0x00F0) >> 4]);
                        pc += 2;
                    break;

                    case 0x0002: //8XY2, Vx = (Vx & Vy)
                        V[(opcode & 0x0F00 >> 8)] = V[(opcode & 0x0F00)] & V[(opcode & 0x00F0) >> 4];
                        pc += 2;
                    break;

                    case 0x0003: // 8XY3 Sets VX to VX xor VY
                        V[(opcode & 0x0F00) >> 8] = (V[(opcode & 0x0F00) >> 8]) ^ (V[(opcode & 0x00F0) >> 4]);
                        pc += 2;
                    break;

                    case 0x0004:
                        
                    break;
                }
            break;

            case 0x9000: //9XY0, Skips next instruction if Vx != Vy
                if (V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4]){
                    pc += 4;
                }
                else{
                    pc += 2;
                }

            break;

            case 0xA000: // ANNN: sets I to address NNN
                I = opcode & 0x0FFF;
                pc += 2;
            break;

            case 0xB000:

            break;

            case 0xC000:

            break;

            case 0xD000:		   
            {
            unsigned short x = V[(opcode & 0x0F00) >> 8];
            unsigned short y = V[(opcode & 0x00F0) >> 4];
            

            unsigned short height = opcode & 0x000F;

            std::cout << "Height = " << std::hex << height << std::endl;
            unsigned short pixel;
            
            V[0xF] = 0;
            for (int yline = 0; yline < height; yline++)
            {
                pixel = memory[I + yline];
                std::cout << "memory isn't the problem" << std::endl;
                for(int xline = 0; xline < 8; xline++)
                {
                if((pixel & (0x80 >> xline)) != 0)
                {
                    unsigned short result = (x + xline + ((y + yline) * 64));
                    std::cout << "Breakpoint 1" << std::endl;
                    if (result < sizeof(gfx))
                    {
                        if(gfx[(x + xline + ((y + yline) * 64))] == 1) //PROBLEM LINE
                        std::cout << "Breakpoint 2" << std::endl;
                        V[0xF] = 1;     
                    }
                    
                    
                    if (result < sizeof(gfx)){
                        std::cout << "SHIT WENT THRU" << std::endl;
                        gfx[x + xline + ((y + yline) * 64)] ^= 1; 
                    }
                    else
                    {
                        std::cout << "BREAK" << std::endl;
                        break;
                    }
                    
                    
                }
                }
            }
            //drawFlag = true;
            pc += 2;
            }
            std::cout << "Finish 0xD000" << std::endl;
            break;
           
            
            
            
            case 0xF000:
                switch(opcode & 0x00FF){
                    case 0x0007:
                        //V[(opcode & 0x0F00)] = get_delay();
                    break;

                    case 0x001E: //FX1E adds VX to I
                    I += V[((opcode & 0x0F00) >> 8)];

                    break;

                }
                pc += 2;
            break;
            
            default:
                memdump();
                gfxdump();
                std::cout << std::endl;
                vdump();

                std::cout << "Program Counter: " << std::dec << pc - 512 << std::endl;
                std::cout << "unknown opcode: "  << std::hex << opcode << std::endl;
                std::cout << "Memory Location of Opcode: " << std::hex << pc << std::endl;
                exit(1);
        }

       
    }

    void loadRom(char* rompath){
        int buffersize = 4096 - 512;
        unsigned char buffer[buffersize];
        FILE * pFile;
        pFile = fopen(rompath, "rb");
        int bytes_read = fread(buffer, sizeof(unsigned char), buffersize, pFile);
        for (int i = 0; i < buffersize; ++i){
            memory[i + 512] = buffer[i];
        }
        fclose(pFile);
    }
};