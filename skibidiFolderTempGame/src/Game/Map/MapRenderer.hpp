#pragma once
#include <SFML/Graphics.hpp>

class MapRenderer {
public:
    MapRenderer(sf::Vector2u windowSize);
    void update(float dt);
    void draw(sf::RenderWindow& window);

private:
    sf::Texture texture;
    sf::Sprite sprite1;
    sf::Sprite sprite2;
    float scrollSpeed = 200.0f;
};