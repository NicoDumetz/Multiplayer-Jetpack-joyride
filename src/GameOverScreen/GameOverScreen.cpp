/*
** EPITECH PROJECT, 2025
** B-NWP-400-LIL-4-1-jetpack-nicolas.dumetz
** File description:
** GameOverScreen
*/

#include "GameOverScreen.hpp"
#include <cmath>

Jetpack::GameOverScreen::GameOverScreen(sf::RenderWindow& window, sf::Font& font, std::shared_ptr<SharedGameState> sharedState, uint8_t numberClients)
    : _window(window), _font(font), _sharedState(sharedState), _numberClients(numberClients), _fadeOut(false), _alpha(0.0f)
{
}

void Jetpack::GameOverScreen::run(uint8_t winnerId)
{
    setupUI(winnerId);
    //playSound("gameover", 70.f); todo : trouver un son de game over
    
    _fadeOut = false;
    _alpha = 0.0f;
    _animClock.restart();
    
    while (_window.isOpen() && !_fadeOut) {
        handleEvents();
        update();
        render();
        
        if (_fadeOut) {
            _alpha += 0.05f;
            if (_alpha >= 1.0f) {
                _window.close();
            }
        }
    }
}

void Jetpack::GameOverScreen::playSound(const std::string& name, float volume)
{
    if (_soundCallback) {
        _soundCallback(name, volume);
    }
}

void Jetpack::GameOverScreen::setupUI(uint8_t winnerId)
{
    _gameOverText.setString("Game Over");
    _gameOverText.setFont(_font);
    _gameOverText.setCharacterSize(60);
    _gameOverText.setFillColor(sf::Color::White);
    sf::FloatRect gameOverBounds = _gameOverText.getLocalBounds();
    _gameOverText.setOrigin(gameOverBounds.width / 2.f, gameOverBounds.height / 2.f);
    _gameOverText.setPosition(_window.getSize().x / 2.f, _window.getSize().y / 3.f);

    std::string winnerStr;
    if (winnerId == 255) {
        winnerStr = "Match nul!";
    } else {
        winnerStr = "Joueur " + std::to_string(winnerId) + " gagne!";
    }
    
    _winnerText.setString(winnerStr);
    _winnerText.setFont(_font);
    _winnerText.setCharacterSize(40);
    _winnerText.setFillColor(sf::Color::Yellow);
    sf::FloatRect winnerBounds = _winnerText.getLocalBounds();
    _winnerText.setOrigin(winnerBounds.width / 2.f, winnerBounds.height / 2.f);
    _winnerText.setPosition(_window.getSize().x / 2.f, _window.getSize().y / 2.f);

    _instructionText.setString("Appuyez sur [Espace] ou [Escape] pour quitter");
    _instructionText.setFont(_font);
    _instructionText.setCharacterSize(24);
    _instructionText.setFillColor(sf::Color(200, 200, 200));
    sf::FloatRect instructionBounds = _instructionText.getLocalBounds();
    _instructionText.setOrigin(instructionBounds.width / 2.f, instructionBounds.height / 2.f);
    _instructionText.setPosition(_window.getSize().x / 2.f, _window.getSize().y * 0.7f);

    setupFinalScores();
}

void Jetpack::GameOverScreen::setupFinalScores()
{
    _finalScores.clear();
    
    for (int i = 0; i < this->_numberClients; ++i) {
        sf::Text scoreText;
        scoreText.setFont(_font);
        scoreText.setCharacterSize(30);
        
        auto playerState = _sharedState->getPlayerState(i);
        std::string scoreStr = "J" + std::to_string(i) + ": " + std::to_string(playerState.getCoins()) + " pieces";
        
        scoreText.setString(scoreStr);
        
        sf::Color textColor;
        if (i == 0) textColor = sf::Color(50, 200, 50);
        else if (i == 1) textColor = sf::Color(200, 50, 50);
        else textColor = sf::Color(50, 50, 200);
        
        scoreText.setFillColor(textColor);
        
        sf::FloatRect scoreBounds = scoreText.getLocalBounds();
        scoreText.setOrigin(scoreBounds.width / 2.f, scoreBounds.height / 2.f);
        scoreText.setPosition(_window.getSize().x / 2.f, _window.getSize().y / 2.f + 50 + i * 40);
        
        _finalScores.push_back(scoreText);
    }
}

void Jetpack::GameOverScreen::handleEvents()
{
    sf::Event event;
    while (_window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            _window.close();
        } else if (event.type == sf::Event::KeyPressed && 
                (event.key.code == sf::Keyboard::Space || event.key.code == sf::Keyboard::Escape)) {
            _fadeOut = true;
        }
    }
}

void Jetpack::GameOverScreen::update()
{
    float sinVal = std::sin(_animClock.getElapsedTime().asSeconds() * 2.0f) * 0.1f + 1.0f;
    _gameOverText.setScale(sinVal, sinVal);
}

void Jetpack::GameOverScreen::render()
{
    _window.clear();
    
    _window.draw(_gameOverText);
    _window.draw(_winnerText);
    
    for (const auto& score : _finalScores) {
        _window.draw(score);
    }
    
    _window.draw(_instructionText);
    
    if (_fadeOut) {
        sf::RectangleShape fadeRect;
        fadeRect.setSize(sf::Vector2f(_window.getSize().x, _window.getSize().y));
        fadeRect.setFillColor(sf::Color(0, 0, 0, static_cast<sf::Uint8>(_alpha * 255)));
        _window.draw(fadeRect);
    }
    
    _window.display();
}