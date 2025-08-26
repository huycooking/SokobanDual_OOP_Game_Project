#pragma once
#include <SFML/Graphics.hpp>

class Game {
public:
    Game() = default;
    void run(sf::RenderWindow &window); // will execute Sokoban gameplay loop
};
