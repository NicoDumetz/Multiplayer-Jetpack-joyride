/*
** EPITECH PROJECT, 2025
** B-NWP-400-LIL-4-1-jetpack-nicolas.dumetz
** File description:
** Game
*/
#include "Game/Game.hpp"

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

void Jetpack::Game::initGraphics()
{
    float scaleY;

    if (!this->_font.loadFromFile("assets/font.ttf"))
        throw GameError("Failed to load font");

    if (!this->_mapTexture.loadFromFile("assets/background.png"))
        throw GameError("Failed to load map image");

    this->_mapSprite.setTexture(this->_mapTexture);

    scaleY = static_cast<float>(WINDOW_HEIGHT) / _mapTexture.getSize().y;
    _mapSprite.setScale(scaleY, scaleY);

    _mapWidth = _mapTexture.getSize().x * scaleY;
    _scrollX = 0.0f;
}

void Jetpack::Game::updateMapScroll(float dt)
{
    float maxScroll;

    _scrollX += 100.0f * dt;
    maxScroll = _mapWidth - WINDOW_WIDTH;

    if (_scrollX > maxScroll)
        _scrollX = maxScroll;

    _mapSprite.setPosition(-_scrollX, 0);
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
    sf::Clock clock;

    while (this->_window.isOpen() && _client->getState() == Jetpack::ClientState::Connected) {
        sf::Event event;
        while (this->_window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                this->_window.close();
                throw GameError("Window closed by user");
            }
        }
    
        float deltaTime = clock.restart().asSeconds();
        updateMapScroll(deltaTime);

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
            auto playerState = this->_client->getSharedState()->getPlayerState(0);
            std::cout << "PLAYER 0:" << "Position: x = " << playerState.getX() << ", y = " << playerState.getY() << "Alive: " << (playerState.isAlive() ? "Yes " : "No ") << "Coins: " << playerState.getCoins() << std::endl;

            auto playerState2 = this->_client->getSharedState()->getPlayerState(1);
            std::cout << "PLAYER 1:" << "Position: x = " << playerState2.getX() << ", y = " << playerState2.getY() << "Alive: " << (playerState2.isAlive() ? "Yes " : "No ") << "Coins: " << playerState2.getCoins() << std::endl;
        }
        this->_window.clear();
        this->_window.draw(this->_mapSprite);
        this->_window.display();
    }
}

