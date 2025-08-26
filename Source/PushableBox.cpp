#include "PushableBox.hpp"

PushableBox::PushableBox(const sf::Texture* tex, float desiredSize)
    : GameObject(tex, desiredSize)
{
    // blocks movement until pushed
    blocksFlag = true;
}

bool PushableBox::isPushable() const {
    return true;
}

bool PushableBox::isPortal() const {
    return false;
}

void PushableBox::setSize(const sf::Vector2f& size) {
    shape.setSize(size);
}
