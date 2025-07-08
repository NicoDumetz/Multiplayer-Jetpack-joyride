/*
** EPITECH PROJECT, 2025
** B-NWP-400-LIL-4-1-jetpack-nicolas.dumetz
** File description:
** PlayerState
*/

#pragma once
#include <string>
#include <vector>
#include <memory>
#include <poll.h>
#include <cmath>


namespace Jetpack {
    class PlayerState {
        public:
            PlayerState()
                : _id(INVALID_ID), _socket(-1), _x(0.f), _y(0.f), _alive(true), _coins(0), _hasJumped(false) {}
            PlayerState(uint8_t id, int socket)
                : _id(id), _socket(socket), _x(0.f), _y(0.f), _alive(true), _coins(0), _hasJumped(false) {}

            inline uint8_t getId() const {return this->_id;}
            inline int getSocket() const {return this->_socket;}
            inline float getX() const {return this->_x;}
            inline float getY() const {return this->_y;}
            inline int getTileX() const {return static_cast<int>(std::ceil(this->_x));}
            inline int getTileY() const {return static_cast<int>(std::ceil(this->_y));}
            inline int getCurrentX() const {return static_cast<int>(this->_x);}
            inline int getCurrentY() const {return static_cast<int>(this->_y);}
            inline bool isAlive() const {return this->_alive;}
            inline int getCoins() const {return this->_coins;}
            inline bool hasJumped() const {return this->_hasJumped;}
            inline const std::vector<std::pair<int, int>>& getCoinCollected() const
            {
                return this->_coinCollected;
            }

            inline void setX(float x) {this->_x = x;}
            inline void setY(float y) {this->_y = y;}
            inline void setAlive(bool alive) {this->_alive = alive;}
            inline void setHasJumped(bool jumped) {this->_hasJumped = jumped;}
            inline void addCoin() {this->_coins++;}
            inline void setCoins(int coins) {this->_coins = coins;}
            inline void addCoinCollected(int coinX, int coinY)
            {
                this->_coinCollected.push_back(std::make_pair(coinX, coinY));
            }


        private:
            uint8_t _id;
            int _socket;
            float _x;
            float _y;
            bool _alive;
            int _coins;
            bool _hasJumped;
            std::vector<std::pair<int, int>> _coinCollected;

        public:
            std::vector<std::vector<TileType>> map;
    };
}
