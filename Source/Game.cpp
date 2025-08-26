#include "Game.hpp"
#include <iostream>
#include <cstdlib>
#include <ctime>

Game::Game() 
    : window(sf::VideoMode({static_cast<unsigned>(MAP_W * TILE), static_cast<unsigned>(MAP_H * TILE)}), "Sokuban dual!"),
      gameMap(MAP_W, MAP_H, TILE),
      player1(&player1Tex, TILE, 1),
      player2(&player2Tex, TILE, 2),
      gameOver(false),
      // Initialize sf::Sound with an empty sound buffer (we'll set it properly later)
      gainPointsSound(gainPointsBuffer),
      // Initialize sf::Text objects with the font
      player1ScoreText(font),
      player2ScoreText(font),
      timerText(font),
      winnerText(font)
{
    window.setFramerateLimit(10);
    window.setKeyRepeatEnabled(false);
    
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    
    // Load textures
    if (!player1Tex.loadFromFile("Assets/Player1.jpg")) {
        std::cerr << "Failed to load Assets/Player1.jpg\n";
    }
    if (!player2Tex.loadFromFile("Assets/Player2.jpg")) {
        std::cerr << "Failed to load Assets/Player2.jpg\n";
    }
    if (!woodenBoxTex.loadFromFile("Assets/box.jpg")) {
        std::cerr << "Failed to load Assets/box.jpg\n";
    }
    
    // Load audio
    if (!themeMusic.openFromFile("Assets/theme.mp3")) {
        std::cerr << "Failed to load theme music: Assets/theme.mp3\n";
    } else {
        themeMusic.setLooping(true);
        themeMusic.play();
    }
    
    if (!gainPointsBuffer.loadFromFile("Assets/gainpoints.mp3")) {
        std::cerr << "Failed to load sound effect: Assets/gainpoints.mp3\n";
    }
    // No need to set buffer again since it was initialized with gainPointsBuffer
    
    // Load font
    if (!font.openFromFile("Assets/ARLRDBD.ttf")) {
        std::cout << "Could not load Assets/ARLRDBD.ttf, using default font\n";
    }
    
    // Setup UI (remove font setting since it's already set in initializer list)
    player1ScoreText.setCharacterSize(24);
    player1ScoreText.setFillColor(sf::Color::Blue);
    player1ScoreText.setPosition(sf::Vector2f(10,10));
    
    player2ScoreText.setCharacterSize(24);
    player2ScoreText.setFillColor(sf::Color::Blue);
    player2ScoreText.setPosition(sf::Vector2f(MAP_W * TILE - TILE * 4,10));

    timerText.setCharacterSize(24);
    timerText.setFillColor(sf::Color::White);
    timerText.setPosition(sf::Vector2f(MAP_W * TILE / 2 - 40.f, 10));

    winnerText.setCharacterSize(48);
    winnerText.setFillColor(sf::Color::White);
    winnerText.setPosition(sf::Vector2f(MAP_W * TILE / 2 - 150.f, MAP_H * TILE / 2 - 40.f));
    
    // Setup game state
    gameMap.populateInitialState();
    player1.setGridPosition(MAP_W / 4, MAP_H / 2);
    player2.setGridPosition((MAP_W * 3) / 4, MAP_H / 2);
}

void Game::run() {
    while (window.isOpen()) {
        processEvents();
        update();
        render();
    }
}

void Game::processEvents() {
    while (auto ev = window.pollEvent()) {
        if (ev->is<sf::Event::Closed>()) {
            window.close();
        }
    }
}

void Game::update() {
    // Timer update
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
            if (themeMusic.getStatus() == sf::Sound::Status::Playing) {
                themeMusic.stop();
            }
            
            if (player1.getScore() > player2.getScore()) {
                winnerText.setString("Player 1 Wins!");
            } else if (player2.getScore() > player1.getScore()) {
                winnerText.setString("Player 2 Wins!");
            } else {
                winnerText.setString("Draw!");
            }
        }
    }

    // Box spawning
    if (!gameOver && boxSpawnClock.getElapsedTime().asSeconds() >= 2.f) {
        boxSpawnClock.restart();
        gameMap.spawnRandomBox();
    }

    // Player input
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

    // Movement helper function
    auto try_move_player = [&](Player& player, int dx, int dy, Player& otherPlayer, int playerNum) {
        if (dx == 0 && dy == 0) return;
        
        int currentX = player.getGridX();
        int currentY = player.getGridY();
        int newX = currentX + dx;
        int newY = currentY + dy;
        int otherX = otherPlayer.getGridX();
        int otherY = otherPlayer.getGridY();

        if (newX == otherX && newY == otherY) return;
        if (newX < 0 || newX >= MAP_W || newY < 0 || newY >= MAP_H) return;

        GameObject* target = gameMap.getTile(newX, newY);

        if (!target->blocks()) {
            player.setGridPosition(newX, newY);
            return;
        }

        WoodenBox* wb = dynamic_cast<WoodenBox*>(target);
        if (wb) {
            int boxNewX = newX + dx;
            int boxNewY = newY + dy;
            if (boxNewX < 0 || boxNewX >= MAP_W || boxNewY < 0 || boxNewY >= MAP_H) return;
            if ((boxNewX == otherX && boxNewY == otherY) || (boxNewX == currentX && boxNewY == currentY)) return;
            GameObject* boxTarget = gameMap.getTile(boxNewX, boxNewY);
            
            if (boxTarget->isPortal()) {
                gameMap.setTile(newX, newY, gameMap.makeFloorAt(newX, newY));
                player.addScore(10);
                
                if (gainPointsSound.getStatus() != sf::Sound::Status::Playing) {
                    gainPointsSound.play();
                }
                
                player.setGridPosition(newX, newY);
                return;
            }
            
            if (!boxTarget->blocks()) {
                WoodenBox* newBox = new WoodenBox(&woodenBoxTex, TILE - 4.f);
                newBox->setPosition(sf::Vector2f(boxNewX * TILE, boxNewY * TILE));
                gameMap.setTile(boxNewX, boxNewY, newBox);
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

    bool conflictSameTile = (p1_dx != 0 || p1_dy != 0) && (p2_dx != 0 || p2_dy != 0)
                            && (p1_targetX == p2_targetX && p1_targetY == p2_targetY);

    bool swapPositions = (p1_targetX == player2.getGridX() && p1_targetY == player2.getGridY()) &&
                         (p2_targetX == player1.getGridX() && p2_targetY == player1.getGridY());

    if (!conflictSameTile && !swapPositions) {
        try_move_player(player1, p1_dx, p1_dy, player2, 1);
        try_move_player(player2, p2_dx, p2_dy, player1, 2);
    }

    // Update score text
    player1ScoreText.setString("Player 1: " + std::to_string(player1.getScore()));
    player2ScoreText.setString("Player 2: " + std::to_string(player2.getScore()));
}

void Game::render() {
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