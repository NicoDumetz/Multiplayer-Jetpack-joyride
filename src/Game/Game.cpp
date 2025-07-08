/*
** EPITECH PROJECT, 2025
** B-NWP-400-LIL-4-1-jetpack-nicolas.dumetz
** File description:
** Game
*/

#include "Game/Game.hpp"
#include "Utils/Utils.hpp"

Jetpack::Game::Game(std::shared_ptr<Jetpack::Client> client)
    : _client(client),
    _window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Jetpack Client", sf::Style::Titlebar | sf::Style::Close),
    _sharedState(client->getSharedState())
{
    this->_window.setFramerateLimit(60);
    initGraphics();
}

Jetpack::Game::~Game()
{
}

void Jetpack::Game::initVisuals()
{
    const std::vector<std::vector<TileType>>& map = this->_client->getMap();

    if (!this->_coinTexture.loadFromFile("assets/coin.png"))
        throw GameError("Failed to load coin texture");
    if (!this->_zapperTexture.loadFromFile("assets/zapper.png"))
        throw GameError("Failed to load zapper texture");

    for (size_t y = 0; y < map.size(); ++y) {
        for (size_t x = 0; x < map[y].size(); ++x) {
            if (map[y][x] == TileType::COIN)
                this->_coins.emplace_back(this->_coinTexture, x, y, _tileSize);
            else if (map[y][x] == TileType::ZAPPER)
                this->_zappers.emplace_back(this->_zapperTexture, x, y, _tileSize);
        }
    }
}

void Jetpack::Game::initGraphics()
{
    float scaleY;

    if (!this->_font.loadFromFile("assets/font.ttf"))
        throw GameError("Failed to load font");
    if (!this->_mapTexture.loadFromFile("assets/background.png"))
        throw GameError("Failed to load map image");
    if (!this->_playerSpriteSheet.loadFromFile("assets/player.png"))
        throw GameError("Failed to load player sprite sheet");

    for (int i = 0; i < NUMBER_CLIENTS; i++) {
        sf::Sprite playerSprite;
        playerSprite.setTexture(this->_playerSpriteSheet);
        _playerSprites.push_back(playerSprite);
    }
    for (auto &playerSprite : _playerSprites) {
        playerSprite.setTextureRect(sf::IntRect(0, 0, PLAYER_SPRITE_WIDTH, PLAYER_SPRITE_HEIGHT));
        playerSprite.setScale(PLAYER_SCALE, PLAYER_SCALE);
        playerSprite.setOrigin(PLAYER_SPRITE_WIDTH * PLAYER_ORIGIN_X, PLAYER_SPRITE_HEIGHT * PLAYER_ORIGIN_Y);
    }
    this->_mapSprite.setTexture(this->_mapTexture);
    scaleY = static_cast<float>(WINDOW_HEIGHT) / _mapTexture.getSize().y;
    _mapSprite.setScale(scaleY, scaleY);
    _mapWidth = _mapTexture.getSize().x * scaleY;
    _scrollX = 0.0f;
    _tileSize = TILE_SIZE;
    _currentFrame = 0;
    _frameTime = ANIMATION_FRAME_TIME;
    _animationClock.restart();

    initVisuals();
}

void Jetpack::Game::updateMapScroll(float dt)
{
    float maxScroll = _mapWidth - WINDOW_WIDTH;
    _scrollX += SCROLL_SPEED_GAME * dt;
    if (_scrollX > maxScroll)
        _scrollX = maxScroll;
    _mapSprite.setPosition(-_scrollX, 0);
}

void Jetpack::Game::updateAnimation()
{
    int row;

    if (_animationClock.getElapsedTime().asSeconds() >= _frameTime) {
        _currentFrame = (_currentFrame + 1) % 4;

        for (std::size_t i = 0; i < _playerSprites.size(); i++) {
            auto playerState = this->_client->getSharedState()->getPlayerState(i);
            row = getPlayerAnimationRow(i, playerState.getY());
            _playerSprites[i].setTextureRect(sf::IntRect(_currentFrame * PLAYER_SPRITE_WIDTH, row * PLAYER_SPRITE_HEIGHT, PLAYER_SPRITE_WIDTH, PLAYER_SPRITE_HEIGHT));
        }
        _animationClock.restart();
    }
}

