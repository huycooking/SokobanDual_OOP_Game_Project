#ifndef METALBOX_HPP
#define METALBOX_HPP

#include "GameObject.hpp"

class MetalBox : public GameObject {
public:
    MetalBox(const sf::Texture* tex = nullptr, float desiredSize = 0.f);
    void setSize(const sf::Vector2f& size) override;

    bool isPortal() const override;
    bool isPushable() const override;
};

#endif // METALBOX_HPP