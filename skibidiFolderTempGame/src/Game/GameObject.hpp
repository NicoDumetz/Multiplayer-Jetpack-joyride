#pragma once
#include <SFML/Graphics.hpp>

class GameObject {
public:
    GameObject(const sf::Texture& texture, float x, float y, float tileSize);
    virtual ~GameObject() = default;
    
    virtual void update(float dt) = 0;
    void draw(sf::RenderWindow& window) const;
    void move(float x, float y);
    sf::Vector2f getPosition() const;
    void setPosition(float x, float y);
    sf::FloatRect getBounds() const;
    
protected:
    sf::Sprite sprite;
    float tileSize;
};