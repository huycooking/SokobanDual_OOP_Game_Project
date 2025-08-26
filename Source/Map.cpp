#include "Map.hpp"
#include <iostream>

Map::Map(int width, int height, float tileSize)
    : MAP_W(width), MAP_H(height), TILE(tileSize) {

    // allocate tile grid
    tiles = new GameObject**[MAP_H];
    for (int y = 0; y < MAP_H; ++y) {
        tiles[y] = new GameObject*[MAP_W];
    }

    // load base textures
    if (!floorTexture.loadFromFile("Assets/floor.jpg"))
        std::cerr << "Failed to load Assets/floor.jpg\n";
    if (!specialBoxTex.loadFromFile("Assets/SpecialBox.jpg"))
        std::cerr << "Failed to load Assets/SpecialBox.jpg\n";
    if (!pushableBoxTex.loadFromFile("Assets/box.jpg"))
        std::cerr << "Failed to load Assets/box.jpg\n";
    if (!portalTex.loadFromFile("Assets/portal.jpg"))
        std::cerr << "Failed to load Assets/portal.jpg\n";

    // initialize with floor
    for (int y = 0; y < MAP_H; ++y) {
        for (int x = 0; x < MAP_W; ++x) {
            tiles[y][x] = makeFloorAt(x, y);
        }
    }
}

Map::~Map() {
    for (int y = 0; y < MAP_H; ++y) {
        for (int x = 0; x < MAP_W; ++x) {
            delete tiles[y][x];
        }
        delete[] tiles[y];
    }
    delete[] tiles;
}

Floor* Map::makeFloorAt(int x, int y) {
    Floor* g = new Floor(&floorTexture, TILE);
    g->setSize(sf::Vector2f(TILE, TILE));
    g->setPosition(sf::Vector2f(x * TILE, y * TILE));
    return g;
}

void Map::draw(sf::RenderWindow& window) {
    for (int y = 0; y < MAP_H; ++y) {
        for (int x = 0; x < MAP_W; ++x) {
            window.draw(*tiles[y][x]);
        }
    }
}

GameObject* Map::getTile(int x, int y) {
    return tiles[y][x];
}

void Map::setTile(int x, int y, GameObject* obj) {
    delete tiles[y][x];
    tiles[y][x] = obj;
}

// --- Example population methods ---
void Map::placeWalls(const sf::Texture& wallTex) {
    std::vector<std::pair<int,int>> wallSpots = {
        {MAP_W/2, MAP_H/2},
        {MAP_W/2-2, MAP_H/2}, {MAP_W/2+2, MAP_H/2},
        {MAP_W/2, MAP_H/2-2}, {MAP_W/2, MAP_H/2+2},
        {5,5}, {5,6}, {6,5},
        {MAP_W-6, MAP_H-6}, {MAP_W-5, MAP_H-6}, {MAP_W-6, MAP_H-5}
    };

    for (auto [wx,wy] : wallSpots) {
        setTile(wx, wy, new Box(&specialBoxTex, TILE));
        tiles[wy][wx]->setPosition(sf::Vector2f(wx * TILE, wy * TILE));
    }
}

void Map::placePushableBoxes(const sf::Texture& pushableTex) {
    std::vector<std::pair<int,int>> pushableSpots = {
        {3,3}, {7,4}, {10,6}, {12,8}, {MAP_W/2-3, MAP_H/2-1},
        {MAP_W/2+3, MAP_H/2+1}, {8, MAP_H-5}, {MAP_W-8, 5}
    };

    for (auto [bx,by] : pushableSpots) {
        setTile(bx, by, new PushableBox(&pushableBoxTex, TILE));
        tiles[by][bx]->setPosition(sf::Vector2f(bx * TILE, by * TILE));
    }
}

void Map::placePortals(const sf::Texture* portalTex) {
    std::vector<std::pair<int,int>> portalSpots = {
        {MAP_W/2+4, MAP_H/2+3},
        {4, MAP_H-4},
        {MAP_W-5, 4},
        {MAP_W/2, 3}
    };

    for (auto [px,py] : portalSpots) {
        setTile(px, py, new Portal(portalTex, TILE));
        tiles[py][px]->setPosition(sf::Vector2f(px * TILE, py * TILE));
    }
}
