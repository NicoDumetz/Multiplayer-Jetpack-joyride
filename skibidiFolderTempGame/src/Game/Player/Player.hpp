#pragma once
#include <SFML/Graphics.hpp>

enum class PlayerState {
    RUNNING,
    JETPACK,
    LANDING,
    DEATH_AIR,
    DEATH_GROUND
};

class Player {
public:
    Player();
    void setPosition(sf::Vector2f pos);
    sf::Vector2f getPosition() const;
    void update(float dt);
    void draw(sf::RenderWindow& window);
    bool isJetpackActive() const;
    void setScale(float scale);
    float getScale() const;
    sf::FloatRect getBounds() const;

    void die(bool inAir);
    bool isDeathAnimationCompleted() const;
    bool isDying() const { return isDead; }
    float getMaxY() const { return maxY; }

private:
    sf::Sprite sprite;
    sf::Texture texture;
    float velocityY;
    float gravity = 1200.f;
    float jetpackForce = -400.f;
    
    PlayerState currentState;
    float animationTimer;
    int currentFrame;
    const float frameDuration = 0.1f;
    
    const int frameWidth = 134;
    const int frameHeight = 134;
    
    const int runFrameCount = 4;
    const int jetpackFrameCount = 4;
    const int landingFrameCount = 4;
    
    const int runningY = 0;
    const int jetpackY = frameHeight;
    const int landingY = frameHeight * 2;
    
    const int deathAirY = frameHeight * 3;
    const int deathGroundY = frameHeight * 5;
    const int deathAirFrameCount = 8;
    const int deathGroundFrameCount = 4;
    
    bool isDead = false;
    bool deathAnimationCompleted = false;
    
    void updatePhysics(float dt);
    void updateState();
    void updateAnimation(float dt);
    void setAnimationState(PlayerState state);
    int getMaxFrames() const;
    int getYPosition() const;
    
    float minY = 0.0f;
    float maxY = 0.0f;
    
    bool wasOnGround;
};