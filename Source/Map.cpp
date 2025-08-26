#include "Map.hpp"
#include <iostream>
#include <vector>
#include <cstdlib> // For std::rand
#include <ctime>   // For std::time

// --- Constructor: Loads textures and initializes map with floor ---
Map::Map(int width, int height, float tileSize)
    : MAP_W(width), MAP_H(height), TILE(tileSize) {

    // Load all textures required for map objects
    if (!floorTexture.loadFromFile("Assets/floor.jpg"))
        std::cerr << "Failed to load Assets/floor.jpg\n";
    if (!specialBoxTex.loadFromFile("Assets/SpecialBox.jpg"))
        std::cerr << "Failed to load Assets/SpecialBox.jpg\n";
    if (!pushableBoxTex.loadFromFile("Assets/box.jpg"))
        std::cerr << "Failed to load Assets/box.jpg\n";
    if (!portalTex.loadFromFile("Assets/portal.jpg"))
        std::cerr << "Failed to load Assets/portal.jpg\n";

    // Allocate the 2D tile grid
    tiles = new GameObject**[MAP_H];
    for (int y = 0; y < MAP_H; ++y) {
        tiles[y] = new GameObject*[MAP_W];
    }

    // Initialize the entire map with floor instances
    for (int y = 0; y < MAP_H; ++y) {
        for (int x = 0; x < MAP_W; ++x) {
            tiles[y][x] = makeFloorAt(x, y);
        }
    }
}

// --- Destructor: Cleans up all dynamically allocated memory ---
Map::~Map() {
    for (int y = 0; y < MAP_H; ++y) {
        for (int x = 0; x < MAP_W; ++x) {
            delete tiles[y][x];
        }
        delete[] tiles[y];
    }
    delete[] tiles;
}

// --- Fills the map with initial walls, boxes, and portals ---
void Map::populateInitialState() {
    // Place immovable wall clusters (obstacles)
    std::vector<std::pair<int, int>> wallSpots = {
        {MAP_W / 2, MAP_H / 2},
        {MAP_W / 2 - 2, MAP_H / 2}, {MAP_W / 2 + 2, MAP_H / 2},
        {MAP_W / 2, MAP_H / 2 - 2}, {MAP_W / 2, MAP_H / 2 + 2},
        {5, 5}, {5, 6}, {6, 5},
        {MAP_W - 6, MAP_H - 6}, {MAP_W - 5, MAP_H - 6}, {MAP_W - 6, MAP_H - 5},
        {3, 8}, {8, 3}, {12, 12},
        {MAP_W - 8, 8}, {8, MAP_H - 8},
        {15, 5}, {5, 15}, {MAP_W - 15, MAP_H - 15},
        {10, 10}, {MAP_W - 10, 10}, {10, MAP_H - 10},
        {2, MAP_H - 2}, {MAP_W - 2, 2}
    };

    for (auto [wx, wy] : wallSpots) {
        GameObject* wall = new Box(&specialBoxTex, TILE - 2.f);
        wall->setPosition(sf::Vector2f(wx * TILE, wy * TILE));
        setTile(wx, wy, wall);
    }

    // Scatter initial pushable boxes
    std::vector<std::pair<int, int>> pushableSpots = {
        {3, 3}, {7, 4}, {10, 6}, {12, 8}, {MAP_W / 2 - 3, MAP_H / 2 - 1},
        {MAP_W / 2 + 3, MAP_H / 2 + 1}, {8, MAP_H - 5}, {MAP_W - 8, 5}
    };
    for (auto [bx, by] : pushableSpots) {
        GameObject* pBox = new PushableBox(&pushableBoxTex, TILE - 4.f);
        pBox->setPosition(sf::Vector2f(bx * TILE, by * TILE));
        setTile(bx, by, pBox);
    }

    // Place multiple portals
    std::vector<std::pair<int, int>> portalSpots = {
        {MAP_W / 2 + 4, MAP_H / 2 + 3},
        {4, MAP_H - 4},
        {MAP_W - 5, 4},
        {MAP_W / 2, 3}
    };
    for (auto [px, py] : portalSpots) {
        GameObject* portal = new Portal(portalTex.getSize().x > 0 ? &portalTex : nullptr, TILE);
        portal->setPosition(sf::Vector2f(px * TILE, py * TILE));
        setTile(px, py, portal);
    }
}

// --- Tries to spawn a new box at a random empty location ---
void Map::spawnRandomBox() {
    int randX = std::rand() % MAP_W;
    int randY = std::rand() % MAP_H;

    GameObject* target = tiles[randY][randX];

    // Only replace the tile if it's a basic floor (not a box, portal, or wall)
    if (dynamic_cast<Floor*>(target)) {
        GameObject* newBox = new PushableBox(&pushableBoxTex, TILE - 4.f);
        newBox->setPosition(sf::Vector2f(randX * TILE, randY * TILE));
        setTile(randX, randY, newBox);
    }
}


// --- Helper to create a floor tile ---
Floor* Map::makeFloorAt(int x, int y) {
    Floor* g = new Floor(&floorTexture, TILE);
    g->setSize(sf::Vector2f(TILE, TILE));
    g->setPosition(sf::Vector2f(x * TILE, y * TILE));
    return g;
}

// --- Draws the entire map grid to the window ---
void Map::draw(sf::RenderWindow& window) {
    for (int y = 0; y < MAP_H; ++y) {
        for (int x = 0; x < MAP_W; ++x) {
            window.draw(*tiles[y][x]);
        }
    }
}

// --- Accessors for getting and setting tiles ---
GameObject* Map::getTile(int x, int y) const {
    if (x >= 0 && x < MAP_W && y >= 0 && y < MAP_H) {
        return tiles[y][x];
    }
    return nullptr; // Out of bounds
}

void Map::setTile(int x, int y, GameObject* obj) {
    if (x >= 0 && x < MAP_W && y >= 0 && y < MAP_H) {
        delete tiles[y][x]; // Free the old object
        tiles[y][x] = obj;
    }
}

PushableBox* Map::createPushableBox(int x, int y) {
    PushableBox* newBox = new PushableBox(&pushableBoxTex, TILE - 4.f);
    newBox->setPosition(sf::Vector2f(x * TILE, y * TILE));
    return newBox;
}