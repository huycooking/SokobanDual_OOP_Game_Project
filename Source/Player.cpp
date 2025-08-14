#include "../include/Player.hpp"
#include "../include/Wall.hpp"

void Player::update(float dt) {
    // Handle input, collision checks (simplified)
}

void Player::move(Direction d) {
    float speed = 100.0f;
    if (d == Direction::UP) position.y -= speed;
    // Similar for others, update sprite
    sprite.setPosition(position);
}

void Player::useAbility(sf::Vector2f pos) {
    if (currentAbility) currentAbility->execute(this, pos);
}

void Player::breakWall(Wall* w, float dt) {
    breakTimer += dt;
    if (breakTimer >= 2.0f) {
        w->destroy();
        breakTimer = 0.0f;
    }
}