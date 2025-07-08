/*
** EPITECH PROJECT, 2025
** jetpack
** File description:
** Zapper.hpp
*/

#ifndef ZAPPER_HPP_
#define ZAPPER_HPP_

#include "Visuals/GameObject.hpp"
#include <SFML/Graphics/Texture.hpp>

namespace Jetpack {

class Zapper : public GameObject {
    public:
    Zapper(const sf::Texture& texture, float x, float y, float tileSize);
    ~Zapper() = default;
};
}
#endif // !
