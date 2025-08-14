#include <SFML/Graphics.hpp>
#include <array>
#include <optional>

int main() {


    // --- Map constants ---
    constexpr int MAP_W = 26;
    constexpr int MAP_H = 20;
    constexpr float TILE = 37.f; // tile size in pixels
    const unsigned winW = static_cast<unsigned>(MAP_W * TILE);    
    const unsigned winH = static_cast<unsigned>(MAP_H * TILE);

    sf::RenderWindow window(sf::VideoMode({winW, winH}), "Sokuban dual!");
    window.setFramerateLimit(10);

    // --- Allocate raw 2D array for tiles ---
    sf::RectangleShape** tiles = new sf::RectangleShape*[MAP_H];
    for (int y = 0; y < MAP_H; ++y) {
        tiles[y] = new sf::RectangleShape[MAP_W];
    }

    // --- Initialize map (checkerboard) ---
    for (int y = 0; y < MAP_H; ++y) {
        for (int x = 0; x < MAP_W; ++x) {
            tiles[y][x].setSize(sf::Vector2f(TILE - 1.f, TILE - 1.f)); // gap to show grid
            tiles[y][x].setPosition(sf::Vector2f(x * TILE, y * TILE));
            bool dark = ((x + y) % 2) == 0;
            tiles[y][x].setFillColor(dark ? sf::Color(220, 226, 234) : sf::Color(240, 244, 248));
        }
    }

    // --- Player 1 setup (sprite from Assets/Player1.jpg, WASD) ---
    // Construct textures directly from filenames (SFML 3 style)
    const sf::Texture player1Tex("Assets/Player1.jpg");
    sf::Sprite player1(player1Tex);
    // scale sprite to desired tile size (tile - 4 px to match previous rectangle size)
    float desiredSize = TILE - 4.f;
    auto t1sz = player1Tex.getSize();
    if (t1sz.x > 0 && t1sz.y > 0) {
        player1.setScale(sf::Vector2<float>(desiredSize / static_cast<float>(t1sz.x),
                                            desiredSize / static_cast<float>(t1sz.y)));
    }

    int p1X = MAP_W / 4;
    int p1Y = MAP_H / 2;
    player1.setPosition(sf::Vector2<float>(p1X * TILE + 2.f, p1Y * TILE + 2.f)); // <-- explicit sf::Vector2<float>

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
    player2.setPosition(sf::Vector2<float>(p2X * TILE + 2.f, p2Y * TILE + 2.f)); // <-- explicit sf::Vector2<float>

    // --- Player grid coordinates ---
    int playerX = MAP_W / 2;
    int playerY = MAP_H / 2;


    while (window.isOpen()) {
        // Event loop (SFML 3 polling returns optional-like object)
        while (auto ev = window.pollEvent()) {
            if (ev->is<sf::Event::Closed>()) {
                window.close();
            }

            if (auto keyEv = ev->getIf<sf::Event::KeyPressed>()) {
                switch (keyEv->scancode) {
                    // Player 1 (WASD)
                    case sf::Keyboard::Scan::W:
                        if (p1Y > 0) --p1Y;
                        break;
                    case sf::Keyboard::Scan::S:
                        if (p1Y < MAP_H - 1) ++p1Y;
                        break;
                    case sf::Keyboard::Scan::A:
                        if (p1X > 0) --p1X;
                        break;
                    case sf::Keyboard::Scan::D:
                        if (p1X < MAP_W - 1) ++p1X;
                        break;

                    // Player 2 (Arrow keys)
                    case sf::Keyboard::Scan::Up:
                        if (p2Y > 0) --p2Y;
                        break;
                    case sf::Keyboard::Scan::Down:
                        if (p2Y < MAP_H - 1) ++p2Y;
                        break;
                    case sf::Keyboard::Scan::Left:
                        if (p2X > 0) --p2X;
                        break;
                    case sf::Keyboard::Scan::Right:
                        if (p2X < MAP_W - 1) ++p2X;
                        break;

                    default:
                        break;
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
                window.draw(tiles[y][x]);
            }
        }
        window.draw(player1);
        window.draw(player2);
        window.display();
    }

    // --- Free memory ---
    for (int y = 0; y < MAP_H; ++y) {
        delete[] tiles[y];
    }
    delete[] tiles;

    return 0;
}
