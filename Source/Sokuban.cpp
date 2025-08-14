#include <SFML/Graphics.hpp>
#include <array>
#include <optional>
#include <iostream>

// --- GameObject: encapsulated, drawable wrapper for either a rectangle or a textured sprite ---
class GameObject : public sf::Drawable {
public:
    GameObject() : isPenetrate(false), hasTexture(false) {}

    virtual ~GameObject() = default;

    // Keep signatures compatible with your usage (sf::Vector2<float> alias)
    virtual void setSize(const sf::Vector2<float>& size) {
        shape.setSize(size);
    }

    virtual void setPosition(const sf::Vector2<float>& pos) {
        shape.setPosition(pos);
        if (hasTexture) sprite.setPosition(pos);
    }

    virtual void setFillColor(const sf::Color& color) {
        shape.setFillColor(color);
    }

    // public attribute requested by you (true => cannot be walked through)
    bool isPenetrate;

protected:
    sf::RectangleShape shape;
    sf::Sprite sprite;
    bool hasTexture;

    // draw either sprite (when textured) or the rectangle shape
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        if (hasTexture) target.draw(sprite, states);
        else target.draw(shape, states);
    }
};

// --- Box: a blocking tile with an optional texture ---
class Box : public GameObject {
public:
    // tex pointer must outlive this Box (we keep textures in main)
    Box(const sf::Texture* tex = nullptr, float desiredSize = 0.f) {
        isPenetrate = true; // box blocks movement as requested

        if (tex) {
            hasTexture = true;
            sprite.setTexture(*tex);

            // if desired pixel size provided, scale sprite to fit
            if (desiredSize > 0.f) {
                auto tsz = tex->getSize();
                if (tsz.x > 0 && tsz.y > 0) {
                    sprite.setScale(sf::Vector2<float>(desiredSize / static_cast<float>(tsz.x),
                                                       desiredSize / static_cast<float>(tsz.y)));
                }
                // keep rectangle shape size in sync (used when no texture or for bounds)
                shape.setSize(sf::Vector2<float>(desiredSize, desiredSize));
            }
        }
    }

    // override setSize so shape stays consistent; sprite scaling already set in ctor
    void setSize(const sf::Vector2<float>& size) override {
        shape.setSize(size);
        // If sprite is textured, keep sprite scale as-is (constructed above),
        // but in case user changes size later, we could re-scale using texture size.
    }

    // setPosition inherited from base will position both shape and sprite correctly
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

    // --- Load box texture (kept alive in main) ---
    const sf::Texture boxTex("Assets/SpecialBox.jpg"); // ensure this file exists in Assets

    // --- Allocate raw 2D array for tiles (now pointers to GameObject so we can store Box) ---
    GameObject*** tiles = new GameObject**[MAP_H];
    for (int y = 0; y < MAP_H; ++y) {
        tiles[y] = new GameObject*[MAP_W];
    }

    // --- Initialize map (checkerboard) with plain GameObject instances ---
    for (int y = 0; y < MAP_H; ++y) {
        for (int x = 0; x < MAP_W; ++x) {
            tiles[y][x] = new GameObject();
            tiles[y][x]->setSize(sf::Vector2<float>(TILE - 1.f, TILE - 1.f)); // gap to show grid
            tiles[y][x]->setPosition(sf::Vector2<float>(x * TILE, y * TILE));
            bool dark = ((x + y) % 2) == 0;
            tiles[y][x]->setFillColor(dark ? sf::Color(220, 226, 234) : sf::Color(240, 244, 248));
            tiles[y][x]->isPenetrate = false; // default: walkable
        }
    }

    // --- Replace a single tile with a Box (example: center tile) ---
    const int boxX = MAP_W / 2;
    const int boxY = MAP_H / 2;

    // delete previous GameObject and put a Box there
    delete tiles[boxY][boxX];
    tiles[boxY][boxX] = new Box(&boxTex, TILE - 4.f); // Box will block walking
    tiles[boxY][boxX]->setPosition(sf::Vector2<float>(boxX * TILE, boxY * TILE));
    // Optionally set a shape fill if texture not visible:
    // tiles[boxY][boxX]->setFillColor(sf::Color::Red);

