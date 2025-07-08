/*
** EPITECH PROJECT, 2025
** B-NWP-400-LIL-4-1-jetpack-nicolas.dumetz
** File description:
** GameShared
*/

#pragma once
#include <mutex>
#include <vector>
#include <cstdint>

namespace Jetpack {
    struct Player {
        float x, y;
        bool alive;
    };
    class SharedGameState {
        public:

            void setPlayerState(uint8_t id, float x, float y, bool alive)
            {
                std::lock_guard<std::mutex> lock(this->_mutex);

                if (id >= this->_players.size())
                    this->_players.resize(id + 1);
                this->_players[id].x = x;
                this->_players[id].y = y;
                this->_players[id].alive = alive;
            }

            std::vector<Player> getPlayers()
            {
                std::lock_guard<std::mutex> lock(this->_mutex);
                return this->_players;
            }

            void setGameOver(bool value)
            {
                std::lock_guard<std::mutex> lock(this->_mutex);
                this->_gameOver = value;
            }

            bool isGameOver()
            {
                std::lock_guard<std::mutex> lock(this->_mutex);
                return this->_gameOver;
            }

        private:
            std::mutex _mutex;
            std::vector<Player> _players;
            bool _gameOver = false;
        };
}
