#include <iostream>
#include <fstream>
#include <string>
#include "chip8.hpp"
#include <SFML/Graphics.hpp>

int gfx_scale = 16;
std::vector<sf::RectangleShape> gfxout;

chip8 mychip8;

//Returns Pixel, used by gfx2pix()
sf::RectangleShape createpixel(int x, int y)
{
    sf::RectangleShape pixel(sf::Vector2f(1.0 * gfx_scale, 1.0 * gfx_scale));
    pixel.setFillColor(sf::Color::Green);
    pixel.setPosition(sf::Vector2f(x * gfx_scale, y * gfx_scale));
    return pixel;
}


//Grabs contents of VRAM and uses createpixel() to create pixel for each bit. Runs once per frame
void gfx2pix()
{
    int y = 0;
    int x = 0;
    for (int i = 0; i < sizeof(mychip8.gfx); ++i)
    {
        int currentplace;
        currentplace = mychip8.gfx[i];
        
        if (i % 64 == 0)
        {
            y += 1;
            x = 0;
            
        }
        else 
        {
            x += 1;
            
        }
        
        if (currentplace == 1)
        {
            gfxout.push_back(createpixel(x, y));
            
        }
        
        
    }
}

int main(int argc, char* argv[])
{

    
    
    //Test Pixels
    gfxout.push_back(createpixel(0, 0));
    gfxout.push_back(createpixel(1,0));








    mychip8.initialize();
    if (argc > 1)
    {
        mychip8.loadRom(argv[1]);
    }
    else 
    {
        std::cout << "Please select a ROM." << std::endl; 
        exit(1);
    }
    //Set up Window

    sf::RenderWindow window(sf::VideoMode(64 * gfx_scale, 32 * gfx_scale), "8Chipper"); 
    
    while (window.isOpen())
    {

        mychip8.emulateCycle();
        gfx2pix();
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
        }

        window.clear();
        //window.draw(gfxout); //used shape as argument

        for (const auto& pixel : gfxout)
        {
            window.draw(pixel);
        }
        gfxout.clear();
        window.display();

    }

   
}