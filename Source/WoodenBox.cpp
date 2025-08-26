#include "WoodenBox.hpp"

WoodenBox::WoodenBox(const sf::Texture* tex, float desiredSize)
    : GameObject(tex, desiredSize)
{
    // blocks movement until pushed
    blocksFlag = true;
}

bool WoodenBox::isPushable() const {
    return true;
}

bool WoodenBox::isPortal() const {
    return false;
}

void WoodenBox::setSize(const sf::Vector2f& size) {
    shape.setSize(size);
}