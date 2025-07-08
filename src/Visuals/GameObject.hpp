/*
** EPITECH PROJECT, 2025
** jetpack
** File description:
** GameObject.hpp
*/

#ifndef GAMEOBJECT_HPP_
#define GAMEOBJECT_HPP_

#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>

namespace Jetpack {

class GameObject {
public:
    GameObject(const sf::Texture& texture, float x, float y, float tileSize, int frames = 1);
    virtual ~GameObject() = default;

    virtual void update(float dt);
    virtual void draw(sf::RenderWindow& window, float offsetX) const;
    void move(float x, float y);
    void setTransparent(bool transparent);
    bool isTransparent() const;
    sf::Vector2f getPosition() const;

protected:
    sf::Sprite _sprite;
    sf::Vector2u _textureSize;
    int _width;
    int _frames;
    sf::Vector2f _initialPos;
    mutable sf::Vector2f _pos;
    int _currentFrame = 0;
    float _animTimer = 0.0f;
    float _frameRate = 0.1f;
    bool _transparent = false;
};

} // namespace Jetpack

#endif /* GAMEOBJECT_HPP_ */
