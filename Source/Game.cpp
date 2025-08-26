#include "Game.hpp"
#include <iostream>
#include <optional>

Game::Game(sf::RenderWindow& win, TextureManager& tex, int mapW, int mapH, float tileSize)
    : window(win)
    , m_tex(tex)
    , m_map(mapW, mapH, tileSize, tex)
    , p1X(mapW/4), p1Y(mapH/2)
    , p2X((mapW*3)/4), p2Y(mapH/2)
    , m_tile(tileSize)
    , player1Score(0), player2Score(0)
    , fontRef(m_tex.font)
    , gameOver(false)
{
    window.setFramerateLimit(10);
    window.setKeyRepeatEnabled(false);

    // --- construct player sprites now that textures are available ---
    player1.emplace(m_tex.player1Tex);
    player2.emplace(m_tex.player2Tex);

    float desiredSize = m_tile - 4.f;
    auto t1sz = m_tex.player1Tex.getSize();
    if (t1sz.x > 0 && t1sz.y > 0) {
        player1->setScale(sf::Vector2f(desiredSize / static_cast<float>(t1sz.x),
                                       desiredSize / static_cast<float>(t1sz.y)));
    }
    auto t2sz = m_tex.player2Tex.getSize();
    if (t2sz.x > 0 && t2sz.y > 0) {
        player2->setScale(sf::Vector2f(desiredSize / static_cast<float>(t2sz.x),
                                       desiredSize / static_cast<float>(t2sz.y)));
    }
    player1->setPosition(sf::Vector2f(p1X * m_tile + 2.f, p1Y * m_tile + 2.f));
    player2->setPosition(sf::Vector2f(p2X * m_tile + 2.f, p2Y * m_tile + 2.f));

    // --- UI texts (sf::Text requires a font at construction in SFML3) ---
    player1ScoreText.emplace(fontRef);
    player1ScoreText->setCharacterSize(24);
    player1ScoreText->setFillColor(sf::Color::Blue);
    player1ScoreText->setPosition(sf::Vector2f(10,10));

    player2ScoreText.emplace(fontRef);
    player2ScoreText->setCharacterSize(24);
    player2ScoreText->setFillColor(sf::Color::Blue);
    player2ScoreText->setPosition(sf::Vector2f(m_map.width() * m_tile - m_tile * 4,10));

    timerText.emplace(fontRef);
    timerText->setCharacterSize(24);
    timerText->setFillColor(sf::Color::White);
    timerText->setPosition(sf::Vector2f(m_map.width() * m_tile / 2 - 40.f, 10));

    winnerText.emplace(fontRef);
    winnerText->setCharacterSize(48);
    winnerText->setFillColor(sf::Color::White);
    winnerText->setPosition(sf::Vector2f(m_map.width() * m_tile / 2 - 150.f, m_map.height() * m_tile / 2 - 40.f));

    gameClock.restart();
    boxSpawnClock.restart();
}

void Game::updateTimer() {
    if (gameOver) return;
    int elapsed = static_cast<int>(gameClock.getElapsedTime().asSeconds());
    int remaining = GAME_DURATION - elapsed;
    if (remaining < 0) remaining = 0;
    int minutes = remaining / 60;
    int seconds = remaining % 60;
    if (timerText) {
        timerText->setString(
            (minutes < 10 ? "0" : "") + std::to_string(minutes) + ":" +
            (seconds < 10 ? "0" : "") + std::to_string(seconds)
        );
    }
    if (remaining <= 0) {
        gameOver = true;
        if (winnerText) {
            if (player1Score > player2Score) winnerText->setString("Player 1 Wins!");
            else if (player2Score > player1Score) winnerText->setString("Player 2 Wins!");
            else winnerText->setString("Draw!");
        }
    }
}

