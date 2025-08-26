#ifndef PORTAL_HPP
#define PORTAL_HPP

#include "GameObject.hpp"

class Portal : public GameObject {
public:
    Portal(const sf::Texture* tex = nullptr, float desiredSize = 0.f);

    bool isPortal() const override;
    bool isPushable() const override;
};

#endif // PORTAL_HPP
