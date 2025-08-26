#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "GameObject.hpp"
#include "Floor.hpp"
#include "Box.hpp"
#include "PushableBox.hpp"
#include "Portal.hpp"

class Map {
public:
    // --- Constructor & Destructor ---
    Map(int width, int height, float tileSize);
    ~Map();

    // --- Public Interface ---
    void draw(sf::RenderWindow& window);
    void populateInitialState();
    void spawnRandomBox();
    
    // --- Tile Accessors ---
    GameObject* getTile(int x, int y) const;
    void setTile(int x, int y, GameObject* obj);
    Floor* makeFloorAt(int x, int y);

    // --- Getters for map properties ---
    int getWidth() const { return MAP_W; }
    int getHeight() const { return MAP_H; }
    float getTileSize() const { return TILE; }
    PushableBox* createPushableBox(int x, int y);

private:
    // --- Map properties ---
    int MAP_W, MAP_H;
    float TILE;

    // --- Textures owned by the map ---
    sf::Texture floorTexture;
    sf::Texture specialBoxTex;      // For immovable walls
    sf::Texture pushableBoxTex;
    sf::Texture portalTex;

    // --- The game board ---
    GameObject*** tiles;
};