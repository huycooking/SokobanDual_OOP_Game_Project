// Menu.cpp
#include "Menu.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

Menu::Menu(TextureManager& /*textures*/, unsigned int winW, unsigned int winH)
    : winW(winW), winH(winH),
      // directly construct sprites with their textures (SFML 3 requirement)
      player1(player1Tex),
      player2(player2Tex)
{
    // Load textures from files
    player1Tex.loadFromFile("Assets/Player1.jpg");
    player2Tex.loadFromFile("Assets/Player2.jpg");

    // Center sprites using SFML 3 rect API
    auto b1 = player1.getGlobalBounds();
    auto b2 = player2.getGlobalBounds();

    player1.setPosition({winW / 3.f - b1.size.x / 2.f,
                         winH / 2.f - b1.size.y / 2.f});
    player2.setPosition({2 * winW / 3.f - b2.size.x / 2.f,
                         winH / 2.f - b2.size.y / 2.f});
}

bool Menu::show(sf::RenderWindow& window) {
    while (window.isOpen()) {
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
                return false;
            }
            if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
                if (key->scancode == sf::Keyboard::Scancode::Enter) {
                    return true;  // Start game
                }
                if (key->scancode == sf::Keyboard::Scancode::Escape) {
                    window.close();
                    return false; // Quit
                }
            }
        }

        window.clear(sf::Color::Black);
        window.draw(player1);
        window.draw(player2);
        window.display();
    }
    return false;
}
