/*
** EPITECH PROJECT, 2025
** B-NWP-400-LIL-4-1-jetpack-nicolas.dumetz
** File description:
** WaitingRoom
*/

#ifndef WAITINGROOM_HPP_
#define WAITINGROOM_HPP_

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <functional>
#include "client/Client.hpp"

namespace Jetpack {

class WaitingRoom {
    public:
        WaitingRoom(sf::RenderWindow& window, sf::Font& font, std::shared_ptr<SharedGameState> sharedState, 
                   std::shared_ptr<Jetpack::Client> client, const sf::Texture& playerTexture);
        ~WaitingRoom() = default;

        void run();

    private:
        struct AnimatedPlayer {
            sf::Sprite sprite;
            float posX;
            float posY;
            float speedX;
            float speedY;
            bool movingRight;
            int currentFrame;
            float animTimer;
        };

        void setupUI();
        void updateUI(float deltaTime);
        void render();
        void handleEvents();
        void setupPlayers();
        void updatePlayers(float deltaTime);
        void renderPlayers();
        
        sf::RenderWindow& _window;
        sf::Font& _font;
        std::shared_ptr<SharedGameState> _sharedState;
        std::shared_ptr<Jetpack::Client> _client;
        const sf::Texture& _playerTexture;
        
        sf::Text _titleText;
        sf::Text _playerCountText;
        sf::Text _playerInfoText;
        sf::Text _instructionText;
        
        sf::RectangleShape _textBackground;
        
        float _dotTimer = 0.0f;
        int _dotCount = 0;
        
        std::vector<AnimatedPlayer> _animatedPlayers;
};

} // namespace Jetpack

#endif /* !WAITINGROOM_HPP_ */