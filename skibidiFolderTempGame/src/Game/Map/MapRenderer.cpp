#include "MapRenderer.hpp"

MapRenderer::MapRenderer(sf::Vector2u windowSize)
{
    if (!texture.loadFromFile("Assets/background.png")) {
        throw std::runtime_error("Erreur: impossible de charger Assets/background.png");
    }

    texture.setSmooth(true);

    float scaleY = static_cast<float>(windowSize.y) / texture.getSize().y;
    float scaleX = scaleY;

    sprite1.setTexture(texture);
    sprite1.setScale(scaleX, scaleY);
    sprite1.setPosition(0, 0);

    sprite2.setTexture(texture);
    sprite2.setScale(scaleX, scaleY);
    sprite2.setPosition(texture.getSize().x * scaleX, 0);
}

void MapRenderer::update(float dt)
{
    float moveX = scrollSpeed * dt;
    sprite1.move(-moveX, 0);
    sprite2.move(-moveX, 0);

    float scaledWidth = texture.getSize().x * sprite1.getScale().x;

    if (sprite1.getPosition().x + scaledWidth < 0)
        sprite1.setPosition(sprite2.getPosition().x + scaledWidth, 0);
    if (sprite2.getPosition().x + scaledWidth < 0)
        sprite2.setPosition(sprite1.getPosition().x + scaledWidth, 0);
}

void MapRenderer::draw(sf::RenderWindow& window)
{
    window.draw(sprite1);
    window.draw(sprite2);
}