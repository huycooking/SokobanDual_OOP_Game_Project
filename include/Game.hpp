#pragma once
#include <SFML/Graphics.hpp>
#include "TextureManager.hpp"
#include "Map.hpp"
#include <optional>

class Game {
public:
    Game(sf::RenderWindow& win, TextureManager& tex, int mapW, int mapH, float tileSize);
    // run a single game round; returns when round ends OR window closed.
    void run();

private:
    void updateTimer();
    void updateInputAndMovement();
    void drawAll();
    void tryMovePlayer(int &px, int &py, int dx, int dy, int otherX, int otherY, int playerNum);

    sf::RenderWindow& window;
    TextureManager& m_tex;
    Map m_map;

    // players (no default ctor in SFML3 -> use optional and emplace later)
    std::optional<sf::Sprite> player1;
    std::optional<sf::Sprite> player2;
    int p1X, p1Y;
    int p2X, p2Y;
    float m_tile;

    // scores & UI
    int player1Score;
    int player2Score;
    std::optional<sf::Text> player1ScoreText;
    std::optional<sf::Text> player2ScoreText;
    sf::Font& fontRef;

    // timer
    sf::Clock gameClock;
    const int GAME_DURATION = 60;
    std::optional<sf::Text> timerText;
    bool gameOver;
    std::optional<sf::Text> winnerText;

    // spawning
    sf::Clock boxSpawnClock;
};
