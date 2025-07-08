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
        float y = 0.f;
        bool alive = true;
    };

    class SharedGameState {
        public:
            void setPlayerState(uint8_t id, float y, bool alive)
            {
                std::lock_guard<std::mutex> lock(this->_mutex);

                if (id >= this->_players.size())
                    this->_players.resize(id + 1);
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

    enum class PlayerActionType {
        NOTHING,
        JUMP,
    };

    class ActionMutex {
        public:
            void pushJump()
            {
                std::lock_guard<std::mutex> lock(this->_mutex);
                this->_hasJumped = true;
            }

            bool consumeJump()
            {
                std::lock_guard<std::mutex> lock(this->_mutex);
                bool jumped = this->_hasJumped;
                this->_hasJumped = false;
                return jumped;
            }

        private:
            std::mutex _mutex;
            bool _hasJumped = false;
    };

}
