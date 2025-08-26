// Sokoban.cpp
#include <SFML/Graphics.hpp>
#include <array>
#include <optional>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>

// include headers
#include "GameObject.hpp"
#include "Floor.hpp"
#include "Box.hpp"
#include "PushableBox.hpp"
#include "Portal.hpp"
#include "Map.hpp"
#include "Player.hpp"

enum class GameState { MENU, GAME };
GameState gameState = GameState::MENU;


int main() {
    // --- Map constants ---
    constexpr int MAP_W = 16 * 2;
    constexpr int MAP_H = 9 * 2;
    constexpr float TILE = 37.f; // tile size in pixels
    const unsigned winW = static_cast<unsigned>(MAP_W * TILE);
    const unsigned winH = static_cast<unsigned>(MAP_H * TILE);

    sf::RenderWindow window(sf::VideoMode({winW, winH}), "Sokuban dual!");
    window.setFramerateLimit(10);
    window.setKeyRepeatEnabled(false);

    // --- Font for score display ---
    sf::Font font;
    if (!font.openFromFile("Assets/ARLRDBD.ttf")) {
        std::cout << "Could not load Assets/ARLRDBD.ttf, using default font\n";
    }
    
    // Score text objects
    sf::Text player1ScoreText(font);
    sf::Text player2ScoreText(font);
    
    player1ScoreText.setFont(font);
    player1ScoreText.setCharacterSize(24);
    player1ScoreText.setFillColor(sf::Color::Blue);
    player1ScoreText.setPosition(sf::Vector2f(10,10));
    
    player2ScoreText.setFont(font);
    player2ScoreText.setCharacterSize(24);
    player2ScoreText.setFillColor(sf::Color::Blue);
    player2ScoreText.setPosition(sf::Vector2f(MAP_W * TILE - TILE * 4,10));

    // --- Timer setup ---
    sf::Clock gameClock;
    const int GAME_DURATION = 60;
    sf::Text timerText(font);
    timerText.setFont(font);
    timerText.setCharacterSize(24);
    timerText.setFillColor(sf::Color::White);
    timerText.setPosition(sf::Vector2f(MAP_W * TILE / 2 - 40.f, 10));

    bool gameOver = false;
    sf::Text winnerText(font);
    winnerText.setFont(font);
    winnerText.setCharacterSize(48);
    winnerText.setFillColor(sf::Color::White);
    winnerText.setPosition(sf::Vector2f(MAP_W * TILE / 2 - 150.f, MAP_H * TILE / 2 - 40.f));

    // --- Box spawner setup ---
    sf::Clock boxSpawnClock;

    // --- Load textures for players ---
    sf::Texture player1Tex;
    if (!player1Tex.loadFromFile("Assets/Player1.jpg")) {
        std::cerr << "Failed to load Assets/Player1.jpg\n";
        return 1;
    }
    sf::Texture player2Tex;
    if (!player2Tex.loadFromFile("Assets/Player2.jpg")) {
        std::cerr << "Failed to load Assets/Player2.jpg\n";
        return 1;
    }

    // --- Load pushable box texture ---
    sf::Texture pushableBoxTex;
    if (!pushableBoxTex.loadFromFile("Assets/box.jpg")) {
        std::cerr << "Failed to load Assets/box.jpg\n";
        return 1;
    }

    // --- Random number generator ---
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // --- Create Map object ---
    Map gameMap(MAP_W, MAP_H, TILE);
    gameMap.populateInitialState();

    // --- Create Players ---
    Player player1(&player1Tex, TILE, 1);
    Player player2(&player2Tex, TILE, 2);
    player1.setGridPosition(MAP_W / 4, MAP_H / 2);
    player2.setGridPosition((MAP_W * 3) / 4, MAP_H / 2);

    // --- Game loop ---
    while (window.isOpen()) {
        // ---------- Timer update ----------
        if (!gameOver) {
            int elapsed = static_cast<int>(gameClock.getElapsedTime().asSeconds());
            int remaining = GAME_DURATION - elapsed;
            if (remaining < 0) remaining = 0;
        
            int minutes = remaining / 60;
            int seconds = remaining % 60;
            timerText.setString(
                (seconds < 10 ? "0" : "") + std::to_string(minutes) + ":" + (seconds < 10 ? "0" : "") + std::to_string(seconds)
            );
        
            if (remaining <= 0) {
                gameOver = true;
            
                // Decide winner
                if (player1.getScore() > player2.getScore()) {
                    winnerText.setString("Player 1 Wins!");
                } else if (player2.getScore() > player1.getScore()) {
                    winnerText.setString("Player 2 Wins!");
                } else {
                    winnerText.setString("Draw!");
                }
            }
        }

        // ---------- Automatic Box Spawning ----------
        if (!gameOver && boxSpawnClock.getElapsedTime().asSeconds() >= 2.f) {
            boxSpawnClock.restart();
            gameMap.spawnRandomBox();
        }

        // Event loop
        while (auto ev = window.pollEvent()) {
            if (ev->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        // ---------- Realtime input handling ----------
        int p1_dx = 0, p1_dy = 0;
        int p2_dx = 0, p2_dy = 0;

        // Player 1 (WASD)
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::W)) p1_dy = -1;
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::S)) p1_dy = 1;
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::A)) p1_dx = -1;
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::D)) p1_dx = 1;

        // Player 2 (Arrow keys)
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Up)) p2_dy = -1;
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Down))  p2_dy = 1;
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Left))  p2_dx = -1;
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Right)) p2_dx = 1;

        // Movement helper function using player grid coordinates
        auto try_move_player = [&](Player& player, int dx, int dy, Player& otherPlayer, int playerNum) {
            if (dx == 0 && dy == 0) return;
            
            int currentX = player.getGridX();
            int currentY = player.getGridY();
            int newX = currentX + dx;
            int newY = currentY + dy;
            int otherX = otherPlayer.getGridX();
            int otherY = otherPlayer.getGridY();

            // prevent moving onto the other player's position
            if (newX == otherX && newY == otherY) return;

            // bounds check
            if (newX < 0 || newX >= MAP_W || newY < 0 || newY >= MAP_H) return;

            GameObject* target = gameMap.getTile(newX, newY);

            // if target does not block, move into it
            if (!target->blocks()) {
                player.setGridPosition(newX, newY);
                return;
            }

            PushableBox* pb = dynamic_cast<PushableBox*>(target);
            if (pb) {
                int boxNewX = newX + dx;
                int boxNewY = newY + dy;
                if (boxNewX < 0 || boxNewX >= MAP_W || boxNewY < 0 || boxNewY >= MAP_H) return;
                if ((boxNewX == otherX && boxNewY == otherY) || (boxNewX == currentX && boxNewY == currentY)) return;
                GameObject* boxTarget = gameMap.getTile(boxNewX, boxNewY);
                
                // Check if box is being pushed into a portal
                if (boxTarget->isPortal()) {
                    // Box disappears into portal, player gets points
                    gameMap.setTile(newX, newY, gameMap.makeFloorAt(newX, newY));
                    
                    // Award points to the player who pushed it
                    player.addScore(10);
                    
                    player.setGridPosition(newX, newY); // Player moves to where box was
                    return;
                }
                
                if (!boxTarget->blocks()) {
                    // Create a new pushable box at the target position
                    PushableBox* newBox = new PushableBox(&pushableBoxTex, TILE - 4.f);
                    newBox->setPosition(sf::Vector2f(boxNewX * TILE, boxNewY * TILE));
                    gameMap.setTile(boxNewX, boxNewY, newBox);
                    
                    // Replace old position with floor
                    gameMap.setTile(newX, newY, gameMap.makeFloorAt(newX, newY));
                    
                    player.setGridPosition(newX, newY);
                }
            }
        };

        // Simple simultaneous-move resolution:
        int p1_targetX = player1.getGridX() + p1_dx;
        int p1_targetY = player1.getGridY() + p1_dy;
        int p2_targetX = player2.getGridX() + p2_dx;
        int p2_targetY = player2.getGridY() + p2_dy;

        // If both intend to move into same tile, cancel both moves
        bool conflictSameTile = (p1_dx != 0 || p1_dy != 0) && (p2_dx != 0 || p2_dy != 0)
                                && (p1_targetX == p2_targetX && p1_targetY == p2_targetY);

        // If they intend to swap positions cancel both
        bool swapPositions = (p1_targetX == player2.getGridX() && p1_targetY == player2.getGridY()) &&
                             (p2_targetX == player1.getGridX() && p2_targetY == player1.getGridY());

        if (!conflictSameTile && !swapPositions) {
            try_move_player(player1, p1_dx, p1_dy, player2, 1);
            try_move_player(player2, p2_dx, p2_dy, player1, 2);
        }

        // Update score text
        player1ScoreText.setString("Player 1: " + std::to_string(player1.getScore()));
        player2ScoreText.setString("Player 2: " + std::to_string(player2.getScore()));

        // ---------- Drawing ----------
        window.clear(sf::Color::Black);
        gameMap.draw(window);
        window.draw(player1);
        window.draw(player2);
        
        window.draw(player1ScoreText);
        window.draw(player2ScoreText);
        window.draw(timerText);

        if (gameOver) {
            window.draw(winnerText);
        }
        
        window.display();
    }

    return 0;
}