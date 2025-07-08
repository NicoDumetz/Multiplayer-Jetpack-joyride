/*
** EPITECH PROJECT, 2025
** B-NWP-400-LIL-4-1-jetpack-nicolas.dumetz
** File description:
** WaitingRoom
*/

#include "GameWaitingRoom/WaitingRoom.hpp"
#include "Utils/Utils.hpp"
#include <cmath>
#include <random>

Jetpack::WaitingRoom::WaitingRoom(sf::RenderWindow& window, sf::Font& font,
                                   std::shared_ptr<SharedGameState> sharedState,
                                   std::shared_ptr<Jetpack::Client> client,
                                   const sf::Texture& playerTexture)
    : _window(window), _font(font), _sharedState(sharedState), _client(client),
      _playerTexture(playerTexture), _dotTimer(0.0f), _dotCount(0)
{
}

void Jetpack::WaitingRoom::run()
{
    initBackground();
    setupUI();

    sf::Clock animClock;

    while (_window.isOpen() && _client->getState() == Jetpack::ClientState::Waiting) {
        handleEvents();
        float deltaTime = animClock.restart().asSeconds();
        size_t currentConnected = static_cast<size_t>(_sharedState->getNumberClients());
        if (_animatedPlayers.size() != currentConnected)
            setupPlayers(currentConnected);
        updateUI(deltaTime);
        updatePlayers(deltaTime);
        render();
    }
}

void Jetpack::WaitingRoom::initBackground()
{
    if (!_backgroundTexture.loadFromFile("assets/waiting.png")) {
        return;
    }
    _backgroundSprite.setTexture(_backgroundTexture);

    float scaleX = static_cast<float>(_window.getSize().x) / _backgroundTexture.getSize().x;
    float scaleY = static_cast<float>(_window.getSize().y) / _backgroundTexture.getSize().y;
    _backgroundSprite.setScale(scaleX, scaleY);
}


void Jetpack::WaitingRoom::setupUI()
{
    _titleText.setFont(_font);
    _titleText.setCharacterSize(60);
    _titleText.setFillColor(sf::Color::White);
    _titleText.setString("Waiting for players...");
    sf::FloatRect titleBounds = _titleText.getLocalBounds();
    _titleText.setOrigin(titleBounds.width / 2.f, titleBounds.height / 2.f);
    _titleText.setPosition(_window.getSize().x / 2.f, _window.getSize().y / 3.f);

    _playerCountText.setFont(_font);
    _playerCountText.setCharacterSize(30);
    _playerCountText.setFillColor(sf::Color(200, 200, 200));

    _playerInfoText.setFont(_font);
    _playerInfoText.setCharacterSize(36);

    _instructionText.setFont(_font);
    _instructionText.setString("Appuyez sur [Escape] pour quitter");
    _instructionText.setCharacterSize(20);
    _instructionText.setFillColor(sf::Color(150, 150, 150));
    sf::FloatRect instructionBounds = _instructionText.getLocalBounds();
    _instructionText.setOrigin(instructionBounds.width / 2.f, instructionBounds.height / 2.f);
    _instructionText.setPosition(_window.getSize().x / 2.f, _window.getSize().y - 40);

    _textBackground.setSize(sf::Vector2f(_window.getSize().x * 0.9f, 250.f));
    _textBackground.setFillColor(sf::Color(0, 0, 0, 180));
    _textBackground.setOrigin(_textBackground.getSize().x / 2.f, _textBackground.getSize().y / 2.f);
    _textBackground.setPosition(_window.getSize().x / 2.f, _window.getSize().y / 2.f);

}

void Jetpack::WaitingRoom::setupPlayers(int count)
{
    _animatedPlayers.clear();

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> speedDist(50.0f, 150.0f);
    std::uniform_real_distribution<float> posDist(50.0f, _window.getSize().x - 50.0f);
    std::uniform_real_distribution<float> heightDist(_window.getSize().y * 0.7f, _window.getSize().y - 80.0f);

    for (int i = 0; i < count; ++i) {
        AnimatedPlayer player;
        player.sprite.setTexture(_playerTexture);
        player.sprite.setTextureRect({0, 0, PLAYER_SPRITE_WIDTH, PLAYER_SPRITE_HEIGHT});

        float scale = 0.5f * PLAYER_SCALE;
        player.sprite.setScale(scale, scale);
        player.posX = posDist(gen);
        player.posY = heightDist(gen);
        player.speedX = speedDist(gen);
        player.speedY = 0;
        player.movingRight = (i % 2 == 0);
        player.currentFrame = i % 4;
        player.animTimer = 0.0f;

        if (!player.movingRight)
            player.sprite.setScale(-scale, scale);

        player.sprite.setPosition(player.posX, player.posY);
        _animatedPlayers.push_back(player);
    }
}

