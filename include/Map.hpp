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
    Map(int width, int height, float tileSize);
    ~Map();

    void draw(sf::RenderWindow& window);
    GameObject* getTile(int x, int y);
    void setTile(int x, int y, GameObject* obj);

    int getWidth() const { return MAP_W; }
    int getHeight() const { return MAP_H; }
    float getTileSize() const { return TILE; }

    // Helpers for spawning objects
    void placeWalls(const sf::Texture& wallTex);
    void placePushableBoxes(const sf::Texture& pushableTex);
    void placePortals(const sf::Texture* portalTex);

    Floor* makeFloorAt(int x, int y);

private:
    int MAP_W, MAP_H;
    float TILE;

    // Textures that the map owns
    sf::Texture floorTexture;
    sf::Texture specialBoxTex;
    sf::Texture pushableBoxTex;
    sf::Texture portalTex;

    GameObject*** tiles;
};
