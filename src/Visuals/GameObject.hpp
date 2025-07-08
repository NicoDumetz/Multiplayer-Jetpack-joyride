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
    void draw(sf::RenderWindow& window) const;
    void move(float x, float y);

    protected:
        sf::Sprite _sprite;
        sf::Vector2u _textureSize;
        int _width;
        int _frames;
        int _currentFrame = 0;
        float _animTimer = 0.0f;
        float _frameRate = 0.1f;
};
}
#endif