int Jetpack::Game::getPlayerAnimationRow(int playerId, float playerY) const
{
    const float groundLevel = 8.0f;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && this->_client->getPlayerId() == playerId)
        return 1;
    else if (playerY < groundLevel - 0.5f)
        return 1;
    else
        return 0;
}


void Jetpack::Game::updatePlayerPositions()
{
    float screenY;

    if (this->_client->getSharedState()->getPlayers().size() <= 1)
        return;

    for (std::size_t i = 0; i < _playerSprites.size(); i++) {
        auto playerState = this->_client->getSharedState()->getPlayerState(i);
        if (playerState.isAlive()) {
            screenY = playerState.getY() * _tileSize - 80.0f;
            _playerSprites[i].setPosition(FIXED_PLAYER_X, screenY);
            _playerSprites[i].setScale(PLAYER_SCALE, PLAYER_SCALE);
        }
    }
}

void Jetpack::Game::renderPlayers()
{
    sf::Color transparentColor(255, 255, 255, 128);
    uint8_t localPlayerId;

    if (this->_client->getSharedState()->getPlayers().size() <= 1)
        return;

    for (std::size_t i = 0; i < _playerSprites.size(); i++) {
        auto playerState = this->_client->getSharedState()->getPlayerState(i);
        localPlayerId = this->_client->getPlayerId();
        if (localPlayerId == i && playerState.isAlive()) {
            _playerSprites[i].setColor(sf::Color::White);
            this->_window.draw(this->_playerSprites[i]);
        } else if (playerState.isAlive()) {
            _playerSprites[i].setColor(transparentColor);
            this->_window.draw(this->_playerSprites[i]);
        }
    }
}

void Jetpack::Game::waitingRoom()
{
    sf::Text text("Waiting for players...", this->_font, 60);
    sf::FloatRect textRect = text.getLocalBounds();

    text.setFillColor(sf::Color::White);
    text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
    text.setPosition(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f);

    while (this->_window.isOpen() && _client->getState() == Jetpack::ClientState::Waiting) {
        sf::Event event;
        while (this->_window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                this->_window.close();
            }
        }
        this->_window.clear();
        this->_window.draw(text);
        this->_window.display();
    }
}

void Jetpack::Game::run()
{
    sf::Event event;
    sf::Clock clock;

    while (this->_window.isOpen() && _client->getState() == Jetpack::ClientState::Connected) {
        this->_window.clear();
        while (this->_window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                this->_window.close();
                throw GameError("Window closed by user");
            }
        }

        float deltaTime = clock.restart().asSeconds();
        updateMapScroll(deltaTime);
        this->_window.draw(this->_mapSprite);
        updateVisuals(deltaTime);
        updateAnimation();
        if (this->_window.hasFocus() && sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
            this->_client->sendJump();
        updatePlayerPositions();
        if (this->_client->getSharedState()->getPlayers().size() > 1) {
            auto playerState1 = this->_client->getSharedState()->getPlayerState(0);
            auto playerState2 = this->_client->getSharedState()->getPlayerState(1);

            std::cout << "PLAYER 0:" << "Position: x = " << playerState1.getX() << ", y = " << playerState1.getY()
                        << "Alive: " << (playerState1.isAlive() ? "Yes " : "No ") << "Coins: " << playerState1.getCoins() << std::endl;

            std::cout << "PLAYER 1:" << "Position: x = " << playerState2.getX() << ", y = " << playerState2.getY()
                        << "Alive: " << (playerState2.isAlive() ? "Yes " : "No ") << "Coins: " << playerState2.getCoins() << std::endl;
        }
        renderPlayers();
        this->_window.display();
    }
}

void Jetpack::Game::updateVisuals(float dt)
{
    for (auto& coin : _coins) {
        coin.update(dt);
        coin.move(-SCROLL_SPEED * this->_tileSize * dt, 0);
        coin.draw(this->_window);
    }
    for (auto& zapper : _zappers) {
        zapper.update(dt);
        zapper.move(-SCROLL_SPEED * this->_tileSize * dt, 0);
        zapper.draw(this->_window);
    }
}
