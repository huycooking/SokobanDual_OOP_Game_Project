#include "../include/Box.hpp"

void Box::onPush(Player* p) {
    // Simulate push into portal
    p->addScore(points);
}