void Jetpack::WaitingRoom::updateUI(float deltaTime)
{
    _dotTimer += deltaTime;
    if (_dotTimer >= 0.5f) {
        _dotTimer = 0.0f;
        _dotCount = (_dotCount + 1) % 4;
    }

    std::string dots(_dotCount, '.');
    _titleText.setString("Waiting for players" + dots);
    sf::FloatRect titleBounds = _titleText.getLocalBounds();
    _titleText.setOrigin(titleBounds.width / 2.f, titleBounds.height / 2.f);
    _titleText.setFillColor(sf::Color::White);

    int connectedPlayers = static_cast<int>(_sharedState->getNumberClients());
    if (connectedPlayers > _client->getExpectedPlayerCount() || connectedPlayers < 0)
        connectedPlayers = 0;

    _playerCountText.setString("Joueurs connectes : " + std::to_string(connectedPlayers) + "/" + std::to_string(_client->getExpectedPlayerCount()));
    sf::FloatRect countBounds = _playerCountText.getLocalBounds();
    _playerCountText.setOrigin(countBounds.width / 2.f, countBounds.height / 2.f);
    _playerCountText.setPosition(_window.getSize().x / 2.f, _window.getSize().y / 2.f);

    sf::Color playerColor = sf::Color(50, 200, 50);
    if (_client->getPlayerId() == 1) playerColor = sf::Color(200, 50, 50);
    else if (_client->getPlayerId() >= 2) playerColor = sf::Color(50, 50, 200);

    _playerInfoText.setString("Vous etes le Joueur " + std::to_string(_client->getPlayerId()));
    _playerInfoText.setFillColor(playerColor);
    sf::FloatRect infoBounds = _playerInfoText.getLocalBounds();
    _playerInfoText.setOrigin(infoBounds.width / 2.f, infoBounds.height / 2.f);
    _playerInfoText.setPosition(_window.getSize().x / 2.f, _window.getSize().y / 2.f + 50);
}

void Jetpack::WaitingRoom::updatePlayers(float deltaTime)
{
    for (auto& player : _animatedPlayers) {
        player.animTimer += deltaTime;
        if (player.animTimer >= 0.1f) {
            player.animTimer = 0.0f;
            player.currentFrame = (player.currentFrame + 1) % 4;
            player.sprite.setTextureRect({
                player.currentFrame * PLAYER_SPRITE_WIDTH,
                player.movingRight ? 0 : 1 * PLAYER_SPRITE_HEIGHT,
                PLAYER_SPRITE_WIDTH,
                PLAYER_SPRITE_HEIGHT
            });
        }

        if (player.movingRight) {
            player.posX += player.speedX * deltaTime;
            if (player.posX > _window.getSize().x - 20) {
                player.movingRight = false;
                float scale = player.sprite.getScale().y;
                player.sprite.setScale(-scale, scale);
            }
        } else {
            player.posX -= player.speedX * deltaTime;
            if (player.posX < 20) {
                player.movingRight = true;
                float scale = player.sprite.getScale().y;
                player.sprite.setScale(scale, scale);
            }
        }
        player.sprite.setPosition(player.posX, player.posY);
    }
}

void Jetpack::WaitingRoom::handleEvents()
{
    sf::Event event;
    while (_window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            _client->disconnect();
            _window.close();
        } else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
            _client->disconnect();
            _window.close();
        }
    }
}

void Jetpack::WaitingRoom::renderPlayers()
{
    for (const auto& player : _animatedPlayers)
        _window.draw(player.sprite);
}

void Jetpack::WaitingRoom::render()
{
    _window.clear();
    _window.draw(_backgroundSprite); 
    renderPlayers();
    _window.draw(_textBackground);
    _window.draw(_titleText);
    _window.draw(_playerCountText);
    _window.draw(_playerInfoText);
    _window.draw(_instructionText);
    _window.display();
}
