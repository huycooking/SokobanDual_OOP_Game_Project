#include "Floor.hpp"

Floor::Floor(const sf::Texture* tex, float desiredSize)
    : GameObject(tex, desiredSize)
{
    // walkable
    blocksFlag = false;
}

bool Floor::isPushable() const {
    return false;
}

bool Floor::isPortal() const {
    return false;
}
