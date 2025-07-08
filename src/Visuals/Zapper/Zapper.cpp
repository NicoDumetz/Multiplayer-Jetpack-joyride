/*
** EPITECH PROJECT, 2025
** jetpack
** File description:
** Zapper.cpp
*/

#include "Zapper.hpp"
#include "Game/Game.hpp"

namespace Jetpack {

Zapper::Zapper(const sf::Texture& texture, float x, float y, float tileSize) : GameObject(texture, x, y, tileSize, ZAPPER_FRAME)
{
    this->_frameRate = ZAPPER_FRAME_RATE;
}
}
