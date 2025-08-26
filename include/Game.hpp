#ifndef GAME_HPP
#define GAME_HPP

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Map.hpp"
#include "Player.hpp"
#include "WoodenBox.hpp"
#include "MetalBox.hpp"

class Game {
public:
    Game();
    void run();
    
private:
    void processEvents();
    void update();
    void render();
    
    // Window and constants
    sf::RenderWindow window;
    static constexpr int MAP_W = 16 * 2;
    static constexpr int MAP_H = 9 * 2;
    static constexpr float TILE = 37.f;
    static constexpr int GAME_DURATION = 150;
    
    // Game objects
    Map gameMap;
    Player player1;
    Player player2;
    
    // Resources
    sf::Font font;
    sf::Texture player1Tex;
    sf::Texture player2Tex;
    sf::Texture woodenBoxTex;
    
    // Audio
    sf::Music themeMusic;
    sf::SoundBuffer gainPointsBuffer;
    sf::Sound gainPointsSound;  // ADD THIS LINE
    
    // UI
    sf::Text player1ScoreText;
    sf::Text player2ScoreText;
    sf::Text timerText;
    sf::Text winnerText;
    
    // State
    sf::Clock gameClock;
    sf::Clock boxSpawnClock;
    bool gameOver;
};

#endif // GAME_HPP