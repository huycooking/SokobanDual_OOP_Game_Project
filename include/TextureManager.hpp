#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>

class TextureManager {
public:
    sf::Texture floorTex;
    sf::Texture pushableBoxTex;
    sf::Texture specialBoxTex;
    sf::Texture player1Tex;
    sf::Texture player2Tex;
    sf::Texture portalTex;
    sf::Texture menuBgTex;
    sf::Font    font;

    // load all textures; return true if most essential assets loaded
    bool loadAll() {
        bool ok = true;
        if (!floorTex.loadFromFile("Assets/floor.jpg")) {
            std::cerr << "Failed to load Assets/floor.jpg\n";
            ok = false;
        }
        if (!pushableBoxTex.loadFromFile("Assets/box.jpg")) {
            std::cerr << "Failed to load Assets/box.jpg\n";
            ok = false;
        }
        if (!specialBoxTex.loadFromFile("Assets/SpecialBox.jpg")) {
            std::cerr << "Failed to load Assets/SpecialBox.jpg\n";
            ok = false;
        }
        if (!player1Tex.loadFromFile("Assets/Player1.jpg")) {
            std::cerr << "Failed to load Assets/Player1.jpg\n";
            ok = false;
        }
        if (!player2Tex.loadFromFile("Assets/Player2.jpg")) {
            std::cerr << "Failed to load Assets/Player2.jpg\n";
            ok = false;
        }
        if (!portalTex.loadFromFile("Assets/portal.jpg")) {
            std::cerr << "Warning: Failed to load Assets/portal.jpg (will use empty texture)\n";
            // not fatal
        }
        if (!menuBgTex.loadFromFile("Assets/backround.jpg")) {
            std::cerr << "Warning: Failed to load Assets/backround.jpg\n";
        }
        if (!font.openFromFile("Assets/ARLRDBD.ttf")) {
            std::cerr << "Warning: Failed to load Assets/ARLRDBD.ttf\n";
        }
        return ok;
    }
};
