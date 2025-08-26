#include "Player.hpp"
#include <iostream>

Player::Player(const sf::Texture* tex, float tileSize, int playerNumber)
    : GameObject(tex, tileSize - 4.f), playerNumber(playerNumber), score(0), tileSize(tileSize) {
    
    // Players don't block movement
    blocksFlag = false;
    
    // Initial grid position will be set by setGridPosition
    gridX = 0;
    gridY = 0;
}

void Player::setGridPosition(int x, int y) {
    gridX = x;
    gridY = y;
    // Convert grid coordinates to pixel coordinates with padding
    setPosition(sf::Vector2f(x * tileSize + 2.f, y * tileSize + 2.f));
}

void Player::move(int dx, int dy) {
    setGridPosition(gridX + dx, gridY + dy);
}

Player::Player() : score(0) {} // only exist here to encounter bugs in Game.cpp