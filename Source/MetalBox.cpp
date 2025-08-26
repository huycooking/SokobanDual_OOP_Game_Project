#include "MetalBox.hpp"

MetalBox::MetalBox(const sf::Texture* tex, float desiredSize)
    : GameObject(tex, desiredSize)
{
    // immovable blocking tile
    blocksFlag = true;
}

void MetalBox::setSize(const sf::Vector2f& size) {
    shape.setSize(size);
}

bool MetalBox::isPortal() const {
    return false;
}

// Metal boxes should NOT be pushable (contrary to original Box behavior)
bool MetalBox::isPushable() const {
    return false;
}