void Game::tryMovePlayer(int &px, int &py, int dx, int dy, int otherX, int otherY, int playerNum) {
    if (dx == 0 && dy == 0) return;
    int newX = px + dx;
    int newY = py + dy;
    if (newX == otherX && newY == otherY) return;
    if (newX < 0 || newX >= m_map.width() || newY < 0 || newY >= m_map.height()) return;

    GameObject* target = m_map.tileAt(newX, newY);
    if (!target->blocks()) {
        px = newX;
        py = newY;
        return;
    }
    PushableBox* pb = dynamic_cast<PushableBox*>(target);
    if (pb) {
        int boxNewX = newX + dx;
        int boxNewY = newY + dy;
        if (boxNewX < 0 || boxNewX >= m_map.width() || boxNewY < 0 || boxNewY >= m_map.height()) return;
        if ((boxNewX == otherX && boxNewY == otherY) || (boxNewX == px && boxNewY == py)) return;
        GameObject* boxTarget = m_map.tileAt(boxNewX, boxNewY);
        if (boxTarget->isPortal()) {
            // award points and remove box
            bool moved = m_map.tryMoveBox(newX, newY, boxNewX, boxNewY); // will delete and replace floor
            if (moved) {
                if (playerNum == 1) player1Score += 10;
                else player2Score += 10;
                px = newX; py = newY;
                return;
            }
        }
        if (!boxTarget->blocks()) {
            bool moved = m_map.tryMoveBox(newX, newY, boxNewX, boxNewY);
            if (moved) {
                px = newX; py = newY;
            }
            return;
        }
    } else {
        return;
    }
}

void Game::updateInputAndMovement() {
    int p1_dx = 0, p1_dy = 0;
    int p2_dx = 0, p2_dy = 0;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::W)) p1_dy = -1;
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::S)) p1_dy = 1;
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::A)) p1_dx = -1;
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::D)) p1_dx = 1;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Up)) p2_dy = -1;
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Down)) p2_dy = 1;
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Left)) p2_dx = -1;
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Right)) p2_dx = 1;

    int p1_targetX = p1X + p1_dx, p1_targetY = p1Y + p1_dy;
    int p2_targetX = p2X + p2_dx, p2_targetY = p2Y + p2_dy;

    bool conflictSameTile = (p1_dx != 0 || p1_dy != 0) && (p2_dx != 0 || p2_dy != 0)
                            && (p1_targetX == p2_targetX && p1_targetY == p2_targetY);
    bool swapPositions = (p1_targetX == p2X && p1_targetY == p2Y) &&
                         (p2_targetX == p1X && p2_targetY == p1Y);

    if (!conflictSameTile && !swapPositions) {
        tryMovePlayer(p1X, p1Y, p1_dx, p1_dy, p2X, p2Y, 1);
        tryMovePlayer(p2X, p2Y, p2_dx, p2_dy, p1X, p1Y, 2);
    }
    // update sprite pixel positions
    if (player1) player1->setPosition(sf::Vector2f(p1X * m_tile + 2.f, p1Y * m_tile + 2.f));
    if (player2) player2->setPosition(sf::Vector2f(p2X * m_tile + 2.f, p2Y * m_tile + 2.f));

    // Update score text
    if (player1ScoreText) player1ScoreText->setString("Player 1: " + std::to_string(player1Score));
    if (player2ScoreText) player2ScoreText->setString("Player 2: " + std::to_string(player2Score));
}

void Game::drawAll() {
    window.clear(sf::Color::Black);
    m_map.draw(window);
    if (player1) window.draw(*player1);
    if (player2) window.draw(*player2);
    if (player1ScoreText) window.draw(*player1ScoreText);
    if (player2ScoreText) window.draw(*player2ScoreText);
    if (timerText) window.draw(*timerText);
    if (gameOver && winnerText) window.draw(*winnerText);
    window.display();
}

void Game::run() {
    gameClock.restart();
    boxSpawnClock.restart();
    gameOver = false;

    while (window.isOpen() && !gameOver) {
        // SFML 3.0 event polling: pollEvent() returns std::optional<sf::Event>
        while (auto ev = window.pollEvent()) {
            if (ev->is<sf::Event::Closed>()) {
                window.close();
                return;
            }
        }

        updateTimer();

        // spawn boxes every 2 seconds
        if (!gameOver && boxSpawnClock.getElapsedTime().asSeconds() >= 2.f) {
            boxSpawnClock.restart();
            m_map.spawnRandomBox();
        }

        updateInputAndMovement();
        drawAll();
    }

    // final draw to show winner if round ended naturally
    if (window.isOpen()) {
        drawAll();
        // brief pause so user sees winner (but we won't block for too long)
        sf::sleep(sf::milliseconds(600));
    }
}
