// Menu.hpp
#pragma once
#include <SFML/Graphics.hpp>
#include "TextureManager.hpp"

class Menu {
public:
    Menu(TextureManager& textures, unsigned int winW, unsigned int winH);

    // Shows the menu loop.
    // Returns true if user presses Enter to start, false if quit.
    bool show(sf::RenderWindow& window);

private:
    // store textures here so they live as long as the sprites
    sf::Texture player1Tex;
    sf::Texture player2Tex;

    // sprites must be constructed with textures
    sf::Sprite player1;
    sf::Sprite player2;

    unsigned int winW;
    unsigned int winH;
};
