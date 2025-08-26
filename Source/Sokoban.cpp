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
    window.setKeyRepeatEnabled(false); // disable OS key repeat so event repeats don't interfere (still using polling below)

    // --- Player scores ---
    int player1Score = 0;
    int player2Score = 0;
    
    // --- Font for score display ---
    sf::Font font;
    // Try to load a font - if it fails, we'll use default rendering
    if (!font.openFromFile("Assets/ARLRDBD.ttf")) {
        // If custom font fails, we'll still display scores but with default font
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
    player2ScoreText.setPosition(sf::Vector2f(MAP_W * TILE - TILE * 3,10));

    // --- Timer setup ---
    sf::Clock gameClock;                   // to measure elapsed time
    const int GAME_DURATION = 60;          // 1 min in seconds
    sf::Text timerText(font);
    timerText.setFont(font);
    timerText.setCharacterSize(24);
    timerText.setFillColor(sf::Color::White);
    timerText.setPosition(sf::Vector2f(MAP_W * TILE / 2 - 40.f, 10)); // center-ish

    bool gameOver = false;   // track if timer has ended
    sf::Text winnerText(font);
    winnerText.setFont(font);
    winnerText.setCharacterSize(48);
    winnerText.setFillColor(sf::Color::White);
    winnerText.setPosition(sf::Vector2f(MAP_W * TILE / 2 - 150.f, MAP_H * TILE / 2 - 40.f));

    // --- Box spawner setup ---
    sf::Clock boxSpawnClock;

    // --- Load textures for players ---
    // player textures
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

    // --- Random number generator ---
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // --- Create Map object ---
    Map gameMap(MAP_W, MAP_H, TILE);
    gameMap.populateInitialState();

    // --- Player 1 setup (sprite from Assets/Player1.jpg, WASD) ---
    sf::Sprite player1(player1Tex);
    float desiredSize = TILE - 4.f;
    auto t1sz = player1Tex.getSize();
    if (t1sz.x > 0 && t1sz.y > 0) {
        player1.setScale(sf::Vector2f(desiredSize / static_cast<float>(t1sz.x),
                                     desiredSize / static_cast<float>(t1sz.y)));
    }

    int p1X = MAP_W / 4;
    int p1Y = MAP_H / 2;
    player1.setPosition(sf::Vector2f(p1X * TILE + 2.f, p1Y * TILE + 2.f));

    // --- Player 2 setup (sprite from Assets/Player2.jpg, Arrow keys) ---
    sf::Sprite player2(player2Tex);
    auto t2sz = player2Tex.getSize();
    if (t2sz.x > 0 && t2sz.y > 0) {
        player2.setScale(sf::Vector2f(desiredSize / static_cast<float>(t2sz.x),
                                     desiredSize / static_cast<float>(t2sz.y)));
    }

    int p2X = (MAP_W * 3) / 4;
    int p2Y = MAP_H / 2;
    player2.setPosition(sf::Vector2f(p2X * TILE + 2.f, p2Y * TILE + 2.f));

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
                if (player1Score > player2Score) {
                    winnerText.setString("Player 1 Wins!");
                } else if (player2Score > player1Score) {
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

        // Event loop: only use events for window/system events now
        while (auto ev = window.pollEvent()) {
            if (ev->is<sf::Event::Closed>()) {
                window.close();
            }
            // remove per-key movement handling from here
        }

        // ---------- Realtime (polled) input handling ----------
        // compute each player's desired direction (dx,dy) based on keys held this frame
        int p1_dx = 0, p1_dy = 0;
        int p2_dx = 0, p2_dy = 0;

        // Player 1 (WASD) - using scancodes to match your event usage
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::W)) p1_dy = -1;
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::S)) p1_dy = 1;
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::A)) p1_dx = -1;
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::D)) p1_dx = 1;

        // Player 2 (Arrow keys)
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Up)) p2_dy = -1;
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Down))  p2_dy = 1;
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Left))  p2_dx = -1;
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Right)) p2_dx = 1;

        // helper used already in your code (keeps push logic centralized)
        auto try_move_player = [&](int &px, int &py, int dx, int dy, int otherX, int otherY, int playerNum) {
    if (dx == 0 && dy == 0) return; // no movement intended
    int newX = px + dx;
    int newY = py + dy;

    // prevent moving onto the other player's *current* position
    if (newX == otherX && newY == otherY) return;

    // bounds check
    if (newX < 0 || newX >= MAP_W || newY < 0 || newY >= MAP_H) return;

    GameObject* target = gameMap.getTile(newX, newY);

    // if target does not block, move into it
    if (!target->blocks()) {
        px = newX;
        py = newY;
        return;
    }

    PushableBox* pb = dynamic_cast<PushableBox*>(target);
    if (pb) {
        int boxNewX = newX + dx;
        int boxNewY = newY + dy;
        if (boxNewX < 0 || boxNewX >= MAP_W || boxNewY < 0 || boxNewY >= MAP_H) return;
        if ((boxNewX == otherX && boxNewY == otherY) || (boxNewX == px && boxNewY == py)) return;
        GameObject* boxTarget = gameMap.getTile(boxNewX, boxNewY);
        
        // Check if box is being pushed into a portal
        if (boxTarget->isPortal()) {
            // Box disappears into portal, player gets points
            gameMap.setTile(newX, newY, gameMap.makeFloorAt(newX, newY));
            
            // Award points to the player who pushed it
            if (playerNum == 1) {
                player1Score += 10;
            } else {
                player2Score += 10;
            }
            
            px = newX; // Player moves to where box was
            py = newY;
            return;
        }
        
        if (!boxTarget->blocks()) {
            // Create a new pushable box at the target position
            PushableBox* newBox = gameMap.createPushableBox(boxNewX, boxNewY);
            gameMap.setTile(boxNewX, boxNewY, newBox);
            
            // Replace old position with floor
            gameMap.setTile(newX, newY, gameMap.makeFloorAt(newX, newY));
            
            px = newX;
            py = newY;
        } else {
            return;
        }
    } else {
        return;
    }
};

        // Simple simultaneous-move resolution:
        // - compute intended destinations and avoid allowing both players to move into the same tile
        int p1_targetX = p1X + p1_dx, p1_targetY = p1Y + p1_dy;
        int p2_targetX = p2X + p2_dx, p2_targetY = p2Y + p2_dy;

        // If both intend to move into same tile, cancel both moves (could also pick priority)
        bool conflictSameTile = (p1_dx != 0 || p1_dy != 0) && (p2_dx != 0 || p2_dy != 0)
                                && (p1_targetX == p2_targetX && p1_targetY == p2_targetY);

        // If they intend to swap positions (p1 -> p2 current and p2 -> p1 current) cancel both
        bool swapPositions = (p1_targetX == p2X && p1_targetY == p2Y) &&
                             (p2_targetX == p1X && p2_targetY == p1Y);

        if (!conflictSameTile && !swapPositions) {
            // apply both moves (order here matters if boxes involved; you might prefer atomic resolution)
            try_move_player(p1X, p1Y, p1_dx, p1_dy, p2X, p2Y, 1);
            try_move_player(p2X, p2Y, p2_dx, p2_dy, p1X, p1Y, 2);
        } else {
            // if conflict, no moves this frame; alternative: prioritize one player
        }

        // update sprite pixel positions
        player1.setPosition(sf::Vector2f(p1X * TILE + 2.f, p1Y * TILE + 2.f));
        player2.setPosition(sf::Vector2f(p2X * TILE + 2.f, p2Y * TILE + 2.f));

        // Update score text
        player1ScoreText.setString("Player 1: " + std::to_string(player1Score));
        player2ScoreText.setString("Player 2: " + std::to_string(player2Score));

        // ---------- Drawing ----------
        window.clear(sf::Color::Black);
        gameMap.draw(window);
        window.draw(player1);
        window.draw(player2);
        
        // Draw score display
        window.draw(player1ScoreText);
        window.draw(player2ScoreText);

        // Draw timer
        window.draw(timerText);

        // If game over, show winner
        if (gameOver) {
            window.draw(winnerText);
        }
        
        window.display();
    }

    return 0;
}