    // --- Player 1 setup (sprite from Assets/Player1.jpg, WASD) ---
    const sf::Texture player1Tex("Assets/Player1.jpg");
    sf::Sprite player1(player1Tex);
    float desiredSize = TILE - 4.f;
    auto t1sz = player1Tex.getSize();
    if (t1sz.x > 0 && t1sz.y > 0) {
        player1.setScale(sf::Vector2<float>(desiredSize / static_cast<float>(t1sz.x),
                                            desiredSize / static_cast<float>(t1sz.y)));
    }

    int p1X = MAP_W / 4;
    int p1Y = MAP_H / 2;
    player1.setPosition(sf::Vector2<float>(p1X * TILE + 2.f, p1Y * TILE + 2.f)); // explicit sf::Vector2<float>

    // --- Player 2 setup (sprite from Assets/Player2.jpg, Arrow keys) ---
    const sf::Texture player2Tex("Assets/Player2.jpg");
    sf::Sprite player2(player2Tex);
    auto t2sz = player2Tex.getSize();
    if (t2sz.x > 0 && t2sz.y > 0) {
        player2.setScale(sf::Vector2<float>(desiredSize / static_cast<float>(t2sz.x),
                                            desiredSize / static_cast<float>(t2sz.y)));
    }

    int p2X = (MAP_W * 3) / 4;
    int p2Y = MAP_H / 2;
    player2.setPosition(sf::Vector2<float>(p2X * TILE + 2.f, p2Y * TILE + 2.f)); // explicit sf::Vector2<float>

    // --- Game loop ---
    while (window.isOpen()) {
        // Event loop (SFML 3 polling returns optional-like object)
        while (auto ev = window.pollEvent()) {
            if (ev->is<sf::Event::Closed>()) {
                window.close();
            }

            if (auto keyEv = ev->getIf<sf::Event::KeyPressed>()) {
                // compute tentative new positions then check tile->isPenetrate
                int newP1X = p1X, newP1Y = p1Y;
                int newP2X = p2X, newP2Y = p2Y;

                switch (keyEv->scancode) {
                    // Player 1 (WASD)
                    case sf::Keyboard::Scan::W:
                        if (p1Y > 0) --newP1Y;
                        break;
                    case sf::Keyboard::Scan::S:
                        if (p1Y < MAP_H - 1) ++newP1Y;
                        break;
                    case sf::Keyboard::Scan::A:
                        if (p1X > 0) --newP1X;
                        break;
                    case sf::Keyboard::Scan::D:
                        if (p1X < MAP_W - 1) ++newP1X;
                        break;

                    // Player 2 (Arrow keys)
                    case sf::Keyboard::Scan::Up:
                        if (p2Y > 0) --newP2Y;
                        break;
                    case sf::Keyboard::Scan::Down:
                        if (p2Y < MAP_H - 1) ++newP2Y;
                        break;
                    case sf::Keyboard::Scan::Left:
                        if (p2X > 0) --newP2X;
                        break;
                    case sf::Keyboard::Scan::Right:
                        if (p2X < MAP_W - 1) ++newP2X;
                        break;

                    default:
                        break;
                }

                // check Box / blocking for Player 1
                if (newP1X != p1X || newP1Y != p1Y) {
                    // ensure indices valid (they should be because we limited earlier)
                    if (!tiles[newP1Y][newP1X]->isPenetrate) {
                        p1X = newP1X;
                        p1Y = newP1Y;
                    } else {
                        // tile blocked; optionally play bump sound or debug
                    }
                }

                // check Box / blocking for Player 2
                if (newP2X != p2X || newP2Y != p2Y) {
                    if (!tiles[newP2Y][newP2X]->isPenetrate) {
                        p2X = newP2X;
                        p2Y = newP2Y;
                    }
                }

                // update pixel positions using sf::Vector2<float>
                player1.setPosition(sf::Vector2<float>(p1X * TILE + 2.f, p1Y * TILE + 2.f));
                player2.setPosition(sf::Vector2<float>(p2X * TILE + 2.f, p2Y * TILE + 2.f));
            }
        }

        // Draw
        window.clear(sf::Color::Black);
        for (int y = 0; y < MAP_H; ++y) {
            for (int x = 0; x < MAP_W; ++x) {
                // tiles[y][x] is a pointer to GameObject; dereference for drawing
                window.draw(*tiles[y][x]);
            }
        }
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
