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
        private:
            std::shared_ptr<Jetpack::Client> _client;
            sf::RenderWindow _window;
            sf::Clock _clock;
            sf::Font _font;
            std::shared_ptr<SharedGameState> _sharedState;
        };
}


#endif /* !GAME_HPP_ */
