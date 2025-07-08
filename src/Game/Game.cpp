/*
** EPITECH PROJECT, 2025
** B-NWP-400-LIL-4-1-jetpack-nicolas.dumetz
** File description:
** Game
*/

#include "Game/Game.hpp"
#include "Utils/Utils.hpp"
#include <cmath>

Jetpack::Game::Game(std::shared_ptr<Jetpack::Client> client)
    : _client(client),
      _window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Jetpack Client", sf::Style::Titlebar | sf::Style::Close),
      _sharedState(client->getSharedState())
{
    _window.setFramerateLimit(60);
    initGraphics();
}

Jetpack::Game::~Game() {}

void Jetpack::Game::initGraphics()
{
    float baseScale;

    if (!_font.loadFromFile("assets/font.ttf"))
        throw GameError("Failed to load font");
    if (!_mapTexture.loadFromFile("assets/background.png"))
        throw GameError("Failed to load map texture");
    baseScale = WINDOW_HEIGHT / static_cast<float>(_mapTexture.getSize().y);
    _mapSprite.setTexture(_mapTexture);
    _mapSprite.setScale(baseScale * BACKGROUND_ZOOM, baseScale * BACKGROUND_ZOOM);
    if (!_playerSpriteSheet.loadFromFile("assets/player.png"))
        throw GameError("Failed to load player sprite sheet");
    for (int i = 0; i < NUMBER_CLIENTS; ++i) {
        sf::Sprite sprite;
        sprite.setTexture(_playerSpriteSheet);
        sprite.setTextureRect({0, 0, PLAYER_SPRITE_WIDTH, PLAYER_SPRITE_HEIGHT});
        sprite.setScale(PLAYER_SCALE, PLAYER_SCALE);
        sprite.setOrigin(PLAYER_SPRITE_WIDTH * PLAYER_ORIGIN_X, PLAYER_SPRITE_HEIGHT * PLAYER_ORIGIN_Y);
        _playerSprites.push_back(sprite);
    }
    _animationClock.restart();
}

void Jetpack::Game::run()
{
    sf::Event event;
    sf::Clock clock;
    float deltaTime;

    while (_window.isOpen() && _client->getState() == Jetpack::ClientState::Connected) {
        while (_window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                _window.close();
                throw GameError("Window closed by user");
            }
        }
        deltaTime = clock.restart().asSeconds();
        updateMapScroll(deltaTime);
        updateAnimation();
        if (_window.hasFocus() && sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
            _client->sendJump();
        updatePlayerPositions();
        _window.clear();
        drawBackground();
        drawGrid();
        renderPlayers();
        _window.display();
    }
}

void Jetpack::Game::waitingRoom()
{
    sf::Text text("Waiting for players...", _font, 60);
    text.setFillColor(sf::Color::White);
    sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
    text.setPosition(WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f);

    while (_window.isOpen() && _client->getState() == Jetpack::ClientState::Waiting) {
        sf::Event event;
        while (_window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                _window.close();
        }
        _window.clear();
        _window.draw(text);
        _window.display();
    }
}

void Jetpack::Game::updateMapScroll(float dt)
{
    _scrollOffset += SCROLL_SPEED * TILE_SIZE * dt;
}

void Jetpack::Game::updateAnimation()
{
    if (_animationClock.getElapsedTime().asSeconds() < _frameTime)
        return;
    _currentFrame = (_currentFrame + 1) % 4;
    for (std::size_t i = 0; i < _playerSprites.size(); ++i) {
        auto state = _sharedState->getPlayerState(i);
        int row = getPlayerAnimationRow(i, state.getY());
        _playerSprites[i].setTextureRect(sf::IntRect(_currentFrame * PLAYER_SPRITE_WIDTH, row * PLAYER_SPRITE_HEIGHT, PLAYER_SPRITE_WIDTH, PLAYER_SPRITE_HEIGHT));
    }

    _animationClock.restart();
}

int Jetpack::Game::getPlayerAnimationRow(int id, float y) const
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && _client->getPlayerId() == id)
        return 1;
    if (y < TILE_ROWS - 2)
        return 1;
    return 0;
}

void Jetpack::Game::updatePlayerPositions()
{
    float tileSize = TILE_SIZE;
    float fixedX = 4 * tileSize;

    for (std::size_t i = 0; i < _playerSprites.size(); i++) {
        auto state = _sharedState->getPlayerState(i);
        if (!state.isAlive())
            continue;
        _playerSprites[i].setOrigin(0.f, 0.f);
        float y = state.getY();
        if (y < 0.1f) y = 0.f;
        y = std::clamp(y, 0.f, static_cast<float>(TILE_ROWS - 1));
        float spriteH = PLAYER_SPRITE_HEIGHT * PLAYER_SCALE;
        float spriteW = PLAYER_SPRITE_WIDTH * PLAYER_SCALE;
        float spriteX = fixedX + tileSize / 2.f - spriteW / 2.f;
        float spriteY = TILE_MARGIN + (y + 1.0f) * tileSize - spriteH;
        _playerSprites[i].setPosition(spriteX, spriteY);
    }
}

void Jetpack::Game::renderPlayers()
{
    if (_sharedState->getPlayers().size() <= 1)
        return;
    for (std::size_t i = 0; i < _playerSprites.size(); ++i) {
        auto state = _sharedState->getPlayerState(i);
        if (!state.isAlive()) continue;
        if (_client->getPlayerId() == i)
            _playerSprites[i].setColor(sf::Color::White);
        else
            _playerSprites[i].setColor(sf::Color(255, 255, 255, 128));
        _window.draw(_playerSprites[i]);
    }
}

void Jetpack::Game::drawGrid()
{
    float tileSize = TILE_SIZE;
    float gridHeight = tileSize * TILE_ROWS;
    float offsetX = -_scrollOffset + FIXED_PLAYER_X;
    const auto &map = _client->getMap();
    int cols = map.empty() ? 0 : static_cast<int>(map[0].size());

    for (int i = 0; i <= TILE_ROWS; ++i) {
        float y = std::round(TILE_MARGIN + i * tileSize);
        sf::RectangleShape line(sf::Vector2f(WINDOW_WIDTH, 1));
        line.setPosition(0.f, y);
        line.setFillColor(sf::Color::White);
        _window.draw(line);
    }
    for (int i = 0; i <= cols; ++i) {
        float x = std::round(i * tileSize + offsetX);
        sf::RectangleShape line(sf::Vector2f(1, gridHeight));
        line.setPosition(x, TILE_MARGIN);
        line.setFillColor(sf::Color::White);
        _window.draw(line);
    }
}

void Jetpack::Game::drawBackground()
{
    float baseScale = WINDOW_HEIGHT / static_cast<float>(_mapTexture.getSize().y);
    float finalScale = baseScale * BACKGROUND_ZOOM;
    _mapSprite.setScale(finalScale, finalScale);
    float mapHeight = _mapTexture.getSize().y * finalScale;
    float y = (WINDOW_HEIGHT - mapHeight) / 2.0f;
    float x = -_scrollOffset;
    _mapSprite.setPosition(x, y);
    _window.draw(_mapSprite);
}
