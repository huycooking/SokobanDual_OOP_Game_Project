#pragma once
#include "GameObject.hpp"

class Player : public GameObject {
public:
    Player(const sf::Texture* tex, float desiredSize, int playerNumber);
    
    // GameObject overrides
    bool isPushable() const override { return false; }
    bool isPortal() const override { return false; }
    
    // Player specific methods
    void setGridPosition(int x, int y);
    void move(int dx, int dy);
    
    int getX() const { return gridX; }
    int getY() const { return gridY; }
    int getPlayerNumber() const { return playerNumber; }
    int getScore() const { return score; }
    void addScore(int points) { score += points; }
    
private:
    int gridX, gridY;
    int score;
    int playerNumber;
    float tileSize;
};