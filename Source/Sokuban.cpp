// Sokoban.cpp
#include <SFML/Graphics.hpp>
#include <array>
#include <optional>
#include <iostream>
#include <memory>

// --- GameObject: encapsulated, drawable wrapper for either a rectangle or a textured sprite ---
class GameObject : public sf::Drawable {
public:
    GameObject() : isPenetrate(false) {}

    virtual ~GameObject() = default;

    // Keep signatures compatible with your usage (sf::Vector2<float> alias)
    virtual void setSize(const sf::Vector2f& size) {
        shape.setSize(size);
    }

    virtual void setPosition(const sf::Vector2f& pos) {
        shape.setPosition(pos);
        if (sprite) sprite->setPosition(pos);
    }

    virtual void setFillColor(const sf::Color& color) {
        shape.setFillColor(color);
    }

    // true => cannot be walked through
    bool isPenetrate;

    // By default not pushable
    virtual bool isPushable() const { return false; }

protected:
    sf::RectangleShape shape;
    std::optional<sf::Sprite> sprite; // sf::Sprite requires a texture at construction

    // draw either sprite (when textured) or the rectangle shape
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        if (sprite) target.draw(*sprite, states);
        else target.draw(shape, states);
    }
};

// --- Box: a blocking tile with an optional texture (immovable) ---
class Box : public GameObject {
public:
    // tex pointer must outlive this Box (we keep textures in main)
    Box(const sf::Texture* tex = nullptr, float desiredSize = 0.f) {
        isPenetrate = true; // immovable blocking tile

        if (tex) {
            sprite.emplace(*tex);
            if (desiredSize > 0.f) {
                auto tsz = tex->getSize();
                if (tsz.x > 0 && tsz.y > 0) {
                    sprite->setScale(sf::Vector2f(desiredSize / static_cast<float>(tsz.x),
                                                  desiredSize / static_cast<float>(tsz.y)));
                }
                shape.setSize(sf::Vector2f(desiredSize, desiredSize));
            }
        }
    }

    void setSize(const sf::Vector2f& size) override {
        shape.setSize(size);
    }
};

// --- PushableBox: a box that players can push one tile at a time ---
class PushableBox : public GameObject {
public:
    PushableBox(const sf::Texture* tex = nullptr, float desiredSize = 0.f) {
        isPenetrate = true; // blocks walking unless pushed
        if (tex) {
            sprite.emplace(*tex);
            if (desiredSize > 0.f) {
                auto tsz = tex->getSize();
                if (tsz.x > 0 && tsz.y > 0) {
                    sprite->setScale(sf::Vector2f(desiredSize / static_cast<float>(tsz.x),
                                                  desiredSize / static_cast<float>(tsz.y)));
                }
                shape.setSize(sf::Vector2f(desiredSize, desiredSize));
            }
        }
    }

    bool isPushable() const override { return true; }

    void setSize(const sf::Vector2f& size) override {
        shape.setSize(size);
    }
};

