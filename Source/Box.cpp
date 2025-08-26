#include "Box.hpp"

Box::Box(const sf::Texture* tex, float desiredSize)
    : GameObject(tex, desiredSize)
{
    // immovable blocking tile
    blocksFlag = true;
}

void Box::setSize(const sf::Vector2f& size) {
    shape.setSize(size);
}

bool Box::isPortal() const {
    return false;
}

// Kept same as your working code: Box considered pushable in original code
bool Box::isPushable() const {
    return true;
}
