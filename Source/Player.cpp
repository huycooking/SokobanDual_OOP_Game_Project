#include "Player.hpp"
#include <iostream>

Player::Player(const sf::Texture* tex, float desiredSize, int playerNumber)
    : GameObject(tex, desiredSize), playerNumber(playerNumber), score(0), tileSize(desiredSize) {
    
    // Players don't block movement
    blocksFlag = false;
    
    // Initial grid position will be set by setGridPosition
    gridX = 0;
    gridY = 0;
}

void Player::setGridPosition(int x, int y) {
    gridX = x;
    gridY = y;
    setPosition(sf::Vector2f(x * tileSize + 2.f, y * tileSize + 2.f));
}

void Player::move(int dx, int dy) {
    setGridPosition(gridX + dx, gridY + dy);
}