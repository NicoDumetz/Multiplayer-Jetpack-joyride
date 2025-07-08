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

            uint8_t getId() const {return this->_id;}
            int getSocket() const {return this->_socket;}
            float getX() const {return this->_x;}
            float getY() const {return this->_y;}
            int getTileX() const {return static_cast<int>(std::ceil(this->_x));}
            int getTileY() const {return static_cast<int>(std::ceil(this->_y));}
            bool isAlive() const {return this->_alive;}
            int getCoins() const {return this->_coins;}
            bool hasJumped() const {return this->_hasJumped;}

            void setX(float x) {this->_x = x;}
            void setY(float y) {this->_y = y;}
            void setAlive(bool alive) {this->_alive = alive;}
            void setHasJumped(bool jumped) {this->_hasJumped = jumped;}
            void addCoin() {this->_coins++;}
            void setCoins(int coins) {this->_coins = coins;}

        private:
            uint8_t _id;
            int _socket;
            float _x;
            float _y;
            bool _alive;
            int _coins;
            bool _hasJumped;

        public:
            std::vector<std::vector<TileType>> map;
    };
}
