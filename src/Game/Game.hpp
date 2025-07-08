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

/// COIN ///
#define COIN_FRAME 6
#define COIN_FRAME_RATE 0.1f

/// ZAPPER ///
#define ZAPPER_FRAME 4
#define ZAPPER_FRAME_RATE 0.08f

/// SCORE DISPLAY ///
#define SCORE_MARGIN_TOP 10
#define SCORE_MARGIN_LEFT 10
#define SCORE_SPACING 25
#define SCORE_FONT_SIZE 18

namespace Jetpack {
    class Game {
      public:
        class GameError : public Jetpack::Error {
          public:
            GameError(const std::string &message) : Jetpack::Error(message) {}
        };

        Game(std::shared_ptr<Jetpack::Client> client);
        ~Game();

        void run();
        void waitingRoom();

      private:
        void initGraphics();
        void updateMapScroll(float dt);
        void updateAnimation();
        void updatePlayerPositions();
        void renderPlayers();
        void drawGrid();
        void drawBackground();
        int getPlayerAnimationRow(int playerId, float playerY) const;
        void initObjectsFromMap();
        void updateObjects(float dt);
        void renderObjects();
        void renderScoreDisplay();
        void initScoreDisplay();

    private:
        std::shared_ptr<Jetpack::Client> _client;
        sf::RenderWindow _window;
        std::shared_ptr<SharedGameState> _sharedState;
        sf::Font _font;
        sf::Texture _mapTexture;
        sf::Sprite _mapSprite;
        sf::Texture _playerSpriteSheet;
        std::vector<sf::Sprite> _playerSprites;
        sf::Clock _animationClock;
        std::vector<Jetpack::Coin> _coins;
        std::vector<Jetpack::Zapper> _zappers;
        sf::Texture _coinTexture;
        sf::Texture _zapperTexture;
        int _currentFrame = 0;
        float _frameTime = ANIMATION_FRAME_TIME;
        float _scrollOffset = 0.0f;
        float _tileSize = TILE_SIZE;
        const float BACKGROUND_ZOOM = 1.25f;
        void updateCoinsVisibility();
        sf::RectangleShape _scoreBackground;
        std::vector<sf::Text> _scoreTexts;
        sf::Sprite _coinIcon;
    };
}

#endif /* !GAME_HPP_ */
