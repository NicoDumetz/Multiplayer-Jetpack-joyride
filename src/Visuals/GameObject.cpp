/*
** EPITECH PROJECT, 2025
** jetpack
** File description:
** GameObject.cpp
*/

#include "GameObject.hpp"
#include "../settings.hpp"

namespace Jetpack {

GameObject::GameObject(const sf::Texture& texture, float x, float y, float tileSize, int frames)
    : _sprite(texture), _frames(frames), _initialPos(x * tileSize, TILE_MARGIN + y * tileSize), _pos(_initialPos)
{
    _textureSize = texture.getSize();
    _width = _textureSize.x / frames;
    float scale = tileSize / std::max(static_cast<float>(_width), static_cast<float>(_textureSize.y));
    _sprite.setTextureRect({0, 0, _width, static_cast<int>(_textureSize.y)});
    _sprite.setScale(scale, scale);
}

void GameObject::update(float dt)
{
    if (_frames <= 1)
        return;
    _animTimer += dt;
    if (_animTimer >= _frameRate) {
        _animTimer -= _frameRate;
        _currentFrame = (_currentFrame + 1) % _frames;
        _sprite.setTextureRect({_currentFrame * _width, 0, _width, static_cast<int>(_textureSize.y)});
    }
}

void Jetpack::GameObject::draw(sf::RenderWindow& window, float offsetX) const
{
    sf::Sprite spriteCopy = _sprite;
    this->_pos = _initialPos;
    this->_pos.x += offsetX;
    spriteCopy.setPosition(this->_pos);
    window.draw(spriteCopy);

}

void GameObject::move(float x, float y)
{
    _sprite.move(x, y);
}

void GameObject::setTransparent(bool transparent)
{
    _transparent = transparent;
}

bool GameObject::isTransparent() const
{
    return _transparent;
}

sf::Vector2f GameObject::getPosition() const
{
    return _initialPos;
}

} // namespace Jetpack
