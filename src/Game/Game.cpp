/*
** EPITECH PROJECT, 2025
** B-NWP-400-LIL-4-1-jetpack-nicolas.dumetz
** File description:
** Game
*/

#include "Game/Game.hpp"
#include "PlayerState/PlayerState.hpp"
#include "Utils/Utils.hpp"
#include <SFML/Window/WindowStyle.hpp>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <iostream>

Jetpack::Game::Game(std::shared_ptr<Jetpack::Client> client)
    : _client(client),
      _window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Jetpack Client", sf::Style::Titlebar | sf::Style::Close),
      _sharedState(client->getSharedState())
{
    _window.setFramerateLimit(60);
    initGraphics();
    std::srand(std::time(nullptr));
}

Jetpack::Game::~Game() {}

void Jetpack::Game::initGraphics()
{
    float baseScale;

    if (!_font.loadFromFile("assets/font.ttf"))
        throw GameError("Failed to load font");
    if (!_mapTexture.loadFromFile("assets/background.png"))
        throw GameError("Failed to load map texture");
    if (!_coinTexture.loadFromFile("assets/coin.png"))
        throw GameError("Failed to load coin texture");
    if (!_zapperTexture.loadFromFile("assets/zapper.png"))
        throw GameError("Failed to load zapper texture");
    initObjectsFromMap();
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
        _playerAnimState.emplace_back();
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
        updatePlayerPositions();
        updateObjects(deltaTime);
        if (_window.hasFocus() && sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
            _client->sendJump();
        _window.clear();
        drawBackground();
        drawGrid();
        renderObjects();
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
    float time = _animationClock.getElapsedTime().asSeconds();
    bool changes = false;

    for (std::size_t i = 0; i < _playerSprites.size(); ++i) {
        this->_playerAnimState[i].clock += time;
        if (this->_playerAnimState[i].clock < _frameTime + this->_playerAnimState[i].slow
            || this->_playerAnimState[i]._state == state::NONE)
            continue;
        this->_playerAnimState[i].clock -= _frameTime + this->_playerAnimState[i].slow;
        this->_playerAnimState[i].frame = (this->_playerAnimState[i].frame + 1) % 4;
        auto& state = _sharedState->getPlayerState(i);
        int row = getPlayerAnimationRow(state, this->_playerAnimState[i]);
        _playerSprites[i].setTextureRect(sf::IntRect(this->_playerAnimState[i].frame * PLAYER_SPRITE_WIDTH, row * PLAYER_SPRITE_HEIGHT, PLAYER_SPRITE_WIDTH, PLAYER_SPRITE_HEIGHT));
        changes = true;
    }
    if (changes)
        _animationClock.restart();
}


int Jetpack::Game::getPlayerAnimationRow(Jetpack::PlayerState& playerState, animState& state)
{
    bool isOnGround = playerState.getY() >= TILE_ROWS - 2;

    if (state._state == state::SPIN || state._state == state::BURN || state._state == state::ELECTROCUTE) {
        if (state.frame != 0)
            return static_cast<int>(state._state);
        if (state.loop == 0) {
            state._state = state::NONE;
            return static_cast<int>(Game::state::NONE);
        }
        --state.loop;
        return static_cast<int>(state._state);
    }

    if (!playerState.isAlive() && isOnGround) {
        state.frame = 0;
        state._state = state::SPIN;
        return static_cast<int>(Game::state::SPIN);
    }

    if (!playerState.isAlive()) {
        state.frame = 0;
        state._state = (rand() % 2) ? Game::state::ELECTROCUTE : Game::state::BURN;
        return static_cast<int>(state._state);
    }

    if (state._state == state::FLY || state._state == state::WALK)
        state.slow = 0.f;

    if (!isOnGround) {
        state._state = state::FLY;
        return static_cast<int>(Game::state::FLY);
    }

    if (isOnGround && state._state == state::FLY) {
        state.frame = 0;
        state.slow = 0.15;
        state._state = Game::state::LAND;
        return static_cast<int>(state::LAND);
    }

    if (isOnGround && state._state == state::LAND && state.frame == 0) {
        state._state = state::WALK;
        return static_cast<int>(state::WALK);
    }

    if (isOnGround && state._state == state::LAND)
        return static_cast<int>(state::LAND);

    state._state = state::WALK;
    return static_cast<int>(state::WALK);
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
        if (y < 0.1f)
            y = 0.f;
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
        if (!state.isAlive() && this->_playerAnimState[i]._state == state::NONE) continue;
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

void Jetpack::Game::initObjectsFromMap()
{
    const auto& map = _client->getMap();
    float tileSize = TILE_SIZE;

    for (std::size_t y = 0; y < map.size(); y++) {
        for (std::size_t x = 0; x < map[y].size(); x++) {
            TileType tile = map[y][x];
            switch (tile) {
                case TileType::COIN:
                    _coins.emplace_back(_coinTexture, x, y, tileSize);
                    break;
                case TileType::ZAPPER:
                    _zappers.emplace_back(_zapperTexture, x, y, tileSize);
                    break;
                default:
                    break;
            }
        }
    }
}

void Jetpack::Game::updateObjects(float dt)
{
    for (auto& coin : _coins)
        coin.update(dt);
    for (auto& zapper : _zappers)
        zapper.update(dt);
}

void Jetpack::Game::renderObjects()
{
    float scrollX = _scrollOffset;
    float offsetX = -scrollX + FIXED_PLAYER_X;
    for (auto& coin : _coins)
        coin.draw(_window, offsetX);
    for (auto& zapper : _zappers)
        zapper.draw(_window, offsetX);
}
