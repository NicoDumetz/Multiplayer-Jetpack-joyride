#include "Coin.hpp"

Coin::Coin(const sf::Texture& texture, float x, float y, float tileSize) 
    : GameObject(texture, x, y, tileSize) {
    setupSprite();
}

void Coin::setupSprite()
{
    sprite.setTextureRect(sf::IntRect(0, 0, frameWidth, frameHeight));
    
    float scale = tileSize / std::max(frameWidth, frameHeight);
    sprite.setScale(scale, scale);
    
    sprite.setOrigin(frameWidth / 2.0f, frameHeight / 2.0f);
    sprite.move(tileSize / 2.0f, 0);
}

void Coin::update(float dt)
{
    updateAnimation(dt);
}

void Coin::updateAnimation(float dt)
{
    animTimer += dt;
    
    if (animTimer >= frameDuration) {
        animTimer -= frameDuration;
        currentFrame = (currentFrame + 1) % frameCount;
        sprite.setTextureRect(sf::IntRect(currentFrame * frameWidth, 0, frameWidth, frameHeight));
    }
}