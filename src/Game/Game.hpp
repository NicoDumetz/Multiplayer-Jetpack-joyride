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
#include "Visuals/Coin/Coin.hpp"
#include "Visuals/Zapper/Zapper.hpp"
#include "client/Client.hpp"
#include "Error/Error.hpp"
#define WINDOW_HEIGHT 600
#define WINDOW_WIDTH 800

/// COIN ///
#define COIN_FRAME 6
#define COIN_FRAME_RATE 0.1f

/// ZAPPER ///
#define ZAPPER_FRAME 4
#define ZAPPER_FRAME_RATE 0.08f

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
            sf::Texture _coinTexture;
            sf::Texture _zapperTexture;
            std::vector<Coin> _coins;
            std::vector<Zapper> _zappers;
            sf::Clock _animationClock;
            int _currentFrame;
            float _frameTime;
            float _scrollX;
            float _mapWidth;
            float _tileSize;

            void initVisuals();
            void updateVisuals(float dt);
            void updatePlayerPositions();
            void renderPlayers();
            int getPlayerAnimationRow(int playerId, float playerY) const;
    };
}


#endif /* !GAME_HPP_ */
