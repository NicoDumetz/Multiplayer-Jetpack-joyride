/*
** EPITECH PROJECT, 2025
** jetpack
** File description:
** Coin.hpp
*/

#ifndef COIN_HPP_
#define COIN_HPP_

#include "Visuals/GameObject.hpp"
#include <SFML/Graphics.hpp>

namespace Jetpack {

class Coin : public GameObject {
public:
    Coin(const sf::Texture& texture, float x, float y, float tileSize);
    ~Coin() = default;

    void update(float dt) override;
};

} // namespace Jetpack

#endif // COIN_HPP_
