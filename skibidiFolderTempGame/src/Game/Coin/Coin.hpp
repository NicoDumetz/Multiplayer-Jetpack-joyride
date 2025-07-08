#pragma once
#include "GameObject.hpp"

class Coin : public GameObject {
public:
    Coin(const sf::Texture& texture, float x, float y, float tileSize);
    void update(float dt) override;
    
    enum {
        frameCount = 6,
        frameWidth = 191,
        frameHeight = 171
    };
    static constexpr float frameDuration = 0.1f;
    
private:
    int currentFrame = 0;
    float animTimer = 0.0f;
    
    void setupSprite();
    void updateAnimation(float dt);
};