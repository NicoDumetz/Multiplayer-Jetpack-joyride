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
#include "GameOverScreen/GameOverScreen.hpp"
#include "GameWaitingRoom/WaitingRoom.hpp"

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
        struct animState;

        void initGraphics();
        void updateMapScroll(float dt);
        void updateAnimation();
        void updatePlayerPositions();
        void renderPlayers();
        void drawGrid();
        void drawBackground();
        int getPlayerAnimationRow(Jetpack::PlayerState& playerState, animState& state);
        void initObjectsFromMap();
        void updateObjects(float dt);
        void renderObjects();
        void renderScoreDisplay();
        void initScoreDisplay();
        void updateCoinsVisibility();
        void playMusic(const std::string& filename, float volume = 100.f);
        void playSound(const std::string& name, float volume = 100.f);
        void showGameOverScreen(uint8_t winnerId);
        static bool isStoppedSound(const sf::Sound& sound);

        enum class state {
            WALK = 0,
            FLY,
            LAND,
            ELECTROCUTE,
            BURN,
            SPIN,
            NONE
        };

        struct animState {
            state _state = Game::state::WALK;
            int frame = 0;
            float clock = 0.f;
            float slow = 0.f;
            int loop = 2;
        };

    private:
        std::shared_ptr<Jetpack::Client> _client;
        sf::RenderWindow _window;
        std::shared_ptr<SharedGameState> _sharedState;
        sf::Font _font;
        sf::Texture _mapTexture;
        sf::Sprite _mapSprite;
        sf::Texture _playerSpriteSheet;
        std::vector<sf::Sprite> _playerSprites;
        std::vector<animState> _playerAnimState;
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
        std::vector<sf::Text> _scoreTexts;
        sf::Sprite _coinIcon;
        sf::Music music;
        std::map<std::string, sf::SoundBuffer> soundBuffers;
        std::vector<sf::Sound> sounds;
        std::unique_ptr<GameOverScreen> _gameOverScreen;
        std::unique_ptr<WaitingRoom> _waitingRoom;
    };
}

#endif /* !GAME_HPP_ */
