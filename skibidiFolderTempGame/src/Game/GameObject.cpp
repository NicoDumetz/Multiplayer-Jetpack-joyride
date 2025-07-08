#include "GameObject.hpp"

GameObject::GameObject(const sf::Texture& texture, float x, float y, float tileSize) : tileSize(tileSize) {
    sprite.setTexture(texture);
    sprite.setPosition(x, y);
}

void GameObject::draw(sf::RenderWindow& window) const
{
    window.draw(sprite);
}

void GameObject::move(float x, float y)
{
    sprite.move(x, y);
}

sf::Vector2f GameObject::getPosition() const
{
    return sprite.getPosition();
}

void GameObject::setPosition(float x, float y)
{
    sprite.setPosition(x, y);
}

sf::FloatRect GameObject::getBounds() const
{
    return sprite.getGlobalBounds();
}