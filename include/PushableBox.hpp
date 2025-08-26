#ifndef PUSHABLEBOX_HPP
#define PUSHABLEBOX_HPP

#include "GameObject.hpp"

class PushableBox : public GameObject {
public:
    PushableBox(const sf::Texture* tex = nullptr, float desiredSize = 0.f);

    bool isPushable() const override;
    bool isPortal() const override;

    void setSize(const sf::Vector2f& size) override;
};

#endif // PUSHABLEBOX_HPP
