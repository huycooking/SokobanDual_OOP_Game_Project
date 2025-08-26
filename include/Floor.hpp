#ifndef FLOOR_HPP
#define FLOOR_HPP

#include "GameObject.hpp"

class Floor : public GameObject {
public:
    Floor(const sf::Texture* tex = nullptr, float desiredSize = 0.f);

    bool isPushable() const override;
    bool isPortal() const override;
};

#endif // FLOOR_HPP
