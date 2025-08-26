#ifndef BOX_HPP
#define BOX_HPP

#include "GameObject.hpp"

class Box : public GameObject {
public:
    Box(const sf::Texture* tex = nullptr, float desiredSize = 0.f);
    void setSize(const sf::Vector2f& size) override;

    bool isPortal() const override;
    bool isPushable() const override;
};

#endif // BOX_HPP
