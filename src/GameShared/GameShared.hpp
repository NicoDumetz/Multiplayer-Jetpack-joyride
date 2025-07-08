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
#include "Error/Error.hpp"
#include "PlayerState/PlayerState.hpp"
#include "server/Server.hpp"

namespace Jetpack {

    class SharedGameState {
        public:
            SharedGameState() = default;
            ~SharedGameState() = default;

            void initPlayers(int numberClients)
            {
                std::lock_guard<std::mutex> lock(this->_mutex);
                this->_players.resize(numberClients);
                for (int i = 0; i < numberClients; i++) {
                    this->_players[i] = PlayerState(i, -1);
                    this->_players[i].setX(0.f);
                    this->_players[i].setY(0.f);
                    this->_players[i].setAlive(true);
                }
                this->_numberClients = numberClients;
            }

            void updatePlayerPosition(uint8_t id, float x, float y)
            {
                std::lock_guard<std::mutex> lock(this->_mutex);

                if (id >= this->_players.size())
                    throw Jetpack::Error("Invalid player ID: " + std::to_string(id));
                this->_players[id].setX(x);
                this->_players[id].setY(y);
            }

            void updatePlayerAliveStatus(uint8_t id, bool alive)
            {
                std::lock_guard<std::mutex> lock(this->_mutex);

                if (id >= this->_players.size())
                    throw Jetpack::Error("Invalid player ID: " + std::to_string(id));
                this->_players[id].setAlive(alive);
            }

            void updatePlayerCoins(uint8_t id, int coins)
            {
                std::lock_guard<std::mutex> lock(this->_mutex);

                if (id >= this->_players.size())
                    throw Jetpack::Error("Invalid player ID: " + std::to_string(id));
                this->_players[id].setCoins(coins);
            }

            void setPlayerState(uint8_t id, float x, float y, bool alive, int coins)
            {
                std::lock_guard<std::mutex> lock(this->_mutex);

                if (id >= this->_players.size())
                    throw Jetpack::Error("Invalid player ID in SharedGameState");
                this->_players[id].setX(x);
                this->_players[id].setY(y);
                this->_players[id].setAlive(alive);
                this->_players[id].setCoins(coins);
            }

            PlayerState &getPlayerState(uint8_t id)
            {
                std::lock_guard<std::mutex> lock(this->_mutex);
                if (id >= this->_players.size())
                    throw Jetpack::Error("Invalid player ID: " + std::to_string(id));
                return this->_players[id];
            }

            std::vector<PlayerState> getPlayers()
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

            void setNumberClients(int number)
            {
                std::lock_guard<std::mutex> lock(this->_mutex);
                this->_numberClients = number;
            }

            int getNumberClients()
            {
                std::lock_guard<std::mutex> lock(this->_mutex);
                return this->_numberClients;
            }

        private:
            std::mutex _mutex;
            std::vector<PlayerState> _players;
            bool _gameOver = false;
            int _numberClients;
    };

    enum class PlayerActionType {
        NOTHING,
        JUMP,
    };
}
