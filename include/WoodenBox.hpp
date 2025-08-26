#ifndef WOODENBOX_HPP
#define WOODENBOX_HPP

#include "GameObject.hpp"

class WoodenBox : public GameObject {
public:
    WoodenBox(const sf::Texture* tex = nullptr, float desiredSize = 0.f);

    bool isPushable() const override;
    bool isPortal() const override;

    void setSize(const sf::Vector2f& size) override;
};

#endif // WOODENBOX_HPP