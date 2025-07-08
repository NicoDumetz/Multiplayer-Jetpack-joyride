/*
** EPITECH PROJECT, 2025
** B-NWP-400-LIL-4-1-jetpack-nicolas.dumetz
** File description:
** GameOverScreen
*/

#ifndef GAMEOVERSCREEN_HPP_
#define GAMEOVERSCREEN_HPP_

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "client/Client.hpp"

namespace Jetpack {

class GameOverScreen {
    public:
        GameOverScreen(sf::RenderWindow& window, sf::Font& font, std::shared_ptr<SharedGameState> sharedState, uint8_t numberClients);
        ~GameOverScreen() = default;

        void run(uint8_t winnerId);
        void playSound(const std::string& name, float volume);
        std::function<void(const std::string&, float)> _soundCallback;

    private:
        void setupUI(uint8_t winnerId);
        void setupFinalScores();
        void handleEvents();
        void update();
        void render();
        void initBackground();
        
        sf::RenderWindow& _window;
        sf::Font& _font;
        std::shared_ptr<SharedGameState> _sharedState;
        uint8_t _numberClients;
        
        sf::Text _gameOverText;
        sf::Text _winnerText;
        sf::Text _instructionText;
        std::vector<sf::Text> _finalScores;
        sf::Texture _backgroundTexture;
        sf::Sprite _backgroundSprite;
        
        sf::Clock _animClock;
        bool _fadeOut;
        float _alpha;
};

} // namespace Jetpack

#endif /* !GAMEOVERSCREEN_HPP_ */