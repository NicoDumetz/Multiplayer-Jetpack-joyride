/*
** EPITECH PROJECT, 2025
** jetpack
** File description:
** Coin.cpp
*/

#include "Coin.hpp"
#include "Game/Game.hpp"

namespace Jetpack {

Coin::Coin(const sf::Texture& texture, float x, float y, float tileSize): GameObject(texture, x, y, tileSize, COIN_FRAME)
{
    this->_frameRate = COIN_FRAME_RATE;
}

void Coin::update(float dt)
{
    float pos = this->_sprite.getPosition().x;

    if (pos < 0 || pos > WINDOW_WIDTH)
        return;
    GameObject::update(dt);
}
}
