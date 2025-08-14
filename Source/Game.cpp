// Game.cpp
#include <SFML/Graphics.hpp>         // ensure SFML symbols are available here
#include <memory>                     // for std::unique_ptr

#include "../include/Game.hpp"
#include "../include/Box.hpp"
#include "../include/SpecialBox.hpp"
#include "../include/Wall.hpp"
#include "../include/Enemy.hpp"
#include "../include/FloorButton.hpp"
#include "../include/CreateWallStrategy.hpp"
#include "../include/GameObjectFactory.hpp"

Game::Game()
: window(sf::VideoMode({800u, 600u}), "Dual Sokoban") // SFML 3: brace-init size
{
    player1 = new Player(sf::Vector2f(100.f, 100.f), "player1.png");
    player2 = new Player(sf::Vector2f(200.f, 100.f), "player2.png");
    portal  = new Portal(sf::Vector2f(400.f, 300.f));

    scoreObs1 = new ScoreObserver(player1);
    scoreObs2 = new ScoreObserver(player2);
    portal->attach(scoreObs1);
    portal->attach(scoreObs2);

    // Non-owning raw pointers above; ownership is stored in the vector below.
    objects.push_back(std::unique_ptr<GameObject>(player1));
    objects.push_back(std::unique_ptr<GameObject>(player2));
    objects.push_back(std::unique_ptr<GameObject>(portal));
    // Add walls, buttons, etc. similarly
}

void Game::handleInput() {
    // SFML 3: pollEvent() returns std::optional<sf::Event>
    while (const auto event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
            continue;
        }

        // SFML 3: access KeyPressed payload via getIf<...>()
        if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
            // WASD for player1, arrows for player2 (simplified)
            if (key->code == sf::Keyboard::Key::W) {
                player1->move(Direction::UP);
            }
            // TODO: handle A/S/D and arrow keys likewise
        }
    }
}

void Game::spawnObjects() {
    // SFML 3 time API unchanged for this usage
    if (spawnClock.getElapsedTime().asSeconds() >= 5.0f) { // Every 5 secs
        GameObjectFactory* factory = new BoxFactory();
        GameObject* obj = factory->createObject(sf::Vector2f(300.f, 300.f));
        objects.push_back(std::unique_ptr<GameObject>(obj));
        delete factory;
        spawnClock.restart();
    }
}

void Game::run() {
    while (window.isOpen()) {
        const float dt = clock.restart().asSeconds();

        handleInput();
        spawnObjects();

        for (auto& obj : objects) {
            obj->update(dt);
        }

        // Collision, portal check (simplified, manual call)
        // portal->onBoxEnter(someBox);

        window.clear(); // default clear color is black
        for (auto& obj : objects) {
            obj->render(window);
        }
        window.display();
    }
}
