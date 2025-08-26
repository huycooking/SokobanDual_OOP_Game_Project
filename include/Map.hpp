#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <utility>
#include <cstdlib>
#include <ctime>
#include "GameObject.hpp"
#include "Floor.hpp"
#include "Box.hpp"
#include "PushableBox.hpp"
#include "Portal.hpp"
#include "TextureManager.hpp"

class Map {
public:
    Map(int w, int h, float tileSize, TextureManager& tex);
    ~Map();

    void init(); // fill with floors and initial obstacles/boxes/portals
    void draw(sf::RenderTarget& target);
    bool blocks(int x, int y) const;         // does tile block movement?
    bool isPushable(int x, int y) const;
    bool isPortal(int x, int y) const;
    GameObject* tileAt(int x, int y) const;
    void replaceWithFloor(int x, int y);
    bool tryMoveBox(int srcX, int srcY, int dstX, int dstY); // move pushable box if possible (returns true if moved)
    void spawnRandomBox(); // spawn pushable box on a random floor tile

    int width() const { return m_w; }
    int height() const { return m_h; }
    float tileSize() const { return m_tile; }

private:
    Floor* makeFloorAt(int x, int y);

    int m_w;
    int m_h;
    float m_tile;
    TextureManager& m_tex;
    GameObject*** tiles;
};
