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

    this->_playerSprite1.setTexture(this->_playerSpriteSheet);
    this->_playerSprite2.setTexture(this->_playerSpriteSheet);

    int spriteWidth = 538 / 4;
    int spriteHeight = 803 / 6;

    this->_playerSprite1.setTextureRect(sf::IntRect(0, 0, spriteWidth, spriteHeight));
    this->_playerSprite2.setTextureRect(sf::IntRect(0, 0, spriteWidth, spriteHeight));

    float scale = 0.5f;
    this->_playerSprite1.setScale(scale, scale);
    this->_playerSprite2.setScale(scale, scale);

    this->_playerSprite1.setOrigin(spriteWidth/2.0f, spriteHeight/2.0f);
    this->_playerSprite2.setOrigin(spriteWidth/2.0f, spriteHeight/2.0f);

    this->_mapSprite.setTexture(this->_mapTexture);

    scaleY = static_cast<float>(WINDOW_HEIGHT) / _mapTexture.getSize().y;
    _mapSprite.setScale(scaleY, scaleY);

    _mapWidth = _mapTexture.getSize().x * scaleY;
    _scrollX = 0.0f;

    _tileSize = WINDOW_HEIGHT / 9.0f;
    _currentFrame = 0;
    _frameTime = 0.1f;
    _animationClock.restart();

    initVisuals();
}

void Jetpack::Game::updateMapScroll(float dt)
{
    float maxScroll;

    _scrollX += SCROLL_SPEED * this->_tileSize * dt;
    maxScroll = _mapWidth - WINDOW_WIDTH;

    if (_scrollX > maxScroll)
        _scrollX = maxScroll;

    _mapSprite.setPosition(-_scrollX, 0);
}

void Jetpack::Game::updateAnimation()
{
    if (_animationClock.getElapsedTime().asSeconds() >= _frameTime) {
        _currentFrame = (_currentFrame + 1) % 4;

        int spriteWidth = 538 / 4;
        int spriteHeight = 803 / 6;

        auto playerState1 = this->_client->getSharedState()->getPlayerState(0);
        auto playerState2 = this->_client->getSharedState()->getPlayerState(1);

        int row1 = getPlayerAnimationRow(0, playerState1.getY());
        int row2 = getPlayerAnimationRow(1, playerState2.getY());

        _playerSprite1.setTextureRect(sf::IntRect(_currentFrame * spriteWidth, row1 * spriteHeight, spriteWidth, spriteHeight));
        _playerSprite2.setTextureRect(sf::IntRect(_currentFrame * spriteWidth, row2 * spriteHeight, spriteWidth, spriteHeight));

        _animationClock.restart();
    }
}

int Jetpack::Game::getPlayerAnimationRow(int playerId, float playerY) const
{
    const float groundLevel = 8.0f;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && this->_client->getPlayerId() == playerId) {
        return 1;
    }
    else if (playerY < groundLevel - 0.5f) {
        return 1;
    }
    else {
        return 0;
    }
}

void Jetpack::Game::updatePlayerPositions()
{
    if (this->_client->getSharedState()->getPlayers().size() <= 1)
        return;

    auto playerState1 = this->_client->getSharedState()->getPlayerState(0);
    auto playerState2 = this->_client->getSharedState()->getPlayerState(1);

    float player1X = WINDOW_WIDTH / 3.0f;
    float player2X = WINDOW_WIDTH / 3.0f;

    if (playerState1.isAlive()) {
        float screenY = playerState1.getY() * _tileSize - 80.0f;
        _playerSprite1.setPosition(player1X, screenY);
        _playerSprite1.setScale(0.5f, 0.5f);
    }

    if (playerState2.isAlive()) {
        float screenY = playerState2.getY() * _tileSize - 80.0f;
        _playerSprite2.setPosition(player2X, screenY);
        _playerSprite2.setScale(0.5f, 0.5f);
    }
}

void Jetpack::Game::renderPlayers()
{
    if (this->_client->getSharedState()->getPlayers().size() <= 1)
        return;

    auto playerState1 = this->_client->getSharedState()->getPlayerState(0);
    auto playerState2 = this->_client->getSharedState()->getPlayerState(1);

    uint8_t localPlayerId = this->_client->getPlayerId();
    sf::Color transparentColor(255, 255, 255, 128);

    if (localPlayerId == 0) {
        if (playerState1.isAlive()) {
            _playerSprite1.setColor(sf::Color::White);
            this->_window.draw(this->_playerSprite1);
        }

        if (playerState2.isAlive()) {
            _playerSprite2.setColor(transparentColor);
            this->_window.draw(this->_playerSprite2);
        }
    } else {
        if (playerState2.isAlive()) {
            _playerSprite2.setColor(sf::Color::White);
            this->_window.draw(this->_playerSprite2);
        }

        if (playerState1.isAlive()) {
            _playerSprite1.setColor(transparentColor);
            this->_window.draw(this->_playerSprite1);
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

        // if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && clock.getElapsedTime().asSeconds() >= JUMP_INTERVAL) {
        //     Jetpack::ProtocolUtils::sendPacket(this->_client->getSocket(), PLAYER_ACTION, {static_cast<uint8_t>(Jetpack::PlayerActionType::JUMP)});
        //     clock.restart();
        // }
        if (this->_window.hasFocus() && sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
            // Jetpack::ProtocolUtils::sendPacket(this->_client->getSocket(), PLAYER_ACTION, {static_cast<uint8_t>(Jetpack::PlayerActionType::JUMP)});
            this->_client->sendJump();
        }

        // if (_client->getSharedState()->getPlayers().size() > 1) {
        //     std::cout << "Actual Player ID: " << static_cast<int>(_client->getPlayerId()) << std::endl;
        //     auto playerState = _client->getSharedState()->getPlayerState(static_cast<int>(_client->getPlayerId()));  // Get player state
        //     std::cout << "Player Info:" << std::endl;
        //     std::cout << "Position: x = " << playerState.getX() << ", y = " << playerState.getY() << std::endl;
        //     std::cout << "Alive: " << (playerState.isAlive() ? "Yes" : "No") << std::endl;
        //     std::cout << "Coins: " << playerState.getCoins() << std::endl;
        // }

        if (this->_client->getSharedState()->getPlayers().size() > 1) {
            auto playerState1 = this->_client->getSharedState()->getPlayerState(0);
            auto playerState2 = this->_client->getSharedState()->getPlayerState(1);

            updatePlayerPositions();

            // std::cout << "PLAYER 0:" << "Position: x = " << playerState1.getX() << ", y = " << playerState1.getY()
                      // << "Alive: " << (playerState1.isAlive() ? "Yes " : "No ") << "Coins: " << playerState1.getCoins() << std::endl;

            // std::cout << "PLAYER 1:" << "Position: x = " << playerState2.getX() << ", y = " << playerState2.getY()
                      // << "Alive: " << (playerState2.isAlive() ? "Yes " : "No ") << "Coins: " << playerState2.getCoins() << std::endl;
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
