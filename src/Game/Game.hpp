/*
** EPITECH PROJECT, 2025
** B-NWP-400-LIL-4-1-jetpack-nicolas.dumetz
** File description:
** Game
*/

#ifndef GAME_HPP_
#define GAME_HPP_
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <memory>
#include "client/Client.hpp"
#include "Error/Error.hpp"
#define WINDOW_HEIGHT 600
#define WINDOW_WIDTH 800


namespace Jetpack
{
    class Game {
        public:
            class GameError : public Jetpack::Error {
                public:
                    GameError(const std::string &message) : Jetpack::Error(message) {}
            };
        public:
            Game(std::shared_ptr<Jetpack::Client> client);
            ~Game();
            void waitingRoom();
            void run();
            void initGraphics();
            void updateMapScroll(float dt);
            void updateAnimation();

        private:
            std::shared_ptr<Jetpack::Client> _client;
            sf::RenderWindow _window;
            sf::Clock _clock;
            sf::Font _font;
            std::shared_ptr<SharedGameState> _sharedState;
            sf::Texture _mapTexture;
            sf::Sprite _mapSprite;
            sf::Texture _playerSpriteSheet;
            sf::Sprite _playerSprite1;
            sf::Sprite _playerSprite2;
            sf::Clock _animationClock;
            int _currentFrame;
            float _frameTime;
            float _scrollX;
            float _mapWidth;
            float _tileSize;
            
            void updatePlayerPositions();
            void renderPlayers();
            int getPlayerAnimationRow(int playerId, float playerY) const;
    };
}


#endif /* !GAME_HPP_ */