int main() {

    // --- Map constants ---
    constexpr int MAP_W = 26;
    constexpr int MAP_H = 20;
    constexpr float TILE = 37.f; // tile size in pixels
    const unsigned winW = static_cast<unsigned>(MAP_W * TILE);
    const unsigned winH = static_cast<unsigned>(MAP_H * TILE);

    sf::RenderWindow window(sf::VideoMode({winW, winH}), "Sokuban dual!");
    window.setFramerateLimit(10);
    window.setKeyRepeatEnabled(false); // disable OS key repeat so event repeats don't interfere (still using polling below)

    // --- Load textures (kept alive in main) ---
    // immovable special box texture
    sf::Texture specialBoxTex;
    if (!specialBoxTex.loadFromFile("Assets/SpecialBox.jpg")) {
        std::cerr << "Failed to load Assets/SpecialBox.jpg\n";
        return 1;
    }

    // pushable box texture (user requested box.jpg in Assets/)
    sf::Texture pushableBoxTex;
    if (!pushableBoxTex.loadFromFile("Assets/box.jpg")) {
        std::cerr << "Failed to load Assets/box.jpg\n";
        return 1;
    }

    // player textures
    sf::Texture player1Tex;
    if (!player1Tex.loadFromFile("Assets/Player1.jpg")) {
        std::cerr << "Failed to load Assets/Player1.jpg\n";
        return 1;
    }
    sf::Texture player2Tex;
    if (!player2Tex.loadFromFile("Assets/Player2.jpg")) {
        std::cerr << "Failed to load Assets/Player2.jpg\n";
        return 1;
    }

    // --- Allocate raw 2D array for tiles (pointers to GameObject so we can store different derived objects) ---
    GameObject*** tiles = new GameObject**[MAP_H];
    for (int y = 0; y < MAP_H; ++y) {
        tiles[y] = new GameObject*[MAP_W];
    }

    // Helper: set floor tile properties (size, position, color)
    auto makeFloorAt = [&](int x, int y) -> GameObject* {
        GameObject* g = new GameObject();
        g->setSize(sf::Vector2f(TILE - 1.f, TILE - 1.f));
        g->setPosition(sf::Vector2f(x * TILE, y * TILE));
        bool dark = ((x + y) % 2) == 0;
        g->setFillColor(dark ? sf::Color(220, 226, 234) : sf::Color(240, 244, 248));
        g->isPenetrate = false; // walkable
        return g;
    };

    // --- Initialize map (checkerboard) with plain GameObject floor instances ---
    for (int y = 0; y < MAP_H; ++y) {
        for (int x = 0; x < MAP_W; ++x) {
            tiles[y][x] = makeFloorAt(x, y);
        }
    }

    // --- Place an immovable special box at center (example) ---
    const int centerX = MAP_W / 2;
    const int centerY = MAP_H / 2;
    delete tiles[centerY][centerX];
    tiles[centerY][centerX] = new Box(&specialBoxTex, TILE - 4.f);
    tiles[centerY][centerX]->setPosition(sf::Vector2f(centerX * TILE, centerY * TILE));

    // --- Place a couple of pushable boxes (player can push these) ---
    const int pb1X = centerX + 1;
    const int pb1Y = centerY;
    delete tiles[pb1Y][pb1X];
    tiles[pb1Y][pb1X] = new PushableBox(&pushableBoxTex, TILE - 4.f);
    tiles[pb1Y][pb1X]->setPosition(sf::Vector2f(pb1X * TILE, pb1Y * TILE));

    const int pb2X = centerX - 2;
    const int pb2Y = centerY;
    delete tiles[pb2Y][pb2X];
    tiles[pb2Y][pb2X] = new PushableBox(&pushableBoxTex, TILE - 4.f);
    tiles[pb2Y][pb2X]->setPosition(sf::Vector2f(pb2X * TILE, pb2Y * TILE));

    // --- Player 1 setup (sprite from Assets/Player1.jpg, WASD) ---
    sf::Sprite player1(player1Tex);
    float desiredSize = TILE - 4.f;
    auto t1sz = player1Tex.getSize();
    if (t1sz.x > 0 && t1sz.y > 0) {
        player1.setScale(sf::Vector2f(desiredSize / static_cast<float>(t1sz.x),
                                     desiredSize / static_cast<float>(t1sz.y)));
    }

    int p1X = MAP_W / 4;
    int p1Y = MAP_H / 2;
    player1.setPosition(sf::Vector2f(p1X * TILE + 2.f, p1Y * TILE + 2.f));

    // --- Player 2 setup (sprite from Assets/Player2.jpg, Arrow keys) ---
    sf::Sprite player2(player2Tex);
    auto t2sz = player2Tex.getSize();
    if (t2sz.x > 0 && t2sz.y > 0) {
        player2.setScale(sf::Vector2f(desiredSize / static_cast<float>(t2sz.x),
                                     desiredSize / static_cast<float>(t2sz.y)));
    }

    int p2X = (MAP_W * 3) / 4;
    int p2Y = MAP_H / 2;
    player2.setPosition(sf::Vector2f(p2X * TILE + 2.f, p2Y * TILE + 2.f));

    // --- Game loop ---
    while (window.isOpen()) {
    // Event loop: only use events for window/system events now
    while (auto ev = window.pollEvent()) {
        if (ev->is<sf::Event::Closed>()) {
            window.close();
        }
        // remove per-key movement handling from here
    }

    // ---------- Realtime (polled) input handling ----------
    // compute each player's desired direction (dx,dy) based on keys held this frame
    int p1_dx = 0, p1_dy = 0;
    int p2_dx = 0, p2_dy = 0;

    // Player 1 (WASD) - using scancodes to match your event usage
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::W)) p1_dy = -1;
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::S)) p1_dy = 1;
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::A)) p1_dx = -1;
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::D)) p1_dx = 1;

    // Player 2 (Arrow keys)
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Up))    p2_dy = -1;
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Down))  p2_dy = 1;
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Left))  p2_dx = -1;
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Right)) p2_dx = 1;

    // helper used already in your code (keeps push logic centralized)
    auto try_move_player = [&](int &px, int &py, int dx, int dy, int otherX, int otherY) {
        if (dx == 0 && dy == 0) return; // no movement intended
        int newX = px + dx;
        int newY = py + dy;

        // prevent moving onto the other player's *current* position
        if (newX == otherX && newY == otherY) return;

        // bounds check
        if (newX < 0 || newX >= MAP_W || newY < 0 || newY >= MAP_H) return;

        GameObject* target = tiles[newY][newX];

        if (!target->isPenetrate) {
            px = newX;
            py = newY;
            return;
        }

        PushableBox* pb = dynamic_cast<PushableBox*>(target);
        if (pb) {
            int boxNewX = newX + dx;
            int boxNewY = newY + dy;
            if (boxNewX < 0 || boxNewX >= MAP_W || boxNewY < 0 || boxNewY >= MAP_H) return;
            if ((boxNewX == otherX && boxNewY == otherY) || (boxNewX == px && boxNewY == py)) return;
            GameObject* boxTarget = tiles[boxNewY][boxNewX];
            if (!boxTarget->isPenetrate) {
                delete boxTarget;
                tiles[boxNewY][boxNewX] = tiles[newY][newX];
                tiles[boxNewY][boxNewX]->setPosition(sf::Vector2f(boxNewX * TILE, boxNewY * TILE));
                tiles[newY][newX] = makeFloorAt(newX, newY);
                px = newX;
                py = newY;
            } else {
                return;
            }
        } else {
            return;
        }
    };

    // Simple simultaneous-move resolution:
    // - compute intended destinations and avoid allowing both players to move into the same tile
    int p1_targetX = p1X + p1_dx, p1_targetY = p1Y + p1_dy;
    int p2_targetX = p2X + p2_dx, p2_targetY = p2Y + p2_dy;

    // If both intend to move into same tile, cancel both moves (could also pick priority)
    bool conflictSameTile = (p1_dx != 0 || p1_dy != 0) && (p2_dx != 0 || p2_dy != 0)
                            && (p1_targetX == p2_targetX && p1_targetY == p2_targetY);

    // If they intend to swap positions (p1 -> p2 current and p2 -> p1 current) cancel both
    bool swapPositions = (p1_targetX == p2X && p1_targetY == p2Y) &&
                         (p2_targetX == p1X && p2_targetY == p1Y);

    if (!conflictSameTile && !swapPositions) {
        // apply both moves (order here matters if boxes involved; you might prefer atomic resolution)
        try_move_player(p1X, p1Y, p1_dx, p1_dy, p2X, p2Y);
        try_move_player(p2X, p2Y, p2_dx, p2_dy, p1X, p1Y);
    } else {
        // if conflict, no moves this frame; alternative: prioritize one player
    }

    // update sprite pixel positions
    player1.setPosition(sf::Vector2f(p1X * TILE + 2.f, p1Y * TILE + 2.f));
    player2.setPosition(sf::Vector2f(p2X * TILE + 2.f, p2Y * TILE + 2.f));

    // ---------- Drawing ----------
    window.clear(sf::Color::Black);
    for (int y = 0; y < MAP_H; ++y) for (int x = 0; x < MAP_W; ++x) window.draw(*tiles[y][x]);
    window.draw(player1);
    window.draw(player2);
    window.display();
}

    // --- Free memory ---
    for (int y = 0; y < MAP_H; ++y) {
        for (int x = 0; x < MAP_W; ++x) {
            delete tiles[y][x];
        }
        delete[] tiles[y];
    }
    delete[] tiles;

    return 0;
}
