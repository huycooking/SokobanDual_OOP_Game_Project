#include "Map.hpp"
#include <iostream>

Map::Map(int w, int h, float tileSize, TextureManager& tex)
    : m_w(w), m_h(h), m_tile(tileSize), m_tex(tex) 
{
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    // allocate array
    tiles = new GameObject**[m_h];
    for (int y = 0; y < m_h; ++y) {
        tiles[y] = new GameObject*[m_w];
    }
    init();
}

Map::~Map() {
    for (int y = 0; y < m_h; ++y) {
        for (int x = 0; x < m_w; ++x) {
            delete tiles[y][x];
        }
        delete[] tiles[y];
    }
    delete[] tiles;
}

Floor* Map::makeFloorAt(int x, int y) {
    Floor* g = new Floor(&m_tex.floorTex, m_tile - 2.f);
    g->setSize(sf::Vector2f(m_tile - 1.f, m_tile - 1.f));
    g->setPosition(sf::Vector2f(x * m_tile, y * m_tile));
    return g;
}

void Map::init() {
    // Fill all with floors
    for (int y = 0; y < m_h; ++y) {
        for (int x = 0; x < m_w; ++x) {
            tiles[y][x] = makeFloorAt(x, y);
        }
    }

    // Add some immovable walls (special box)
    std::vector<std::pair<int,int>> wallSpots = {
        {m_w/2, m_h/2},
        {m_w/2-2, m_h/2}, {m_w/2+2, m_h/2},
        {m_w/2, m_h/2-2}, {m_w/2, m_h/2+2},
        {5,5}, {5,6}, {6,5},
        {m_w-6, m_h-6}, {m_w-5, m_h-6}, {m_w-6, m_h-5}
    };
    for (auto [wx,wy] : wallSpots) {
        if (wx>=0 && wx<m_w && wy>=0 && wy<m_h) {
            delete tiles[wy][wx];
            tiles[wy][wx] = new Box(&m_tex.specialBoxTex, m_tile - 2.f);
            tiles[wy][wx]->setPosition(sf::Vector2f(wx * m_tile, wy * m_tile));
        }
    }

    // Scatter pushable boxes
    std::vector<std::pair<int,int>> pushableSpots = {
        {3,3}, {7,4}, {10,6}, {12,8}, {m_w/2-3, m_h/2-1},
        {m_w/2+3, m_h/2+1}, {8, m_h-5}, {m_w-8, 5}
    };
    for (auto [bx,by] : pushableSpots) {
        if (bx>=0 && bx<m_w && by>=0 && by<m_h) {
            delete tiles[by][bx];
            tiles[by][bx] = new PushableBox(&m_tex.pushableBoxTex, m_tile - 4.f);
            tiles[by][bx]->setPosition(sf::Vector2f(bx * m_tile, by * m_tile));
        }
    }

    // Place portals
    std::vector<std::pair<int,int>> portalSpots = {
        {m_w/2+4, m_h/2+3},
        {4, m_h-4},
        {m_w-5, 4},
        {m_w/2, 3}
    };
    for (auto [px,py] : portalSpots) {
        if (px>=0 && px<m_w && py>=0 && py<m_h) {
            delete tiles[py][px];
            tiles[py][px] = new Portal(m_tex.portalTex.getSize().x > 0 ? &m_tex.portalTex : nullptr, m_tile - 1.f);
            tiles[py][px]->setPosition(sf::Vector2f(px * m_tile, py * m_tile));
        }
    }
}

void Map::draw(sf::RenderTarget& target) {
    for (int y = 0; y < m_h; ++y) {
        for (int x = 0; x < m_w; ++x) {
            target.draw(*tiles[y][x]);
        }
    }
}

bool Map::blocks(int x, int y) const {
    if (x<0 || x>=m_w || y<0 || y>=m_h) return true;
    return tiles[y][x]->blocks();
}

bool Map::isPushable(int x, int y) const {
    if (x<0 || x>=m_w || y<0 || y>=m_h) return false;
    return tiles[y][x]->isPushable();
}

bool Map::isPortal(int x, int y) const {
    if (x<0 || x>=m_w || y<0 || y>=m_h) return false;
    return tiles[y][x]->isPortal();
}

GameObject* Map::tileAt(int x, int y) const {
    if (x<0 || x>=m_w || y<0 || y>=m_h) return nullptr;
    return tiles[y][x];
}

void Map::replaceWithFloor(int x, int y) {
    if (x<0 || x>=m_w || y<0 || y>=m_h) return;
    delete tiles[y][x];
    tiles[y][x] = makeFloorAt(x, y);
}

bool Map::tryMoveBox(int srcX, int srcY, int dstX, int dstY) {
    if (srcX<0 || srcX>=m_w || srcY<0 || srcY>=m_h) return false;
    if (dstX<0 || dstX>=m_w || dstY<0 || dstY>=m_h) return false;

    GameObject* src = tiles[srcY][srcX];
    GameObject* dst = tiles[dstY][dstX];

    PushableBox* pb = dynamic_cast<PushableBox*>(src);
    if (!pb) return false;

    if (dst->isPortal()) {
        // box disappears into portal
        delete tiles[srcY][srcX];
        tiles[srcY][srcX] = makeFloorAt(srcX, srcY);
        return true; // signal that the box vanished (points should be awarded by caller)
    }

    if (!dst->blocks()) {
        delete dst;
        tiles[dstY][dstX] = tiles[srcY][srcX];
        tiles[dstY][dstX]->setPosition(sf::Vector2f(dstX * m_tile, dstY * m_tile));
        tiles[srcY][srcX] = makeFloorAt(srcX, srcY);
        return true;
    }
    return false;
}

void Map::spawnRandomBox() {
    int attempts = 0;
    while (attempts < 20) {
        int rx = std::rand() % m_w;
        int ry = std::rand() % m_h;
        GameObject* t = tiles[ry][rx];
        if (!t->isPushable() && !t->isPortal() && !t->blocks()) {
            delete tiles[ry][rx];
            tiles[ry][rx] = new PushableBox(&m_tex.pushableBoxTex, m_tile - 4.f);
            tiles[ry][rx]->setPosition(sf::Vector2f(rx * m_tile, ry * m_tile));
            break;
        }
        ++attempts;
    }
}
