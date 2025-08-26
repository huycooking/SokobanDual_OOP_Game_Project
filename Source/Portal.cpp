#include "Portal.hpp"

Portal::Portal(const sf::Texture* tex, float desiredSize)
    : GameObject(tex, desiredSize)
{
    // walkable like floor
    blocksFlag = false;

    if (!sprite) {
        if (desiredSize > 0.f) shape.setSize(sf::Vector2f(desiredSize, desiredSize));
        shape.setFillColor(sf::Color(255, 105, 180)); // hot pink fallback
    }
}

bool Portal::isPortal() const {
    return true;
}

bool Portal::isPushable() const {
    return false;
}
