#pragma once
#include "GameObject.hpp"

class Zapper : public GameObject {
public:
    Zapper(const sf::Texture& texture, float x, float y, float tileSize);
    void update(float dt) override;
    
    enum {
        frameCount = 4,
        frameWidth = 105,
        frameHeight = 122
    };
    static constexpr float frameDuration = 0.08f;
    
private:
    int currentFrame = 0;
    float animTimer = 0.0f;
    
    void setupSprite();
    void updateAnimation(float dt);
};