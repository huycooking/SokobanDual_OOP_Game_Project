#include "GameObject.hpp"

#include <utility>

GameObject::GameObject(const sf::Texture* tex, float desiredSize)
{
    shape.setFillColor(sf::Color::White);

    if (tex) {
        sprite.emplace(*tex);

        if (desiredSize > 0.f) {
            auto tsz = tex->getSize();
            if (tsz.x > 0 && tsz.y > 0) {
                sprite->setScale(sf::Vector2f(
                    desiredSize / static_cast<float>(tsz.x),
                    desiredSize / static_cast<float>(tsz.y)
                ));
            }
            shape.setSize(sf::Vector2f(desiredSize, desiredSize));
        }
    }
}

GameObject::~GameObject() = default;

void GameObject::setSize(const sf::Vector2f& size) {
    shape.setSize(size);
}

void GameObject::setPosition(const sf::Vector2f& pos) {
    shape.setPosition(pos);
    if (sprite) sprite->setPosition(pos);
}

void GameObject::setFillColor(const sf::Color& color) {
    shape.setFillColor(color);
}

bool GameObject::blocks() const {
    return blocksFlag;
}

void GameObject::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    if (sprite) target.draw(*sprite, states);
    else target.draw(shape, states);
}
