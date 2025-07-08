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
    for (int i = 0; i < this->_client->getExpectedPlayerCount(); ++i) {
        sf::Sprite sprite;
        sprite.setTexture(_playerSpriteSheet);
        sprite.setTextureRect({0, 0, PLAYER_SPRITE_WIDTH, PLAYER_SPRITE_HEIGHT});
        sprite.setScale(PLAYER_SCALE, PLAYER_SCALE);
        sprite.setOrigin(PLAYER_SPRITE_WIDTH * PLAYER_ORIGIN_X, PLAYER_SPRITE_HEIGHT * PLAYER_ORIGIN_Y);
        _playerSprites.push_back(sprite);
    }
    _animationClock.restart();
    initScoreDisplay();
}

void Jetpack::Game::run()
{
    sf::Event event;
    sf::Clock clock;
    float deltaTime;

    playMusic("theme", 50.f);

    while (_window.isOpen()) {
        if (_client->getState() == Jetpack::ClientState::GameOver) {
            music.stop();
            showGameOverScreen(_client->getGameOverWinnerId());
            break;
        }
        
        if (_client->getState() != Jetpack::ClientState::Connected) {
            break;
        }
        
        while (_window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                _window.close();
                this->_client->disconnect();
            } else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                _window.close();
                this->_client->disconnect();
            }
        }
        deltaTime = clock.restart().asSeconds();
        updateMapScroll(deltaTime);
        updateAnimation();
        updatePlayerPositions();
        updateObjects(deltaTime);
        updateCoinsVisibility();
        if (_window.hasFocus() && sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
            _client->sendJump();
        _window.clear();
        drawBackground();
        drawGrid();
        renderObjects();
        renderPlayers();
        renderScoreDisplay();
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
            if (event.type == sf::Event::Closed) {
                _client->disconnect();
                _window.close();
                return;
            } else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                _window.close();
                return;
            }
        }
        _window.clear();
        _window.draw(text);
        _window.display();
    }
}

void Jetpack::Game::showGameOverScreen(uint8_t winnerId)
{
    if (!_gameOverScreen) {
        _gameOverScreen = std::make_unique<GameOverScreen>(_window, _font, _sharedState);
        _gameOverScreen->_soundCallback = [this](const std::string& name, float volume) {
            this->playSound(name, volume);
        };
    }
    
    _gameOverScreen->run(winnerId);
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
    
    float mapWidth = _mapTexture.getSize().x * finalScale;
    float mapHeight = _mapTexture.getSize().y * finalScale;
    float y = (WINDOW_HEIGHT - mapHeight) / 2.0f;
    
    float effectiveScrollOffset = std::fmod(_scrollOffset, mapWidth);
    
    for (int i = -1; i < 2; i++) {
        float x = -effectiveScrollOffset + (i * mapWidth);
        _mapSprite.setPosition(x, y);
        _window.draw(_mapSprite);
    }
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


void Jetpack::Game::updateCoinsVisibility()
{
    uint8_t clientPlayerId = _client->getPlayerId();
    const auto& collectedCoins = _sharedState->getPlayerState(clientPlayerId).getCoinCollected();
    
    for (auto& coin : _coins) {
        auto [x, y] = coin.getTilePosition();
        std::pair<int, int> coinPos(x, y);
        bool isCollected = std::find(collectedCoins.begin(), collectedCoins.end(), coinPos) != collectedCoins.end();
        
        if (isCollected) {
            coin.setTransparent(true);
        }
    }
}

void Jetpack::Game::initScoreDisplay()
{
    _scoreBackground.setSize(sf::Vector2f(120, 10 + this->_client->getExpectedPlayerCount() * 25));
    _scoreBackground.setFillColor(sf::Color(0, 0, 0, 150));
    _scoreBackground.setPosition(SCORE_MARGIN_LEFT, SCORE_MARGIN_TOP);

    _scoreTexts.clear();
    for (int i = 0; i < this->_client->getExpectedPlayerCount(); ++i) {
        sf::Text scoreText;
        scoreText.setFont(_font);
        scoreText.setCharacterSize(SCORE_FONT_SIZE);
        scoreText.setPosition(
            SCORE_MARGIN_LEFT + 10, 
            SCORE_MARGIN_TOP + 5 + i * 25
        );
        sf::Color textColor;
        if (i == 0) 
            textColor = sf::Color(50, 200, 50);
        else if (i == 1)
            textColor = sf::Color(200, 50, 50);
        else 
            textColor = sf::Color(50, 50, 200);
        
        scoreText.setFillColor(textColor);
        scoreText.setString("J" + std::to_string(i) + ": 0");
        _scoreTexts.push_back(scoreText);
    }
}


void Jetpack::Game::renderScoreDisplay()
{
    _window.draw(_scoreBackground);

    for (int i = 0; i < std::min(static_cast<int>(_scoreTexts.size()), static_cast<int>(this->_client->getExpectedPlayerCount())); ++i) {
        auto playerState = _sharedState->getPlayerState(i);
        
        _scoreTexts[i].setString("J" + std::to_string(i) + ": " + 
                                std::to_string(playerState.getCoins()));
        
        sf::Color color = _scoreTexts[i].getFillColor();
        color.a = playerState.isAlive() ? 255 : 128;
        _scoreTexts[i].setFillColor(color);
        
        _window.draw(_scoreTexts[i]);
    }
}

void Jetpack::Game::playMusic(const std::string& filename, float volume)
{
    std::string filepath = "assets/" + filename + ".ogg";
    std::ifstream fileCheck(filepath);
    if (!fileCheck) {
        return;
    }
    fileCheck.close();
    
    if (!music.openFromFile(filepath)) {
        Jetpack::Utils::consoleLog("Failed to load music: " + filename, Jetpack::LogInfo::ERROR);
        return;
    }
    
    music.setLoop(true);
    music.setVolume(volume);
    music.play();
}

void Jetpack::Game::playSound(const std::string& name, float volume)
{
    std::string filepath = "assets/" + name + ".ogg";
    std::ifstream fileCheck(filepath);
    if (!fileCheck) {
        return;
    }
    fileCheck.close();
    
    auto it = soundBuffers.find(name);
    
    if (it == soundBuffers.end()) {
        sf::SoundBuffer buffer;
        if (!buffer.loadFromFile(filepath)) {
            Jetpack::Utils::consoleLog("Failed to load sound: " + name, Jetpack::LogInfo::ERROR);
            return;
        }
        it = soundBuffers.insert(std::make_pair(name, buffer)).first;
    }

    sounds.erase(
        std::remove_if(sounds.begin(), sounds.end(), Jetpack::Game::isStoppedSound),
        sounds.end()
    );
    
    sounds.emplace_back(it->second);
    sounds.back().setVolume(volume);
    sounds.back().play();
}

bool Jetpack::Game::isStoppedSound(const sf::Sound& sound)
{
    return sound.getStatus() == sf::Sound::Stopped;
}

