/*
** EPITECH PROJECT, 2025
** jetpack
** File description:
** Coin.cpp
*/

#include "Coin.hpp"
#include "../settings.hpp"
#include "Game/Game.hpp"

namespace Jetpack {

Coin::Coin(const sf::Texture& texture, float x, float y, float tileSize)
    : GameObject(texture, x, y, tileSize, COIN_FRAME)
{
    _frameRate = COIN_FRAME_RATE;
}

void Coin::update(float dt)
{
    if (this->_pos.x < 0 || this->_pos.x > WINDOW_WIDTH)
        return;
    GameObject::update(dt);
}

std::pair<int, int> Coin::getTilePosition() const
{
    int tileX = static_cast<int>(_initialPos.x / TILE_SIZE);
    int tileY = static_cast<int>((_initialPos.y - TILE_MARGIN) / TILE_SIZE);

    return {tileX, tileY};
}

} // namespace Jetpack
