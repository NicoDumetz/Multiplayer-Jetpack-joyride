/*
** EPITECH PROJECT, 2025
** jetpack
** File description:
** GameObject.cpp
*/

#include "GameObject.hpp"
#include "Game/Game.hpp"

namespace Jetpack {

GameObject::GameObject(const sf::Texture& texture, float x, float y, float tileSize, int frames): _sprite(texture), _frames(frames)
{
    float scale;

    this->_textureSize = texture.getSize();
    this->_width = this->_textureSize.x / frames;
    scale = tileSize / std::max(static_cast<float>(this->_width), static_cast<float>(this->_textureSize.y));
    this->_sprite.setTextureRect({0, 0, this->_width, static_cast<int>(this->_textureSize.y)});
    _sprite.setPosition((x * tileSize) + WINDOW_WIDTH / 4.f, y * tileSize);
    _sprite.setScale(scale, scale);
}

void GameObject::draw(sf::RenderWindow& window) const
{
    window.draw(_sprite);
}

void GameObject::move(float x, float y)
{
    _sprite.move(x, y);
}

void GameObject::update(float dt)
{
    _animTimer += dt;
    if (_animTimer >= _frameRate) {
        _animTimer -= _frameRate;
        _currentFrame = (_currentFrame + 1) % _frames;
        _sprite.setTextureRect({_currentFrame * _width, 0, _width, static_cast<int>(_textureSize.y)});
    }
}
}
