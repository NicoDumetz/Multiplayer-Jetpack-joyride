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
#define SPRITE_SHEET_WIDTH 538  // the total width of the sprite sheet
#define SPRITE_SHEET_HEIGHT 803 // the total height of the sprite sheet
#define PLAYER_SCALE 0.5f // Player scale factor (resize the player sprites)
#define PLAYER_ORIGIN_X 0.5f // The origin point of the player sprite (centered in the middle of the sprite)
#define PLAYER_ORIGIN_Y 0.5f // The origin point of the player sprite (centered in the middle of the sprite)
#define PLAYER_SPRITE_WIDTH (SPRITE_SHEET_WIDTH / 4)  // Dividing the total width by the number of columns (4)
#define PLAYER_SPRITE_HEIGHT (SPRITE_SHEET_HEIGHT / 6) // Dividing the total height by the number of rows (6)
#define FIXED_PLAYER_X (WINDOW_WIDTH / 4.0f) // Fixed position for all players on the X axis, placed at 1/4 of the screen width
#define ANIMATION_FRAME_TIME 0.1f // Time between each animation frame for the player sprite
#define TILE_SIZE (WINDOW_HEIGHT / 9.0f) // Size of one tile in the game (based on the screen height, divided by 9 for the tile grid)
#define SCROLL_SPEED_GAME 100.0f // Speed of the scrolling background

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
            sf::Texture _coinTexture;
            sf::Texture _zapperTexture;
            std::vector<Coin> _coins;
            std::vector<Zapper> _zappers;
            std::vector<sf::Sprite> _playerSprites;
            std::vector<sf::IntRect> _playerTextureRects;
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
